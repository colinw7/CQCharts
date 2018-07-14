#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsViewExpander.h>
#include <CQChartsViewSettings.h>
#include <CQChartsTable.h>
#include <CQChartsTree.h>
#include <CQChartsViewStatus.h>
#include <CQChartsViewToolBar.h>
#include <CQChartsFilterEdit.h>
#include <CQChartsModelView.h>
#include <CQChartsGradientPaletteControl.h>
#include <CQPixmapCache.h>

#include <svg/charts_svg.h>

#include <QStackedWidget>
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
  setWindowTitle("Window: View " + view_->id());
  setWindowIcon(CQPixmapCacheInst->getIcon("CHARTS"));

  setObjectName("window");

  setAttribute(Qt::WA_DeleteOnClose);

  //---

  view_->setParent(this);

  //---

  settings_ = new CQChartsViewSettings(this);

  connect(settings_, SIGNAL(propertyItemSelected(QObject *, const QString &)),
          this, SLOT(propertyItemSelected(QObject *, const QString &)));

  settingsExpander_ =
    new CQChartsViewExpander(this, settings_, CQChartsViewExpander::Side::RIGHT);

  settingsExpander_->setObjectName("settingsExpander");

  settingsExpander_->setTitle(view_->id() + ": Settings");
  settingsExpander_->setIcon(CQPixmapCacheInst->getIcon("CHARTS"));

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

  modelView_ = new CQChartsModelView(view_->charts());

  connect(modelView_, SIGNAL(filterChanged()), this, SLOT(filterChangedSlot()));

  tableLayout->addWidget(modelView_);

  tableExpander_ =
   new CQChartsViewExpander(this, tableFrame, CQChartsViewExpander::Side::BOTTOM);

  tableExpander_->setObjectName("tableExpander");

  tableExpander_->setTitle(view_->id() + ": Model");
  tableExpander_->setIcon(CQPixmapCacheInst->getIcon("CHARTS"));

  //---

  status_  = new CQChartsViewStatus(this);
  toolbar_ = new CQChartsViewToolBar(this);

  //----

  connect(view_, SIGNAL(currentPlotChanged()), this, SLOT(plotSlot()));

  connect(view_, SIGNAL(modeChanged()), this, SLOT(modeSlot()));
  connect(view_, SIGNAL(selectModeChanged()), this, SLOT(selectModeSlot()));

  connect(view_, SIGNAL(interfacePaletteChanged()), this, SIGNAL(interfacePaletteChanged()));
  connect(view_, SIGNAL(themePalettesChanged()), this, SIGNAL(themePalettesChanged()));

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
  int sew = (! settingsExpander_->isDetached() ? settingsExpander_->width () : 0);
  int teh = (! tableExpander_   ->isDetached() ? tableExpander_   ->height() : 0);

  setMinimumSize(16 + sew, 16 + statusHeight_ + toolBarHeight_ + teh);

  //---

  view_->resize(width() - sew, height() - statusHeight_ - toolBarHeight_ - teh);

  view_->move(0, toolBarHeight_);

  //---

  if (! settingsExpander_->isDetached()) {
    settingsExpander_->setVisible(true);
    settingsExpander_->updateGeometry();
  }

  if (! tableExpander_->isDetached()) {
    tableExpander_->setVisible(true);
    tableExpander_->updateGeometry();
  }

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
  modelView_->setFilter(text);
}

void
CQChartsWindow::
addFilterSlot(const QString &text)
{
  modelView_->addFilter(text);
}

void
CQChartsWindow::
replaceSearchSlot(const QString &text)
{
  modelView_->setSearch(text);
}

void
CQChartsWindow::
addSearchSlot(const QString &text)
{
  modelView_->addSearch(text);
}

void
CQChartsWindow::
filterChangedSlot()
{
  //CQChartsPlot *plot = view_->currentPlot(/*remap*/false);
  //if (! plot) return;

  //plot->updateRangeAndObjs();
}

void
CQChartsWindow::
plotSlot()
{
  CQChartsPlot *plot = view_->currentPlot(/*remap*/false);
  if (! plot) return;

  modelView_->setModel(plot->model(), plot->isHierarchical());

  plot->setSelectionModel(modelView_->selectionModel());

  tableExpander_->setTitle(view_->id() + ": Model (" + plot->id() + ")");
}

void
CQChartsWindow::
modeSlot()
{
  toolbar_->updateMode();
}

void
CQChartsWindow::
selectModeSlot()
{
  toolbar_->updateMode();
}

void
CQChartsWindow::
updateInterfacePalette()
{
  emit interfacePaletteChanged();
}

void
CQChartsWindow::
updateThemePalettes()
{
  emit themePalettesChanged();
}

void
CQChartsWindow::
propertyItemSelected(QObject *obj, const QString &path)
{
  QObject *obj1 = obj;

  while (obj1) {
    CQChartsPlot *plot = qobject_cast<CQChartsPlot *>(obj1);

    if (plot) {
      plot->propertyItemSelected(obj, path);
      return;
    }

    obj1 = obj1->parent();
  }
}

QSize
CQChartsWindow::
sizeHint() const
{
  return QSize(1400, 1200);
}
