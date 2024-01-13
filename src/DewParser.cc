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
#include <sstream>
#include <string_view>
#include <tree_sitter/api.h>
#include <unordered_map>
#include <vector>

using Expr = ast::Expr;
using Stmt = ast::Stmt;
using Block = ast::Block;

extern "C" TSLanguage *tree_sitter_dew();

void dbg(TSNode node) { std::cout << SExpression{node}.get() << std::endl; }

DewParser::DewParser(std::string source)
    : source(source), parser(ts_parser_new()) {
  ts_parser_set_language(parser, tree_sitter_dew());
  tree = ts_parser_parse_string(parser, NULL, source.c_str(), source.length());
}

TSNode DewParser::root() const { return ts_tree_root_node(tree); }

ast::Parameter DewParser::parseParameter(TSNode node) {
  TSNode type{getField(node, "type")};
  std::string_view typeName{ts_node_type(type)};
  std::string_view name{nodeStr(getField(node, "name"))};
  if (typeName != "builtin_type") {
    // TODO: Do check here??
  }
  return ast::Parameter{nodeStr(type), name};
}

ParamList DewParser::parseParamList(TSNode node) {
  TSNode params{getField(node, "params")};
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
  static const std::unordered_map<std::string_view, ast::BinaryOp> enumMap{
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
  auto it{enumMap.find(str)};
  if (it != enumMap.end()) {
    return it->second;
  } else {
    std::cerr << "Invalid operand: " << str << "\n";
    return std::nullopt;
  }
}

ast::Expr DewParser::parseExpr(TSNode node) {
  if (ts_node_is_null(node)) {
    return Expr(nullptr);
  }
  std::string_view type{ts_node_type(node)};
  if (type == "parenthesized_expression") {
    return parseExpr(ts_node_named_child(node, 0));
  } else if (type == "binary_expression") {
    Expr left{parseExpr(getField(node, "left"))};
    Expr right{parseExpr(getField(node, "right"))};
    std::string_view op{nodeStr(getField(node, "operator"))};
    // TODO: Throw on invalid expression?
    if (!left || !right) {
      return Expr(nullptr);
    }
    if (left->type != right->type) {
      // TODO: handle error?
    }
    std::optional<ast::BinaryOp> binOp{getBinaryOp(op)};
    if (binOp.has_value()) {
      // TODO: CHeck if the operation is valid
      // check if the operator is valid, but then again we only have integers?
      return std::make_unique<ast::BinaryExpression>(
          std::move(left), binOp.value(), std::move(right));
    }
    return Expr(nullptr);
  } else if (type == "identifier") {
    // TODO: check for conflicts here
    return std::make_unique<ast::Identifier>(nodeStr(node));
  } else if (type == "call_expression") {
    Expr function{parseExpr(getField(node, "function"))};
    TSNode arguments{getField(node, "arguments")};
    std::vector<Expr> args{parseExprList(arguments)};
    return std::make_unique<ast::CallExpression>(std::move(function),
                                                 std::move(args));
  } else if (type == "int_literal") {
    int base{10};
    std::stringstream ss;
    for (const char c : nodeStr(node)) {
      switch (c) {
      case 'x':
      case 'X':
        base = 16;
        break;
      case 'o':
      case 'O':
        base = 8;
        break;
      case 'b':
      case 'B':
        base = 2;
        break;
      case '_':
        break;
      default:
        ss << c;
      }
    }
    return std::make_unique<ast::IntegerLiteral>(std::stoi(ss.str(), 0, base));
  } else {
    // TODO: do the rest of the expression types
    std::cerr << "Invalid type: " << type << "\n";
  }
  return Expr(nullptr);
}

std::vector<Expr> DewParser::parseExprList(TSNode node) {
  std::vector<Expr> values;
  if (!ts_node_is_null(node)) {
    TSNode s{ts_node_named_child(node, 0)};
    while (!ts_node_is_null(s)) {
      values.push_back(parseExpr(s));
      s = ts_node_next_named_sibling(s);
    }
  }

  return values;
}

Stmt DewParser::parseStmt(TSNode node) {
  if (ts_node_is_null(node)) {
    return Stmt(nullptr);
  }
  std::string_view type{ts_node_type(node)};
  if (type == "if_statement") {
    TSNode condition{getField(node, "condition")};
    TSNode consequence{getField(node, "consequence")};
    TSNode alternative{getField(node, "alternative")};
    return std::make_unique<ast::IfStatement>(
        parseExpr(condition), parseBlock(consequence), parseBlock(alternative));
  } else if (type == "for_statement") {
    TSNode init{getField(node, "init")};
    TSNode cond{getField(node, "cond")};
    TSNode update{getField(node, "update")};
    return std::make_unique<ast::ForStatement>(parseStmt(init), parseExpr(cond),
                                               parseStmt(update));
  } else if (type == "return_statement") {
    return std::make_unique<ast::ReturnStatement>(
        parseExprList(ts_node_named_child(node, 0)));
  } else if (type == "expression_statement") {
    return std::make_unique<ast::ExpressionStatement>(
        parseExpr(ts_node_named_child(node, 0)));
  } else if (type == "assignment_statement") {
    return std::make_unique<ast::AssignmentStatement>(
        parseExprList(getField(node, "left")),
        parseExprList(getField(node, "right")));
  } else if (type == "var_declaration") {
    // Assignment statement??
    return Stmt(nullptr);
  } else {
    // TODO: do the rest of the statement types
    std::cerr << "Invalid type: " << type << "\n";
    dbg(node);
    return Stmt(nullptr);
  }
}

Block DewParser::parseBlock(TSNode node) {
  if (ts_node_is_null(node)) {
    return Block(nullptr);
  }
  std::vector<Stmt> list;
  TSNode s{ts_node_named_child(node, 0)};
  while (!ts_node_is_null(s)) {
    list.push_back(parseStmt(s));
    s = ts_node_next_named_sibling(s);
  }
  return std::make_unique<ast::BlockStatement>(std::move(list));
}

ast::Function DewParser::parseFunction(TSNode node) {
  return ast::Function{
      nodeStr(getField(node, "name")),
      std::vector<DataType>(), // TODO: Parse return type
      parseParamList(getField(node, "parameters")),
      parseBlock(getField(node, "body")),
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
