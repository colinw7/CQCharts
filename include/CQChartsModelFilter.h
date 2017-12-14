#ifndef CQChartsModelFilter_H
#define CQChartsModelFilter_H

#include <CQChartsRegExp.h>
#include <QSortFilterProxyModel>
#include <set>
#include <cassert>

class CQChartsModelExprMatch;
class QItemSelectionModel;

class CQChartsModelFilterData {
 public:
  enum class Type {
    EXPRESSION,
    REGEXP,
    SELECTED
  };

 public:
  CQChartsModelFilterData() { }

  const Type &type() const { return type_; }
  void setType(const Type &v) { type_ = v; }

  const QString &filter() const { return filter_; }
  void setFilter(const QString &filter) { filter_ = filter; }

  bool isExpr    () const { return type_ == Type::EXPRESSION; }
  bool isRegExp  () const { return type_ == Type::REGEXP; }
  bool isSelected() const { return type_ == Type::SELECTED; }

  const CQChartsRegExp &regexp() const { return regexp_; }
  void setRegExp(const CQChartsRegExp &regexp) { regexp_ = regexp; }

  const QModelIndexList &filterRows() const { return filterRows_; }
  void setFilterRows(const QModelIndexList &filterRows) { filterRows_ = filterRows; }

  bool isInvert() const { return invert_; }
  void setInvert(bool b) { invert_ = b; }

  bool isEmpty() const {
    if (! isSelected())
      return ! filter_.length();
    else
      return false;
  }

 private:
  Type            type_           { Type::EXPRESSION };
  QString         filter_;
  bool            isExpr_         { true };
  bool            filterSelected_ { false };
  bool            invert_         { false };
  CQChartsRegExp  regexp_;
  QModelIndexList filterRows_;
};

//------

class CQChartsModelFilter : public QSortFilterProxyModel {
  Q_OBJECT

  Q_PROPERTY(QString filter READ filter   WRITE setFilter)
  Q_PROPERTY(bool    invert READ isInvert WRITE setInvert)

  // model indices are from source model
 public:
  CQChartsModelFilter();

 ~CQChartsModelFilter();

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

  void setSelectionFilter(bool invert);

  bool filterAcceptsRow(int row, const QModelIndex &parent) const override;

  bool acceptsItem(const QModelIndex &ind) const;

 private:
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

 private:
  typedef std::map<QModelIndex,bool> IndexMatches;
  typedef std::set<QModelIndex>      ExpandInds;

  using FilterDatas = std::vector<CQChartsModelFilterData>;

  CQChartsModelExprMatch* expr_           { nullptr };
  QItemSelectionModel*    selectionModel_ { nullptr };
  FilterDatas             filterDatas_;
  mutable IndexMatches    matches_;
  mutable ExpandInds      expand_;
};

#endif
