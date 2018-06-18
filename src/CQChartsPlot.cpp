#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsTitle.h>
#include <CQChartsPlotObj.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsBoxObj.h>
#include <CQChartsPlotObjTree.h>
#include <CQChartsNoDataObj.h>
#include <CQChartsAnnotation.h>
#include <CQChartsColorSet.h>
#include <CQChartsDisplayTransform.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsRotatedText.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQChartsGradientPalette.h>

#include <CHRTimer.h>

#include <QItemSelectionModel>
#include <QSortFilterProxyModel>
#include <QPainter>

//------

CQChartsPlot::
CQChartsPlot(CQChartsView *view, CQChartsPlotType *type, const ModelP &model) :
 view_(view), type_(type), model_(model), editHandles_(view)
{
  displayRange_     = new CQChartsDisplayRange();
  displayTransform_ = new CQChartsDisplayTransform(displayRange_);

  displayRange_->setPixelAdjust(0.0);

  borderObj_     = new CQChartsBoxObj(this);
  dataBorderObj_ = new CQChartsBoxObj(this);

  borderObj_    ->setObjectName("border");
  dataBorderObj_->setObjectName("dataBorder");

  borderObj_    ->setBorder(false);
  dataBorderObj_->setBorder(false);

  plotObjTree_ = new CQChartsPlotObjTree(this);

  //--

  setBackground    (true);
  setDataBackground(true);

  setDataClip(true);

  CQChartsColor plotThemeBg(CQChartsColor::Type::INTERFACE_VALUE, 0.0);
  CQChartsColor dataThemeBg(CQChartsColor::Type::INTERFACE_VALUE, 0.1);

  borderObj_    ->setBackgroundColor(plotThemeBg);
  dataBorderObj_->setBackgroundColor(dataThemeBg);

  //--

  double vr = CQChartsView::viewportRange();

  bbox_ = CQChartsGeom::BBox(0, 0, vr, vr);

  displayRange_->setPixelRange(0, vr, vr, 0);

  displayRange_->setWindowRange(0, 0, 1, 1);

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

  for (auto &annotation : annotations())
    delete annotation;

  deleteValueSets();

  delete plotObjTree_;

  delete displayRange_;
  delete displayTransform_;

  delete borderObj_;
  delete dataBorderObj_;

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

  CQChartsModelData *modelData = charts()->getModelData(model_.data());

  if (modelData)
    modelData->setName(this->id());

  connect(model_.data(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
          this, SLOT(modelDataChangedSlot(const QModelIndex &, const QModelIndex &)));
  connect(model_.data(), SIGNAL(layoutChanged()),
          this, SLOT(modelLayoutChangedSlot()));
  connect(model_.data(), SIGNAL(modelReset()),
          this, SLOT(modelResetSlot()));

  connect(model_.data(), SIGNAL(rowsInserted(QModelIndex,int,int)),
          this, SLOT(modelRowsInsertedSlot()));
  connect(model_.data(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
          this, SLOT(modelRowsRemovedSlot()));
  connect(model_.data(), SIGNAL(columnsInserted(QModelIndex,int,int)),
          this, SLOT(modelColumnsInsertedSlot()));
  connect(model_.data(), SIGNAL(columnsRemoved(QModelIndex,int,int)),
          this, SLOT(modelColumnsRemovedSlot()));
}

void
CQChartsPlot::
disconnectModel()
{
  if (! model_.data())
    return;

  CQChartsModelData *modelData = charts()->getModelData(model_.data());

  if (modelData)
    modelData->setName("");

  disconnect(model_.data(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
             this, SLOT(modelDataChangedSlot(const QModelIndex &, const QModelIndex &)));
  disconnect(model_.data(), SIGNAL(layoutChanged()),
             this, SLOT(modelLayoutChangedSlot()));
  disconnect(model_.data(), SIGNAL(modelReset()),
             this, SLOT(modelResetSlot()));

  disconnect(model_.data(), SIGNAL(rowsInserted(QModelIndex,int,int)),
             this, SLOT(modelRowsInsertedSlot()));
  disconnect(model_.data(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
             this, SLOT(modelRowsRemovedSlot()));
  disconnect(model_.data(), SIGNAL(columnsInserted(QModelIndex,int,int)),
             this, SLOT(modelColumnsInsertedSlot()));
  disconnect(model_.data(), SIGNAL(columnsRemoved(QModelIndex,int,int)),
             this, SLOT(modelColumnsRemovedSlot()));
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
modelDataChangedSlot(const QModelIndex & /*tl*/, const QModelIndex & /*br*/)
{
  // TODO: check if model uses changed columns
  //int column1 = tl.column();
  //int column2 = br.column();

  updateRangeAndObjs();
}

void
CQChartsPlot::
modelLayoutChangedSlot()
{
  updateRangeAndObjs();
}

void
CQChartsPlot::
modelResetSlot()
{
  updateRangeAndObjs();
}

void
CQChartsPlot::
modelRowsInsertedSlot()
{
  updateRangeAndObjs();
}

void
CQChartsPlot::
modelRowsRemovedSlot()
{
  updateRangeAndObjs();
}

void
CQChartsPlot::
modelColumnsInsertedSlot()
{
  updateRangeAndObjs();
}

void
CQChartsPlot::
modelColumnsRemovedSlot()
{
  updateRangeAndObjs();
}

void
CQChartsPlot::
updateTimerSlot()
{
  updateRangeAndObjsInternal();
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

  // deselect all objects
  for (auto &plotObj : plotObjs_)
    plotObj->setSelected(false);

  // select objects with matching indices
  for (int i = 0; i < indices.size(); ++i) {
    const QModelIndex &ind = indices[i];

    QModelIndex ind1 = normalizeIndex(ind);

    for (auto &plotObj : plotObjs_) {
      if (plotObj->isSelectIndex(ind1))
        plotObj->setSelected(true);
    }
  }

  view()->updateSelText();

  update();
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

void
CQChartsPlot::
setDataRange(const CQChartsGeom::Range &r, bool update)
{
  if (r != dataRange_) {
    dataRange_ = r;

    if (update)
      updateObjs();
  }
}

//---

void
CQChartsPlot::
setTitleStr(const QString &s)
{
  titleStr_ = s;

  if (title())
    title()->setTextStr(titleStr_);
}

//---

bool
CQChartsPlot::
isBackground() const
{
  return borderObj_->isBackground();
}

void
CQChartsPlot::
setBackground(bool b)
{
  borderObj_->setBackground(b); update();
}

QColor
CQChartsPlot::
interpBackgroundColor(int i, int n) const
{
  return borderObj_->interpBackgroundColor(i, n);
}

const CQChartsColor &
CQChartsPlot::
backgroundColor() const
{
  return borderObj_->backgroundColor();
}

void
CQChartsPlot::
setBackgroundColor(const CQChartsColor &c)
{
  borderObj_->setBackgroundColor(c);
}

bool
CQChartsPlot::
isBorder() const
{
  return borderObj_->isBorder();
}

void
CQChartsPlot::
setBorder(bool b)
{
  borderObj_->setBorder(b); update();
}

QColor
CQChartsPlot::
interpBorderColor(int i, int n) const
{
  return borderObj_->interpBorderColor(i, n);
}

const CQChartsColor &
CQChartsPlot::
borderColor() const
{
  return borderObj_->borderColor();
}

void
CQChartsPlot::
setBorderColor(const CQChartsColor &c)
{
  borderObj_->setBorderColor(c);
}

const CQChartsLength &
CQChartsPlot::
borderWidth() const
{
  return borderObj_->borderWidth();
}

void
CQChartsPlot::
setBorderWidth(const CQChartsLength &l)
{
  borderObj_->setBorderWidth(l); update();
}

const QString &
CQChartsPlot::
borderSides() const
{
  return borderObj_->borderSides();
}

void
CQChartsPlot::
setBorderSides(const QString &s)
{
  borderObj_->setBorderSides(s); update();
}

//---

bool
CQChartsPlot::
isDataBackground() const
{
  return dataBorderObj_->isBackground();
}

void
CQChartsPlot::
setDataBackground(bool b)
{
  dataBorderObj_->setBackground(b); update();
}

QColor
CQChartsPlot::
interpDataBackgroundColor(int i, int n) const
{
  return dataBorderObj_->interpBackgroundColor(i, n);
}

const CQChartsColor &
CQChartsPlot::
dataBackgroundColor() const
{
  return dataBorderObj_->backgroundColor();
}

void
CQChartsPlot::
setDataBackgroundColor(const CQChartsColor &c)
{
  dataBorderObj_->setBackgroundColor(c);
}

bool
CQChartsPlot::
isDataBorder() const
{
  return dataBorderObj_->isBorder();
}

void
CQChartsPlot::
setDataBorder(bool b)
{
  dataBorderObj_->setBorder(b); update();
}

QColor
CQChartsPlot::
interpDataBorderColor(int i, int n) const
{
  return dataBorderObj_->interpBorderColor(i, n);
}

const CQChartsColor &
CQChartsPlot::
dataBorderColor() const
{
  return dataBorderObj_->borderColor();
}

void
CQChartsPlot::
setDataBorderColor(const CQChartsColor &c)
{
  dataBorderObj_->setBorderColor(c);
}

const CQChartsLength &
CQChartsPlot::
dataBorderWidth() const
{
  return dataBorderObj_->borderWidth();
}

void
CQChartsPlot::
setDataBorderWidth(const CQChartsLength &l)
{
  dataBorderObj_->setBorderWidth(l); update();
}

const QString &
CQChartsPlot::
dataBorderSides() const
{
  return dataBorderObj_->borderSides();
}

void
CQChartsPlot::
setDataBorderSides(const QString &s)
{
  dataBorderObj_->setBorderSides(s); update();
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
  equalScale_ = b;

  setDataRange(CQChartsGeom::Range());

  //updateMargin();
}

void
CQChartsPlot::
setBBox(const CQChartsGeom::BBox &bbox)
{
  bbox_ = bbox;

  updateMargin();
}

void
CQChartsPlot::
updateMargin()
{
  double xml = bbox_.getWidth ()*marginLeft  ()/100.0;
  double ymt = bbox_.getHeight()*marginTop   ()/100.0;
  double xmr = bbox_.getWidth ()*marginRight ()/100.0;
  double ymb = bbox_.getHeight()*marginBottom()/100.0;

  displayRange_->setPixelRange(bbox_.getXMin() + xml, bbox_.getYMax() - ymt,
                               bbox_.getXMax() - xmr, bbox_.getYMin() + ymb);

  updateKeyPosition(/*force*/true);

  update();
}

QRectF
CQChartsPlot::
rect() const
{
  return CQChartsUtil::toQRect(bbox());
}

void
CQChartsPlot::
setRect(const QRectF &r)
{
  setBBox(CQChartsUtil::fromQRect(r));
}

QRectF
CQChartsPlot::
range() const
{
  CQChartsGeom::BBox dataRange = calcDataRange();

  return CQChartsUtil::toQRect(dataRange);
}

void
CQChartsPlot::
setRange(const QRectF &r)
{
  if (view_->isZoomData()) {
    assert(dataScaleX() == 1.0 && dataScaleY() == 1.0);
  }

  CQChartsGeom::BBox bbox = CQChartsUtil::fromQRect(r);

  CQChartsGeom::Range range(bbox.getXMin(), bbox.getYMin(), bbox.getXMax(), bbox.getYMax());

  setDataRange(range);

  applyDataRange();
}

double
CQChartsPlot::
aspect() const
{
  double px1, py1, px2, py2;

  view_->windowToPixel(bbox_.getXMin(), bbox_.getYMin(), px1, py1);
  view_->windowToPixel(bbox_.getXMax(), bbox_.getYMax(), px2, py2);

  if (py1 == py2)
    return 1.0;

  return fabs(px2 - px1)/fabs(py2 - py1);
}

//------

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

    invertX_ = b;

    CQChartsPlot *plot1 = nextPlot();

    while (plot1) {
      plot1->invertX_ = b;

      plot1 = plot1->nextPlot();
    }
  }
  else {
    invertX_ = b;
  }

  update();
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

    invertY_ = b;

    CQChartsPlot *plot1 = nextPlot();

    while (plot1) {
      plot1->invertY_ = b;

      plot1 = plot1->nextPlot();
    }
  }
  else {
    invertY_ = b;

    update();
  }
}

void
CQChartsPlot::
setLogX(bool b)
{
  logX_ = b;

  if (xAxis())
    xAxis()->setLog(b);

  updateRangeAndObjs();
}

void
CQChartsPlot::
setLogY(bool b)
{
  logY_ = b;

  if (yAxis())
    yAxis()->setLog(b);

  updateRangeAndObjs();
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
  addProperty("", this, "viewId");
  addProperty("", this, "visible");

  addProperty("columns", this, "idColumn", "id");

  addProperty("range", this, "rect"   , "view"   );
  addProperty("range", this, "range"  , "data"   );
  addProperty("range", this, "autoFit", "autoFit");

  addProperty("scaling", this, "dataScaleX", "dataX");
  addProperty("scaling", this, "dataScaleY", "dataY");
  addProperty("scaling", this, "equalScale", "equal");

  addProperty("grouping", this, "overlay");
  addProperty("grouping", this, "x1x2"   );
  addProperty("grouping", this, "y1y2"   );

  addProperty("invert", this, "invertX", "x");
  addProperty("invert", this, "invertY", "y");

  addProperty("log", this, "logX", "x");
  addProperty("log", this, "logY", "y");

  if (CQChartsUtil::getBoolEnv("CQCHARTS_DEBUG", true)) {
    addProperty("debug", this, "showBoxes");
    addProperty("debug", this, "followMouse");
  }

  QString plotStyleStr       = "plotStyle";
  QString plotStyleFillStr   = plotStyleStr + "/fill";
  QString plotStyleStrokeStr = plotStyleStr + "/stroke";

  addProperty(plotStyleFillStr  , this, "background"     , "visible");
  addProperty(plotStyleFillStr  , this, "backgroundColor", "color");
  addProperty(plotStyleStrokeStr, this, "border"         , "visible");
  addProperty(plotStyleStrokeStr, this, "borderColor"    , "color");
  addProperty(plotStyleStrokeStr, this, "borderWidth"    , "width");
  addProperty(plotStyleStrokeStr, this, "borderSides"    , "sides");
  addProperty(plotStyleStr      , this, "clip"           , "clip");

  QString dataStyleStr       = "dataStyle";
  QString dataStyleFillStr   = dataStyleStr + "/fill";
  QString dataStyleStrokeStr = dataStyleStr + "/stroke";

  addProperty(dataStyleFillStr  , this, "dataBackground"     , "visible");
  addProperty(dataStyleFillStr  , this, "dataBackgroundColor", "color");
  addProperty(dataStyleStrokeStr, this, "dataBorder"         , "visible");
  addProperty(dataStyleStrokeStr, this, "dataBorderColor"    , "color");
  addProperty(dataStyleStrokeStr, this, "dataBorderWidth"    , "width");
  addProperty(dataStyleStrokeStr, this, "dataBorderSides"    , "sides");
  addProperty(dataStyleStr      , this, "dataClip"           , "clip");

  addProperty("margin", this, "marginLeft"  , "left"  );
  addProperty("margin", this, "marginTop"   , "top"   );
  addProperty("margin", this, "marginRight" , "right" );
  addProperty("margin", this, "marginBottom", "bottom");

  addProperty("every", this, "everyEnabled", "enabled");
  addProperty("every", this, "everyStart"  , "start"  );
  addProperty("every", this, "everyEnd"    , "end"    );
  addProperty("every", this, "everyStep"   , "step"   );

  if (xAxis())
    xAxis()->addProperties(propertyModel(), id() + "/" + "xaxis");

  if (yAxis())
    yAxis()->addProperties(propertyModel(), id() + "/" + "yaxis");

  if (key())
    key()->addProperties(propertyModel(), id() + "/" + "key");

  if (title())
    title()->addProperties(propertyModel(), id() + "/" + "title");

  addProperty("scaledFont", this, "minScaleFontSize", "minSize");
  addProperty("scaledFont", this, "maxScaleFontSize", "maxSize");
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
getProperty(const QString &name, QVariant &value)
{
  return propertyModel()->getProperty(this, name, value);
}

void
CQChartsPlot::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  QString path1 = id();

  if (path.length())
    path1 += "/" + path;

  view_->addProperty(path1, object, name, alias);
}

void
CQChartsPlot::
propertyItemSelected(QObject *obj, const QString &)
{
  view()->deselectAll();

  if      (obj == this) {
    setSelected(true);

    view()->setCurrentPlot(this);

    update();
  }
  else if (obj == titleObj_) {
    titleObj_->setSelected(true);

    update();
  }
  else if (obj == keyObj_) {
    keyObj_->setSelected(true);

    update();
  }
  else if (obj == xAxis_) {
    xAxis_->setSelected(true);

    update();
  }
  else if (obj == yAxis_) {
    yAxis_->setSelected(true);

    update();
  }
  else {
    for (const auto &annotation : annotations()) {
      if (obj == annotation) {
        annotation->setSelected(true);

        update();
      }
    }

    update();
  }
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
  xAxis_ = new CQChartsAxis(this, CQChartsAxis::Direction::HORIZONTAL, 0, 1);

  xAxis_->setObjectName("xaxis");
}

void
CQChartsPlot::
addYAxis()
{
  yAxis_ = new CQChartsAxis(this, CQChartsAxis::Direction::VERTICAL, 0, 1);

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

    key()->clearItems();

    addKeyItems(key());

    CQChartsPlot *plot1 = nextPlot();

    while (plot1) {
      plot1->addKeyItems(key());

      plot1 = plot1->nextPlot();
    }
  }
  else {
    key()->clearItems();

    addKeyItems(key());
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
  dataRange_.reset();

  clearPlotObjects();
}

void
CQChartsPlot::
updateRangeAndObjs()
{
  updateTimer_->start(100);
}

void
CQChartsPlot::
updateRangeAndObjsInternal()
{
  CScopeTimer timer("updateRangeAndObjsInternal");

  updateRange();

  updateObjs();
}

void
CQChartsPlot::
updateObjs()
{
  CScopeTimer timer("updateObjs");

  clearPlotObjects();

  update();
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
  CQChartsGeom::BBox bbox;

  if (dataRange_.isSet())
    bbox = CQChartsGeom::BBox(dataRange_.xmin(), dataRange_.ymin(),
                              dataRange_.xmax(), dataRange_.ymax());
  else
    bbox = CQChartsGeom::BBox(0, 0, 1, 1);

  // adjust by zoom data pan offset, zoom scale
  if (view_->isZoomData() && adjust) {
    CQChartsGeom::Point c = bbox.getCenter();
    double              w = 0.5*bbox.getWidth ()/dataScaleX();
    double              h = 0.5*bbox.getHeight()/dataScaleY();

    double x = c.x + dataOffset().x;
    double y = c.y + dataOffset().y;

    bbox = CQChartsGeom::BBox(x - w, y - h, x + w, y + h);
  }

  return bbox;
}

void
CQChartsPlot::
applyDataRange(bool propagate)
{
  CQChartsGeom::BBox dataRange;

  if (propagate) {
    if      (isX1X2()) {
      CQChartsPlot *plot1 = firstPlot();

      dataRange = plot1->calcDataRange();
    }
    else if (isY1Y2()) {
      CQChartsPlot *plot1 = firstPlot();

      dataRange = plot1->calcDataRange();
    }
    else if (isOverlay()) {
      CQChartsPlot *plot1 = firstPlot();

      while (plot1) {
        plot1->setDataRange(CQChartsGeom::Range(), /*update*/false);

        plot1->updateRange(/*update*/false);

        CQChartsGeom::BBox dataRange1 = plot1->calcDataRange(/*adjust*/false);

        dataRange += dataRange1;

        plot1 = plot1->nextPlot();
      }
    }
    else
      dataRange = calcDataRange();
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
    CQChartsPlot *plot1 = firstPlot();

    if      (isX1X2()) {
      CQChartsGeom::Range dataRange1 =
        CQChartsGeom::Range(dataRange.getXMin(), dataRange.getYMin(),
                            dataRange.getXMax(), dataRange.getYMax());

      plot1->setDataScaleX(dataScaleX());
      plot1->setDataScaleY(dataScaleY());
      plot1->setDataOffset(dataOffset());

      plot1->applyDataRange(/*propagate*/false);

      //---

      CQChartsPlot *plot2 = plot1->nextPlot();

      if (plot2) {
        plot2->setDataRange(CQChartsGeom::Range(), /*update*/false);

        plot2->updateRange(/*update*/false);

        CQChartsGeom::BBox bbox2 = plot2->calcDataRange(/*adjust*/false);

        CQChartsGeom::Range dataRange2 =
          CQChartsGeom::Range(bbox2.getXMin(), dataRange1.bottom(),
                              bbox2.getXMax(), dataRange1.top   ());

        plot2->setDataRange (dataRange2, /*update*/false);
        plot2->setDataScaleX(dataScaleX());
        plot2->setDataScaleY(dataScaleY());
        plot2->setDataOffset(dataOffset());

        plot2->applyDataRange(/*propagate*/false);
      }
    }
    else if (isY1Y2()) {
      CQChartsGeom::Range dataRange1 =
        CQChartsGeom::Range(dataRange.getXMin(), dataRange.getYMin(),
                            dataRange.getXMax(), dataRange.getYMax());

      plot1->setDataScaleX(dataScaleX());
      plot1->setDataScaleY(dataScaleY());
      plot1->setDataOffset(dataOffset());

      plot1->applyDataRange(/*propagate*/false);

      //---

      CQChartsPlot *plot2 = plot1->nextPlot();

      if (plot2) {
        plot2->setDataRange(CQChartsGeom::Range(), /*update*/false);

        plot2->updateRange(/*update*/false);

        CQChartsGeom::BBox bbox2 = plot2->calcDataRange(/*adjust*/false);

        CQChartsGeom::Range dataRange2 =
          CQChartsGeom::Range(dataRange1.left (), bbox2.getYMin(),
                              dataRange1.right(), bbox2.getYMax());

        plot2->setDataRange (dataRange2, /*update*/false);
        plot2->setDataScaleX(dataScaleX());
        plot2->setDataScaleY(dataScaleY());
        plot2->setDataOffset(dataOffset());

        plot2->applyDataRange(/*propagate*/false);
      }
    }
    else if (isOverlay()) {
      CQChartsGeom::Range dataRange1 =
        CQChartsGeom::Range(dataRange.getXMin(), dataRange.getYMin(),
                            dataRange.getXMax(), dataRange.getYMax());

      if (plot1) {
        //plot1->setDataRange (dataRange1);
        //plot1->setDataScaleX(dataScaleX());
        //plot1->setDataScaleY(dataScaleY());
        //plot1->setDataOffset(dataOffset());

        //plot1->applyDataRange(/*propagate*/false);

        while (plot1) {
          plot1->setDataRange (dataRange1, /*update*/false);
          plot1->setDataScaleX(dataScaleX());
          plot1->setDataScaleY(dataScaleY());
          plot1->setDataOffset(dataOffset());

          plot1->applyDataRange(/*propagate*/false);

          plot1 = plot1->nextPlot();
        }
      }
    }
    else {
      if (plot1) {
        while (plot1) {
          if (plot1 != this) {
            plot1->setDataScaleX(dataScaleX());
            plot1->setDataScaleY(dataScaleY());
            plot1->setDataOffset(dataOffset());

            plot1->applyDataRange(/*propagate*/false);
          }

          plot1 = plot1->nextPlot();
        }
      }
    }
  }

  updateKeyPosition(/*force*/true);
}

void
CQChartsPlot::
applyDisplayTransform(bool propagate)
{
  if (propagate) {
    if (isOverlay()) {
      CQChartsPlot *plot1 = firstPlot();

      if (plot1) {
        plot1->setDisplayTransform(*displayTransform_);

        plot1->applyDisplayTransform(/*propagate*/false);

        while (plot1) {
          plot1->setDisplayTransform(*displayTransform_);

          plot1->applyDisplayTransform(/*propagate*/false);

          plot1 = plot1->nextPlot();
        }
      }
    }
  }
}

void
CQChartsPlot::
adjustDataRange()
{
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
initPlotObjs()
{
  bool changed = initObjs();

  if (! dataRange_.isSet()) {
    dataRange_.updateRange(0, 0);
    dataRange_.updateRange(1, 1);

    changed = true;
  }

  noData_ = false;

  if (plotObjs_.empty()) {
    CQChartsNoDataObj *obj = new CQChartsNoDataObj(this);

    addPlotObject(obj);

    noData_ = true;

    changed = true;
  }

  if (changed)
    initObjTree();

  if (changed && isAutoFit()) {
    needsAutoFit_ = true;
  }
}

void
CQChartsPlot::
initObjTree()
{
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

  insidePlotObjs_    .clear();
  sizeInsidePlotObjs_.clear();
}

bool
CQChartsPlot::
updatePlotObjects(const CQChartsGeom::Point &w)
{
  PlotObjs objs;

  objsAtPoint(w, objs);

  bool changed = false;

  if (objs.size() == insidePlotObjs_.size()) {
    for (const auto &obj : objs) {
      if (insidePlotObjs_.find(obj) == insidePlotObjs_.end()) {
        changed = true;
        break;
      }
    }
  }
  else {
    changed = true;
  }

  if (changed) {
    insidePlotInd_ = 0;

    insidePlotObjs_    .clear();
    sizeInsidePlotObjs_.clear();

    for (const auto &obj : plotObjs_)
      obj->setInside(false);

    for (const auto &obj : objs) {
      insidePlotObjs_.insert(obj);

      sizeInsidePlotObjs_[obj->rect().area()].insert(obj);
    }

    setInsidePlotObject();
  }

  return changed;
}

CQChartsPlotObj *
CQChartsPlot::
insidePlotObject() const
{
  int i = 0;

  for (const auto &sizeObjs : sizeInsidePlotObjs_) {
    for (const auto &obj : sizeObjs.second) {
      if (i == insidePlotInd_)
        return obj;

      ++i;
    }
  }

  return nullptr;
}

void
CQChartsPlot::
nextInsidePlotInd()
{
  ++insidePlotInd_;

  if (insidePlotInd_ >= int(insidePlotObjs_.size()))
    insidePlotInd_ = 0;
}

void
CQChartsPlot::
prevInsidePlotInd()
{
  --insidePlotInd_;

  if (insidePlotInd_ < 0)
    insidePlotInd_ = insidePlotObjs_.size() - 1;
}

void
CQChartsPlot::
setInsidePlotObject()
{
  CQChartsPlotObj *insideObj = insidePlotObject();

  for (auto &obj : insidePlotObjs_)
    obj->setInside(obj == insideObj);
}

QString
CQChartsPlot::
insidePlotObjectText() const
{
  QString objText;

  for (const auto &sizeObjs : sizeInsidePlotObjs_) {
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
selectMousePress(const QPointF &p, ModSelect modSelect)
{
  CQChartsGeom::Point w;

  pixelToWindow(CQChartsUtil::fromQPoint(QPointF(p)), w);

  return selectPress(w, modSelect);
}

bool
CQChartsPlot::
selectPress(const CQChartsGeom::Point &w, ModSelect modSelect)
{
  if (key() && key()->contains(w)) {
    CQChartsKeyItem *item = key()->getItemAt(w);

    if (item) {
      bool handled = item->selectPress(w);

      if (handled) {
        emit keyItemPressed  (item);
        emit keyItemIdPressed(item->id());

        return true;
      }
    }

    bool handled = key()->selectPress(w);

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
        emit annotationPressed  (annotation);
        emit annotationIdPressed(annotation->id());

        return true;
      }
    }
  }

  //---

  // for replace init all objects to unselected
  // for add/remove/toggle init all objects to current state
  using ObjsSelected = std::map<CQChartsPlotObj*,bool>;

  ObjsSelected objsSelected;

  for (auto &plotObj : plotObjs_) {
    if (modSelect == ModSelect::REPLACE)
      objsSelected[plotObj] = false;
    else
      objsSelected[plotObj] = plotObj->isSelected();
  }

  //---

  // get object under mouse
  CQChartsPlotObj *selectObj = nullptr;

  if (isFollowMouse()) {
    selectObj = insidePlotObject();

    nextInsidePlotInd();

    setInsidePlotObject();
  }
  else {
    PlotObjs objs;

    objsAtPoint(w, objs);

    if (! objs.empty())
      selectObj = *objs.begin();
  }

  //---

  // change selection depending on selection modifier
  if (selectObj) {
    if      (modSelect == ModSelect::TOGGLE)
      objsSelected[selectObj] = ! selectObj->isSelected();
    else if (modSelect == ModSelect::REPLACE)
      objsSelected[selectObj] = true;
    else if (modSelect == ModSelect::ADD)
      objsSelected[selectObj] = true;
    else if (modSelect == ModSelect::REMOVE)
      objsSelected[selectObj] = false;

    //---

    //selectObj->selectPress();

    emit objPressed  (selectObj);
    emit objIdPressed(selectObj->id());

    // potential crash if signals cause objects to be deleted (defer !!!)
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
      if (objSelected.first->isSelected())
        objSelected.first->addSelectIndices();
    }

    endSelect();

    update();
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

    objText = insidePlotObjectText();

    if (changed)
      update();
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
    CQChartsResizeHandle::Side side = editHandles_.inside(v);

    if (side != CQChartsResizeHandle::Side::NONE) {
      mouseData_.dragObj = DragObj::PLOT_HANDLE;

      editHandles_.setDragSide(side);
      editHandles_.setDragPos (w);

      return true;
    }
  }

  // start drag on already selected key handle
  if (key() && key()->isSelected()) {
    CQChartsResizeHandle::Side side = key()->editHandles().inside(w);

    if (side != CQChartsResizeHandle::Side::NONE) {
      mouseData_.dragObj = DragObj::KEY;

      key()->editPress(w);

      key()->editHandles().setDragSide(side);
      key()->editHandles().setDragPos (w);

      return true;
    }
  }

  // start drag on already selected x axis handle
  if (xAxis() && xAxis()->isSelected()) {
    CQChartsResizeHandle::Side side = xAxis()->editHandles().inside(w);

    if (side != CQChartsResizeHandle::Side::NONE) {
      mouseData_.dragObj = DragObj::XAXIS;

      xAxis()->editPress(w);

      xAxis()->editHandles().setDragSide(side);
      xAxis()->editHandles().setDragPos (w);

      return true;
    }
  }

  // start drag on already selected y axis handle
  if (yAxis() && yAxis()->isSelected()) {
    CQChartsResizeHandle::Side side = yAxis()->editHandles().inside(w);

    if (side != CQChartsResizeHandle::Side::NONE) {
      mouseData_.dragObj = DragObj::YAXIS;

      yAxis()->editPress(w);

      yAxis()->editHandles().setDragSide(side);
      yAxis()->editHandles().setDragPos (w);

      return true;
    }
  }

  // start drag on already selected title handle
  if (title() && title()->isSelected()) {
    CQChartsResizeHandle::Side side = title()->editHandles().inside(w);

    if (side != CQChartsResizeHandle::Side::NONE) {
      mouseData_.dragObj = DragObj::TITLE;

      title()->editPress(w);

      title()->editHandles().setDragSide(side);
      title()->editHandles().setDragPos (w);

      return true;
    }
  }

  // start drag on already selected annotation handle
  for (const auto &annotation : annotations()) {
    if (annotation->isSelected()) {
      CQChartsResizeHandle::Side side = annotation->editHandles().inside(w);

      if (side != CQChartsResizeHandle::Side::NONE) {
        mouseData_.dragObj = DragObj::ANNOTATION;

        annotation->editHandles().setDragSide(side);
        annotation->editHandles().setDragPos (w);

        return true;
      }
    }
  }

  //---

  // select/deselect key
  if (key()) {
    if (key()->contains(w)) {
      if (! key()->isSelected()) {
        deselectAll();

        key()->setSelected(true);

        return true;
      }

      if (key()->editPress(w)) {
        mouseData_.dragObj = DragObj::KEY;
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
        deselectAll();

        xAxis()->setSelected(true);

        return true;
      }

      if (xAxis()->editPress(w)) {
        mouseData_.dragObj = DragObj::XAXIS;
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
        deselectAll();

        yAxis()->setSelected(true);

        return true;
      }

      if (yAxis()->editPress(w)) {
        mouseData_.dragObj = DragObj::YAXIS;
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
        deselectAll();

        title()->setSelected(true);

        return true;
      }

      if (title()->editPress(w)) {
        mouseData_.dragObj = DragObj::TITLE;
        return true;
      }

      return false;
    }
  }

  //---

  for (const auto &annotation : annotations()) {
    if (annotation->contains(w)) {
      if (! annotation->isSelected()) {
        deselectAll();

        annotation->setSelected(true);

        return true;
      }

      if (annotation->editPress(w)) {
        mouseData_.dragObj = DragObj::ANNOTATION;
        return true;
      }

      return false;
    }
  }

  //---

  // select/deselect plot
  // (to select point must be inside a plot object)
  PlotObjs objs;

  objsAtPoint(w, objs);

  if (! objs.empty()) {
    if (! isSelected()) {
      view()->deselectAll();

      setSelected(true);

      view()->setCurrentPlot(this);

      return true;
    }

    mouseData_.dragObj = DragObj::PLOT;
    return true;
  }

  deselectAll();

  return false;
}

void
CQChartsPlot::
deselectAll()
{
  if (key() && key()->isSelected())
    key()->setSelected(false);

  if (xAxis() && xAxis()->isSelected())
    xAxis()->setSelected(false);

  if (yAxis() && yAxis()->isSelected())
    yAxis()->setSelected(false);

  if (title() && title()->isSelected())
    title()->setSelected(false);

  for (auto &annotation : annotations())
    annotation->setSelected(false);

  if (isSelected())
    setSelected(false);
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
    return true;
  }
  else if (mouseData_.dragObj == DragObj::XAXIS) {
    (void) xAxis()->editMove(w);
    return true;
  }
  else if (mouseData_.dragObj == DragObj::YAXIS) {
    (void) yAxis()->editMove(w);
    return true;
  }
  else if (mouseData_.dragObj == DragObj::TITLE) {
    (void) title()->editMove(w);
    return true;
  }
  else if (mouseData_.dragObj == DragObj::ANNOTATION) {
    for (const auto &annotation : annotations()) {
      if (annotation->isSelected()) {
        (void) annotation->editMove(w);
        return true;
      }
    }
  }
  else if (mouseData_.dragObj == DragObj::PLOT) {
    double dx = mouseData_.movePoint.x() - lastMovePoint.x();
    double dy = lastMovePoint.y() - mouseData_.movePoint.y();

    double dx1 =  view_->pixelToSignedWindowWidth (dx);
    double dy1 = -view_->pixelToSignedWindowHeight(dy);

    bbox_.moveBy(CQChartsGeom::Point(dx1, dy1));

    updateMargin();

    return true;
  }
  else if (mouseData_.dragObj == DragObj::PLOT_HANDLE) {
    double dx = mouseData_.movePoint.x() - lastMovePoint.x();
    double dy = lastMovePoint.y() - mouseData_.movePoint.y();

    double dx1 =  view_->pixelToSignedWindowWidth (dx);
    double dy1 = -view_->pixelToSignedWindowHeight(dy);

    editHandles_.updateBBox(dx1, dy1);

    bbox_ = editHandles_.bbox();

    updateMargin();

    return true;
  }

  return false;
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

    if (editHandles_.selectInside(v))
      update();
  }
  else if (key() && key()->isSelected()) {
    if (key()->editMotion(w))
      update();
  }
  else if (xAxis() && xAxis()->isSelected()) {
    if (xAxis()->editMotion(w))
      update();
  }
  else if (yAxis() && yAxis()->isSelected()) {
    if (yAxis()->editMotion(w))
      update();
  }
  else if (title() && title()->isSelected()) {
    if (title()->editMotion(w))
      update();
  }
  else {
    for (const auto &annotation : annotations()) {
      if (annotation->isSelected()) {
        if (annotation->editMotion(w))
          update();
      }
    }
  }

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
  QRectF r = this->range();

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
        return;
      }
    }
  }
}

//------

bool
CQChartsPlot::
rectSelect(const CQChartsGeom::BBox &r, ModSelect modSelect)
{
  // for replace init all objects to unselected
  // for add/remove/toggle init all objects to current state
  using ObjsSelected = std::map<CQChartsPlotObj*,bool>;

  ObjsSelected objsSelected;

  for (auto &plotObj : plotObjs_) {
    if (modSelect == ModSelect::REPLACE)
      objsSelected[plotObj] = false;
    else
      objsSelected[plotObj] = plotObj->isSelected();
  }

  //---

  // get objects touching rectangle
  PlotObjs objs;

  objsTouchingRect(r, objs);

  // change selection depending on selection modifier
  for (auto &obj : objs) {
    if      (modSelect == ModSelect::TOGGLE)
      objsSelected[obj] = ! obj->isSelected();
    else if (modSelect == ModSelect::REPLACE)
      objsSelected[obj] = true;
    else if (modSelect == ModSelect::ADD)
      objsSelected[obj] = true;
    else if (modSelect == ModSelect::REMOVE)
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
      if (objSelected.first->isSelected())
        objSelected.first->addSelectIndices();
    }

    endSelect();

    update();
  }

  //---

  return ! objs.empty();
}

void
CQChartsPlot::
selectedObjs(PlotObjs &objs) const
{
  for (const auto &plotObj : plotObjs_) {
    if (plotObj->isSelected())
      objs.push_back(plotObj);
  }
}

//------

void
CQChartsPlot::
setXValueColumn(const CQChartsColumn &c)
{
  xValueColumn_ = c;

  if (xAxis())
    xAxis()->setColumn(xValueColumn_);
}

void
CQChartsPlot::
setYValueColumn(const CQChartsColumn &c)
{
  yValueColumn_ = c;

  if (yAxis())
    yAxis()->setColumn(yValueColumn_);
}

//------

void
CQChartsPlot::
setIdColumn(const CQChartsColumn &c)
{
  if (c != idColumn_) {
    idColumn_ = c;

    updateRangeAndObjs();
  }
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

  return columnStr(xValueColumn(), x);
}

QString
CQChartsPlot::
yStr(double y) const
{
  if (isLogY())
    y = expValue(y);

  return columnStr(yValueColumn(), y);
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

  if (! CQChartsUtil::formatColumnValue(charts(), model, column, x, str))
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
      double f = (! is_shift ? 0.125 : 0.25);

      if      (key == Qt::Key_Right)
        panLeft(f);
      else if (key == Qt::Key_Left)
        panRight(f);
      else if (key == Qt::Key_Up)
        panDown(f);
      else if (key == Qt::Key_Down)
        panUp(f);
    }
    else {
      double f = (! is_shift ? 0.025 : 0.05);

      if      (key == Qt::Key_Right)
        editMoveBy(QPointF( f, 0));
      else if (key == Qt::Key_Left)
        editMoveBy(QPointF(-f, 0));
      else if (key == Qt::Key_Up)
        editMoveBy(QPointF(0, f));
      else if (key == Qt::Key_Down)
        editMoveBy(QPointF(0, -f));
    }
  }
  else if (key == Qt::Key_Plus || key == Qt::Key_Minus) {
    double f = (! is_shift ? 1.5 : 2.0);

    if (key == Qt::Key_Plus)
      zoomIn(f);
    else
      zoomOut(f);
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

void
CQChartsPlot::
updateSlot()
{
  update();
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
  if (! insidePlotObjs_.empty()) {
    if (! prev)
      nextInsidePlotInd();
    else
      prevInsidePlotInd();

    setInsidePlotObject();

    QString objText = insidePlotObjectText();

    view_->setStatusText(objText);

    update();
  }
}

void
CQChartsPlot::
panLeft(double f)
{
  if (! allowPanX())
    return;

  if (view_->isZoomData()) {
    CQChartsGeom::BBox dataRange = calcDataRange();

    dataOffset_.setX(dataOffset_.x - f*dataRange.getWidth());

    applyDataRange();

    update();
  }
  else {
    displayTransform_->panLeft();

    updateTransform();
  }
}

void
CQChartsPlot::
panRight(double f)
{
  if (! allowPanX())
    return;

  if (view_->isZoomData()) {
    CQChartsGeom::BBox dataRange = calcDataRange();

    dataOffset_.setX(dataOffset_.x + f*dataRange.getWidth());

    applyDataRange();

    update();
  }
  else {
    displayTransform_->panRight();

    updateTransform();
  }
}

void
CQChartsPlot::
panUp(double f)
{
  if (! allowPanY())
    return;

  if (view_->isZoomData()) {
    CQChartsGeom::BBox dataRange = calcDataRange();

    dataOffset_.setY(dataOffset_.y + f*dataRange.getHeight());

    applyDataRange();

    update();
  }
  else {
    displayTransform_->panUp();

    updateTransform();
  }
}

void
CQChartsPlot::
panDown(double f)
{
  if (! allowPanY())
    return;

  if (view_->isZoomData()) {
    CQChartsGeom::BBox dataRange = calcDataRange();

    dataOffset_.setY(dataOffset_.y - f*dataRange.getHeight());

    applyDataRange();

    update();
  }
  else {
    displayTransform_->panDown();

    updateTransform();
  }
}

void
CQChartsPlot::
pan(double dx, double dy)
{
  if (view_->isZoomData()) {
    //CQChartsGeom::BBox dataRange = calcDataRange();

    if (allowPanX())
      dataOffset_.setX(dataOffset_.x + dx);

    if (allowPanY())
      dataOffset_.setY(dataOffset_.y + dy);

    applyDataRange();

    update();
  }
  else {
    // TODO

    //displayTransform_->pan(dx, dy);

    //updateTransform();
  }
}

void
CQChartsPlot::
zoomIn(double f)
{
  if (view_->isZoomData()) {
    if (allowZoomX())
      dataScaleX_ *= f;

    if (allowZoomY())
      dataScaleY_ *= f;

    applyDataRange();

    update();
  }
  else {
    displayTransform_->zoomIn(f);

    updateTransform();
  }
}

void
CQChartsPlot::
zoomOut(double f)
{
  if (view_->isZoomData()) {
    if (allowZoomX())
      dataScaleX_ /= f;

    if (allowZoomY())
      dataScaleY_ /= f;

    applyDataRange();

    update();
  }
  else {
    displayTransform_->zoomOut(f);

    updateTransform();
  }
}

void
CQChartsPlot::
zoomTo(const CQChartsGeom::BBox &bbox)
{
  CQChartsGeom::BBox bbox1 = bbox;

  double w = bbox1.getWidth ();
  double h = bbox1.getHeight();

  if (w < 1E-50 || h < 1E-50) {
    double dataScale = 2*std::min(dataScaleX_, dataScaleY_);

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

    //dataScaleX_ = std::min(xscale, yscale);
    //dataScaleY_ = std::min(xscale, yscale);

    if (allowZoomX())
      dataScaleX_ = xscale;

    if (allowZoomY())
      dataScaleY_ = yscale;

    CQChartsGeom::Point c1 = CQChartsGeom::Point(dataRange_.xmid(), dataRange_.ymid());

    double cx = (allowPanX() ? c.x - c1.x : 0.0);
    double cy = (allowPanY() ? c.y - c1.y : 0.0);

    dataOffset_ = CQChartsGeom::Point(cx, cy);

    applyDataRange();

    update();
  }
  else {
    displayTransform_->zoomTo(bbox);

    updateTransform();
  }
}

void
CQChartsPlot::
zoomFull()
{
  if (view_->isZoomData()) {
    if (allowZoomX())
      dataScaleX_ = 1.0;

    if (allowZoomY())
      dataScaleY_ = 1.0;

    dataOffset_ = CQChartsGeom::Point(0.0, 0.0);

    applyDataRange();

    update();
  }
  else {
    displayTransform_->reset();

    updateTransform();
  }
}

void
CQChartsPlot::
updateTransform()
{
  applyDisplayTransform();

  handleResize();

  update();
}

bool
CQChartsPlot::
tipText(const CQChartsGeom::Point &p, QString &tip) const
{
  int objNum  = 0;
  int numObjs = 0;

  CQChartsPlotObj *tipObj = nullptr;

  if (isFollowMouse()) {
    objNum  = insidePlotInd_;
    numObjs = insidePlotObjs_.size();

    tipObj = insidePlotObject();
  }
  else {
    PlotObjs objs;

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
objsAtPoint(const CQChartsGeom::Point &p, PlotObjs &objs) const
{
  plotObjTree_->objectsAtPoint(p, objs);
}

void
CQChartsPlot::
objsTouchingRect(const CQChartsGeom::BBox &r, PlotObjs &objs) const
{
  plotObjTree_->objectsTouchingRect(r, objs);
}

void
CQChartsPlot::
handleResize()
{
  if (isEqualScale())
    dataRange_.reset();

  for (auto &obj : plotObjs_)
    obj->handleResize();

  updateKeyPosition(/*force*/true);
}

void
CQChartsPlot::
updateKeyPosition(bool force)
{
  if (! key() || ! key()->isVisible())
    return;

  if (force)
    key()->invalidateLayout();

  CQChartsGeom::BBox bbox = calcDataRange();

  if (! bbox.isSet())
    return;

  key()->updateLocation(bbox);
}

void
CQChartsPlot::
drawBackground(QPainter *painter)
{
  if (isBackground() || isBorder()) {
    QRectF plotRect = CQChartsUtil::toQRect(calcPixelRect());

    if (isBackground())
      painter->fillRect(plotRect, QBrush(interpBackgroundColor(0, 1)));

    if (isBorder()) {
      QColor borderColor = interpBorderColor(0, 1);

      double bw = lengthPixelWidth(borderWidth());

      drawSides(painter, plotRect, borderSides(), bw, borderColor);
    }
  }

  if (isDataBackground() || isDataBorder()) {
    QRectF dataRect = CQChartsUtil::toQRect(calcDataPixelRect());

    if (isDataBackground())
      painter->fillRect(dataRect, QBrush(interpDataBackgroundColor(0, 1)));

    if (isDataBorder()) {
      QColor borderColor = interpDataBorderColor(0, 1);

      double bw = lengthPixelWidth(dataBorderWidth());

      drawSides(painter, dataRect, dataBorderSides(), bw, borderColor);
    }
  }
}

void
CQChartsPlot::
drawSides(QPainter *painter, const QRectF &rect, const QString &sides,
          double width, const QColor &color)
{
  QPen pen(color);

  pen.setWidthF(width);

  painter->setPen(pen);
  painter->setBrush(Qt::NoBrush);

  if (sides.indexOf('t') >= 0) painter->drawLine(rect.topLeft   (), rect.topRight   ());
  if (sides.indexOf('l') >= 0) painter->drawLine(rect.topLeft   (), rect.bottomLeft ());
  if (sides.indexOf('b') >= 0) painter->drawLine(rect.bottomLeft(), rect.bottomRight());
  if (sides.indexOf('r') >= 0) painter->drawLine(rect.topRight  (), rect.bottomRight());
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
calcPixelRect() const
{
  return view_->windowToPixel(bbox_);
}

void
CQChartsPlot::
autoFit()
{
  if (isOverlay()) {
    if (prevPlot())
      return;

    //---

    CQChartsGeom::BBox bbox = fitBBox();

    //---

    // combine bboxes of overlay plots
    CQChartsPlot *plot1 = nextPlot();

    while (plot1) {
      CQChartsGeom::BBox bbox1 = plot1->fitBBox();

      CQChartsGeom::BBox bbox2;

      plot1->windowToPixel(bbox1, bbox2);

      pixelToWindow(bbox2, bbox1);

      bbox += bbox1;

      plot1 = plot1->nextPlot();
    }

    //---

    // set all overlay plot bboxes
    plot1 = nextPlot();

    while (plot1) {
      CQChartsGeom::BBox bbox1;

      windowToPixel(bbox, bbox1);

      CQChartsGeom::BBox bbox2;

      plot1->pixelToWindow(bbox1, bbox2);

      plot1->setFitBBox(bbox2);

      plot1 = plot1->nextPlot();
    }

    //---

    setFitBBox(bbox);
  }
  else {
    CQChartsGeom::BBox bbox = fitBBox();

    setFitBBox(bbox);
  }
}

void
CQChartsPlot::
setFitBBox(const CQChartsGeom::BBox &bbox)
{
  // calc margin so plot box fits in specified box
  CQChartsGeom::BBox pbbox = displayRangeBBox();

  margin_.left   = 100.0*(pbbox.getXMin() -  bbox.getXMin())/bbox.getWidth ();
  margin_.bottom = 100.0*(pbbox.getYMin() -  bbox.getYMin())/bbox.getHeight();
  margin_.right  = 100.0*( bbox.getXMax() - pbbox.getXMax())/bbox.getWidth ();
  margin_.top    = 100.0*( bbox.getYMax() - pbbox.getYMax())/bbox.getHeight();

  if (isInvertX()) std::swap(margin_.left, margin_.right );
  if (isInvertY()) std::swap(margin_.top , margin_.bottom);

  updateMargin();
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
    bbox += xAxis()->bbox();

  if (yAxis() && yAxis()->isVisible())
    bbox += yAxis()->bbox();

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
keyFitBBox() const
{
  CQChartsGeom::BBox bbox;

  if (key() && key()->isVisible())
    bbox += key()->bbox();

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

//------

void
CQChartsPlot::
drawParts(QPainter *painter)
{
  drawBackground(painter);

  //---

  drawBgAxes(painter);
  drawBgKey(painter);

  drawObjs(painter, Layer::BG );
  drawObjs(painter, Layer::MID);
  drawObjs(painter, Layer::FG );

  drawFgAxes(painter);
  drawFgKey(painter);

  //---

  drawTitle(painter);

  //---

  drawForeground(painter);

  //---

  if (showBoxes()) {
    CQChartsGeom::BBox bbox = fitBBox();

    drawWindowColorBox(painter, bbox);

    drawWindowColorBox(painter, dataFitBBox   ());
    drawWindowColorBox(painter, axesFitBBox   ());
    drawWindowColorBox(painter, keyFitBBox    ());
    drawWindowColorBox(painter, titleFitBBox  ());
    drawWindowColorBox(painter, annotationBBox());
  }

  //---

  if (needsAutoFit_) {
    needsAutoFit_ = false;

    autoFit();
  }
}

//------

CQChartsTextAnnotation *
CQChartsPlot::
addTextAnnotation(const QPointF &pos, const QString &text)
{
  CQChartsTextAnnotation *textAnnotation = new CQChartsTextAnnotation(this, pos, text);

  addAnnotation(textAnnotation);

  return textAnnotation;
}

CQChartsArrowAnnotation *
CQChartsPlot::
addArrowAnnotation(const QPointF &start, const QPointF &end)
{
  CQChartsArrowAnnotation *arrowAnnotation = new CQChartsArrowAnnotation(this, start, end);

  addAnnotation(arrowAnnotation);

  return arrowAnnotation;
}

CQChartsRectAnnotation *
CQChartsPlot::
addRectAnnotation(const QPointF &start, const QPointF &end)
{
  CQChartsRectAnnotation *rectAnnotation = new CQChartsRectAnnotation(this, start, end);

  addAnnotation(rectAnnotation);

  return rectAnnotation;
}

CQChartsEllipseAnnotation *
CQChartsPlot::
addEllipseAnnotation(const QPointF &center, double xRadius, double yRadius)
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
addPointAnnotation(const QPointF &pos, const CQChartsPlotSymbol::Type &type)
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

  connect(annotation, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

  annotation->addProperties(propertyModel(), id() + "/" + "annotations");
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

//------

void
CQChartsPlot::
setLayerActive(const Layer &layer, bool b)
{
  layerActive_[layer] = b;
}

bool
CQChartsPlot::
isLayerActive(const Layer &layer) const
{
  if (layer == CQChartsPlot::Layer::MID)
    return true;

  auto p = layerActive_.find(layer);

  if (p == layerActive_.end())
    return false;

  return (*p).second;
}

void
CQChartsPlot::
drawObjs(QPainter *painter, const Layer &layer)
{
  if (! isLayerActive(layer))
    return;

  painter->save();

  setClipRect(painter);

  CQChartsGeom::BBox bbox = displayRangeBBox();

  for (const auto &plotObj : plotObjs_) {
    if (! bbox.overlaps(plotObj->rect()))
      continue;

    plotObj->draw(painter, layer);
  }

  painter->restore();
}

void
CQChartsPlot::
setClipRect(QPainter *painter)
{
  CQChartsPlot *plot1 = firstPlot();

  if      (plot1->isDataClip()) {
    CQChartsGeom::BBox bbox = displayRangeBBox();

    bbox += annotationBBox();

    CQChartsGeom::BBox pbbox;

    windowToPixel(bbox, pbbox);

    QRectF dataRect = CQChartsUtil::toQRect(pbbox);

    painter->setClipRect(dataRect);
  }
  else if (plot1->isClip()) {
    QRectF plotRect = CQChartsUtil::toQRect(calcPixelRect());

    painter->setClipRect(plotRect);
  }
}

void
CQChartsPlot::
drawBgAxes(QPainter *painter)
{
  if (xAxis() && xAxis()->isVisible() && ! xAxis()->isGridAbove())
    xAxis()->drawGrid(this, painter);

  if (yAxis() && yAxis()->isVisible() && ! yAxis()->isGridAbove())
    yAxis()->drawGrid(this, painter);
}

void
CQChartsPlot::
drawFgAxes(QPainter *painter)
{
  if (xAxis() && xAxis()->isVisible() && xAxis()->isGridAbove())
    xAxis()->drawGrid(this, painter);

  if (yAxis() && yAxis()->isVisible() && yAxis()->isGridAbove())
    yAxis()->drawGrid(this, painter);

  //---

  if (xAxis() && xAxis()->isVisible())
    xAxis()->draw(this, painter);

  if (yAxis() && yAxis()->isVisible())
    yAxis()->draw(this, painter);
}

void
CQChartsPlot::
drawBgKey(QPainter *painter)
{
  if (key() && ! key()->isAbove())
    drawKey(painter);
}

void
CQChartsPlot::
drawFgKey(QPainter *painter)
{
  if (key() && key()->isAbove())
    drawKey(painter);
}

void
CQChartsPlot::
drawKey(QPainter *painter)
{
  CQChartsPlot *plot1 = firstPlot();

  if (! plot1->key())
    return;

  // draw key under first plot
  if (! plot1->key()->isAbove()) {
    if (plot1 == this)
      plot1->key()->draw(painter);
  }
  // draw key above last plot
  else {
    if (lastPlot() == this)
      plot1->key()->draw(painter);
  }
}

void
CQChartsPlot::
drawTitle(QPainter *painter)
{
  if (title())
    title()->draw(painter);
}

void
CQChartsPlot::
drawForeground(QPainter *painter)
{
  for (auto &annotation : annotations())
    annotation->draw(painter);

  if (view()->mode() != CQChartsView::Mode::EDIT)
    return;

  if (isSelected()) {
    if (view()->mode() == CQChartsView::Mode::EDIT) {
      editHandles_.setBBox(this->bbox());

      editHandles_.draw(painter);
    }
  }
}

//------

void
CQChartsPlot::
drawLine(QPainter *painter, const QPointF &p1, const QPointF &p2, const CQChartsLineData &data)
{
  QColor c = data.color.interpColor(this, 0, 1);

  c.setAlphaF(data.alpha);

  double lw = lengthPixelWidth(data.width);

  //---

  QPen pen(c);

  if (lw > 0.0)
    pen.setWidthF(lw);

  CQChartsUtil::penSetLineDash(pen, data.dash);

  painter->setPen(pen);

  painter->drawLine(p1, p2);
}

void
CQChartsPlot::
drawSymbol(QPainter *painter, const QPointF &p, const CQChartsSymbolData &data)
{
  QColor strokeColor = data.stroke.color.interpColor(this, 0, 1);
  QColor fillColor   = data.fill  .color.interpColor(this, 0, 1);

  strokeColor.setAlphaF(data.stroke.alpha);
  fillColor  .setAlphaF(data.fill  .alpha);

  double lw = lengthPixelWidth(data.stroke.width);

  //---

  QPen   pen;
  QBrush brush;

  if (data.stroke.visible)
    pen.setColor(strokeColor);
  else
    pen.setStyle(Qt::NoPen);

  pen.setWidthF(lw);

  if (data.stroke.visible) {
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(fillColor);
  }
  else
    brush.setStyle(Qt::NoBrush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  CQChartsSymbol2DRenderer srenderer(painter, CQChartsUtil::fromQPoint(p), data.size);

  if (data.fill.visible)
    CQChartsPlotSymbolMgr::fillSymbol(data.type, &srenderer);

  if (data.stroke.visible)
    CQChartsPlotSymbolMgr::drawSymbol(data.type, &srenderer);
}

void
CQChartsPlot::
drawSymbol(QPainter *painter, const QPointF &p, const CQChartsPlotSymbol::Type &symbol,
           double size, bool stroked, const QColor &strokeColor, double lineWidth,
           bool filled, const QColor &fillColor)
{
  QPen   pen;
  QBrush brush;

  if (stroked)
    pen.setColor(strokeColor);
  else
    pen.setStyle(Qt::NoPen);

  pen.setWidthF(lineWidth);

  if (filled) {
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(fillColor);
  }
  else
    brush.setStyle(Qt::NoBrush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  drawSymbol(painter, p, symbol, size);
}

void
CQChartsPlot::
drawSymbol(QPainter *painter, const QPointF &p, const CQChartsPlotSymbol::Type &symbol, double size)
{
  CQChartsSymbol2DRenderer srenderer(painter, CQChartsUtil::fromQPoint(p), size);

  if (painter->brush().style() != Qt::NoBrush)
    CQChartsPlotSymbolMgr::fillSymbol(symbol, &srenderer);

  if (painter->pen().style() != Qt::NoPen)
    CQChartsPlotSymbolMgr::drawSymbol(symbol, &srenderer);
}

void
CQChartsPlot::
drawTextAtPoint(QPainter *painter, const QPointF &point, const QString &text,
                const QPen &pen, const CQChartsTextOptions &options) const
{
  if (CQChartsUtil::isZero(options.angle)) {
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
#if 0
  painter->save();

  if (CQChartsUtil::isZero(options.angle)) {
    QFontMetricsF fm(painter->font());

    if (options.clipped)
      painter->setClipRect(rect);

    if (! options.contrast)
      painter->setPen(pen);

    //---

    QStringList strs;

    if (options.formatted)
      CQChartsUtil::formatStringInRect(text, painter->font(), rect, strs);
    else
      strs << text;

    //---

    double tw = 0;

    for (int i = 0; i < strs.size(); ++i)
      tw = std::max(tw, fm.width(strs[i]));

    double th = strs.size()*fm.height();

    if (options.scaled) {
      double sx = (tw > 0 ? rect.width ()/tw : 1);
      double sy = (th > 0 ? rect.height()/th : 1);

      double s = std::min(sx, sy);

      double fs = painter->font().pointSizeF()*s;

      fs = std::min(std::max(fs, minScaleFontSize()), maxScaleFontSize());

      QFont font1 = painter->font();

      font1.setPointSizeF(fs);

      painter->setFont(font1);

      fm = QFontMetricsF(painter->font());

      th = strs.size()*fm.height();
    }

    //---

    double dy = 0.0;

    if      (options.align & Qt::AlignVCenter)
      dy = (rect.height() - th)/2.0;
    else if (options.align & Qt::AlignBottom)
      dy = rect.height() - th;

    double y = rect.top() + dy + fm.ascent();

    for (int i = 0; i < strs.size(); ++i) {
      double dx = 0.0;

      double tw = fm.width(strs[i]);

      if      (options.align & Qt::AlignHCenter)
         dx = (rect.width() - tw)/2;
      else if (options.align & Qt::AlignRight)
         dx = rect.width() - tw;

      double x = rect.left() + dx;

      if (options.contrast)
        drawContrastText(painter, x, y, strs[i], pen);
      else
        painter->drawText(x, y, strs[i]);

      y += fm.height();
    }
  }
  else {
    painter->setPen(pen);

    // TODO: support align and contrast
    CQChartsRotatedText::drawRotatedText(painter, rect.center().x(), rect.center().y(),
                                         text, options.angle, Qt::AlignHCenter | Qt::AlignVCenter,
                                         /*alignBox*/false);
  }

  painter->restore();
#endif
}

void
CQChartsPlot::
drawContrastText(QPainter *painter, double x, double y, const QString &text,
                 const QPen &pen) const
{
  CQChartsView::drawContrastText(painter, x, y, text, pen);
#if 0
  QColor icolor = CQChartsUtil::invColor(pen.color());

  icolor.setAlphaF(0.5);

  painter->setPen(icolor);

  painter->drawText(QPointF(x + 1, y + 1), text);

  painter->setPen(pen);

  painter->drawText(QPointF(x, y), text);
#endif
}

//------

void
CQChartsPlot::
drawWindowColorBox(QPainter *painter, const CQChartsGeom::BBox &bbox, const QColor &c)
{
  if (! bbox.isSet())
    return;

  CQChartsGeom::BBox prect;

  windowToPixel(bbox, prect);

  drawColorBox(painter, prect, c);
}

void
CQChartsPlot::
drawColorBox(QPainter *painter, const CQChartsGeom::BBox &bbox, const QColor &c)
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

  if (! CQChartsUtil::isZero(ri)) {
    CQChartsGeom::BBox bbox1(c.x - ri, c.y - ri, c.x + ri, c.y + ri);

    CQChartsGeom::BBox pbbox1;

    windowToPixel(bbox1, pbbox1);

    //---

    double da = (isInvertX() != isInvertY() ? -1 : 1);

    double ra1 = da*CQChartsUtil::Deg2Rad(a1);
    double ra2 = da*CQChartsUtil::Deg2Rad(a2);

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
  view_->update();
}

//------

void
CQChartsPlot::
updateObjPenBrushState(CQChartsPlotObj *obj, QPen &pen, QBrush &brush) const
{
  // inside and selected
  if      (obj->isInside() && obj->isSelected()) {
    updateSelectedObjPenBrushState(pen, brush);
    updateInsideObjPenBrushState  (pen, brush);
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

void
CQChartsPlot::
updateInsideObjPenBrushState(QPen &pen, QBrush &brush) const
{
  // fill and stroke
  if (brush.style() != Qt::NoBrush) {
    if (view()->insideMode() == CQChartsView::HighlightDataMode::OUTLINE) {
      QColor opc;

      if (pen.style() != Qt::NoPen) {
        QColor pc = pen.color();

        if (view()->isInsideStrokeColorEnabled())
          opc = view()->insideStrokeColor();
        else
          opc = CQChartsUtil::invColor(pc);

        opc.setAlphaF(pc.alphaF());
      }
      else {
        QColor bc = brush.color();

        if (view()->isInsideStrokeColorEnabled())
          opc = view()->insideStrokeColor();
        else
          opc = CQChartsUtil::invColor(bc);
      }

      CQChartsUtil::penSetLineDash(pen, view()->insideStrokeDash());

      pen.setColor(opc);
      pen.setWidthF(view()->insideStrokeWidth());
    }
    else {
      QColor bc = brush.color();

      QColor ibc;

      if (view()->isInsideFillColorEnabled())
        ibc = view()->insideFillColor();
      else
        ibc = insideColor(bc);

      ibc.setAlphaF(bc.alphaF());

      brush.setColor(ibc);
    }
  }
  // just stroke
  else {
    assert(pen.style() != Qt::NoPen);

    QColor pc = pen.color();

    QColor opc;

    if (view()->isInsideStrokeColorEnabled())
      opc = view()->insideStrokeColor();
    else
      opc = CQChartsUtil::invColor(pc);

    opc.setAlphaF(pc.alphaF());

    CQChartsUtil::penSetLineDash(pen, view()->insideStrokeDash());

    pen.setColor(opc);
    pen.setWidthF(view()->insideStrokeWidth());
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

      if (pen.style() != Qt::NoPen) {
        QColor pc = pen.color();

        QColor opc;

        if (view()->isSelectedStrokeColorEnabled())
          opc = view()->selectedStrokeColor();
        else
          opc = selectedColor(pc);

        opc.setAlphaF(pc.alphaF());
      }
      else {
        QColor bc = brush.color();

        if (view()->isSelectedStrokeColorEnabled())
          opc = view()->selectedStrokeColor();
        else
          opc = CQChartsUtil::invColor(bc);
      }

      CQChartsUtil::penSetLineDash(pen, view()->selectedStrokeDash());

      pen.setColor(opc);
      pen.setWidthF(view()->selectedStrokeWidth());
    }
    else {
      QColor bc = brush.color();

      QColor ibc;

      if (view()->isSelectedFillColorEnabled())
        ibc = view()->selectedFillColor();
      else
        ibc = selectedColor(bc);

      ibc.setAlphaF(bc.alphaF());

      brush.setColor(ibc);
    }
  }
  // just stroke
  else if (pen.style() != Qt::NoPen) {
    QColor pc = pen.color();

    QColor opc;

    if (view()->isSelectedStrokeColorEnabled())
      opc = view()->selectedStrokeColor();
    else
      opc = CQChartsUtil::invColor(pc);

    opc.setAlphaF(pc.alphaF());

    CQChartsUtil::penSetLineDash(pen, view()->selectedStrokeDash());

    pen.setColor(opc);
    pen.setWidthF(view()->selectedStrokeWidth());
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
  double r = CQChartsUtil::norm(i + 1, 0, n + 1);

  return interpPaletteColor(r, scale);
}

QColor
CQChartsPlot::
interpPaletteColor(double r, bool scale) const
{
  CQChartsGradientPalette *palette = view()->themePalette();

  QColor c = palette->getColor(r, scale);

  return c;
}

QColor
CQChartsPlot::
interpIndPaletteColor(int ind, double r, bool scale) const
{
  CQChartsGradientPalette *palette = view()->themePalette(ind);

  QColor c = palette->getColor(r, scale);

  return c;
}

QColor
CQChartsPlot::
interpGroupPaletteColor(int ig, int ng, int i, int n, bool scale) const
{
  CQChartsGradientPalette *palette = view()->themeGroupPalette(ig, ng);

  double r = CQChartsUtil::norm(i + 1, 0, n  + 1);

  QColor c = palette->getColor(r, scale);

  return c;
}

QColor
CQChartsPlot::
interpGroupPaletteColor(double r1, double r2, double dr) const
{
  CQChartsTheme *theme = view()->theme();

  // r1 is parent color and r2 is child color
  QColor c1 = theme->palette()->getColor(r1 - dr/2.0);
  QColor c2 = theme->palette()->getColor(r1 + dr/2.0);

  return CQChartsUtil::blendColors(c1, c2, r2);
}

QColor
CQChartsPlot::
interpThemeColor(double r) const
{
  CQChartsGradientPalette *palette = view()->interfacePalette();

  QColor c = palette->getColor(r, /*scale*/true);

  return c;
}

QColor
CQChartsPlot::
textColor(const QColor &bg) const
{
  return CQChartsUtil::bwColor(bg);
}

//------

CQChartsPlot::ColumnType
CQChartsPlot::
columnValueType(const CQChartsColumn &column) const
{
  CQBaseModel::Type  columnType;
  CQChartsNameValues nameValues;

  (void) columnValueType(column, columnType, nameValues);

  return columnType;
}

bool
CQChartsPlot::
columnValueType(const CQChartsColumn &column, CQBaseModel::Type &columnType,
                CQChartsNameValues &nameValues) const
{
  QAbstractItemModel *model = this->model().data();
  assert(model);

  if (! column.isValid())
    return false;

  return CQChartsUtil::columnValueType(charts(), model, column, columnType, nameValues);
}

bool
CQChartsPlot::
columnTypeStr(const CQChartsColumn &column, QString &typeStr) const
{
  QAbstractItemModel *model = this->model().data();
  assert(model);

  return CQChartsUtil::columnTypeStr(charts(), model, column, typeStr);
}

bool
CQChartsPlot::
setColumnTypeStr(const CQChartsColumn &column, const QString &typeStr)
{
  QAbstractItemModel *model = this->model().data();
  assert(model);

  return CQChartsUtil::setColumnTypeStr(charts(), model, column, typeStr);
}

bool
CQChartsPlot::
columnDetails(const CQChartsColumn &column, QString &typeName,
              QVariant &minValue, QVariant &maxValue) const
{
  QAbstractItemModel *model = this->model().data();
  assert(model);

  CQChartsModelColumnDetails columnDetails(charts(), model, column);

  typeName = columnDetails.typeName();
  minValue = columnDetails.minValue();
  maxValue = columnDetails.maxValue();

  return true;
}

//------

bool
CQChartsPlot::
getHierColumnNames(const QModelIndex &parent, int row, const Columns &nameColumns,
                   const QString &separator, QStringList &nameStrs, ModelIndices &nameInds)
{
  QAbstractItemModel *model = this->model().data();
  assert(model);

  // single column (seprated names)
  if (nameColumns.size() == 1) {
    const CQChartsColumn &nameColumn = nameColumns[0];

    //---

    bool ok;

    QString name = modelString(model, row, nameColumn, parent, ok);

    if (ok && ! name.simplified().length())
      ok = false;

    if (ok) {
      if (separator.length())
        nameStrs = name.split(separator, QString::SkipEmptyParts);
      else
        nameStrs << name;
    }

    QModelIndex nameInd = model->index(row, nameColumn.column(), parent);

    nameInds.push_back(nameInd);
  }
  else {
    for (auto &nameColumn : nameColumns) {
      bool ok;

      QString name = modelString(model, row, nameColumn, parent, ok);

      if (ok && ! name.simplified().length())
        ok = false;

      if (ok) {
        nameStrs << name;

        QModelIndex nameInd = model->index(row, nameColumn.column(), parent);

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

  return CQChartsUtil::isHierarchical(model);
}

//------

CQChartsValueSet *
CQChartsPlot::
addValueSet(const QString &name, double min, double max)
{
  CQChartsValueSet *valueSet = addValueSet(name);

  valueSet->setMapMin(min);
  valueSet->setMapMax(max);

  return valueSet;
}

CQChartsValueSet *
CQChartsPlot::
addValueSet(const QString &name)
{
  assert(! getValueSet(name));

  CQChartsValueSet *valueSet = new CQChartsValueSet(this);

  valueSets_[name] = valueSet;

  return valueSet;
}

CQChartsValueSet *
CQChartsPlot::
addColorSet(const QString &name)
{
  assert(! getColorSet(name));

  CQChartsColorSet *colorSet = new CQChartsColorSet(this);

  valueSets_[name] = colorSet;

  return colorSet;
}

CQChartsValueSet *
CQChartsPlot::
getValueSet(const QString &name) const
{
  auto p = valueSets_.find(name);

  if (p == valueSets_.end())
    return nullptr;

  return (*p).second;
}

CQChartsColorSet *
CQChartsPlot::
getColorSet(const QString &name) const
{
  CQChartsValueSet *valueSet = getValueSet(name);

  return dynamic_cast<CQChartsColorSet *>(valueSet);
}

void
CQChartsPlot::
clearValueSets()
{
  for (auto &valueSet : valueSets_)
    valueSet.second->clear();
}

void
CQChartsPlot::
deleteValueSets()
{
  for (auto &valueSet : valueSets_)
    delete valueSet.second;

  valueSets_.clear();
}

const CQChartsColumn &
CQChartsPlot::
valueSetColumn(const QString &name) const
{
  CQChartsValueSet *valueSet = getValueSet(name);
  assert(valueSet);

  return valueSet->column();
}

bool
CQChartsPlot::
setValueSetColumn(const QString &name, const CQChartsColumn &c)
{
  CQChartsValueSet *valueSet = getValueSet(name);
  assert(valueSet);

  if (valueSet->column() != c) {
    valueSet->setColumn(c);
    return true;
  }

  return false;
}

bool
CQChartsPlot::
isValueSetMapped(const QString &name) const
{
  CQChartsValueSet *valueSet = getValueSet(name);
  assert(valueSet);

  return valueSet->isMapped();
}

void
CQChartsPlot::
setValueSetMapped(const QString &name, bool b)
{
  CQChartsValueSet *valueSet = getValueSet(name);
  assert(valueSet);

  valueSet->setMapped(b);
}

double
CQChartsPlot::
valueSetMapMin(const QString &name) const
{
  CQChartsValueSet *valueSet = getValueSet(name);
  assert(valueSet);

  return valueSet->mapMin();
}

void
CQChartsPlot::
setValueSetMapMin(const QString &name, double min)
{
  CQChartsValueSet *valueSet = getValueSet(name);
  assert(valueSet);

  valueSet->setMapMin(min);
}

double
CQChartsPlot::
valueSetMapMax(const QString &name) const
{
  CQChartsValueSet *valueSet = getValueSet(name);
  assert(valueSet);

  return valueSet->mapMax();
}

void
CQChartsPlot::
setValueSetMapMax(const QString &name, double max)
{
  CQChartsValueSet *valueSet = getValueSet(name);
  assert(valueSet);

  valueSet->setMapMax(max);
}

bool
CQChartsPlot::
colorSetColor(const QString &name, int i, OptColor &color)
{
  CQChartsColorSet *colorSet = getColorSet(name);
  assert(colorSet);

  return colorSet->icolor(i, color);
}

void
CQChartsPlot::
initValueSets()
{
  // if no columns set then skip
  bool anyColumn = false;

  for (auto &valueSet : valueSets_) {
    if (valueSet.second->column().isValid()) {
      anyColumn = true;
      break;
    }
  }

  if (! anyColumn)
    return;

  //---

  // if any sets non-empty then already populated
  // TODO: only check active and fill all empty ?
  bool empty = true;

  for (auto &valueSet : valueSets_) {
    if (! valueSet.second->empty()) {
      empty = false;
      break;
    }
  }

  if (! empty)
    return;

  //---

  // process model data
  class ValueSetVisitor : public ModelVisitor {
   public:
    ValueSetVisitor(CQChartsPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *model, const QModelIndex &ind, int row) override {
      plot_->addValueSetRow(model, ind, row);

      return State::OK;
    }

   private:
    CQChartsPlot *plot_ { nullptr };
  };

  ValueSetVisitor valueSetVisitor(this);

  visitModel(valueSetVisitor);
}

void
CQChartsPlot::
addValueSetRow(QAbstractItemModel *model, const QModelIndex &parent, int row)
{
  for (auto &valueSet : valueSets_) {
    const CQChartsColumn &column = valueSet.second->column();

    if (column.isValid()) {
      bool ok;

      QVariant value = modelValue(model, row, column, parent, ok);

      valueSet.second->addValue(value); // always add some value
    }
  }
}

void
CQChartsPlot::
addColumnValues(const CQChartsColumn &column, CQChartsValueSet &valueSet)
{
  class ValueSetVisitor : public ModelVisitor {
   public:
    ValueSetVisitor(CQChartsPlot *plot, const CQChartsColumn &column, CQChartsValueSet &valueSet) :
     plot_(plot), column_(column), valueSet_(valueSet) {
    }

    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      bool ok;

      QVariant value = plot_->modelValue(model, row, column_, parent, ok);

      // TODO: skip if not ok ?

      valueSet_.addValue(value);

      return State::OK;
    }

   private:
    CQChartsPlot*     plot_   { nullptr };
    CQChartsColumn    column_;
    CQChartsValueSet& valueSet_;
  };

  ValueSetVisitor valueSetVisitor(this, column, valueSet);

  visitModel(valueSetVisitor);
}

//------

// init group buckets depending on:
//  group column
//  multiple value columns
//  row grouping
void
CQChartsPlot::
initGroup(const GroupData &data)
{
  groupBucket_.clear();

  QAbstractItemModel *model = this->model().data();
  if (! model) return;

  //---

  // for row grouping we use the column header as the grouping id so all row
  // values in the column are added to the group
  if (data.columns.size() > 1 && data.rowGrouping) {
    groupBucket_.setDataType   (CQChartsColumnBucket::DataType::HEADER);
    groupBucket_.setColumnType (ColumnType::INTEGER);
    groupBucket_.setRowGrouping(true);

    for (const auto &column : data.columns) {
      bool ok;

      QString name = modelHeaderString(model, column, Qt::Horizontal, Qt::DisplayRole, ok);

      int ind = groupBucket_.addValue(column.column());

      groupBucket_.setIndName(ind, name);
    }

    return;
  }

  //---

  // for specified group column set column and column type
  if      (data.column.isValid()) {
    ColumnType columnType = CQBaseModel::Type::STRING;

    if (data.column.type() == CQChartsColumn::Type::DATA)
      columnType = columnValueType(data.column);

    groupBucket_.setDataType  (CQChartsColumnBucket::DataType::COLUMN);
    groupBucket_.setColumnType(columnType);
    groupBucket_.setColumn    (data.column);
  }
  // no group column then use parent path (hierarchical)
  else if (isHierarchical()) {
    groupBucket_.setDataType  (CQChartsColumnBucket::DataType::PATH);
    groupBucket_.setColumnType(ColumnType::STRING);
  }
  else {
    groupBucket_.setColumnType(ColumnType::STRING);
  }

  groupBucket_.setDefaultRow(data.defaultRow);

  // process model data
  class GroupVisitor : public ModelVisitor {
   public:
    GroupVisitor(CQChartsPlot *plot, CQChartsColumnBucket *bucket) :
     plot_(plot), bucket_(bucket) {
    }

    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      // add column value
      if      (bucket_->dataType() == CQChartsColumnBucket::DataType::COLUMN) {
        bool ok;

        QVariant value = plot_->modelHierValue(model, row, bucket_->column(), parent, ok);

        if (value.isValid())
          bucket_->addValue(value);
      }
      // add parent path (hierarchical)
      else if (bucket_->dataType() == CQChartsColumnBucket::DataType::PATH) {
        QString path = CQChartsUtil::parentPath(model, parent);

        bucket_->addString(path);
      }
      else if (bucket_->isDefaultRow()) {
        bucket_->addValue(row); // default to row
      }
      else {
        bucket_->addString(""); // no bucket
      }

      return State::OK;
    }

   private:
    CQChartsPlot*         plot_   { nullptr };
    CQChartsColumnBucket* bucket_ { nullptr };
  };

  GroupVisitor groupVisitor(this, &groupBucket_);

  visitModel(groupVisitor);
}

int
CQChartsPlot::
rowGroupInd(QAbstractItemModel *model, const QModelIndex &parent, int row,
            const CQChartsColumn &column) const
{
  // header has multiple groups (one per column)
  if      (groupBucket_.dataType() == CQChartsColumnBucket::DataType::HEADER) {
    return groupBucket_.ind(column.column());
  }
  // get group id from value in group column
  else if (groupBucket_.dataType() == CQChartsColumnBucket::DataType::COLUMN) {
    bool ok;

    QVariant value = modelHierValue(model, row, groupBucket_.column(), parent, ok);

    return groupBucket_.ind(value);
  }
  // get group id from parent path name
  else if (groupBucket_.dataType() == CQChartsColumnBucket::DataType::PATH) {
    QString path = CQChartsUtil::parentPath(model, parent);

    return groupBucket_.ind(path);
  }
  else if (groupBucket_.isDefaultRow()) {
    return row; // default to row
  }
  else {
    return 0; // no bucket
  }
}

//------

void
CQChartsPlot::
visitModel(ModelVisitor &visitor)
{
  visitor.setPlot(this);

  //if (isPreview())
  //  visitor.setMaxRows(previewMaxRows());

  (void) CQChartsUtil::visitModel(model().data(), visitor);
}

//------

bool
CQChartsPlot::
modelMappedReal(QAbstractItemModel *model, int row, const CQChartsColumn &col,
                const QModelIndex &parent, double &r, bool log, double def) const
{
  if (col.isValid()) {
    bool ok1;

    r = modelReal(model, row, col, parent, ok1);

    if (! ok1)
      r = def;

    if (CQChartsUtil::isNaN(r) || CQChartsUtil::isInf(r))
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
    IdVisitor(CQChartsPlot *plot, const QString &id) :
     plot_(plot), id_(id) {
    }

    State visit(QAbstractItemModel *, const QModelIndex &ind, int row) override {
      bool ok;

      QString id = plot_->idColumnString(row, ind, ok);

      if (ok && id == id_)
        row_ = row;

      return State::OK;
    }

    int row() const { return row_; }

   private:
    CQChartsPlot *plot_ { nullptr };
    QString       id_;
    int           row_ { -1 };
  };

  CQChartsPlot *th = const_cast<CQChartsPlot *>(this);

  IdVisitor idVisitor(th, id);

  th->visitModel(idVisitor);

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

  if (CQChartsUtil::toReal(var, r))
    str = columnStr(idColumn(), r);
  else {
    bool ok;

    str = CQChartsUtil::toString(var, ok);
  }

  return str;
}

//------

QString
CQChartsPlot::
modelHeaderString(const CQChartsColumn &column, bool &ok) const
{
  return modelHeaderString(model().data(), column, Qt::Horizontal, Qt::DisplayRole, ok);
}

QString
CQChartsPlot::
modelHeaderString(const CQChartsColumn &column, int role, bool &ok) const
{
  return modelHeaderString(model().data(), column, Qt::Horizontal, role, ok);
}

QString
CQChartsPlot::
modelHeaderString(const CQChartsColumn &column, Qt::Orientation orient, int role, bool &ok) const
{
  return modelHeaderString(model().data(), column, orient, role, ok);
}

QString
CQChartsPlot::
modelHeaderString(const CQChartsColumn &column, Qt::Orientation orient, bool &ok) const
{
  return modelHeaderString(model().data(), column, orient, Qt::DisplayRole, ok);
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
modelString(int row, const CQChartsColumn &column, const QModelIndex &parent, bool &ok) const
{
  return modelString(model().data(), row, column, parent, ok);
}

double
CQChartsPlot::
modelReal(int row, const CQChartsColumn &column, const QModelIndex &parent, bool &ok) const
{
  return modelReal(model().data(), row, column, parent, ok);
}

long
CQChartsPlot::
modelInteger(int row, const CQChartsColumn &column, const QModelIndex &parent, bool &ok) const
{
  return modelInteger(model().data(), row, column, parent, ok);
}

CQChartsColor
CQChartsPlot::
modelColor(int row, const CQChartsColumn &column, const QModelIndex &parent, bool &ok) const
{
  return modelColor(model().data(), row, column, parent, ok);
}

//------

QString
CQChartsPlot::
modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                  Qt::Orientation orientation, int role, bool &ok) const
{
  return CQChartsUtil::modelHeaderString(model, column, orientation, role, ok);
}

QVariant
CQChartsPlot::
modelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
           const QModelIndex &parent, int role, bool &ok) const
{
  return CQChartsUtil::modelValue(model, row, column, parent, role, ok);
}

QVariant
CQChartsPlot::
modelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
           const QModelIndex &parent, bool &ok) const
{
  return CQChartsUtil::modelValue(model, row, column, parent, ok);
}

QString
CQChartsPlot::
modelString(QAbstractItemModel *model, int row, const CQChartsColumn &column,
            const QModelIndex &parent, bool &ok) const
{
  return CQChartsUtil::modelString(model, row, column, parent, ok);
}

double
CQChartsPlot::
modelReal(QAbstractItemModel *model, int row, const CQChartsColumn &column,
          const QModelIndex &parent, bool &ok) const
{
  return CQChartsUtil::modelReal(model, row, column, parent, ok);
}

long
CQChartsPlot::
modelInteger(QAbstractItemModel *model, int row, const CQChartsColumn &column,
             const QModelIndex &parent, bool &ok) const
{
  return CQChartsUtil::modelInteger(model, row, column, parent, ok);
}

CQChartsColor
CQChartsPlot::
modelColor(QAbstractItemModel *model, int row, const CQChartsColumn &column,
           const QModelIndex &parent, bool &ok) const
{
  return CQChartsUtil::modelColor(model, row, column, parent, ok);
}

//------

QVariant
CQChartsPlot::
modelHierValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                const QModelIndex &parent, bool &ok) const
{
  QVariant v = modelValue(model, row, column, parent, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      v = modelValue(model, row1, column, parent1, ok);
    }
  }

  return v;
}

QString
CQChartsPlot::
modelHierString(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                const QModelIndex &parent, bool &ok) const
{
  QString s = modelString(model, row, column, parent, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      s = modelString(model, row1, column, parent1, ok);
    }
  }

  return s;
}

double
CQChartsPlot::
modelHierReal(QAbstractItemModel *model, int row, const CQChartsColumn &column,
              const QModelIndex &parent, bool &ok) const
{
  double r = modelReal(model, row, column, parent, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      r = modelReal(model, row1, column, parent1, ok);
    }
  }

  return r;
}

long
CQChartsPlot::
modelHierInteger(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                 const QModelIndex &parent, bool &ok) const
{
  int i = modelInteger(model, row, column, parent, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      i = modelInteger(model, row1, column, parent1, ok);
    }
  }

  return i;
}

//------

bool
CQChartsPlot::
isSelectIndex(const QModelIndex &ind, int row, const CQChartsColumn &column,
              const QModelIndex &parent) const
{
  if (column.type() != CQChartsColumn::Type::DATA)
    return false;

  return isSelectIndex(ind, row, column.column(), parent);
}

bool
CQChartsPlot::
isSelectIndex(const QModelIndex &ind, int row, int col, const QModelIndex &parent) const
{
  return (ind == selectIndex(row, col, parent));
}

QModelIndex
CQChartsPlot::
selectIndex(int row, const CQChartsColumn &col, const QModelIndex &parent) const
{
  if (col.type() == CQChartsColumn::Type::DATA)
    return selectIndex(row, col.column(), parent);
  else
    return QModelIndex();
}

QModelIndex
CQChartsPlot::
selectIndex(int row, int col, const QModelIndex &parent) const
{
  std::vector<QSortFilterProxyModel *> proxyModels;
  QAbstractItemModel*                  sourceModel;

  this->proxyModels(proxyModels, sourceModel);

  return sourceModel->index(row, col, parent);
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
addSelectIndex(int row, int col, const QModelIndex &parent)
{
  addSelectIndex(selectIndex(row, col, parent));
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
          QModelIndex ind1 = model->index(startRow, column, parent);
          QModelIndex ind2 = model->index(endRow  , column, parent);

          optItemSelection.select(ind1, ind2);

          startRow = row;
          endRow   = row;
        }
      }

      if (startRow >= 0) {
        QModelIndex ind1 = model->index(startRow, column, parent);
        QModelIndex ind2 = model->index(endRow  , column, parent);

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
    return CQChartsUtil::getNaN();
}

double
CQChartsPlot::
expValue(double x, int base) const
{
  if (x <= 709.78271289)
    return std::exp(x*log(base));
  else
    return CQChartsUtil::getNaN();
}

//------

QPointF
CQChartsPlot::
positionToPlot(const CQChartsPosition &pos) const
{
  CQChartsGeom::Point p = CQChartsUtil::fromQPoint(pos.p());

  CQChartsGeom::Point p1 = p;

  if      (pos.units() == CQChartsPosition::Units::PIXEL)
    p1 = pixelToWindow(p);
  else if (pos.units() == CQChartsPosition::Units::PLOT)
    p1 = p;
  else if (pos.units() == CQChartsPosition::Units::VIEW)
    p1 = pixelToWindow(view_->windowToPixel(p));
  else if (pos.units() == CQChartsPosition::Units::PERCENT) {
    CQChartsGeom::BBox pbbox = displayRangeBBox();

    p1.setX(p.getX()*pbbox.getWidth ()/100.0);
    p1.setY(p.getX()*pbbox.getHeight()/100.0);
  }

  return CQChartsUtil::toQPoint(p1);
}

QPointF
CQChartsPlot::
positionToPixel(const CQChartsPosition &pos) const
{
  CQChartsGeom::Point p = CQChartsUtil::fromQPoint(pos.p());

  CQChartsGeom::Point p1 = p;

  if      (pos.units() == CQChartsPosition::Units::PIXEL)
    p1 = p;
  else if (pos.units() == CQChartsPosition::Units::PLOT)
    p1 = windowToPixel(p);
  else if (pos.units() == CQChartsPosition::Units::VIEW)
    p1 = view_->windowToPixel(p);
  else if (pos.units() == CQChartsPosition::Units::PERCENT) {
    CQChartsGeom::BBox pbbox = calcPixelRect();

    p1.setX(p.getX()*pbbox.getWidth ()/100.0);
    p1.setY(p.getX()*pbbox.getHeight()/100.0);
  }

  return CQChartsUtil::toQPoint(p1);
}

//------

double
CQChartsPlot::
lengthPlotWidth(const CQChartsLength &len) const
{
  if      (len.units() == CQChartsLength::Units::PIXEL)
    return pixelToWindowWidth(len.value());
  else if (len.units() == CQChartsLength::Units::PLOT)
    return len.value();
  else if (len.units() == CQChartsLength::Units::VIEW)
    return pixelToWindowWidth(view_->windowToPixelWidth(len.value()));
  else if (len.units() == CQChartsLength::Units::PERCENT)
    return len.value()*displayRangeBBox().getWidth()/100.0;
  else
    return len.value();
}

double
CQChartsPlot::
lengthPlotHeight(const CQChartsLength &len) const
{
  if      (len.units() == CQChartsLength::Units::PIXEL)
    return pixelToWindowHeight(len.value());
  else if (len.units() == CQChartsLength::Units::PLOT)
    return len.value();
  else if (len.units() == CQChartsLength::Units::VIEW)
    return pixelToWindowHeight(view_->windowToPixelHeight(len.value()));
  else if (len.units() == CQChartsLength::Units::PERCENT)
    return len.value()*displayRangeBBox().getHeight()/100.0;
  else
    return len.value();
}

double
CQChartsPlot::
lengthPixelWidth(const CQChartsLength &len) const
{
  if      (len.units() == CQChartsLength::Units::PIXEL)
    return len.value();
  else if (len.units() == CQChartsLength::Units::PLOT)
    return windowToPixelWidth(len.value());
  else if (len.units() == CQChartsLength::Units::VIEW)
    return view_->windowToPixelWidth(len.value());
  else if (len.units() == CQChartsLength::Units::PERCENT)
    return len.value()*calcPixelRect().getWidth()/100.0;
  else
    return len.value();
}

double
CQChartsPlot::
lengthPixelHeight(const CQChartsLength &len) const
{
  if      (len.units() == CQChartsLength::Units::PIXEL)
    return len.value();
  else if (len.units() == CQChartsLength::Units::PLOT)
    return windowToPixelHeight(len.value());
  else if (len.units() == CQChartsLength::Units::VIEW)
    return view_->windowToPixelHeight(len.value());
  else if (len.units() == CQChartsLength::Units::PERCENT)
    return len.value()*calcPixelRect().getHeight()/100.0;
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

void
CQChartsPlot::
windowToPixel(const CQChartsGeom::BBox &wrect, CQChartsGeom::BBox &prect) const
{
  double px1, py1, px2, py2;

  windowToPixel(wrect.getXMin(), wrect.getYMin(), px1, py2);
  windowToPixel(wrect.getXMax(), wrect.getYMax(), px2, py1);

  prect = CQChartsGeom::BBox(px1, py1, px2, py2);
}

void
CQChartsPlot::
pixelToWindow(const CQChartsGeom::BBox &prect, CQChartsGeom::BBox &wrect) const
{
  double wx1, wy1, wx2, wy2;

  pixelToWindow(prect.getXMin(), prect.getYMin(), wx1, wy2);
  pixelToWindow(prect.getXMax(), prect.getYMax(), wx2, wy1);

  wrect = CQChartsGeom::BBox(wx1, wy1, wx2, wy2);
}

double
CQChartsPlot::
pixelToSignedWindowWidth (double ww) const
{
  return CQChartsUtil::sign(ww)*pixelToWindowWidth(ww);
}

double
CQChartsPlot::
pixelToSignedWindowHeight(double wh) const
{
  return -CQChartsUtil::sign(wh)*pixelToWindowHeight(wh);
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
  return CQChartsUtil::sign(ww)*windowToPixelWidth(ww);
}

double
CQChartsPlot::
windowToSignedPixelHeight(double wh) const
{
  return -CQChartsUtil::sign(wh)*windowToPixelHeight(wh);
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

CQChartsPlot::ModelVisitor::State
CQChartsPlot::ModelVisitor::
preVisit(QAbstractItemModel *, const QModelIndex &, int)
{
  int vrow = vrow_++;

  if (! plot_->isEveryEnabled())
    return State::OK;

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

  return State::OK;
}
