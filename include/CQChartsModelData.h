#ifndef CQChartsModelData_H
#define CQChartsModelData_H

#include <CQChartsModelDetails.h>
#include <QObject>
#include <QSharedPointer>

class QAbstractItemModel;
class QItemSelectionModel;

class CQChartsModelData : public QObject {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

#ifdef CQCHARTS_FOLDED_MODEL
  using FoldedModels = std::vector<ModelP>;
#endif

 public:
  CQChartsModelData(CQCharts *charts, ModelP &model);

 ~CQChartsModelData();

  CQCharts *charts() const { return charts_; }

  // get model
  ModelP &model() { return model_; }
  const ModelP &model() const { return model_; }

  // get (unique) index
  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  QString id() const;

  // get/set selection model
  QItemSelectionModel *selectionModel() const { return selectionModel_; }
  void setSelectionModel(QItemSelectionModel *p) { selectionModel_ = p; }

  // get/set name
  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

#ifdef CQCHARTS_FOLDED_MODEL
  // get associated fold models
  ModelP foldProxyModel() const { return foldProxyModel_; }
  void setFoldProxyModel(ModelP &model) { foldProxyModel_ = model; }
  void resetFoldProxyModel() { foldProxyModel_ = ModelP(); }

  const FoldedModels &foldedModels() const { return foldedModels_; }

  void addFoldedModel(ModelP &model) { foldedModels_.push_back(model); }
  void clearFoldedModels() { foldedModels_.clear(); }

  // fold model
  void foldModel(const QString &str);
  void foldClear();
#endif

  // get details
  CQChartsModelDetails *details();
  const CQChartsModelDetails *details() const;

  ModelP currentModel() const;

 private:
  void connectModel();
  void disconnectModel();

 private slots:
  void modelDataChangedSlot(const QModelIndex &, const QModelIndex &);

  void modelLayoutChangedSlot();
  void modelResetSlot();

  void modelRowsInsertedSlot();
  void modelRowsRemovedSlot();
  void modelColumnsInsertedSlot();
  void modelColumnsRemovedSlot();

 signals:
  void modelChanged();

 private:
  CQCharts*             charts_         { nullptr };
  ModelP                model_;
  int                   ind_            { -1 };
  QItemSelectionModel*  selectionModel_ { nullptr };
  QString               name_;
  CQChartsModelDetails* details_        { nullptr };
#ifdef CQCHARTS_FOLDED_MODEL
  ModelP                foldProxyModel_;
  FoldedModels          foldedModels_;
#endif
};

#endif
