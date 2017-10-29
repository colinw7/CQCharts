#ifndef CQChartsTable_H
#define CQChartsTable_H

#include <QTableView>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQHeaderView;

class CQChartsTable : public QTableView {
  Q_OBJECT

 public:
  typedef QSharedPointer<QAbstractItemModel> ModelP;

 public:
  CQChartsTable(QWidget *parent=nullptr);

  void setModel(const ModelP &model);

  void setFilter(const QString &filter);

  QSize sizeHint() const override;

 signals:
  void columnClicked(int);

 private slots:
  void headerClickSlot(int section);

 private:
  ModelP        model_;
  CQHeaderView* header_ { nullptr };
};

#endif
