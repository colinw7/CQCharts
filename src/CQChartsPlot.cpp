#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsMapKey.h>
#include <CQChartsTitle.h>
#include <CQChartsPlotObj.h>
#include <CQChartsPlotObjTree.h>
#include <CQChartsNoDataObj.h>
#include <CQChartsAnnotation.h>
#include <CQChartsValueSet.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsModelExprMatch.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsPlotParameter.h>
#include <CQChartsColumnType.h>
#include <CQChartsModelUtil.h>
#include <CQChartsEditHandles.h>
#include <CQChartsVariant.h>
#include <CQChartsTip.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQChartsSVGPaintDevice.h>
#include <CQChartsStatsPaintDevice.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsSymbolSet.h>
#include <CQChartsSymbolBuffer.h>
#include <CQChartsHtml.h>
#include <CQChartsEnv.h>
#include <CQChartsJS.h>
#include <CQCharts.h>

#include <CQChartsPlotControlWidgets.h>
#include <CQChartsModelViewHolder.h>
#include <CQChartsModelDetailsTable.h>
#include <CQChartsModelDetailsWidget.h>
#include <CQChartsPlotPropertyEdit.h>
#include <CQChartsModelColumnDataControl.h>
#include <CQChartsModelExprControl.h>
#include <CQChartsModelControl.h>
#ifdef CQCHARTS_MODULE_SHLIB
#include <CQChartsModuleData.h>
#endif

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColors.h>
#include <CQColorsTheme.h>
#include <CQColorsPalette.h>
#include <CQBaseModel.h>
#include <CQThreadObject.h>
#include <CQPerfMonitor.h>
#include <CQUtil.h>
#include <CQTclUtil.h>

#include <CMathUtil.h>
#include <CMathRound.h>

#ifdef CQCHARTS_MODULE_SHLIB
#define OS_UNIX 1
#include <CShLib.h>
#endif

#include <QApplication>
#include <QItemSelectionModel>
#include <QAbstractProxyModel>
#include <QTextBrowser>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>
#include <QMenu>
#include <QAction>
#include <QScrollBar>

//------

CQChartsPlot::
CQChartsPlot(View *view, PlotType *type, const ModelP &model) :
 CQChartsObj(view->charts()),
 CQChartsObjPlotShapeData<CQChartsPlot>(this),
 CQChartsObjDataShapeData<CQChartsPlot>(this),
 CQChartsObjFitShapeData <CQChartsPlot>(this),
 view_(view), type_(type)
{
  models_.push_back(model);

  updateModelInds();

  CQChartsObjPlotShapeData<CQChartsPlot>::setPlotReloadObj(false);
  CQChartsObjDataShapeData<CQChartsPlot>::setDataReloadObj(false);
  CQChartsObjFitShapeData <CQChartsPlot>::setFitReloadObj(false);
}

CQChartsPlot::
~CQChartsPlot()
{
  CQChartsPlot::term();
}

//---

void
CQChartsPlot::
init()
{
  NoUpdate noUpdate(this);

  //---

  assert(! propertyModel_);

  propertyModel_ = std::make_unique<CQPropertyViewModel>();

  connect(propertyModel_.get(), SIGNAL(valueChanged(QObject *, const QString &)),
          this, SLOT(propertyItemChanged(QObject *, const QString &)));

  //---

  preview_       = CQChartsEnv::getInt ("CQ_CHARTS_PLOT_PREVIEW"  , preview_);
  sequential_    = CQChartsEnv::getBool("CQ_CHARTS_SEQUENTIAL"    , sequential_); // TODO: remove
  queueUpdate_   = CQChartsEnv::getBool("CQ_CHARTS_PLOT_QUEUE"    , queueUpdate_);
  bufferSymbols_ = CQChartsEnv::getInt ("CQ_CHARTS_BUFFER_SYMBOLS", bufferSymbols_);

  displayRange_    = std::make_unique<DisplayRange>();
  rawDisplayRange_ = std::make_unique<DisplayRange>();

  displayRange_   ->setPixelAdjust(0.0);
  rawDisplayRange_->setPixelAdjust(0.0);

  //---

  bool objTreeWait = CQChartsEnv::getBool("CQ_CHARTS_OBJ_TREE_WAIT", false);

  objTreeData_.tree = std::make_unique<CQChartsPlotObjTree>(this, objTreeWait);

  //---

  animateData_.tickLen = CQChartsEnv::getInt("CQ_CHARTS_TICK_LEN", animateData_.tickLen);

  debugUpdate_   = CQChartsEnv::getBool("CQ_CHARTS_DEBUG_UPDATE"   , debugUpdate_  );
  debugQuadTree_ = CQChartsEnv::getBool("CQ_CHARTS_DEBUG_QUAD_TREE", debugQuadTree_);

  //--

  // plot, data, fit background
  setPlotFilled(true ); setPlotStroked(false);
  setDataFilled(true ); setDataStroked(false);
  setFitFilled (false); setFitStroked (false);

  setPlotFillColor(Color::makeInterfaceValue(0.00));
  setDataFillColor(Color::makeInterfaceValue(0.12)); // #e5ecf6 (BLEND_INTERFACE)
  setFitFillColor (Color::makeInterfaceValue(0.08));

  //--

  double vr = View::viewportRange();

  viewBBox_      = BBox(0, 0, vr, vr);
  innerViewBBox_ = viewBBox_;

  innerMargin_ = PlotMargin(Length::plot(0), Length::plot(0), Length::plot(0), Length::plot(0));
  outerMargin_ = PlotMargin(Length::percent(10), Length::percent(10),
                            Length::percent(10), Length::percent(10));
  fitMargin_   = PlotMargin(Length::percent(1), Length::percent(1),
                            Length::percent(1), Length::percent(1));

  setPixelRange(BBox(0.0, 0.0,  vr,  vr));

  resetWindowRange();

  //---

  // all layers active except BG_PLOT and FG_PLOT
  initLayer(Layer::Type::BACKGROUND   , Buffer::Type::BACKGROUND, true);
  initLayer(Layer::Type::BG_AXES      , Buffer::Type::BACKGROUND, true);
  initLayer(Layer::Type::BG_KEY       , Buffer::Type::BACKGROUND, true);
  initLayer(Layer::Type::BG_ANNOTATION, Buffer::Type::BACKGROUND, true);

  initLayer(Layer::Type::BG_PLOT , Buffer::Type::MIDDLE, false);
  initLayer(Layer::Type::MID_PLOT, Buffer::Type::MIDDLE, true );
  initLayer(Layer::Type::FG_PLOT , Buffer::Type::MIDDLE, false);

  initLayer(Layer::Type::FG_AXES      , Buffer::Type::FOREGROUND, true);
  initLayer(Layer::Type::FG_KEY       , Buffer::Type::FOREGROUND, true);
  initLayer(Layer::Type::FG_ANNOTATION, Buffer::Type::FOREGROUND, true);
  initLayer(Layer::Type::TITLE        , Buffer::Type::FOREGROUND, true);
  initLayer(Layer::Type::FOREGROUND   , Buffer::Type::FOREGROUND, true);

  initLayer(Layer::Type::EDIT_HANDLE, Buffer::Type::OVERLAY, true);
  initLayer(Layer::Type::BOXES      , Buffer::Type::OVERLAY, true);
  initLayer(Layer::Type::SELECTION  , Buffer::Type::OVERLAY, true);
  initLayer(Layer::Type::MOUSE_OVER , Buffer::Type::OVERLAY, true);

  //---

  connectModels();

  //---

  {
  std::unique_lock<std::mutex> lock(updatesMutex_);

  ++updatesData_.stateFlag[UpdateState::UPDATE_RANGE    ];
  ++updatesData_.stateFlag[UpdateState::UPDATE_OBJS     ];
  ++updatesData_.stateFlag[UpdateState::UPDATE_DRAW_OBJS];

  updateData_.rangeThread = std::make_unique<ThreadObj>("updateRange");
  updateData_.objsThread  = std::make_unique<ThreadObj>("updateObjs" );
  updateData_.drawThread  = std::make_unique<ThreadObj>("drawObjs"   );

  updateData_.rangeThread->setDebug(debugUpdate_);
  updateData_.objsThread ->setDebug(debugUpdate_);
  updateData_.drawThread ->setDebug(debugUpdate_);
  }

  //---

  registerSlot("zoom_full", QStringList());
  registerSlot("fit"      , QStringList());
  registerSlot("fit_data" , QStringList());

  //---

  createScrollBars();

  startThreadTimer();

  //---

  if (isAutoFit())
    setNeedsAutoFit(true);
}

void
CQChartsPlot::
term()
{
  clearPlotObjects1();

  for (auto &layer : layers_)
    delete layer.second;

  for (auto &buffer : buffers_)
    delete buffer.second;

  for (auto &annotation : annotations())
    delete annotation;

  deleteScrollBars();
}

//---

void
CQChartsPlot::
createScrollBars()
{
  scrollData_.hbar = new QScrollBar(Qt::Horizontal, view());
  scrollData_.hbar->setObjectName("plotHBar");
  scrollData_.hbar->setVisible(false);

  connect(scrollData_.hbar, SIGNAL(valueChanged(int)), this, SLOT(hbarScrollSlot(int)));

  scrollData_.vbar = new QScrollBar(Qt::Vertical, view());
  scrollData_.vbar->setObjectName("plotVBar");
  scrollData_.vbar->setVisible(false);

  connect(scrollData_.vbar, SIGNAL(valueChanged(int)), this, SLOT(vbarScrollSlot(int)));

  scrollData_.invalid = true;
}

void
CQChartsPlot::
deleteScrollBars()
{
  delete scrollData_.hbar;
  delete scrollData_.vbar;
}

//---

CQCharts *
CQChartsPlot::
charts() const
{
  return view()->charts();
}

//---

void
CQChartsPlot::
setView(View *view)
{
  assert(view != view_);

  deleteScrollBars();

  view_ = view;

  createScrollBars();
}

QString
CQChartsPlot::
viewId() const
{
  return view()->id();
}

void
CQChartsPlot::
emitParentViewChanged()
{
  for (auto &annotation : annotations())
    annotation->parentViewChanged();

  Q_EMIT parentViewChanged();
}

//---

QString
CQChartsPlot::
typeName() const
{
  return type()->name();
}

QString
CQChartsPlot::
calcName() const
{
  auto name = this->name();

  if (! name.length())
    name = this->id();

  return name;
}

QString
CQChartsPlot::
pathId() const
{
  return view()->id() + "|" + id();
}

//---

void
CQChartsPlot::
startThreadTimer()
{
  if (view()->is3D())
    return;

  if (isSequential())
    return;

  if (isOverlay() && ! isFirstPlot())
    return;

  if (parentPlot())
    return;

  if (view()->is3D())
    return;

  //---

  assert(! parentPlot());

  if (! updateData_.timer) {
    updateData_.timer = new QTimer(this);

    connect(updateData_.timer, SIGNAL(timeout()), this, SLOT(threadTimerSlot()));
  }

  if (! updateData_.timer->isActive())
    updateData_.timer->start(updateData_.timeout);
}

void
CQChartsPlot::
stopThreadTimer()
{
  assert(! parentPlot());

  if (updateData_.timer) {
    if (updateData_.timer->isActive())
      updateData_.timer->stop();
  }
}

//---

CQChartsPlot::ModelP
CQChartsPlot::
currentModel() const
{
  for (auto &model : models_) {
    auto *modelData = getModelData(model);

    if (modelData && modelData->ind() == currentModelIndex())
      return model;
  }

  if (! models_.empty())
    return *models_.begin();

  return ModelP();
}

CQChartsModelData *
CQChartsPlot::
currentModelData() const
{
  const auto &model = currentModel();

  return getModelData(model);
}

int
CQChartsPlot::
currentModelIndex() const
{
  return currentModelInd().modelInd();
}

const CQChartsModelInd &
CQChartsPlot::
currentModelInd() const
{
  return currentModelInd_;
}

void
CQChartsPlot::
setCurrentModelInd(const CQChartsModelInd &ind)
{
  if (ind.modelInd() != currentModelIndex()) {
    currentModelInd_ = ind;

    updateModelInds();

    updateRangeAndObjs();

    Q_EMIT currentModelChanged();
  }
}

const CQChartsValueList &
CQChartsPlot::
modelInds() const
{
  return modelInds_;
}

void
CQChartsPlot::
setModelInds(const CQChartsValueList &inds)
{
  if (inds != modelInds_) {
    auto modelInd = CQChartsModelInd::fromVariant(inds.value(inds.currentInd()));

    modelInd.setCharts(charts());

    setCurrentModelInd(modelInd);
  }
}

void
CQChartsPlot::
updateModelInds()
{
  modelInds_.clear();

  int currentInd = -1;

  if (! models_.empty()) {
    int ind = 0;

    for (const auto &model : models_) {
      auto *modelData = getModelData(model);
      if (! modelData) continue;

      CQChartsModelInd modelInd(charts(), modelData->ind());

      modelInds_.addValue(CQChartsModelInd::toVariant(modelInd));

      if (modelData->ind() == currentModelIndex())
        currentInd = ind;

      ++ind;
    }
  }

  modelInds_.setCurrentInd(currentInd);

  //---

  CQChartsModelInd ind;

  ind.setCharts(charts());

  auto *modelData = currentModelData();

  ind.setModelInd(modelData ? modelData->ind() : -1);

  if (ind != currentModelInd()) {
    currentModelInd_ = ind;

    modelInds_.setCurrentInd(currentModelIndex());

    updateRangeAndObjs();

    Q_EMIT currentModelChanged();
  }
}

//---

void
CQChartsPlot::
addModel(const ModelP &model)
{
  disconnectModels();

  addModelI(model);

  connectModels();

  updateModelInds();

  updateRangeAndObjs();

  Q_EMIT modelChanged();
}

void
CQChartsPlot::
replaceModel(const ModelP &oldModel, const ModelP &newModel)
{
  disconnectModels();

  int ind = 0;

  for (size_t i = 0; i < models_.size(); ++i) {
    if (models_[i] == oldModel) {
      ind = i;
      break;
    }
  }

  for (size_t i = ind + 1; i < models_.size(); ++i)
    models_[i - 1] = models_[i];

  models_.pop_back();

  addModelI(newModel);

  connectModels();

  updateModelInds();

  updateRangeAndObjs();

  Q_EMIT modelChanged();
}

void
CQChartsPlot::
addModelI(const ModelP &model)
{
  models_.push_back(model);
}

//---

void
CQChartsPlot::
addExtraModel(const ModelData *modelData)
{
  int ind;

  if (! modelData || hasExtraModel(modelData, ind))
    return;

  extraModels_.push_back(modelData->model());
}

void
CQChartsPlot::
removeExtraModel(const ModelData *modelData)
{
  int ind;

  if (! modelData || ! hasExtraModel(modelData, ind))
    return;

  for (uint i = ind + 1; i < extraModels_.size(); ++i)
    extraModels_[i - 1] = extraModels_[i];

  extraModels_.pop_back();
}

bool
CQChartsPlot::
hasExtraModel(const ModelData *modelData, int &ind) const
{
  if (! modelData) return false;

  ind = 0;

  for (const auto &model : extraModels_) {
    if (modelData->model() == model)
      return true;

    ++ind;
  }

  ind = -1;

  return false;
}

//---

void
CQChartsPlot::
connectModels()
{
  connectDisconnectModels(true);
}

void
CQChartsPlot::
disconnectModels()
{
  connectDisconnectModels(false);
}

void
CQChartsPlot::
connectDisconnectModels(bool isConnect)
{
  for (auto &model : models_)
    connectDisconnectModel(model, isConnect);
}

void
CQChartsPlot::
connectDisconnectModel(const ModelP &model, bool isConnect)
{
  if (! model.data())
    return;

  auto *modelData = getModelData(model);
  if (! modelData) return;

  //---

  auto connectDisconnect = [&](bool b, QObject *obj, const char *from, const char *to) {
    CQUtil::connectDisconnect(b, obj, from, this, to);
  };

  //---

  auto setModelName = [&]() {
    if (! modelData->name().length() && this->hasId()) {
      auto modelName = QString("%1:%2").arg(this->id()).arg(modelData->ind());

      modelData->setName(modelName);

      modelData->setPlotName(true);
    }
  };

  auto unsetModelName = [&]()  {
    if (modelData->isPlotName()) {
      modelData->setName("");

      modelData->setPlotName(false);
    }
  };

  if (modelData) {
    if (isConnect)
      setModelName();
    else
      unsetModelName();

    connectDisconnect(isConnect, modelData, SIGNAL(deleted()),
                      SLOT(modelDeletedSlot()));

    connectDisconnect(isConnect, modelData, SIGNAL(modelChanged()),
                      SLOT(modelChangedSlot()));

    connectDisconnect(isConnect, modelData, SIGNAL(currentModelChanged()),
                      SLOT(currentModelChangedSlot()));

    connectDisconnect(isConnect, modelData, SIGNAL(selectionChanged(QItemSelectionModel *)),
                      SLOT(selectionSlot(QItemSelectionModel *)));
  }
  else {
    // TODO: on connect, check if model uses changed columns
    //int column1 = tl.column();
    //int column2 = br.column();

    connectDisconnect(isConnect,
                      model.data(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
                      SLOT(modelChangedSlot()));

    connectDisconnect(isConnect, model.data(), SIGNAL(layoutChanged()),
                      SLOT(modelChangedSlot()));
    connectDisconnect(isConnect, model.data(), SIGNAL(modelReset()),
                      SLOT(modelChangedSlot()));

    connectDisconnect(isConnect, model.data(), SIGNAL(rowsInserted(QModelIndex, int, int)),
                      SLOT(modelChangedSlot()));
    connectDisconnect(isConnect, model.data(), SIGNAL(rowsRemoved(QModelIndex, int, int)),
                      SLOT(modelChangedSlot()));
    connectDisconnect(isConnect, model.data(), SIGNAL(columnsInserted(QModelIndex, int, int)),
                      SLOT(modelChangedSlot()));
    connectDisconnect(isConnect, model.data(), SIGNAL(columnsRemoved(QModelIndex, int, int)),
                      SLOT(modelChangedSlot()));
  }
}

//---

void
CQChartsPlot::
setAnimating(bool b)
{
  if (b != animateData_.running) {
    setAnimating1(b);

    Q_EMIT animateStateChanged(isAnimating());
  }
}

void
CQChartsPlot::
setAnimating1(bool b)
{
  animateData_.running = b;

  if (! isAnimating())
    stopAnimateTimer();
  else
    startAnimateTimer();
}

void
CQChartsPlot::
setAnimTick(int i)
{
  if (i != animateData_.tickLen) {
    animateData_.tickLen = i;

    if (isAnimating()) {
      stopAnimateTimer();
      startAnimateTimer();
    }
  }
}

void
CQChartsPlot::
startAnimateTimer()
{
  if (! animateData_.timer) {
    animateData_.timer = new QTimer(this);

    animateData_.timer->moveToThread(this->thread());

    connect(animateData_.timer, SIGNAL(timeout()), this, SLOT(animateSlot()));
  }

  animateData_.timer->start(animTick());
}

void
CQChartsPlot::
stopAnimateTimer()
{
  if (animateData_.timer) {
    animateData_.timer->moveToThread(this->thread());

    animateData_.timer->stop();

    //delete animateData_.timer; animateData_.timer = nullptr;
  }
}

void
CQChartsPlot::
animateSlot()
{
  // interupt range, calc objs and draw
  interruptRange();

  animateStep();
}

void
CQChartsPlot::
animateStep()
{
  Q_EMIT updateAnimData();

  for (auto &annotation : annotations())
    annotation->animateStep();
}

//---

void
CQChartsPlot::
modelDeletedSlot()
{
  auto *modelData = qobject_cast<CQChartsModelData *>(sender());
  if (! modelData) return;

  auto pm = std::find(models_.begin(), models_.end(), modelData->model());

  if (pm != models_.end()) {
    models_.erase(pm);

    updateModelInds();

    updateRangeAndObjs();

    Q_EMIT modelChanged();
  }
}

void
CQChartsPlot::
modelChangedSlot()
{
  updateRangeAndObjs();
}

void
CQChartsPlot::
currentModelChangedSlot()
{
  auto *modelData = qobject_cast<CQChartsModelData *>(sender());
  if (! modelData) return;

  replaceModel(currentModel(), modelData->currentModel());
}

//---

void
CQChartsPlot::
selectionSlot(QItemSelectionModel *sm)
{
  startSelection();

  // deselect all plot objects
  (void) deselectAllPlotObjs1();

  // select objects with matching indices
  // get selected (normalized) indices from selection model
  // TODO: optimize for row select or column select ?
  PlotObj::Indices selectIndices;

  getSelectIndices(sm, selectIndices);

  //---

  for (auto &plotObj : plotObjects()) {
    if (! plotObj->isSelectable())
      continue;

    if (plotObj->isSelectIndices(selectIndices))
      plotObj->setSelected(true);
  }

  endSelection();

  //---

  invalidateOverlay();

  if (selectInvalidateObjs())
    drawObjs();
}

void
CQChartsPlot::
selectObjsFromModel()
{
  startSelection();

  // deselect all plot objects
  bool changed = deselectAllPlotObjs1();

  auto *modelData = currentModelData();
  if (! modelData) return;

  const auto &sel = modelData->selection();

  auto indices = sel.indexes();

  if (! indices.empty()) {
    PlotObj::Indices selectIndices;

    for (int i = 0; i < indices.size(); ++i) {
      const auto &ind = indices[i];

      auto ind1 = normalizeIndex(ind);

      selectIndices.insert(ind1);
    }

    //---

    // select objects with matching indices
    for (auto &plotObj : plotObjects()) {
      if (! plotObj->isSelectable())
        continue;

      if (plotObj->isAllSelectIndices(selectIndices)) {
        plotObj->setNotificationsEnabled(false);

        if (! plotObj->isSelected()) {
          plotObj->setSelected(true);
          changed = true;
        }

        plotObj->setNotificationsEnabled(true);
      }
    }
  }

  endSelection(changed);
}

void
CQChartsPlot::
getSelectIndices(QItemSelectionModel *sm, QModelIndexSet &selectIndices)
{
  auto indices = sm->selectedIndexes();
  if (indices.empty()) return;

  for (int i = 0; i < indices.size(); ++i) {
    const auto &ind = indices[i];

    auto ind1 = normalizeIndex(ind);

    selectIndices.insert(ind1);
  }
}

//---

bool
CQChartsPlot::
calcVisible() const
{
  if (parentPlot())
    return false;

  if (isTabbed() && ! isCurrent())
    return false;

  //---

  return isVisible();
}

void
CQChartsPlot::
setVisible(bool b)
{
  CQChartsUtil::testAndSet(visible_, b, [&]() {
    if (! isVisible()) {
      if (view()->currentPlot(/*remap*/false) == this)
        view()->setCurrentPlot(nullptr);
    }
    else {
      if (! view()->currentPlot())
        view()->setCurrentPlot(this);
    }

    if (isVisible() || isOverlay())
      updateRangeAndObjs();

    if (isOverlay(/*checkVisible*/false))
      view()->initOverlayAxes();
  } );
}

void
CQChartsPlot::
setSelected(bool b)
{
  CQChartsUtil::testAndSet(selected_, b, [&]() { drawObjs(); } );
}

//---

bool
CQChartsPlot::
isUpdatesEnabled() const
{
  if (parentPlot())
    return parentPlot()->isUpdatesEnabled();

  assert(! parentPlot());

  return updatesData_.enabled == 0;
}

void
CQChartsPlot::
setUpdatesEnabled(bool b, bool update)
{
  if (parentPlot())
    return parentPlot()->setUpdatesEnabled(b, update);

  setUpdatesEnabled1(b, update);
}

void
CQChartsPlot::
setUpdatesEnabled1(bool b, bool update)
{
  bool doUpdate = false;

  assert(! parentPlot());

  {
  std::unique_lock<std::mutex> lock(updatesMutex_);

  if (b) {
    assert(updatesData_.enabled > 0);

    --updatesData_.enabled;

    if (updatesData_.enabled == 0) {
      if (update)
        doUpdate = true;
    }
  }
  else {
    if (updatesData_.enabled == 0) {
      updatesData_.reset();
    }

    ++updatesData_.enabled;
  }
  }

  //---

  if (doUpdate) {
    // calc range and objs
    if      (isUpdateRangeAndObjs()) {
      updateRangeAndObjs();

      drawObjs();
    }
    // calc objs
    else if (isUpdateObjs()) {
      updateObjs();

      drawObjs();
    }
    // apply range
    else if (isApplyDataRange()) {
      applyDataRangeAndDraw();
    }
    // draw objs
    else if (isInvalidateLayers()) {
      drawObjs();
    }

    updatesData_.reset();
  }
}

//---

bool
CQChartsPlot::
isUpdateRangeAndObjs() const
{
  assert(! parentPlot());

  return updatesData_.updateRangeAndObjs;
}

bool
CQChartsPlot::
isUpdateObjs() const
{
  assert(! parentPlot());

  return updatesData_.updateObjs;
}

void
CQChartsPlot::
updateRange()
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->updateRange1();

  //---

  updateRange1();
}

void
CQChartsPlot::
updateRange1()
{
  if (! isUpdatesEnabled()) {
    execUpdateRange();
    return;
  }

  //---

  if (isQueueUpdate()) {
    startUpdateRange();
  }
  else {
    execUpdateRange();
  }
}

void
CQChartsPlot::
startUpdateRange()
{
  if (parentPlot())
    return parentPlot()->startUpdateRange();

  //---

  if (debugUpdate_)
    std::cerr << "updateRange : " << id().toStdString() << "\n";

  assert(! parentPlot());

  {
  std::unique_lock<std::mutex> lock(updatesMutex_);

  ++updatesData_.stateFlag[UpdateState::UPDATE_RANGE    ];
  ++updatesData_.stateFlag[UpdateState::UPDATE_DRAW_OBJS];
  }

  startThreadTimer();
}

void
CQChartsPlot::
updateRangeAndObjs()
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->updateRangeAndObjs1();

  //---

  updateRangeAndObjs1();
}

void
CQChartsPlot::
updateRangeAndObjs1()
{
  if (! isUpdatesEnabled()) {
    execUpdateRangeAndObjs();
    return;
  }

  //---

  if (view()->is3D())
    init3D();

  if (isQueueUpdate()) {
    startUpdateRangeAndObjs();
  }
  else {
    execUpdateRangeAndObjs();
  }
}

void
CQChartsPlot::
startUpdateRangeAndObjs()
{
  if (parentPlot())
    return parentPlot()->startUpdateRangeAndObjs();

  //---

  if (debugUpdate_)
    std::cerr << "updateRangeAndObjs : " << id().toStdString() << "\n";

  assert(! parentPlot());

  {
  std::unique_lock<std::mutex> lock(updatesMutex_);

  ++updatesData_.stateFlag[UpdateState::UPDATE_RANGE    ];
  ++updatesData_.stateFlag[UpdateState::UPDATE_OBJS     ];
  ++updatesData_.stateFlag[UpdateState::UPDATE_DRAW_OBJS];
  }

  startThreadTimer();
}

void
CQChartsPlot::
updateObjsSlot()
{
  updateObjs();
}

void
CQChartsPlot::
updateObjs()
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->updateObjs1();

  if (parentPlot())
    return parentPlot()->updateObjs();

  //---

  updateObjs1();
}

void
CQChartsPlot::
updateObjs1()
{
  if (! isUpdatesEnabled()) {
    execUpdateObjs();
    return;
  }

  //---

  if (isQueueUpdate()) {
    startUpdateObjs();
  }
  else {
    execUpdateObjs();
  }
}

void
CQChartsPlot::
startUpdateObjs()
{
  if (parentPlot())
    return parentPlot()->startUpdateObjs();

  //---

  if (debugUpdate_)
    std::cerr << "updateObjs : " << id().toStdString() << "\n";

  assert(! parentPlot());

  {
  std::unique_lock<std::mutex> lock(updatesMutex_);

  ++updatesData_.stateFlag[UpdateState::UPDATE_OBJS     ];
  ++updatesData_.stateFlag[UpdateState::UPDATE_DRAW_OBJS];
  }

  startThreadTimer();
}

//------

void
CQChartsPlot::
applyVisibleFilter()
{
  if (visibleFilterStr().length()) {
    const auto &model = this->currentModel();

    auto expr = std::make_unique<CQChartsModelExprMatch>();

    expr->setModel(model.data());

    expr->initColumns();

    expr->initMatch(visibleFilterStr());

    for (auto &plotObj : plotObjects()) {
      auto ind = plotObj->modelInd();

      bool ok;

      bool visible = expr->match(ind, ok);
      if (!ok) visible = true;

      plotObj->setVisible(visible);
    }
  }
  else {
    for (auto &plotObj : plotObjects())
      plotObj->setVisible(true);
  }
}

//------

void
CQChartsPlot::
updateZoomScroll()
{
  if (isZoomScroll()) {
    if (! scrollData_.invalid)
      return;

    scrollData_.invalid = false;

    auto rawRange      = getDataRange();                // unzoomed
    auto adjustedRange = adjustDataRangeBBox(rawRange); // zoomed

    auto wf = rawRange.getWidth ();
    auto hf = rawRange.getHeight();

    auto wz = adjustedRange.getWidth ();
    auto hz = adjustedRange.getHeight();

    auto pfll = windowToPixel(rawRange.getLL());
    auto pfur = windowToPixel(rawRange.getUR());

    auto pzll = windowToPixel(adjustedRange.getLL());
    auto pzur = windowToPixel(adjustedRange.getUR());

    bool showH = (wf > wz);
    bool showV = (hf > hz);

    showScrollHBar(showH);
    showScrollVBar(showV);

    if (showH) {
      placeScrollHBar();

//    auto pwf = windowToPixelWidth(wf);
      auto pwz = windowToPixelWidth(wz);

      setHBarRange(pfll.x, pfur.x, pwz, pzll.x, 0.0, 1.0);
    }

    if (showV) {
      placeScrollVBar();

//    auto phf = windowToPixelHeight(hf);
      auto phz = windowToPixelHeight(hz);

      setVBarRange(pfur.y, pfll.y, phz, pzur.y, 0.0, 1.0);
    }
  }
  else {
    showScrollHBar(false);
    showScrollVBar(false);
  }
}

void
CQChartsPlot::
showScrollHBar(bool b)
{
  scrollData_.hShown = b;

  scrollData_.hbar->setVisible(scrollData_.hShown);
}

void
CQChartsPlot::
showScrollVBar(bool b)
{
  scrollData_.vShown = b;

  scrollData_.vbar->setVisible(scrollData_.vShown);
}

void
CQChartsPlot::
placeScrollHBar()
{
  int hh = (scrollData_.hShown ? scrollData_.hbar->sizeHint().height() : 0);
  int vw = (scrollData_.vShown ? scrollData_.vbar->sizeHint().width () : 0);

  auto pixelRect = calcPlotPixelRect();

  scrollData_.hbar->move(pixelRect.getXMin(), pixelRect.getYMax() - hh);
  scrollData_.hbar->resize(pixelRect.getWidth() - vw, hh);
}

void
CQChartsPlot::
placeScrollVBar()
{
  int hh = (scrollData_.hShown ? scrollData_.hbar->sizeHint().height() : 0);
  int vw = (scrollData_.vShown ? scrollData_.vbar->sizeHint().width () : 0);

  auto pixelRect = calcPlotPixelRect();

  scrollData_.vbar->move(pixelRect.getXMax() - vw, pixelRect.getYMin());
  scrollData_.vbar->resize(vw, pixelRect.getHeight() - hh);
}

void
CQChartsPlot::
setHBarRange(int start, int end, int step, int value, double min, double max)
{
  disconnect(scrollData_.hbar, SIGNAL(valueChanged(int)), this, SLOT(hbarScrollSlot(int)));

  scrollData_.hbar->setRange(start, end - step);
  scrollData_.hbar->setPageStep(step);
  scrollData_.hbar->setSingleStep(1);
  scrollData_.hbar->setValue(value);

  scrollData_.xvalue = value;
  scrollData_.xmin   = min;
  scrollData_.xmax   = max;

  connect(scrollData_.hbar, SIGNAL(valueChanged(int)), this, SLOT(hbarScrollSlot(int)));
}

void
CQChartsPlot::
setVBarRange(int start, int end, int step, int value, double min, double max)
{
  disconnect(scrollData_.vbar, SIGNAL(valueChanged(int)), this, SLOT(vbarScrollSlot(int)));

  scrollData_.vbar->setRange(start, end - step);
  scrollData_.vbar->setPageStep(step);
  scrollData_.vbar->setSingleStep(1);
  scrollData_.vbar->setValue(value);

  scrollData_.yvalue = value;
  scrollData_.ymin   = min;
  scrollData_.ymax   = max;

  connect(scrollData_.vbar, SIGNAL(valueChanged(int)), this, SLOT(vbarScrollSlot(int)));
}

void
CQChartsPlot::
hbarScrollSlot(int v)
{
  scrollData_.oldXValue = scrollData_.xvalue;
  scrollData_.xvalue    = v;

  double ds = double(v - scrollData_.hbar->minimum())/
              double(scrollData_.hbar->maximum() - scrollData_.hbar->minimum());

  scrollData_.xpos = ds*(scrollData_.xmax - scrollData_.xmin) + scrollData_.xmin;

  auto dx = pixelToSignedWindowWidth(scrollData_.xvalue - scrollData_.oldXValue);

  hscrollBy(dx);
}

void
CQChartsPlot::
vbarScrollSlot(int v)
{
  scrollData_.oldYValue = scrollData_.yvalue;
  scrollData_.yvalue    = v;

  double ds = double(v - scrollData_.vbar->minimum())/
              double(scrollData_.vbar->maximum() - scrollData_.vbar->minimum());

  scrollData_.ypos = ds*(scrollData_.ymax - scrollData_.ymin) + scrollData_.ymin;

  auto dy = pixelToSignedWindowHeight(scrollData_.yvalue - scrollData_.oldYValue);

  vscrollBy(dy);
}

void
CQChartsPlot::
hscrollBy(double dx)
{
  pan(dx, 0.0);

  adjustPan();

  applyDataRangeAndDraw();
}

void
CQChartsPlot::
vscrollBy(double dy)
{
  pan(0.0, -dy);

  adjustPan();

  applyDataRangeAndDraw();
}

//---

void
CQChartsPlot::
drawBackground()
{
  if (! isUpdatesEnabled())
    return;

  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->drawBackground();

  if (isQueueUpdate()) {
    startUpdateDrawBackground();
  }
  else {
    invalidateLayer(Buffer::Type::BACKGROUND);
  }
}

void
CQChartsPlot::
startUpdateDrawBackground()
{
  if (parentPlot())
    return parentPlot()->startUpdateDrawBackground();

  //---

  if (debugUpdate_)
    std::cerr << "drawBackground : " << id().toStdString() << "\n";

  assert(! parentPlot());

  {
  std::unique_lock<std::mutex> lock(updatesMutex_);

  ++updatesData_.stateFlag[UpdateState::UPDATE_DRAW_BACKGROUND];
  }

  startThreadTimer();
}

//---

void
CQChartsPlot::
drawForeground()
{
  if (! isUpdatesEnabled())
    return;

  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->drawForeground();

  if (isQueueUpdate()) {
    startUpdateDrawForeground();
  }
  else {
    invalidateLayer(Buffer::Type::FOREGROUND);

    invalidateOverlay();
  }
}

void
CQChartsPlot::
startUpdateDrawForeground()
{
  if (parentPlot())
    return parentPlot()->startUpdateDrawForeground();

  //---

  if (debugUpdate_)
    std::cerr << "drawForeground : " << id().toStdString() << "\n";

  assert(! parentPlot());

  {
  std::unique_lock<std::mutex> lock(updatesMutex_);

  ++updatesData_.stateFlag[UpdateState::UPDATE_DRAW_FOREGROUND];
  }

  startThreadTimer();
}

//---

void
CQChartsPlot::
drawObjs()
{
  if (view()->is3D()) {
    view()->update();
    return;
  }

  //---

  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->drawObjs1();

  if (parentPlot())
    return parentPlot()->drawObjs();

  //---

  drawObjs1();
}

void
CQChartsPlot::
drawObjs1()
{
  if (! isUpdatesEnabled())
    return;

  //---

  if (isQueueUpdate()) {
    startUpdateDrawObjs();
  }
  else
    invalidateLayers();
}

void
CQChartsPlot::
startUpdateDrawObjs()
{
  if (parentPlot())
    return parentPlot()->startUpdateDrawObjs();

  //---

  if (debugUpdate_)
    std::cerr << "drawObjs : " << id().toStdString() << "\n";

  assert(! parentPlot());

  {
  std::unique_lock<std::mutex> lock(updatesMutex_);

  ++updatesData_.stateFlag[UpdateState::UPDATE_DRAW_OBJS];
  }

  startThreadTimer();
}

//---

void
CQChartsPlot::
writeScript(ScriptPaintDevice *device) const
{
  std::string plotId = CQChartsJS::encodeId("plot_" + this->id().toStdString());

  device->setId(plotId);

  //---

  device->setContext("charts");

  std::ostream &os = device->os();

  //---

  os << "function Charts_" << plotId << "() {\n";
  os << "  this.visible = " << (isVisible() ? 1 : 0) << ";\n";
  os << "  this.objs = [];\n";
  os << "}\n";

  //---

  os << "\n";
  os << "Charts_" << plotId << ".prototype.init = function() {\n";

  int imajor = 0;

  for (const auto &plotObj : plotObjects()) {
    if (! plotObj->isVisible()) continue;

    if (plotObj->detailHint() == PlotObj::DetailHint::MAJOR) {
      auto objId  = QString("obj_") + QString::fromStdString(plotId) + "_" + plotObj->id();
      auto objStr = device->encodeObjId(objId).toStdString();

      if (imajor > 0)
        os << "\n";

      os << "  this." << objStr << " = new Charts_" << objStr << "(this);\n";
      os << "  this.objs.push(this." << objStr << ");\n";
      os << "  this." << objStr << ".init();\n";

      ++imajor;
    }
  }

  os << "  this.objs.reverse();\n"; // reverse order for tooltip

  os << "}\n";

  //---

  os << "\n";
  os << "Charts_" << plotId << ".prototype.eventMouseDown = function(e) {\n";
  os << "  if (! this.visible) return;\n";
  os << "  var rect = charts.canvas.getBoundingClientRect();\n";
  os << "  var mouseX = e.clientX - rect.left;\n";
  os << "  var mouseY = e.clientY - rect.top;\n";
  os << "  this.objs.forEach(obj => obj.eventMouseDown(mouseX, mouseY));\n";
  os << "}\n";
  os << "\n";
  os << "Charts_" << plotId << ".prototype.eventMouseMove = function(e) {\n";
  os << "  if (! this.visible) return;\n";
  os << "  var rect = charts.canvas.getBoundingClientRect();\n";
  os << "  var mouseX = e.clientX - rect.left;\n";
  os << "  var mouseY = e.clientY - rect.top;\n";
  os << "  this.objs.forEach(obj => obj.eventMouseMove(mouseX, mouseY));\n";
  os << "}\n";
  os << "\n";
  os << "Charts_" << plotId << ".prototype.eventMouseUp = function(e) {\n";
  os << "  if (! this.visible) return;\n";
  os << "  var rect = charts.canvas.getBoundingClientRect();\n";
  os << "  var mouseX = e.clientX - rect.left;\n";
  os << "  var mouseY = e.clientY - rect.top;\n";
  os << "  this.objs.forEach(obj => obj.eventMouseUp(mouseX, mouseY));\n";
  os << "}\n";

  //---

  device->resetData();

  auto vrect = viewBBox();

  os << "\n";
  os << "Charts_" << plotId << ".prototype.initRange = function() {\n";
  os << "  charts.invertX = " << isInvertX() << ";\n";
  os << "  charts.invertY = " << isInvertY() << ";\n";
  os << "\n";
  os << "  charts.vxmin = " << vrect.getXMin() << ";\n";
  os << "  charts.vymin = " << vrect.getYMin() << ";\n";
  os << "  charts.vxmax = " << vrect.getXMax() << ";\n";
  os << "  charts.vymax = " << vrect.getYMax() << ";\n";

  writeScriptRange(device);

  os << "}\n";

  //---

  // draw
  os << "\n";
  os << "Charts_" << plotId << ".prototype.draw = function() {\n";
  os << "  if (! this.visible) return;\n";
  os << "  this.initRange();\n";

  //---

  // background rects
  if (hasBackgroundRects()) {
    os << "\n"; drawBackgroundRects(device);
  }

  // custom background
  if (hasBackgroundI()) {
    os << "\n"; drawCustomBackground(device);
  }

  // background axis and key
  bool bgAxes = hasGroupedBgAxes();
  bool bgKey  = hasGroupedBgKey();

  if (bgAxes) device->writeProc("drawBgAxes");
  if (bgKey ) device->writeProc("drawBgKey");

  // background annotations
  if (hasGroupedAnnotations(Layer::Type::BG_ANNOTATION)) device->writeProc("drawBgAnnotations");

  //---

  // middle parts (objects)
  device->writeProc("drawObjs");

  //---

  // foreground annotations
  if (hasGroupedAnnotations(Layer::Type::FG_ANNOTATION)) device->writeProc("drawFgAnnotations");

  // foreground axis and key
  bool fgAxes = hasGroupedFgAxes();
  bool fgKey  = hasGroupedFgKey();

  if (fgAxes) device->writeProc("drawFgAxes");
  if (fgKey ) device->writeProc("drawFgKey");

  // foreground title
  bool title = hasTitle();

  if (title) device->writeProc("drawTitle");

  // custom foreground
  if (this->hasForegroundI()) {
    os << "\n"; drawCustomForeground(device);
  }

  //---

  os << "}\n";

  //---

  // plot object procs
  for (const auto &plotObj : plotObjects()) {
    if (! plotObj->isVisible()) continue;

    if (plotObj->detailHint() == PlotObj::DetailHint::MAJOR) {
      auto objId  = QString("obj_") + QString::fromStdString(plotId) + "_" + plotObj->id();
      auto objStr = device->encodeObjId(objId).toStdString();

      os << "\n";
      os << "function Charts_" << objStr << "(plot) {\n";
      os << "  this.plot = plot;\n";
      os << "}\n";

      os << "\n";
      os << "Charts_" << objStr << ".prototype.init = function() {\n";
      plotObj->writeScriptData(device);
      os << "}\n";

      //---

      os << "\n";
      os << "Charts_" << objStr << ".prototype.eventMouseDown = function(mouseX, mouseY) {\n";
      os << "  this.plot.initRange();\n";
      os << "  if (this.inside(mouseX, mouseY)) {\n";

      if (view()->scriptSelectProc().length())
        os << "    " << view()->scriptSelectProc().toStdString() << "(this.id);\n";
      else
        os << "    charts.log(this.tipId);\n";

      os << "  }\n";
      os << "}\n";

      os << "\n";
      os << "Charts_" << objStr << ".prototype.eventMouseMove = function(mouseX, mouseY) {\n";
      os << "  this.plot.initRange();\n";
      os << "  var isInside = this.inside(mouseX, mouseY);\n";
      os << "  if (isInside) {\n";
      os << "    if (! charts.mouseTipObj) {\n";
      os << "      charts.mouseTipObj = this;\n";
      os << "      showTooltip(mouseX, mouseY, this.tipId);\n";
      os << "    }\n";
      os << "  }\n";
      os << "  if (isInside != this.isInside) {\n";
      os << "    this.isInside = isInside;\n";
      os << "\n";
      os << "    if (this.isInside) {\n";
      plotObj->writeScriptInsideColor(device, /*isSave*/true);
      os << "    }\n";
      os << "    else {\n";
      plotObj->writeScriptInsideColor(device, /*isSave*/false);
      os << "    }\n";
      os << "    charts.update();\n";
      os << "  }\n";
      os << "}\n";

      os << "\n";
      os << "Charts_" << objStr << ".prototype.eventMouseUp = function(mouseX, mouseY) {\n";
      os << "}\n";

      //---

      os << "\n";
      os << "Charts_" << objStr << ".prototype.inside = function(px, py) {\n";

      if      (plotObj->isPolygon()) {
        if (plotObj->isSolid())
          os << "  return charts.pointInsidePoly(px, py, this.poly);\n";
        else
          os << "  return charts.pointInsidePolyline(px, py, this.poly);\n";
      }
      else if (plotObj->isCircle()) {
        os << "  return charts.pointInsideCircle(px, py, this.xc, this.yc, this.radius);\n";
      }
      else if (plotObj->isArc()) {
        os << "  return charts.pointInsideArc(px, py, this.arc);\n";
      }
      else {
        os << "  return charts.pointInsideRect(px, py, this.xmin, this.ymin, "
              "this.xmax, this.ymax);\n";
      }

      os << "}\n";

      os << "\n";
      os << "Charts_" << objStr << ".prototype.draw = function() {\n";

      plotObj->drawBg(device);
      plotObj->draw  (device);
      plotObj->drawFg(device);

      os << "}\n";
    }
  }

  //---

  // draw background annotations proc
  if (hasGroupedAnnotations(Layer::Type::BG_ANNOTATION)) {
    device->startGroup("bgAnnotations");

    drawGroupedAnnotations(device, Layer::Type::BG_ANNOTATION);

    device->endGroup();
  }

  //---

  // draw objects proc
  device->resetData();

  os << "\n";
  os << "Charts_" << plotId << ".prototype.drawObjs = function() {\n";

  for (const auto &plotObj : plotObjects()) {
    if (! plotObj->isVisible()) continue;

    if (plotObj->detailHint() == PlotObj::DetailHint::MAJOR) {
      auto objId  = QString("obj_") + QString::fromStdString(plotId) + "_" + plotObj->id();
      auto objStr = device->encodeObjId(objId).toStdString();

      os << "  this." << objStr << ".draw();\n";
    }
    else {
      plotObj->drawBg(device);
      plotObj->draw  (device);
      plotObj->drawFg(device);
    }
  }

  writeScriptParts(device);

  os << "}\n";

  //---

  // draw annotations proc
  if (hasGroupedAnnotations(Layer::Type::FG_ANNOTATION)) {
    device->startGroup("fgAnnotations");

    drawGroupedAnnotations(device, Layer::Type::FG_ANNOTATION);

    device->endGroup();
  }

  //---

  // draw axes procs
  if (bgAxes) {
    device->resetData();

    device->startGroup("bgAxes");

    drawGroupedBgAxes(device);

    device->endGroup();
  }

  if (fgAxes) {
    device->resetData();

    device->startGroup("fgAxes");

    drawGroupedFgAxes(device);

    device->endGroup();
  }

  //---

  // draw key procs
  if (bgKey) {
    device->resetData();

    device->startGroup("bgKey");

    drawBgKey(device);

    device->endGroup();
  }

  if (fgKey) {
    device->resetData();

    device->startGroup("fgKey");

    drawFgKey(device);

    device->endGroup();
  }

  //---

  // draw title proc
  if (title) {
    device->resetData();

    device->startGroup("title");

    drawTitle(device);

    device->endGroup();
  }

  //---

  device->setContext("");
}

void
CQChartsPlot::
writeScriptRange(ScriptPaintDevice *device) const
{
  std::ostream &os = device->os();

  auto prect = calcPlotRect();

  os << "\n";
  os << "  charts.xmin = " << prect.getXMin() << ";\n";
  os << "  charts.ymin = " << prect.getYMin() << ";\n";
  os << "  charts.xmax = " << prect.getXMax() << ";\n";
  os << "  charts.ymax = " << prect.getYMax() << ";\n";
}

void
CQChartsPlot::
writeSVG(SVGPaintDevice *device) const
{
  auto plotId = QString("plot_") + this->id();

  SVGPaintDevice::GroupData groupData;

  groupData.visible = isVisible();

  device->startGroup(plotId, groupData);

  if (hasBackgroundRects())
    drawBackgroundRects(device);

  if (hasBackgroundI())
    drawCustomBackground(device);

  if (hasGroupedBgAxes()) {
    device->startGroup("bg_axis");

    drawGroupedBgAxes(device);

    device->endGroup();
  }

  if (hasGroupedBgKey()) {
    device->startGroup("bg_key");

    drawBgKey(device);

    device->endGroup();
  }

  if (hasGroupedAnnotations(Layer::Type::BG_ANNOTATION))
    drawGroupedAnnotations(device, Layer::Type::BG_ANNOTATION);

  //---

  for (const auto &plotObj : plotObjects()) {
    auto objId = QString("obj_") + plotId + "_" + plotObj->id();

    SVGPaintDevice::GroupData objGroupData;

    objGroupData.visible   = plotObj->isVisible();
    objGroupData.onclick   = true;
    objGroupData.clickProc = "plotObjClick";
    objGroupData.tipStr    = SVGPaintDevice::encodeString(plotObj->tipId());
    objGroupData.hasTip    = plotObj->hasTipId();

    device->startGroup(device->encodeObjId(objId), objGroupData);

    plotObj->drawBg(device);
    plotObj->draw  (device);
    plotObj->drawFg(device);

    device->endGroup();
  }

  if (hasGroupedAnnotations(Layer::Type::FG_ANNOTATION)) {
    drawGroupedAnnotations(device, Layer::Type::FG_ANNOTATION);
  }

  //---

  if (hasForegroundI())
    drawCustomForeground(device);

  if (hasGroupedFgAxes()) {
    device->startGroup("fg_axis");

    drawGroupedFgAxes(device);

    device->endGroup();
  }

  if (hasGroupedFgKey()) {
    device->startGroup("fg_key");

    drawFgKey(device);

    device->endGroup();
  }

  if (hasTitle()) {
    device->startGroup("title");

    drawTitle(device);

    device->endGroup();
  }

  device->endGroup();
}

void
CQChartsPlot::
writeHtml(HtmlPaintDevice *device) const
{
  auto isAnnotationLayer = [&](const Annotation *annotation, const CQChartsLayer::Type &layerType) {
    return ((layerType == Layer::Type::BG_ANNOTATION &&
             annotation->drawLayer() == Annotation::DrawLayer::BACKGROUND) ||
            (layerType == Layer::Type::FG_ANNOTATION &&
             annotation->drawLayer() == Annotation::DrawLayer::FOREGROUND));
  };

  auto writeAnnotations = [&](const CQChartsLayer::Type &layerType) {
    if (hasGroupedAnnotations(layerType)) {
      if (isOverlay()) {
        if (! isFirstPlot())
          return;

        processOverlayPlots([&](const Plot *plot) {
          device->setPlot(const_cast<Plot *>(plot));

          for (auto &annotation : plot->annotations()) {
            if (isAnnotationLayer(annotation, layerType))
              annotation->writeHtml(device);
          }
        });

        device->setPlot(const_cast<Plot *>(this));
      }
      else {
        for (auto &annotation : annotations()) {
          if (isAnnotationLayer(annotation, layerType))
            annotation->writeHtml(device);
        }
      }
    }
  };

  writeAnnotations(Layer::Type::BG_ANNOTATION);
  writeAnnotations(Layer::Type::FG_ANNOTATION);
}

//---

void
CQChartsPlot::
writeStats(StatsPaintDevice *device) const
{
  if (hasBackgroundRects())
    drawBackgroundRects(device);

  if (hasBackgroundI())
    drawCustomBackground(device);

  if (hasGroupedBgAxes())
    drawGroupedBgAxes(device);

  if (hasGroupedBgKey())
    drawBgKey(device);

  if (hasGroupedAnnotations(Layer::Type::BG_ANNOTATION))
    drawGroupedAnnotations(device, Layer::Type::BG_ANNOTATION);

  //---

  for (const auto &plotObj : plotObjects()) {
    plotObj->drawBg(device);
    plotObj->draw  (device);
    plotObj->drawFg(device);
  }

  if (hasGroupedAnnotations(Layer::Type::FG_ANNOTATION)) {
    drawGroupedAnnotations(device, Layer::Type::FG_ANNOTATION);
  }

  //---

  if (hasForegroundI())
    drawCustomForeground(device);

  if (hasGroupedFgAxes())
    drawGroupedFgAxes(device);

  if (hasGroupedFgKey())
    drawFgKey(device);

  if (hasTitle())
    drawTitle(device);
}

//---

void
CQChartsPlot::
invalidateOverlay()
{
  if (parentPlot())
    return parentPlot()->invalidateOverlay();

  //---

  execInvalidateOverlay();
}

void
CQChartsPlot::
execInvalidateOverlay()
{
  execInvalidateLayer(Buffer::Type::OVERLAY);
}

//---

const CQChartsDisplayRange &
CQChartsPlot::
displayRange() const
{
  assert(! isComposite());

  return *displayRange_;
}

void
CQChartsPlot::
setDisplayRange(const DisplayRange &r)
{
  assert(r.isValid());

  assert(! isComposite());

  *displayRange_ = r;
}

const CQChartsDisplayRange &
CQChartsPlot::
rawDisplayRange() const
{
  assert(! isComposite());

  return *rawDisplayRange_;
}

//---

const CQChartsGeom::Range &
CQChartsPlot::
dataRange() const
{
  assert(! isComposite());

  return dataRange_;
}

void
CQChartsPlot::
setDataRange(const Range &r, bool update)
{
  assert(r.isValid());

  assert(! isComposite());

  CQChartsUtil::testAndSet(dataRange_, r, [&]() {
    if (debugUpdate_)
      std::cerr << "setDataRange: " << id().toStdString() << " : " << dataRange_ << "\n";

    if (update)
      updateObjs();
  } );
}

void
CQChartsPlot::
resetDataRange(bool updateRange, bool updateObjs)
{
  assert(! isComposite());

  dataRange_ = Range();

  if (updateRange)
    this->execUpdateRange();

  if (updateObjs)
    this->updateObjs();
}

void
CQChartsPlot::
resetRange()
{
  Range r;

  setDataRange(r, /*update*/false);
}

CQChartsGeom::Range
CQChartsPlot::
objTreeRange() const
{
  return dataRange();
}

//---

double
CQChartsPlot::
dataScale() const
{
  if (! isAllowZoomX() && ! isAllowZoomY())
    return 1.0;

  if      (! isAllowZoomX())
    return dataScaleY();
  else if (! isAllowZoomY())
    return dataScaleX();
  else
    return CMathUtil::avg(dataScaleX(), dataScaleY());
}

double
CQChartsPlot::
dataScaleX() const
{
  if      (isOverlay()) {
    if (! isFirstPlot())
      return firstPlot()->dataScaleX();
  }
  else if (isY1Y2()) {
    if (! isFirstPlot())
      return firstPlot()->dataScaleX();
  }

  return zoomData().dataScale.x;
}

void
CQChartsPlot::
setDataScaleX(double x)
{
  assert(x > 0.0);

  assert(isAllowZoomX());

  assert(! isComposite());

  zoomData_.dataScale.x = x;

  if (minDataScaleX_ > 0.0)
    zoomData_.dataScale.x = std::max(zoomData_.dataScale.x, minDataScaleX_);

  if (maxDataScaleX_ > 0.0)
    zoomData_.dataScale.x = std::min(zoomData_.dataScale.x, maxDataScaleX_);

  if (isScaleSymbolSize()) // TODO: only if symbols ?
    invalidateObjTree();

  scrollData_.invalid = true;
}

double
CQChartsPlot::
dataScaleY() const
{
  if      (isOverlay()) {
    if (! isFirstPlot())
      return firstPlot()->dataScaleY();
  }
  else if (isX1X2()) {
    if (! isFirstPlot())
      return firstPlot()->dataScaleY();
  }

  return zoomData().dataScale.y;
}

void
CQChartsPlot::
setDataScaleY(double y)
{
  assert(y > 0.0);

  assert(isAllowZoomY());

  assert(! isComposite());

  zoomData_.dataScale.y = y;

  if (minDataScaleY_ > 0.0)
    zoomData_.dataScale.y = std::max(zoomData_.dataScale.y, minDataScaleY_);

  if (maxDataScaleY_ > 0.0)
    zoomData_.dataScale.y = std::min(zoomData_.dataScale.y, maxDataScaleY_);

  if (isScaleSymbolSize()) // TODO: only if symbols ?
    invalidateObjTree();

  scrollData_.invalid = true;
}

double
CQChartsPlot::
dataOffsetX() const
{
  if      (isOverlay()) {
    if (! isFirstPlot())
      return firstPlot()->dataOffsetX();
  }
  else if (isY1Y2()) {
    if (! isFirstPlot())
      return firstPlot()->dataOffsetX();
  }

  return zoomData().dataOffset.x;
}

void
CQChartsPlot::
setDataOffsetX(double x)
{
  assert(! isComposite());

  zoomData_.dataOffset.x = x;
}

double
CQChartsPlot::
dataOffsetY() const
{
  if      (isOverlay()) {
    if (! isFirstPlot())
      return firstPlot()->dataOffsetY();
  }
  else if (isX1X2()) {
    if (! isFirstPlot())
      return firstPlot()->dataOffsetY();
  }

  return zoomData().dataOffset.y;
}

void
CQChartsPlot::
setDataOffsetY(double y)
{
  assert(! isComposite());

  zoomData_.dataOffset.y = y;
}

//---

const CQChartsPlot::ZoomData &
CQChartsPlot::
zoomData() const
{
  assert(! isComposite());

  return zoomData_;
}

void
CQChartsPlot::
setZoomData(const ZoomData &zoomData)
{
  assert(! isComposite());

  zoomData_ = zoomData;

  applyDataRangeAndDraw();
}

//---

void
CQChartsPlot::
updateDataScale(double r)
{
  if (isAllowZoomX())
    setDataScaleX(r);

  if (isAllowZoomY())
    setDataScaleY(r);

  applyDataRangeAndDraw();
}

void
CQChartsPlot::
updateDataScaleX(double r)
{
  if (isAllowZoomX())
    setDataScaleX(r);

  applyDataRangeAndDraw();
}

void
CQChartsPlot::
updateDataScaleY(double r)
{
  if (isAllowZoomY())
    setDataScaleY(r);

  applyDataRangeAndDraw();
}

//---

const CQChartsOptReal &
CQChartsPlot::
xmin() const
{
  return xmin_;
}

void
CQChartsPlot::
setXMin(const OptReal &r)
{
  assert(! isComposite());

  CQChartsUtil::testAndSet(xmin_, r, [&]() { updateRangeAndObjs(); } );
}

const CQChartsOptReal &
CQChartsPlot::
ymin() const
{
  return ymin_;
}

void
CQChartsPlot::
setYMin(const OptReal &r)
{
  assert(! isComposite());

  CQChartsUtil::testAndSet(ymin_, r, [&]() { updateRangeAndObjs(); } );
}

const CQChartsOptReal &
CQChartsPlot::
xmax() const
{
  return xmax_;
}

void
CQChartsPlot::
setXMax(const OptReal &r)
{
  assert(! isComposite());

  CQChartsUtil::testAndSet(xmax_, r, [&]() { updateRangeAndObjs(); } );
}

const CQChartsOptReal &
CQChartsPlot::
ymax() const
{
  return ymax_;
}

void
CQChartsPlot::
setYMax(const OptReal &r)
{
  assert(! isComposite());

  CQChartsUtil::testAndSet(ymax_, r, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPlot::
setEveryEnabled(bool b)
{
  auto everyData = this->everyData(); everyData.enabled = b; setEveryData(everyData);
}

void
CQChartsPlot::
setEveryStart(int i)
{
  auto everyData = this->everyData(); everyData.start = i; setEveryData(everyData);
}

void
CQChartsPlot::
setEveryEnd(int i)
{
  auto everyData = this->everyData(); everyData.end = i; setEveryData(everyData);
}

void
CQChartsPlot::
setEveryStep(int i)
{
  auto everyData = this->everyData(); everyData.step = i; setEveryData(everyData);
}

const CQChartsPlot::EveryData &
CQChartsPlot::
everyData() const
{
  assert(! isComposite());

  return everyData_;
}

void
CQChartsPlot::
setEveryData(const EveryData &everyData)
{
  assert(! isComposite());

  CQChartsUtil::testAndSet(everyData_, everyData, [&]() { updateRangeAndObjs(); } );
}

//---

const QString &
CQChartsPlot::
filterStr() const
{
  assert(! isComposite());

  return filterStr_;
}

void
CQChartsPlot::
setFilterStr(const QString &s)
{
  CQChartsUtil::testAndSet(filterStr_, s, [&]() { updateRangeAndObjs(); } );
}

const QString &
CQChartsPlot::
visibleFilterStr() const
{
  assert(! isComposite());

  return visibleFilterStr_;
}

void
CQChartsPlot::
setVisibleFilterStr(const QString &s)
{
  CQChartsUtil::testAndSet(visibleFilterStr_, s, [&]() { applyVisibleFilter(); drawObjs(); } );
}

//---

void
CQChartsPlot::
setSkipBad(bool b)
{
  assert(! isComposite());

  CQChartsUtil::testAndSet(badData_.skip, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPlot::
setBadUseRow(bool b)
{
  CQChartsUtil::testAndSet(badData_.useRow, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPlot::
setBadValue(double v)
{
  CQChartsUtil::testAndSet(badData_.value, v, [&]() { updateRangeAndObjs(); } );
}

double
CQChartsPlot::
getModelBadValue(const Column &column, int row) const
{
  if (isBadUseRow())
    return double(row);

  if (! column.isValid())
    return this->badValue();

  auto *details = columnDetails(column);

  if ( details) {
    auto badValue = details->badValue();

    if (badValue.isValid()) {
      bool ok;
      auto r = CQChartsVariant::toReal(badValue, ok);
      if (ok) return r;
    }
  }

  return this->badValue();
}

//---

const QString &
CQChartsPlot::
titleStr() const
{
  assert(! isComposite());

  return titleStr_;
}

void
CQChartsPlot::
setTitleStr(const QString &s)
{
  assert(! isComposite());

  if (title()) {
    // title calls drawForeground
    CQChartsUtil::testAndSet(titleStr_, s, [&]() { title()->setTextStr(titleStr_); } );
  }
}

//---

QString
CQChartsPlot::
xLabel() const
{
  return (mappedXAxis() ? mappedXAxis()->userLabel() : "");
}

void
CQChartsPlot::
setXLabel(const QString &s)
{
  if (mappedXAxis())
    mappedXAxis()->setUserLabel(s);
}

QString
CQChartsPlot::
yLabel() const
{
  return (mappedYAxis() ? mappedYAxis()->userLabel() : "");
}

void
CQChartsPlot::
setYLabel(const QString &s)
{
  if (mappedYAxis())
    mappedYAxis()->setUserLabel(s);
}

//---

CQChartsAxis *
CQChartsPlot::
xAxis() const
{
  assert(! isComposite());

  return xAxis_.get();
}

CQChartsAxis *
CQChartsPlot::
yAxis() const
{
  assert(! isComposite());

  return yAxis_.get();
}

bool
CQChartsPlot::
isXAxisVisible() const
{
  return (xAxis_ && xAxis_->isVisible());
}

bool
CQChartsPlot::
isYAxisVisible() const
{
  return (yAxis_ && yAxis_->isVisible());
}

CQChartsAxis *
CQChartsPlot::
mappedXAxis() const
{
  return xAxis();
}

CQChartsAxis *
CQChartsPlot::
mappedYAxis() const
{
  return yAxis();
}

void
CQChartsPlot::
setOverlayPlotsAxisNames()
{
  assert(isFirstPlot() && isOverlay());

  Plots oplots;

  overlayPlots(oplots);

  setPlotsAxisNames(oplots, this);
}

void
CQChartsPlot::
setPlotsAxisNames(const Plots &plots, Plot *axisPlot)
{
  assert(axisPlot->yAxis());

  QStringList xAxisLabels, yAxisLabels;

  for (auto &plot : plots) {
    if (! plot->isVisible())
      continue;

    if (plot->xAxis()) {
      QString xAxisLabel;

      if (! plot->xAxisName(xAxisLabel, "X"))
        xAxisLabel = plot->xAxis()->label().string();

      if (xAxisLabel.length() && ! xAxisLabels.contains(xAxisLabel))
        xAxisLabels += xAxisLabel;
    }

    if (plot->yAxis()) {
      QString yAxisLabel;

      if (! plot->yAxisName(yAxisLabel, "Y"))
        yAxisLabel = plot->yAxis()->label().string();

      if (yAxisLabel.length() && ! yAxisLabels.contains(yAxisLabel))
        yAxisLabels += yAxisLabel;
    }
  }

  if (! isX1X2()) {
    if (xAxisLabels.length())
      axisPlot->xAxis()->setDefLabel(xAxisLabels.join(", "), /*notify*/false);
  }

  if (! isY1Y2()) {
    if (yAxisLabels.length())
      axisPlot->yAxis()->setDefLabel(yAxisLabels.join(", "), /*notify*/false);
  }
}

//---

void
CQChartsPlot::
clearAxisSideDelta()
{
  assert(! isComposite());

  xAxisSideDelta_.clear();
  yAxisSideDelta_.clear();
}

double
CQChartsPlot::
xAxisSideDelta(const CQChartsAxisSide::Type &side) const
{
  assert(! isComposite());

  auto p = xAxisSideDelta_.find(side);

  return (p != xAxisSideDelta_.end() ? (*p).second : 0.0);
}

void
CQChartsPlot::
setXAxisSideDelta(const CQChartsAxisSide::Type &side, double d)
{
  assert(! isComposite());

  xAxisSideDelta_[side] = d;
}

double
CQChartsPlot::
yAxisSideDelta(const CQChartsAxisSide::Type &side) const
{
  assert(! isComposite());

  auto p = yAxisSideDelta_.find(side);

  return (p != yAxisSideDelta_.end() ? (*p).second : 0.0);
}

void
CQChartsPlot::
setYAxisSideDelta(const CQChartsAxisSide::Type &side, double d)
{
  assert(! isComposite());

  yAxisSideDelta_[side] = d;
}

//---

CQChartsPlotKey *
CQChartsPlot::
key() const
{
  // plots of composite plot use shared key (from composite plot)
  if (parentPlot())
    return parentPlot()->key();

  return keyObj_.get();
}

CQChartsTitle *
CQChartsPlot::
title() const
{
  assert(! isComposite());

  return titleObj_.get();
}

//---

void
CQChartsPlot::
setPlotBorderSides(const Sides &s)
{
  CQChartsUtil::testAndSet(plotBorderSides_, s, [&]() { drawBackground(); } );
}

void
CQChartsPlot::
setPlotClip(bool b)
{
  CQChartsUtil::testAndSet(plotClip_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsPlot::
setDataBorderSides(const Sides &s)
{
  CQChartsUtil::testAndSet(dataBorderSides_, s, [&]() { drawBackground(); } );
}

void
CQChartsPlot::
setDataClip(bool b, bool notify)
{
  CQChartsUtil::testAndSet(dataClip_, b, [&]() {
    if (notify)
      drawObjs();
  } );
}

void
CQChartsPlot::
setDataRawClip(bool b)
{
  CQChartsUtil::testAndSet(dataRawClip_, b, [&]() { drawObjs(); } );
}

void
CQChartsPlot::
setDataRawRange(bool b)
{
  CQChartsUtil::testAndSet(dataRawRange_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsPlot::
setFitBorderSides(const Sides &s)
{
  CQChartsUtil::testAndSet(fitBorderSides_, s, [&]() { drawBackground(); } );
}

void
CQChartsPlot::
setFitClip(bool b)
{
  CQChartsUtil::testAndSet(fitClip_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsPlot::
setFont(const Font &f)
{
  CQChartsUtil::testAndSet(font_, f, [&]() { drawObjs(); } );
}

QFont
CQChartsPlot::
qfont() const
{
  return qfont(this->font());
}

QFont
CQChartsPlot::
qfont(const Font &font) const
{
  return view()->plotFont(this, font);
}

//---

void
CQChartsPlot::
setDefaultPalette(const PaletteName &name)
{
  CQChartsUtil::testAndSet(defaultPalette_, name, [&]() { drawObjs(); } );
}

//---

void
CQChartsPlot::
setDefaultSymbolSetName(const QString &name)
{
  CQChartsUtil::testAndSet(defaultSymbolSetName_, name, [&]() { drawObjs(); } );
}

//---

void
CQChartsPlot::
setScaleSymbolSize(bool b)
{
  CQChartsUtil::testAndSet(scaleSymbolSize_, b, [&]() { updateRangeAndObjs(); } );
}

//---

bool
CQChartsPlot::
isKeyVisible() const
{
  if (parentPlot())
    return parentPlot()->isKeyVisible();

  return (key() && key()->isVisible());
}

void
CQChartsPlot::
setKeyVisible(bool b)
{
  if (parentPlot())
    return parentPlot()->setKeyVisible(b);

  if (key())
    key()->setVisible(b);

  if (b)
    updateKeyPosition(/*force*/true);

  Q_EMIT keyVisibleChanged(b);
}

bool
CQChartsPlot::
isKeyVisibleAndNonEmpty() const
{
  if (parentPlot())
    return parentPlot()->isKeyVisibleAndNonEmpty();

  return (key() && key()->isVisibleAndNonEmpty());
}

//---

void
CQChartsPlot::
setControlsKey(bool b)
{
  controlsKey_ = b;

  Q_EMIT customDataChanged();
}

//---

bool
CQChartsPlot::
isColorKey() const
{
  assert(! isComposite());

  return colorKey_;
}

void
CQChartsPlot::
setColorKey(bool b)
{
  assert(! isComposite());

  CQChartsUtil::testAndSet(colorKey_, b, [&]() {
    resetSetHidden();

    resetKeyItems();

    updateKeyPosition(/*force*/true);
  });
}

//---

bool
CQChartsPlot::
isEqualScale() const
{
  assert(! isComposite());

  return equalScale_;
}

void
CQChartsPlot::
setEqualScale(bool b)
{
  assert(! isComposite());

  CQChartsUtil::testAndSet(equalScale_, b, [&]() {
    updateRange();

    if (isAllowZoomX() && isAllowZoomY() && isEqualScale()) {
      auto scale = std::min(dataScaleX(), dataScaleY());

      setDataScaleX(scale);
      setDataScaleY(scale);
    }
  });
}

void
CQChartsPlot::
applyEqualScale(Range &dataRange) const
{
  if (isEqualScale() && dataRange.isValid()) {
    auto aspect = pixelAspect();

    dataRange.equalScale(aspect);
  }
}

//---

bool
CQChartsPlot::
isAutoFit() const
{
  assert(! isComposite());

  return autoFit_;
}

void
CQChartsPlot::
setAutoFit(bool b)
{
  assert(! isComposite());

  CQChartsUtil::testAndSet(autoFit_, b, [&]() { postResize(); });
}

const CQChartsPlotMargin &
CQChartsPlot::
fitMargin() const
{
  assert(! isComposite());

  return fitMargin_;
}

// fit margin
void
CQChartsPlot::
setFitMarginLeft(const Length &l)
{
  assert(l.isValid());

  auto fitMargin = this->fitMargin(); fitMargin.setLeft(l); setFitMargin(fitMargin);
}

void
CQChartsPlot::
setFitMarginTop(const Length &t)
{
  assert(t.isValid());

  auto fitMargin = this->fitMargin(); fitMargin.setTop(t); setFitMargin(fitMargin);
}

void
CQChartsPlot::
setFitMarginRight(const Length &r)
{
  assert(r.isValid());

  auto fitMargin = this->fitMargin(); fitMargin.setRight(r); setFitMargin(fitMargin);
}

void
CQChartsPlot::
setFitMarginBottom(const Length &b)
{
  assert(b.isValid());

  auto fitMargin = this->fitMargin(); fitMargin.setBottom(b); setFitMargin(fitMargin);
}

void
CQChartsPlot::
setFitMargin(const PlotMargin &m)
{
  assert(! isComposite());

  if (m != fitMargin_) { fitMargin_ = m; postResize(); }
}

bool
CQChartsPlot::
needsAutoFit() const
{
  assert(! isComposite());

  return needsAutoFit_;
}

void
CQChartsPlot::
setNeedsAutoFit(bool b)
{
  assert(! isComposite());

  needsAutoFit_ = b;
}

void
CQChartsPlot::
resetExtraFitBBox() const
{
  assert(! isComposite());

  extraFitBBox_ = BBox();
}

CQChartsGeom::BBox
CQChartsPlot::
extraFitBBox() const
{
  assert(! isComposite());

  if (extraFitBBox_.isSet())
    return extraFitBBox_;

  extraFitBBox_ = calcExtraFitBBox();

  return extraFitBBox_;
}

//---

void
CQChartsPlot::
setShowBoxes(bool b)
{
  CQChartsUtil::testAndSet(showBoxes_, b, [&]() { drawObjs(); } );
}

void
CQChartsPlot::
setShowSelectedBoxes(bool b)
{
  CQChartsUtil::testAndSet(showSelectedBoxes_, b, [&]() { drawObjs(); } );
}

//---

const CQChartsGeom::BBox &
CQChartsPlot::
viewBBox() const
{
  assert(! isComposite());

  return viewBBox_;
}

void
CQChartsPlot::
setViewBBox(const BBox &bbox)
{
  assert(! isComposite());

  viewBBox_      = bbox;
  innerViewBBox_ = viewBBox_;

  updateMargins();

  Q_EMIT viewBoxChanged();
}

void
CQChartsPlot::
updateMargins(bool update)
{
  if (isOverlay()) {
    if (! isFirstPlot())
      return firstPlot()->updateMargins(update);

    processOverlayPlots([&](Plot *plot) {
      plot->updateMargins(outerMargin());
    });
  }
  else {
    updateMargins(outerMargin());
  }

  updateKeyPosition(/*force*/true);

  if (update)
    drawObjs();
}

void
CQChartsPlot::
updateMargins(const PlotMargin &outerMargin)
{
  assert(! isComposite());

  innerViewBBox_ = outerMargin.adjustViewRange(this, calcViewBBox(), /*inside*/false);

  setPixelRange(innerViewBBox_);
}

//---

void
CQChartsPlot::
updateTitlePosition()
{
  if (title())
    title()->updateLocation();
}

//---

CQChartsGeom::BBox
CQChartsPlot::
calcDataRect() const
{
  return getCalcDataRange().bbox();
}

CQChartsGeom::Range
CQChartsPlot::
getCalcDataRange() const
{
  assert(! isComposite());

  return calcDataRange_;
}

CQChartsGeom::BBox
CQChartsPlot::
outerDataRect() const
{
  assert(! isComposite());

  return outerDataRange_.bbox();
}

CQChartsGeom::BBox
CQChartsPlot::
dataRect() const
{
  return dataRange().bbox();
}

//---

CQChartsGeom::BBox
CQChartsPlot::
calcViewBBox() const
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->calcViewBBox();

  return viewBBox();
}

CQChartsGeom::BBox
CQChartsPlot::
innerViewBBox() const
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->innerViewBBox();

  assert(! isComposite());

  return innerViewBBox_;
}

CQChartsGeom::BBox
CQChartsPlot::
range() const
{
  return dataRect();
}

void
CQChartsPlot::
setRange(const BBox &bbox)
{
  assert(dataScaleX() == 1.0 && dataScaleY() == 1.0);

  auto range = CQChartsUtil::bboxRange(bbox);

  setDataRange(range);

  applyDataRange();
}

double
CQChartsPlot::
pixelAspect() const
{
  double pw, ph;

  pixelSize(pw, ph);

  if (pw <= 0.0)
    return 1.0;

  return pw/ph;
}

void
CQChartsPlot::
pixelSize(double &pw, double &ph) const
{
  //auto viewBBox = calcViewBBox();
  auto viewBBox = innerViewBBox();

  auto p1 = view()->windowToPixel(Point(viewBBox.getXMin(), viewBBox.getYMin()));
  auto p2 = view()->windowToPixel(Point(viewBBox.getXMax(), viewBBox.getYMax()));

  pw = fabs(p2.x - p1.x);
  ph = fabs(p2.y - p1.y);
}

//---

// inner margin
const CQChartsPlotMargin &
CQChartsPlot::
innerMargin() const
{
  assert(! isComposite());

  return innerMargin_;
}

void
CQChartsPlot::
setInnerMarginLeft(const Length &l)
{
  auto innerMargin = this->innerMargin(); innerMargin.setLeft(l); setInnerMargin(innerMargin);
}

void
CQChartsPlot::
setInnerMarginTop(const Length &t)
{
  auto innerMargin = this->innerMargin(); innerMargin.setTop(t); setInnerMargin(innerMargin);
}

void
CQChartsPlot::
setInnerMarginRight(const Length &r)
{
  auto innerMargin = this->innerMargin(); innerMargin.setRight(r); setInnerMargin(innerMargin);
}

void
CQChartsPlot::
setInnerMarginBottom(const Length &b)
{
  auto innerMargin = this->innerMargin(); innerMargin.setBottom(b); setInnerMargin(innerMargin);
}

void
CQChartsPlot::
setInnerMargin(const PlotMargin &m)
{
  assert(! isComposite());

  if (m != innerMargin_) { innerMargin_ = m; applyDataRangeAndDraw(); }
}

//---

// outer margin
const CQChartsPlotMargin &
CQChartsPlot::
outerMargin() const
{
  assert(! isComposite());

  return outerMargin_;
}

void
CQChartsPlot::
setOuterMarginLeft(const Length &l)
{
  auto outerMargin = this->outerMargin(); outerMargin.setLeft(l); setOuterMargin(outerMargin);
}

void
CQChartsPlot::
setOuterMarginTop(const Length &t)
{
  auto outerMargin = this->outerMargin(); outerMargin.setTop(t); setOuterMargin(outerMargin);
}

void
CQChartsPlot::
setOuterMarginRight(const Length &r)
{
  auto outerMargin = this->outerMargin(); outerMargin.setRight(r); setOuterMargin(outerMargin);
}

void
CQChartsPlot::
setOuterMarginBottom(const Length &b)
{
  auto outerMargin = this->outerMargin(); outerMargin.setBottom(b); setOuterMargin(outerMargin);
}

void
CQChartsPlot::
setOuterMargin(const PlotMargin &m)
{
  assert(! isComposite());

  if (m != outerMargin_) { outerMargin_ = m; updateMargins(); }
}

void
CQChartsPlot::
setPixelMarginSize(double r)
{
  assert(! isComposite());

  if (r != pixelMarginSize_) { pixelMarginSize_ = r; updateMargins(); }
}

//---

void
CQChartsPlot::
emitTitleChanged()
{
  Q_EMIT titleChanged();
}

//---

bool
CQChartsPlot::
isOverlay(bool checkVisible) const
{
  if (! connectData_.overlay)
    return false;

  if (checkVisible) {
    Plots oplots;

    overlayPlots(oplots);

    return (oplots.size() > 1);
  }

  return true;
}

void
CQChartsPlot::
setOverlay(bool b, bool notify)
{
  connectData_.overlay = b;

  if (notify)
    Q_EMIT connectDataChanged();
}

void
CQChartsPlot::
updateOverlay()
{
  processOverlayPlots([&](Plot *plot) {
    plot->stopThreadTimer ();
    plot->startThreadTimer();

    {
    assert(! parentPlot());

    std::unique_lock<std::mutex> lock(updatesMutex_);

    plot->updatesData_.reset();
    }
  });

  applyDataRangeAndDraw();
}

bool
CQChartsPlot::
isX1X2(bool checkVisible) const
{
  if (! connectData_.x1x2)
    return false;

  if (checkVisible && isOverlay()) {
    Plots oplots;

    overlayPlots(oplots);

    return (oplots.size() > 1);
  }

  return true;
}

void
CQChartsPlot::
setX1X2(bool b, bool notify)
{
  connectData_.x1x2 = b;

  if (notify)
    Q_EMIT connectDataChanged();
}

bool
CQChartsPlot::
isY1Y2(bool checkVisible) const
{
  if (! connectData_.y1y2)
    return false;

  if (checkVisible && isOverlay()) {
    Plots oplots;

    overlayPlots(oplots);

    return (oplots.size() > 1);
  }

  return true;
}

void
CQChartsPlot::
setY1Y2(bool b, bool notify)
{
  connectData_.y1y2 = b;

  if (notify)
    Q_EMIT connectDataChanged();
}

bool
CQChartsPlot::
isTabbed(bool checkVisible) const
{
  if (! connectData_.tabbed)
    return false;

  if (checkVisible) {
    Plots plots;

    tabbedPlots(plots);

    return plots.size() > 1;
  }

  return true;
}

void
CQChartsPlot::
setTabbed(bool b, bool notify)
{
  connectData_.tabbed = b;

  if (notify)
    Q_EMIT connectDataChanged();
}

//---

void
CQChartsPlot::
setShowAllXOverlayAxes(bool b)
{
  CQChartsUtil::testAndSet(showAllXOverlayAxes_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPlot::
setShowAllYOverlayAxes(bool b)
{
  CQChartsUtil::testAndSet(showAllYOverlayAxes_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPlot::
setCurrent(bool b, bool notify)
{
  connectData_.current = b;

  if (notify) {
    Q_EMIT currentPlotChanged(this);
    Q_EMIT currentPlotIdChanged(id());
  }
}

void
CQChartsPlot::
setNextPlot(Plot *plot, bool notify)
{
  assert(plot != this && ! connectData_.next);

  connectData_.next = plot;

  if (notify)
    Q_EMIT connectDataChanged();
}

void
CQChartsPlot::
setPrevPlot(Plot *plot, bool notify)
{
  assert(plot != this && ! connectData_.prev);

  connectData_.prev = plot;

  if (notify)
    Q_EMIT connectDataChanged();
}

CQChartsPlot *
CQChartsPlot::
firstPlot()
{
  if (connectData_.prev)
    return connectData_.prev->firstPlot();

  auto *plot = this;

  while (plot && ! plot->isVisible())
    plot = plot->nextPlot();

  if (! plot)
    plot = this;

  return plot;
}

const CQChartsPlot *
CQChartsPlot::
firstPlot() const
{
  return const_cast<Plot *>(this)->firstPlot();
}

CQChartsPlot *
CQChartsPlot::
lastPlot()
{
  if (connectData_.next)
    return connectData_.next->lastPlot();

  auto *plot = this;

  while (plot && ! plot->isVisible())
    plot = plot->prevPlot();

  if (! plot)
    plot = this;

  return plot;
}

const CQChartsPlot *
CQChartsPlot::
lastPlot() const
{
  return const_cast<Plot *>(this)->lastPlot();
}

void
CQChartsPlot::
overlayPlots(Plots &plots, bool visibleOnly) const
{
  assert(connectData_.overlay);

  auto *plot1 = firstPlot();

  while (plot1) {
    assert(plot1->connectData_.overlay);

    if (! visibleOnly || plot1->isVisible())
      plots.push_back(const_cast<Plot *>(plot1));

    plot1 = plot1->nextPlot();
  }
}

void
CQChartsPlot::
x1x2Plots(Plots &plots)
{
  auto *plot1 = firstPlot();

  while (plot1) {
    assert(plot1->connectData_.x1x2);

    plots.push_back(const_cast<Plot *>(plot1));

    plot1 = plot1->nextPlot();
  }

  assert(plots.size() >= 2);
}

void
CQChartsPlot::
y1y2Plots(Plots &plots)
{
  auto *plot1 = firstPlot();

  while (plot1) {
    assert(plot1->connectData_.y1y2);

    plots.push_back(const_cast<Plot *>(plot1));

    plot1 = plot1->nextPlot();
  }

  assert(plots.size() >= 2);
}

bool
CQChartsPlot::
tabbedPlots(Plots &plots, bool visibleOnly) const
{
  assert(connectData_.tabbed);

  bool valid = true;

  auto *plot1 = firstPlot();

  while (plot1) {
    if (! plot1->connectData_.tabbed)
      valid = false;

    if (! visibleOnly || plot1->isVisible())
      plots.push_back(const_cast<Plot *>(plot1));

    plot1 = plot1->nextPlot();
  }

  return valid;
}

void
CQChartsPlot::
resetConnectData(bool notify)
{
  connectData_.reset();

  if (notify)
    Q_EMIT connectDataChanged();
}

//---

CQChartsPlot *
CQChartsPlot::
selectionPlot() const
{
  if (isOverlay()) {
    Plots oplots;

    overlayPlots(oplots);

    for (const auto &oplot : oplots) {
      for (auto &plotObj : oplot->plotObjects()) {
        if (plotObj->isSelected())
          return oplot;
      }

      for (auto &annotation : oplot->annotations()) {
        if (annotation->isSelected())
          return oplot;
      }
    }
  }

  return const_cast<Plot *>(this);
}

//---

void
CQChartsPlot::
cycleNextPlot()
{
  auto *plot = firstPlot();

  while (plot) {
    if (plot->isCurrent())
      break;

    plot = plot->nextPlot();
  }

  if (! plot)
    plot = firstPlot();

  plot->setCurrent(false, /*notify*/false);

  plot = plot->nextPlot();

  if (! plot)
    plot = firstPlot();

  plot->setCurrent(true, /*notify*/true);

  plot->updateRangeAndObjs();
}

void
CQChartsPlot::
cyclePrevPlot()
{
  auto *plot = lastPlot();

  while (plot) {
    if (plot->isCurrent())
      break;

    plot = plot->prevPlot();
  }

  if (! plot)
    plot = lastPlot();

  plot->setCurrent(false, /*notify*/false);

  plot = plot->prevPlot();

  if (! plot)
    plot = lastPlot();

  plot->setCurrent(true, /*notify*/true);

  plot->updateRangeAndObjs();
}

QString
CQChartsPlot::
connectionStateStr() const
{
  QStringList states;

  if      (isX1X2()) states += "x1x2";
  else if (isY1Y2()) states += "y1y2";

  if (isOverlay()) states += "overlay";

  if (isTabbed()) states += "tabbed";

  auto stateStr = states.join("|");

  if (stateStr == "")
    stateStr = "none";

  return stateStr;
}

//------

const CQChartsFont &
CQChartsPlot::
tabbedFont() const
{
  if (isTabbed() && ! isFirstPlot())
    return firstPlot()->tabbedFont();

  return tabbedFont_;
}

void
CQChartsPlot::
setTabbedFont(const Font &f)
{
  if (isTabbed()) {
    Plots plots;

    tabbedPlots(plots);

    for (auto &plot : plots)
      plot->tabbedFont_ = f;

    if (isFirstPlot())
      firstPlot()->drawObjs();
  }
  else
    CQChartsUtil::testAndSet(tabbedFont_, f, [&]() { drawObjs(); } );
}

void
CQChartsPlot::
setZoomText(bool b)
{
  CQChartsUtil::testAndSet(zoomText_, b, [&]() { updateRangeAndObjs(); } );
}

//------

void
CQChartsPlot::
setInvertX(bool b)
{
  if (isOverlay()) {
    // if first plot then set all plots to same invert value
    if (prevPlot())
      return;

    //---

    processOverlayPlots([&](Plot *plot) {
      plot->invertX_ = b;
    });
  }
  else {
    invertX_ = b;
  }

  if (isAutoFit())
    setNeedsAutoFit(true);

  drawObjs();
}

void
CQChartsPlot::
setInvertY(bool b)
{
  if (isOverlay()) {
    // if first plot then set all plots to same invert value
    if (prevPlot())
      return;

    //---

    processOverlayPlots([&](Plot *plot) {
      plot->invertY_ = b;
    });
  }
  else {
    invertY_ = b;
  }

  if (isAutoFit())
    setNeedsAutoFit(true);

  drawObjs();
}

//------

bool
CQChartsPlot::
isLogX() const
{
  // return logX_;
  return (mappedXAxis() && mappedXAxis()->valueType().type() == CQChartsAxisValueType::Type::LOG);
}

bool
CQChartsPlot::
isLogY() const
{
  // return logY_;
  return (mappedYAxis() && mappedYAxis()->valueType().type() == CQChartsAxisValueType::Type::LOG);
}

void
CQChartsPlot::
setLogX(bool b)
{
  if (mappedXAxis() && b != isLogX()) {
    mappedXAxis()->setValueType(CQChartsAxisValueType(b ? CQChartsAxisValueType::Type::LOG :
                                                          CQChartsAxisValueType::Type::REAL));
    updateRangeAndObjs();
  }
}

void
CQChartsPlot::
setLogY(bool b)
{
  if (mappedYAxis() && b != isLogY()) {
    mappedYAxis()->setValueType(CQChartsAxisValueType(b ? CQChartsAxisValueType::Type::LOG :
                                                          CQChartsAxisValueType::Type::REAL));
    updateRangeAndObjs();
  }
}

bool
CQChartsPlot::
isPolar() const
{
  return polar_;
}

void
CQChartsPlot::
setPolar(bool b)
{
  if (b != isPolar()) {
    polar_ = b; updateRangeAndObjs();
  }
}

//------

CQChartsEditHandles *
CQChartsPlot::
editHandles() const
{
  if (! editHandles_) {
    auto *th = const_cast<CQChartsPlot *>(this);

    th->editHandles_ = std::make_unique<EditHandles>(view());
  }

  return editHandles_.get();
}

//------

const CQPropertyViewModel *
CQChartsPlot::
propertyModel() const
{
  return propertyModel_.get();
}

CQPropertyViewModel *
CQChartsPlot::
propertyModel()
{
  return propertyModel_.get();
}

void
CQChartsPlot::
addProperties()
{
  addBaseProperties();
}

void
CQChartsPlot::
addBaseProperties()
{
  // data
  addPropI("state", "viewId"    , "view", "Parent view id" );
  addPropI("state", "typeName"  , "type", "Type name"      );
  addPropI("state", "visible"   , ""    , "Plot visible"   );
  addPropI("state", "selected"  , ""    , "Plot selected"  );
  addPropI("state", "selectable", ""    , "Plot selectable");
  addPropI("state", "editable"  , ""    , "Plot editable"  );

  addPropI("state", "name", "", "Plot name");

  addPropI("state", "currentModelIndex", "currentModelInd", "Current model index");
  addPropI("state", "modelInds"        , "currentModel"   , "Current model from plot models");

  // font
  addStyleProp("font", "font"      , "font"      , "Base font");
  addStyleProp("font", "tabbedFont", "tabbedFont", "Font for tabs");
  addStyleProp("font", "zoomText"  , "zoomText"  , "Scale text when zoom");

  // columns
  if (type()->supportsIdColumn())
    addProp("columns", "idColumn", "id", "Id column");

  addProp("columns", "tipHeaderColumn", "tipHeader", "Tip header column");
  addProp("columns", "tipColumns"     , "tips"     , "Tip columns");
  addProp("columns", "noTipColumns"   , "notips"   , "No Tip columns");

  addProp("columns", "visibleColumn", "visible", "Visible column");

  if (type()->supportsColorColumn()) {
    addProp("columns", "colorColumn"     , "color"     , "Color column");
    addProp("columns", "colorLabelColumn", "colorLabel", "Color key label column");
  }

  if (type()->supportsAlphaColumn())
    addProp("columns", "alphaColumn", "alpha", "Alpha column");

  if (type()->supportsFontColumn())
    addProp("columns", "fontColumn", "font", "Font column");

  if (type()->supportsImageColumn())
    addProp("columns", "imageColumn", "image", "Image column");

  addProp("columns", "controlColumns", "controls", "Control columns");

  // range
  addProp("range", "viewRect", "view", "View rectangle");
  addProp("range", "dataRect", "data", "Data rectangle");

  addPropI("range", "innerViewRect", "innerView", "Inner view rectangle"     );
  addPropI("range", "calcDataRect" , "calcData" , "Calculated data rectangle");
  addPropI("range", "outerDataRect", "outerData", "Outer data rectangle"     );

  addProp("range", "autoFit", "autoFit", "Auto fit to data");

  if (type()->customXRange()) addProp("range", "xmin", "xmin", "Explicit minimum x value");
  if (type()->customYRange()) addProp("range", "ymin", "ymin", "Explicit minimum y value");
  if (type()->customXRange()) addProp("range", "xmax", "xmax", "Explicit maximum x value");
  if (type()->customYRange()) addProp("range", "ymax", "ymax", "Explicit maximum y value");

  // scaling
  if (type()->canEqualScale())
    addProp("scaling", "equalScale", "equal", "Equal x/y scaling");

  addPropI("scaling/data/scale" , "dataScaleX" , "x", "X data scale" );
  addPropI("scaling/data/scale" , "dataScaleY" , "y", "Y data scale" );
  addPropI("scaling/data/offset", "dataOffsetX", "x", "X data offset");
  addPropI("scaling/data/offset", "dataOffsetY", "y", "Y data offset");

  // grouping
  addPropI("grouping", "overlay", "", "Overlay plots to shared range"    );
  addPropI("grouping", "x1x2"   , "", "Independent x axes, shared y axis");
  addPropI("grouping", "y1y2"   , "", "Independent y axes, shared x axis");
  addPropI("grouping", "tabbed" , "", "Tabbed plots in same region"      );

  // invert
  addProp("invert", "invertX", "x", "Invert x values");
  addProp("invert", "invertY", "y", "Invert y values");

#if 0
  // log
  if (type()->allowXLog()) addProp("log", "logX", "x", "Use log x axis");
  if (type()->allowYLog()) addProp("log", "logY", "y", "Use log y axis");
#endif

  if (type()->allowPolar()) addProp("polar", "polar", "enabled", "Use polar values");

  // animation
  addProp("animation", "animating", "running", "Is Animation Running");
  addProp("animation", "animTick" , "tick"   , "Animation tick (ms)");

  // debug
  addPropI("debug", "showBoxes"        , "", "Show object bounding boxes"         );
  addPropI("debug", "showSelectedBoxes", "", "Show selected object bounding boxes");
  addPropI("debug", "followMouse"      , "", "Enable mouse tracking");

#ifdef CQCHARTS_MODULE_SHLIB
  // modules
  addPropI("module", "plotModule", "name", "Set module name");
#endif

  //------

  // plot box
  auto plotStyleStr       = QString("plotBox");
  auto plotStyleFillStr   = plotStyleStr + "/fill";
  auto plotStyleStrokeStr = plotStyleStr + "/stroke";

  addProp(plotStyleStr, "plotClip", "clip", "Clip to plot bounding box");

  addPropI(plotStyleStr, "plotShapeData", "shape", "Plot background shape data");

  addStyleProp(plotStyleFillStr, "plotFilled", "visible",
               "Plot background bounding box fill visible");

  addFillProperties(plotStyleFillStr, "plotFill", "Plot background");

  addStyleProp(plotStyleStrokeStr, "plotStroked", "visible",
               "Plot background bounding box stroke visible");

  addLineProperties(plotStyleStrokeStr, "plotStroke", "Plot background");

  addStyleProp(plotStyleStrokeStr, "plotBorderSides", "sides",
               "Plot background bounding box stroked sides");

  //---

  // data box
  auto dataStyleStr       = QString("dataBox");
  auto dataStyleFillStr   = dataStyleStr + "/fill";
  auto dataStyleStrokeStr = dataStyleStr + "/stroke";

  addProp(dataStyleStr, "dataRawRange", "unscaledRange", "Draw unscaled bounding box");
  addProp(dataStyleStr, "dataClip"    , "clip"         , "Clip to scaled data bounding box");
  addProp(dataStyleStr, "dataRawClip" , "unscaledClip" , "Clip to unscaled data bounding box");

  addPropI(dataStyleStr, "dataShapeData", "shape", "Data background shape data");

  addStyleProp(dataStyleFillStr, "dataFilled", "visible",
               "Data background bounding box fill visible");

  addFillProperties(dataStyleFillStr, "dataFill", "Data background");

  addStyleProp(dataStyleStrokeStr, "dataStroked", "visible",
               "Data background bounding box stroke visible");

  addLineProperties(dataStyleStrokeStr, "dataStroke", "Data background");

  addStyleProp(dataStyleStrokeStr, "dataBorderSides", "sides",
               "Data background bounding box stroked sides");

  //---

  // fit box
  auto fitStyleStr       = QString("fitBox");
  auto fitStyleFillStr   = fitStyleStr + "/fill";
  auto fitStyleStrokeStr = fitStyleStr + "/stroke";

  addProp(fitStyleStr, "fitClip", "clip", "Clip to fit bounding box");

  addStylePropI(fitStyleFillStr, "fitFilled", "visible",
                "Fit background bounding box fill visible");

  addFillProperties(fitStyleFillStr, "fitFill", "Fit background",
                    uint(CQChartsFillDataTypes::STANDARD), /*hidden*/true);

  addStyleProp(fitStyleStrokeStr, "fitStroked", "visible",
               "Fit background bounding box stroke visible", /*hidden*/true);

  addLineProperties(fitStyleStrokeStr, "fitStroke", "Fit background",
                     uint(CQChartsStrokeDataTypes::STANDARD), /*hidden*/true);

  addStyleProp(fitStyleStrokeStr, "fitBorderSides", "sides",
               "Fit background bounding box stroked sides", /*hidden*/true);

  //---

  // margin
  addProp("margins/inner", "innerMarginLeft"  , "left"  , "Size of inner margin at left of plot");
  addProp("margins/inner", "innerMarginTop"   , "top"   , "Size of inner margin at top of plot");
  addProp("margins/inner", "innerMarginRight" , "right" , "Size of inner margin at right of plot");
  addProp("margins/inner", "innerMarginBottom", "bottom", "Size of inner margin at bottom of plot");

  addProp("margins/outer", "outerMarginLeft"  , "left"  , "Size of outer margin at left of plot");
  addProp("margins/outer", "outerMarginTop"   , "top"   , "Size of outer margin at top of plot");
  addProp("margins/outer", "outerMarginRight" , "right" , "Size of outer margin at right of plot");
  addProp("margins/outer", "outerMarginBottom", "bottom", "Size of outer margin at bottom of plot");

  addProp("margins/fit", "fitMarginLeft"  , "left"  , "Size of fit margin at left of plot");
  addProp("margins/fit", "fitMarginTop"   , "top"   , "Size of fit margin at top of plot");
  addProp("margins/fit", "fitMarginRight" , "right" , "Size of fit margin at right of plot");
  addProp("margins/fit", "fitMarginBottom", "bottom", "Size of fit margin at bottom of plot");

  addProp("margins", "pixelMarginSize", "pixelMargin", "Extra margin around plot in pixels");

  //---

  // every
  addPropI("every", "everyEnabled", "enabled", "Enable every row filter"  );
  addPropI("every", "everyStart"  , "start"  , "Start of every row filter");
  addPropI("every", "everyEnd"    , "end"    , "End of every row filter"  );
  addPropI("every", "everyStep"   , "step"   , "Step of every row filter" );

  // filter
  addPropI("filter", "filterStr"       , "expression", "Filter expression");
  addPropI("filter", "visibleFilterStr", "visible"   , "Filter visible expression");

  // bad data
  addProp("filter/bad", "skipBad"  , "skip"  , "Skip bad values");
  addProp("filter/bad", "badUseRow", "useRow", "Use row number for bad values");
  addProp("filter/bad", "badValue" , "value" , "Bad value (when not using row number)");

  //---

  // xaxis
  if (xAxis()) {
    xAxis()->addProperties(propertyModel(), "xaxis");

//  addProperty("xaxis", xAxis(), "userLabel", "userLabel")->
//    setDesc("User defined x axis label");

    addProp("xaxis", "showAllXOverlayAxes", "showOverlayAxes", "Show all overlay x axes");
  }

  // yaxis
  if (yAxis()) {
    yAxis()->addProperties(propertyModel(), "yaxis");

//  addProperty("yaxis", yAxis(), "userLabel", "userLabel")->
//    setDesc("User defined y axis label");

    addProp("yaxis", "showAllYOverlayAxes", "showOverlayAxes", "Show all overlay y axes");
  }

  // key
  if (key()) {
    key()->addProperties(propertyModel(), "key");

    addProp("key", "controlsKey", "inControls" , "Show Key in Controls Widget");
    addProp("key", "colorKey"   , "colorColumn", "Use Color Column for Key");
  }

  // title
  if (title())
    title()->addProperties(propertyModel(), "title");

  // coloring
  addProp("coloring", "defaultPalette", "defaultPalette", "Default palette");
  addProp("coloring", "colorType"     , "type"          , "Color interpolation type");
  addProp("coloring", "colorXStops"   , "xStops"        , "Color x stop coordinates");
  addProp("coloring", "colorYStops"   , "yStops"        , "Color y stop coordinates");

  // scaled font
  addPropI("scaledFont", "minScaleFontSize", "minSize", "Min scaled font size");
  addPropI("scaledFont", "maxScaleFontSize", "maxSize", "Max scaled font size");

  // scaled symbol
  addPropI("points", "scaleSymbolSize", "scaled", "Are symbols scaled on zoom");
  addPropI("points", "defaultSymbolSetName", "defaultSymbolSet", "Default symbol set");

  // zoom/pan
  addPropI("zoom", "allowZoomX", "x"     , "Allow zoom x");
  addPropI("zoom", "allowZoomY", "y"     , "Allow zoom y");
  addPropI("zoom", "zoomScroll", "scroll", "Add scrollbars on zoom");

  addPropI("pan" , "allowPanX" , "x", "Allow pan x");
  addPropI("pan" , "allowPanY" , "y", "Allow pan y");

  // overview
  addProp("overview", "overviewDisplayed"  , "displayed"  , "Overview displayed");
  addProp("overview", "overviewSize"       , "size"       , "Overview size");
  addProp("overview", "overviewFillColor"  , "fillColor"  , "Overview fill color");
  addProp("overview", "overviewFillAlpha"  , "fillAlpha"  , "Overview fill alpha");
  addProp("overview", "overviewStrokeColor", "strokeColor", "Overview stroke color");
  addProp("overview", "overviewStrokeAlpha", "strokeAlpha", "Overview stroke alpha");

  // ruler
  addProp("ruler", "rulerDisplayed", "displayed", "Ruler displayed");
  addProp("ruler", "rulerSize"     , "size"     , "Ruler size");
  addProp("ruler", "rulerFontScale", "fontScale", "Ruler font scale");
}

void
CQChartsPlot::
addSymbolProperties(const QString &path, const QString &prefix, const QString &descPrefix,
                    bool hidden)
{
  auto prefix1 = (descPrefix.length() ? descPrefix + " symbol" : "Symbol");

  auto strokePath = path + "/stroke";
  auto fillPath   = path + "/fill";

  auto symbolPrefix = (prefix.length() ? prefix + "Symbol" : "symbol");

  addProp(path, symbolPrefix         , "symbol", prefix1          , hidden);
  addProp(path, symbolPrefix + "Size", "size"  , prefix1 + " size", hidden);

  //---

  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc, bool hidden) {
    auto *item = addProp(path, name, alias, desc, hidden);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  addStyleProp(fillPath, symbolPrefix + "Filled"     , "visible",
               prefix1 + " fill visible", hidden);
  addStyleProp(fillPath, symbolPrefix + "FillColor"  , "color",
               prefix1 + " fill color", hidden);
  addStyleProp(fillPath, symbolPrefix + "FillAlpha"  , "alpha",
               prefix1 + " fill alpha", hidden);
  addStyleProp(fillPath, symbolPrefix + "FillPattern", "pattern",
               prefix1 + " fill pattern", hidden);

  addStyleProp(strokePath, symbolPrefix + "Stroked"    , "visible",
               prefix1 + " stroke visible", hidden);
  addStyleProp(strokePath, symbolPrefix + "StrokeColor", "color",
               prefix1 + " stroke color", hidden);
  addStyleProp(strokePath, symbolPrefix + "StrokeAlpha", "alpha",
               prefix1 + " stroke alpha", hidden);
  addStyleProp(strokePath, symbolPrefix + "StrokeWidth", "width",
               prefix1 + " stroke width", hidden);
  addStyleProp(strokePath, symbolPrefix + "StrokeDash" , "dash",
               prefix1 + " stroke dash", hidden);
  addStyleProp(strokePath, symbolPrefix + "StrokeJoin" , "join",
               prefix1 + " stroke join", hidden);
  addStyleProp(strokePath, symbolPrefix + "StrokeCap"  , "cap",
               prefix1 + " stroke cap", hidden);
}

void
CQChartsPlot::
addLineProperties(const QString &path, const QString &prefix, const QString &descPrefix,
                  uint types, bool hidden)
{
  auto prefix1 = (descPrefix.length() ? descPrefix + " stroke" : "Stroke");

  if (types & uint(CQChartsStrokeDataTypes::VISIBLE))
    addStyleProp(path, prefix + "Stroked", "visible", prefix1 + " is visible", hidden);

  if (types & uint(CQChartsStrokeDataTypes::COLOR))
    addStyleProp(path, prefix + "Color", "color", prefix1 + " color", hidden);

  if (types & uint(CQChartsStrokeDataTypes::ALPHA))
    addStyleProp(path, prefix + "Alpha", "alpha", prefix1 + " alpha", hidden);

  if (types & uint(CQChartsStrokeDataTypes::WIDTH))
    addStyleProp(path, prefix + "Width", "width", prefix1 + " width", hidden);

  if (types & uint(CQChartsStrokeDataTypes::DASH))
    addStyleProp(path, prefix + "Dash" , "dash", prefix1 + " dash" , hidden);

  if (types & uint(CQChartsStrokeDataTypes::CAP))
    addStyleProp(path, prefix + "Cap" , "cap", prefix1 + " cap" , hidden);

  if (types & uint(CQChartsStrokeDataTypes::JOIN))
    addStyleProp(path, prefix + "Join" , "join", prefix1 + " join" , hidden);
}

void
CQChartsPlot::
addFillProperties(const QString &path, const QString &prefix, const QString &descPrefix,
                  uint types, bool hidden)
{
  auto prefix1 = (descPrefix.length() ? descPrefix + " fill" : "Fill");

  if (types & uint(CQChartsFillDataTypes::VISIBLE))
    addStyleProp(path, prefix + "Filled", "visible", prefix1 + " fill visible", hidden);

  if (types & uint(CQChartsFillDataTypes::COLOR))
    addStyleProp(path, prefix + "Color", "color", prefix1 + " color", hidden);

  if (types & uint(CQChartsFillDataTypes::ALPHA))
    addStyleProp(path, prefix + "Alpha", "alpha", prefix1 + " alpha", hidden);

  if (types & uint(CQChartsFillDataTypes::PATTERN))
    addStyleProp(path, prefix + "Pattern", "pattern", prefix1 + " pattern", hidden);
}

void
CQChartsPlot::
addTextProperties(const QString &path, const QString &prefix, const QString &descPrefix,
                  uint valueTypes, bool hidden)
{
  auto prefix1 = (descPrefix.length() ? descPrefix + " text" : "Text");

  // style
  addStyleProp(path, prefix + "Color", "color", prefix1 + " color", hidden);
  addStyleProp(path, prefix + "Alpha", "alpha", prefix1 + " alpha", hidden);
  addStyleProp(path, prefix + "Font" , "font" , prefix1 + " font" , hidden);

  // options
  if (valueTypes & CQChartsTextOptions::ValueType::ANGLE)
    addStyleProp(path, prefix + "Angle", "angle", prefix1 + " angle", hidden);

  if (valueTypes & CQChartsTextOptions::ValueType::CONTRAST) {
    addStyleProp(path, prefix + "Contrast"     , "contrast"     , prefix1 + " contrast", hidden);
    addStyleProp(path, prefix + "ContrastAlpha", "contrastAlpha",
                 prefix1 + " contrast alpha", hidden);
  }

  if (valueTypes & CQChartsTextOptions::ValueType::ALIGN)
    addStyleProp(path, prefix + "Align", "align", prefix1 + " align", hidden);

  if (valueTypes & CQChartsTextOptions::ValueType::FORMATTED)
    addStyleProp(path, prefix + "Formatted", "formatted",
                 prefix1 + " formatted to fit box", hidden);

  if (valueTypes & CQChartsTextOptions::ValueType::SCALED)
    addStyleProp(path, prefix + "Scaled", "scaled", prefix1 + " scaled to box", hidden);

  if (valueTypes & CQChartsTextOptions::ValueType::HTML)
    addStyleProp(path, prefix + "Html", "html", prefix1 + " is HTML", hidden);

  if (valueTypes & CQChartsTextOptions::ValueType::CLIP_LENGTH)
    addStyleProp(path, prefix + "ClipLength", "clipLength", prefix1 + " clip length", hidden);

  if (valueTypes & CQChartsTextOptions::ValueType::CLIP_ELIDE)
    addStyleProp(path, prefix + "ClipElide", "clipElide", prefix1 + " clip elide", hidden);
}

void
CQChartsPlot::
addColorMapProperties()
{
  addProp("mapping/color", "colorMapped"    , "enabled"  , "Color values mapped by column range");
  addProp("mapping/color", "colorMapMin"    , "min"      , "Color value map target min");
  addProp("mapping/color", "colorMapMax"    , "max"      , "Color value map target max");
  addProp("mapping/color", "colorMapPalette", "palette"  , "Color map palette");
  addProp("mapping/color", "colorMap"       , "color_map", "Value to color map");
  addProp("mapping/color", "colorMapColumn" , "column"   , "Color map column");
}

//--

void
CQChartsPlot::
updateProperties()
{
  enableProp(this, "font.tabbedFont", isTabbed());
}

//--

CQPropertyViewItem *
CQChartsPlot::
addStyleProp(const QString &path, const QString &name, const QString &alias,
             const QString &desc, bool hidden)
{
  auto *item = addProp(path, name, alias, desc, hidden);
  CQCharts::setItemIsStyle(item);
  return item;
}

CQPropertyViewItem *
CQChartsPlot::
addProp(const QString &path, const QString &name, const QString &alias,
        const QString &desc, bool hidden)
{
  auto *item = this->addProperty(path, this, name, alias);
  item->setDesc(desc);
  if (hidden) CQCharts::setItemIsHidden(item);
  return item;
}

void
CQChartsPlot::
hideProp(QObject *obj, const QString &path)
{
  auto *item = propertyModel()->propertyItem(obj, path);

  CQCharts::setItemIsHidden(item);
}

void
CQChartsPlot::
enableProp(QObject *obj, const QString &path, bool enabled)
{
  auto *item = propertyModel()->propertyItem(obj, path);

  if (item)
    item->setEditable(enabled);
}

//---

bool
CQChartsPlot::
setProperties(const QString &properties)
{
  bool rc = true;

  CQChartsNameValues nameValues(properties);

  for (const auto &nv : nameValues.nameValues()) {
    const auto &name  = nv.first;
    const auto &value = nv.second;

    if (! setProperty(name, value))
      rc = false;
  }

  return rc;
}

bool
CQChartsPlot::
setProperty(const QString &name, const QVariant &value)
{
  return propertyModel()->setProperty(this, name, value);
}

bool
CQChartsPlot::
getProperty(const QString &name, QVariant &value) const
{
  return propertyModel()->getProperty(this, name, value);
}

bool
CQChartsPlot::
getTclProperty(const QString &name, QVariant &value) const
{
  return propertyModel()->getTclProperty(this, name, value);
}

bool
CQChartsPlot::
getPropertyDesc(const QString &name, QString &desc, bool hidden) const
{
  const auto *item = propertyModel()->propertyItem(this, name, hidden);
  if (! item) return false;

  desc = item->desc();

  return true;
}

bool
CQChartsPlot::
getPropertyType(const QString &name, QString &type, bool hidden) const
{
  const auto *item = propertyModel()->propertyItem(this, name, hidden);
  if (! item) return false;

  type = item->typeName();

  return true;
}

bool
CQChartsPlot::
getPropertyUserType(const QString &name, QString &type, bool hidden) const
{
  const auto *item = propertyModel()->propertyItem(this, name, hidden);
  if (! item) return false;

  type = item->userTypeName();

  return true;
}

bool
CQChartsPlot::
getPropertyObject(const QString &name, QObject* &object, bool hidden) const
{
  object = nullptr;

  const auto *item = propertyModel()->propertyItem(this, name, hidden);
  if (! item) return false;

  object = item->object();

  return true;
}

bool
CQChartsPlot::
getPropertyIsHidden(const QString &name, bool &is_hidden) const
{
  is_hidden = false;

  const auto *item = propertyModel()->propertyItem(this, name, /*hidden*/true);
  if (! item) return false;

  is_hidden = CQCharts::getItemIsHidden(item);

  return true;
}

bool
CQChartsPlot::
getPropertyIsStyle(const QString &name, bool &is_style) const
{
  is_style = false;

  const auto *item = propertyModel()->propertyItem(this, name, /*hidden*/true);
  if (! item) return false;

  is_style = CQCharts::getItemIsStyle(item);

  return true;
}

CQPropertyViewItem *
CQChartsPlot::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  CQCHARTS_QASSERT(CQUtil::hasProperty(object, name), QString("No property '" + name + "'"));

  return propertyModel()->addProperty(path, object, name, alias);
}

void
CQChartsPlot::
propertyItemSelected(QObject *obj, const QString &)
{
  startSelection();

  view()->deselectAll();

  bool changed = false;

  if      (obj == this) {
    if (isSelectable()) {
      setSelected(true);

      if (isVisible())
        view()->setCurrentPlot(this);

      drawObjs();

      changed = true;
    }
  }
  else if (obj == title()) {
    if (title()->isSelectable()) {
      title()->setSelected(true);

      drawForeground();

      changed = true;
    }
  }
  else if (obj == key()) {
    if (key()->isSelectable()) {
      key()->setSelected(true);

      drawBackground();
      drawForeground();

      changed = true;
    }
  }
  else if (obj == xAxis()) {
    if (xAxis()->isSelectable()) {
      xAxis()->setSelected(true);

      drawBackground();
      drawForeground();

      changed = true;
    }
  }
  else if (obj == yAxis()) {
    if (yAxis()->isSelectable()) {
      yAxis()->setSelected(true);

      drawBackground();
      drawForeground();

      changed = true;
    }
  }
  else {
    for (const auto &annotation : annotations()) {
      if (! annotation->isVisible() || ! annotation->isSelectable())
        continue;

      if (obj == annotation) {
        annotation->setSelected(true);

        drawForeground();

        changed = true;
      }
    }

    if (! changed) {
      for (auto &plotObj : plotObjects()) {
        if (! plotObj->isSelectable())
          continue;

        if (obj == plotObj) {
          plotObj->setSelected(true);
          changed = true;
        }
      }
    }
  }

  endSelection();

  //---

  if (changed)
    invalidateOverlay();
}

void
CQChartsPlot::
getPropertyNames(QStringList &names, bool hidden) const
{
  propertyModel()->objectNames(this, names, hidden);

  auto getObjPropertyNames = [&](std::initializer_list<Obj *> objs) {
    for (const auto &obj : objs) {
      if (obj)
        propertyModel()->objectNames(obj, names, hidden);
    }
  };

  getObjPropertyNames({title(), xAxis(), yAxis(), key()});
}

void
CQChartsPlot::
getObjectPropertyNames(PlotObj *plotObj, QStringList &names) const
{
  plotObj->getPropertyNames(names);
}

bool
CQChartsPlot::
getObjectProperty(PlotObj *plotObj, const QString &name, QVariant &value) const
{
  return plotObj->getProperty(name, value);
}

void
CQChartsPlot::
hideProperty(const QString &path, QObject *object)
{
  propertyModel()->hideProperty(path, object);
}

//------

void
CQChartsPlot::
addAxes()
{
  addXAxis();
  addYAxis();
}

void
CQChartsPlot::
addXAxis()
{
  xAxis_ = std::make_unique<Axis>(this, Qt::Horizontal, 0.0, 1.0);

  xAxis_->setObjectName("xaxis");
}

void
CQChartsPlot::
addYAxis()
{
  yAxis_ = std::make_unique<Axis>(this, Qt::Vertical, 0.0, 1.0);

  yAxis_->setObjectName("yaxis");
}

//------

void
CQChartsPlot::
setAxisColumnLabels(Axis *axis)
{
  auto *details = columnDetails(axis->column());

  axis->clearTickLabels();

  if (! details || details->isNumeric()) {
    axis->setMajorIncrement(0);
    axis->setValueType(CQChartsAxisValueType::real());
  }
  else {
    axis->setMajorIncrement(1);
    axis->setValueType(CQChartsAxisValueType::integer());

    for (int i = 0; i < details->numUnique(); ++i)
      axis->setTickLabel(i, details->uniqueValue(i).toString());
  }
}

//------

void
CQChartsPlot::
addKey()
{
  keyObj_ = std::make_unique<PlotKey>(this);
}

void
CQChartsPlot::
resetKeyItems(bool add)
{
  CQPerfTrace trace("CQChartsPlot::resetKeyItems");

  if (isOverlay()) {
    // if first plot then add all chained plot items to this plot's key
    if (isFirstPlot())
      processOverlayPlots([&](Plot *plot) {
        resetPlotKeyItems(plot, add);
      });
  }
  else if (parentPlot()) {
    parentPlot()->resetPlotKeyItems(this, add);
  }
  else {
    resetPlotKeyItems(this, add);
  }
}

void
CQChartsPlot::
resetPlotKeyItems(CQChartsPlot *plot, bool add)
{
  if (isOverlay()) {
    auto *key = firstPlot()->key();
    if (! key) return;

    if (plot->isFirstPlot())
      key->clearItems();

    if (add)
      plot->doAddKeyItems(key);
  }
  else {
    auto *key = plot->key();
    if (! key) return;

    key->clearItems();

    if (add)
      plot->doAddKeyItems(key);
  }
}

void
CQChartsPlot::
doAddKeyItems(PlotKey *key)
{
  if (! currentModel().data())
    return;

  // add key items from color column
  if (isColorKey()) {
    if (addColorKeyItems(key))
      return;
  }

  addKeyItems(key);
}

bool
CQChartsPlot::
addColorKeyItems(PlotKey *key)
{
  int row = (! key->isHorizontal() ? key->maxRow() : 0);
  int col = (! key->isHorizontal() ? 0 : key->maxCol());

  auto addKeyRow = [&](const QString &name, const QColor &c) {
    auto *colorItem = new CQChartsColorBoxKeyItem(this, ColorInd(), ColorInd(), ColorInd());
    auto *textItem  = new CQChartsTextKeyItem    (this, name, ColorInd());

    auto *groupItem = new CQChartsGroupKeyItem(this);

    groupItem->addRowItems(colorItem, textItem);

    if (c.isValid())
      colorItem->setColor(Color(c));

    if (! key->isHorizontal()) {
      //key->addItem(colorItem, row, 0);
      //key->addItem(textItem , row, 1);

      key->addItem(groupItem, row, 0);

      ++row;
    }
    else {
      //key->addItem(colorItem, 0, col++);
      //key->addItem(textItem , 0, col++);

      key->addItem(groupItem, 0, col++);
    }

    return colorItem;
  };

  auto colorColumn1 = this->colorColumn();

  if (! colorColumn1.isValid())
    return false;

  auto *columnDetails = this->columnDetails(colorColumn1);
  if (! columnDetails) return false;

  auto uniqueValues = columnDetails->uniqueValues();

  for (auto &value : uniqueValues) {
    QString name;

    if (! CQChartsVariant::toString(value, name))
      name = value.toString();

    Color color;

    columnValueColor(value, color, colorColumn1);

    auto c = interpColor(color, ColorInd());

    auto *colorItem = addKeyRow(name, c);

    colorItem->setValue(value);
  }

  return true;
}

//------

void
CQChartsPlot::
addTitle()
{
  assert(! titleObj_);

  titleObj_ = std::make_unique<CQChartsTitle>(this);

  title()->setTextStr(titleStr());
}

//------

void
CQChartsPlot::
addColorMapKey()
{
  assert(! colorMapKey_);

  colorMapKey_ = std::make_unique<CQChartsColorMapKey>(this);

  colorMapKey_->setVisible(false);
  colorMapKey_->setAlign(Qt::AlignLeft | Qt::AlignBottom);

  colorMapKey_->connectDisconnectDataChanged(true, this, SLOT(updateSlot()));

  connect(colorMapKey_.get(), SIGNAL(itemSelected(const QColor &, bool)),
          this, SLOT(colorSelected(const QColor &, bool)));

  mapKeys_.push_back(colorMapKey_.get());

  //---

  registerSlot("show_color_key", QStringList() << "bool");
}

bool
CQChartsPlot::
isColorMapKey() const
{
  assert(colorMapKey_);

  return colorMapKey_->isVisible();
}

void
CQChartsPlot::
setColorMapKey(bool b)
{
  assert(colorMapKey_);

  if (b != colorMapKey_->isVisible()) {
    colorMapKey_->setVisible(b);

    drawObjs();
  }
}

void
CQChartsPlot::
addColorMapKeyProperties()
{
  auto colorMapKeyPath = QString("mapKeys/color");

  addProp(colorMapKeyPath, "colorMapKey", "visible", "Color key visible");

  colorMapKey_->addProperties(propertyModel(), colorMapKeyPath);
}

bool
CQChartsPlot::
canDrawColorMapKey() const
{
  auto colorColumn = calcColorMapColumn();

  return (colorMapKey_ && colorColumn.isValid());
}

void
CQChartsPlot::
drawColorMapKey(PaintDevice *device) const
{
  auto colorColumn = calcColorMapColumn();

  if (! colorColumn.isValid())
    return;

  //---

  updateColorMapKey();

  colorMapKey_->draw(device);

  //---

  auto *th = const_cast<CQChartsPlot *>(this);

  auto colorMapKeyPath = QString("mapKeys.color");

  colorMapKey_->updateProperties(th->propertyModel(), colorMapKeyPath);
}

void
CQChartsPlot::
updateMapKey(MapKey *key) const
{
  if (key == colorMapKey())
    updateColorMapKey();
}

void
CQChartsPlot::
updateColorMapKey() const
{
  bool isReal     = false;
  bool isIntegral = false;
  bool isColor    = false;
  bool isMapped   = false;

  int          numUnique = 0;
  QVariantList uniqueValues;
  QVariantList uniqueCounts;

  auto colorColumn = calcColorMapColumn();

  auto *columnDetails = (colorColumn.isValid() ? this->columnDetails(colorColumn) : nullptr);

  if (columnDetails) {
    isMapped = colorColumnData_.isMapped();

    if      (columnDetails->type() == CQBaseModelType::REAL)
      isReal = true;
    else if (columnDetails->type() == CQBaseModelType::INTEGER)
      isIntegral = true;
    else if (columnDetails->type() == CQBaseModelType::COLOR)
      isColor = true;

    if (! isReal) {
      numUnique    = columnDetails->numUnique();
      uniqueValues = columnDetails->uniqueValues();
      uniqueCounts = columnDetails->uniqueCounts();
    }
  }

  //---

  QVariantList uniqueLabels;

  if (isColor && colorLabelColumn().isValid()) {
    auto *columnDetails1 = this->columnDetails(colorLabelColumn());

    using ColorNameMap = std::map<QVariant, QVariant, CQChartsVariant::VariantCmp>;

    ColorNameMap colorNameMap;

    uint n = std::min(columnDetails->valueCount(), columnDetails1->valueCount());

    bool unique = true;

    for (uint i = 0; i < n; ++i) {
      const auto &value  = columnDetails ->value(i);
      const auto &value1 = columnDetails1->value(i);

      auto pc = colorNameMap.find(value);
      if (pc != colorNameMap.end()) unique = false;

      colorNameMap[value] = value1;
    }

    if (unique) {
      for (const auto &p : colorNameMap)
        uniqueLabels.push_back(p.second);
    }
  }

  //---

#if 0
  if (hasSkipColors()) {
    int          numUnique1 = 0;
    QVariantList uniqueValues1;
    QVariantList uniqueCounts1;

    bool changed = false;

    int iv = 0;

    for (const auto &v : uniqueValues) {
      auto n = uniqueCounts[iv].toInt();

      if (int(numSkipColors(v)) != n) {
        ++numUnique1;

        uniqueValues1.push_back(v);
        uniqueCounts1.push_back(n);

        changed = true;
      }

      ++iv;
    }

    if (changed) {
      numUnique    = numUnique1;
      uniqueValues = uniqueValues1;
      uniqueCounts = uniqueCounts1;
    }
  }
#endif

  //---

  auto *th = const_cast<CQChartsPlot *>(this);

  CQUtil::AutoDisconnect autoDisconnect(colorMapKey_.get(),
    SIGNAL(dataChanged()), th, SLOT(updateSlot()));

  colorMapKey_->setData(colorColumnData());

  colorMapKey_->setNumeric     (isReal || isIntegral);
  colorMapKey_->setIntegral    (isIntegral);
  colorMapKey_->setNative      (isColor);
  colorMapKey_->setMapped      (isMapped);
  colorMapKey_->setNumUnique   (numUnique);
  colorMapKey_->setUniqueValues(uniqueValues);
  colorMapKey_->setUniqueCounts(uniqueCounts);
  colorMapKey_->setUniqueLabels(uniqueLabels);
}

void
CQChartsPlot::
addColorMapKeyItems(QMenu *menu)
{
  auto *keysMenu = new QMenu("Keys", menu);

  addColorMapKeySubItems(keysMenu);

  menu->addMenu(keysMenu);
}

void
CQChartsPlot::
addColorMapKeySubItems(QMenu *keysMenu)
{
  auto addSubMenu = [](QMenu *menu, const QString &name) {
    auto *subMenu = new QMenu(name, menu);

    menu->addMenu(subMenu);

    return subMenu;
  };

  //---

  auto *colorKeyMenu = addSubMenu(keysMenu, "Color Key");

  addMenuCheckedAction(colorKeyMenu, "Visible", isColorMapKey(), SLOT(setColorMapKey(bool)));

  //---

  view()->addKeyLocationActions(colorKeyMenu, colorMapKey_->location(),
                                this, SLOT(colorMapKeyPositionSlot(QAction *)),
                                /*includeAuto*/false);

  //---

  bool insideXChecked = colorMapKey_->isInsideX();
  bool insideYChecked = colorMapKey_->isInsideY();

  (void) addMenuCheckedAction(colorKeyMenu, "Inside X", insideXChecked,
                              SLOT(colorMapKeyInsideXSlot(bool)));
  (void) addMenuCheckedAction(colorKeyMenu, "Inside Y", insideYChecked,
                              SLOT(colorMapKeyInsideYSlot(bool)));
}

void
CQChartsPlot::
colorMapKeyPositionSlot(QAction *action)
{
  CQChartsKeyLocation::Type location;

  if (! CQChartsKeyLocation::decodeString(action->text(), location))
    assert(false);

  colorMapKey_->setLocation(CQChartsKeyLocation(location));
}

void
CQChartsPlot::
colorMapKeyInsideXSlot(bool b)
{
  if (b != colorMapKey_->isInsideX())
    colorMapKey_->setInsideX(b);
}

void
CQChartsPlot::
colorMapKeyInsideYSlot(bool b)
{
  if (b != colorMapKey_->isInsideY())
    colorMapKey_->setInsideY(b);
}

CQChartsColumn
CQChartsPlot::
calcColorMapColumn() const
{
  if (colorMapColumn().isValid())
    return colorMapColumn();

  return colorColumn();
}

//---

void
CQChartsPlot::
colorSelected(const QColor &color, bool visible)
{
  if (! visible)
    colorFilter_.insert(Color(color));
  else
    colorFilter_.erase(Color(color));

  updateRangeAndObjs();
}

bool
CQChartsPlot::
colorVisible(const QColor &color) const
{
  auto p = colorFilter_.find(Color(color));

  return (p == colorFilter_.end());
}

QStringList
CQChartsPlot::
colorFilterNames() const
{
  CQColorsPalette *colorsPalette = nullptr;

  if (colorColumnData_.palette().isValid())
    colorsPalette = colorColumnData_.palette().palette();
  else
    colorsPalette = view()->themePalette();

  int n = (colorMapKey_ ? colorMapKey_->uniqueValues().size() : 0);

  auto mapMin = colorColumnData_.mapMin();
  auto mapMax = colorColumnData_.mapMax();

  using ColorName = std::map<Color, QString>;

  ColorName colorName;

  for (int i = 0; i < n; ++i) {
    auto name = colorMapKey_->uniqueValues().at(i).toString();

    Color color;

    if (! colorColumnData_.colorMap().valueToColor(name, color)) {
      double r = CMathUtil::map(i, 0, n - 1, mapMin, mapMax);

      color = Color(colorsPalette->getColor(r));
    }

    colorName[color] = name;
  }

  QStringList names;

  for (const auto &s : colorFilter_) {
    auto p = colorName.find(s);

    if (p != colorName.end())
      names << (*p).second;
  }

  return names;
}

void
CQChartsPlot::
setColorFilterNames(const QStringList &names)
{
  colorFilter_.clear();

  for (const auto &name : names) {
    Color color;

    if (colorColumnData_.colorMap().valueToColor(name, color))
      colorFilter_.insert(color);
  }

  updateRangeAndObjs();
}

//------

void
CQChartsPlot::
addFilterColumn(const Column &column)
{
  filterColumns_.insert(column);
}

void
CQChartsPlot::
removeFilterColumn(const Column &column)
{
  filterColumns_.erase(column);
}

bool
CQChartsPlot::
isValueVisible(int row, const QModelIndex &parent) const
{
  if (filterColumns_.empty())
    return true;

  for (const auto &c : filterColumns_) {
    auto ind = ModelIndex(this, row, c, parent);

    bool ok;
    auto var = modelValue(ind, ok);
    if (! ok || ! var.isValid()) continue;

    if (! isColumnValueVisible(c, var))
      return false;
  }

  return true;
}

void
CQChartsPlot::
setColumnValueVisible(const Column &column, const QVariant &value, bool visible)
{
  auto p = columnValueFilter_.find(column);

  if (p == columnValueFilter_.end())
    p = columnValueFilter_.emplace_hint(p, column, VariantSet());

  if (visible)
    (*p).second.erase(value);
  else
    (*p).second.insert(value);
}

bool
CQChartsPlot::
isColumnValueVisible(const Column &column, const QVariant &value) const
{
  // visible if not in hidden value list
  auto p = columnValueFilter_.find(column);
  if (p == columnValueFilter_.end()) return true;

  auto pv = (*p).second.find(value);
  if (pv == (*p).second.end()) return true;

  return false;
}

//------

void
CQChartsPlot::
threadTimerSlot()
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->startThreadTimer();

  if (parentPlot())
    return parentPlot()->startThreadTimer();

  //---

  assert(! parentPlot());

  auto updateState = this->updateState();
  auto nextState   = UpdateState::INVALID;
  bool updateView  = false;

  {
  TryLockMutex lock(this, "threadTimerSlot");

  if (! lock.locked)
    return;

  //---

  if (isUpdatesEnabled())
    nextState = calcNextState();

  //---

  // ensure threads state is up to date (thread may have finished)

  if      (updateState == UpdateState::CALC_RANGE) {
    // check if calc range result ready
    if (updateData_.rangeThread->isReady()) {
      // if ready then mark finished (handled)
      updateData_.rangeThread->finish();

      // ensure all plot ranges done
      waitRange();

      // need post update range
      applyDataRange();

      // post update range
      postUpdateRange();

      // reset state
      setGroupedUpdateState(UpdateState::INVALID);
    }
    // update range running so redraw view (busy)
    else {
      updateView = true;
    }
  }
  else if (updateState == UpdateState::CALC_OBJS) {
    // check if calc objs result ready
    if (updateData_.objsThread->isReady()) {
      // if ready then mark finished (handled)
      updateData_.objsThread->finish();

      // ensure all plot objs done
      waitObjs();

      // post update objs
      postUpdateObjs();

      // reset state
      setGroupedUpdateState(UpdateState::INVALID);
    }
    // update objs running so redraw view (busy)
    else {
      updateView = true;
    }
  }
  else if (updateState == UpdateState::DRAW_OBJS) {
    // check if draw objs result ready
    if (updateData_.drawThread->isReady()) {
      // if ready then mark finished (handled)
      updateData_.drawThread->finish();

      // ensure all plot draw done
      waitDraw();

      // post draw
      postDraw();

      // move to ready state (all done)
      setGroupedUpdateState(UpdateState::READY);

      // need draw
      updateView = true;
    }
    // draw running so redraw view (busy)
    else {
      updateView = true;
    }
  }
  else if (updateState == UpdateState::INVALID) {
    //if (nextState == UpdateState::INVALID) {
    //  std::unique_lock<std::mutex> lock(updatesMutex_);

    //  ++updatesData_.stateFlag[UpdateState::UPDATE_RANGE    ];
    //  ++updatesData_.stateFlag[UpdateState::UPDATE_OBJS     ];
    //  ++updatesData_.stateFlag[UpdateState::UPDATE_DRAW_OBJS];

    //  nextState = UpdateState::UPDATE_RANGE;
    //}
  }
  else if (updateState == UpdateState::DRAWN) {
    waitTree();
  }
  else {
    updateView = true;

    setGroupedUpdateState(nextState);
  }
  }

  //---

  doPostObjTree();

  //---

  if      (nextState == UpdateState::UPDATE_RANGE) {
    {
    std::unique_lock<std::mutex> lock(updatesMutex_);

    updatesData_.stateFlag[UpdateState::UPDATE_RANGE] = 0;
    }

    this->execUpdateRange();
  }
  else if (nextState == UpdateState::UPDATE_OBJS) {
    // don't update until range calculated
    if (updateState != UpdateState::CALC_RANGE) {
      {
      std::unique_lock<std::mutex> lock(updatesMutex_);

      updatesData_.stateFlag[UpdateState::UPDATE_OBJS] = 0;
      }

      this->execUpdateObjs();
    }
  }
  else if (nextState == UpdateState::UPDATE_DRAW_OBJS) {
    // don't update until range and objs calculated
    if (updateState != UpdateState::CALC_RANGE &&
        updateState != UpdateState::CALC_OBJS) {
      {
      std::unique_lock<std::mutex> lock(updatesMutex_);

      updatesData_.stateFlag[UpdateState::UPDATE_DRAW_OBJS      ] = 0;
      updatesData_.stateFlag[UpdateState::UPDATE_DRAW_BACKGROUND] = 0;
      updatesData_.stateFlag[UpdateState::UPDATE_DRAW_FOREGROUND] = 0;
      }

      this->execInvalidateLayers();

      updateView = true;
    }
  }
  else if (nextState == UpdateState::UPDATE_DRAW_BACKGROUND) {
    // don't update until objs drawn
    if (updateState != UpdateState::DRAW_OBJS) {
      {
      std::unique_lock<std::mutex> lock(updatesMutex_);

      updatesData_.stateFlag[UpdateState::UPDATE_DRAW_BACKGROUND] = 0;
      }

      this->invalidateLayer(Buffer::Type::BACKGROUND);

      this->invalidateOverlay();

      updateView = true;
    }
  }
  else if (nextState == UpdateState::UPDATE_DRAW_FOREGROUND) {
    // don't update until objs drawn
    if (updateState != UpdateState::DRAW_OBJS) {
      {
      std::unique_lock<std::mutex> lock(updatesMutex_);

      updatesData_.stateFlag[UpdateState::UPDATE_DRAW_FOREGROUND] = 0;
      }

      this->invalidateLayer(Buffer::Type::FOREGROUND);

      this->invalidateOverlay();

      updateView = true;
    }
  }

  //---

  if (updateView)
    view()->doUpdate();
}

void
CQChartsPlot::
doPostObjTree()
{
  assert(! isComposite());

  if (objTreeData_.notify) {
    objTreeData_.notify = false;

    postObjTree();
  }
}

CQChartsPlot::UpdateState
CQChartsPlot::
calcNextState() const
{
  auto *th = const_cast<Plot *>(this);

  auto nextState = UpdateState::INVALID;

  assert(! parentPlot());

  std::unique_lock<std::mutex> lock(updatesMutex_);

  // check queued updates to determine required state
  if      (th->updatesData_.stateFlag[UpdateState::UPDATE_RANGE] > 0) {
    if (debugUpdate_)
      std::cerr << "UpdateState::UPDATE_RANGE : " <<
        th->updatesData_.stateFlag[UpdateState::UPDATE_RANGE] << "\n";

    nextState = UpdateState::UPDATE_RANGE;
  }
  else if (th->updatesData_.stateFlag[UpdateState::UPDATE_OBJS] > 0) {
    if (debugUpdate_)
      std::cerr << "UpdateState::UPDATE_OBJS : " <<
        th->updatesData_.stateFlag[UpdateState::UPDATE_OBJS] << "\n";

    nextState = UpdateState::UPDATE_OBJS;
  }
  else if (th->updatesData_.stateFlag[UpdateState::UPDATE_DRAW_OBJS] > 0) {
    if (debugUpdate_)
      std::cerr << "UpdateState::UPDATE_DRAW_OBJS : " <<
        th->updatesData_.stateFlag[UpdateState::UPDATE_DRAW_OBJS] << "\n";

    nextState = UpdateState::UPDATE_DRAW_OBJS;
  }
  else if (th->updatesData_.stateFlag[UpdateState::UPDATE_DRAW_BACKGROUND] > 0) {
    if (debugUpdate_)
      std::cerr << "UpdateState::UPDATE_DRAW_BACKGROUND : " <<
        th->updatesData_.stateFlag[UpdateState::UPDATE_DRAW_BACKGROUND] << "\n";

    nextState = UpdateState::UPDATE_DRAW_BACKGROUND;
  }
  else if (th->updatesData_.stateFlag[UpdateState::UPDATE_DRAW_FOREGROUND] > 0) {
    if (debugUpdate_)
      std::cerr << "UpdateState::UPDATE_DRAW_FOREGROUND : " <<
        th->updatesData_.stateFlag[UpdateState::UPDATE_DRAW_FOREGROUND] << "\n";

    nextState = UpdateState::UPDATE_DRAW_FOREGROUND;
  }

  return nextState;
}

void
CQChartsPlot::
setGroupedUpdateState(UpdateState state)
{
  if (isOverlay()) {
    processOverlayPlots([&](Plot *plot) {
      plot->setUpdateState(state);
    });
  }
  else {
    setUpdateState(state);
  }
}

void
CQChartsPlot::
setUpdateState(UpdateState state)
{
  assert(! parentPlot());

  updateData_.state.store(static_cast<int>(state));

  if (debugUpdate_) {
    std::cerr << "State: " << id().toStdString() << ": ";

    auto updateState = this->updateState();

    switch (updateState) {
      case UpdateState::CALC_RANGE:       std::cerr << "Calc Range\n"; break;
      case UpdateState::CALC_OBJS:        std::cerr << "Calc Objs\n"; break;
      case UpdateState::DRAW_OBJS:        std::cerr << "Draw Objs\n"; break;
      case UpdateState::READY:            std::cerr << "Ready\n"; break;
      case UpdateState::UPDATE_RANGE:     std::cerr << "Update Range\n"; break;
      case UpdateState::UPDATE_OBJS:      std::cerr << "Update Objs\n"; break;
      case UpdateState::UPDATE_DRAW_OBJS: std::cerr << "Update Draw Objs\n"; break;
      case UpdateState::UPDATE_VIEW:      std::cerr << "Update View\n"; break;
      case UpdateState::DRAWN:            std::cerr << "Drawn\n"; break;
      default:                            std::cerr << "Invalid\n"; break;
    }
  }
}

void
CQChartsPlot::
setInterrupt(bool b)
{
  if (parentPlot())
    return parentPlot()->setInterrupt(b);

  //---

  assert(! parentPlot());

  if (b)
    updateData_.interrupt++;
  else {
    assert(updateData_.interrupt.load() > 0);

    updateData_.interrupt--;
  }
}

bool
CQChartsPlot::
isReady() const
{
  if (! isBufferLayers())
    return true;

  auto updateState = const_cast<Plot *>(this)->updateState();

  return (updateState == UpdateState::READY || updateState == UpdateState::DRAWN);
}

//------

void
CQChartsPlot::
clearRangeAndObjs()
{
  resetRange();

  clearPlotObjects();

  clearInsideObjects();
}

void
CQChartsPlot::
execUpdateRangeAndObjs()
{
  if (! isUpdatesEnabled()) {
    assert(! parentPlot());

    std::unique_lock<std::mutex> lock(updatesMutex_);

    updatesData_.updateRangeAndObjs = true;

    return;
  }

  updateAndApplyRange(/*apply*/true, /*updateObjs*/true);
}

//------

void
CQChartsPlot::
execUpdateRange()
{
  updateAndApplyRange(/*apply*/true, /*updateObjs*/false);
}

void
CQChartsPlot::
updateAndApplyRange(bool apply, bool updateObjs)
{
  CQPerfTrace trace("CQChartsPlot::updateAndApplyRange");

  // update overlay objects
  if (isOverlay()) {
    if (! isFirstPlot())
      return;

    processOverlayPlots([&](Plot *plot) {
      plot->updateAndApplyPlotRange(apply, updateObjs);
    });
  }
  else {
    updateAndApplyPlotRange(apply, updateObjs);
  }
}

void
CQChartsPlot::
updateAndApplyPlotRange(bool apply, bool updateObjs)
{
  if (apply) {
    updateAndApplyPlotRange1(updateObjs);
  }
  else {
    updateRangeThread();

    if (updateObjs)
      this->updateObjs();
  }
}

void
CQChartsPlot::
updateAndApplyPlotRange1(bool updateObjs)
{
  if (! isSequential()) {
    startCalcRange(updateObjs);
  }
  else {
    updateRangeThread();

    if (updateObjs) {
      // calc range
      applyDataRange();

      postUpdateRange();

      //---

      // add objects
      this->updateObjs();

      postUpdateObjs();
    }

    // draw objects
    drawObjs();

    postDraw();
  }
}

void
CQChartsPlot::
startCalcRange(bool updateObjs)
{
  if (parentPlot())
    parentPlot()->startCalcRange(updateObjs);

  //---

  LockMutex lock1(this, "startCalcRange");

  // finish current threads
  interruptRange();

  //---

  assert(! parentPlot());

  // start update range thread
  updateData_.updateObjs   = updateObjs;
  updateData_.drawBusy.ind = -100;

  setGroupedUpdateState(UpdateState::CALC_RANGE);

  updateData_.rangeThread->exec(updateRangeASync, this);

  startThreadTimer();
}

//---

void
CQChartsPlot::
interruptRange()
{
  if (parentPlot())
    return parentPlot()->interruptRange();

  //---

  setInterrupt(true);

  waitRange();
  waitObjs ();
  waitDraw ();

  setInterrupt(false);
}

//---

#if 0
void
CQChartsPlot::
waitDataRange()
{
  // ensure all overlay plot ranges done
  if (isOverlay())
    waitRange();
}

void
CQChartsPlot::
waitCalcObjs()
{
  // ensure all overlay plot objs done
  if (isOverlay())
    waitObjs();
}

void
CQChartsPlot::
waitDrawObjs()
{
  // ensure all overlay draw done
  if (isOverlay())
    waitDraw();
}
#endif

//---

void
CQChartsPlot::
syncAll()
{
  if (debugUpdate_)
    std::cerr << "CQChartsPlot::syncAll\n";

  syncRange();
  syncObjs ();
  syncDraw ();
}

void
CQChartsPlot::
syncState()
{
  if (debugUpdate_)
    std::cerr << "CQChartsPlot::syncState\n";

  if (isOverlay() && ! isFirstPlot())
    return;

  auto updateState = this->updateState();

  while (updateState == UpdateState::INVALID) {
    if (isInterrupt())
      return;

    if (hasLockId())
      return;

    threadTimerSlot();

    updateState = this->updateState();
  }
}

void
CQChartsPlot::
syncRange()
{
  if (debugUpdate_)
    std::cerr << "CQChartsPlot::syncRange\n";

  syncState();

  waitRange();
}

void
CQChartsPlot::
waitRange()
{
  if (debugUpdate_)
    std::cerr << "CQChartsPlot::waitRange\n";

  if (isOverlay()) {
    processOverlayPlots([&](Plot *plot) {
      plot->execWaitRange();
    });
  }
  else {
    execWaitRange();
  }
}

void
CQChartsPlot::
execWaitRange()
{
  if (debugUpdate_)
    std::cerr << "CQChartsPlot::execWaitRange\n";

  auto updateState = this->updateState();

  while (updateState == UpdateState::CALC_RANGE) {
    assert(! parentPlot());

    // if busy wait for range thread to finish
    if (updateData_.rangeThread->isBusy()) {
      (void) updateData_.rangeThread->term();
      return;
    }

    if (isInterrupt())
      return;

    if (hasLockId())
      return;

    if (! isFirstPlot())
      break;

    threadTimerSlot();

    updateState = this->updateState();
  }
}

void
CQChartsPlot::
updateRangeASync(Plot *plot)
{
  plot->updateRangeThread();
}

void
CQChartsPlot::
updateRangeThread()
{
  CQPerfTrace trace("CQChartsPlot::updateRangeThread");

  //---

  updateAndAdjustRanges();

  //---

  assert(! parentPlot());

  // mark thread done
  updateData_.rangeThread->end();
}

void
CQChartsPlot::
updateAndAdjustRanges()
{
  assert(! isComposite());

  if (isOverlay())
    clearOverlayErrors();

  resetExtraFitBBox();

  //---

  initRange();

  if (currentModel().data()) {
    calcDataRange_ = calcRange();
  }
  else {
    calcDataRange_ = Range();

    calcDataRange_.makeNonZero();
  }

  unequalDataRange_ = adjustDataRange(getCalcDataRange());

  // adjust to equal scale
  dataRange_ = unequalDataRange_;

  applyEqualScale(dataRange_);

  outerDataRange_ = dataRange_;

  postCalcRange();

  //---

  if (isOverlay())
    updateOverlayRanges();

  //---

  scrollData_.invalid = true;
}

//------

void
CQChartsPlot::
updateGroupedObjs()
{
  if (isOverlay()) {
    processOverlayPlots([&](Plot *plot) {
      plot->execUpdateObjs();
    });
  }
  else {
    execUpdateObjs();
  }
}

void
CQChartsPlot::
execUpdateObjs()
{
  if (! isUpdatesEnabled()) {
    assert(! parentPlot());

    std::unique_lock<std::mutex> lock(updatesMutex_);

    updatesData_.updateObjs = true;

    return;
  }

  //---

  if (! dataRange().isSet()) {
    execUpdateRangeAndObjs();
    return;
  }

  // update overlay objects
  if (isOverlay()) {
    if (! isFirstPlot())
      return;

    processOverlayPlots([&](Plot *plot) {
      plot->updatePlotObjs();
    });
  }
  else {
    updatePlotObjs();
  }
}

void
CQChartsPlot::
updatePlotObjs()
{
  if (! isSequential()) {
    startCalcObjs();
  }
  else {
    // add objs
    // TODO: non threaded version ?
    updateObjsThread();

    postUpdateObjs();

    //---

    // draw objects
    drawObjs();

    postDraw();
  }
}

void
CQChartsPlot::
startCalcObjs()
{
  if (parentPlot())
    parentPlot()->startCalcObjs();

  //---

  LockMutex lock(this, "startCalcObjs");

  //---

  assert(! parentPlot());

  auto updateState = this->updateState();

  // if calc range still running then run update objects after finished
  if (updateState == UpdateState::CALC_RANGE) {
    if (isOverlay())
      firstPlot()->updateData_.updateObjs = true;
    //return;
  }

  //---

  // finish update objs thread
  interruptObjs();

  //---

  // start update objs thread
  updateData_.drawBusy.ind = -100;

  setGroupedUpdateState(UpdateState::CALC_OBJS);

  updateData_.objsThread->exec(updateObjsASync, this);

  startThreadTimer();
}

//---

void
CQChartsPlot::
interruptObjs()
{
  if (parentPlot())
    return parentPlot()->interruptObjs();

  //---

  setInterrupt(true);

  waitObjs();
  waitDraw();

  setInterrupt(false);
}

void
CQChartsPlot::
syncObjs()
{
  if (debugUpdate_)
    std::cerr << "CQChartsPlot::syncObjs\n";

  syncState();

  waitRange();
  waitObjs ();
}

void
CQChartsPlot::
waitObjs()
{
  if (debugUpdate_)
    std::cerr << "CQChartsPlot::waitObjs\n";

  if (isOverlay()) {
    processOverlayPlots([&](Plot *plot) {
      plot->execWaitObjs();
    });
  }
  else {
    execWaitObjs();
  }
}

void
CQChartsPlot::
execWaitObjs()
{
  if (debugUpdate_)
    std::cerr << "CQChartsPlot::execWaitObjs\n";

  assert(! parentPlot());

  auto updateState = this->updateState();

  while (updateState == UpdateState::CALC_OBJS) {
    // if busy wait for objs thread to finish
    if (updateData_.objsThread->isBusy()) {
      (void) updateData_.objsThread->term();
      return;
    }

    if (isInterrupt())
      return;

    if (hasLockId())
      return;

    if (! isFirstPlot())
      break;

    threadTimerSlot();

    updateState = this->updateState();
  }
}

void
CQChartsPlot::
updateObjsASync(Plot *plot)
{
  plot->updateObjsThread();
}

void
CQChartsPlot::
updateObjsThread()
{
  CQPerfTrace trace("CQChartsPlot::updateObjsThread");

  //---

  clearPlotObjects();

  clearInsideObjects();

  initColorColumnData();

  initPlotObjs();

  //---

  assert(! parentPlot());

  // mark thread done
  updateData_.objsThread->end();
}

void
CQChartsPlot::
postInit()
{
}

CQChartsGeom::BBox
CQChartsPlot::
calcDataRange(bool adjust) const
{
  auto bbox = getDataRange();

  // if zoom data, adjust bbox by pan offset, zoom scale
  if (adjust)
    bbox = adjustDataRangeBBox(bbox);

  return bbox;
}

void
CQChartsPlot::
calcDataRanges(BBox &rawRange, BBox &adjustedRange) const
{
  rawRange = getDataRange();

  // if zoom data, adjust bbox by pan offset, zoom scale
  adjustedRange = adjustDataRangeBBox(rawRange);
}

CQChartsGeom::BBox
CQChartsPlot::
adjustDataRangeBBox(const BBox &bbox) const
{
  assert(! isComposite());

  // get center and x/y sizes
  auto c = bbox.getCenter();

  double bw = bbox.getWidth ();
  double bh = bbox.getHeight();

  // adjust by scale
  double w = 0.5*bw/dataScaleX();
  double h = 0.5*bh/dataScaleY();
  double x = c.x + bw*dataOffsetX();
  double y = c.y + bh*dataOffsetY();

  // calc scaled/offset bbox
  auto bbox1 = BBox(x - w, y - h, x + w, y + h);

  //----

  // save original range
  auto displayRange = this->displayRange();

  // update to calculated range
  auto dataRange = calcDataRange(/*adjust*/false);

  auto *th = const_cast<Plot *>(this);

//th->setWindowRange(dataRange);
  th->displayRange_->setWindowRange(dataRange.getXMin(), dataRange.getYMin(),
                                    dataRange.getXMax(), dataRange.getYMax());

  //--

  // adjust range to margin
  BBox ibbox;

  if (isOverlay()) {
    const auto &innerMargin = firstPlot()->innerMargin();

    ibbox = innerMargin.adjustPlotRange(this, bbox1, /*inside*/true);
  }
  else {
    ibbox = innerMargin().adjustPlotRange(this, bbox1, /*inside*/true);
  }

  //--

  // restore original range
  th->setDisplayRange(displayRange);

  return ibbox;
}

CQChartsGeom::BBox
CQChartsPlot::
getDataRange() const
{
  if (dataRange().isSet())
    return CQChartsUtil::rangeBBox(dataRange());

  return BBox(0.0, 0.0, 1.0, 1.0);
}

//---

bool
CQChartsPlot::
isApplyDataRange() const
{
  assert(! parentPlot());

  return updatesData_.applyDataRange;
}

void
CQChartsPlot::
applyDataRangeAndDraw()
{
  applyDataRange();

  drawObjs();

  if (! isOverlay()) {
    if (isX1X2() || isY1Y2()) {
      auto *plot1 = firstPlot();

      while (plot1) {
        if (plot1 != this)
          plot1->drawObjs();

        plot1 = plot1->nextPlot();
      }
    }
  }
}

void
CQChartsPlot::
applyDataRange(bool propagate)
{
  if (propagate) {
    if (isOverlay() && ! isFirstPlot()) {
      return firstPlot()->applyDataRange(propagate);
    }
  }

  //---

  if (! isUpdatesEnabled()) {
    assert(! parentPlot());

    std::unique_lock<std::mutex> lock(updatesMutex_);

    updatesData_.applyDataRange = true;

    return;
  }

  if (! dataRange().isSet()) {
    assert(! parentPlot());

    std::unique_lock<std::mutex> lock(updatesMutex_);

    updatesData_.applyDataRange = true;

    return;
  }

  //---

  BBox rawRange, adjustedRange;

  if (propagate) {
    if (isOverlay()) {
      if      (isX1X2()) {
        auto *plot1 = firstPlot();

        plot1->calcDataRanges(rawRange, adjustedRange);
      }
      else if (isY1Y2()) {
        auto *plot1 = firstPlot();

        plot1->calcDataRanges(rawRange, adjustedRange);
      }
      else {
        processOverlayPlots([&](Plot *plot) {
          //plot->resetDataRange(/*updateRange*/false, /*updateObjs*/false);
          //plot->updateAndApplyRange(/*apply*/false, /*updateObjs*/false);

          BBox rawRange1, adjustedRange1;

          plot->calcDataRanges(rawRange1, adjustedRange1);

          rawRange      += rawRange1;
          adjustedRange += adjustedRange1;
        });
      }
    }
    else {
      calcDataRanges(rawRange, adjustedRange);
    }
  }
  else {
    calcDataRanges(rawRange, adjustedRange);
  }

  if (isOverlay()) {
    if (! propagate) {
      if      (isX1X2()) {
        setWindowRange(rawRange, adjustedRange);
      }
      else if (isY1Y2()) {
        setWindowRange(rawRange, adjustedRange);
      }
    }
    else {
      // This breaks X1X2 and Y1Y2 plots (wrong range)
      if (! isX1X2() && ! isY1Y2()) {
        processOverlayPlots([&](Plot *plot) {
          plot->setWindowRange(rawRange, adjustedRange);
        });
      }
    }
  }
  else {
    setWindowRange(rawRange, adjustedRange);
  }

  if (propagate) {
    if (isOverlay()) {
      if      (isX1X2()) {
        auto dataRange1 = CQChartsUtil::bboxRange(rawRange);

        Plots plots;

        x1x2Plots(plots);

        int i = 0;

        for (auto &plot : plots) {
          if (i == 0) {
            plot->applyDataRange(/*propagate*/false);
          }
          else {
            //plot->resetDataRange(/*updateRange*/false, /*updateObjs*/false);
            //plot->updateAndApplyRange(/*apply*/false, /*updateObjs*/false);

            auto bbox2 = plot->calcDataRange(/*adjust*/false);

            auto dataRange2 = Range(bbox2.getXMin(), dataRange1.bottom(),
                                    bbox2.getXMax(), dataRange1.top   ());

            plot->setDataRange(dataRange2, /*update*/false);

            plot->applyDataRange(/*propagate*/false);
          }

          ++i;
        }
      }
      else if (isY1Y2()) {
        auto dataRange1 = CQChartsUtil::bboxRange(rawRange);

        Plots plots;

        y1y2Plots(plots);

        int i = 0;

        for (auto &plot : plots) {
          if (i == 0) {
            plot->applyDataRange(/*propagate*/false);
          }
          else {
            //plot->resetDataRange(/*updateRange*/false, /*updateObjs*/false);
            //plot->updateAndApplyRange(/*apply*/false, /*updateObjs*/false);

            auto bbox2 = plot->calcDataRange(/*adjust*/false);

            auto dataRange2 = Range(dataRange1.left (), bbox2.getYMin(),
                                    dataRange1.right(), bbox2.getYMax());

            plot->setDataRange(dataRange2, /*update*/false);

            plot->applyDataRange(/*propagate*/false);
          }

          ++i;
        }
      }
      else {
        auto dataRange1 = CQChartsUtil::bboxRange(rawRange);

        processOverlayPlots([&](Plot *plot) {
          plot->setDataRange(dataRange1, /*update*/false);

          plot->applyDataRange(/*propagate*/false);
        });
      }
    }
    else {
      if (isX1X2() || isY1Y2()) {
        auto dataRange1 = CQChartsUtil::bboxRange(rawRange);

        auto *plot1 = firstPlot();

        while (plot1) {
          if (plot1 != this) {
            auto bbox2 = plot1->calcDataRange(/*adjust*/false);

            if (isX1X2()) {
              double ymin = std::min(bbox2.getYMin(), dataRange1.bottom());
              double ymax = std::max(bbox2.getYMax(), dataRange1.top   ());

              Range dataRange1(rawRange.getXMin(), ymin, rawRange.getXMax(), ymax);
              Range dataRange2(bbox2   .getXMin(), ymin, bbox2   .getXMax(), ymax);

              this ->setDataRange(dataRange1, /*update*/false);
              plot1->setDataRange(dataRange2, /*update*/false);

              plot1->applyDataRange(/*propagate*/false);
            }
            else {
              double xmin = std::min(bbox2.getXMin(), dataRange1.left ());
              double xmax = std::max(bbox2.getXMax(), dataRange1.right());

              Range dataRange1(xmin, rawRange.getYMin(), xmax, rawRange.getYMax());
              Range dataRange2(xmin, bbox2   .getYMin(), xmax, bbox2   .getYMax());

              this ->setDataRange(dataRange1, /*update*/false);
              plot1->setDataRange(dataRange2, /*update*/false);

              plot1->applyDataRange(/*propagate*/false);
            }
          }

          plot1 = plot1->nextPlot();
        }
      }
    }
  }

  updateAxisRanges(adjustedRange);

  updateKeyPosition(/*force*/true);

  Q_EMIT rangeChanged();
}

void
CQChartsPlot::
updateAxisRanges(const BBox &adjustedRange)
{
  if (xAxis())
    xAxis()->setRange(adjustedRange.getXMin(), adjustedRange.getXMax());

  if (yAxis())
    yAxis()->setRange(adjustedRange.getYMin(), adjustedRange.getYMax());
}

void
CQChartsPlot::
clearOverlayErrors()
{
  if (! isFirstPlot())
    return firstPlot()->clearOverlayErrors();

  if (isOverlay()) {
    processOverlayPlots([&](Plot *plot) {
      plot->clearErrors();
    });
  }
}

void
CQChartsPlot::
updateOverlayRanges()
{
  if (! isFirstPlot())
    return firstPlot()->updateOverlayRanges();

  if (isOverlay()) {
    if (! isX1X2() && ! isY1Y2()) {
      Range dataRange;

      processOverlayPlots([&](Plot *plot) {
        dataRange += plot->dataRange();
      });

      processOverlayPlots([&](Plot *plot) {
        plot->setDataRange(dataRange, /*update*/false);

        plot->applyDataRange(/*propagate*/false);

        plot->postCalcRange();
      });
    }
  }
}

void
CQChartsPlot::
setPixelRange(const BBox &bbox)
{
  assert(! isComposite());

  displayRange_   ->setPixelRange(bbox.getXMin(), bbox.getYMax(), bbox.getXMax(), bbox.getYMin());
  rawDisplayRange_->setPixelRange(bbox.getXMin(), bbox.getYMax(), bbox.getXMax(), bbox.getYMin());
}

void
CQChartsPlot::
resetWindowRange()
{
  assert(! isComposite());

  displayRange_   ->setWindowRange(0.0, 0.0, 1.0, 1.0);
  rawDisplayRange_->setWindowRange(0.0, 0.0, 1.0, 1.0);
}

void
CQChartsPlot::
setWindowRange(const BBox &rawBBox, const BBox &bbox)
{
  assert(! isComposite());

  rawDisplayRange_->setWindowRange(rawBBox.getXMin(), rawBBox.getYMin(),
                                   rawBBox.getXMax(), rawBBox.getYMax());

  displayRange_->setWindowRange(bbox.getXMin(), bbox.getYMin(), bbox.getXMax(), bbox.getYMax());
}

CQChartsGeom::Range
CQChartsPlot::
adjustDataRange(const Range &calcDataRange) const
{
  auto dataRange = calcDataRange;

  // adjust data range to custom values
  if (xmin().isSet()) dataRange.setLeft  (xmin().real());
  if (ymin().isSet()) dataRange.setBottom(ymin().real());
  if (xmax().isSet()) dataRange.setRight (xmax().real());
  if (ymax().isSet()) dataRange.setTop   (ymax().real());

  //---

  // include zero
  if (xAxis() && xAxis()->isIncludeZero()) {
    if (dataRange.isSet())
      dataRange.updateRange(0, dataRange.ymid());
  }

  if (yAxis() && yAxis()->isIncludeZero()) {
    if (dataRange.isSet())
      dataRange.updateRange(dataRange.xmid(), 0);
  }

  return dataRange;
}

//------

CQChartsGeom::BBox
CQChartsPlot::
calcGroupedDataRange(const RangeTypes &rangeTypes) const
{
  BBox bbox;

  if (isOverlay()) {
    processOverlayPlots([&](const Plot *plot) {
      auto bbox1 = plot->calcDataRange();
      if (! bbox1.isSet()) return;

      if (bbox1.isSet()) {
        if (rangeTypes.extra)
          bbox1 += plot->extraFitBBox();

        if (rangeTypes.axes) {
          bbox1 += plot->calcGroupedXAxisRange(CQChartsAxisSide::Type::NONE);
          bbox1 += plot->calcGroupedYAxisRange(CQChartsAxisSide::Type::NONE);
        }

        if (rangeTypes.key)
          bbox1 += plot->keyFitBBox();

        if (rangeTypes.title)
          bbox1 += plot->titleFitBBox();
      }

      if (plot != this)
        bbox1 = viewToWindow(plot->windowToView(bbox1));

      if (bbox1.isSet())
        bbox += bbox1;
    });

    if (! bbox.isSet())
      bbox = BBox(0, 0, 1, 1);
  }
  else {
    bbox = calcDataRange();

    if (bbox.isSet()) {
      if (rangeTypes.extra)
        bbox += extraFitBBox();

      if (rangeTypes.axes) {
        bbox += calcGroupedXAxisRange(CQChartsAxisSide::Type::NONE);
        bbox += calcGroupedYAxisRange(CQChartsAxisSide::Type::NONE);
      }

      if (rangeTypes.key)
        bbox += keyFitBBox();

      if (rangeTypes.title)
        bbox += titleFitBBox();
    }
    else
      bbox = BBox(0, 0, 1, 1);
  }

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
calcGroupedXAxisRange(const CQChartsAxisSide::Type &side) const
{
  BBox xbbox;

  if (isOverlay()) {
    processOverlayPlots([&](const Plot *plot) {
      auto *axis = plot->xAxis();

      if (! axis || ! axis->isVisible()) return;

      if (side == CQChartsAxisSide::Type::NONE || axis->side().type() == side) {
        auto xbbox1 = axis->bbox();
        if (! xbbox1.isSet()) return;

        if (plot != this)
          xbbox1 = viewToWindow(plot->windowToView(xbbox1));

        xbbox += xbbox1;
      }
    });
  }
  else {
    if (! isXAxisVisible())
      return BBox();

    if (side == CQChartsAxisSide::Type::NONE || xAxis()->side().type() == side)
      xbbox = xAxis()->bbox();
  }

  return xbbox;
}

CQChartsGeom::BBox
CQChartsPlot::
calcGroupedYAxisRange(const CQChartsAxisSide::Type &side) const
{
  BBox ybbox;

  if (isOverlay()) {
    processOverlayPlots([&](const Plot *plot) {
      auto *axis = plot->yAxis();

      if (! axis || ! axis->isVisible()) return;

      if (side == CQChartsAxisSide::Type::NONE || axis->side().type() == side) {
        auto ybbox1 = axis->bbox();
        if (! ybbox1.isSet()) return;

        if (plot != this)
          ybbox1 = viewToWindow(plot->windowToView(ybbox1));

        ybbox += ybbox1;
      }
    });
  }
  else {
    if (! yAxis())
      return BBox();

    if (side == CQChartsAxisSide::Type::NONE || yAxis()->side().type() == side)
      ybbox = yAxis()->bbox();
  }

  return ybbox;
}

//------

void
CQChartsPlot::
addPlotObject(PlotObj *obj)
{
  assert(obj);

  assert(! isComposite());

  assert(! objTreeData_.tree->isBusy());

  plotObjs_.push_back(obj);

  // TODO: needed ? Do post thread finished
#if 0
  obj->moveToThread(this->thread());

  obj->setParent(this);
#endif

  //obj->addProperties(propertyModel(), "objects");
}

// Note: only called when not threaded (no buffer layers)
void
CQChartsPlot::
initGroupedPlotObjs()
{
  CQPerfTrace trace("CQChartsPlot::initGroupedPlotObjs");

  // init overlay plots before draw
  if (isOverlay()) {
    if (! isFirstPlot())
      return;

    processOverlayPlots([&](Plot *plot) {
      initPlotRange(); // plot ?

      plot->initPlotObjs();
    });
  }
  else {
    initPlotRange();

    initPlotObjs();
  }
}

bool
CQChartsPlot::
initPlotRange()
{
  assert(! isComposite());

  if (! dataRange().isSet()) {
    execUpdateRange();

    if (! dataRange().isSet())
      return false;
  }

  return true;
}

void
CQChartsPlot::
initPlotObjs()
{
  CQPerfTrace trace("CQChartsPlot::initPlotObjs");

  //---

  bool changed = initObjs();

  assert(! isComposite());

  // ensure some range defined
  if (! dataRange().isSet()) {
    Range r;

    r.updateRange(0, 0);
    r.updateRange(1, 1);

    setDataRange(r, /*update*/false);

    changed = true;
  }

  //---

  if (addNoDataObj())
    changed = true;

  //---

  // init search tree
  if (changed)
    invalidateObjTree();

  //---

  // auto fit
  if (changed && isAutoFit()) {
    //setNeedsAutoFit(true);
  }

  //---

  if (changed) {
    plotObjsAdded_ = true;

    Q_EMIT plotObjsAdded();
  }
}

bool
CQChartsPlot::
addNoDataObj()
{
  CQPerfTrace trace("CQChartsPlot::addNoDataObj");

  assert(! isComposite());

  // if no objects then add a no data object
  noData_ = false;

  if (type()->hasObjs() && plotObjs_.empty()) {
    auto *obj = new CQChartsNoDataObj(this);

    addPlotObject(obj);

    noData_ = true;
  }

  return noData_;
}

bool
CQChartsPlot::
initObjs()
{
  CQPerfTrace trace("CQChartsPlot::initObjs");

  if (hasPlotObjs())
    return false;

  //---

  resetKeyItems(/*add*/false);

  if (! createObjs())
    return false;

  selectObjsFromModel();

  //---

  resetKeyItems(/*add*/true);

  //---

  scrollData_.invalid = true;

  //---

  Q_EMIT customDataChanged();

  view_->updateSelText();

  return true;
}

bool
CQChartsPlot::
hasPlotObjs() const
{
  assert(! isComposite());

  return ! plotObjs_.empty();
}

bool
CQChartsPlot::
createObjs()
{
  //std::cerr << "createObjs " << calcName().toStdString() << "\n";

  resetExtraFitBBox();

  //---

  if (! currentModel().data())
    return true;

  //---

  PlotObjs objs;

  if (! createObjs(objs))
    return false;

  if (type()->isPrioritySort()) {
    using PriorityObjs = std::map<int, PlotObjs>;

    PriorityObjs priorityObjs;

    for (auto &obj : objs)
      priorityObjs[obj->priority()].push_back(obj);

    for (auto &po : priorityObjs) {
      for (auto &obj : po.second)
        addPlotObject(obj);
    }
  }
  else {
    for (auto &obj : objs)
      addPlotObject(obj);
  }

  //---

  restoreSelection();

  //---

  applyVisibleFilter();

  return true;
}

QString
CQChartsPlot::
columnsHeaderName(const Columns &columns, bool tip) const
{
  QString str;

  for (const auto &column : columns.columns()) {
    auto str1 = columnHeaderName(column, tip);
    if (! str1.length()) continue;

    if (str.length())
      str += ", ";

    str += str1;
  }

  return str;
}

QString
CQChartsPlot::
columnHeaderName(const Column &column, bool tip, const QString &def) const
{
  auto p = columnNames_.find(column);
  if (p != columnNames_.end()) return (*p).second;

  bool ok;

  if (tip) {
    auto str = modelHHeaderTip(column, ok);
    if (ok && str.length()) return str;
  }

  auto str = modelHHeaderString(column, ok);
  if (ok && str.length()) return str;

  return def;
}

void
CQChartsPlot::
updateColumnNames()
{
  if (type()->supportsIdColumn())
    setColumnHeaderName(idColumn(), "Id");

  if (type()->supportsColorColumn())
    setColumnHeaderName(colorColumn(), "Color");

  if (type()->supportsAlphaColumn())
    setColumnHeaderName(alphaColumn(), "Alpha");

  if (type()->supportsFontColumn())
    setColumnHeaderName(fontColumn(), "Font" );

  if (type()->supportsImageColumn())
    setColumnHeaderName(imageColumn(), "Image");
}

void
CQChartsPlot::
setColumnHeaderName(const Column &column, const QString &def)
{
  if (! column.isValid())
    return;

  bool ok;

  auto str = modelHHeaderString(column, ok);
  if (! str.length()) str = def;

  columnNames_[column] = str;
}

void
CQChartsPlot::
initObjTree()
{
  if (isOverlay()) {
    processOverlayPlots([&](Plot *plot) {
      plot->execInitObjTree();
    });
  }

  execInitObjTree();
}

void
CQChartsPlot::
execInitObjTree()
{
  //std::cerr << "execInitObjTree " << calcName().toStdString() << "\n";

  CQPerfTrace trace("CQChartsPlot::execInitObjTree");

  assert(! isComposite());

  if (objTreeData_.init) {
    objTreeData_.init = false;

    if (! isPreview() && useObjTree())
      objTreeData_.tree->addObjects();
  }
}

void
CQChartsPlot::
clearPlotObjects()
{
  // overlay ?

  clearPlotObjects1();
}

void
CQChartsPlot::
clearPlotObjects1()
{
  //std::cerr << "clearPlotObjects1 " << calcName().toStdString() << "\n";

  CQPerfTrace trace("CQChartsPlot::clearPlotObjects1");

  assert(! isComposite());

  objTreeData_.tree->clearObjects();

  clearPlotObjList();
}

void
CQChartsPlot::
clearPlotObjList()
{
  PlotObjs plotObjs;

  std::swap(plotObjs, plotObjs_);

#if 0
  for (auto &plotObj : plotObjs)
    propertyModel()->removeProperties("objects/" + plotObj->propertyId());
#endif

  for (auto &plotObj : plotObjs)
    delete plotObj;
}

void
CQChartsPlot::
clearInsideObjects()
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->clearInsideObjects1();

  if (parentPlot())
    return parentPlot()->clearInsideObjects();

  //---

  clearInsideObjects1();
}

void
CQChartsPlot::
clearInsideObjects1()
{
  //std::cerr << "clearInsideObjects1 " << calcName().toStdString() << "\n";

  //assert(! isComposite()); no harm to clear child plots

  insideData_.clear();
}

void
CQChartsPlot::
invalidateObjTree()
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->invalidateObjTree();

  if (parentPlot())
    return parentPlot()->invalidateObjTree();

  //---

  if (isOverlay()) {
    Plots oplots;

    overlayPlots(oplots);

    for (auto &oplot : oplots)
      oplot->invalidateObjTree1();
  }
  else
    invalidateObjTree1();
}

void
CQChartsPlot::
invalidateObjTree1()
{
  //std::cerr << "invalidateObjTree1 " << calcName().toStdString() << "\n";

  assert(! isComposite());

  objTreeData_.init = true;

  objTreeData_.tree->clearObjects();
}

CQChartsGeom::BBox
CQChartsPlot::
findEmptyBBox(double w, double h) const
{
  assert(! isComposite());

  assert(useObjTree());

  return objTreeData_.tree->findEmptyBBox(w, h);
}

//------

// get inside objects at point (grouped)
bool
CQChartsPlot::
updateInsideObjects(const Point &w, Constraints constraints)
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->updateInsideObjects1(w, constraints);

  if (parentPlot())
    return parentPlot()->updateInsideObjects(w, constraints);

  //---

  return updateInsideObjects1(w, constraints);
}

bool
CQChartsPlot::
updateInsideObjects1(const Point &w, Constraints constraints)
{
  // get objects and annotations at point
  Objs objs;

  insideData_.setPoint(w);

  groupedObjsAtPoint(insideData_.p, objs, constraints);

  return setInsideObjects(w, objs);
}

bool
CQChartsPlot::
setInsideObjects(const Point &w, Objs &objs)
{
  assert(! parentPlot());

  //std::cerr << "setInsideObjects " << calcName().toStdString() << "\n";

  insideData_.setPoint(w);

  //---

  // check if changed
  bool changed = false;

  if (objs.size() == insideData_.objs.size()) {
    for (const auto &obj : objs) {
      if (insideData_.objs.find(obj) == insideData_.objs.end()) {
        changed = true;
        break;
      }
    }
  }
  else {
    changed = true;
  }

  //---

  // if changed update inside objects
  if (changed) {
    // reset current inside index
    insideData_.ind = 0;

    //---

    // reset inside objects
    resetInsideObjs();

    //---

    // set new inside objects (and inside objects sorted by size)
    insideData_.sizeObjs.clear();

    for (const auto &obj : objs) {
      insideData_.objs.insert(obj);

      insideData_.sizeObjs[obj->rect().area()].insert(obj);
    }

    // set current inside obj
    setInsideObject();

    //std::cerr << "  #SizeObjs " << insideData_.numSizeObjs() << "\n";

    //---

    view()->updateTip();
  }

  //---

  return changed;
}

void
CQChartsPlot::
resetInsideObjs()
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->resetInsideObjs1();

  if (parentPlot())
    return parentPlot()->resetInsideObjs();

  //---

  resetInsideObjs1();
}

void
CQChartsPlot::
resetInsideObjs1()
{
  //std::cerr << "resetInsideObjs1 " << calcName().toStdString() << "\n";

  clearInsideObjects1();

  if (isOverlay()) {
    Plots oplots;

    overlayPlots(oplots);

    for (const auto &oplot : oplots) {
      for (auto &obj : oplot->plotObjects())
        obj->setInside(false);

      for (auto &annotation : oplot->annotations())
        if (annotation->isInside())
          annotation->setInside(false);
    }
  }
  else {
    for (auto &obj : plotObjects())
      if (obj->isInside())
        obj->setInside(false);

    for (auto &annotation : annotations())
      if (annotation->isInside())
        annotation->setInside(false);
  }
}

CQChartsObj *
CQChartsPlot::
insideObject() const
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->insideObject1();

  if (parentPlot())
    return parentPlot()->insideObject();

  //---

  return insideObject1();
}

CQChartsObj *
CQChartsPlot::
insideObject1() const
{
  assert(! parentPlot());

  //std::cerr << "insideObject1 " << calcName().toStdString() << "\n";

  // get nth inside object
  int i = 0;

  for (const auto &sizeObj : insideData_.sizeObjs) {
    for (const auto &obj : sizeObj.second) {
      if (i == insideData_.ind)
        return obj;

      ++i;
    }
  }

  return nullptr;
}

void
CQChartsPlot::
nextInsideInd()
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->nextInsideInd();

  if (parentPlot())
    return parentPlot()->nextInsideInd();

  //---

  assert(! parentPlot());

  // cycle to next inside object
  insideData_.nextInd();
}

void
CQChartsPlot::
prevInsideInd()
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->prevInsideInd();

  if (parentPlot())
    return parentPlot()->prevInsideInd();

  //---

  assert(! parentPlot());

  // cycle to prev inside object
  insideData_.prevInd();
}

void
CQChartsPlot::
setInsideObject()
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->setInsideObject();

  if (parentPlot())
    return parentPlot()->setInsideObject();

  //---

  assert(! parentPlot());

  // get nth inside object
  auto *insideObj = insideObject();

  // update object is inside (TODO: update all objs state ?)
  for (auto &obj : insideData_.objs) {
    bool inside = (obj == insideObj);

    if (inside != obj->isInside())
      obj->setInside(inside);
  }
}

QString
CQChartsPlot::
insideObjectText() const
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->insideObjectText1();

  if (parentPlot())
    return parentPlot()->insideObjectText();

  //---

  return insideObjectText1();
}

QString
CQChartsPlot::
insideObjectText1() const
{
  assert(! parentPlot());

  //std::cerr << "insideObjectText1 " << calcName().toStdString() << "\n";

  QString objText;

  for (const auto &sizeObj : insideData_.sizeObjs) {
    for (const auto &obj : sizeObj.second) {
      if (obj->isInside()) {
        if (objText != "")
          objText += " ";

        objText += obj->id();
      }
    }
  }

  return objText;
}

//------

CQChartsObj *
CQChartsPlot::
currentObj() const
{
  if (currentObjId_ == "")
    return nullptr;

  for (auto &plotObj : plotObjects()) {
    if (plotObj->id() == currentObjId_)
      return plotObj;
  }

  return nullptr;
}

//------

void
CQChartsPlot::
setOverviewDisplayed(bool b)
{
  if (b != overviewData_.displayed) {
    overviewData_.displayed = b;

    if (overviewData_.displayed)
      updateOverview();
  }
}

void
CQChartsPlot::
setOverviewSize(int s)
{
  overviewData_.size = s;

  if (overviewData_.displayed)
    updateOverview();

  Q_EMIT overviewChanged();
}

void
CQChartsPlot::
setOverviewFillColor(const QColor &c)
{
  overviewData_.fillColor = c;

  Q_EMIT overviewChanged();
}

void
CQChartsPlot::
setOverviewFillAlpha(double a)
{
  overviewData_.fillAlpha = a;

  Q_EMIT overviewChanged();
}

void
CQChartsPlot::
setOverviewStrokeColor(const QColor &c)
{
  overviewData_.strokeColor = c;

  Q_EMIT overviewChanged();
}

void
CQChartsPlot::
setOverviewStrokeAlpha(double a)
{
  overviewData_.strokeAlpha = a;

  Q_EMIT overviewChanged();
}

void
CQChartsPlot::
updateOverview()
{
  auto saveZoomData = this->zoomData();

  setZoomData(ZoomData());

  setLayersChanged(true);

  waitDraw();

  setZoomData(saveZoomData);

  setLayersChanged(true);
}

//------

void
CQChartsPlot::
setRulerDisplayed(bool b)
{
  if (b != rulerData_.displayed) {
    rulerData_.displayed = b;

    drawObjs();
  }
}

void
CQChartsPlot::
setRulerSize(int s)
{
  if (s != rulerData_.pixelSize) {
    rulerData_.pixelSize = s;

    drawObjs();
  }
}

void
CQChartsPlot::
setRulerFontScale(double s)
{
  if (s != rulerData_.fontScale) {
    rulerData_.fontScale = s;

    drawObjs();
  }
}

//------

void
CQChartsPlot::
clearErrors()
{
  if (parentPlot())
    return parentPlot()->clearErrors();

  //---

  errorData_.clear();

  Q_EMIT errorsCleared();
}

bool
CQChartsPlot::
hasErrors() const
{
  if (parentPlot())
    return parentPlot()->hasErrors();

  //---

  return errorData_.hasErrors();
}

bool
CQChartsPlot::
addError(const QString &msg)
{
  if (parentPlot())
    return parentPlot()->addError(msg);

  //---

  if (! isPreview()) {
    Error err { msg };

    errorData_.globalErrors.push_back(err);

    // TODO: add to log
    //charts()->errorMsg(msg);

    Q_EMIT errorAdded();
  }

  return false;
}

bool
CQChartsPlot::
addColumnError(const Column &c, const QString &msg)
{
  if (parentPlot())
    return parentPlot()->addColumnError(c, msg);

  //---

  if (! isPreview()) {
    ColumnError err { c, msg };

    errorData_.columnErrors.push_back(err);

    // TODO: add to log
    //charts()->errorMsg(msg);

    Q_EMIT errorAdded();
  }

  return false;
}

bool
CQChartsPlot::
addDataError(const ModelIndex &ind, const QString &msg)
{
  if (parentPlot())
    return parentPlot()->addDataError(ind, msg);

  //---

  if (! isPreview()) {
    DataError err { ind, msg };

    errorData_.dataErrors.push_back(err);

    // TODO: add to log
    //charts()->errorMsg(msg);

    Q_EMIT errorAdded();
  }

  return false;
}

void
CQChartsPlot::
getErrors(QStringList &strs)
{
  if (parentPlot())
    return parentPlot()->getErrors(strs);

  //---

  if (! errorData_.globalErrors.empty()) {
    for (const auto &error : errorData_.globalErrors) {
      strs << error.msg;
    }
  }

  if (! errorData_.columnErrors.empty()) {
    for (const auto &error : errorData_.columnErrors) {
      auto msg = QString("Column %1 : %2").arg(error.column.toString()).arg(error.msg);

      strs << msg;
    }
  }

  if (! errorData_.dataErrors.empty()) {
    for (const auto &error : errorData_.dataErrors) {
      auto msg = QString("Ind %1 : %2").arg(error.ind.toString()).arg(error.msg);

      strs << msg;
    }
  }
}

void
CQChartsPlot::
addErrorsToWidget(QTextBrowser *text)
{
  if (parentPlot())
    return parentPlot()->addErrorsToWidget(text);

  //---

  CQChartsHtml html;

  if (! errorData_.globalErrors.empty()) {
    html.h2("Global Errors");

    for (const auto &error : errorData_.globalErrors) {
      html.p(error.msg);
    }
  }

  if (! errorData_.columnErrors.empty()) {
    html.h2("Column Errors");

    for (const auto &error : errorData_.columnErrors) {
      auto msg = QString("Column %1 : %2").arg(error.column.toString()).arg(error.msg);

      html.p(msg);
    }
  }

  if (! errorData_.dataErrors.empty()) {
    html.h2("Data Errors");

    for (const auto &error : errorData_.dataErrors) {
      auto msg = QString("Ind %1 : %2").arg(error.ind.toString()).arg(error.msg);

      html.p(msg);
    }
  }

  text->setHtml(html);
}

//------

bool
CQChartsPlot::
canPointSelect() const
{
  return type()->canPointSelect();
}

bool
CQChartsPlot::
canRectSelect() const
{
  return type()->canRectSelect();
}

bool
CQChartsPlot::
selectMousePress(const Point &p, SelMod selMod)
{
  if (! isReady()) return false;

  auto w = pixelToWindow(p);

  if (handleSelectPress(w, selMod))
    return true;

#ifdef CQCHARTS_MODULE_SHLIB
  if (moduleData_.eventProc) {
    CQChartsModuleEventData moduleEventData;

    moduleEventData.id     = moduleData_.id;
    moduleEventData.mode   = CQCHARTS_EVENT_MODE_SELECT;
    moduleEventData.type   = CQCHARTS_EVENT_TYPE_MOUSE_PRESS;
    moduleEventData.button = 0;
    moduleEventData.key    = 0;

    moduleEventData.pixel_x = p.x;
    moduleEventData.pixel_y = p.y;

    moduleEventData.plot_x = w.x;
    moduleEventData.plot_y = w.y;

    (*moduleData_.eventProc)(&moduleEventData);
  }
#endif

  Q_EMIT selectPressSignal(w);

  return false;
}

bool
CQChartsPlot::
handleSelectPress(const Point &w, SelMod selMod)
{
  if (isOverlay() && ! isFirstPlot())
    return false;

  //---

  if (keySelectPress(key(), w, selMod))
    return true;

  if (titleSelectPress(title(), w, selMod))
    return true;

  if (mapKeySelectPress(w, selMod))
    return true;

  //---

#if 0
  if (annotationsSelectPress(w, selMod))
    return true;
#endif

  //---

  if (objectsSelectPress(w, selMod))
    return true;

  return false;
}

bool
CQChartsPlot::
tabbedSelectPress(const Point &w, SelMod)
{
  if (! isTabbed() || ! isCurrent())
    return false;

  Plots plots;

  tabbedPlots(plots);

  auto *pressPlot = tabbedPressPlot(w, plots);
  if (! pressPlot) return false;

  for (auto &plot : plots)
    plot->setCurrent(false, /*notify*/false);

  pressPlot->setCurrent(true, /*notify*/true);

  pressPlot->updateRangeAndObjs();

  return true;
}

CQChartsPlot *
CQChartsPlot::
tabbedPressPlot(const Point &w, Plots &plots) const
{
  for (const auto &plot : plots) {
    if (plot->tabRect().inside(w)) {
      if (! plot->isCurrent())
        return plot;
    }
  }

  return nullptr;
}

bool
CQChartsPlot::
keySelectPress(PlotKey *key, const Point &w, SelMod selMod)
{
  // select key
  if (key && key->contains(w)) {
    auto *item = key->getItemAt(w);

    if (item) {
      CQChartsSelectableIFace::SelData selData(selMod);

      bool handled = item->selectPress(w, selData);

      if (handled) {
        Q_EMIT keyItemPressed  (item);
        Q_EMIT keyItemIdPressed(item->id());

        return true;
      }
    }

    auto *group = (item ? const_cast<CQChartsGroupKeyItem *>(item->group()) : nullptr);

    if (group) {
      CQChartsSelectableIFace::SelData selData(selMod);

      bool handled = group->selectPress(w, selData);

      if (handled) {
        Q_EMIT keyItemPressed  (group);
        Q_EMIT keyItemIdPressed(group->id());

        return true;
      }
    }

    CQChartsSelectableIFace::SelData selData(selMod);

    bool handled = key->selectPress(w, selData);

    if (handled) {
      Q_EMIT keyPressed  (key);
      Q_EMIT keyIdPressed(key->id());

      return true;
    }
  }

  return false;
}

bool
CQChartsPlot::
mapKeySelectPress(const Point &w, SelMod selMod)
{
  // select map key
  for (const auto &mapKey : mapKeys_) {
    if (! mapKey->isVisible())
      continue;

    if (mapKey->inside(w, MapKey::DrawType::VIEW)) {
      CQChartsSelectableIFace::SelData selData(selMod);

      bool handled = mapKey->selectPress(w, selData);

      if (handled) {
        Q_EMIT mapKeyPressed(mapKey);
        return true;
      }
    }
  }

  return false;
}

bool
CQChartsPlot::
titleSelectPress(CQChartsTitle *title, const Point &w, SelMod selMod)
{
  // select title
  if (title && title->contains(w)) {
    CQChartsSelectableIFace::SelData selData(selMod);

    if (title->selectPress(w, selData)) {
      Q_EMIT titlePressed  (title);
      Q_EMIT titleIdPressed(title->id());

      return true;
    }
  }

  return false;
}

#if 0
bool
CQChartsPlot::
annotationsSelectPress(const Point &w, SelMod selMod)
{
  groupedAnnotationsAtPoint(w, pressAnnotations_, Constraints::SELECTABLE);

  for (const auto &annotation : pressAnnotations_) {
    CQChartsSelectableIFace::SelData selData(selMod);

    annotation->selectPress(w, selData);
  }

  for (const auto &annotation : pressAnnotations_) {
    if (! annotation->isSelectable())
      continue;

    CQChartsSelectableIFace::SelData selData(selMod);

    if (! annotation->selectPress(w, selData))
      continue;

    selectOneObj(annotation, SelMod::REPLACE);

    drawForeground();

    Q_EMIT annotationPressed  (annotation);
    Q_EMIT annotationIdPressed(annotation->id());

    return true;
  }

  return false;
}
#endif

CQChartsObj *
CQChartsPlot::
objectsSelectPress(const Point &w, SelMod selMod)
{
  // for replace init all objects to unselected
  // for add/remove/toggle init all objects to current state
  // TODO: only selectable
  using ObjsSelected = std::map<Obj*, bool>;

  ObjsSelected objsSelected;

  auto updateObjSelected = [&](Obj *obj) {
    if (obj->isSelectable())
      objsSelected[obj] = (selMod != SelMod::REPLACE ? obj->isSelected() : false);
  };

  if (isOverlay()) {
    processOverlayPlots([&](Plot *plot) {
      for (auto &plotObj : plot->plotObjects())
        updateObjSelected(plotObj);

      for (const auto &annotation : plot->annotations())
        updateObjSelected(annotation);
    });
  }
  else {
    for (auto &plotObj : plotObjects())
      updateObjSelected(plotObj);

    for (const auto &annotation : annotations())
      updateObjSelected(annotation);
  }

  //---

  // get object under mouse
  Obj *selectObj = nullptr;

  if (isFollowMouse() && insideData_.isSet()) {
    // get nth inside object
    selectObj = insideObject();

    // TODO: handle overlay
    nextInsideInd();

    setInsideObject();
  }
  else {
    // get selectable objects and annotations at point
    Objs objs;

    groupedObjsAtPoint(w, objs, Constraints::SELECTABLE);

    // use first object for select
    if (! objs.empty())
      selectObj = *objs.begin();
  }

  //---

  // change selection depending on selection modifier
  if (selectObj) {
    if      (selMod == SelMod::TOGGLE)
      objsSelected[selectObj] = ! selectObj->isSelected();
    else if (selMod == SelMod::REPLACE)
      objsSelected[selectObj] = true;
    else if (selMod == SelMod::ADD)
      objsSelected[selectObj] = true;
    else if (selMod == SelMod::REMOVE)
      objsSelected[selectObj] = false;

    //---

    auto *selectPlotObj    = dynamic_cast<PlotObj    *>(selectObj);
    auto *selectAnnotation = dynamic_cast<Annotation *>(selectObj);

    if (selectPlotObj) {
      CQChartsSelectableIFace::SelData selData(selMod);

      if (selectPlotObj->selectPress(w, selData)) {
        Q_EMIT objPressed  (selectPlotObj);
        Q_EMIT objIdPressed(selectPlotObj->id());
      }
    }
    else if (selectAnnotation) {
      CQChartsSelectableIFace::SelData selData(selMod);

      if (selectAnnotation->selectPress(w, selData)) {
        if (! selData.select)
           objsSelected[selectAnnotation] = false;

        drawForeground();

        Q_EMIT annotationPressed  (selectAnnotation);
        Q_EMIT annotationIdPressed(selectAnnotation->id());
      }
    }

    // potential crash if signals cause objects to be deleted (defer !!!)
  }

  //---

  // select objects and track if selection changed
  bool changed = false;

  auto setObjSelected = [&](Obj *obj, bool selected) {
    if (! changed) { startSelection(); changed = true; }

    obj->setSelected(selected);
  };

  for (const auto &objSelected : objsSelected) {
    if (objSelected.first->isSelected() == objSelected.second)
      continue;

    if (! objSelected.second)
      setObjSelected(objSelected.first, false);
  }

  for (const auto &objSelected : objsSelected) {
    if (! objSelected.first->isSelectable())
      continue;

    if (objSelected.first->isSelected() == objSelected.second)
      continue;

    if (objSelected.second)
      setObjSelected(objSelected.first, true);
  }

  //----

  // update selection if changed
  if (changed) {
    auto *selPlot = selectionPlot();

    if (selPlot != view()->currentPlot(/*remap*/false))
      view()->setCurrentPlot(selPlot);

    beginSelectIndex();

    for (const auto &objSelected : objsSelected) {
      auto *selectPlotObj = dynamic_cast<PlotObj *>(objSelected.first);

      if (! selectPlotObj || ! selectPlotObj->isSelected())
        continue;

      if (selectPlotObj->plot() == selPlot)
        selectPlotObj->addSelectIndices(this);
    }

    endSelectIndex();

    //---

    invalidateOverlay();

    if (selectInvalidateObjs())
      drawObjs();

    //---

    endSelection();
  }

  //---

  return selectObj;
}

bool
CQChartsPlot::
selectMouseMove(const Point &pos, bool first)
{
  if (! isReady()) return false;

  auto w = pixelToWindow(pos);

  return handleSelectMove(w, Constraints::SELECTABLE, first);
}

// handle mouse move in select mode for current plot
bool
CQChartsPlot::
handleSelectMove(const Point &w, Constraints constraints, bool first)
{
  if (isOverlay() && ! isFirstPlot())
    return false;

  //---

  if (hasRulers()) {
    invalidateOverlay();

    //drawObjs();
  }

  //---

  mouseData_.movePoint = w;

  if (isKeyVisible()) {
    bool handled = key()->selectMove(w);

    if (handled)
      return true;
  }

  //---

  // notify annotations under point annotation
  Annotations annotations;

  groupedAnnotationsAtPoint(w, annotations, constraints);

  for (const auto &annotation : annotations) {
    annotation->selectMove(w);
  }

  //---

  QString objText;

  if (isFollowMouse()) {
    bool changed = updateInsideObjects(w, constraints);

    objText = insideObjectText();

    if (changed)
      invalidateOverlay();
  }

  //---

  if (first) {
    if (objText != "") {
      view()->setStatusText(objText);

      return true;
    }
  }

  //---

  return false;
}

bool
CQChartsPlot::
selectMouseRelease(const Point &p)
{
  if (! isReady()) return false;

  auto w = pixelToWindow(p);

  return handleSelectRelease(w);
}

bool
CQChartsPlot::
handleSelectRelease(const Point &w)
{
  if (isOverlay() && ! isFirstPlot())
    return false;

  // release pressed annotations
  for (const auto &annotation : pressAnnotations_) {
    annotation->selectRelease(w);
  }

  return true;
}

bool
CQChartsPlot::
selectMouseDoubleClick(const Point &p, SelMod selMod)
{
  if (! isReady()) return false;

  auto w = pixelToWindow(p);

  if (handleSelectDoubleClick(w, selMod))
    return true;

  Q_EMIT selectDoubleClickSignal(w);

  return false;
}

bool
CQChartsPlot::
handleSelectDoubleClick(const Point &w, SelMod selMod)
{
  if (isOverlay() && ! isFirstPlot())
    return false;

  //---

#if 0
  if (keySelectDoubleClick(key(), w, selMod))
    return true;

  if (titleSelectDoubleClick(title(), w, selMod))
    return true;
#endif

  //---

#if 0
  if (annotationsSelectDoubleClick(w, selMod))
    return true;
#endif

  //---

  if (objectsSelectDoubleClick(w, selMod))
    return true;

  return false;
}

CQChartsObj *
CQChartsPlot::
objectsSelectDoubleClick(const Point &w, SelMod selMod)
{
  if (! useObjTree())
    return nullptr;

  // get selectable objects and annotations at point
  Objs objs;

  groupedObjsAtPoint(w, objs, Constraints::CLICKABLE);

  // use first object for select
  Obj *selectObj = nullptr;

  if (! objs.empty())
    selectObj = *objs.begin();

  //---

  if (selectObj) {
    auto *selectPlotObj    = dynamic_cast<PlotObj    *>(selectObj);
    auto *selectAnnotation = dynamic_cast<Annotation *>(selectObj);

    if (selectPlotObj) {
      selectPlotObj->selectDoubleClick(w, selMod);

      Q_EMIT objDoubleClicked  (selectPlotObj);
      Q_EMIT objIdDoubleClicked(selectPlotObj->id());
    }
    else if (selectAnnotation) {
      selectAnnotation->selectDoubleClick(w, selMod);

      drawForeground();

      Q_EMIT annotationDoubleClicked  (selectAnnotation);
      Q_EMIT annotationIdDoubleClicked(selectAnnotation->id());
    }

    // potential crash if signals cause objects to be deleted (defer !!!)
  }

  //---

  return selectObj;
}
#if 0
void
CQChartsPlot::
selectObjsAtPoint(const Point &w, Objs &objs)
{
  if (key() && key()->contains(w))
    objs.push_back(key());

  if (title() && title()->contains(w))
    objs.push_back(title());

  Annotations annotations;

  groupedAnnotationsAtPoint(w, annotations, Constraints::SELECTABLE);

  for (const auto &annotation : annotations)
    objs.push_back(annotation);
}
#endif

//------

bool
CQChartsPlot::
editMousePress(const Point &pos, bool inside)
{
  if (! isReady()) return false;

  auto p = pos;
  auto w = pixelToWindow(p);

  editing_ = true;

  return handleEditPress(p, w, inside);
}

bool
CQChartsPlot::
handleEditPress(const Point &p, const Point &w, bool inside)
{
  if (isOverlay() && ! isFirstPlot())
    return false;

  //---

  setDragObj(DragObjType::NONE, nullptr);

  mouseData_.pressPoint = p;
  mouseData_.movePoint  = mouseData_.pressPoint;
  mouseData_.dragged    = false;
  mouseData_.dragSide   = CQChartsResizeSide::NONE;

  //---

  // start drag on already selected plot handle
  if (isEditable() && isSelected()) {
    auto v = windowToView(w);

    // to edit must be in handle
    EditHandles::InsideData insideData;

    if (editHandlePress(this, v, DragObjType::PLOT_HANDLE))
      return true;
  }

  //---

  if (keyEditPress(key(), w))
    return true;

  if (mapKeyEditPress(w))
    return true;

  //---

  if (axisEditPress(xAxis(), w) || axisEditPress(yAxis(), w))
    return true;

  if (titleEditPress(title(), w))
    return true;

  if (annotationsEditPress(w))
    return true;

  if (objectsEditPress(w, inside))
    return true;

  //---

  if (keyEditSelect(key(), w))
    return true;

  if (mapKeyEditSelect(w))
    return true;

  //---

  if (axisEditSelect(xAxis(), w) || axisEditSelect(yAxis(), w))
    return true;

  if (titleEditSelect(title(), w))
    return true;

#if 0
  if (annotationsEditSelect(w))
    return true;
#endif

  if (objectsEditSelect(w, inside))
    return true;

  return false;
}

bool
CQChartsPlot::
keyEditPress(PlotKey *key, const Point &w)
{
  if (! key || ! key->isEditable() || ! key->isSelected())
    return false;

  mouseData_.dragSide = CQChartsResizeSide::NONE;

  // start drag on already selected key handle
  EditHandles::InsideData insideData;

  if (editHandlePress(key, w, DragObjType::KEY)) {
    key->editPress(w);
    return true;
  }

  return false;
}

bool
CQChartsPlot::
mapKeyEditPress(const Point &w)
{
  for (auto *mapKey : mapKeys_) {
    if (! mapKey->isEditable() || ! mapKey->isSelected())
      continue;

    mouseData_.dragSide = CQChartsResizeSide::NONE;

    // start drag on already selected map key handle
    EditHandles::InsideData insideData;

    if (editHandlePress(mapKey, w, DragObjType::MAP_KEY)) {
      mapKey->editPress(w);
      return true;
    }
  }

  return false;
}

bool
CQChartsPlot::
axisEditPress(CQChartsAxis *axis, const Point &w)
{
  if (! axis || ! axis->isEditable() || ! axis->isSelected())
    return false;

  mouseData_.dragSide = CQChartsResizeSide::NONE;

  // start drag on already selected axis handle
  EditHandles::InsideData insideData;

  if (editHandlePress(axis, w, axis == xAxis() ? DragObjType::XAXIS : DragObjType::YAXIS)) {
    axis->editPress(w);
    return true;
  }

  return false;
}

bool
CQChartsPlot::
titleEditPress(CQChartsTitle *title, const Point &w)
{
  if (! title || ! title->isEditable() || ! title->isSelected())
    return false;

  mouseData_.dragSide = CQChartsResizeSide::NONE;

  // start drag on already selected title handle
  EditHandles::InsideData insideData;

  if (editHandlePress(title, w, DragObjType::TITLE)) {
    title->editPress(w);
    return true;
  }

  return false;
}

bool
CQChartsPlot::
annotationsEditPress(const Point &w)
{
  mouseData_.dragSide = CQChartsResizeSide::NONE;

  auto annotationsEditPress1 = [&](Plot *plot) {
    auto w1 = (plot != this ? plot->pixelToWindow(this->windowToPixel(w)) : w);

    // start drag on already selected annotation handle
    for (const auto &annotation : plot->annotations()) {
      if (! annotation->isVisible() || ! annotation->isEditable())
        continue;

      if (! annotation->isSelected())
        continue;

      EditHandles::InsideData insideData;

      if (editHandlePress(annotation, w1, DragObjType::ANNOTATION))
        return true;
    }

    return false;
  };

  //---

  if (isOverlay()) {
    Plots oplots;

    overlayPlots(oplots);

    for (const auto &oplot : oplots) {
      if (annotationsEditPress1(oplot))
        return true;
    }
  }
  else {
    if (annotationsEditPress1(this))
      return true;
  }

  return false;
}

bool
CQChartsPlot::
objectsEditPress(const Point &w, bool)
{
  mouseData_.dragSide = CQChartsResizeSide::NONE;

  // start drag on already selected object handle
  for (auto &plotObj : plotObjects()) {
    if (! plotObj->isVisible() || ! plotObj->isEditable())
      continue;

    if (! plotObj->isSelected())
      continue;

    EditHandles::InsideData insideData;

    if (editHandlePress(plotObj, w, DragObjType::OBJECT))
      return true;
  }

  return false;
}

bool
CQChartsPlot::
editHandlePress(CQChartsObj *obj, const Point &w, const DragObjType &dragObjType)
{
  auto *editIFace = dynamic_cast<CQChartsEditableIFace *>(obj);
  assert(editIFace);

  EditHandles::InsideData insideData;

  if (! editIFace->editHandles()->inside(w, insideData))
    return false;

  mouseData_.dragSide = insideData.resizeSide;

  setDragObj(dragObjType, obj);

  editIFace->editHandles()->setDragData(insideData);
  editIFace->editHandles()->setDragPos (w);

  invalidateOverlay();

  return true;
}

//---

bool
CQChartsPlot::
keyEditSelect(PlotKey *key, const Point &w)
{
  if (! key || ! key->isEditable())
    return false;

  if (! key->contains(w))
    return false;

  // select/deselect key
  if (! key->isSelected()) {
    selectOneObj(key, SelMod::REPLACE);
    return true;
  }

  if (key->editPress(w)) {
    setDragObj(DragObjType::KEY, key);
    invalidateOverlay();
    return true;
  }

  return false;
}

bool
CQChartsPlot::
mapKeyEditSelect(const Point &w)
{
  for (auto *mapKey : mapKeys_) {
    if (! mapKey->isEditable())
      continue;

    if (! mapKey->inside(w, MapKey::DrawType::VIEW))
      continue;

    // select/deselect key
    if (! mapKey->isSelected()) {
      selectOneObj(mapKey, SelMod::REPLACE);
      return true;
    }

    if (mapKey->editPress(w)) {
      setDragObj(DragObjType::MAP_KEY, mapKey);
      invalidateOverlay();
      return true;
    }
  }

  return false;
}

bool
CQChartsPlot::
axisEditSelect(CQChartsAxis *axis, const Point &w)
{
  if (! axis)
    return false;

  if (! axis->isEditable())
    return false;

  if (! axis->contains(w))
    return false;

  // select/deselect x axis
  if (! axis->isSelected()) {
    selectOneObj(axis, SelMod::REPLACE);
    return true;
  }

  if (axis->editPress(w)) {
    setDragObj(axis == xAxis() ? DragObjType::XAXIS : DragObjType::YAXIS, axis);
    invalidateOverlay();
    return true;
  }

  return false;
}

bool
CQChartsPlot::
titleEditSelect(CQChartsTitle *title, const Point &w)
{
  if (! title)
    return false;

  if (! title->isEditable())
    return false;

  if (! title->contains(w))
    return false;

  // select/deselect title
  if (! title->isSelected()) {
    selectOneObj(title, SelMod::REPLACE);
    return true;
  }

  if (title->editPress(w)) {
    setDragObj(DragObjType::TITLE, title);
    invalidateOverlay();
    return true;
  }

  return false;
}

#if 0
bool
CQChartsPlot::
annotationsEditSelect(const Point &w)
{
  Annotations annotations;

  groupedAnnotationsAtPoint(w, annotations, Constraints::EDITABLE);

  for (const auto &annotation : annotations) {
    if (! annotation->isVisible() || ! annotation->isEditable())
      continue;

    if (! annotation->contains(w))
      continue;

    if (! annotation->isSelected()) {
      selectOneObj(annotation, SelMod::REPLACE);
      return true;
    }

    if (annotation->editPress(w)) {
      setDragObj(DragObjType::ANNOTATION, annotation);
      invalidateOverlay();
      return true;
    }

    return false;
  }

  return false;
}
#endif

bool
CQChartsPlot::
objectsEditSelect(const Point &w, bool inside)
{
  auto selectPlot = [&]() {
    startSelection();

    view()->deselectAll();

    setSelected(true);

    endSelection();

    //---

    view()->setCurrentPlot(this);

    invalidateOverlay();
  };

  //---

  // get editiable objects and annotations at point
  Objs objs;

  groupedObjsAtPoint(w, objs, Constraints::EDITABLE);

  //---

  // select/deselect plot
  // (to select point must be inside a plot object)
  for (const auto &obj : objs) {
    auto *plotObj    = dynamic_cast<PlotObj    *>(obj);
    auto *annotation = dynamic_cast<Annotation *>(obj);

    if (plotObj) {
      if (! plotObj->isEditable())
        continue;

      if (! plotObj->isSelected()) {
        selectOneObj(plotObj, SelMod::REPLACE);
        return true;
      }

      if (plotObj->editPress(w)) {
        setDragObj(DragObjType::OBJECT, plotObj);
        invalidateOverlay();
        return true;
      }
    }
    else if (annotation) {
      if (! annotation->isVisible() || ! annotation->isEditable())
        continue;

      if (! annotation->contains(w))
        continue;

      if (! annotation->isSelected()) {
        selectOneObj(annotation, SelMod::REPLACE);
        return true;
      }

      if (annotation->editPress(w)) {
        setDragObj(DragObjType::ANNOTATION, annotation);
        invalidateOverlay();
        return true;
      }
    }
  }

  //---

  if (! objs.empty()) {
    if (isEditable()) {
      if (! isSelected()) {
        selectPlot();
        return true;
      }

      setDragObj(DragObjType::PLOT, this);
      invalidateOverlay();
      return true;
    }

    if (inside) {
      if (contains(w)) {
        if (! isSelected()) {
          selectPlot();
          return true;
        }
      }
    }
  }

  //---

  //view()->deselectAll();

  return false;
}

//---

void
CQChartsPlot::
selectOneObj(Obj *obj, SelMod selMod)
{
  startSelection();

  //---

  // for add, deselect all but specified object's type
  // for replace, deselect all objects
  auto deselectTypes1 = SelectTypes::ALL;

  if (selMod != SelMod::REPLACE) {
    auto *plot       = dynamic_cast<Plot       *>(obj);
    auto *plotObj    = dynamic_cast<PlotObj    *>(obj);
    auto *annotation = dynamic_cast<Annotation *>(obj);
    auto *title      = dynamic_cast<Title      *>(obj);
    auto *key        = dynamic_cast<PlotKey    *>(obj);
    auto *axis       = dynamic_cast<Axis       *>(obj);
    auto *mapKey     = dynamic_cast<MapKey     *>(obj);

    auto removeType = [&](SelectTypes type) {
      auto types = uint(deselectTypes1);
      types &= ~uint(type);
      return SelectTypes(types);
    };

    if      (plot)
      deselectTypes1 = removeType(SelectTypes::PLOT);
    else if (plotObj)
      deselectTypes1 = removeType(SelectTypes::PLOT_OBJ);
    else if (annotation)
      deselectTypes1 = removeType(SelectTypes::ANNOTATION);
    else if (title)
      deselectTypes1 = removeType(SelectTypes::TITLE);
    else if (key)
      deselectTypes1 = removeType(SelectTypes::KEY);
    else if (axis)
      deselectTypes1 = removeType(SelectTypes::AXIS);
    else if (mapKey)
      deselectTypes1 = removeType(SelectTypes::MAP_KEY);
  }

  view()->deselectAll(/*propagate*/false);

  deselectAll(deselectTypes1);

  //---

  // select object
  obj->setSelected(true);

  //---

  endSelection();

  invalidateOverlay();
}

// deselect all plot objects
void
CQChartsPlot::
deselectAllPlotObjs()
{
  startSelection();

  (void) deselectAllPlotObjs1();

  endSelection();
}

bool
CQChartsPlot::
deselectAllPlotObjs1()
{
  bool changed = false;

  if (isOverlay()) {
    processOverlayPlots([&](Plot *plot) {
      for (auto &plotObj : plot->plotObjects()) {
        if (plotObj->isSelected()) {
          plotObj->setSelected(false);
          changed = true;
        }
      }
    });
  }
  else {
    for (auto &plotObj : plotObjects()) {
      if (plotObj->isSelected()) {
        plotObj->setSelected(false);
        changed = true;
      }
    }
  }

  return changed;
}

// deselect all plot objects, and other plot objects (title, key, axes, annotations, ...)
void
CQChartsPlot::
deselectAll(SelectTypes selectTypes)
{
  bool changed = false;

  //---

  // note: will call startSelection if changed
  if (isOverlay()) {
    processOverlayPlots([&](Plot *plot) {
      plot->deselectAll1(selectTypes, changed);
    });
  }
  else {
    deselectAll1(selectTypes, changed);
  }

  //---

  if (changed) {
    endSelection();

    invalidateOverlay();
  }
}

void
CQChartsPlot::
deselectAll1(SelectTypes selectTypes, bool &changed)
{
  auto updateChanged = [&] {
    if (! changed) { startSelection(); changed = true; }
  };

  auto deselectObjs = [&](const std::vector<Obj *> &objs) {
    for (const auto &obj : objs) {
      if (obj && obj->isSelected()) {
        obj->setSelected(false);

        updateChanged();
      }
    }
  };

  std::vector<Obj *> objs;

  if (selectTypes & SelectTypes::KEY  ) { objs.push_back(key()); }
  if (selectTypes & SelectTypes::AXIS ) { objs.push_back(xAxis()); objs.push_back(yAxis()); }
  if (selectTypes & SelectTypes::TITLE) { objs.push_back(title()); }

  deselectObjs(objs);

  //---

  if (selectTypes & SelectTypes::MAP_KEY) {
    for (auto *mapKey : mapKeys_) {
      if (mapKey->isSelected()) {
        mapKey->setSelected(false);

        updateChanged();
      }
    }
  }

  if (selectTypes & SelectTypes::ANNOTATION) {
    for (auto &annotation : annotations()) {
      if (annotation->isSelected()) {
        annotation->setSelected(false);

        updateChanged();
      }
    }
  }

  // deselect all plot objects
  if (selectTypes & SelectTypes::PLOT_OBJ) {
    for (auto &plotObj : plotObjects()) {
      if (plotObj->isSelected()) {
        plotObj->setSelected(false);

        updateChanged();
      }
    }
  }

  if (selectTypes & SelectTypes::PLOT) {
    if (isSelected()) {
      setSelected(false);

      updateChanged();
    }
  }
}

//------

bool
CQChartsPlot::
hasXAxis() const
{
  assert(! isComposite());

  return type()->hasXAxis();
}

bool
CQChartsPlot::
hasYAxis() const
{
  assert(! isComposite());

  return type()->hasYAxis();
}

//------

bool
CQChartsPlot::
editMouseMove(const Point &pos, bool first)
{
  if (! isReady()) return false;

  auto p = pos;
  auto w = pixelToWindow(p);

  return handleEditMove(p, w, first);
}

bool
CQChartsPlot::
handleEditMove(const Point &p, const Point &w, bool /*first*/)
{
  if (isOverlay() && ! isFirstPlot())
    return false;

  //---

  auto lastMovePoint = mouseData_.movePoint;

  mouseData_.movePoint = p;

  if (mouseData_.dragObjType == DragObjType::NONE)
    return false;

  if      (mouseData_.dragObjType == DragObjType::KEY) {
    if (key()->editMove(w))
      mouseData_.dragged = true;
  }
  else if (mouseData_.dragObjType == DragObjType::MAP_KEY) {
    auto *editIFace = dynamic_cast<CQChartsEditableIFace *>(mouseData_.dragObj);
    assert(editIFace);

    if (editIFace->editMove(w))
      mouseData_.dragged = true;
  }
  else if (mouseData_.dragObjType == DragObjType::XAXIS) {
    // TODO: all overlay plot axes
    if (xAxis()->editMove(w))
      mouseData_.dragged = true;
  }
  else if (mouseData_.dragObjType == DragObjType::YAXIS) {
    // TODO: all overlay plot axes
    if (yAxis()->editMove(w))
      mouseData_.dragged = true;
  }
  else if (mouseData_.dragObjType == DragObjType::TITLE) {
    if (title()->editMove(w))
      mouseData_.dragged = true;
  }
  else if (mouseData_.dragObjType == DragObjType::ANNOTATION) {
    bool edited = false;

    auto editAnnotationsMove = [&](Plot *plot) {
      auto w1 = (plot != this ? plot->pixelToWindow(this->windowToPixel(w)) : w);

      for (const auto &annotation : plot->annotations()) {
        if (! annotation->isSelected())
          continue;

        if (annotation->editMove(w1))
          mouseData_.dragged = true;

        edited = true;
      }
    };

    if (isOverlay()) {
      Plots oplots;

      overlayPlots(oplots);

      for (const auto &oplot : oplots)
        editAnnotationsMove(oplot);
    }
    else {
      editAnnotationsMove(this);
    }

    if (! edited)
      return false;

    invalidateLayer(Buffer::Type::BACKGROUND);
    invalidateLayer(Buffer::Type::FOREGROUND);

    invalidateOverlay();
  }
  else if (mouseData_.dragObjType == DragObjType::OBJECT) {
    bool edited = false;

    auto editObjectsMove = [&](Plot *plot) {
      auto w1 = (plot != this ? plot->pixelToWindow(this->windowToPixel(w)) : w);

      for (const auto &plotObj : plot->plotObjects()) {
        if (! plotObj->isEditable())
          continue;

        if (! plotObj->isSelected())
          continue;

        if (plotObj->editMove(w1))
          mouseData_.dragged = true;

        edited = true;
      }
    };

    if (isOverlay()) {
      Plots oplots;

      overlayPlots(oplots);

      for (const auto &oplot : oplots)
        editObjectsMove(oplot);
    }
    else {
      editObjectsMove(this);
    }

    if (! edited)
      return false;
  }
  else if (mouseData_.dragObjType == DragObjType::PLOT ||
           mouseData_.dragObjType == DragObjType::PLOT_HANDLE) {
    // TODO: all overlay plots ?
    double dx = mouseData_.movePoint.x - lastMovePoint.x;
    double dy = lastMovePoint.y - mouseData_.movePoint.y;

    double dx1 =  view()->pixelToSignedWindowWidth (dx);
    double dy1 = -view()->pixelToSignedWindowHeight(dy);

    if (isOverlay()) {
      processOverlayPlots([&](Plot *plot) {
        assert(! plot->isComposite());

        if (mouseData_.dragObjType == DragObjType::PLOT)
          plot->viewBBox_.moveBy(Point(dx1, dy1));
        else {
          editHandles()->updateBBox(dx1, dy1);

          plot->viewBBox_ = editHandles()->bbox();
        }

        if (mouseData_.dragSide == CQChartsResizeSide::MOVE)
          plot->updateMargins(false);
        else
          plot->updateMargins();

        plot->invalidateLayer(Buffer::Type::BACKGROUND);
        plot->invalidateLayer(Buffer::Type::MIDDLE);
        plot->invalidateLayer(Buffer::Type::FOREGROUND);

        plot->invalidateOverlay();
      });
    }
    else {
      assert(! isComposite());

      if (mouseData_.dragObjType == DragObjType::PLOT)
        viewBBox_.moveBy(Point(dx1, dy1));
      else {
        editHandles()->updateBBox(dx1, dy1);

        viewBBox_ = editHandles()->bbox();
      }

      if (mouseData_.dragSide == CQChartsResizeSide::MOVE)
        updateMargins(false);
      else
        updateMargins();

      invalidateLayer(Buffer::Type::BACKGROUND);
      invalidateLayer(Buffer::Type::MIDDLE);
      invalidateLayer(Buffer::Type::FOREGROUND);

      invalidateOverlay();
    }

    if (dx != 0.0 || dy != 0.0)
      mouseData_.dragged = true;

    view()->doUpdate();
  }
  else {
    return false;
  }

  invalidateOverlay();

  return true;
}

bool
CQChartsPlot::
editMouseMotion(const Point &pos)
{
  if (! isReady()) return false;

  auto p = pos;
  auto w = pixelToWindow(p);

  return handleEditMotion(p, w);
}

bool
CQChartsPlot::
handleEditMotion(const Point &, const Point &w)
{
  if (isOverlay() && ! isFirstPlot())
    return false;

  //---

  if      (isSelected()) {
    // TODO: process all plots
    auto v = windowToView(w);

    if (! editHandles()->selectInside(v))
      return false;
  }
  else if (key() && key()->isSelected()) {
    if (! key()->editMotion(w))
      return false;
  }
  else if (xAxis() && xAxis()->isSelected()) {
    // TODO: process all axes
    if (! xAxis()->editMotion(w))
      return false;
  }
  else if (yAxis() && yAxis()->isSelected()) {
    // TODO: process all axes
    if (! yAxis()->editMotion(w))
      return false;
  }
  else if (title() && title()->isSelected()) {
    if (! title()->editMotion(w))
      return false;
  }
  else {
    for (auto *mapKey : mapKeys_) {
      if (mapKey->isSelected()) {
        if (! mapKey->editMotion(w))
          return false;
      }
    }

    //---

    bool inside = false;

    auto editAnnotationsMotion = [&](Plot *plot) {
      auto w1 = (plot != this ? plot->pixelToWindow(this->windowToPixel(w)) : w);

      for (const auto &annotation : plot->annotations()) {
        if (! annotation->isSelected())
          continue;

        if (annotation->editMotion(w1)) {
          inside = true;
          break;
        }
      }

      return inside;
    };

    auto editObjectsMotion = [&](Plot *plot) {
      auto w1 = (plot != this ? plot->pixelToWindow(this->windowToPixel(w)) : w);

      for (const auto &plotObj : plot->plotObjects()) {
        if (! plotObj->isEditable())
          continue;

        if (! plotObj->isSelected())
          continue;

        if (plotObj->editMotion(w1)) {
          inside = true;
          break;
        }
      }

      return inside;
    };

    //---

    if (isOverlay()) {
      Plots oplots;

      overlayPlots(oplots);

      for (const auto &oplot : oplots) {
        if (editAnnotationsMotion(oplot))
          break;

        //---

        if (editObjectsMotion(oplot))
          break;
      }
    }
    else {
      if (! editAnnotationsMotion(this))
        (void) editObjectsMotion(this);
    }

    if (! inside)
      return false;
  }

  invalidateOverlay();

  return true;
}

bool
CQChartsPlot::
editMouseRelease(const Point &pos)
{
  if (! isReady()) return false;

  auto p = pos;
  auto w = pixelToWindow(p);

  editing_ = false;

  return handleEditRelease(p, w);
}

bool
CQChartsPlot::
handleEditRelease(const Point &, const Point &w)
{
  if (isOverlay() && ! isFirstPlot())
    return false;

  //---

  if (mouseData_.dragObj) {
    if (mouseData_.dragObjType == DragObjType::PLOT ||
        mouseData_.dragObjType == DragObjType::PLOT_HANDLE) {
      auto *plot = dynamic_cast<Plot *>(mouseData_.dragObj);
      assert(plot);
    }
    else {
      auto *editIFace = dynamic_cast<CQChartsEditableIFace *>(mouseData_.dragObj);
      assert(editIFace);

      editIFace->editRelease(w);
    }
  }

  //---

  setDragObj(DragObjType::NONE, nullptr);

  if (mouseData_.dragged)
    drawObjs();

  return true;
}

void
CQChartsPlot::
handleEditMoveBy(const Point &d)
{
  if (isOverlay() && ! isFirstPlot())
    return;

  //---

  auto r = calcDataRect();

  double dw = d.x*r.getWidth ();
  double dh = d.y*r.getHeight();

  Point dp(dw, dh);

  bool selected = false;

  if      (isSelected()) {
    selected = true;
  }
  else if (key() && key()->isSelected()) {
    key()->editMoveBy(dp);

    selected = true;
  }
  else if (xAxis() && xAxis()->isSelected()) {
    xAxis()->editMoveBy(dp);

    selected = true;
  }
  else if (yAxis() && yAxis()->isSelected()) {
    yAxis()->editMoveBy(dp);

    selected = true;
  }
  else if (title() && title()->isSelected()) {
    title()->editMoveBy(dp);

    selected = true;
  }
  else {
    for (auto *mapKey : mapKeys_) {
      if (! mapKey->isSelected())
        continue;

      mapKey->editMoveBy(dp);

      selected = true;
    }

    //---

    for (const auto &annotation : annotations()) {
      if (! annotation->isSelected())
        continue;

      (void) annotation->editMoveBy(dp);

      selected = true;

      break;
    }

    if (! selected) {
      for (auto &plotObj : plotObjects()) {
        if (! plotObj->isEditable())
          continue;

        if (! plotObj->isSelected())
          continue;

        (void) plotObj->editMoveBy(dp);

        selected = true;

        break;
      }
    }
  }

  if (selected)
    invalidateOverlay();
}

void
CQChartsPlot::
setDragObj(DragObjType objType, Obj *obj)
{
  mouseData_.dragObjType = objType;
  mouseData_.dragObj     = obj;
}

//---

void
CQChartsPlot::
probeMouseMove(const Point &pw)
{
#ifdef CQCHARTS_MODULE_SHLIB
  if (moduleData_.eventProc) {
    CQChartsModuleEventData moduleEventData;

    auto pp = windowToPixel(pw);

    moduleEventData.id     = moduleData_.id;
    moduleEventData.mode   = CQCHARTS_EVENT_MODE_PROBE;
    moduleEventData.type   = CQCHARTS_EVENT_TYPE_MOUSE_MOVE;
    moduleEventData.button = 0;
    moduleEventData.key    = 0;

    moduleEventData.pixel_x = pp.x;
    moduleEventData.pixel_y = pp.y;

    moduleEventData.plot_x = pw.x;
    moduleEventData.plot_y = pw.y;

    (*moduleData_.eventProc)(&moduleEventData);
  }
#else
  Q_UNUSED(pw);
#endif
}

//---

void
CQChartsPlot::
flipSelected(Qt::Orientation orient)
{
  for (auto &annotation : annotations()) {
    if (annotation->isSelected())
      annotation->flip(orient);
  }

  //---

  if (isOverlay()) {
    processOverlayPlots([&](Plot *plot) {
      for (auto &plotObj : plot->plotObjects()) {
        if (plotObj->isSelected())
          plotObj->flip(orient);
      }
    });
  }
  else {
    for (auto &plotObj : plotObjects()) {
      if (plotObj->isSelected())
        plotObj->flip(orient);
    }
  }
}

//------

void
CQChartsPlot::
editObjs(Objs &objs)
{
  if (isKeyVisibleAndNonEmpty())
    objs.push_back(key());

  if (title() && title()->isDrawn())
    objs.push_back(title());

  if (isXAxisVisible())
    objs.push_back(xAxis());

  if (isYAxisVisible())
    objs.push_back(yAxis());
}

bool
CQChartsPlot::
pointSelect(const Point &p, SelMod selMod)
{
  return objectsSelectPress(p, selMod);
}

bool
CQChartsPlot::
rectSelect(const BBox &r, SelMod selMod)
{
  // for replace init all objects to unselected
  // for add/remove/toggle init all objects to current state
  using ObjsSelected = std::map<Obj*, bool>;

  ObjsSelected objsSelected;

  if (isOverlay()) {
    processOverlayPlots([&](Plot *plot) {
      for (auto &plotObj : plot->plotObjects()) {
        if (selMod == SelMod::REPLACE)
          objsSelected[plotObj] = false;
        else
          objsSelected[plotObj] = plotObj->isSelected();
      }
    });
  }
  else {
    for (auto &plotObj : plotObjects()) {
      if (selMod == SelMod::REPLACE)
        objsSelected[plotObj] = false;
      else
        objsSelected[plotObj] = plotObj->isSelected();
    }
  }

  //---

  // get objects inside/touching rectangle
  Objs objs;

  groupedObjsIntersectRect(r, objs, view()->isSelectInside(), Constraints::SELECTABLE);

  //---

  // change selection depending on selection modifier
  for (auto &obj : objs) {
    if      (selMod == SelMod::TOGGLE)
      objsSelected[obj] = ! obj->isSelected();
    else if (selMod == SelMod::REPLACE)
      objsSelected[obj] = true;
    else if (selMod == SelMod::ADD)
      objsSelected[obj] = true;
    else if (selMod == SelMod::REMOVE)
      objsSelected[obj] = false;
  }

  //---

  // select objects and track if selection changed
  bool changed = false;

  auto setObjSelected = [&](Obj *obj, bool selected) {
    if (! changed) { startSelection(); changed = true; }

    obj->setSelected(selected);
  };

  for (const auto &objSelected : objsSelected) {
    if (objSelected.first->isSelected() == objSelected.second)
      continue;

    if (! objSelected.second)
      setObjSelected(objSelected.first, false);
  }

  for (const auto &objSelected : objsSelected) {
    if (! objSelected.first->isSelectable())
      continue;

    if (objSelected.first->isSelected() == objSelected.second)
      continue;

    if (objSelected.second)
      setObjSelected(objSelected.first, true);
  }

  //----

  // update selection if changed
  if (changed) {
    auto *selPlot = selectionPlot();

    if (selPlot != view()->currentPlot(/*remap*/false))
      view()->setCurrentPlot(selPlot);

    beginSelectIndex();

    for (const auto &objSelected : objsSelected) {
      auto *selectPlotObj = dynamic_cast<PlotObj *>(objSelected.first);

      if (! selectPlotObj || ! selectPlotObj->isSelected())
        continue;

      if (selectPlotObj->plot() == selPlot)
        selectPlotObj->addSelectIndices(this);
    }

    endSelectIndex();

    //---

    invalidateOverlay();

    if (selectInvalidateObjs())
      drawObjs();

    //---

    endSelection();
  }

  //---

  return ! objs.empty();
}

void
CQChartsPlot::
saveSelection()
{
  selectedObjNames_.clear();

  for (auto &plotObj : plotObjects()) {
    if (plotObj->isSelected())
      selectedObjNames_.insert(plotObj->id());
  }
}

void
CQChartsPlot::
restoreSelection()
{
  if (selectedObjNames_.empty())
    return;

  PlotObjs objs;

  for (auto &plotObj : plotObjects()) {
    auto po = selectedObjNames_.find(plotObj->id());

    if (po != selectedObjNames_.end())
      objs.push_back(plotObj);
  }

  selectObjs(objs, /*exportSel*/true);

  selectedObjNames_.clear();
}

void
CQChartsPlot::
selectObjs(const PlotObjs &objs, bool exportSel)
{
  startSelection();

  //---

  (void) deselectAllPlotObjs1();

  for (const auto &obj : objs) {
    if (! isSelectable())
      continue;

    obj->setSelected(true);
  }

  //---

  if (exportSel) {
    auto *selPlot = selectionPlot();

    beginSelectIndex();

    for (const auto &obj : objs) {
      if (obj->plot() == selPlot)
        obj->addSelectIndices(this);
    }

    endSelectIndex();
  }

  //--

  invalidateOverlay();

  //---

  endSelection();
}

void
CQChartsPlot::
startSelection()
{
  view()->startSelection();
}

void
CQChartsPlot::
endSelection(bool changed)
{
  view()->endSelection(changed);

  if (changed)
    Q_EMIT selectionChanged();
}

void
CQChartsPlot::
selectedObjs(Objs &objs) const
{
  PlotObjs plotObjs;

  selectedPlotObjs(plotObjs);

  for (auto &plotObj : plotObjs)
    objs.push_back(plotObj);

  for (auto &annotation : annotations()) {
    if (annotation->isSelected())
      objs.push_back(annotation);
  }

  if (key() && key()->isSelected())
    objs.push_back(key());

  for (auto *mapKey : mapKeys_) {
    if (mapKey->isSelected())
      objs.push_back(mapKey);
  }

  if (title() && title()->isSelected())
    objs.push_back(title());

  if (xAxis() && xAxis()->isSelected())
    objs.push_back(xAxis());

  if (yAxis() && yAxis()->isSelected())
    objs.push_back(yAxis());
}

void
CQChartsPlot::
selectedPlotObjs(PlotObjs &plotObjs) const
{
  auto objs = plotObjects();

  for (auto &plotObj : objs) {
    if (plotObj && plotObj->isSelected())
      plotObjs.push_back(plotObj);
  }
}

//---

bool
CQChartsPlot::
isPlotObjTreeSet() const
{
  assert(! isComposite());

  return objTreeData_.isSet;
}

void
CQChartsPlot::
setPlotObjTreeSet(bool b)
{
  assert(! isComposite());

  if (b != objTreeData_.isSet) {
    objTreeData_.isSet = b;

    if (b)
      objTreeData_.notify = true;
  }
}

//------

void
CQChartsPlot::
setXValueColumn(const Column &c)
{
  if (mappedXAxis()) {
    // calls drawBackground and drawForeground
    CQChartsUtil::testAndSet(xValueColumn_, c, [&]() { mappedXAxis()->setColumn(xValueColumn_); } );
  }
}

void
CQChartsPlot::
setYValueColumn(const Column &c)
{
  if (mappedYAxis()) {
    // calls drawBackground and drawForeground
    CQChartsUtil::testAndSet(yValueColumn_, c, [&]() { mappedYAxis()->setColumn(yValueColumn_); } );
  }
}

//------

void
CQChartsPlot::
setIdColumn(const Column &c)
{
  CQChartsUtil::testAndSet(idColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPlot::
setTipHeaderColumn(const Column &c)
{
  CQChartsUtil::testAndSet(tipHeaderColumn_, c, [&]() { resetObjTips(); } );
}

void
CQChartsPlot::
setTipColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(tipColumns_, c, [&]() { resetObjTips(); } );
}

void
CQChartsPlot::
setNoTipColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(noTipColumns_, c, [&]() { resetObjTips(); } );
}

void
CQChartsPlot::
setVisibleColumn(const Column &c)
{
  CQChartsUtil::testAndSet(visibleColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPlot::
setImageColumn(const Column &c)
{
  CQChartsUtil::testAndSet(imageColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPlot::
setControlColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(controlColumns_, c, [&]() {
    updateRangeAndObjs();

    Q_EMIT controlColumnsChanged();
  } );
}

//---

CQChartsColumn
CQChartsPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "id"         ) c = this->idColumn();
  else if (name == "visible"    ) c = this->visibleColumn();
  else if (name == "image"      ) c = this->imageColumn();
  else if (name == "color"      ) c = this->colorColumn();
  else if (name == "color_label") c = this->colorLabelColumn();
  else if (name == "alpha"      ) c = this->alphaColumn();
  else if (name == "font"       ) c = this->fontColumn();
//else CQCHARTS_QASSERT(false, "Invalid column name: " + name);

  return c;
}

void
CQChartsPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "id"         ) this->setIdColumn(c);
  else if (name == "tip_header" ) this->tipHeaderColumn();
  else if (name == "visible"    ) this->setVisibleColumn(c);
  else if (name == "image"      ) this->setImageColumn(c);
  else if (name == "color"      ) this->setColorColumn(c);
  else if (name == "color_label") this->setColorLabelColumn(c);
  else if (name == "alpha"      ) this->setAlphaColumn(c);
  else if (name == "font"       ) this->setFontColumn(c);
  else CQCHARTS_QASSERT(false, "Invalid column name: " + name);
}

CQChartsColumns
CQChartsPlot::
getNamedColumns(const QString &name) const
{
  Columns c;
  if      (name == "tip"    ) c = this->tipColumns();
  else if (name == "notip"  ) c = this->noTipColumns();
  else if (name == "control") c = this->controlColumns();
  else CQCHARTS_QASSERT(false, "Invalid columns name: " + name);

  return c;
}

void
CQChartsPlot::
setNamedColumns(const QString &name, const Columns &c)
{
  if      (name == "tip"    ) this->setTipColumns(c);
  else if (name == "notip"  ) this->setNoTipColumns(c);
  else if (name == "control") this->setControlColumns(c);
  else CQCHARTS_QASSERT(false, "Invalid columns name: " + name);
}

//---

void
CQChartsPlot::
setColorColumn(const Column &c)
{
  if (c != colorColumnData_.column()) {
    colorColumnData_.setColumn(c); updateObjs(); Q_EMIT colorDetailsChanged();
  }
}

void
CQChartsPlot::
setColorLabelColumn(const Column &c)
{
  CQChartsUtil::testAndSet(colorLabelColumn_, c, [&]() {
    updateObjs(); Q_EMIT colorDetailsChanged();
  } );
}

void
CQChartsPlot::
setColorType(const ColorType &t)
{
  auto t1 = static_cast<CQChartsColorType>(t);

  if (t1 != colorColumnData_.colorType()) {
    colorColumnData_.setColorType(t1); updateObjs(); Q_EMIT colorDetailsChanged();
  }
}

void
CQChartsPlot::
setColorMapped(bool b)
{
  if (b != colorColumnData_.isMapped()) {
    colorColumnData_.setMapped(b); updateObjs(); Q_EMIT colorDetailsChanged();
  }
}

void
CQChartsPlot::
setColorIntMapped(bool b)
{
  if (b != colorColumnData_.isIntMapped()) {
    colorColumnData_.setIntMapped(b); updateObjs(); Q_EMIT colorDetailsChanged();
  }
}

void
CQChartsPlot::
setColorMapMin(double r)
{
  if (r != colorColumnData_.mapMin()) {
    colorColumnData_.setMapMin(r); updateObjs(); Q_EMIT colorDetailsChanged();
  }
}

void
CQChartsPlot::
setColorMapMax(double r)
{
  if (r != colorColumnData_.mapMax()) {
    colorColumnData_.setMapMax(r); updateObjs(); Q_EMIT colorDetailsChanged();
  }
}

void
CQChartsPlot::
setColorMapPalette(const PaletteName &name)
{
  if (name != colorColumnData_.palette()) {
    colorColumnData_.setPalette(name); updateObjs(); Q_EMIT colorDetailsChanged();
  }
}

void
CQChartsPlot::
setColorXStops(const ColorStops &s)
{
  if (s != colorColumnData_.xStops()) {
    colorColumnData_.setXStops(s); updateObjs(); Q_EMIT colorDetailsChanged();
  }
}

void
CQChartsPlot::
setColorYStops(const ColorStops &s)
{
  if (s != colorColumnData_.yStops()) {
    colorColumnData_.setYStops(s); updateObjs(); Q_EMIT colorDetailsChanged();
  }
}

void
CQChartsPlot::
setColorMap(const CQChartsColorMap &colorMap)
{
  if (colorMap != colorColumnData_.colorMap()) {
    colorColumnData_.setColorMap(colorMap); updateRangeAndObjs(); Q_EMIT colorDetailsChanged();
  }
}

void
CQChartsPlot::
setColorMapColumn(const Column &c)
{
  if (c != colorColumnData_.colorColumn()) {
    colorColumnData_.setColorColumn(c); updateObjs(); Q_EMIT colorDetailsChanged();
  }
}

bool
CQChartsPlot::
calcColorIntMapped(const Column &colorColumn, bool defVal) const
{
  auto colorColumn1 = (colorColumn     .isValid() ? colorColumn      : this->colorColumn());
  auto colorColumn2 = (colorMapColumn().isValid() ? colorMapColumn() : this->colorColumn());

  if (colorColumn1 == colorColumn2)
    return isColorIntMapped();

  return defVal;
}

bool
CQChartsPlot::
calcColorMapped(const Column &colorColumn, bool defVal) const
{
  auto colorColumn1 = (colorColumn     .isValid() ? colorColumn      : this->colorColumn());
  auto colorColumn2 = (colorMapColumn().isValid() ? colorMapColumn() : this->colorColumn());

  if (colorColumn1 == colorColumn2)
    return isColorMapped();

  return defVal;
}

//---

void
CQChartsPlot::
initColorColumnData()
{
  CQPerfTrace trace("CQChartsPlot::initColorColumnData");

  std::unique_lock<std::mutex> lock(colorMutex_);

  //---

  auto colorColumn = calcColorMapColumn();

  colorColumnData_.setValid(false);

  if (! colorColumn.isValid())
    return;

  //---

  auto *columnDetails = this->columnDetails(colorColumn);
  if (! columnDetails) return;

  if (colorColumn.isGroup()) {
    colorColumnData_.setDataRange(0.0, std::max(numGroups() - 1, 0));
  }
  else {
    if (colorColumnData_.isMapped()) {
      auto minVar = columnDetails->minValue();
      auto maxVar = columnDetails->maxValue();

      colorColumnData_.setDataRange(CQChartsVariant::toRealOr(minVar, 0.0),
                                    CQChartsVariant::toRealOr(maxVar, 1.0));
    }

    ModelTypeData columnTypeData;

    (void) modelColumnValueType(colorColumn, columnTypeData);

    if (columnTypeData.type == ColumnType::COLOR) {
      auto *columnTypeMgr = charts()->columnTypeMgr();

      const auto *colorType = dynamic_cast<const CQChartsColumnColorType *>(
        columnTypeMgr->getType(columnTypeData.type));
      assert(colorType);

      const auto &model = this->currentModel();

      colorType->getMapData(charts(), model.data(), colorColumn,
                            columnTypeData.nameValues, colorColumnData_);
    }

    colorColumnData_.setModelType(columnTypeData.type);
  }

  colorColumnData_.setValid(true);
}

// get color from colorColumn at specified row
bool
CQChartsPlot::
colorColumnColor(int row, const QModelIndex &parent, Color &color,
                 const Column &colorColumn) const
{
  auto *th = const_cast<Plot *>(this);

  auto colorColumn1 = (colorColumn.isValid() ? colorColumn : this->colorColumn());

  ModelIndex colorInd(th, row, colorColumn1, parent);

  return modelIndexColor(colorInd, color);
}

bool
CQChartsPlot::
modelIndexColor(const ModelIndex &colorInd, Color &color) const
{
  //if (! isColorMapped())
  //  return false;

  // get model edit value
  bool ok;

  auto var = modelValue(colorInd, ok);
  if (! ok || ! var.isValid()) return false;

  return columnValueColor(var, color);
}

bool
CQChartsPlot::
columnValueColor(const QVariant &var, Color &color, const Column &colorColumn) const
{
  auto colorColumn1 = (colorColumn.isValid() ? colorColumn : this->colorColumn());

  CQChartsModelColumnDetails *columnDetails { nullptr };
  int                         numUnique     { 0 };

  //---

  // populate details
  auto getDetails = [&]() {
    if (! columnDetails) {
      columnDetails = this->columnDetails(colorColumn1);
      numUnique = (columnDetails ? columnDetails->numUnique() : 0);
    }
    return bool(columnDetails);
  };

  // use index of value in unique values to generate value in range
  // use unique index/count of edit values (which may have been converted)
  // not same as CQChartsColumnColorType::userData
  auto mapVarToColor = [&](const QVariant &var) {
    Color color;

    if (colorMap().valueToColor(var, color))
      return color;

    if (! getDetails()) return color;

    int    i = columnDetails->valueInd(var);
    double r = CMathUtil::map(i, 0, numUnique - 1, colorMapMin(), colorMapMax());

    return colorFromColorMapPaletteValue(r);
  };

  //---

  if (CQChartsVariant::isInt(var) && calcColorIntMapped(colorColumn1)) {
    if (! getDetails()) return false;

    if (numUnique <= maxMappedValues()) {
      color = mapVarToColor(var);

      return color.isValid();
    }
  }

  //---

  if      (CQChartsVariant::isNumeric(var)) {
    // get real value
    bool ok;
    double r = CQChartsVariant::toReal(var, ok);
    if (! ok) return false;

    color = colorMapRealColor(r, colorColumn1);
  }
  else if (CQChartsVariant::isColor(var)) {
    // use color value directly
    bool ok;
    color = CQChartsVariant::toColor(var, ok);
    if (! ok) return false;
  }
  else {
    if (calcColorMapped(colorColumn1))
      color = mapVarToColor(var);
    else {
      // assume value is color name
      bool ok;
      auto str = CQChartsVariant::toString(var, ok);

      if (ok)
        color = Color(str);
    }
  }

  return color.isValid();
}

CQChartsColor
CQChartsPlot::
colorMapRealColor(double r, const Column &colorColumn) const
{
  auto colorColumn1 = (colorColumn.isValid() ? colorColumn : this->colorColumn());

  // map real from data range if enabled
  double r1;

  if (calcColorMapped(colorColumn1))
    r1 = CMathUtil::map(r, colorMapDataMin(), colorMapDataMax(), colorMapMin(), colorMapMax());
  else
    r1 = r;

  // clamp to valid value
  r1 = CMathUtil::clamp(r1, 0.0, 1.0);

  //--

  return colorFromColorMapPaletteValue(r1);
}

CQChartsColor
CQChartsPlot::
normalizedColorMapRealColor(double r, const Column &colorColumn) const
{
  auto colorColumn1 = (colorColumn.isValid() ? colorColumn : this->colorColumn());

  // map real from data range if enabled
  double r1;

  if (calcColorMapped(colorColumn1))
    r1 = CMathUtil::map(r, 0.0, 1.0, colorMapMin(), colorMapMax());
  else
    r1 = r;

  // clamp to valid value
  r1 = CMathUtil::clamp(r1, 0.0, 1.0);

  //--

  return colorFromColorMapPaletteValue(r1);
}

CQChartsColor
CQChartsPlot::
colorFromColorMapPaletteValue(double r) const
{
  // use named palette if defined or current palette value
  Color color;

  CQColorsPalette *palette = nullptr;

  if      (colorMapPalette().isValid())
    palette = colorMapPalette().palette();
  else if (defaultPalette().isValid())
    palette = defaultPalette().palette();

  if (palette)
    color = Color(palette->getColor(r));
  else
    color = Color::makePaletteValue(r);

  return color;
}

bool
CQChartsPlot::
addColorTipColumn(CQChartsTableTip &tableTip, int row, const Column &column,
                  const QModelIndex &parent, bool force) const
{
  bool exists = (column.isValid() && tableTip.hasColumn(column));

  if (! force && exists)
    return false;

  QString value;

  if (! column.isValid())
    return false;

  ModelIndex columnInd(this, row, column, parent);

  bool ok;
  auto var = this->modelValue(columnInd, ok);
  if (! ok) return false;

  value = var.toString();

  Color color;

  if (! columnValueColor(var, color))
    return false;

  if (! value.length())
    value = color.toString();

  auto headerStr = QString("Color");

  if (column.isValid() && ! exists) {
    headerStr = this->columnHeaderName(column, /*tip*/true);

    if (headerStr == "")
      headerStr = "Color";
  }

  if (color.color().isValid())
    tableTip.addTableRow(headerStr, value, color.color());
  else
    tableTip.addTableRow(headerStr, value);

  if (column.isValid())
    tableTip.addColumn(column);

  return true;
}

//------

void
CQChartsPlot::
setAlphaColumn(const Column &c)
{
  if (c != alphaColumnData_.column()) {
    alphaColumnData_.setColumn(c); updateObjs();
  }
}

void
CQChartsPlot::
setAlphaMapped(bool b)
{
  if (b != alphaColumnData_.isMapped()) {
    alphaColumnData_.setMapped(b); updateObjs();
  }
}

void
CQChartsPlot::
setAlphaMapMin(double r)
{
  if (r != alphaColumnData_.mapMin()) {
    alphaColumnData_.setMapMin(r); updateObjs();
  }
}

void
CQChartsPlot::
setAlphaMapMax(double r)
{
  if (r != alphaColumnData_.mapMax()) {
    alphaColumnData_.setMapMax(r); updateObjs();
  }
}

//---

// get alpha from alphaColumn at specified row
bool
CQChartsPlot::
alphaColumnAlpha(int row, const QModelIndex &parent, Alpha &alpha) const
{
  auto *th = const_cast<Plot *>(this);

  ModelIndex alphaInd(th, row, alphaColumn(), parent);

  return modelIndexAlpha(alphaInd, alpha);
}

bool
CQChartsPlot::
modelIndexAlpha(const ModelIndex &alphaInd, Alpha &alpha) const
{
  //if (! isAlphaMapped())
  //  return false;

  // get model edit value
  bool ok;

  auto var = modelValue(alphaInd, ok);
  if (! ok || ! var.isValid()) return false;

  return columnValueAlpha(var, alpha);
}

bool
CQChartsPlot::
columnValueAlpha(const QVariant &var, Alpha &alpha) const
{
  if      (CQChartsVariant::isNumeric(var)) {
    // get real value
    bool ok;
    double r = CQChartsVariant::toReal(var, ok);
    if (! ok) return false;

    //--

    // map real from data range if enabled
    double r1;

    if (isAlphaMapped())
      r1 = CMathUtil::map(r, alphaMapDataMin(), alphaMapDataMax(), alphaMapMin(), alphaMapMax());
    else
      r1 = r;

    // skip if invalid value
    if (r1 < 0.0 || r1 > 1.0) return false;

    //--

    alpha = Alpha(r);

    return true;
  }
  else {
    if (isAlphaMapped()) {
      // use index of value in unique values to generate value in range
      auto *columnDetails = this->columnDetails(alphaColumn());
      if (! columnDetails) return false;

      // use unique index/count of edit values (which may have been converted)
      // not same as CQChartsColumnAlphaType::userData
      int n = columnDetails->numUnique();
      int i = columnDetails->valueInd(var);

      double r = CMathUtil::map(i, 0, n - 1, alphaMapMin(), alphaMapMax());

      alpha = Alpha(r);

      return true;
    }
    else
      return false;
  }
}

//------

void
CQChartsPlot::
setFontColumn(const Column &c)
{
  CQChartsUtil::testAndSet(fontColumn_, c, [&]() { updateObjs(); } );
}

bool
CQChartsPlot::
fontColumnFont(int row, const QModelIndex &parent, Font &font) const
{
  auto *th = const_cast<Plot *>(this);

  ModelIndex fontInd(th, row, fontColumn(), parent);

  return modelIndexFont(fontInd, font);
}

bool
CQChartsPlot::
modelIndexFont(const ModelIndex &fontInd, Font &font) const
{
  // get model edit value
  bool ok;

  auto var = modelValue(fontInd, ok);
  if (! ok || ! var.isValid()) return false;

  return columnValueFont(var, font);
}

bool
CQChartsPlot::
columnValueFont(const QVariant &var, Font &font) const
{
  if      (CQChartsVariant::isNumeric(var)) {
    // get real value
    bool ok;
    double r = CQChartsVariant::toReal(var, ok);
    if (! ok) return false;

    //--

    font.setFontSize(r);
  }
  else if (CQChartsVariant::isFont(var)) {
    // use font value directly
    bool ok;
    font = CQChartsVariant::toFont(var, ok);
  }
  else {
    bool ok;
    auto str = CQChartsVariant::toString(var, ok);

    font = Font(str);
  }

  return font.isValid();
}

//------

void
CQChartsPlot::
initSymbolTypeData(SymbolTypeData &symbolTypeData) const
{
  symbolTypeData.setValid(false);

  if (! symbolTypeData.column().isValid())
    return;

  auto *columnDetails = this->columnDetails(symbolTypeData.column());
  if (! columnDetails) return;

  if (symbolTypeData.column().isGroup()) {
    symbolTypeData.setDataRange(0, std::max(numGroups() - 1, 0));
  }
  else {
    if (symbolTypeData.isMapped()) {
      auto minVar = columnDetails->minValue();
      auto maxVar = columnDetails->maxValue();

      symbolTypeData.setDataRange(long(CQChartsVariant::toRealOr(minVar, 0.0)),
                                  long(CQChartsVariant::toRealOr(maxVar, 1.0)));
    }

    ModelTypeData columnTypeData;

    (void) modelColumnValueType(symbolTypeData.column(), columnTypeData);

    if (columnTypeData.type == ColumnType::SYMBOL) {
      auto *columnTypeMgr = charts()->columnTypeMgr();

      auto *symbolTypeType =
        columnTypeMgr->getTypeT<CQChartsColumnSymbolTypeType>(columnTypeData.type);

      const auto &model = this->currentModel();

      symbolTypeType->getMapData(charts(), model.data(), symbolTypeData.column(),
                                 columnTypeData.nameValues, symbolTypeData);
    }
  }

  symbolTypeData.setValid(true);
}

bool
CQChartsPlot::
columnSymbolType(int row, const QModelIndex &parent, const SymbolTypeData &symbolTypeData,
                 Symbol &symbolType) const
{
  if (! symbolTypeData.isValid())
    return false;

  auto *th = const_cast<Plot *>(this);

  ModelIndex symbolTypeModelInd(th, row, symbolTypeData.column(), parent);

  bool ok;

  auto var = modelValue(symbolTypeModelInd, ok);
  if (! ok || ! var.isValid()) return false;

  return varSymbolType(var, symbolTypeData, symbolType);
}

bool
CQChartsPlot::
varSymbolType(const QVariant &var, const SymbolTypeData &symbolTypeData, Symbol &symbolType) const
{
  auto uniqueInd = [&]() {
    int i = 0, n = 1;

    // use index of value in unique values to generate value in range
    // (CQChartsSymbolType::minValue, CQChartsSymbolType::maxValue)
    auto *columnDetails = this->columnDetails(symbolTypeData.column());

    if (columnDetails) {
      // use unique index/count of edit values (which may have been converted)
      // not same as CQChartsColumnColorType::userData
      // TODO: use map min/max
      n = columnDetails->numUnique();
      i = columnDetails->valueInd(var);
    }

    return std::pair<int, int>(i, n);
  };

  //---

  auto *symbolSetMgr = charts()->symbolSetMgr();
  auto *symbolSet    = symbolSetMgr->symbolSet(symbolTypeData.setName());

  // map symbol index into symbol set range
  auto interpInd = [&](long i) {
    Symbol symbolType;

    if (symbolSet) {
      CQChartsSymbolSet::SymbolData symbolData;

      if (symbolTypeData.isMapped())
        symbolData = symbolSet->interpI(int(i + symbolTypeData.mapMin()),
                                        int(symbolTypeData.mapMin()),
                                        int(symbolTypeData.mapMax()));
      else
        symbolData = symbolSet->interpI(int(i));

      symbolType = symbolData.symbol;
    }
    else {
      if (symbolTypeData.isMapped())
        symbolType = Symbol::interpOutlineWrap(int(i + symbolTypeData.mapMin()),
                                               int(symbolTypeData.mapMin()),
                                               int(symbolTypeData.mapMax()));
      else
        symbolType = Symbol::interpOutlineWrap(int(i + CQChartsSymbolType::minOutlineValue()));
    }

    return symbolType;
  };

  // map value in range (imin, imax) to (symbolTypeData.mapMin(), symbolTypeData.mapMax())
  auto mapData = [&](long i, long imin, long imax) {
    return CMathRound::Round(
             CMathUtil::map(double(i), double(imin), double(imax),
                            double(symbolTypeData.mapMin()), double(symbolTypeData.mapMax())));
  };

  //---

  // interpolate symbol from number in column range to symbol range
  if      (CQChartsVariant::isNumeric(var)) {
    // get integer value
    bool ok;
    auto i = CQChartsVariant::toInt(var, ok);

    if (! ok) {
      double r = CQChartsVariant::toReal(var, ok);

      i = CMathRound::Round(r);
    }

    //---

    if (symbolTypeData.isMapped()) {
      // map integer value in column range (map min/max) to symbol range (data min/max)
      symbolType = interpInd(mapData(i, int(symbolTypeData.dataMin()),
                                        int(symbolTypeData.dataMax())));
    }
    else {
      // use integer value directly (no column range map)
      symbolType = interpInd(i);
    }
  }
  else if (CQChartsVariant::isSymbol(var)) {
    // use symbol directly for type
    bool ok;
    symbolType = CQChartsVariant::toSymbol(var, ok);
  }
  else {
    if (symbolTypeData.isMapped()) {
      if (! symbolTypeData.typeMap().valueToSymbol(var, symbolType)) {
        // get unique id of value
        auto ind = uniqueInd();

        symbolType = interpInd(ind.first);
        //symbolType = interpInd(mapData(ind.first, 0, ind.second - 1));
      }
    }
    else {
      // init symbol from string
      // TODO: only if column marked as symbol ?
      bool ok;
      auto str = CQChartsVariant::toString(var, ok);

      symbolType = Symbol(str);
    }
  }

  return symbolType.isValid();
}

//------

CQChartsSymbolSet *
CQChartsPlot::
defaultSymbolSet() const
{
  auto *symbolSetMgr = charts()->symbolSetMgr();
  auto *symbolSet    = symbolSetMgr->symbolSet(defaultSymbolSetName_.length() ?
                         defaultSymbolSetName_ : "all");

  return symbolSet;
}

//------

void
CQChartsPlot::
initSymbolSizeData(SymbolSizeData &symbolSizeData) const
{
  bool initMap = ! symbolSizeData.isValid();

  symbolSizeData.setValid(false);

  if (! symbolSizeData.column().isValid())
    return;

  auto *columnDetails = this->columnDetails(symbolSizeData.column());
  if (! columnDetails) return;

  if (symbolSizeData.column().isGroup()) {
    symbolSizeData.setDataRange(0.0, std::max(numGroups() - 1, 0));
    symbolSizeData.setDataMean(symbolSizeData.dataMax()/2.0);
  }
  else {
    if (symbolSizeData.isMapped()) {
      auto minVar  = columnDetails->minValue();
      auto maxVar  = columnDetails->maxValue();
      auto meanVar = columnDetails->meanValue();

      symbolSizeData.setDataRange(CQChartsVariant::toRealOr(minVar, 0.0),
                                  CQChartsVariant::toRealOr(maxVar, 1.0));

      symbolSizeData.setDataMean(CQChartsVariant::toRealOr(meanVar,
        CMathUtil::avg(symbolSizeData.dataMin(), symbolSizeData.dataMax())));
    }

    ModelTypeData columnTypeData;

    (void) modelColumnValueType(symbolSizeData.column(), columnTypeData);

    if (columnTypeData.type == ColumnType::SYMBOL_SIZE) {
      auto *columnTypeMgr = charts()->columnTypeMgr();

      auto *symbolSizeType =
        columnTypeMgr->getTypeT<CQChartsColumnSymbolSizeType>(columnTypeData.type);

      const auto &model = this->currentModel();

      symbolSizeType->getMapData(charts(), model.data(), symbolSizeData.column(),
                                 columnTypeData.nameValues, symbolSizeData);
    }
  }

  if (initMap) {
    symbolSizeData.setUserMapMin(symbolSizeData.mapMin());
    symbolSizeData.setUserMapMax(symbolSizeData.mapMax());
  }

  symbolSizeData.setValid(true);
}

bool
CQChartsPlot::
columnSymbolSize(int row, const QModelIndex &parent, const SymbolSizeData &symbolSizeData,
                 Length &symbolSize, Qt::Orientation &sizeDir) const
{
  if (! symbolSizeData.isValid())
    return false;

  auto *th = const_cast<Plot *>(this);

  ModelIndex symbolSizeModelInd(th, row, symbolSizeData.column(), parent);

  bool ok;

  auto var = modelValue(symbolSizeModelInd, ok);
  if (! ok || ! var.isValid()) return false;

  return varSymbolSize(var, symbolSizeData, symbolSize, sizeDir);
}

bool
CQChartsPlot::
varSymbolSize(const QVariant &var, const SymbolSizeData &symbolSizeData,
              Length &symbolSize, Qt::Orientation &sizeDir) const
{
  auto uniqueInd = [&]() {
    int i = 0, n = 1;

    // use index of value in unique values to generate value in range
    // (CQChartsSymbolSize::minValue, CQChartsSymbolSize::maxValue)
    auto *columnDetails = this->columnDetails(symbolSizeData.column());

    if (columnDetails) {
      // use unique index/count of edit values (which may have been converted)
      // not same as CQChartsColumnColorSize::userData
      // TODO: use map min/max
      n = columnDetails->numUnique();
      i = columnDetails->valueInd(var);
    }

    return std::pair<int, int>(i, n);
  };

  /// map value in range (min, max) to (symbolSizeData.userMapMin(), symbolSizeData.userMapMax())
  auto mapData = [&](double r, double min, double max) {
    return CMathUtil::map(r, min, max, symbolSizeData.userMapMin(), symbolSizeData.userMapMax());
  };

  //---

  auto units = symbolSizeData.units();

  // interpolate size from number in column range to size
  if      (CQChartsVariant::isNumeric(var)) {
    if (symbolSizeData.isMapped()) {
      // map value in range (symbolSizeData.dataMin(), symbolSizeData.dataMax()) to
      // (symbolSizeData.userMapMin(), symbolSizeData.userMapMax())
      bool ok;
      double r  = CQChartsVariant::toReal(var, ok);
      double r1 = mapData(r, symbolSizeData.dataMin(), symbolSizeData.dataMax());

      symbolSize = Length(r1, units.type());
    }
    else {
      // use value directly for size
      bool ok;
      symbolSize = CQChartsVariant::toLength(var, ok);
    }
  }
  else if (CQChartsVariant::isLength(var)) {
    // use length directly for size
    bool ok;
    symbolSize = CQChartsVariant::toLength(var, ok);
  }
  else {
    if (symbolSizeData.isMapped()) {
      if (! symbolSizeData.sizeMap().valueToLength(var, symbolSize)) {
        // get unique id of value
        auto ind = uniqueInd();

        double r = mapData(double(ind.first), 0, double(ind.second - 1));

        symbolSize = Length(r, units.type());
      }
    }
    else {
      bool ok;
      auto str = CQChartsVariant::toString(var, ok);

      symbolSize = Length(str, units.type());
    }
  }

  sizeDir = symbolSizeData.direction();

  return symbolSize.isValid();
}

//------

void
CQChartsPlot::
initFontSizeData(FontSizeData &fontSizeData) const
{
  bool initMap = ! fontSizeData.isValid();

  fontSizeData.setValid(false);

  if (! fontSizeData.column().isValid())
    return;

  auto *columnDetails = this->columnDetails(fontSizeData.column());
  if (! columnDetails) return;

  if (fontSizeData.column().isGroup()) {
    fontSizeData.setDataRange(0.0, std::max(numGroups() - 1, 0));
  }
  else {
    if (fontSizeData.isMapped()) {
      auto minVar = columnDetails->minValue();
      auto maxVar = columnDetails->maxValue();

      fontSizeData.setDataRange(CQChartsVariant::toRealOr(minVar, 0.0),
                                CQChartsVariant::toRealOr(maxVar, 1.0));
    }

    ModelTypeData columnTypeData;

    (void) modelColumnValueType(fontSizeData.column(), columnTypeData);

    if (columnTypeData.type == ColumnType::FONT_SIZE) {
      auto *columnTypeMgr = charts()->columnTypeMgr();

      const auto *fontSizeType = dynamic_cast<const CQChartsColumnFontSizeType *>(
        columnTypeMgr->getType(columnTypeData.type));
      assert(fontSizeType);

      const auto &model = this->currentModel();

      fontSizeType->getMapData(charts(), model.data(), fontSizeData.column(),
                               columnTypeData.nameValues, fontSizeData);
    }
  }

  if (initMap) {
    fontSizeData.setUserMapMin(fontSizeData.mapMin());
    fontSizeData.setUserMapMax(fontSizeData.mapMax());
  }

  fontSizeData.setValid(true);
}

bool
CQChartsPlot::
columnFontSize(int row, const QModelIndex &parent, const FontSizeData &fontSizeData,
               Length &fontSize, Qt::Orientation &sizeDir) const
{
  if (! fontSizeData.isValid())
    return false;

  auto *th = const_cast<Plot *>(this);

  auto units = fontSizeData.units();

  ModelIndex fontSizeModelInd(th, row, fontSizeData.column(), parent);

  bool ok;

  auto var = modelValue(fontSizeModelInd, ok);
  if (! ok || ! var.isValid()) return false;

  if      (CQChartsVariant::isNumeric(var)) {
    if (fontSizeData.isMapped()) {
      // map value in range (fontSizeData.dataMin(), fontSizeData.dataMax()) to
      // (fontSizeData.userMapMin(), fontSizeData.userMapMax())
      double r = CQChartsVariant::toReal(var, ok);

      double r1 = CMathUtil::map(r, fontSizeData.dataMin(), fontSizeData.dataMax(),
                                 fontSizeData.userMapMin(), fontSizeData.userMapMax());

      fontSize = Length(r1, units.type());
    }
    else {
      // use value directly for size
      fontSize = CQChartsVariant::toLength(var, ok);
    }
  }
  else if (CQChartsVariant::isLength(var)) {
    // use length directly for size
    fontSize = CQChartsVariant::toLength(var, ok);
  }
  else {
    if (fontSizeData.isMapped()) {
      // use index of value in unique values to generate value in range
      // (CQChartsFontSize::minValue, CQChartsFontSize::maxValue)
      auto *columnDetails = this->columnDetails(fontSizeData.column());
      if (! columnDetails) return false;

      // use unique index/count of edit values (which may have been converted)
      // not same as CQChartsColumnColorSize::userData
      // TODO: use map min/max
      int n = columnDetails->numUnique();
      int i = columnDetails->valueInd(var);

//    double r = CMathUtil::map(i, 0, n - 1, CQChartsFontSize::minValue(),
//                              CQChartsFontSize::maxValue());
      double r = CMathUtil::map(i, 0, n - 1,
                                fontSizeData.userMapMin(), fontSizeData.userMapMax());

      fontSize = Length(r, units.type());
    }
    else {
      auto str = CQChartsVariant::toString(var, ok);

      fontSize = Length(str, units.type());
    }
  }

  sizeDir = fontSizeData.direction();

  return fontSize.isValid();
}

//------

QString
CQChartsPlot::
keyText() const
{
  QString text;

  text = this->titleStr();

  if (! text.length())
    text = this->id();

  return text;
}

//------

QString
CQChartsPlot::
posStr(const Point &w) const
{
  return xStr(w.x) + " " + yStr(w.y);
}

#if 0
QString
CQChartsPlot::
xStr(const QVariant &var) const
{
  bool ok;
  auto r = CQChartsVariant::toReal(var, ok);

  if (ok)
    return xStr(r);

  return var.toString();
}
#endif

QString
CQChartsPlot::
xStr(double x) const
{
  if (isLogX())
    x = expValue(x);

  if (xValueColumn().isValid())
    return columnStr(xValueColumn(), x);
  else
    return CQChartsUtil::formatReal(x);
}

#if 0
QString
CQChartsPlot::
yStr(const QVariant &var) const
{
  bool ok;
  auto r = CQChartsVariant::toReal(var, ok);

  if (ok)
    return yStr(r);

  return var.toString();
}
#endif

QString
CQChartsPlot::
yStr(double y) const
{
  if (isLogY())
    y = expValue(y);

  if (yValueColumn().isValid())
    return columnStr(yValueColumn(), y);
  else
    return CQChartsUtil::formatReal(y);
}

QString
CQChartsPlot::
columnStr(const Column &column, double r) const
{
  if (! column.isValid())
    return CQChartsUtil::formatReal(r);

  auto *model = this->currentModel().data();

  if (! model)
    return CQChartsUtil::formatReal(r);

  QString str;

  QVariant var(r);

  if (! formatColumnValue(mapColumn(column), var, str))
    return CQChartsUtil::formatReal(r);

  return str;
}

QString
CQChartsPlot::
columnValueToString(const Column &c, const QVariant &var) const
{
  auto *columnDetails = this->columnDetails(c);

  if (columnDetails && columnDetails->type() == CQBaseModelType::TIME)
    return columnDetails->dataName(var).toString();

  bool ok;
  return CQChartsVariant::toString(var, ok);
}

bool
CQChartsPlot::
formatColumnValue(const Column &column, const QVariant &var, QString &str) const
{
  auto *model = this->currentModel().data();
  if (! model) return false;

  return CQChartsModelUtil::formatColumnValue(charts(), model, mapColumn(column), var, str);
}

bool
CQChartsPlot::
keyPress(int key, int modifier)
{
  bool is_shift = (uint(modifier) & Qt::ShiftModifier  );
//bool is_ctrl  = (uint(modifier) & Qt::ControlModifier);

  if      (key == Qt::Key_Left || key == Qt::Key_Right ||
           key == Qt::Key_Up   || key == Qt::Key_Down) {
    if (view()->mode() != View::Mode::EDIT) {
      if      (key == Qt::Key_Right)
        panLeft(getPanX(is_shift));
      else if (key == Qt::Key_Left)
        panRight(getPanX(is_shift));
      else if (key == Qt::Key_Up)
        panDown(getPanY(is_shift));
      else if (key == Qt::Key_Down)
        panUp(getPanY(is_shift));
    }
    else {
      if      (key == Qt::Key_Right)
        handleEditMoveBy(Point( getMoveX(is_shift), 0));
      else if (key == Qt::Key_Left)
        handleEditMoveBy(Point(-getMoveX(is_shift), 0));
      else if (key == Qt::Key_Up)
        handleEditMoveBy(Point(0, getMoveY(is_shift)));
      else if (key == Qt::Key_Down)
        handleEditMoveBy(Point(0, -getMoveY(is_shift)));
    }
  }
  else if (key == Qt::Key_Plus || key == Qt::Key_Minus) {
    if (key == Qt::Key_Plus)
      zoomIn(getZoomFactor(is_shift));
    else
      zoomOut(getZoomFactor(is_shift));
  }
  else if (key == Qt::Key_Home) {
    zoomFull();
  }
  else if (key == Qt::Key_Tab || key == Qt::Key_Backtab) {
    if (key == Qt::Key_Tab)
      cycleNext();
    else
      cyclePrev();
  }
  else if (key == Qt::Key_F1) {
    if (! is_shift)
      cycleNext();
    else
      cyclePrev();
  }
  else if (key == Qt::Key_Period) {
    auto p = pixelToWindow(Point(view()->mapFromGlobal(QCursor::pos())));

    centerAt(p);
  }
  else {
    return false;
  }

  return true;
}

double
CQChartsPlot::
getPanX(bool is_shift) const
{
  // get pan x in view coords
  if (mappedXAxis()) {
    return windowToViewWidth(! is_shift ? mappedXAxis()->minorTickIncrement() :
                                          mappedXAxis()->majorTickIncrement());
  }
  else {
    return (! is_shift ? 0.125 : 0.25);
  }
}

double
CQChartsPlot::
getPanY(bool is_shift) const
{
  // get pan y in view coords
  if (mappedYAxis()) {
    return windowToViewHeight(! is_shift ? mappedYAxis()->minorTickIncrement() :
                                           mappedYAxis()->majorTickIncrement());
  }
  else {
    return (! is_shift ? 0.125 : 0.25);
  }
}

double
CQChartsPlot::
getMoveX(bool is_shift) const
{
  return (! is_shift ? 0.025 : 0.05);
}

double
CQChartsPlot::
getMoveY(bool is_shift) const
{
  return (! is_shift ? 0.025 : 0.05);
}

double
CQChartsPlot::
getZoomFactor(bool is_shift) const
{
  return (! is_shift ? 1.5 : 2.0);
}

//---

bool
CQChartsPlot::
executeSlot(const QString &name, const QStringList &args, QVariant &res)
{
  res = QVariant();

  //---

  auto p = namedSlots_.find(name);
  if (p == namedSlots_.end()) return false;

  const auto &argTypes = (*p).second;

  QVariantList values;

  view()->processSlotArgs(args, argTypes, values);

  return executeSlotFn(name, values, res);
}

bool
CQChartsPlot::
executeSlotFn(const QString &name, const QVariantList &args, QVariant &)
{
  bool ok;

  if      (name == "fit")
    autoFit();
  else if (name == "zoom_data")
    zoomData();
  else if (name == "zoom_full")
    zoomFull();
  else if (name == "show_color_key")
    setColorMapKey(CQChartsVariant::toBool(args[0], ok));
  else
    return false;

  return true;
}

void
CQChartsPlot::
registerSlot(const QString &name, const QStringList &argTypes)
{
  namedSlots_[name] = argTypes;
}

QStringList
CQChartsPlot::
getSlotNames() const
{
  QStringList names;

  for (const auto &p : namedSlots_)
    names.push_back(p.first);

  return names;
}

//---

void
CQChartsPlot::
propertyItemChanged(QObject *, const QString &)
{
}

//---

void
CQChartsPlot::
updateSlot()
{
  drawObjs();
}

//---

void
CQChartsPlot::
cycleNext()
{
  cycleNextPrev(/*prev*/false);
}

void
CQChartsPlot::
cyclePrev()
{
  cycleNextPrev(/*prev*/true);
}

void
CQChartsPlot::
cycleNextPrev(bool prev)
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->cycleNextPrev(prev);

  if (parentPlot())
    return parentPlot()->cycleNextPrev(prev);

  //---

  assert(! parentPlot());

  if (! insideData_.sizeObjs.empty()) {
    // TODO: handle overlay
    if (! prev)
      nextInsideInd();
    else
      prevInsideInd();

    setInsideObject();

    auto objText = insideObjectText();

    view()->setStatusText(objText);

    view()->updateTip();

    invalidateOverlay();
  }
}

//---

void
CQChartsPlot::
wheelHScroll(int delta)
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->wheelHScroll(delta);

  double panFactor = 0.50;

  if      (delta > 0)
    panLeft(panFactor);
  else if (delta < 0)
    panRight(panFactor);
}

void
CQChartsPlot::
wheelVScroll(int delta)
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->wheelVScroll(delta);

  double panFactor = 0.50;

  if      (delta > 0)
    panUp(panFactor);
  else if (delta < 0)
    panDown(panFactor);
}

void
CQChartsPlot::
wheelZoom(const Point &pp, int delta)
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->wheelZoom(pp, delta);

  if (! isAllowZoomX() && ! isAllowZoomY())
    return wheelVScroll(delta);

  double zoomFactor = 1.10;

  auto pp1 = pixelToWindow(pp);

  if      (delta > 0)
    updateDataScale(dataScale()*zoomFactor);
  else if (delta < 0)
    updateDataScale(dataScale()/zoomFactor);

  auto pp2 = pixelToWindow(pp); // mapping may have changed

  pan(pp1.x - pp2.x, pp1.y - pp2.y);

  if (delta < 0) {
    if (minDataScaleX_ > 0.0 && zoomData_.dataScale.x == minDataScaleX_ &&
        minDataScaleY_ > 0.0 && zoomData_.dataScale.x == minDataScaleY_) {
      autoFit();
    }
  }
}

//---

void
CQChartsPlot::
panLeft(double f)
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->panLeft(f);

  if (! isAllowPanX())
    return;

  double dx = viewToWindowWidth(f)/getDataRange().getWidth();

  auto panX = [&](Plot *plot) {
    plot->setDataOffsetX(plot->dataOffsetX() - dx);

    plot->adjustPan();

    plot->applyDataRangeAndDraw();
  };

  panX(this);

#if 0
  if (! isOverlay() && isY1Y2()) {
    Plots plots;

    y1y2Plots(plots);

    for (auto &plot : plots) {
      if (plot != this)
        panX(plot);
    }
  }
#endif

  Q_EMIT zoomPanChanged();
}

void
CQChartsPlot::
panRight(double f)
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->panRight(f);

  if (! isAllowPanX())
    return;

  double dx = viewToWindowWidth(f)/getDataRange().getWidth();

  auto panX = [&](Plot *plot) {
    plot->setDataOffsetX(plot->dataOffsetX() + dx);

    plot->adjustPan();

    plot->applyDataRangeAndDraw();
  };

  panX(this);

/*
  if (! isOverlay() && isY1Y2()) {
    Plots plots;

    y1y2Plots(plots);

    for (auto &plot : plots) {
      if (plot != this)
        panX(plot);
    }
  }
*/

  Q_EMIT zoomPanChanged();
}

void
CQChartsPlot::
panUp(double f)
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->panUp(f);

  if (! isAllowPanY())
    return;

  double dy = viewToWindowHeight(f)/getDataRange().getHeight();

  auto panY = [&](Plot *plot) {
    plot->setDataOffsetY(plot->dataOffsetY() + dy);

    plot->adjustPan();

    plot->applyDataRangeAndDraw();
  };

  panY(this);

/*
  if (! isOverlay() && isX1X2()) {
    Plots plots;

    x1x2Plots(plots);

    for (auto &plot : plots) {
      if (plot != this)
        panY(plot);
    }
  }
*/

  Q_EMIT zoomPanChanged();
}

void
CQChartsPlot::
panDown(double f)
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->panDown(f);

  if (! isAllowPanY())
    return;

  double dy = viewToWindowHeight(f)/getDataRange().getHeight();

  auto panY = [&](Plot *plot) {
    plot->setDataOffsetY(plot->dataOffsetY() - dy);

    plot->adjustPan();

    plot->applyDataRangeAndDraw();
  };

  panY(this);

#if 0
  if (! isOverlay() && isX1X2()) {
    Plots plots;

    x1x2Plots(plots);

    for (auto &plot : plots) {
      if (plot != this)
        panY(plot);
    }
  }
#endif

  Q_EMIT zoomPanChanged();
}

void
CQChartsPlot::
pan(double dx, double dy)
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->pan(dx, dy);

  if (isAllowPanX())
    setDataOffsetX(dataOffsetX() + dx/getDataRange().getWidth());

  if (isAllowPanY())
    setDataOffsetY(dataOffsetY() + dy/getDataRange().getHeight());

  adjustPan();

  applyDataRangeAndDraw();

  Q_EMIT zoomPanChanged();
}

//---

void
CQChartsPlot::
zoomIn(double f)
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->zoomIn(f);

  if (isAllowZoomX())
    setDataScaleX(dataScaleX()*f);

  if (isAllowZoomY())
    setDataScaleY(dataScaleY()*f);

  applyDataRangeAndDraw();

  Q_EMIT zoomPanChanged();
}

void
CQChartsPlot::
zoomOut(double f)
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->zoomOut(f);

  if (isAllowZoomX())
    setDataScaleX(dataScaleX()/f);

  if (isAllowZoomY())
    setDataScaleY(dataScaleY()/f);

  applyDataRangeAndDraw();

  Q_EMIT zoomPanChanged();
}

void
CQChartsPlot::
zoomTo(const BBox &bbox)
{
  if (isOverlay() && ! isFirstPlot()) {
    auto bbox1 = firstPlot()->pixelToWindow(windowToPixel(bbox));

    return firstPlot()->zoomTo(bbox1);
  }

  assert(! isComposite());

  if (! dataRange().isSet())
    return;

  double w = bbox.getWidth ();
  double h = bbox.getHeight();

  if (w < 1E-50 || h < 1E-50) {
    double dataScale = 2*std::min(dataScaleX(), dataScaleY());

    w = dataRange().xsize()/dataScale;
    h = dataRange().ysize()/dataScale;
  }

  auto c = bbox.getCenter();

  double w1 = dataRange().xsize();
  double h1 = dataRange().ysize();

  double xscale = w1/w;
  double yscale = h1/h;

  if (isAllowZoomX() && isAllowZoomY() && isEqualScale()) {
    xscale = std::min(xscale, yscale);
    yscale = xscale;
  }

  if (isAllowZoomX())
    setDataScaleX(xscale);

  if (isAllowZoomY())
    setDataScaleY(yscale);

  auto c1 = Point(dataRange().xmid(), dataRange().ymid());

  double cx = (isAllowPanX() ? c.x - c1.x : 0.0)/getDataRange().getWidth ();
  double cy = (isAllowPanY() ? c.y - c1.y : 0.0)/getDataRange().getHeight();

  setDataOffsetX(cx);
  setDataOffsetY(cy);

  applyDataRangeAndDraw();

  Q_EMIT zoomPanChanged();
}

void
CQChartsPlot::
unzoomTo(const BBox &bbox)
{
  if (isOverlay() && ! isFirstPlot()) {
    auto bbox1 = firstPlot()->pixelToWindow(windowToPixel(bbox));

    return firstPlot()->unzoomTo(bbox1);
  }

  assert(! isComposite());

  if (! dataRange().isSet())
    return;

  double w = bbox.getWidth ();
  double h = bbox.getHeight();

  auto c = bbox.getCenter();

  double w1 = dataRange().xsize();
  double h1 = dataRange().ysize();

  if (w1 < 1E-50 || h1 < 1E-50)
    return;

  double xscale = w*dataScaleX()/w1;
  double yscale = h*dataScaleY()/h1;

  if (isAllowZoomX() && isAllowZoomY() && isEqualScale()) {
    xscale = std::min(xscale, yscale);
    yscale = xscale;
  }

  if (isAllowZoomX())
    setDataScaleX(xscale*dataScaleX());

  if (isAllowZoomY())
    setDataScaleY(yscale*dataScaleY());

  auto c1 = Point(dataRange().xmid(), dataRange().ymid());

  double cx = (isAllowPanX() ? c.x - c1.x : 0.0)/getDataRange().getWidth ();
  double cy = (isAllowPanY() ? c.y - c1.y : 0.0)/getDataRange().getHeight();

  setDataOffsetX(cx);
  setDataOffsetY(cy);

  applyDataRangeAndDraw();

  Q_EMIT zoomPanChanged();
}

bool
CQChartsPlot::
isZoomFull() const
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->isZoomFull();

  if (isAllowZoomX() && ! CMathUtil::realEq(dataScaleX(), 1.0))
    return false;

  if (isAllowZoomY() && ! CMathUtil::realEq(dataScaleY(), 1.0))
    return false;

  if (! CMathUtil::realEq(dataOffsetX(), 0.0))
    return false;

  if (! CMathUtil::realEq(dataOffsetY(), 0.0))
    return false;

  return true;
}

void
CQChartsPlot::
zoomFull(bool notify)
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->zoomFull1(notify);

  zoomFull1(notify);
}

void
CQChartsPlot::
zoomFull1(bool notify)
{
  if (isAllowZoomX())
    setDataScaleX(1.0);

  if (isAllowZoomY())
    setDataScaleY(1.0);

  setDataOffsetX(0.0);
  setDataOffsetY(0.0);

  applyDataRangeAndDraw();

  if (notify)
    Q_EMIT zoomPanChanged();
}

void
CQChartsPlot::
zoomToData()
{
  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->zoomToData();

  // calc fit box
  BBox bbox;

  bbox += dataFitBBox();

  bbox += annotationsFitBBox();
  bbox += extraFitBBox      ();

  bbox.makeNonZero();

  //---

  zoomTo(bbox);
}

void
CQChartsPlot::
centerAt(const Point &c)
{
  if (isOverlay() && ! isFirstPlot()) {
    auto c1 = firstPlot()->pixelToWindow(windowToPixel(c));

    return firstPlot()->centerAt(c1);
  }

  assert(! isComposite());

  if (! dataRange().isSet())
    return;

  auto c1 = Point(dataRange().xmid(), dataRange().ymid());

  double cx = (isAllowPanX() ? c.x - c1.x : 0.0)/getDataRange().getWidth ();
  double cy = (isAllowPanY() ? c.y - c1.y : 0.0)/getDataRange().getHeight();

  setDataOffsetX(cx);
  setDataOffsetY(cy);

  applyDataRangeAndDraw();

  Q_EMIT zoomPanChanged();
}

void
CQChartsPlot::
updateTransform()
{
  postResize();

  drawObjs();
}

//------

void
CQChartsPlot::
setZoomScroll(bool b)
{
  scrollData_.zoomScroll = b;

  updateZoomScroll();
}

//------

bool
CQChartsPlot::
tipText(const Point &p, QString &tip) const
{
  return plotTipText(p, tip, /*single*/true);
}

// called for each plot by view
bool
CQChartsPlot::
plotTipText(const Point &p, QString &tip, bool single) const
{
  if (isOverlay() && ! isFirstPlot())
    return false;

  if (parentPlot())
    return false;

  //---

  int objNum  = 0;
  int numObjs = 0;

  Obj *tipObj = nullptr;

  Objs tipObjs;

  if (isFollowMouse() && insideData_.isSet()) {
    assert(! parentPlot());

    objNum = insideData_.ind;

    // get nth inside object
    tipObj = insideObject();

    tipObjs.clear();

    for (const auto &obj : insideData_.objs)
      tipObjs.push_back(obj);
  }
  else {
    // get selectable objects and annotations at point
    groupedObjsAtPoint(p, tipObjs, Constraints::SELECTABLE);

    // use first object for tip
    if (! tipObjs.empty())
      tipObj = *tipObjs.begin();
  }

  numObjs = int(tipObjs.size());

  if (tipObj) {
    if (single) {
      if (tip != "")
        tip += " ";

      tip += tipObj->tipId();

      if (numObjs > 1)
        tip += QString("<br><font color=\"blue\">&nbsp;&nbsp;%1 of %2</font>"
                       " <small>(F1=cycle,?=query,Ctrl+F1=lock)</small>").
                 arg(objNum + 1).arg(numObjs);
    }
    else {
      for (const auto &obj : tipObjs) {
        if (tip != "")
          tip += " ";

        tip += obj->tipId();
      }
    }
  }

  return tip.length();
}

void
CQChartsPlot::
addTipHeader(CQChartsTableTip &tableTip, const QModelIndex &ind) const
{
  if (! tipHeaderColumn().isValid()) return;

  auto *th = const_cast<Plot *>(this);

//ModelIndex tipModelInd(th, ind.row(), tipHeaderColumn(), ind.parent());

//auto tipInd  = modelIndex(tipModelInd);
  auto tipInd  = modelIndex(ind.row(), tipHeaderColumn(), ind.parent(), /*normalized*/true);
  auto tipInd1 = unnormalizeIndex(tipInd);

  ModelIndex tipModelInd1(th, tipInd1.row(), tipHeaderColumn(), tipInd1.parent());

  bool ok;
  auto value = modelValue(tipModelInd1, ok);
  if (! ok) return;

  auto valueStr = columnValueToString(tipHeaderColumn(), value);

  tableTip.addBoldLine(valueStr);

  tableTip.addColumn(tipHeaderColumn());
}

void
CQChartsPlot::
addTipColumns(CQChartsTableTip &tableTip, const QModelIndex &ind) const
{
  for (const auto &c : tipColumns().columns())
    addTipColumn(tableTip, c, ind);
}

void
CQChartsPlot::
addTipColumn(CQChartsTableTip &tableTip, const Column &c, const QModelIndex &ind) const
{
  if (! c.isValid()) return;

  if (tableTip.hasColumn(c))
    return;

  auto name = columnHeaderName(c, /*tip*/true);

  auto *th = const_cast<Plot *>(this);

//ModelIndex tipModelInd(th, ind.row(), c, ind.parent());

//auto tipInd  = modelIndex(tipModelInd);
  auto tipInd  = modelIndex(ind.row(), c, ind.parent(), /*normalized*/true);
  auto tipInd1 = unnormalizeIndex(tipInd);

  ModelIndex tipModelInd1(th, tipInd1.row(), c, tipInd1.parent());

  bool ok;
  auto value = modelValue(tipModelInd1, ok);
  if (! ok) value = "";

  auto valueStr = columnValueToString(c, value);

  tableTip.addTableRow(name, valueStr);

  tableTip.addColumn(c);
}

void
CQChartsPlot::
addNoTipColumns(CQChartsTableTip &tableTip) const
{
  for (const auto &c : noTipColumns().columns()) {
    if (! c.isValid()) continue;

    tableTip.addColumn(c);
  }
}

void
CQChartsPlot::
resetObjTips()
{
  for (auto &obj : plotObjects())
    obj->resetTipId();
}

//------

// get objects and annotations at point
void
CQChartsPlot::
groupedObjsAtPoint(const Point &p, Objs &objs, const Constraints &constraints) const
{
  PlotObjs plotObjs;

  groupedPlotObjsAtPoint(p, plotObjs, constraints);

  for (const auto &plotObj : plotObjs)
    objs.push_back(plotObj);

  //---

  Annotations annotations;

  groupedAnnotationsAtPoint(p, annotations, constraints);

  for (const auto &annotation : annotations)
    objs.push_back(annotation);
}

void
CQChartsPlot::
groupedPlotObjsAtPoint(const Point &p, PlotObjs &plotObjs, const Constraints &constraints) const
{
  if (isOverlay()) {
    processOverlayPlots([&](const Plot *plot) {
      auto p1 = p;

      if (plot != this)
        p1 = plot->pixelToWindow(windowToPixel(p));

      plot->plotObjsAtPoint(p1, plotObjs, constraints);
    });
  }
  else {
    plotObjsAtPoint(p, plotObjs, constraints);
  }
}

void
CQChartsPlot::
plotObjsAtPoint(const Point &p, PlotObjs &plotObjs, const Constraints &constraints) const
{
  assert(! isComposite());

  assert(useObjTree());

  // get all objects at point from quad tree
  PlotObjs plotObjs1;

  objTreeData_.tree->objectsAtPoint(p, plotObjs1);

  //---

  // filter to constraints
  auto iconstraints = static_cast<uint>(constraints);

  for (const auto &plotObj : plotObjs1) {
    if ((iconstraints & static_cast<uint>(Constraints::SELECTABLE)) && ! plotObj->isSelectable())
      continue;

    if ((iconstraints & static_cast<uint>(Constraints::EDITABLE)) && ! plotObj->isEditable())
      continue;

    if (clipRect_.isSet() && ! plotObj->rectIntersect(clipRect_, /*inside*/false))
      continue;

    plotObjs.push_back(plotObj);
  }
}

void
CQChartsPlot::
groupedAnnotationsAtPoint(const Point &p, Annotations &annotations,
                          const Constraints &constraints) const
{
  annotations.clear();

  if (isOverlay()) {
    processOverlayPlots([&](const Plot *plot) {
      auto p1 = p;

      if (plot != this)
        p1 = plot->pixelToWindow(windowToPixel(p));

      plot->annotationsAtPoint(p1, annotations, constraints);
    });
  }
  else {
    annotationsAtPoint(p, annotations, constraints);
  }
}

void
CQChartsPlot::
annotationsAtPoint(const Point &p, Annotations &annotations, const Constraints &constraints) const
{
  auto iconstraints = static_cast<uint>(constraints);

  for (const auto &annotation : this->annotations()) {
    if (! annotation->isVisible())
      continue;

    if (! annotation->contains(p))
      continue;

    if ((iconstraints & static_cast<uint>(Constraints::SELECTABLE)) && ! annotation->isSelectable())
      continue;

    if ((iconstraints & static_cast<uint>(Constraints::EDITABLE)) && ! annotation->isEditable())
      continue;

    if (clipRect_.isSet() && ! annotation->intersects(clipRect_, /*inside*/false))
      continue;

    annotations.push_back(annotation);
  }
}

void
CQChartsPlot::
groupedObjsIntersectRect(const BBox &r, Objs &objs, bool inside,
                         const Constraints &constraints) const
{
  PlotObjs plotObjs;

  groupedPlotObjsIntersectRect(r, plotObjs, inside, constraints);

  for (const auto &plotObj : plotObjs)
    objs.push_back(plotObj);

  //---

  Annotations annotations;

  annotationsIntersectRect(r, annotations, inside, constraints);

  for (const auto &annotation : annotations)
    objs.push_back(annotation);
}

void
CQChartsPlot::
groupedPlotObjsIntersectRect(const BBox &r, PlotObjs &plotObjs, bool inside,
                             const Constraints &constraints) const
{
  if (isOverlay()) {
    processOverlayPlots([&](const Plot *plot) {
      auto r1 = r;

      if (plot != this)
        r1 = windowToPixel(plot->pixelToWindow(r));

      plot->plotObjsIntersectRect(r1, plotObjs, inside, constraints);
    });
  }
  else {
    plotObjsIntersectRect(r, plotObjs, inside, constraints);
  }
}

void
CQChartsPlot::
plotObjsIntersectRect(const BBox &r, PlotObjs &plotObjs, bool inside,
                      const Constraints &constraints) const
{
  assert(! isComposite());

  assert(useObjTree());

  // get all objects intersecting rect from quad tree
  PlotObjs plotObjs1;

  if (inside && ! isScaleSymbolSize()) {
    PlotObjs plotObjs2;

    objTreeData_.tree->objectsIntersectRect(r, plotObjs1, false);

    for (const auto &plotObj : plotObjs1) {
      if (plotObj->rectIntersect(r, true))
        plotObjs2.push_back(plotObj);
    }

    plotObjs1 = plotObjs2;
  }
  else
    objTreeData_.tree->objectsIntersectRect(r, plotObjs1, inside);

  //---

  // filter to constraints
  auto iconstraints = static_cast<uint>(constraints);

  for (const auto &plotObj : plotObjs1) {
    if ((iconstraints & static_cast<uint>(Constraints::SELECTABLE)) && ! plotObj->isSelectable())
      continue;

    if ((iconstraints & static_cast<uint>(Constraints::EDITABLE)) && ! plotObj->isEditable())
      continue;

    if ((iconstraints & static_cast<uint>(Constraints::CLICKABLE)) && ! plotObj->isClickable())
      continue;

    if (clipRect_.isSet() && ! plotObj->rectIntersect(clipRect_, /*inside*/false))
      continue;

    plotObjs.push_back(plotObj);
  }
}

void
CQChartsPlot::
annotationsIntersectRect(const BBox &r, Annotations &annotations, bool inside,
                         const Constraints &constraints) const
{
  annotations.clear();

  if (isOverlay()) {
    processOverlayPlots([&](const Plot *plot) {
      auto r1 = r;

      if (plot != this)
        r1 = plot->pixelToWindow(windowToPixel(r));

      plot->annotationsIntersectRect1(r1, annotations, inside, constraints);
    });
  }
  else {
    annotationsIntersectRect1(r, annotations, inside, constraints);
  }
}

void
CQChartsPlot::
annotationsIntersectRect1(const BBox &r, Annotations &annotations, bool inside,
                          const Constraints &constraints) const
{
  auto iconstraints = static_cast<uint>(constraints);

  for (const auto &annotation : this->annotations()) {
    if (! annotation->isVisible())
      continue;

    if (! annotation->intersects(r, inside))
      continue;

    if ((iconstraints & static_cast<uint>(Constraints::SELECTABLE)) && ! annotation->isSelectable())
      continue;

    if ((iconstraints & static_cast<uint>(Constraints::EDITABLE)) && ! annotation->isEditable())
      continue;

    if ((iconstraints & static_cast<uint>(Constraints::CLICKABLE)) && ! annotation->isClickable())
      continue;

    if (clipRect_.isSet() && ! annotation->intersects(clipRect_, /*inside*/false))
      continue;

    annotations.push_back(annotation);
  }
}

bool
CQChartsPlot::
objNearestPoint(const Point &p, PlotObj* &obj) const
{
  obj = nullptr;

  assert(! isComposite());

  assert(useObjTree());

  double tx = dataRange().xsize()/32.0;
  double ty = dataRange().ysize()/32.0;

  return objTreeData_.tree->objectNearest(p, tx, ty, obj);
}

//---

void
CQChartsPlot::
addRootMenuItems(QMenu *menu)
{
  if (rootPlot()) {
    menu->addSeparator();

    addMenuAction(menu, "Collapse", SLOT(collapseRootSlot()));
  }
}

void
CQChartsPlot::
collapseRootSlot()
{
  assert(rootPlot());

  collapseRoot();
}

void
CQChartsPlot::
collapseRoot()
{
  auto *rootPlot = const_cast<CQChartsPlot *>(this->rootPlot());
  if (! rootPlot) return;

  rootPlot->setVisible(true);

  setVisible(false);

  view()->setCurrentPlot(rootPlot);

  rootPlot->notifyCollapse();
}

//---

QAction *
CQChartsPlot::
addMenuAction(QMenu *menu, const QString &name, const char *slot)
{
  return CQUtil::addAction(menu, name, this, slot);
}

QAction *
CQChartsPlot::
addMenuCheckedAction(QMenu *menu, const QString &name, bool isSet, const char *slot)
{
  return CQUtil::addCheckedAction(menu, name, isSet, this, slot);
}

//---

void
CQChartsPlot::
preResize()
{
  std::unique_lock<std::mutex> lock(resizeMutex_);

  interruptDraw();

  // TODO: added instead of drawObjs in postResize (force all buffers to update)
  invalidateLayers();
}

void
CQChartsPlot::
postResize()
{
  if (isOverlay() && ! isFirstPlot())
    return;

  if (parentPlot())
    return parentPlot()->postResize();

  //--

  bool updated = postResizeUpdateRange();

#if 0
  // TODO: does obj postResize need range set ?
  for (auto &obj : plotObjects())
    obj->postResize();
#endif

  // TODO: does key position need range set ?
  if (updated)
    updateKeyPosition(/*force*/true);

  if (! isOverview() && isAutoFit())
    setNeedsAutoFit(true);

  scrollData_.invalid = true;

  // TODO: replaced with invalidateLayers in preResize (without this mid layer not redrawn)
  //drawObjs();
}

bool
CQChartsPlot::
postResizeUpdateRange()
{
  applyDataRange();

  if (isEqualScale())
    resetDataRange(/*updateRange*/true, /*updateObjs*/false);

  return true;
}

void
CQChartsPlot::
updateKeyPosition(bool force)
{
  if (isOverlay()) {
    if (isFirstPlot())
      updatePlotKeyPosition(this, force);

    return;
  }

  if (parentPlot())
    return parentPlot()->updatePlotKeyPosition(this, force);

  //---

  updatePlotKeyPosition(this, force);
}

void
CQChartsPlot::
updatePlotKeyPosition(Plot *plot, bool force)
{
  // update plot key
  if (isKeyVisibleAndNonEmpty()) {
    if (force)
      key()->invalidateLayout();

    if (plot->dataRange().isSet())
      key()->updatePlotLocation();
  }

  // update all annotation keys
  for (auto &annotation : annotations()) {
    auto *keyAnnotation = dynamic_cast<CQChartsKeyAnnotation *>(annotation);
    if (! keyAnnotation) continue;

    auto *key = dynamic_cast<PlotKey *>(keyAnnotation->key());
    if (! key) continue;

    if (force)
      key->invalidateLayout();

    if (plot->dataRange().isSet())
      key->updatePlotLocation();
  }
}

//------

bool
CQChartsPlot::
printLayer(Layer::Type type, const QString &filename) const
{
  auto *image = layerImage(type);

  if (! image || image->isNull())
    return false;

  image->save(filename);

  return true;
}

QImage *
CQChartsPlot::
layerImage(Layer::Type type) const
{
  auto *layer = getLayer(type);
  if (! layer) return nullptr;

  const auto *buffer = getBuffer(layer->buffer());

  if (! buffer->image())
    return nullptr;

  return buffer->image();
}

//------

void
CQChartsPlot::
draw(QPainter *painter)
{
  if (isOverview() || ! isBufferLayers()) {
    initGroupedPlotObjs();

    //---

    drawPlotParts(painter);

    //---

    if (! isOverview())
      updateAutoFit();

    return;
  }

  //---

  bool        drawLayers  { false };
  UpdateState updateState { UpdateState::INVALID };

  {
  LockMutex lock(this, "draw");

  updateState = this->updateState();

  if      (updateState == UpdateState::READY) {
    drawLayers = true;

    setGroupedUpdateState(UpdateState::DRAWN);
  }
  else if (updateState == UpdateState::DRAWN) {
    drawLayers = true;
  }
  else if (updateState == UpdateState::INVALID) {
    drawLayers = true;
  }
  }

  //---

  if (! isSequential()) {
    if (drawLayers) {
      this->drawLayers(painter);

      updateAutoFit();
    }
    else {
      this->drawLayers(painter);

      this->drawBusy(painter, updateState);
    }
  }
  else {
    this->drawLayers(painter);
  }
}

bool
CQChartsPlot::
isBufferLayers() const
{
  return view()->isBufferLayers();
}

void
CQChartsPlot::
drawPlotParts(QPainter *painter) const
{
  drawParts(painter);

  auto *th = const_cast<Plot *>(this);

  if (plotObjsAdded_) {
    plotObjsAdded_ = false;

    Q_EMIT th->newPlotObjsDrawn();
  }

  Q_EMIT th->plotDrawn();
}

void
CQChartsPlot::
updateGroupedDraw()
{
  if (isOverlay()) {
    processOverlayPlots([&](Plot *plot) {
      plot->updateDraw();
    });
  }
  else {
    updateDraw();
  }
}

void
CQChartsPlot::
updateDraw()
{
  // only draw first plot for overlay plots
  if (isOverlay() && ! isFirstPlot())
    return;

  //---

  // init object tree(s)
  initObjTree();

  //---

  if (! isSequential()) {
    startDrawObjs();
  }
  else {
    // draw objs
    drawThread();

    postDraw();

    view()->doUpdate();
  }
}

void
CQChartsPlot::
startDrawObjs()
{
  if (parentPlot())
    parentPlot()->startDrawObjs();

  //---

  // ignore draw until after calc range and objs finished
  {
    LockMutex lock(this, "startDrawObjs1");

    auto updateState = this->updateState();

    if (updateState == UpdateState::CALC_RANGE)
      return;

    if (updateState == UpdateState::CALC_OBJS)
      return;

    interruptDraw();
  }

  //---

  assert(! parentPlot());

  {
  LockMutex lock(this, "startDrawObjs2");

  getBuffer(Buffer::Type::BACKGROUND)->setValid(false);
  getBuffer(Buffer::Type::MIDDLE    )->setValid(false);
  getBuffer(Buffer::Type::FOREGROUND)->setValid(false);

  updateData_.drawBusy.ind = -100;

  setGroupedUpdateState(UpdateState::DRAW_OBJS);

  updateData_.drawThread->exec(drawASync, this);

  startThreadTimer();
  }
}

//---

void
CQChartsPlot::
interruptDraw()
{
  if (parentPlot())
    return parentPlot()->interruptDraw();

  //---

  setInterrupt(true);

  waitDraw();

  setInterrupt(false);
}

void
CQChartsPlot::
syncDraw()
{
  if (debugUpdate_)
    std::cerr << "CQChartsPlot::syncDraw\n";

  syncState();

  waitRange();
  waitObjs ();
  waitDraw ();
}

void
CQChartsPlot::
waitDraw()
{
  if (debugUpdate_)
    std::cerr << "CQChartsPlot::waitDraw\n";

  if (isOverlay()) {
    processOverlayPlots([&](Plot *plot) {
      plot->execWaitDraw();
    });
  }
  else {
    execWaitDraw();
  }
}

void
CQChartsPlot::
execWaitDraw()
{
  if (debugUpdate_)
    std::cerr << "CQChartsPlot::execWaitDraw\n";

  assert(! parentPlot());

  auto updateState = this->updateState();

  while (updateState == UpdateState::DRAW_OBJS) {
    // if busy wait for draw thread to finish
    if (updateData_.drawThread->isBusy()) {
      (void) updateData_.drawThread->term();
      return;
    }

    if (isInterrupt())
      return;

    if (hasLockId())
      return;

    if (! isFirstPlot())
      break;

    threadTimerSlot();

    updateState = this->updateState();
  }
}

void
CQChartsPlot::
drawASync(Plot *plot)
{
  plot->drawThread();
}

void
CQChartsPlot::
drawThread()
{
  CQPerfTrace trace("CQChartsPlot::drawThread");

  //---

  view()->lockPainter(true);

  drawParts(view()->ipainter());

  view()->lockPainter(false);

  //---

  assert(! parentPlot());

  // mark thread done
  updateData_.drawThread->end();

  if (plotObjsAdded_) {
    plotObjsAdded_ = false;

    Q_EMIT newPlotObjsDrawn();
  }

  Q_EMIT plotDrawn();
}

void
CQChartsPlot::
drawBusy(QPainter *painter, const UpdateState &updateState) const
{
  assert(! parentPlot());

  if (updateData_.drawBusy.ind < 0) {
    ++updateData_.drawBusy.ind;
    return;
  }

  //---

  auto viewBBox = calcViewBBox();

  auto p1 = view()->windowToPixel(Point(viewBBox.getXMin(), viewBBox.getYMin()));
  auto p2 = view()->windowToPixel(Point(viewBBox.getXMax(), viewBBox.getYMax()));

  //---

  double xc = (p1.x + p2.x)/2.0;
  double yc = (p1.y + p2.y)/2.0;

  int ind = updateData_.drawBusy.ind/updateData_.drawBusy.multiple;

  double a  = 0.0;
  double da = 2.0*M_PI/updateData_.drawBusy.count;

  double r1 = 32;
  double r2 = 4;
  double r3 = 10;

  QPen   pen(Qt::NoPen);
  QBrush brush(updateData_.drawBusy.fgColor);

  painter->setBrush(brush);
  painter->setPen(pen);

  for (int i = 0; i < updateData_.drawBusy.count; ++i) {
    int i1 = i - ind;

    if (i1 < 0) i1 += updateData_.drawBusy.count;

    double r = i1*(r2 - r3)/(updateData_.drawBusy.count - 1) + r3;

    Point c(xc, yc);

    auto p1 = circlePoint(c, r1, a);

    BBox bbox(p1.x - r, p1.y - r, p1.x + r, p1.y + r);

    painter->drawEllipse(bbox.qrect());

    a += da;
  }

  ++updateData_.drawBusy.ind;

  if (updateData_.drawBusy.ind >= updateData_.drawBusy.count*updateData_.drawBusy.multiple)
    updateData_.drawBusy.ind = 0;

  QString text;

  if      (updateState == UpdateState::CALC_RANGE)
    text = "Calc Range";
  else if (updateState == UpdateState::CALC_OBJS)
    text = "Calc Objects";
  else if (updateState == UpdateState::DRAW_OBJS)
    text = "Draw Objects";

  if (text.length()) {
    auto color = Color::makeInterfaceValue(1.0);

    auto tc = charts()->interpColor(color, ColorInd());

    painter->setPen(tc);

    auto font = view()->viewFont(updateData_.drawBusy.font);

    QFontMetricsF fm(font);

    double tw = fm.horizontalAdvance(text);
    double ta = fm.ascent();

    double tx = xc - tw/2.0;
    double ty = yc + r1 + r3 + 4 + ta;

    painter->drawText(int(tx), int(ty), text);
  }
}

void
CQChartsPlot::
drawLayers(QPainter *painter) const
{
  assert(! parentPlot());

  for (auto &tb : buffers_) {
    auto *buffer = tb.second;

    if (buffer->isActive() && buffer->isValid())
      buffer->draw(painter);
  }

  //---

#ifdef CQCHARTS_MODULE_SHLIB
  drawModule(painter);
#endif
}

void
CQChartsPlot::
drawLayer(QPainter *painter, Layer::Type type) const
{
  assert(! parentPlot());

  auto *layer = getLayer(type);
  if (! layer) return;

  auto *buffer = getBuffer(layer->buffer());

  buffer->draw(painter, 0, 0);
}

//---

#ifdef CQCHARTS_MODULE_SHLIB
void
CQChartsPlot::
drawModule(QPainter *painter) const
{
  if (moduleData_.drawProc) {
    CQChartsModuleDrawData moduleDrawData;

    moduleDrawData.id      = moduleData_.id;
    moduleDrawData.painter = painter;

    auto pixelRect = calcPlotPixelRect();
    auto plotRect  = pixelToWindow(calcPlotPixelRect());

    moduleDrawData.pixel_xmin = pixelRect.getXMin();
    moduleDrawData.pixel_ymin = pixelRect.getYMin();
    moduleDrawData.pixel_xmax = pixelRect.getXMax();
    moduleDrawData.pixel_ymax = pixelRect.getYMax();

    moduleDrawData.plot_xmin = plotRect.getXMin();
    moduleDrawData.plot_ymin = plotRect.getYMin();
    moduleDrawData.plot_xmax = plotRect.getXMax();
    moduleDrawData.plot_ymax = plotRect.getYMax();

    (*moduleData_.drawProc)(&moduleDrawData);
  }
}
#endif

//---

void
CQChartsPlot::
drawParts(QPainter *painter) const
{
  CQPerfTrace trace("CQChartsPlot::drawParts");

  if (! calcVisible())
    return;

  //---

  auto *th = const_cast<Plot *>(this);

  th->updateZoomScroll();

  //---

  drawBackgroundParts(painter);

  //---

  drawMiddleParts(painter);

  //---

  drawForegroundParts(painter);

  //---

  drawOverlayParts(painter);

  //---

  if (zoomData_.isFullScreen()) {
    QImage image;

    if (isOverviewDisplayed()) {
      QPainter painter;

      auto initImage = [&](CQChartsBuffer *buffer) {
        if (image.isNull()) {
          image = CQChartsUtil::initImage(buffer->image()->size());

          painter.begin(&image);
        }
      };

      auto *bgBuffer = getBuffer(Buffer::Type::BACKGROUND);
      auto *mdBuffer = getBuffer(Buffer::Type::MIDDLE);
      auto *fgBuffer = getBuffer(Buffer::Type::FOREGROUND);

      if (bgBuffer->isActive() && bgBuffer->image()) {
        initImage(bgBuffer);
        painter.drawImage(0, 0, *bgBuffer->image());
      }

      if (mdBuffer->isActive() && mdBuffer->image()) {
        initImage(mdBuffer);

        painter.drawImage(0, 0, *mdBuffer->image());
      }

      if (fgBuffer->isActive() && fgBuffer->image()) {
        initImage(fgBuffer);

        painter.drawImage(0, 0, *fgBuffer->image());
      }

      painter.end();
    }

    th->saveOverview(&image);
  }
}

void
CQChartsPlot::
drawBackgroundParts(QPainter *painter) const
{
  CQPerfTrace trace("CQChartsPlot::drawBackgroundParts");

  assert(! parentPlot());

  auto *buffer = getBuffer(Buffer::Type::BACKGROUND);
  if (! buffer->isActive()) return;

  //---

  // check for background rects (plot bbox, data bbox, fit bbox), background axes,
  // background key, background annotations or custom background
  BackgroundParts bgParts;

  bgParts.rects       = hasBackgroundRects();
  bgParts.axes        = hasGroupedBgAxes();
  bgParts.key         = hasGroupedBgKey();
  bgParts.annotations = hasGroupedAnnotations(Layer::Type::BG_ANNOTATION);
  bgParts.custom      = hasBackgroundI();

  if (! bgParts.rects && ! bgParts.axes && ! bgParts.key &&
      ! bgParts.annotations && ! bgParts.custom) {
    buffer->clear();
    return;
  }

  //---

  auto *painter1 = beginPaint(buffer, painter);

  //---

  if (painter1) {
    auto *th = const_cast<Plot *>(this);

    CQChartsPlotPaintDevice device(th, painter1);

    drawBackgroundDeviceParts(&device, bgParts);

    //---

    if (debugQuadTree_) {
      assert(! isComposite());

      painter1->setPen(Qt::black);

      if (useObjTree())
        objTreeData_.tree->draw(painter1);
    }
  }

  //---

  endPaint(buffer);
}

void
CQChartsPlot::
drawBackgroundDeviceParts(PaintDevice *device, const BackgroundParts &bgParts) const
{
  const_cast<Plot *>(this)->initAxisSizes();

  //---

  // draw background (plot/data fill)
  if (bgParts.rects)
    drawBackgroundRects(device);

  //---

  // draw custom background
  if (bgParts.custom)
    drawCustomBackground(device);

  //---

  // draw axes/key below plot
  if (bgParts.axes)
    drawGroupedBgAxes(device);

  if (bgParts.key)
    drawBgKey(device);

  //---

  // draw annotations
  if (bgParts.annotations)
    drawGroupedAnnotations(device, Layer::Type::BG_ANNOTATION);
}

void
CQChartsPlot::
initAxisSizes()
{
  if (! isOverlay()) {
    const_cast<Plot *>(this)->clearAxisSideDelta();
    return;
  }

  //---

  if (this == firstPlot())
    const_cast<Plot *>(this)->clearAxisSideDelta();

  //---

  bool x1x2 = this->isY1Y2();

  if (x1x2 || (isOverlay() && isShowAllXOverlayAxes())) {
    // only init on first plot
    if (this != firstPlot()) return;

    // get overlay plots
    Plots oplots;

    overlayPlots(oplots);

    //---

    // init bottom axes
    double bdelta = 0.0;

    for (auto &oplot : oplots) {
      oplot->setXAxisSideDelta(CQChartsAxisSide::Type::BOTTOM_LEFT, bdelta);

      if (! oplot->xAxis())
        continue;

      if (oplot->xAxis()->position().isSet())
        continue;

      bdelta += oplot->xAxisHeight(CQChartsAxisSide::Type::BOTTOM_LEFT);
    }

    // init top axes
    double tdelta = 0.0;

    for (auto &oplot : oplots) {
      oplot->setXAxisSideDelta(CQChartsAxisSide::Type::TOP_RIGHT, tdelta);

      if (! oplot->xAxis())
        continue;

      if (oplot->xAxis()->position().isSet())
        continue;

      tdelta += oplot->xAxisHeight(CQChartsAxisSide::Type::TOP_RIGHT);
    }
  }

  //---

  bool y1y2 = this->isY1Y2();

  if (y1y2 || (isOverlay() && isShowAllYOverlayAxes())) {
    // only init on first plot
    if (this != firstPlot()) return;

    // get overlay plots
    Plots oplots;

    overlayPlots(oplots);

    //---

    // init left axes
    double ldelta = 0.0;

    for (auto &oplot : oplots) {
      oplot->setYAxisSideDelta(CQChartsAxisSide::Type::BOTTOM_LEFT, ldelta);

      if (! oplot->yAxis())
        continue;

      if (oplot->yAxis()->position().isSet())
        continue;

      ldelta += oplot->yAxisWidth(CQChartsAxisSide::Type::BOTTOM_LEFT);
    }

    // init right axes
    double rdelta = 0.0;

    for (auto &oplot : oplots) {
      oplot->setYAxisSideDelta(CQChartsAxisSide::Type::TOP_RIGHT, rdelta);

      if (! oplot->yAxis())
        continue;

      if (oplot->yAxis()->position().isSet())
        continue;

      rdelta += oplot->yAxisWidth(CQChartsAxisSide::Type::TOP_RIGHT);
    }
  }
}

void
CQChartsPlot::
drawMiddleParts(QPainter *painter) const
{
  CQPerfTrace trace("CQChartsPlot::drawMiddleParts");

  assert(! parentPlot());

  auto *buffer = getBuffer(Buffer::Type::MIDDLE);
  if (! buffer->isActive()) return;

  //---

  bool bg  = hasGroupedObjs(Layer::Type::BG_PLOT );
  bool mid = hasGroupedObjs(Layer::Type::MID_PLOT);
  bool fg  = hasGroupedObjs(Layer::Type::FG_PLOT );

  if (! bg && ! mid && ! fg) {
    buffer->clear();
    return;
  }

  //---

  auto *painter1 = beginPaint(buffer, painter);

  //---

  if (painter1) {
    auto *th = const_cast<Plot *>(this);

    CQChartsPlotPaintDevice device(th, painter1);

    drawMiddleDeviceParts(&device, bg, mid, fg);
  }

  //---

  endPaint(buffer);
}

void
CQChartsPlot::
drawMiddleDeviceParts(PaintDevice *device, bool bg, bool mid, bool fg) const
{
  // draw objects (background, mid, foreground)
  if (bg ) drawGroupedObjs(device, Layer::Type::BG_PLOT );
  if (mid) drawGroupedObjs(device, Layer::Type::MID_PLOT);
  if (fg ) drawGroupedObjs(device, Layer::Type::FG_PLOT );
}

void
CQChartsPlot::
drawForegroundParts(QPainter *painter) const
{
  CQPerfTrace trace("CQChartsPlot::drawForegroundParts");

  assert(! parentPlot());

  auto *buffer = getBuffer(Buffer::Type::FOREGROUND);
  if (! buffer->isActive()) return;

  //---

  ForegroundParts fgParts;

  // check for foreground axes, foreground key, foreground annotations, title or
  // custom foreground
  fgParts.axes        = hasGroupedFgAxes();
  fgParts.key         = hasGroupedFgKey();
  fgParts.annotations = hasGroupedAnnotations(Layer::Type::FG_ANNOTATION);
  fgParts.title       = hasTitle();
  fgParts.rulers      = hasRulers();
  fgParts.custom      = hasForegroundI();

  if (! fgParts.axes && ! fgParts.key && ! fgParts.annotations &&
      ! fgParts.title && ! fgParts.rulers && ! fgParts.custom) {
    buffer->clear();
    return;
  }

  //---

  auto *painter1 = beginPaint(buffer, painter);

  //---

  if (painter1) {
    auto *th = const_cast<Plot *>(this);

    CQChartsPlotPaintDevice device(th, painter1);

    fgParts.tabbed = (isTabbed() && isCurrent());

    drawForegroundDeviceParts(&device, fgParts);
  }

  //---

  endPaint(buffer);
}

void
CQChartsPlot::
drawForegroundDeviceParts(PaintDevice *device, const ForegroundParts &fgParts) const
{
  // draw annotations
  if (fgParts.annotations)
    drawGroupedAnnotations(device, Layer::Type::FG_ANNOTATION);

  //---

  // draw axes/key above plot
  if (fgParts.axes)
    drawGroupedFgAxes(device);

  if (fgParts.key)
    drawFgKey(device);

  //---

  // draw title
  if (fgParts.title)
    drawTitle(device);

  //---

  // draw rulers
  if (fgParts.rulers)
    drawRulers(device);

  //---

  // draw custom foreground
  if (fgParts.custom)
    drawCustomForeground(device);

  //---

  if (fgParts.tabbed)
    drawTabs(device);
}

//---

void
CQChartsPlot::
saveOverview(QImage *image)
{
  if (isOverviewDisplayed()) {
    auto pixelRect = calcPlotPixelRect();

    overviewData_.image = (image ? *image : QImage());

    if (! overviewData_.image.isNull()) {
      //auto transform = Qt::FastTransformation;
      auto transform = Qt::SmoothTransformation;

      if (pixelRect.getWidth() > pixelRect.getHeight())
        overviewData_.image = overviewData_.image.scaledToWidth(overviewData_.size, transform);
      else
        overviewData_.image = overviewData_.image.scaledToHeight(overviewData_.size, transform);
    }

    overviewData_.plotRect = calcPlotRect();
  }
  else {
    if (! overviewData_.image.isNull())
      overviewData_.image = QImage();
  }

  Q_EMIT overviewChanged();
}

//---

void
CQChartsPlot::
drawTabs(PaintDevice *device) const
{
  Plots plots;

  tabbedPlots(plots);

  drawTabs(device, plots);
}

void
CQChartsPlot::
drawTabs(PaintDevice *device, const Plots &plots) const
{
  Plot *currentPlot = nullptr;

  for (auto &plot : plots) {
    if (plot->isCurrent()) {
      currentPlot = plot;
      break;
    }
  }

  drawTabs(device, plots, currentPlot);
}

void
CQChartsPlot::
drawTabs(PaintDevice *device, const Plots &plots, Plot *currentPlot) const
{
  auto *th = const_cast<Plot *>(this);

  device->setFont(tabbedFont().font());

  th->tabData_.drawType = TabData::DrawType::TITLE;

  calcTabData(plots);

  auto plotBBox = calcPlotPixelRect();

  if (tabData_.ptw > plotBBox.getWidth()) {
    th->tabData_.drawType = TabData::DrawType::CIRCLES;

    calcTabData(plots);
  }

  QFontMetricsF fm(device->font());

  //---

  auto textColor        = Color::makeInterfaceValue(1.0);
  auto currentTextColor = Color::makeInterfaceValue(0.0);
  auto fillColor        = Color::makeInterfaceValue(0.3);
  auto currentFillColor = Color::makeInterfaceValue(0.6);
  auto borderColor      = Color::makeInterfaceValue(0.0);

  auto drawTab = [&](const BBox &rect, bool current) {
    device->setBrush(interpColor(current ? currentFillColor : fillColor, ColorInd()));
    device->setPen  (interpColor(borderColor, ColorInd()));

    if (tabData_.drawType == TabData::DrawType::TITLE) {
      device->fillRect(rect);
      device->drawRect(rect);
    }
    else {
      QFontMetricsF fm(tabbedFont().font());

      double ps = fm.height();

      double xs = pixelToWindowWidth (ps - 4);
      double ys = pixelToWindowHeight(ps - 4);

      BBox rect1(rect.getXMid() - xs/2.0, rect.getYMid() - ys/2.0,
                 rect.getXMid() + xs/2.0, rect.getYMid() + ys/2.0);

      device->drawEllipse(rect1);
    }
  };

  auto drawText = [&](const BBox &rect, const QString &text, bool current) {
    auto xc = rect.getXMid();
    auto yc = rect.getYMid();

    double tw1 = pixelToWindowWidth(fm.horizontalAdvance(text));

    device->setPen(interpColor(current ? currentTextColor : textColor, ColorInd()));

    double tyo = pixelToWindowHeight((fm.ascent() - fm.descent())/2.0);
    double th  = pixelToWindowHeight(tabData_.pth);

    device->drawText(Point(xc - tw1/2, yc - th/2 + tyo), text);
  };

  //---

  auto tabRect = this->calcTabPixelRect();

  double px = tabRect.getXMin();
  double py = tabRect.getYMin();

  for (auto &plot : plots) {
    BBox prect;

    QString title;
    double  ptw1 = 0.0;

    if (tabData_.drawType == TabData::DrawType::TITLE) {
      title = plot->titleStr();

      if (! title.length())
        title = plot->calcName();

      ptw1  = fm.horizontalAdvance(title) + 2*tabData_.pxm;
      prect = BBox(px, py, px + ptw1, py + tabData_.pth);
    }
    else {
      ptw1  = 4*tabData_.pxm;
      prect = BBox(px, py, px + ptw1, py + tabData_.pth);
    }

    plot->setTabRect(pixelToWindow(prect));

    drawTab(plot->tabRect(), plot == currentPlot);

    if (tabData_.drawType == TabData::DrawType::TITLE)
      drawText(plot->tabRect(), title, plot == currentPlot);

    px += ptw1;
  }
}

void
CQChartsPlot::
drawOverlayParts(QPainter *painter) const
{
  CQPerfTrace trace("CQChartsPlot::drawOverlayParts");

  assert(! parentPlot());

  auto *buffer = getBuffer(Buffer::Type::OVERLAY);
  if (! buffer->isActive()) return;

  //---

  // check for selected objects/annotations, debug boxes, edit handle,
  // mouse objects/annotations or custom overlay
  OverlayParts overlayParts;

  overlayParts.selObjs         = hasGroupedObjs(Layer::Type::SELECTION);
  overlayParts.selAnnotations  = hasGroupedAnnotations(Layer::Type::SELECTION);
  overlayParts.boxes           = hasGroupedBoxes();
  overlayParts.editHandles     = hasGroupedEditHandles();
  overlayParts.overObjs        = hasGroupedObjs(Layer::Type::MOUSE_OVER);
  overlayParts.overAnnotations = hasGroupedAnnotations(Layer::Type::MOUSE_OVER);
  overlayParts.rulers          = hasRulers();
  overlayParts.custom          = hasOverlayI();

  bool anyOverlayParts =
    overlayParts.selObjs || overlayParts.selAnnotations || overlayParts.boxes ||
    overlayParts.editHandles || overlayParts.overObjs || overlayParts.overAnnotations ||
    overlayParts.rulers || overlayParts.custom;

  if (! anyOverlayParts) {
    buffer->clear();
    return;
  }

  //---

  auto *painter1 = beginPaint(buffer, painter);

  //---

  if (painter1) {
    auto *th = const_cast<Plot *>(this);

    CQChartsPlotPaintDevice device(th, painter1);

    drawOverlayDeviceParts(&device, overlayParts);
  }

  //---

  endPaint(buffer);
}

void
CQChartsPlot::
drawOverlayDeviceParts(PaintDevice *device, const OverlayParts &overlayParts) const
{
  if (view()->isOverlayFade() && (overlayParts.overObjs || overlayParts.overAnnotations))
    drawOverlayFade(device);

  //---

  // draw custom overlay
  if (overlayParts.custom)
    drawCustomOverlay(device);

  //---

  // draw selection
  if (overlayParts.selObjs)
    drawGroupedObjs(device, Layer::Type::SELECTION);

  if (overlayParts.selAnnotations)
    drawGroupedAnnotations(device, Layer::Type::SELECTION);

  //---

  // draw debug boxes
  if (overlayParts.boxes)
    drawGroupedBoxes(device);

  //---

  // draw mouse over
  if (overlayParts.overObjs)
    drawGroupedObjs(device, Layer::Type::MOUSE_OVER);

  if (overlayParts.overAnnotations)
    drawGroupedAnnotations(device, Layer::Type::MOUSE_OVER);

  //---

  if (overlayParts.rulers)
    drawRulerMarks(device);

  //---

  // draw edit handles last
  if (overlayParts.editHandles) {
    if (device->isInteractive())
      drawGroupedEditHandles(device);
  }
}

void
CQChartsPlot::
drawOverlayFade(PaintDevice *device) const
{
  PenBrush penBrush;

  setPenBrush(penBrush, PenData(false),
    BrushData(true, interpPlotFillColor(ColorInd()), view()->overlayFadeAlpha()));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  auto rect = calcPlotRect();

  device->fillRect(rect);
}

//---

QStringList
CQChartsPlot::
clipTextsToLength(PaintDevice *device, const QStringList &strs, const BBox &bbox,
                  double clipLength, const Qt::TextElideMode &clipElide,
                  bool isScaled) const
{
  QStringList strs1;

  if (isScaled) {
    // calc text size
    QFontMetricsF fm(device->font());

    double tw = 0.0;

    for (int i = 0; i < strs.size(); ++i) {
      auto str1 = CQChartsDrawUtil::clipTextToLength(strs[i], device->font(),
                                                     clipLength, clipElide);

      tw = std::max(tw, fm.horizontalAdvance(str1));

      strs1.push_back(str1);
    }

    double th = strs1.size()*fm.height();

    //---

    // calc scale factor
    auto pbbox = this->windowToPixel(bbox);

    double sx = (tw > 0 ? pbbox.getWidth ()/tw : 1.0);
    double sy = (th > 0 ? pbbox.getHeight()/th : 1.0);

    double s = std::min(sx, sy);

    //---

    // scale font
    device->setFont(CQChartsUtil::scaleFontSize(device->font(), s), /*scale*/false);
  }
  else {
    for (int i = 0; i < strs.size(); ++i) {
      auto str1 = CQChartsDrawUtil::clipTextToLength(strs[i], device->font(),
                                                     clipLength, clipElide);

      strs1.push_back(str1);
    }
  }

  return strs1;
}

//---

bool
CQChartsPlot::
hasBackgroundRects() const
{
  // only first plot has background for overlay
  if (isOverlay() && ! isFirstPlot())
    return false;

  //---

  bool hasPlotBackground = (isPlotFilled() || isPlotStroked());
  bool hasDataBackground = (isDataFilled() || isDataStroked());
  bool hasFitBackground  = (isFitFilled () || isFitStroked ());

  if (! hasPlotBackground && ! hasDataBackground && ! hasFitBackground)
    return false;

  if (! isLayerActive(Layer::Type::BACKGROUND))
    return false;

  return true;
}

void
CQChartsPlot::
drawBackgroundRects(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsPlot::drawBackgroundRects");

  auto *th = const_cast<Plot *>(this);

  th->plotBgColor_ = QColor();

  if (isPlotFilled() || isPlotStroked())
    drawBackgroundRect(device, DrawRegion::PLOT, calcPlotRect(),
                       plotBrushData(ColorInd()), plotPenData(ColorInd()),
                       plotBorderSides());

  if (isFitFilled () || isFitStroked())
    drawBackgroundRect(device, DrawRegion::FIT, fitBBox(),
                       fitBrushData(ColorInd()), fitPenData(ColorInd()),
                       fitBorderSides());

  if (isDataFilled() || isDataStroked()) {
    auto isClip = (isDataRawClip() || isDataClip());

    device->save();

    if (isClip) {
      auto clipBBox = (isDataRawClip() ? unequalDataRange_.bbox() : displayRangeBBox());

      device->setClipRect(clipBBox);
    }

    auto drawBBox = (isDataRawRange() ? unequalDataRange_.bbox() : displayRangeBBox());

    drawBackgroundRect(device, DrawRegion::DATA, drawBBox,
                       dataBrushData(ColorInd()), dataPenData(ColorInd()),
                       dataBorderSides());

    device->restore();
  }
}

void
CQChartsPlot::
drawBackgroundRect(PaintDevice *device, const DrawRegion &drawRegion, const BBox &rect,
                   const BrushData &brushData, const PenData &penData, const Sides &sides) const
{
  if (brushData.isVisible()) {
    PenBrush penBrush;

    setBrush(penBrush,
      BrushData(true, brushData.color(), brushData.alpha(), brushData.pattern()));

    device->setBrush(penBrush.brush);

    device->fillRect(rect);

    //---

    auto *th = const_cast<Plot *>(this);

    th->plotBgColor_ = brushData.color();
  }

  if (penData.isVisible()) {
    PenBrush penBrush;

    setPen(penBrush,
      PenData(true, penData.color(), penData.alpha(), penData.width(), penData.dash()));

    device->setPen(penBrush.pen);

    drawBackgroundSides(device, drawRegion, rect, sides);
  }
}

void
CQChartsPlot::
drawBackgroundSides(PaintDevice *device, const DrawRegion & /*drawRegion*/,
                    const BBox &bbox, const Sides &sides) const
{
  if (sides.isAll()) {
    device->setBrush(Qt::NoBrush);

    device->drawRect(bbox);
  }
  else {
    if (sides.isTop   ()) device->drawLine(bbox.getUL(), bbox.getUR());
    if (sides.isLeft  ()) device->drawLine(bbox.getUL(), bbox.getLL());
    if (sides.isBottom()) device->drawLine(bbox.getLL(), bbox.getLR());
    if (sides.isRight ()) device->drawLine(bbox.getUR(), bbox.getLR());
  }
}

//---

bool
CQChartsPlot::
hasBackgroundI() const
{
  if (isOverlay()) {
    Plots oplots;

    overlayPlots(oplots);

    for (const auto &oplot : oplots)
      if (oplot->hasBackground())
        return true;
  }
  else {
    if (hasBackground())
      return true;
  }

  return false;
}

bool
CQChartsPlot::
hasBackground() const
{
  return false;
}

void
CQChartsPlot::
drawCustomBackground(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsPlot::drawCustomBackground");

  if (isOverlay()) {
    Plots oplots;

    overlayPlots(oplots);

    for (const auto &oplot : oplots) {
      if (oplot->hasBackground())
        oplot->execDrawBackground(device);
    }
  }
  else {
    if (this->hasBackground())
      execDrawBackground(device);
  }
}

void
CQChartsPlot::
execDrawBackground(PaintDevice *) const
{
}

//---

void
CQChartsPlot::
redrawAxis(CQChartsAxis *, bool wait)
{
  if (wait) {
    drawBackground();
    drawForeground();
  }
  else {
    invalidateLayer(CQChartsBuffer::Type::BACKGROUND);
    invalidateLayer(CQChartsBuffer::Type::FOREGROUND);
  }
}

bool
CQChartsPlot::
hasGroupedBgAxes() const
{
  if (isOverlay()) {
    if (! isFirstPlot())
      return false;

    bool anyAxis = processOverlayPlots([&](const Plot *plot) {
      return plot->hasBgAxes();
    }, false);

    if (! anyAxis)
      return false;
  }
  else {
    if (! hasBgAxes())
      return false;
  }

  //---

  if (! isLayerActive(Layer::Type::BG_AXES))
    return false;

  return true;
}

bool
CQChartsPlot::
hasBgAxes() const
{
  if (isOverview())
    return false;

  // just axis grid on background
  bool showXAxis = isXAxisVisible();
  bool showYAxis = isYAxisVisible();

  bool showXGrid = (showXAxis && ! xAxis()->isGridAbove() && xAxis()->isDrawGrid());
  bool showYGrid = (showYAxis && ! yAxis()->isGridAbove() && yAxis()->isDrawGrid());

  if (! showXGrid && ! showYGrid)
    return false;

  return true;
}

void
CQChartsPlot::
drawGroupedBgAxes(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsPlot::drawGroupedBgAxes");

  if (isOverlay()) {
    if (! isFirstPlot())
      return;

    processOverlayPlots([&](const Plot *plot) {
      device->setPlot(const_cast<Plot *>(plot));

      plot->drawBgAxes(device);
    });

    device->setPlot(const_cast<Plot *>(this));
  }
  else {
    drawBgAxes(device);
  }
}

void
CQChartsPlot::
drawBgAxes(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsPlot::drawBgAxes");

  drawBgXAxis(device);
  drawBgYAxis(device);
}

bool
CQChartsPlot::
drawBgXAxis(PaintDevice *device) const
{
  bool showXAxis = isXAxisVisible();
  bool showXGrid = (showXAxis && ! xAxis()->isGridAbove() && xAxis()->isDrawGrid());

  if (showXGrid) drawXGrid(device);

  return showXAxis;
}

bool
CQChartsPlot::
drawBgYAxis(PaintDevice *device) const
{
  bool showYAxis = isYAxisVisible();
  bool showYGrid = (showYAxis && ! yAxis()->isGridAbove() && yAxis()->isDrawGrid());

  //---

  if (showYGrid) drawYGrid(device);

  return showYAxis;
}

//---

bool
CQChartsPlot::
hasGroupedBgKey() const
{
  if (isOverview())
    return false;

  PlotKey *key1 = nullptr;

  if (isOverlay()) {
    // only draw key under first plot - use first plot key (for overlay)
    auto *plot = firstPlot();

    if (plot)
      key1 = plot->getFirstPlotKey();
  }
  else {
    key1 = this->key();
  }

  //---

  bool showKey = (key1 && ! key1->isAbove());

  if (! showKey)
    return false;

  //---

  if (! isLayerActive(Layer::Type::BG_KEY))
    return false;

  return true;
}

void
CQChartsPlot::
drawBgKey(PaintDevice *device) const
{
  if (isPreview())
    return;

  CQPerfTrace trace("CQChartsPlot::drawBgKey");

  if (isOverlay()) {
    // only draw key under first plot - use first plot key (for overlay)
    //auto *plot = firstPlot();

    processOverlayPlots([&](const Plot *plot) {
      auto *key = plot->key();
      if (! key) return;

      device->setPlot(const_cast<Plot *>(plot));

      key->draw(device);
    });

    device->setPlot(const_cast<Plot *>(this));
  }
  else {
    auto *key = this->key();

    if (key)
      key->draw(device);
  }
}

//---

bool
CQChartsPlot::
hasGroupedObjs(const Layer::Type &layerType) const
{
  // for overlay draw all combine objects on common layers
  if (isOverlay()) {
    if (! isFirstPlot())
      return false;

    bool anyObjs = processOverlayPlots([&](const Plot *plot) {
      return plot->hasObjs(layerType);
    }, false);

    if (! anyObjs)
      return false;
  }
  else {
    if (! hasObjs(layerType))
      return false;
  }

  //---

  if (! isLayerActive(layerType))
    return false;

  return true;
}

void
CQChartsPlot::
drawGroupedObjs(PaintDevice *device, const Layer::Type &layerType) const
{
  CQPerfTrace trace("CQChartsPlot::drawGroupedObjs");

  // for overlay draw all combine objects on common layers
  if (isOverlay()) {
    if (! isFirstPlot())
      return;

    processOverlayPlots([&](const Plot *plot) {
      device->setPlot(const_cast<Plot *>(plot));

      plot->execDrawObjs(device, layerType);
    });

    device->setPlot(const_cast<Plot *>(this));
  }
  else {
    execDrawObjs(device, layerType);
  }
}

bool
CQChartsPlot::
hasObjs(const Layer::Type &layerType) const
{
//auto bbox = displayRangeBBox();
  auto bbox = calcPlotViewRect();

  bool anyObjs = false;

  for (const auto &plotObj : plotObjects()) {
    if (! plotObj->isVisible())
      continue;

    // skip unselected objects on selection layer
    if      (layerType == Layer::Type::SELECTION) {
      if (! plotObj->isSelected())
        continue;
    }
    // skip non-inside objects on mouse over layer
    else if (layerType == Layer::Type::MOUSE_OVER) {
      if (! plotObj->isInside())
        continue;
    }

    //---

    // skip objects not inside plot
    if (isPlotClip() && ! objInsideBox(plotObj, bbox))
      continue;

    //---

    anyObjs = true;

    break;
  }

  if (! anyObjs)
    return false;

  //---

  if (! isLayerActive(layerType))
    return false;

  return true;
}

void
CQChartsPlot::
execDrawObjs(PaintDevice *device, const Layer::Type &layerType) const
{
  CQPerfTrace trace("CQChartsPlot::execDrawObjs");

  // set draw layer
  view()->setDrawLayerType(layerType);

  //---

  // init paint (clipped)
  device->save();

  setClipRect(device);

  //---

  if      (layerType == Layer::Type::BG_PLOT)
    preDrawBgObjs(device);
  else if (layerType == Layer::Type::FG_PLOT)
    preDrawFgObjs(device);
  else if (layerType == Layer::Type::MID_PLOT)
    preDrawObjs(device);

  //---

//auto bbox = displayRangeBBox();
  auto bbox = calcPlotViewRect();

  for (const auto &plotObj : plotObjects()) {
    if (! plotObj->isVisible())
      continue;

    if (plotObj->isFiltered())
      continue;

    // skip unselected objects on selection layer
    if      (layerType == Layer::Type::SELECTION) {
      if (! plotObj->isSelected())
        continue;
    }
    // skip non-inside objects on mouse over layer
    else if (layerType == Layer::Type::MOUSE_OVER) {
      if (! plotObj->isInside())
        continue;

      if (! plotObj->drawMouseOver())
        continue;
    }

    //---

    // skip objects not inside plot
    if (isPlotClip() && ! objInsideBox(plotObj, bbox))
      continue;

    //---

    bool isZoomText = plotObj->isZoomText().boolOr(this->isZoomText());

    auto *viewPlotDevice = dynamic_cast<CQChartsViewPlotPaintDevice *>(device);

    if (isZoomText && viewPlotDevice)
      viewPlotDevice->setZoomFont(true);

    // draw object on layer
    if      (layerType == Layer::Type::SELECTION) {
      plotObj->draw  (device);
      plotObj->drawFg(device);
    }
    else if (layerType == Layer::Type::MOUSE_OVER) {
      plotObj->draw  (device);
      plotObj->drawFg(device);
    }
    else {
      auto drawLayer = plotObj->drawLayer();

      if (drawLayer != PlotObj::DrawLayer::NONE) {
        bool draw = ((drawLayer == PlotObj::DrawLayer::BACKGROUND &&
                      layerType == Layer::Type::BG_PLOT) ||
                     (drawLayer == PlotObj::DrawLayer::MIDDLE &&
                      layerType == Layer::Type::MID_PLOT) ||
                     (drawLayer == PlotObj::DrawLayer::FOREGROUND &&
                      layerType == Layer::Type::FG_PLOT));

        if (draw)
          plotObj->draw(device);
      }
      else {
        if      (layerType == Layer::Type::BG_PLOT)
          plotObj->drawBg(device);
        else if (layerType == Layer::Type::FG_PLOT)
          plotObj->drawFg(device);
        else if (layerType == Layer::Type::MID_PLOT)
          plotObj->draw  (device);
      }
    }

    //---

    if (isZoomText && viewPlotDevice)
      viewPlotDevice->setZoomFont(false);

    //---

    // show debug box
    if (isShowBoxes() || (plotObj->isSelected() && showSelectedBoxes()))
      plotObj->drawDebugRect(device);
  }

  //---

  if      (layerType == Layer::Type::BG_PLOT)
    postDrawBgObjs(device);
  else if (layerType == Layer::Type::FG_PLOT)
    postDrawFgObjs(device);
  else if (layerType == Layer::Type::MID_PLOT)
    postDrawObjs(device);

  //---

  // reset draw layer
  view()->setDrawLayerType(Layer::Type::NONE);

  //---

  device->restore();
}

bool
CQChartsPlot::
objInsideBox(PlotObj *plotObj, const BBox &bbox) const
{
  return plotObj->rectIntersect(bbox, /*inside*/false);
}

//---

bool
CQChartsPlot::
hasGroupedFgAxes() const
{
  if (isOverlay()) {
    if (! isFirstPlot())
      return false;

    bool anyAxis = processOverlayPlots([&](const Plot *plot) {
      return plot->hasFgAxes();
    }, false);

    if (! anyAxis)
      return false;
  }
  else {
    if (! hasFgAxes())
      return false;
  }

  //---

  if (! isLayerActive(Layer::Type::FG_AXES))
    return false;

  return true;
}

bool
CQChartsPlot::
hasFgAxes() const
{
  if (isOverview())
    return false;

  bool showXAxis = isXAxisVisible();
  bool showYAxis = isYAxisVisible();

  if (! showXAxis && ! showYAxis)
    return false;

  return true;
}

void
CQChartsPlot::
drawGroupedFgAxes(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsPlot::drawGroupedFgAxes");

  if (isOverlay()) {
    if (! isFirstPlot())
      return;

    processOverlayPlots([&](const Plot *plot) {
      device->setPlot(const_cast<Plot *>(plot));

      plot->drawFgAxes(device);
    });

    device->setPlot(const_cast<Plot *>(this));
  }
  else {
    drawFgAxes(device);
  }
}

void
CQChartsPlot::
drawFgAxes(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsPlot::drawFgAxes");

  drawFgXAxis(device);
  drawFgYAxis(device);
}

bool
CQChartsPlot::
drawFgXAxis(PaintDevice *device) const
{
  bool showXAxis = isXAxisVisible();
  bool showXGrid = (showXAxis && xAxis()->isGridAbove() && xAxis()->isDrawGrid());

  if (showXGrid) drawXGrid(device);
  if (showXAxis) drawXAxis(device);

  return showXAxis;
}

bool
CQChartsPlot::
drawFgYAxis(PaintDevice *device) const
{
  bool showYAxis = isYAxisVisible();
  bool showYGrid = (showYAxis && yAxis()->isGridAbove() && yAxis()->isDrawGrid());

  if (showYGrid) drawYGrid(device);
  if (showYAxis) drawYAxis(device);

  return showYAxis;
}

void
CQChartsPlot::
drawXGrid(PaintDevice *device) const
{
  xAxis()->drawGrid(this, device);
}

void
CQChartsPlot::
drawYGrid(PaintDevice *device) const
{
  yAxis()->drawGrid(this, device);
}

void
CQChartsPlot::
drawXAxis(PaintDevice *device) const
{
  // Draw separate X axes if X1/X2 (separate x values and shared y values) or
  // showAllXOverlayAxes property is set for overlay.
  //
  // Note: X1/X2 uses individual plot's coordinates. Overlay uses shared (first plot) coordinates.
  //
  // TODO: Handle flip X

  bool x1x2 = this->isX1X2();

  auto drawPlotAxis = [&](Plot *plot, double pos) {
    if (x1x2) {
      device->setPlot(plot);

      plot->drawXAxisAt(device, plot, pos);

      device->setPlot(const_cast<Plot *>(this));
    }
    else
      plot->drawXAxisAt(device, const_cast<Plot *>(this), pos);
  };

  //---

  if      (x1x2 || (isOverlay() && isShowAllXOverlayAxes())) {
    // always draw on first plot
    if (this != firstPlot()) return;

    //---

    // get overlay plots
    Plots oplots;

    overlayPlots(oplots);

    //---

    // draw fixed position axes as is
    for (auto &oplot : oplots) {
      if (oplot->xAxis() && oplot->xAxis()->position().isSet())
        drawPlotAxis(oplot, oplot->xAxis()->position().real());
    }

    //---

    auto dataRange = calcDataRange();

    //if (dataRange.isSet())
    //  dataRange += extraFitBBox();

    // draw bottom axes
    double bpos = (dataRange.isSet() ? dataRange.getYMin() : 0.0);

    for (auto &oplot : oplots) {
      if (! oplot->xAxis())
        continue;

      if (oplot->xAxis()->position().isSet())
        continue;

      // draw x axis at adjusted position and move position down for next axis
      if (oplot->xAxis()->side().type() == CQChartsAxisSide::Type::BOTTOM_LEFT)
        drawPlotAxis(oplot, bpos);

      bpos -= oplot->xAxisHeight(CQChartsAxisSide::Type::BOTTOM_LEFT);
    }

    // draw top axes
    double tpos = (dataRange.isSet() ? dataRange.getYMax() : 1.0);

    for (auto &oplot : oplots) {
      if (! oplot->xAxis())
        continue;

      if (oplot->yAxis()->position().isSet())
        continue;

      // draw x axis at adjusted position and move position up for next axis
      if (oplot->xAxis()->side().type() == CQChartsAxisSide::Type::TOP_RIGHT)
        drawPlotAxis(oplot, tpos);

      tpos += oplot->xAxisHeight(CQChartsAxisSide::Type::TOP_RIGHT);
    }
  }
  else {
    drawXAxis1(device);
  }
}

void
CQChartsPlot::
drawXAxisAt(PaintDevice *device, Plot *plot, double pos) const
{
  xAxis()->drawAt(pos, plot, device);
}

void
CQChartsPlot::
drawXAxis1(PaintDevice *device) const
{
  xAxis()->draw(this, device);
}

void
CQChartsPlot::
drawYAxis(PaintDevice *device) const
{
  // Draw separate Y axes if Y1/Y2 (separate y values and shared x values) or
  // showAllYOverlayAxes property is set for overlay.
  //
  // Note: Y1/Y2 uses individual plot's coordinates. Overlay uses shared (first plot) coordinates.
  //
  // TODO: Handle flip Y

  bool y1y2 = this->isY1Y2();

  auto drawPlotAxis = [&](Plot *plot, double pos) {
    if (y1y2) {
      device->setPlot(plot);

      plot->drawYAxisAt(device, plot, pos);

      device->setPlot(const_cast<Plot *>(this));
    }
    else
      plot->drawYAxisAt(device, const_cast<Plot *>(this), pos);
  };

  //---

  if      (y1y2 || (isOverlay() && isShowAllYOverlayAxes())) {
    // always draw on first plot
    if (this != firstPlot()) return;

    //---

    // get overlay plots
    Plots oplots;

    overlayPlots(oplots);

    //---

    // draw fixed position axes as is
    for (auto &oplot : oplots) {
      if (oplot->yAxis() && oplot->yAxis()->position().isSet())
        drawPlotAxis(oplot, oplot->yAxis()->position().real());
    }

    //---

    auto dataRange = calcDataRange();

    //if (dataRange.isSet())
    //  dataRange += extraFitBBox();

    // draw left axes
    double lpos = (dataRange.isSet() ? dataRange.getXMin() : 0.0);

    for (auto &oplot : oplots) {
      if (! oplot->yAxis())
        continue;

      if (oplot->yAxis()->position().isSet())
        continue;

      // draw y axis at adjusted position and move position left for next axis
      if (oplot->yAxis()->side().type() == CQChartsAxisSide::Type::BOTTOM_LEFT)
        drawPlotAxis(oplot, lpos);

      double w = oplot->yAxisWidth(CQChartsAxisSide::Type::BOTTOM_LEFT);

      if (isShowBoxes())
        drawWindowColorBox(device,
          BBox(lpos - w, dataRange.getYMin(), lpos, dataRange.getYMax()), Qt::green);

      lpos -= w;
    }

    // draw right axes
    double rpos = (dataRange.isSet() ? dataRange.getXMax() : 1.0);

    for (auto &oplot : oplots) {
      if (! oplot->yAxis())
        continue;

      if (oplot->yAxis()->position().isSet())
        continue;

      // draw y axis at adjusted position and move position left for next axis
      if (oplot->yAxis()->side().type() == CQChartsAxisSide::Type::TOP_RIGHT)
        drawPlotAxis(oplot, rpos);

      double w = oplot->yAxisWidth(CQChartsAxisSide::Type::TOP_RIGHT);

      if (isShowBoxes())
        drawWindowColorBox(device,
          BBox(rpos, dataRange.getYMin(), rpos + w, dataRange.getYMax()), Qt::green);

      rpos += w;
    }
  }
  else {
    drawYAxis1(device);
  }
}

void
CQChartsPlot::
drawYAxisAt(PaintDevice *device, Plot *plot, double pos) const
{
  yAxis()->drawAt(pos, plot, device);
}

void
CQChartsPlot::
drawYAxis1(PaintDevice *device) const
{
  yAxis()->draw(this, device);
}

double
CQChartsPlot::
xAxisHeight(const CQChartsAxisSide::Type &side) const
{
  if (xAxis()->side().type() == side) {
    if (xAxis()->bbox().isSet())
      return xAxis()->bbox().getHeight();
    else
      return pixelToWindowHeight(8); // TODO
  }
  else
    return 0.0;
}

double
CQChartsPlot::
yAxisWidth(const CQChartsAxisSide::Type &side) const
{
  if (yAxis()->side().type() == side) {
    if (yAxis()->bbox().isSet())
      return yAxis()->bbox().getWidth();
    else
      return pixelToWindowWidth(8); // TODO
  }
  else
    return 0.0;
}

//---

bool
CQChartsPlot::
hasGroupedFgKey() const
{
  if (isOverview())
    return false;

  PlotKey *key1 = nullptr;

  if (isOverlay()) {
    // only draw fg key on last plot - use first plot key (for overlay)
    auto *plot = lastPlot();

    if (plot)
      key1 = plot->getFirstPlotKey();
  }
  else {
    key1 = this->key();
  }

  //---

  bool showKey = (key1 && key1->isAbove());

  if (! showKey)
    return false;

  //---

  if (! isLayerActive(Layer::Type::FG_KEY))
    return false;

  return true;
}

void
CQChartsPlot::
drawFgKey(PaintDevice *device) const
{
  if (isPreview())
    return;

  CQPerfTrace trace("CQChartsPlot::drawFgKey");

  if (isOverlay()) {
    // only draw key above last plot - use first plot key (for overlay)
    //auto *plot = lastPlot();

    processOverlayPlots([&](const Plot *plot) {
      auto *key = plot->key();
      if (! key) return;

      device->setPlot(const_cast<Plot *>(plot));

      key->draw(device);
    });

    device->setPlot(const_cast<Plot *>(this));
  }
  else {
    auto *key = this->key();

    if (key)
      key->draw(device);
  }
}

//---

bool
CQChartsPlot::
hasTitle() const
{
  // only first plot has title for overlay
  if (isOverlay() && ! isFirstPlot())
    return false;

  assert(! isComposite());

  //---

  if (! title() || ! title()->isDrawn())
    return false;

  //---

  if (! isLayerActive(Layer::Type::TITLE))
    return false;

  return true;
}

void
CQChartsPlot::
drawTitle(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsPlot::drawTitle");

  title()->draw(device);
}

//---

bool
CQChartsPlot::
hasRulers() const
{
  return isRulerDisplayed();
}

void
CQChartsPlot::
drawRulers(PaintDevice *device) const
{
  auto rawRange      = getDataRange(); // unzoomed
  auto adjustedRange = adjustDataRangeBBox(rawRange); // zoomed

  CInterval xinterval(adjustedRange.getXMin(), adjustedRange.getXMax(), 20);
  CInterval yinterval(adjustedRange.getYMin(), adjustedRange.getYMax(), 20);

  auto pixelRect = calcPlotPixelRect();

  auto ll = pixelToWindow(pixelRect.getLL());
  auto ur = pixelToWindow(pixelRect.getUR());

  int rulerSize = rulerData_.pixelSize;

  auto xs = pixelToWindowWidth (rulerSize);
  auto ys = pixelToWindowHeight(rulerSize);

  auto majorX = pixelToWindowWidth (rulerSize*0.8);
  auto majorY = pixelToWindowHeight(rulerSize*0.8);

  auto minorX = pixelToWindowWidth (rulerSize*0.3);
  auto minorY = pixelToWindowHeight(rulerSize*0.3);

  auto bg = interpColor(Color::makeInterfaceValue(0.2), ColorInd());
  auto fg = interpColor(Color::makeInterfaceValue(1.0), ColorInd());

  device->save();

  device->setFont(CQChartsUtil::scaleFontSize(device->font(),
    rulerData_.fontScale), /*scale*/false);

  // left
  device->setBrush(bg);

  device->fillRect(BBox(ll.x, ur.y + ys, ll.x + xs, ll.y - ys));

  device->setBrush(fg);

  for (uint i = 0; i < yinterval.calcNumMajor(); ++i) {
    auto y = yinterval.interval(i);

    device->drawLine(Point(ll.x + xs - majorX, y), Point(ll.x + xs, y));

    for (uint j = 1; j < yinterval.calcNumMinor(); ++j) {
      auto y = yinterval.minorInterval(i, j);

      device->drawLine(Point(ll.x + xs - minorX, y), Point(ll.x + xs, y));
    }

    auto text = (mappedYAxis() ? mappedYAxis()->valueStr(y) : QString::number(y));

    device->drawText(Point(ll.x + xs/rulerSize, y + 2.0*ys/rulerSize), text);
  }

  // top
  QFontMetricsF fm(device->font());

  auto fh = pixelToWindowHeight(fm.height());

  device->setBrush(bg);

  device->fillRect(BBox(ll.x + xs, ll.y - ys, ur.x - xs, ll.y));

  device->setBrush(fg);

  for (uint i = 0; i < xinterval.calcNumMajor(); ++i) {
    auto x = xinterval.interval(i);

    device->drawLine(Point(x, ll.y - ys), Point(x, ll.y - ys + majorY));

    for (uint j = 1; j < xinterval.calcNumMinor(); ++j) {
      auto x = xinterval.minorInterval(i, j);

      device->drawLine(Point(x, ll.y - ys), Point(x, ll.y - ys + minorY));
    }

    auto text = (mappedXAxis() ? mappedXAxis()->valueStr(x) : QString::number(x));

    device->drawText(Point(x + 2.0*xs/rulerSize, ll.y - fh), text);
  }

  device->restore();
}

void
CQChartsPlot::
drawRulerMarks(PaintDevice *device) const
{
  auto rawRange      = getDataRange(); // unzoomed
  auto adjustedRange = adjustDataRangeBBox(rawRange); // zoomed

  auto pixelRect = calcPlotPixelRect();

  auto ll = pixelToWindow(pixelRect.getLL());
//auto ur = pixelToWindow(pixelRect.getUR());

  int rulerSize = rulerData_.pixelSize;

  auto xs = pixelToWindowWidth (rulerSize);
  auto ys = pixelToWindowHeight(rulerSize);

  auto xs1 = 0.5*xs;
  auto ys1 = 0.5*ys;

  auto bg = interpColor(Color::makeInterfaceValue(0.0), ColorInd());
  auto fg = interpColor(Color::makeInterfaceValue(1.0), ColorInd());

  QPen pen;
  pen.setColor(fg);
  device->setPen(pen);
  device->setBrush(bg);

  if (mouseData_.movePoint.y >= adjustedRange.getYMin() &&
      mouseData_.movePoint.y <= adjustedRange.getYMax()) {
    QPainterPath xpath;
    xpath.moveTo(ll.x + xs - xs1, mouseData_.movePoint.y + ys1);
    xpath.lineTo(ll.x + xs      , mouseData_.movePoint.y      );
    xpath.lineTo(ll.x + xs - xs1, mouseData_.movePoint.y - ys1);
    xpath.closeSubpath();

    device->drawLine(Point(ll.x     , mouseData_.movePoint.y),
                     Point(ll.x + xs, mouseData_.movePoint.y));
    device->drawPath(xpath);
  }

  if (mouseData_.movePoint.x >= adjustedRange.getXMin() &&
      mouseData_.movePoint.x <= adjustedRange.getXMax()) {
    QPainterPath ypath;
    ypath.moveTo(mouseData_.movePoint.x - xs1, ll.y - ys + ys1);
    ypath.lineTo(mouseData_.movePoint.x      , ll.y - ys      );
    ypath.lineTo(mouseData_.movePoint.x + xs1, ll.y - ys + ys1);
    ypath.closeSubpath();

    device->drawLine(Point(mouseData_.movePoint.x, ll.y - ys),
                     Point(mouseData_.movePoint.x, ll.y     ));
    device->drawPath(ypath);
  }
}

//---

bool
CQChartsPlot::
hasGroupedAnnotations(const Layer::Type &layerType) const
{
  // for overlay draw all combine objects on common layers
  if (isOverlay()) {
    if (! isFirstPlot())
      return false;

    bool anyObjs = processOverlayPlots([&](const Plot *plot) {
      return plot->hasAnnotations(layerType);
    }, false);

    if (! anyObjs)
      return false;
  }
  else {
    if (! hasAnnotations(layerType))
      return false;
  }

  //---

  if (! isLayerActive(layerType))
    return false;

  return true;
}

void
CQChartsPlot::
drawGroupedAnnotations(PaintDevice *device, const Layer::Type &layerType) const
{
  CQPerfTrace trace("CQChartsPlot::drawGroupedAnnotations");

  if (! hasGroupedAnnotations(layerType))
    return;

  //---

  // for overlay draw all combine objects on common layers
  if (isOverlay()) {
    if (! isFirstPlot())
      return;

    processOverlayPlots([&](const Plot *plot) {
      device->setPlot(const_cast<Plot *>(plot));

      plot->drawAnnotations(device, layerType);
    });

    device->setPlot(const_cast<Plot *>(this));
  }
  else {
    drawAnnotations(device, layerType);
  }
}

bool
CQChartsPlot::
hasAnnotations(const Layer::Type &layerType) const
{
  auto isAnnotationLayer = [&](const Annotation *annotation, const CQChartsLayer::Type &layerType) {
    return ((layerType == Layer::Type::BG_ANNOTATION &&
             annotation->drawLayer() == Annotation::DrawLayer::BACKGROUND) ||
            (layerType == Layer::Type::FG_ANNOTATION &&
             annotation->drawLayer() == Annotation::DrawLayer::FOREGROUND) ||
            (layerType == Layer::Type::MOUSE_OVER &&
             annotation->drawLayer() == Annotation::DrawLayer::MOUSE_OVER));
  };

  //---

  bool anyObjs = false;

  for (const auto &annotation : annotations()) {
    if (! annotation->isVisible())
      continue;

    if      (layerType == Layer::Type::SELECTION) {
      if (! annotation->isSelected())
        continue;
    }
    else if (layerType == Layer::Type::MOUSE_OVER) {
      if (annotation->drawLayer() != Annotation::DrawLayer::MOUSE_OVER &&
          ! annotation->isInside())
        continue;
    }
    else {
      if (! isAnnotationLayer(annotation, layerType))
        continue;
    }

//  if (! bbox.overlaps(annotation->rect()))
//    continue;

    anyObjs = true;

    break;
  }

  if (! anyObjs)
    return false;

  //---

  if (! isLayerActive(layerType))
    return false;

  return true;
}

void
CQChartsPlot::
drawAnnotations(PaintDevice *device, const Layer::Type &layerType) const
{
  auto isAnnotationLayer = [&](const Annotation *annotation, const CQChartsLayer::Type &layerType) {
    return ((layerType == Layer::Type::BG_ANNOTATION &&
             annotation->drawLayer() == Annotation::DrawLayer::BACKGROUND) ||
            (layerType == Layer::Type::FG_ANNOTATION &&
             annotation->drawLayer() == Annotation::DrawLayer::FOREGROUND) ||
            (layerType == Layer::Type::MOUSE_OVER &&
             annotation->drawLayer() == Annotation::DrawLayer::MOUSE_OVER));
  };

  //---

  CQPerfTrace trace("CQChartsPlot::drawAnnotations");

  // set draw layer
  view()->setDrawLayerType(layerType);

  //---

  for (auto &annotation : annotations()) {
    if (! annotation->isVisible())
      continue;

    // drawn by group
    if (annotation->group())
      continue;

    if      (layerType == Layer::Type::SELECTION) {
      if (! annotation->isSelected())
        continue;
    }
    else if (layerType == Layer::Type::MOUSE_OVER) {
      if (! annotation->isInside()) {
        if (annotation->drawLayer() != Annotation::DrawLayer::MOUSE_OVER)
          continue;

        if (! annotation->isMouseOverVisible())
          continue;
      }
    }
    else {
      if (! isAnnotationLayer(annotation, layerType))
        continue;
    }

//  if (! bbox.overlaps(annotation->rect()))
//    continue;

    annotation->draw(device);
  }

  //---

  // reset draw layer
  view()->setDrawLayerType(Layer::Type::NONE);
}

//---

bool
CQChartsPlot::
hasForegroundI() const
{
  if (isOverlay()) {
    Plots oplots;

    overlayPlots(oplots);

    for (const auto &oplot : oplots)
      if (oplot->hasForeground())
        return true;
  }
  else {
    if (hasForeground())
      return true;
  }

  return false;
}

bool
CQChartsPlot::
hasForeground() const
{
  if (colorMapKey_ && isColorMapKey())
    return true;

  return false;
}

void
CQChartsPlot::
drawCustomForeground(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsPlot::drawCustomForeground");

  if (isOverlay()) {
    Plots oplots;

    overlayPlots(oplots);

    for (const auto &oplot : oplots) {
      if (oplot->hasForeground())
        oplot->execDrawForeground(device);
    }
  }
  else {
    if (this->hasForeground())
      execDrawForeground(device);
  }
}

void
CQChartsPlot::
execDrawForeground(PaintDevice *device) const
{
  if (colorMapKey_ && isColorMapKey())
    drawColorMapKey(device);
}

//---

bool
CQChartsPlot::
hasGroupedBoxes() const
{
  // for overlay draw all combine objects on common layers
  if (isOverlay()) {
    if (! isFirstPlot())
      return false;

    bool anyObjs = processOverlayPlots([&](const Plot *plot) {
      return plot->hasBoxes();
    }, false);

    if (! anyObjs)
      return false;
  }
  else {
    if (! hasBoxes())
      return false;
  }

  //---

  if (! isLayerActive(Layer::Type::BOXES))
    return false;

  return true;
}

void
CQChartsPlot::
drawGroupedBoxes(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsPlot::drawGroupedBoxes");

  // for overlay draw all combine objects on common layers
  if (isOverlay()) {
    if (! isFirstPlot())
      return;

    processOverlayPlots([&](const Plot *plot) {
      device->setPlot(const_cast<Plot *>(plot));

      plot->drawBoxes(device);
    });

    device->setPlot(const_cast<Plot *>(this));
  }
  else {
    drawBoxes(device);
  }
}

bool
CQChartsPlot::
hasBoxes() const
{
  if (! isShowBoxes())
    return false;

  //---

  if (! isLayerActive(Layer::Type::BOXES))
    return false;

  return true;
}

void
CQChartsPlot::
drawBoxes(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsPlot::drawBoxes");

  auto bbox = fitBBox();

  drawWindowColorBox(device, bbox);

  drawWindowColorBox(device, dataFitBBox       ());
  drawWindowColorBox(device, axesFitBBox       ());
  drawWindowColorBox(device, keyFitBBox        ());
  drawWindowColorBox(device, titleFitBBox      ());
  drawWindowColorBox(device, annotationsFitBBox());
  drawWindowColorBox(device, extraFitBBox      ());

  //---

  assert(! isComposite());

  drawWindowColorBox(device, CQChartsUtil::rangeBBox(getCalcDataRange()), Qt::green);
  drawWindowColorBox(device, CQChartsUtil::rangeBBox(dataRange()       ), Qt::green);
  drawWindowColorBox(device, CQChartsUtil::rangeBBox(outerDataRange_   ), Qt::green);
}

//---

bool
CQChartsPlot::
hasGroupedEditHandles() const
{
  // for overlay draw all combine objects on common layers
  if (isOverlay()) {
    if (! isFirstPlot())
      return false;

    bool anyObjs = processOverlayPlots([&](const Plot *plot) {
      return plot->hasEditHandles();
    }, false);

    if (! anyObjs)
      return false;
  }
  else {
    if (! hasEditHandles())
      return false;
  }

  //---

  if (! isLayerActive(Layer::Type::EDIT_HANDLE))
    return false;

  return true;
}

void
CQChartsPlot::
drawGroupedEditHandles(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsPlot::drawGroupedEditHandles");

  // for overlay draw all combine objects on common layers
  if (isOverlay()) {
    if (! isFirstPlot())
      return;

    processOverlayPlots([&](const Plot *plot) {
      plot->drawEditHandles(device);
    });
  }
  else {
    drawEditHandles(device);
  }
}

bool
CQChartsPlot::
hasEditHandles() const
{
  if (view()->mode() != View::Mode::EDIT)
    return false;

  //---

  bool selected = (isSelected() ||
                   (title() && title()->isSelected()) ||
                   (xAxis() && xAxis()->isSelected()) ||
                   (yAxis() && yAxis()->isSelected()));

  if (! selected) {
    auto *key1 = getFirstPlotKey();

    selected = (key1 && key1->isSelected());
  }

  if (! selected) {
    for (const auto *mapKey : mapKeys_) {
      if (! mapKey->isSelected())
        continue;

      selected = true;

      break;
    }
  }

  if (! selected) {
    for (const auto &annotation : annotations()) {
      if (! annotation->isVisible() || ! annotation->isSelected())
        continue;

      selected = true;

      break;
    }
  }

  if (! selected) {
    for (auto &plotObj : plotObjects()) {
      if (! plotObj->isEditable())
        continue;

      if (! plotObj->isSelected())
        continue;

      selected = true;

      break;
    }
  }

  if (! selected)
    return false;

  //---

  if (! isLayerActive(Layer::Type::EDIT_HANDLE))
    return false;

  return true;
}

void
CQChartsPlot::
drawEditHandles(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsPlot::drawEditHandles");

  if      (isEditable() && isSelected()) {
    const_cast<Plot *>(this)->editHandles()->setBBox(this->calcViewBBox());

    editHandles()->draw(device);
  }

  if (title() && title()->isEditable() && title()->isSelected())
    title()->drawEditHandles(device);

  auto *key1 = getFirstPlotKey();

  if (key1 && key1->isEditable() && key1->isSelected())
    key1->drawEditHandles(device);

  for (const auto *mapKey : mapKeys_) {
    if (mapKey->isVisible() && mapKey->isEditable() && mapKey->isSelected())
      mapKey->drawEditHandles(device);
  }

  if (xAxis() && xAxis()->isEditable() && xAxis()->isSelected())
    xAxis()->drawEditHandles(device);

  if (yAxis() && yAxis()->isEditable() && yAxis()->isSelected())
    yAxis()->drawEditHandles(device);

  for (const auto &annotation : annotations()) {
    if (annotation->isVisible() && annotation->isEditable() && annotation->isSelected())
      annotation->drawEditHandles(device);
  }

  for (auto &plotObj : plotObjects()) {
    if (plotObj->isEditable() && plotObj->isSelected())
      plotObj->drawEditHandles(device);
  }
}

//---

bool
CQChartsPlot::
hasOverlayI() const
{
  if (isOverlay()) {
    Plots oplots;

    overlayPlots(oplots);

    for (const auto &oplot : oplots)
      if (oplot->hasOverlay())
        return true;
  }
  else {
    if (hasOverlay())
      return true;
  }

  return false;
}

bool
CQChartsPlot::
hasOverlay() const
{
  return false;
}

void
CQChartsPlot::
drawCustomOverlay(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsPlot::drawCustomOverlay");

  if (isOverlay()) {
    Plots oplots;

    overlayPlots(oplots);

    for (const auto &oplot : oplots) {
      if (oplot->hasOverlay())
        oplot->execDrawOverlay(device);
    }
  }
  else {
    if (this->hasOverlay())
      execDrawOverlay(device);
  }
}

void
CQChartsPlot::
execDrawOverlay(PaintDevice *) const
{
}

//---

const CQChartsLayer::Type &
CQChartsPlot::
drawLayerType() const
{
  return view()->drawLayerType();
}

//---

void
CQChartsPlot::
waitTree()
{
  if      (isOverlay()) {
    Plots oplots;

    overlayPlots(oplots);

    for (const auto &oplot : oplots)
      oplot->execWaitTree();
  }
  else if (parentPlot()) {
    parentPlot()->waitTree();
  }
  else
    execWaitTree();
}

void
CQChartsPlot::
execWaitTree()
{
  assert(! isComposite());

  assert(useObjTree());

  if (! isPlotObjTreeSet()) {
    objTreeData_.tree->waitTree();
  }
}

//---

CQChartsGeom::BBox
CQChartsPlot::
equalDisplayRangeBBox() const
{
  if (! isEqualScale())
    return displayRangeBBox();

  auto bbox = unequalDataRange_.bbox();

  if (! bbox.isSet())
    bbox = displayRangeBBox();

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
displayRangeBBox() const
{
  // calc current (zoomed/panned) data range
  double xmin, ymin, xmax, ymax;

  displayRange().getWindowRange(&xmin, &ymin, &xmax, &ymax);

  BBox bbox(xmin, ymin, xmax, ymax);

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
rawDisplayRangeBBox() const
{
  // calc current (zoomed/panned) data range
  double xmin, ymin, xmax, ymax;

  rawDisplayRange().getWindowRange(&xmin, &ymin, &xmax, &ymax);

  BBox bbox(xmin, ymin, xmax, ymax);

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
calcDataPixelRect() const
{
  // calc current (zoomed/panned) pixel range
  auto bbox = displayRangeBBox();

  auto pbbox = windowToPixel(bbox);

  return pbbox;
}

CQChartsGeom::BBox
CQChartsPlot::
calcPlotRect() const
{
  return pixelToWindow(calcPlotPixelRect());
}

CQChartsGeom::BBox
CQChartsPlot::
calcPlotPixelRect() const
{
  return view()->windowToPixel(calcViewBBox());
}

CQChartsGeom::BBox
CQChartsPlot::
calcPlotViewRect() const
{
  return pixelToWindow(calcPlotPixelRect());
}

CQChartsGeom::Size
CQChartsPlot::
calcPixelSize() const
{
  auto bbox = calcPlotPixelRect();

  return Size(bbox.getWidth(), bbox.getHeight());
}

void
CQChartsPlot::
calcTabData(const Plots &plots) const
{
  auto *th = const_cast<Plot *>(this);

  QFontMetricsF fm(tabbedFont().font());

  th->tabData_.pxm = fm.horizontalAdvance("X")/2.0;
  th->tabData_.pym = fm.height()/4.0;

  th->tabData_.ptw = 0.0;

  for (auto &plot : plots) {
    if (tabData_.drawType == TabData::DrawType::TITLE) {
      auto title = plot->titleStr();

      if (! title.length())
        title = plot->calcName();

      th->tabData_.ptw += fm.horizontalAdvance(title) + 2*tabData_.pxm;
    }
    else {
      th->tabData_.ptw += 4*tabData_.pxm;
    }
  }

  th->tabData_.pth = fm.height() + 2*tabData_.pym;
}

CQChartsGeom::BBox
CQChartsPlot::
calcTabPixelRect() const
{
  auto pixelRect = calcPlotPixelRect();

  int px = int(pixelRect.getXMid() - tabData_.ptw/2);
  int py = int(pixelRect.getYMax() - tabData_.pth);

  return BBox(px, py, px + tabData_.ptw, py + tabData_.pth);
}

//---

void
CQChartsPlot::
updateAutoFit()
{
  // auto fit based on last draw
  if (needsAutoFit()) {
    if (calcNextState() != UpdateState::INVALID)
      return;

    setNeedsAutoFit(false);

    autoFit();
  }
}

void
CQChartsPlot::
autoFit()
{
  if (isOverlay() && ! isFirstPlot())
    return;

  CQPerfTrace trace("CQChartsPlot::autoFit");

  if (! isZoomFull()) {
    setNeedsAutoFit(true);

    zoomFull(/*notify*/false);

    return;
  }

  //---

  if (isOverlay()) {
    if (prevPlot())
      return;

    //---

    // combine bboxes of overlay plots
    BBox bbox;

    processOverlayPlots([&](const Plot *plot) {
      auto bbox1 = plot->fitBBox();
      auto bbox2 = plot->windowToPixel(bbox1);
      auto bbox3 = pixelToWindow(bbox2);

      bbox += bbox3;
    });

    //---

    // set all overlay plot bboxes
    using BBoxes = std::vector<BBox>;

    BBoxes bboxes;

    processOverlayPlots([&](const Plot *plot) {
      auto bbox1 = windowToPixel(bbox);
      auto bbox2 = plot->pixelToWindow(bbox1);

      bboxes.push_back(std::move(bbox2));
    });

    int i = 0;

    processOverlayPlots([&](Plot *plot) {
      plot->setFitBBox(bboxes[size_t(i)]);

      ++i;
    });
  }
  else {
    autoFitOne();
  }
}

void
CQChartsPlot::
autoFitOne()
{
  inAutoFit_ = true;

  if (title())
    title()->preAutoFit();

#if 0
  for (int i = 0; i < 5; ++i) {
    auto bbox = fitBBox();

    setFitBBox(bbox);

    updateRangeAndObjs();

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
  }
#else
  auto outerMargin = PlotMargin(Length::plot(0), Length::plot(0), Length::plot(0), Length::plot(0));

  setOuterMargin(outerMargin);

  //---

  auto bbox = fitBBox();

  setFitBBox(bbox);

  autoFitUpdate();
#endif

  if (title())
    title()->postAutoFit();

  Q_EMIT zoomPanChanged();

  inAutoFit_ = false;
}

void
CQChartsPlot::
autoFitUpdate()
{
  saveSelection();

  updateRangeAndObjs();
}

void
CQChartsPlot::
setFitBBox(const BBox &bbox)
{
  auto bbox1 = fitMargin().adjustPlotRange(this, bbox, /*inside*/true);

  // calc margin so plot box fits in specified box
  auto pbbox = equalDisplayRangeBBox();

  double left   = 100.0*(pbbox.getXMin() - bbox1.getXMin())/bbox1.getWidth ();
  double bottom = 100.0*(pbbox.getYMin() - bbox1.getYMin())/bbox1.getHeight();
  double right  = 100.0*(bbox1.getXMax() - pbbox.getXMax())/bbox1.getWidth ();
  double top    = 100.0*(bbox1.getYMax() - pbbox.getYMax())/bbox1.getHeight();

  if (isInvertX()) std::swap(left, right );
  if (isInvertY()) std::swap(top , bottom);

  if (isEqualScale()) {
#if 0
    auto size = std::max(std::max(std::max(left, bottom), right), top);

    left = size; bottom = size; right = size; top = size;
#else
    auto xmargin = left + right;
    auto ymargin = top + bottom;

    auto size = std::max(xmargin, ymargin);

    auto xf = (size > 0 ? xmargin/size : 1.0);
    auto yf = (size > 0 ? ymargin/size : 1.0);

    if (xf > 1E-6) { left   /= xf; right /= xf; }
    if (yf > 1E-6) { bottom /= yf; top   /= yf; }
#endif
  }

  auto outerMargin = PlotMargin(Length::percent(left ), Length::percent(top   ),
                                Length::percent(right), Length::percent(bottom));

  setOuterMargin(outerMargin);
}

CQChartsGeom::BBox
CQChartsPlot::
fitBBox() const
{
  // calc fit box
  BBox bbox;

  bbox += dataFitBBox();

  bbox += axesFitBBox ();
  bbox += keyFitBBox  ();
  bbox += titleFitBBox();

  bbox += annotationsFitBBox();
  bbox += extraFitBBox      ();

  if (colorMapKey_ && colorMapKey_->isVisible())
    bbox += colorMapKey_->bbox();

  bbox.makeNonZero();

  //---

  // add margin (TODO: config pixel margin size)
  auto marginSize = pixelToWindowSize(Size(pixelMarginSize(), pixelMarginSize()));

  bbox.expand(-marginSize.width(), -marginSize.height(),
               marginSize.width(),  marginSize.height());

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
dataFitBBox() const
{
//auto bbox = displayRangeBBox();
  auto bbox = equalDisplayRangeBBox();

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
axesFitBBox() const
{
  BBox bbox;

  if (isXAxisVisible())
    bbox += xAxis()->fitBBox();

  if (isYAxisVisible())
    bbox += yAxis()->fitBBox();

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
keyFitBBox() const
{
  BBox bbox;

  if (isKeyVisibleAndNonEmpty() && key()->hasBBox()) {
    auto bbox1 = key()->bbox();

    if (bbox1.isSet()) {
      bbox.add(bbox1.getCenter());

      if (! key()->isPixelWidthExceeded())
        bbox.addX(bbox1);
      if (! key()->isPixelHeightExceeded())
        bbox.addY(bbox1);
    }
  }

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
titleFitBBox() const
{
  BBox bbox;

  if (title() && title()->isDrawn())
    bbox += title()->fitBBox();

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
annotationsFitBBox() const
{
  BBox bbox;

  for (auto &annotation : annotations()) {
    if (! annotation->isFitted())
      continue;

    if (! annotation->isVisible())
      continue;

    bbox += annotation->bbox();
  }

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
calcFitPixelRect() const
{
  // calc current (zoomed/panned) pixel range
  auto bbox = fitBBox();

  auto pbbox = windowToPixel(bbox);

  return pbbox;
}

//------

CQChartsAnnotation *
CQChartsPlot::
addAnnotation(CQChartsAnnotationType annotationType)
{
  auto type = static_cast<CQChartsAnnotation::Type>(annotationType);

  switch (type) {
    case Annotation::Type::GROUP:
      return addAnnotationT<AnnotationGroup>(this);
    case Annotation::Type::RECT:
      return addAnnotationT<CQChartsRectangleAnnotation>(this);
    case Annotation::Type::ELLIPSE:
      return addAnnotationT<CQChartsEllipseAnnotation>(this);
    case Annotation::Type::POLYGON:
      return addAnnotationT<CQChartsPolygonAnnotation>(this);
    case Annotation::Type::POLYLINE:
      return addAnnotationT<CQChartsPolylineAnnotation>(this);
    case Annotation::Type::TEXT:
      return addAnnotationT<CQChartsTextAnnotation>(this);
    case Annotation::Type::IMAGE:
      return addAnnotationT<CQChartsImageAnnotation>(this);
    case Annotation::Type::PATH:
      return addAnnotationT<CQChartsPathAnnotation>(this);
    case Annotation::Type::ARROW:
      return addAnnotationT<CQChartsArrowAnnotation>(this);
    case Annotation::Type::ARC:
      return addAnnotationT<CQChartsArcAnnotation>(this);
    case Annotation::Type::ARC_CONNECTOR:
      return addAnnotationT<CQChartsArcConnectorAnnotation>(this);
    case Annotation::Type::POINT:
      return addAnnotationT<CQChartsPointAnnotation>(this);
    case Annotation::Type::PIE_SLICE:
      return addAnnotationT<CQChartsPieSliceAnnotation>(this);
    case Annotation::Type::AXIS:
      return addAnnotationT<CQChartsAxisAnnotation>(this);
    case Annotation::Type::KEY:
      return addAnnotationT<CQChartsKeyAnnotation>(this);
    case Annotation::Type::POINT3D_SET:
      return addAnnotationT<CQChartsPoint3DSetAnnotation>(this);
    case Annotation::Type::POINT_SET:
      return addAnnotationT<CQChartsPointSetAnnotation>(this);
    case Annotation::Type::VALUE_SET:
      return addAnnotationT<CQChartsValueSetAnnotation>(this);
    case Annotation::Type::BUTTON:
      return addAnnotationT<CQChartsButtonAnnotation>(this);
    case Annotation::Type::WIDGET:
      return addAnnotationT<CQChartsWidgetAnnotation>(this);
#ifdef CQCHARTS_TK_WIDGET
    case Annotation::Type::TK_WIDGET:
      return addAnnotationT<CQChartsTkWidgetAnnotation>(this);
#endif
    case Annotation::Type::SYMBOL_MAP_KEY:
      return addAnnotationT<CQChartsSymbolSizeMapKeyAnnotation>(this);
    default:
      assert(false);
  }

  return nullptr;
}

CQChartsAnnotationGroup *
CQChartsPlot::
addAnnotationGroup()
{
  return addAnnotationT<AnnotationGroup>(new AnnotationGroup(this));
}

CQChartsArrowAnnotation *
CQChartsPlot::
addArrowAnnotation(const CQChartsObjRefPos &start, const CQChartsObjRefPos &end)
{
  return addAnnotationT<ArrowAnnotation>(new ArrowAnnotation(this, start, end));
}

CQChartsArcAnnotation *
CQChartsPlot::
addArcAnnotation(const ObjRefPos &start, const ObjRefPos &end)
{
  return addAnnotationT<ArcAnnotation>(new ArcAnnotation(this, start, end));
}

CQChartsArcConnectorAnnotation *
CQChartsPlot::
addArcConnectorAnnotation(const ObjRefPos &center, const Length &radius,
                          const Angle &srcStartAngle, const Angle &srcSpanAngle,
                          const Angle &destStartAngle, const Angle &destSpanAngle, bool self)
{
  return addAnnotationT<ArcConnectorAnnotation>(
    new ArcConnectorAnnotation(this, center, radius, srcStartAngle, srcSpanAngle,
                               destStartAngle, destSpanAngle, self));
}

CQChartsAxisAnnotation *
CQChartsPlot::
addAxisAnnotation(Qt::Orientation direction, double start, double end)
{
  return addAnnotationT<AxisAnnotation>(new AxisAnnotation(this, direction, start, end));
}

CQChartsEllipseAnnotation *
CQChartsPlot::
addEllipseAnnotation(const ObjRefPos &center, const Length &xRadius, const Length &yRadius)
{
  return addAnnotationT<EllipseAnnotation>(new EllipseAnnotation(this, center, xRadius, yRadius));
}

CQChartsImageAnnotation *
CQChartsPlot::
addImageAnnotation(const ObjRefPos &pos, const Image &image)
{
  return addAnnotationT<ImageAnnotation>(new ImageAnnotation(this, pos, image));
}

CQChartsImageAnnotation *
CQChartsPlot::
addImageAnnotation(const Rect &rect, const Image &image)
{
  return addAnnotationT<ImageAnnotation>(new ImageAnnotation(this, rect, image));
}

CQChartsPathAnnotation *
CQChartsPlot::
addPathAnnotation(const Path &path)
{
  return addAnnotationT<PathAnnotation>(new PathAnnotation(this, path));
}

CQChartsKeyAnnotation *
CQChartsPlot::
addKeyAnnotation(const Column &column)
{
  return addAnnotationT<KeyAnnotation>(new KeyAnnotation(this, column));
}

CQChartsPieSliceAnnotation *
CQChartsPlot::
addPieSliceAnnotation(const ObjRefPos &pos, const Length &innerRadius, const Length &outerRadius,
                      const Angle &startAngle, const Angle &spanAngle)
{
  return addAnnotationT<PieSliceAnnotation>(
    new PieSliceAnnotation(this, pos, innerRadius, outerRadius, startAngle, spanAngle));
}

CQChartsPointAnnotation *
CQChartsPlot::
addPointAnnotation(const ObjRefPos &pos, const Symbol &type)
{
  return addAnnotationT<PointAnnotation>(new PointAnnotation(this, pos, type));
}

CQChartsPointSetAnnotation *
CQChartsPlot::
addPointSetAnnotation(const Rect &rect, const CQChartsPoints &values)
{
  return addAnnotationT<PointSetAnnotation>(new PointSetAnnotation(this, rect, values));
}

CQChartsPoint3DSetAnnotation *
CQChartsPlot::
addPoint3DSetAnnotation(const Point3DArray &points)
{
  return addAnnotationT<Point3DSetAnnotation>(new Point3DSetAnnotation(this, points));
}

CQChartsPolygonAnnotation *
CQChartsPlot::
addPolygonAnnotation(const CQChartsPolygon &points)
{
  return addAnnotationT<PolygonAnnotation>(new PolygonAnnotation(this, points));
}

CQChartsPolylineAnnotation *
CQChartsPlot::
addPolylineAnnotation(const CQChartsPolygon &points)
{
  return addAnnotationT<PolylineAnnotation>(new PolylineAnnotation(this, points));
}

CQChartsRectangleAnnotation *
CQChartsPlot::
addRectangleAnnotation(const Rect &rect)
{
  return addAnnotationT<RectangleAnnotation>(new RectangleAnnotation(this, rect));
}

CQChartsShapeAnnotation *
CQChartsPlot::
addShapeAnnotation(const Rect &rect)
{
  return addAnnotationT<ShapeAnnotation>(new ShapeAnnotation(this, rect));
}

CQChartsTextAnnotation *
CQChartsPlot::
addTextAnnotation(const ObjRefPos &pos, const QString &text)
{
  return addAnnotationT<TextAnnotation>(new TextAnnotation(this, pos, text));
}

CQChartsTextAnnotation *
CQChartsPlot::
addTextAnnotation(const Rect &rect, const QString &text)
{
  return addAnnotationT<TextAnnotation>(new TextAnnotation(this, rect, text));
}

CQChartsValueSetAnnotation *
CQChartsPlot::
addValueSetAnnotation(const Rect &rectangle, const CQChartsReals &values)
{
  return addAnnotationT<ValueSetAnnotation>(new ValueSetAnnotation(this, rectangle, values));
}

CQChartsButtonAnnotation *
CQChartsPlot::
addButtonAnnotation(const ObjRefPos &pos, const QString &text)
{
  return addAnnotationT<ButtonAnnotation>(new ButtonAnnotation(this, pos, text));
}

CQChartsWidgetAnnotation *
CQChartsPlot::
addWidgetAnnotation(const ObjRefPos &pos, const Widget &widget)
{
  initWidgetAnnotation(widget);

  return addAnnotationT<WidgetAnnotation>(new WidgetAnnotation(this, pos, widget));
}

CQChartsWidgetAnnotation *
CQChartsPlot::
addWidgetAnnotation(const Rect &rect, const Widget &widget)
{
  initWidgetAnnotation(widget);

  return addAnnotationT<WidgetAnnotation>(new WidgetAnnotation(this, rect, widget));
}

#ifdef CQCHARTS_TK_WIDGET
CQChartsTkWidgetAnnotation *
CQChartsPlot::
addTkWidgetAnnotation(const ObjRefPos &pos, const QString &id)
{
  return addAnnotationT<TkWidgetAnnotation>(new TkWidgetAnnotation(this, pos, id));
}

CQChartsTkWidgetAnnotation *
CQChartsPlot::
addTkWidgetAnnotation(const Rect &rect, const QString &id)
{
  return addAnnotationT<TkWidgetAnnotation>(new TkWidgetAnnotation(this, rect, id));
}
#endif

void
CQChartsPlot::
initWidgetAnnotation(const Widget &widget)
{
  auto *control = dynamic_cast<CQChartsPlotControlIFace *>(widget.widget());

  if (control) {
    control->setPlot(this);

    control->connectValueChanged(this, SLOT(plotControlUpdateSlot()));

    controls_.push_back(control);
  }

  //---

  using Widgets = std::vector<QWidget *>;

  Widgets widgets;

  widgets.push_back(widget.widget());

  auto widgets1 = widget.widget()->findChildren<QWidget *>();

  for (auto *widget1 : widgets1) {
    auto *widgetIFace1 = (widget1 ? dynamic_cast<CQChartsWidgetIFace *>(widget1) : nullptr);

    if (widgetIFace1)
      widgets.push_back(widget1);
  }

  //---

  const auto &model = currentModel();

  auto *modelData = getModelData(model);
  if (! modelData) return;

  for (auto *widget : widgets) {
    auto widgetIFace = dynamic_cast<CQChartsWidgetIFace *>(widget);

    if (widgetIFace) {
      // set in increasing dependency
      widgetIFace->setCharts   (charts());
      widgetIFace->setView     (view());
      widgetIFace->setModelData(modelData);
      widgetIFace->setPlot     (this);
    }

    //---

    auto *modelHolder = dynamic_cast<CQChartsModelViewHolder *>(widget);

    if (modelHolder) {
      const auto &model = this->currentModel();

      modelHolder->setModel(model, isHierarchical());
    }

#if 0
    auto *modelDetailsTable = dynamic_cast<CQChartsModelDetailsTable *>(widget);

    if (modelDetailsTable)
      modelDetailsTable->setModelData(modelData);

    auto *modelDetailsWidget = dynamic_cast<CQChartsModelDetailsWidget *>(widget);

    if (modelDetailsWidget)
      modelDetailsWidget->setModelData(modelData);
#endif

    //---

#if 0
    auto *modelColumnDataControl = dynamic_cast<CQChartsModelColumnDataControl *>(widget);

    if (modelColumnDataControl)
      modelColumnDataControl->setModelData(modelData);

    auto *modelExprControl = dynamic_cast<CQChartsModelExprControl *>(widget);

    if (modelExprControl)
      modelExprControl->setModelData(modelData);

    auto *modelControl = dynamic_cast<CQChartsModelControl *>(widget);

    if (modelControl)
      modelControl->setModelData(modelData);
#endif
  }
}

CQChartsSymbolSizeMapKeyAnnotation *
CQChartsPlot::
addSymbolMapKeyAnnotation()
{
  return addAnnotationT<SymbolMapKeyAnnotation>(new SymbolMapKeyAnnotation(this));
}

//------

CQChartsPlot::Annotation *
CQChartsPlot::
addAnnotationI(Annotation *annotation)
{
  annotations_.push_back(annotation);

  connect(annotation, SIGNAL(idChanged()), this, SLOT(updateAnnotationSlot()));

  annotation->connectDataChanged(this, SLOT(updateAnnotationSlot()));

  annotation->addProperties(propertyModel(), "annotations");

  drawObjs();

//Q_EMIT annotationAdded(annotation->id());
  Q_EMIT annotationsChanged();

  return annotation;
}

CQChartsAnnotation *
CQChartsPlot::
getAnnotationById(const QString &id) const
{
  for (auto &annotation : annotations()) {
    if (annotation->id() == id)
      return annotation;
  }

  return nullptr;
}

CQChartsAnnotation *
CQChartsPlot::
getAnnotationByPathId(const QString &pathId) const
{
  for (auto &annotation : annotations()) {
    if (annotation->pathId() == pathId)
      return annotation;
  }

  return nullptr;
}

CQChartsAnnotation *
CQChartsPlot::
getAnnotationByInd(int ind) const
{
  for (auto &annotation : annotations()) {
    if (annotation->ind() == ind)
      return annotation;
  }

  return nullptr;
}

void
CQChartsPlot::
raiseAnnotation(Annotation *annotation)
{
  int pos = annotationPos(annotation);
  if (pos < 0) return; // not found

  int np = int(annotations().size());
  if (np < 2) return;

  if (pos < np - 1)
    std::swap(annotations_[size_t(pos + 1)], annotations_[size_t(pos)]);

  drawObjs();

  Q_EMIT annotationsReordered();
}

void
CQChartsPlot::
lowerAnnotation(Annotation *annotation)
{
  int pos = annotationPos(annotation);
  if (pos < 0) return; // not found

  int np = int(annotations().size());
  if (np < 2) return;

  if (pos > 0)
    std::swap(annotations_[size_t(pos - 1)], annotations_[size_t(pos)]);

  drawObjs();

  Q_EMIT annotationsReordered();
}

int
CQChartsPlot::
annotationPos(Annotation *annotation) const
{
  auto np = annotations().size();

  for (size_t i = 0; i < np; ++i) {
    if (annotations_[i] == annotation)
      return int(i);
  }

  return -1;
}

void
CQChartsPlot::
removeAnnotation(Annotation *annotation)
{
  int pos = 0;

  for (auto &annotation1 : annotations_) {
    if (annotation1 == annotation)
      break;

    ++pos;
  }

  int n = int(annotations_.size());

  assert(pos >= 0 && pos < n);

  propertyModel()->removeProperties("annotations/" + annotation->propertyId());

  delete annotation;

  for (int i = pos + 1; i < n; ++i)
    annotations_[size_t(i - 1)] = annotations_[size_t(i)];

  annotations_.pop_back();

  drawObjs();

  Q_EMIT annotationsChanged();
}

void
CQChartsPlot::
removeAllAnnotations()
{
  for (auto &annotation : annotations_)
    delete annotation;

  annotations_.clear();

  propertyModel()->removeProperties("annotations");

  drawObjs();

  Q_EMIT annotationsChanged();
}

void
CQChartsPlot::
updateAnnotationSlot()
{
  if (editing_) {
    invalidateLayer(Buffer::Type::FOREGROUND);

    invalidateOverlay();
  }
  else
    drawObjs();

  Q_EMIT annotationsChanged();
}

//------

void
CQChartsPlot::
plotControlUpdateSlot()
{
  auto cmpStr = QString("==");

  QStringList filters;

  for (const auto &control : controls_) {
    auto filter = control->filterStr(cmpStr);

    if (filter.length())
      filters.push_back(filter);
  }

  auto combStr = QString("&&");

  auto filterStr = filters.join(QString(" %1 ").arg(combStr));

  setVisibleFilterStr(filterStr);
}

//------

void
CQChartsPlot::
setCustomWidget(const Widget &v)
{
  customWidget_ = v;

  Q_EMIT customWidgetChanged();
}

//------

CQChartsPlotObj *
CQChartsPlot::
getPlotObject(const QString &objectId) const
{
  for (auto &plotObj : plotObjects()) {
    if (plotObj->id() == objectId)
      return plotObj;
  }

  return nullptr;
}

CQChartsObj *
CQChartsPlot::
getObject(const QString &objectId) const
{
  auto *plotObj = getPlotObject(objectId);
  if (plotObj) return plotObj;

  auto checkObjs = [&](std::initializer_list<Obj *> objs) {
    for (const auto &obj : objs)
      if (obj && obj->id() == objectId)
        return obj;

    return static_cast<Obj *>(nullptr);
  };

  return checkObjs({xAxis(), yAxis(), key(), title()});
}

QList<QModelIndex>
CQChartsPlot::
getObjectInds(const QString &objectId) const
{
  QList<QModelIndex> inds;

  auto *plotObj = getPlotObject(objectId);

  if (plotObj) {
    PlotObj::Indices inds1;

    plotObj->getNormalizedSelectIndices(inds1); // normalized

    for (auto &ind1 : inds1)
      inds.push_back(ind1);
  }

  return inds;
}

std::vector<CQChartsObj *>
CQChartsPlot::
getObjectConnected(const QString &objectId) const
{
  std::vector<Obj *> objs;

  auto *plotObj = getPlotObject(objectId);

  if (plotObj) {
    auto objs1 = plotObj->getConnected();

    for (auto &obj1 : objs1)
      objs.push_back(obj1);
  }

  return objs;
}

//------

CQChartsLayer *
CQChartsPlot::
initLayer(const Layer::Type &type, const Buffer::Type &buffer, bool active)
{
  auto pb = buffers_.find(buffer);

  if (pb == buffers_.end()) {
    auto *layerBuffer = new Buffer(view(), buffer);

    pb = buffers_.emplace_hint(pb, buffer, layerBuffer);
  }

  //auto *layerBuffer = (*pb).second;

  //---

  auto pl = layers_.find(type);

  if (pl == layers_.end()) {
    auto *layer = new Layer(type, buffer);

    pl = layers_.emplace_hint(pl, type, layer);
  }

  auto *layer = (*pl).second;

  layer->setActive(active);

  return layer;
}

void
CQChartsPlot::
setLayerActive(const Layer::Type &type, bool b)
{
  if (isOverlay()) {
    processOverlayPlots([&](Plot *plot) {
      plot->setLayerActive1(type, b);
    });
  }
  else {
    // composite ?
    setLayerActive1(type, b);
  }
}

void
CQChartsPlot::
setLayerActive1(const Layer::Type &type, bool b)
{
  assert(! parentPlot());

  auto *layer = getLayer(type);
  if (! layer) return;

  layer->setActive(b);

  setLayersChanged(true);
}

bool
CQChartsPlot::
isLayerActive(const Layer::Type &type) const
{
  if (isOverlay()) {
    bool anyActive = processOverlayPlots([&](const Plot *plot) {
      return plot->isLayerActive1(type);
    }, false);

    return anyActive;
  }

  if (parentPlot())
    return parentPlot()->isLayerActive(type);

  return isLayerActive1(type);
}

bool
CQChartsPlot::
isLayerActive1(const Layer::Type &type) const
{
  assert(! parentPlot());

  auto *layer = getLayer(type);
  if (! layer) return false;

  return layer->isActive();
}

bool
CQChartsPlot::
isInvalidateLayers() const
{
  assert(! parentPlot());

  return updatesData_.invalidateLayers;
}

void
CQChartsPlot::
invalidateLayers()
{
  if (parentPlot())
    return parentPlot()->invalidateLayers();

  //---

  execInvalidateLayers();
}

void
CQChartsPlot::
execInvalidateLayers()
{
  if (! isUpdatesEnabled()) {
    assert(! parentPlot());

    std::unique_lock<std::mutex> lock(updatesMutex_);

    updatesData_.invalidateLayers = true;

    return;
  }

  //---

  assert(! parentPlot());

  if (isOverlay()) {
    processOverlayPlots([&](Plot *plot) {
      for (auto &buffer : plot->buffers_)
        buffer.second->setValid(false);
    });
  }
  else {
    for (auto &buffer : buffers_)
      buffer.second->setValid(false);
  }

  setLayersChanged(true);

  fromInvalidate_ = true;

  update();
}

void
CQChartsPlot::
invalidateLayer(const Buffer::Type &type)
{
  if (parentPlot())
    return parentPlot()->invalidateLayer(type);

  //---

  execInvalidateLayer(type);
}

void
CQChartsPlot::
execInvalidateLayer(const Buffer::Type &type)
{
  if (! isUpdatesEnabled()) {
    assert(! parentPlot());

    std::unique_lock<std::mutex> lock(updatesMutex_);

    updatesData_.invalidateLayers = true;

    return;
  }

  //assert(type != Buffer::Type::MIDDLE);

  if (isOverlay()) {
    processOverlayPlots([&](Plot *plot) {
      plot->invalidateLayer1(type);
    });
  }
  else {
    invalidateLayer1(type);
  }
}

void
CQChartsPlot::
invalidateLayer1(const Buffer::Type &type)
{
  assert(! parentPlot());

//std::cerr << "invalidateLayer1: " << Buffer::typeName(type) << "\n";
  auto *layer = getBuffer(type);

  layer->setValid(false);

  setLayersChanged(false);

  fromInvalidate_ = true;

  update();
}

void
CQChartsPlot::
setLayersChanged(bool update)
{
  assert(! parentPlot());

  if (updateData_.rangeThread->isBusy())
    return;

  //---

  {
    TryLockMutex lock(this, "setLayersChanged");

    if (! lock.locked)
      return;

    interruptDraw();
  }

  if (update) {
    if (isUpdatesEnabled())
      updateDraw();
  }
  else {
  //drawNonMiddleParts(view()->ipainter());

    drawParts(view()->ipainter());

    fromInvalidate_ = true;

    this->update();
  }

  Q_EMIT layersChanged();
}

CQChartsBuffer *
CQChartsPlot::
getBuffer(const Buffer::Type &type) const
{
  auto p = buffers_.find(type);
  assert(p != buffers_.end());

  return (*p).second;
}

CQChartsLayer *
CQChartsPlot::
getLayer(const Layer::Type &type) const
{
  auto p = layers_.find(type);
  if (p == layers_.end()) return nullptr;

  return (*p).second;
}

//---

void
CQChartsPlot::
setClipRect(PaintDevice *device) const
{
  auto *plot1 = firstPlot();

  if      (plot1->isDataClip()) {
    auto bbox  = (isDataRawClip() ? rawDisplayRangeBBox() : displayRangeBBox());
    auto abbox = annotationsFitBBox();
    auto ebbox = extraFitBBox();

    if      (dataScaleX() <= 1.0 && dataScaleY() <= 1.0) {
      bbox.add(abbox);
      bbox.add(ebbox);
    }
    else if (dataScaleX() <= 1.0) {
      bbox.addX(abbox);
      bbox.addX(ebbox);
    }
    else if (dataScaleY() <= 1.0) {
      bbox.addY(abbox);
      bbox.addY(ebbox);
    }

    clipRect_ = bbox;
  }
  else if (plot1->isFitClip()) {
    auto fitRect = fitBBox();

    clipRect_ = fitRect;
  }
  else if (plot1->isPlotClip()) {
    auto plotRect = calcPlotRect();

    clipRect_ = plotRect;
  }
  else {
    clipRect_ = BBox();
  }

  device->setClipRect(clipRect_);
}

QPainter *
CQChartsPlot::
beginPaint(Buffer *buffer, QPainter *painter, const QRectF &rect) const
{
  drawBuffer_ = buffer->type();

  if (isOverview() || ! isBufferLayers())
    return painter;

  // resize and clear
  auto prect = (! rect.isValid() ? calcPlotPixelRect().qrect() : rect);

  auto *painter1 = buffer->beginPaint(painter, prect, view()->isAntiAlias());

  // don't paint if not active
  if (! buffer->isActive())
    return nullptr;

  return painter1;
}

void
CQChartsPlot::
endPaint(Buffer *buffer) const
{
  if (isOverview() || ! isBufferLayers())
    return;

  buffer->endPaint(false);
}

CQChartsPlotKey *
CQChartsPlot::
getFirstPlotKey() const
{
  auto *plot = firstPlot();

  while (plot) {
    auto *key = plot->key();

    if (key && key->isVisibleAndNonEmpty())
      return key;

    plot = plot->nextPlot();
  }

  return nullptr;
}

//------

// Note: symbol size already scaled
void
CQChartsPlot::
drawSymbol(PaintDevice *device, const Point &p, const Symbol &symbol,
           double pxs, double pys, const PenBrush &penBrush, bool scaled) const
{
  CQChartsDrawUtil::setPenBrush(device, penBrush);

  drawSymbol(device, p, symbol, pxs, pys, scaled);
}

// Note: symbol size already scaled
void
CQChartsPlot::
drawSymbol(PaintDevice *device, const Point &p, const Symbol &symbol,
           double pxs, double pys, bool scaled) const
{
  double xs, ys;

  if (scaled) {
    useRawRange_ = isScaleSymbolSize();

    xs = pixelToWindowWidth (pxs);
    ys = pixelToWindowHeight(pys);

    useRawRange_ = false;

    pxs = windowToPixelWidth (xs);
    pys = windowToPixelHeight(ys);
  }

  if (bufferSymbols_) {
    auto *viewPlotDevice = dynamic_cast<CQChartsViewPlotPaintDevice *>(device);

    if (viewPlotDevice)
      drawBufferedSymbol(viewPlotDevice->painter(), p, symbol, std::min(pxs, pys));
    else
      CQChartsDrawUtil::drawSymbol(device, symbol, p, Length::pixel(pxs), Length::pixel(pys),
                                   /*scale*/false);
  }
  else {
    CQChartsDrawUtil::drawSymbol(device, symbol, p, Length::pixel(pxs), Length::pixel(pys),
                                 /*scale*/false);
  }
}

void
CQChartsPlot::
drawBufferedSymbol(QPainter *painter, const Point &p, const Symbol &symbol, double size) const
{
  auto image = CQChartsSymbolBufferInst->getImage(symbol, size, painter->pen(), painter->brush());

  auto pp = windowToPixel(p);

  double is = image.width()/2.0;

  painter->drawImage(int(pp.x - is), int(pp.y - is), image);
}

//------

CQChartsTextOptions
CQChartsPlot::
adjustTextOptions(const CQChartsTextOptions &options) const
{
  auto options1 = options;

  options1.minScaleFontSize = minScaleFontSize();
  options1.maxScaleFontSize = maxScaleFontSize();

  return options1;
}

//------

void
CQChartsPlot::
drawWindowColorBox(PaintDevice *device, const BBox &bbox, const QColor &c) const
{
  auto *viewPlotDevice = dynamic_cast<CQChartsViewPlotPaintDevice *>(device);
  if (! viewPlotDevice) return;

  if (! bbox.isSet())
    return;

  //auto prect = windowToPixel(bbox);
  //drawColorBox(viewPlotDevice, prect, c);

  drawColorBox(viewPlotDevice, bbox, c);
}

void
CQChartsPlot::
drawColorBox(PaintDevice *device, const BBox &bbox, const QColor &c) const
{
  auto *viewPlotDevice = dynamic_cast<CQChartsViewPlotPaintDevice *>(device);
  if (! viewPlotDevice) return;

  viewPlotDevice->setPen(c);
  viewPlotDevice->setBrush(Qt::NoBrush);

  viewPlotDevice->drawRect(bbox);
}

//------

bool
CQChartsPlot::
isSetHidden(int id) const
{
  auto p = idHidden_.find(id);

  if (p == idHidden_.end())
    return false;

  return (*p).second;
}

void
CQChartsPlot::
setSetHidden(int id, bool hidden)
{
  if (hidden != isSetHidden(id)) {
    idHidden_[id] = hidden;

    updateRangeAndObjs();
  }
}

void
CQChartsPlot::
resetSetHidden()
{
  if (! idHidden_.empty()) {
    idHidden_.clear();

    updateRangeAndObjs();
  }
}

bool
CQChartsPlot::
isHideValue(const QVariant &value) const
{
  return (CQChartsVariant::cmp(value, hideValue()) == 0);
}

//---

void
CQChartsPlot::
update()
{
  assert(fromInvalidate_);

  view()->doUpdate();

  fromInvalidate_ = false;
}

//------

void
CQChartsPlot::
setPenBrush(PenBrush &penBrush, const PenData &penData, const BrushData &brushData) const
{
  setPen  (penBrush, penData  );
  setBrush(penBrush, brushData);
}

void
CQChartsPlot::
setPen(PenBrush &penBrush, const PenData &penData) const
{
  double width = 0.0;

  if (penData.width().isValid())
    width = limitLineWidth(lengthPixelWidth(penData.width()));

  CQChartsUtil::setPen(penBrush.pen, penData.isVisible(), penData.color(), penData.alpha(),
                       width, penData.dash(), penData.lineCap(), penData.lineJoin());
}

void
CQChartsPlot::
setBrush(PenBrush &penBrush, const BrushData &brushData) const
{
  CQChartsDrawUtil::setBrush(penBrush.brush, brushData);

  if (brushData.pattern().altColor().isValid())
    penBrush.altColor = brushData.pattern().altColor().color();
  else
    penBrush.altColor = QColor();

  if (brushData.pattern().altAlpha().isSet())
    penBrush.altAlpha = brushData.pattern().altAlpha().value();
  else
    penBrush.altAlpha = 1.0;

  penBrush.fillAngle  = brushData.pattern().angle().degrees();
  penBrush.fillType   = brushData.pattern().type();
  penBrush.fillRadius = brushData.pattern().radius();
  penBrush.fillDelta  = brushData.pattern().delta();
}

//------

void
CQChartsPlot::
setPenBrush(PaintDevice *device, const PenData &penData, const BrushData &brushData) const
{
  PenBrush penBrush;

  setPenBrush(penBrush, penData, brushData);

  CQChartsDrawUtil::setPenBrush(device, penBrush);
}

void
CQChartsPlot::
setPen(PaintDevice *device, const PenData &penData) const
{
  PenBrush penBrush;

  setPen(penBrush, penData);

  device->setPen(penBrush.pen);
}

void
CQChartsPlot::
setBrush(PaintDevice *device, const BrushData &brushData) const
{
  PenBrush penBrush;

  setBrush(penBrush, brushData);

  device->setBrush(penBrush.brush);
}

//------

void
CQChartsPlot::
setPainterFont(PaintDevice *device, const Font &font) const
{
  view()->setPlotPainterFont(this, device, font);
}

//------

void
CQChartsPlot::
updateObjPenBrushState(const Obj *obj, PenBrush &penBrush, DrawType drawType) const
{
  updateObjPenBrushState(obj, ColorInd(), penBrush, drawType);
}

void
CQChartsPlot::
updateObjPenBrushState(const Obj *obj, const ColorInd &ic, PenBrush &penBrush,
                       DrawType drawType) const
{
  view()->updateObjPenBrushState(obj, ic, penBrush, drawType);
}

void
CQChartsPlot::
updatePenBrushState(const ColorInd &ic, PenBrush &penBrush, bool selected, bool inside) const
{
  view()->updatePenBrushState(ic, penBrush, selected, inside);
}

QColor
CQChartsPlot::
insideColor(const QColor &c) const
{
  return view()->calcInsideColor(c);
}

QColor
CQChartsPlot::
selectedColor(const QColor &c) const
{
  return view()->calcSelectedColor(c);
}

//------

QColor
CQChartsPlot::
interpPaletteColor(const ColorInd &ind, bool scale) const
{
  auto c = Color::makePalette();

  c.setScale(scale);

  return interpColor(c, ind);

  //return view()->interpPaletteColor(ind, scale);
}

QColor
CQChartsPlot::
interpGroupPaletteColor(const ColorInd &ig, const ColorInd &iv, bool scale) const
{
  return view()->interpGroupPaletteColor(ig, iv, scale);
}

QColor
CQChartsPlot::
blendGroupPaletteColor(double r1, double r2, double dr) const
{
  auto *theme = view()->theme();

  // r1 is parent color and r2 is child color
  auto c1 = theme->palette()->getColor(r1 - dr/2.0);
  auto c2 = theme->palette()->getColor(r1 + dr/2.0);

  return CQChartsUtil::blendColors(c1, c2, r2);
}

QColor
CQChartsPlot::
interpThemeColor(const ColorInd &ind) const
{
  return view()->interpThemeColor(ind);
}

QColor
CQChartsPlot::
interpInterfaceColor(double r) const
{
  return view()->interpInterfaceColor(r);
}

QColor
CQChartsPlot::
interpColor(const Color &c, int ig, int ng, const ColorInd &ind) const
{
  auto c1 = c;

  if (defaultPalette_.isValid())
    c1 = charts()->adjustDefaultPalette(c, defaultPalette_.name());

  return view()->interpColor(c1, ig, ng, ind);
}

QColor
CQChartsPlot::
interpColor(const Color &c, const ColorInd &ind) const
{
  auto c1 = c;

  if (defaultPalette_.isValid())
    c1 = charts()->adjustDefaultPalette(c, defaultPalette_.name());

  return view()->interpColor(c1, ind);
}

//---

QColor
CQChartsPlot::
calcTextColor(const QColor &bg) const
{
  return CQChartsUtil::bwColor(bg);
}

CQChartsColor
CQChartsPlot::
calcTextColor(const CQChartsColor &bg) const
{
  return CQChartsColor(CQChartsUtil::bwColor(interpColor(bg, ColorInd())));
}

CQChartsPlot::ColorInd
CQChartsPlot::
calcColorInd(const PlotObj *obj, const CQChartsColorBoxKeyItem *keyBox,
             const ColorInd &is, const ColorInd &ig, const ColorInd &iv) const
{
  ColorInd colorInd;

  auto colorType = this->colorType();

  if (obj && colorType == ColorType::AUTO)
    colorType = static_cast<CQChartsPlot::ColorType>(obj->colorType());

  if      (colorType == ColorType::AUTO)
    colorInd = (is.n <= 1 ? (ig.n <= 1 ? iv : ig) : is);
  else if (colorType == ColorType::SET)
    colorInd = is;
  else if (colorType == ColorType::GROUP)
    colorInd = ig;
  else if (colorType == ColorType::INDEX)
    colorInd = iv;
  else if (colorType == ColorType::X_VALUE) {
    const auto &stops = colorXStops();

    bool hasStops = stops.isValid();
    bool relative = (hasStops ? stops.isPercent() : true);

    double x = 0.0;

    if      (obj)
      x = obj->xColorValue(relative);
    else if (keyBox)
      x = keyBox->xColorValue(relative);

    if (hasStops) {
      if (stops.isDiscreet()) {
        int ind = stops.ind(x);

        colorInd = ColorInd(ind, stops.size() + 1);
      }
      else {
        double c = stops.interp(x);

        colorInd = ColorInd(c);
      }
    }
    else
      colorInd = ColorInd(x);
  }
  else if (colorType == ColorType::Y_VALUE) {
    const auto &stops = colorYStops();

    bool hasStops = stops.isValid();
    bool relative = (hasStops ? stops.isPercent() : true);

    double y = 0.0;

    if      (obj)
      y = obj->yColorValue(relative);
    else if (keyBox)
      y = keyBox->yColorValue(relative);

    if (hasStops) {
      int ind = stops.ind(y);

      colorInd = ColorInd(ind, stops.size() + 1);
    }
    else
      colorInd = ColorInd(y);
  }

  return colorInd;
}

//------

bool
CQChartsPlot::
checkNumericColumns(const Columns &columns, const QString &name, bool required) const
{
  if (required && ! columns.isValid())
    return const_cast<Plot *>(this)->
      addError(QString("Missing required %1 columns").arg(name));

  bool valid = true;

  int iv = 0;

  for (const auto &column : columns) {
    if (! column.isColumn()) // TODO: handle non-column column type
      continue;

    auto type = columnValueType(column);

    if (type == ColumnType::NONE)
      valid = const_cast<Plot *>(this)->
        addColumnError(column, QString("Invalid %1 column (#%2)").arg(name).arg(iv));

    if (type != ColumnType::REAL && type != ColumnType::INTEGER)
      valid = const_cast<Plot *>(this)->
        addColumnError(column, QString("Non-numeric %1 column (#%2)").arg(name).arg(iv));

    ++iv;
  }

  return valid;
}

bool
CQChartsPlot::
checkNumericColumn(const Column &column, const QString &name, bool required) const
{
  ColumnType type;

  return checkNumericColumn(column, name, type, required);
}

bool
CQChartsPlot::
checkNumericColumn(const Column &column, const QString &name,
                   ColumnType &type, bool required) const
{
  if (! required && ! column.isValid())
    return true;

  if (! checkColumn(column, name, type, required))
    return false;

  if (! column.isColumn()) // TODO: handle non-column column type
    return true;

  if (type != ColumnType::REAL && type != ColumnType::INTEGER) {
    const_cast<Plot *>(this)->
      addColumnError(column, QString("Non-numeric %1 column").arg(name));
    return false;
  }

  return true;
}

bool
CQChartsPlot::
checkColumns(const Columns &columns, const QString &name, bool required) const
{
  if (required && ! columns.isValid())
    return const_cast<Plot *>(this)->
      addError(QString("Missing required %1 columns").arg(name));

  bool valid = true;

  int iv = 0;

  for (const auto &column : columns) {
    if (columnValueType(column) == ColumnType::NONE)
      valid = const_cast<Plot *>(this)->
        addColumnError(column, QString("Invalid %1 column (#%2)").arg(name).arg(iv));

    ++iv;
  }

  return valid;
}

bool
CQChartsPlot::
checkColumn(const Column &column, const QString &name, bool required) const
{
  ColumnType type;

  return checkColumn(column, name, type, required);
}

bool
CQChartsPlot::
checkColumn(const Column &column, const QString &name, ColumnType &type, bool required) const
{
  type = ColumnType::NONE;

  if (required && ! column.isValid())
    return const_cast<Plot *>(this)->
      addColumnError(column, QString("Missing required %1 column").arg(name));

  if (column.isValid()) {
    type = columnValueType(column);

    if (type == ColumnType::NONE)
      return const_cast<Plot *>(this)->
        addColumnError(column, QString("Invalid %1 column").arg(name));
  }

  return true;
}

CQChartsPlot::ColumnType
CQChartsPlot::
columnValueType(const Column &column, const ColumnType &defType) const
{
  ModelTypeData columnTypeData;

  if (! columnValueType(column, columnTypeData, defType))
    return ColumnType::NONE;

  return columnTypeData.type;
}

bool
CQChartsPlot::
columnValueType(const Column &column, ModelTypeData &columnTypeData,
                const ColumnType &defType) const
{
  if (! column.isValid()) {
    columnTypeData.type     = ColumnType::NONE;
    columnTypeData.baseType = ColumnType::NONE;
    return false;
  }

  if (column.hasExpr()) {
    columnTypeData.type     = ColumnType::REAL;
    columnTypeData.baseType = ColumnType::REAL;
    return true;
  }

  if (column.isRow()) {
    columnTypeData.type     = ColumnType::INTEGER;
    columnTypeData.baseType = ColumnType::INTEGER;
    return true;
  }

  auto *columnDetails = this->columnDetails(column);

  if (columnDetails) {
    // if has details column is valid
    columnTypeData.type       = columnDetails->type();
    columnTypeData.baseType   = columnDetails->baseType();
    columnTypeData.nameValues = columnDetails->nameValues();

    if (columnTypeData.type == ColumnType::NONE) {
      // if no column type then could not be calculated (still return true)
      columnTypeData.type     = defType;
      columnTypeData.baseType = defType;
    }
  }
  else {
    if (! modelColumnValueType(column, columnTypeData)) {
      // if fail column is invalid
      columnTypeData.type     = ColumnType::NONE;
      columnTypeData.baseType = ColumnType::NONE;
      return false;
    }
  }

  return true;
}

bool
CQChartsPlot::
modelColumnValueType(const Column &column, ModelTypeData &columnTypeData) const
{
  auto *model = this->currentModel().data();
  assert(model);

  return CQChartsModelUtil::columnValueType(charts(), model, mapColumn(column), columnTypeData);
}

#if 0
bool
CQChartsPlot::
columnTypeStr(const Column &column, QString &typeStr) const
{
  auto *model = this->currentModel().data();
  assert(model);

  return CQChartsModelUtil::columnTypeStr(charts(), model, mapColumn(column), typeStr);
}

bool
CQChartsPlot::
setColumnTypeStr(const Column &column, const QString &typeStr)
{
  auto *model = this->currentModel().data();
  assert(model);

  return CQChartsModelUtil::setColumnTypeStr(charts(), model, mapColumn(column), typeStr);
}
#endif

bool
CQChartsPlot::
columnDetails(const Column &column, QString &typeName, QVariant &minValue, QVariant &maxValue) const
{
  if (! column.isValid())
    return false;

  auto *details = this->columnDetails(column);
  if (! details) return false;

  typeName = details->typeName();
  minValue = details->minValue();
  maxValue = details->maxValue();

  return true;
}

CQChartsModelColumnDetails *
CQChartsPlot::
columnDetails(const Column &column) const
{
  assert(column.isValid());

  const auto &model = currentModel();

  auto *details = modelDetails(model);
  if (! details) return nullptr;

  return details->columnDetails(mapColumn(column));
}

CQChartsModelData *
CQChartsPlot::
getModelData(const ModelP &model) const
{
  return charts()->getModelData(model.data());
}

CQChartsModelDetails *
CQChartsPlot::
modelDetails(const ModelP &model) const
{
  auto *modelData = getModelData(model);
  if (! modelData) return nullptr;

  return modelData->details();
}

//------

bool
CQChartsPlot::
getHierColumnNames(const QModelIndex &parent, int row, const Columns &nameColumns,
                   const QString &separator, QStringList &nameStrs, QModelIndices &nameInds) const
{
  auto *model = this->currentModel().data();
  assert(model);

  auto *th = const_cast<Plot *>(this);

  // single column (separated names)
  if (nameColumns.count() == 1) {
    const auto &nameColumn = nameColumns.column();

    //---

    ModelIndex nameModelInd(th, row, nameColumn, parent);

    bool ok;

    auto name = modelString(nameModelInd, ok);

    if (ok && ! name.trimmed().length())
      ok = false;

    if (ok) {
      if (separator.length())
        nameStrs = name.split(separator, Qt::SkipEmptyParts);
      else
        nameStrs << name;
    }

    auto nameInd = modelIndex(nameModelInd);

    nameInds.push_back(nameInd);
  }
  else {
    for (auto &nameColumn : nameColumns) {
      ModelIndex nameModelInd(th, row, nameColumn, parent);

      bool ok;

      auto name = modelString(nameModelInd, ok);

      if (ok && ! name.trimmed().length())
        ok = false;

      if (ok) {
        nameStrs << name;

        auto nameInd = modelIndex(nameModelInd);

        nameInds.push_back(nameInd);
      }
    }
  }

  return nameStrs.length();
}

//------

bool
CQChartsPlot::
isNormalizedIndex(const ModelIndex &ind) const
{
  assert(ind.plot() == this);

  if (ind.parent().model()) {
    ProxyModels         proxyModels;
    QAbstractItemModel* sourceModel;

    this->proxyModels(proxyModels, sourceModel);

    return (ind.parent().model() == sourceModel);
  }
  else
    return false; // default index (from plot model) should be unnormalized
}

CQChartsPlot::ModelIndex
CQChartsPlot::
normalizeIndex(const ModelIndex &ind) const
{
  assert(ind.plot() == this && ! ind.isNormalized());

  auto *th = const_cast<Plot *>(this);

  auto ind1 = normalizeIndex(modelIndex(ind));

  ModelIndex nind;

  if (ind1.column() == ind.column().column())
    nind = ModelIndex(th, ind1.row(), ind.column(), ind1.parent());
  else
    nind = ModelIndex(th, ind1.row(), Column(ind1.column()), ind1.parent());

  nind.setNormalized(true);

  return nind;
}

CQChartsPlot::ModelIndex
CQChartsPlot::
unnormalizeIndex(const ModelIndex &ind) const
{
  assert(ind.plot() == this && ind.isNormalized());

  auto *th = const_cast<Plot *>(this);

  auto ind1 = unnormalizeIndex(modelIndex(ind));

  ModelIndex nind;

  if (ind1.column() == ind.column().column())
    nind = ModelIndex(th, ind1.row(), ind.column(), ind1.parent());
  else
    nind = ModelIndex(th, ind1.row(), Column(ind1.column()), ind1.parent());

  nind.setNormalized(false);

  return nind;
}

QModelIndex
CQChartsPlot::
normalizeIndex(const QModelIndex &ind) const
{
  // map index in proxy model, to source model (non-proxy model)
  auto *model = this->currentModel().data();
  assert(model);

  ProxyModels         proxyModels;
  QAbstractItemModel* sourceModel;

  this->proxyModels(proxyModels, sourceModel);

  // assert(ind.model() != sourceModel); // TODO: assert

  auto ind1 = ind;

  size_t i = 0;

  // ind model should match first proxy model
  for ( ; i < proxyModels.size(); ++i)
    if (ind1.model() == proxyModels[i])
      break;

  for ( ; i < proxyModels.size(); ++i)
    ind1 = proxyModels[i]->mapToSource(ind1);

  return ind1;
}

QModelIndex
CQChartsPlot::
unnormalizeIndex(const QModelIndex &ind) const
{
  // map index in source model (non-proxy model), to proxy model
  auto *model = this->currentModel().data();
  assert(model);

  ProxyModels         proxyModels;
  QAbstractItemModel* sourceModel;

  this->proxyModels(proxyModels, sourceModel);

  // assert(ind.model() == sourceModel); // TODO: assert

  auto ind1 = ind;

  // ind model should match source model of last proxy model
  int i = int(proxyModels.size()) - 1;

  for ( ; i >= 0; --i)
    if (ind1.model() == proxyModels[size_t(i)]->sourceModel())
      break;

  for ( ; i >= 0; --i)
    ind1 = proxyModels[size_t(i)]->mapFromSource(ind1);

  return ind1;
}

QAbstractItemModel *
CQChartsPlot::
sourceModel() const
{
  ProxyModels         proxyModels;
  QAbstractItemModel* sourceModel;

  this->proxyModels(proxyModels, sourceModel);

  return sourceModel;
}

void
CQChartsPlot::
proxyModels(ProxyModels &proxyModels, QAbstractItemModel* &sourceModel) const
{
  // map index in source model (non-proxy model), to proxy model
  auto *model = this->currentModel().data();
  assert(model);

  auto *proxyModel = qobject_cast<QAbstractProxyModel *>(model);

  if (proxyModel) {
    while (proxyModel) {
      proxyModels.push_back(proxyModel);

      sourceModel = proxyModel->sourceModel();

      proxyModel = qobject_cast<QAbstractProxyModel *>(sourceModel);
    }
  }
  else
    sourceModel = model;
}

bool
CQChartsPlot::
isHierarchical() const
{
  const auto &model = currentModel();

  auto *details = modelDetails(model);

  if (details)
    return details->isHierarchical();

  return CQChartsModelUtil::isHierarchical(model.data());
}

//------

void
CQChartsPlot::
addColumnValues(const Column &column, ValueSet &valueSet) const
{
  class ValueSetVisitor : public ModelVisitor {
   public:
    ValueSetVisitor(const Plot *plot, const Column &column, ValueSet &valueSet) :
     plot_(plot), column_(column), valueSet_(valueSet) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      ModelIndex columnInd(plot_, data.row, column_, data.parent);

      bool ok;

      auto value = plot_->modelValue(columnInd, ok);

      // TODO: skip if not ok ?

      valueSet_.addValue(value);

      return State::OK;
    }

   private:
    const CQChartsPlot* plot_   { nullptr };
    Column              column_;
    ValueSet&           valueSet_;
  };

  ValueSetVisitor valueSetVisitor(this, column, valueSet);

  visitModel(valueSetVisitor);
}

//------

void
CQChartsPlot::
visitModel(ModelVisitor &visitor) const
{
  CQPerfTrace trace("CQChartsPlot::visitModel");

  visitor.setPlot(this);

  //visitor.init();

  //if (isPreview())
  //  visitor.setMaxRows(previewMaxRows());

  const auto &model = this->currentModel();

  (void) CQChartsModelVisit::exec(charts(), model.data(), visitor);

  //visitor.term();
}

//------

double
CQChartsPlot::
modelNumericValue(const ModelIndex &ind, bool &ok) const
{
  auto *columnDetails = this->columnDetails(ind.column());

  if (! columnDetails || columnDetails->isNumeric())
    return modelReal(ind, ok);

  auto var = modelValue(ind, ok);
  if (! ok) return 0.0;

  ok = true;

  return columnDetails->uniqueId(var);
}

//------

bool
CQChartsPlot::
modelMappedReal(int row, const Column &column, const QModelIndex &parent, double &r,
                bool log, double def) const
{
  auto *th = const_cast<Plot *>(this);

  return modelMappedReal(ModelIndex(th, row, column, parent), r, log, def);
}

bool
CQChartsPlot::
modelMappedReal(const ModelIndex &ind, double &r, bool log, double def) const
{
  bool ok = false;

  if (ind.isValid()) {
    r = modelReal(ind, ok);

    if (! ok)
      r = def;

    if (CMathUtil::isNaN(r) || CMathUtil::isInf(r))
      return false;
  }
  else
    r = def;

  if (log) {
    if (r <= 0)
      return false;

    r = logValue(r);
  }

  return ok;
}

//------

// used to lookup row by name in tcl
int
CQChartsPlot::
getRowForId(const QString &id) const
{
  if (! idColumn().isValid())
    return -1;

  // process model data
  class IdVisitor : public ModelVisitor {
   public:
    IdVisitor(const Plot *plot, const QString &id) :
     plot_(plot), id_(id) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      bool ok;

      auto id = plot_->idColumnString(data.row, data.parent, ok);

      if (ok && id == id_)
        row_ = data.row;

      return State::OK;
    }

    int row() const { return row_; }

   private:
    const Plot *plot_ { nullptr };
    QString     id_;
    int         row_ { -1 };
  };

  IdVisitor idVisitor(this, id);

  visitModel(idVisitor);

  return idVisitor.row();
}

QString
CQChartsPlot::
idColumnString(int row, const QModelIndex &parent, bool &ok) const
{
  ok = false;

  if (! idColumn().isValid())
    return "";

  auto *th = const_cast<Plot *>(this);

  ModelIndex idColumnInd(th, row, idColumn(), parent);

  auto var = modelValue(idColumnInd, ok);

  if (! ok)
    return "";

  QString str;

  double r;

  if (CQChartsVariant::toReal(var, r))
    str = columnStr(idColumn(), r);
  else {
    bool ok;
    str = CQChartsVariant::toString(var, ok);
  }

  return str;
}

//------

QModelIndex
CQChartsPlot::
normalizedModelIndex(const ModelIndex &ind) const
{
  auto ind1 = ind;

  if (! ind1.isNormalized())
    ind1 = normalizeIndex(ind1);

  return modelIndex(ind1);
}

QModelIndex
CQChartsPlot::
modelIndex(const ModelIndex &ind) const
{
  assert(ind.plot() == this);

  return modelIndex(ind.row(), ind.column(), ind.parent(), ind.isNormalized());
}

QModelIndex
CQChartsPlot::
modelIndex(int row, const Column &column, const QModelIndex &parent,
           bool normalized /*=false*/) const
{
  auto column1 = mapColumn(column);

  if (! column1.hasColumn())
    return QModelIndex();

  if (! normalized) {
    auto *model = this->currentModel().data();
    if (! model) return QModelIndex();

    assert(! parent.model() || parent.model() == model);

    return model->index(row, column1.column(), parent);
  }
  else {
    ProxyModels         proxyModels;
    QAbstractItemModel* sourceModel;

    this->proxyModels(proxyModels, sourceModel);

    assert(! parent.model() || parent.model() == sourceModel);

    return sourceModel->index(row, column1.column(), parent);
  }
}

//------

#if 0
QVariant
CQChartsPlot::
modelHHeaderValue(const Column &column, bool &ok) const
{
  const auto &model = this->currentModel();

  return modelHHeaderValue(model.data(), column, ok);
}

QVariant
CQChartsPlot::
modelHHeaderValue(const Column &column, int role, bool &ok) const
{
  const auto &model = this->currentModel();

  return modelHHeaderValue(model.data(), column, role, ok);
}

QVariant
CQChartsPlot::
modelHHeaderValue(QAbstractItemModel *model, const Column &column, bool &ok) const
{
  return CQChartsModelUtil::modelHeaderValue(model, mapColumn(column), ok);
}

QVariant
CQChartsPlot::
modelHHeaderValue(QAbstractItemModel *model, const Column &column, int role, bool &ok) const
{
  return CQChartsModelUtil::modelHeaderValue(model, mapColumn(column), role, ok);
}
#endif

//--

#if 0
QVariant
CQChartsPlot::
modelVHeaderValue(int section, Qt::Orientation orient, int role, bool &ok) const
{
  const auto &model = this->currentModel();

  return modelVHeaderValue(model.data(), section, orient, role, ok);
}

QVariant
CQChartsPlot::
modelVHeaderValue(int section, Qt::Orientation orient, bool &ok) const
{
  const auto &model = this->currentModel();

  return modelVHeaderValue(model.data(), section, orient, Qt::DisplayRole, ok);
}

QVariant
CQChartsPlot::
modelVHeaderValue(QAbstractItemModel *model, int section, Qt::Orientation orientation,
                  bool &ok) const
{
  return CQChartsModelUtil::modelHeaderValue(model, section, orientation, ok);
}

QVariant
CQChartsPlot::
modelVHeaderValue(QAbstractItemModel *model, int section, Qt::Orientation orientation,
                  int role, bool &ok) const
{
  return CQChartsModelUtil::modelHeaderValue(model, section, orientation, role, ok);
}
#endif

//--

QString
CQChartsPlot::
modelHHeaderString(const Column &column, bool &ok) const
{
  const auto &model = this->currentModel();

  return modelHHeaderString(model.data(), column, ok);
}

QString
CQChartsPlot::
modelHHeaderString(const Column &column, int role, bool &ok) const
{
  const auto &model = this->currentModel();

  return modelHHeaderString(model.data(), column, role, ok);
}

QString
CQChartsPlot::
modelHHeaderString(QAbstractItemModel *model, const Column &column, bool &ok) const
{
  return CQChartsModelUtil::modelHHeaderString(model, mapColumn(column), ok);
}

QString
CQChartsPlot::
modelHHeaderString(QAbstractItemModel *model, const Column &column, int role, bool &ok) const
{
  return CQChartsModelUtil::modelHHeaderString(model, mapColumn(column), role, ok);
}

//--

QString
CQChartsPlot::
modelHHeaderTip(const Column &column, bool &ok) const
{
  HeaderNameData data;

  data.tip    = true;
  data.number = true;

  return modelHHeaderName(column, data, ok);
}

QString
CQChartsPlot::
modelHHeaderName(const Column &column, const HeaderNameData &data, bool &ok) const
{
  QString str;

  const auto &model = this->currentModel();
  if (! model.data()) return str;

  if (data.tip)
    str = modelHHeaderString(model.data(), column,
            CQModelUtil::roleCast(CQBaseModelRole::Tip), ok);
  else
    ok = true;

  if (! ok || ! str.length())
    str = CQChartsModelUtil::modelHHeaderString(model.data(), mapColumn(column), ok);

  auto column1 = mapColumn(column);

  if (column1.hasColumn() && data.number)
    str += QString(" (#%1)").arg(column1.column());

  return str;
}

//--

QString
CQChartsPlot::
modelVHeaderString(int section, int role, bool &ok) const
{
  const auto &model = this->currentModel();

  return modelVHeaderString(model.data(), section, role, ok);
}

QString
CQChartsPlot::
modelVHeaderString(int section, bool &ok) const
{
  const auto &model = this->currentModel();

  return modelVHeaderString(model.data(), section, Qt::DisplayRole, ok);
}

QString
CQChartsPlot::
modelVHeaderString(QAbstractItemModel *model, int section, int role, bool &ok) const
{
  return CQChartsModelUtil::modelVHeaderString(model, section, role, ok);
}

QString
CQChartsPlot::
modelVHeaderString(QAbstractItemModel *model, int section, bool &ok) const
{
  return CQChartsModelUtil::modelVHeaderString(model, section, ok);
}

//------

QVariant
CQChartsPlot::
modelValue(int row, const Column &column, const QModelIndex &parent, int role, bool &ok) const
{
  auto *th = const_cast<Plot *>(this);

  return modelValue(ModelIndex(th, row, column, parent), role, ok);
}

QVariant
CQChartsPlot::
modelValue(int row, const Column &column, const QModelIndex &parent, bool &ok) const
{
  auto *th = const_cast<Plot *>(this);

  return modelValue(ModelIndex(th, row, column, parent), ok);
}

QVariant
CQChartsPlot::
modelValue(const ModelIndex &ind, int role, bool &ok) const
{
  if (ind.column().isColumn() || ind.column().isCell()) {
    auto c = ind.column();

    if (ind.column().isColumn())
      c.setColumnCol(ind.cellCol());
    else
      c.setCellCol(ind.cellCol());

    const auto &model = this->currentModel();

    return modelValue(model.data(), ind.row(), c, ind.parent(), role, ok);
  }
  else {
    const auto &model = this->currentModel();

    return modelValue(model.data(), ind.row(), ind.column(), ind.parent(), role, ok);
  }
}

QVariant
CQChartsPlot::
modelValue(const ModelIndex &ind, bool &ok) const
{
  if (ind.column().isColumn() || ind.column().isCell()) {
    auto c = ind.column();

    if (ind.column().isColumn())
      c.setColumnCol(ind.cellCol());
    else
      c.setCellCol(ind.cellCol());

    const auto &model = this->currentModel();

    return modelValue(model.data(), ind.row(), c, ind.parent(), ok);
  }
  else {
    const auto &model = this->currentModel();

    return modelValue(model.data(), ind.row(), ind.column(), ind.parent(), ok);
  }
}

QVariant
CQChartsPlot::
modelValue(QAbstractItemModel *model, int row, const Column &column,
           const QModelIndex &parent, int role, bool &ok) const
{
  return CQChartsModelUtil::modelValue(charts(), model, row, mapColumn(column), parent, role, ok);
}

QVariant
CQChartsPlot::
modelValue(QAbstractItemModel *model, int row, const Column &column,
           const QModelIndex &parent, bool &ok) const
{
  if (column.hasRole())
    return CQChartsModelUtil::modelValue(charts(), model, row, mapColumn(column),
                                         parent, column.role(), ok);

  return CQChartsModelUtil::modelValue(charts(), model, row, mapColumn(column), parent, ok);
}

//---

QString
CQChartsPlot::
modelString(int row, const Column &column, const QModelIndex &parent, int role, bool &ok) const
{
  auto *th = const_cast<Plot *>(this);

  return modelString(ModelIndex(th, row, column, parent), role, ok);
}

QString
CQChartsPlot::
modelString(int row, const Column &column, const QModelIndex &parent, bool &ok) const
{
  auto *th = const_cast<Plot *>(this);

  return modelString(ModelIndex(th, row, column, parent), ok);
}

QString
CQChartsPlot::
modelString(const ModelIndex &ind, int role, bool &ok) const
{
  const auto &model = this->currentModel();

  return modelString(model.data(), ind.row(), ind.column(), ind.parent(), role, ok);
}

QString
CQChartsPlot::
modelString(const ModelIndex &ind, bool &ok) const
{
  const auto &model = this->currentModel();

  return modelString(model.data(), ind.row(), ind.column(), ind.parent(), ok);
}

QString
CQChartsPlot::
modelString(QAbstractItemModel *model, const ModelIndex &ind, int role, bool &ok) const
{
  return CQChartsModelUtil::modelString(charts(), model, ind.row(), mapColumn(ind.column()),
                                        ind.parent(), role, ok);
}

QString
CQChartsPlot::
modelString(QAbstractItemModel *model, const ModelIndex &ind, bool &ok) const
{
  return CQChartsModelUtil::modelString(charts(), model, ind.row(), mapColumn(ind.column()),
                                        ind.parent(), ok);
}

QString
CQChartsPlot::
modelString(QAbstractItemModel *model, int row, const Column &column,
            const QModelIndex &parent, int role, bool &ok) const
{
  return CQChartsModelUtil::modelString(charts(), model, row, mapColumn(column),
                                        parent, role, ok);
}

QString
CQChartsPlot::
modelString(QAbstractItemModel *model, int row, const Column &column,
            const QModelIndex &parent, bool &ok) const
{
  if (column.hasRole())
    return CQChartsModelUtil::modelString(charts(), model, row, mapColumn(column),
                                          parent, column.role(), ok);

  return CQChartsModelUtil::modelString(charts(), model, row, mapColumn(column), parent, ok);
}

//---

double
CQChartsPlot::
modelReal(int row, const Column &column, const QModelIndex &parent, int role, bool &ok) const
{
  auto *th = const_cast<Plot *>(this);

  return modelReal(ModelIndex(th, row, column, parent), role, ok);
}

double
CQChartsPlot::
modelReal(int row, const Column &column, const QModelIndex &parent, bool &ok) const
{
  auto *th = const_cast<Plot *>(this);

  return modelReal(ModelIndex(th, row, column, parent), ok);
}

double
CQChartsPlot::
modelReal(const ModelIndex &ind, int role, bool &ok) const
{
  if (ind.column().isColumn() || ind.column().isCell()) {
    auto c = ind.column();

    if (ind.column().isColumn())
      c.setColumnCol(ind.cellCol());
    else
      c.setCellCol(ind.cellCol());

    const auto &model = this->currentModel();

    return modelReal(model.data(), ind.row(), c, ind.parent(), role, ok);
  }
  else {
    const auto &model = this->currentModel();

    return modelReal(model.data(), ind.row(), ind.column(), ind.parent(), role, ok);
  }
}

double
CQChartsPlot::
modelReal(const ModelIndex &ind, bool &ok) const
{
  if (ind.column().isColumn() || ind.column().isCell()) {
    auto c = ind.column();

    if (ind.column().isColumn())
      c.setColumnCol(ind.cellCol());
    else
      c.setCellCol(ind.cellCol());

    const auto &model = this->currentModel();

    return modelReal(model.data(), ind.row(), c, ind.parent(), ok);
  }
  else {
    const auto &model = this->currentModel();

    return modelReal(model.data(), ind.row(), ind.column(), ind.parent(), ok);
  }
}

double
CQChartsPlot::
modelReal(QAbstractItemModel *model, int row, const Column &column,
          const QModelIndex &parent, int role, bool &ok) const
{
  return CQChartsModelUtil::modelReal(charts(), model, row, mapColumn(column), parent, role, ok);
}

double
CQChartsPlot::
modelReal(QAbstractItemModel *model, int row, const Column &column,
          const QModelIndex &parent, bool &ok) const
{
  if (column.hasRole())
    return CQChartsModelUtil::modelReal(charts(), model, row, mapColumn(column),
                                        parent, column.role(), ok);

  return CQChartsModelUtil::modelReal(charts(), model, row, mapColumn(column), parent, ok);
}

//--

long
CQChartsPlot::
modelInteger(int row, const Column &column, const QModelIndex &parent, int role, bool &ok) const
{
  auto *th = const_cast<Plot *>(this);

  return modelInteger(ModelIndex(th, row, column, parent), role, ok);
}

long
CQChartsPlot::
modelInteger(int row, const Column &column, const QModelIndex &parent, bool &ok) const
{
  auto *th = const_cast<Plot *>(this);

  return modelInteger(ModelIndex(th, row, column, parent), ok);
}

long
CQChartsPlot::
modelInteger(const ModelIndex &ind, int role, bool &ok) const
{
  if (ind.column().isColumn() || ind.column().isCell()) {
    auto c = ind.column();

    if (ind.column().isColumn())
      c.setColumnCol(ind.cellCol());
    else
      c.setCellCol(ind.cellCol());

    const auto &model = this->currentModel();

    return modelInteger(model.data(), ind.row(), c, ind.parent(), role, ok);
  }
  else {
    const auto &model = this->currentModel();

    return modelInteger(model.data(), ind.row(), ind.column(), ind.parent(), role, ok);
  }
}

long
CQChartsPlot::
modelInteger(const ModelIndex &ind, bool &ok) const
{
  if (ind.column().isColumn() || ind.column().isCell()) {
    auto c = ind.column();

    if (ind.column().isColumn())
      c.setColumnCol(ind.cellCol());
    else
      c.setCellCol(ind.cellCol());

    const auto &model = this->currentModel();

    return modelInteger(model.data(), ind.row(), c, ind.parent(), ok);
  }
  else {
    const auto &model = this->currentModel();

    return modelInteger(model.data(), ind.row(), ind.column(), ind.parent(), ok);
  }
}

long
CQChartsPlot::
modelInteger(QAbstractItemModel *model, int row, const Column &column,
             const QModelIndex &parent, int role, bool &ok) const
{
  return CQChartsModelUtil::modelInteger(charts(), model, row, mapColumn(column),
                                         parent, role, ok);
}

long
CQChartsPlot::
modelInteger(QAbstractItemModel *model, int row, const Column &column,
             const QModelIndex &parent, bool &ok) const
{
  if (column.hasRole())
    return CQChartsModelUtil::modelInteger(charts(), model, row, mapColumn(column),
                                           parent, column.role(), ok);

  return CQChartsModelUtil::modelInteger(charts(), model, row, mapColumn(column), parent, ok);
}

//---

CQChartsColumn
CQChartsPlot::
mapColumn(const Column &column) const
{
  if (column.hasRef())
    return getNamedColumn(column.refName());

  return column;
}

//---

std::vector<double>
CQChartsPlot::
modelReals(const ModelIndex &ind, bool &ok) const
{
  return modelReals(ind.row(), ind.column(), ind.parent(), ok);
}

std::vector<double>
CQChartsPlot::
modelReals(int row, const Column &column, const QModelIndex &parent, bool &ok) const
{
  std::vector<double> reals;

  const auto &model = this->currentModel();

  auto var = modelValue(model.data(), row, column, parent, ok);

  if (! ok)
    return reals;

  return CQChartsVariant::toReals(var, ok);
}

//------

QVariant
CQChartsPlot::
modelRootValue(const ModelIndex &ind, bool &ok) const
{
  return modelRootValue(ind.row(), ind.column(), ind.parent(), ok);
}

QVariant
CQChartsPlot::
modelRootValue(const ModelIndex &ind, int role, bool &ok) const
{
  return modelRootValue(ind.row(), ind.column(), ind.parent(), role, ok);
}

QVariant
CQChartsPlot::
modelRootValue(int row, const Column &column, const QModelIndex &parent, int role, bool &ok) const
{
  if (column.column() == 0 && parent.isValid())
    return modelRootValue(parent.row(), column, parent.parent(), role, ok);

  return modelHierValue(row, column, parent, role, ok);
}

QVariant
CQChartsPlot::
modelRootValue(int row, const Column &column, const QModelIndex &parent, bool &ok) const
{
  if (column.column() == 0 && parent.isValid())
    return modelRootValue(parent.row(), column, parent.parent(), ok);

  return modelHierValue(row, column, parent, ok);
}

//------

QVariant
CQChartsPlot::
modelHierValue(const ModelIndex &ind, bool &ok) const
{
  return modelHierValue(ind.row(), ind.column(), ind.parent(), ok);
}

QVariant
CQChartsPlot::
modelHierValue(int row, const Column &column, const QModelIndex &parent, bool &ok) const
{
  auto v = modelValue(row, column, parent, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    auto parent1 = parent;
    int  row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      v = modelValue(row1, column, parent1, ok);
    }
  }

  return v;
}

QVariant
CQChartsPlot::
modelHierValue(int row, const Column &column, const QModelIndex &parent, int role, bool &ok) const
{
  auto v = modelValue(row, column, parent, role, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    auto parent1 = parent;
    int  row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      v = modelValue(row1, column, parent1, role, ok);
    }
  }

  return v;
}

//--

QString
CQChartsPlot::
modelHierString(const ModelIndex &ind, bool &ok) const
{
  return modelHierString(ind.row(), ind.column(), ind.parent(), ok);
}

QString
CQChartsPlot::
modelHierString(int row, const Column &column, const QModelIndex &parent, bool &ok) const
{
  auto var = modelHierValue(row, column, parent, ok);

  if (! ok)
    return QString();

  QString str;

  bool rc = CQChartsVariant::toString(var, str);
  assert(rc);

  return str;
}

QString
CQChartsPlot::
modelHierString(int row, const Column &column, const QModelIndex &parent, int role, bool &ok) const
{
  auto var = modelHierValue(row, column, parent, role, ok);

  if (! ok)
    return QString();

  QString str;

  bool rc = CQChartsVariant::toString(var, str);
  assert(rc);

  return str;
}

//--

#if 0
double
CQChartsPlot::
modelHierReal(int row, const Column &column, const QModelIndex &parent, bool &ok) const
{
  auto var = modelHierValue(row, column, parent, ok);

  if (! ok)
    return 0.0;

  return CQChartsVariant::toReal(var, ok);
}

double
CQChartsPlot::
modelHierReal(int row, const Column &column, const QModelIndex &parent, int role, bool &ok) const
{
  auto var = modelHierValue(row, column, parent, role, ok);

  if (! ok)
    return 0.0;

  return CQChartsVariant::toReal(var, ok);
}
#endif

//--

#if 0
long
CQChartsPlot::
modelHierInteger(int row, const Column &column, const QModelIndex &parent, bool &ok) const
{
  auto var = modelHierValue(row, column, parent, ok);

  if (! ok)
    return 0;

  return CQChartsVariant::toInt(var, ok);
}

long
CQChartsPlot::
modelHierInteger(int row, const Column &column, const QModelIndex &parent, int role, bool &ok) const
{
  auto var = modelHierValue(row, column, parent, role, ok);

  if (! ok)
    return 0;

  return CQChartsVariant::toInt(var, ok);
}
#endif

//------

bool
CQChartsPlot::
isSelectIndex(const QModelIndex &ind, int row, const Column &column,
              const QModelIndex &parent) const
{
  if (column.type() != Column::Type::DATA && column.type() != Column::Type::DATA_INDEX)
    return false;

  return (ind == selectIndex(row, column, parent));
}

QModelIndex
CQChartsPlot::
selectIndex(int row, const Column &column, const QModelIndex &parent) const
{
  if (column.type() != Column::Type::DATA && column.type() != Column::Type::DATA_INDEX)
    return QModelIndex();

  ProxyModels         proxyModels;
  QAbstractItemModel* sourceModel;

  this->proxyModels(proxyModels, sourceModel);

  return sourceModel->index(row, column.column(), parent);
}

void
CQChartsPlot::
beginSelectIndex()
{
  selIndexColumnRows_.clear();
}

void
CQChartsPlot::
addSelectIndex(const ModelIndex &ind)
{
  addSelectIndex(ind.row(), ind.column().column(), ind.parent());
}

void
CQChartsPlot::
addSelectIndex(int row, int column, const QModelIndex &parent)
{
  addSelectIndex(selectIndex(row, Column(column), parent));
}

void
CQChartsPlot::
addSelectIndex(const QModelIndex &ind)
{
  if (! ind.isValid())
    return;

  auto *selPlot = selectionPlot();

  auto ind1 = selPlot->unnormalizeIndex(ind);

  if (! ind1.isValid())
    return;

  // add to map ordered by parent, column, row
  selIndexColumnRows_[ind1.parent()][ind1.column()].insert(ind1.row());
}

void
CQChartsPlot::
endSelectIndex()
{
//auto *model = this->currentModel().data();
//assert(model);

  //---

  auto *selPlot = selectionPlot();

  // build new selection
  QItemSelection optItemSelection;

  // build row range per index column
  for (const auto &p : selIndexColumnRows_) {
    const auto &parent     = p.first;
    const auto &columnRows = p.second;

    // build row range per column
    for (const auto &p1 : columnRows) {
      int         ic   = p1.first;
      const auto &rows = p1.second;

      Column column(ic);

      int startRow = -1;
      int endRow   = -1;

      for (const auto &row : rows) {
        if      (startRow < 0) {
          startRow = row;
          endRow   = row;
        }
        else if (row == endRow + 1) {
          endRow = row;
        }
        else {
          auto ind1 = selPlot->modelIndex(startRow, column, parent);
          auto ind2 = selPlot->modelIndex(endRow  , column, parent);

          optItemSelection.select(ind1, ind2);

          startRow = row;
          endRow   = row;
        }
      }

      if (startRow >= 0) {
        auto ind1 = selPlot->modelIndex(startRow, column, parent);
        auto ind2 = selPlot->modelIndex(endRow  , column, parent);

        optItemSelection.select(ind1, ind2);
      }
    }
  }

  //---

  if (optItemSelection.length()) {
    for (auto &model : models_) {
      auto *modelData = selPlot->getModelData(model);

      if (modelData)
        modelData->select(optItemSelection);
    }
  }
}

//------

double
CQChartsPlot::
logValue(double x, int base) const
{
  if (x >= 1E-6)
    return std::log(x)/std::log(base);
  else
    return CMathUtil::getNaN();
}

double
CQChartsPlot::
expValue(double x, int base) const
{
  if (x <= 709.78271289)
    return std::exp(x*std::log(base));
  else
    return CMathUtil::getNaN();
}

//------

CQChartsGeom::Point
CQChartsPlot::
subPlotToPlot(const Point &p) const
{
  return p;
}

CQChartsGeom::Point
CQChartsPlot::
plotToSubPlot(const Point &p) const
{
  return p;
}

//------

CQChartsGeom::Point
CQChartsPlot::
positionToPlot(const Position &pos) const
{
  auto p  = pos.p();
  auto p1 = p;

  if      (pos.units() == Units::PIXEL)
    p1 = pixelToWindow(p);
  else if (pos.units() == Units::PLOT)
    p1 = p;
  else if (pos.units() == Units::SUB_PLOT)
    p1 = subPlotToPlot(p);
  else if (pos.units() == Units::VIEW)
    p1 = pixelToWindow(view()->windowToPixel(p));
  else if (pos.units() == Units::PERCENT) {
    auto pbbox = (useRawRange_ ? rawDisplayRangeBBox() : displayRangeBBox());

    double refWidth, refHeight;
    percentRefSize(pbbox, refWidth, refHeight);

    p1.setX(pbbox.getXMin() + p.getX()*refWidth /100.0);
    p1.setY(pbbox.getYMin() + p.getY()*refHeight/100.0);
  }
  else if (pos.units() == Units::EM) {
    double x = pixelToWindowWidth (p.getX()*view()->fontEm());
    double y = pixelToWindowHeight(p.getY()*view()->fontEm());

    return Point(x, y);
  }
  else if (pos.units() == Units::EX) {
    double x = pixelToWindowWidth (p.getX()*view()->fontEx());
    double y = pixelToWindowHeight(p.getY()*view()->fontEx());

    return Point(x, y);
  }

  return p1;
}

CQChartsGeom::Point
CQChartsPlot::
positionToPixel(const Position &pos) const
{
  auto p  = pos.p();
  auto p1 = p;

  if      (pos.units() == Units::PIXEL)
    p1 = p;
  else if (pos.units() == Units::PLOT)
    p1 = windowToPixel(p);
  else if (pos.units() == Units::SUB_PLOT)
    p1 = windowToPixel(positionToPlot(pos));
  else if (pos.units() == Units::VIEW)
    p1 = view()->windowToPixel(p);
  else if (pos.units() == Units::PERCENT) {
    auto pbbox = calcPlotPixelRect();

    double refWidth, refHeight;
    percentPixelRefSize(pbbox, refWidth, refHeight);

    p1.setX(pbbox.getXMin() + p.getX()*refWidth /100.0);
    p1.setY(pbbox.getYMin() + p.getY()*refHeight/100.0);
  }
  else if (pos.units() == Units::EM) {
    double x = p.getX()*view()->fontEm();
    double y = p.getY()*view()->fontEm();

    return Point(x, y);
  }
  else if (pos.units() == Units::EX) {
    double x = p.getX()*view()->fontEx();
    double y = p.getY()*view()->fontEx();

    return Point(x, y);
  }
  else {
    assert(false);
  }

  return p1;
}

//------

CQChartsGeom::BBox
CQChartsPlot::
rectToPlot(const Rect &rect) const
{
  auto r  = rect.bbox();
  auto r1 = r;

  if      (rect.units() == Units::PIXEL)
    r1 = pixelToWindow(r);
  else if (rect.units() == Units::PLOT)
    r1 = r;
  else if (rect.units() == Units::SUB_PLOT) {
    auto units = rect.units();
    auto p1 = positionToPlot(Position(rect.bbox().getLL(), units));
    auto p2 = positionToPlot(Position(rect.bbox().getUR(), units));
    r1 = BBox(p1, p2);
  }
  else if (rect.units() == Units::VIEW)
    r1 = pixelToWindow(view()->windowToPixel(r));
  else if (rect.units() == Units::PERCENT) {
    auto pbbox = (useRawRange_ ? rawDisplayRangeBBox() : displayRangeBBox());

    double refWidth, refHeight;
    percentRefSize(pbbox, refWidth, refHeight);

    r1.setXMin(pbbox.getXMin() + r.getXMin()*refWidth /100.0);
    r1.setYMin(pbbox.getYMin() + r.getYMin()*refHeight/100.0);
    r1.setXMax(pbbox.getXMin() + r.getXMax()*refWidth /100.0);
    r1.setYMax(pbbox.getYMin() + r.getYMax()*refHeight/100.0);
  }
  else if (rect.units() == Units::EM) {
    double x1 = pixelToWindowWidth (r.getXMin()*view()->fontEm());
    double y1 = pixelToWindowHeight(r.getYMin()*view()->fontEm());
    double x2 = pixelToWindowWidth (r.getXMax()*view()->fontEm());
    double y2 = pixelToWindowHeight(r.getYMax()*view()->fontEm());

    return BBox(x1, y1, x2, y2);
  }
  else if (rect.units() == Units::EX) {
    double x1 = pixelToWindowWidth (r.getXMin()*view()->fontEx());
    double y1 = pixelToWindowHeight(r.getYMin()*view()->fontEx());
    double x2 = pixelToWindowWidth (r.getXMax()*view()->fontEx());
    double y2 = pixelToWindowHeight(r.getYMax()*view()->fontEx());

    return BBox(x1, y1, x2, y2);
  }
  else {
    assert(false);
  }

  return r1;
}

CQChartsGeom::BBox
CQChartsPlot::
rectToPixel(const Rect &rect) const
{
  auto r  = rect.bbox();
  auto r1 = r;

  if      (rect.units() == Units::PIXEL)
    r1 = r;
  else if (rect.units() == Units::PLOT)
    r1 = windowToPixel(r);
  else if (rect.units() == Units::SUB_PLOT)
    r1 = windowToPixel(rectToPlot(rect));
  else if (rect.units() == Units::VIEW)
    r1 = view()->windowToPixel(r);
  else if (rect.units() == Units::PERCENT) {
    auto pbbox = calcPlotPixelRect();

    double refWidth, refHeight;
    percentPixelRefSize(pbbox, refWidth, refHeight);

    r1.setXMin(pbbox.getXMin() + r.getXMin()*refWidth /100.0);
    r1.setYMin(pbbox.getYMin() + r.getYMin()*refHeight/100.0);
    r1.setXMax(pbbox.getXMin() + r.getXMax()*refWidth /100.0);
    r1.setYMax(pbbox.getYMin() + r.getYMax()*refHeight/100.0);
  }
  else if (rect.units() == Units::EM) {
    double x1 = r.getXMin()*view()->fontEm();
    double y1 = r.getYMin()*view()->fontEm();
    double x2 = r.getXMax()*view()->fontEm();
    double y2 = r.getYMax()*view()->fontEm();

    return BBox(x1, y1, x2, y2);
  }
  else if (rect.units() == Units::EX) {
    double x1 = r.getXMin()*view()->fontEx();
    double y1 = r.getYMin()*view()->fontEx();
    double x2 = r.getXMax()*view()->fontEx();
    double y2 = r.getYMax()*view()->fontEx();

    return BBox(x1, y1, x2, y2);
  }
  else {
    assert(false);
  }

  return r1;
}

CQChartsGeom::Polygon
CQChartsPlot::
polyToPlot(const CQChartsPolygon &poly) const
{
  auto units = poly.units();

  const auto &gpoly = poly.polygon();

  if (units == Units::PLOT)
    return gpoly;

  auto gpoly1 = gpoly;

  for (int i = 0; i < gpoly.size(); ++i)
    gpoly1.setPoint(i, positionToPlot(Position(gpoly.point(i), units)));

  return gpoly1;
}

//------

double
CQChartsPlot::
lengthPlotSize(const Length &len, bool horizontal) const
{
  return (horizontal ? lengthPlotWidth(len) : lengthPlotHeight(len));
}

double
CQChartsPlot::
lengthPlotPerpSize(const Length &len, bool horizontal) const
{
  return (horizontal ? lengthPlotHeight(len) : lengthPlotWidth(len));
}

double
CQChartsPlot::
lengthPlotWidth(const Length &len) const
{
  if (! len.isValid()) return 0.0;

  if      (len.units() == Units::PIXEL)
    return pixelToWindowWidth(len.value());
  else if (len.units() == Units::PLOT)
    return len.value();
  else if (len.units() == Units::VIEW)
    return pixelToWindowWidth(view()->windowToPixelWidth(len.value()));
  else if (len.units() == Units::PERCENT) {
    double refWidth, refHeight;
    percentRefSize(refWidth, refHeight);

    return len.value()*refWidth/100.0;
  }
  else if (len.units() == Units::EM)
    return pixelToWindowWidth(len.value()*view()->fontEm());
  else if (len.units() == Units::EX)
    return pixelToWindowWidth(len.value()*view()->fontEx());
  else {
    assert(false);
  }

  return len.value();
}

double
CQChartsPlot::
lengthPlotHeight(const Length &len) const
{
  if (! len.isValid()) return 0.0;

  if      (len.units() == Units::PIXEL)
    return pixelToWindowHeight(len.value());
  else if (len.units() == Units::PLOT)
    return len.value();
  else if (len.units() == Units::VIEW)
    return pixelToWindowHeight(view()->windowToPixelHeight(len.value()));
  else if (len.units() == Units::PERCENT) {
    double refWidth, refHeight;
    percentRefSize(refWidth, refHeight);

    return len.value()*refHeight/100.0;
  }
  else if (len.units() == Units::EM)
    return pixelToWindowHeight(len.value()*view()->fontEm());
  else if (len.units() == Units::EX)
    return pixelToWindowHeight(len.value()*view()->fontEx());
  else {
    assert(false);
  }

  return len.value();
}

double
CQChartsPlot::
lengthPlotSignedWidth(const Length &len) const
{
  if (! len.isValid()) return 0.0;

  if      (len.units() == Units::PIXEL)
    return pixelToSignedWindowWidth(len.value());
  else if (len.units() == Units::PLOT)
    return len.value();
  else if (len.units() == Units::VIEW)
    return pixelToSignedWindowWidth(view()->windowToPixelWidth(len.value()));
  else if (len.units() == Units::PERCENT) {
    double refWidth, refHeight;
    percentRefSize(refWidth, refHeight);

    return len.value()*refWidth/100.0;
  }
  else if (len.units() == Units::EM)
    return pixelToSignedWindowWidth(len.value()*view()->fontEm());
  else if (len.units() == Units::EX)
    return pixelToSignedWindowWidth(len.value()*view()->fontEx());
  else {
    assert(false);
  }

  return len.value();
}

double
CQChartsPlot::
lengthPlotSignedHeight(const Length &len) const
{
  if (! len.isValid()) return 0.0;

  if      (len.units() == Units::PIXEL)
    return pixelToSignedWindowHeight(len.value());
  else if (len.units() == Units::PLOT)
    return len.value();
  else if (len.units() == Units::VIEW)
    return pixelToSignedWindowHeight(view()->windowToPixelHeight(len.value()));
  else if (len.units() == Units::PERCENT) {
    double refWidth, refHeight;
    percentRefSize(refWidth, refHeight);

    return len.value()*refHeight/100.0;
  }
  else if (len.units() == Units::EM)
    return pixelToSignedWindowHeight(len.value()*view()->fontEm());
  else if (len.units() == Units::EX)
    return pixelToSignedWindowHeight(len.value()*view()->fontEx());
  else {
    assert(false);
  }

  return len.value();
}

double
CQChartsPlot::
lengthPixelSize(const Length &len, bool vertical) const
{
  return (vertical ? lengthPixelWidth(len) : lengthPixelHeight(len));
}

double
CQChartsPlot::
lengthPixelWidth(const Length &len) const
{
  if (! len.isValid()) return 0.0;

  if      (len.units() == Units::PIXEL)
    return len.value();
  else if (len.units() == Units::PLOT)
    return windowToPixelWidth(len.value());
  else if (len.units() == Units::VIEW)
    return view()->windowToPixelWidth(len.value());
  else if (len.units() == Units::PERCENT) {
    double refWidth, refHeight;
    percentPixelRefSize(refWidth, refHeight);

    return len.value()*refWidth/100.0;
  }
  else if (len.units() == Units::EM)
    return len.value()*view()->fontEm();
  else if (len.units() == Units::EX)
    return len.value()*view()->fontEx();
  else {
    assert(false);
  }

  return len.value();
}

double
CQChartsPlot::
lengthPixelHeight(const Length &len) const
{
  if (! len.isValid()) return 0.0;

  if      (len.units() == Units::PIXEL)
    return len.value();
  else if (len.units() == Units::PLOT)
    return windowToPixelHeight(len.value());
  else if (len.units() == Units::VIEW)
    return view()->windowToPixelHeight(len.value());
  else if (len.units() == Units::PERCENT) {
    double refWidth, refHeight;
    percentPixelRefSize(refWidth, refHeight);

    return len.value()*refHeight/100.0;
  }
  else if (len.units() == Units::EM)
    return len.value()*view()->fontEm();
  else if (len.units() == Units::EX)
    return len.value()*view()->fontEx();
  else {
    assert(false);
  }

  return len.value();
}

void
CQChartsPlot::
percentRefSize(double &refWidth, double &refHeight) const
{
  auto pbbox = (useRawRange_ ? rawDisplayRangeBBox() : displayRangeBBox());

  return percentRefSize(pbbox, refWidth, refHeight);
}

void
CQChartsPlot::
percentRefSize(const BBox &pbbox, double &refWidth, double &refHeight) const
{
  refWidth  = pbbox.getWidth ();
  refHeight = pbbox.getHeight();

  if      (refLength_.isSet()) {
    refWidth  = refLength().real();
    refHeight = refWidth;
  }
  else if (refRect_.isSet()) {
    auto bbox = rectToPlot(refRect().rect());

    refWidth  = bbox.getWidth ();
    refHeight = bbox.getHeight();
  }
}

void
CQChartsPlot::
percentPixelRefSize(double &refWidth, double &refHeight) const
{
  auto pbbox = calcPlotPixelRect();

  percentPixelRefSize(pbbox, refWidth, refHeight);
}

void
CQChartsPlot::
percentPixelRefSize(const BBox &pbbox, double &refWidth, double &refHeight) const
{
  refWidth  = pbbox.getWidth ();
  refHeight = pbbox.getHeight();

  if      (refLength_.isSet()) {
    refWidth  = windowToPixelWidth (refLength().real());
    refHeight = windowToPixelHeight(refLength().real());
  }
  else if (refRect_.isSet()) {
    auto bbox = rectToPlot(refRect().rect());

    refWidth  = windowToPixelWidth (bbox.getWidth ());
    refHeight = windowToPixelHeight(bbox.getHeight());
  }
}

//------

double
CQChartsPlot::
windowToViewWidth(double wx) const
{
  double vx1, vy1, vx2, vy2;

  windowToViewI(0.0, 0.0, vx1, vy1);
  windowToViewI(wx , wx , vx2, vy2);

  return fabs(vx2 - vx1);
}

double
CQChartsPlot::
windowToViewHeight(double wy) const
{
  double vx1, vy1, vx2, vy2;

  windowToViewI(0.0, 0.0, vx1, vy1);
  windowToViewI(wy , wy , vx2, vy2);

  return fabs(vy2 - vy1);
}

void
CQChartsPlot::
windowToViewI(double wx, double wy, double &vx, double &vy) const
{
  if (useRawRange_)
    rawDisplayRange().windowToPixel(wx, wy, &vx, &vy);
  else
    displayRange().windowToPixel(wx, wy, &vx, &vy);

  if (isInvertX() || isInvertY()) {
    double ivx, ivy;

    displayRange().invertPixel(vx, vy, ivx, ivy);

    if (isInvertX()) vx = ivx;
    if (isInvertY()) vy = ivy;
  }
}

//------

CQChartsGeom::Point
CQChartsPlot::
pixelToWindow(const Point &w) const
{
  Point p;

  pixelToWindowI(w.x, w.y, p.x, p.y);

  return p;
}

#if 0
void
CQChartsPlot::
pixelToWindowI(const BBox &prect, BBox &wrect) const
{
  wrect = pixelToWindow(prect);
}
#endif

void
CQChartsPlot::
pixelToWindowI(const Point &p, Point &w) const
{
  pixelToWindowI(p.x, p.y, w.x, w.y);
}

CQChartsGeom::BBox
CQChartsPlot::
pixelToWindow(const BBox &wrect) const
{
  double wx1, wy1, wx2, wy2;

  pixelToWindowI(wrect.getXMin(), wrect.getYMin(), wx1, wy1);
  pixelToWindowI(wrect.getXMax(), wrect.getYMax(), wx2, wy2);

  return BBox(wx1, wy1, wx2, wy2);
}

void
CQChartsPlot::
pixelToWindowI(double px, double py, double &wx, double &wy) const
{
  auto pv = view()->pixelToWindow(Point(px, py));

  viewToWindowI(pv.x, pv.y, wx, wy);
}

//-------

void
CQChartsPlot::
viewToWindowI(double vx, double vy, double &wx, double &wy) const
{
  if (isInvertX() || isInvertY()) {
    double ivx, ivy;

    displayRange().invertPixel(vx, vy, ivx, ivy);

    if (isInvertX()) vx = ivx;
    if (isInvertY()) vy = ivy;
  }

  if (useRawRange_)
    rawDisplayRange().pixelToWindow(vx, vy, &wx, &wy);
  else
    displayRange().pixelToWindow(vx, vy, &wx, &wy);
}

double
CQChartsPlot::
viewToWindowWidth(double vx) const
{
  double wx1, wy1, wx2, wy2;

  viewToWindowI(0.0, 0.0, wx1, wy1);
  viewToWindowI(vx , vx , wx2, wy2);

  return fabs(wx2 - wx1);
}

double
CQChartsPlot::
viewToWindowHeight(double vy) const
{
  double wx1, wy1, wx2, wy2;

  viewToWindowI(0.0, 0.0, wx1, wy1);
  viewToWindowI(vy , vy , wx2, wy2);

  return fabs(wy2 - wy1);
}

CQChartsGeom::Point
CQChartsPlot::
windowToPixel(const Point &w) const
{
  Point p;

  windowToPixelI(w.x, w.y, p.x, p.y);

  return p;
}

#if 0
void
CQChartsPlot::
windowToPixelI(const BBox &wrect, BBox &prect) const
{
  prect = windowToPixel(wrect);
}
#endif

CQChartsGeom::BBox
CQChartsPlot::
windowToPixel(const BBox &wrect) const
{
  double px1, py1, px2, py2;

  windowToPixelI(wrect.getXMin(), wrect.getYMin(), px1, py2);
  windowToPixelI(wrect.getXMax(), wrect.getYMax(), px2, py1);

  return BBox(px1, py1, px2, py2);
}

void
CQChartsPlot::
windowToPixelI(const Point &w, Point &p) const
{
  windowToPixelI(w.x, w.y, p.x, p.y);
}

void
CQChartsPlot::
windowToPixelI(double wx, double wy, double &px, double &py) const
{
  double vx, vy;

  windowToViewI(wx, wy, vx, vy);

  auto p = view()->windowToPixel(Point(vx, vy));

  px = p.x;
  py = p.y;
}

CQChartsGeom::Point
CQChartsPlot::
windowToView(const Point &w) const
{
  double vx, vy;

  windowToViewI(w.x, w.y, vx, vy);

  return Point(vx, vy);
}

CQChartsGeom::Point
CQChartsPlot::
viewToWindow(const Point &v) const
{
  double wx, wy;

  viewToWindowI(v.x, v.y, wx, wy);

  return Point(wx, wy);
}

CQChartsGeom::BBox
CQChartsPlot::
windowToView(const BBox &wrect) const
{
  double vx1, vy1, vx2, vy2;

  windowToViewI(wrect.getXMin(), wrect.getYMin(), vx1, vy1);
  windowToViewI(wrect.getXMax(), wrect.getYMax(), vx2, vy2);

  return BBox(vx1, vy1, vx2, vy2);
}

CQChartsGeom::BBox
CQChartsPlot::
viewToWindow(const BBox &vrect) const
{
  double wx1, wy1, wx2, wy2;

  viewToWindowI(vrect.getXMin(), vrect.getYMin(), wx1, wy1);
  viewToWindowI(vrect.getXMax(), vrect.getYMax(), wx2, wy2);

  return BBox(wx1, wy1, wx2, wy2);
}

double
CQChartsPlot::
pixelToSignedWindowWidth(double ww) const
{
  return CMathUtil::sign(ww)*pixelToWindowWidth(ww);
}

double
CQChartsPlot::
pixelToSignedWindowHeight(double wh) const
{
  return CMathUtil::sign(wh)*pixelToWindowHeight(wh);
}

double
CQChartsPlot::
pixelToWindowSize(double ps, bool horizontal) const
{
  return (horizontal ? pixelToWindowWidth(ps) : pixelToWindowHeight(ps));
}

double
CQChartsPlot::
pixelToWindowPerpSize(double ps, bool horizontal) const
{
  return (horizontal ? pixelToWindowHeight(ps) : pixelToWindowWidth(ps));
}

CQChartsGeom::Size
CQChartsPlot::
pixelToWindowSize(const Size &ps) const
{
  return Size(pixelToWindowWidth(ps.width()), pixelToWindowHeight(ps.height()));
}

double
CQChartsPlot::
pixelToWindowWidth(double pw) const
{
  double wx1, wy1, wx2, wy2;

  pixelToWindowI( 0, 0, wx1, wy1);
  pixelToWindowI(pw, 0, wx2, wy2);

  return std::abs(wx2 - wx1);
}

double
CQChartsPlot::
pixelToWindowHeight(double ph) const
{
  double wx1, wy1, wx2, wy2;

  pixelToWindowI(0, 0 , wx1, wy1);
  pixelToWindowI(0, ph, wx2, wy2);

  return std::abs(wy2 - wy1);
}

double
CQChartsPlot::
windowToSignedPixelWidth(double ww) const
{
  return CMathUtil::sign(ww)*windowToPixelWidth(ww);
}

double
CQChartsPlot::
windowToSignedPixelHeight(double wh) const
{
  return -CMathUtil::sign(wh)*windowToPixelHeight(wh);
}

double
CQChartsPlot::
windowToPixelSize(double ws, bool horizontal) const
{
  return (horizontal ? windowToPixelWidth(ws) : windowToPixelHeight(ws));
}

double
CQChartsPlot::
windowToPixelPerpSize(double ws, bool horizontal) const
{
  return (horizontal ? windowToPixelHeight(ws) : windowToPixelWidth(ws));
}

CQChartsGeom::Size
CQChartsPlot::
windowToPixelSize(const Size &s) const
{
  return Size(windowToPixelWidth(s.width()), windowToPixelHeight(s.height()));
}

double
CQChartsPlot::
windowToPixelWidth(double ww) const
{
  double px1, py1, px2, py2;

  windowToPixelI( 0, 0, px1, py1);
  windowToPixelI(ww, 0, px2, py2);

  return std::abs(px2 - px1);
}

double
CQChartsPlot::
windowToPixelHeight(double wh) const
{
  double px1, py1, px2, py2;

  windowToPixelI(0, 0 , px1, py1);
  windowToPixelI(0, wh, px2, py2);

  return std::abs(py2 - py1);
}

CQChartsGeom::Polygon
CQChartsPlot::
windowToPixel(const Polygon &poly) const
{
  Polygon ppoly;

  int np = poly.size();

  for (int i = 0; i < np; ++i)
    ppoly.addPoint(windowToPixel(poly.point(i)));

  return ppoly;
}

QPainterPath
CQChartsPlot::
windowToPixel(const QPainterPath &path) const
{
  class PathVisitor : public CQChartsDrawUtil::PathVisitor {
   public:
    PathVisitor(const Plot *plot) :
     plot_(plot) {
    }

    void moveTo(const Point &p) override {
      auto pp = plot_->windowToPixel(p);

      path_.moveTo(pp.qpoint());
    }

    void lineTo(const Point &p) override {
      auto pp = plot_->windowToPixel(p);

      path_.lineTo(pp.qpoint());
    }

    void quadTo(const Point &p1, const Point &p2) override {
      auto pp1 = plot_->windowToPixel(p1);
      auto pp2 = plot_->windowToPixel(p2);

      path_.quadTo(pp1.qpoint(), pp2.qpoint());
    }

    void curveTo(const Point &p1, const Point &p2, const Point &p3) override {
      auto pp1 = plot_->windowToPixel(p1);
      auto pp2 = plot_->windowToPixel(p2);
      auto pp3 = plot_->windowToPixel(p3);

      path_.cubicTo(pp1.qpoint(), pp2.qpoint(), pp3.qpoint());
    }

    const QPainterPath &path() const { return path_; }

   private:
    const Plot*  plot_ { nullptr };
    QPainterPath path_;
  };

  PathVisitor visitor(this);

  CQChartsDrawUtil::visitPath(path, visitor);

  return visitor.path();
}

//---

void
CQChartsPlot::
zoomedPixelSize(double px, double py, double &px1, double &py1) const
{
  useRawRange_ = true;

  double wx = pixelToWindowWidth (px);
  double wy = pixelToWindowHeight(py);

  useRawRange_ = false;

  px1 = windowToPixelWidth (wx);
  py1 = windowToPixelHeight(wy);
}

//------

void
CQChartsPlot::
plotSymbolSize(const Length &xs, const Length &ys, double &sx, double &sy, bool scale) const
{
  plotSymbolSize1(xs, ys, sx, sy, scale);
}

void
CQChartsPlot::
plotSymbolSize(const Length &xs, const Length &ys, double &sx, double &sy) const
{
  plotSymbolSize1(xs, ys, sx, sy, isScaleSymbolSize());
}

void
CQChartsPlot::
plotSymbolSize1(const Length &xs, const Length &ys, double &sx, double &sy, bool scale) const
{
  useRawRange_ = scale;

  sx = lengthPlotWidth (xs);
  sy = lengthPlotHeight(ys);

  useRawRange_ = false;
}

void
CQChartsPlot::
plotSymbolSize(const Length &s, double &sx, double &sy, bool scale,
               const Qt::Orientation &dir) const
{
  plotSymbolSize1(s, sx, sy, scale, dir);
}

void
CQChartsPlot::
plotSymbolSize(const Length &s, double &sx, double &sy, const Qt::Orientation &dir) const
{
  plotSymbolSize1(s, sx, sy, isScaleSymbolSize(), dir);
}

void
CQChartsPlot::
plotSymbolSize1(const Length &s, double &sx, double &sy, bool scale,
                const Qt::Orientation &dir) const
{
  useRawRange_ = scale;

  if (dir == Qt::Horizontal) {
    sx = lengthPlotWidth(s);
    sy = pixelToWindowHeight(windowToPixelWidth(sx));
  }
  else {
    sy = lengthPlotHeight(s);
    sx = pixelToWindowHeight(windowToPixelWidth(sy));
  }

  useRawRange_ = false;
}

void
CQChartsPlot::
pixelSymbolSize(const Length &s, double &sx, double &sy, bool scale,
                const Qt::Orientation &dir) const
{
  pixelSymbolSize1(s, sx, sy, scale, dir);
}

void
CQChartsPlot::
pixelSymbolSize(const Length &s, double &sx, double &sy, const Qt::Orientation &dir) const
{
  pixelSymbolSize1(s, sx, sy, isScaleSymbolSize(), dir);
}

void
CQChartsPlot::
pixelSymbolSize1(const Length &s, double &sx, double &sy, bool scale,
                 const Qt::Orientation &dir) const
{
  double sx1, sy1;

  plotSymbolSize1(s, sx1, sy1, scale, dir);

  if (dir == Qt::Horizontal) {
    sx = limitSymbolSize(lengthPixelWidth (Length::plot(sx1)));
    sy = sx;
  }
  else {
    sx = limitSymbolSize(lengthPixelHeight(Length::plot(sx1)));
    sy = sx;
  }

#if 0
  if (dir == Qt::Horizontal) {
    sx = limitSymbolSize(lengthPixelWidth(s));
    sy = sx;
  }
  else {
    sy = limitSymbolSize(lengthPixelHeight(s));
    sx = sy;
  }
#endif
}

//------

double
CQChartsPlot::
limitSymbolSize(double s) const
{
  // ensure not a crazy number : TODO: property for limits
  return CMathUtil::clamp(s, 1.0, charts()->maxSymbolSize());
}

double
CQChartsPlot::
limitFontSize(double s) const
{
  // ensure not a crazy number : TODO: property for limits
  return CMathUtil::clamp(s, 1.0, charts()->maxFontSize());
}

double
CQChartsPlot::
limitLineWidth(double w) const
{
  // ensure not a crazy number : TODO: property for limits
  return CMathUtil::clamp(w, 0.0, charts()->maxLineWidth());
}

//------

bool
CQChartsPlot::
setParameter(PlotParameter *param, const QVariant &value)
{
  return CQUtil::setProperty(this, param->propName(), value);
}

bool
CQChartsPlot::
getParameter(PlotParameter *param, QVariant &value) const
{
  return CQUtil::getProperty(this, param->propName(), value);
}

bool
CQChartsPlot::
contains(const Point &p) const
{
  return dataRange().inside(p);
}

//------

void
CQChartsPlot::
write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
      const QString & /*viewVarName*/) const
{
  auto plotName = [&]() {
    return (plotVarName != "" ? plotVarName : "plot");
  };

  auto modelName = [&]() {
    return (modelVarName != "" ? modelVarName : "model");
  };

  os << "set " << plotName().toStdString();
  os << " [create_charts_plot -model $" << modelName().toStdString() <<
        " -type " << type_->name().toStdString();

  //---

  // add columns
  QVariantList columnsStrs;
//QStringList  parametersStrs;
  QStringList  parameterPropPaths;

  for (const auto &param : type()->parameters()) {
    QString defStr;

    if (! CQChartsVariant::toString(param->defValue(), defStr))
      defStr.clear();

    QVariant value;

    if (! getParameter(param, value))
      continue;

    QString str;

    if (! CQChartsVariant::toString(value, str))
      str.clear();

    if (str == defStr)
      continue;

    QStringList strs1;

    strs1 << param->name() << str;

    if (param->type() == PlotParameter::Type::COLUMN ||
        param->type() == PlotParameter::Type::COLUMN_LIST) {
      columnsStrs += strs1;
    }
    else {
      //parametersStrs += "{" + CQTcl::mergeList(strs1) + "}";
      auto propPath = param->propPath();

      if (propPath == "") {
        if (! propertyModel()->nameToPath(this, param->propName(), propPath))
          continue;
      }

      if (propPath != "")
        parameterPropPaths << propPath;
    }
  }

  if (columnsStrs.length())
    os << " -columns {" << CQTclUtil::variantListToString(columnsStrs).toStdString() + "}";

#if 0
  for (int i = 0; i < parametersStrs.length(); ++i)
    os << " -parameter " << parametersStrs[i].toStdString();
#endif

  if (titleStr() != "")
    os << " -title {" << titleStr().toStdString() + "}";

  //---

  os << "]\n";

  //---

  // get changed name values
  CQPropertyViewModel::NameValues nameValues;

  propertyModel()->getChangedNameValues(this, nameValues, /*tcl*/true);

  // add changed parameter values
  for (auto &propPath : parameterPropPaths) {
    QVariant value;

    bool rc = getTclProperty(propPath, value);
    assert(rc);

    nameValues[propPath] = value;
  }

  //---

  if (! nameValues.empty())
    os << "\n";

  for (const auto &nv : nameValues) {
    QString str;

    if (! CQChartsVariant::toString(nv.second, str))
      str.clear();

    os << "set_charts_property -plot $" << plotName().toStdString();

    os << " -name " << nv.first.toStdString() << " -value {" << str.toStdString() << "}\n";
  }

  //---

  // xaxis
  if (xAxis()) xAxis()->write(propertyModel(), plotName(), os);
  if (yAxis()) yAxis()->write(propertyModel(), plotName(), os);

  // key
  if (key()) key()->write(propertyModel(), plotName(), os);

  // title
  if (title()) title()->write(propertyModel(), plotName(), os);
}

//---

#ifdef CQCHARTS_MODULE_SHLIB
bool
CQChartsPlot::
setPlotModule(const QString &name)
{
  auto errorMsg = [](const QString &msg) {
    std::cerr << msg.toStdString() << "\n";
    return false;
  };

  //---

  // <module> <args>
  QString     moduleName;
  QStringList moduleArgs;

  QStringList strs;

  if (CQTcl::splitList(name, strs) && strs.length() > 0) {
    moduleName = strs[0];

    for (int i = 1; i < strs.length(); ++i)
      moduleArgs.push_back(strs[i]);
  }
  else
    moduleName = name;

  //---

  delete moduleData_.shlib;

  moduleData_.name      = moduleName;
  moduleData_.initProc  = nullptr;
  moduleData_.drawProc  = nullptr;
  moduleData_.eventProc = nullptr;

  auto libname = CQChartsEnv::getString("CQCHARTS_MODULE_PATH", "./modules");

  libname += "/" + moduleName + ".so";

  moduleData_.shlib = new CShLib(libname.toLatin1().constData());

  if (! moduleData_.shlib->open())
    return errorMsg(QString("Failed to Open Library %1").arg(libname));

  //---

  auto initName    = moduleName + "_charts_init";
  auto drawName    = moduleName + "_charts_draw";
  auto eventName   = moduleName + "_charts_event";
  auto getDataName = moduleName + "_charts_get_data";
  auto setDataName = moduleName + "_charts_set_data";

  auto getProc = [&](const QString &procName, auto *proc) {
    CShLibProc shProc;
    if (! moduleData_.shlib->getProc(procName.toLatin1().constData(), &shProc))
      return errorMsg(QString("Failed to Find Proc %1").arg(procName));
    *proc = reinterpret_cast<decltype(*proc)>(shProc);
    return true;
  };

  if (! getProc(initName, &moduleData_.initProc))
    return false;

  if (! getProc(drawName, &moduleData_.drawProc))
    return false;

  if (! getProc(eventName, &moduleData_.eventProc))
    return false;

  if (! getProc(getDataName, &moduleData_.getDataProc))
    return false;

  if (! getProc(setDataName, &moduleData_.setDataProc))
    return false;

  //---

  auto str = moduleArgs.join(" ");

  moduleData_.id = (*moduleData_.initProc)(reinterpret_cast<void *>(str.toLatin1().data()));

  //---

  CQChartsModuleGetData getData;

  getData.id     = moduleData_.id;
  getData.buffer = nullptr;

  (*moduleData_.getDataProc)(&getData);

  if (getData.buffer) {
    std::cerr << "buffer: " << getData.buffer << "\n";

    // list of named value sets
    QStringList strs;

    if (CQTcl::splitList(getData.buffer, strs) && strs.length() > 0) {
      // named value set
      for (const auto &str : strs) {
        std::cerr << "named value set: " << str.toStdString() << "\n";

        // named values
        QStringList strs1;

        if (CQTcl::splitList(str, strs1) && strs1.length() > 0) {
          std::cerr << "named values" << strs1[0].toStdString() << "\n";

          // properties name/types
          if (strs1[0] == "properties" && strs1.length() > 1) {
            QStringList strs2;

            if (CQTcl::splitList(strs1[1], strs2) && strs2.length() > 0) {
              for (const auto &str2 : strs2) {
                std::cerr << "name/type: " << str2.toStdString() << "\n";

                QStringList strs3;

                if (CQTcl::splitList(str2, strs3) && strs3.length() > 1) {
                  auto name = strs3[0];
                  auto type = strs3[1];

                  QVariant value;

                  if (strs3.length() > 2)
                    value = strs3[2];

                  std::cerr << "name=" << name.toStdString() << " " <<
                               "type=" << type.toStdString() << " " <<
                               "value=" << value.toString().toStdString() << "\n";

                  moduleData_.properties[name] = ModulePropertyData(type, value);
                }
              }
            }
          }
        }
      }
    }
  }

  Q_EMIT plotModuleChanged();

  return true;
}
#endif

#ifdef CQCHARTS_MODULE_SHLIB
void
CQChartsPlot::
setModuleProperty(const QString &name, const QString &type, const QVariant &value)
{
  moduleData_.properties[name] = ModulePropertyData(type, value);

  CQChartsModuleGetData setData;

  setData.id = moduleData_.id;

  QString str;

  str += "{properties {";

  for (const auto &p : moduleData_.properties) {
    str += "{" + p.first + " " + p.second.first + " " + p.second.second.toString() + "}";
  }

  str += "}}";

  setData.buffer = str.toLatin1().data();

  (*moduleData_.setDataProc)(&setData);
}
#endif
