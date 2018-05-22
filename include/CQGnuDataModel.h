#ifndef CQGnuDataModel_H
#define CQGnuDataModel_H

#include <CQDataModel.h>
#include <boost/optional.hpp>

class CQGnuDataModel : public CQDataModel {
  Q_OBJECT

  Q_PROPERTY(bool    commentHeader    READ isCommentHeader   WRITE setCommentHeader   )
  Q_PROPERTY(bool    firstLineHeader  READ isFirstLineHeader WRITE setFirstLineHeader )
  Q_PROPERTY(QString commentChars     READ commentChars      WRITE setCommentChars    )
  Q_PROPERTY(QString missingStr       READ missingStr        WRITE setMissingStr      )
  Q_PROPERTY(char    separator        READ separator         WRITE setSeparator       )
  Q_PROPERTY(bool    parseStrings     READ isParseStrings    WRITE setParseStrings    )
  Q_PROPERTY(int     setBlankLines    READ setBlankLines     WRITE setSetBlankLines   )
  Q_PROPERTY(int     subSetBlankLines READ subSetBlankLines  WRITE setSubSetBlankLines)
  Q_PROPERTY(bool    keepQuotes       READ isKeepQuotes      WRITE setKeepQuotes      )

 public:
  using Fields    = std::vector<QString>;
  using FieldsSet = std::vector<Fields>;
  using SubSet    = std::vector<FieldsSet>;
  using Set       = std::vector<SubSet>;

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
  using LineString   = std::map<int,QString>;
  using SubSetString = std::map<int,LineString>;
  using SetString    = std::map<int,SubSetString>;
  using OptString    = boost::optional<QString>;

  bool        commentHeader_    { true };  // use comment line for header
  bool        firstLineHeader_  { false }; // use first line for header
  OptString   commentChars_;               // comment start string
  QString     missingStr_;                 // missing string
  char        separator_        { '\0' };  // field separator
  bool        parseStrings_     { true };  // parse strings
  int         setBlankLines_    { 2 };     // number of blank lines between sets
  int         subSetBlankLines_ { 1 };     // number of blank lines between sub sets
  bool        keepQuotes_       { false }; // key quotes for strings
  SetString   commentStrs_;                // comment strings
  QString     filename_;                   // filename
  QStringList lines_;                      // file lines
  Set         set_;                        // set of sub sets of lines
  Fields      columnHeaderFields_;         // column headers
  int         maxNumFields_     { 0 };     // max number of fields in file lines
};

#endif