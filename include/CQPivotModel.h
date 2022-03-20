#ifndef CQPivotModel_H
#define CQPivotModel_H

#include <CQBaseModel.h>
#include <QStringList>
#include <QString>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <cassert>

/*!
 * \brief pivot model
 *
 * . header   is x key names
 * . column 0 is y key names
 * . cells    are calculated values for x/y key
 *
 * If include totals there is an extra row and column for the column/row totals
 */
class CQPivotModel : public CQBaseModel {
  Q_OBJECT

  Q_PROPERTY(ValueType valueType     READ valueType       WRITE setValueType)
  Q_PROPERTY(bool      includeTotals READ isIncludeTotals WRITE setIncludeTotals)
  Q_PROPERTY(QVariant  fillValue     READ fillValue       WRITE setFillValue)
  Q_PROPERTY(QChar     separator     READ separator       WRITE setSeparator)

 public:
  enum class ValueType {
    NONE,
    COUNT,
    COUNT_UNIQUE,
    SUM,
    MIN,
    MAX,
    MEAN
  };

  Q_ENUMS(ValueType)

  using Column           = int;
  using Columns          = std::vector<Column>;
  using Inds             = std::vector<QModelIndex>;
  using ValueTypes       = std::vector<ValueType>;
  using ColumnValueTypes = std::map<Column, ValueTypes>;
  using ColumnType       = CQBaseModelType;

 public:
  CQPivotModel(QAbstractItemModel *model);
 ~CQPivotModel();

  //---

  //! get/set source model
  QAbstractItemModel *sourceModel() const;
  void setSourceModel(QAbstractItemModel *sourceModel);

  //---

  //! get/set horizontal columns
  const Columns &hColumns() const { return hColumns_; }
  void setHColumns(const Columns &columns) { hColumns_ = columns; invalidateModel(); }

  //! get/set vertical columns
  const Columns &vColumns() const { return vColumns_; }
  void setVColumns(const Columns &columns) { vColumns_ = columns; invalidateModel(); }

  //! get/set value columns
  const Columns &valueColumns() const { return valueColumns_; }
  void setValueColumns(const Columns &column) { valueColumns_ = column; invalidateModel(); }

  //---

  //! get/set column value types
  const ColumnValueTypes &columnValueTypes() const { return columnValueTypes_; }
  void setColumnValueTypes(const ColumnValueTypes &v) { columnValueTypes_ = v; invalidateModel(); }

  //! get/set value types
  ValueTypes valueTypes() const { return columnValueTypes(-1); }
  void setValueTypes(const ValueTypes &v) { setColumnValueTypes(-1, v); }

  ValueTypes columnValueTypes(Column column) const {
    auto p = columnValueTypes_.find(column);
    if (p == columnValueTypes_.end()) return ValueTypes();
    return (*p).second;
  }
  void setColumnValueTypes(Column column, const ValueTypes &v) {
    columnValueTypes_[column] = v; invalidateModel();
  }

  //! get/set single value type
  ValueType valueType() const {
    auto valueTypes = this->valueTypes();
    return (! valueTypes.empty() ? valueTypes[0] : ValueType::SUM);
  }
  void setValueType(ValueType valueType) {
    ValueTypes valueTypes;
    valueTypes.push_back(valueType);
    setValueTypes(valueTypes);
  }

  //! convert value type to/from string
  static ValueType stringToValueType(const QString &str);
  static QString valueTypeToString(ValueType valueType);

  //---

  //! get horizontal header
  const QString &hheader() const { return hheader_; }

  //! get vertical header
  const QString &vheader() const { return vheader_; }

  //---

  //! get/set include totals
  bool isIncludeTotals() const { return includeTotals_; }
  void setIncludeTotals(bool b) { includeTotals_ = b; invalidateModel(); }

  //! get/set fill value
  const QVariant &fillValue() const { return fillValue_; }
  void setFillValue(const QVariant &v) { fillValue_ = v; }

  const QChar &separator() const { return separator_; }
  void setSeparator(const QChar &v) { separator_ = v; }

  //---

  ColumnType columnType(Column column) const;
  void setColumnType(Column column, const ColumnType &t);

  //---

  // # Abstract Model APIS

  //! get column count
  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  //! get child row count of index
  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

  //! get child of parent at row/column
  QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const override;

  //! get parent of child
  QModelIndex parent(const QModelIndex &child) const override;

  //! does parent have children
  bool hasChildren(const QModelIndex &parent=QModelIndex()) const override;

  //! get/set role data for index
  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;
  bool setData(const QModelIndex &index, const QVariant &value,
               int role=Qt::DisplayRole) override;

  //! get/set header data for column/section
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role=Qt::DisplayRole) const override;
  bool setHeaderData(int section, Qt::Orientation orientation,
                     const QVariant &value, int role=Qt::DisplayRole) override;

  //! get flags for index
  Qt::ItemFlags flags(const QModelIndex &index) const override;

  //---

  bool modelInds(const QString &hkey, const QString &vkey, Inds &inds) const;

  //---

  QStringList hkeys(bool sorted) const;
  QStringList vkeys(bool sorted) const;

  int hkeyCol(const QString &key) const;
  int vkeyRow(const QString &key) const;

  QVariant hmin(int c) const;
  QVariant hmax(int c) const;

  QVariant vmin(int r) const;
  QVariant vmax(int r) const;

 private:
  void invalidateModel() { modelValid_ = false; }

  void updateModel() const;
  void updateModel();

  void calcData();

  QVariant calcFillValue() const;

 private:
  class KeyString {
   public:
    enum Type {
      STRING,
      VALUE_TYPE
    };

   public:
    KeyString() { }

    KeyString(Type type, const QString &str) :
     type_(type), str_(str) {
    }

    const QString &str() const { return str_; }

    friend bool operator<(const KeyString &lhs, const KeyString &rhs) {
      if (lhs.type_ != rhs.type_)
        return (lhs.type_ < rhs.type_);

      return (lhs.str_ < rhs.str_);
    }

   private:
    Type    type_ { Type::STRING };
    QString str_;
  };

  //---

  class Keys {
   public:
    Keys() { }

    explicit Keys(const QChar &sep) :
     sep_(sep) {
    }

    Keys(const KeyString &str, const QChar &sep) :
     sep_(sep) {
      add(str);
    }

    void add(const KeyString &str) { keys_.push_back(str); key_ = ""; }

    void addFront(const KeyString &str) { keys_.push_front(str); key_ = ""; }

    bool empty() const { return keys_.empty(); }

    QString key() const {
      if (key_ == "") {
        for (const auto &key : keys_) {
          if (key_ != "")
            key_ += sep_;

          key_ += key.str();
        }
      }

      return key_;
    }

    friend bool operator<(const Keys &lhs, const Keys &rhs) {
      return (lhs.key() < rhs.key());
    }

   private:
    using Strs = std::deque<KeyString>;

    QChar           sep_ { '/' };
    Strs            keys_;
    mutable QString key_;
  };

  using KeyInd  = std::map<Keys, int>;
  using IndKeys = std::map<int, Keys>;

  class Values {
   public:
    using Reals = std::vector<double>;

   public:
    Values() { }

    //! get/set data type
    const ColumnType &dataType() const { return dataType_; }
    void setDataType(const ColumnType &v) { dataType_ = v; }

    //! get/set value type
    const ValueType &valueType() const { return valueType_; }
    void setValueType(const ValueType &v) { valueType_ = v; }

    // add real value
    void addReal(double r) {
      if (dataType_ == ColumnType::NONE)
        dataType_ = ColumnType::REAL;

      rmin_ = (! rvalues_.empty() ? std::min(rmin_, r) : r);
      rmax_ = (! rvalues_.empty() ? std::max(rmax_, r) : r);

      rsum_ += r;

      rvalues_.push_back(r);
    }

    // add string value and index
    void addValue(const QModelIndex &ind, const QString &s) {
      if (dataType_ == ColumnType::NONE)
        dataType_ = ColumnType::STRING;

      smin_ = (! svalues_.empty() ? std::min(smin_, s) : s);
      smax_ = (! svalues_.empty() ? std::max(smax_, s) : s);

      ssum_ += s;

      svalues_.insert(s);

      inds_.push_back(ind);

    }

    double rsum() const { return rsum_; }
    double rmin() const { return rmin_; }
    double rmax() const { return rmax_; }

    const QString &ssum() const { return ssum_; }
    const QString &smin() const { return smin_; }
    const QString &smax() const { return smax_; }

    double rmean() const { int nv = int(rvalues_.size()); return (nv > 0 ? rsum_/nv : 0.0); }

    int count      () const { return int(inds_.size()); }
    int countUnique() const { return int(svalues_.size()); }

    const Inds &inds() const { return inds_; }

    int          rcount () const { return int(rvalues_.size()); }
    const Reals &rvalues() const { return rvalues_; }

   private:
    using Strings = std::set<QString>;

    ColumnType dataType_  { ColumnType::NONE }; //!< data type
    ValueType  valueType_ { ValueType::SUM };   //!< value calculation type
    Reals      rvalues_;                        //!< real values
    Strings    svalues_;                        //!< string values
    Inds       inds_;                           //!< model indices
    double     rsum_       { 0.0 };             //!< real sum
    double     rmin_       { 0.0 };             //!< real min
    double     rmax_       { 0.0 };             //!< real max
    QString    ssum_;                           //!< string sum
    QString    smin_;                           //!< string min
    QString    smax_;                           //!< string max
  };

  using VValues  = std::map<Keys, Values>;
  using HVValues = std::map<Keys, VValues>;

  //---

  struct ValueData {
    bool     set { false }; //!< is value set
    QVariant min;           //!< min value
    QVariant max;           //!< max value
    QVariant sum;           //!< sum value
  };

  using ValueDatas = std::vector<ValueData>;

  using ColumnTypes = std::map<Column, ColumnType>;

 private:
  QVariant typeValue(const Values &values) const;

 private:
  QAbstractItemModel* sourceModel_   { nullptr }; //!< parent model
  Columns             hColumns_;                  //!< horizontal key columns
  Columns             vColumns_;                  //!< vertical key columns
  Columns             valueColumns_;              //!< value columns
  ColumnValueTypes    columnValueTypes_;          //!< per column value types
  bool                includeTotals_ { true };    //!< include totals for rows/columns
  QVariant            fillValue_;                 //!< fill value
  QChar               separator_     { '/' };     //!< separator
  ColumnTypes         columnTypes_;               //!< column types

  // calculated data
  bool                modelValid_ { false }; //!< is data value
  KeyInd              hKeysCol_;             //!< horizontal key to column
  IndKeys             hColKeys_;             //!< horizontal column to key
  KeyInd              vKeysRow_;             //!< vertical key to roe
  IndKeys             vRowKeys_;             //!< row to vertical key
  HVValues            values_;               //!< grid values
  QString             hheader_;              //!< horizontal header
  QString             vheader_;              //!< vertical header
  ValueDatas          vdata_;                //!< vertical row data
  ValueDatas          hdata_;                //!< horizontal column data
  ValueData           data_;                 //!< data summary
};

#endif
