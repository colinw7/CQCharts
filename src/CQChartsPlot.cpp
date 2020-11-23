#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
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
#include <CQChartsDrawUtil.h>
#include <CQChartsHtml.h>
#include <CQChartsEnv.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>

#include <CQChartsPlotControlWidgets.h>
#include <CQChartsModelViewHolder.h>
#include <CQChartsModelDetailsTable.h>
#include <CQChartsPlotPropertyEdit.h>
#include <CQChartsModelColumnDataControl.h>
#include <CQChartsModelExprControl.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColors.h>
#include <CQColorsTheme.h>
#include <CQColorsPalette.h>
#include <CQThreadObject.h>
#include <CQPerfMonitor.h>
#include <CQUtil.h>
#include <CQTclUtil.h>

#include <CMathUtil.h>
#include <CMathRound.h>

#include <QApplication>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>
#include <QTextBrowser>
#include <QPainter>

//------

CQChartsPlot::
CQChartsPlot(View *view, PlotType *type, const ModelP &model) :
 CQChartsObj(view->charts()),
 CQChartsObjPlotShapeData<CQChartsPlot>(this),
 CQChartsObjDataShapeData<CQChartsPlot>(this),
 CQChartsObjFitShapeData <CQChartsPlot>(this),
 view_(view), type_(type), model_(model)
{
  init();
}

CQChartsPlot::
~CQChartsPlot()
{
  term();
}

//---

void
CQChartsPlot::
init()
{
  NoUpdate noUpdate(this);

  //---

  propertyModel_ = new CQPropertyViewModel;

  //---

  preview_       = CQChartsEnv::getInt ("CQ_CHARTS_PLOT_PREVIEW"  , preview_);
  sequential_    = CQChartsEnv::getBool("CQ_CHARTS_SEQUENTIAL"    , sequential_); // TODO: remove
  queueUpdate_   = CQChartsEnv::getBool("CQ_CHARTS_PLOT_QUEUE"    , queueUpdate_);
  bufferSymbols_ = CQChartsEnv::getInt ("CQ_CHARTS_BUFFER_SYMBOLS", bufferSymbols_);

  displayRange_ = new DisplayRange();

  displayRange_->setPixelAdjust(0.0);

  bool objTreeWait = CQChartsEnv::getBool("CQ_CHARTS_OBJ_TREE_WAIT", false);

  objTreeData_.tree = new CQChartsPlotObjTree(this, objTreeWait);

  animateData_.tickLen = CQChartsEnv::getInt("CQ_CHARTS_TICK_LEN", animateData_.tickLen);

  debugUpdate_   = CQChartsEnv::getBool("CQ_CHARTS_DEBUG_UPDATE"   , debugUpdate_  );
  debugQuadTree_ = CQChartsEnv::getBool("CQ_CHARTS_DEBUG_QUAD_TREE", debugQuadTree_);

  editHandles_ = new EditHandles(view());

  //--

  // plot, data, fit background
  setPlotFilled(true ); setPlotStroked(false);
  setDataFilled(true ); setDataStroked(false);
  setFitFilled (false); setFitStroked (false);

  setDataClip(true);

  setPlotFillColor(Color(Color::Type::INTERFACE_VALUE, 0.00));
  setDataFillColor(Color(Color::Type::INTERFACE_VALUE, 0.12)); // #e5ecf6 (BLEND_INTERFACE)
  setFitFillColor (Color(Color::Type::INTERFACE_VALUE, 0.08));

  //--

  double vr = View::viewportRange();

  viewBBox_      = BBox(0, 0, vr, vr);
  innerViewBBox_ = viewBBox_;

  innerMargin_ = PlotMargin(Length("0P" ), Length("0P" ), Length("0P" ), Length("0P" ));
  outerMargin_ = PlotMargin(Length("10%"), Length("10%"), Length("10%"), Length("10%"));
  fitMargin_   = PlotMargin(Length("1%" ), Length("1%" ), Length("1%" ), Length("1%" ));

  displayRange_->setPixelAdjust(0.0);

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

  connectModel();

  //---

  ++updatesData_.stateFlag[UpdateState::UPDATE_RANGE    ];
  ++updatesData_.stateFlag[UpdateState::UPDATE_OBJS     ];
  ++updatesData_.stateFlag[UpdateState::UPDATE_DRAW_OBJS];

  updateData_.rangeThread = new ThreadObj("updateRange");
  updateData_.objsThread  = new ThreadObj("updateObjs" );
  updateData_.drawThread  = new ThreadObj("drawObjs"   );

  updateData_.rangeThread->setDebug(debugUpdate_);
  updateData_.objsThread ->setDebug(debugUpdate_);
  updateData_.drawThread ->setDebug(debugUpdate_);

  startThreadTimer();
}

void
CQChartsPlot::
term()
{
  CQChartsPlot::clearPlotObjects();

  for (auto &layer : layers_)
    delete layer.second;

  for (auto &buffer : buffers_)
    delete buffer.second;

  for (auto &annotation : annotations())
    delete annotation;

  delete objTreeData_.tree;

  delete propertyModel_;

  delete displayRange_;

  delete titleObj_;
  delete keyObj_;
  delete xAxis_;
  delete yAxis_;

  delete editHandles_;

  delete animateData_.timer;

  delete updateData_.timer;
  delete updateData_.rangeThread;
  delete updateData_.objsThread;
  delete updateData_.drawThread;
}

//---

QString
CQChartsPlot::
viewId() const
{
  return view()->id();
}

QString
CQChartsPlot::
typeStr() const
{
  return type_->name();
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

//---

void
CQChartsPlot::
startThreadTimer()
{
  if (isSequential())
    return;

  if (parentPlot())
    return;

  if (isOverlay() && ! isFirstPlot())
    return;

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
  if (updateData_.timer) {
    if (updateData_.timer->isActive())
      updateData_.timer->stop();
  }
}

//---

void
CQChartsPlot::
setModel(const ModelP &model)
{
  disconnectModel();

  model_ = model;

  connectModel();

  updateRangeAndObjs();

  emit modelChanged();
}

void
CQChartsPlot::
connectModel()
{
  connectDisconnectModel(true);
}

void
CQChartsPlot::
disconnectModel()
{
  connectDisconnectModel(false);
}

void
CQChartsPlot::
connectDisconnectModel(bool isConnect)
{
  if (! model_.data())
    return;

  auto *modelData = getModelData();

  //---

  auto connectDisconnect = [&](bool b, QObject *obj, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(b, obj, from, this, to);
  };

  //---

  if (modelData) {
    if (isConnect) {
      if (! modelData->name().length() && this->hasId()) {
        charts()->setModelName(modelData, this->id());

        modelNameSet_ = true;
      }
      else
        modelNameSet_ = false;
    }
    else {
      if (modelNameSet_) {
        charts()->setModelName(modelData, this->id());

        modelNameSet_ = false;
      }
    }

    connectDisconnect(isConnect, modelData, SIGNAL(modelChanged()),
                      SLOT(modelChangedSlot()));

    connectDisconnect(isConnect, modelData, SIGNAL(currentModelChanged()),
                      SLOT(currentModelChangedSlot()));

    connectDisconnect(isConnect, modelData, SIGNAL(selectionChanged(QItemSelectionModel *)),
                      SLOT(selectionSlot(QItemSelectionModel *)));
  }
  else {
    modelNameSet_ = false;

    // TODO: on connect, check if model uses changed columns
    //int column1 = tl.column();
    //int column2 = br.column();

    connectDisconnect(isConnect,
                      model_.data(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
                      SLOT(modelChangedSlot()));

    connectDisconnect(isConnect, model_.data(), SIGNAL(layoutChanged()),
                      SLOT(modelChangedSlot()));
    connectDisconnect(isConnect, model_.data(), SIGNAL(modelReset()),
                      SLOT(modelChangedSlot()));

    connectDisconnect(isConnect, model_.data(), SIGNAL(rowsInserted(QModelIndex, int, int)),
                      SLOT(modelChangedSlot()));
    connectDisconnect(isConnect, model_.data(), SIGNAL(rowsRemoved(QModelIndex, int, int)),
                      SLOT(modelChangedSlot()));
    connectDisconnect(isConnect, model_.data(), SIGNAL(columnsInserted(QModelIndex, int, int)),
                      SLOT(modelChangedSlot()));
    connectDisconnect(isConnect, model_.data(), SIGNAL(columnsRemoved(QModelIndex, int, int)),
                      SLOT(modelChangedSlot()));
  }
}

//---

void
CQChartsPlot::
startAnimateTimer()
{
  if (! animateData_.timer) {
    animateData_.timer = new QTimer;

    connect(animateData_.timer, SIGNAL(timeout()), this, SLOT(animateSlot()));
  }

  animateData_.timer->start(animateData_.tickLen);
}

void
CQChartsPlot::
stopAnimateTimer()
{
  delete animateData_.timer;

  animateData_.timer = nullptr;
}

void
CQChartsPlot::
animateSlot()
{
  interruptRange();
//interruptDraw();

  animateStep();
}

//---

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

  if (! modelData)
    return;

  setModel(modelData->currentModel());
}

//---

void
CQChartsPlot::
selectionSlot(QItemSelectionModel *sm)
{
  // get selected (normalized) indices from selection model
  PlotObj::Indices selectIndices;

  getSelectIndices(sm, selectIndices);

  //---

  startSelection();

  // deselect all objects
  deselectAllObjs();

  // select objects with matching indices
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

CQCharts *
CQChartsPlot::
charts() const
{
  return view()->charts();
}

QString
CQChartsPlot::
typeName() const
{
  return type()->name();
}

QString
CQChartsPlot::
pathId() const
{
  return view()->id() + "|" + id();
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

  return isVisible();
}

void
CQChartsPlot::
setVisible(bool b)
{
  CQChartsUtil::testAndSet(visible_, b, [&]() {
    if (! isVisible()) {
      if (view()->currentPlot() == this)
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

void
CQChartsPlot::
setUpdatesEnabled(bool b, bool update)
{
  if (b) {
    assert(updatesData_.enabled > 0);

    --updatesData_.enabled;

    if (isUpdatesEnabled()) {
      if (update) {
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
      }

      updatesData_.reset();
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

void
CQChartsPlot::
updateRange()
{
  if (! isUpdatesEnabled()) {
    execUpdateRange();
    return;
  }

  if (parentPlot())
    return parentPlot()->updateRange();

  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->updateRange();

  if (isQueueUpdate()) {
    if (debugUpdate_)
      std::cerr << "updateRange : " << id().toStdString() << "\n";

    ++updatesData_.stateFlag[UpdateState::UPDATE_RANGE    ];
    ++updatesData_.stateFlag[UpdateState::UPDATE_DRAW_OBJS];

    startThreadTimer();
  }
  else {
    execUpdateRange();
  }
}

void
CQChartsPlot::
updateRangeAndObjs()
{
  if (! isUpdatesEnabled()) {
    execUpdateRangeAndObjs();
    return;
  }

  if (parentPlot())
    return parentPlot()->updateRangeAndObjs();

  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->updateRangeAndObjs();

  if (isQueueUpdate()) {
    if (debugUpdate_)
      std::cerr << "updateRangeAndObjs : " << id().toStdString() << "\n";

    ++updatesData_.stateFlag[UpdateState::UPDATE_RANGE    ];
    ++updatesData_.stateFlag[UpdateState::UPDATE_OBJS     ];
    ++updatesData_.stateFlag[UpdateState::UPDATE_DRAW_OBJS];

    startThreadTimer();
  }
  else {
    execUpdateRangeAndObjs();
  }
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
  if (! isUpdatesEnabled()) {
    execUpdateObjs();
    return;
  }

  if (parentPlot())
    return parentPlot()->updateObjs();

  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->updateObjs();

  if (isQueueUpdate()) {
    if (debugUpdate_)
      std::cerr << "updateObjs : " << id().toStdString() << "\n";

    ++updatesData_.stateFlag[UpdateState::UPDATE_OBJS     ];
    ++updatesData_.stateFlag[UpdateState::UPDATE_DRAW_OBJS];

    startThreadTimer();
  }
  else {
    execUpdateObjs();
  }
}

//------

void
CQChartsPlot::
applyVisibleFilter()
{
  if (visibleFilterStr().length()) {
    auto *expr = new CQChartsModelExprMatch;

    expr->setModel(model().data());

    expr->initColumns();

    expr->initMatch(visibleFilterStr());

    for (auto &plotObj : plotObjects()) {
      auto ind = plotObj->modelInd();

      bool ok;

      bool visible = expr->match(ind, ok);
      if (!ok) visible = true;

      plotObj->setVisible(visible);
    }

    delete expr;
  }
  else {
    for (auto &plotObj : plotObjects())
      plotObj->setVisible(true);
  }
}

//------

void
CQChartsPlot::
drawBackground()
{
  if (! isUpdatesEnabled())
    return;

  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->drawBackground();

  if (isQueueUpdate()) {
    if (debugUpdate_)
      std::cerr << "drawBackground : " << id().toStdString() << "\n";

    ++updatesData_.stateFlag[UpdateState::UPDATE_DRAW_BACKGROUND];

    startThreadTimer();
  }
  else {
    invalidateLayer(Buffer::Type::BACKGROUND);
  }
}

void
CQChartsPlot::
drawForeground()
{
  if (! isUpdatesEnabled())
    return;

  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->drawForeground();

  if (isQueueUpdate()) {
    if (debugUpdate_)
      std::cerr << "drawForeground : " << id().toStdString() << "\n";

    ++updatesData_.stateFlag[UpdateState::UPDATE_DRAW_FOREGROUND];

    startThreadTimer();
  }
  else {
    invalidateLayer(Buffer::Type::FOREGROUND);

    invalidateOverlay();
  }
}

void
CQChartsPlot::
drawObjs()
{
  if (! isUpdatesEnabled())
    return;

  if (parentPlot())
    return parentPlot()->drawObjs();

  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->drawObjs();

  if (isQueueUpdate()) {
    if (debugUpdate_)
      std::cerr << "drawObjs : " << id().toStdString() << "\n";

    ++updatesData_.stateFlag[UpdateState::UPDATE_DRAW_OBJS];

    startThreadTimer();
  }
  else
    invalidateLayers();
}

//---

void
CQChartsPlot::
writeScript(ScriptPaintDevice *device) const
{
  std::string plotId = "plot_" + this->id().toStdString();

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
      auto objId  = QString("obj_") + plotId.c_str() + "_" + plotObj->id();
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

  // background parts (bg axis, bg key, bg annotations, backgroun)
  if (hasBackgroundLayer()) {
    os << "\n"; drawBackgroundLayer(device);
  }

  bool bgAxes = hasGroupedBgAxes();
  bool bgKey  = hasGroupedBgKey();

  if (bgAxes) { os << "\n"; os << "  this.drawBgAxis();\n"; }
  if (bgKey ) { os << "\n"; os << "  this.drawBgKey ();\n"; }

  if (hasGroupedAnnotations(Layer::Type::BG_ANNOTATION)) {
    os << "\n"; os << "  this.drawBgAnnotations();\n";
  }

  //---

  // middle parts (objects)
  os << "\n"; os << "  this.drawObjs();\n";

  //---

  // foreground parts (fg annotations, fg axis, fg key, title, foreground)
  if (hasGroupedAnnotations(Layer::Type::FG_ANNOTATION)) {
    os << "\n"; os << "  this.drawFgAnnotations();\n";
  }

  bool fgAxes = hasGroupedFgAxes();
  bool fgKey  = hasGroupedFgKey();

  if (fgAxes) { os << "\n"; os << "  this.drawFgAxis();\n"; }
  if (fgKey ) { os << "\n"; os << "  this.drawFgKey ();\n"; }

  bool title = hasTitle();

  if (title) {
    os << "\n"; os << "  this.drawTitle();\n";
  }

  if (this->hasForeground()) {
    os << "\n"; execDrawForeground(device);
  }

  //---

  os << "}\n";

  //---

  // plot object procs
  for (const auto &plotObj : plotObjects()) {
    if (! plotObj->isVisible()) continue;

    if (plotObj->detailHint() == PlotObj::DetailHint::MAJOR) {
      auto objId  = QString("obj_") + plotId.c_str() + "_" + plotObj->id();
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
    os << "\n";
    os << "Charts_" << plotId << ".prototype.drawBgAnnotations = function() {\n";
    drawGroupedAnnotations(device, Layer::Type::BG_ANNOTATION);
    os << "}\n";
  }

  //---

  // draw objects proc
  device->resetData();

  os << "\n";
  os << "Charts_" << plotId << ".prototype.drawObjs = function() {\n";

  for (const auto &plotObj : plotObjects()) {
    if (! plotObj->isVisible()) continue;

    if (plotObj->detailHint() == PlotObj::DetailHint::MAJOR) {
      auto objId  = QString("obj_") + plotId.c_str() + "_" + plotObj->id();
      auto objStr = device->encodeObjId(objId).toStdString();

      os << "  this." << objStr << ".draw();\n";
    }
    else {
      plotObj->drawBg(device);
      plotObj->draw  (device);
      plotObj->drawFg(device);
    }
  }

  drawDeviceParts(device);

  os << "}\n";

  //---

  // draw annotations proc
  if (hasGroupedAnnotations(Layer::Type::FG_ANNOTATION)) {
    os << "\n";
    os << "Charts_" << plotId << ".prototype.drawFgAnnotations = function() {\n";
    drawGroupedAnnotations(device, Layer::Type::FG_ANNOTATION);
    os << "}\n";
  }

  //---

  // draw axes procs
  if (bgAxes) {
    device->resetData();

    os << "\n";
    os << "Charts_" << plotId << ".prototype.drawBgAxis = function() {\n";
    drawGroupedBgAxes(device);
    os << "}\n";
  }

  if (fgAxes) {
    device->resetData();

    os << "\n";
    os << "Charts_" << plotId << ".prototype.drawFgAxis = function() {\n";
    drawGroupedFgAxes(device);
    os << "}\n";
  }

  //---

  // draw key procs
  if (bgKey) {
    device->resetData();

    os << "\n";
    os << "Charts_" << plotId << ".prototype.drawBgKey = function() {\n";
    drawBgKey(device);
    os << "}\n";
  }

  if (fgKey) {
    device->resetData();

    os << "\n";
    os << "Charts_" << plotId << ".prototype.drawFgKey = function() {\n";
    drawFgKey(device);
    os << "}\n";
  }

  //---

  // draw title proc
  if (title) {
    device->resetData();

    os << "\n";
    os << "Charts_" << plotId << ".prototype.drawTitle = function() {\n";
    drawTitle(device);
    os << "}\n";
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
  auto plotId = "plot_" + this->id();

  SVGPaintDevice::GroupData groupData;

  groupData.visible = isVisible();

  device->startGroup(plotId, groupData);

  if (hasBackgroundLayer()) {
    drawBackgroundLayer(device);
  }

  if (hasForeground()) {
    execDrawBackground(device);
  }

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

  if (hasGroupedAnnotations(Layer::Type::BG_ANNOTATION)) {
    drawGroupedAnnotations(device, Layer::Type::BG_ANNOTATION);
  }

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

  if (hasForeground()) {
    execDrawForeground(device);
  }

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

        processOverlayPlots([&](const CQChartsPlot *plot) {
          device->setPlot(const_cast<CQChartsPlot *>(plot));

          for (auto &annotation : plot->annotations()) {
            if (isAnnotationLayer(annotation, layerType))
              annotation->writeHtml(device);
          }
        });

        device->setPlot(const_cast<CQChartsPlot *>(this));
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
invalidateOverlay()
{
  if (parentPlot())
    return parentPlot()->invalidateOverlay();

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
  return *displayRange_;
}

void
CQChartsPlot::
setDisplayRange(const DisplayRange &r)
{
  assert(r.isValid());

  *displayRange_ = r;
}

//---

void
CQChartsPlot::
setDataRange(const Range &r, bool update)
{
  assert(r.isValid());

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
  setDataRange(Range(), /*update*/false);

  if (updateRange)
    this->execUpdateRange();

  if (updateObjs)
    this->updateObjs();
}

//---

double
CQChartsPlot::
dataScale() const
{
  return CMathUtil::avg(dataScaleX(), dataScaleY());
}

double
CQChartsPlot::
dataScaleX() const
{
  if (parentPlot())
    return parentPlot()->dataScaleX();

  if      (isOverlay()) {
    if (! isFirstPlot())
      return firstPlot()->dataScaleX();
  }
  else if (isY1Y2()) {
    if (! isFirstPlot())
      return firstPlot()->dataScaleX();
  }

  return zoomData_.dataScale.x;
}

void
CQChartsPlot::
setDataScaleX(double x)
{
  zoomData_.dataScale.x = x;
}

double
CQChartsPlot::
dataScaleY() const
{
  if (parentPlot())
    return parentPlot()->dataScaleY();

  if      (isOverlay()) {
    if (! isFirstPlot())
      return firstPlot()->dataScaleY();
  }
  else if (isX1X2()) {
    if (! isFirstPlot())
      return firstPlot()->dataScaleY();
  }

  return zoomData_.dataScale.y;
}

void
CQChartsPlot::
setDataScaleY(double y)
{
  zoomData_.dataScale.y = y;
}

double
CQChartsPlot::
dataOffsetX() const
{
  if (parentPlot())
    return parentPlot()->dataOffsetX();

  if      (isOverlay()) {
    if (! isFirstPlot())
      return firstPlot()->dataOffsetX();
  }
  else if (isY1Y2()) {
    if (! isFirstPlot())
      return firstPlot()->dataOffsetX();
  }

  return zoomData_.dataOffset.x;
}

void
CQChartsPlot::
setDataOffsetX(double x)
{
  zoomData_.dataOffset.x = x;
}

double
CQChartsPlot::
dataOffsetY() const
{
  if (parentPlot())
    return parentPlot()->dataOffsetY();

  if      (isOverlay()) {
    if (! isFirstPlot())
      return firstPlot()->dataOffsetY();
  }
  else if (isX1X2()) {
    if (! isFirstPlot())
      return firstPlot()->dataOffsetY();
  }

  return zoomData_.dataOffset.y;
}

void
CQChartsPlot::
setDataOffsetY(double y)
{
  zoomData_.dataOffset.y = y;
}

//---

void
CQChartsPlot::
setZoomData(const ZoomData &zoomData)
{
  zoomData_ = zoomData;

  applyDataRangeAndDraw();
}

void
CQChartsPlot::
updateDataScale(double r)
{
  setDataScaleX(r);
  setDataScaleY(r);

  applyDataRangeAndDraw();
}

void
CQChartsPlot::
updateDataScaleX(double r)
{
  setDataScaleX(r);

  applyDataRangeAndDraw();
}

void
CQChartsPlot::
updateDataScaleY(double r)
{
  setDataScaleY(r);

  applyDataRangeAndDraw();
}

//---

void
CQChartsPlot::
setXMin(const OptReal &r)
{
  CQChartsUtil::testAndSet(xmin_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPlot::
setXMax(const OptReal &r)
{
  CQChartsUtil::testAndSet(xmax_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPlot::
setYMin(const OptReal &r)
{
  CQChartsUtil::testAndSet(ymin_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPlot::
setYMax(const OptReal &r)
{
  CQChartsUtil::testAndSet(ymax_, r, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPlot::
setEveryEnabled(bool b)
{
  CQChartsUtil::testAndSet(everyData_.enabled, b, [&]() { updateObjs(); } );
}

void
CQChartsPlot::
setEveryStart(int i)
{
  CQChartsUtil::testAndSet(everyData_.start, i, [&]() { updateObjs(); } );
}

void
CQChartsPlot::
setEveryEnd(int i)
{
  CQChartsUtil::testAndSet(everyData_.end, i, [&]() { updateObjs(); } );
}

void
CQChartsPlot::
setEveryStep(int i)
{
  CQChartsUtil::testAndSet(everyData_.step, i, [&]() { updateObjs(); } );
}

//---

void
CQChartsPlot::
setFilterStr(const QString &s)
{
  CQChartsUtil::testAndSet(filterStr_, s, [&]() { updateRangeAndObjs(); } );
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
  CQChartsUtil::testAndSet(skipBad_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPlot::
setTitleStr(const QString &s)
{
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
setPlotsAxisNames(const Plots &plots, CQChartsPlot *axisPlot)
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

double
CQChartsPlot::
xAxisSideDelta(const CQChartsAxisSide::Type &side) const
{
  auto p = xAxisSideDelta_.find(side);

  return (p != xAxisSideDelta_.end() ? (*p).second : 0.0);
}

double
CQChartsPlot::
yAxisSideDelta(const CQChartsAxisSide::Type &side) const
{
  auto p = yAxisSideDelta_.find(side);

  return (p != yAxisSideDelta_.end() ? (*p).second : 0.0);
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
setDataClip(bool b)
{
  CQChartsUtil::testAndSet(dataClip_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsPlot::
setFitBorderSides(const Sides &s)
{
  CQChartsUtil::testAndSet(fitBorderSides_, s, [&]() { drawBackground(); } );
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

bool
CQChartsPlot::
isKeyVisible() const
{
  return (key() && key()->isVisible());
}

void
CQChartsPlot::
setKeyVisible(bool b)
{
  if (key())
    key()->setVisible(b);
}

bool
CQChartsPlot::
isKeyVisibleAndNonEmpty() const
{
  return (key() && key()->isVisibleAndNonEmpty());
}

void
CQChartsPlot::
setColorKey(bool b)
{
  CQChartsUtil::testAndSet(colorKey_, b, [&]() {
    resetSetHidden();

    resetKeyItems();

    updateRangeAndObjs();

    updateKeyPosition(/*force*/true);
  });
}

//---

void
CQChartsPlot::
setEqualScale(bool b)
{
  CQChartsUtil::testAndSet(equalScale_, b, [&]() { updateRange(); });
}

//---

void
CQChartsPlot::
setAutoFit(bool b)
{
  CQChartsUtil::testAndSet(autoFit_, b, [&]() { postResize(); });
}

// fit margin
void
CQChartsPlot::
setFitMarginLeft(const Length &l)
{
  assert(l.isValid());

  if (l != fitMargin_.left()) { fitMargin_.setLeft(l); postResize(); }
}

void
CQChartsPlot::
setFitMarginTop(const Length &t)
{
  assert(t.isValid());

  if (t != fitMargin_.top()) { fitMargin_.setTop(t); postResize(); }
}

void
CQChartsPlot::
setFitMarginRight(const Length &r)
{
  assert(r.isValid());

  if (r != fitMargin_.right()) { fitMargin_.setRight(r); postResize(); }
}

void
CQChartsPlot::
setFitMarginBottom(const Length &b)
{
  assert(b.isValid());

  if (b != fitMargin_.bottom()) { fitMargin_.setBottom(b); postResize(); }
}

void
CQChartsPlot::
setFitMargin(const PlotMargin &m)
{
  if (m != fitMargin_) { fitMargin_ = m; postResize(); }
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

void
CQChartsPlot::
setViewBBox(const BBox &bbox)
{
  viewBBox_      = bbox;
  innerViewBBox_ = viewBBox_;

  updateMargins();

  emit viewBoxChanged();
}

void
CQChartsPlot::
updateMargins(bool update)
{
  if (parentPlot())
    return parentPlot()->updateMargins(update);

  if (isOverlay()) {
    if (! isFirstPlot())
      return firstPlot()->updateMargins(update);

    processOverlayPlots([&](CQChartsPlot *plot) {
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
  innerViewBBox_ = outerMargin.adjustViewRange(this, calcViewBBox(), /*inside*/false);

  setPixelRange(innerViewBBox_);
}

//---

CQChartsGeom::BBox
CQChartsPlot::
calcDataRect() const
{
  if (parentPlot())
    return parentPlot()->calcDataRect();

  return getCalcDataRange().bbox();
}

CQChartsGeom::Range
CQChartsPlot::
getCalcDataRange() const
{
  return calcDataRange_;
}

CQChartsGeom::BBox
CQChartsPlot::
outerDataRect() const
{
  if (parentPlot())
    return parentPlot()->outerDataRect();

  return outerDataRange_.bbox();
}

CQChartsGeom::BBox
CQChartsPlot::
dataRect() const
{
  if (parentPlot())
    return parentPlot()->dataRect();

  return dataRange_.bbox();
}

//---

CQChartsGeom::BBox
CQChartsPlot::
calcViewBBox() const
{
  if (parentPlot())
    return parentPlot()->adjustedViewBBox(this);

  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->calcViewBBox();

  return viewBBox();
}

CQChartsGeom::BBox
CQChartsPlot::
adjustedViewBBox(const CQChartsPlot *) const
{
  return viewBBox();
}

CQChartsGeom::BBox
CQChartsPlot::
innerViewBBox() const
{
  if (parentPlot())
    return parentPlot()->innerViewBBox();

  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->innerViewBBox();

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
aspect() const
{
  auto viewBBox = calcViewBBox();

  auto p1 = view()->windowToPixel(Point(viewBBox.getXMin(), viewBBox.getYMin()));
  auto p2 = view()->windowToPixel(Point(viewBBox.getXMax(), viewBBox.getYMax()));

  if (p1.y == p2.y)
    return 1.0;

  return fabs(p2.x - p1.x)/fabs(p2.y - p1.y);
}

//---

// inner margin
void
CQChartsPlot::
setInnerMarginLeft(const Length &l)
{
  if (l != innerMargin_.left()) { innerMargin_.setLeft(l); applyDataRangeAndDraw(); }
}

void
CQChartsPlot::
setInnerMarginTop(const Length &t)
{
  if (t != innerMargin_.top()) { innerMargin_.setTop(t); applyDataRangeAndDraw(); }
}

void
CQChartsPlot::
setInnerMarginRight(const Length &r)
{
  if (r != innerMargin_.right()) { innerMargin_.setRight(r); applyDataRangeAndDraw(); }
}

void
CQChartsPlot::
setInnerMarginBottom(const Length &b)
{
  if (b != innerMargin_.bottom()) { innerMargin_.setBottom(b); applyDataRangeAndDraw(); }
}

void
CQChartsPlot::
setInnerMargin(const PlotMargin &m)
{
  if (m != innerMargin_) { innerMargin_ = m; applyDataRangeAndDraw(); }
}

//---

void
CQChartsPlot::
setOuterMarginLeft(const Length &l)
{
  if (l != outerMargin_.left()) { outerMargin_.setLeft(l); updateMargins(); }
}

void
CQChartsPlot::
setOuterMarginTop(const Length &t)
{
  if (t != outerMargin_.top()) { outerMargin_.setTop(t); updateMargins(); }
}

void
CQChartsPlot::
setOuterMarginRight(const Length &r)
{
  if (r != outerMargin_.right()) { outerMargin_.setRight(r); updateMargins(); }
}

void
CQChartsPlot::
setOuterMarginBottom(const Length &b)
{
  if (b != outerMargin_.bottom()) { outerMargin_.setBottom(b); updateMargins(); }
}

void
CQChartsPlot::
setOuterMargin(const PlotMargin &m)
{
  if (m != outerMargin_) { outerMargin_ = m; updateMargins(); }
}

//---

void
CQChartsPlot::
emitTitleChanged()
{
  emit titleChanged();
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
    emit connectDataChanged();
}

void
CQChartsPlot::
updateOverlay()
{
  processOverlayPlots([&](CQChartsPlot *plot) {
    plot->stopThreadTimer ();
    plot->startThreadTimer();

    plot->updatesData_.reset();
  });

  applyDataRangeAndDraw();
}

bool
CQChartsPlot::
isX1X2(bool checkVisible) const
{
  if (! connectData_.x1x2)
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
setX1X2(bool b, bool notify)
{
  connectData_.x1x2 = b;

  if (notify)
    emit connectDataChanged();
}

bool
CQChartsPlot::
isY1Y2(bool checkVisible) const
{
  if (! connectData_.y1y2)
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
setY1Y2(bool b, bool notify)
{
  connectData_.y1y2 = b;

  if (notify)
    emit connectDataChanged();
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
    emit connectDataChanged();
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
    emit currentPlotChanged(this);
    emit currentPlotIdChanged(id());
  }
}

void
CQChartsPlot::
setNextPlot(CQChartsPlot *plot, bool notify)
{
  assert(plot != this && ! connectData_.next);

  connectData_.next = plot;

  if (notify)
    emit connectDataChanged();
}

void
CQChartsPlot::
setPrevPlot(CQChartsPlot *plot, bool notify)
{
  assert(plot != this && ! connectData_.prev);

  connectData_.prev = plot;

  if (notify)
    emit connectDataChanged();
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
  return const_cast<CQChartsPlot *>(this)->firstPlot();
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
  return const_cast<CQChartsPlot *>(this)->lastPlot();
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
      plots.push_back(const_cast<CQChartsPlot *>(plot1));

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

    plots.push_back(const_cast<CQChartsPlot *>(plot1));

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

    plots.push_back(const_cast<CQChartsPlot *>(plot1));

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
      plots.push_back(const_cast<CQChartsPlot *>(plot1));

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
    emit connectDataChanged();
}

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

    processOverlayPlots([&](CQChartsPlot *plot) {
      plot->invertX_ = b;
    });
  }
  else {
    invertX_ = b;
  }

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

    processOverlayPlots([&](CQChartsPlot *plot) {
      plot->invertY_ = b;
    });
  }
  else {
    invertY_ = b;
  }

  drawObjs();
}

//------

bool
CQChartsPlot::
isLogX() const
{
  // return logX_;
  return (mappedXAxis() && mappedXAxis()->valueType() == CQChartsAxisValueType::Type::LOG);
}

bool
CQChartsPlot::
isLogY() const
{
  // return logY_;
  return (mappedYAxis() && mappedYAxis()->valueType() == CQChartsAxisValueType::Type::LOG);
}

void
CQChartsPlot::
setLogX(bool b)
{
  if (mappedXAxis() && b != isLogX()) {
    mappedXAxis()->setValueType(b ? CQChartsAxisValueType::Type::LOG :
                                    CQChartsAxisValueType::Type::REAL);
    updateRangeAndObjs();
  }
}

void
CQChartsPlot::
setLogY(bool b)
{
  if (mappedYAxis() && b != isLogY()) {
    mappedYAxis()->setValueType(b ? CQChartsAxisValueType::Type::LOG :
                                    CQChartsAxisValueType::Type::REAL);
    updateRangeAndObjs();
  }
}

//------

const CQPropertyViewModel *
CQChartsPlot::
propertyModel() const
{
  return propertyModel_;
}

CQPropertyViewModel *
CQChartsPlot::
propertyModel()
{
  return propertyModel_;
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
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc, bool hidden=false) {
    auto *item = &(this->addProperty(path, this, name, alias)->setDesc(desc));
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc, bool hidden=false) {
    auto *item = addProp(path, name, alias, desc, hidden);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  // data
  addProp("", "viewId"  , "view"    , "Parent view id", true);
  addProp("", "typeStr" , "type"    , "Type name"     , true);
  addProp("", "visible" , "visible" , "Plot visible"  , true);
  addProp("", "selected", "selected", "Plot selected" , true);
  addProp("", "editable", "editable", "Plot editable" , true);

  addProp("", "name", "name", "Plot name", true);

  // font
  addStyleProp("font", "font"      , "font"      , "Base font");
  addStyleProp("font", "tabbedFont", "tabbedFont", "Font for tabs");

  // columns
  addProp("columns", "idColumn"      , "id"      , "Id column");
  addProp("columns", "tipColumns"    , "tips"    , "Tip columns");
  addProp("columns", "noTipColumns"  , "notips"  , "No Tip columns");
  addProp("columns", "visibleColumn" , "visible" , "Visible column");
  addProp("columns", "colorColumn"   , "color"   , "Color column");
  addProp("columns", "fontColumn"    , "font"    , "Font column");
  addProp("columns", "imageColumn"   , "image"   , "Image column");
  addProp("columns", "controlColumns", "controls", "Control columns");

  // range
  addProp("range", "viewRect", "view", "View rectangle");
  addProp("range", "dataRect", "data", "Data rectangle");

  addProp("range", "innerViewRect", "innerView", "Inner view rectangle"     , true);
  addProp("range", "calcDataRect" , "calcData" , "Calculated data rectangle", true);
  addProp("range", "outerDataRect", "outerData", "Outer data rectangle"     , true);

  addProp("range", "autoFit", "autoFit", "Auto fit to data");

  if (type()->customXRange()) addProp("range", "xmin", "xmin", "Explicit minimum x value");
  if (type()->customYRange()) addProp("range", "ymin", "ymin", "Explicit minimum y value");
  if (type()->customXRange()) addProp("range", "xmax", "xmax", "Explicit maximum x value");
  if (type()->customYRange()) addProp("range", "ymax", "ymax", "Explicit maximum y value");

  // scaling
  addProp("scaling", "equalScale", "equal", "Equal x/y scaling");

  addProp("scaling/data/scale" , "dataScaleX" , "x", "X data scale" , true);
  addProp("scaling/data/scale" , "dataScaleY" , "y", "Y data scale" , true);
  addProp("scaling/data/offset", "dataOffsetX", "x", "X data offset", true);
  addProp("scaling/data/offset", "dataOffsetY", "y", "Y data offset", true);

  // grouping
  addProp("grouping", "overlay", "", "Overlay plots to shared range"    , true);
  addProp("grouping", "x1x2"   , "", "Independent x axes, shared y axis", true);
  addProp("grouping", "y1y2"   , "", "Independent y axes, shared x axis", true);
  addProp("grouping", "tabbed" , "", "Tabbed plots in same region"      , true);

  // invert
  addProp("invert", "invertX", "x", "Invert x values");
  addProp("invert", "invertY", "y", "Invert y values");

#if 0
  // log
  if (type()->allowXLog()) addProp("log", "logX", "x", "Use log x axis");
  if (type()->allowYLog()) addProp("log", "logY", "y", "Use log y axis");
#endif

  // debug
  if (CQChartsEnv::getBool("CQ_CHARTS_DEBUG")) {
    addProp("debug", "showBoxes"        , "", "Show object bounding boxes");
    addProp("debug", "showSelectedBoxes", "", "Show selected object bounding boxes");
    addProp("debug", "followMouse"      , "", "Enable mouse tracking");
  }

  //------

  // plot box
  QString plotStyleStr       = "plotBox";
  auto    plotStyleFillStr   = plotStyleStr + "/fill";
  auto    plotStyleStrokeStr = plotStyleStr + "/stroke";

  addProp(plotStyleStr, "plotClip", "clip" , "Clip to plot bounding box");

  addProp(plotStyleStr, "plotShapeData", "shape", "Plot background shape data", true);

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
  QString dataStyleStr       = "dataBox";
  auto    dataStyleFillStr   = dataStyleStr + "/fill";
  auto    dataStyleStrokeStr = dataStyleStr + "/stroke";

  addProp(dataStyleStr, "dataClip", "clip" , "Clip to data bounding box");

  addProp(dataStyleStr, "dataShapeData", "shape", "Data background shape data", true);

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
  QString fitStyleStr       = "fitBox";
  auto    fitStyleFillStr   = fitStyleStr + "/fill";
  auto    fitStyleStrokeStr = fitStyleStr + "/stroke";

  addStyleProp(fitStyleFillStr, "fitFilled", "visible",
               "Fit background bounding box fill visible", true);

  addFillProperties(fitStyleFillStr, "fitFill", "Fit background", /*hidden*/true);

  addStyleProp(fitStyleStrokeStr, "fitStroked", "visible",
               "Fit background bounding box stroke visible", true);

  addLineProperties(fitStyleStrokeStr, "fitStroke", "Fit background", /*hidden*/true);

  addStyleProp(fitStyleStrokeStr, "fitBorderSides", "sides",
               "Fit background bounding box stroked sides", true);

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

  //---

  // every
  addProp("every", "everyEnabled", "enabled", "Enable every row filter"  , true);
  addProp("every", "everyStart"  , "start"  , "Start of every row filter", true);
  addProp("every", "everyEnd"    , "end"    , "End of every row filter"  , true);
  addProp("every", "everyStep"   , "step"   , "Step of every row filter" , true);

  // filter
  addProp("filter", "filterStr"       , "expression", "Filter expression", true);
  addProp("filter", "visibleFilterStr", "visible"   , "Filter visible expression", true);

  addProp("filter", "skipBad", "skipBad", "Skip bad values");

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

    addProp("key", "colorKey", "colorColumn", "Use Color Column for Key");
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
  addProp("scaledFont", "minScaleFontSize", "minSize", "Min scaled font size", true);
  addProp("scaledFont", "maxScaleFontSize", "maxSize", "Max scaled font size", true);
}

void
CQChartsPlot::
addSymbolProperties(const QString &path, const QString &prefix, const QString &descPrefix,
                    bool hidden)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc, bool hidden) {
    auto *item = &(this->addProperty(path, this, name, alias)->setDesc(desc));
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  //---

  auto prefix1 = (descPrefix.length() ? descPrefix + " symbol" : "Symbol");

  auto strokePath = path + "/stroke";
  auto fillPath   = path + "/fill";

  auto symbolPrefix = (prefix.length() ? prefix + "Symbol" : "symbol");

  addProp(path, symbolPrefix + "Type", "type", prefix1 + " type", hidden);
  addProp(path, symbolPrefix + "Size", "size", prefix1 + " size", hidden);

  //---

  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc, bool hidden) {
    auto *item = addProp(path, name, alias, desc, hidden);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  addStyleProp(fillPath, symbolPrefix + "Filled"     , "visible",
               prefix1 + " fill visible", hidden);
  addStyleProp(fillPath, symbolPrefix + "FillColor"  , "color"  ,
               prefix1 + " fill color", hidden);
  addStyleProp(fillPath, symbolPrefix + "FillAlpha"  , "alpha"  ,
               prefix1 + " fill alpha", hidden);
  addStyleProp(fillPath, symbolPrefix + "FillPattern", "pattern",
               prefix1 + " fill pattern", hidden);

  addStyleProp(strokePath, symbolPrefix + "Stroked"    , "visible",
               prefix1 + " stroke visible", hidden);
  addStyleProp(strokePath, symbolPrefix + "StrokeColor", "color"  ,
               prefix1 + " stroke color", hidden);
  addStyleProp(strokePath, symbolPrefix + "StrokeAlpha", "alpha"  ,
               prefix1 + " stroke alpha", hidden);
  addStyleProp(strokePath, symbolPrefix + "StrokeWidth", "width"  ,
               prefix1 + " stroke width", hidden);
  addStyleProp(strokePath, symbolPrefix + "StrokeDash" , "dash"   ,
               prefix1 + " stroke dash", hidden);
}

void
CQChartsPlot::
addLineProperties(const QString &path, const QString &prefix, const QString &descPrefix,
                  bool hidden)
{
  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc, bool hidden) {
    auto *item = this->addProperty(path, this, name, alias);
    item->setDesc(desc);
    CQCharts::setItemIsStyle(item);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  //---

  auto prefix1 = (descPrefix.length() ? descPrefix + " stroke" : "Stroke");

  addStyleProp(path, prefix + "Color", "color", prefix1 + " color", hidden);
  addStyleProp(path, prefix + "Alpha", "alpha", prefix1 + " alpha", hidden);
  addStyleProp(path, prefix + "Width", "width", prefix1 + " width", hidden);
  addStyleProp(path, prefix + "Dash" , "dash" , prefix1 + " dash" , hidden);
}

void
CQChartsPlot::
addFillProperties(const QString &path, const QString &prefix, const QString &descPrefix,
                  bool hidden)
{
  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc, bool hidden) {
    auto *item = this->addProperty(path, this, name, alias);
    item->setDesc(desc);
    CQCharts::setItemIsStyle(item);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  //---

  auto prefix1 = (descPrefix.length() ? descPrefix + " fill" : "Fill");

  addStyleProp(path, prefix + "Color"  , "color"  , prefix1 + " color"  , hidden);
  addStyleProp(path, prefix + "Alpha"  , "alpha"  , prefix1 + " alpha"  , hidden);
  addStyleProp(path, prefix + "Pattern", "pattern", prefix1 + " pattern", hidden);
}

void
CQChartsPlot::
addTextProperties(const QString &path, const QString &prefix, const QString &descPrefix,
                  uint valueTypes, bool hidden)
{
  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc, bool hidden) {
    auto *item = this->addProperty(path, this, name, alias);
    item->setDesc(desc);
    CQCharts::setItemIsStyle(item);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  //---

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
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  addProp("mapping/color", "colorMapped"    , "enabled", "Color values mapped");
  addProp("mapping/color", "colorMapMin"    , "min"    , "Color value map min");
  addProp("mapping/color", "colorMapMax"    , "max"    , "Color value map max");
  addProp("mapping/color", "colorMapPalette", "palette", "Color map palette");
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
  assert(CQUtil::hasProperty(object, name));

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
  names = CQUtil::getPropertyList(plotObj, /*inherited*/ false);
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
  xAxis_ = new CQChartsAxis(this, Qt::Horizontal, 0.0, 1.0);

  xAxis_->setObjectName("xaxis");
}

void
CQChartsPlot::
addYAxis()
{
  yAxis_ = new CQChartsAxis(this, Qt::Vertical, 0.0, 1.0);

  yAxis_->setObjectName("yaxis");
}

//------

void
CQChartsPlot::
addKey()
{
  keyObj_ = new CQChartsPlotKey(this);
}

void
CQChartsPlot::
resetKeyItems()
{
  CQPerfTrace trace("CQChartsPlot::resetKeyItems");

  if (isOverlay()) {
    // if first plot then add all chained plot items to this plot's key
    if (prevPlot())
      return;

    //---

    if (key()) {
      key()->clearItems();

      processOverlayPlots([&](CQChartsPlot *plot) {
        plot->doAddKeyItems(key());
      });
    }
  }
  else {
    if (key()) {
      key()->clearItems();

      doAddKeyItems(key());
    }
  }
}

void
CQChartsPlot::
doAddKeyItems(CQChartsPlotKey *key)
{
  // add key items from color column
  if (isColorKey()) {
    if (addColorKeyItems(key))
      return;
  }

  addKeyItems(key);
}

bool
CQChartsPlot::
addColorKeyItems(CQChartsPlotKey *key)
{
  int row = (! key->isHorizontal() ? key->maxRow() : 0);
  int col = (! key->isHorizontal() ? 0 : key->maxCol());

  auto addKeyRow = [&](const QString &name, const QColor &c) {
    auto *colorItem = new CQChartsKeyColorBox(this, ColorInd(), ColorInd(), ColorInd());
    auto *textItem  = new CQChartsKeyText    (this, name, ColorInd());

    auto *groupItem = new CQChartsKeyItemGroup(this);

    groupItem->addItem(colorItem);
    groupItem->addItem(textItem );

    if (c.isValid())
      colorItem->setColor(c);

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

  if (! colorColumn().isValid())
    return false;

  auto *columnDetails = this->columnDetails(colorColumn());
  if (! columnDetails) return false;

  auto uniqueValues = columnDetails->uniqueValues();

  for (auto &value : uniqueValues) {
    QString name;

    if (! CQChartsVariant::toString(value, name))
      name = value.toString();

    Color color;

    columnValueColor(value, color);

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

  titleObj_ = new CQChartsTitle(this);

  title()->setTextStr(titleStr());
}

//------

void
CQChartsPlot::
threadTimerSlot()
{
  if (parentPlot())
    return parentPlot()->startThreadTimer();

  if (isOverlay() && ! isFirstPlot())
    return firstPlot()->startThreadTimer();

  //---

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

      // ensure all overlay plot ranges done
      if (isOverlay())
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

      // ensure all overlay plot objs done
      if (isOverlay())
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

      // ensure all overlay draw done
      if (isOverlay())
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

  if (objTreeData_.notify) {
    objTreeData_.notify = false;

    postObjTree();
  }

  //---

  if      (nextState == UpdateState::UPDATE_RANGE) {
    updatesData_.stateFlag[UpdateState::UPDATE_RANGE] = 0;

    this->execUpdateRange();
  }
  else if (nextState == UpdateState::UPDATE_OBJS) {
    // don't update until range calculated
    if (updateState != UpdateState::CALC_RANGE) {
      updatesData_.stateFlag[UpdateState::UPDATE_OBJS] = 0;

      this->execUpdateObjs();
    }
  }
  else if (nextState == UpdateState::UPDATE_DRAW_OBJS) {
    // don't update until range and objs calculated
    if (updateState != UpdateState::CALC_RANGE &&
        updateState != UpdateState::CALC_OBJS) {
      updatesData_.stateFlag[UpdateState::UPDATE_DRAW_OBJS      ] = 0;
      updatesData_.stateFlag[UpdateState::UPDATE_DRAW_BACKGROUND] = 0;
      updatesData_.stateFlag[UpdateState::UPDATE_DRAW_FOREGROUND] = 0;

      this->execInvalidateLayers();

      updateView = true;
    }
  }
  else if (nextState == UpdateState::UPDATE_DRAW_BACKGROUND) {
    // don't update until objs drawn
    if (updateState != UpdateState::DRAW_OBJS) {
      updatesData_.stateFlag[UpdateState::UPDATE_DRAW_BACKGROUND] = 0;

      this->invalidateLayer(Buffer::Type::BACKGROUND);

      this->invalidateOverlay();

      updateView = true;
    }
  }
  else if (nextState == UpdateState::UPDATE_DRAW_FOREGROUND) {
    // don't update until objs drawn
    if (updateState != UpdateState::DRAW_OBJS) {
      updatesData_.stateFlag[UpdateState::UPDATE_DRAW_FOREGROUND] = 0;

      this->invalidateLayer(Buffer::Type::FOREGROUND);

      this->invalidateOverlay();

      updateView = true;
    }
  }

  //---

  if (updateView)
    view()->update();
}

CQChartsPlot::UpdateState
CQChartsPlot::
calcNextState() const
{
  auto *th = const_cast<CQChartsPlot *>(this);

  auto nextState = UpdateState::INVALID;

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
    processOverlayPlots([&](CQChartsPlot *plot) {
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
  updateData_.state.store((int) state);

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
  auto updateState = const_cast<CQChartsPlot *>(this)->updateState();

  return (updateState == UpdateState::READY || updateState == UpdateState::DRAWN);
}

//------

void
CQChartsPlot::
clearRangeAndObjs()
{
  resetRange();

  clearPlotObjects();
}

void
CQChartsPlot::
execUpdateRangeAndObjs()
{
  if (! isUpdatesEnabled()) {
    updatesData_.updateRangeAndObjs = true;
    return;
  }

  updateAndApplyRange(/*apply*/true, /*updateObjs*/true);
}

void
CQChartsPlot::
resetRange()
{
  dataRange_.reset();
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

    processOverlayPlots([&](CQChartsPlot *plot) {
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
    LockMutex lock1(this, "updateAndApplyPlotRange");

    // finish current threads
    interruptRange();

    //---

    // start update range thread
    updateData_.updateObjs   = updateObjs;
    updateData_.drawBusy.ind = -100;

    setGroupedUpdateState(UpdateState::CALC_RANGE);

    updateData_.rangeThread->exec(updateRangeASync, this);

    startThreadTimer();
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
interruptRange()
{
  setInterrupt(true);

  waitRange();
  waitObjs ();
  waitDraw ();

  setInterrupt(false);
}

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
    processOverlayPlots([&](CQChartsPlot *plot) {
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
updateRangeASync(CQChartsPlot *plot)
{
  plot->updateRangeThread();
}

void
CQChartsPlot::
updateRangeThread()
{
  CQPerfTrace trace("CQChartsPlot::updateRangeThread");

  //---

  if (isOverlay())
    clearOverlayErrors();

  resetAnnotationBBox();

  calcDataRange_  = calcRange();
  dataRange_      = adjustDataRange(getCalcDataRange());
  outerDataRange_ = dataRange_;

  postCalcRange();

  //---

  if (isOverlay())
    updateOverlayRanges();

  //---

  // mark thread done
  updateData_.rangeThread->end();
}

//------

void
CQChartsPlot::
updateGroupedObjs()
{
  if (isOverlay()) {
    processOverlayPlots([&](CQChartsPlot *plot) {
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
    updatesData_.updateObjs = true;
    return;
  }

  //---

  if (! dataRange_.isSet()) {
    execUpdateRangeAndObjs();
    return;
  }

  // update overlay objects
  if (isOverlay()) {
    if (! isFirstPlot())
      return;

    processOverlayPlots([&](CQChartsPlot *plot) {
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
    LockMutex lock(this, "updatePlotObjs");

    //---

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
interruptObjs()
{
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
    processOverlayPlots([&](CQChartsPlot *plot) {
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
updateObjsASync(CQChartsPlot *plot)
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

  initColorColumnData();

  initPlotObjs();

  //---

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

  auto *th = const_cast<CQChartsPlot *>(this);

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
  if (parentPlot())
    return parentPlot()->getDataRange();

  if (dataRange_.isSet())
    return CQChartsUtil::rangeBBox(dataRange_);
  else
    return BBox(0.0, 0.0, 1.0, 1.0);
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
    updatesData_.applyDataRange = true;
    return;
  }

  if (! dataRange_.isSet()) {
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
        processOverlayPlots([&](CQChartsPlot *plot) {
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
        setWindowRange(adjustedRange);
      }
      else if (isY1Y2()) {
        setWindowRange(adjustedRange);
      }
    }
    else {
      // This breaks X1X2 and Y1Y2 plots (wrong range)
      if (! isX1X2() && ! isY1Y2()) {
        processOverlayPlots([&](CQChartsPlot *plot) {
          plot->setWindowRange(adjustedRange);
        });
      }
    }
  }
  else {
    setWindowRange(adjustedRange);
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

        processOverlayPlots([&](CQChartsPlot *plot) {
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

  emit rangeChanged();
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
    processOverlayPlots([&](CQChartsPlot *plot) {
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

      processOverlayPlots([&](CQChartsPlot *plot) {
        dataRange += plot->dataRange();
      });

      processOverlayPlots([&](CQChartsPlot *plot) {
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
  displayRange_->setPixelRange(bbox.getXMin(), bbox.getYMax(), bbox.getXMax(), bbox.getYMin());
}

void
CQChartsPlot::
resetWindowRange()
{
  displayRange_->setWindowRange(0.0, 0.0, 1.0, 1.0);
}

void
CQChartsPlot::
setWindowRange(const BBox &bbox)
{
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
    processOverlayPlots([&](const CQChartsPlot *plot) {
      auto bbox1 = plot->calcDataRange();
      if (! bbox1.isSet()) return;

      if (bbox1.isSet()) {
        if (rangeTypes.annotation)
          bbox1 += plot->annotationBBox();

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
      if (rangeTypes.annotation)
        bbox += annotationBBox();

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
  if (! xAxis())
    return BBox();

  BBox xbbox;

  if (isOverlay()) {
    processOverlayPlots([&](const CQChartsPlot *plot) {
      if (! plot->xAxis()) return;

      if (side == CQChartsAxisSide::Type::NONE || plot->xAxis()->side() == side) {
        auto xbbox1 = plot->xAxis()->bbox();
        if (! xbbox1.isSet()) return;

        if (plot != this)
          xbbox1 = viewToWindow(plot->windowToView(xbbox1));

        xbbox += xbbox1;
      }
    });
  }
  else {
    if (side == CQChartsAxisSide::Type::NONE || xAxis()->side() == side)
      xbbox = xAxis()->bbox();
  }

  return xbbox;
}

CQChartsGeom::BBox
CQChartsPlot::
calcGroupedYAxisRange(const CQChartsAxisSide::Type &side) const
{
  if (! yAxis())
    return BBox();

  BBox ybbox;

  if (isOverlay()) {
    processOverlayPlots([&](const CQChartsPlot *plot) {
      if (! plot->yAxis()) return;

      if (side == CQChartsAxisSide::Type::NONE || plot->yAxis()->side() == side) {
        auto ybbox1 = plot->yAxis()->bbox();
        if (! ybbox1.isSet()) return;

        if (plot != this)
          ybbox1 = viewToWindow(plot->windowToView(ybbox1));

        ybbox += ybbox1;
      }
    });
  }
  else {
    if (side == CQChartsAxisSide::Type::NONE || yAxis()->side() == side)
      ybbox = yAxis()->bbox();
  }

  return ybbox;
}

void
CQChartsPlot::
resetAnnotationBBox() const
{
  annotationBBox_ = BBox();
}

//------

void
CQChartsPlot::
addPlotObject(PlotObj *obj)
{
  assert(obj);

  assert(! objTreeData_.tree->isBusy());

  plotObjs_.push_back(obj);

  // TODO: needed ? Do post thread finished
#if 0
  obj->moveToThread(this->thread());

  obj->setParent(this);
#endif

  //obj->addProperties(propertyModel(), "objects");
}

void
CQChartsPlot::
initGroupedPlotObjs()
{
  CQPerfTrace trace("CQChartsPlot::initGroupedPlotObjs");

  // init overlay plots before draw
  if (isOverlay()) {
    if (! isFirstPlot())
      return;

    processOverlayPlots([&](CQChartsPlot *plot) {
      initPlotRange();

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
  if (! dataRange_.isSet()) {
    execUpdateRange();

    if (! dataRange_.isSet())
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

  // ensure some range defined
  if (! dataRange_.isSet()) {
    dataRange_.updateRange(0, 0);
    dataRange_.updateRange(1, 1);

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
    //needsAutoFit_ = true;
  }

  //---

  if (changed)
    emit plotObjsAdded();
}

bool
CQChartsPlot::
addNoDataObj()
{
  CQPerfTrace trace("CQChartsPlot::addNoDataObj");

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

  if (! plotObjs_.empty())
    return false;

  //---

  if (! createObjs())
    return false;

  //---

  resetKeyItems();

  return true;
}

bool
CQChartsPlot::
createObjs()
{
  resetAnnotationBBox();

  //---

  PlotObjs objs;

  if (! createObjs(objs))
    return false;

  for (auto &obj : objs)
    addPlotObject(obj);

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
columnHeaderName(const Column &column, bool tip) const
{
  auto p = columnNames_.find(column);
  if (p != columnNames_.end()) return (*p).second;

  bool ok;

  auto str = (tip ? modelHHeaderTip(column, ok) : modelHHeaderString(column, ok));
  if (! ok) return "";

  return str;
}

void
CQChartsPlot::
updateColumnNames()
{
  setColumnHeaderName(idColumn   (), "Id"   );
  setColumnHeaderName(colorColumn(), "Color");
  setColumnHeaderName(fontColumn (), "Font" );
  setColumnHeaderName(imageColumn(), "Image");
}

void
CQChartsPlot::
setColumnHeaderName(const Column &column, const QString &def)
{
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
    processOverlayPlots([&](CQChartsPlot *plot) {
      plot->execInitObjTree();
    });
  }
  else {
    execInitObjTree();
  }
}

void
CQChartsPlot::
execInitObjTree()
{
  CQPerfTrace trace("CQChartsPlot::execInitObjTree");

  if (objTreeData_.init) {
    objTreeData_.init = false;

    if (! isPreview())
      objTreeData_.tree->addObjects();
  }
}

void
CQChartsPlot::
clearPlotObjects()
{
  CQPerfTrace trace("CQChartsPlot::clearPlotObjects");

  objTreeData_.tree->clearObjects();

  PlotObjs plotObjs;

  std::swap(plotObjs, plotObjs_);

#if 0
  for (auto &plotObj : plotObjs)
    propertyModel()->removeProperties("objects/" + plotObj->propertyId());
#endif

  for (auto &plotObj : plotObjs)
    delete plotObj;

  insideObjs_    .clear();
  sizeInsideObjs_.clear();
}

void
CQChartsPlot::
invalidateObjTree()
{
  objTreeData_.init = true;

  objTreeData_.tree->clearObjects();
}

CQChartsGeom::BBox
CQChartsPlot::
findEmptyBBox(double w, double h) const
{
  return objTreeData_.tree->findEmptyBBox(w, h);
}

//------

bool
CQChartsPlot::
updateInsideObjects(const Point &w, Constraints constraints)
{
  // get objects at point
  Objs objs;

  objsAtPoint(w, objs, constraints);

  //---

  // check if changed
  bool changed = false;

  if (objs.size() == insideObjs_.size()) {
    for (const auto &obj : objs) {
      if (insideObjs_.find(obj) == insideObjs_.end()) {
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
    insideInd_ = 0;

    //---

    // reset inside objects
    if (isOverlay()) {
      processOverlayPlots([&](CQChartsPlot *plot) {
        plot->resetInsideObjs();
      });
    }
    else {
      resetInsideObjs();
    }

    //---

    // set new inside objects (and inside objects sorted by size)
    sizeInsideObjs_.clear();

    for (const auto &obj : objs) {
      insideObjs_.insert(obj);

      sizeInsideObjs_[obj->rect().area()].insert(obj);
    }

    // set current inside obj
    setInsideObject();
  }

  //---

  return changed;
}

void
CQChartsPlot::
resetInsideObjs()
{
  insideObjs_.clear();

  for (auto &obj : plotObjects())
    obj->setInside(false);

  for (auto &annotation : annotations()) {
    annotation->setInside(false);
  }
}

CQChartsObj *
CQChartsPlot::
insideObject() const
{
  // get nth inside object
  int i = 0;

  for (const auto &sizeObjs : sizeInsideObjs_) {
    for (const auto &obj : sizeObjs.second) {
      if (i == insideInd_)
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
  // cycle to next inside object
  ++insideInd_;

  if (insideInd_ >= int(insideObjs_.size()))
    insideInd_ = 0;
}

void
CQChartsPlot::
prevInsideInd()
{
  // cycle to prev inside object
  --insideInd_;

  if (insideInd_ < 0)
    insideInd_ = int(insideObjs_.size()) - 1;
}

void
CQChartsPlot::
setInsideObject()
{
  auto *insideObj = insideObject();

  for (auto &obj : insideObjs_) {
    if (obj == insideObj)
      obj->setInside(true);
  }
}

QString
CQChartsPlot::
insideObjectText() const
{
  QString objText;

  for (const auto &sizeObjs : sizeInsideObjs_) {
    for (const auto &obj : sizeObjs.second) {
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

void
CQChartsPlot::
clearErrors()
{
  errorData_.clear();

  emit errorsCleared();
}

bool
CQChartsPlot::
hasErrors() const
{
  return errorData_.hasErrors();
}

bool
CQChartsPlot::
addError(const QString &msg)
{
  if (! isPreview()) {
    Error err { msg };

    errorData_.globalErrors.push_back(err);

    // TODO: add to log
    //charts()->errorMsg(msg);

    emit errorAdded();
  }

  return false;
}

bool
CQChartsPlot::
addColumnError(const Column &c, const QString &msg)
{
  if (! isPreview()) {
    ColumnError err { c, msg };

    errorData_.columnErrors.push_back(err);

    // TODO: add to log
    //charts()->errorMsg(msg);

    emit errorAdded();
  }

  return false;
}

bool
CQChartsPlot::
addDataError(const ModelIndex &ind, const QString &msg)
{
  if (! isPreview()) {
    DataError err { ind, msg };

    errorData_.dataErrors.push_back(err);

    // TODO: add to log
    //charts()->errorMsg(msg);

    emit errorAdded();
  }

  return false;
}

void
CQChartsPlot::
getErrors(QStringList &strs)
{
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
selectMousePress(const Point &p, SelMod selMod)
{
  if (! isReady()) return false;

  auto w = pixelToWindow(p);

  if (selectPress(w, selMod))
    return true;

  emit selectPressSignal(w);

  return true;
}

bool
CQChartsPlot::
selectPress(const Point &w, SelMod selMod)
{
  if (tabbedSelectPress(w, selMod))
    return true;

  //---

  if (keySelectPress(key(), w, selMod))
    return true;

  if (titleSelectPress(title(), w, selMod))
    return true;

  //---

  if (annotationsSelectPress(w, selMod))
    return true;

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
keySelectPress(CQChartsPlotKey *key, const Point &w, SelMod selMod)
{
  // select key
  if (key && key->contains(w)) {
    auto *item = key->getItemAt(w);

    if (item) {
      bool handled = item->selectPress(w, selMod);

      if (handled) {
        emit keyItemPressed  (item);
        emit keyItemIdPressed(item->id());

        return true;
      }
    }

    bool handled = key->selectPress(w, selMod);

    if (handled) {
      emit keyPressed  (key);
      emit keyIdPressed(key->id());

      return true;
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
    if (title->selectPress(w, selMod)) {
      emit titlePressed  (title);
      emit titleIdPressed(title->id());

      return true;
    }
  }

  return false;
}

bool
CQChartsPlot::
annotationsSelectPress(const Point &w, SelMod selMod)
{
  annotationsAtPoint(w, pressAnnotations_);

  for (const auto &annotation : pressAnnotations_) {
    annotation->mousePress(w, selMod);
  }

  for (const auto &annotation : pressAnnotations_) {
    if (! annotation->isSelectable())
      continue;

    if (! annotation->selectPress(w, selMod))
      continue;

    selectOneObj(annotation, /*allObjs*/true);

    drawForeground();

    emit annotationPressed  (annotation);
    emit annotationIdPressed(annotation->id());

    return true;
  }

  return false;
}

CQChartsObj *
CQChartsPlot::
objectsSelectPress(const Point &w, SelMod selMod)
{
  // for replace init all objects to unselected
  // for add/remove/toggle init all objects to current state
  using ObjsSelected = std::map<Obj*, bool>;

  ObjsSelected objsSelected;

  if (isOverlay()) {
    processOverlayPlots([&](CQChartsPlot *plot) {
      for (auto &plotObj : plot->plotObjects())
        objsSelected[plotObj] = (selMod != SelMod::REPLACE ? plotObj->isSelected() : false);

      for (const auto &annotation : plot->annotations())
        objsSelected[annotation] = (selMod != SelMod::REPLACE ? annotation->isSelected() : false);
    });
  }
  else {
    for (auto &plotObj : plotObjects())
      objsSelected[plotObj] = (selMod != SelMod::REPLACE ? plotObj->isSelected() : false);

    for (const auto &annotation : annotations())
      objsSelected[annotation] = (selMod != SelMod::REPLACE ? annotation->isSelected() : false);
  }

  //---

  // get object under mouse
  Obj *selectObj = nullptr;

  if (isFollowMouse()) {
    selectObj = insideObject();

    nextInsideInd();

    setInsideObject();
  }
  else {
    Objs objs;

    objsAtPoint(w, objs, Constraints::SELECTABLE);

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

    //selectObj->selectPress();

    auto *selectPlotObj = dynamic_cast<PlotObj *>(selectObj);

    if (selectPlotObj) {
      emit objPressed  (selectPlotObj);
      emit objIdPressed(selectPlotObj->id());
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
    beginSelectIndex();

    for (const auto &objSelected : objsSelected) {
      auto *selectPlotObj = dynamic_cast<PlotObj *>(objSelected.first);

      if (! selectPlotObj || ! selectPlotObj->isSelected())
        continue;

      selectPlotObj->addSelectIndices();
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

  return selectMove(w, Constraints::SELECTABLE, first);
}

bool
CQChartsPlot::
selectMove(const Point &w, Constraints constraints, bool first)
{
  if (key()) {
    bool handled = key()->selectMove(w);

    if (handled)
      return true;
  }

  //---

  // select annotation
  Annotations annotations;

  annotationsAtPoint(w, annotations);

  for (const auto &annotation : annotations) {
    annotation->mouseMove(w);
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

  return selectRelease(w);
}

bool
CQChartsPlot::
selectRelease(const Point &w)
{
  // release pressed annotations
  for (const auto &annotation : pressAnnotations_) {
    annotation->mouseRelease(w);
  }

  return true;
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

  annotationsAtPoint(w, annotations);

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

  return editPress(p, w, inside);
}

bool
CQChartsPlot::
editPress(const Point &p, const Point &w, bool inside)
{
  if (isOverlay() && ! isFirstPlot())
    return false;

  //---

  setDragObj(DragObjType::NONE, nullptr);

  mouseData_.pressPoint = p;
  mouseData_.movePoint  = mouseData_.pressPoint;
  mouseData_.dragged    = false;

  //---

  // start drag on already selected plot handle
  if (isEditable() && isSelected()) {
    auto v = windowToView(w);

    // to edit must be in handle
    mouseData_.dragSide = editHandles_->inside(v);

    if (mouseData_.dragSide != CQChartsResizeSide::NONE) {
      setDragObj(DragObjType::PLOT_HANDLE, this);

      editHandles_->setDragSide(mouseData_.dragSide);
      editHandles_->setDragPos (w);

      invalidateOverlay();

      return true;
    }
  }

  //---

  if (keyEditPress(key(), w))
    return true;

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

  if (axisEditSelect(xAxis(), w) || axisEditSelect(yAxis(), w))
    return true;

  if (titleEditSelect(title(), w))
    return true;

  if (annotationsEditSelect(w))
    return true;

  if (objectsEditSelect(w, inside))
    return true;

  return false;
}

bool
CQChartsPlot::
keyEditPress(CQChartsPlotKey *key, const Point &w)
{
  if (! key)
    return false;

  if (! key->isEditable())
    return false;

  if (! key->isSelected())
    return false;

  // start drag on already selected key handle
  mouseData_.dragSide = key->editHandles()->inside(w);

  if (mouseData_.dragSide != CQChartsResizeSide::NONE) {
    setDragObj(DragObjType::KEY, key);

    key->editPress(w);

    key->editHandles()->setDragSide(mouseData_.dragSide);
    key->editHandles()->setDragPos (w);

    invalidateOverlay();

    return true;
  }

  return false;
}

bool
CQChartsPlot::
axisEditPress(CQChartsAxis *axis, const Point &w)
{
  if (! axis)
    return false;

  if (! axis->isEditable())
    return false;

  if (! axis->isSelected())
    return false;

  // start drag on already selected axis handle
  mouseData_.dragSide = axis->editHandles()->inside(w);

  if (mouseData_.dragSide != CQChartsResizeSide::NONE) {
    setDragObj(axis == xAxis() ? DragObjType::XAXIS : DragObjType::YAXIS, axis);

    axis->editPress(w);

    axis->editHandles()->setDragSide(mouseData_.dragSide);
    axis->editHandles()->setDragPos (w);

    invalidateOverlay();

    return true;
  }

  return false;
}

bool
CQChartsPlot::
titleEditPress(CQChartsTitle *title, const Point &w)
{
  if (! title)
    return false;

  if (! title->isEditable())
    return false;

  if (! title->isSelected())
    return false;

  // start drag on already selected title handle
  mouseData_.dragSide = title->editHandles()->inside(w);

  if (mouseData_.dragSide != CQChartsResizeSide::NONE) {
    setDragObj(DragObjType::TITLE, title);

    title->editPress(w);

    title->editHandles()->setDragSide(mouseData_.dragSide);
    title->editHandles()->setDragPos (w);

    invalidateOverlay();

    return true;
  }

  return false;
}

bool
CQChartsPlot::
annotationsEditPress(const Point &w)
{
  // start drag on already selected annotation handle
  for (const auto &annotation : annotations()) {
    if (! annotation->isVisible() || ! annotation->isEditable())
      continue;

    if (! annotation->isSelected())
      continue;

    mouseData_.dragSide = annotation->editHandles()->inside(w);

    if (mouseData_.dragSide != CQChartsResizeSide::NONE) {
      setDragObj(DragObjType::ANNOTATION, annotation);

      annotation->editHandles()->setDragSide(mouseData_.dragSide);
      annotation->editHandles()->setDragPos (w);

      invalidateOverlay();

      return true;
    }
  }

  return false;
}

bool
CQChartsPlot::
objectsEditPress(const Point &w, bool)
{
  // start drag on already selected object handle
  for (auto &plotObj : plotObjects()) {
    if (! plotObj->isEditable())
      continue;

    if (! plotObj->isSelected())
      continue;

    mouseData_.dragSide = plotObj->editHandles()->inside(w);

    if (mouseData_.dragSide != CQChartsResizeSide::NONE) {
      setDragObj(DragObjType::OBJECT, plotObj);

      plotObj->editHandles()->setDragSide(mouseData_.dragSide);
      plotObj->editHandles()->setDragPos (w);

      invalidateOverlay();

      return true;
    }
  }

  return false;
}

//---

bool
CQChartsPlot::
keyEditSelect(CQChartsPlotKey *key, const Point &w)
{
  if (! key)
    return false;

  if (! key->isEditable())
    return false;

  if (! key->contains(w))
    return false;

  // select/deselect key
  if (! key->isSelected()) {
    selectOneObj(key, /*allObjs*/false);
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
    selectOneObj(axis, /*allObjs*/false);
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
    selectOneObj(title, /*allObjs*/false);
    return true;
  }

  if (title->editPress(w)) {
    setDragObj(DragObjType::TITLE, title);
    invalidateOverlay();
    return true;
  }

  return false;
}

bool
CQChartsPlot::
annotationsEditSelect(const Point &w)
{
  Annotations annotations;

  annotationsAtPoint(w, annotations);

  for (const auto &annotation : annotations) {
    if (! annotation->isVisible() || ! annotation->isEditable())
      continue;

    if (! annotation->contains(w))
      continue;

    if (! annotation->isSelected()) {
      selectOneObj(annotation, /*allObjs*/false);
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

  // select/deselect plot
  // (to select point must be inside a plot object)
  Objs objs;

  objsAtPoint(w, objs, Constraints::EDITABLE);

  for (const auto &obj : objs) {
    auto *plotObj = dynamic_cast<PlotObj *>(obj);
    if (! plotObj) continue;

    if (! plotObj->isEditable())
      continue;

    if (! plotObj->isSelected()) {
      selectOneObj(plotObj, /*allObjs*/false);
      return true;
    }

    if (plotObj->editPress(w)) {
      setDragObj(DragObjType::OBJECT, plotObj);
      invalidateOverlay();
      return true;
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
selectOneObj(Obj *obj, bool allObjs)
{
  startSelection();

  if (allObjs)
    deselectAllObjs();

  view()->deselectAll();

  obj->setSelected(true);

  endSelection();

  invalidateOverlay();
}

void
CQChartsPlot::
deselectAllObjs()
{
  startSelection();

  //---

  if (isOverlay()) {
    processOverlayPlots([&](CQChartsPlot *plot) {
      for (auto &plotObj : plot->plotObjects())
        plotObj->setSelected(false);
    });
  }
  else {
    for (auto &plotObj : plotObjects())
      plotObj->setSelected(false);
  }

  //---

  endSelection();
}

void
CQChartsPlot::
deselectAll()
{
  bool changed = false;

  //---

  if (isOverlay()) {
    processOverlayPlots([&](CQChartsPlot *plot) {
      plot->deselectAll1(changed);
    });
  }
  else {
    deselectAll1(changed);
  }

  //---

  if (changed) {
    endSelection();

    invalidateOverlay();
  }
}

void
CQChartsPlot::
deselectAll1(bool &changed)
{
  auto updateChanged = [&] {
    if (! changed) { startSelection(); changed = true; }
  };

  auto deselectObjs = [&](std::initializer_list<Obj *> objs) {
    for (const auto &obj : objs) {
      if (obj && obj->isSelected()) {
        obj->setSelected(false);

        updateChanged();
      }
    }
  };

  deselectObjs({key(), xAxis(), yAxis(), title()});

  //---

  for (auto &annotation : annotations()) {
    if (annotation->isSelected()) {
      annotation->setSelected(false);

      updateChanged();
    }
  }

  for (auto &plotObj : plotObjects()) {
    if (plotObj->isSelected()) {
      plotObj->setSelected(false);

      updateChanged();
    }
  }

  if (isSelected()) {
    setSelected(false);

    updateChanged();
  }
}

//------

bool
CQChartsPlot::
hasXAxis() const
{
  return type()->hasXAxis();
}

bool
CQChartsPlot::
hasYAxis() const
{
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

  return editMove(p, w, first);
}

bool
CQChartsPlot::
editMove(const Point &p, const Point &w, bool /*first*/)
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
  else if (mouseData_.dragObjType == DragObjType::XAXIS) {
    if (xAxis()->editMove(w))
      mouseData_.dragged = true;
  }
  else if (mouseData_.dragObjType == DragObjType::YAXIS) {
    if (yAxis()->editMove(w))
      mouseData_.dragged = true;
  }
  else if (mouseData_.dragObjType == DragObjType::TITLE) {
    if (title()->editMove(w))
      mouseData_.dragged = true;
  }
  else if (mouseData_.dragObjType == DragObjType::ANNOTATION) {
    bool edited = false;

    for (const auto &annotation : annotations()) {
      if (! annotation->isSelected())
        continue;

      if (annotation->editMove(w))
        mouseData_.dragged = true;

      edited = true;
    }

    if (! edited)
      return false;

    invalidateLayer(Buffer::Type::BACKGROUND);
    invalidateLayer(Buffer::Type::FOREGROUND);

    invalidateOverlay();
  }
  else if (mouseData_.dragObjType == DragObjType::OBJECT) {
    bool edited = false;

    for (const auto &plotObj : plotObjects()) {
      if (! plotObj->isEditable())
        continue;

      if (! plotObj->isSelected())
        continue;

      if (plotObj->editMove(w))
        mouseData_.dragged = true;

      edited = true;
    }

    if (! edited)
      return false;
  }
  else if (mouseData_.dragObjType == DragObjType::PLOT ||
           mouseData_.dragObjType == DragObjType::PLOT_HANDLE) {
    double dx = mouseData_.movePoint.x - lastMovePoint.x;
    double dy = lastMovePoint.y - mouseData_.movePoint.y;

    double dx1 =  view()->pixelToSignedWindowWidth (dx);
    double dy1 = -view()->pixelToSignedWindowHeight(dy);

    if (isOverlay()) {
      processOverlayPlots([&](CQChartsPlot *plot) {
        if (mouseData_.dragObjType == DragObjType::PLOT)
          plot->viewBBox_.moveBy(Point(dx1, dy1));
        else {
          editHandles_->updateBBox(dx1, dy1);

          plot->viewBBox_ = editHandles_->bbox();
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
      if (mouseData_.dragObjType == DragObjType::PLOT)
        viewBBox_.moveBy(Point(dx1, dy1));
      else {
        editHandles_->updateBBox(dx1, dy1);

        viewBBox_ = editHandles_->bbox();
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

    view()->update();
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

  return editMotion(p, w);
}

bool
CQChartsPlot::
editMotion(const Point &, const Point &w)
{
  if (isOverlay() && ! isFirstPlot())
    return false;

  //---

  if      (isSelected()) {
    auto v = windowToView(w);

    if (! editHandles_->selectInside(v))
      return false;
  }
  else if (key() && key()->isSelected()) {
    if (! key()->editMotion(w))
      return false;
  }
  else if (xAxis() && xAxis()->isSelected()) {
    if (! xAxis()->editMotion(w))
      return false;
  }
  else if (yAxis() && yAxis()->isSelected()) {
    if (! yAxis()->editMotion(w))
      return false;
  }
  else if (title() && title()->isSelected()) {
    if (! title()->editMotion(w))
      return false;
  }
  else {
    bool inside = false;

    for (const auto &annotation : annotations()) {
      if (! annotation->isSelected())
        continue;

      if (annotation->editMotion(w)) {
        inside = true;
        break;
      }
    }

    for (const auto &plotObj : plotObjects()) {
      if (! plotObj->isEditable())
        continue;

      if (! plotObj->isSelected())
        continue;

      if (plotObj->editMotion(w)) {
        inside = true;
        break;
      }
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

  return editRelease(p, w);
}

bool
CQChartsPlot::
editRelease(const Point &, const Point &w)
{
  if (isOverlay() && ! isFirstPlot())
    return false;

  //---

  if (mouseData_.dragObj) {
    if (mouseData_.dragObjType == DragObjType::PLOT ||
        mouseData_.dragObjType == DragObjType::PLOT_HANDLE) {
      auto *plot = dynamic_cast<CQChartsPlot *>(mouseData_.dragObj);
      assert(plot);
    }
    else if (mouseData_.dragObjType == DragObjType::OBJECT) {
      auto *plotObj = dynamic_cast<PlotObj *>(mouseData_.dragObj);
      assert(plotObj);

      plotObj->editRelease(w);
    }
    else if (mouseData_.dragObjType == DragObjType::XAXIS ||
             mouseData_.dragObjType == DragObjType::YAXIS) {
      auto *axis = dynamic_cast<CQChartsAxis *>(mouseData_.dragObj);
      assert(axis);

      axis->editRelease(w);
    }
    else if (mouseData_.dragObjType == DragObjType::KEY ||
             mouseData_.dragObjType == DragObjType::TITLE ||
             mouseData_.dragObjType == DragObjType::ANNOTATION) {
      auto *obj = dynamic_cast<CQChartsViewPlotObj *>(mouseData_.dragObj);
      assert(obj);

      obj->editRelease(w);
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
editMoveBy(const Point &d)
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
flipSelected(Qt::Orientation orient)
{
  for (auto &annotation : annotations()) {
    if (annotation->isSelected())
      annotation->flip(orient);
  }

  //---

  if (isOverlay()) {
    processOverlayPlots([&](CQChartsPlot *plot) {
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

  if (xAxis() && xAxis()->isVisible())
    objs.push_back(xAxis());

  if (yAxis() && yAxis()->isVisible())
    objs.push_back(yAxis());
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
    processOverlayPlots([&](CQChartsPlot *plot) {
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

  objsIntersectRect(r, objs, view()->isSelectInside(), /*select*/true);

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
    beginSelectIndex();

    for (const auto &objSelected : objsSelected) {
      auto *selectPlotObj = dynamic_cast<PlotObj *>(objSelected.first);

      if (! selectPlotObj || ! selectPlotObj->isSelected())
        continue;

      selectPlotObj->addSelectIndices();
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
startSelection()
{
  view()->startSelection();
}

void
CQChartsPlot::
endSelection()
{
  view()->endSelection();

  emit selectionChanged();
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
  for (auto &plotObj : plotObjects()) {
    if (plotObj->isSelected())
      plotObjs.push_back(plotObj);
  }
}

void
CQChartsPlot::
setPlotObjTreeSet(bool b)
{
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

    emit controlColumnsChanged();
  } );
}

//---

void
CQChartsPlot::
setColorColumn(const Column &c)
{
  CQChartsUtil::testAndSet(colorColumnData_.column, c, [&]() { updateObjs(); } );
}

void
CQChartsPlot::
setColorType(const ColorType &t)
{
  CQChartsUtil::testAndSet(colorColumnData_.colorType, t, [&]() { updateObjs(); } );
}

void
CQChartsPlot::
setColorMapped(bool b)
{
  CQChartsUtil::testAndSet(colorColumnData_.mapped, b, [&]() { updateObjs(); } );
}

void
CQChartsPlot::
setColorMapMin(double r)
{
  CQChartsUtil::testAndSet(colorColumnData_.map_min, r, [&]() { updateObjs(); } );
}

void
CQChartsPlot::
setColorMapMax(double r)
{
  CQChartsUtil::testAndSet(colorColumnData_.map_max, r, [&]() { updateObjs(); } );
}

void
CQChartsPlot::
setColorMapPalette(const PaletteName &name)
{
  CQChartsUtil::testAndSet(colorColumnData_.palette, name, [&]() { updateObjs(); } );
}

void
CQChartsPlot::
setColorXStops(const ColorStops &s)
{
  CQChartsUtil::testAndSet(colorColumnData_.xStops, s, [&]() { updateObjs(); } );
}

void
CQChartsPlot::
setColorYStops(const ColorStops &s)
{
  CQChartsUtil::testAndSet(colorColumnData_.yStops, s, [&]() { updateObjs(); } );
}

//------

void
CQChartsPlot::
initColorColumnData()
{
  CQPerfTrace trace("CQChartsPlot::initColorColumnData");

  std::unique_lock<std::mutex> lock(colorMutex_);

  //---

  colorColumnData_.valid = false;

  if (! colorColumn().isValid())
    return;

  //---

  auto *columnDetails = this->columnDetails(colorColumn());
  if (! columnDetails) return;

  if (colorColumn().isGroup()) {
    colorColumnData_.data_min = 0.0;
    colorColumnData_.data_max = std::max(numGroups() - 1, 0);
  }
  else {
    if (colorColumnData_.mapped) {
      auto minVar = columnDetails->minValue();
      auto maxVar = columnDetails->maxValue();

      bool ok;

      colorColumnData_.data_min = CQChartsVariant::toReal(minVar, ok);
      if (! ok) colorColumnData_.data_min = 0.0;

      colorColumnData_.data_max = CQChartsVariant::toReal(maxVar, ok);
      if (! ok) colorColumnData_.data_max = 1.0;
    }

    CQChartsModelTypeData columnTypeData;

    (void) CQChartsModelUtil::columnValueType(charts(), model().data(), colorColumn(),
                                              columnTypeData);

    if (columnTypeData.type == ColumnType::COLOR) {
      auto *columnTypeMgr = charts()->columnTypeMgr();

      const auto *colorType = dynamic_cast<const CQChartsColumnColorType *>(
        columnTypeMgr->getType(columnTypeData.type));
      assert(colorType);

      colorType->getMapData(charts(), model().data(), colorColumn(),
                            columnTypeData.nameValues, colorColumnData_.mapped,
                            colorColumnData_.data_min, colorColumnData_.data_max,
                            colorColumnData_.palette);
    }

    colorColumnData_.modelType = columnTypeData.type;
  }

  colorColumnData_.valid = true;
}

// get color from colorColumn at specified row
bool
CQChartsPlot::
colorColumnColor(int row, const QModelIndex &parent, Color &color) const
{
  auto *th = const_cast<CQChartsPlot *>(this);

  ModelIndex colorInd(th, row, colorColumn(), parent);

  return modelIndexColor(colorInd, color);
}

bool
CQChartsPlot::
modelIndexColor(const ModelIndex &colorInd, Color &color) const
{
  if (! isColorMapped())
    return false;

  // get model edit value
  bool ok;

  auto var = modelValue(colorInd, ok);
  if (! ok || ! var.isValid()) return false;

  return columnValueColor(var, color);
}

bool
CQChartsPlot::
columnValueColor(const QVariant &var, Color &color) const
{
  auto colorFromPaletteValue = [&](double r) {
    // use named palette if defined or current palette value
    Color color;

    if (colorMapPalette().isValid()) {
      auto *palette = colorMapPalette().palette();

      if (palette)
        color = palette->getColor(r);
      else
        color = Color(Color::Type::PALETTE_VALUE, r);
    }
    else
      color = Color(Color::Type::PALETTE_VALUE, r);

    return color;
  };

  //---

  if (CQChartsVariant::isNumeric(var)) {
    // get real value
    bool ok;
    double r = CQChartsVariant::toReal(var, ok);
    if (! ok) return false;

    //--

    // map real from data range if enabled
    double r1;

    if (isColorMapped())
      r1 = CMathUtil::map(r, colorMapDataMin(), colorMapDataMax(), 0.0, 1.0);
    else
      r1 = r;

    // skip if invalid value
    if (r1 < 0.0 || r1 > 1.0) return false;

    //--

    color = colorFromPaletteValue(r1);
  }
  else if (CQChartsVariant::isColor(var)) {
    // use color value directly
    bool ok;
    color = CQChartsVariant::toColor(var, ok);
  }
  else {
    if (isColorMapped()) {
      // use index of value in unique values to generate value in range
      auto *columnDetails = this->columnDetails(colorColumn());
      if (! columnDetails) return false;

      // use unique index/count of edit values (which may have been converted)
      // not same as CQChartsColumnColorType::userData
      int n = columnDetails->numUnique();
      int i = columnDetails->valueInd(var);

      double r = CMathUtil::map(i, 0, n - 1, 0.0, 1.0);

      color = colorFromPaletteValue(r);
    }
    else {
      bool ok;
      auto str = CQChartsVariant::toString(var, ok);

      color = Color(str);
    }
  }

  return color.isValid();
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
  auto *th = const_cast<CQChartsPlot *>(this);

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
  if (CQChartsVariant::isNumeric(var)) {
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
  symbolTypeData.valid = false;

  if (! symbolTypeData.column.isValid())
    return;

  auto *columnDetails = this->columnDetails(symbolTypeData.column);
  if (! columnDetails) return;

  if (symbolTypeData.column.isGroup()) {
    symbolTypeData.data_min = 0;
    symbolTypeData.data_max = std::max(numGroups() - 1, 0);
  }
  else {
    if (symbolTypeData.mapped) {
      auto minVar = columnDetails->minValue();
      auto maxVar = columnDetails->maxValue();

      bool ok;

      symbolTypeData.data_min = int(CQChartsVariant::toReal(minVar, ok));
      if (! ok) symbolTypeData.data_min = 0;

      symbolTypeData.data_max = int(CQChartsVariant::toReal(maxVar, ok));
      if (! ok) symbolTypeData.data_max = 1;
    }

    CQChartsModelTypeData columnTypeData;

    (void) CQChartsModelUtil::columnValueType(charts(), model().data(), symbolTypeData.column,
                                              columnTypeData);

    if (columnTypeData.type == ColumnType::SYMBOL) {
      auto *columnTypeMgr = charts()->columnTypeMgr();

      const auto *symbolTypeType = dynamic_cast<const CQChartsColumnSymbolTypeType *>(
        columnTypeMgr->getType(columnTypeData.type));
      assert(symbolTypeType);

      symbolTypeType->getMapData(charts(), model().data(), symbolTypeData.column,
                                 columnTypeData.nameValues, symbolTypeData.mapped,
                                 symbolTypeData.map_min, symbolTypeData.map_max,
                                 symbolTypeData.data_min, symbolTypeData.data_max);
    }
  }

  symbolTypeData.valid = true;
}

bool
CQChartsPlot::
columnSymbolType(int row, const QModelIndex &parent, const SymbolTypeData &symbolTypeData,
                 Symbol &symbolType) const
{
  if (! symbolTypeData.valid)
    return false;

  auto *th = const_cast<CQChartsPlot *>(this);

  ModelIndex symbolTypeModelInd(th, row, symbolTypeData.column, parent);

  bool ok;

  auto var = modelValue(symbolTypeModelInd, ok);
  if (! ok || ! var.isValid()) return false;

  if (CQChartsVariant::isNumeric(var)) {
    int i = (int) CQChartsVariant::toInt(var, ok);
    if (! ok) return false;

    if (symbolTypeData.mapped) {
      // map value in range (symbolTypeData.data_min, symbolTypeData.data_max) to
      // (symbolTypeData.map_min, symbolTypeData.map_max)
      int i1 = (int) CMathUtil::map(i, symbolTypeData.data_min, symbolTypeData.data_max,
                                    symbolTypeData.map_min, symbolTypeData.map_max);

      symbolType = Symbol::outlineFromInt(i1);
    }
    else {
      // use value directly for type
      symbolType = Symbol::outlineFromInt(i);
    }
  }
  else if (CQChartsVariant::isSymbol(var)) {
    // use symbol directly for type
    symbolType = CQChartsVariant::toSymbol(var, ok);
  }
  else {
    if (symbolTypeData.mapped) {
      // use index of value in unique values to generate value in range
      // (CQChartsSymbolSize::minValue, CQChartsSymbolSize::maxValue)
      auto *columnDetails = this->columnDetails(symbolTypeData.column);
      if (! columnDetails) return false;

      // use unique index/count of edit values (which may have been converted)
      // not same as CQChartsColumnColorType::userData
      // TODO: use map min/max
      int n = columnDetails->numUnique();
      int i = columnDetails->valueInd(var);

      double r = CMathUtil::map(i, 0, n - 1, 0.0, 1.0);

      symbolType = Symbol::interpOutline(r);
    }
    else {
      auto str = CQChartsVariant::toString(var, ok);

      symbolType = Symbol(str);
    }
  }

  return symbolType.isValid();
}

//------

void
CQChartsPlot::
initSymbolSizeData(SymbolSizeData &symbolSizeData) const
{
  symbolSizeData.valid = false;

  if (! symbolSizeData.column.isValid())
    return;

  auto *columnDetails = this->columnDetails(symbolSizeData.column);
  if (! columnDetails) return;

  if (symbolSizeData.column.isGroup()) {
    symbolSizeData.data_min  = 0.0;
    symbolSizeData.data_max  = std::max(numGroups() - 1, 0);
    symbolSizeData.data_mean = symbolSizeData.data_max/2.0;
  }
  else {
    if (symbolSizeData.mapped) {
      auto minVar  = columnDetails->minValue();
      auto maxVar  = columnDetails->maxValue();
      auto meanVar = columnDetails->meanValue();

      bool ok;

      symbolSizeData.data_min = CQChartsVariant::toReal(minVar, ok);
      if (! ok) symbolSizeData.data_min = 0.0;

      symbolSizeData.data_max = CQChartsVariant::toReal(maxVar, ok);
      if (! ok) symbolSizeData.data_max = 1.0;

      symbolSizeData.data_mean = CQChartsVariant::toReal(meanVar, ok);
      if (! ok) symbolSizeData.data_mean =
                  CMathUtil::avg(symbolSizeData.data_min, symbolSizeData.data_max);
    }

    CQChartsModelTypeData columnTypeData;

    (void) CQChartsModelUtil::columnValueType(charts(), model().data(), symbolSizeData.column,
                                              columnTypeData);

    if (columnTypeData.type == ColumnType::SYMBOL_SIZE) {
      auto *columnTypeMgr = charts()->columnTypeMgr();

      const auto *symbolSizeType = dynamic_cast<const CQChartsColumnSymbolSizeType *>(
        columnTypeMgr->getType(columnTypeData.type));
      assert(symbolSizeType);

      symbolSizeType->getMapData(charts(), model().data(), symbolSizeData.column,
                                 columnTypeData.nameValues, symbolSizeData.mapped,
                                 symbolSizeData.map_min, symbolSizeData.map_max,
                                 symbolSizeData.data_min, symbolSizeData.data_max);
    }
  }

  symbolSizeData.valid = true;
}

bool
CQChartsPlot::
columnSymbolSize(int row, const QModelIndex &parent, const SymbolSizeData &symbolSizeData,
                 Length &symbolSize) const
{
  if (! symbolSizeData.valid)
    return false;

  auto *th = const_cast<CQChartsPlot *>(this);

  auto units = CQChartsUnits::PIXEL;

  (void) CQChartsUtil::decodeUnits(symbolSizeData.units, units);

  ModelIndex symbolSizeModelInd(th, row, symbolSizeData.column, parent);

  bool ok;

  auto var = modelValue(symbolSizeModelInd, ok);
  if (! ok || ! var.isValid()) return false;

  if (CQChartsVariant::isNumeric(var)) {
    if (symbolSizeData.mapped) {
      // map value in range (symbolSizeData.data_min, symbolSizeData.data_max) to
      // (symbolSizeData.map_min, symbolSizeData.map_max)
      double r = CQChartsVariant::toReal(var, ok);
      if (! ok) return false;

      double r1 = CMathUtil::map(r, symbolSizeData.data_min, symbolSizeData.data_max,
                                 symbolSizeData.map_min, symbolSizeData.map_max);

      symbolSize = Length(r1, units);
    }
    else {
      // use value directly for size
      symbolSize = CQChartsVariant::toLength(var, ok);
    }
  }
  else if (CQChartsVariant::isLength(var)) {
    // use length directly for size
    symbolSize = CQChartsVariant::toLength(var, ok);
  }
  else {
    if (symbolSizeData.mapped) {
      // use index of value in unique values to generate value in range
      // (CQChartsSymbolSize::minValue, CQChartsSymbolSize::maxValue)
      auto *columnDetails = this->columnDetails(symbolSizeData.column);
      if (! columnDetails) return false;

      // use unique index/count of edit values (which may have been converted)
      // not same as CQChartsColumnColorSize::userData
      // TODO: use map min/max
      int n = columnDetails->numUnique();
      int i = columnDetails->valueInd(var);

      double r = CMathUtil::map(i, 0, n - 1, CQChartsSymbolSize::minValue(),
                                CQChartsSymbolSize::maxValue());

      symbolSize = Length(r, units);
    }
    else {
      auto str = CQChartsVariant::toString(var, ok);

      symbolSize = Length(str, units);
    }
  }

  return symbolSize.isValid();
}

//------

void
CQChartsPlot::
initFontSizeData(FontSizeData &fontSizeData) const
{
  fontSizeData.valid = false;

  if (! fontSizeData.column.isValid())
    return;

  auto *columnDetails = this->columnDetails(fontSizeData.column);
  if (! columnDetails) return;

  if (fontSizeData.column.isGroup()) {
    fontSizeData.data_min = 0.0;
    fontSizeData.data_max = std::max(numGroups() - 1, 0);
  }
  else {
    if (fontSizeData.mapped) {
      auto minVar = columnDetails->minValue();
      auto maxVar = columnDetails->maxValue();

      bool ok;

      fontSizeData.data_min = CQChartsVariant::toReal(minVar, ok);
      if (! ok) fontSizeData.data_min = 0.0;

      fontSizeData.data_max = CQChartsVariant::toReal(maxVar, ok);
      if (! ok) fontSizeData.data_max = 1.0;
    }

    CQChartsModelTypeData columnTypeData;

    (void) CQChartsModelUtil::columnValueType(charts(), model().data(), fontSizeData.column,
                                              columnTypeData);

    if (columnTypeData.type == ColumnType::FONT_SIZE) {
      auto *columnTypeMgr = charts()->columnTypeMgr();

      const auto *fontSizeType = dynamic_cast<const CQChartsColumnFontSizeType *>(
        columnTypeMgr->getType(columnTypeData.type));
      assert(fontSizeType);

      fontSizeType->getMapData(charts(), model().data(), fontSizeData.column,
                               columnTypeData.nameValues, fontSizeData.mapped,
                               fontSizeData.map_min, fontSizeData.map_max,
                               fontSizeData.data_min, fontSizeData.data_max);
    }
  }

  fontSizeData.valid = true;
}

bool
CQChartsPlot::
columnFontSize(int row, const QModelIndex &parent, const FontSizeData &fontSizeData,
               Length &fontSize) const
{
  if (! fontSizeData.valid)
    return false;

  auto *th = const_cast<CQChartsPlot *>(this);

  auto units = CQChartsUnits::PIXEL;

  (void) CQChartsUtil::decodeUnits(fontSizeData.units, units);

  ModelIndex fontSizeModelInd(th, row, fontSizeData.column, parent);

  bool ok;

  auto var = modelValue(fontSizeModelInd, ok);
  if (! ok || ! var.isValid()) return false;

  if (CQChartsVariant::isNumeric(var)) {
    if (fontSizeData.mapped) {
      // map value in range (fontSizeData.map_min, fontSizeData.data_max) to
      // (fontSizeData.map_min, fontSizeData.map_max)
      double r = CQChartsVariant::toReal(var, ok);
      if (! ok) return false;

      double r1 = CMathUtil::map(r, fontSizeData.data_min, fontSizeData.data_max,
                                 fontSizeData.map_min, fontSizeData.map_max);

      fontSize = Length(r1, units);
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
    if (fontSizeData.mapped) {
      // use index of value in unique values to generate value in range
      // (CQChartsSymbolSize::minValue, CQChartsSymbolSize::maxValue)
      auto *columnDetails = this->columnDetails(fontSizeData.column);
      if (! columnDetails) return false;

      // use unique index/count of edit values (which may have been converted)
      // not same as CQChartsColumnColorSize::userData
      // TODO: use map min/max
      int n = columnDetails->numUnique();
      int i = columnDetails->valueInd(var);

      double r = CMathUtil::map(i, 0, n - 1, CQChartsFontSize::minValue(),
                                CQChartsFontSize::maxValue());

      fontSize = Length(r, units);
    }
    else {
      auto str = CQChartsVariant::toString(var, ok);

      fontSize = Length(str, units);
    }
  }

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

  auto *model = this->model().data();

  if (! model)
    return CQChartsUtil::formatReal(r);

  QString str;

  QVariant var(r);

  if (! CQChartsModelUtil::formatColumnValue(charts(), model, column, var, str))
    return CQChartsUtil::formatReal(r);

  return str;
}

void
CQChartsPlot::
keyPress(int key, int modifier)
{
  bool is_shift = (modifier & Qt::ShiftModifier  );
//bool is_ctrl  = (modifier & Qt::ControlModifier);

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
        editMoveBy(Point( getMoveX(is_shift), 0));
      else if (key == Qt::Key_Left)
        editMoveBy(Point(-getMoveX(is_shift), 0));
      else if (key == Qt::Key_Up)
        editMoveBy(Point(0, getMoveY(is_shift)));
      else if (key == Qt::Key_Down)
        editMoveBy(Point(0, -getMoveY(is_shift)));
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
  else
    return;
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

void
CQChartsPlot::
updateSlot()
{
  drawObjs();
}

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
  if (! insideObjs_.empty()) {
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

void
CQChartsPlot::
panLeft(double f)
{
  if (! allowPanX())
    return;

  double dx = viewToWindowWidth(f)/getDataRange().getWidth();

  auto panX = [&](CQChartsPlot *plot) {
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

  emit zoomPanChanged();
}

void
CQChartsPlot::
panRight(double f)
{
  if (! allowPanX())
    return;

  double dx = viewToWindowWidth(f)/getDataRange().getWidth();

  auto panX = [&](CQChartsPlot *plot) {
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

  emit zoomPanChanged();
}

void
CQChartsPlot::
panUp(double f)
{
  if (! allowPanY())
    return;

  double dy = viewToWindowHeight(f)/getDataRange().getHeight();

  auto panY = [&](CQChartsPlot *plot) {
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

  emit zoomPanChanged();
}

void
CQChartsPlot::
panDown(double f)
{
  if (! allowPanY())
    return;

  double dy = viewToWindowHeight(f)/getDataRange().getHeight();

  auto panY = [&](CQChartsPlot *plot) {
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

  emit zoomPanChanged();
}

void
CQChartsPlot::
pan(double dx, double dy)
{
  if (allowPanX())
    setDataOffsetX(dataOffsetX() + dx/getDataRange().getWidth());

  if (allowPanY())
    setDataOffsetY(dataOffsetY() + dy/getDataRange().getHeight());

  adjustPan();

  applyDataRangeAndDraw();

  emit zoomPanChanged();
}

//---

void
CQChartsPlot::
zoomIn(double f)
{
  if (allowZoomX())
    setDataScaleX(dataScaleX()*f);

  if (allowZoomY())
    setDataScaleY(dataScaleY()*f);

  applyDataRangeAndDraw();

  emit zoomPanChanged();
}

void
CQChartsPlot::
zoomOut(double f)
{
  if (allowZoomX())
    setDataScaleX(dataScaleX()/f);

  if (allowZoomY())
    setDataScaleY(dataScaleY()/f);

  applyDataRangeAndDraw();

  emit zoomPanChanged();
}

void
CQChartsPlot::
zoomTo(const BBox &bbox)
{
  if (! dataRange_.isSet())
    return;

  double w = bbox.getWidth ();
  double h = bbox.getHeight();

  if (w < 1E-50 || h < 1E-50) {
    double dataScale = 2*std::min(dataScaleX(), dataScaleY());

    w = dataRange_.xsize()/dataScale;
    h = dataRange_.ysize()/dataScale;
  }

  auto c = bbox.getCenter();

  double w1 = dataRange_.xsize();
  double h1 = dataRange_.ysize();

  double xscale = w1/w;
  double yscale = h1/h;

  //setDataScaleX(std::min(xscale, yscale));
  //setDataScaleY(std::min(xscale, yscale));

  if (allowZoomX())
    setDataScaleX(xscale);

  if (allowZoomY())
    setDataScaleY(yscale);

  auto c1 = Point(dataRange_.xmid(), dataRange_.ymid());

  double cx = (allowPanX() ? c.x - c1.x : 0.0)/getDataRange().getWidth ();
  double cy = (allowPanY() ? c.y - c1.y : 0.0)/getDataRange().getHeight();

  setDataOffsetX(cx);
  setDataOffsetY(cy);

  applyDataRangeAndDraw();

  emit zoomPanChanged();
}

void
CQChartsPlot::
unzoomTo(const BBox &bbox)
{
  if (! dataRange_.isSet())
    return;

  double w = bbox.getWidth ();
  double h = bbox.getHeight();

  auto c = bbox.getCenter();

  double w1 = dataRange_.xsize();
  double h1 = dataRange_.ysize();

  if (w1 < 1E-50 || h1 < 1E-50)
    return;

  double xscale = w*dataScaleX()/w1;
  double yscale = h*dataScaleY()/h1;

  if (allowZoomX())
    setDataScaleX(xscale*dataScaleX());

  if (allowZoomY())
    setDataScaleY(yscale*dataScaleY());

  auto c1 = Point(dataRange_.xmid(), dataRange_.ymid());

  double cx = (allowPanX() ? c.x - c1.x : 0.0)/getDataRange().getWidth ();
  double cy = (allowPanY() ? c.y - c1.y : 0.0)/getDataRange().getHeight();

  setDataOffsetX(cx);
  setDataOffsetY(cy);

  applyDataRangeAndDraw();

  emit zoomPanChanged();
}

bool
CQChartsPlot::
isZoomFull() const
{
  if (allowZoomX() && ! CMathUtil::realEq(dataScaleX(), 1.0))
    return false;

  if (allowZoomY() && ! CMathUtil::realEq(dataScaleY(), 1.0))
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
  if (allowZoomX())
    setDataScaleX(1.0);

  if (allowZoomY())
    setDataScaleY(1.0);

  setDataOffsetX(0.0);
  setDataOffsetY(0.0);

  applyDataRangeAndDraw();

  if (notify)
    emit zoomPanChanged();
}

void
CQChartsPlot::
centerAt(const Point &c)
{
  if (! dataRange_.isSet())
    return;

  auto c1 = Point(dataRange_.xmid(), dataRange_.ymid());

  double cx = (allowPanX() ? c.x - c1.x : 0.0)/getDataRange().getWidth ();
  double cy = (allowPanY() ? c.y - c1.y : 0.0)/getDataRange().getHeight();

  setDataOffsetX(cx);
  setDataOffsetY(cy);

  applyDataRangeAndDraw();

  emit zoomPanChanged();
}

void
CQChartsPlot::
updateTransform()
{
  postResize();

  drawObjs();
}

//------

bool
CQChartsPlot::
tipText(const Point &p, QString &tip) const
{
  if (isOverlay() && ! isFirstPlot())
    return false;

  return plotTipText(p, tip, /*single*/true);
}

bool
CQChartsPlot::
plotTipText(const Point &p, QString &tip, bool single) const
{
  int objNum  = 0;
  int numObjs = 0;

  Obj *tipObj = nullptr;

  Objs tipObjs;

  if (isFollowMouse()) {
    objNum = insideInd_;

    tipObj = insideObject();

    tipObjs.clear();

    for (const auto &obj : insideObjs_)
      tipObjs.push_back(obj);
  }
  else {
    objsAtPoint(p, tipObjs, Constraints::SELECTABLE);

    if (! tipObjs.empty())
      tipObj = *tipObjs.begin();
  }

  numObjs = tipObjs.size();

  if (tipObj) {
    if (single) {
      if (tip != "")
        tip += " ";

      tip += tipObj->tipId();

      if (numObjs > 1)
        tip += QString("<br><font color=\"blue\">&nbsp;&nbsp;%1 of %2</font>").
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
addTipColumns(CQChartsTableTip &tableTip, const QModelIndex &ind) const
{
  auto *th = const_cast<CQChartsPlot *>(this);

  for (const auto &c : tipColumns().columns()) {
    if (! c.isValid()) continue;

    if (tableTip.hasColumn(c))
      continue;

    ModelIndex tipModelInd(th, ind.row(), c, ind.parent());

    auto tipInd  = modelIndex(tipModelInd);
    auto tipInd1 = unnormalizeIndex(tipInd);

    ModelIndex tipModelInd1(th, tipInd1.row(), c, tipInd1.parent());

    bool ok1, ok2;

    auto name  = modelHHeaderString(c, ok1);
    auto value = modelString(tipModelInd1, ok2);

    if (ok1 && ok2)
      tableTip.addTableRow(name, value);

    tableTip.addColumn(c);
  }
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

void
CQChartsPlot::
objsAtPoint(const Point &p, Objs &objs, const Constraints &constraints) const
{
  PlotObjs plotObjs;

  plotObjsAtPoint1(p, plotObjs);

  int iconstraints = (int) constraints;

  for (const auto &plotObj : plotObjs) {
    if ((iconstraints & (int) Constraints::SELECTABLE) && ! plotObj->isSelectable())
      continue;

    if ((iconstraints & (int) Constraints::EDITABLE) && ! plotObj->isEditable())
      continue;

    objs.push_back(plotObj);
  }

  //---

  Annotations annotations;

  annotationsAtPoint(p, annotations);

  for (const auto &annotation : annotations) {
    if ((iconstraints & (int) Constraints::SELECTABLE) && ! annotation->isSelectable())
      continue;

    if ((iconstraints & (int) Constraints::EDITABLE) && ! annotation->isEditable())
      continue;

    objs.push_back(annotation);
  }
}

void
CQChartsPlot::
plotObjsAtPoint1(const Point &p, PlotObjs &plotObjs) const
{
  if (isOverlay()) {
    processOverlayPlots([&](const CQChartsPlot *plot) {
      auto p1 = p;

      if (plot != this)
        p1 = plot->pixelToWindow(windowToPixel(p));

      plot->objTreeData_.tree->objectsAtPoint(p1, plotObjs);
    });
  }
  else {
    plotObjsAtPoint(p, plotObjs);
  }
}

void
CQChartsPlot::
plotObjsAtPoint(const Point &p, PlotObjs &plotObjs) const
{
  objTreeData_.tree->objectsAtPoint(p, plotObjs);
}

void
CQChartsPlot::
annotationsAtPoint(const Point &p, Annotations &annotations) const
{
  annotations.clear();

  if (isOverlay()) {
    processOverlayPlots([&](const CQChartsPlot *plot) {
      auto p1 = p;

      if (plot != this)
        p1 = plot->pixelToWindow(windowToPixel(p));

      plot->annotationsAtPoint1(p1, annotations);
    });
  }
  else {
    annotationsAtPoint1(p, annotations);
  }
}

void
CQChartsPlot::
annotationsAtPoint1(const Point &p, Annotations &annotations) const
{
  for (const auto &annotation : this->annotations()) {
    if (! annotation->isVisible())
      continue;

    if (! annotation->contains(p))
      continue;

    annotations.push_back(annotation);
  }
}

void
CQChartsPlot::
objsIntersectRect(const BBox &r, Objs &objs, bool inside, bool select) const
{
  PlotObjs plotObjs;

  plotObjsIntersectRect(r, plotObjs, inside);

  for (const auto &plotObj : plotObjs) {
    if (select && ! plotObj->isSelectable())
      continue;

    objs.push_back(plotObj);
  }

  //---

  Annotations annotations;

  annotationsIntersectRect(r, annotations, inside);

  for (const auto &annotation : annotations) {
    if (select && ! annotation->isSelectable())
      continue;

    objs.push_back(annotation);
  }
}

void
CQChartsPlot::
plotObjsIntersectRect(const BBox &r, PlotObjs &plotObjs, bool inside) const
{
  if (isOverlay()) {
    processOverlayPlots([&](const CQChartsPlot *plot) {
      auto r1 = r;

      if (plot != this)
        r1 = windowToPixel(plot->pixelToWindow(r));

      plot->objTreeData_.tree->objectsIntersectRect(r1, plotObjs, inside);
    });
  }
  else {
    objTreeData_.tree->objectsIntersectRect(r, plotObjs, inside);
  }
}

void
CQChartsPlot::
annotationsIntersectRect(const BBox &r, Annotations &annotations, bool inside) const
{
  annotations.clear();

  if (isOverlay()) {
    processOverlayPlots([&](const CQChartsPlot *plot) {
      auto r1 = r;

      if (plot != this)
        r1 = plot->pixelToWindow(windowToPixel(r));

      plot->annotationsIntersectRect1(r1, annotations, inside);
    });
  }
  else {
    annotationsIntersectRect1(r, annotations, inside);
  }
}

void
CQChartsPlot::
annotationsIntersectRect1(const BBox &r, Annotations &annotations, bool inside) const
{
  for (const auto &annotation : this->annotations()) {
    if (! annotation->isVisible())
      continue;

    if (! annotation->intersects(r, inside))
      continue;

    annotations.push_back(annotation);
  }
}

bool
CQChartsPlot::
objNearestPoint(const Point &p, PlotObj* &obj) const
{
  obj = nullptr;

  double tx = dataRange_.xsize()/32.0;
  double ty = dataRange_.ysize()/32.0;

  return objTreeData_.tree->objectNearest(p, tx, ty, obj);
}

//---

void
CQChartsPlot::
preResize()
{
  std::unique_lock<std::mutex> lock(resizeMutex_);

  interruptDraw();
}

void
CQChartsPlot::
postResize()
{
  if (isOverlay() && ! isFirstPlot())
    return;

  applyDataRange();

  if (isEqualScale()) {
    resetDataRange(/*updateRange*/true, /*updateObjs*/false);
  }

#if 0
  // TODO: does obj postResize need range set ?
  for (auto &obj : plotObjects())
    obj->postResize();
#endif

  // TODO: does key position need range set ?
  updateKeyPosition(/*force*/true);

  if (isAutoFit())
    needsAutoFit_ = true;

  drawObjs();
}

void
CQChartsPlot::
updateKeyPosition(bool force)
{
  if (! isKeyVisibleAndNonEmpty())
    return;

  if (isOverlay() && ! isFirstPlot())
    return;

  if (force)
    key()->invalidateLayout();

  if (! dataRange_.isSet())
    return;

  key()->updatePlotLocation();
}

//------

bool
CQChartsPlot::
printLayer(Layer::Type type, const QString &filename) const
{
  auto *layer = getLayer(type);

  const auto *buffer = getBuffer(layer->buffer());

  if (! buffer->image())
    return false;

  buffer->image()->save(filename);

  return true;
}

//------

void
CQChartsPlot::
draw(QPainter *painter)
{
  if (! view()->isBufferLayers()) {
    initGroupedPlotObjs();

    //---

    drawParts(painter);

    //---

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

void
CQChartsPlot::
updateGroupedDraw()
{
  if (isOverlay()) {
    processOverlayPlots([&](CQChartsPlot *plot) {
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
    // ignore draw until after calc range and objs finished
    {
      LockMutex lock(this, "draw::updateDraw");

      auto updateState = this->updateState();

      if (updateState == UpdateState::CALC_RANGE)
        return;

      if (updateState == UpdateState::CALC_OBJS)
        return;

      interruptDraw();
    }

    //---

    {
    LockMutex lock(this, "draw::updateDraw");

    getBuffer(Buffer::Type::BACKGROUND)->setValid(false);
    getBuffer(Buffer::Type::MIDDLE    )->setValid(false);
    getBuffer(Buffer::Type::FOREGROUND)->setValid(false);

    updateData_.drawBusy.ind = -100;

    setGroupedUpdateState(UpdateState::DRAW_OBJS);

    updateData_.drawThread->exec(drawASync, this);

    startThreadTimer();
    }
  }
  else {
    // draw objs
    drawThread();

    postDraw();

    view()->update();
  }
}

void
CQChartsPlot::
interruptDraw()
{
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
    processOverlayPlots([&](CQChartsPlot *plot) {
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
drawASync(CQChartsPlot *plot)
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

  // mark thread done
  updateData_.drawThread->end();
}

void
CQChartsPlot::
drawBusy(QPainter *painter, const UpdateState &updateState) const
{
  if (updateData_.drawBusy.ind < 0) {
    ++updateData_.drawBusy.ind;
    return;
  }

  //---

  auto viewBBox = calcViewBBox();

  auto p1 = view()->windowToPixel(Point(viewBBox.getXMin(), viewBBox.getYMin()));
  auto p2 = view()->windowToPixel(Point(viewBBox.getXMax(), viewBBox.getYMax()));

  //---

  double x = (p1.x + p2.x)/2.0;
  double y = (p1.y + p2.y)/2.0;

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

    Point c(x, y);

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
    Color color(Color::Type::INTERFACE_VALUE, 1.0);

    auto tc = charts()->interpColor(color, ColorInd());

    painter->setPen(tc);

    auto font = view()->viewFont(updateData_.drawBusy.font);

    QFontMetricsF fm(font);

    double tw = fm.width(text);
    double ta = fm.ascent();

    double tx = x - tw/2.0;
    double ty = y + r1 + r3 + 4 + ta;

    painter->drawText(int(tx), int(ty), text);
  }
}

void
CQChartsPlot::
drawLayers(QPainter *painter) const
{
  for (auto &tb : buffers_) {
    auto *buffer = tb.second;

    if (buffer->isActive() && buffer->isValid())
      buffer->draw(painter);
  }
}

void
CQChartsPlot::
drawLayer(QPainter *painter, Layer::Type type) const
{
  auto *layer = getLayer(type);

  auto *buffer = getBuffer(layer->buffer());

  buffer->draw(painter, 0, 0);
}

void
CQChartsPlot::
drawParts(QPainter *painter) const
{
  CQPerfTrace trace("CQChartsPlot::drawParts");

  if (! calcVisible())
    return;

  //---

  drawBackgroundParts(painter);

  //---

  drawMiddleParts(painter);

  //---

  drawForegroundParts(painter);

  //---

  drawOverlayParts(painter);
}

void
CQChartsPlot::
drawBackgroundParts(QPainter *painter) const
{
  CQPerfTrace trace("CQChartsPlot::drawBackgroundParts");

  bool bgLayer       = hasBackgroundLayer();
  bool bgAxes        = hasGroupedBgAxes();
  bool bgKey         = hasGroupedBgKey();
  bool bgAnnotations = hasGroupedAnnotations(Layer::Type::BG_ANNOTATION);

  if (! bgLayer && ! bgAxes && ! bgKey && ! bgAnnotations)
    return;

  //---

  auto *buffer = getBuffer(Buffer::Type::BACKGROUND);
  if (! buffer->isActive()) return;

  auto *painter1 = beginPaint(buffer, painter);

  //---

  if (painter1) {
    auto *th = const_cast<CQChartsPlot *>(this);

    CQChartsPlotPaintDevice device(th, painter1);

    drawBackgroundDeviceParts(&device, bgLayer, bgAxes, bgKey, bgAnnotations);

    //---

    if (debugQuadTree_) {
      painter1->setPen(Qt::black);

      objTreeData_.tree->draw(painter1);
    }
  }

  //---

  endPaint(buffer);
}

void
CQChartsPlot::
drawBackgroundDeviceParts(PaintDevice *device, bool bgLayer, bool bgAxes,
                          bool bgKey, bool bgAnnotations) const
{
  const_cast<CQChartsPlot *>(this)->initAxisSizes();

  //---

  // draw background (plot/data fill)
  if (bgLayer)
    drawBackgroundLayer(device);

  //---

  // draw axes/key below plot
  if (bgAxes)
    drawGroupedBgAxes(device);

  if (bgKey)
    drawBgKey(device);

  //---

  // draw annotations
  if (bgAnnotations)
    drawGroupedAnnotations(device, Layer::Type::BG_ANNOTATION);
}

void
CQChartsPlot::
initAxisSizes()
{
  if (! isOverlay()) {
    const_cast<CQChartsPlot *>(this)->clearAxisSideDelta();
    return;
  }

  //---

  if (this == firstPlot())
    const_cast<CQChartsPlot *>(this)->clearAxisSideDelta();

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
      oplot->xAxisSideDelta_[CQChartsAxisSide::Type::BOTTOM_LEFT] = bdelta;

      if (! oplot->xAxis())
        continue;

      if (oplot->xAxis()->position().isSet())
        continue;

      bdelta += oplot->xAxisHeight(CQChartsAxisSide::Type::BOTTOM_LEFT);
    }

    // init top axes
    double tdelta = 0.0;

    for (auto &oplot : oplots) {
      oplot->xAxisSideDelta_[CQChartsAxisSide::Type::TOP_RIGHT] = tdelta;

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
      oplot->yAxisSideDelta_[CQChartsAxisSide::Type::BOTTOM_LEFT] = ldelta;

      if (! oplot->yAxis())
        continue;

      if (oplot->yAxis()->position().isSet())
        continue;

      ldelta += oplot->yAxisWidth(CQChartsAxisSide::Type::BOTTOM_LEFT);
    }

    // init right axes
    double rdelta = 0.0;

    for (auto &oplot : oplots) {
      oplot->yAxisSideDelta_[CQChartsAxisSide::Type::TOP_RIGHT] = rdelta;

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
    auto *th = const_cast<CQChartsPlot *>(this);

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

  bool fgAxes        = hasGroupedFgAxes();
  bool fgKey         = hasGroupedFgKey();
  bool fgAnnotations = hasGroupedAnnotations(Layer::Type::FG_ANNOTATION);
  bool title         = hasTitle();
  bool foreground    = hasForeground();

  if (! fgAxes && ! fgKey && ! fgAnnotations && ! title && ! foreground)
    return;

  //---

  auto *buffer = getBuffer(Buffer::Type::FOREGROUND);
  if (! buffer->isActive()) return;

  auto *painter1 = beginPaint(buffer, painter);

  //---

  if (painter1) {
    auto *th = const_cast<CQChartsPlot *>(this);

    CQChartsPlotPaintDevice device(th, painter1);

    bool tabbed = (isTabbed() && isCurrent());

    drawForegroundDeviceParts(&device, fgAxes, fgKey, fgAnnotations, title, foreground, tabbed);
  }

  //---

  endPaint(buffer);
}

void
CQChartsPlot::
drawForegroundDeviceParts(PaintDevice *device, bool fgAxes, bool fgKey, bool fgAnnotations,
                          bool title, bool foreground, bool tabbed) const
{
  // draw annotations
  if (fgAnnotations)
    drawGroupedAnnotations(device, Layer::Type::FG_ANNOTATION);

  //---

  // draw axes/key above plot
  if (fgAxes)
    drawGroupedFgAxes(device);

  if (fgKey)
    drawFgKey(device);

  //---

  // draw title
  if (title)
    drawTitle(device);

  //---

  // draw foreground
  if (foreground)
    execDrawForeground(device);

  //---

  if (tabbed)
    drawTabs(device);
}

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
  CQChartsPlot *currentPlot = nullptr;

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
drawTabs(PaintDevice *device, const Plots &plots, CQChartsPlot *currentPlot) const
{
  device->setFont(tabbedFont().font());

  calcTabData(plots);

  QFontMetrics fm(device->font());

  //---

  Color textColor       (Color::Type::INTERFACE_VALUE, 1.0);
  Color currentTextColor(Color::Type::INTERFACE_VALUE, 0.0);
  Color fillColor       (Color::Type::INTERFACE_VALUE, 0.3);
  Color currentFillColor(Color::Type::INTERFACE_VALUE, 0.6);
  Color borderColor     (Color::Type::INTERFACE_VALUE, 0.0);

  auto drawTab = [&](const BBox &rect, bool current) {
    device->setBrush(interpColor(current ? currentFillColor : fillColor, ColorInd()));

    device->fillRect(rect);

    device->setPen(interpColor(borderColor, ColorInd()));

    device->drawRect(rect);
  };

  auto drawText = [&](const BBox &rect, const QString &text, bool current) {
    auto xc = rect.getXMid();
    auto yc = rect.getYMid();

    double tw1 = pixelToWindowWidth(fm.width(text));

    device->setPen(interpColor(current ? currentTextColor : textColor, ColorInd()));

    double tyo = pixelToWindowHeight((fm.ascent() - fm.descent())/2.0);
    double th  = pixelToWindowHeight(tabData_.pth);

    device->drawText(Point(xc - tw1/2, yc - th/2 + tyo), text);
  };

  //---

  auto tabRect = this->calcTabPixelRect();

  int px = tabRect.getXMin();
  int py = tabRect.getYMin();

  for (auto &plot : plots) {
    double ptw1 = fm.width(plot->calcName()) + 2*tabData_.pxm;

    BBox prect(px, py, px + ptw1, py + tabData_.pth);

    plot->setTabRect(pixelToWindow(prect));

    drawTab(plot->tabRect(), plot == currentPlot);

    drawText(plot->tabRect(), plot->calcName(), plot == currentPlot);

    px += ptw1;
  }
}

void
CQChartsPlot::
drawOverlayParts(QPainter *painter) const
{
  CQPerfTrace trace("CQChartsPlot::drawOverlayParts");

  bool sel_objs         = hasGroupedObjs(Layer::Type::SELECTION);
  bool sel_annotations  = hasGroupedAnnotations(Layer::Type::SELECTION);
  bool boxes            = hasGroupedBoxes();
  bool edit_handles     = hasGroupedEditHandles();
  bool over_objs        = hasGroupedObjs(Layer::Type::MOUSE_OVER);
  bool over_annotations = hasGroupedAnnotations(Layer::Type::MOUSE_OVER);

  if (! sel_objs && ! sel_annotations && ! boxes &&
      ! edit_handles && ! over_objs && ! over_annotations)
    return;

  //---

  auto *buffer = getBuffer(Buffer::Type::OVERLAY);
  if (! buffer->isActive()) return;

  auto *painter1 = beginPaint(buffer, painter);

  //---

  if (painter1) {
    auto *th = const_cast<CQChartsPlot *>(this);

    CQChartsPlotPaintDevice device(th, painter1);

    drawOverlayDeviceParts(&device, sel_objs, sel_annotations, boxes, edit_handles,
                           over_objs, over_annotations);
  }

  //---

  endPaint(buffer);
}

void
CQChartsPlot::
drawOverlayDeviceParts(PaintDevice *device, bool sel_objs, bool sel_annotations, bool boxes,
                       bool edit_handles, bool over_objs, bool over_annotations) const
{
  // draw selection
  if (sel_objs)
    drawGroupedObjs(device, Layer::Type::SELECTION);

  if (sel_annotations)
    drawGroupedAnnotations(device, Layer::Type::SELECTION);

  //---

  // draw debug boxes
  if (boxes)
    drawGroupedBoxes(device);

  //---

  if (edit_handles) {
    if (device->isInteractive()) {
      auto *viewPlotDevice = dynamic_cast<CQChartsViewPlotPaintDevice *>(device);

      drawGroupedEditHandles(viewPlotDevice->painter());
    }
  }

  //---

  // draw mouse over
  if (over_objs)
    drawGroupedObjs(device, Layer::Type::MOUSE_OVER);

  if (over_annotations)
    drawGroupedAnnotations(device, Layer::Type::MOUSE_OVER);
}

bool
CQChartsPlot::
hasBackgroundLayer() const
{
  // only first plot has background for overlay
  if (isOverlay() && ! isFirstPlot())
    return false;

  //---

  bool hasPlotBackground = (isPlotFilled() || isPlotStroked());
  bool hasDataBackground = (isDataFilled() || isDataStroked());
  bool hasFitBackground  = (isFitFilled () || isFitStroked ());
  bool hasBackground     = this->hasBackground();

  if (! hasPlotBackground && ! hasDataBackground && ! hasFitBackground && ! hasBackground)
    return false;

  if (! isLayerActive(Layer::Type::BACKGROUND))
    return false;

  return true;
}

void
CQChartsPlot::
drawBackgroundLayer(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsPlot::drawBackgroundLayer");

  //---

  drawBackgroundRects(device);

  //---

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
drawBackgroundRects(PaintDevice *device) const
{
  auto drawBackgroundRect = [&](const BBox &rect, const BrushData &brushData,
                                const PenData &penData, const Sides &sides) {
    if (brushData.isVisible()) {
      PenBrush penBrush;

      setBrush(penBrush,
        BrushData(true, brushData.color(), brushData.alpha(), brushData.pattern()));

      device->setBrush(penBrush.brush);

      device->fillRect(rect);
    }

    if (penData.isVisible()) {
      PenBrush penBrush;

      setPen(penBrush,
        PenData(true, penData.color(), penData.alpha(), penData.width(), penData.dash()));

      device->setPen(penBrush.pen);

      drawBackgroundSides(device, rect, sides);
    }
  };

  if (isPlotFilled() || isPlotStroked())
    drawBackgroundRect(calcPlotRect(), plotBrushData(ColorInd()), plotPenData(ColorInd()),
                       plotBorderSides());

  if (isFitFilled () || isFitStroked())
    drawBackgroundRect(fitBBox(), fitBrushData(ColorInd()), fitPenData(ColorInd()),
                       fitBorderSides());

  if (isDataFilled() || isDataStroked())
    drawBackgroundRect(displayRangeBBox(), dataBrushData(ColorInd()), dataPenData(ColorInd()),
                       dataBorderSides());
}

bool
CQChartsPlot::
hasBackground() const
{
  if (isOverlay()) {
    Plots oplots;

    overlayPlots(oplots);

    for (const auto &oplot : oplots)
      if (oplot->hasBackground())
        return true;
  }

  return false;
}

void
CQChartsPlot::
execDrawBackground(PaintDevice *) const
{
}

void
CQChartsPlot::
drawBackgroundSides(PaintDevice *device, const BBox &bbox, const Sides &sides) const
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

bool
CQChartsPlot::
hasGroupedBgAxes() const
{
  if (isOverlay()) {
    if (! isFirstPlot())
      return false;

    bool anyAxis = processOverlayPlots([&](const CQChartsPlot *plot) {
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
  // just axis grid on background
  bool showXAxis = (xAxis() && xAxis()->isVisible());
  bool showYAxis = (yAxis() && yAxis()->isVisible());

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

    processOverlayPlots([&](const CQChartsPlot *plot) {
      device->setPlot(const_cast<CQChartsPlot *>(plot));

      plot->drawBgAxes(device);
    });

    device->setPlot(const_cast<CQChartsPlot *>(this));
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

  bool showXAxis = (xAxis() && xAxis()->isVisible());
  bool showYAxis = (yAxis() && yAxis()->isVisible());

  bool showXGrid = (showXAxis && ! xAxis()->isGridAbove() && xAxis()->isDrawGrid());
  bool showYGrid = (showYAxis && ! yAxis()->isGridAbove() && yAxis()->isDrawGrid());

  //---

  if (showXGrid) drawXGrid(device);
  if (showYGrid) drawYGrid(device);
}

bool
CQChartsPlot::
hasGroupedBgKey() const
{
  if (isOverview())
    return false;

  CQChartsPlotKey *key1 = nullptr;

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

    processOverlayPlots([&](const CQChartsPlot *plot) {
      auto *key = plot->key();
      if (! key) return;

      device->setPlot(const_cast<CQChartsPlot *>(plot));

      key->draw(device);
    });

    device->setPlot(const_cast<CQChartsPlot *>(this));
  }
  else {
    auto *key = this->key();

    if (key)
      key->draw(device);
  }
}

bool
CQChartsPlot::
hasGroupedObjs(const Layer::Type &layerType) const
{
  // for overlay draw all combine objects on common layers
  if (isOverlay()) {
    if (! isFirstPlot())
      return false;

    bool anyObjs = processOverlayPlots([&](const CQChartsPlot *plot) {
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

    processOverlayPlots([&](const CQChartsPlot *plot) {
      device->setPlot(const_cast<CQChartsPlot *>(plot));

      plot->execDrawObjs(device, layerType);
    });

    device->setPlot(const_cast<CQChartsPlot *>(this));
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

    auto *viewPlotDevice = dynamic_cast<CQChartsViewPlotPaintDevice *>(device);

    if (plotObj->isZoomText() && viewPlotDevice)
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

      if (drawLayer != CQChartsPlotObj::DrawLayer::NONE) {
        bool draw = ((drawLayer == CQChartsPlotObj::DrawLayer::BACKGROUND &&
                      layerType == Layer::Type::BG_PLOT) ||
                     (drawLayer == CQChartsPlotObj::DrawLayer::MIDDLE &&
                      layerType == Layer::Type::MID_PLOT) ||
                     (drawLayer == CQChartsPlotObj::DrawLayer::FOREGROUND &&
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

    if (plotObj->isZoomText() && viewPlotDevice)
      viewPlotDevice->setZoomFont(false);

    //---

    // show debug box
    if (showBoxes() || (plotObj->isSelected() && showSelectedBoxes()))
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
  return plotObj->rectIntersect(bbox, /*inside*/ false);
}

bool
CQChartsPlot::
hasGroupedFgAxes() const
{
  if (isOverlay()) {
    if (! isFirstPlot())
      return false;

    bool anyAxis = processOverlayPlots([&](const CQChartsPlot *plot) {
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
  bool showXAxis = (xAxis() && xAxis()->isVisible());
  bool showYAxis = (yAxis() && yAxis()->isVisible());

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

    processOverlayPlots([&](const CQChartsPlot *plot) {
      device->setPlot(const_cast<CQChartsPlot *>(plot));

      plot->drawFgAxes(device);
    });

    device->setPlot(const_cast<CQChartsPlot *>(this));
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

  bool showXAxis = (xAxis() && xAxis()->isVisible());
  bool showYAxis = (yAxis() && yAxis()->isVisible());

  bool showXGrid = (showXAxis && xAxis()->isGridAbove() && xAxis()->isDrawGrid());
  bool showYGrid = (showYAxis && yAxis()->isGridAbove() && yAxis()->isDrawGrid());

  //---

  if (showXGrid) drawXGrid(device);
  if (showYGrid) drawYGrid(device);

  if (showXAxis) drawXAxis(device);
  if (showYAxis) drawYAxis(device);
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

//---

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

  auto drawPlotAxis = [&](CQChartsPlot *plot, double pos) {
    if (x1x2) {
      device->setPlot(plot);

      plot->drawXAxisAt(device, plot, pos);

      device->setPlot(const_cast<CQChartsPlot *>(this));
    }
    else
      plot->drawXAxisAt(device, const_cast<CQChartsPlot *>(this), pos);
  };

  //---

  if (x1x2 || (isOverlay() && isShowAllXOverlayAxes())) {
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
    //  dataRange += annotationBBox();

    // draw bottom axes
    double bpos = (dataRange.isSet() ? dataRange.getYMin() : 0.0);

    for (auto &oplot : oplots) {
      if (! oplot->xAxis())
        continue;

      if (oplot->xAxis()->position().isSet())
        continue;

      // draw x axis at adjusted position and move position down for next axis
      if (oplot->xAxis()->side() == CQChartsAxisSide::Type::BOTTOM_LEFT)
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
      if (oplot->xAxis()->side() == CQChartsAxisSide::Type::TOP_RIGHT)
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
drawXAxisAt(PaintDevice *device, CQChartsPlot *plot, double pos) const
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

  auto drawPlotAxis = [&](CQChartsPlot *plot, double pos) {
    if (y1y2) {
      device->setPlot(plot);

      plot->drawYAxisAt(device, plot, pos);

      device->setPlot(const_cast<CQChartsPlot *>(this));
    }
    else
      plot->drawYAxisAt(device, const_cast<CQChartsPlot *>(this), pos);
  };

  //---

  if (y1y2 || (isOverlay() && isShowAllYOverlayAxes())) {
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
    //  dataRange += annotationBBox();

    // draw left axes
    double lpos = (dataRange.isSet() ? dataRange.getXMin() : 0.0);

    for (auto &oplot : oplots) {
      if (! oplot->yAxis())
        continue;

      if (oplot->yAxis()->position().isSet())
        continue;

      // draw y axis at adjusted position and move position left for next axis
      if (oplot->yAxis()->side() == CQChartsAxisSide::Type::BOTTOM_LEFT)
        drawPlotAxis(oplot, lpos);

      double w = oplot->yAxisWidth(CQChartsAxisSide::Type::BOTTOM_LEFT);

      if (showBoxes())
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
      if (oplot->yAxis()->side() == CQChartsAxisSide::Type::TOP_RIGHT)
        drawPlotAxis(oplot, rpos);

      double w = oplot->yAxisWidth(CQChartsAxisSide::Type::TOP_RIGHT);

      if (showBoxes())
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
drawYAxisAt(PaintDevice *device, CQChartsPlot *plot, double pos) const
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
  if (xAxis()->side() == side) {
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
  if (yAxis()->side() == side) {
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

  CQChartsPlotKey *key1 = nullptr;

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

    processOverlayPlots([&](const CQChartsPlot *plot) {
      auto *key = plot->key();
      if (! key) return;

      device->setPlot(const_cast<CQChartsPlot *>(plot));

      key->draw(device);
    });

    device->setPlot(const_cast<CQChartsPlot *>(this));
  }
  else {
    auto *key = this->key();

    if (key)
      key->draw(device);
  }
}

bool
CQChartsPlot::
hasTitle() const
{
  // only first plot has title for overlay
  if (isOverlay() && ! isFirstPlot())
    return false;

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

bool
CQChartsPlot::
hasGroupedAnnotations(const Layer::Type &layerType) const
{
  // for overlay draw all combine objects on common layers
  if (isOverlay()) {
    if (! isFirstPlot())
      return false;

    bool anyObjs = processOverlayPlots([&](const CQChartsPlot *plot) {
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

    processOverlayPlots([&](const CQChartsPlot *plot) {
      device->setPlot(const_cast<CQChartsPlot *>(plot));

      plot->drawAnnotations(device, layerType);
    });

    device->setPlot(const_cast<CQChartsPlot *>(this));
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
             annotation->drawLayer() == Annotation::DrawLayer::FOREGROUND));
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
      if (! annotation->isInside())
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
             annotation->drawLayer() == Annotation::DrawLayer::FOREGROUND));
  };

  //---

  CQPerfTrace trace("CQChartsPlot::drawAnnotations");

  // set draw layer
  view()->setDrawLayerType(layerType);

  //---

  for (auto &annotation : annotations()) {
    if (! annotation->isVisible())
      continue;

    if      (layerType == Layer::Type::SELECTION) {
      if (! annotation->isSelected())
        continue;
    }
    else if (layerType == Layer::Type::MOUSE_OVER) {
      if (! annotation->isInside())
        continue;
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

bool
CQChartsPlot::
hasForeground() const
{
  return false;
}

void
CQChartsPlot::
execDrawForeground(PaintDevice *) const
{
}

bool
CQChartsPlot::
hasGroupedBoxes() const
{
  // for overlay draw all combine objects on common layers
  if (isOverlay()) {
    if (! isFirstPlot())
      return false;

    bool anyObjs = processOverlayPlots([&](const CQChartsPlot *plot) {
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

    processOverlayPlots([&](const CQChartsPlot *plot) {
      device->setPlot(const_cast<CQChartsPlot *>(plot));

      plot->drawBoxes(device);
    });

    device->setPlot(const_cast<CQChartsPlot *>(this));
  }
  else {
    drawBoxes(device);
  }
}

bool
CQChartsPlot::
hasBoxes() const
{
  if (! showBoxes())
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

  drawWindowColorBox(device, dataFitBBox   ());
  drawWindowColorBox(device, axesFitBBox   ());
  drawWindowColorBox(device, keyFitBBox    ());
  drawWindowColorBox(device, titleFitBBox  ());
  drawWindowColorBox(device, annotationBBox());

  //---

  drawWindowColorBox(device, CQChartsUtil::rangeBBox(getCalcDataRange()), Qt::green);
  drawWindowColorBox(device, CQChartsUtil::rangeBBox(dataRange_        ), Qt::green);
  drawWindowColorBox(device, CQChartsUtil::rangeBBox(outerDataRange_   ), Qt::green);
}

bool
CQChartsPlot::
hasGroupedEditHandles() const
{
  // for overlay draw all combine objects on common layers
  if (isOverlay()) {
    if (! isFirstPlot())
      return false;

    bool anyObjs = processOverlayPlots([&](const CQChartsPlot *plot) {
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
drawGroupedEditHandles(QPainter *painter) const
{
  CQPerfTrace trace("CQChartsPlot::drawGroupedEditHandles");

  // for overlay draw all combine objects on common layers
  if (isOverlay()) {
    if (! isFirstPlot())
      return;

    processOverlayPlots([&](const CQChartsPlot *plot) {
      plot->drawEditHandles(painter);
    });
  }
  else {
    drawEditHandles(painter);
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
drawEditHandles(QPainter *painter) const
{
  CQPerfTrace trace("CQChartsPlot::drawEditHandles");

  if      (isEditable() && isSelected()) {
    const_cast<CQChartsPlot *>(this)->editHandles_->setBBox(this->calcViewBBox());

    editHandles_->draw(painter);
  }

  if (title() && title()->isEditable() && title()->isSelected()) {
    title()->drawEditHandles(painter);
  }

  auto *key1 = getFirstPlotKey();

  if (key1 && key1->isEditable() && key1->isSelected()) {
    key1->drawEditHandles(painter);
  }

  if (xAxis() && xAxis()->isEditable() && xAxis()->isSelected()) {
    xAxis()->drawEditHandles(painter);
  }

  if (yAxis() && yAxis()->isEditable() && yAxis()->isSelected()) {
    yAxis()->drawEditHandles(painter);
  }

  for (const auto &annotation : annotations()) {
    if (annotation->isVisible() && annotation->isEditable() && annotation->isSelected())
      annotation->drawEditHandles(painter);
  }

  for (auto &plotObj : plotObjects()) {
    if (plotObj->isEditable() && plotObj->isSelected())
      plotObj->drawEditHandles(painter);
  }
}

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
  execWaitTree();
}

void
CQChartsPlot::
execWaitTree()
{
  if (! isPlotObjTreeSet())
    objTreeData_.tree->waitTree();
}

//---

CQChartsGeom::BBox
CQChartsPlot::
displayRangeBBox() const
{
  if (parentPlot())
    return parentPlot()->displayRangeBBox();

  // calc current (zoomed/panned) data range
  double xmin, ymin, xmax, ymax;

  displayRange().getWindowRange(&xmin, &ymin, &xmax, &ymax);

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
  auto *th = const_cast<CQChartsPlot *>(this);

  QFontMetrics fm(tabbedFont().font());

  th->tabData_.pxm = fm.width("X")/2.0;
  th->tabData_.pym = fm.height()/4.0;

  th->tabData_.ptw = 0.0;

  for (auto &plot : plots)
    th->tabData_.ptw += fm.width(plot->calcName()) + 2*tabData_.pxm;

  th->tabData_.pth = fm.height() + 2*tabData_.pym;
}

CQChartsGeom::BBox
CQChartsPlot::
calcTabPixelRect() const
{
  auto pixelRect = calcPlotPixelRect();

  int px = pixelRect.getXMid() - tabData_.ptw/2;
  int py = pixelRect.getYMax() - tabData_.pth - 2*tabData_.pym;

  return BBox(px, py, px + tabData_.ptw, py + tabData_.pth);
}

//---

void
CQChartsPlot::
updateAutoFit()
{
  // auto fit based on last draw
  if (needsAutoFit_) {
    if (calcNextState() != UpdateState::INVALID)
      return;

    needsAutoFit_ = false;

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
    needsAutoFit_ = true;

    zoomFull(/*notify*/false);

    return;
  }

  if (isOverlay()) {
    if (prevPlot())
      return;

    //---

    // combine bboxes of overlay plots
    BBox bbox;

    processOverlayPlots([&](const CQChartsPlot *plot) {
      auto bbox1 = plot->fitBBox();
      auto bbox2 = plot->windowToPixel(bbox1);
      auto bbox3 = pixelToWindow(bbox2);

      bbox += bbox3;
    });

    //---

    // set all overlay plot bboxes
    using BBoxes = std::vector<BBox>;

    BBoxes bboxes;

    processOverlayPlots([&](const CQChartsPlot *plot) {
      auto bbox1 = windowToPixel(bbox);
      auto bbox2 = plot->pixelToWindow(bbox1);

      bboxes.push_back(bbox2);
    });

    int i = 0;

    processOverlayPlots([&](CQChartsPlot *plot) {
      plot->setFitBBox(bboxes[i]);

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
#if 0
  for (int i = 0; i < 5; ++i) {
    auto bbox = fitBBox();

    setFitBBox(bbox);

    updateRangeAndObjs();

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
  }
#else
  outerMargin_ = PlotMargin(Length("0P"), Length("0P"), Length("0P"), Length("0P"));

  updateMargins();

  //---

  auto bbox = fitBBox();

  setFitBBox(bbox);

  updateRangeAndObjs();
#endif

  emit zoomPanChanged();
}

void
CQChartsPlot::
setFitBBox(const BBox &bbox)
{
  auto bbox1 = fitMargin().adjustPlotRange(this, bbox, /*inside*/true);

  // calc margin so plot box fits in specified box
  auto pbbox = displayRangeBBox();

  double left   = 100.0*(pbbox.getXMin() - bbox1.getXMin())/bbox1.getWidth ();
  double bottom = 100.0*(pbbox.getYMin() - bbox1.getYMin())/bbox1.getHeight();
  double right  = 100.0*(bbox1.getXMax() - pbbox.getXMax())/bbox1.getWidth ();
  double top    = 100.0*(bbox1.getYMax() - pbbox.getYMax())/bbox1.getHeight();

  if (isInvertX()) std::swap(left, right );
  if (isInvertY()) std::swap(top , bottom);

  outerMargin_ = PlotMargin(Length(left , Units::PERCENT), Length(top   , Units::PERCENT),
                            Length(right, Units::PERCENT), Length(bottom, Units::PERCENT));

  updateMargins();
}

CQChartsGeom::BBox
CQChartsPlot::
fitBBox() const
{
  // calc fit box
  BBox bbox;

  bbox += dataFitBBox   ();
  bbox += axesFitBBox   ();
  bbox += keyFitBBox    ();
  bbox += titleFitBBox  ();
  bbox += annotationBBox();

  // add margin (TODO: config pixel margin size)
  auto marginSize = pixelToWindowSize(Size(8, 8));

  bbox.expand(-marginSize.width(), -marginSize.height(),
               marginSize.width(),  marginSize.height());

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
dataFitBBox() const
{
  auto bbox = displayRangeBBox();

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
axesFitBBox() const
{
  BBox bbox;

  if (xAxis() && xAxis()->isVisible())
    bbox += xAxis()->fitBBox();

  if (yAxis() && yAxis()->isVisible())
    bbox += yAxis()->fitBBox();

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
keyFitBBox() const
{
  BBox bbox;

  if (isKeyVisibleAndNonEmpty()) {
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

  if (title() && title()->isVisible())
    bbox += title()->fitBBox();

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

CQChartsGeom::BBox
CQChartsPlot::
annotationBBox() const
{
  if (annotationBBox_.isSet())
    return annotationBBox_;

  annotationBBox_ = calcAnnotationBBox();

  return annotationBBox_;
}

//------

CQChartsAnnotationGroup *
CQChartsPlot::
addAnnotationGroup()
{
  return addAnnotationT<CQChartsAnnotationGroup>(new CQChartsAnnotationGroup(this));
}

CQChartsArrowAnnotation *
CQChartsPlot::
addArrowAnnotation(const Position &start, const Position &end)
{
  return addAnnotationT<CQChartsArrowAnnotation>(
    new CQChartsArrowAnnotation(this, start, end));
}

CQChartsArcAnnotation *
CQChartsPlot::
addArcAnnotation(const Rect &start, const Rect &end)
{
  return addAnnotationT<CQChartsArcAnnotation>(
    new CQChartsArcAnnotation(this, start, end));
}

CQChartsAxisAnnotation *
CQChartsPlot::
addAxisAnnotation(Qt::Orientation direction, double start, double end)
{
  return addAnnotationT<CQChartsAxisAnnotation>(
    new CQChartsAxisAnnotation(this, direction, start, end));
}

CQChartsEllipseAnnotation *
CQChartsPlot::
addEllipseAnnotation(const Position &center, const Length &xRadius, const Length &yRadius)
{
  return addAnnotationT<CQChartsEllipseAnnotation>(
    new CQChartsEllipseAnnotation(this, center, xRadius, yRadius));
}

CQChartsImageAnnotation *
CQChartsPlot::
addImageAnnotation(const Position &pos, const Image &image)
{
  return addAnnotationT<CQChartsImageAnnotation>(
    new CQChartsImageAnnotation(this, pos, image));
}

CQChartsImageAnnotation *
CQChartsPlot::
addImageAnnotation(const Rect &rect, const Image &image)
{
  return addAnnotationT<CQChartsImageAnnotation>(
    new CQChartsImageAnnotation(this, rect, image));
}

CQChartsPathAnnotation *
CQChartsPlot::
addPathAnnotation(const Path &path)
{
  return addAnnotationT<CQChartsPathAnnotation>(
    new CQChartsPathAnnotation(this, path));
}

CQChartsKeyAnnotation *
CQChartsPlot::
addKeyAnnotation(const Column &column)
{
  return addAnnotationT<CQChartsKeyAnnotation>(
    new CQChartsKeyAnnotation(this, column));
}

CQChartsPieSliceAnnotation *
CQChartsPlot::
addPieSliceAnnotation(const Position &pos, const Length &innerRadius, const Length &outerRadius,
                      const Angle &startAngle, const Angle &spanAngle)
{
  return addAnnotationT<CQChartsPieSliceAnnotation>(
    new CQChartsPieSliceAnnotation(this, pos, innerRadius, outerRadius, startAngle, spanAngle));
}

CQChartsPointAnnotation *
CQChartsPlot::
addPointAnnotation(const Position &pos, const Symbol &type)
{
  return addAnnotationT<CQChartsPointAnnotation>(
    new CQChartsPointAnnotation(this, pos, type));
}

CQChartsPointSetAnnotation *
CQChartsPlot::
addPointSetAnnotation(const CQChartsPoints &values)
{
  return addAnnotationT<CQChartsPointSetAnnotation>(
    new CQChartsPointSetAnnotation(this, values));
}

CQChartsPolygonAnnotation *
CQChartsPlot::
addPolygonAnnotation(const CQChartsPolygon &points)
{
  return addAnnotationT<CQChartsPolygonAnnotation>(
    new CQChartsPolygonAnnotation(this, points));
}

CQChartsPolylineAnnotation *
CQChartsPlot::
addPolylineAnnotation(const CQChartsPolygon &points)
{
  return addAnnotationT<CQChartsPolylineAnnotation>(
    new CQChartsPolylineAnnotation(this, points));
}

CQChartsRectangleAnnotation *
CQChartsPlot::
addRectangleAnnotation(const Rect &rect)
{
  return addAnnotationT<CQChartsRectangleAnnotation>(
    new CQChartsRectangleAnnotation(this, rect));
}

CQChartsTextAnnotation *
CQChartsPlot::
addTextAnnotation(const Position &pos, const QString &text)
{
  return addAnnotationT<CQChartsTextAnnotation>(
    new CQChartsTextAnnotation(this, pos, text));
}

CQChartsTextAnnotation *
CQChartsPlot::
addTextAnnotation(const Rect &rect, const QString &text)
{
  return addAnnotationT<CQChartsTextAnnotation>(
    new CQChartsTextAnnotation(this, rect, text));
}

CQChartsValueSetAnnotation *
CQChartsPlot::
addValueSetAnnotation(const Rect &rectangle, const CQChartsReals &values)
{
  return addAnnotationT<CQChartsValueSetAnnotation>(
    new CQChartsValueSetAnnotation(this, rectangle, values));
}

CQChartsButtonAnnotation *
CQChartsPlot::
addButtonAnnotation(const Position &pos, const QString &text)
{
  return addAnnotationT<CQChartsButtonAnnotation>(
    new CQChartsButtonAnnotation(this, pos, text));
}

CQChartsWidgetAnnotation *
CQChartsPlot::
addWidgetAnnotation(const Position &pos, const Widget &widget)
{
  auto *control = dynamic_cast<CQChartsPlotControlIFace *>(widget.widget());

  if (control) {
    control->setPlot(this);

    control->connectValueChanged(this, SLOT(plotControlUpdateSlot()));

    controls_.push_back(control);
  }

  auto *controlFrame = dynamic_cast<CQChartsPlotControlFrame *>(widget.widget());

  if (controlFrame)
    controlFrame->setPlot(this);

  //---

  auto *propertyEditGroup = dynamic_cast<CQChartsPlotPropertyEditGroup *>(widget.widget());

  if (propertyEditGroup)
    propertyEditGroup->setPlot(this);

  auto *propertyEdit = dynamic_cast<CQChartsPlotPropertyEdit *>(widget.widget());

  if (propertyEdit)
    propertyEdit->setPlot(this);

  //---

  auto *modelHolder = dynamic_cast<CQChartsModelViewHolder *>(widget.widget());

  if (modelHolder) {
    modelHolder->setCharts(charts());
    modelHolder->setModel(model(), isHierarchical());
  }

  auto *detailsTable = dynamic_cast<CQChartsModelDetailsTable *>(widget.widget());

  if (detailsTable)
    detailsTable->setModelData(getModelData());

  //---

  auto *modelControlData = dynamic_cast<CQChartsModelColumnDataControl *>(widget.widget());

  if (modelControlData)
    modelControlData->setModelData(getModelData());

  auto *modelExpr = dynamic_cast<CQChartsModelExprControl *>(widget.widget());

  if (modelExpr)
    modelExpr->setModelData(getModelData());

  //---

  return addAnnotationT<CQChartsWidgetAnnotation>(
    new CQChartsWidgetAnnotation(this, pos, widget));
}

CQChartsWidgetAnnotation *
CQChartsPlot::
addWidgetAnnotation(const Rect &rect, const Widget &widget)
{
  return addAnnotationT<CQChartsWidgetAnnotation>(
    new CQChartsWidgetAnnotation(this, rect, widget));
}

CQChartsSymbolMapKeyAnnotation *
CQChartsPlot::
addSymbolMapKeyAnnotation()
{
  return addAnnotationT<CQChartsSymbolMapKeyAnnotation>(
    new CQChartsSymbolMapKeyAnnotation(this));
}

void
CQChartsPlot::
plotControlUpdateSlot()
{
  QString cmpStr = "==";

  QStringList filters;

  for (const auto &control : controls_) {
    auto filter = control->filterStr(cmpStr);

    if (filter.length())
      filters.push_back(filter);
  }

  QString combStr = "&&";

  auto filterStr = filters.join(QString(" %1 ").arg(combStr));

  setVisibleFilterStr(filterStr);
}

void
CQChartsPlot::
addAnnotation(Annotation *annotation)
{
  annotations_.push_back(annotation);

  connect(annotation, SIGNAL(idChanged()), this, SLOT(updateAnnotationSlot()));
  connect(annotation, SIGNAL(dataChanged()), this, SLOT(updateAnnotationSlot()));

  annotation->addProperties(propertyModel(), "annotations");

//emit annotationAdded(annotation->id());
  emit annotationsChanged();
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

  int np = annotations().size();
  if (np < 2) return;

  if (pos < np - 1)
    std::swap(annotations_[pos + 1], annotations_[pos]);

  drawObjs();

  emit annotationsReordered();
}

void
CQChartsPlot::
lowerAnnotation(Annotation *annotation)
{
  int pos = annotationPos(annotation);
  if (pos < 0) return; // not found

  int np = annotations().size();
  if (np < 2) return;

  if (pos > 0)
    std::swap(annotations_[pos - 1], annotations_[pos]);

  drawObjs();

  emit annotationsReordered();
}

int
CQChartsPlot::
annotationPos(Annotation *annotation) const
{
  int np = annotations().size();

  for (int i = 0; i < np; ++i) {
    if (annotations_[i] == annotation)
      return i;
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

  int n = annotations_.size();

  assert(pos >= 0 && pos < n);

  propertyModel()->removeProperties("annotations/" + annotation->propertyId());

  delete annotation;

  for (int i = pos + 1; i < n; ++i)
    annotations_[i - 1] = annotations_[i];

  annotations_.pop_back();

  emit annotationsChanged();
}

void
CQChartsPlot::
removeAllAnnotations()
{
  for (auto &annotation : annotations_)
    delete annotation;

  annotations_.clear();

  propertyModel()->removeProperties("annotations");

  emit annotationsChanged();
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

  emit annotationsChanged();
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

    plotObj->getSelectIndices(inds1); // normalized

    for (auto &ind1 : inds1) {
      auto ind2 = unnormalizeIndex(ind1);

      inds.push_back(ind2);
    }
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

    pb = buffers_.insert(pb, Buffers::value_type(buffer, layerBuffer));
  }

  //auto *layerBuffer = (*pb).second;

  //---

  auto pl = layers_.find(type);

  if (pl == layers_.end()) {
    auto *layer = new Layer(type, buffer);

    pl = layers_.insert(pl, Layers::value_type(type, layer));
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
    processOverlayPlots([&](CQChartsPlot *plot) {
      plot->setLayerActive1(type, b);
    });
  }
  else
    setLayerActive1(type, b);
}

void
CQChartsPlot::
setLayerActive1(const Layer::Type &type, bool b)
{
  auto *layer = getLayer(type);

  layer->setActive(b);

  setLayersChanged(true);
}

bool
CQChartsPlot::
isLayerActive(const Layer::Type &type) const
{
  auto *layer = getLayer(type);

  return layer->isActive();
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
    updatesData_.invalidateLayers = true;
    return;
  }

  //---

  if (isOverlay()) {
    processOverlayPlots([&](CQChartsPlot *plot) {
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
    updatesData_.invalidateLayers = true;
    return;
  }

  //assert(type != Buffer::Type::MIDDLE);

  if (isOverlay()) {
    processOverlayPlots([&](CQChartsPlot *plot) {
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
  if (updateData_.rangeThread->isBusy())
    return;

  //---

  {
    LockMutex lock(this, "setLayersChanged");

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

  emit layersChanged();
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
  assert(p != layers_.end());

  return (*p).second;
}

//---

void
CQChartsPlot::
setClipRect(PaintDevice *device) const
{
  auto *plot1 = firstPlot();

  if      (plot1->isDataClip()) {
    auto bbox  = displayRangeBBox();
    auto abbox = annotationBBox();

    if      (dataScaleX() <= 1.0 && dataScaleY() <= 1.0)
      bbox.add(abbox);
    else if (dataScaleX() <= 1.0)
      bbox.addX(abbox);
    else if (dataScaleY() <= 1.0)
      bbox.addY(abbox);

    device->setClipRect(bbox);
  }
  else if (plot1->isPlotClip()) {
    auto plotRect = calcPlotRect();

    device->setClipRect(plotRect);
  }
}

QPainter *
CQChartsPlot::
beginPaint(Buffer *buffer, QPainter *painter, const QRectF &rect) const
{
  drawBuffer_ = buffer->type();

  if (! view()->isBufferLayers())
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
  if (! view()->isBufferLayers())
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

#if 0
void
CQChartsPlot::
drawSymbol(PaintDevice *device, const Point &p, const Symbol &symbol, const Length &size) const
{
  if (bufferSymbols_) {
    auto *viewPlotDevice = dynamic_cast<CQChartsViewPlotPaintDevice *>(device);

    if (viewPlotDevice) {
      double sx, sy;

      plotSymbolSize(size, sx, sy);

      drawBufferedSymbol(viewPlotDevice->painter(), p, symbol, std::min(sx, sy));
    }
    else {
      CQChartsDrawUtil::drawSymbol(device, symbol, p, size);
    }
  }
  else {
    CQChartsDrawUtil::drawSymbol(device, symbol, p, size);
  }
}

void
CQChartsPlot::
drawBufferedSymbol(QPainter *painter, const Point &p, const Symbol &symbol, double size) const
{
  auto cmpPen = [](const QPen &pen1, const QPen &pen2) {
    if (pen1.style () != pen2.style ()) return false;
    if (pen1.color () != pen2.color ()) return false;
    if (pen1.widthF() != pen2.widthF()) return false;
    return true;
  };

  auto cmpBrush = [](const QBrush &brush1, const QBrush &brush2) {
    if (brush1.style () != brush2.style ()) return false;
    if (brush1.color () != brush2.color ()) return false;
    return true;
  };

  struct ImageBuffer {
    Symbol symbol;
    double size { 0.0 };
    int    isize { 0 };
    QPen   pen;
    QBrush brush;
    QImage image;
  };

  static ImageBuffer imageBuffer;

  if (symbol != imageBuffer.symbol ||
      size   != imageBuffer.size   ||
      ! cmpPen  (painter->pen  (), imageBuffer.pen  ) ||
      ! cmpBrush(painter->brush(), imageBuffer.brush)) {
    imageBuffer.symbol = symbol;
    imageBuffer.size   = size;
    imageBuffer.isize  = CMathRound::RoundUp(2*(size + std::max(painter->pen().widthF(), 1.0)));
    imageBuffer.pen    = painter->pen  ();
    imageBuffer.brush  = painter->brush();
    imageBuffer.image  = CQChartsUtil::initImage(QSize(imageBuffer.isize, imageBuffer.isize));

    imageBuffer.image.fill(QColor(0, 0, 0, 0));

    QPainter ipainter(&imageBuffer.image);

    ipainter.setRenderHints(QPainter::Antialiasing);

    ipainter.setPen  (imageBuffer.pen  );
    ipainter.setBrush(imageBuffer.brush);

    CQChartsPixelPaintDevice device(&ipainter);

    Point  spos (size, size);
    Length ssize(size, CQChartsUnits::PIXEL);

    CQChartsDrawUtil::drawSymbol(&device, symbol, spos, ssize);
  }

  double is = imageBuffer.isize/2.0;

  painter->drawImage(int(p.x - is), int(p.y - is), imageBuffer.image);
}
#endif

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
  idHidden_[id] = hidden;
}

void
CQChartsPlot::
resetSetHidden()
{
  idHidden_.clear();
}

void
CQChartsPlot::
update()
{
  assert(fromInvalidate_);

  view()->update();

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
  double width = CQChartsUtil::limitLineWidth(lengthPixelWidth(penData.width()));

  CQChartsUtil::setPen(penBrush.pen, penData.isVisible(), penData.color(), penData.alpha(),
                       width, penData.dash(), penData.lineCap());
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

  penBrush.fillAngle = brushData.pattern().angle();
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
updateInsideObjPenBrushState(const ColorInd &ic, PenBrush &penBrush, bool outline,
                             DrawType drawType) const
{
  view()->updateInsideObjPenBrushState(ic, penBrush, outline, drawType);
}

void
CQChartsPlot::
updateSelectedObjPenBrushState(const ColorInd &ic, PenBrush &penBrush, DrawType drawType) const
{
  view()->updateSelectedObjPenBrushState(ic, penBrush, drawType);
}

QColor
CQChartsPlot::
insideColor(const QColor &c) const
{
  return view()->insideColor(c);
}

QColor
CQChartsPlot::
selectedColor(const QColor &c) const
{
  return view()->selectedColor(c);
}

//------

QColor
CQChartsPlot::
interpPaletteColor(const ColorInd &ind, bool scale) const
{
  Color c(Color::Type::PALETTE);

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

CQChartsPlot::ColorInd
CQChartsPlot::
calcColorInd(const PlotObj *obj, const CQChartsKeyColorBox *keyBox,
             const ColorInd &is, const ColorInd &ig, const ColorInd &iv) const
{
  ColorInd colorInd;

  auto colorType = this->colorType();

  if (obj && colorType == ColorType::AUTO)
    colorType = (CQChartsPlot::ColorType) obj->colorType();

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
      int ind = stops.ind(x);

      colorInd = ColorInd(ind, stops.size() + 1);
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
checkColumns(const Columns &columns, const QString &name, bool required) const
{
  if (required) {
    if (! columns.isValid())
      return const_cast<CQChartsPlot *>(this)->
        addError(QString("Missing required %1 columns").arg(name));
  }

  bool valid = true;

  int iv = 0;

  for (const auto &column : columns) {
    if (columnValueType(column) == ColumnType::NONE)
      valid = const_cast<CQChartsPlot *>(this)->
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

  if (required) {
    if (! column.isValid())
      return const_cast<CQChartsPlot *>(this)->
        addColumnError(column, QString("Missing required %1 column").arg(name));
  }

  if (column.isValid()) {
    type = columnValueType(column);

    if (type == ColumnType::NONE)
      return const_cast<CQChartsPlot *>(this)->
        addColumnError(column, QString("Invalid %1 column").arg(name));
  }

  return true;
}

CQChartsPlot::ColumnType
CQChartsPlot::
columnValueType(const Column &column, const ColumnType &defType) const
{
  CQChartsModelTypeData columnTypeData;

  if (! columnValueType(column, columnTypeData, defType))
    return ColumnType::NONE;

  return columnTypeData.type;
}

bool
CQChartsPlot::
columnValueType(const Column &column, CQChartsModelTypeData &columnTypeData,
                const ColumnType &defType) const
{
  if (! column.isValid()) {
    columnTypeData.type     = ColumnType::NONE;
    columnTypeData.baseType = ColumnType::NONE;
    return false;
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
    auto *model = this->model().data();
    assert(model);

    if (! CQChartsModelUtil::columnValueType(charts(), model, column, columnTypeData)) {
      // if fail column is invalid
      columnTypeData.type     = ColumnType::NONE;
      columnTypeData.baseType = ColumnType::NONE;
      return false;
    }
  }

  return true;
}

#if 0
bool
CQChartsPlot::
columnTypeStr(const Column &column, QString &typeStr) const
{
  auto *model = this->model().data();
  assert(model);

  return CQChartsModelUtil::columnTypeStr(charts(), model, column, typeStr);
}

bool
CQChartsPlot::
setColumnTypeStr(const Column &column, const QString &typeStr)
{
  auto *model = this->model().data();
  assert(model);

  return CQChartsModelUtil::setColumnTypeStr(charts(), model, column, typeStr);
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
  auto *modelData = getModelData();
  if (! modelData) return nullptr;

  auto *details = modelData->details();
  if (! details) return nullptr;

  return details->columnDetails(column);
}

CQChartsModelData *
CQChartsPlot::
getModelData() const
{
  return charts()->getModelData(model_.data());
}

//------

bool
CQChartsPlot::
getHierColumnNames(const QModelIndex &parent, int row, const Columns &nameColumns,
                   const QString &separator, QStringList &nameStrs, QModelIndices &nameInds) const
{
  auto *model = this->model().data();
  assert(model);

  auto *th = const_cast<CQChartsPlot *>(this);

  // single column (separated names)
  if (nameColumns.count() == 1) {
    const auto &nameColumn = nameColumns.column();

    //---

    ModelIndex nameModelInd(th, row, nameColumn, parent);

    bool ok;

    auto name = modelString(nameModelInd, ok);

    if (ok && ! name.simplified().length())
      ok = false;

    if (ok) {
      if (separator.length())
        nameStrs = name.split(separator, QString::SkipEmptyParts);
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

      if (ok && ! name.simplified().length())
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

CQChartsPlot::ModelIndex
CQChartsPlot::
normalizeIndex(const ModelIndex &ind) const
{
  assert(ind.plot() == this && ! ind.isNormalized());

  auto *th = const_cast<CQChartsPlot *>(this);

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

  auto *th = const_cast<CQChartsPlot *>(this);

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
  auto *model = this->model().data();
  assert(model);

  std::vector<QSortFilterProxyModel *> proxyModels;
  QAbstractItemModel*                  sourceModel;

  this->proxyModels(proxyModels, sourceModel);

  auto ind1 = ind;

  int i = 0;

  // ind model should match first proxy model
  for ( ; i < int(proxyModels.size()); ++i)
    if (ind1.model() == proxyModels[i])
      break;

  for ( ; i < int(proxyModels.size()); ++i)
    ind1 = proxyModels[i]->mapToSource(ind1);

  return ind1;
}

QModelIndex
CQChartsPlot::
unnormalizeIndex(const QModelIndex &ind) const
{
  // map index in source model (non-proxy model), to proxy model
  auto *model = this->model().data();
  assert(model);

  std::vector<QSortFilterProxyModel *> proxyModels;
  QAbstractItemModel*                  sourceModel;

  this->proxyModels(proxyModels, sourceModel);

  auto ind1 = ind;

  // ind model should match source model of last proxy model
  int i = int(proxyModels.size()) - 1;

  for ( ; i >= 0; --i)
    if (ind1.model() == proxyModels[i]->sourceModel())
      break;

  for ( ; i >= 0; --i)
    ind1 = proxyModels[i]->mapFromSource(ind1);

  return ind1;
}

QAbstractItemModel *
CQChartsPlot::
sourceModel() const
{
  std::vector<QSortFilterProxyModel *> proxyModels;
  QAbstractItemModel*                  sourceModel;

  this->proxyModels(proxyModels, sourceModel);

  return sourceModel;
}

void
CQChartsPlot::
proxyModels(std::vector<QSortFilterProxyModel *> &proxyModels,
            QAbstractItemModel* &sourceModel) const
{
  // map index in source model (non-proxy model), to proxy model
  auto *model = this->model().data();
  assert(model);

  auto *proxyModel = qobject_cast<QSortFilterProxyModel *>(model);

  if (proxyModel) {
    while (proxyModel) {
      proxyModels.push_back(proxyModel);

      sourceModel = proxyModel->sourceModel();

      proxyModel = qobject_cast<QSortFilterProxyModel *>(sourceModel);
    }
  }
  else
    sourceModel = model;
}

bool
CQChartsPlot::
isHierarchical() const
{
  auto *model = this->model().data();

  return CQChartsModelUtil::isHierarchical(model);
}

//------

void
CQChartsPlot::
addColumnValues(const Column &column, ValueSet &valueSet) const
{
  class ValueSetVisitor : public ModelVisitor {
   public:
    ValueSetVisitor(const CQChartsPlot *plot, const Column &column, ValueSet &valueSet) :
     plot_(plot), column_(column), valueSet_(valueSet) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      auto *plot = const_cast<CQChartsPlot *>(plot_);

      ModelIndex columnInd(plot, data.row, column_, data.parent);

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

  (void) CQChartsModelVisit::exec(charts(), model().data(), visitor);

  //visitor.term();
}

//------

bool
CQChartsPlot::
modelMappedReal(int row, const Column &column, const QModelIndex &parent, double &r,
                bool log, double def) const
{
  auto *th = const_cast<CQChartsPlot *>(this);

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
    IdVisitor(const CQChartsPlot *plot, const QString &id) :
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
    const CQChartsPlot *plot_ { nullptr };
    QString             id_;
    int                 row_ { -1 };
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

  auto *th = const_cast<CQChartsPlot *>(this);

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
  if (! column.hasColumn())
    return QModelIndex();

  if (! normalized) {
    auto *model = this->model().data();
    if (! model) return QModelIndex();

    return model->index(row, column.column(), parent);
  }
  else {
    std::vector<QSortFilterProxyModel *> proxyModels;
    QAbstractItemModel*                  sourceModel;

    this->proxyModels(proxyModels, sourceModel);

    return sourceModel->index(row, column.column(), parent);
  }
}

//------

#if 0
QVariant
CQChartsPlot::
modelHHeaderValue(const Column &column, bool &ok) const
{
  return modelHHeaderValue(model().data(), column, ok);
}

QVariant
CQChartsPlot::
modelHHeaderValue(const Column &column, int role, bool &ok) const
{
  return modelHHeaderValue(model().data(), column, role, ok);
}

QVariant
CQChartsPlot::
modelHHeaderValue(QAbstractItemModel *model, const Column &column, bool &ok) const
{
  return CQChartsModelUtil::modelHeaderValue(model, column, ok);
}

QVariant
CQChartsPlot::
modelHHeaderValue(QAbstractItemModel *model, const Column &column, int role, bool &ok) const
{
  return CQChartsModelUtil::modelHeaderValue(model, column, role, ok);
}
#endif

//--

#if 0
QVariant
CQChartsPlot::
modelVHeaderValue(int section, Qt::Orientation orient, int role, bool &ok) const
{
  return modelVHeaderValue(model().data(), section, orient, role, ok);
}

QVariant
CQChartsPlot::
modelVHeaderValue(int section, Qt::Orientation orient, bool &ok) const
{
  return modelVHeaderValue(model().data(), section, orient, Qt::DisplayRole, ok);
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
  return modelHHeaderString(model().data(), column, ok);
}

QString
CQChartsPlot::
modelHHeaderString(const Column &column, int role, bool &ok) const
{
  return modelHHeaderString(model().data(), column, role, ok);
}

QString
CQChartsPlot::
modelHHeaderString(QAbstractItemModel *model, const Column &column, bool &ok) const
{
  return CQChartsModelUtil::modelHHeaderString(model, column, ok);
}

QString
CQChartsPlot::
modelHHeaderString(QAbstractItemModel *model, const Column &column, int role, bool &ok) const
{
  return CQChartsModelUtil::modelHHeaderString(model, column, role, ok);
}

//--

QString
CQChartsPlot::
modelHHeaderTip(const Column &column, bool &ok) const
{
  auto str = modelHHeaderString(model().data(), column, (int) CQBaseModelRole::Tip, ok);

  if (! ok)
    str = CQChartsModelUtil::modelHHeaderString(model().data(), column, ok);

  return str;
}

//--

QString
CQChartsPlot::
modelVHeaderString(int section, Qt::Orientation orient, int role, bool &ok) const
{
  return modelVHeaderString(model().data(), section, orient, role, ok);
}

QString
CQChartsPlot::
modelVHeaderString(int section, Qt::Orientation orient, bool &ok) const
{
  return modelVHeaderString(model().data(), section, orient, Qt::DisplayRole, ok);
}

QString
CQChartsPlot::
modelVHeaderString(QAbstractItemModel *model, int section, Qt::Orientation orientation,
                   int role, bool &ok) const
{
  return CQChartsModelUtil::modelHeaderString(model, section, orientation, role, ok);
}

QString
CQChartsPlot::
modelVHeaderString(QAbstractItemModel *model, int section, Qt::Orientation orientation,
                   bool &ok) const
{
  return CQChartsModelUtil::modelHeaderString(model, section, orientation, ok);
}

//------

QVariant
CQChartsPlot::
modelValue(int row, const Column &column, const QModelIndex &parent, int role, bool &ok) const
{
  auto *th = const_cast<CQChartsPlot *>(this);

  return modelValue(ModelIndex(th, row, column, parent), role, ok);
}

QVariant
CQChartsPlot::
modelValue(int row, const Column &column, const QModelIndex &parent, bool &ok) const
{
  auto *th = const_cast<CQChartsPlot *>(this);

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

    return modelValue(model().data(), ind.row(), c, ind.parent(), role, ok);
  }
  else
    return modelValue(model().data(), ind.row(), ind.column(), ind.parent(), role, ok);
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

    return modelValue(model().data(), ind.row(), c, ind.parent(), ok);
  }
  else
    return modelValue(model().data(), ind.row(), ind.column(), ind.parent(), ok);
}

QVariant
CQChartsPlot::
modelValue(QAbstractItemModel *model, int row, const Column &column,
           const QModelIndex &parent, int role, bool &ok) const
{
  return CQChartsModelUtil::modelValue(charts(), model, row, column, parent, role, ok);
}

QVariant
CQChartsPlot::
modelValue(QAbstractItemModel *model, int row, const Column &column,
           const QModelIndex &parent, bool &ok) const
{
  return CQChartsModelUtil::modelValue(charts(), model, row, column, parent, ok);
}

//---

QString
CQChartsPlot::
modelString(int row, const Column &column, const QModelIndex &parent, int role, bool &ok) const
{
  auto *th = const_cast<CQChartsPlot *>(this);

  return modelString(ModelIndex(th, row, column, parent), role, ok);
}

QString
CQChartsPlot::
modelString(int row, const Column &column, const QModelIndex &parent, bool &ok) const
{
  auto *th = const_cast<CQChartsPlot *>(this);

  return modelString(ModelIndex(th, row, column, parent), ok);
}

QString
CQChartsPlot::
modelString(const ModelIndex &ind, int role, bool &ok) const
{
  return modelString(model().data(), ind.row(), ind.column(), ind.parent(), role, ok);
}

QString
CQChartsPlot::
modelString(const ModelIndex &ind, bool &ok) const
{
  return modelString(model().data(), ind.row(), ind.column(), ind.parent(), ok);
}

QString
CQChartsPlot::
modelString(QAbstractItemModel *model, const ModelIndex &ind, bool &ok) const
{
  return CQChartsModelUtil::modelString(charts(), model, ind.row(), ind.column(), ind.parent(), ok);
}

QString
CQChartsPlot::
modelString(QAbstractItemModel *model, int row, const Column &column,
            const QModelIndex &parent, int role, bool &ok) const
{
  return CQChartsModelUtil::modelString(charts(), model, row, column, parent, role, ok);
}

QString
CQChartsPlot::
modelString(QAbstractItemModel *model, int row, const Column &column,
            const QModelIndex &parent, bool &ok) const
{
  return CQChartsModelUtil::modelString(charts(), model, row, column, parent, ok);
}

//---

double
CQChartsPlot::
modelReal(int row, const Column &column, const QModelIndex &parent, int role, bool &ok) const
{
  auto *th = const_cast<CQChartsPlot *>(this);

  return modelReal(ModelIndex(th, row, column, parent), role, ok);
}

double
CQChartsPlot::
modelReal(int row, const Column &column, const QModelIndex &parent, bool &ok) const
{
  auto *th = const_cast<CQChartsPlot *>(this);

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

    return modelReal(model().data(), ind.row(), c, ind.parent(), role, ok);
  }
  else
    return modelReal(model().data(), ind.row(), ind.column(), ind.parent(), role, ok);
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

    return modelReal(model().data(), ind.row(), c, ind.parent(), ok);
  }
  else
    return modelReal(model().data(), ind.row(), ind.column(), ind.parent(), ok);
}

double
CQChartsPlot::
modelReal(QAbstractItemModel *model, int row, const Column &column,
          const QModelIndex &parent, int role, bool &ok) const
{
  return CQChartsModelUtil::modelReal(charts(), model, row, column, parent, role, ok);
}

double
CQChartsPlot::
modelReal(QAbstractItemModel *model, int row, const Column &column,
          const QModelIndex &parent, bool &ok) const
{
  return CQChartsModelUtil::modelReal(charts(), model, row, column, parent, ok);
}

//--

long
CQChartsPlot::
modelInteger(int row, const Column &column, const QModelIndex &parent, int role, bool &ok) const
{
  auto *th = const_cast<CQChartsPlot *>(this);

  return modelInteger(ModelIndex(th, row, column, parent), role, ok);
}

long
CQChartsPlot::
modelInteger(int row, const Column &column, const QModelIndex &parent, bool &ok) const
{
  auto *th = const_cast<CQChartsPlot *>(this);

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

    return modelInteger(model().data(), ind.row(), c, ind.parent(), role, ok);
  }
  else
    return modelInteger(model().data(), ind.row(), ind.column(), ind.parent(), role, ok);
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

    return modelInteger(model().data(), ind.row(), c, ind.parent(), ok);
  }
  else
    return modelInteger(model().data(), ind.row(), ind.column(), ind.parent(), ok);
}

long
CQChartsPlot::
modelInteger(QAbstractItemModel *model, int row, const Column &column,
             const QModelIndex &parent, int role, bool &ok) const
{
  return CQChartsModelUtil::modelInteger(charts(), model, row, column, parent, role, ok);
}

long
CQChartsPlot::
modelInteger(QAbstractItemModel *model, int row, const Column &column,
             const QModelIndex &parent, bool &ok) const
{
  return CQChartsModelUtil::modelInteger(charts(), model, row, column, parent, ok);
}

//--

#if 0
CQChartsColor
CQChartsPlot::
modelColor(int row, const Column &column, const QModelIndex &parent, int role, bool &ok) const
{
  return modelColor(model().data(), row, column, parent, role, ok);
}

CQChartsColor
CQChartsPlot::
modelColor(int row, const Column &column, const QModelIndex &parent, bool &ok) const
{
  return modelColor(model().data(), row, column, parent, ok);
}

CQChartsColor
CQChartsPlot::
modelColor(QAbstractItemModel *model, int row, const Column &column,
           const QModelIndex &parent, int role, bool &ok) const
{
  return CQChartsModelUtil::modelColor(charts(), model, row, column, parent, role, ok);
}

CQChartsColor
CQChartsPlot::
modelColor(QAbstractItemModel *model, int row, const Column &column,
           const QModelIndex &parent, bool &ok) const
{
  return CQChartsModelUtil::modelColor(charts(), model, row, column, parent, ok);
}
#endif

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

  auto var = modelValue(model().data(), row, column, parent, ok);

  if (! ok)
    return reals;

  return CQChartsVariant::toReals(var, ok);
}

//------

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

  std::vector<QSortFilterProxyModel *> proxyModels;
  QAbstractItemModel*                  sourceModel;

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

  auto ind1 = unnormalizeIndex(ind);

  if (! ind1.isValid())
    return;

  // add to map ordered by parent, column, row
  selIndexColumnRows_[ind1.parent()][ind1.column()].insert(ind1.row());
}

void
CQChartsPlot::
endSelectIndex()
{
//auto *model = this->model().data();
//assert(model);

  //---

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
          auto ind1 = modelIndex(startRow, column, parent);
          auto ind2 = modelIndex(endRow  , column, parent);

          optItemSelection.select(ind1, ind2);

          startRow = row;
          endRow   = row;
        }
      }

      if (startRow >= 0) {
        auto ind1 = modelIndex(startRow, column, parent);
        auto ind2 = modelIndex(endRow  , column, parent);

        optItemSelection.select(ind1, ind2);
      }
    }
  }

  //---

  if (optItemSelection.length()) {
    auto *modelData = getModelData();

    if (modelData)
      modelData->select(optItemSelection);
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
positionToPlot(const Position &pos) const
{
  auto p  = pos.p();
  auto p1 = p;

  if      (pos.units() == CQChartsUnits::PIXEL)
    p1 = pixelToWindow(p);
  else if (pos.units() == CQChartsUnits::PLOT)
    p1 = p;
  else if (pos.units() == CQChartsUnits::VIEW)
    p1 = pixelToWindow(view()->windowToPixel(p));
  else if (pos.units() == CQChartsUnits::PERCENT) {
    auto pbbox = displayRangeBBox();

    p1.setX(pbbox.getXMin() + p.getX()*pbbox.getWidth ()/100.0);
    p1.setY(pbbox.getYMin() + p.getY()*pbbox.getHeight()/100.0);
  }
  else if (pos.units() == CQChartsUnits::EM) {
    double x = pixelToWindowWidth (p.getX()*view()->fontEm());
    double y = pixelToWindowHeight(p.getY()*view()->fontEm());

    return Point(x, y);
  }
  else if (pos.units() == CQChartsUnits::EX) {
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

  if      (pos.units() == CQChartsUnits::PIXEL)
    p1 = p;
  else if (pos.units() == CQChartsUnits::PLOT)
    p1 = windowToPixel(p);
  else if (pos.units() == CQChartsUnits::VIEW)
    p1 = view()->windowToPixel(p);
  else if (pos.units() == CQChartsUnits::PERCENT) {
    auto pbbox = calcPlotPixelRect();

    p1.setX(pbbox.getXMin() + p.getX()*pbbox.getWidth ()/100.0);
    p1.setY(pbbox.getYMin() + p.getY()*pbbox.getHeight()/100.0);
  }
  else if (pos.units() == CQChartsUnits::EM) {
    double x = p.getX()*view()->fontEm();
    double y = p.getY()*view()->fontEm();

    return Point(x, y);
  }
  else if (pos.units() == CQChartsUnits::EX) {
    double x = p.getX()*view()->fontEx();
    double y = p.getY()*view()->fontEx();

    return Point(x, y);
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

  if      (rect.units() == CQChartsUnits::PIXEL)
    r1 = pixelToWindow(r);
  else if (rect.units() == CQChartsUnits::PLOT)
    r1 = r;
  else if (rect.units() == CQChartsUnits::VIEW)
    r1 = pixelToWindow(view()->windowToPixel(r));
  else if (rect.units() == CQChartsUnits::PERCENT) {
    auto pbbox = displayRangeBBox();

    r1.setXMin(pbbox.getXMin() + r.getXMin()*pbbox.getWidth ()/100.0);
    r1.setYMin(pbbox.getYMin() + r.getYMin()*pbbox.getHeight()/100.0);
    r1.setXMax(pbbox.getXMin() + r.getXMax()*pbbox.getWidth ()/100.0);
    r1.setYMax(pbbox.getYMin() + r.getYMax()*pbbox.getHeight()/100.0);
  }
  else if (rect.units() == CQChartsUnits::EM) {
    double x1 = pixelToWindowWidth (r.getXMin()*view()->fontEm());
    double y1 = pixelToWindowHeight(r.getYMin()*view()->fontEm());
    double x2 = pixelToWindowWidth (r.getXMax()*view()->fontEm());
    double y2 = pixelToWindowHeight(r.getYMax()*view()->fontEm());

    return BBox(x1, y1, x2, y2);
  }
  else if (rect.units() == CQChartsUnits::EX) {
    double x1 = pixelToWindowWidth (r.getXMin()*view()->fontEx());
    double y1 = pixelToWindowHeight(r.getYMin()*view()->fontEx());
    double x2 = pixelToWindowWidth (r.getXMax()*view()->fontEx());
    double y2 = pixelToWindowHeight(r.getYMax()*view()->fontEx());

    return BBox(x1, y1, x2, y2);
  }

  return r1;
}

CQChartsGeom::BBox
CQChartsPlot::
rectToPixel(const Rect &rect) const
{
  auto r  = rect.bbox();
  auto r1 = r;

  if      (rect.units() == CQChartsUnits::PIXEL)
    r1 = r;
  else if (rect.units() == CQChartsUnits::PLOT)
    r1 = windowToPixel(r);
  else if (rect.units() == CQChartsUnits::VIEW)
    r1 = view()->windowToPixel(r);
  else if (rect.units() == CQChartsUnits::PERCENT) {
    auto pbbox = calcPlotPixelRect();

    r1.setXMin(pbbox.getXMin() + r.getXMin()*pbbox.getWidth ()/100.0);
    r1.setYMin(pbbox.getYMin() + r.getYMin()*pbbox.getHeight()/100.0);
    r1.setXMax(pbbox.getXMin() + r.getXMax()*pbbox.getWidth ()/100.0);
    r1.setYMax(pbbox.getYMin() + r.getYMax()*pbbox.getHeight()/100.0);
  }
  else if (rect.units() == CQChartsUnits::EM) {
    double x1 = r.getXMin()*view()->fontEm();
    double y1 = r.getYMin()*view()->fontEm();
    double x2 = r.getXMax()*view()->fontEm();
    double y2 = r.getYMax()*view()->fontEm();

    return BBox(x1, y1, x2, y2);
  }
  else if (rect.units() == CQChartsUnits::EX) {
    double x1 = r.getXMin()*view()->fontEx();
    double y1 = r.getYMin()*view()->fontEx();
    double x2 = r.getXMax()*view()->fontEx();
    double y2 = r.getYMax()*view()->fontEx();

    return BBox(x1, y1, x2, y2);
  }

  return r1;
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
lengthPlotWidth(const Length &len) const
{
  if      (len.units() == CQChartsUnits::PIXEL)
    return pixelToWindowWidth(len.value());
  else if (len.units() == CQChartsUnits::PLOT)
    return len.value();
  else if (len.units() == CQChartsUnits::VIEW)
    return pixelToWindowWidth(view()->windowToPixelWidth(len.value()));
  else if (len.units() == CQChartsUnits::PERCENT)
    return len.value()*displayRangeBBox().getWidth()/100.0;
  else if (len.units() == CQChartsUnits::EM)
    return pixelToWindowWidth(len.value()*view()->fontEm());
  else if (len.units() == CQChartsUnits::EX)
    return pixelToWindowWidth(len.value()*view()->fontEx());

  return len.value();
}

double
CQChartsPlot::
lengthPlotHeight(const Length &len) const
{
  if      (len.units() == CQChartsUnits::PIXEL)
    return pixelToWindowHeight(len.value());
  else if (len.units() == CQChartsUnits::PLOT)
    return len.value();
  else if (len.units() == CQChartsUnits::VIEW)
    return pixelToWindowHeight(view()->windowToPixelHeight(len.value()));
  else if (len.units() == CQChartsUnits::PERCENT)
    return len.value()*displayRangeBBox().getHeight()/100.0;
  else if (len.units() == CQChartsUnits::EM)
    return pixelToWindowHeight(len.value()*view()->fontEm());
  else if (len.units() == CQChartsUnits::EX)
    return pixelToWindowHeight(len.value()*view()->fontEx());

  return len.value();
}

double
CQChartsPlot::
lengthPlotSignedWidth(const Length &len) const
{
  if      (len.units() == CQChartsUnits::PIXEL)
    return pixelToSignedWindowWidth(len.value());
  else if (len.units() == CQChartsUnits::PLOT)
    return len.value();
  else if (len.units() == CQChartsUnits::VIEW)
    return pixelToSignedWindowWidth(view()->windowToPixelWidth(len.value()));
  else if (len.units() == CQChartsUnits::PERCENT)
    return len.value()*displayRangeBBox().getWidth()/100.0;
  else if (len.units() == CQChartsUnits::EM)
    return pixelToSignedWindowWidth(len.value()*view()->fontEm());
  else if (len.units() == CQChartsUnits::EX)
    return pixelToSignedWindowWidth(len.value()*view()->fontEx());

  return len.value();
}

double
CQChartsPlot::
lengthPlotSignedHeight(const Length &len) const
{
  if      (len.units() == CQChartsUnits::PIXEL)
    return pixelToSignedWindowHeight(len.value());
  else if (len.units() == CQChartsUnits::PLOT)
    return len.value();
  else if (len.units() == CQChartsUnits::VIEW)
    return pixelToSignedWindowHeight(view()->windowToPixelHeight(len.value()));
  else if (len.units() == CQChartsUnits::PERCENT)
    return len.value()*displayRangeBBox().getHeight()/100.0;
  else if (len.units() == CQChartsUnits::EM)
    return pixelToSignedWindowHeight(len.value()*view()->fontEm());
  else if (len.units() == CQChartsUnits::EX)
    return pixelToSignedWindowHeight(len.value()*view()->fontEx());

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
  if      (len.units() == CQChartsUnits::PIXEL)
    return len.value();
  else if (len.units() == CQChartsUnits::PLOT)
    return windowToPixelWidth(len.value());
  else if (len.units() == CQChartsUnits::VIEW)
    return view()->windowToPixelWidth(len.value());
  else if (len.units() == CQChartsUnits::PERCENT)
    return len.value()*calcPlotPixelRect().getWidth()/100.0;
  else if (len.units() == CQChartsUnits::EM)
    return len.value()*view()->fontEm();
  else if (len.units() == CQChartsUnits::EX)
    return len.value()*view()->fontEx();

  return len.value();
}

double
CQChartsPlot::
lengthPixelHeight(const Length &len) const
{
  if      (len.units() == CQChartsUnits::PIXEL)
    return len.value();
  else if (len.units() == CQChartsUnits::PLOT)
    return windowToPixelHeight(len.value());
  else if (len.units() == CQChartsUnits::VIEW)
    return view()->windowToPixelHeight(len.value());
  else if (len.units() == CQChartsUnits::PERCENT)
    return len.value()*calcPlotPixelRect().getHeight()/100.0;
  else if (len.units() == CQChartsUnits::EM)
    return len.value()*view()->fontEm();
  else if (len.units() == CQChartsUnits::EX)
    return len.value()*view()->fontEx();

  return len.value();
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

void
CQChartsPlot::
pixelToWindowI(const BBox &prect, BBox &wrect) const
{
  wrect = pixelToWindow(prect);
}

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
  if (parentPlot())
    return parentPlot()->pixelToWindowI(px, py, wx, wy);

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

void
CQChartsPlot::
windowToPixelI(const BBox &wrect, BBox &prect) const
{
  prect = windowToPixel(wrect);
}

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
  if (parentPlot())
    return parentPlot()->windowToPixelI(wx, wy, px, py);

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

CQChartsGeom::Size
CQChartsPlot::
pixelToWindowSize(const Size &ps) const
{
  double w = pixelToWindowWidth (ps.width ());
  double h = pixelToWindowHeight(ps.height());

  return Size(w, h);
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
  QPainterPath ppath;

  int n = path.elementCount();

  for (int i = 0; i < n; ++i) {
    const auto &e = path.elementAt(i);

    if      (e.isMoveTo()) {
      auto p1 = windowToPixel(Point(e.x, e.y));

      ppath.moveTo(p1.qpoint());
    }
    else if (e.isLineTo()) {
      auto p1 = windowToPixel(Point(e.x, e.y));

      ppath.lineTo(p1.qpoint());
    }
    else if (e.isCurveTo()) {
      QPainterPath::Element     e1, e2;
      QPainterPath::ElementType e1t { QPainterPath::MoveToElement };
      QPainterPath::ElementType e2t { QPainterPath::MoveToElement };

      auto p1 = windowToPixel(Point(e.x, e.y));

      if (i < n - 1) {
        e1  = path.elementAt(i + 1);
        e1t = e1.type;
      }

      if (i < n - 2) {
        e2  = path.elementAt(i + 2);
        e2t = e2.type;
      }

      if (e1t == QPainterPath::CurveToDataElement) {
        auto p2 = windowToPixel(Point(e1.x, e1.y)); ++i;

        if (e2t == QPainterPath::CurveToDataElement) {
          auto p3 = windowToPixel(Point(e2.x, e2.y)); ++i;

          ppath.cubicTo(p1.qpoint(), p2.qpoint(), p3.qpoint());
        }
        else {
          ppath.quadTo(p1.qpoint(), p2.qpoint());
        }
      }
    }
    else {
      assert(false);
    }
  }

  return ppath;
}

//------

void
CQChartsPlot::
plotSymbolSize(const Length &s, double &sx, double &sy) const
{
  sx = lengthPlotWidth (s);
  sy = lengthPlotHeight(s);
}

void
CQChartsPlot::
pixelSymbolSize(const Length &s, double &sx, double &sy) const
{
  sx = limitSymbolSize(lengthPixelWidth (s));
  sy = limitSymbolSize(lengthPixelHeight(s));
}

double
CQChartsPlot::
limitSymbolSize(double s) const
{
  // ensure not a crazy number : TODO: property for limits
  return CMathUtil::clamp(s, 1.0, CQChartsSymbolSize::maxPixelValue());
}

double
CQChartsPlot::
limitFontSize(double s) const
{
  // ensure not a crazy number : TODO: property for limits
  return CMathUtil::clamp(s, 1.0, CQChartsFontSize::maxPixelValue());
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
  return dataRange_.inside(p);
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

  //auto *modelData = getModelData();

  os << "set " << plotName().toStdString();
  os << " [create_charts_plot -model $" << modelName().toStdString() <<
        " -type " << type_->name().toStdString();

  //---

  // add columns
  QVariantList columnsStrs;
//QStringList parametersStrs;
  QStringList parameterPropPaths;

  for (const auto &param : type()->parameters()) {
    QString defStr;

    if (! CQChartsVariant::toString(param->defValue(), defStr))
      defStr = "";

    QVariant value;

    if (! getParameter(param, value))
      continue;

    QString str;

    if (! CQChartsVariant::toString(value, str))
      str = "";

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

    bool rc = getProperty(propPath, value);
    assert(rc);

    nameValues[propPath] = value;
  }

  //---

  if (! nameValues.empty())
    os << "\n";

  for (const auto &nv : nameValues) {
    QString str;

    if (! CQChartsVariant::toString(nv.second, str))
      str = "";

    os << "set_charts_property -plot $" << plotName().toStdString();

    os << " -name " << nv.first.toStdString() << " -value {" << str.toStdString() << "}\n";
  }
}
