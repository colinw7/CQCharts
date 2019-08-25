#ifndef CQChartsTree_H
#define CQChartsTree_H

#include <CQTreeView.h>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQCharts;
class CQChartsTableDelegate;
class CQChartsSelectionModel;
class CQChartsModelData;
class CQChartsModelDetails;

/*!
 * \brief Charts Tree View class
 * \ingroup Charts
 */
class CQChartsTree : public CQTreeView {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsTree(CQCharts *charts, QWidget *parent=nullptr);
 ~CQChartsTree();

  CQCharts *charts() const { return charts_; }

  ModelP modelP() const { return model_; }
  void setModelP(const ModelP &model);

  void setFilter(const QString &filter);

  CQChartsModelDetails *getDetails();

  QSize sizeHint() const override;

 private:
  void addMenuActions(QMenu *menu) override;

  CQChartsModelData *getModelData();

 signals:
  void columnClicked(int);

  void filterChanged();

  void selectionHasChanged();

 private slots:
  void modelTypeChangedSlot(int);

  void headerClickedSlot(int section);
  void itemClickedSlot(const QModelIndex &);

  void selectionSlot();

  void selectionBehaviorSlot(QAction *action);

  void exportSlot(QAction *action);

  void resetModelData();

 private:
  CQCharts*               charts_    { nullptr };
  ModelP                  model_;
  CQChartsSelectionModel* sm_        { nullptr };
  CQChartsTableDelegate*  delegate_  { nullptr };
  CQChartsModelData*      modelData_ { nullptr };
};

#endif
