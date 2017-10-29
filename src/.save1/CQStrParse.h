#ifndef CQStrParse_H
#define CQStrParse_H

#include <QString>
#include <sys/types.h>

class CQStrParse {
 public:
  explicit CQStrParse(const QString &str="");

  virtual ~CQStrParse() { }

  bool isAutoSkipSpace() const { return autoSkipSpace_; }
  void setAutoSkipSpace(bool autoSkipSpace=true) { autoSkipSpace_ = autoSkipSpace; }

  virtual void setString(const QString &str);

  virtual const QString &getString() const { return str_; }

  virtual void addString(const QString &str);

  virtual int getPos() const { return pos_; }

  virtual int getLen() const { return len_; }

  virtual void setPos(int pos);

  int lineNum() const { return lineNum_; }

  virtual QString getAt    () const;
  virtual QString getAt    (int pos) const;
  virtual QString getAt    (int pos, int len) const;
  virtual QString getBefore(int pos=0) const;
  virtual QString getAfter () const;

  virtual QChar getCharBefore(int offset = 1) const;
  virtual QChar getCharAt    () const;
  virtual QChar getCharAt    (int pos) const;
  virtual QChar getCharAfter (int offset = 1) const;

  virtual void skipSpace();
  virtual void skipNonSpace();

  virtual QChar getChar();

  virtual bool skipChar();
  virtual bool skipChar(QChar c);
  virtual bool skipChars(const QString &s);
  virtual bool skipChars(int n);
  virtual bool skipString();
  virtual void skipToEnd();

  virtual void autoSkipSpace() const;

  virtual bool readNonSpace(QString &text);

  virtual bool skipBracedString();
  virtual bool readBracedString(QString &text, bool includeBraces=false);

  virtual bool  readChar(QChar *c);
  virtual QChar readChar();
  virtual bool  unreadChar();

  //virtual bool readUtf8Char(ulong *c);

  virtual bool unreadString(const QString &str);

  virtual bool readNumber(double &real, int &integer, bool &is_real);

  virtual bool readInteger(int *integer);
  virtual bool readBaseInteger(int base, int *integer);
  virtual bool readReal(double *real);
  virtual bool readString(QString &str, bool strip_quotes=false);

  virtual bool skipToChar(QChar c);
  virtual bool readToChar(QChar c, QString &text);

  virtual bool isIdentifier(int offset=0);
  virtual bool readIdentifier(QString &identifier);

  //virtual bool readRealFormat(QString &real_format);
  //virtual bool readIntegerFormat(QString &integer_format);
  //virtual bool readStringFormat(QString &string_format);

  virtual bool isSpace() const;
  virtual bool isNewline() const;

  virtual bool isAlpha() const;
  virtual bool isLower() const;
  virtual bool isUpper() const;
  virtual bool isAlnum() const;
  virtual bool isDigit() const;
  virtual bool isDigitAt(int offset) const;
  virtual bool isODigit() const;
  virtual bool isXDigit() const;

  virtual bool isCharBefore(QChar c, int offset = 1) const;
  virtual bool isChar(QChar c) const;
  virtual bool isCharAfter(QChar c, int offset = 1) const;

  virtual bool isNextChar(QChar c) const;

  virtual bool isString(const QString &str) const;
  virtual bool isOneOf(const QString &str) const;
  virtual bool isWord(const QString &str) const;

  virtual bool eof() const;

  virtual bool neof(int n) const;

  virtual QString stateStr() const {
    return getBefore() + getCharAt() + "\b_" + getAfter();
  }

 private:
  bool skipSpaceI(const QString &str, int *pos);
  bool skipNonSpaceI(const QString &str, int *pos);

  bool skipDoubleQuotedStringI(const QString &str, int *pos);
  bool skipSingleQuotedStringI(const QString &str, int *pos);

  bool toRealI(const QString &str, double *real);
  bool toIntegerI(const QString &str, int *integer);
  bool toIntegerI(const QString &str, long *integer);

  bool readIntegerI(const QString &str, int *pos, int *integer);
  bool readBaseIntegerI(const QString &str, int base, int *pos, int *integer);
  bool readRealI(const QString &str, int *pos, double *real);
  bool readNumberI(const QString &str, int *pos, double &real, int &integer, bool &is_real);

  bool skipIntegerI(const QString &str, int *pos);
  bool skipBaseIntegerI(const QString &str, int base, int *pos);

  bool isBaseCharI(const QChar &c, int base, int *value);

  bool toBaseIntegerI(const QString &str, int base, long *integer);

  bool readIdentifierI(const QString &str, int *pos, QString &identifier);

  bool isIdentifierI(const QString &str, int pos);

 private:
  QString str_;
  int     pos_ { 0 };
  int     len_ { 0 };
  bool    autoSkipSpace_ { false };
  int     lineNum_ { 1 };
};

#endif
