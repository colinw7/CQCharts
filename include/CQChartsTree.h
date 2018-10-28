#ifndef CQChartsTree_H
#define CQChartsTree_H

#include <CQTreeView.h>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQCharts;
class CQChartsTreeSelectionModel;
class CQChartsModelDetails;

class CQChartsTree : public CQTreeView {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsTree(CQCharts *charts, QWidget *parent=nullptr);

  ModelP modelP() const { return model_; }
  void setModelP(const ModelP &model);

  void setFilter(const QString &filter);

  CQChartsModelDetails *getDetails();

  QSize sizeHint() const override;

 private:
  void addMenuActions(QMenu *menu) override;

 private slots:
  void headerClickedSlot(int section);
  void itemClickedSlot(const QModelIndex &);

  void selectionSlot();

  void selectionBehaviorSlot(QAction *action);

  void exportSlot(QAction *action);

 signals:
  void columnClicked(int);

  void filterChanged();

  void selectionChanged();

 private:
  CQCharts*                   charts_ { nullptr };
  ModelP                      model_;
  CQChartsTreeSelectionModel* sm_     { nullptr };
};

#endif
