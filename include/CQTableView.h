#ifndef CQTableView_H
#define CQTableView_H

#include <QTableView>

class CQHeaderView;

class CQTableView : public QTableView {
  Q_OBJECT

 public:
  CQTableView(QWidget *parent=nullptr);

  virtual ~CQTableView();

  virtual void addMenuActions(QMenu *menu);

 private slots:
  void customContextMenuSlot(const QPoint &pos);

  void showVertical();

 protected:
  CQHeaderView *header_ { nullptr };
};

#endif
