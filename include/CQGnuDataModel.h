#ifndef CQGnuDataModel_H
#define CQGnuDataModel_H

#include <QAbstractItemModel>
#include <boost/optional.hpp>

class CQGnuDataModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  typedef std::vector<std::string> Fields;
  typedef std::vector<Fields>      FieldsSet;
  typedef std::vector<FieldsSet>   SubSet;
  typedef std::vector<SubSet>      Set;

 public:
  CQGnuDataModel();

  ~CQGnuDataModel() { }

  bool isCommentHeader() const { return commentHeader_; }
  void setCommentHeader(bool b) { commentHeader_ = b; }

  bool isFirstLineHeader() const { return firstLineHeader_; }
  void setFirstLineHeader(bool b) { firstLineHeader_ = b; }

  std::string commentChars() const { return commentChars_.get_value_or("#"); }
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

  //---

  void addColumn(const QString &name);

  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  QVariant headerData(int section, Qt::Orientation orientation=Qt::Horizontal,
                      int role=Qt::DisplayRole) const override;

  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                     int role=Qt::DisplayRole) override;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const;

 private:
  void parseFileLine(const QString &str, Fields &fields);

 private:
  class Column {
   public:
    Column(const QString &name="", const QString &type="") :
     name_(name), type_(type) {
    }

    const QString &name() const { return name_; }
    void setName(const QString &v) { name_ = v; }

    const QString &type() const { return type_; }
    void setType(const QString &v) { type_ = v; }

   private:
    QString name_;
    QString type_;
  };

  //---

  typedef std::map<int,std::string>    LineString;
  typedef std::map<int,LineString>     SubSetString;
  typedef std::map<int,SubSetString>   SetString;
  typedef boost::optional<std::string> OptString;
  typedef std::vector<QString>         Cells;
  typedef std::vector<Cells>           Data;
  typedef std::vector<Column>          Columns;

  bool        commentHeader_   { true };
  bool        firstLineHeader_ { false };
  OptString   commentChars_;
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
  Columns     columns_;
  Data        data_;
};

#endif
