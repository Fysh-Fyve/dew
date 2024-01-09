#ifndef DEW_MAIN_H_
#define DEW_MAIN_H_
#include <string>
#include <tree_sitter/api.h>

extern "C" TSLanguage *tree_sitter_dew();

class SExpression {
public:
  SExpression(TSNode node);
  ~SExpression();
  const char *str() const;

private:
  char *string;
};

class DewParser {
public:
  DewParser(std::string source);
  TSNode root() const;
  std::string_view nodeStr(TSNode node) const;
  ~DewParser();

private:
  TSParser *parser;
  TSTree *tree;
  std::string source;
};

#endif // DEW_MAIN_H_
