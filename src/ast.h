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
namespace ast {

class Statement {};

class SimpleStatement : Statement {};

class ForStatement : Statement {};

class ReturnStatement : Statement {};

class Block : Statement {};

class Function {
private:
  std::unique_ptr<Block> block;
}; // No first-class functions here 😭

class Expression {
private:
  DataType type;
};
} // namespace ast

#endif // !DEW_AST_H_
