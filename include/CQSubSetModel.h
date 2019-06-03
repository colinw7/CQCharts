#ifndef CQSubSetModel_H
#define CQSubSetModel_H

#include <QSortFilterProxyModel>

class CQSubSetModel : public QSortFilterProxyModel {
  Q_OBJECT

 public:
  CQSubSetModel(QAbstractItemModel *model, const QModelIndex &tl=QModelIndex(),
                const QModelIndex &br=QModelIndex());

 ~CQSubSetModel();

  //---

  // get/set source model
  QAbstractItemModel *sourceModel() const;
  void setSourceModel(QAbstractItemModel *sourceModel) override;

  // get source model as sort filter proxy model
  QSortFilterProxyModel *sourceFilterModel() const;

  //---

  void setBounds(const QModelIndex &tl, const QModelIndex &br);

  void setTopLeft    (const QModelIndex &tl);
  void setBottomRight(const QModelIndex &br);

  bool filterAcceptsColumn(int column, const QModelIndex &parent) const override;
  bool filterAcceptsRow   (int row   , const QModelIndex &parent) const override;

 private:
  QPersistentModelIndex tl_;
  QPersistentModelIndex br_;
};

#endif
