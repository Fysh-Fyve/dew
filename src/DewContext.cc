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
#include "DewContext.h"
#include <optional>

DewContextStack::DewContextStack() : head{nullptr} {}

DewContextStack::~DewContextStack() {
  DewContext *cur{head};
  while (cur != nullptr) {
    head = cur->next;
    delete cur;
  }
}

DewContext::DewContext(DewContext *parent)
    : parent(parent), children(new DewContextStack{}) {
  if (!parent) {
    return;
  }

  if (parent->children->head != nullptr) {
    next = parent->children->head;
    parent->children->head = next;
  } else {
    parent->children->head = this;
  }
}

DewContext::~DewContext() {
  delete children;
  for (const auto &[_, ptr] : definitions) {
    delete ptr;
  }
}

std::optional<Definition *> DewContext::resolve(const std::string_view &name) {
  auto def = definitions.find(name);
  if (def != definitions.end()) {
    return def->second;
  } else if (parent != nullptr) {
    return parent->resolve(name);
  } else {
    return std::nullopt;
  }
}

void DewContext::define(const std::string_view &name, Definition *definition) {
  definitions.emplace(name, definition);
}
