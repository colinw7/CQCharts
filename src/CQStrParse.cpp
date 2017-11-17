#include <CQStrParse.h>
#include <cassert>

namespace {
  static QString base_chars_ = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
}

CQStrParse::
CQStrParse(const QString &str)
{
  setString(str);
}

void
CQStrParse::
setString(const QString &str)
{
  str_     = str;
  pos_     = 0;
  len_     = str_.size();
  lineNum_ = 1;
}

void
CQStrParse::
addString(const QString &str)
{
  str_ += str;

  len_ = str_.size();
}

void
CQStrParse::
setPos(int pos)
{
  if (pos < 0        ) pos = 0;
  if (pos > int(len_)) pos = len_;

  int pos1 = pos;

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

QString
CQStrParse::
getAt() const
{
  return getAt(pos_);
}

QString
CQStrParse::
getAt(int pos) const
{
  if (pos < len_)
    return str_.mid(pos);
  else
    return "";
}

QString
CQStrParse::
getAt(int pos, int len) const
{
  if (pos < len_ && pos + len <= len_)
    return str_.mid(pos, len);
  else
    return getAt(pos);
}

QString
CQStrParse::
getBefore(int pos) const
{
  if      (pos < len_ && pos < pos_)
    return str_.mid(pos, pos_ - pos);
  else if (pos < len_ && pos > pos_)
    return str_.mid(pos, pos - pos_);
  else
    return "";
}

QString
CQStrParse::
getAfter() const
{
  if (pos_ + 1 < len_)
    return str_.mid(pos_ + 1);
  else
    return "";
}

QChar
CQStrParse::
getCharBefore(int offset) const
{
  if (pos_ >= offset)
    return str_[pos_ - offset];
  else
    return '\0';
}

QChar
CQStrParse::
getCharAt() const
{
  if (eof()) return '\0';

  return str_[pos_];
}

QChar
CQStrParse::
getCharAt(int pos) const
{
  if (pos >= 0 && pos_ < len_)
    return str_[pos];

  return '\0';
}

QChar
CQStrParse::
getCharAfter(int offset) const
{
  if (pos_ + offset < len_)
    return str_[pos_ + offset];
  else
    return '\0';
}

void
CQStrParse::
skipSpace()
{
  if (eof()) return;

  int pos1 = pos_;

  skipSpaceI(str_, &pos1);

  setPos(pos1);
}

bool
CQStrParse::
skipSpaceI(const QString &str, int *pos)
{
  int len = str.length();

  if (*pos >= len || ! str[*pos].isSpace())
    return false;

  while (*pos < len && str[*pos].isSpace())
    ++(*pos);

  return true;
}

void
CQStrParse::
skipNonSpace()
{
  if (eof()) return;

  int pos1 = pos_;

  skipNonSpaceI(str_, &pos1);

  setPos(pos1);
}

bool
CQStrParse::
skipNonSpaceI(const QString &str, int *pos)
{
  int len = str.length();

  if (*pos >= len || str[*pos].isSpace())
    return false;

  while (*pos < len && ! str[*pos].isSpace())
    ++(*pos);

  return true;
}

bool
CQStrParse::
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

  if (eof())
    return false;

  assert(isChar('}'));

  skipChar();

  return true;
}

bool
CQStrParse::
readBracedString(QString &text, bool includeBraces)
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

  if (eof())
    return false;

  assert(isChar('}'));

  if (includeBraces)
    text += readChar();
  else
    skipChar();

  return true;
}

void
CQStrParse::
autoSkipSpace() const
{
  if (eof()) return;

  if (isAutoSkipSpace()) {
    CQStrParse *th = const_cast<CQStrParse *>(this);

    th->skipSpace();
  }
}

QChar
CQStrParse::
getChar()
{
  autoSkipSpace();

  if (eof())
    return '\0';

  return str_[pos_++];
}

bool
CQStrParse::
skipChar()
{
  autoSkipSpace();

  if (eof())
    return false;

  setPos(pos_ + 1);

  return true;
}

bool
CQStrParse::
skipChar(QChar c)
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
CQStrParse::
skipChars(const QString &s)
{
  return skipChars(s.size());
}

bool
CQStrParse::
skipChars(int n)
{
  if (pos_ + n > len_) {
    setPos(len_);
    return false;
  }

  setPos(pos_ + n);

  return true;
}

bool
CQStrParse::
skipString()
{
  autoSkipSpace();

  if (eof())
    return false;

  int pos1 = pos_;

  if      (str_[pos1] == '\"') {
    if (! skipDoubleQuotedStringI(str_, &pos1))
      return false;
  }
  else if (str_[pos1] == '\'') {
    if (! skipSingleQuotedStringI(str_, &pos1))
      return false;
  }
  else
    return false;

  setPos(pos1);

  return true;
}

bool
CQStrParse::
skipDoubleQuotedStringI(const QString &str, int *pos)
{
  int len = str.size();

  if (*pos >= len || str[*pos] != '\"')
    return false;

  ++(*pos);

  while (*pos < len) {
    if      (str[*pos] == '\\') {
      ++(*pos);

      if (*pos < len)
        ++(*pos);
    }
    else if (str[*pos] == '\"')
      break;
    else
      ++(*pos);
  }

  if (*pos >= len)
    return false;

  ++(*pos);

  return true;
}

bool
CQStrParse::
skipSingleQuotedStringI(const QString &str, int *pos)
{
  int len = str.size();

  if (*pos >= len || str[*pos] != '\'')
    return false;

  if (str[*pos] != '\'')
    return false;

  ++(*pos);

  while (*pos < len) {
    if      (str[*pos] == '\\') {
      ++(*pos);

      if (*pos < len)
        ++(*pos);
    }
    else if (str[*pos] == '\'')
      break;
    else
      ++(*pos);
  }

  if (*pos >= len)
    return false;

  ++(*pos);

  return true;
}

void
CQStrParse::
skipToEnd()
{
  setPos(len_);
}

bool
CQStrParse::
readNonSpace(QString &text)
{
  if (eof() || str_[pos_].isSpace())
    return false;

  int pos = pos_;

  skipNonSpace();

  text = str_.mid(pos, pos_ - pos);

  return true;
}

QChar
CQStrParse::
readChar()
{
  autoSkipSpace();

  assert(! eof());

  QChar c = str_[pos_];

  setPos(pos_ + 1);

  return c;
}

bool
CQStrParse::
readChar(QChar *c)
{
  autoSkipSpace();

  if (eof())
    return false;

  *c = str_[pos_];

  setPos(pos_ + 1);

  return true;
}

#if 0
bool
CQStrParse::
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
#endif

bool
CQStrParse::
unreadChar()
{
  if (pos_ == 0)
    return false;

  setPos(pos_ - 1);

  return true;
}

bool
CQStrParse::
unreadString(const QString &str)
{
  int len = str.size();

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
CQStrParse::
readNumber(double &real, int &integer, bool &is_real)
{
  autoSkipSpace();

  int pos1 = pos_;

  bool b = readNumberI(str_, &pos1, real, integer, is_real);

  setPos(pos1);

  return b;
}

bool
CQStrParse::
readNumberI(const QString &str, int *pos, double &real, int &integer, bool &is_real)
{
  is_real = false;

  //------

  int len = str.size();

  QString number_str;

  //------

  if (*pos < len && (str[*pos] == '+' || str[*pos] == '-'))
    number_str += str[(*pos)++];

  //------

  while (*pos < len && str[*pos].isDigit())
    number_str += str[(*pos)++];

  //------

  if (*pos < len && str[*pos] == '.') {
    is_real = true;

    number_str += str[(*pos)++];

    while (*pos < len && str[*pos].isDigit())
      number_str += str[(*pos)++];
  }

  //------

  if (*pos < len && (str[*pos] == 'e' || str[*pos] == 'E')) {
    is_real = true;

    number_str += str[(*pos)++];

    if (*pos < len && (str[*pos] == '+' || str[*pos] == '-'))
      number_str += str[(*pos)++];

    if (*pos >= len || ! str[*pos].isDigit())
      return false;

    while (*pos < len && str[*pos].isDigit())
      number_str += str[(*pos)++];
  }

  //------

  if (number_str.size() == 0)
    return false;

  if (is_real) {
    if (! toRealI(number_str, &real))
      return false;
  }
  else {
    if (! toIntegerI(number_str, &integer))
      return false;
  }

  //------

  return true;
}

bool
CQStrParse::
toRealI(const QString &str, double *real)
{
  QByteArray ba = str.toLatin1();

  const char *c_str = ba.constData();

  int i = 0;

  while (c_str[i] != 0 && ::isspace(c_str[i]))
    ++i;

  if (c_str[i] == '\0') {
    //error_msg_ = "Empty String";
    return false;
  }

  const char *p;

#ifdef ALLOW_NAN
  if (COS::has_nan() && strncmp(&c_str[i], "NaN", 3) == 0) {
    p = &c_str[i + 3];

    COS::set_nan(real);
  }
  else {
    errno = 0;

    *real = strtod(&c_str[i], (char **) &p);

    if (errno == ERANGE) {
      //error_msg_ = "Out of Range";
      return false;
    }
  }
#else
  errno = 0;

  *real = strtod(&c_str[i], (char **) &p);

  if (errno == ERANGE) {
    //error_msg_ = "Out of Range";
    return false;
  }
#endif

  while (*p != 0 && ::isspace(*p))
    ++p;

  if (*p != '\0') {
    //error_msg_ = "Trailing Characters";
    return false;
  }

  return true;
}

bool
CQStrParse::
toIntegerI(const QString &str, int *integer)
{
  long integer1;

  if (! toIntegerI(str, &integer1))
    return false;

  *integer = (int) integer1;

  return true;
}

bool
CQStrParse::
toIntegerI(const QString &str, long *integer)
{
  QByteArray ba = str.toLatin1();

  const char *c_str = ba.constData();

  int i = 0;

  while (c_str[i] != 0 && ::isspace(c_str[i]))
    ++i;

  if (c_str[i] == '\0') {
    //error_msg_ = "Empty String";
    return false;
  }

  const char *p;

  errno = 0;

  *integer = strtol(&c_str[i], (char **) &p, 10);

  if (errno == ERANGE) {
    //error_msg_ = "Out of Range";
    return false;
  }

  while (*p != 0 && ::isspace(*p))
    ++p;

  if (*p != '\0') {
    //error_msg_ = "Trailing Characters";
    return false;
  }

  return true;
}

bool
CQStrParse::
readInteger(int *integer)
{
  autoSkipSpace();

  int pos1 = pos_;

  bool b = readIntegerI(str_, &pos1, integer);

  setPos(pos1);

  return b;
}

bool
CQStrParse::
readIntegerI(const QString &str, int *pos, int *integer)
{
  int i = *pos;

  //------

  if (! skipIntegerI(str, pos))
    return false;

  //------

  if (integer) {
    if (! toIntegerI(str.mid(i, *pos - i), integer))
      return false;
  }

  //------

  return true;
}

bool
CQStrParse::
readBaseInteger(int base, int *integer)
{
  autoSkipSpace();

  int pos1 = pos_;

  bool b = readBaseIntegerI(str_, base, &pos1, integer);

  setPos(pos1);

  return b;
}

bool
CQStrParse::
readBaseIntegerI(const QString &str, int base, int *pos, int *integer)
{
  int i = *pos;

  //------

  if (! skipBaseIntegerI(str, base, pos))
    return false;

  //------

  if (integer) {
    long l;

    if (! toBaseIntegerI(str.mid(i, *pos - i), base, &l))
      return false;

    *integer = l;
  }

  //------

  return true;
}

bool
CQStrParse::
toBaseIntegerI(const QString &str, int base, long *integer)
{
  *integer = 0;

  if (base < 2 || base > base_chars_.size()) {
    //error_msg_ = "Unsupported Base " + toString(base);
    return false;
  }

  int i   = 0;
  int len = str.size();

  while (i < len) {
    QChar c = str[i];

    int value;

    if (! isBaseCharI(c, base, &value))
      return false;

    long integer1 = base*(*integer) + value;

    if (long((integer1 - (long) value)/base) != *integer) {
      //error_msg_ = "Overflow.";
      return false;
    }

    *integer = integer1;

    ++i;
  }

  if (*integer > std::numeric_limits<int>::max() || *integer < std::numeric_limits<int>::min()) {
    //error_msg_ = "Overflow.";
    return false;
  }

  return true;
}

bool
CQStrParse::
skipIntegerI(const QString &str, int *pos)
{
  int len = str.size();

  if (*pos < len && (str[*pos] == '+' || str[*pos] == '-'))
    ++(*pos);

  if (*pos >= len || ! str[*pos].isNumber())
    return false;

  ++(*pos);

  while (*pos < len && str[*pos].isNumber())
    ++(*pos);

  return true;
}

bool
CQStrParse::
skipBaseIntegerI(const QString &str, int base, int *pos)
{
  int len = str.size();

  if (*pos < len && (str[*pos] == '+' || str[*pos] == '-'))
    ++(*pos);

  if (*pos >= len || ! isBaseCharI(str[*pos], base, 0))
    return false;

  ++(*pos);

  while (*pos < len && isBaseCharI(str[*pos], base, 0))
    ++(*pos);

  return true;
}

bool
CQStrParse::
isBaseCharI(const QChar &c, int base, int *value)
{
  if (base < 2 || base > base_chars_.size()) {
    //error_msg_ = "Unsupported Base " + toString(base);
    return false;
  }

  QChar c1 = c;

  if (c1.isLower())
    c1 = c1.toUpper();

  int pos = base_chars_.indexOf(c1);

  if (pos < 0 || pos >= base) {
    //error_msg_ = QString("Invalid Character ") + char(c) +
    //             " for Base " + toString(base);
    return false;
  }

  if (value)
    *value = int(pos);

  return true;
}

bool
CQStrParse::
readReal(double *real)
{
  autoSkipSpace();

  int pos1 = pos_;

  if (! readRealI(str_, &pos1, real))
    return false;

  setPos(pos1);

  return true;

}

bool
CQStrParse::
readRealI(const QString &str, int *pos, double *real)
{
  double r       = 0.0;
  int    i       = 0;
  bool   is_real = false;

  if (! readNumberI(str, pos, r, i, is_real))
    return false;

  if (is_real)
    *real = r;
  else
    *real = i;

  return true;
}

bool
CQStrParse::
readString(QString &str, bool strip_quotes)
{
  autoSkipSpace();

  int pos = pos_;

  if (! skipString())
    return false;

  if (! strip_quotes)
    str = str_.mid(pos, pos_ - pos);
  else
    str = str_.mid(pos + 1, pos_ - pos - 2);

  return true;
}

bool
CQStrParse::
skipToChar(QChar c)
{
  int pos1 = pos_;

  while (pos1 < len_ && str_[pos1] != c)
    ++pos1;

  setPos(pos1);

  if (eof())
    return false;

  return true;
}

bool
CQStrParse::
readToChar(QChar c, QString &text)
{
  text = "";

  int pos1 = pos_;

  while (pos1 < len_ && str_[pos1] != c)
    text += str_[pos1++];

  setPos(pos1);

  if (eof())
    return false;

  return true;
}

bool
CQStrParse::
isIdentifier(int offset)
{
  return isIdentifierI(str_, pos_ + offset);
}

bool
CQStrParse::
isIdentifierI(const QString &str, int pos)
{
  int len = str.size();

  if (pos < len && (str[pos] == '_' || str[pos].isLetter()))
    return true;

  return false;
}

bool
CQStrParse::
readIdentifier(QString &identifier)
{
  autoSkipSpace();

  int pos1 = pos_;

  bool b = readIdentifierI(str_, &pos1, identifier);

  setPos(pos1);

  return b;
}

bool
CQStrParse::
readIdentifierI(const QString &str, int *pos, QString &identifier)
{
  int len = str.size();

  if (*pos < len && (str[*pos] == '_' || str[*pos].isLetter())) {
    identifier = str[(*pos)++];

    while (*pos < len && (str[*pos] == '_' || str[*pos].isLetterOrNumber()))
      identifier += str[(*pos)++];

    return true;
  }
  else
    return false;
}

#if 0
bool
CQStrParse::
readRealFormat(QString &real_format)
{
  autoSkipSpace();

  int pos1 = pos_;

  bool b = readRealFormat(str_, &pos1, real_format);

  setPos(pos1);

  return b;
}

bool
CQStrParse::
readIntegerFormat(QString &integer_format)
{
  autoSkipSpace();

  int pos1 = pos_;

  bool b = readIntegerFormat(str_, &pos1, integer_format);

  setPos(pos1);

  return b;
}

bool
CQStrParse::
readStringFormat(QString &string_format)
{
  autoSkipSpace();

  int pos1 = pos_;

  bool b = readStringFormat(str_, &pos1, string_format);

  setPos(pos1);

  return b;
}
#endif

bool
CQStrParse::
isSpace() const
{
  if (eof()) return false;

  return (pos_ < len_ && str_[pos_].isSpace());
}

bool
CQStrParse::
isNewline() const
{
  if (eof()) return false;

  if (str_[pos_] == '\n' || str_[pos_] == '\r')
    return true;

  // utf \u2028 (LS), \u2029 (PS)

  return false;
}

bool
CQStrParse::
isLower() const
{
  autoSkipSpace();

  return (pos_ < len_ && str_[pos_].isLetter() && str_[pos_].isUpper());
}

bool
CQStrParse::
isUpper() const
{
  autoSkipSpace();

  return (pos_ < len_ && str_[pos_].isLetter() && str_[pos_].isUpper());
}

bool
CQStrParse::
isAlpha() const
{
  autoSkipSpace();

  return (pos_ < len_ && str_[pos_].isLetter());
}

bool
CQStrParse::
isAlnum() const
{
  autoSkipSpace();

  return (pos_ < len_ && str_[pos_].isLetterOrNumber());
}

bool
CQStrParse::
isDigit() const
{
  autoSkipSpace();

  return (pos_ < len_ && str_[pos_].isNumber());
}

bool
CQStrParse::
isDigitAt(int offset) const
{
  return (pos_ + offset < len_ && str_[pos_ + offset].isNumber());
}

bool
CQStrParse::
isODigit() const
{
  static QString ochars = "01234567";

  autoSkipSpace();

  return (pos_ < len_ && ochars.indexOf(str_[pos_]) >= 0);
}

bool
CQStrParse::
isXDigit() const
{
  static QString xchars = "0123456789ABCDEFabcdef";

  autoSkipSpace();

  return (pos_ < len_ && xchars.indexOf(str_[pos_]) >= 0);
}

bool
CQStrParse::
isCharBefore(QChar c, int offset) const
{
  return (getCharBefore(offset) == c);
}

bool
CQStrParse::
isChar(QChar c) const
{
  return (getCharAt() == c);
}

bool
CQStrParse::
isCharAfter(QChar c, int offset) const
{
  return (getCharAfter(offset) == c);
}

bool
CQStrParse::
isNextChar(QChar c) const
{
  return (getCharAfter(1) == c);
}

bool
CQStrParse::
isString(const QString &str) const
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
CQStrParse::
isOneOf(const QString &str) const
{
  autoSkipSpace();

  return (pos_ < len_ && str.indexOf(str_[pos_]) != -1);
}

bool
CQStrParse::
isWord(const QString &str) const
{
  int len = str.size();

  if (pos_ + len > len_)
    return false;

  for (int i = 0; i < len; ++i)
    if (str_[pos_ + i] != str[i])
      return false;

  if (pos_ + len <= len_ && str_[pos_ + len].isLetterOrNumber())
    return false;

  return true;
}

bool
CQStrParse::
eof() const
{
  return (pos_ >= len_);
}

bool
CQStrParse::
neof(int n) const
{
  return (pos_ + n >= len_);
}
