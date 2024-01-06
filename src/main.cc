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
    parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_dew());
    // TODO: Create custom TSInput?
    // Not sure, maybe reading everything into a string is enough for our
    // purposes
    std::ifstream sourceFile(filePath);
    std::stringstream buffer;
    buffer << sourceFile.rdbuf();
    source = buffer.str();
    tree =
        ts_parser_parse_string(parser, NULL, source.c_str(), source.length());
  }

  TSNode root() const { return ts_tree_root_node(tree); }

  ~DewParser() {
    ts_tree_delete(tree);
    ts_parser_delete(parser);
  }

  TSParser *parser;
  TSTree *tree;
  std::string source;
};

// TODO: No copying!
// But also we're just trying to make this work, maybe it's fine for now
std::string node_str(TSNode node, DewParser &p) {
  uint32_t start = ts_node_start_byte(node);
  uint32_t end = ts_node_end_byte(node);
  return p.source.substr(start, end - start);
}

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    std::cerr << "USAGE: " << argv[0] << " FILE\n";
    std::exit(1);
  }
  DewParser p = DewParser(argv[1]);
  TSNode root_node = p.root();

  TSNode func_node = ts_node_named_child(root_node, 0);

  auto s = SExpression(func_node);
  std::cout << "Syntax tree:\n" << s.str << "\n";

  auto identifier = ts_node_named_child(func_node, 0);
  std::cout << "Function name: " << node_str(identifier, p) << "\n";
  /*
  // Get some child nodes.
  TSNode array_node = ts_node_named_child(root_node, 0);
  TSNode number_node = ts_node_named_child(array_node, 0);

  // Check that the nodes have the expected types.
  assert(strcmp(ts_node_type(root_node), "document") == 0);
  assert(strcmp(ts_node_type(array_node), "array") == 0);
  assert(strcmp(ts_node_type(number_node), "number") == 0);

  // Check that the nodes have the expected child counts.
  assert(ts_node_child_count(root_node) == 1);
  assert(ts_node_child_count(array_node) == 5);
  assert(ts_node_named_child_count(array_node) == 2);
  assert(ts_node_child_count(number_node) == 0);
  */

  // Print the syntax tree as an S-expression.
  return 0;
}
