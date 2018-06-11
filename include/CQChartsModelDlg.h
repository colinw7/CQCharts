#ifndef CQChartsModelDlg_H
#define CQChartsModelDlg_H

#include <QDialog>

class CQCharts;
class CQChartsModelList;
class CQChartsModelControl;

class CQChartsModelDlg : public QDialog {
  Q_OBJECT

 public:
  CQChartsModelDlg(CQCharts *charts);
 ~CQChartsModelDlg();

 private slots:
  void cancelSlot();

 private:
  CQCharts*             charts_       { nullptr };
  CQChartsModelList*    modelList_    { nullptr };
  CQChartsModelControl* modelControl_ { nullptr };
};

#endif
