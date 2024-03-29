#ifndef CQChartsSelectionModel_H
#define CQChartsSelectionModel_H

#include <QItemSelectionModel>

class CQChartsModelData;
class QAbstractItemView;

#ifdef CQCHARTS_MODEL_VIEW
#include <CQModelView.h>

using CQChartsSelectionModelBase = CQModelViewSelectionModel;
#else
using CQChartsSelectionModelBase = QItemSelectionModel;
#endif

/*!
 * \brief Selection Model wrapper for Model
 * \ingroup Charts
 */
class CQChartsSelectionModel : public CQChartsSelectionModelBase {
 public:
  CQChartsSelectionModel(QAbstractItemView *view, CQChartsModelData *modelData);
  CQChartsSelectionModel(QAbstractItemView *view, QAbstractItemModel *model);

  void select(const QModelIndex &ind, SelectionFlags flags) override;

  void select(const QItemSelection &selection, SelectionFlags flags) override;

 private:
  SelectionFlags adjustFlags(SelectionFlags flags) const;

 private:
  QAbstractItemView* view_ { nullptr };
};

#endif
