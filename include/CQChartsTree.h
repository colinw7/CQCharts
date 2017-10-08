#ifndef CQChartsTree_H
#define CQChartsTree_H

#include <QTreeView>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQHeaderView;

class CQChartsTree : public QTreeView {
  Q_OBJECT

 public:
  typedef QSharedPointer<QAbstractItemModel> ModelP;

 public:
  CQChartsTree(QWidget *parent=nullptr);

  void setModel(const ModelP &model);

  void setFilter(const QString &filter);

 private:
  ModelP        model_;
  CQHeaderView* header_ { nullptr };
};

#endif
