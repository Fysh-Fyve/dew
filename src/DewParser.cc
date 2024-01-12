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
#include "DewParser.h"
#include "ast.h"
#include "main.h"
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <string_view>
#include <tree_sitter/api.h>
#include <unordered_map>
#include <vector>

extern "C" TSLanguage *tree_sitter_dew();

void dbg(TSNode node) { std::cout << SExpression{node}.get() << std::endl; }

DewParser::DewParser(std::string source)
    : source(source), parser(ts_parser_new()) {
  ts_parser_set_language(parser, tree_sitter_dew());
  tree = ts_parser_parse_string(parser, NULL, source.c_str(), source.length());
}

TSNode DewParser::root() const { return ts_tree_root_node(tree); }

ast::Parameter DewParser::parseParameter(TSNode node) {
  TSNode type = getField(node, "type");
  std::string_view typeName{ts_node_type(type)};
  std::string_view name = nodeStr(getField(node, "name"));
  if (typeName != "builtin_type") {
    // TODO: Do check here??
  }
  return ast::Parameter{.type = nodeStr(type), .name = name};
}

ParamList DewParser::parseParamList(TSNode node) {
  TSNode params = getField(node, "params");
  if (ts_node_is_null(params)) {
    return std::nullopt;
  }
  DewCursor cur{params};
  TSTreeCursor *c{&cur.get()->cur};
  ParamList p;
  do {
    p->push_back(parseParameter(ts_tree_cursor_current_node(c)));
  } while (ts_tree_cursor_goto_next_sibling(c));

  return p;
}

std::optional<ast::BinaryOp> getBinaryOp(const std::string_view &str) {
  static const std::unordered_map<std::string_view, ast::BinaryOp> enumMap = {
      {"*", ast::BinaryOp::Mul},         {"/", ast::BinaryOp::Div},
      {"%", ast::BinaryOp::Mod},         {"<<", ast::BinaryOp::ShiftLeft},
      {">>", ast::BinaryOp::ShiftRight}, {"&", ast::BinaryOp::BitAnd},
      {"+", ast::BinaryOp::Add},         {"-", ast::BinaryOp::Sub},
      {"|", ast::BinaryOp::BitOr},       {"^", ast::BinaryOp::BitXor},
      {">", ast::BinaryOp::GT},          {"<", ast::BinaryOp::LT},
      {">=", ast::BinaryOp::GTEq},       {"<=", ast::BinaryOp::LTEq},
      {"==", ast::BinaryOp::Eq},         {"!=", ast::BinaryOp::Neq},
      {"&&", ast::BinaryOp::And},        {"||", ast::BinaryOp::Or},
  };
  auto it = enumMap.find(str);
  if (it != enumMap.end()) {
    return it->second;
  } else {
    std::cerr << "Invalid operand: " << str << "\n";
    return std::nullopt;
  }
}

std::unique_ptr<ast::Expression> DewParser::parseExpression(TSNode node) {
  std::string_view type{ts_node_type(node)};
  if (type == "parenthesized_expression") {
    return parseExpression(ts_node_named_child(node, 0));
  } else if (type == "binary_expression") {
    std::unique_ptr<ast::Expression> left =
        parseExpression(getField(node, "left"));
    std::unique_ptr<ast::Expression> right =
        parseExpression(getField(node, "right"));
    std::string_view op = nodeStr(getField(node, "operator"));
    // TODO: Throw on invalid expression?
    if (!left || !right) {
      return std::unique_ptr<ast::Expression>(nullptr);
    }
    if (left->type != right->type) {
      // TODO: handle error?
    }
    std::optional<ast::BinaryOp> binOp = getBinaryOp(op);
    if (binOp.has_value()) {
      // TODO: CHeck if the operation is valid
      // check if the operator is valid, but then again we only have integers?
      return std::make_unique<ast::BinaryExpression>(
          std::move(left), binOp.value(), std::move(right));
    }
    return std::unique_ptr<ast::Expression>(nullptr);
  } else if (type == "identifier") {
    // TODO: check for conflicts here
    return std::make_unique<ast::Identifier>(nodeStr(node));
  } else if (type == "int_literal") {
    // TODO: parse integer
    nodeStr(node);
  } else {
    // TODO: do the rest of the expression types
    std::cerr << "Invalid type: " << type << "\n";
  }
  dbg(node);
  return std::unique_ptr<ast::Expression>(nullptr);
}

std::unique_ptr<ast::Statement> DewParser::parseStatement(TSNode node) {
  std::string_view type{ts_node_type(node)};
  if (type == "if_statement") {
    TSNode condition = getField(node, "condition");
    TSNode consequence = getField(node, "consequence");
    TSNode alternative = getField(node, "alternative");
    return std::make_unique<ast::IfStatement>(
        parseExpression(condition), parseBlock(consequence),
        ts_node_is_null(alternative)
            ? std::unique_ptr<ast::BlockStatement>(nullptr)
            : parseBlock(alternative));

  } else if (type == "return_statement") {
    dbg(node);
    return std::unique_ptr<ast::Statement>(nullptr);
  } else if (type == "expression_statement") {
    return std::make_unique<ast::ExpressionStatement>(
        parseExpression(ts_node_named_child(node, 0)));
    return std::unique_ptr<ast::Statement>(nullptr);
  } else {
    // TODO: do the rest of the statement types
    std::cerr << "Invalid type: " << type << "\n";
    dbg(node);
    return std::unique_ptr<ast::Statement>(nullptr);
  }
}

std::unique_ptr<ast::BlockStatement> DewParser::parseBlock(TSNode node) {
  std::vector<std::unique_ptr<ast::Statement>> list;
  TSNode s{ts_node_named_child(node, 0)};
  while (!ts_node_is_null(s)) {
    list.push_back(parseStatement(s));
    s = ts_node_next_named_sibling(s);
  }
  return std::make_unique<ast::BlockStatement>(std::move(list));
}

ast::Function DewParser::parseFunction(TSNode node) {
  return ast::Function{
      .name = nodeStr(getField(node, "name")),
      .params = parseParamList(getField(node, "parameters")),
      .block = parseBlock(getField(node, "body")),
  };
}

void DewParser::parseSource() {
  TSNode rootNode{root()};
  DewCursor cur{rootNode};
  TSTreeCursor *c{&cur.get()->cur};
  ts_tree_cursor_goto_first_child(c);
  do {
    TSNode node{ts_tree_cursor_current_node(c)};
    std::string_view type{ts_node_type(node)};
    if (type == "function_declaration") {
      parseFunction(node);
    } else {
      std::cerr << "Invalid type: " << type << "\n";
      return;
    }
  } while (ts_tree_cursor_goto_next_sibling(c));
}

std::string_view DewParser::nodeStr(TSNode node) const {
  uint32_t start{ts_node_start_byte(node)};
  uint32_t end{ts_node_end_byte(node)};
  return std::string_view(source).substr(start, end - start);
}

DewParser::~DewParser() {
  ts_tree_delete(tree);
  ts_parser_delete(parser);
}

Cursor *newCursor(TSNode node) { return new Cursor{ts_tree_cursor_new(node)}; }
void deleteCursor(Cursor *cur) {
  ts_tree_cursor_delete(&cur->cur);
  delete cur;
}
