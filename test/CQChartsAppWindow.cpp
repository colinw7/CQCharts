#include <CQChartsAppWindow.h>
#include <CQChartsModelWidgets.h>
#include <CQChartsLoadModelDlg.h>
#include <CQChartsCreatePlotDlg.h>
#include <CQChartsModelData.h>
#include <CQCharts.h>
#include <CQUtil.h>

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStackedWidget>
#include <QVBoxLayout>

CQChartsAppWindow::
CQChartsAppWindow(CQCharts *charts) :
 CQAppWindow(), charts_(charts)
{
  // add menus
  addMenus();

  //---

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(centralWidget(), 0, 0);

  //---

  // create model widgets
  modelWidgets_ = new CQChartsModelWidgets(charts_);

  layout->addWidget(modelWidgets_);
}

CQChartsAppWindow::
~CQChartsAppWindow()
{
  delete loadModelDlg_;
}

void
CQChartsAppWindow::
addMenus()
{
  auto *menuBar = addMenuBar();

  auto *fileMenu = menuBar->addMenu("&File");

  auto loadAction = new QAction("Load Model", menuBar);

  loadAction->setShortcut(QKeySequence("Ctrl+L"));

  connect(loadAction, SIGNAL(triggered()), this, SLOT(loadModelSlot()));

  fileMenu->addAction(loadAction);

  auto closeAction = new QAction("Close", menuBar);

  connect(closeAction, SIGNAL(triggered()), this, SLOT(closeSlot()));

  fileMenu->addAction(closeAction);

  //---

  auto *plotMenu = menuBar->addMenu("&Plot");

  auto createAction = new QAction("Create Plot", menuBar);

  createAction->setShortcut(QKeySequence("Ctrl+P"));

  connect(createAction, SIGNAL(triggered()), this, SLOT(createPlotSlot()));

  plotMenu->addAction(createAction);

  //---

  auto *helpMenu = menuBar->addMenu("&Help");

  auto helpAction = new QAction("Help"  , menuBar);

  helpMenu->addAction(helpAction);
}

void
CQChartsAppWindow::
loadModelSlot()
{
  if (! loadModelDlg_) {
    loadModelDlg_ = new CQChartsLoadModelDlg(charts_);

    connect(loadModelDlg_, SIGNAL(modelLoaded(int)), this, SLOT(modelLoadedSlot(int)));
  }

  loadModelDlg_->exec();
}

void
CQChartsAppWindow::
modelLoadedSlot(int ind)
{
  charts_->setCurrentModelInd(ind);
}

void
CQChartsAppWindow::
closeSlot()
{
  close();
}

void
CQChartsAppWindow::
createPlotSlot()
{
  auto *modelData = charts_->currentModelData();

  if (! modelData)
    return;

  //---

  auto createPlotDlg = new CQChartsCreatePlotDlg(charts_, modelData);

  if (! createPlotDlg->exec())
    return;

  //---

  delete createPlotDlg;
}

QSize
CQChartsAppWindow::
sizeHint() const
{
  return QSize(1024, 1024);
}
