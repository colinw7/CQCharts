#include <CQPropertyViewFilter.h>
#include <CQPropertyView.h>
#include <CQPropertyModel.h>
#include <CQPropertyItem.h>
#include <cassert>

CQPropertyViewFilter::
CQPropertyViewFilter(CQPropertyView *view) :
 view_(view) {
}

bool
CQPropertyViewFilter::
filterAcceptsRow(int row, const QModelIndex &parent) const
{
  if (filter_.length() == 0)
    return true;

  //CQPropertyItem *parentItem = view_->getModelItem(parent, /*map*/false);
  //if (! parentItem) return true;

  QAbstractItemModel *model = view_->propertyModel();

  QModelIndex child = model->index(row, 0, parent);

  CQPropertyItem *item = view_->getModelItem(child, /*map*/false);
  if (! item) return false;

  if (! item->object()) {
    if (! anyChildMatch(child))
      return false;

    expand_.insert(child);

    //view_->setExpanded(child, true);

    return true;
  }
  else {
    return acceptsItem(child);
  }
}

bool
CQPropertyViewFilter::
acceptsItem(const QModelIndex &ind) const
{
  auto p = matches_.find(ind);

  if (p != matches_.end())
    return (*p).second;

  CQPropertyItem *item = view_->getModelItem(ind, /*map*/false);
  if (! item) return false;

  bool match = true;

  if (item->object()) {
    QString str = item->name();

    match = regexp_.exactMatch(str);
  }

  matches_[ind] = match;

  return match;
}

bool
CQPropertyViewFilter::
anyChildMatch(const QModelIndex &parent) const
{
  QAbstractItemModel *model = view_->propertyModel();

  for (int i = 0; i < model->rowCount(parent); ++i) {
    QModelIndex child = model->index(i, 0, parent);

    CQPropertyItem *item = view_->getModelItem(child, /*map*/false);
    assert(item);

    if (! item->object()) {
      if (anyChildMatch(child))
        return true;
    }
    else {
      if (acceptsItem(child))
        return true;
    }
  }

  return false;
}

#if 0
void
CQPropertyViewFilter::
expandMatches()
{
  CQPropertyViewFilter *filterModel = view_->filterModel();

  std::vector<QModelIndex> inds;

  for (const auto &ind : expand_) {
    QModelIndex ind1 = filterModel->mapFromSource(ind);

    inds.push_back(ind1);
  }

  for (int i = inds.size() - 1; i >= 0; --i) {
    view_->setExpanded(inds[i], true);
  }
}
#endif

void
CQPropertyViewFilter::
setFilter(const QString &filter)
{
  matches_.clear();
  expand_ .clear();

  filter_ = filter;
  regexp_ = QRegExp(filter_, Qt::CaseSensitive, QRegExp::Wildcard);

  invalidateFilter();
  //invalidate();

  view_->expandAll();

  //expandMatches();
}
