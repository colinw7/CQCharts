#ifndef CQChartsData_H
#define CQChartsData_H

#include <CQChartsModel.h>
#include <COptVal.h>

class CQChartsData : public CQChartsModel {
  Q_OBJECT

 public:
  typedef std::vector<std::string> Fields;
  typedef std::vector<Fields>      FieldsSet;
  typedef std::vector<FieldsSet>   SubSet;
  typedef std::vector<SubSet>      Set;

 public:
  CQChartsData();

  bool isCommentHeader() const { return commentHeader_; }
  void setCommentHeader(bool b) { commentHeader_ = b; }

  bool isFirstLineHeader() const { return firstLineHeader_; }
  void setFirstLineHeader(bool b) { firstLineHeader_ = b; }

  std::string commentChars() const { return commentChars_.getValue("#"); }
  void setCommentChars(const std::string &chars) { commentChars_ = chars; }

  const std::string &missingStr() const { return missingStr_; }
  void setMissingStr(const std::string &chars) { missingStr_ = chars; }

  char separator() const { return separator_; }
  void setSeparator(char c) { separator_ = c; }

  bool isParseStrings() const { return parseStrings_; }
  void setParseStrings(bool b) { parseStrings_ = b; }

  int setBlankLines() const { return setBlankLines_; }
  void setSetBlankLines(int i) { setBlankLines_ = i; }

  int subSetBlankLines() const { return subSetBlankLines_; }
  void setSubSetBlankLines(int i) { subSetBlankLines_ = i; }

  bool load(const QString &filename);

 private:
  void parseFileLine(const QString &str, Fields &fields);

 private:
  typedef std::map<int,std::string>  LineString;
  typedef std::map<int,LineString>   SubSetString;
  typedef std::map<int,SubSetString> SetString;

  bool        commentHeader_   { true };
  bool        firstLineHeader_ { false };
  COptString  commentChars_;
  std::string missingStr_;
  char        separator_    { '\0' };
  bool        parseStrings_ { true };
  QStringList lines_;
  QString     filename_;
  Set         set_;
  SetString   commentStrs_;
  Fields      columnHeaderFields_;
  int         maxNumFields_     { 0 };
  int         setBlankLines_    { 2 };
  int         subSetBlankLines_ { 1 };
};

#endif
