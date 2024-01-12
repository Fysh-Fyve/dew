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
#include <optional>
#include <string_view>
#include <tree_sitter/api.h>
#include <vector>

extern "C" TSLanguage *tree_sitter_dew();

DewParser::DewParser(std::string source)
    : source(source), parser(ts_parser_new()) {
  ts_parser_set_language(parser, tree_sitter_dew());
  // TODO: Create custom TSInput?
  // Not sure, maybe reading everything into a string is enough for our
  // purposes
  tree = ts_parser_parse_string(parser, NULL, source.c_str(), source.length());
}

TSNode DewParser::root() const { return ts_tree_root_node(tree); }

ast::Parameter DewParser::parseParameter(TSNode node) {
  TSNode type = getField(node, "type");
  std::string_view typeName{ts_node_type(type)};
  std::string_view name = nodeStr(getField(node, "name"));
  if (typeName != "builtin_type") {
    // Do check here??
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

ast::Block DewParser::parseBlock(TSNode node) {
  std::cout << "TODO: BLOCC " << SExpression{node}.get() << "\n";
  return ast::Block{};
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
