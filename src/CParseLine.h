#ifndef CPARSE_LINE_H
#define CPARSE_LINE_H

#include <cassert>
#include <cstring>

class CParseLine {
 public:
  CParseLine(const std::string &line="") :
   str_(line), pos_(0), len_(str_.size()) {
  }

  void clear() {
    str_ = "";
    pos_ = 0;
    len_ = 0;
  }

  const std::string &str() const { return str_; }

  std::string substr() const { return substr(pos_); }

  std::string substr(int pos) const {
    assert(pos >= 0 && pos <= len_);

    return str_.substr(pos);
  }

  std::string substr(int pos, int len) const {
    assert(pos >= 0 && len >= 0 && pos + len <= len_);

    return str_.substr(pos, len);
  }

  int pos() const { return pos_; }

  void setPos(int pos) { pos_ = pos; }

  void incPos(int n) { pos_ += n; }

  void addChar(char c) {
    str_ += c;

    ++len_;
  }

  char lookChar() const {
    assert(pos_ < len_);

    return str_[pos_];
  }

  char getChar() {
    assert(pos_ < len_);

    return str_[pos_++];
  }

  void ungetChar(int n=1) { if (n <= pos_) pos_ -= n; }

  std::string getChars(int n) {
    std::string s;

    for (int i = 0; i < n && pos_ < len_; ++i)
      s += str_[pos_++];

    return s;
  }

  char lookNextChar(int offset=1) {
    assert(pos_ + offset <= len_);

    return str_[pos_ + offset];
  }

  void skipChar() { ++pos_; }

  void skipChars(int n) { pos_ += n; }

  bool isValid() const { return pos_ < len_; }

  void skipSpace() {
    while (pos_ < len_ && isSpace())
      ++pos_;
  }

  void backSkipSpace() {
    while (pos_ > 0 && backIsSpace())
      --pos_;
  }

  bool skipSpaceAndChar(char c) {
    skipSpace();

    if (! isChar(c))
      return false;

    skipChar();

    return true;
  }

  bool skipToChar(char c) {
    while (pos_ < len_ && ! isChar(c))
      ++pos_;

    return (pos_ < len_);
  }

  void skipNonSpace() {
    while (pos_ < len_ && ! isSpace())
      ++pos_;
  }

  std::string readNonSpace() {
    std::string str;

    while (pos_ < len_ && ! isSpace())
      str += str_[pos_++];

    return str;
  }

  bool isSpace() const {
    return (pos_ < len_ && isspace(str_[pos_]));
  }

  bool backIsSpace() const {
    return (pos_ > 0 && isspace(str_[pos_ - 1]));
  }

  bool isDigit() const {
    return (pos_ < len_ && isdigit(str_[pos_]));
  }

  bool isODigit() const {
    return (pos_ < len_ && isodigit(str_[pos_]));
  }

  bool isAlpha() const {
    return (pos_ < len_ && isalpha(str_[pos_]));
  }

  bool isAlNum() const {
    return (pos_ < len_ && isalnum(str_[pos_]));
  }

  bool isChar(char c) const {
    return (pos_ < len_ && str_[pos_] == c);
  }

  bool isOneOf(const std::string &chars) const {
    if (pos_ >= len_) return false;

    return std::strchr(chars.c_str(), str_[pos_]) != 0;
  }

  bool isChars(const std::string &str) const {
    int n = str.size();

    if (pos_ + n > len_) return false;

    if (strncmp(&str_[pos_], &str[0], n) != 0)
      return false;

    return true;
  }

  bool isOneOf(std::initializer_list<std::string> strs) {
    for (auto s : strs) {
      if (isString(s))
        return true;
    }

    return false;
  }

  bool isString(const std::string &str) const {
    int n = str.size();

    if (pos_ + n > len_) return false;

    if (strncmp(&str_[pos_], &str[0], n) != 0)
      return false;

    // check next character is end of string or space
    if (pos_ + n  + 1 > len_)
      return true;

    char c = str_[pos_ + n];

    return isspace(c);
  }

  bool skipCIdentifier() {
    char c = lookChar();

    if (! isalpha(c))
      return false;

    skipChar();

    while (isValid()) {
      char c = lookChar();

      if (! isalnum(c) && c != '_')
        break;

      skipChar();
    }

    return true;
  }

  void insert(const std::string &str) {
    str_ = str_.substr(0, pos_) + str + str_.substr(pos_);

    len_ += str.size();
  }

  static bool isodigit(char c) {
    return (c >= '0' && c <= '7');
  }

 private:
  std::string str_;
  int         pos_;
  int         len_;
};

#endif
