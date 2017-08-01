#ifndef CQChartsTree_H
#define CQChartsTree_H

#include <QTreeView>

class CQHeaderView;

class CQChartsTree : public QTreeView {
  Q_OBJECT

 public:
  CQChartsTree(QWidget *parent=nullptr);

  void setModel(QAbstractItemModel *model);

 private:
  QAbstractItemModel* model_  { nullptr };
  CQHeaderView*       header_ { nullptr };
};

#endif
