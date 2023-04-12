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

#include <CQPixmapCache.h>
#include <CQTabSplit.h>
#include <CQUtil.h>
#include <CQChartsEnv.h>

#include <QStackedWidget>
#include <QComboBox>
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

CQChartsTabWindow *
CQChartsWindowMgr::
getTabWindow()
{
  if (! tabWindow_)
    tabWindow_ = new CQChartsTabWindow;

  tabWindow_->show();

  return tabWindow_;
}

CQChartsWindow *
CQChartsWindowMgr::
createWindow(View *view)
{
  auto *window = new CQChartsWindow(view);

  windows_.push_back(window);

  int n = windows_.size();

  window->setObjectName(QString("window.%1").arg(n));

  //---

  if (CQChartsEnv::getBool("CQCHARTS_TAB_WINDOW")) {
    auto *tabWindow = getTabWindow();

    tabWindow->addWindow(window);
  }

  return window;
}

void
CQChartsWindowMgr::
removeWindow(CQChartsWindow *window)
{
  size_t i = 0;
  size_t n = windows_.size();

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

CQChartsTabWindow::
CQChartsTabWindow() :
 QFrame(nullptr)
{
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //---

  // view area
  split_ = CQUtil::makeWidget<QSplitter>("split");

  split_->setOrientation(Qt::Horizontal);

  layout->addWidget(split_);

  //---

  // windows area
  windowsSplit_ = CQUtil::makeWidget<CQTabSplit>("split");

  windowsSplit_->setState(CQTabSplit::State::TAB);

  split_->addWidget(windowsSplit_);

  connect(windowsSplit_, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexSlot(int)));

  //---

  // setting area
  settingsArea_ = CQUtil::makeWidget<QFrame>("settingsArea");

  split_->addWidget(settingsArea_);

  //---

  int i1 = int(INT_MAX*0.8);
  int i2 = INT_MAX - i1;

  split_->setSizes(QList<int>({i1, i2}));
}

void
CQChartsTabWindow::
addWindow(Window *window)
{
  setCurrentWindow(window);

  windowsSplit_->addWidget(window, window->objectName());
}

void
CQChartsTabWindow::
currentIndexSlot(int i)
{
  auto *window = qobject_cast<Window *>(i >= 0 ? windowsSplit_->widget(i) : nullptr);

  setCurrentWindow(window);
}

void
CQChartsTabWindow::
setCurrentWindow(Window *window)
{
  if (currentWindow_)
    currentWindow_->viewSettings()->setVisible(false);

  currentWindow_ = window;

  if (currentWindow_) {
    delete settingsArea_->layout();

    auto *layout = CQUtil::makeLayout<QVBoxLayout>(settingsArea_, 0, 0);

    layout->addWidget(currentWindow_->viewSettings());

    currentWindow_->viewSettings()->setVisible(true);
  }
}

//---

QSize
CQChartsTabWindow::
sizeHint() const
{
  QFontMetrics fm(font());

  int w = fm.horizontalAdvance("X")*200;
  int h = fm.height()*80;

  return QSize(w, h);
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

  // create range scrolls (for overview)
  xrangeScroll_ = new CQChartsWindowRangeScroll(this, Qt::Horizontal);
  yrangeScroll_ = new CQChartsWindowRangeScroll(this, Qt::Vertical  );

  updateOverview();

  viewLayout->addWidget(xrangeScroll_, 1, 0);
  viewLayout->addWidget(yrangeScroll_, 0, 1);

  connect(xrangeScroll_, SIGNAL(windowChanged()), this, SLOT(rangeScrollSlot()));
  connect(yrangeScroll_, SIGNAL(windowChanged()), this, SLOT(rangeScrollSlot()));

  xrangeScroll_->setVisible(false);
  yrangeScroll_->setVisible(false);

  //---

  tableFrame_ = CQUtil::makeWidget<QFrame>(this, "tableFrame");

  auto *tableLayout = CQUtil::makeLayout<QVBoxLayout>(tableFrame_, 2, 2);

  tableFrame_->setVisible(false);

  //---

  // frame for model selector
  modelSelFrame_ = CQUtil::makeWidget<QFrame>(this, "modelSelFrame");
  auto *modelSelLayout = CQUtil::makeLayout<QHBoxLayout>(modelSelFrame_, 2, 2);

  modelSelLayout->addStretch(1);

  modelSelLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Models", "label"));

  stackCombo_ = CQUtil::makeWidget<QComboBox>(this, "stackCombo");

  connect(stackCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(stackSlot(int)));

  modelSelLayout->addWidget(stackCombo_);

  tableLayout->addWidget(modelSelFrame_);

  //---

  // create model view
  tableStack_ = CQUtil::makeWidget<QStackedWidget>(this, "tableStack");

  tableStack_->setAutoFillBackground(true);

  addModelView(/*current*/true);

  setDataTable(showDataTable_, /*force*/true);

  tableLayout->addWidget(tableStack_);

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

CQChartsView *
CQChartsWindow::
view() const
{
  return view_.data();
}

void
CQChartsWindow::
resizeEvent(QResizeEvent *)
{
}

void
CQChartsWindow::
addModelView(bool current)
{
  connectModelViewExpand(false);

  //---

  TableData tableData;

  tableData.isCurrent  = current;
  tableData.tableFrame = CQUtil::makeWidget<QFrame>("tableFrame");

  auto *tableLayout = CQUtil::makeLayout<QVBoxLayout>(tableData.tableFrame, 0, 2);

  tableData.filterEdit = new CQChartsFilterEdit;

  connect(tableData.filterEdit, SIGNAL(filterAnd(bool)), this, SLOT(filterAndSlot(bool)));

  connect(tableData.filterEdit, SIGNAL(replaceFilter(const QString &)),
          this, SLOT(replaceFilterSlot(const QString &)));
  connect(tableData.filterEdit, SIGNAL(addFilter(const QString &)),
          this, SLOT(addFilterSlot(const QString &)));

  connect(tableData.filterEdit, SIGNAL(replaceSearch(const QString &)),
          this, SLOT(replaceSearchSlot(const QString &)));
  connect(tableData.filterEdit, SIGNAL(addSearch(const QString &)),
          this, SLOT(addSearchSlot(const QString &)));

  tableLayout->addWidget(tableData.filterEdit);

  tableData.modelView = new CQChartsModelViewHolder(view_->charts());

  connect(tableData.modelView, SIGNAL(filterChanged()), this, SLOT(filterChangedSlot()));

  tableDatas_.push_back(tableData);

  tableLayout->addWidget(tableData.modelView);

  tableStack_->addWidget(tableData.tableFrame);

  //---

  connectModelViewExpand(true);
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
  if (! plot_) return;

  CQUtil::AutoDisconnect xscrollDisconnect(
    xrangeScroll_, SIGNAL(windowChanged()), this, SLOT(rangeScrollSlot()));
  CQUtil::AutoDisconnect yscrollDisconnect(
    yrangeScroll_, SIGNAL(windowChanged()), this, SLOT(rangeScrollSlot()));

  // get fill range and current range
  auto bbox1 = plot_->getDataRange ();
  auto bbox2 = plot_->calcDataRange();

  double xsize = bbox2.getWidth ()/bbox1.getWidth ();
  double xpos  = (bbox2.getXMin() - bbox1.getXMin())/bbox1.getWidth();
  double ysize = bbox2.getHeight()/bbox1.getHeight();
  double ypos  = (bbox2.getYMin() - bbox1.getYMin())/bbox1.getHeight();

  if (xsize != xrangeScroll_->len() || xpos != xrangeScroll_->pos()) {
    xrangeScroll_->setLen(xsize);
    xrangeScroll_->setPos(xpos);
  }

  if (ysize != yrangeScroll_->len() || ypos != yrangeScroll_->pos()) {
    yrangeScroll_->setLen(ysize);
    yrangeScroll_->setPos(ypos);
  }
}

void
CQChartsWindow::
setDataTable(bool b, bool force)
{
  if (force || b != showDataTable_) {
    if      (showDataTable_ && ! b) {
      viewSplitter_->removeWidget(tableFrame_, /*destroy*/false);
      //tableStack_->setVisible(false);
    }
    else if (! showDataTable_ && b) {
      viewSplitter_->addWidget(tableFrame_, "Table");
      //tableStack_->setVisible(true);
    }

    showDataTable_ = b;

    plotSlot();
  }
}

//---

void
CQChartsWindow::
setViewSettings(bool b)
{
  viewSettingsData_.visible = b;

  if (settings_)
    settings_->setVisible(viewSettingsData_.visible);
}

void
CQChartsWindow::
setViewSettingsMajorObjects(bool b)
{
  viewSettingsData_.majorObjects = b;

  if (settings_)
    settings_->updatePlotObjects();
}

void
CQChartsWindow::
setViewSettingsMinorObjects(bool b)
{
  viewSettingsData_.minorObjects = b;

  if (settings_)
    settings_->updatePlotObjects();
}

void
CQChartsWindow::
setViewSettingsMaxObjects(int n)
{
  viewSettingsData_.maxObjects = n;

  if (settings_)
    settings_->updatePlotObjects();
}

//---

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
  if (! plot_) return;

  auto dataRange = plot_->getDataRange();

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

  plot_->zoomTo(dataRange);
}

void
CQChartsWindow::
stackSlot(int ind)
{
  tableStack_->setCurrentIndex(ind);
}

void
CQChartsWindow::
filterAndSlot(bool b)
{
  auto *filterEdit = qobject_cast<FilterEdit *>(sender());
  if (! filterEdit) return;

  for (const auto &tableData : tableDatas_) {
    if (tableData.filterEdit == filterEdit) {
      tableData.modelView->setFilterAnd(b);

      auto details = tableData.modelView->filterDetails();

      filterEdit->setFilterDetails(details);
    }
  }
}

void
CQChartsWindow::
replaceFilterSlot(const QString &text)
{
  auto *filterEdit = qobject_cast<FilterEdit *>(sender());
  if (! filterEdit) return;

  for (const auto &tableData : tableDatas_) {
    if (tableData.filterEdit == filterEdit) {
      tableData.modelView->setFilter(text);

      auto details = tableData.modelView->filterDetails();

      filterEdit->setFilterDetails(details);
    }
  }
}

void
CQChartsWindow::
addFilterSlot(const QString &text)
{
  auto *filterEdit = qobject_cast<FilterEdit *>(sender());
  if (! filterEdit) return;

  for (const auto &tableData : tableDatas_) {
    if (tableData.filterEdit == filterEdit) {
      tableData.modelView->addFilter(text);

      auto details = tableData.modelView->filterDetails();

      filterEdit->setFilterDetails(details);
    }
  }
}

void
CQChartsWindow::
replaceSearchSlot(const QString &text)
{
  auto *filterEdit = qobject_cast<FilterEdit *>(sender());
  if (! filterEdit) return;

  for (const auto &tableData : tableDatas_) {
    if (tableData.filterEdit == filterEdit) {
      tableData.modelView->setSearch(text);
    }
  }
}

void
CQChartsWindow::
addSearchSlot(const QString &text)
{
  auto *filterEdit = qobject_cast<FilterEdit *>(sender());
  if (! filterEdit) return;

  for (const auto &tableData : tableDatas_) {
    if (tableData.filterEdit == filterEdit) {
      tableData.modelView->addSearch(text);
    }
  }
}

void
CQChartsWindow::
filterChangedSlot()
{
  //if (! plot_) return;

  //plot_->updateRangeAndObjs();
}

//------

void
CQChartsWindow::
updateOverview()
{
  xrangeScroll_->setFixedHeight(view_->overviewXSize());
  yrangeScroll_->setFixedWidth (view_->overviewYSize());

  xrangeScroll_->setRangeColor(view_->overviewRangeColor());
  xrangeScroll_->setRangeAlpha(view_->overviewRangeAlpha());

  yrangeScroll_->setRangeColor(view_->overviewRangeColor());
  yrangeScroll_->setRangeAlpha(view_->overviewRangeAlpha());
}

//------

void
CQChartsWindow::
removeViewSlot(View *view)
{
  if (view_ != view)
    return;

  view_->setParent(nullptr);

  view_ = ViewP();

  deleteLater();
}

void
CQChartsWindow::
plotSlot()
{
  if (plot_)
    disconnect(plot_, SIGNAL(currentModelChanged()), this, SLOT(plotModelSlot()));

  //---

  plot_ = view_->currentPlot(/*remap*/false);

  // set title
  if (plot_)
    setWindowTitle(QString("Window: View %1, Plot %2").arg(view_->id()).arg(plot_->id()));
  else
    setWindowTitle(QString("Window: View %1, Plot <none>").arg(view_->id()));

  if (tableStack_->isVisible()) {
    auto numExtra = plot_->extraModels().size();

    while (tableDatas_.size() > numExtra + 1) {
      delete tableDatas_.back().tableFrame;

      tableDatas_.pop_back();
    }

    while (tableDatas_.size() < numExtra + 1) {
      addModelView(/*current*/false);
    }

    setViewModel();
  }
  else {
    while (tableDatas_.size() > 1) {
      delete tableDatas_.back().tableFrame;

      tableDatas_.pop_back();
    }

    for (const auto &tableData : tableDatas_)
      tableData.modelView->setModel(CQChartsModelViewHolder::ModelP(), false);
  }

  //---

  disconnect(stackCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(stackSlot(int)));

  stackCombo_->clear();

  int i = 0;

  for (const auto &tableData : tableDatas_) {
    if (tableData.isCurrent)
      stackCombo_->addItem("Current");
    else
      stackCombo_->addItem(QString("Extra %1").arg(i));

    ++i;
  }

  connect(stackCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(stackSlot(int)));

  modelSelFrame_->setVisible(tableDatas_.size() > 1);

  //---

  if (plot_)
    connect(plot_, SIGNAL(currentModelChanged()), this, SLOT(plotModelSlot()));
}

void
CQChartsWindow::
plotModelSlot()
{
  setViewModel();
}

void
CQChartsWindow::
setViewModel()
{
  if (plot_) {
    using Models = std::vector<CQChartsModelViewHolder::ModelP>;

    Models models;

    models.push_back(plot_->currentModel());

    for (const auto &model : plot_->extraModels())
      models.push_back(model);

    assert(models.size() == tableDatas_.size());

    for (uint i = 0; i < models.size(); ++i) {
      auto &model     = models[i];
      auto &tableData = tableDatas_[i];

      if (model != tableData.modelView->model()) {
        if (i == 0)
          tableData.modelView->setModel(model, plot_->isHierarchical());
        else
          tableData.modelView->setModel(model, /*isHierarchical*/ false);
      }
    }
  }
  else {
    for (const auto &tableData : tableDatas_) {
      if (tableData.isCurrent)
        tableData.modelView->setModel(CQChartsModelViewHolder::ModelP(), false);
    }
  }
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
  Q_EMIT themePalettesChanged();
}

void
CQChartsWindow::
selectPropertyObjects()
{
  using ObjSet = std::set<QObject *>;

  //---

  if (settings_)
    disconnect(settings_, SIGNAL(propertyItemSelected(QObject *, const QString &)),
               this, SLOT(propertyItemSelected(QObject *, const QString &)));

  //---

  // get currently selected property view objects
  CQPropertyViewTree::Objs selectedObjs;

  if (settings_)
    settings_->viewPropertyTree()->getSelectedObjects(selectedObjs);

  ObjSet selectedObjSet;

  for (auto &obj : selectedObjs)
    selectedObjSet.insert(obj);

  View::Plots plots;

  view_->getPlots(plots);

  for (auto &plot : plots) {
    CQPropertyViewTree::Objs selectedObjs1;

    if (settings_ && settings_->plotPropertyTree(plot))
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
    if (settings_) {
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

  //---

  if (settings_)
    connect(settings_, SIGNAL(propertyItemSelected(QObject *, const QString &)),
            this, SLOT(propertyItemSelected(QObject *, const QString &)));
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
  for (const auto &tableData : tableDatas_) {
    if (tableData.isCurrent) {
      CQUtil::connectDisconnect(connect,
        tableData.modelView->view(), SIGNAL(expanded(const QModelIndex &)),
        this, SLOT(expansionChangeSlot()));
      CQUtil::connectDisconnect(connect,
        tableData.modelView->view(), SIGNAL(collapsed(const QModelIndex &)),
        this, SLOT(expansionChangeSlot()));
    }
  }
#endif
}

void
CQChartsWindow::
expansionChangeSlot()
{
  Q_EMIT expansionChanged();
}

bool
CQChartsWindow::
isExpandModelIndex(const QModelIndex &ind) const
{
  const_cast<CQChartsWindow *>(this)->setViewModel();

#ifdef CQCHARTS_MODEL_VIEW
  for (const auto &tableData : tableDatas_) {
    if (tableData.isCurrent)
      return tableData.modelView->view()->isExpanded(ind);
  }

  return false;
#endif
}

void
CQChartsWindow::
expandModelIndex(const QModelIndex &ind, bool b)
{
  setViewModel();

#ifdef CQCHARTS_MODEL_VIEW
  connectModelViewExpand(false);

  for (const auto &tableData : tableDatas_) {
    if (tableData.isCurrent)
      tableData.modelView->view()->setExpanded(ind, b);
  }

  connectModelViewExpand(true);
#endif
}

void
CQChartsWindow::
expandedModelIndices(QModelIndexList &inds)
{
  setViewModel();

#ifdef CQCHARTS_MODEL_VIEW
  for (const auto &tableData : tableDatas_) {
    if (tableData.isCurrent)
      tableData.modelView->view()->expandedIndices(inds);
  }
#endif
}

//---

void
CQChartsWindow::
showErrorsTab()
{
  if (settings_)
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

  int w = fm.horizontalAdvance("X")*200;
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

  auto *view = window_->view();

  plot->setOverview(true);

  auto zoomData    = plot->zoomData();
  auto outerMargin = plot->outerMargin();

  view->doResize(p->device()->width(), p->device()->height());

  auto margin = CQChartsLength::pixel(this->margin());

  plot->setOuterMargin(CQChartsPlotMargin(margin, CQChartsLength(), margin, CQChartsLength()));

  plot->zoomFull(/*notify*/false);

  plot->draw(p);

  view->doResize(view->width(), view->height());

  plot->setOuterMargin(outerMargin);

  plot->setZoomData(zoomData);

  plot->setOverview(false);
}
