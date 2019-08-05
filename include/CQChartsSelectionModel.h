#ifndef CQChartsSelectionModel_H
#define CQChartsSelectionModel_H

#include <QItemSelectionModel>

class CQChartsModelData;
class QAbstractItemView;

class CQChartsSelectionModel : public QItemSelectionModel {
 public:
  CQChartsSelectionModel(QAbstractItemView *view, CQChartsModelData *modelData);
  CQChartsSelectionModel(QAbstractItemView *view, QAbstractItemModel *model);

  void select(const QModelIndex &ind, SelectionFlags flags);

  void select(const QItemSelection &selection, SelectionFlags flags);

 private:
  SelectionFlags adjustFlags(SelectionFlags flags) const;

 private:
  QAbstractItemView* view_      { nullptr };
  CQChartsModelData* modelData_ { nullptr };
};

#endif
