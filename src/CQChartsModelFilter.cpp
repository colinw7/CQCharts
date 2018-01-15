#include <CQChartsModelFilter.h>
#include <CQChartsModelExprMatch.h>
#include <CQChartsUtil.h>
#include <QItemSelectionModel>
#include <cassert>

CQChartsModelFilter::
CQChartsModelFilter()
{
  expr_ = new CQChartsModelExprMatch;

  resetFilterData();
}

CQChartsModelFilter::
~CQChartsModelFilter()
{
  delete expr_;
}

void
CQChartsModelFilter::
resetFilterData()
{
  filterDatas_.clear();

  pushFilterData();
}

void
CQChartsModelFilter::
pushFilterData()
{
  if (filterDatas_.empty() || ! currentFilterData().isEmpty())
    filterDatas_.push_back(CQChartsModelFilterData());
}

void
CQChartsModelFilter::
popFilterData()
{
  if (! filterDatas_.empty())
    filterDatas_.pop_back();

  if (filterDatas_.empty())
    pushFilterData();
}

void
CQChartsModelFilter::
setExpressionFilter(const QString &filter)
{
  CQChartsModelFilterData &filterData = currentFilterData();

  filterData.setType  (CQChartsModelFilterData::Type::EXPRESSION);
  filterData.setFilter(filter);
  filterData.setInvert(false);

  initFilterData(filterData);

  initFilter();
}

void
CQChartsModelFilter::
setRegExpFilter(const QString &filter)
{
  CQChartsModelFilterData &filterData = currentFilterData();

  filterData.setType  (CQChartsModelFilterData::Type::REGEXP);
  filterData.setFilter(filter);
  filterData.setInvert(false);

  initFilterData(filterData);

  initFilter();
}

void
CQChartsModelFilter::
setSimpleFilter(const QString &filter)
{
  CQChartsModelFilterData &filterData = currentFilterData();

  filterData.setType  (CQChartsModelFilterData::Type::SIMPLE);
  filterData.setFilter(filter);
  filterData.setInvert(false);

  initFilterData(filterData);

  initFilter();
}

void
CQChartsModelFilter::
setSelectionFilter(bool invert)
{
  CQChartsModelFilterData &filterData = currentFilterData();

  filterData.setType  (CQChartsModelFilterData::Type::SELECTED);
  filterData.setFilter("");
  filterData.setInvert(invert);

  initFilterData(filterData);

  initFilter();
}

bool
CQChartsModelFilter::
allFiltersEmpty() const
{
  for (const auto &filterData : filterDatas_)
    if (! filterData.isEmpty())
      return false;

  return true;
}

bool
CQChartsModelFilter::
filterAcceptsRow(int row, const QModelIndex &parent) const
{
  if (allFiltersEmpty())
    return true;

  QAbstractItemModel *model = sourceModel();
  assert(model);

  int column = filterKeyColumn();

  QModelIndex ind = model->index(row, 0, parent);

  if (model->hasChildren(ind)) {
    QModelIndex ind1 = model->index(row, column, parent);

    if (! anyChildMatch(ind1))
      return false;

    expand_.insert(ind1);

    return true;
  }
  else {
    QModelIndex ind1 = model->index(row, column, parent);

    return acceptsItem(ind1);
  }
}

bool
CQChartsModelFilter::
acceptsItem(const QModelIndex &ind) const
{
  auto p = matches_.find(ind);

  if (p != matches_.end())
    return (*p).second;

  bool match = itemMatch(ind);

  matches_[ind] = match;

  return match;
}

bool
CQChartsModelFilter::
itemMatch(const QModelIndex &ind) const
{
  for (const auto &filterData : filterDatas_) {
    if (! filterItemMatch(filterData, ind))
      return false;
  }

  return true;
}

bool
CQChartsModelFilter::
filterItemMatch(const CQChartsModelFilterData &filterData, const QModelIndex &ind) const
{
  if      (filterData.isSelected()) {
    assert(selectionModel_);

    if (filterData.filterRows().empty())
      return ! filterData.isInvert();

    QAbstractItemModel *model = sourceModel();
    assert(model);

    QModelIndex ind1 = model->index(ind.row(), 0, ind.parent());

    bool rc = filterData.filterRows().contains(ind1);

    return (filterData.isInvert() ? ! rc  : rc);
  }
  else if (filterData.isRegExp()) {
    QAbstractItemModel *model = sourceModel();
    assert(model);

    QString str = model->data(ind).toString();

    return filterData.regexp().match(str);
  }
  else if (filterData.isSimple()) {
    QAbstractItemModel *model = sourceModel();
    assert(model);

    for (const auto &columnFilter : filterData.columnFilterMap()) {
      QModelIndex ind1 = model->index(ind.row(), columnFilter.first, ind.parent());

      QString str = model->data(ind1).toString();

      if (! columnFilter.second.match(str))
        return false;
    }

    return true;
  }
  else {
    bool ok;

    return expr_->match(filterData.filter(), ind.row(), ind.column(), ok);
  }
}

bool
CQChartsModelFilter::
anyChildMatch(const QModelIndex &parent) const
{
  QAbstractItemModel *model = sourceModel();
  assert(model);

  int column = filterKeyColumn();

  int nr = model->rowCount(parent);

  for (int row = 0; row < nr; ++row) {
    QModelIndex child = model->index(row, 0, parent);

    if (model->hasChildren(child)) {
      QModelIndex child1 = model->index(row, column, parent);

      if (! anyChildMatch(child1))
        return true;
    }
    else {
      QModelIndex child1 = model->index(row, column, parent);

      if (acceptsItem(child1))
        return true;
    }
  }

  return false;
}

#if 0
void
CQChartsModelFilter::
expandMatches()
{
  std::vector<QModelIndex> inds;

  for (const auto &ind : expand_) {
    QModelIndex ind1 = filterModel->mapFromSource(ind);

    inds.push_back(ind1);
  }

  for (int i = inds.size() - 1; i >= 0; --i) {
    setExpanded(inds[i], true);
  }
}
#endif

void
CQChartsModelFilter::
initFilterData(CQChartsModelFilterData &filterData)
{
  if      (filterData.isSelected()) {
    assert(selectionModel_);

    if (selectionModel_->hasSelection()) {
      QModelIndexList selectedRows = selectionModel_->selectedRows();

      QModelIndexList selectedRows1;

      for (int i = 0; i < selectedRows.size(); ++i) {
        QModelIndex ind1 = mapToSource(selectedRows[i]);

        selectedRows1.push_back(ind1);
      }

      filterData.setFilterRows(selectedRows1);
    }
    else {
      filterData.setFilterRows(QModelIndexList());
    }
  }
  else if (filterData.isRegExp()) {
    QAbstractItemModel *model = this->sourceModel();
    assert(model);

    QString filter;
    int     column = -1;

    if (CQChartsUtil::decodeModelFilterStr(model, filterData.filter(), filter, column))
      setFilterKeyColumn(column);

    filterData.setRegExp(CQChartsRegExp(filter));
  }
  else if (filterData.isSimple()) {
    QAbstractItemModel *model = this->sourceModel();
    assert(model);

    QStringList strs = filterData.filter().split(",");

    CQChartsModelFilterData::ColumnFilterMap columnFilterMap;

    for (int i = 0; i < strs.size(); ++i) {
      QString filter;
      int     column = -1;

      if (! CQChartsUtil::decodeModelFilterStr(model, strs[i], filter, column))
        continue;

      columnFilterMap[column] = CQChartsRegExp(filter);
    }

    filterData.setColumnFilterMap(columnFilterMap);
  }
  else {
  }
}

void
CQChartsModelFilter::
initFilter()
{
  matches_.clear();
  expand_ .clear();

  //---

  if (selectionModel_)
    selectionModel_->clear();

  QAbstractItemModel *model = this->sourceModel();
  assert(model);

  expr_->setModel(model);

  expr_->initColumns();

  //---

  invalidateFilter();
  //invalidate();

  //expandMatches();
}
