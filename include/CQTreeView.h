#ifndef CQTreeView_H
#define CQTreeView_H

#include <QTreeView>

class CQHeaderView;

class CQTreeView : public QTreeView {
  Q_OBJECT

 public:
  CQTreeView(QWidget *parent=nullptr);
 ~CQTreeView();

 private slots:
  void customContextMenuSlot(const QPoint &pos);

  void expandAll  (const QModelIndex &ind=QModelIndex());
  void collapseAll(const QModelIndex &ind=QModelIndex());

 private:
  CQHeaderView *header_ { nullptr };
};

#endif
