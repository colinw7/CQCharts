#ifndef CQChartsTable_H
#define CQChartsTable_H

#include <CQTableView.h>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQCharts;
class CQChartsTableSelectionModel;

class CQChartsTable : public CQTableView {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

  struct ColumnDetails {
    QString  typeName;
    QVariant minValue;
    QVariant maxValue;

    ColumnDetails() = default;

    ColumnDetails(const QString &typeName, const QVariant &minValue, const QVariant &maxValue) :
     typeName(typeName), minValue(minValue), maxValue(maxValue) {
    }
  };

  using DetailsColumns = std::vector<ColumnDetails>;

  struct Details {
    int            numColumns { 0 };
    int            numRows    { 0 };
    DetailsColumns columns;
  };

 public:
  CQChartsTable(CQCharts *charts);

  ModelP model() const {return model_; }
  void setModel(const ModelP &model);

  void setFilter(const QString &filter);

  void calcDetails(Details &details);

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
  CQCharts*                    charts_ { nullptr };
  ModelP                       model_;
  CQChartsTableSelectionModel* sm_     { nullptr };
};

#endif
