#ifndef CQChartsModelViewHolder_H
#define CQChartsModelViewHolder_H

#include <CQChartsWidgetIFace.h>

#include <QFrame>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQCharts;
class CQChartsModelData;

#ifdef CQCHARTS_MODEL_VIEW
class CQChartsModelView;
#else
class CQChartsTable;
class CQChartsTree;
#endif

class QStackedWidget;
class QItemSelectionModel;

/*!
 * \brief Model View Holder Widget (Tree or Table)
 * \ingroup Charts
 */
class CQChartsModelViewHolder : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

  Q_PROPERTY(int modelInd READ modelInd WRITE setModelInd)

 public:
  using ModelData = CQChartsModelData;
  using ModelP    = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsModelViewHolder(CQCharts *charts=nullptr, QWidget *parent=nullptr);
 ~CQChartsModelViewHolder();

  //! get/set charts
  CQCharts *charts() const { return charts_; }
  void setCharts(CQCharts *charts) override;

#ifdef CQCHARTS_MODEL_VIEW
  CQChartsModelView *view() const { return modelView_; }
#else
  CQChartsTable *table() const { return table_; }
  CQChartsTree  *tree () const { return tree_ ; }
#endif

  void setFilterAnd(bool b);

  void setFilter(const QString &text);
  void addFilter(const QString &text);

  QString filterDetails() const;

  void setSearch(const QString &text);
  void addSearch(const QString &text);

  //! get/set model ind
  int modelInd() const;
  void setModelInd(int i);

  //! set model data
  void setModelData(ModelData *modelData) override;

  ModelP model() const;
  void setModel(ModelP model, bool hierarchical);

  QItemSelectionModel *selectionModel();

  void showColumn(int column);
  void hideColumn(int column);

 Q_SIGNALS:
  void filterChanged();
  void columnClicked(int);

 private Q_SLOTS:
  void columnClickedSlot(int);

 private:
  void init();

 private:
  CQCharts*          charts_       { nullptr };
#ifdef CQCHARTS_MODEL_VIEW
  CQChartsModelView* modelView_    { nullptr };
#else
  QStackedWidget*    stack_        { nullptr };
  CQChartsTable*     table_        { nullptr };
  CQChartsTree*      tree_         { nullptr };
#endif
  bool               hierarchical_ { false };
};

#endif
