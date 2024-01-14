#
# Copyright (C) 2024 Charles Ancheta
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

GRAMMAR := tree-sitter-dew
TS_INCLUDE_FLAGS := -I tree-sitter/lib/include
GRAMMAR_INCLUDE_FLAGS := -I $(GRAMMAR)/src

SHARED_LIB := tree-sitter/libtree-sitter.a


CXXFLAGS := -std=c++17 -Wall -Wpedantic

# juicy
EXE := dewc

SRC := $(call rwildcard,src,*.cc)

OBJ := $(SRC:src/%.cc=obj/%.o)

all: $(EXE)

$(EXE): $(OBJ) $(GRAMMAR)/src/parser.o $(SHARED_LIB)
	@echo CXX $^
	@$(CXX) $(CXXFLAGS) -o $@ $(TS_INCLUDE_FLAGS) $^

mem-test: $(EXE)
	valgrind -s --leak-check=full ./$(EXE) ./examples/fib.dew

obj/%.o: src/%.cc $(SHARED_LIB) | obj
	@echo CXX $<
	@$(CXX) -c $(CXXFLAGS) $(TS_INCLUDE_FLAGS) $< -o $@

obj:
	mkdir $@

$(GRAMMAR)/%.o: $(GRAMMAR)/%.c
	@echo CC $^
	@$(CC) -c $(GRAMMAR_INCLUDE_FLAGS) $^ -o $@

COMP_DB := compile_commands.json

lsp: $(COMP_DB)

# did you `sudo apt install libstdc++-12-dev` ??
$(COMP_DB): $(SRC)
	bear -- $(MAKE) clean all

$(GRAMMAR)/src/parser.c: $(GRAMMAR)/grammar.js
	cd $(GRAMMAR) && tree-sitter generate

$(GRAMMAR)/grammar.js:
	git submodule init
	git submodule update

tree-sitter/Makefile:
	git submodule init
	git submodule update

$(SHARED_LIB): tree-sitter/Makefile
	cd tree-sitter && $(MAKE)

clean:
	rm -rf $(EXE) $(OBJ) $(COMP_DB)

.PHONY: all clean mem-test lsp
