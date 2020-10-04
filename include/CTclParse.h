#ifndef CTCL_PARSE_H
#define CTCL_PARSE_H

#include <map>
#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <sys/types.h>

class CStrParse;

class CTclToken {
 public:
  enum class Type {
    NONE,
    LITERAL_STRING,
    QUOTED_STRING,
    STRING,
    COMMAND,
    SUB_COMMAND,
    VARIABLE
  };

 public:
  using Tokens = std::vector<CTclToken *>;

 public:
  CTclToken(Type type, const std::string &str, int pos) :
   type_(type), str_(str), pos_(pos) {
  }

  Type type() const { return type_; }
  void setType(Type type) { type_ = type; }

  const std::string &str() const { return str_; }

  int pos() const { return pos_; }

  int endPos() const { return pos_ + str_.size() - 1; }

  void addToken(CTclToken *token) {
    tokens_.push_back(token);
  }

  const Tokens &tokens() const { return tokens_; }

  bool hasPos(int pos) {
    return (pos >= pos_ && pos <= endPos());
  }

  void print(std::ostream &os) {
    os << typeName(type_) << ":" << str_ << "@" << pos_;

    if (! tokens_.empty()) {
      os << '[';

      for (const auto &token : tokens_)
        token->print(os);

      os << ']';
    }
  }

  static std::string typeName(Type type) {
    if      (type == Type::LITERAL_STRING) return "literal_string";
    else if (type == Type::QUOTED_STRING ) return "quoted_string";
    else if (type == Type::STRING        ) return "string";
    else if (type == Type::COMMAND       ) return "command";
    else if (type == Type::SUB_COMMAND   ) return "sub_command";
    else if (type == Type::VARIABLE      ) return "variable";
    else                                   return "?";
  }

 private:
  Type        type_ { Type::NONE };
  std::string str_;
  int         pos_  { -1 };
  Tokens      tokens_;
};

//---

class CTclParse {
 private:
  class SetSeparator {
   public:
    SetSeparator(CTclParse *tcl, char c) :
     tcl_(tcl) {
      old_c_ = tcl_->getSeparator();

      tcl_->setSeparator(c);
    }

   ~SetSeparator() {
      tcl_->setSeparator(old_c_);
    }

   private:
    CTclParse* tcl_   { nullptr };
    char       old_c_ { '\0' };
  };

 public:
  using Tokens = std::vector<CTclToken *>;

 public:
  CTclParse();
 ~CTclParse();

  bool getDebug() const { return debug_; }
  void setDebug(bool debug=true) { debug_ = debug; }

  char getSeparator() const { return separator_; }
  void setSeparator(char c) { separator_ = c; }

  bool parseFile(const std::string &filename, Tokens &tokens);

  bool isCompleteLine(const std::string &line);

  bool parseLine(const std::string &str, Tokens &tokens);

  bool parseString(const std::string &str, Tokens &tokens);

  bool processLine(const std::string &line, Tokens &tokens);

  bool readArgList(Tokens &tokens);

  bool readExecString(Tokens &tokens);

  bool readLiteralString(std::string &str, Tokens &tokens);

  bool readDoubleQuotedString(std::string &str, Tokens &tokens);

  bool readSingleQuotedString(std::string &str);

  bool readVariableName(std::string &varName, bool &is_array, Tokens &tokens);

  bool readWord(std::string &str, char endChar, Tokens &tokens);

  std::string expandExpr(const std::string &str);

  std::string lookupPathCommand(const std::string &name) const;

  void startFileParse(const std::string &fileName);
  void startStringParse(const std::string &str);
  void endParse();

  CTclToken *getTokenForPos(const Tokens &tokens, int pos) const;

  static bool needsBraces(const std::string &str);

 private:
  bool isCompleteLine1(char endChar);

 private:
  using ParseStack = std::vector<CStrParse *>;

  CStrParse* parse_     { nullptr };
  ParseStack parseStack_;
  char       separator_ { ';' };
  bool       debug_     { false };
};

#endif
