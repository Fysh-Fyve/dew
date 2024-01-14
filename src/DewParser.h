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
#include <string>
#include <tree_sitter/api.h>

using ParamList = std::vector<ast::Parameter>;

class DewParser {

public:
  DewParser(std::string source);
  TSNode root() const;
  std::string_view nodeStr(TSNode node) const;
  void parseSource();

  ast::Function parseFunction(TSNode node);
  ast::Parameter parseParameter(TSNode node);
  ast::Block parseBlock(TSNode node);
  ast::Stmt parseStmt(TSNode node);
  ast::Expr parseExpr(TSNode node);
  std::vector<ast::Expr> parseExprList(TSNode node);
  std::optional<ParamList> parseParamList(TSNode node);
  ~DewParser();

private:
  std::string source;
  TSParser *parser;
  TSTree *tree;
};

#endif // !DEW_PARSER_H_
