#include <CStrParse.h>
#include <CStrUtil.h>
#include <CUtf8.h>
#include <cassert>

CStrParse::
CStrParse(const std::string &str)
{
  setString(str);
}

void
CStrParse::
setString(const std::string &str)
{
  str_     = str;
  pos_     = 0;
  len_     = str_.size();
  lineNum_ = 1;
}

void
CStrParse::
addString(const std::string &str)
{
  str_ += str;

  len_ = str_.size();
}

void
CStrParse::
setPos(int pos)
{
  if (pos < 0        ) pos = 0;
  if (pos > int(len_)) pos = len_;

  uint pos1 = pos;

  if      (pos1 > pos_) {
    while (pos_ < pos1) {
      if (str_[pos_] == '\n')
        ++lineNum_;

      ++pos_;
    }
  }
  else if (pos1 < pos_) {
    while (pos_ > pos1) {
      --pos_;

      if (str_[pos_] == '\n')
        --lineNum_;
    }
  }

  assert(pos_ == pos1);
}

std::string
CStrParse::
getAt() const
{
  return getAt(pos_);
}

std::string
CStrParse::
getAt(uint pos) const
{
  if (pos < len_)
    return str_.substr(pos);
  else
    return "";
}

std::string
CStrParse::
getAt(uint pos, uint len) const
{
  if (pos < len_ && pos + len <= len_)
    return str_.substr(pos, len);
  else
    return getAt(pos);
}

std::string
CStrParse::
getBefore(uint pos) const
{
  if      (pos < len_ && pos < pos_)
    return str_.substr(pos, pos_ - pos);
  else if (pos < len_ && pos > pos_)
    return str_.substr(pos, pos - pos_);
  else
    return "";
}

std::string
CStrParse::
getAfter() const
{
  if (pos_ + 1 < len_)
    return str_.substr(pos_ + 1);
  else
    return "";
}

char
CStrParse::
getCharBefore(uint offset) const
{
  if (pos_ >= offset)
    return str_[pos_ - offset];
  else
    return '\0';
}

char
CStrParse::
getCharAt() const
{
  if (eof()) return '\0';

  return str_[pos_];
}

char
CStrParse::
getCharAt(int pos) const
{
  if (pos >= 0 && pos_ < len_)
    return str_[pos];

  return '\0';
}

char
CStrParse::
getCharAfter(uint offset) const
{
  if (pos_ + offset < len_)
    return str_[pos_ + offset];
  else
    return '\0';
}

void
CStrParse::
skipSpace()
{
  if (eof()) return;

  uint pos1 = pos_;

  CStrUtil::skipSpace(str_, &pos1);

  setPos(pos1);
}

void
CStrParse::
skipNonSpace()
{
  if (eof()) return;

  uint pos1 = pos_;

  CStrUtil::skipNonSpace(str_, &pos1);

  setPos(pos1);
}

bool
CStrParse::
skipBracedString()
{
  autoSkipSpace();

  if (! isChar('{'))
    return false;

  skipChar();

  int nbraces = 1;

  while (! eof()) {
    if      (isChar('{')) {
      ++nbraces;
    }
    else if (isChar('}')) {
      --nbraces;

      if (nbraces == 0)
        break;
    }

    skipChar();
  }

  if (isChar('}'))
    skipChar();

  return true;
}

bool
CStrParse::
readBracedString(std::string &text, bool includeBraces)
{
  text = "";

  autoSkipSpace();

  if (! isChar('{'))
    return false;

  if (includeBraces)
    text += readChar();
  else
    skipChar();

  int pos1 = getPos();

  int nbraces = 1;

  while (! eof()) {
    if      (isChar('{')) {
      ++nbraces;
    }
    else if (isChar('}')) {
      --nbraces;

      if (nbraces == 0)
        break;
    }

    skipChar();
  }

  int pos2 = getPos();

  text += getAt(pos1, pos2 - pos1);

  if (! isChar('}'))
    return false;

  if (includeBraces)
    text += readChar();
  else
    skipChar();

  return true;
}

void
CStrParse::
autoSkipSpace() const
{
  if (eof()) return;

  if (isAutoSkipSpace()) {
    CStrParse *th = const_cast<CStrParse *>(this);

    th->skipSpace();
  }
}

bool
CStrParse::
skipChar()
{
  autoSkipSpace();

  if (eof())
    return false;

  setPos(pos_ + 1);

  return true;
}

bool
CStrParse::
skipChar(char c)
{
  autoSkipSpace();

  if (eof())
    return false;

  if (str_[pos_] != c)
    return false;

  setPos(pos_ + 1);

  return true;
}

bool
CStrParse::
skipChars(const std::string &s)
{
  return skipChars(s.size());
}

bool
CStrParse::
skipChars(uint n)
{
  if (pos_ + n > len_) {
    setPos(len_);
    return false;
  }

  setPos(pos_ + n);

  return true;
}

bool
CStrParse::
skipString()
{
  autoSkipSpace();

  if (eof())
    return false;

  uint pos1 = pos_;

  if      (str_[pos1] == '\"') {
    if (! CStrUtil::skipDoubleQuotedString(str_, &pos1))
      return false;
  }
  else if (str_[pos1] == '\'') {
    if (! CStrUtil::skipSingleQuotedString(str_, &pos1))
      return false;
  }
  else
    return false;

  setPos(pos1);

  return true;
}

void
CStrParse::
skipToEnd()
{
  setPos(len_);
}

bool
CStrParse::
readNonSpace(std::string &text)
{
  if (eof() || isspace(str_[pos_]))
    return false;

  int pos = pos_;

  skipNonSpace();

  text = str_.substr(pos, pos_ - pos);

  return true;
}

char
CStrParse::
readChar()
{
  autoSkipSpace();

  assert(! eof());

  char c = str_[pos_];

  setPos(pos_ + 1);

  return c;
}

bool
CStrParse::
readChar(char *c)
{
  autoSkipSpace();

  if (eof())
    return false;

  *c = str_[pos_];

  setPos(pos_ + 1);

  return true;
}

bool
CStrParse::
readUtf8Char(ulong *c)
{
  autoSkipSpace();

  if (eof())
    return false;

  int pos1 = pos_;

  *c = CUtf8::readNextChar(str_, pos1, len_);

  setPos(pos1);

  return true;
}

bool
CStrParse::
unreadChar()
{
  if (pos_ == 0)
    return false;

  setPos(pos_ - 1);

  return true;
}

bool
CStrParse::
unreadString(const std::string &str)
{
  uint len = str.size();

  if (pos_ < len) {
    setPos(0);

    return false;
  }
  else {
    setPos(pos_ - len);

    return true;
  }
}

bool
CStrParse::
readNumber(double &real, int &integer, bool &is_real)
{
  autoSkipSpace();

  uint pos1 = pos_;

  bool b = CStrUtil::readNumber(str_, &pos1, real, integer, is_real);

  setPos(pos1);

  return b;
}

bool
CStrParse::
readInteger(int *integer)
{
  autoSkipSpace();

  uint pos1 = pos_;

  bool b = CStrUtil::readInteger(str_, &pos1, integer);

  setPos(pos1);

  return b;
}

bool
CStrParse::
readBaseInteger(int base, int *integer)
{
  autoSkipSpace();

  uint pos1 = pos_;

  bool b = CStrUtil::readBaseInteger(str_, base, &pos1, integer);

  setPos(pos1);

  return b;
}

bool
CStrParse::
readReal(double *real)
{
  autoSkipSpace();

  uint pos1 = pos_;

  if (! CStrUtil::readReal(str_, &pos1, real))
    return false;

  setPos(pos1);

  return true;

}

bool
CStrParse::
readString(std::string &str, bool strip_quotes)
{
  autoSkipSpace();

  int pos = pos_;

  if (! skipString())
    return false;

  if (! strip_quotes)
    str = str_.substr(pos, pos_ - pos);
  else
    str = str_.substr(pos + 1, pos_ - pos - 2);

  return true;
}

bool
CStrParse::
skipToChar(char c)
{
  uint pos1 = pos_;

  while (pos1 < len_ && str_[pos1] != c)
    ++pos1;

  setPos(pos1);

  if (eof())
    return false;

  return true;
}

bool
CStrParse::
readToChar(char c, std::string &text)
{
  text = "";

  uint pos1 = pos_;

  while (pos1 < len_ && str_[pos1] != c)
    text += str_[pos1++];

  setPos(pos1);

  if (eof())
    return false;

  return true;
}

bool
CStrParse::
isIdentifier(int offset)
{
  return CStrUtil::isIdentifier(str_, pos_ + offset);
}

bool
CStrParse::
readIdentifier(std::string &identifier)
{
  autoSkipSpace();

  uint pos1 = pos_;

  bool b = CStrUtil::readIdentifier(str_, &pos1, identifier);

  setPos(pos1);

  return b;
}

bool
CStrParse::
readRealFormat(std::string &real_format)
{
  autoSkipSpace();

  uint pos1 = pos_;

  bool b = CStrUtil::readRealFormat(str_, &pos1, real_format);

  setPos(pos1);

  return b;
}

bool
CStrParse::
readIntegerFormat(std::string &integer_format)
{
  autoSkipSpace();

  uint pos1 = pos_;

  bool b = CStrUtil::readIntegerFormat(str_, &pos1, integer_format);

  setPos(pos1);

  return b;
}

bool
CStrParse::
readStringFormat(std::string &string_format)
{
  autoSkipSpace();

  uint pos1 = pos_;

  bool b = CStrUtil::readStringFormat(str_, &pos1, string_format);

  setPos(pos1);

  return b;
}

bool
CStrParse::
isSpace() const
{
  if (eof()) return false;

  return (pos_ < len_ && isspace(str_[pos_]));
}

bool
CStrParse::
isNewline() const
{
  if (eof()) return false;

  if (str_[pos_] == '\n' || str_[pos_] == '\r')
    return true;

  // utf \u2028 (LS), \u2029 (PS)

  return false;
}

bool
CStrParse::
isLower() const
{
  autoSkipSpace();

  return (pos_ < len_ && isalpha(str_[pos_]) && isupper(str_[pos_]));
}

bool
CStrParse::
isUpper() const
{
  autoSkipSpace();

  return (pos_ < len_ && isalpha(str_[pos_]) && isupper(str_[pos_]));
}

bool
CStrParse::
isAlpha() const
{
  autoSkipSpace();

  return (pos_ < len_ && isalpha(str_[pos_]));
}

bool
CStrParse::
isAlnum() const
{
  autoSkipSpace();

  return (pos_ < len_ && isalnum(str_[pos_]));
}

bool
CStrParse::
isDigit() const
{
  autoSkipSpace();

  return (pos_ < len_ && isdigit(str_[pos_]));
}

bool
CStrParse::
isDigitAt(int offset) const
{
  return (pos_ + offset < len_ && isdigit(str_[pos_ + offset]));
}

bool
CStrParse::
isODigit() const
{
  autoSkipSpace();

  return (pos_ < len_ && CStrUtil::isodigit(str_[pos_]));
}

bool
CStrParse::
isXDigit() const
{
  autoSkipSpace();

  return (pos_ < len_ && CStrUtil::isxdigit(str_[pos_]));
}

bool
CStrParse::
isCharBefore(char c, uint offset) const
{
  return (getCharBefore(offset) == c);
}

bool
CStrParse::
isChar(char c) const
{
  return (getCharAt() == c);
}

bool
CStrParse::
isCharAfter(char c, uint offset) const
{
  return (getCharAfter(offset) == c);
}

bool
CStrParse::
isNextChar(char c) const
{
  return (getCharAfter(1) == c);
}

bool
CStrParse::
isString(const std::string &str) const
{
  int len = str.size();

  if (pos_ + len > len_)
    return false;

  for (int i = 0; i < len; ++i)
    if (str_[pos_ + i] != str[i])
      return false;

  return true;
}

bool
CStrParse::
isOneOf(const std::string &str) const
{
  autoSkipSpace();

  return (pos_ < len_ && str.find(str_[pos_]) != std::string::npos);
}

bool
CStrParse::
isWord(const std::string &str) const
{
  int len = str.size();

  if (pos_ + len > len_)
    return false;

  for (int i = 0; i < len; ++i)
    if (str_[pos_ + i] != str[i])
      return false;

  if (pos_ + len <= len_ && isalnum(str_[pos_ + len]))
    return false;

  return true;
}

bool
CStrParse::
eof() const
{
  return (pos_ >= len_);
}

bool
CStrParse::
neof(int n) const
{
  return (pos_ + n >= len_);
}
