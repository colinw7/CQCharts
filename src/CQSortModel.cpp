#include <CQSortModel.h>

CQSortModel::
CQSortModel(QAbstractItemModel *model) :
 QSortFilterProxyModel()
{
  setSourceModel(model);
}

void
CQSortModel::
setFilter(const QString &filter)
{
  filter_ = filter;

  QAbstractItemModel *model = sourceModel();

  QStringList strs = filter.split(':', QString::KeepEmptyParts);

  if (strs.size() == 2) {
    QString name   = strs[0];
    QString filter = strs[1];

    int column = -1;

    for (int i = 0; i < model->columnCount(); ++i) {
      QString name1 = model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();

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
