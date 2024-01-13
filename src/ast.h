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
#ifndef DEW_AST_H_
#define DEW_AST_H_

#include "type.h"
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

namespace ast {
enum class BinaryOp {
  Add,
  Sub,
  BitOr,
  BitXor,
  Mul,
  Div,
  Mod,
  ShiftLeft,
  ShiftRight,
  BitAnd,
  GT,
  LT,
  GTEq,
  LTEq,
  Eq,
  Neq,
  And,
  Or,
};

enum class UnaryOp { Pos, Neg, Not, BitNot, Deref, Ref };
class Expression {
public:
  DataType type;
};

using Expr = std::unique_ptr<ast::Expression>;

class BinaryExpression : public Expression {
public:
  BinaryExpression(Expr left, BinaryOp op, Expr right)
      : left(std::move(left)), right(std::move(right)), op(op) {}
  Expr left;
  Expr right;
  BinaryOp op;
};

class Identifier : public Expression {
public:
  Identifier(std::string_view name) : name(name) {}
  std::string_view name;
};

class IntegerLiteral : public Expression {
public:
  IntegerLiteral(uint64_t num) : num(num) {}
  uint64_t num;
};

class CallExpression : public Expression {
public:
  CallExpression(Expr function, std::vector<Expr> arguments)
      : function(std::move(function)), arguments(std::move(arguments)) {}
  Expr function;
  std::vector<Expr> arguments;
};

class Statement {};

using Stmt = std::unique_ptr<ast::Statement>;

class VarDeclaration : public Statement {
public:
  VarDeclaration(std::string_view name, DataType type)
      : name(name), type(type) {}
  std::string_view name;
  DataType type;
};

class ExpressionStatement : public Statement {
public:
  ExpressionStatement(Expr expr) : expr(std::move(expr)) {}
  Expr expr;
};
class IncrementStatement : public Statement {
public:
  IncrementStatement(Expr expr) : expr(std::move(expr)) {}
  Expr expr;
};
class DecrementStatement : public Statement {
public:
  DecrementStatement(Expr expr) : expr(std::move(expr)) {}
  Expr expr;
};

class AssignmentStatement : public Statement {
public:
  AssignmentStatement(std::vector<Expr> left, std::vector<Expr> right)
      : left(std::move(left)), right(std::move(right)) {}
  std::vector<Expr> left;
  std::vector<Expr> right;
};

class ForStatement : public Statement {
public:
  ForStatement(Stmt initial, Expr condition, Stmt update)
      : initial(std::move(initial)), condition(std::move(condition)),
        update(std::move(update)) {}
  Stmt initial;
  Expr condition;
  Stmt update;
};

class ReturnStatement : public Statement {
public:
  ReturnStatement(std::vector<Expr> values) : values(std::move(values)) {}
  std::vector<Expr> values;
};

class BlockStatement : Statement {
public:
  BlockStatement(std::vector<Stmt> statements)
      : statements(std::move(statements)) {}
  std::vector<Stmt> statements;
};

using Block = std::unique_ptr<ast::BlockStatement>;

class IfStatement : public Statement {
public:
  IfStatement(Expr condition, Block consequence, Block alternative)
      : condition(std::move(condition)), consequence(std::move(consequence)),
        alternative(std::move(alternative)) {}

  Expr condition;
  Block consequence;
  Block alternative;
};

class Parameter {
public:
  DataType type;
  std::string_view name;
};

class Function {
public:
  std::string_view name;
  std::vector<DataType> returnValues;
  std::optional<std::vector<Parameter>> params;
  Block block;
}; // No first-class functions here 😭

} // namespace ast

#endif // !DEW_AST_H_
