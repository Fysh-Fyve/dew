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
#ifndef DEW_CONTEXT_H_
#define DEW_CONTEXT_H_

#include "ast.h"
#include <optional>
#include <string_view>
#include <unordered_map>

class DewContextStack;

class DewContext {
public:
  DewContext(DewContext *parent);
  ~DewContext();
  void define(const std::string_view &name, Definition *definition);
  std::optional<Definition *> resolve(const std::string_view &name);
  DewContext *next;

private:
  DewContext *parent;
  DewContextStack *children;
  std::unordered_map<std::string_view, Definition *> definitions;
};

class DewContextStack {
public:
  DewContextStack();
  DewContext *head;
  ~DewContextStack();
};
#endif // !DEW_CONTEXT_H_
