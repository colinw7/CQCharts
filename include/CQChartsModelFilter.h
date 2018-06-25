#ifndef CQChartsModelFilter_H
#define CQChartsModelFilter_H

#include <CQChartsRegExp.h>
#include <QSortFilterProxyModel>
#include <set>
#include <cassert>

class CQCharts;
class CQChartsModelExprMatch;
class QItemSelectionModel;

class CQChartsModelFilterData {
 public:
  enum class Type {
    EXPRESSION,
    REGEXP,
    SIMPLE,
    SELECTED
  };

 public:
  using ColumnFilterMap = std::map<int,CQChartsRegExp>;

 public:
  CQChartsModelFilterData() { }

  const Type &type() const { return type_; }
  void setType(const Type &v) { type_ = v; }

  const QString &filter() const { return filter_; }
  void setFilter(const QString &filter) { filter_ = filter; }

  bool isExpr    () const { return type_ == Type::EXPRESSION; }
  bool isRegExp  () const { return type_ == Type::REGEXP;     }
  bool isSimple  () const { return type_ == Type::SIMPLE;     }
  bool isSelected() const { return type_ == Type::SELECTED;   }

  bool isInvert() const { return invert_; }
  void setInvert(bool b) { invert_ = b; }

  const CQChartsRegExp &regexp() const { return regexp_; }
  void setRegExp(const CQChartsRegExp &regexp) { regexp_ = regexp; }

  const QModelIndexList &filterRows() const { return filterRows_; }
  void setFilterRows(const QModelIndexList &filterRows) { filterRows_ = filterRows; }

  const ColumnFilterMap &columnFilterMap() const { return columnFilterMap_; }
  void setColumnFilterMap(const ColumnFilterMap &v) { columnFilterMap_ = v; }

  bool isEmpty() const {
    if (! isSelected())
      return ! filter_.length();
    else
      return false;
  }

 private:
  Type            type_           { Type::EXPRESSION };
  QString         filter_;
  bool            invert_         { false };
  CQChartsRegExp  regexp_;
  QModelIndexList filterRows_;
  ColumnFilterMap columnFilterMap_;
};

//------

class CQChartsExprModel;

class CQChartsModelFilter : public QSortFilterProxyModel {
  Q_OBJECT

  Q_PROPERTY(QString filter READ filter   WRITE setFilter)
  Q_PROPERTY(bool    invert READ isInvert WRITE setInvert)

  // model indices are from source model
 public:
  CQChartsModelFilter(CQCharts *charts);

 ~CQChartsModelFilter();

  CQChartsModelExprMatch *exprMatch() const { return expr_; }

  virtual CQChartsExprModel  *exprModel() const = 0;
  virtual QAbstractItemModel *baseModel() const = 0;

  QItemSelectionModel *selectionModel() const { return selectionModel_; }
  void setSelectionModel(QItemSelectionModel *sm) { selectionModel_ = sm; }

  void setFilterType(const CQChartsModelFilterData::Type &type) {
    currentFilterData().setType(type);
  }

  const QString &filter() const { return currentFilterData().filter(); }
  void setFilter(const QString &filter) { currentFilterData().setFilter(filter); }

  bool isExpr    () const { return currentFilterData().isExpr(); }
  bool isRegExp  () const { return currentFilterData().isRegExp(); }
  bool isSelected() const { return currentFilterData().isSelected(); }

  bool isInvert() const { return currentFilterData().isInvert(); }
  void setInvert(bool b) { currentFilterData().setInvert(b); }

  void resetFilterData();

  void pushFilterData();
  void popFilterData();

  void setExpressionFilter(const QString &filter);

  void setRegExpFilter(const QString &filter);

  void setSimpleFilter(const QString &filter);

  void setSelectionFilter(bool invert);

  bool filterAcceptsRow(int row, const QModelIndex &parent) const override;

  bool acceptsItem(const QModelIndex &ind) const;

  //---

  QVariant data(const QModelIndex &index, int role) const;

  //---

  void addExpand(const QModelIndex &ind) const { expand_.insert(ind); }

 protected:
  const CQChartsModelFilterData &currentFilterData() const {
    assert(! filterDatas_.empty());

    return filterDatas_.back();
  }

  CQChartsModelFilterData &currentFilterData() {
    assert(! filterDatas_.empty());

    return filterDatas_.back();
  }

  void initFilterData(CQChartsModelFilterData &filterData);

  void initFilter();

  bool allFiltersEmpty() const;

  bool anyChildMatch(const QModelIndex &ind) const;

  bool itemMatch(const QModelIndex &ind) const;

  bool filterItemMatch(const CQChartsModelFilterData &filterData, const QModelIndex &ind) const;

 protected:
  using IndexMatches = std::map<QModelIndex,bool>;
  using ExpandInds   = std::set<QModelIndex>;

  using FilterDatas = std::vector<CQChartsModelFilterData>;

  CQCharts*               charts_         { nullptr };
  CQChartsModelExprMatch* expr_           { nullptr };
  QItemSelectionModel*    selectionModel_ { nullptr };
  FilterDatas             filterDatas_;
  mutable IndexMatches    matches_;
  mutable ExpandInds      expand_;
};

#endif
