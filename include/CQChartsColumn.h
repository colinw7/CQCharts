#ifndef CQChartsColumn_H
#define CQChartsColumn_H

#include <CQChartsTmpl.h>
#include <CQUtilMeta.h>
#include <CQBaseModelTypes.h>
#include <CSafeIndex.h>

#include <QString>
#include <QStringList>
#include <QVariant>

#include <cassert>
#include <vector>
#include <iostream>

/*!
 * \brief Class to store how column data is referenced from the model
 * \ingroup Charts
 *
 * A column can be:
 *  . column number for model data values in that column
 *  . column number for single header data value
 *  . Group id
 *  . Expression
 *  . Index into data from a column's values
 *
 * The column can also specify the role of the data extracted from the model
 */
class CQChartsColumn :
  public CQChartsComparatorBase<CQChartsColumn>, public CQChartsToStringBase<CQChartsColumn> {
 public:
  enum class Type {
    NONE,
    DATA,       // model row data
    DATA_INDEX, // model row data sub value (e.g. time sub value)
    EXPR,       // expression
    COLUMN_REF, // reference column (by plot column name)
    TCL_DATA,   // tcl data
    ROW,        // model row number
    COLUMN,     // model column number
    CELL,       // model cell data
    HHEADER,    // model horizontal header data
    VHEADER,    // model vertical header data
    GROUP       // model row group id (from group role)
  };

 public:
  using Column = CQChartsColumn;

  using StringToColumnProc = bool (*)(const QString &str, CQChartsColumn &column);

 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsColumn, metaTypeId)

 public:
  static Column makeData(int column, int role=-1) {
    return Column(Type::DATA, column, "", role);
  }

  static Column makeDataIndex(int column, const QString &ind, int role=-1) {
    return Column(Type::DATA_INDEX, column, ind, role);
  }

  static Column makeExpr(const QString &expr) {
    return Column(Type::EXPR, -1, expr, -1);
  }

  static Column makeColumnRef(const QString &str) { return Column(Type::COLUMN_REF, -1, str, -1); }

  static Column makeTclData(const QString &str) { return Column(Type::TCL_DATA, -1, str, -1); }

  static Column makeRow() { return Column(Type::ROW, -1, "", -1); }

  static Column makeColumn() { return Column(Type::COLUMN, -1, "", -1); }

  static Column makeCell() { return Column(Type::CELL, -1, "", -1); }

  static Column makeHHeader(int c) { return Column(Type::HHEADER, c, "", -1); }
  static Column makeVHeader() { return Column(Type::VHEADER, -1, "", -1); }

  static Column makeGroup() { return Column(Type::GROUP, -1, "", -1); }

  //---

  static void setStringToColumnProc(StringToColumnProc proc) {
    s_stringToColumnProc = proc;
  }

 public:
  explicit CQChartsColumn() = default;

  explicit CQChartsColumn(int column, int role=-1); // data

  explicit CQChartsColumn(Type type, int column, const QString &s, int role=-1); // explicit

  explicit CQChartsColumn(const QString &s); // parsed

  CQChartsColumn(const Column &rhs);
  CQChartsColumn(Column &&rhs);

 ~CQChartsColumn();

  Column &operator=(const Column &rhs);
  Column &operator=(Column &&rhs);

  //--

  bool isValid() const { return type_ != Type::NONE; }

  Type type() const { return type_; }

  //--

  bool hasColumn() const {
    return ((type_ == Type::DATA || type_ == Type::DATA_INDEX ||
             type_ == Type::HHEADER) && column_ >= 0);
  }

  int column() const { return (hasColumn() ? column_ : -1); }

  //--

  bool hasRole() const {
    return ((type_ == Type::DATA || type_ == Type::DATA_INDEX) && role_ >= 0);
  }

  int role() const { return (hasRole() ? role_ : -1); }

  int role(int defRole) const { return (hasRole() ? role_ : defRole); }

  //--

  bool hasExpr() const { return (type_ == Type::EXPR && expr_); }

  QString expr() const { return QString(hasExpr() ? expr_ : ""); }

  //--

  bool hasIndex() const { return (type_ == Type::DATA_INDEX && expr_); }

  QString index() const { return QString(hasIndex() ? expr_ : ""); }

  //--

  bool hasRef() const { return (type_ == Type::COLUMN_REF && expr_); }

  QString refName() const { return QString(hasRef() ? expr_ : ""); }

  //--

  bool isTclData() const { return (type_ == Type::TCL_DATA && expr_); }

  QString tclData() const { return QString(isTclData() ? expr_ : ""); }

  //--

  bool isRow() const { return (type_ == Type::ROW); }

  int rowOffset() const { return (role_ > 0 ? role_ : 0); }

  //--

  bool isColumn() const { return (type_ == Type::COLUMN); }

  void setColumnCol(int col) { assert(isColumn()); role_ = col; }
  int columnCol() const { assert(isColumn()); return role_; }

  //--

  bool isCell() const { return (type_ == Type::CELL); }

  void setCellCol(int col) { assert(isCell()); role_ = col; }
  int cellCol() const { assert(isCell()); return role_; }

  //--

  bool isHHeader() const { return (type_ == Type::HHEADER); }
  bool isVHeader() const { return (type_ == Type::VHEADER); }

  //--

  bool isGroup() const { return (type_ == Type::GROUP); }

  //--

  // header name for column e.g. expression
  bool hasName() const { return name_ && strlen(name_); }

  QString name() const { return QString(hasName() ? name_ : ""); }
  void setName(const QString &name);

  //---

  bool setValue(const QString &str);

  //---

  QString toString() const;

  bool fromString(const QString &s);

  //---

  int cmp(const Column &c) const;

  friend int cmp(const Column &c1, const Column &c2) {
    return c1.cmp(c2);
  }

  //---

 public:
  using Columns = std::vector<Column>;

  static bool stringToColumns(const QString &str, Columns &columns);

  static QString columnsToString(const Columns &columns);

  void reset() {
    delete [] expr_;
    delete [] name_;

    type_   = Type::NONE;
    column_ = -1;
    role_   = -1;
    expr_   = nullptr;
    name_   = nullptr;
  }

  //---

  CQBaseModelType getTclType(bool &ok) const;

  QVariant getTclValue(int i, bool &ok) const;

 private:
  bool hasExprPtr() const { return hasExprPtr(type_); }

  static bool hasExprPtr(const Type &type) {
    return (type == Type::EXPR || type == Type::DATA_INDEX ||
            type == Type::COLUMN_REF || type == Type::TCL_DATA);
  }

  bool decodeString(const QString &str, Type &type, int &column, int &role,
                    QString &expr, QString &name);

  void updateType();

 private:
  static StringToColumnProc s_stringToColumnProc;

  Type  type_   { Type::NONE }; //!< column type
  int   column_ { -1 };         //!< column number for data (-1 unset)
  int   role_   { -1 };         //!< column role for data (-1 unset)
  char* expr_   { nullptr };    //!< column expression (EXPR) or index string (DATA_INDEX)
  char* name_   { nullptr };    //!< column name
};

//---

/*!
 * \brief manage list of columns or single column
 * \ingroup Charts
 */
class CQChartsColumns :
  public CQChartsComparatorBase<CQChartsColumns>, public CQChartsToStringBase<CQChartsColumns> {
 public:
  using Column  = CQChartsColumn;
  using Columns = std::vector<Column>;

 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsColumns, metaTypeId)

 public:
  explicit CQChartsColumns() = default;

  explicit CQChartsColumns(const Column &c) {
    setColumn(c);
  }

  explicit CQChartsColumns(const QString &s) {
    setColumnsStr(s);
  }

  //---

  // get single column
  const Column &column() const { return column_; }

  // set single column (multiple columns empty if column invalid)
  void setColumn(const Column &c) {
    column_ = c;

    columns_.clear();

    if (column_.isValid())
      columns_.push_back(column_);
  }

  // add column
  void addColumn(const Column &c) {
    if (columns_.empty())
      setColumn(c);
    else
      columns_.push_back(c);
  }

  // remove column
  void removeColumn() {
    if (! columns_.empty())
      columns_.pop_back();
    else
      column_ = Column();
  }

  // get multiple columns
  const Columns &columns() const { return columns_; }

  // set multiple columns (single column is invalid if columns empty)
  void setColumns(const Columns &columns) {
    columns_ = columns;

    if (! columns_.empty())
      column_ = columns_[0];
    else
      column_ = Column();
  }

  QString columnsStr() const {
    return Column::columnsToString(columns_);
  }

  bool setColumnsStr(const QString &s) {
    Columns cols;

    if (! Column::stringToColumns(s, cols))
      return false;

    setColumns(cols);

    return true;
  }

  bool isValid() const {
    for (const auto &column : columns_)
      if (column.isValid())
        return true;

    return false;
  }

  // return number of columns (minimum is one as single invalid counts)
  int count() const {
    if (columns_.empty())
      return 1;

    return int(columns_.size());
  }

  const Column &getColumn(int i) const {
    if (! columns_.empty())
      return CUtil::safeIndex(columns_, i);

    assert(i == 0);

    return column_;
  }

  void setColumn(int i, const Column &column) {
    if (i == 0 && ! isValid()) {
      setColumn(column);

      return;
    }

    assert(i >= 0 && i < int(columns_.size()));

    columns_[size_t(i)] = column;

    if (i == 0)
      column_ = columns_[0];
  }

  //---

  int columnInd(const Column &c) const {
    int ind = 0;

    for (const auto &column : columns_) {
      if (c == column)
        return ind;

      ++ind;
    }

    return -1;
  }

  bool hasColumn(const Column &c) const {
    return (columnInd(c) >= 0);
  }

  //---

  friend bool operator==(const CQChartsColumns &lhs, const CQChartsColumns &rhs) {
    int nl = int(lhs.columns_.size());
    int nr = int(rhs.columns_.size());

    if (nl != nr)
      return false;

    if (! nl)
      return (lhs.column_ == rhs.column_);

    for (int i = 0; i < nl; ++i) {
      if (lhs.getColumn(i) != rhs.getColumn(i))
        return false;
    }

    return true;
  }

  //---

  Columns::iterator begin() { return columns_.begin(); }
  Columns::iterator end  () { return columns_.end  (); }

  Columns::const_iterator begin() const { return columns_.begin(); }
  Columns::const_iterator end  () const { return columns_.end  (); }

  //---

  QString toString() const { return columnsStr(); }

  bool fromString(const QString &s) { return setColumnsStr(s); }

  //---

  int cmp(const CQChartsColumns &c) const;

  friend int cmp(const CQChartsColumns &c1, const CQChartsColumns &c2) {
    return c1.cmp(c2);
  }

  //---

 private:
  Column  column_;  //!< single column
  Columns columns_; //!< multiple columns
};

//---

CQUTIL_DCL_META_TYPE(CQChartsColumn)
CQUTIL_DCL_META_TYPE(CQChartsColumns)

#endif
