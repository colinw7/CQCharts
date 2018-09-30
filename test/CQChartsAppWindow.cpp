#include <CQChartsAppWindow.h>
#include <CQChartsModelWidgets.h>
#include <CQChartsLoadDlg.h>
#include <CQChartsPlotDlg.h>
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

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(centralWidget(), 0, 0);

  //---

  // create model widgets
  modelWidgets_ = new CQChartsModelWidgets(charts_);

  layout->addWidget(modelWidgets_);
}

CQChartsAppWindow::
~CQChartsAppWindow()
{
  delete loadDlg_;
}

void
CQChartsAppWindow::
addMenus()
{
  QMenuBar *menuBar = addMenuBar();

  QMenu *fileMenu = menuBar->addMenu("&File");

  QAction *loadAction = new QAction("Load Model", menuBar);

  loadAction->setShortcut(QKeySequence("Ctrl+L"));

  connect(loadAction, SIGNAL(triggered()), this, SLOT(loadModelSlot()));

  fileMenu->addAction(loadAction);

  QAction *closeAction = new QAction("Close", menuBar);

  connect(closeAction, SIGNAL(triggered()), this, SLOT(closeSlot()));

  fileMenu->addAction(closeAction);

  //---

  QMenu *plotMenu = menuBar->addMenu("&Plot");

  QAction *createAction = new QAction("Create Plot", menuBar);

  createAction->setShortcut(QKeySequence("Ctrl+P"));

  connect(createAction, SIGNAL(triggered()), this, SLOT(createPlotSlot()));

  plotMenu->addAction(createAction);

  //---

  QMenu *helpMenu = menuBar->addMenu("&Help");

  QAction *helpAction = new QAction("Help"  , menuBar);

  helpMenu->addAction(helpAction);
}

void
CQChartsAppWindow::
loadModelSlot()
{
  if (! loadDlg_) {
    loadDlg_ = new CQChartsLoadDlg(charts_);

    connect(loadDlg_, SIGNAL(modelLoaded(int)), this, SLOT(modelLoadedSlot(int)));
  }

  loadDlg_->exec();
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
  CQChartsModelData *modelData = charts_->currentModelData();

  if (! modelData)
    return;

  //---

  CQChartsPlotDlg *dlg = new CQChartsPlotDlg(charts_, modelData);

  if (modelData->selectionModel())
    dlg->setSelectionModel(modelData->selectionModel());

  if (! dlg->exec())
    return;

  //---

  delete dlg;
}

QSize
CQChartsAppWindow::
sizeHint() const
{
  return QSize(1024, 1024);
}
