#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsViewExpander.h>
#include <CQChartsViewSettings.h>
#include <CQChartsTable.h>
#include <CQChartsViewStatus.h>
#include <CQChartsViewToolBar.h>
#include <CQChartsFilterEdit.h>
#include <CQGradientControlIFace.h>

#include <QVBoxLayout>

CQChartsWindowMgr *
CQChartsWindowMgr::
instance()
{
  static CQChartsWindowMgr *inst;

  if (! inst)
    inst = new CQChartsWindowMgr;

  return inst;
}

CQChartsWindowMgr::
CQChartsWindowMgr()
{
}

CQChartsWindowMgr::
~CQChartsWindowMgr()
{
  for (auto &window : windows_)
    delete window;
}

CQChartsWindow *
CQChartsWindowMgr::
createWindow(CQChartsView *view)
{
  CQChartsWindow *window = new CQChartsWindow(view);

  windows_.push_back(window);

  return window;
}

CQChartsWindow *
CQChartsWindowMgr::
getWindowForView(CQChartsView *view) const
{
  for (const auto &window : windows_)
    if (window->view() == view)
      return window;

  return nullptr;
}

//------

CQChartsWindow::
CQChartsWindow(CQChartsView *view) :
 QFrame(nullptr), view_(view)
{
  setWindowTitle("CQChartsWindow");

  setObjectName("window");

  setAttribute(Qt::WA_DeleteOnClose);

  //---

  view->setParent(this);

  //---

  settings_ = new CQChartsViewSettings(this);

  settingsExpander_ =
   new CQChartsViewExpander(this, settings_, CQChartsViewExpander::Side::RIGHT);

  //---

  QFrame *tableFrame = new QFrame(this);

  tableFrame->setObjectName("tableFrame");

  tableFrame->setAutoFillBackground(true);

  QVBoxLayout *tableLayout = new QVBoxLayout(tableFrame);
  tableLayout->setMargin(0); tableLayout->setSpacing(2);

  CQChartsFilterEdit *filterEdit = new CQChartsFilterEdit;

  connect(filterEdit, SIGNAL(replaceFilter(const QString &)), this,
          SLOT(replaceFilterSlot(const QString &)));
  connect(filterEdit, SIGNAL(addFilter(const QString &)), this,
          SLOT(addFilterSlot(const QString &)));

  connect(filterEdit, SIGNAL(replaceSearch(const QString &)),
          this, SLOT(replaceSearchSlot(const QString &)));
  connect(filterEdit, SIGNAL(addSearch(const QString &)),
          this, SLOT(addSearchSlot(const QString &)));

  tableLayout->addWidget(filterEdit);

  table_ = new CQChartsTable(view->charts(), this);

  tableLayout->addWidget(table_);

  connect(table_, SIGNAL(filterChanged()), this, SLOT(filterChangedSlot()));

  tableExpander_ =
   new CQChartsViewExpander(this, tableFrame, CQChartsViewExpander::Side::BOTTOM);

  //---

  status_  = new CQChartsViewStatus(this);
  toolbar_ = new CQChartsViewToolBar(this);

  //----

  connect(view_, SIGNAL(currentPlotChanged()), this, SLOT(plotSlot()));

  connect(view_, SIGNAL(modeChanged()), this, SLOT(modeSlot()));

  connect(view_, SIGNAL(posTextChanged(const QString &)),
          this, SLOT(setPosText(const QString &)));
  connect(view_, SIGNAL(statusTextChanged(const QString &)),
          this, SLOT(setStatusText(const QString &)));
  connect(view_, SIGNAL(selTextChanged(const QString &)),
          this, SLOT(setSelText(const QString &)));

  //---

  updateMargins();
}

CQChartsWindow::
~CQChartsWindow()
{
  delete settingsExpander_;
  delete tableExpander_;

  delete settings_;
  delete table_;
  delete status_;
  delete toolbar_;
}

void
CQChartsWindow::
resizeEvent(QResizeEvent *)
{
  updateGeometry();
}

void
CQChartsWindow::
updateMargins()
{
  statusHeight_  = status_ ->sizeHint().height();
  toolBarHeight_ = toolbar_->sizeHint().height();

  tableExpander_->setMargins(0, statusHeight_, 0, toolBarHeight_);
}

void
CQChartsWindow::
updateGeometry()
{
  setMinimumSize(16 + settingsExpander_->width(), 16 + statusHeight_ + toolBarHeight_);

  //---

  view_->resize(width() - settingsExpander_->width(), height() - statusHeight_ - toolBarHeight_);

  view_->move(0, toolBarHeight_);

  //---

  settingsExpander_->setVisible(true);
  settingsExpander_->updateGeometry();

  tableExpander_->setVisible(true);
  tableExpander_->updateGeometry();

  //---

  toolbar_->move(0, 0);
  toolbar_->resize(width(), toolBarHeight_);

  //---

  status_->move(0, height() - statusHeight_);
  status_->resize(width(), statusHeight_);

  //---

  toolbar_ ->raise();
  status_  ->raise();
  settings_->raise();
}

void
CQChartsWindow::
setStatusText(const QString &text)
{
  status_->setStatusText(text);
}

void
CQChartsWindow::
setPosText(const QString &text)
{
  status_->setPosText(text);
}

void
CQChartsWindow::
setSelText(const QString &text)
{
  status_->setSelText(text);
}

void
CQChartsWindow::
replaceFilterSlot(const QString &text)
{
  table_->setFilter(text);
}

void
CQChartsWindow::
addFilterSlot(const QString &text)
{
  table_->addFilter(text);
}

void
CQChartsWindow::
replaceSearchSlot(const QString &text)
{
  table_->setSearch(text);
}

void
CQChartsWindow::
addSearchSlot(const QString &text)
{
  table_->addSearch(text);
}

void
CQChartsWindow::
filterChangedSlot()
{
  //CQChartsPlot *plot = view_->currentPlot();
  //if (! plot) return;

  //plot->updateRangeAndObjs();
}

void
CQChartsWindow::
plotSlot()
{
  CQChartsPlot *plot = view_->currentPlot();
  if (! plot) return;

  table_->setModel(plot->modelp());

  plot->setSelectionModel(table_->selectionModel());
}

void
CQChartsWindow::
modeSlot()
{
  toolbar_->updateMode();
}

void
CQChartsWindow::
updatePalette()
{
  settings_->paletteControl()->updateState();
}

QSize
CQChartsWindow::
sizeHint() const
{
  return QSize(1600, 1200);
}
