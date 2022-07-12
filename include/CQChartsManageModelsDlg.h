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

  void init();

  QSize sizeHint() const override;

 private Q_SLOTS:
  void loadSlot();
  void writeSlot();
  void plotSlot();
  void cancelSlot();

 private:
  CQCharts*             charts_       { nullptr }; //!< charts
  CQChartsModelWidgets* modelWidgets_ { nullptr }; //!< model list widget
};

#endif
