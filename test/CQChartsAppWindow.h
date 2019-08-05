#ifndef CQChartsAppWindow_H
#define CQChartsAppWindow_H

#include <CQAppWindow.h>
#include <QAbstractItemModel>
#include <QSharedPointer>

class CQCharts;
class CQChartsModelWidgets;
class CQChartsLoadModelDlg;
class QVBoxLayout;

/*!
 * \brief Charts Test Application Window
 * \ingroup Charts
 */
class CQChartsAppWindow : public CQAppWindow {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsAppWindow(CQCharts *charts);
 ~CQChartsAppWindow();

  CQCharts *charts() const { return charts_; }

  CQChartsModelWidgets *modelWidgets() const { return modelWidgets_; }

 private:
  void addControlWidgets(QVBoxLayout *layout);

  void addMenus();

  QSize sizeHint() const;

 public slots:
  void closeSlot();

 private slots:
  void loadModelSlot();

  void modelLoadedSlot(int);

  void createPlotSlot();

 private:
  CQCharts*             charts_       { nullptr };
  CQChartsModelWidgets* modelWidgets_ { nullptr };
  CQChartsLoadModelDlg* loadModelDlg_ { nullptr };
};

#endif
