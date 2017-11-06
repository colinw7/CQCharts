#ifndef CQSortModel_H
#define CQSortModel_H

#include <QSortFilterProxyModel>

class CQSortModel : public QSortFilterProxyModel {
  Q_OBJECT

  Q_PROPERTY(QString filter READ filter WRITE setFilter)

 public:
  CQSortModel(QAbstractItemModel *model);

  const QString &filter() const { return filter_; }
  void setFilter(const QString &filter);

 private:
  QString filter_;
};

#endif
