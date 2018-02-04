#ifndef CQChartsTree_H
#define CQChartsTree_H

#include <CQTreeView.h>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQCharts;
class CQChartsTreeSelectionModel;

class CQChartsTree : public CQTreeView {
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
  CQChartsTree(CQCharts *charts, QWidget *parent=nullptr);

  ModelP model() const {return model_; }
  void setModel(const ModelP &model);

  void setFilter(const QString &filter);

  void calcDetails(Details &details);

  QSize sizeHint() const override;

 private:
  void addMenuActions(QMenu *menu) override;

 private slots:
  void selectionSlot();

  void selectionBehaviorSlot(QAction *action);

 signals:
  void filterChanged();

 private:
  CQCharts*                   charts_ { nullptr };
  ModelP                      model_;
  CQChartsTreeSelectionModel* sm_     { nullptr };
};

#endif
