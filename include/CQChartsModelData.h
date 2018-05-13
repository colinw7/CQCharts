#ifndef CQChartsModelData_H
#define CQChartsModelData_H

#include <CQChartsModelDetails.h>
#include <QSharedPointer>

class QAbstractItemModel;
class QItemSelectionModel;

class CQChartsModelData {
 public:
  using ModelP       = QSharedPointer<QAbstractItemModel>;
  using FoldedModels = std::vector<ModelP>;

 public:
  CQChartsModelData(CQCharts *charts, ModelP &model) :
   charts_(charts), model_(model), details_(charts, model_.data()) {
  }

  CQCharts *charts() const { return charts_; }

  ModelP &model() { return model_; }
  const ModelP &model() const { return model_; }

  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  QItemSelectionModel *selectionModel() const { return selectionModel_; }
  void setSelectionModel(QItemSelectionModel *p) { selectionModel_ = p; }

  bool isHierarchical() const { return hierarchical_; }
  void setHierarchical(bool b) { hierarchical_ = b; }

  ModelP foldProxyModel() const { return foldProxyModel_; }
  void setFoldProxyModel(ModelP &model) { foldProxyModel_ = model; }
  void resetFoldProxyModel() { foldProxyModel_ = ModelP(); }

  const FoldedModels &foldedModels() const { return foldedModels_; }

  void addFoldedModel(ModelP &model) { foldedModels_.push_back(model); }
  void clearFoldedModels() { foldedModels_.clear(); }

  CQChartsModelDetails &details() { return details_; }
  const CQChartsModelDetails &details() const { return details_; }
  void setDetails(const CQChartsModelDetails &v) { details_ = v; }

  ModelP currentModel() const {
    if (! foldedModels_.empty())
      return foldProxyModel_;
    else
      return model_;
  }

 private:
  CQCharts*            charts_         { nullptr };
  ModelP               model_;
  int                  ind_            { -1 };
  QItemSelectionModel* selectionModel_ { nullptr };
  bool                 hierarchical_   { false };
  ModelP               foldProxyModel_;
  FoldedModels         foldedModels_;
  CQChartsModelDetails details_;
};

#endif
