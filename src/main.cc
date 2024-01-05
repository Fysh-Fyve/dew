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
#include <cstring>
#include <iostream>
#include <tree_sitter/api.h>

extern "C" TSLanguage *tree_sitter_dew();

int main() {
  // Create a parser.
  TSParser *parser = ts_parser_new();

  // Set the parser's language (Dew in this case).
  ts_parser_set_language(parser, tree_sitter_dew());

  // Build a syntax tree based on source code stored in a string.
  const char *source_code = "fun main() {}";
  TSTree *tree = ts_parser_parse_string(parser, NULL, source_code,
                                        std::strlen(source_code));

  // Get the root node of the syntax tree.
  TSNode root_node = ts_tree_root_node(tree);

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
  char *string = ts_node_string(root_node);
  std::cout << "Syntax tree: " << string << "\n";

  // Free all of the heap-allocated memory.
  free(string);
  ts_tree_delete(tree);
  ts_parser_delete(parser);
  return 0;
}
