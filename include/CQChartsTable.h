#ifndef CQChartsTable_H
#define CQChartsTable_H

#include <CQTableView.h>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQChartsTableSelectionModel;

class CQChartsTable : public CQTableView {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsTable(QWidget *parent=nullptr);

  ModelP model() const {return model_; }
  void setModel(const ModelP &model);

  void setFilter(const QString &filter);

  QSize sizeHint() const override;

 private:
  void addMenuActions(QMenu *menu) override;

 signals:
  void columnClicked(int);

 private slots:
  void selectionSlot();

  void headerClickSlot(int section);

  void selectionBehaviorSlot(QAction *action);

 private:
  ModelP                       model_;
  CQChartsTableSelectionModel* sm_ { nullptr };
};

#endif
