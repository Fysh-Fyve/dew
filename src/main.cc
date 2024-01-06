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
#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>
#include <tree_sitter/api.h>

extern "C" TSLanguage *tree_sitter_dew();

class SExpression {
public:
  SExpression(TSNode node) { str = ts_node_string(node); }
  ~SExpression() { free(str); }

  char *str;
};

class DewParser {
public:
  DewParser(std::string filePath) {
    // TODO: Create custom TSInput?
    // Not sure, maybe reading everything into a string is enough for our
    // purposes
    std::ifstream sourceFile{filePath};
    if (sourceFile.is_open()) {
      parser = ts_parser_new();
      ts_parser_set_language(parser, tree_sitter_dew());
      std::stringstream ss;
      ss << sourceFile.rdbuf();
      source = ss.str();
      tree =
          ts_parser_parse_string(parser, NULL, source.c_str(), source.length());
    } else {
      sourceFile.close();
      std::cerr << "file `" << filePath << "` could not be read\n";
      std::exit(1);
    }
  }

  TSNode root() const { return ts_tree_root_node(tree); }

  std::string_view nodeStr(TSNode node) const {
    uint32_t start = ts_node_start_byte(node);
    uint32_t end = ts_node_end_byte(node);
    return std::string_view(source).substr(start, end - start);
  }

  ~DewParser() {
    ts_tree_delete(tree);
    ts_parser_delete(parser);
  }

  TSParser *parser;
  TSTree *tree;
  std::string source;
};

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    std::cerr << "USAGE: " << argv[0] << " FILE\n";
    std::exit(1);
  }
  DewParser p = DewParser(argv[1]);
  TSNode root = p.root();

  std::cout << "Syntax tree:\n" << SExpression(root).str << "\n";

  TSNode funNode = ts_node_named_child(root, 0);
  TSNode identifier = ts_node_named_child(funNode, 0);
  std::cout << "Function name: " << p.nodeStr(identifier) << "\n";

  return 0;
}
