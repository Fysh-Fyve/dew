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
#ifndef DEW_PARSER_H_
#define DEW_PARSER_H_

#include "ast.h"
#include "main.h"
#include <string>
#include <tree_sitter/api.h>

class Cursor {
public:
  TSTreeCursor cur;
};
Cursor *newCursor(TSNode node);
void deleteCursor(Cursor *cur);
using DewCursor = DewTSObject<Cursor, newCursor, deleteCursor>;

using ParamList = std::optional<std::vector<ast::Parameter>>;

class DewParser {
public:
  DewParser(std::string source);
  TSNode root() const;
  std::string_view nodeStr(TSNode node) const;
  void parseSource();

  ast::Function parseFunction(TSNode node);
  ast::Parameter parseParameter(TSNode node);
  std::unique_ptr<ast::BlockStatement> parseBlock(TSNode node);
  std::unique_ptr<ast::Statement> parseStatement(TSNode node);
  std::unique_ptr<ast::Expression> parseExpression(TSNode node);

  ParamList parseParamList(TSNode node);

  ~DewParser();

private:
  std::string source;
  TSParser *parser;
  TSTree *tree;
};

template <std::size_t N>
constexpr TSNode getField(TSNode &node, const char (&str)[N]) {
  return ts_node_child_by_field_name(node, str, N - 1);
}

#endif // !DEW_PARSER_H_
