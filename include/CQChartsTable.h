#ifndef CQChartsTable_H
#define CQChartsTable_H

#include <CQTableView.h>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQCharts;
class CQChartsTableDelegate;
class CQChartsTableSelectionModel;
class CQChartsModelExprMatch;
class CQChartsModelDetails;

class CQChartsTable : public CQTableView {
  Q_OBJECT

  Q_PROPERTY(bool exprFilter READ isExprFilter WRITE setIsExprFilter)

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsTable(CQCharts *charts, QWidget *parent=nullptr);
 ~CQChartsTable();

  CQCharts *charts() const { return charts_; }

  ModelP modelP() const { return model_; }
  void setModelP(const ModelP &model);

  bool isExprFilter() const { return isExprFilter_; }
  void setIsExprFilter(bool b) { isExprFilter_ = b; }

  void setFilterAnd(bool b);

  void setFilter(const QString &filter);
  void addFilter(const QString &filter);

  QString filterDetails() const;

  void setSearch(const QString &text);
  void addSearch(const QString &text);

  CQChartsModelDetails *getDetails();

  void scrollTo(const QModelIndex &index, ScrollHint hint=EnsureVisible);

  QSize sizeHint() const override;

 private:
  void addReplaceFilter(const QString &filter, bool add);
  void addReplaceSearch(const QString &text, bool add);

  void addMenuActions(QMenu *menu) override;

 signals:
  void columnClicked(int);

  void filterChanged();

 private slots:
  void modelTypeChangedSlot(int);

  void selectionSlot();

  void headerClickedSlot(int section);
  void itemClickedSlot(const QModelIndex &);

  void selectionBehaviorSlot(QAction *action);

  void exportSlot(QAction *action);

 private:
  using Matches = std::vector<QString>;

  CQCharts*                    charts_       { nullptr };
  ModelP                       model_;
  CQChartsTableSelectionModel* sm_           { nullptr };
  CQChartsTableDelegate*       delegate_     { nullptr };
  bool                         isExprFilter_ { true };
  CQChartsModelExprMatch*      match_        { nullptr };
  Matches                      matches_;
};

#endif
