#include <CQSortModel.h>

CQSortModel::
CQSortModel(QAbstractItemModel *model) :
 QSortFilterProxyModel()
{
  setSortRole(Qt::EditRole);

  setSourceModel(model);
}

void
CQSortModel::
setFilter(const QString &filter)
{
  filter_ = filter;

  auto *model = sourceModel();

  auto strs = filter.split(':', QString::KeepEmptyParts);

  if (strs.size() == 2) {
    auto name   = strs[0];
    auto filter = strs[1];

    int column = -1;

    for (int i = 0; i < model->columnCount(); ++i) {
      auto name1 = model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();

      if (name == name1) {
        column = i;
        break;
      }
    }

    if (column > 0)
      setFilterKeyColumn(column);

    filter = strs[1];

    setFilterWildcard(filter);
  }
  else {
    setFilterWildcard(filter_);
  }
}
