#ifndef CQChartsModelDataWidget_H
#define CQChartsModelDataWidget_H

#include <QFrame>

class CQChartsModelDetailsWidget;
class CQChartsModelData;
#ifdef CQCHARTS_MODEL_VIEW
class CQChartsModelView;
#else
class CQChartsTable;
class CQChartsTree;
#endif
class CQCharts;

class QStackedWidget;

/*!
 * \brief Model Data Widget
 * \ingroup Charts
 */
class CQChartsModelDataWidget : public QFrame {
  Q_OBJECT

 public:
  CQChartsModelDataWidget(CQCharts *charts, CQChartsModelData *modelData);

 ~CQChartsModelDataWidget();

  CQChartsModelData *modelData() const { return modelData_; }
  void setModelData(CQChartsModelData *modelData);

  void reloadModel();

  void setDetails();

 private slots:
  void filterSlot();

  void columnClicked(int column);
  void selectionChanged();

 private:
  CQCharts*                   charts_        { nullptr };
  CQChartsModelData*          modelData_     { nullptr };
#ifdef CQCHARTS_MODEL_VIEW
  CQChartsModelView*          view_          { nullptr };
#else
  QStackedWidget*             stack_         { nullptr };
  CQChartsTree*               tree_          { nullptr };
  CQChartsTable*              table_         { nullptr };
#endif
  CQChartsModelDetailsWidget* detailsWidget_ { nullptr };
};

#endif
