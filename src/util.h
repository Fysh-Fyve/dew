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
/**
 * \file util.h
 */
#ifndef DEW_UTIL_H_
#define DEW_UTIL_H_

#include "ast.h"
#include <cstdint>
#include <string_view>
#include <tree_sitter/api.h>

namespace dew {
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

uint64_t parseInt(const std::string_view &literal);

ast::BinaryOp getBinaryOp(const std::string_view &str);

template <std::size_t N>
constexpr TSNode getField(TSNode &node, const char (&str)[N]) {
  return ts_node_child_by_field_name(node, str, N - 1);
}

class Cursor {
public:
  TSTreeCursor cur;
};
Cursor *newCursor(TSNode node);
void deleteCursor(Cursor *cur);
using DewCursor = DewTSObject<Cursor, newCursor, deleteCursor>;
} // namespace dew
#endif // !DEW_UTIL_H_
