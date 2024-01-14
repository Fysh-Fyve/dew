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
 * \file main.cc
 */
#include "DewParser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <tree_sitter/api.h>

std::string fileToString(const char *path) {
  std::ifstream sourceFile{path};
  if (!sourceFile.is_open()) {
    sourceFile.close();
    std::cerr << "file `" << path << "` could not be read\n";
    std::exit(1);
  }
  std::stringstream ss;
  ss << sourceFile.rdbuf();
  return ss.str();
}

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    std::cerr << "USAGE: " << argv[0] << " FILE\n";
    return 1;
  }

  DewParser p{fileToString(argv[1])};
  p.parseSource();

  return 0;
}
