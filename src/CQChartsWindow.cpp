#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>
#include <CQChartsViewSettings.h>
#include <CQChartsViewStatus.h>
#include <CQChartsViewToolBar.h>
#include <CQChartsFilterEdit.h>
#include <CQChartsModelViewHolder.h>
#include <CQChartsPropertyViewTree.h>
#include <CQChartsModelView.h>
#include <CQChartsWidgetUtil.h>

#include <CQPixmapCache.h>
#include <CQTabSplit.h>
#include <CQUtil.h>

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
createWindow(View *view)
{
  auto *window = new CQChartsWindow(view);

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
getWindowForView(View *view) const
{
  for (const auto &window : windows_)
    if (window->view() == view)
      return window;

  return nullptr;
}

//------

CQChartsWindow::
CQChartsWindow(View *view) :
 QFrame(nullptr), view_(view)
{
  setWindowTitle("Window: View " + view_->id());
  setWindowIcon(CQPixmapCacheInst->getLightDarkIcon("CHARTS"));

  setObjectName("window");

  setAttribute(Qt::WA_DeleteOnClose);

  //---

  view_->setWindow(this);

  //---

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //---

  toolbar_ = new CQChartsViewToolBar(this);

  layout->addWidget(toolbar_);

  //---

  auto *settingsSplitter = CQUtil::makeWidget<QSplitter>("hsplitter");

  settingsSplitter->setOrientation(Qt::Horizontal);

  layout->addWidget(settingsSplitter);

  //---

  status_ = new CQChartsViewStatus(this);

  layout->addWidget(status_);

  //----

  viewSplitter_ = CQUtil::makeWidget<CQTabSplit>("vsplitter");

  viewSplitter_->setState(CQTabSplit::State::VSPLIT);
//viewSplitter_->setGrouped(true);

  settingsSplitter->addWidget(viewSplitter_);

  //---

  settings_ = new CQChartsViewSettings(this);

  connect(settings_, SIGNAL(propertyItemSelected(QObject *, const QString &)),
          this, SLOT(propertyItemSelected(QObject *, const QString &)));

  settingsSplitter->addWidget(settings_);

  //---

  auto *viewFrame = CQUtil::makeWidget<QFrame>("viewFrame");

  viewFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  auto *viewLayout = CQUtil::makeLayout<QGridLayout>(viewFrame, 2, 2);

  viewSplitter_->addWidget(viewFrame, "View");

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

  auto *tableLayout = CQUtil::makeLayout<QVBoxLayout>(tableFrame_, 0, 2);

  filterEdit_ = new CQChartsFilterEdit;

  connect(filterEdit_, SIGNAL(filterAnd(bool)), this, SLOT(filterAndSlot(bool)));

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

  connectModelViewExpand(true);

  tableLayout->addWidget(modelView_);

  tableFrame_->setVisible(false);

  setDataTable(showDataTable_, /*force*/true);

  //---

  connect(view_->charts(), SIGNAL(viewRemoved(CQChartsView *)),
          this, SLOT(removeViewSlot(CQChartsView *)));

  connect(view_, SIGNAL(currentPlotChanged()), this, SLOT(plotSlot()));

  connect(view_, SIGNAL(modeChanged()), this, SLOT(modeSlot()));
  connect(view_, SIGNAL(selectModeChanged()), this, SLOT(selectModeSlot()));

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
  auto *plot = view()->currentPlot();
  if (! plot) return;

  CQChartsWidgetUtil::AutoDisconnect xscrollDisconnect(
    xrangeScroll_, SIGNAL(windowChanged()), this, SLOT(rangeScrollSlot()));
  CQChartsWidgetUtil::AutoDisconnect yscrollDisconnect(
    yrangeScroll_, SIGNAL(windowChanged()), this, SLOT(rangeScrollSlot()));

  auto bbox1 = plot->getDataRange ();
  auto bbox2 = plot->calcDataRange();

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
}

void
CQChartsWindow::
setDataTable(bool b, bool force)
{
  if (force || b != showDataTable_) {
    if      (showDataTable_ && ! b) {
      viewSplitter_->removeWidget(tableFrame_, /*destroy*/false);
      //tableFrame_->setVisible(false);
    }
    else if (! showDataTable_ && b) {
      viewSplitter_->addWidget(tableFrame_, "Table");
      //tableFrame_->setVisible(true);
    }

    showDataTable_ = b;

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
  auto *plot = view()->currentPlot();
  if (! plot) return;

  auto dataRange = plot->getDataRange();

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

  auto details = modelView_->filterDetails();

  filterEdit_->setFilterDetails(details);
}

void
CQChartsWindow::
replaceFilterSlot(const QString &text)
{
  modelView_->setFilter(text);

  auto details = modelView_->filterDetails();

  filterEdit_->setFilterDetails(details);
}

void
CQChartsWindow::
addFilterSlot(const QString &text)
{
  modelView_->addFilter(text);

  auto details = modelView_->filterDetails();

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
  //auto *plot = view_->currentPlot(/*remap*/false);
  //if (! plot) return;

  //plot->updateRangeAndObjs();
}

//------

void
CQChartsWindow::
removeViewSlot(View *view)
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
  auto *plot = view_->currentPlot(/*remap*/false);

  if (plot)
    setWindowTitle(QString("Window: View %1, Plot %2").arg(view_->id()).arg(plot->id()));
  else
    setWindowTitle(QString("Window: View %1, Plot <none>").arg(view_->id()));

  if (tableFrame_->isVisible())
    setViewModel();
  else
    modelView_->setModel(CQChartsModelViewHolder::ModelP(), false);
}

void
CQChartsWindow::
setViewModel()
{
  auto *plot = view_->currentPlot(/*remap*/false);

  if (plot) {
    if (plot->model() != modelView_->model())
      modelView_->setModel(plot->model(), plot->isHierarchical());
  }
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

  CQChartsWidgetUtil::AutoDisconnect settingsDisconnect(
    settings_, SIGNAL(propertyItemSelected(QObject *, const QString &)),
    this, SLOT(propertyItemSelected(QObject *, const QString &)));

  //---

  // get currently selected property view objects
  CQPropertyViewTree::Objs selectedObjs;

  settings_->viewPropertyTree()->getSelectedObjects(selectedObjs);

  ObjSet selectedObjSet;

  for (auto &obj : selectedObjs)
    selectedObjSet.insert(obj);

  View::Plots plots;

  view_->getPlots(plots);

  for (auto &plot : plots) {
    CQPropertyViewTree::Objs selectedObjs1;

    settings_->plotPropertyTree(plot)->getSelectedObjects(selectedObjs1);

    for (auto &obj : selectedObjs1)
      selectedObjSet.insert(obj);
  }

  //---

  // get selected charts objects
  View::Objs objs;

  view_->allSelectedObjs(objs);

  View::Plots selectedPlots;

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
      auto *plot = objectPlot(obj);

      if (plot)
        settings_->plotPropertyTree(plot)->selectObject(obj);
      else
        settings_->viewPropertyTree()->selectObject(obj);
    }
  }
}

void
CQChartsWindow::
propertyItemSelected(QObject *obj, const QString &path)
{
  auto *plot = objectPlot(obj);

  if (plot)
    plot->propertyItemSelected(obj, path);
}

//---

void
CQChartsWindow::
connectModelViewExpand(bool connect)
{
#ifdef CQCHARTS_MODEL_VIEW
  CQChartsWidgetUtil::connectDisconnect(connect,
    modelView_->view(), SIGNAL(expanded(const QModelIndex &)),
    this, SLOT(expansionChangeSlot()));
  CQChartsWidgetUtil::connectDisconnect(connect,
    modelView_->view(), SIGNAL(collapsed(const QModelIndex &)),
    this, SLOT(expansionChangeSlot()));
#endif
}

void
CQChartsWindow::
expansionChangeSlot()
{
  emit expansionChanged();
}

bool
CQChartsWindow::
isExpandModelIndex(const QModelIndex &ind) const
{
  const_cast<CQChartsWindow *>(this)->setViewModel();

#ifdef CQCHARTS_MODEL_VIEW
  return modelView_->view()->isExpanded(ind);
#endif
}

void
CQChartsWindow::
expandModelIndex(const QModelIndex &ind, bool b)
{
  setViewModel();

#ifdef CQCHARTS_MODEL_VIEW
  connectModelViewExpand(false);

  modelView_->view()->setExpanded(ind, b);

  connectModelViewExpand(true);
#endif
}

void
CQChartsWindow::
expandedModelIndices(QModelIndexList &inds)
{
  setViewModel();

#ifdef CQCHARTS_MODEL_VIEW
  modelView_->view()->expandedIndices(inds);
#endif
}

//---

void
CQChartsWindow::
showErrorsTab()
{
  settings_->showErrorsTab();
}

void
CQChartsWindow::
setHasErrors(bool b)
{
  status_->setHasErrors(b);
}

//---

CQChartsPlot *
CQChartsWindow::
objectPlot(QObject *obj) const
{
  auto *obj1 = obj;

  while (obj1) {
    auto *plot = qobject_cast<Plot *>(obj1);

    if (plot)
      return plot;

    obj1 = obj1->parent();
  }

  return nullptr;
}

//---

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
  auto *plot = window_->view()->currentPlot();
  if (! plot) return;

  plot->setOverview(true);

  window_->view()->doResize(p->device()->width(), p->device()->height());

  auto zoomData    = plot->zoomData();
  auto outerMargin = plot->outerMargin();

  auto margin = CQChartsLength::pixel(this->margin());

  plot->setOuterMargin(CQChartsPlotMargin(margin, CQChartsLength(), margin, CQChartsLength()));

  plot->zoomFull(/*notify*/false);

  plot->draw(p);

  window_->view()->doResize(window_->view()->width(), window_->view()->height());

  plot->setOuterMargin(outerMargin);

  plot->setZoomData(zoomData);

  plot->setOverview(false);
}
