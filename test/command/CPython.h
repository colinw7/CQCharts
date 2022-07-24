#ifndef CPython_H
#define CPython_H

#include <string>
#include <vector>

class CStrParse;

class CPython {
 public:
  CPython();

  bool isCompleteLine(const std::string &line);

  void startStringParse(const std::string &str);
  void endParse();

 private:
  bool isCompleteLine1(char endChar);

 private:
  using ParseStack = std::vector<CStrParse *>;

  CStrParse* parse_ { nullptr };
  ParseStack parseStack_;
};

#endif
