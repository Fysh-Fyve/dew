/**
 * Copyright (C) 2024 Charles Ancheta
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DEW_MAIN_H_
#define DEW_MAIN_H_
#include <string>
#include <tree_sitter/api.h>

#include "ast.h"

extern "C" TSLanguage *tree_sitter_dew();

template <typename TSObject, TSObject *(*Alloc)(TSNode),
          void (*Dealloc)(TSObject *)>
class DewTSObject {
public:
  DewTSObject(TSNode node) : ptr{Alloc(node)} {}
  ~DewTSObject() { Dealloc(ptr); }
  TSObject *get() const { return ptr; }

private:
  TSObject *ptr;
};

inline void freeChar(char *b) { free(b); }
using SExpression = DewTSObject<char, ts_node_string, freeChar>;

#endif // DEW_MAIN_H_
