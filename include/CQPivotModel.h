#ifndef CQPivotModel_H
#define CQPivotModel_H

#include <CQBaseModel.h>
#include <QStringList>
#include <QString>
#include <map>
#include <set>
#include <vector>
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

 public:
  enum class ValueType {
    COUNT,
    COUNT_UNIQUE,
    SUM,
    MIN,
    MAX,
    MEAN
  };

  Q_ENUMS(ValueType);

  using Column  = int;
  using Columns = std::vector<Column>;
  using Inds    = std::vector<QModelIndex>;

 public:
  CQPivotModel(QAbstractItemModel *model);
 ~CQPivotModel();

  //---

  // get/set source model
  QAbstractItemModel *sourceModel() const;
  void setSourceModel(QAbstractItemModel *sourceModel);

  //---

  void setHColumns(const Columns &columns) { hColumns_ = columns; invalidateModel(); }
  void setVColumns(const Columns &columns) { vColumns_ = columns; invalidateModel(); }

  void setValueColumn(const Column &column) { valueColumn_ = column; invalidateModel(); }

  ValueType valueType() const { return valueType_; }
  void setValueType(ValueType valueType) { valueType_ = valueType; invalidateModel(); }

  const QString &hheader() const { return hheader_; }
  const QString &vheader() const { return vheader_; }

  //---

  bool isIncludeTotals() const { return includeTotals_; }
  void setIncludeTotals(bool b) { includeTotals_ = b; invalidateModel(); }

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

  double hmin(int c) const;
  double hmax(int c) const;

  double vmin(int r) const;
  double vmax(int r) const;

 private:
  void invalidateModel() { modelValid_ = false; }

  void updateModel() const;
  void updateModel();

  void calcData();

 private:
  class Keys {
   public:
    Keys() { }

    void add(const QString &str) {
      keys_.push_back(str);

      if (key_ != "")
        key_ += "/";

      key_ += str;
    }

    bool empty() const { return keys_.empty(); }

    QString key() const { return key_; }

   private:
    using Strs = std::vector<QString>;

    QString key_;
    Strs    keys_;
  };

  using KeyInd  = std::map<QString,int>;
  using IndKeys = std::map<int,Keys>;

  class Values {
   public:
    Values() { }

    // add real value
    void add(double r) {
      min_ = (! rvalues_.empty() ? std::min(min_, r) : r);
      max_ = (! rvalues_.empty() ? std::max(max_, r) : r);

      sum_ += r;

      rvalues_.push_back(r);
    }

    // add string value and index
    void add(const QModelIndex &ind, const QString &s) {
      inds_   .push_back(ind);
      svalues_.insert(s);
    }

    double sum() const { return sum_; }
    double min() const { return min_; }
    double max() const { return max_; }

    double mean() const { int nv = rvalues_.size(); return (nv > 0 ? sum_/nv : 0.0); }

    int count      () const { return inds_.size(); }
    int countUnique() const { return svalues_.size(); }

    const Inds &inds() const { return inds_; }

   private:
    using Reals   = std::vector<double>;
    using Strings = std::set<QString>;

    Reals   rvalues_;
    Inds    inds_;
    Strings svalues_;
    double  sum_   { 0.0 };
    double  min_   { 0.0 };
    double  max_   { 0.0 };
  };

  using VValues  = std::map<QString,Values>;
  using HVValues = std::map<QString,VValues>;

  //---

  struct ValueData {
    bool   set { false };
    double min { 0.0 };
    double max { 0.0 };
    double sum { 0.0 };
  };

  using ValueDatas = std::vector<ValueData>;

 private:
  double typeValue(const Values &values) const;

 private:
  QAbstractItemModel* sourceModel_   { nullptr };        //!< parent model
  Columns             hColumns_;                         //!< horizontal key columns
  Columns             vColumns_;                         //!< vertical key columns
  Column              valueColumn_   { -1 };             //!< value column
  ValueType           valueType_     { ValueType::SUM }; //!< value type
  bool                includeTotals_ { true };           //!< include totals for rows/columns

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
