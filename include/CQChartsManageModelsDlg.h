#ifndef CQChartsManageModelsDlg_H
#define CQChartsManageModelsDlg_H

#include <QDialog>

class CQCharts;
class CQChartsModelWidgets;

/*!
 * \brief dialog to manage charts models
 * \ingroup Charts
 */
class CQChartsManageModelsDlg : public QDialog {
  Q_OBJECT

 public:
  CQChartsManageModelsDlg(CQCharts *charts);
 ~CQChartsManageModelsDlg();

 private slots:
  void writeSlot();
  void plotSlot();
  void cancelSlot();

 private:
  CQCharts*             charts_       { nullptr };
  CQChartsModelWidgets* modelWidgets_ { nullptr };
};

#endif
