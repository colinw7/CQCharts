#ifndef CQChartsTree_H
#define CQChartsTree_H

#include <CQTreeView.h>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQChartsTreeSelectionModel;

class CQChartsTree : public CQTreeView {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsTree(QWidget *parent=nullptr);

  ModelP model() const {return model_; }
  void setModel(const ModelP &model);

  void setFilter(const QString &filter);

  QSize sizeHint() const override;

 private:
  void addMenuActions(QMenu *menu) override;

 private slots:
  void selectionSlot();

  void selectionBehaviorSlot(QAction *action);

 private:
  ModelP                      model_;
  CQChartsTreeSelectionModel* sm_ { nullptr };
};

#endif
