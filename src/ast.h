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

class BinaryExpression : public Expression {
public:
  BinaryExpression(std::unique_ptr<Expression> left, BinaryOp op,
                   std::unique_ptr<Expression> right)
      : left(std::move(left)), right(std::move(right)), op(op) {}
  std::unique_ptr<Expression> left;
  std::unique_ptr<Expression> right;
  BinaryOp op;
};

class Identifier : public Expression {
public:
  Identifier(std::string_view name) : name(name) {}
  std::string_view name;
};

class Statement {};

class ExpressionStatement : public Statement {
public:
  ExpressionStatement(std::unique_ptr<Expression> expr)
      : expr(std::move(expr)) {}
  std::unique_ptr<Expression> expr;
};
class IncrementStatement : public Statement {
public:
  IncrementStatement(std::unique_ptr<Expression> expr)
      : expr(std::move(expr)) {}
  std::unique_ptr<Expression> expr;
};
class DecrementStatement : public Statement {
public:
  DecrementStatement(std::unique_ptr<Expression> expr)
      : expr(std::move(expr)) {}
  std::unique_ptr<Expression> expr;
};
class AssignmentStatement : public Statement {
public:
  AssignmentStatement(std::unique_ptr<Expression> left,
                      std::unique_ptr<Expression> right)
      : left(std::move(left)), right(std::move(right)) {}
  std::unique_ptr<Expression> left;
  std::unique_ptr<Expression> right;
};

class ForStatement : Statement {};

class ReturnStatement : public Statement {
public:
  ReturnStatement(std::unique_ptr<Expression> value)
      : value(std::move(value)) {}
  std::unique_ptr<Expression> value;
};

class BlockStatement : Statement {
public:
  BlockStatement(std::vector<std::unique_ptr<Statement>> statements)
      : statements(std::move(statements)) {}
  std::vector<std::unique_ptr<Statement>> statements;
};

class IfStatement : public Statement {
public:
  IfStatement(std::unique_ptr<Expression> condition,
              std::unique_ptr<BlockStatement> consequence,
              std::unique_ptr<BlockStatement> alternative)
      : condition(std::move(condition)), consequence(std::move(consequence)),
        alternative(std::move(alternative)) {}

  std::unique_ptr<Expression> condition;
  std::unique_ptr<BlockStatement> consequence;
  std::unique_ptr<BlockStatement> alternative;
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
  std::unique_ptr<BlockStatement> block;
}; // No first-class functions here 😭

} // namespace ast

#endif // !DEW_AST_H_
