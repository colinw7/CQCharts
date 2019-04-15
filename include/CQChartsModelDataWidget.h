#ifndef CQChartsModelDataWidget_H
#define CQChartsModelDataWidget_H

#include <QFrame>

class CQChartsModelDetailsWidget;
class CQChartsModelData;
class CQChartsTable;
class CQChartsTree;
class CQCharts;

class QStackedWidget;

/*!
 * \brief Model Data Widget
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

  void treeColumnClicked(int column);
  void treeSelectionChanged();

  void tableColumnClicked(int column);
  void tableSelectionChanged();

 private:
  CQCharts*                   charts_        { nullptr };
  CQChartsModelData*          modelData_     { nullptr };
  QStackedWidget*             stack_         { nullptr };
  CQChartsTree*               tree_          { nullptr };
  CQChartsTable*              table_         { nullptr };
  CQChartsModelDetailsWidget* detailsWidget_ { nullptr };
};

#endif
