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
 * \file util.cc
 */
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "ast.h"
#include "util.h"

ast::BinaryOp getBinaryOp(const std::string_view &str) {
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
    // shouldn't really get here
    std::cerr << "Invalid operand: " << str << "\n";
    std::exit(1);
  }
}

uint64_t parseInt(const std::string_view &literal) {
  int base{10};
  std::stringstream ss;
  for (const char c : literal) {
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
  return std::stoll(ss.str(), 0, base);
}

Cursor *newCursor(TSNode node) { return new Cursor{ts_tree_cursor_new(node)}; }
void deleteCursor(Cursor *cur) {
  ts_tree_cursor_delete(&cur->cur);
  delete cur;
}
