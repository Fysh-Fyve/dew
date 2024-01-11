# Dew

> You only need THREE things for a programming language: loops, functions,
> arithmetic

## Requirements

- C and C++ compiler
- [Tree Sitter CLI](https://github.com/tree-sitter/tree-sitter/blob/master/cli/README.md)
  (`tree-sitter`)

## Compilation

```
make
```

The first invocation will clone the `tree-sitter` and `tree-sitter-dew`
submodules and compile the `libtree-sitter` static library.

Any time the `grammar.js` is modified, it will also generate and compile the
parser into an object for linkage.

Run the program with an example file:

```
./dewc ./examples/main.dew
```

## Tree-sitter Parser

[Tree-sitter: Using Parsers](https://tree-sitter.github.io/tree-sitter/using-parsers)
