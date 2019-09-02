#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>
#include <CQChartsViewExpander.h>
#include <CQChartsViewSettings.h>
#include <CQChartsViewStatus.h>
#include <CQChartsViewToolBar.h>
#include <CQChartsFilterEdit.h>
#include <CQChartsModelViewHolder.h>
#include <CQChartsPropertyViewTree.h>
#include <CQPixmapCache.h>
#include <CQTabSplit.h>
#include <CQUtil.h>

#include <svg/charts_svg.h>

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QPainter>

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

void
CQChartsWindowMgr::
removeWindow(CQChartsWindow *window)
{
  int i = 0;
  int n = windows_.size();

  for ( ; i < n; ++i) {
    if (windows_[i] == window)
      break;
  }

  assert(i < n);

  ++i;

  for ( ; i < n; ++i)
    windows_[i - 1] = windows_[i];

  windows_.pop_back();

  delete window;
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

  view_->setWindow(this);

  //---

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //---

  toolbar_ = new CQChartsViewToolBar(this);

  layout->addWidget(toolbar_);

  //---

  QSplitter *settingsSplitter = CQUtil::makeWidget<QSplitter>("hsplitter");

  settingsSplitter->setOrientation(Qt::Horizontal);

  layout->addWidget(settingsSplitter);

  //---

  status_ = new CQChartsViewStatus(this);

  layout->addWidget(status_);

  //----

  CQTabSplit *viewSplitter = CQUtil::makeWidget<CQTabSplit>("vsplitter");

  viewSplitter->setState(CQTabSplit::State::VSPLIT);

  settingsSplitter->addWidget(viewSplitter);

  //---

  settings_ = new CQChartsViewSettings(this);

  connect(settings_, SIGNAL(propertyItemSelected(QObject *, const QString &)),
          this, SLOT(propertyItemSelected(QObject *, const QString &)));

  settingsSplitter->addWidget(settings_);

  //---

  QFrame *viewFrame = CQUtil::makeWidget<QFrame>("viewFrame");

  viewFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QGridLayout *viewLayout = CQUtil::makeLayout<QGridLayout>(viewFrame, 2, 2);

  viewSplitter->addWidget(viewFrame, "View");

  viewLayout->addWidget(view, 0, 0);

  //---

  xrangeScroll_ = new CQChartsWindowRangeScroll(this, Qt::Horizontal);
  yrangeScroll_ = new CQChartsWindowRangeScroll(this, Qt::Vertical  );

  xrangeScroll_->setFixedHeight(128);
  yrangeScroll_->setFixedWidth (128);

  viewLayout->addWidget(xrangeScroll_, 1, 0);
  viewLayout->addWidget(yrangeScroll_, 0, 1);

  connect(xrangeScroll_, SIGNAL(windowChanged()), this, SLOT(rangeScrollSlot()));
  connect(yrangeScroll_, SIGNAL(windowChanged()), this, SLOT(rangeScrollSlot()));

  xrangeScroll_->setVisible(false);
  yrangeScroll_->setVisible(false);

  //---

  tableFrame_ = CQUtil::makeWidget<QFrame>(this, "tableFrame");

  tableFrame_->setAutoFillBackground(true);

  QVBoxLayout *tableLayout = CQUtil::makeLayout<QVBoxLayout>(tableFrame_, 0, 2);

  filterEdit_ = new CQChartsFilterEdit;

  connect(filterEdit_, SIGNAL(filterAnd(bool)),
          this, SLOT(filterAndSlot(bool)));

  connect(filterEdit_, SIGNAL(replaceFilter(const QString &)),
          this, SLOT(replaceFilterSlot(const QString &)));
  connect(filterEdit_, SIGNAL(addFilter(const QString &)),
          this, SLOT(addFilterSlot(const QString &)));

  connect(filterEdit_, SIGNAL(replaceSearch(const QString &)),
          this, SLOT(replaceSearchSlot(const QString &)));
  connect(filterEdit_, SIGNAL(addSearch(const QString &)),
          this, SLOT(addSearchSlot(const QString &)));

  tableLayout->addWidget(filterEdit_);

  modelView_ = new CQChartsModelViewHolder(view_->charts());

  connect(modelView_, SIGNAL(filterChanged()), this, SLOT(filterChangedSlot()));

  tableLayout->addWidget(modelView_);

  viewSplitter->addWidget(tableFrame_, "Table");

  tableFrame_->setVisible(dataTable_);

  //---

  connect(view_->charts(), SIGNAL(viewRemoved(CQChartsView *)),
          this, SLOT(removeViewSlot(CQChartsView *)));

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
}

CQChartsWindow::
~CQChartsWindow()
{
  delete settings_;
  delete status_;
  delete toolbar_;
}

void
CQChartsWindow::
resizeEvent(QResizeEvent *)
{
}

void
CQChartsWindow::
setXRangeMap(bool b)
{
  xRangeMap_ = b;

  xrangeScroll_->setVisible(xRangeMap_);
}

void
CQChartsWindow::
setYRangeMap(bool b)
{
  yRangeMap_ = b;

  yrangeScroll_->setVisible(yRangeMap_);
}

void
CQChartsWindow::
updateRangeMap()
{
  CQChartsPlot *plot = view()->currentPlot();
  if (! plot) return;

  disconnect(xrangeScroll_, SIGNAL(windowChanged()), this, SLOT(rangeScrollSlot()));
  disconnect(yrangeScroll_, SIGNAL(windowChanged()), this, SLOT(rangeScrollSlot()));

  CQChartsGeom::BBox bbox1 = plot->getDataRange ();
  CQChartsGeom::BBox bbox2 = plot->calcDataRange();

  double xsize = bbox2.getWidth ()/bbox1.getWidth ();
  double xpos  = (bbox2.getXMin() - bbox1.getXMin())/bbox1.getWidth();
  double ysize = bbox2.getHeight()/bbox1.getHeight();
  double ypos  = (bbox2.getYMin() - bbox1.getYMin())/bbox1.getHeight();

  if (xsize != xrangeScroll_->len()) {
    xrangeScroll_->setLen(xsize);
    xrangeScroll_->setPos(xpos);
  }

  if (ysize != yrangeScroll_->len()) {
    yrangeScroll_->setLen(ysize);
    xrangeScroll_->setPos(ypos);
  }

  connect(xrangeScroll_, SIGNAL(windowChanged()), this, SLOT(rangeScrollSlot()));
  connect(yrangeScroll_, SIGNAL(windowChanged()), this, SLOT(rangeScrollSlot()));
}

void
CQChartsWindow::
setDataTable(bool b)
{
  if (b != dataTable_) {
    dataTable_ = b;

    tableFrame_->setVisible(dataTable_);

    plotSlot();
  }
}

void
CQChartsWindow::
setViewSettings(bool b)
{
  viewSettings_ = b;

  settings_->setVisible(viewSettings_);
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
rangeScrollSlot()
{
  CQChartsPlot *plot = view()->currentPlot();
  if (! plot) return;

  CQChartsGeom::BBox dataRange = plot->getDataRange();

  if (xrangeScroll_->isVisible()) {
    double pos = xrangeScroll_->pos();
    double len = xrangeScroll_->len();

    double xmin = dataRange.getXMin() + pos*dataRange.getWidth();
    double xmax = xmin + len*dataRange.getWidth();

    dataRange.setXMin(xmin);
    dataRange.setXMax(xmax);
  }

  if (yrangeScroll_->isVisible()) {
    double pos = yrangeScroll_->pos();
    double len = yrangeScroll_->len();

    double ymin = dataRange.getYMin() + pos*dataRange.getHeight();
    double ymax = ymin + len*dataRange.getHeight();

    dataRange.setYMin(ymin);
    dataRange.setYMax(ymax);
  }

  plot->zoomTo(dataRange);
}

void
CQChartsWindow::
filterAndSlot(bool b)
{
  modelView_->setFilterAnd(b);

  QString details = modelView_->filterDetails();

  filterEdit_->setFilterDetails(details);
}

void
CQChartsWindow::
replaceFilterSlot(const QString &text)
{
  modelView_->setFilter(text);

  QString details = modelView_->filterDetails();

  filterEdit_->setFilterDetails(details);
}

void
CQChartsWindow::
addFilterSlot(const QString &text)
{
  modelView_->addFilter(text);

  QString details = modelView_->filterDetails();

  filterEdit_->setFilterDetails(details);
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
removeViewSlot(CQChartsView *view)
{
  if (view_ != view)
    return;

  view_->setParent(nullptr);

  view_ = nullptr;

  deleteLater();
}

void
CQChartsWindow::
plotSlot()
{
  CQChartsPlot *plot = view_->currentPlot(/*remap*/false);

  if (plot)
    setWindowTitle(QString("Window: View %1, Plot %2").arg(view_->id()).arg(plot->id()));
  else
    setWindowTitle(QString("Window: View %1, Plot <none>").arg(view_->id()));

  if (tableFrame_->isVisible() && plot)
    modelView_->setModel(plot->model(), plot->isHierarchical());
  else
    modelView_->setModel(CQChartsModelViewHolder::ModelP(), false);
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
selectPropertyObjects()
{
  using ObjSet = std::set<QObject *>;

  //---

  disconnect(settings_, SIGNAL(propertyItemSelected(QObject *, const QString &)),
             this, SLOT(propertyItemSelected(QObject *, const QString &)));

  //---

  // get currently selected property view objects
  CQPropertyViewTree::Objs selectedObjs;

  settings_->viewPropertyTree()->getSelectedObjects(selectedObjs);

  ObjSet selectedObjSet;

  for (auto &obj : selectedObjs)
    selectedObjSet.insert(obj);

  CQChartsView::Plots plots;

  view_->getPlots(plots);

  for (auto &plot : plots) {
    CQPropertyViewTree::Objs selectedObjs;

    settings_->plotPropertyTree(plot)->getSelectedObjects(selectedObjs);

    for (auto &obj : selectedObjs)
      selectedObjSet.insert(obj);
  }

  //---

  // get selected charts objects
  CQChartsView::Objs objs;

  view_->allSelectedObjs(objs);

  CQChartsView::Plots selectedPlots;

  view_->selectedPlots(selectedPlots);

  ObjSet objSet;

  for (auto &obj : objs)
    objSet.insert(obj);

  for (auto &plot : selectedPlots)
    objSet.insert(plot);

  //---

  // check if selection has changed
  bool changed = (objSet.size() != selectedObjSet.size());

  if (! changed) {
    for (auto &obj : objSet) {
      if (selectedObjSet.find(obj) == selectedObjSet.end()) {
        changed = true;
        break;
      }
    }
  }

  //---

  // update selected if changed
  if (changed) {
    settings_->viewPropertyTree()->deselectAllObjects();

    for (auto &plot : plots)
      settings_->plotPropertyTree(plot)->deselectAllObjects();

    for (auto &obj : objSet) {
      CQChartsPlot *plot = objectPlot(obj);

      if (plot)
        settings_->plotPropertyTree(plot)->selectObject(obj);
      else
        settings_->viewPropertyTree()->selectObject(obj);
    }
  }

  //---

  connect(settings_, SIGNAL(propertyItemSelected(QObject *, const QString &)),
          this, SLOT(propertyItemSelected(QObject *, const QString &)));
}

void
CQChartsWindow::
propertyItemSelected(QObject *obj, const QString &path)
{
  CQChartsPlot *plot = objectPlot(obj);

  if (plot)
    plot->propertyItemSelected(obj, path);
}

CQChartsPlot *
CQChartsWindow::
objectPlot(QObject *obj) const
{
  QObject *obj1 = obj;

  while (obj1) {
    CQChartsPlot *plot = qobject_cast<CQChartsPlot *>(obj1);

    if (plot)
      return plot;

    obj1 = obj1->parent();
  }

  return nullptr;
}

QSize
CQChartsWindow::
sizeHint() const
{
  QFontMetrics fm(font());

  int w = fm.width("X")*150;
  int h = fm.height()*80;

  return QSize(w, h);
}

//------

CQChartsWindowRangeScroll::
CQChartsWindowRangeScroll(CQChartsWindow *window, Qt::Orientation orientation) :
 CQRangeScroll(orientation), window_(window)
{
}

void
CQChartsWindowRangeScroll::
drawBackground(QPainter *p)
{
  CQChartsPlot *plot = window_->view()->currentPlot();
  if (! plot) return;

  plot->setOverview(true);

  window_->view()->doResize(p->device()->width(), p->device()->height());

  CQChartsPlot::ZoomData zoomData = plot->zoomData();

  CQChartsPlotMargin outerMargin = plot->outerMargin();

  CQChartsLength margin(this->margin(), CQChartsUnits::PIXEL);

  plot->setOuterMargin(CQChartsPlotMargin(margin, CQChartsLength(), margin, CQChartsLength()));

  plot->zoomFull(/*notify*/false);

  plot->draw(p);

  window_->view()->doResize(window_->view()->width(), window_->view()->height());

  plot->setOuterMargin(outerMargin);

  plot->setZoomData(zoomData);

  plot->setOverview(false);
}
