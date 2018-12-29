#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsTitle.h>
#include <CQChartsPlotObj.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsPlotObjTree.h>
#include <CQChartsNoDataObj.h>
#include <CQChartsAnnotation.h>
#include <CQChartsValueSet.h>
#include <CQChartsDisplayTransform.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsRotatedText.h>
#include <CQChartsGradientPalette.h>
#include <CQChartsModelExprMatch.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsPlotParameter.h>
#include <CQChartsColumnType.h>
#include <CQChartsModelUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsEnv.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPerfMonitor.h>
#include <CQUtil.h>

#include <CMathUtil.h>
#include <CMathRound.h>

#include <QApplication>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>
#include <QPainter>

//------

CQChartsPlot::
CQChartsPlot(CQChartsView *view, CQChartsPlotType *type, const ModelP &model) :
 CQChartsObjPlotShapeData<CQChartsPlot>(this),
 CQChartsObjDataShapeData<CQChartsPlot>(this),
 CQChartsObjFitShapeData <CQChartsPlot>(this),
 view_(view), type_(type), model_(model), editHandles_(view)
{
  NoUpdate noUpdate(this);

  updateTimeout_ = CQChartsEnv::getInt("CQ_CHARTS_PLOT_UPDATE_TIMEOUT", updateTimeout_);

  preview_ = CQChartsEnv::getInt("CQ_CHARTS_PLOT_PREVIEW", preview_);

  bufferSymbols_ = CQChartsEnv::getInt("CQ_CHARTS_BUFFER_SYMBOLS", bufferSymbols_);

  displayRange_     = new CQChartsDisplayRange();
  displayTransform_ = new CQChartsDisplayTransform(displayRange_);

  displayRange_->setPixelAdjust(0.0);

  bool objTreeWait = CQChartsEnv::getBool("CQ_CHARTS_OBJ_TREE_WAIT", false);

  plotObjTree_ = new CQChartsPlotObjTree(this, objTreeWait);

  //--

  // plot, data, fit background
  setPlotFilled(true ); setPlotBorder(false);
  setDataFilled(true ); setDataBorder(false);
  setFitFilled (false); setFitBorder (false);

  setDataClip(true);

  setPlotFillColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.00));
  setDataFillColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.12));
  setFitFillColor (CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.08));

  //--

  double vr = CQChartsView::viewportRange();

  viewBBox_      = CQChartsGeom::BBox(0, 0, vr, vr);
  innerViewBBox_ = viewBBox_;

  displayRange_->setPixelAdjust(0.0);

  displayRange_->setPixelRange (0, vr, vr, 0);
  displayRange_->setWindowRange(0, 0, 1, 1);

  //---

  // all layers active except BG_PLOT and FG_PLOT
  initLayer(CQChartsLayer::Type::BACKGROUND , CQChartsBuffer::Type::BACKGROUND, true );
  initLayer(CQChartsLayer::Type::BG_AXES    , CQChartsBuffer::Type::BACKGROUND, true );
  initLayer(CQChartsLayer::Type::BG_KEY     , CQChartsBuffer::Type::BACKGROUND, true );

  initLayer(CQChartsLayer::Type::BG_PLOT    , CQChartsBuffer::Type::MIDDLE    , false);
  initLayer(CQChartsLayer::Type::MID_PLOT   , CQChartsBuffer::Type::MIDDLE    , true );
  initLayer(CQChartsLayer::Type::FG_PLOT    , CQChartsBuffer::Type::MIDDLE    , false);

  initLayer(CQChartsLayer::Type::FG_AXES    , CQChartsBuffer::Type::FOREGROUND, true );
  initLayer(CQChartsLayer::Type::FG_KEY     , CQChartsBuffer::Type::FOREGROUND, true );
  initLayer(CQChartsLayer::Type::TITLE      , CQChartsBuffer::Type::FOREGROUND, true );
  initLayer(CQChartsLayer::Type::ANNOTATION , CQChartsBuffer::Type::FOREGROUND, true );
  initLayer(CQChartsLayer::Type::FOREGROUND , CQChartsBuffer::Type::FOREGROUND, true );

  initLayer(CQChartsLayer::Type::EDIT_HANDLE, CQChartsBuffer::Type::OVERLAY   , true );
  initLayer(CQChartsLayer::Type::BOXES      , CQChartsBuffer::Type::OVERLAY   , true );
  initLayer(CQChartsLayer::Type::SELECTION  , CQChartsBuffer::Type::OVERLAY   , true );
  initLayer(CQChartsLayer::Type::MOUSE_OVER , CQChartsBuffer::Type::OVERLAY   , true );

  //---

  connectModel();

  //---

  updateTimer_ = new CQChartsPlotUpdateTimer(this);

  connect(updateTimer_, SIGNAL(timeout()), this, SLOT(updateTimerSlot()));
}

CQChartsPlot::
~CQChartsPlot()
{
  clearPlotObjects();

  for (auto &layer : layers_)
    delete layer.second;

  for (auto &buffer : buffers_)
    delete buffer.second;

  for (auto &annotation : annotations())
    delete annotation;

  delete plotObjTree_;

  delete displayRange_;
  delete displayTransform_;

  delete titleObj_;
  delete keyObj_;
  delete xAxis_;
  delete yAxis_;

  delete animateData_.timer;

  delete updateTimer_;
}

QString
CQChartsPlot::
viewId() const
{
  return view_->id();
}

QString
CQChartsPlot::
typeStr() const
{
  return type_->name();
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
  if (! model_.data())
    return;

  CQChartsModelData *modelData = getModelData();

  modelNameSet_ = false;

  if (modelData) {
    if (! modelData->name().length() && this->hasId()) {
      charts()->setModelName(modelData, this->id());

      modelNameSet_ = true;
    }

    connect(modelData, SIGNAL(modelChanged()), this, SLOT(modelChangedSlot()));

    connect(modelData, SIGNAL(currentModelChanged()), this, SLOT(currentModelChangedSlot()));
  }
  else {
    // TODO: check if model uses changed columns
    //int column1 = tl.column();
    //int column2 = br.column();
    connect(model_.data(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(modelChangedSlot()));

    connect(model_.data(), SIGNAL(layoutChanged()),
            this, SLOT(modelChangedSlot()));
    connect(model_.data(), SIGNAL(modelReset()),
            this, SLOT(modelChangedSlot()));

    connect(model_.data(), SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(modelChangedSlot()));
    connect(model_.data(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(modelChangedSlot()));
    connect(model_.data(), SIGNAL(columnsInserted(QModelIndex,int,int)),
            this, SLOT(modelChangedSlot()));
    connect(model_.data(), SIGNAL(columnsRemoved(QModelIndex,int,int)),
            this, SLOT(modelChangedSlot()));
  }
}

void
CQChartsPlot::
disconnectModel()
{
  if (! model_.data())
    return;

  CQChartsModelData *modelData = getModelData();

  if (modelData) {
    if (modelNameSet_) {
      charts()->setModelName(modelData, this->id());

      modelNameSet_ = false;
    }

    disconnect(modelData, SIGNAL(modelChanged()), this, SLOT(modelChangedSlot()));

    disconnect(modelData, SIGNAL(currentModelChanged()), this, SLOT(modelChangedSlot()));
  }
  else {
    disconnect(model_.data(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
               this, SLOT(modelChangedSlot()));
    disconnect(model_.data(), SIGNAL(layoutChanged()),
               this, SLOT(modelChangedSlot()));
    disconnect(model_.data(), SIGNAL(modelReset()),
               this, SLOT(modelChangedSlot()));

    disconnect(model_.data(), SIGNAL(rowsInserted(QModelIndex,int,int)),
               this, SLOT(modelChangedSlot()));
    disconnect(model_.data(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
               this, SLOT(modelChangedSlot()));
    disconnect(model_.data(), SIGNAL(columnsInserted(QModelIndex,int,int)),
               this, SLOT(modelChangedSlot()));
    disconnect(model_.data(), SIGNAL(columnsRemoved(QModelIndex,int,int)),
               this, SLOT(modelChangedSlot()));
  }
}

//---

void
CQChartsPlot::
startAnimateTimer()
{
  animateData_.timer = new QTimer;

  connect(animateData_.timer, SIGNAL(timeout()), this, SLOT(animateSlot()));

  animateData_.timer->start(animateData_.tickLen);
}

void
CQChartsPlot::
animateSlot()
{
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
  CQChartsModelData *modelData = qobject_cast<CQChartsModelData *>(sender());

  if (! modelData)
    return;

  setModel(modelData->currentModel());
}

//---

void
CQChartsPlot::
setSelectionModel(QItemSelectionModel *sm)
{
  QItemSelectionModel *sm1 = this->selectionModel();

  if (sm1)
    disconnect(sm1, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
               this, SLOT(selectionSlot()));

  selectionModel_ = sm;

  if (sm)
    connect(sm, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(selectionSlot()));
}

QItemSelectionModel *
CQChartsPlot::
selectionModel() const
{
  return selectionModel_.data();
}

void
CQChartsPlot::
selectionSlot()
{
  QItemSelectionModel *sm = this->selectionModel();
  if (! sm) return;

  QModelIndexList indices = sm->selectedIndexes();
  if (indices.empty()) return;

  //---

  view()->startSelection();

  // deselect all objects
  deselectAllObjs();

  // select objects with matching indices
  for (int i = 0; i < indices.size(); ++i) {
    const QModelIndex &ind = indices[i];

    QModelIndex ind1 = normalizeIndex(ind);

    for (auto &plotObj : plotObjects()) {
      if (plotObj->isSelectIndex(ind1))
        plotObj->setSelected(true);
    }
  }

  view()->endSelection();

  //---

  invalidateLayer(CQChartsBuffer::Type::FOREGROUND);

  if (selectInvalidateObjs())
    invalidateLayer(CQChartsBuffer::Type::MIDDLE);
}

//---

CQCharts *
CQChartsPlot::
charts() const
{
  return view_->charts();
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
  return view_->id() + ":" + id();
}

//---

void
CQChartsPlot::
setVisible(bool b)
{
  CQChartsUtil::testAndSet(visible_, b, [&]() { invalidateLayers(); } );
}

void
CQChartsPlot::
setSelected(bool b)
{
  CQChartsUtil::testAndSet(selected_, b, [&]() { invalidateLayers(); } );
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
        if      (updatesData_.updateRangeAndObjs) {
          updateRangeAndObjs();

          invalidateLayers();
        }
        else if (updatesData_.updateObjs) {
          updateObjs();

          invalidateLayers();
        }
        else if (updatesData_.applyDataRange) {
          applyDataRange();

          invalidateLayers();
        }
        else if (updatesData_.invalidateLayers) {
          invalidateLayers();
        }
      }

      updatesData_.updateRangeAndObjs = false;
      updatesData_.updateObjs         = false;
      updatesData_.applyDataRange     = false;
      updatesData_.invalidateLayers   = false;
    }
  }
  else {
    if (updatesData_.enabled == 0) {
      updatesData_.updateRangeAndObjs = false;
      updatesData_.updateObjs         = false;
      updatesData_.applyDataRange     = false;
      updatesData_.invalidateLayers   = false;
    }

    ++updatesData_.enabled;
  }
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
setDisplayRange(const CQChartsDisplayRange &r)
{
  *displayRange_ = r;
}

const CQChartsDisplayTransform &
CQChartsPlot::
displayTransform() const
{
  return *displayTransform_;
}

void
CQChartsPlot::
setDisplayTransform(const CQChartsDisplayTransform &t)
{
  *displayTransform_ = t;
}

//---

void
CQChartsPlot::
setDataRange(const CQChartsGeom::Range &r, bool update)
{
  CQChartsUtil::testAndSet(dataRange_, r, [&]() {
    if (update)
      updateObjs();
  });
}

//---

void
CQChartsPlot::
setDataScaleX(double r)
{
  zoomData_.dataScaleX = r;
}

void
CQChartsPlot::
setDataScaleY(double r)
{
  zoomData_.dataScaleY = r;
}

void
CQChartsPlot::
setDataOffsetX(double x)
{
  zoomData_.dataOffset.x = x;
}

void
CQChartsPlot::
setDataOffsetY(double y)
{
  zoomData_.dataOffset.y = y;
}

void
CQChartsPlot::
setZoomData(const ZoomData &zoomData)
{
  zoomData_ = zoomData;

  applyDataRange();

  invalidateLayers();
}

void
CQChartsPlot::
updateDataScaleX(double r)
{
  setDataScaleX(r);

  applyDataRange();

  invalidateLayers();
}

void
CQChartsPlot::
updateDataScaleY(double r)
{
  setDataScaleY(r);

  applyDataRange();

  invalidateLayers();
}

//---

void
CQChartsPlot::
setXMin(const OptReal &r)
{
  CQChartsUtil::testAndSet(xmin_, r, [&]() { updateObjs(); } );
}

void
CQChartsPlot::
setXMax(const OptReal &r)
{
  CQChartsUtil::testAndSet(xmax_, r, [&]() { updateObjs(); } );
}

void
CQChartsPlot::
setYMin(const OptReal &r)
{
  CQChartsUtil::testAndSet(ymin_, r, [&]() { updateObjs(); } );
}

void
CQChartsPlot::
setYMax(const OptReal &r)
{
  CQChartsUtil::testAndSet(ymax_, r, [&]() { updateObjs(); } );
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

//---

void
CQChartsPlot::
setTitleStr(const QString &s)
{
  CQChartsUtil::testAndSet(titleStr_, s, [&]() {
    if (title())
      title()->setTextStr(titleStr_);
  } );
}

//---

void
CQChartsPlot::
setPlotBorderSides(const CQChartsSides &s)
{
  CQChartsUtil::testAndSet(plotBorderSides_, s, [&]() {
    invalidateLayer(CQChartsBuffer::Type::BACKGROUND); } );
}

void
CQChartsPlot::
setPlotClip(bool b)
{
  CQChartsUtil::testAndSet(plotClip_, b, [&]() { invalidateLayers(); } );
}

//---

void
CQChartsPlot::
setDataBorderSides(const CQChartsSides &s)
{
  CQChartsUtil::testAndSet(dataBorderSides_, s, [&]() {
    invalidateLayer(CQChartsBuffer::Type::BACKGROUND); } );
}

void
CQChartsPlot::
setDataClip(bool b)
{
  CQChartsUtil::testAndSet(dataClip_, b, [&]() { invalidateLayers(); } );
}

//---

void
CQChartsPlot::
setFitBorderSides(const CQChartsSides &s)
{
  CQChartsUtil::testAndSet(fitBorderSides_, s, [&]() {
    invalidateLayer(CQChartsBuffer::Type::BACKGROUND); } );
}

//---

bool
CQChartsPlot::
isKeyVisible() const
{
  return (key() ? key()->isVisible() : false);
}

void
CQChartsPlot::
setKeyVisible(bool b)
{
  if (key())
    key()->setVisible(b);
}

//---

void
CQChartsPlot::
setEqualScale(bool b)
{
  CQChartsUtil::testAndSet(equalScale_, b, [&]() {
    setDataRange(CQChartsGeom::Range());

    //updateMargins();
  });
}

void
CQChartsPlot::
setShowBoxes(bool b)
{
  CQChartsUtil::testAndSet(showBoxes_, b, [&]() {
    invalidateLayer(CQChartsBuffer::Type::OVERLAY); } );
}

void
CQChartsPlot::
setViewBBox(const CQChartsGeom::BBox &bbox)
{
  viewBBox_      = bbox;
  innerViewBBox_ = viewBBox_;

  updateMargins();
}

void
CQChartsPlot::
updateMargins(bool update)
{
  if (isOverlay()) {
    if (this != firstPlot()) {
      firstPlot()->updateMargins(update);
      return;
    }

    processOverlayPlots([&](CQChartsPlot *plot) {
      plot->updateMargins(outerMargin());
    });
  }
  else {
    updateMargins(outerMargin());
  }

  updateKeyPosition(/*force*/true);

  if (update)
    invalidateLayers();
}

void
CQChartsPlot::
updateMargins(const CQChartsPlotMargin &outerMargin)
{
  innerViewBBox_ = outerMargin.adjustViewRange(this, viewBBox_, /*inside*/false);

  displayRange_->setPixelRange(innerViewBBox_.getXMin(), innerViewBBox_.getYMax(),
                               innerViewBBox_.getXMax(), innerViewBBox_.getYMin());
}

QRectF
CQChartsPlot::
viewRect() const
{
  return CQChartsUtil::toQRect(viewBBox());
}

void
CQChartsPlot::
setViewRect(const QRectF &r)
{
  setViewBBox(CQChartsUtil::fromQRect(r));
}

QRectF
CQChartsPlot::
innerViewRect() const
{
  return CQChartsUtil::toQRect(innerViewBBox());
}

//---

QRectF
CQChartsPlot::
calcDataRect() const
{
  if (calcDataRange_.isSet())
    return CQChartsUtil::toQRect(calcDataRange_);
  else
    return QRectF();
}

QRectF
CQChartsPlot::
outerDataRect() const
{
  if (outerDataRange_.isSet())
    return CQChartsUtil::toQRect(outerDataRange_);
  else
    return QRectF();
}

QRectF
CQChartsPlot::
dataRect() const
{
  if (dataRange_.isSet())
    return CQChartsUtil::toQRect(dataRange_);
  else
    return QRectF();
}

//---

QRectF
CQChartsPlot::
range() const
{
  return dataRect();
}

void
CQChartsPlot::
setRange(const QRectF &r)
{
  if (view_->isZoomData()) {
    assert(dataScaleX() == 1.0 && dataScaleY() == 1.0);
  }

  CQChartsGeom::BBox bbox = CQChartsUtil::fromQRect(r);

  CQChartsGeom::Range range = CQChartsUtil::bboxRange(bbox);

  setDataRange(range);

  applyDataRange();
}

double
CQChartsPlot::
aspect() const
{
  double px1, py1, px2, py2;

  view_->windowToPixel(viewBBox_.getXMin(), viewBBox_.getYMin(), px1, py1);
  view_->windowToPixel(viewBBox_.getXMax(), viewBBox_.getYMax(), px2, py2);

  if (py1 == py2)
    return 1.0;

  return fabs(px2 - px1)/fabs(py2 - py1);
}

//---

// inner margin
void
CQChartsPlot::
setInnerMarginLeft(const CQChartsLength &l)
{
  if (l != innerMargin_.left()) {
    innerMargin_.setLeft(l);

    applyDataRange();

    invalidateLayers();
  }
}

void
CQChartsPlot::
setInnerMarginTop(const CQChartsLength &t)
{
  if (t != innerMargin_.top()) {
    innerMargin_.setTop(t);

    applyDataRange();

    invalidateLayers();
  }
}

void
CQChartsPlot::
setInnerMarginRight(const CQChartsLength &r)
{
  if (r != innerMargin_.right()) {
    innerMargin_.setRight(r);

    applyDataRange();

    invalidateLayers();
  }
}

void
CQChartsPlot::
setInnerMarginBottom(const CQChartsLength &b)
{
  if (b != innerMargin_.bottom()) {
    innerMargin_.setBottom(b);

    applyDataRange();

    invalidateLayers();
  }
}

void
CQChartsPlot::
setInnerMargin(const CQChartsLength &l, const CQChartsLength &t,
               const CQChartsLength &r, const CQChartsLength &b)
{
  setInnerMargin(CQChartsPlotMargin(l, t, r, b));
}

void
CQChartsPlot::
setInnerMargin(const CQChartsPlotMargin &m)
{
  innerMargin_ = m;

  adjustDataRange();

  invalidateLayers();
}

//---

void
CQChartsPlot::
setOuterMarginLeft(const CQChartsLength &l)
{
  if (l != outerMargin_.left()) {
    outerMargin_.setLeft(l);

    updateMargins();
  }
}

void
CQChartsPlot::
setOuterMarginTop(const CQChartsLength &t)
{
  if (t != outerMargin_.top()) {
    outerMargin_.setTop(t);

    updateMargins();
  }
}

void
CQChartsPlot::
setOuterMarginRight(const CQChartsLength &r)
{
  if (r != outerMargin_.right()) {
    outerMargin_.setRight(r);

    updateMargins();
  }
}

void
CQChartsPlot::
setOuterMarginBottom(const CQChartsLength &b)
{
  if (b != outerMargin_.bottom()) {
    outerMargin_.setBottom(b);

    updateMargins();
  }
}

void
CQChartsPlot::
setOuterMargin(const CQChartsLength &l, const CQChartsLength &t,
               const CQChartsLength &r, const CQChartsLength &b)
{
  return setOuterMargin(CQChartsPlotMargin(l, t, r, b));
}

void
CQChartsPlot::
setOuterMargin(const CQChartsPlotMargin &m)
{
  outerMargin_ = m;

  updateMargins();
}

//---

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
setX1X2(bool b, bool notify)
{
  connectData_.x1x2 = b;

  if (notify)
    emit connectDataChanged();
}

void
CQChartsPlot::
setY1Y2(bool b, bool notify)
{
  connectData_.y1y2 = b;

  if (notify)
    emit connectDataChanged();
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
firstPlot() const
{
  if (connectData_.prev)
    return connectData_.prev->firstPlot();

  return const_cast<CQChartsPlot *>(this);
}

CQChartsPlot *
CQChartsPlot::
lastPlot() const
{
  if (connectData_.next)
    return connectData_.next->lastPlot();

  return const_cast<CQChartsPlot *>(this);
}

void
CQChartsPlot::
overlayPlots(Plots &plots) const
{
  CQChartsPlot *plot1 = firstPlot();

  while (plot1) {
    plots.push_back(plot1);

    plot1 = plot1->nextPlot();
  }
}

void
CQChartsPlot::
x1x2Plots(CQChartsPlot* &plot1, CQChartsPlot* &plot2)
{
  plot1 = firstPlot();
  plot2 = (plot1 ? plot1->nextPlot() : nullptr);
}

void
CQChartsPlot::
y1y2Plots(CQChartsPlot* &plot1, CQChartsPlot* &plot2)
{
  plot1 = firstPlot();
  plot2 = (plot1 ? plot1->nextPlot() : nullptr);
}

void
CQChartsPlot::
resetConnectData(bool notify)
{
  connectData_.reset();

  if (notify)
    emit connectDataChanged();
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

  invalidateLayers();
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

  invalidateLayers();
}

void
CQChartsPlot::
setLogX(bool b)
{
  CQChartsUtil::testAndSet(logX_, b, [&]() {
    if (xAxis())
      xAxis()->setLog(b);

    updateRangeAndObjs();
  });
}

void
CQChartsPlot::
setLogY(bool b)
{
  CQChartsUtil::testAndSet(logY_, b, [&]() {
    if (yAxis())
      yAxis()->setLog(b);

    updateRangeAndObjs();
  });
}

//------

CQPropertyViewModel *
CQChartsPlot::
propertyModel() const
{
  return view_->propertyModel();
}

void
CQChartsPlot::
addProperties()
{
  addProperty("", this, "viewId" );
  addProperty("", this, "typeStr");
  addProperty("", this, "visible");

  addProperty("columns", this, "idColumn"     , "id"     );
  addProperty("columns", this, "tipColumn"    , "tip"    );
  addProperty("columns", this, "visibleColumn", "visible");
  addProperty("columns", this, "colorColumn"  , "color"  );
  addProperty("columns", this, "imageColumn"  , "image"  );

  addProperty("range", this, "viewRect"     , "view"     );
  addProperty("range", this, "innerViewRect", "innerView");

  addProperty("range", this, "dataRect"     , "data"     );
  addProperty("range", this, "calcDataRect" , "calcData" );
  addProperty("range", this, "outerDataRect", "outerData");

  addProperty("range", this, "autoFit", "autoFit");

  addProperty("scaling"            , this, "equalScale" , "equal");
  addProperty("scaling/data/scale" , this, "dataScaleX" , "x"    );
  addProperty("scaling/data/scale" , this, "dataScaleY" , "y"    );
  addProperty("scaling/data/offset", this, "dataOffsetX", "x"    );
  addProperty("scaling/data/offset", this, "dataOffsetY", "y"    );

  addProperty("grouping", this, "overlay");
  addProperty("grouping", this, "x1x2"   );
  addProperty("grouping", this, "y1y2"   );

  addProperty("invert", this, "invertX", "x");
  addProperty("invert", this, "invertY", "y");

  addProperty("log", this, "logX", "x");
  addProperty("log", this, "logY", "y");

  if (CQChartsEnv::getBool("CQ_CHARTS_DEBUG", true)) {
    addProperty("debug", this, "showBoxes"    );
    addProperty("debug", this, "followMouse"  );
    addProperty("debug", this, "updateTimeout");
  }

  //------

  QString plotStyleStr       = "plotBackground";
  QString plotStyleFillStr   = plotStyleStr + "/fill";
  QString plotStyleStrokeStr = plotStyleStr + "/stroke";

  addProperty(plotStyleStr      , this, "plotClip"       , "clip");
  addProperty(plotStyleFillStr  , this, "plotFilled"     , "visible");
  addProperty(plotStyleStrokeStr, this, "plotBorder"     , "visible");
  addProperty(plotStyleStrokeStr, this, "plotBorderSides", "sides");

  addFillProperties(plotStyleFillStr  , "plotFill"  );
  addLineProperties(plotStyleStrokeStr, "plotBorder");

  //---

  QString dataStyleStr       = "dataBackground";
  QString dataStyleFillStr   = dataStyleStr + "/fill";
  QString dataStyleStrokeStr = dataStyleStr + "/stroke";

  addProperty(dataStyleStr      , this, "dataClip"       , "clip");
  addProperty(dataStyleFillStr  , this, "dataFilled"     , "visible");
  addProperty(dataStyleStrokeStr, this, "dataBorder"     , "visible");
  addProperty(dataStyleStrokeStr, this, "dataBorderSides", "sides");

  addFillProperties(dataStyleFillStr  , "dataFill"  );
  addLineProperties(dataStyleStrokeStr, "dataBorder");

  //---

  QString fitStyleStr       = "fitBackground";
  QString fitStyleFillStr   = fitStyleStr + "/fill";
  QString fitStyleStrokeStr = fitStyleStr + "/stroke";

  addProperty(fitStyleFillStr  , this, "fitFilled"     , "visible");
  addProperty(fitStyleStrokeStr, this, "fitBorder"     , "visible");
  addProperty(fitStyleStrokeStr, this, "fitBorderSides", "sides");

  addFillProperties(fitStyleFillStr  , "fitFill"  );
  addLineProperties(fitStyleStrokeStr, "fitBorder");

  //---

  addProperty("margin/inner", this, "innerMarginLeft"  , "left"  );
  addProperty("margin/inner", this, "innerMarginTop"   , "top"   );
  addProperty("margin/inner", this, "innerMarginRight" , "right" );
  addProperty("margin/inner", this, "innerMarginBottom", "bottom");

  addProperty("margin/outer", this, "outerMarginLeft"  , "left"  );
  addProperty("margin/outer", this, "outerMarginTop"   , "top"   );
  addProperty("margin/outer", this, "outerMarginRight" , "right" );
  addProperty("margin/outer", this, "outerMarginBottom", "bottom");

  //---

  addProperty("every", this, "everyEnabled", "enabled");
  addProperty("every", this, "everyStart"  , "start"  );
  addProperty("every", this, "everyEnd"    , "end"    );
  addProperty("every", this, "everyStep"   , "step"   );

  addProperty("filter", this, "filterStr", "expression");

  //---

  if (xAxis()) {
    xAxis()->addProperties(propertyModel(), id() + "/" + "xaxis");

    addProperty("xaxis", this, "xLabel", "userLabel");
  }

  if (yAxis()) {
    yAxis()->addProperties(propertyModel(), id() + "/" + "yaxis");

    addProperty("yaxis", this, "yLabel", "userLabel");
  }

  if (key())
    key()->addProperties(propertyModel(), id() + "/" + "key");

  if (title())
    title()->addProperties(propertyModel(), id() + "/" + "title");

  addProperty("scaledFont", this, "minScaleFontSize", "minSize");
  addProperty("scaledFont", this, "maxScaleFontSize", "maxSize");
}

void
CQChartsPlot::
addSymbolProperties(const QString &path, const QString &prefix)
{
  QString strokePath = path + "/stroke";
  QString fillPath   = path + "/fill";

  QString symbolPrefix = (prefix.length() ? prefix + "Symbol" : "symbol");

  addProperty(path      , this, symbolPrefix + "Type"       , "type"   );
  addProperty(path      , this, symbolPrefix + "Size"       , "size"   );
  addProperty(strokePath, this, symbolPrefix + "Stroked"    , "visible");
  addProperty(strokePath, this, symbolPrefix + "StrokeColor", "color"  );
  addProperty(strokePath, this, symbolPrefix + "StrokeAlpha", "alpha"  );
  addProperty(strokePath, this, symbolPrefix + "StrokeWidth", "width"  );
  addProperty(strokePath, this, symbolPrefix + "StrokeDash" , "dash"   );
  addProperty(fillPath  , this, symbolPrefix + "Filled"     , "visible");
  addProperty(fillPath  , this, symbolPrefix + "FillColor"  , "color"  );
  addProperty(fillPath  , this, symbolPrefix + "FillAlpha"  , "alpha"  );
  addProperty(fillPath  , this, symbolPrefix + "FillPattern", "pattern");
}

void
CQChartsPlot::
addLineProperties(const QString &path, const QString &prefix)
{
  addProperty(path, this, prefix + "Color", "color");
  addProperty(path, this, prefix + "Alpha", "alpha");
  addProperty(path, this, prefix + "Width", "width");
  addProperty(path, this, prefix + "Dash" , "dash" );
}

void
CQChartsPlot::
addFillProperties(const QString &path, const QString &prefix)
{
  addProperty(path, this, prefix + "Color"  , "color"  );
  addProperty(path, this, prefix + "Alpha"  , "alpha"  );
  addProperty(path, this, prefix + "Pattern", "pattern");
}

void
CQChartsPlot::
addTextProperties(const QString &path, const QString &prefix)
{
  addProperty(path, this, prefix + "Font"    , "font"    );
  addProperty(path, this, prefix + "Color"   , "color"   );
  addProperty(path, this, prefix + "Alpha"   , "alpha"   );
  addProperty(path, this, prefix + "Contrast", "contrast");
}

void
CQChartsPlot::
addColorMapProperties()
{
  addProperty("color/map", this, "colorMapped"    , "enabled");
  addProperty("color/map", this, "colorMapMin"    , "min"    );
  addProperty("color/map", this, "colorMapMax"    , "max"    );
  addProperty("color/map", this, "colorMapPalette", "palette");
}

bool
CQChartsPlot::
setProperties(const QString &properties)
{
  bool rc = true;

  QStringList strs = properties.split(",", QString::SkipEmptyParts);

  for (int i = 0; i < strs.size(); ++i) {
    QString str = strs[i].simplified();

    int pos = str.indexOf("=");

    QString name  = str.mid(0, pos).simplified();
    QString value = str.mid(pos + 1).simplified();

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

CQPropertyViewItem *
CQChartsPlot::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  assert(CQUtil::hasProperty(object, name));

  QString path1 = id();

  if (path.length())
    path1 += "/" + path;

  return view_->addProperty(path1, object, name, alias);
}

void
CQChartsPlot::
propertyItemSelected(QObject *obj, const QString &)
{
  view()->startSelection();

  view()->deselectAll();

  bool changed = false;

  if      (obj == this) {
    setSelected(true);

    view()->setCurrentPlot(this);

    invalidateLayers();

    changed = true;
  }
  else if (obj == titleObj_) {
    titleObj_->setSelected(true);

    invalidateLayer(CQChartsBuffer::Type::FOREGROUND);

    changed = true;
  }
  else if (obj == keyObj_) {
    keyObj_->setSelected(true);

    invalidateLayer(CQChartsBuffer::Type::BACKGROUND);
    invalidateLayer(CQChartsBuffer::Type::FOREGROUND);

    changed = true;
  }
  else if (obj == xAxis_) {
    xAxis_->setSelected(true);

    invalidateLayer(CQChartsBuffer::Type::BACKGROUND);
    invalidateLayer(CQChartsBuffer::Type::FOREGROUND);

    changed = true;
  }
  else if (obj == yAxis_) {
    yAxis_->setSelected(true);

    invalidateLayer(CQChartsBuffer::Type::BACKGROUND);
    invalidateLayer(CQChartsBuffer::Type::FOREGROUND);

    changed = true;
  }
  else {
    for (const auto &annotation : annotations()) {
      if (obj == annotation) {
        annotation->setSelected(true);

        invalidateLayer(CQChartsBuffer::Type::FOREGROUND);

        changed = true;
      }
    }
  }

  view()->endSelection();

  //---

  if (changed)
    invalidateLayer(CQChartsBuffer::Type::OVERLAY);
}

void
CQChartsPlot::
getPropertyNames(QStringList &names) const
{
  view()->propertyModel()->objectNames(this, names);
}

void
CQChartsPlot::
getObjectPropertyNames(CQChartsPlotObj *plotObj, QStringList &names) const
{
  names = CQUtil::getPropertyList(plotObj, /*inherited*/ false);
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
  xAxis_ = new CQChartsAxis(this, Qt::Horizontal, 0, 1);

  xAxis_->setObjectName("xaxis");
}

void
CQChartsPlot::
addYAxis()
{
  yAxis_ = new CQChartsAxis(this, Qt::Vertical, 0, 1);

  yAxis_->setObjectName("yaxis");
}

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
  if (isOverlay()) {
    // if first plot then add all chained plot items to this plot's key
    if (prevPlot())
      return;

    //---

    if (key()) {
      key()->clearItems();

      processOverlayPlots([&](CQChartsPlot *plot) {
        plot->addKeyItems(key());
      });
    }
  }
  else {
    if (key()) {
      key()->clearItems();

      addKeyItems(key());
    }
  }
}

void
CQChartsPlot::
addTitle()
{
  titleObj_ = new CQChartsTitle(this);

  titleObj_->setTextStr(titleStr());
}

void
CQChartsPlot::
clearRangeAndObjs()
{
  resetRange();

  clearPlotObjects();
}

void
CQChartsPlot::
updateRangeAndObjs()
{
  if (! isUpdatesEnabled()) {
    updatesData_.updateRangeAndObjs = true;
    return;
  }

  if (updateTimeout_ >= 0)
    updateTimer_->start(updateTimeout_);
  else
    updateRangeAndObjsInternal();
}

void
CQChartsPlot::
updateTimerSlot()
{
  updateRangeAndObjsInternal();
}

void
CQChartsPlot::
updateRangeAndObjsInternal()
{
  CQPerfTrace trace("CQChartsPlot::updateRangeAndObjsInternal");

  updateRange();

  updateObjs();
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
updateRange(bool apply)
{
  CQPerfTrace trace("CQChartsPlot::updateRange");

  calcDataRange_ = calcRange();

  adjustDataRange();

  outerDataRange_ = dataRange_;

  if (apply)
    applyDataRange();
}

void
CQChartsPlot::
updateObjs()
{
  if (! isUpdatesEnabled()) {
    updatesData_.updateObjs = true;
    return;
  }

  //---

  CQPerfTrace trace("CQChartsPlot::updateObjs");

  clearPlotObjects();

  invalidateLayers();

  initColorColumnData();
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
  CQChartsGeom::BBox bbox = getDataRange();

  // if zoom data, adjust bbox by pan offset, zoom scale
  if (view_->isZoomData()) {
    if (adjust)
      bbox = adjustDataRangeBBox(bbox);
  }

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
adjustDataRangeBBox(const CQChartsGeom::BBox &bbox) const
{
  CQChartsGeom::Point c = bbox.getCenter();

  double bw = bbox.getWidth ();
  double bh = bbox.getHeight();

  double w = 0.5*bw/dataScaleX();
  double h = 0.5*bh/dataScaleY();
  double x = c.x + bw*dataOffsetX();
  double y = c.y + bh*dataOffsetY();

  CQChartsGeom::BBox bbox1 = CQChartsGeom::BBox(x - w, y - h, x + w, y + h);

  //---

  CQChartsGeom::BBox ibbox =
    firstPlot()->innerMargin().adjustPlotRange(this, bbox1, /*inside*/true);

  //---

  CQChartsPlot *th = const_cast<CQChartsPlot *>(this);

  th->outerDataRange_ = CQChartsUtil::bboxRange(ibbox);

  return ibbox;
}

CQChartsGeom::BBox
CQChartsPlot::
getDataRange() const
{
  if (dataRange_.isSet())
    return CQChartsUtil::rangeBBox(dataRange_);
  else
    return CQChartsGeom::BBox(0, 0, 1, 1);
}

void
CQChartsPlot::
applyDataRange(bool propagate)
{
  if (! isUpdatesEnabled()) {
    updatesData_.applyDataRange = true;
    return;
  }

  //---

  CQChartsGeom::BBox dataRange;

  if (propagate) {
    if      (isX1X2()) {
      CQChartsPlot *plot1 = firstPlot();

      dataRange = plot1->calcDataRange(/*adjust*/false);
    }
    else if (isY1Y2()) {
      CQChartsPlot *plot1 = firstPlot();

      dataRange = plot1->calcDataRange(/*adjust*/false);
    }
    else if (isOverlay()) {
      processOverlayPlots([&](CQChartsPlot *plot) {
        plot->setDataRange(CQChartsGeom::Range(), /*update*/false);

        plot->updateRange(/*update*/false);

        CQChartsGeom::BBox dataRange1 = plot->calcDataRange(/*adjust*/false);

        dataRange += dataRange1;
      });
    }
    else {
      dataRange = calcDataRange();
    }
  }
  else {
    dataRange = calcDataRange();
  }

  displayRange_->setWindowRange(dataRange.getXMin(), dataRange.getYMin(),
                                dataRange.getXMax(), dataRange.getYMax());

  if (xAxis()) {
    xAxis()->setRange(dataRange.getXMin(), dataRange.getXMax());
    yAxis()->setRange(dataRange.getYMin(), dataRange.getYMax());
  }

  if (propagate) {
    if      (isX1X2()) {
      CQChartsGeom::Range dataRange1 = CQChartsUtil::bboxRange(dataRange);

      CQChartsPlot *plot1, *plot2;

      x1x2Plots(plot1, plot2);

      if (plot1) {
        plot1->setDataScaleX (dataScaleX ());
        plot1->setDataScaleY (dataScaleY ());
        plot1->setDataOffsetX(dataOffsetX());
        plot1->setDataOffsetY(dataOffsetY());

        plot1->applyDataRange(/*propagate*/false);
      }

      //---

      if (plot2) {
        plot2->setDataRange(CQChartsGeom::Range(), /*update*/false);

        plot2->updateRange(/*update*/false);

        CQChartsGeom::BBox bbox2 = plot2->calcDataRange(/*adjust*/false);

        CQChartsGeom::Range dataRange2 =
          CQChartsGeom::Range(bbox2.getXMin(), dataRange1.bottom(),
                              bbox2.getXMax(), dataRange1.top   ());

        plot2->setDataRange(dataRange2, /*update*/false);

        plot2->setDataScaleX (dataScaleX ());
        plot2->setDataScaleY (dataScaleY ());
        plot2->setDataOffsetX(dataOffsetX());
        plot2->setDataOffsetY(dataOffsetY());

        plot2->applyDataRange(/*propagate*/false);
      }
    }
    else if (isY1Y2()) {
      CQChartsGeom::Range dataRange1 = CQChartsUtil::bboxRange(dataRange);

      CQChartsPlot *plot1, *plot2;

      y1y2Plots(plot1, plot2);

      if (plot1) {
        plot1->setDataScaleX (dataScaleX ());
        plot1->setDataScaleY (dataScaleY ());
        plot1->setDataOffsetX(dataOffsetX());
        plot1->setDataOffsetY(dataOffsetY());

        plot1->applyDataRange(/*propagate*/false);
      }

      //---

      if (plot2) {
        plot2->setDataRange(CQChartsGeom::Range(), /*update*/false);

        plot2->updateRange(/*update*/false);

        CQChartsGeom::BBox bbox2 = plot2->calcDataRange(/*adjust*/false);

        CQChartsGeom::Range dataRange2 =
          CQChartsGeom::Range(dataRange1.left (), bbox2.getYMin(),
                              dataRange1.right(), bbox2.getYMax());

        plot2->setDataRange(dataRange2, /*update*/false);

        plot2->setDataScaleX (dataScaleX ());
        plot2->setDataScaleY (dataScaleY ());
        plot2->setDataOffsetX(dataOffsetX());
        plot2->setDataOffsetY(dataOffsetY());

        plot2->applyDataRange(/*propagate*/false);
      }
    }
    else if (isOverlay()) {
      CQChartsGeom::Range dataRange1 = CQChartsUtil::bboxRange(dataRange);

      processOverlayPlots([&](CQChartsPlot *plot) {
        plot->setDataRange(dataRange1, /*update*/false);

        plot->setDataScaleX (dataScaleX ());
        plot->setDataScaleY (dataScaleY ());
        plot->setDataOffsetX(dataOffsetX());
        plot->setDataOffsetY(dataOffsetY());

        plot->applyDataRange(/*propagate*/false);
      });
    }
    else {
      CQChartsPlot *plot1 = firstPlot();

      while (plot1) {
        if (plot1 != this) {
          plot1->setDataScaleX (dataScaleX ());
          plot1->setDataScaleY (dataScaleY ());
          plot1->setDataOffsetX(dataOffsetX());
          plot1->setDataOffsetY(dataOffsetY());

          plot1->applyDataRange(/*propagate*/false);
        }

        plot1 = plot1->nextPlot();
      }
    }
  }

  updateKeyPosition(/*force*/true);

  emit rangeChanged();
}

void
CQChartsPlot::
applyDisplayTransform(bool propagate)
{
  if (propagate) {
    if (isOverlay()) {
      processOverlayPlots([&](CQChartsPlot *plot) {
        plot->setDisplayTransform(*displayTransform_);

        plot->applyDisplayTransform(/*propagate*/false);
      });
    }
  }
}

void
CQChartsPlot::
adjustDataRange()
{
  dataRange_ = calcDataRange_;

  // adjust data range to custom values
  if (xmin()) dataRange_.setLeft  (*xmin());
  if (ymin()) dataRange_.setBottom(*ymin());
  if (xmax()) dataRange_.setRight (*xmax());
  if (ymax()) dataRange_.setTop   (*ymax());

  if (xAxis() && xAxis()->isIncludeZero()) {
    if (dataRange_.isSet())
      dataRange_.updateRange(0, dataRange_.ymid());
  }

  if (yAxis() && yAxis()->isIncludeZero()) {
    if (dataRange_.isSet())
      dataRange_.updateRange(dataRange_.xmid(), 0);
  }
}

void
CQChartsPlot::
addPlotObject(CQChartsPlotObj *obj)
{
  plotObjs_.push_back(obj);
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
      plot->initPlotObjs();
    });
  }
  else {
    initPlotObjs();
  }
}

bool
CQChartsPlot::
initPlotRange()
{
  if (! dataRange_.isSet()) {
    updateRange();

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
    initObjTree();

  //---

  // auto fit
  if (changed && isAutoFit()) {
    needsAutoFit_ = true;
  }

  //---

  if (changed)
    emit plotObjsAdded();
}

bool
CQChartsPlot::
addNoDataObj()
{
  // if no objects than add a no data object
  noData_ = false;

  if (plotObjs_.empty()) {
    CQChartsNoDataObj *obj = new CQChartsNoDataObj(this);

    addPlotObject(obj);

    noData_ = true;
  }

  return noData_;
}

bool
CQChartsPlot::
initObjs()
{
  initPlotRange();

  //---

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
  PlotObjs objs;

  if (! createObjs(objs))
    return false;

  for (auto &obj : objs)
    addPlotObject(obj);

  return true;
}

bool
CQChartsPlot::
createObjs(PlotObjs &)
{
  assert(false);
}

void
CQChartsPlot::
initObjTree()
{
  CQPerfTrace trace("CQChartsPlot::initObjTree");

  if (! isPreview())
    plotObjTree_->addObjects();
}

void
CQChartsPlot::
clearPlotObjects()
{
  PlotObjs plotObjs;

  std::swap(plotObjs, plotObjs_);

  for (auto &plotObj : plotObjs)
    delete plotObj;

  plotObjTree_->clearObjects();

  insideObjs_    .clear();
  sizeInsideObjs_.clear();
}

bool
CQChartsPlot::
updatePlotObjects(const CQChartsGeom::Point &w)
{
  Objs objs;

  objsAtPoint(w, objs);

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

  if (changed) {
    insideInd_ = 0;

    insideObjs_    .clear();
    sizeInsideObjs_.clear();

    for (auto &obj : plotObjects())
      obj->setInside(false);

    for (auto &annotation : annotations()) {
      annotation->setInside(false);
    }

    for (const auto &obj : objs) {
      insideObjs_.insert(obj);

      sizeInsideObjs_[obj->rect().area()].insert(obj);
    }

    setInsideObject();
  }

  return changed;
}

CQChartsObj *
CQChartsPlot::
insideObject() const
{
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
  ++insideInd_;

  if (insideInd_ >= int(insideObjs_.size()))
    insideInd_ = 0;
}

void
CQChartsPlot::
prevInsideInd()
{
  --insideInd_;

  if (insideInd_ < 0)
    insideInd_ = insideObjs_.size() - 1;
}

void
CQChartsPlot::
setInsideObject()
{
  CQChartsObj *insideObj = insideObject();

  for (auto &obj : insideObjs_)
    obj->setInside(obj == insideObj);
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

bool
CQChartsPlot::
selectMousePress(const QPointF &p, SelMod selMod)
{
  CQChartsGeom::Point w;

  pixelToWindow(CQChartsUtil::fromQPoint(QPointF(p)), w);

  return selectPress(w, selMod);
}

bool
CQChartsPlot::
selectPress(const CQChartsGeom::Point &w, SelMod selMod)
{
  if (key() && key()->contains(w)) {
    CQChartsKeyItem *item = key()->getItemAt(w);

    if (item) {
      bool handled = item->selectPress(w, selMod);

      if (handled) {
        emit keyItemPressed  (item);
        emit keyItemIdPressed(item->id());

        return true;
      }
    }

    bool handled = key()->selectPress(w, selMod);

    if (handled) {
      emit keyPressed  (key());
      emit keyIdPressed(key()->id());

      return true;
    }
  }

  //---

  if (title() && title()->contains(w)) {
    if (title()->selectPress(w)) {
      emit titlePressed  (title());
      emit titleIdPressed(title()->id());

      return true;
    }
  }

  //---

  for (const auto &annotation : annotations()) {
    if (annotation->contains(w)) {
      if (annotation->selectPress(w)) {
        selectOneObj(annotation, /*allObjs*/true);

        invalidateLayer(CQChartsBuffer::Type::FOREGROUND);
        invalidateLayer(CQChartsBuffer::Type::OVERLAY);

        emit annotationPressed  (annotation);
        emit annotationIdPressed(annotation->id());

        return true;
      }
    }
  }

  //---

  // for replace init all objects to unselected
  // for add/remove/toggle init all objects to current state
  using ObjsSelected = std::map<CQChartsObj*,bool>;

  ObjsSelected objsSelected;

  for (auto &plotObj : plotObjects()) {
    if (selMod == SelMod::REPLACE)
      objsSelected[plotObj] = false;
    else
      objsSelected[plotObj] = plotObj->isSelected();
  }

  //---

  // get object under mouse
  CQChartsObj *selectObj = nullptr;

  if (isFollowMouse()) {
    selectObj = insideObject();

    nextInsideInd();

    setInsideObject();
  }
  else {
    Objs objs;

    objsAtPoint(w, objs);

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

    CQChartsPlotObj *selectPlotObj = dynamic_cast<CQChartsPlotObj *>(selectObj);

    if (selectPlotObj) {
      emit objPressed  (selectPlotObj);
      emit objIdPressed(selectPlotObj->id());
    }

    // potential crash if signals cause objects to be deleted (defer !!!)
  }

  //---

  // determine if selection changed
  bool changed = false;

  for (const auto &objSelected : objsSelected) {
    if (objSelected.first->isSelected() == objSelected.second)
      continue;

    if (! changed) {
      view()->startSelection();

      changed = true;
    }

    objSelected.first->setSelected(objSelected.second);
  }

  //----

  // update selection if changed
  if (changed) {
    beginSelect();

    for (const auto &objSelected : objsSelected) {
      CQChartsPlotObj *selectPlotObj = dynamic_cast<CQChartsPlotObj *>(objSelected.first);

      if (! selectPlotObj || ! selectPlotObj->isSelected())
        continue;

      selectPlotObj->addSelectIndices();
    }

    endSelect();

    //---

    invalidateLayer(CQChartsBuffer::Type::OVERLAY);

    if (selectInvalidateObjs())
      invalidateLayer(CQChartsBuffer::Type::MIDDLE);

    //---

    view()->endSelection();
  }

  //---

  return selectObj;
}

bool
CQChartsPlot::
selectMouseMove(const QPointF &pos, bool first)
{
  CQChartsGeom::Point p = CQChartsUtil::fromQPoint(pos);
  CQChartsGeom::Point w = pixelToWindow(p);

  return selectMove(w, first);
}

bool
CQChartsPlot::
selectMove(const CQChartsGeom::Point &w, bool first)
{
  if (key()) {
    bool handled = key()->selectMove(w);

    if (handled)
      return true;
  }

  //---

  QString objText;

  if (isFollowMouse()) {
    bool changed = updatePlotObjects(w);

    objText = insideObjectText();

    if (changed)
      invalidateLayer(CQChartsBuffer::Type::OVERLAY);
  }

  //---

  if (first) {
    if (objText != "") {
      view_->setStatusText(objText);

      return true;
    }
  }

  //---

  return false;
}

bool
CQChartsPlot::
selectMouseRelease(const QPointF &p)
{
  CQChartsGeom::Point w;

  pixelToWindow(CQChartsUtil::fromQPoint(QPointF(p)), w);

  return selectRelease(w);
}

bool
CQChartsPlot::
selectRelease(const CQChartsGeom::Point &)
{
  return true;
}

#if 0
void
CQChartsPlot::
selectObjsAtPoint(const CQChartsGeom::Point &w, Objs &objs)
{
  if (key() && key()->contains(w))
    objs.push_back(key());

  if (title() && title()->contains(w))
    objs.push_back(title());

  for (const auto &annotation : annotations()) {
    if (annotation->contains(w))
      objs.push_back(annotation);
  }
}
#endif

//------

bool
CQChartsPlot::
editMousePress(const QPointF &pos)
{
  CQChartsGeom::Point p = CQChartsUtil::fromQPoint(pos);
  CQChartsGeom::Point w = pixelToWindow(p);

  return editPress(p, w);
}

bool
CQChartsPlot::
editPress(const CQChartsGeom::Point &p, const CQChartsGeom::Point &w)
{
  mouseData_.dragObj    = DragObj::NONE;
  mouseData_.pressPoint = CQChartsUtil::toQPoint(p);
  mouseData_.movePoint  = mouseData_.pressPoint;

  //---

  // start drag on already selected plot handle
  if (isSelected()) {
    CQChartsGeom::Point v = windowToView(w);

    // to edit must be in handle
    mouseData_.dragSide = editHandles_.inside(v);

    if (mouseData_.dragSide != CQChartsResizeHandle::Side::NONE) {
      mouseData_.dragObj = DragObj::PLOT_HANDLE;

      editHandles_.setDragSide(mouseData_.dragSide);
      editHandles_.setDragPos (w);

      invalidateLayer(CQChartsBuffer::Type::OVERLAY);

      return true;
    }
  }

  // start drag on already selected key handle
  if (key() && key()->isSelected()) {
    mouseData_.dragSide = key()->editHandles().inside(w);

    if (mouseData_.dragSide != CQChartsResizeHandle::Side::NONE) {
      mouseData_.dragObj = DragObj::KEY;

      key()->editPress(w);

      key()->editHandles().setDragSide(mouseData_.dragSide);
      key()->editHandles().setDragPos (w);

      invalidateLayer(CQChartsBuffer::Type::OVERLAY);

      return true;
    }
  }

  // start drag on already selected x axis handle
  if (xAxis() && xAxis()->isSelected()) {
    mouseData_.dragSide = xAxis()->editHandles().inside(w);

    if (mouseData_.dragSide != CQChartsResizeHandle::Side::NONE) {
      mouseData_.dragObj = DragObj::XAXIS;

      xAxis()->editPress(w);

      xAxis()->editHandles().setDragSide(mouseData_.dragSide);
      xAxis()->editHandles().setDragPos (w);

      invalidateLayer(CQChartsBuffer::Type::OVERLAY);

      return true;
    }
  }

  // start drag on already selected y axis handle
  if (yAxis() && yAxis()->isSelected()) {
    mouseData_.dragSide = yAxis()->editHandles().inside(w);

    if (mouseData_.dragSide != CQChartsResizeHandle::Side::NONE) {
      mouseData_.dragObj = DragObj::YAXIS;

      yAxis()->editPress(w);

      yAxis()->editHandles().setDragSide(mouseData_.dragSide);
      yAxis()->editHandles().setDragPos (w);

      invalidateLayer(CQChartsBuffer::Type::OVERLAY);

      return true;
    }
  }

  // start drag on already selected title handle
  if (title() && title()->isSelected()) {
    mouseData_.dragSide = title()->editHandles().inside(w);

    if (mouseData_.dragSide != CQChartsResizeHandle::Side::NONE) {
      mouseData_.dragObj = DragObj::TITLE;

      title()->editPress(w);

      title()->editHandles().setDragSide(mouseData_.dragSide);
      title()->editHandles().setDragPos (w);

      invalidateLayer(CQChartsBuffer::Type::OVERLAY);

      return true;
    }
  }

  // start drag on already selected annotation handle
  for (const auto &annotation : annotations()) {
    if (annotation->isSelected()) {
      mouseData_.dragSide = annotation->editHandles().inside(w);

      if (mouseData_.dragSide != CQChartsResizeHandle::Side::NONE) {
        mouseData_.dragObj = DragObj::ANNOTATION;

        annotation->editHandles().setDragSide(mouseData_.dragSide);
        annotation->editHandles().setDragPos (w);

        invalidateLayer(CQChartsBuffer::Type::OVERLAY);

        return true;
      }
    }
  }

  //---

  // select/deselect key
  if (key()) {
    if (key()->contains(w)) {
      if (! key()->isSelected()) {
        selectOneObj(key(), /*allObjs*/false);

        invalidateLayer(CQChartsBuffer::Type::OVERLAY);

        return true;
      }

      if (key()->editPress(w)) {
        mouseData_.dragObj = DragObj::KEY;

        invalidateLayer(CQChartsBuffer::Type::OVERLAY);

        return true;
      }

      return false;
    }
  }

  //---

  // select/deselect x axis
  if (xAxis()) {
    if (xAxis()->contains(w)) {
      if (! xAxis()->isSelected()) {
        selectOneObj(xAxis(), /*allObjs*/false);

        invalidateLayer(CQChartsBuffer::Type::OVERLAY);

        return true;
      }

      if (xAxis()->editPress(w)) {
        mouseData_.dragObj = DragObj::XAXIS;

        invalidateLayer(CQChartsBuffer::Type::OVERLAY);

        return true;
      }

      return false;
    }
  }

  //---

  // select/deselect y axis
  if (yAxis()) {
    if (yAxis()->contains(w)) {
      if (! yAxis()->isSelected()) {
        selectOneObj(yAxis(), /*allObjs*/false);

        invalidateLayer(CQChartsBuffer::Type::OVERLAY);

        return true;
      }

      if (yAxis()->editPress(w)) {
        mouseData_.dragObj = DragObj::YAXIS;

        invalidateLayer(CQChartsBuffer::Type::OVERLAY);

        return true;
      }

      return false;
    }
  }

  //---

  // select/deselect title
  if (title()) {
    if (title()->contains(w)) {
      if (! title()->isSelected()) {
        selectOneObj(title(), /*allObjs*/false);

        invalidateLayer(CQChartsBuffer::Type::OVERLAY);

        return true;
      }

      if (title()->editPress(w)) {
        mouseData_.dragObj = DragObj::TITLE;

        invalidateLayer(CQChartsBuffer::Type::OVERLAY);

        return true;
      }

      return false;
    }
  }

  //---

  for (const auto &annotation : annotations()) {
    if (annotation->contains(w)) {
      if (! annotation->isSelected()) {
        selectOneObj(annotation, /*allObjs*/false);

        invalidateLayer(CQChartsBuffer::Type::OVERLAY);

        return true;
      }

      if (annotation->editPress(w)) {
        mouseData_.dragObj = DragObj::ANNOTATION;

        invalidateLayer(CQChartsBuffer::Type::OVERLAY);

        return true;
      }

      return false;
    }
  }

  //---

  // select/deselect plot
  // (to select point must be inside a plot object)
  Objs objs;

  objsAtPoint(w, objs);

  if (! objs.empty()) {
    if (! isSelected()) {
      view()->startSelection();

      view()->deselectAll();

      setSelected(true);

      view()->endSelection();

      //---

      view()->setCurrentPlot(this);

      invalidateLayer(CQChartsBuffer::Type::OVERLAY);

      return true;
    }

    mouseData_.dragObj = DragObj::PLOT;

    invalidateLayer(CQChartsBuffer::Type::OVERLAY);

    return true;
  }

  //---

  view()->deselectAll();

  return false;
}

void
CQChartsPlot::
selectOneObj(CQChartsObj *obj, bool allObjs)
{
  view()->startSelection();

  if (allObjs)
    deselectAllObjs();

  view()->deselectAll();

  obj->setSelected(true);

  view()->endSelection();
}

void
CQChartsPlot::
deselectAllObjs()
{
  view()->startSelection();

  for (auto &plotObj : plotObjects())
    plotObj->setSelected(false);

  view()->endSelection();
}

void
CQChartsPlot::
deselectAll()
{
  bool changed = false;

  auto updateChanged = [&] {
    if (! changed) {
      view()->startSelection();

      changed = true;
    }
  };

  if (key() && key()->isSelected()) {
    key()->setSelected(false);

    updateChanged();
  }

  if (xAxis() && xAxis()->isSelected()) {
    xAxis()->setSelected(false);

    updateChanged();
  }

  if (yAxis() && yAxis()->isSelected()) {
    yAxis()->setSelected(false);

    updateChanged();
  }

  if (title() && title()->isSelected()) {
    title()->setSelected(false);

    updateChanged();
  }

  for (auto &annotation : annotations()) {
    if (annotation->isSelected()) {
      annotation->setSelected(false);

      updateChanged();
    }
  }

  if (isSelected()) {
    setSelected(false);

    updateChanged();
  }

  if (changed) {
    view()->endSelection();

    invalidateLayer(CQChartsBuffer::Type::OVERLAY);
  }
}

bool
CQChartsPlot::
editMouseMove(const QPointF &pos, bool first)
{
  CQChartsGeom::Point p = CQChartsUtil::fromQPoint(pos);
  CQChartsGeom::Point w = pixelToWindow(p);

  return editMove(p, w, first);
}

bool
CQChartsPlot::
editMove(const CQChartsGeom::Point &p, const CQChartsGeom::Point &w, bool /*first*/)
{
  QPointF lastMovePoint = mouseData_.movePoint;

  mouseData_.movePoint = CQChartsUtil::toQPoint(p);

  if (mouseData_.dragObj == DragObj::NONE)
    return false;

  if      (mouseData_.dragObj == DragObj::KEY) {
    (void) key()->editMove(w);
  }
  else if (mouseData_.dragObj == DragObj::XAXIS) {
    (void) xAxis()->editMove(w);
  }
  else if (mouseData_.dragObj == DragObj::YAXIS) {
    (void) yAxis()->editMove(w);
  }
  else if (mouseData_.dragObj == DragObj::TITLE) {
    (void) title()->editMove(w);
  }
  else if (mouseData_.dragObj == DragObj::ANNOTATION) {
    bool edited = false;

    for (const auto &annotation : annotations()) {
      if (annotation->isSelected()) {
        (void) annotation->editMove(w);

        edited = true;
      }
    }

    if (! edited)
      return false;
  }
  else if (mouseData_.dragObj == DragObj::PLOT) {
    double dx = mouseData_.movePoint.x() - lastMovePoint.x();
    double dy = lastMovePoint.y() - mouseData_.movePoint.y();

    double dx1 =  view_->pixelToSignedWindowWidth (dx);
    double dy1 = -view_->pixelToSignedWindowHeight(dy);

    viewBBox_.moveBy(CQChartsGeom::Point(dx1, dy1));

    if (mouseData_.dragSide == CQChartsResizeHandle::Side::MOVE)
      updateMargins(false);
    else
      updateMargins();
  }
  else if (mouseData_.dragObj == DragObj::PLOT_HANDLE) {
    double dx = mouseData_.movePoint.x() - lastMovePoint.x();
    double dy = lastMovePoint.y() - mouseData_.movePoint.y();

    double dx1 =  view_->pixelToSignedWindowWidth (dx);
    double dy1 = -view_->pixelToSignedWindowHeight(dy);

    editHandles_.updateBBox(dx1, dy1);

    viewBBox_ = editHandles_.bbox();

    if (mouseData_.dragSide == CQChartsResizeHandle::Side::MOVE)
      updateMargins(false);
    else
      updateMargins();
  }
  else {
    return false;
  }

  invalidateLayer(CQChartsBuffer::Type::OVERLAY);

  return true;
}

bool
CQChartsPlot::
editMouseMotion(const QPointF &pos)
{
  CQChartsGeom::Point p = CQChartsUtil::fromQPoint(pos);
  CQChartsGeom::Point w = pixelToWindow(p);

  return editMotion(p, w);
}

bool
CQChartsPlot::
editMotion(const CQChartsGeom::Point &, const CQChartsGeom::Point &w)
{
  if      (isSelected()) {
    CQChartsGeom::Point v = windowToView(w);

    if (! editHandles_.selectInside(v))
      return true;
  }
  else if (key() && key()->isSelected()) {
    if (! key()->editMotion(w))
      return true;
  }
  else if (xAxis() && xAxis()->isSelected()) {
    if (! xAxis()->editMotion(w))
      return true;
  }
  else if (yAxis() && yAxis()->isSelected()) {
    if (! yAxis()->editMotion(w))
      return true;
  }
  else if (title() && title()->isSelected()) {
    if (! title()->editMotion(w))
      return true;
  }
  else {
    bool edited = false;

    for (const auto &annotation : annotations()) {
      if (annotation->isSelected()) {
        if (annotation->editMotion(w)) {
          edited = true;
          break;
        }
      }
    }

    if (! edited)
      return true;
  }

  invalidateLayer(CQChartsBuffer::Type::OVERLAY);

  return true;
}

bool
CQChartsPlot::
editMouseRelease(const QPointF &pos)
{
  CQChartsGeom::Point p = CQChartsUtil::fromQPoint(pos);
  CQChartsGeom::Point w = pixelToWindow(p);

  return editRelease(p, w);
}

bool
CQChartsPlot::
editRelease(const CQChartsGeom::Point & /*p*/, const CQChartsGeom::Point & /*w*/)
{
  mouseData_.dragObj = DragObj::NONE;

  return true;
}

void
CQChartsPlot::
editMoveBy(const QPointF &d)
{
  QRectF r = calcDataRect();

  double dw = d.x()*r.width ();
  double dh = d.y()*r.height();

  QPointF dp(dw, dh);

  if      (isSelected()) {
  }
  else if (key() && key()->isSelected()) {
    key()->editMoveBy(dp);
  }
  else if (xAxis() && xAxis()->isSelected()) {
    xAxis()->editMoveBy(dp);
  }
  else if (yAxis() && yAxis()->isSelected()) {
    yAxis()->editMoveBy(dp);
  }
  else if (title() && title()->isSelected()) {
    title()->editMoveBy(dp);
  }
  else {
    for (const auto &annotation : annotations()) {
      if (annotation->isSelected()) {
        (void) annotation->editMoveBy(dp);
        break;
      }
    }
  }

  invalidateLayer(CQChartsBuffer::Type::OVERLAY);
}

//------

void
CQChartsPlot::
editObjs(Objs &objs)
{
  if (key() && key()->isVisible())
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
rectSelect(const CQChartsGeom::BBox &r, SelMod selMod)
{
  // for replace init all objects to unselected
  // for add/remove/toggle init all objects to current state
  using ObjsSelected = std::map<CQChartsObj*,bool>;

  ObjsSelected objsSelected;

  for (auto &plotObj : plotObjects()) {
    if (selMod == SelMod::REPLACE)
      objsSelected[plotObj] = false;
    else
      objsSelected[plotObj] = plotObj->isSelected();
  }

  //---

  // get objects touching rectangle
  Objs objs;

  objsTouchingRect(r, objs);

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

  // determine if selection changed
  bool changed = false;

  for (const auto &objSelected : objsSelected) {
    if (objSelected.first->isSelected() == objSelected.second)
      continue;

    objSelected.first->setSelected(objSelected.second);

    changed = true;
  }

  //----

  // update selection if changed
  if (changed) {
    beginSelect();

    for (const auto &objSelected : objsSelected) {
      CQChartsPlotObj *selectPlotObj = dynamic_cast<CQChartsPlotObj *>(objSelected.first);

      if (! selectPlotObj || ! selectPlotObj->isSelected())
        continue;

      selectPlotObj->addSelectIndices();
    }

    endSelect();

    //---

    invalidateLayer(CQChartsBuffer::Type::OVERLAY);

    if (selectInvalidateObjs())
      invalidateLayer(CQChartsBuffer::Type::MIDDLE);
  }

  //---

  return ! objs.empty();
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

//------

void
CQChartsPlot::
setXValueColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(xValueColumn_, c, [&]() {
    if (xAxis())
      xAxis()->setColumn(xValueColumn_);
  } );
}

void
CQChartsPlot::
setYValueColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(yValueColumn_, c, [&]() {
    if (yAxis())
      yAxis()->setColumn(yValueColumn_);
  } );
}

//------

void
CQChartsPlot::
setIdColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(idColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPlot::
setTipColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(tipColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPlot::
setVisibleColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(visibleColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPlot::
setImageColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(imageColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

const CQChartsColumn &
CQChartsPlot::
colorColumn() const
{
  return colorColumnData_.column;
}

void
CQChartsPlot::
setColorColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(colorColumnData_.column, c, [&]() { updateObjs(); } );
}

bool
CQChartsPlot::
isColorMapped() const
{
  return colorColumnData_.mapped;
}

void
CQChartsPlot::
setColorMapped(bool b)
{
  CQChartsUtil::testAndSet(colorColumnData_.mapped, b, [&]() { updateObjs(); } );
}

double
CQChartsPlot::
colorMapMin() const
{
  return colorColumnData_.map_min;
}

void
CQChartsPlot::
setColorMapMin(double r)
{
  CQChartsUtil::testAndSet(colorColumnData_.map_min, r, [&]() { updateObjs(); } );
}

double
CQChartsPlot::
colorMapMax() const
{
  return colorColumnData_.map_max;
}

void
CQChartsPlot::
setColorMapMax(double r)
{
  CQChartsUtil::testAndSet(colorColumnData_.map_max, r, [&]() { updateObjs(); } );
}

const QString &
CQChartsPlot::
colorMapPalette() const
{
  return colorColumnData_.palette;
}

void
CQChartsPlot::
setColorMapPalette(const QString &s)
{
  CQChartsUtil::testAndSet(colorColumnData_.palette, s, [&]() { updateObjs(); } );
}

void
CQChartsPlot::
initColorColumnData()
{
  colorColumnData_.valid = false;

  if (! colorColumn().isValid())
    return;

  CQChartsModelColumnDetails *columnDetails = this->columnDetails(colorColumn());
  if (! columnDetails) return;

  if (colorColumn().isGroup()) {
    colorColumnData_.data_min = 0.0;
    colorColumnData_.data_max = std::max(numGroups() - 1, 0);
  }
  else {
    if (colorColumnData_.mapped) {
      QVariant minVar = columnDetails->minValue();
      QVariant maxVar = columnDetails->maxValue();

      bool ok;

      colorColumnData_.data_min = CQChartsVariant::toReal(minVar, ok);
      if (! ok) colorColumnData_.data_min = 0.0;

      colorColumnData_.data_max = CQChartsVariant::toReal(maxVar, ok);
      if (! ok) colorColumnData_.data_max = 1.0;
    }

    CQBaseModelType    columnType;
    CQBaseModelType    columnBaseType;
    CQChartsNameValues nameValues;

    (void) CQChartsModelUtil::columnValueType(charts(), model().data(), colorColumn(),
                                              columnType, columnBaseType, nameValues);

    if (columnType == CQBaseModelType::COLOR) {
      CQChartsColumnTypeMgr *columnTypeMgr = charts()->columnTypeMgr();

      CQChartsColumnColorType *colorType =
        dynamic_cast<CQChartsColumnColorType *>(columnTypeMgr->getType(columnType));
      assert(colorType);

      colorType->getMapData(charts(), model().data(), colorColumn(), nameValues,
                            colorColumnData_.mapped,
                            colorColumnData_.data_min, colorColumnData_.data_max,
                            colorColumnData_.palette);
    }
  }

  colorColumnData_.valid = true;
}

bool
CQChartsPlot::
columnColor(int row, const QModelIndex &parent, CQChartsColor &color) const
{
  if (! colorColumnData_.valid)
    return false;

  // get mode edit value
  bool ok;

  QVariant var = modelValue(row, colorColumn(), parent, ok);
  if (! ok || ! var.isValid()) return false;

  if (colorColumnData_.mapped) {
    if (CQChartsVariant::isNumeric(var)) {
      double r = CQChartsVariant::toReal(var, ok);
      if (! ok) return false;

      double r1 =
        CMathUtil::map(r, colorColumnData_.data_min, colorColumnData_.data_max, 0.0, 1.0);

      if (r1 < 0.0 || r1 > 1.0)
        return false;

      if (colorColumnData_.palette != "")
        color = CQChartsThemeMgrInst->getNamedPalette(colorColumnData_.palette)->getColor(r1);
      else
        color = CQChartsColor(CQChartsColor::Type::PALETTE_VALUE, r1);
    }
    else {
      if (CQChartsVariant::isColor(var)) {
        color = CQChartsVariant::toColor(var, ok);
      }
      else {
        CQChartsModelColumnDetails *columnDetails = this->columnDetails(colorColumn());
        if (! columnDetails) return false;

        // use unique index/count of edit values (which may have been converted)
        // not same as CQChartsColumnColorType::userData
        int n = columnDetails->numUnique();
        int i = columnDetails->valueInd(var);

        double r = (n > 1 ? double(i)/(n - 1) : 0.0);

        if (colorColumnData_.palette != "")
          color = CQChartsThemeMgrInst->getNamedPalette(colorColumnData_.palette)->getColor(r);
        else
          color = CQChartsColor(CQChartsColor::Type::PALETTE_VALUE, r);
      }
    }
  }
  else {
    if      (CQChartsVariant::isColor(var)) {
      color = CQChartsVariant::toColor(var, ok);
    }
    else {
      color = CQChartsColor(var.toString());
    }
  }

  return color.isValid();
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
posStr(const CQChartsGeom::Point &w) const
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
    return CQChartsUtil::toString(x);
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
    return CQChartsUtil::toString(y);
}

QString
CQChartsPlot::
columnStr(const CQChartsColumn &column, double x) const
{
  if (! column.isValid())
    return CQChartsUtil::toString(x);

  QAbstractItemModel *model = this->model().data();

  if (! model)
    return CQChartsUtil::toString(x);

  QString str;

  if (! CQChartsModelUtil::formatColumnValue(charts(), model, column, x, str))
    return CQChartsUtil::toString(x);

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
    if (view()->mode() != CQChartsView::Mode::EDIT) {
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
        editMoveBy(QPointF( getMoveX(is_shift), 0));
      else if (key == Qt::Key_Left)
        editMoveBy(QPointF(-getMoveX(is_shift), 0));
      else if (key == Qt::Key_Up)
        editMoveBy(QPointF(0, getMoveY(is_shift)));
      else if (key == Qt::Key_Down)
        editMoveBy(QPointF(0, -getMoveY(is_shift)));
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
  else
    return;
}

double
CQChartsPlot::
getPanX(bool is_shift) const
{
  return (! is_shift ? 0.125 : 0.25);
}

double
CQChartsPlot::
getPanY(bool is_shift) const
{
  return (! is_shift ? 0.125 : 0.25);
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
  invalidateLayers();
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

    QString objText = insideObjectText();

    view_->setStatusText(objText);

    invalidateLayer(CQChartsBuffer::Type::OVERLAY);
  }
}

void
CQChartsPlot::
panLeft(double f)
{
  if (! allowPanX())
    return;

  if (view_->isZoomData()) {
    double dx = viewToWindowWidth(f)/getDataRange().getWidth();

    setDataOffsetX(dataOffsetX() - dx);

    applyDataRange();

    invalidateLayers();
  }
  else {
    displayTransform_->panLeft();

    updateTransform();
  }

  emit zoomPanChanged();
}

void
CQChartsPlot::
panRight(double f)
{
  if (! allowPanX())
    return;

  if (view_->isZoomData()) {
    double dx = viewToWindowWidth(f)/getDataRange().getWidth();

    setDataOffsetX(dataOffsetX() + dx);

    applyDataRange();

    invalidateLayers();
  }
  else {
    displayTransform_->panRight();

    updateTransform();
  }

  emit zoomPanChanged();
}

void
CQChartsPlot::
panUp(double f)
{
  if (! allowPanY())
    return;

  if (view_->isZoomData()) {
    double dy = viewToWindowHeight(f)/getDataRange().getHeight();

    setDataOffsetY(dataOffsetY() + dy);

    applyDataRange();

    invalidateLayers();
  }
  else {
    displayTransform_->panUp();

    updateTransform();
  }

  emit zoomPanChanged();
}

void
CQChartsPlot::
panDown(double f)
{
  if (! allowPanY())
    return;

  if (view_->isZoomData()) {
    double dy = viewToWindowHeight(f)/getDataRange().getHeight();

    setDataOffsetY(dataOffsetY() - dy);

    applyDataRange();

    invalidateLayers();
  }
  else {
    displayTransform_->panDown();

    updateTransform();
  }

  emit zoomPanChanged();
}

void
CQChartsPlot::
pan(double dx, double dy)
{
  if (view_->isZoomData()) {
    if (allowPanX())
      setDataOffsetX(dataOffsetX() + dx/getDataRange().getWidth());

    if (allowPanY())
      setDataOffsetY(dataOffsetY() + dy/getDataRange().getHeight());

    applyDataRange();

    invalidateLayers();
  }
  else {
    // TODO

    //displayTransform_->pan(dx, dy);

    //updateTransform();
  }

  emit zoomPanChanged();
}

void
CQChartsPlot::
zoomIn(double f)
{
  if (view_->isZoomData()) {
    if (allowZoomX())
      setDataScaleX(dataScaleX()*f);

    if (allowZoomY())
      setDataScaleY(dataScaleY()*f);

    applyDataRange();

    invalidateLayers();
  }
  else {
    displayTransform_->zoomIn(f);

    updateTransform();
  }

  emit zoomPanChanged();
}

void
CQChartsPlot::
zoomOut(double f)
{
  if (view_->isZoomData()) {
    if (allowZoomX())
      setDataScaleX(dataScaleX()/f);

    if (allowZoomY())
      setDataScaleY(dataScaleY()/f);

    applyDataRange();

    invalidateLayers();
  }
  else {
    displayTransform_->zoomOut(f);

    updateTransform();
  }

  emit zoomPanChanged();
}

void
CQChartsPlot::
zoomTo(const CQChartsGeom::BBox &bbox)
{
  CQChartsGeom::BBox bbox1 = bbox;

  double w = bbox1.getWidth ();
  double h = bbox1.getHeight();

  if (w < 1E-50 || h < 1E-50) {
    double dataScale = 2*std::min(dataScaleX(), dataScaleY());

    w = dataRange_.xsize()/dataScale;
    h = dataRange_.ysize()/dataScale;
  }

  if (view_->isZoomData()) {
    if (! dataRange_.isSet())
      return;

    CQChartsGeom::Point c = bbox.getCenter();

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

    CQChartsGeom::Point c1 = CQChartsGeom::Point(dataRange_.xmid(), dataRange_.ymid());

    double cx = (allowPanX() ? c.x - c1.x : 0.0)/getDataRange().getWidth ();
    double cy = (allowPanY() ? c.y - c1.y : 0.0)/getDataRange().getHeight();

    setDataOffsetX(cx);
    setDataOffsetY(cy);

    applyDataRange();

    invalidateLayers();
  }
  else {
    displayTransform_->zoomTo(bbox);

    updateTransform();
  }

  emit zoomPanChanged();
}

void
CQChartsPlot::
zoomFull(bool notify)
{
  if (view_->isZoomData()) {
    if (allowZoomX())
      setDataScaleX(1.0);

    if (allowZoomY())
      setDataScaleY(1.0);

    setDataOffsetX(0.0);
    setDataOffsetY(0.0);

    applyDataRange();

    invalidateLayers();
  }
  else {
    displayTransform_->reset();

    updateTransform();
  }

  if (notify)
    emit zoomPanChanged();
}

void
CQChartsPlot::
updateTransform()
{
  applyDisplayTransform();

  postResize();

  invalidateLayers();
}

bool
CQChartsPlot::
tipText(const CQChartsGeom::Point &p, QString &tip) const
{
  if (isOverlay() && ! isFirstPlot())
    return false;

  //---

  int objNum  = 0;
  int numObjs = 0;

  CQChartsObj *tipObj = nullptr;

  if (isFollowMouse()) {
    objNum  = insideInd_;
    numObjs = insideObjs_.size();

    tipObj = insideObject();
  }
  else {
    Objs objs;

    objsAtPoint(p, objs);

    numObjs = objs.size();

    if (numObjs)
      tipObj = *objs.begin();
  }

  if (tipObj) {
    if (tip != "")
      tip += " ";

    tip += tipObj->tipId();

    if (numObjs > 1)
      tip += QString("<br><font color=\"blue\">&nbsp;&nbsp;%1 of %2</font>").
               arg(objNum + 1).arg(numObjs);
  }

  return tip.length();
}

void
CQChartsPlot::
objsAtPoint(const CQChartsGeom::Point &p, Objs &objs) const
{
  PlotObjs plotObjs;

  plotObjsAtPoint(p, plotObjs);

  for (const auto &plotObj : plotObjs)
    objs.push_back(plotObj);

  //---

  for (const auto &annotation : annotations()) {
    if (annotation->contains(p))
      objs.push_back(annotation);
  }
}

void
CQChartsPlot::
plotObjsAtPoint(const CQChartsGeom::Point &p, PlotObjs &plotObjs) const
{
  if (isOverlay()) {
    processOverlayPlots([&](const CQChartsPlot *plot) {
      plot->plotObjTree_->objectsAtPoint(p, plotObjs);
    });
  }
  else {
    plotObjTree_->objectsAtPoint(p, plotObjs);
  }
}

void
CQChartsPlot::
objsTouchingRect(const CQChartsGeom::BBox &r, Objs &objs) const
{
  if (isOverlay()) {
    processOverlayPlots([&](const CQChartsPlot *plot) {
      PlotObjs plotObjs;

      plot->plotObjTree_->objectsTouchingRect(r, plotObjs);

      for (const auto &plotObj : plotObjs)
        objs.push_back(plotObj);
    });
  }
  else {
    PlotObjs plotObjs;

    plotObjTree_->objectsTouchingRect(r, plotObjs);

    for (const auto &plotObj : plotObjs)
      objs.push_back(plotObj);
  }
}

void
CQChartsPlot::
postResize()
{
  applyDataRange();

  if (isEqualScale())
    dataRange_.reset();

  for (auto &obj : plotObjects())
    obj->postResize();

  updateKeyPosition(/*force*/true);

  if (isAutoFit())
    needsAutoFit_ = true;
}

void
CQChartsPlot::
updateKeyPosition(bool force)
{
  if (! key() || ! key()->isVisible())
    return;

  if (isOverlay() && ! isFirstPlot())
    return;

  if (force)
    key()->invalidateLayout();

  CQChartsGeom::BBox bbox = calcDataRange();

  if (! bbox.isSet())
    return;

  key()->updateLocation(bbox);
}

//------

bool
CQChartsPlot::
printLayer(CQChartsLayer::Type type, const QString &filename) const
{
  CQChartsLayer *layer = getLayer(type);

  const CQChartsBuffer *buffer = getBuffer(layer->buffer());

  if (! buffer->image())
    return false;

  buffer->image()->save(filename);

  return true;
}

void
CQChartsPlot::
draw(QPainter *painter)
{
  CQPerfTrace trace("CQChartsPlot::draw");

  initGroupedPlotObjs();

  //---

  drawParts(painter);

  //---

  emit layersChanged();
}

void
CQChartsPlot::
drawLayer(QPainter *painter, CQChartsLayer::Type type)
{
  CQChartsLayer *layer = getLayer(type);

  const CQChartsBuffer *buffer = getBuffer(layer->buffer());

  if (buffer->image())
    painter->drawImage(0, 0, *buffer->image());
}

void
CQChartsPlot::
drawParts(QPainter *painter)
{
  CQPerfTrace trace("CQChartsPlot::drawParts");

  drawBackgroundParts(painter);

  //---

  drawMiddleParts(painter);

  //---

  drawForegroundParts(painter);

  //---

  drawOverlayParts(painter);

  //---

  // auto fit based on last draw
  if (needsAutoFit_) {
    needsAutoFit_ = false;

    autoFit();
  }
}

void
CQChartsPlot::
drawBackgroundParts(QPainter *painter)
{
  CQPerfTrace trace("CQChartsPlot::drawBackgroundParts");

  bool bgLayer = hasBackgroundLayer();
  bool bgAxes  = hasBgAxes();
  bool bgKey   = hasBgKey();

  if (! bgLayer && ! bgAxes && ! bgKey)
    return;

  //---

  CQChartsBuffer *buffer = getBuffer(CQChartsBuffer::Type::BACKGROUND);
  if (! buffer->isActive()) return;

  QPainter *painter1 = beginPaint(buffer, painter);

  //---

  if (painter1) {
    // draw background (plot/data fill)
    if (bgLayer)
      drawBackgroundLayer(painter1);

    //---

    // draw axes/key below plot
    if (bgAxes)
      drawBgAxes(painter1);

    if (bgKey)
      drawBgKey(painter1);
  }

  //---

  endPaint(buffer);
}

void
CQChartsPlot::
drawMiddleParts(QPainter *painter)
{
  CQPerfTrace trace("CQChartsPlot::drawMiddleParts");

  bool bg  = hasGroupedObjs(CQChartsLayer::Type::BG_PLOT );
  bool mid = hasGroupedObjs(CQChartsLayer::Type::MID_PLOT);
  bool fg  = hasGroupedObjs(CQChartsLayer::Type::FG_PLOT );

  if (! bg && ! mid && ! fg)
    return;

  //---

  CQChartsBuffer *buffer = getBuffer(CQChartsBuffer::Type::MIDDLE);
  if (! buffer->isActive()) return;

  QPainter *painter1 = beginPaint(buffer, painter);

  //---

  if (painter1) {
    // draw objects (background, mid, foreground)
    drawGroupedObjs(painter1, CQChartsLayer::Type::BG_PLOT );
    drawGroupedObjs(painter1, CQChartsLayer::Type::MID_PLOT);
    drawGroupedObjs(painter1, CQChartsLayer::Type::FG_PLOT );
  }

  //---

  endPaint(buffer);
}

void
CQChartsPlot::
drawForegroundParts(QPainter *painter)
{
  CQPerfTrace trace("CQChartsPlot::drawForegroundParts");

  bool fgAxes      = hasFgAxes();
  bool fgKey       = hasFgKey();
  bool title       = hasTitle();
  bool annotations = hasGroupedAnnotations(CQChartsLayer::Type::ANNOTATION);
  bool foreground  = hasForeground();

  if (! fgAxes && ! fgKey && ! title && ! annotations && ! foreground)
    return;

  //---

  CQChartsBuffer *buffer = getBuffer(CQChartsBuffer::Type::FOREGROUND);
  if (! buffer->isActive()) return;

  QPainter *painter1 = beginPaint(buffer, painter);

  //---

  if (painter1) {
    // draw axes/key above plot
    if (fgAxes)
      drawFgAxes(painter1);

    if (fgKey)
      drawFgKey(painter1);

    //---

    // draw title
    if (title)
      drawTitle(painter1);

    //---

    // draw annotations
    if (annotations)
      drawGroupedAnnotations(painter1, CQChartsLayer::Type::ANNOTATION);

    //---

    // draw foreground
    if (foreground)
      drawForeground(painter1);
  }

  //---

  endPaint(buffer);
}

void
CQChartsPlot::
drawOverlayParts(QPainter *painter)
{
  CQPerfTrace trace("CQChartsPlot::drawOverlayParts");

  bool sel_objs         = hasGroupedObjs(CQChartsLayer::Type::SELECTION);
  bool sel_annotations  = hasGroupedAnnotations(CQChartsLayer::Type::SELECTION);
  bool boxes            = hasGroupedBoxes();
  bool edit_handles     = hasGroupedEditHandles();
  bool over_objs        = hasGroupedObjs(CQChartsLayer::Type::MOUSE_OVER);
  bool over_annotations = hasGroupedAnnotations(CQChartsLayer::Type::MOUSE_OVER);

  if (! sel_objs && ! sel_annotations && ! boxes &&
      ! edit_handles && ! over_objs && ! over_annotations)
    return;

  //---

  CQChartsBuffer *buffer = getBuffer(CQChartsBuffer::Type::OVERLAY);
  if (! buffer->isActive()) return;

  QPainter *painter1 = beginPaint(buffer, painter);

  //---

  if (painter1) {
    // draw selection
    if (sel_objs)
      drawGroupedObjs(painter1, CQChartsLayer::Type::SELECTION);

    if (sel_annotations)
      drawGroupedAnnotations(painter1, CQChartsLayer::Type::SELECTION);

    //---

    // draw debug boxes
    if (boxes)
      drawGroupedBoxes(painter1);

    //---

    if (edit_handles)
      drawGroupedEditHandles(painter1);

    //---

    // draw mouse over
    if (over_objs)
      drawGroupedObjs(painter1, CQChartsLayer::Type::MOUSE_OVER);

    if (over_annotations)
      drawGroupedAnnotations(painter1, CQChartsLayer::Type::MOUSE_OVER);
  }

  //---

  endPaint(buffer);
}

bool
CQChartsPlot::
hasBackgroundLayer() const
{
  // only first plot has background for overlay
  if (isOverlay() && ! isFirstPlot())
    return false;

  //---

  bool hasPlotBackground = (isPlotFilled() || isPlotBorder());
  bool hasDataBackground = (isDataFilled() || isDataBorder());
  bool hasFitBackground  = (isFitFilled () || isFitBorder ());
  bool hasBackground     = this->hasBackground();

  if (! hasPlotBackground && ! hasDataBackground && ! hasFitBackground && ! hasBackground)
    return false;

  if (! isLayerActive(CQChartsLayer::Type::BACKGROUND))
    return false;

  return true;
}

void
CQChartsPlot::
drawBackgroundLayer(QPainter *painter)
{
  CQPerfTrace trace("CQChartsPlot::drawBackgroundLayer");

  bool hasPlotBackground = (isPlotFilled() || isPlotBorder());
  bool hasDataBackground = (isDataFilled() || isDataBorder());
  bool hasFitBackground  = (isFitFilled () || isFitBorder ());
  bool hasBackground     = this->hasBackground();

  //---

  if (hasPlotBackground) {
    QRectF plotRect = CQChartsUtil::toQRect(calcPlotPixelRect());

    if (isPlotFilled()) {
      QBrush brush;

      setBrush(brush, true, interpPlotFillColor(0, 1), plotFillAlpha(), plotFillPattern());

      painter->fillRect(plotRect, brush);
    }

    if (isPlotBorder()) {
      QPen pen;

      setPen(pen, true, interpPlotBorderColor(0, 1), plotBorderAlpha(),
             plotBorderWidth(), plotBorderDash());

      painter->setPen(pen);

      drawBackgroundSides(painter, plotRect, plotBorderSides());
    }
  }

  if (hasFitBackground) {
    QRectF fitRect = CQChartsUtil::toQRect(calcFitPixelRect());

    if (isFitFilled()) {
      QBrush brush;

      setBrush(brush, true, interpFitFillColor(0, 1), fitFillAlpha(), fitFillPattern());

      painter->fillRect(fitRect, brush);
    }

    if (isFitBorder()) {
      QPen pen;

      setPen(pen, true, interpFitBorderColor(0, 1), fitBorderAlpha(),
             fitBorderWidth(), fitBorderDash());

      painter->setPen(pen);

      drawBackgroundSides(painter, fitRect, fitBorderSides());
    }
  }

  if (hasDataBackground) {
    QRectF dataRect = CQChartsUtil::toQRect(calcDataPixelRect());

    if (isDataFilled()) {
      QBrush brush;

      setBrush(brush, true, interpDataFillColor(0, 1), dataFillAlpha(), dataFillPattern());

      painter->fillRect(dataRect, brush);
    }

    if (isDataBorder()) {
      QPen pen;

      setPen(pen, true, interpDataBorderColor(0, 1), dataBorderAlpha(),
             dataBorderWidth(), dataBorderDash());

      painter->setPen(pen);

      drawBackgroundSides(painter, dataRect, dataBorderSides());
    }
  }

  //---

  if (hasBackground)
    drawBackground(painter);
}

bool
CQChartsPlot::
hasBackground() const
{
  return false;
}

void
CQChartsPlot::
drawBackground(QPainter *)
{
}

void
CQChartsPlot::
drawBackgroundSides(QPainter *painter, const QRectF &rect, const CQChartsSides &sides)
{
  if (sides.isTop   ()) painter->drawLine(rect.topLeft   (), rect.topRight   ());
  if (sides.isLeft  ()) painter->drawLine(rect.topLeft   (), rect.bottomLeft ());
  if (sides.isBottom()) painter->drawLine(rect.bottomLeft(), rect.bottomRight());
  if (sides.isRight ()) painter->drawLine(rect.topRight  (), rect.bottomRight());
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

  if (! isLayerActive(CQChartsLayer::Type::BG_AXES))
    return false;

  return true;
}

void
CQChartsPlot::
drawBgAxes(QPainter *painter)
{
  CQPerfTrace trace("CQChartsPlot::drawBgAxes");

  bool showXAxis = (xAxis() && xAxis()->isVisible());
  bool showYAxis = (yAxis() && yAxis()->isVisible());

  bool showXGrid = (showXAxis && ! xAxis()->isGridAbove() && xAxis()->isDrawGrid());
  bool showYGrid = (showYAxis && ! yAxis()->isGridAbove() && yAxis()->isDrawGrid());

  //---

  if (showXGrid)
    xAxis()->drawGrid(this, painter);

  if (showYGrid)
    yAxis()->drawGrid(this, painter);
}

bool
CQChartsPlot::
hasBgKey() const
{
  if (isOverview())
    return false;

  CQChartsPlotKey *key1;

  if (isOverlay()) {
    // only draw key under first plot
    if (! isFirstPlot())
      return false;

    // use first plot key (for overlay)
    key1 = getFirstPlotKey();
  }
  else {
    key1 = this->key();
  }

  //---

  bool showKey = (key1 && ! key1->isAbove());

  if (! showKey)
    return false;

  //---

  if (! isLayerActive(CQChartsLayer::Type::BG_KEY))
    return false;

  return true;
}

void
CQChartsPlot::
drawBgKey(QPainter *painter)
{
  CQPerfTrace trace("CQChartsPlot::drawBgKey");

  CQChartsPlotKey *key1;

  if (isOverlay()) {
    // only draw key under first plot
    assert(! isFirstPlot());

    // use first plot key (for overlay)
    key1 = getFirstPlotKey();
  }
  else {
    key1 = this->key();
  }

  //---

  key1->draw(painter);
}

bool
CQChartsPlot::
hasGroupedObjs(const CQChartsLayer::Type &layerType) const
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
drawGroupedObjs(QPainter *painter, const CQChartsLayer::Type &layerType)
{
  CQPerfTrace trace("CQChartsPlot::drawGroupedObjs");

  // for overlay draw all combine objects on common layers
  if (isOverlay()) {
    if (! isFirstPlot())
      return;

    processOverlayPlots([&](CQChartsPlot *plot) {
      plot->drawObjs(painter, layerType);
    });
  }
  else {
    drawObjs(painter, layerType);
  }
}

bool
CQChartsPlot::
hasObjs(const CQChartsLayer::Type &layerType) const
{
  CQChartsGeom::BBox bbox = displayRangeBBox();

  bool anyObjs = false;

  for (const auto &plotObj : plotObjects()) {
    if      (layerType == CQChartsLayer::Type::SELECTION) {
      if (! plotObj->isSelected())
        continue;
    }
    else if (layerType == CQChartsLayer::Type::MOUSE_OVER) {
      if (! plotObj->isInside())
        continue;
    }

    if (! bbox.overlaps(plotObj->rect()))
      continue;

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
drawObjs(QPainter *painter, const CQChartsLayer::Type &layerType)
{
  CQPerfTrace trace("CQChartsPlot::drawObjs");

  drawLayer_ = layerType;

  CQChartsGeom::BBox bbox = displayRangeBBox();

  //---

  painter->save();

  setClipRect(painter);

  for (const auto &plotObj : plotObjects()) {
    if      (layerType == CQChartsLayer::Type::SELECTION) {
      if (! plotObj->isSelected())
        continue;
    }
    else if (layerType == CQChartsLayer::Type::MOUSE_OVER) {
      if (! plotObj->isInside())
        continue;
    }

    if (! bbox.overlaps(plotObj->rect()))
      continue;

    if      (layerType == CQChartsLayer::Type::BG_PLOT)
      plotObj->drawBg(painter);
    else if (layerType == CQChartsLayer::Type::FG_PLOT)
      plotObj->drawFg(painter);
    else if (layerType == CQChartsLayer::Type::MID_PLOT)
      plotObj->draw(painter);
    else if (layerType == CQChartsLayer::Type::SELECTION) {
      plotObj->draw  (painter);
      plotObj->drawFg(painter);
    }
    else if (layerType == CQChartsLayer::Type::MOUSE_OVER) {
      plotObj->draw  (painter);
      plotObj->drawFg(painter);
    }
  }

  painter->restore();
}

bool
CQChartsPlot::
hasFgAxes() const
{
  bool showXAxis = (xAxis() && xAxis()->isVisible());
  bool showYAxis = (yAxis() && yAxis()->isVisible());

  if (! showXAxis && ! showYAxis)
    return false;

  //---

  if (! isLayerActive(CQChartsLayer::Type::FG_AXES))
    return false;

  return true;
}

void
CQChartsPlot::
drawFgAxes(QPainter *painter)
{
  CQPerfTrace trace("CQChartsPlot::drawFgAxes");

  bool showXAxis = (xAxis() && xAxis()->isVisible());
  bool showYAxis = (yAxis() && yAxis()->isVisible());

  bool showXGrid = (showXAxis && xAxis()->isGridAbove() && xAxis()->isDrawGrid());
  bool showYGrid = (showYAxis && yAxis()->isGridAbove() && yAxis()->isDrawGrid());

  //---

  if (showXGrid)
    xAxis()->drawGrid(this, painter);

  if (showYGrid)
    yAxis()->drawGrid(this, painter);

  //---

  if (showXAxis)
    xAxis()->draw(this, painter);

  if (showYAxis)
    yAxis()->draw(this, painter);
}

bool
CQChartsPlot::
hasFgKey() const
{
  if (isOverview())
    return false;

  CQChartsPlotKey *key1;

  if (isOverlay()) {
    // only draw key above last plot
    if (lastPlot() != this)
      return false;

    // use first plot key (for overlay)
    key1 = getFirstPlotKey();
  }
  else {
    key1 = this->key();
  }

  //---

  bool showKey = (key1 && key1->isAbove());

  if (! showKey)
    return false;

  //---

  if (! isLayerActive(CQChartsLayer::Type::FG_KEY))
    return false;

  return true;
}

void
CQChartsPlot::
drawFgKey(QPainter *painter)
{
  CQPerfTrace trace("CQChartsPlot::drawFgKey");

  CQChartsPlotKey *key1;

  if (isOverlay()) {
    // only draw key above last plot
    assert(lastPlot() == this);

    // use first plot key (for overlay)
    key1 = getFirstPlotKey();
  }
  else {
    key1 = this->key();
  }

  //---

  key1->draw(painter);
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

  if (! isLayerActive(CQChartsLayer::Type::TITLE))
    return false;

  return true;
}

void
CQChartsPlot::
drawTitle(QPainter *painter)
{
  CQPerfTrace trace("CQChartsPlot::drawTitle");

  title()->draw(painter);
}

bool
CQChartsPlot::
hasGroupedAnnotations(const CQChartsLayer::Type &layerType) const
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
drawGroupedAnnotations(QPainter *painter, const CQChartsLayer::Type &layerType)
{
  CQPerfTrace trace("CQChartsPlot::drawGroupedAnnotations");

  if (! hasGroupedAnnotations(layerType))
    return;

  //---

  // for overlay draw all combine objects on common layers
  if (isOverlay()) {
    if (! isFirstPlot())
      return;

    processOverlayPlots([&](CQChartsPlot *plot) {
      plot->drawAnnotations(painter, layerType);
    });
  }
  else {
    drawAnnotations(painter, layerType);
  }
}

bool
CQChartsPlot::
hasAnnotations(const CQChartsLayer::Type &layerType) const
{
  bool anyObjs = false;

  for (const auto &annotation : annotations()) {
    if      (layerType == CQChartsLayer::Type::SELECTION) {
      if (! annotation->isSelected())
        continue;
    }
    else if (layerType == CQChartsLayer::Type::MOUSE_OVER) {
      if (! annotation->isInside())
        continue;
    }

//  if (! bbox.overlaps(annotation->bbox()))
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
drawAnnotations(QPainter *painter, const CQChartsLayer::Type &layerType)
{
  CQPerfTrace trace("CQChartsPlot::drawAnnotations");

  drawLayer_ = layerType;

  //---

  for (auto &annotation : annotations()) {
    if      (layerType == CQChartsLayer::Type::SELECTION) {
      if (! annotation->isSelected())
        continue;
    }
    else if (layerType == CQChartsLayer::Type::MOUSE_OVER) {
      if (! annotation->isInside())
        continue;
    }

//  if (! bbox.overlaps(annotation->bbox()))
//    continue;

    annotation->draw(painter);
  }
}

bool
CQChartsPlot::
hasForeground() const
{
  return false;
}

void
CQChartsPlot::
drawForeground(QPainter *)
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

  if (! isLayerActive(CQChartsLayer::Type::BOXES))
    return false;

  return true;
}

void
CQChartsPlot::
drawGroupedBoxes(QPainter *painter)
{
  CQPerfTrace trace("CQChartsPlot::drawGroupedBoxes");

  // for overlay draw all combine objects on common layers
  if (isOverlay()) {
    if (! isFirstPlot())
      return;

    processOverlayPlots([&](CQChartsPlot *plot) {
      plot->drawBoxes(painter);
    });
  }
  else {
    drawBoxes(painter);
  }
}

bool
CQChartsPlot::
hasBoxes() const
{
  if (! showBoxes())
    return false;

  //---

  if (! isLayerActive(CQChartsLayer::Type::BOXES))
    return false;

  return true;
}

void
CQChartsPlot::
drawBoxes(QPainter *painter)
{
  CQPerfTrace trace("CQChartsPlot::drawBoxes");

  CQChartsGeom::BBox bbox = fitBBox();

  drawWindowColorBox(painter, bbox);

  drawWindowColorBox(painter, dataFitBBox   ());
  drawWindowColorBox(painter, axesFitBBox   ());
  drawWindowColorBox(painter, keyFitBBox    ());
  drawWindowColorBox(painter, titleFitBBox  ());
  drawWindowColorBox(painter, annotationBBox());

  //---

  drawWindowColorBox(painter, CQChartsUtil::rangeBBox(calcDataRange_ ), Qt::green);
  drawWindowColorBox(painter, CQChartsUtil::rangeBBox(dataRange_     ), Qt::green);
  drawWindowColorBox(painter, CQChartsUtil::rangeBBox(outerDataRange_), Qt::green);
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

  if (! isLayerActive(CQChartsLayer::Type::EDIT_HANDLE))
    return false;

  return true;
}

void
CQChartsPlot::
drawGroupedEditHandles(QPainter *painter)
{
  CQPerfTrace trace("CQChartsPlot::drawGroupedEditHandles");

  // for overlay draw all combine objects on common layers
  if (isOverlay()) {
    if (! isFirstPlot())
      return;

    processOverlayPlots([&](CQChartsPlot *plot) {
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
  if (view()->mode() != CQChartsView::Mode::EDIT)
    return false;

  //---

  bool selected = (isSelected() ||
                   (title() && title()->isSelected()) ||
                   (xAxis() && xAxis()->isSelected()) ||
                   (yAxis() && yAxis()->isSelected()));

  if (! selected) {
    CQChartsPlotKey *key1 = getFirstPlotKey();

    selected = (key1 && key1->isSelected());
  }

  if (! selected) {
    for (const auto &annotation : annotations()) {
      if (annotation->isSelected()) {
        selected = true;
        break;
      }
    }
  }

  if (! selected)
    return false;

  //---

  if (! isLayerActive(CQChartsLayer::Type::EDIT_HANDLE))
    return false;

  return true;
}

void
CQChartsPlot::
drawEditHandles(QPainter *painter)
{
  CQPerfTrace trace("CQChartsPlot::drawEditHandles");

  CQChartsPlotKey *key1 = getFirstPlotKey();

  if      (isSelected()) {
    editHandles_.setBBox(this->viewBBox());

    editHandles_.draw(painter);
  }
  else if (title() && title()->isSelected()) {
    title()->drawEditHandles(painter);
  }
  else if (key1 && key1->isSelected()) {
    key1->drawEditHandles(painter);
  }
  else if (xAxis() && xAxis()->isSelected()) {
    xAxis()->drawEditHandles(painter);
  }
  else if (yAxis() && yAxis()->isSelected()) {
    yAxis()->drawEditHandles(painter);
  }
  else {
    for (const auto &annotation : annotations()) {
      if (annotation->isSelected())
        annotation->drawEditHandles(painter);
    }
  }
}

CQChartsGeom::BBox
CQChartsPlot::
displayRangeBBox() const
{
  // calc current (zoomed/panned) data range
  double xmin, ymin, xmax, ymax;

  displayRange_->getWindowRange(&xmin, &ymin, &xmax, &ymax);

  CQChartsGeom::BBox bbox(xmin, ymin, xmax, ymax);

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
calcDataPixelRect() const
{
  // calc current (zoomed/panned) pixel range
  CQChartsGeom::BBox bbox = displayRangeBBox();

  CQChartsGeom::BBox pbbox;

  windowToPixel(bbox, pbbox);

  return pbbox;
}

CQChartsGeom::BBox
CQChartsPlot::
calcPlotPixelRect() const
{
  return view_->windowToPixel(viewBBox_);
}

QSizeF
CQChartsPlot::
calcPixelSize() const
{
  CQChartsGeom::BBox bbox = calcPlotPixelRect();

  return QSizeF(bbox.getWidth(), bbox.getHeight());
}

//---

void
CQChartsPlot::
autoFit()
{
  CQPerfTrace trace("CQChartsPlot::autoFit");

  zoomFull(/*notify*/false);

  if (isOverlay()) {
    if (prevPlot())
      return;

    //---

    // combine bboxes of overlay plots
    CQChartsGeom::BBox bbox;

    processOverlayPlots([&](const CQChartsPlot *plot) {
      CQChartsGeom::BBox bbox1 = plot->fitBBox();

      CQChartsGeom::BBox bbox2;

      plot->windowToPixel(bbox1, bbox2);

      pixelToWindow(bbox2, bbox1);

      bbox += bbox1;
    });

    //---

    // set all overlay plot bboxes
    using BBoxes = std::vector<CQChartsGeom::BBox>;

    BBoxes bboxes;

    processOverlayPlots([&](const CQChartsPlot *plot) {
      CQChartsGeom::BBox bbox1;

      windowToPixel(bbox, bbox1);

      CQChartsGeom::BBox bbox2;

      plot->pixelToWindow(bbox1, bbox2);

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
  for (int i = 0; i < 3; ++i) {
    CQChartsGeom::BBox bbox = fitBBox();

    setFitBBox(bbox);

    updateRangeAndObjsInternal();

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
  }

  emit zoomPanChanged();
}

void
CQChartsPlot::
setFitBBox(const CQChartsGeom::BBox &bbox)
{
  // calc margin so plot box fits in specified box
  CQChartsGeom::BBox pbbox = displayRangeBBox();

  double left   = 100.0*(pbbox.getXMin() -  bbox.getXMin())/bbox.getWidth ();
  double bottom = 100.0*(pbbox.getYMin() -  bbox.getYMin())/bbox.getHeight();
  double right  = 100.0*( bbox.getXMax() - pbbox.getXMax())/bbox.getWidth ();
  double top    = 100.0*( bbox.getYMax() - pbbox.getYMax())/bbox.getHeight();

  if (isInvertX()) std::swap(left, right );
  if (isInvertY()) std::swap(top , bottom);

  outerMargin_ = CQChartsPlotMargin(left, top, right, bottom);

  updateMargins();
}

CQChartsGeom::BBox
CQChartsPlot::
fitBBox() const
{
  CQChartsGeom::BBox bbox;

  bbox += dataFitBBox   ();
  bbox += axesFitBBox   ();
  bbox += keyFitBBox    ();
  bbox += titleFitBBox  ();
  bbox += annotationBBox();

  // add margin (TODO: config pixel margin size)
  double xm = pixelToWindowWidth (8);
  double ym = pixelToWindowHeight(8);

  bbox.expand(-xm, -ym, xm, ym);

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
dataFitBBox() const
{
  CQChartsGeom::BBox bbox = displayRangeBBox();

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
axesFitBBox() const
{
  CQChartsGeom::BBox bbox;

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
  CQChartsGeom::BBox bbox;

  if (key() && key()->isVisible()) {
    CQChartsGeom::BBox bbox1 = key()->bbox();

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
  CQChartsGeom::BBox bbox;

  if (title() && title()->isVisible())
    bbox += title()->bbox();

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
calcFitPixelRect() const
{
  // calc current (zoomed/panned) pixel range
  CQChartsGeom::BBox bbox = fitBBox();

  CQChartsGeom::BBox pbbox;

  windowToPixel(bbox, pbbox);

  return pbbox;
}

//------

CQChartsTextAnnotation *
CQChartsPlot::
addTextAnnotation(const CQChartsPosition &pos, const QString &text)
{
  CQChartsTextAnnotation *textAnnotation = new CQChartsTextAnnotation(this, pos, text);

  addAnnotation(textAnnotation);

  return textAnnotation;
}

CQChartsTextAnnotation *
CQChartsPlot::
addTextAnnotation(const CQChartsRect &rect, const QString &text)
{
  CQChartsTextAnnotation *textAnnotation = new CQChartsTextAnnotation(this, rect, text);

  addAnnotation(textAnnotation);

  return textAnnotation;
}

CQChartsArrowAnnotation *
CQChartsPlot::
addArrowAnnotation(const CQChartsPosition &start, const CQChartsPosition &end)
{
  CQChartsArrowAnnotation *arrowAnnotation = new CQChartsArrowAnnotation(this, start, end);

  addAnnotation(arrowAnnotation);

  return arrowAnnotation;
}

CQChartsRectAnnotation *
CQChartsPlot::
addRectAnnotation(const CQChartsRect &rect)
{
  CQChartsRectAnnotation *rectAnnotation = new CQChartsRectAnnotation(this, rect);

  addAnnotation(rectAnnotation);

  return rectAnnotation;
}

CQChartsEllipseAnnotation *
CQChartsPlot::
addEllipseAnnotation(const CQChartsPosition &center, const CQChartsLength &xRadius,
                     const CQChartsLength &yRadius)
{
  CQChartsEllipseAnnotation *ellipseAnnotation =
    new CQChartsEllipseAnnotation(this, center, xRadius, yRadius);

  addAnnotation(ellipseAnnotation);

  return ellipseAnnotation;
}

CQChartsPolygonAnnotation *
CQChartsPlot::
addPolygonAnnotation(const QPolygonF &points)
{
  CQChartsPolygonAnnotation *polyAnnotation = new CQChartsPolygonAnnotation(this, points);

  addAnnotation(polyAnnotation);

  return polyAnnotation;
}

CQChartsPolylineAnnotation *
CQChartsPlot::
addPolylineAnnotation(const QPolygonF &points)
{
  CQChartsPolylineAnnotation *polyAnnotation = new CQChartsPolylineAnnotation(this, points);

  addAnnotation(polyAnnotation);

  return polyAnnotation;
}

CQChartsPointAnnotation *
CQChartsPlot::
addPointAnnotation(const CQChartsPosition &pos, const CQChartsSymbol &type)
{
  CQChartsPointAnnotation *pointAnnotation = new CQChartsPointAnnotation(this, pos, type);

  addAnnotation(pointAnnotation);

  return pointAnnotation;
}

void
CQChartsPlot::
addAnnotation(CQChartsAnnotation *annotation)
{
  annotations_.push_back(annotation);

  connect(annotation, SIGNAL(idChanged()), this, SLOT(updateAnnotationSlot()));
  connect(annotation, SIGNAL(dataChanged()), this, SLOT(updateAnnotationSlot()));

  annotation->addProperties(propertyModel(), id() + "/" + "annotations");

//emit annotationAdded(annotation->id());
  emit annotationsChanged();
}

CQChartsAnnotation *
CQChartsPlot::
getAnnotationByName(const QString &id) const
{
  for (auto &annotation : annotations()) {
    if (annotation->id() == id)
      return annotation;
  }

  return nullptr;
}

void
CQChartsPlot::
removeAnnotation(CQChartsAnnotation *annotation)
{
//QString id = annotation->id();

  int pos = 0;

  for (auto &annotation1 : annotations_) {
    if (annotation1 == annotation)
      break;

    ++pos;
  }

  int n = annotations_.size();

  assert(pos >= 0 && pos < n);

  propertyModel()->removeProperties(id() + "/" + "annotations/" + annotation->propertyId());

  delete annotation;

  for (int i = pos + 1; i < n; ++i)
    annotations_[i - 1] = annotations_[i];

  annotations_.pop_back();

//emit annotationRemoved(id);
  emit annotationsChanged();
}

void
CQChartsPlot::
removeAllAnnotations()
{
  for (auto &annotation : annotations_)
    delete annotation;

  annotations_.clear();

  propertyModel()->removeProperties(id() + "/" + "annotations");

//emit allAnnotationsRemoved();
  emit annotationsChanged();
}

void
CQChartsPlot::
updateAnnotationSlot()
{
  updateSlot();

  emit annotationsChanged();
}

//------

CQChartsPlotObj *
CQChartsPlot::
getObject(const QString &objectId) const
{
  QList<QModelIndex> inds;

  for (auto &plotObj : plotObjects()) {
    if (plotObj->id() == objectId)
      return plotObj;
  }

  return nullptr;
}

QList<QModelIndex>
CQChartsPlot::
getObjectInds(const QString &objectId) const
{
  QList<QModelIndex> inds;

  CQChartsPlotObj *plotObj = getObject(objectId);

  if (plotObj) {
    CQChartsPlotObj::Indices inds1;

    plotObj->getSelectIndices(inds1);

    for (auto &ind1 : inds1)
      inds.push_back(ind1);
  }

  return inds;
}

//------

CQChartsLayer *
CQChartsPlot::
initLayer(const CQChartsLayer::Type &type, const CQChartsBuffer::Type &buffer, bool active)
{
  auto pb = buffers_.find(buffer);

  if (pb == buffers_.end()) {
    CQChartsBuffer *layerBuffer = new CQChartsBuffer(buffer);

    pb = buffers_.insert(pb, Buffers::value_type(buffer, layerBuffer));
  }

  //CQChartsBuffer *layerBuffer = (*pb).second;

  //---

  auto pl = layers_.find(type);

  if (pl == layers_.end()) {
    CQChartsLayer *layer = new CQChartsLayer(type, buffer);

    pl = layers_.insert(pl, Layers::value_type(type, layer));
  }

  CQChartsLayer *layer = (*pl).second;

  layer->setActive(active);

  return layer;
}

void
CQChartsPlot::
setLayerActive(const CQChartsLayer::Type &type, bool b)
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
setLayerActive1(const CQChartsLayer::Type &type, bool b)
{
  CQChartsLayer *layer = getLayer(type);

  layer->setActive(b);

  emit layersChanged();
}

bool
CQChartsPlot::
isLayerActive(const CQChartsLayer::Type &type) const
{
  CQChartsLayer *layer = getLayer(type);

  return layer->isActive();
}

void
CQChartsPlot::
invalidateLayers()
{
  if (! isUpdatesEnabled()) {
    updatesData_.invalidateLayers = true;
    return;
  }

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

  emit layersChanged();

  fromInvalidate_ = true;

  update();
}

void
CQChartsPlot::
invalidateLayer(const CQChartsBuffer::Type &type)
{
  if (! isUpdatesEnabled()) {
    updatesData_.invalidateLayers = true;
    return;
  }

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
invalidateLayer1(const CQChartsBuffer::Type &type)
{
  CQChartsBuffer *layer = getBuffer(type);

  layer->setValid(false);

  emit layersChanged();

  fromInvalidate_ = true;

  update();
}

CQChartsBuffer *
CQChartsPlot::
getBuffer(const CQChartsBuffer::Type &type) const
{
  auto p = buffers_.find(type);
  assert(p != buffers_.end());

  return (*p).second;
}

CQChartsLayer *
CQChartsPlot::
getLayer(const CQChartsLayer::Type &type) const
{
  auto p = layers_.find(type);
  assert(p != layers_.end());

  return (*p).second;
}

//---

void
CQChartsPlot::
setClipRect(QPainter *painter) const
{
  CQChartsPlot *plot1 = firstPlot();

  if      (plot1->isDataClip()) {
    CQChartsGeom::BBox bbox = displayRangeBBox();

    CQChartsGeom::BBox abbox = annotationBBox();

    if      (dataScaleX() <= 1.0 && dataScaleY() <= 1.0)
      bbox.add(abbox);
    else if (dataScaleX() <= 1.0)
      bbox.addX(abbox);
    else if (dataScaleY() <= 1.0)
      bbox.addY(abbox);

    CQChartsGeom::BBox pbbox;

    windowToPixel(bbox, pbbox);

    QRectF dataRect = CQChartsUtil::toQRect(pbbox);

    painter->setClipRect(dataRect);
  }
  else if (plot1->isPlotClip()) {
    QRectF plotRect = CQChartsUtil::toQRect(calcPlotPixelRect());

    painter->setClipRect(plotRect);
  }
}

QPainter *
CQChartsPlot::
beginPaint(CQChartsBuffer *buffer, QPainter *painter, const QRectF &rect)
{
  drawBuffer_ = buffer->type();

  if (! view_->isBufferLayers())
    return painter;

  // resize and clear
  QRectF prect = (! rect.isValid() ? CQChartsUtil::toQRect(calcPlotPixelRect()) : rect);

  QPainter *painter1 = buffer->beginPaint(painter, prect, view()->isAntiAlias());

  // don't paint if not active
  if (! buffer->isActive())
    return nullptr;

  return painter1;
}

void
CQChartsPlot::
endPaint(CQChartsBuffer *buffer)
{
  if (! view_->isBufferLayers())
    return;

  buffer->endPaint();
}

CQChartsPlotKey *
CQChartsPlot::
getFirstPlotKey() const
{
  CQChartsPlot *plot1 = firstPlot();

  return (plot1 ? plot1->key() : nullptr);
}

//------

void
CQChartsPlot::
drawSymbol(QPainter *painter, const QPointF &p, const CQChartsSymbol &symbol,
           double size, const QPen &pen, const QBrush &brush) const
{
  painter->setPen  (pen);
  painter->setBrush(brush);

  drawSymbol(painter, p, symbol, size);
}

void
CQChartsPlot::
drawSymbol(QPainter *painter, const QPointF &p, const CQChartsSymbol &symbol, double size) const
{
  if (bufferSymbols_) {
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
      CQChartsSymbol symbol;
      double         size { 0.0 };
      int            isize { 0 };
      QPen           pen;
      QBrush         brush;
      QImage         image;
    };

    static ImageBuffer imageBuffer;

    if (symbol != imageBuffer.symbol ||
        size   != imageBuffer.size   ||
        ! cmpPen  (painter->pen  (), imageBuffer.pen  ) ||
        ! cmpBrush(painter->brush(), imageBuffer.brush)) {
      imageBuffer.symbol = symbol;
      imageBuffer.size   = size;
      imageBuffer.isize  =
        CMathRound::RoundUp(2*(size + std::max(painter->pen().widthF(), 1.0)));
      imageBuffer.pen    = painter->pen  ();
      imageBuffer.brush  = painter->brush();
      imageBuffer.image  =
        QImage(QSize(imageBuffer.isize, imageBuffer.isize), QImage::Format_ARGB32);

      imageBuffer.image.fill(QColor(0,0,0,0));

      QPainter ipainter(&imageBuffer.image);

      ipainter.setRenderHints(QPainter::Antialiasing);

      ipainter.setPen  (imageBuffer.pen  );
      ipainter.setBrush(imageBuffer.brush);

      QPointF p1(size, size);

      CQChartsSymbol2DRenderer srenderer(&ipainter, CQChartsUtil::fromQPoint(p1), size);

      if (painter->brush().style() != Qt::NoBrush) {
        CQChartsPlotSymbolMgr::fillSymbol(symbol, &srenderer);

        if (painter->pen().style() != Qt::NoPen)
          CQChartsPlotSymbolMgr::strokeSymbol(symbol, &srenderer);
      }
      else {
        if (painter->pen().style() != Qt::NoPen)
          CQChartsPlotSymbolMgr::drawSymbol(symbol, &srenderer);
      }
    }

    double is = imageBuffer.isize/2.0;

    painter->drawImage(p.x() - is, p.y() - is, imageBuffer.image);
  }
  else {
    CQChartsSymbol2DRenderer srenderer(painter, CQChartsUtil::fromQPoint(p), size);

    if (painter->brush().style() != Qt::NoBrush) {
      CQChartsPlotSymbolMgr::fillSymbol(symbol, &srenderer);

      if (painter->pen().style() != Qt::NoPen)
        CQChartsPlotSymbolMgr::strokeSymbol(symbol, &srenderer);
    }
    else {
      if (painter->pen().style() != Qt::NoPen)
        CQChartsPlotSymbolMgr::drawSymbol(symbol, &srenderer);
    }
  }
}

void
CQChartsPlot::
drawTextAtPoint(QPainter *painter, const QPointF &point, const QString &text,
                const QPen &pen, const CQChartsTextOptions &options) const
{
  if (CMathUtil::isZero(options.angle)) {
    QFontMetricsF fm(painter->font());

    double tw = fm.width(text);
    double ta = fm.ascent();
    double td = fm.descent();

    double dx = 0.0;

    if      (options.align & Qt::AlignHCenter)
      dx = -tw/2.0;
    else if (options.align & Qt::AlignRight)
      dx = -tw;

    double dy = 0.0;

    if      (options.align & Qt::AlignTop)
      dy = -ta;
    else if (options.align & Qt::AlignVCenter)
      dy = (ta - td)/2.0;
    else if (options.align & Qt::AlignBottom)
      dy = td;

    if (options.contrast)
      drawContrastText(painter, point.x() + dx, point.y() + dy, text, pen);
    else {
      painter->setPen(pen);

      painter->drawText(point.x() + dx, point.y() + dy, text);
    }
  }
  else {
    assert(false);
  }
}

void
CQChartsPlot::
drawTextInBox(QPainter *painter, const QRectF &rect, const QString &text,
              const QPen &pen, const CQChartsTextOptions &options) const
{
  CQChartsTextOptions options1 = options;

  options1.minScaleFontSize = minScaleFontSize();
  options1.maxScaleFontSize = maxScaleFontSize();

  CQChartsView::drawTextInBox(painter, rect, text, pen, options1);
}

void
CQChartsPlot::
drawContrastText(QPainter *painter, double x, double y, const QString &text,
                 const QPen &pen) const
{
  CQChartsView::drawContrastText(painter, x, y, text, pen);
}

//------

void
CQChartsPlot::
drawWindowColorBox(QPainter *painter, const CQChartsGeom::BBox &bbox, const QColor &c) const
{
  if (! bbox.isSet())
    return;

  CQChartsGeom::BBox prect;

  windowToPixel(bbox, prect);

  drawColorBox(painter, prect, c);
}

void
CQChartsPlot::
drawColorBox(QPainter *painter, const CQChartsGeom::BBox &bbox, const QColor &c) const
{
  painter->setPen(c);
  painter->setBrush(Qt::NoBrush);

  painter->drawRect(CQChartsUtil::toQRect(bbox));
}

//------

void
CQChartsPlot::
drawPieSlice(QPainter *painter, const CQChartsGeom::Point &c,
             double ri, double ro, double a1, double a2) const
{
  CQChartsGeom::BBox bbox(c.x - ro, c.y - ro, c.x + ro, c.y + ro);

  CQChartsGeom::BBox pbbox;

  windowToPixel(bbox, pbbox);

  //---

  QPainterPath path;

  if (! CMathUtil::isZero(ri)) {
    CQChartsGeom::BBox bbox1(c.x - ri, c.y - ri, c.x + ri, c.y + ri);

    CQChartsGeom::BBox pbbox1;

    windowToPixel(bbox1, pbbox1);

    //---

    double da = (isInvertX() != isInvertY() ? -1 : 1);

    double ra1 = da*CMathUtil::Deg2Rad(a1);
    double ra2 = da*CMathUtil::Deg2Rad(a2);

    double x1 = c.x + ri*cos(ra1);
    double y1 = c.y + ri*sin(ra1);
    double x2 = c.x + ro*cos(ra1);
    double y2 = c.y + ro*sin(ra1);

    double x3 = c.x + ri*cos(ra2);
    double y3 = c.y + ri*sin(ra2);
    double x4 = c.x + ro*cos(ra2);
    double y4 = c.y + ro*sin(ra2);

    double px1, py1, px2, py2, px3, py3, px4, py4;

    windowToPixel(x1, y1, px1, py1);
    windowToPixel(x2, y2, px2, py2);
    windowToPixel(x3, y3, px3, py3);
    windowToPixel(x4, y4, px4, py4);

    path.moveTo(px1, py1);
    path.lineTo(px2, py2);

    path.arcTo(CQChartsUtil::toQRect(pbbox), a1, a2 - a1);

    path.lineTo(px4, py4);
    path.lineTo(px3, py3);

    path.arcTo(CQChartsUtil::toQRect(pbbox1), a2, a1 - a2);
  }
  else {
    CQChartsGeom::Point pc;

    windowToPixel(c, pc);

    //---

    double a21 = a2 - a1;

    if (std::abs(a21) < 360.0) {
      path.moveTo(QPointF(pc.x, pc.y));

      path.arcTo(CQChartsUtil::toQRect(pbbox), a1, a2 - a1);
    }
    else {
      path.addEllipse(CQChartsUtil::toQRect(pbbox));
    }
  }

  path.closeSubpath();

  painter->drawPath(path);
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
hiddenChanged()
{
  updateObjs();
}

void
CQChartsPlot::
update()
{
  assert(fromInvalidate_);

  view_->update();

  fromInvalidate_ = false;
}

//------

void
CQChartsPlot::
setPenBrush(QPen &pen, QBrush &brush,
            bool stroked, const QColor &strokeColor, double strokeAlpha,
            const CQChartsLength &strokeWidth, const CQChartsLineDash &strokeDash,
            bool filled, const QColor &fillColor, double fillAlpha,
            const CQChartsFillPattern &pattern) const
{
  setPen(pen, stroked, strokeColor, strokeAlpha, strokeWidth, strokeDash);

  setBrush(brush, filled, fillColor, fillAlpha, pattern);
}

void
CQChartsPlot::
setPen(QPen &pen, bool stroked, const QColor &strokeColor, double strokeAlpha,
       const CQChartsLength &strokeWidth, const CQChartsLineDash &strokeDash) const
{
  // calc pen (stroke)
  if (stroked) {
    QColor color = strokeColor;

    color.setAlphaF(CMathUtil::clamp(strokeAlpha, 0.0, 1.0));

    pen.setColor(color);

    double width = limitLineWidth(lengthPixelWidth(strokeWidth));

    if (width > 0)
      pen.setWidthF(width);
    else
      pen.setWidthF(0.0);

    CQChartsUtil::penSetLineDash(pen, strokeDash);
  }
  else {
    pen.setStyle(Qt::NoPen);
  }
}

void
CQChartsPlot::
setBrush(QBrush &brush, bool filled, const QColor &fillColor, double fillAlpha,
         const CQChartsFillPattern &pattern) const
{
  // calc brush (fill)
  if (filled) {
    QColor color = fillColor;

    color.setAlphaF(CMathUtil::clamp(fillAlpha, 0.0, 1.0));

    brush.setColor(color);

    brush.setStyle(pattern.style());
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }
}

double
CQChartsPlot::
limitLineWidth(double w) const
{
  // TODO: configuration setting
  return CMathUtil::clamp(w, 0.0, CQChartsLineWidth::maxPixelValue());
}

//------

void
CQChartsPlot::
updateObjPenBrushState(const CQChartsObj *obj, QPen &pen, QBrush &brush) const
{
  if (! view_->isBufferLayers()) {
    // inside and selected
    if      (obj->isInside() && obj->isSelected()) {
      updateSelectedObjPenBrushState(pen, brush);
      updateInsideObjPenBrushState  (pen, brush, false);
    }
    // inside
    else if (obj->isInside()) {
      updateInsideObjPenBrushState(pen, brush);
    }
    // selected
    else if (obj->isSelected()) {
      updateSelectedObjPenBrushState(pen, brush);
    }
  }
  else {
    // inside
    if      (drawLayer_ == CQChartsLayer::Type::MOUSE_OVER) {
      if (obj->isInside())
        updateInsideObjPenBrushState(pen, brush);
    }
    // selected
    else if (drawLayer_ == CQChartsLayer::Type::SELECTION) {
      if (obj->isSelected())
        updateSelectedObjPenBrushState(pen, brush);
    }
  }
}

void
CQChartsPlot::
updateInsideObjPenBrushState(QPen &pen, QBrush &brush, bool outline) const
{
  // fill and stroke
  if (brush.style() != Qt::NoBrush) {
    if (view()->insideMode() == CQChartsView::HighlightDataMode::OUTLINE) {
      QColor opc;
      double alpha = 1.0;

      if (pen.style() != Qt::NoPen) {
        QColor pc = pen.color();

        if (view()->isInsideBorder())
          opc = view()->interpInsideBorderColor(0, 1);
        else
          opc = CQChartsUtil::invColor(pc);

        alpha = pc.alphaF();
      }
      else {
        QColor bc = brush.color();

        if (view()->isInsideBorder())
          opc = view()->interpInsideBorderColor(0, 1);
        else
          opc = CQChartsUtil::invColor(bc);
      }

      setPen(pen, true, opc, alpha,
             view()->insideBorderWidth(), view()->insideBorderDash());

      if (outline)
        setBrush(brush, false);
    }
    else {
      QColor bc = brush.color();

      QColor ibc;

      if (view()->isInsideFilled())
        ibc = view()->interpInsideFillColor(0, 1);
      else
        ibc = insideColor(bc);

      setBrush(brush, true, ibc, bc.alphaF(), view()->insideFillPattern());
    }
  }
  // just stroke
  else {
    QColor pc = pen.color();

    QColor opc;

    if (view()->isInsideBorder())
      opc = view()->interpInsideBorderColor(0, 1);
    else
      opc = CQChartsUtil::invColor(pc);

    setPen(pen, true, opc, pc.alphaF(),
           view()->insideBorderWidth(), view()->insideBorderDash());
  }
}

void
CQChartsPlot::
updateSelectedObjPenBrushState(QPen &pen, QBrush &brush) const
{
  // fill and stroke
  if      (brush.style() != Qt::NoBrush) {
    if (view()->selectedMode() == CQChartsView::HighlightDataMode::OUTLINE) {
      QColor opc;
      double alpha = 1.0;

      if (pen.style() != Qt::NoPen) {
        QColor pc = pen.color();

        if (view()->isSelectedBorder())
          opc = view()->interpSelectedBorderColor(0, 1);
        else
          opc = selectedColor(pc);

        alpha = pc.alphaF();
      }
      else {
        QColor bc = brush.color();

        if (view()->isSelectedBorder())
          opc = view()->interpSelectedBorderColor(0, 1);
        else
          opc = CQChartsUtil::invColor(bc);
      }

      setPen(pen, true, opc, alpha,
             view()->selectedBorderWidth(), view()->selectedBorderDash());

      setBrush(brush, false);
    }
    else {
      QColor bc = brush.color();

      QColor ibc;

      if (view()->isSelectedFilled())
        ibc = view()->interpSelectedFillColor(0, 1);
      else
        ibc = selectedColor(bc);

      double alpha = 1.0;

      if (view_->isBufferLayers())
        alpha = 0.5*bc.alphaF();
      else
        alpha = bc.alphaF();

      setBrush(brush, true, ibc, alpha, view()->selectedFillPattern());
    }
  }
  // just stroke
  else if (pen.style() != Qt::NoPen) {
    QColor pc = pen.color();

    QColor opc;

    if (view()->isSelectedBorder())
      opc = view()->interpSelectedBorderColor(0, 1);
    else
      opc = CQChartsUtil::invColor(pc);

    setPen(pen, true, opc, pc.alphaF(),
           view()->selectedBorderWidth(), view()->selectedBorderDash());
  }
}

QColor
CQChartsPlot::
insideColor(const QColor &c) const
{
  return CQChartsUtil::blendColors(c, CQChartsUtil::bwColor(c), 0.8);
}

QColor
CQChartsPlot::
selectedColor(const QColor &c) const
{
  return CQChartsUtil::blendColors(c, CQChartsUtil::bwColor(c), 0.6);
}

QColor
CQChartsPlot::
interpPaletteColor(int i, int n, bool scale) const
{
  double r = CMathUtil::norm(i, 0, n - 1);

  return interpPaletteColor(r, scale);
}

QColor
CQChartsPlot::
interpPaletteColor(double r, bool scale) const
{
  return view()->interpPaletteColor(r, scale);
}

QColor
CQChartsPlot::
interpIndPaletteColor(int ind, double r, bool scale) const
{
  return view()->interpIndPaletteColor(ind, r, scale);
}

QColor
CQChartsPlot::
interpGroupPaletteColor(int ig, int ng, int i, int n, bool scale) const
{
  double r = CMathUtil::norm(i + 1, 0, n  + 1);

  return view()->interpGroupPaletteColor(ig, ng, r, scale);
}

QColor
CQChartsPlot::
interpGroupPaletteColor(double r1, double r2, double dr) const
{
  CQChartsThemeObj *theme = view()->themeObj();

  // r1 is parent color and r2 is child color
  QColor c1 = theme->palette()->getColor(r1 - dr/2.0);
  QColor c2 = theme->palette()->getColor(r1 + dr/2.0);

  return CQChartsUtil::blendColors(c1, c2, r2);
}

QColor
CQChartsPlot::
interpThemeColor(double r) const
{
  return view()->interpThemeColor(r);
}

QColor
CQChartsPlot::
calcTextColor(const QColor &bg) const
{
  return CQChartsUtil::bwColor(bg);
}

//------

CQChartsPlot::ColumnType
CQChartsPlot::
columnValueType(const CQChartsColumn &column, const ColumnType &defType) const
{
  ColumnType         columnType;
  ColumnType         columnBaseType;
  CQChartsNameValues nameValues;

  (void) columnValueType(column, columnType, columnBaseType, nameValues, defType);

  return columnType;
}

bool
CQChartsPlot::
columnValueType(const CQChartsColumn &column, ColumnType &columnType, ColumnType &columnBaseType,
                CQChartsNameValues &nameValues, const ColumnType &defType) const
{
  if (! column.isValid()) {
    columnType = defType;
    return false;
  }

  CQChartsModelColumnDetails *columnDetails = this->columnDetails(column);

  if (columnDetails) {
    columnType     = columnDetails->type();
    columnBaseType = columnDetails->baseType();
    nameValues     = columnDetails->nameValues();

    if (columnType == ColumnType::NONE) {
      columnType     = defType;
      columnBaseType = defType;
      return false;
    }
  }
  else {
    QAbstractItemModel *model = this->model().data();
    assert(model);

    if (! CQChartsModelUtil::columnValueType(charts(), model, column, columnType,
                                             columnBaseType, nameValues)) {
      columnType     = defType;
      columnBaseType = defType;
      return false;
    }
  }

  return true;
}

bool
CQChartsPlot::
columnTypeStr(const CQChartsColumn &column, QString &typeStr) const
{
  QAbstractItemModel *model = this->model().data();
  assert(model);

  return CQChartsModelUtil::columnTypeStr(charts(), model, column, typeStr);
}

bool
CQChartsPlot::
setColumnTypeStr(const CQChartsColumn &column, const QString &typeStr)
{
  QAbstractItemModel *model = this->model().data();
  assert(model);

  return CQChartsModelUtil::setColumnTypeStr(charts(), model, column, typeStr);
}

bool
CQChartsPlot::
columnDetails(const CQChartsColumn &column, QString &typeName,
              QVariant &minValue, QVariant &maxValue) const
{
  if (! column.isValid())
    return false;

  CQChartsModelColumnDetails *details = this->columnDetails(column);
  if (! details) return false;

  typeName = details->typeName();
  minValue = details->minValue();
  maxValue = details->maxValue();

  return true;
}

CQChartsModelColumnDetails *
CQChartsPlot::
columnDetails(const CQChartsColumn &column) const
{
  CQChartsModelData *modelData = getModelData();
  if (! modelData) return nullptr;

  CQChartsModelDetails *details = modelData->details();
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
getHierColumnNames(const QModelIndex &parent, int row, const CQChartsColumns &nameColumns,
                   const QString &separator, QStringList &nameStrs, ModelIndices &nameInds) const
{
  QAbstractItemModel *model = this->model().data();
  assert(model);

  // single column (seprated names)
  if (nameColumns.count() == 1) {
    const CQChartsColumn &nameColumn = nameColumns.column();

    //---

    bool ok;

    QString name = modelString(row, nameColumn, parent, ok);

    if (ok && ! name.simplified().length())
      ok = false;

    if (ok) {
      if (separator.length())
        nameStrs = name.split(separator, QString::SkipEmptyParts);
      else
        nameStrs << name;
    }

    QModelIndex nameInd = modelIndex(row, nameColumn, parent);

    nameInds.push_back(nameInd);
  }
  else {
    for (auto &nameColumn : nameColumns) {
      bool ok;

      QString name = modelString(row, nameColumn, parent, ok);

      if (ok && ! name.simplified().length())
        ok = false;

      if (ok) {
        nameStrs << name;

        QModelIndex nameInd = modelIndex(row, nameColumn, parent);

        nameInds.push_back(nameInd);
      }
    }
  }

  return nameStrs.length();
}

//------

QModelIndex
CQChartsPlot::
normalizeIndex(const QModelIndex &ind) const
{
  // map index in proxy model, to source model (non-proxy model)
  QAbstractItemModel *model = this->model().data();
  assert(model);

  std::vector<QSortFilterProxyModel *> proxyModels;
  QAbstractItemModel*                  sourceModel;

  this->proxyModels(proxyModels, sourceModel);

  QModelIndex ind1 = ind;

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
  QAbstractItemModel *model = this->model().data();
  assert(model);

  std::vector<QSortFilterProxyModel *> proxyModels;
  QAbstractItemModel*                  sourceModel;

  this->proxyModels(proxyModels, sourceModel);

  QModelIndex ind1 = ind;

  // ind model should match source model of last proxy model
  int i = int(proxyModels.size()) - 1;

  for ( ; i >= 0; --i)
    if (ind1.model() == proxyModels[i]->sourceModel())
      break;

  for ( ; i >= 0; --i)
    ind1 = proxyModels[i]->mapFromSource(ind1);

  return ind1;
}

void
CQChartsPlot::
proxyModels(std::vector<QSortFilterProxyModel *> &proxyModels,
            QAbstractItemModel* &sourceModel) const
{
  // map index in source model (non-proxy model), to proxy model
  QAbstractItemModel *model = this->model().data();
  assert(model);

  QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(model);

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
  QAbstractItemModel *model = this->model().data();

  return CQChartsModelUtil::isHierarchical(model);
}

//------

void
CQChartsPlot::
addColumnValues(const CQChartsColumn &column, CQChartsValueSet &valueSet)
{
  class ValueSetVisitor : public ModelVisitor {
   public:
    ValueSetVisitor(const CQChartsPlot *plot, const CQChartsColumn &column,
                    CQChartsValueSet &valueSet) :
     plot_(plot), column_(column), valueSet_(valueSet) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      bool ok;

      QVariant value = plot_->modelValue(data.row, column_, data.parent, ok);

      // TODO: skip if not ok ?

      valueSet_.addValue(value);

      return State::OK;
    }

   private:
    const CQChartsPlot* plot_   { nullptr };
    CQChartsColumn      column_;
    CQChartsValueSet&   valueSet_;
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

  visitor.init();

  //if (isPreview())
  //  visitor.setMaxRows(previewMaxRows());

  (void) CQChartsModelVisit::exec(charts(), model().data(), visitor);
}

//------

bool
CQChartsPlot::
modelMappedReal(int row, const CQChartsColumn &column, const QModelIndex &parent,
                double &r, bool log, double def) const
{
  if (column.isValid()) {
    bool ok1;

    r = modelReal(row, column, parent, ok1);

    if (! ok1)
      r = def;

    if (CMathUtil::isNaN(r) || CMathUtil::isInf(r))
      return false;
  }
  else
    r = def;

  if (log)
    r = logValue(r);

  return true;
}

//------

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

      QString id = plot_->idColumnString(data.row, data.parent, ok);

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

  QVariant var = modelValue(row, idColumn(), parent, ok);

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
modelIndex(const CQChartsModelIndex &ind) const
{
  return modelIndex(ind.row, ind.column, ind.parent);
}

QModelIndex
CQChartsPlot::
modelIndex(int row, const CQChartsColumn &column, const QModelIndex &parent) const
{
  return modelIndex(row, column.column(), parent);
}

QModelIndex
CQChartsPlot::
modelIndex(int row, int column, const QModelIndex &parent) const
{
  QAbstractItemModel *model = this->model().data();
  if (! model) return QModelIndex();

  return model->index(row, column, parent);
}

//------

QVariant
CQChartsPlot::
modelHeaderValue(const CQChartsColumn &column, bool &ok) const
{
  return modelHeaderValue(model().data(), column, ok);
}

QVariant
CQChartsPlot::
modelHeaderValue(const CQChartsColumn &column, int role, bool &ok) const
{
  return modelHeaderValue(model().data(), column, role, ok);
}

QVariant
CQChartsPlot::
modelHeaderValue(int section, Qt::Orientation orient, int role, bool &ok) const
{
  return modelHeaderValue(model().data(), section, orient, role, ok);
}

QVariant
CQChartsPlot::
modelHeaderValue(int section, Qt::Orientation orient, bool &ok) const
{
  return modelHeaderValue(model().data(), section, orient, Qt::DisplayRole, ok);
}

QString
CQChartsPlot::
modelHeaderString(const CQChartsColumn &column, bool &ok) const
{
  return modelHeaderString(model().data(), column, ok);
}

QString
CQChartsPlot::
modelHeaderString(const CQChartsColumn &column, int role, bool &ok) const
{
  return modelHeaderString(model().data(), column, role, ok);
}

QString
CQChartsPlot::
modelHeaderString(int section, Qt::Orientation orient, int role, bool &ok) const
{
  return modelHeaderString(model().data(), section, orient, role, ok);
}

QString
CQChartsPlot::
modelHeaderString(int section, Qt::Orientation orient, bool &ok) const
{
  return modelHeaderString(model().data(), section, orient, Qt::DisplayRole, ok);
}

//------

QVariant
CQChartsPlot::
modelValue(const CQChartsModelIndex &ind, bool &ok) const
{
  return modelValue(ind.row, ind.column, ind.parent, ok);
}

QVariant
CQChartsPlot::
modelValue(int row, const CQChartsColumn &column, const QModelIndex &parent,
           int role, bool &ok) const
{
  return modelValue(model().data(), row, column, parent, role, ok);
}

QVariant
CQChartsPlot::
modelValue(int row, const CQChartsColumn &column, const QModelIndex &parent, bool &ok) const
{
  return modelValue(model().data(), row, column, parent, ok);
}

QString
CQChartsPlot::
modelString(const CQChartsModelIndex &ind, bool &ok) const
{
  return modelString(ind.row, ind.column, ind.parent, ok);
}

QString
CQChartsPlot::
modelString(int row, const CQChartsColumn &column, const QModelIndex &parent,
            int role, bool &ok) const
{
  return modelString(model().data(), row, column, parent, role, ok);
}

QString
CQChartsPlot::
modelString(int row, const CQChartsColumn &column, const QModelIndex &parent, bool &ok) const
{
  return modelString(model().data(), row, column, parent, ok);
}

double
CQChartsPlot::
modelReal(const CQChartsModelIndex &ind, bool &ok) const
{
  return modelReal(ind.row, ind.column, ind.parent, ok);
}

double
CQChartsPlot::
modelReal(int row, const CQChartsColumn &column, const QModelIndex &parent,
          int role, bool &ok) const
{
  return modelReal(model().data(), row, column, parent, role, ok);
}

double
CQChartsPlot::
modelReal(int row, const CQChartsColumn &column, const QModelIndex &parent, bool &ok) const
{
  return modelReal(model().data(), row, column, parent, ok);
}

long
CQChartsPlot::
modelInteger(const CQChartsModelIndex &ind, bool &ok) const
{
  return modelInteger(ind.row, ind.column, ind.parent, ok);
}

long
CQChartsPlot::
modelInteger(int row, const CQChartsColumn &column, const QModelIndex &parent,
             int role, bool &ok) const
{
  return modelInteger(model().data(), row, column, parent, role, ok);
}

long
CQChartsPlot::
modelInteger(int row, const CQChartsColumn &column, const QModelIndex &parent, bool &ok) const
{
  return modelInteger(model().data(), row, column, parent, ok);
}

CQChartsColor
CQChartsPlot::
modelColor(int row, const CQChartsColumn &column, const QModelIndex &parent,
           int role, bool &ok) const
{
  return modelColor(model().data(), row, column, parent, role, ok);
}

CQChartsColor
CQChartsPlot::
modelColor(int row, const CQChartsColumn &column, const QModelIndex &parent, bool &ok) const
{
  return modelColor(model().data(), row, column, parent, ok);
}

//---

std::vector<double>
CQChartsPlot::
modelReals(int row, const CQChartsColumn &column, const QModelIndex &parent, bool &ok) const
{
  std::vector<double> reals;

  QVariant var = modelValue(model().data(), row, column, parent, ok);

  if (! ok)
    return reals;

  return CQChartsVariant::toReals(var, ok);
}

//------

QVariant
CQChartsPlot::
modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column, bool &ok) const
{
  return CQChartsModelUtil::modelHeaderValue(model, column, ok);
}

QVariant
CQChartsPlot::
modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                 int role, bool &ok) const
{
  return CQChartsModelUtil::modelHeaderValue(model, column, role, ok);
}

QVariant
CQChartsPlot::
modelHeaderValue(QAbstractItemModel *model, int section, Qt::Orientation orientation,
                 bool &ok) const
{
  return CQChartsModelUtil::modelHeaderValue(model, section, orientation, ok);
}

QVariant
CQChartsPlot::
modelHeaderValue(QAbstractItemModel *model, int section, Qt::Orientation orientation,
                 int role, bool &ok) const
{
  return CQChartsModelUtil::modelHeaderValue(model, section, orientation, role, ok);
}

QString
CQChartsPlot::
modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column, bool &ok) const
{
  return CQChartsModelUtil::modelHeaderString(model, column, ok);
}

QString
CQChartsPlot::
modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                  int role, bool &ok) const
{
  return CQChartsModelUtil::modelHeaderString(model, column, role, ok);
}

QString
CQChartsPlot::
modelHeaderString(QAbstractItemModel *model, int section, Qt::Orientation orientation,
                  bool &ok) const
{
  return CQChartsModelUtil::modelHeaderString(model, section, orientation, ok);
}

QString
CQChartsPlot::
modelHeaderString(QAbstractItemModel *model, int section, Qt::Orientation orientation,
                  int role, bool &ok) const
{
  return CQChartsModelUtil::modelHeaderString(model, section, orientation, role, ok);
}

//--

QVariant
CQChartsPlot::
modelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
           const QModelIndex &parent, int role, bool &ok) const
{
  return CQChartsModelUtil::modelValue(charts(), model, row, column, parent, role, ok);
}

QVariant
CQChartsPlot::
modelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
           const QModelIndex &parent, bool &ok) const
{
  return CQChartsModelUtil::modelValue(charts(), model, row, column, parent, ok);
}

QString
CQChartsPlot::
modelString(QAbstractItemModel *model, int row, const CQChartsColumn &column,
            const QModelIndex &parent, int role, bool &ok) const
{
  return CQChartsModelUtil::modelString(charts(), model, row, column, parent, role, ok);
}

QString
CQChartsPlot::
modelString(QAbstractItemModel *model, int row, const CQChartsColumn &column,
            const QModelIndex &parent, bool &ok) const
{
  return CQChartsModelUtil::modelString(charts(), model, row, column, parent, ok);
}

double
CQChartsPlot::
modelReal(QAbstractItemModel *model, int row, const CQChartsColumn &column,
          const QModelIndex &parent, int role, bool &ok) const
{
  return CQChartsModelUtil::modelReal(charts(), model, row, column, parent, role, ok);
}

double
CQChartsPlot::
modelReal(QAbstractItemModel *model, int row, const CQChartsColumn &column,
          const QModelIndex &parent, bool &ok) const
{
  return CQChartsModelUtil::modelReal(charts(), model, row, column, parent, ok);
}

long
CQChartsPlot::
modelInteger(QAbstractItemModel *model, int row, const CQChartsColumn &column,
             const QModelIndex &parent, int role, bool &ok) const
{
  return CQChartsModelUtil::modelInteger(charts(), model, row, column, parent, role, ok);
}

long
CQChartsPlot::
modelInteger(QAbstractItemModel *model, int row, const CQChartsColumn &column,
             const QModelIndex &parent, bool &ok) const
{
  return CQChartsModelUtil::modelInteger(charts(), model, row, column, parent, ok);
}

CQChartsColor
CQChartsPlot::
modelColor(QAbstractItemModel *model, int row, const CQChartsColumn &column,
           const QModelIndex &parent, int role, bool &ok) const
{
  return CQChartsModelUtil::modelColor(charts(), model, row, column, parent, role, ok);
}

CQChartsColor
CQChartsPlot::
modelColor(QAbstractItemModel *model, int row, const CQChartsColumn &column,
           const QModelIndex &parent, bool &ok) const
{
  return CQChartsModelUtil::modelColor(charts(), model, row, column, parent, ok);
}

//------

QVariant
CQChartsPlot::
modelRootValue(int row, const CQChartsColumn &column, const QModelIndex &parent,
               int role, bool &ok) const
{
  if (column.column() == 0 && parent.isValid())
    return modelRootValue(parent.row(), column, parent.parent(), role, ok);

  return modelHierValue(row, column, parent, role, ok);
}

QVariant
CQChartsPlot::
modelRootValue(int row, const CQChartsColumn &column, const QModelIndex &parent, bool &ok) const
{
  if (column.column() == 0 && parent.isValid())
    return modelRootValue(parent.row(), column, parent.parent(), ok);

  return modelHierValue(row, column, parent, ok);
}

//------

QVariant
CQChartsPlot::
modelHierValue(int row, const CQChartsColumn &column, const QModelIndex &parent, bool &ok) const
{
  QVariant v = modelValue(row, column, parent, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

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
modelHierValue(int row, const CQChartsColumn &column,
               const QModelIndex &parent, int role, bool &ok) const
{
  QVariant v = modelValue(row, column, parent, role, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

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
modelHierString(int row, const CQChartsColumn &column,
                const QModelIndex &parent, bool &ok) const
{
  QVariant var = modelHierValue(row, column, parent, ok);

  if (! ok)
    return QString();

  QString str;

  bool rc = CQChartsVariant::toString(var, str);
  assert(rc);

  return str;
}

QString
CQChartsPlot::
modelHierString(int row, const CQChartsColumn &column,
                const QModelIndex &parent, int role, bool &ok) const
{
  QVariant var = modelHierValue(row, column, parent, role, ok);

  if (! ok)
    return QString();

  QString str;

  bool rc = CQChartsVariant::toString(var, str);
  assert(rc);

  return str;
}

//--

double
CQChartsPlot::
modelHierReal(int row, const CQChartsColumn &column, const QModelIndex &parent, bool &ok) const
{
  QVariant var = modelHierValue(row, column, parent, ok);

  if (! ok)
    return 0.0;

  return CQChartsVariant::toReal(var, ok);
}

double
CQChartsPlot::
modelHierReal(int row, const CQChartsColumn &column,
              const QModelIndex &parent, int role, bool &ok) const
{
  QVariant var = modelHierValue(row, column, parent, role, ok);

  if (! ok)
    return 0.0;

  return CQChartsVariant::toReal(var, ok);
}

//--

long
CQChartsPlot::
modelHierInteger(int row, const CQChartsColumn &column, const QModelIndex &parent, bool &ok) const
{
  QVariant var = modelHierValue(row, column, parent, ok);

  if (! ok)
    return 0;

  return CQChartsVariant::toInt(var, ok);
}

long
CQChartsPlot::
modelHierInteger(int row, const CQChartsColumn &column,
                 const QModelIndex &parent, int role, bool &ok) const
{
  QVariant var = modelHierValue(row, column, parent, role, ok);

  if (! ok)
    return 0;

  return CQChartsVariant::toInt(var, ok);
}

//------

bool
CQChartsPlot::
isSelectIndex(const QModelIndex &ind, int row, const CQChartsColumn &column,
              const QModelIndex &parent) const
{
  if (column.type() != CQChartsColumn::Type::DATA &&
      column.type() != CQChartsColumn::Type::DATA_INDEX)
    return false;

  return (ind == selectIndex(row, column.column(), parent));
}

QModelIndex
CQChartsPlot::
selectIndex(int row, const CQChartsColumn &column, const QModelIndex &parent) const
{
  if (column.type() != CQChartsColumn::Type::DATA &&
      column.type() != CQChartsColumn::Type::DATA_INDEX)
    return QModelIndex();

  std::vector<QSortFilterProxyModel *> proxyModels;
  QAbstractItemModel*                  sourceModel;

  this->proxyModels(proxyModels, sourceModel);

  return sourceModel->index(row, column.column(), parent);
}

void
CQChartsPlot::
beginSelect()
{
  selIndexColumnRows_.clear();

//itemSelection_ = QItemSelection();
}

void
CQChartsPlot::
addSelectIndex(const CQChartsModelIndex &ind)
{
  addSelectIndex(ind.row, ind.column.column(), ind.parent);
}

void
CQChartsPlot::
addSelectIndex(int row, int column, const QModelIndex &parent)
{
  addSelectIndex(selectIndex(row, column, parent));
}

void
CQChartsPlot::
addSelectIndex(const QModelIndex &ind)
{
  if (! ind.isValid())
    return;

  QModelIndex ind1 = unnormalizeIndex(ind);

  if (! ind1.isValid())
    return;

  // add to map ordered by parent, column, row
  selIndexColumnRows_[ind1.parent()][ind1.column()].insert(ind1.row());

//itemSelection_.select(ind1, ind1);
}

void
CQChartsPlot::
endSelect()
{
  QItemSelectionModel *sm = this->selectionModel();
  if (! sm) return;

  disconnect(sm, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
             this, SLOT(selectionSlot()));

  //---

  QAbstractItemModel *model = this->model().data();
  assert(model);

  QItemSelection optItemSelection;

  // build row range per index column
  for (const auto &p : selIndexColumnRows_) {
    const QModelIndex &parent     = p.first;
    const ColumnRows  &columnRows = p.second;

    // build row range per column
    for (const auto &p1 : columnRows) {
      int         column = p1.first;
      const Rows &rows   = p1.second;

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
          QModelIndex ind1 = modelIndex(startRow, column, parent);
          QModelIndex ind2 = modelIndex(endRow  , column, parent);

          optItemSelection.select(ind1, ind2);

          startRow = row;
          endRow   = row;
        }
      }

      if (startRow >= 0) {
        QModelIndex ind1 = modelIndex(startRow, column, parent);
        QModelIndex ind2 = modelIndex(endRow  , column, parent);

        optItemSelection.select(ind1, ind2);
      }
    }
  }

  if (optItemSelection.length())
    sm->select(optItemSelection, QItemSelectionModel::ClearAndSelect);

//sm->select(itemSelection_, QItemSelectionModel::ClearAndSelect);

  //---

  connect(sm, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this, SLOT(selectionSlot()));
}

//------

double
CQChartsPlot::
logValue(double x, int base) const
{
  if (x >= 1E-6)
    return std::log(x)/log(base);
  else
    return CMathUtil::getNaN();
}

double
CQChartsPlot::
expValue(double x, int base) const
{
  if (x <= 709.78271289)
    return std::exp(x*log(base));
  else
    return CMathUtil::getNaN();
}

//------

QPointF
CQChartsPlot::
positionToPlot(const CQChartsPosition &pos) const
{
  CQChartsGeom::Point p = CQChartsUtil::fromQPoint(pos.p());

  CQChartsGeom::Point p1 = p;

  if      (pos.units() == CQChartsUnits::PIXEL)
    p1 = pixelToWindow(p);
  else if (pos.units() == CQChartsUnits::PLOT)
    p1 = p;
  else if (pos.units() == CQChartsUnits::VIEW)
    p1 = pixelToWindow(view_->windowToPixel(p));
  else if (pos.units() == CQChartsUnits::PERCENT) {
    CQChartsGeom::BBox pbbox = displayRangeBBox();

    p1.setX(p.getX()*pbbox.getWidth ()/100.0);
    p1.setY(p.getY()*pbbox.getHeight()/100.0);
  }

  return CQChartsUtil::toQPoint(p1);
}

QPointF
CQChartsPlot::
positionToPixel(const CQChartsPosition &pos) const
{
  CQChartsGeom::Point p = CQChartsUtil::fromQPoint(pos.p());

  CQChartsGeom::Point p1 = p;

  if      (pos.units() == CQChartsUnits::PIXEL)
    p1 = p;
  else if (pos.units() == CQChartsUnits::PLOT)
    p1 = windowToPixel(p);
  else if (pos.units() == CQChartsUnits::VIEW)
    p1 = view_->windowToPixel(p);
  else if (pos.units() == CQChartsUnits::PERCENT) {
    CQChartsGeom::BBox pbbox = calcPlotPixelRect();

    p1.setX(p.getX()*pbbox.getWidth ()/100.0);
    p1.setY(p.getY()*pbbox.getHeight()/100.0);
  }

  return CQChartsUtil::toQPoint(p1);
}

//------

QRectF
CQChartsPlot::
rectToPlot(const CQChartsRect &rect) const
{
  CQChartsGeom::BBox r = CQChartsUtil::fromQRect(rect.rect());

  CQChartsGeom::BBox r1 = r;

  if      (rect.units() == CQChartsUnits::PIXEL)
    r1 = pixelToWindow(r);
  else if (rect.units() == CQChartsUnits::PLOT)
    r1 = r;
  else if (rect.units() == CQChartsUnits::VIEW)
    r1 = pixelToWindow(view_->windowToPixel(r));
  else if (rect.units() == CQChartsUnits::PERCENT) {
    CQChartsGeom::BBox pbbox = displayRangeBBox();

    r1.setXMin(r.getXMin()*pbbox.getWidth ()/100.0);
    r1.setYMin(r.getYMin()*pbbox.getHeight()/100.0);
    r1.setXMax(r.getXMax()*pbbox.getWidth ()/100.0);
    r1.setYMax(r.getYMax()*pbbox.getHeight()/100.0);
  }

  return CQChartsUtil::toQRect(r1);
}

QRectF
CQChartsPlot::
rectToPixel(const CQChartsRect &rect) const
{
  CQChartsGeom::BBox r = CQChartsUtil::fromQRect(rect.rect());

  CQChartsGeom::BBox r1 = r;

  if      (rect.units() == CQChartsUnits::PIXEL)
    r1 = r;
  else if (rect.units() == CQChartsUnits::PLOT)
    r1 = windowToPixel(r);
  else if (rect.units() == CQChartsUnits::VIEW)
    r1 = view_->windowToPixel(r);
  else if (rect.units() == CQChartsUnits::PERCENT) {
    CQChartsGeom::BBox pbbox = calcPlotPixelRect();

    r1.setXMin(r.getXMin()*pbbox.getWidth ()/100.0);
    r1.setYMin(r.getYMin()*pbbox.getHeight()/100.0);
    r1.setXMax(r.getXMax()*pbbox.getWidth ()/100.0);
    r1.setYMax(r.getYMax()*pbbox.getHeight()/100.0);
  }

  return CQChartsUtil::toQRect(r1);
}

//------

double
CQChartsPlot::
lengthPlotSize(const CQChartsLength &len, bool horizontal) const
{
  return (horizontal ? lengthPlotWidth(len) : lengthPlotHeight(len));
}

double
CQChartsPlot::
lengthPlotWidth(const CQChartsLength &len) const
{
  if      (len.units() == CQChartsUnits::PIXEL)
    return pixelToWindowWidth(len.value());
  else if (len.units() == CQChartsUnits::PLOT)
    return len.value();
  else if (len.units() == CQChartsUnits::VIEW)
    return pixelToWindowWidth(view_->windowToPixelWidth(len.value()));
  else if (len.units() == CQChartsUnits::PERCENT)
    return len.value()*displayRangeBBox().getWidth()/100.0;
  else
    return len.value();
}

double
CQChartsPlot::
lengthPlotHeight(const CQChartsLength &len) const
{
  if      (len.units() == CQChartsUnits::PIXEL)
    return pixelToWindowHeight(len.value());
  else if (len.units() == CQChartsUnits::PLOT)
    return len.value();
  else if (len.units() == CQChartsUnits::VIEW)
    return pixelToWindowHeight(view_->windowToPixelHeight(len.value()));
  else if (len.units() == CQChartsUnits::PERCENT)
    return len.value()*displayRangeBBox().getHeight()/100.0;
  else
    return len.value();
}

double
CQChartsPlot::
lengthPixelSize(const CQChartsLength &len, bool horizontal) const
{
  return (horizontal ? lengthPixelWidth(len) : lengthPixelHeight(len));
}

double
CQChartsPlot::
lengthPixelWidth(const CQChartsLength &len) const
{
  if      (len.units() == CQChartsUnits::PIXEL)
    return len.value();
  else if (len.units() == CQChartsUnits::PLOT)
    return windowToPixelWidth(len.value());
  else if (len.units() == CQChartsUnits::VIEW)
    return view_->windowToPixelWidth(len.value());
  else if (len.units() == CQChartsUnits::PERCENT)
    return len.value()*calcPlotPixelRect().getWidth()/100.0;
  else
    return len.value();
}

double
CQChartsPlot::
lengthPixelHeight(const CQChartsLength &len) const
{
  if      (len.units() == CQChartsUnits::PIXEL)
    return len.value();
  else if (len.units() == CQChartsUnits::PLOT)
    return windowToPixelHeight(len.value());
  else if (len.units() == CQChartsUnits::VIEW)
    return view_->windowToPixelHeight(len.value());
  else if (len.units() == CQChartsUnits::PERCENT)
    return len.value()*calcPlotPixelRect().getHeight()/100.0;
  else
    return len.value();
}

//------

void
CQChartsPlot::
windowToPixel(double wx, double wy, double &px, double &py) const
{
  double vx, vy;

  windowToView(wx, wy, vx, vy);

  view_->windowToPixel(vx, vy, px, py);
}

double
CQChartsPlot::
windowToViewWidth(double wx) const
{
  double vx1, vy1, vx2, vy2;

  windowToView(0.0, 0.0, vx1, vy1);
  windowToView(wx , wx , vx2, vy2);

  return fabs(vx2 - vx1);
}

double
CQChartsPlot::
windowToViewHeight(double wy) const
{
  double vx1, vy1, vx2, vy2;

  windowToView(0.0, 0.0, vx1, vy1);
  windowToView(wy , wy , vx2, vy2);

  return fabs(vy2 - vy1);
}

void
CQChartsPlot::
windowToView(double wx, double wy, double &vx, double &vy) const
{
  double wx1, wy1;

  displayTransform_->getMatrix().multiplyPoint(wx, wy, &wx1, &wy1);

  displayRange_->windowToPixel(wx1, wy1, &vx, &vy);

  if (isInvertX() || isInvertY()) {
    double ivx, ivy;

    displayRange_->invertPixel(vx, vy, ivx, ivy);

    if (isInvertX()) vx = ivx;
    if (isInvertY()) vy = ivy;
  }
}

void
CQChartsPlot::
pixelToWindow(double px, double py, double &wx, double &wy) const
{
  double vx, vy;

  view_->pixelToWindow(px, py, vx, vy);

  viewToWindow(vx, vy, wx, wy);
}

double
CQChartsPlot::
viewToWindowWidth(double vx) const
{
  double wx1, wy1, wx2, wy2;

  viewToWindow(0.0, 0.0, wx1, wy1);
  viewToWindow(vx , vx , wx2, wy2);

  return fabs(wx2 - wx1);
}

double
CQChartsPlot::
viewToWindowHeight(double vy) const
{
  double wx1, wy1, wx2, wy2;

  viewToWindow(0.0, 0.0, wx1, wy1);
  viewToWindow(vy , vy , wx2, wy2);

  return fabs(wy2 - wy1);
}

void
CQChartsPlot::
viewToWindow(double vx, double vy, double &wx, double &wy) const
{
  if (isInvertX() || isInvertY()) {
    double ivx, ivy;

    displayRange_->invertPixel(vx, vy, ivx, ivy);

    if (isInvertX()) vx = ivx;
    if (isInvertY()) vy = ivy;
  }

  double wx2, wy2;

  displayRange_->pixelToWindow(vx, vy, &wx2, &wy2);

  displayTransform_->getIMatrix().multiplyPoint(wx2, wy2, &wx, &wy);
}

void
CQChartsPlot::
windowToPixel(const CQChartsGeom::Point &w, CQChartsGeom::Point &p) const
{
  windowToPixel(w.x, w.y, p.x, p.y);
}

void
CQChartsPlot::
pixelToWindow(const CQChartsGeom::Point &p, CQChartsGeom::Point &w) const
{
  pixelToWindow(p.x, p.y, w.x, w.y);
}

CQChartsGeom::Point
CQChartsPlot::
windowToPixel(const CQChartsGeom::Point &w) const
{
  CQChartsGeom::Point p;

  windowToPixel(w, p);

  return p;
}

CQChartsGeom::Point
CQChartsPlot::
windowToView(const CQChartsGeom::Point &w) const
{
  double vx, vy;

  windowToView(w.x, w.y, vx, vy);

  return CQChartsGeom::Point(vx, vy);
}

CQChartsGeom::Point
CQChartsPlot::
pixelToWindow(const CQChartsGeom::Point &w) const
{
  CQChartsGeom::Point p;

  pixelToWindow(w, p);

  return p;
}

QPointF
CQChartsPlot::
windowToPixel(const QPointF &w) const
{
  return CQChartsUtil::toQPoint(windowToPixel(CQChartsUtil::fromQPoint(w)));
}

QPointF
CQChartsPlot::
pixelToWindow(const QPointF &w) const
{
  return CQChartsUtil::toQPoint(pixelToWindow(CQChartsUtil::fromQPoint(w)));
}

void
CQChartsPlot::
windowToPixel(const CQChartsGeom::BBox &wrect, CQChartsGeom::BBox &prect) const
{
  prect = windowToPixel(wrect);
}

void
CQChartsPlot::
pixelToWindow(const CQChartsGeom::BBox &prect, CQChartsGeom::BBox &wrect) const
{
  wrect = pixelToWindow(prect);
}

CQChartsGeom::BBox
CQChartsPlot::
windowToPixel(const CQChartsGeom::BBox &wrect) const
{
  double px1, py1, px2, py2;

  windowToPixel(wrect.getXMin(), wrect.getYMin(), px1, py2);
  windowToPixel(wrect.getXMax(), wrect.getYMax(), px2, py1);

  return CQChartsGeom::BBox(px1, py1, px2, py2);
}

CQChartsGeom::BBox
CQChartsPlot::
pixelToWindow(const CQChartsGeom::BBox &wrect) const
{
  double px1, py1, px2, py2;

  pixelToWindow(wrect.getXMin(), wrect.getYMin(), px1, py2);
  pixelToWindow(wrect.getXMax(), wrect.getYMax(), px2, py1);

  return CQChartsGeom::BBox(px1, py1, px2, py2);
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
  return -CMathUtil::sign(wh)*pixelToWindowHeight(wh);
}

double
CQChartsPlot::
pixelToWindowSize(double ps, bool horizontal) const
{
  return (horizontal ? pixelToWindowWidth(ps) : pixelToWindowHeight(ps));
}

double
CQChartsPlot::
pixelToWindowWidth(double pw) const
{
  double wx1, wy1, wx2, wy2;

  pixelToWindow( 0, 0, wx1, wy1);
  pixelToWindow(pw, 0, wx2, wy2);

  return std::abs(wx2 - wx1);
}

double
CQChartsPlot::
pixelToWindowHeight(double ph) const
{
  double wx1, wy1, wx2, wy2;

  pixelToWindow(0, 0 , wx1, wy1);
  pixelToWindow(0, ph, wx2, wy2);

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

  windowToPixel( 0, 0, px1, py1);
  windowToPixel(ww, 0, px2, py2);

  return std::abs(px2 - px1);
}

double
CQChartsPlot::
windowToPixelHeight(double wh) const
{
  double px1, py1, px2, py2;

  windowToPixel(0, 0 , px1, py1);
  windowToPixel(0, wh, px2, py2);

  return std::abs(py2 - py1);
}

//------

void
CQChartsPlot::
plotSymbolSize(const CQChartsLength &s, double &sx, double &sy) const
{
  sx = lengthPlotWidth (s);
  sy = lengthPlotHeight(s);
}

void
CQChartsPlot::
pixelSymbolSize(const CQChartsLength &s, double &sx, double &sy) const
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
setParameter(CQChartsPlotParameter *param, const QVariant &value)
{
  return CQUtil::setProperty(this, param->propName(), value);
}

bool
CQChartsPlot::
getParameter(CQChartsPlotParameter *param, QVariant &value) const
{
  return CQUtil::getProperty(this, param->propName(), value);
}

void
CQChartsPlot::
write(std::ostream &os) const
{
  CQChartsModelData *modelData = getModelData();

  os << "create_plot -model " << modelData->ind() << " -type " << type_->name().toStdString();

  QString columnsStr, parametersStr;

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

    if (param->type() == CQChartsPlotParameter::Type::COLUMN ||
        param->type() == CQChartsPlotParameter::Type::COLUMN_LIST) {
      if (columnsStr.length())
        columnsStr += ",";

      columnsStr += param->name() + "=" + str;
    }
    else {
      if (parametersStr.length())
        parametersStr += ",";

      parametersStr += param->name() + "=" + str;
    }
  }

  if (columnsStr.length())
    os << " \\\n  -columns \"" << columnsStr.toStdString() << "\"";

  if (parametersStr.length())
    os << " \\\n  -parameters \"" << parametersStr.toStdString() << "\"";

  CQPropertyViewModel::NameValues nameValues;

  view()->propertyModel()->getChangedNameValues(this, nameValues);

  QString propertiesStr;

  for (const auto &nv : nameValues) {
    QString str;

    if (! CQChartsVariant::toString(nv.second, str))
      str = "";

    if (propertiesStr.length())
      propertiesStr += ",";

    propertiesStr += nv.first + "=" + str;
  }

  if (propertiesStr.length())
    os << " \\\n  -properties \"" << propertiesStr.toStdString() << "\"";

  os << "\n";
}

//------

CQChartsPlot::ModelVisitor::
ModelVisitor()
{
}

CQChartsPlot::ModelVisitor::
~ModelVisitor()
{
  delete expr_;
}

void
CQChartsPlot::ModelVisitor::
init()
{
  assert(plot_);

  if (plot_->filterStr().length()) {
    expr_ = new CQChartsModelExprMatch;

    expr_->setModel(plot_->model().data());

    expr_->initMatch(plot_->filterStr());

    expr_->initColumns();
  }
}

CQChartsPlot::ModelVisitor::State
CQChartsPlot::ModelVisitor::
preVisit(const QAbstractItemModel *model, const VisitData &data)
{
  int vrow = vrow_++;

  //---

  if (expr_) {
    bool ok;

    QModelIndex ind = model->index(data.row, 0, data.parent);

    if (! expr_->match(ind, ok))
      return State::SKIP;
  }

  //---

  if (plot_->isEveryEnabled()) {
    int start = plot_->everyStart();
    int end   = plot_->everyEnd();

    if (vrow < start || vrow > end)
      return State::SKIP;

    int step = plot_->everyStep();

    if (step > 1) {
      int n = (vrow - start) % step;

      if (n != 0)
        return State::SKIP;
    }
  }

  //---

  if (plot_->visibleColumn().isValid()) {
    bool ok;

    QVariant value = plot_->modelValue(data.row, plot_->visibleColumn(), data.parent, ok);

    if (ok && ! CQChartsVariant::toBool(value, ok))
      return State::SKIP;
  }

  return State::OK;
}
