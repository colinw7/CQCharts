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

  void setModel(const ModelP &model);

  void setFilter(const QString &filter);

 private slots:
  void selectionSlot();

 private:
  ModelP                      model_;
  CQChartsTreeSelectionModel* sm_ { nullptr };
};

#endif
