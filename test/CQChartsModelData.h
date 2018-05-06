#ifndef CQCHartsModelData_H
#define CQCHartsModelData_H

#include <QSharedPointer>

class QAbstractItemModel;
class QItemSelectionModel;

struct CQChartsModelData {
  using ModelP       = QSharedPointer<QAbstractItemModel>;
  using FoldedModels = std::vector<ModelP>;

  int                  ind          { -1 };
  ModelP               model;
  QItemSelectionModel* sm           { nullptr };
  bool                 hierarchical { false };
  ModelP               foldProxyModel;
  FoldedModels         foldedModels;

  ModelP currentModel() const {
    if (! foldedModels.empty())
      return foldProxyModel;
    else
      return model;
  }
};

#endif
