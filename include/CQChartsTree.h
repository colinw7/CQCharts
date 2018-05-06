#ifndef CQChartsTree_H
#define CQChartsTree_H

#include <CQChartsModelDetails.h>
#include <CQTreeView.h>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQCharts;
class CQChartsTreeSelectionModel;

class CQChartsTree : public CQTreeView {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsTree(CQCharts *charts, QWidget *parent=nullptr);

  ModelP model() const {return model_; }
  void setModel(const ModelP &model);

  void setFilter(const QString &filter);

  void calcDetails(CQChartsModelDetails &details);

  QSize sizeHint() const override;

 private:
  void addMenuActions(QMenu *menu) override;

 private slots:
  void selectionSlot();

  void selectionBehaviorSlot(QAction *action);

 signals:
  void filterChanged();

 private:
  CQCharts*                   charts_ { nullptr };
  ModelP                      model_;
  CQChartsTreeSelectionModel* sm_     { nullptr };
};

#endif
