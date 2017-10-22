#ifndef CQGnuDataModel_H
#define CQGnuDataModel_H

#include <CQDataModel.h>
#include <boost/optional.hpp>

class CQGnuDataModel : public CQDataModel {
  Q_OBJECT

 public:
  typedef std::vector<QString>   Fields;
  typedef std::vector<Fields>    FieldsSet;
  typedef std::vector<FieldsSet> SubSet;
  typedef std::vector<SubSet>    Set;

 public:
  CQGnuDataModel();

 ~CQGnuDataModel() { }

  bool isCommentHeader() const { return commentHeader_; }
  void setCommentHeader(bool b) { commentHeader_ = b; }

  bool isFirstLineHeader() const { return firstLineHeader_; }
  void setFirstLineHeader(bool b) { firstLineHeader_ = b; }

  QString commentChars() const { return commentChars_.get_value_or("#"); }
  void setCommentChars(const QString &chars) { commentChars_ = chars; }

  const QString &missingStr() const { return missingStr_; }
  void setMissingStr(const QString &chars) { missingStr_ = chars; }

  char separator() const { return separator_; }
  void setSeparator(char c) { separator_ = c; }

  bool isParseStrings() const { return parseStrings_; }
  void setParseStrings(bool b) { parseStrings_ = b; }

  int setBlankLines() const { return setBlankLines_; }
  void setSetBlankLines(int i) { setBlankLines_ = i; }

  int subSetBlankLines() const { return subSetBlankLines_; }
  void setSubSetBlankLines(int i) { subSetBlankLines_ = i; }

  bool isKeepQuotes() const { return keepQuotes_; }
  void setKeepQuotes(bool b) { keepQuotes_ = b; }

  bool load(const QString &filename);

  //---

  void addColumn(const QString &name);

 private:
  void parseFileLine(const QString &str, Fields &fields);

 private:
  typedef std::map<int,QString>      LineString;
  typedef std::map<int,LineString>   SubSetString;
  typedef std::map<int,SubSetString> SetString;
  typedef boost::optional<QString>   OptString;

  bool        commentHeader_    { true };
  bool        firstLineHeader_  { false };
  OptString   commentChars_;
  QString     missingStr_;
  char        separator_        { '\0' };
  bool        parseStrings_     { true };
  QStringList lines_;
  QString     filename_;
  Set         set_;
  SetString   commentStrs_;
  Fields      columnHeaderFields_;
  int         maxNumFields_     { 0 };
  int         setBlankLines_    { 2 };
  int         subSetBlankLines_ { 1 };
  bool        keepQuotes_       { false };
};

#endif
