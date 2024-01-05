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
GRAMMAR := tree-sitter-dew
TS_INCLUDE_FLAGS := -I tree-sitter/lib/include
GRAMMAR_INCLUDE_FLAGS := -I $(GRAMMAR)/src

SHARED_LIB := tree-sitter/libtree-sitter.a

CXX := g++

# juicy
EXE := dewc

SRC := src/main.cc

all: $(EXE)

$(EXE): $(SRC) $(GRAMMAR)/src/parser.o $(SHARED_LIB)
	$(CXX) -o $@ $(TS_INCLUDE_FLAGS) $^

$(GRAMMAR)/%.o: $(GRAMMAR)/%.c
	$(CC) -c $(GRAMMAR_INCLUDE_FLAGS) $^ -o $@

# did you `sudo apt install libstdc++-12-dev` ??
compile_commands.json: $(SRC)
	bear -- $(MAKE) clean all

$(GRAMMAR)/src/parser.c: $(GRAMMAR)/grammar.js
	cd $(GRAMMAR) && tree-sitter generate

$(GRAMMAR)/grammar.js:
	git submodule init
	git submodule update

$(SHARED_LIB):
	cd tree-sitter && $(MAKE)

clean:
	rm -rf $(EXE)

.PHONY: all clean
