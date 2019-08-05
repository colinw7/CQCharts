#ifndef CQChartsModelFilter_H
#define CQChartsModelFilter_H

#include <CQChartsRegExp.h>

#include <QObject>
#include <QSortFilterProxyModel>

#include <set>
#include <future>
#include <cassert>

class CQCharts;
class CQChartsModelExprMatch;
class QItemSelectionModel;

/*!
 * \brief Model Filter Data class
 * \ingroup Charts
 */
class CQChartsModelFilterData {
 public:
  enum class Type {
    EXPRESSION,
    REGEXP,
    WILDCARD,
    SIMPLE,
    SELECTED
  };

  using ColumnFilterMap = std::map<int,CQChartsRegExp>;

 public:
  CQChartsModelFilterData() { }

  const Type &type() const { return type_; }
  void setType(const Type &v) { type_ = v; }

  const QString &filter() const { return filter_; }
  void setFilter(const QString &filter) { filter_ = filter; }

  bool isExpr    () const { return type_ == Type::EXPRESSION; }
  bool isRegExp  () const { return type_ == Type::REGEXP;     }
  bool isWildcard() const { return type_ == Type::WILDCARD;   }
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

  const QString &filterExpr() const { return filterExpr_; }
  void setFilterExpr(const QString &filter) { filterExpr_ = filter; }

  QString details() const {
    if      (type_ == Type::EXPRESSION)
      return filter_;
    else if (type_ == Type::REGEXP)
      return filter_;
    else if (type_ == Type::SIMPLE)
      return filter_;
    else if (type_ == Type::SELECTED)
      return "selected";
    else
      return "";
  }

 private:
  Type            type_           { Type::EXPRESSION }; //!< type
  QString         filter_;                              //!< filter string
  bool            invert_         { false };            //!< invert filter
  CQChartsRegExp  regexp_;                              //!< cached regexp for REGEXP
  QModelIndexList filterRows_;                          //!< cached rows (for SELECTED)
  ColumnFilterMap columnFilterMap_;                     //!< column filters for SIMPLE
  QString         filterExpr_;                          //!< preprocessed filter for EXPRESSION
};

//------

class CQChartsExprModel;

/*!
 * \brief Model Filter Model
 * \ingroup Charts
 */
class CQChartsModelFilter : public QSortFilterProxyModel {
  Q_OBJECT

  Q_PROPERTY(QString filter       READ filter         WRITE setFilter      )
  Q_PROPERTY(Type    filterType   READ filterType     WRITE setFilterType  )
  Q_PROPERTY(bool    filterInvert READ isFilterInvert WRITE setFilterInvert)

  Q_ENUMS(Type)

  // model indices are from source model
 public:
  enum class Combine {
    AND,
    OR
  };

  using Type = CQChartsModelFilterData::Type;

 public:
  CQChartsModelFilter(CQCharts *charts);

 ~CQChartsModelFilter();

  CQChartsModelExprMatch *exprMatch() const { return expr_; }

  virtual CQChartsExprModel  *exprModel() const = 0;
  virtual QAbstractItemModel *baseModel() const = 0;

  QItemSelectionModel *selectionModel() const { return selectionModel_; }
  void setSelectionModel(QItemSelectionModel *sm) { selectionModel_ = sm; }

  const QString &filter() const { return currentFilterData().filter(); }
  void setFilter(const QString &filter) { currentFilterData().setFilter(filter); }

  const Type &filterType() const { return currentFilterData().type(); }
  void setFilterType(const Type &type) { currentFilterData().setType(type); }

  bool isFilterInvert() const { return currentFilterData().isInvert(); }
  void setFilterInvert(bool b) { currentFilterData().setInvert(b); }

  bool isExpr    () const { return currentFilterData().isExpr(); }
  bool isRegExp  () const { return currentFilterData().isRegExp(); }
  bool isWildcard() const { return currentFilterData().isWildcard(); }
  bool isSelected() const { return currentFilterData().isSelected(); }

  bool isInvert() const { return currentFilterData().isInvert(); }
  void setInvert(bool b) { currentFilterData().setInvert(b); }

  void resetFilterData();

  void pushFilterData();
  void popFilterData();

  void setExpressionFilter(const QString &filter);
  void setRegExpFilter    (const QString &filter);
  void setWildcardFilter  (const QString &filter);
  void setSimpleFilter    (const QString &filter);
  void setSelectionFilter (bool invert);

  bool isMapping() const { return mapping_; }
  void setMapping(bool b) { mapping_ = b; }

  //---

  const Combine &filterCombine() const { return filterCombine_; }
  void setFilterCombine(const Combine &c);

  //---

  QString filterDetails() const;

  //---

  bool filterAcceptsRow(int row, const QModelIndex &parent) const override;

  bool acceptsItem(const QModelIndex &ind) const;

  //---

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  //---

  void addExpand(const QModelIndex &ind) const { expand_.insert(ind); }

  //---

  void sort(int column, Qt::SortOrder order=Qt::AscendingOrder) override;

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

  QString replaceNamedColumns(QAbstractItemModel *model, const QString &expr) const;

 protected:
  using IndexMatches = std::map<QModelIndex,bool>;
  using ExpandInds   = std::set<QModelIndex>;

  using FilterDatas = std::vector<CQChartsModelFilterData>;

  CQCharts*               charts_         { nullptr };
  CQChartsModelExprMatch* expr_           { nullptr };
  QItemSelectionModel*    selectionModel_ { nullptr };
  FilterDatas             filterDatas_;
  Combine                 filterCombine_  { Combine::AND };
  mutable IndexMatches    matches_;
  mutable ExpandInds      expand_;
  bool                    mapping_        { true };
  mutable std::mutex      mutex_;
};

#endif
