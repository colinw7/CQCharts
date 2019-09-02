#ifndef CQChartsModelViewHolder_H
#define CQChartsModelViewHolder_H

#include <QFrame>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQCharts;

#ifdef CQCHARTS_MODEL_VIEW
class CQChartsModelView;
#else
class CQChartsTable;
class CQChartsTree;
#endif

class QStackedWidget;
class QItemSelectionModel;

/*!
 * \brief Model View Widget
 * \ingroup Charts
 */
class CQChartsModelViewHolder : public QFrame {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsModelViewHolder(CQCharts *charts, QWidget *parent=nullptr);
 ~CQChartsModelViewHolder();

  void setFilterAnd(bool b);

  void setFilter(const QString &text);
  void addFilter(const QString &text);

  QString filterDetails() const;

  void setSearch(const QString &text);
  void addSearch(const QString &text);

  void setModel(ModelP model, bool hierarchical);

  QItemSelectionModel *selectionModel();

 signals:
  void filterChanged();

 private:
  CQCharts*          charts_       { nullptr };
#ifdef CQCHARTS_MODEL_VIEW
  CQChartsModelView* view_        { nullptr };
#else
  QStackedWidget*    stack_        { nullptr };
  CQChartsTable*     table_        { nullptr };
  CQChartsTree*      tree_         { nullptr };
#endif
  bool               hierarchical_ { false };
};

#endif
