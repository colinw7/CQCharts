#ifndef CQChartsModelDlg_H
#define CQChartsModelDlg_H

#include <QDialog>

class CQCharts;
class CQChartsModelWidgets;

class CQChartsModelDlg : public QDialog {
  Q_OBJECT

 public:
  CQChartsModelDlg(CQCharts *charts);
 ~CQChartsModelDlg();

 private slots:
  void cancelSlot();

 private:
  CQCharts*             charts_       { nullptr };
  CQChartsModelWidgets* modelWidgets_ { nullptr };
};

#endif
