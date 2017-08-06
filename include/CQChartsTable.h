#ifndef CQChartsTable_H
#define CQChartsTable_H

#include <QTableView>

class CQHeaderView;

class CQChartsTable : public QTableView {
  Q_OBJECT

 public:
  CQChartsTable(QWidget *parent=nullptr);

  void setModel(QAbstractItemModel *model);

  QSize sizeHint() const;

 signals:
  void columnClicked(int);

 private slots:
  void headerClickSlot(int section);

 private:
  QAbstractItemModel* model_  { nullptr };
  CQHeaderView*       header_ { nullptr };
};

#endif
