#ifndef CQChartsEditModelDlg_H
#define CQChartsEditModelDlg_H

#include <QDialog>

class CQCharts;
class CQChartsModelData;
class CQChartsModelDataWidget;
class CQChartsModelControl;

/*!
 * \brief edit model dialog
 * \ingroup Charts
 */
class CQChartsEditModelDlg : public QDialog {
  Q_OBJECT

 public:
  CQChartsEditModelDlg(CQCharts *charts, CQChartsModelData *modelData=nullptr);
 ~CQChartsEditModelDlg();

  CQChartsModelData *modelData() const { return modelData_; }
  void setModelData(CQChartsModelData *modelData);

  QSize sizeHint() const override;

 private slots:
  void writeSlot();
  void plotSlot();
  void cancelSlot();

 private:
  bool writeCSVModel(const QString &fileName);

//void writeModelCmds();

 private:
  CQCharts*                charts_       { nullptr };
  CQChartsModelData*       modelData_    { nullptr };
  CQChartsModelDataWidget* modelWidget_  { nullptr };
  CQChartsModelControl*    modelControl_ { nullptr };
};

#endif
