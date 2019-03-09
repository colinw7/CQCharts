#ifndef CQChartsEditModelDlg_H
#define CQChartsEditModelDlg_H

#include <QDialog>

class CQCharts;
class CQChartsModelData;
class CQChartsModelDataWidget;
class CQChartsModelControl;
class CQChartsCreatePlotDlg;

class CQChartsEditModelDlg : public QDialog {
  Q_OBJECT

 public:
  CQChartsEditModelDlg(CQCharts *charts, CQChartsModelData *modelData=nullptr);
 ~CQChartsEditModelDlg();

  void setModelData(CQChartsModelData *modelData);

 private slots:
  void writeSlot();
  void plotSlot();
  void cancelSlot();

 private:
  CQCharts*                charts_        { nullptr };
  CQChartsModelData*       modelData_     { nullptr };
  CQChartsModelDataWidget* modelWidget_   { nullptr };
  CQChartsModelControl*    modelControl_  { nullptr };
  CQChartsCreatePlotDlg*   createPlotDlg_ { nullptr };
};

#endif
