#ifndef CQChartsTable_H
#define CQChartsTable_H

#include <CQTableView.h>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQCharts;
class CQChartsTableSelectionModel;
class CQChartsModelExprMatch;

class CQChartsTable : public CQTableView {
  Q_OBJECT

  Q_PROPERTY(bool exprFilter READ isExprFilter WRITE setIsExprFilter)

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
  CQChartsTable(CQCharts *charts, QWidget *parent=nullptr);
 ~CQChartsTable();

  CQCharts *charts() const { return charts_; }

  ModelP model() const {return model_; }
  void setModel(const ModelP &model);

  bool isExprFilter() const { return isExprFilter_; }
  void setIsExprFilter(bool b) { isExprFilter_ = b; }

  void setFilter(const QString &filter);
  void addFilter(const QString &filter);

  void setSearch(const QString &text);
  void addSearch(const QString &text);

  void calcDetails(Details &details);

  QSize sizeHint() const override;

 private:
  void addReplaceFilter(const QString &filter, bool add);
  void addReplaceSearch(const QString &text, bool add);

  void addMenuActions(QMenu *menu) override;

 signals:
  void columnClicked(int);

  void filterChanged();

 private slots:
  void selectionSlot();

  void headerClickSlot(int section);

  void selectionBehaviorSlot(QAction *action);

  void exportSlot(QAction *action);

 private:
  using Matches = std::vector<QString>;

  CQCharts*                    charts_       { nullptr };
  ModelP                       model_;
  CQChartsTableSelectionModel* sm_           { nullptr };
  bool                         isExprFilter_ { true };
  CQChartsModelExprMatch*      match_        { nullptr };
  Matches                      matches_;
};

#endif
