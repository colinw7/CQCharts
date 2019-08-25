#ifndef CQChartsTable_H
#define CQChartsTable_H

#include <CQTableView.h>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQCharts;
class CQChartsTableDelegate;
class CQChartsSelectionModel;
class CQChartsModelExprMatch;
class CQChartsModelData;
class CQChartsModelDetails;

/*!
 * \brief Charts Table View class
 * \ingroup Charts
 */
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

  void scrollTo(const QModelIndex &index, ScrollHint hint=EnsureVisible) override;

  QSize sizeHint() const override;

 private:
  void addReplaceFilter(const QString &filter, bool add);
  void addReplaceSearch(const QString &text, bool add);

  void addMenuActions(QMenu *menu) override;

  CQChartsModelData *getModelData();

 signals:
  void columnClicked(int);

  void filterChanged();

  void selectionHasChanged();

 private slots:
  void modelTypeChangedSlot(int);

  void headerClickedSlot(int section);
  void itemClickedSlot(const QModelIndex &);

  void selectionSlot();

  void selectionBehaviorSlot(QAction *action);

  void exportSlot(QAction *action);

  void editSlot();

  void resetModelData();

 private:
  using Matches = std::vector<QString>;

  CQCharts*               charts_       { nullptr };
  ModelP                  model_;
  CQChartsSelectionModel* sm_           { nullptr };
  CQChartsTableDelegate*  delegate_     { nullptr };
  CQChartsModelData*      modelData_    { nullptr };
  bool                    isExprFilter_ { true };
  CQChartsModelExprMatch* match_        { nullptr };
  Matches                 matches_;
};

#endif
