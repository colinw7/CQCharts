#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsTitle.h>
#include <CQChartsPlotObj.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsColumn.h>
#include <CQChartsBoxObj.h>
#include <CQChartsPointObj.h>
#include <CQChartsPlotObjTree.h>
#include <CQChartsNoDataObj.h>
#include <CQChartsColorSet.h>
#include <CQChartsRotatedText.h>
#include <CQChartsDisplayTransform.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQChartsGradientPalette.h>

#include <QItemSelectionModel>
#include <QSortFilterProxyModel>
#include <QPainter>

//------

CQChartsPlotTypeMgr::
CQChartsPlotTypeMgr()
{
}

CQChartsPlotTypeMgr::
~CQChartsPlotTypeMgr()
{
  for (auto &type : types_)
    delete type.second;
}

void
CQChartsPlotTypeMgr::
addType(const QString &name, CQChartsPlotType *type)
{
  types_[name] = type;

  type->addParameters();
}

bool
CQChartsPlotTypeMgr::
isType(const QString &name) const
{
  auto p = types_.find(name);

  return (p != types_.end());
}

CQChartsPlotType *
CQChartsPlotTypeMgr::
type(const QString &name) const
{
  auto p = types_.find(name);
  assert(p != types_.end());

  return (*p).second;
}

void
CQChartsPlotTypeMgr::
getTypeNames(QStringList &names, QStringList &descs) const
{
  for (const auto &type : types_) {
    names.push_back(type.second->name());
    descs.push_back(type.second->desc());
  }
}

//------

CQChartsPlotType::
CQChartsPlotType()
{
}

void
CQChartsPlotType::
addParameters()
{
  addBoolParameter("key", "Key", "keyVisible", "optional");
}

//------

CQChartsHierPlotType::
CQChartsHierPlotType()
{
}

void
CQChartsHierPlotType::
addParameters()
{
  addColumnParameter ("name" , "Name" , "nameColumn" , "", 0);
  addColumnsParameter("names", "Names", "nameColumns", "optional");
  addColumnParameter ("value", "Value", "valueColumn", "optional");
  addColumnParameter ("color", "Color", "colorColumn", "optional");

  addStringParameter("separator", "Separator", "separator", "optional", "/");
}

//------

CQChartsPlot::
CQChartsPlot(CQChartsView *view, CQChartsPlotType *type, const ModelP &model) :
 view_(view), type_(type), model_(model),
 llHandle_(this, CQChartsResizeHandle::Side::LL),
 lrHandle_(this, CQChartsResizeHandle::Side::LR),
 ulHandle_(this, CQChartsResizeHandle::Side::UL),
 urHandle_(this, CQChartsResizeHandle::Side::UR)
{
  displayRange_     = new CQChartsDisplayRange();
  displayTransform_ = new CQChartsDisplayTransform(displayRange_);

  displayRange_->setPixelAdjust(0.0);

  borderObj_     = new CQChartsBoxObj(this);
  dataBorderObj_ = new CQChartsBoxObj(this);

  plotObjTree_ = new CQChartsPlotObjTree(this);

  //--

  setBackground    (true);
  setDataBackground(true);

  setDataClip(true);

  CQChartsPaletteColor plotThemeBg(CQChartsPaletteColor::Type::THEME_VALUE, 0.0);
  CQChartsPaletteColor dataThemeBg(CQChartsPaletteColor::Type::THEME_VALUE, 0.1);

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

  connect(model_.data(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
          this, SLOT(modelDataChangedSlot(const QModelIndex &, const QModelIndex &)));
  connect(model_.data(), SIGNAL(layoutChanged()),
          this, SLOT(modelLayoutChangedSlot()));

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

  disconnect(model_.data(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
             this, SLOT(modelDataChangedSlot(const QModelIndex &, const QModelIndex &)));
  disconnect(model_.data(), SIGNAL(layoutChanged()),
             this, SLOT(modelLayoutChangedSlot()));

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
      if (plotObj->isIndex(ind1))
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

QString
CQChartsPlot::
backgroundColorStr() const
{
  return borderObj_->backgroundColorStr();
}

void
CQChartsPlot::
setBackgroundColorStr(const QString &s)
{
  borderObj_->setBackgroundColorStr(s);
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

QString
CQChartsPlot::
borderColorStr() const
{
  return borderObj_->borderColorStr();
}

void
CQChartsPlot::
setBorderColorStr(const QString &s)
{
  borderObj_->setBorderColorStr(s);
}

double
CQChartsPlot::
borderWidth() const
{
  return borderObj_->borderWidth();
}

void
CQChartsPlot::
setBorderWidth(double r)
{
  borderObj_->setBorderWidth(r); update();
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

QString
CQChartsPlot::
dataBackgroundColorStr() const
{
  return dataBorderObj_->backgroundColorStr();
}

void
CQChartsPlot::
setDataBackgroundColorStr(const QString &s)
{
  dataBorderObj_->setBackgroundColorStr(s);
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

QString
CQChartsPlot::
dataBorderColorStr() const
{
  return dataBorderObj_->borderColorStr();
}

void
CQChartsPlot::
setDataBorderColorStr(const QString &s)
{
  dataBorderObj_->setBorderColorStr(s);
}

double
CQChartsPlot::
dataBorderWidth() const
{
  return dataBorderObj_->borderWidth();
}

void
CQChartsPlot::
setDataBorderWidth(double r)
{
  dataBorderObj_->setBorderWidth(r); update();
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
  addProperty("", this, "visible"    );
  addProperty("", this, "rect"       );
  addProperty("", this, "range"      );
  addProperty("", this, "dataScaleX" );
  addProperty("", this, "dataScaleY" );
  addProperty("", this, "equalScale" );
  addProperty("", this, "followMouse");
  addProperty("", this, "overlay"    );
  addProperty("", this, "y1y2"       );
  addProperty("", this, "invertX"    );
  addProperty("", this, "invertY"    );
  addProperty("", this, "logX"       );
  addProperty("", this, "logY"       );
  addProperty("", this, "autoFit"    );
  addProperty("", this, "showBoxes"  );

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
}

void
CQChartsPlot::
addYAxis()
{
  yAxis_ = new CQChartsAxis(this, CQChartsAxis::Direction::VERTICAL, 0, 1);
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
  // if first plot then add all chained plot items to this plot's key
  if (! prevPlot()) {
    key()->clearItems();

    addKeyItems(key());

    CQChartsPlot *plot1 = nextPlot();

    while (plot1) {
      plot1->addKeyItems(key());

      plot1 = plot1->nextPlot();
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
    if      (isOverlay()) {
      CQChartsPlot *plot1 = firstPlot();

      while (plot1) {
        plot1->setDataRange(CQChartsGeom::Range(), /*update*/false);

        plot1->updateRange(/*update*/false);

        CQChartsGeom::BBox dataRange1 = plot1->calcDataRange(/*adjust*/false);

        dataRange += dataRange1;

        plot1 = plot1->nextPlot();
      }
    }
    else if (isY1Y2()) {
      CQChartsPlot *plot1 = firstPlot();

      dataRange = plot1->calcDataRange();
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

    if      (isOverlay()) {
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
  if (xmin_) dataRange_.setLeft  (*xmin_);
  if (ymin_) dataRange_.setBottom(*ymin_);
  if (xmax_) dataRange_.setRight (*xmax_);
  if (ymax_) dataRange_.setTop   (*ymax_);

  if (xAxis() && xAxis()->isIncludeZero())
    dataRange_.updateRange(0, dataRange_.ymid());

  if (yAxis() && yAxis()->isIncludeZero())
    dataRange_.updateRange(dataRange_.xmid(), 0);
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

  if (plotObjs_.empty()) {
    CQChartsNoDataObj *obj = new CQChartsNoDataObj(this);

    addPlotObject(obj);

    changed = true;
  }

  if (changed)
    initObjTree();

  if (changed && isAutoFit())
    autoFit();
}

void
CQChartsPlot::
initObjTree()
{
  plotObjTree_->addObjects(plotObjs_);
}

void
CQChartsPlot::
clearPlotObjects()
{
  for (auto &plotObj : plotObjs_)
    delete plotObj;

  plotObjs_.clear();

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
mousePress(const CQChartsGeom::Point &w, ModSelect modSelect)
{
  if (key() && key()->contains(w)) {
    CQChartsKeyItem *item = key()->getItemAt(w);

    bool handled = false;

    if (item)
      handled = item->mousePress(w);

    if (! handled)
      handled = key()->mousePress(w);

    if (handled)
      return true;
  }

  //---

  if (title() && title()->contains(w)) {
    if (title()->mousePress(w))
      return true;
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

    selectObj->mousePress();

    emit objPressed(selectObj);

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

    for (const auto &objSelected : objsSelected)
      if (objSelected.first->isSelected())
        objSelected.first->addSelectIndex();

    endSelect();

    update();
  }

  //---

  return selectObj;
}

bool
CQChartsPlot::
mouseMove(const CQChartsGeom::Point &w, bool first)
{
  if (key()) {
    bool handled = key()->mouseMove(w);

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

void
CQChartsPlot::
mouseRelease(const CQChartsGeom::Point &)
{
}

//------

bool
CQChartsPlot::
mouseDragPress(const CQChartsGeom::Point &p, const CQChartsGeom::Point &w)
{
  mouseData_.dragObj    = DragObj::NONE;
  mouseData_.pressPoint = CQChartsUtil::toQPoint(p);
  mouseData_.movePoint  = mouseData_.pressPoint;

  if      (key() && key()->contains(w)) {
    if (key()->mouseDragPress(w)) {
      mouseData_.dragObj = DragObj::KEY;
      return true;
    }
  }
  else if (xAxis() && xAxis()->contains(w)) {
    if (xAxis()->mouseDragPress(w)) {
      mouseData_.dragObj = DragObj::XAXIS;
      return true;
    }
  }
  else if (yAxis() && yAxis()->contains(w)) {
    if (yAxis()->mouseDragPress(w)) {
      mouseData_.dragObj = DragObj::YAXIS;
      return true;
    }
  }
  else if (title() && title()->contains(w)) {
    if (title()->mouseDragPress(w)) {
      mouseData_.dragObj = DragObj::TITLE;
      return true;
    }
  }
  else {
    // to drag plot must be inside a plot object
    PlotObjs objs;

    objsAtPoint(w, objs);

    if (! objs.empty()) {
      mouseData_.dragObj = DragObj::PLOT;
      return true;
    }

    // to resize must be in handle
    if      (llHandle_.inside(w)) {
      mouseData_.dragObj = DragObj::PLOT_LL;
      return true;
    }
    else if (lrHandle_.inside(w)) {
      mouseData_.dragObj = DragObj::PLOT_LR;
      return true;
    }
    else if (ulHandle_.inside(w)) {
      mouseData_.dragObj = DragObj::PLOT_UL;
      return true;
    }
    else if (urHandle_.inside(w)) {
      mouseData_.dragObj = DragObj::PLOT_UR;
      return true;
    }
  }

  return false;
}

bool
CQChartsPlot::
mouseDragMove(const CQChartsGeom::Point &p, const CQChartsGeom::Point &w, bool /*first*/)
{
  QPointF lastMovePoint = mouseData_.movePoint;

  mouseData_.movePoint = CQChartsUtil::toQPoint(p);

  if      (mouseData_.dragObj == DragObj::KEY) {
    (void) key()->mouseDragMove(w);
    return true;
  }
  else if (mouseData_.dragObj == DragObj::XAXIS) {
    (void) xAxis()->mouseDragMove(w);
    return true;
  }
  else if (mouseData_.dragObj == DragObj::YAXIS) {
    (void) yAxis()->mouseDragMove(w);
    return true;
  }
  else if (mouseData_.dragObj == DragObj::TITLE) {
    (void) title()->mouseDragMove(w);
    return true;
  }
  else if (mouseData_.dragObj == DragObj::PLOT) {
    double dx = mouseData_.movePoint.x() - lastMovePoint.x();
    double dy = lastMovePoint.y() - mouseData_.movePoint.y();

    double dx1 = CQChartsUtil::sign(dx)*view_->pixelToWindowWidth (dx);
    double dy1 = CQChartsUtil::sign(dy)*view_->pixelToWindowHeight(dy);

    bbox_.moveBy(CQChartsGeom::Point(dx1, dy1));

    updateMargin();

    return true;
  }
  else if (mouseData_.dragObj == DragObj::PLOT_LL) {
    double dx = mouseData_.movePoint.x() - lastMovePoint.x();
    double dy = lastMovePoint.y() - mouseData_.movePoint.y();

    double dx1 = CQChartsUtil::sign(dx)*view_->pixelToWindowWidth (dx);
    double dy1 = CQChartsUtil::sign(dy)*view_->pixelToWindowHeight(dy);

    bbox_.setLL(bbox_.getLL() + CQChartsGeom::Point(dx1, dy1));

    updateMargin();

    return true;
  }
  else if (mouseData_.dragObj == DragObj::PLOT_LR) {
    double dx = mouseData_.movePoint.x() - lastMovePoint.x();
    double dy = lastMovePoint.y() - mouseData_.movePoint.y();

    double dx1 = CQChartsUtil::sign(dx)*view_->pixelToWindowWidth (dx);
    double dy1 = CQChartsUtil::sign(dy)*view_->pixelToWindowHeight(dy);

    bbox_.setLR(bbox_.getLR() + CQChartsGeom::Point(dx1, dy1));

    updateMargin();

    return true;
  }
  else if (mouseData_.dragObj == DragObj::PLOT_UL) {
    double dx = mouseData_.movePoint.x() - lastMovePoint.x();
    double dy = lastMovePoint.y() - mouseData_.movePoint.y();

    double dx1 = CQChartsUtil::sign(dx)*view_->pixelToWindowWidth (dx);
    double dy1 = CQChartsUtil::sign(dy)*view_->pixelToWindowHeight(dy);

    bbox_.setUL(bbox_.getUL() + CQChartsGeom::Point(dx1, dy1));

    updateMargin();

    return true;
  }
  else if (mouseData_.dragObj == DragObj::PLOT_UR) {
    double dx = mouseData_.movePoint.x() - lastMovePoint.x();
    double dy = lastMovePoint.y() - mouseData_.movePoint.y();

    double dx1 = CQChartsUtil::sign(dx)*view_->pixelToWindowWidth (dx);
    double dy1 = CQChartsUtil::sign(dy)*view_->pixelToWindowHeight(dy);

    bbox_.setUR(bbox_.getUR() + CQChartsGeom::Point(dx1, dy1));

    updateMargin();

    return true;
  }

  return false;
}

void
CQChartsPlot::
mouseDragRelease(const CQChartsGeom::Point & /*p*/, const CQChartsGeom::Point & /*w*/)
{
  mouseData_.dragObj = DragObj::NONE;
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

    for (const auto &objSelected : objsSelected)
      if (objSelected.first->isSelected())
        objSelected.first->addSelectIndex();

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
setXValueColumn(int column)
{
  xValueColumn_ = column;

  if (xAxis())
    xAxis()->setColumn(xValueColumn_);
}

void
CQChartsPlot::
setYValueColumn(int column)
{
  yValueColumn_ = column;

  if (yAxis())
    yAxis()->setColumn(yValueColumn_);
}

//------

void
CQChartsPlot::
setIdColumn(int i)
{
  if (i != idColumn_) {
    idColumn_ = i;

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
columnStr(int column, double x) const
{
  if (column < 0)
    return CQChartsUtil::toString(x);

  QAbstractItemModel *model = this->model();

  if (! model)
    return CQChartsUtil::toString(x);

  CQChartsColumnTypeMgr *columnTypeMgr = charts()->columnTypeMgr();

  ColumnType         columnType;
  CQChartsNameValues nameValues;

  if (! columnTypeMgr->getModelColumnType(model, column, columnType, nameValues))
    return CQChartsUtil::toString(x);

  CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);

  if (! typeData)
    return CQChartsUtil::toString(x);

  return typeData->dataName(x, nameValues).toString();
}

void
CQChartsPlot::
keyPress(int key, int modifier)
{
  bool is_shift = (modifier & Qt::ShiftModifier  );
//bool is_ctrl  = (modifier & Qt::ControlModifier);

  if      (key == Qt::Key_Left || key == Qt::Key_Right ||
           key == Qt::Key_Up   || key == Qt::Key_Down) {
    double f = (! is_shift ? 0.125 : 0.25);

    if      (key == Qt::Key_Right)
      panLeft(f);
    else if (key == Qt::Key_Left)
      panRight(f);
    else if (key == Qt::Key_Up)
      panDown(f);
    else if ( key == Qt::Key_Down)
      panUp(f);
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

      drawSides(painter, plotRect, borderSides(), borderWidth(), borderColor);
    }
  }

  if (isDataBackground() || isDataBorder()) {
    QRectF dataRect = CQChartsUtil::toQRect(calcDataPixelRect());

    if (isDataBackground())
      painter->fillRect(dataRect, QBrush(interpDataBackgroundColor(0, 1)));

    if (isDataBorder()) {
      QColor borderColor = interpDataBorderColor(0, 1);

      drawSides(painter, dataRect, dataBorderSides(), dataBorderWidth(), borderColor);
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
  if (prevPlot())
    return;

  //---

  CQChartsGeom::BBox bbox = fitBBox();

  //---

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
}

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
  if (view()->mode() == CQChartsView::Mode::EDIT) {
    llHandle_.draw(painter);
    lrHandle_.draw(painter);
    ulHandle_.draw(painter);
    urHandle_.draw(painter);
  }
}

void
CQChartsPlot::
drawTextInBox(QPainter *painter, const QRectF &rect, const QString &text,
              const QPen &pen, bool contrast) const
{
  QFontMetricsF fm(painter->font());

  painter->setClipRect(rect);

  if (! contrast)
    painter->setPen(pen);

  //---

  QStringList strs;

  CQChartsUtil::formatStringInRect(text, painter->font(), rect, strs);

  //---

  double th = 0;

  for (int i = 0; i < strs.size(); ++i) {
    th += fm.height();
  }

  //---

  double dy = (rect.height() - th)/2.0;

  double y = rect.top() + dy + fm.ascent();

  for (int i = 0; i < strs.size(); ++i) {
    double dx = (rect.width() - fm.width(strs[i]))/2;

    double x = rect.left() + dx;

    if (contrast)
      drawContrastText(painter, x, y, strs[i], pen);
    else
      painter->drawText(x, y, strs[i]);

    y += fm.height();
  }
}

void
CQChartsPlot::
drawContrastText(QPainter *painter, double x, double y, const QString &text,
                 const QPen &pen) const
{
  painter->setPen(CQChartsUtil::invColor(pen.color()));

  painter->drawText(QPointF(x + 1, y + 1), text);

  painter->setPen(pen);

  painter->drawText(QPointF(x, y), text);
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
  if (brush.style() != Qt::NoBrush) {
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
  else {
    assert(pen.style() != Qt::NoPen);

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
textColor(const QColor &bg) const
{
  return CQChartsUtil::bwColor(bg);
}

QColor
CQChartsPlot::
interpThemeColor(double r) const
{
  CQChartsTheme *theme = view()->theme();

  QColor c = theme->theme()->getColor(r, /*scale*/true);

  return c;
}

//------

CQChartsPlot::ColumnType
CQChartsPlot::
columnValueType(QAbstractItemModel *model, int column) const
{
  assert(model);

  if (column < 0 || column >= model->columnCount())
    return ColumnType::NONE;

  //---

  // use defined column type if available
  CQChartsColumnTypeMgr *columnTypeMgr = charts()->columnTypeMgr();

  ColumnType         columnType;
  CQChartsNameValues nameValues;

  if (columnTypeMgr->getModelColumnType(model, column, columnType, nameValues))
    return columnType;

  //---

  // determine column type from values
  // TODO: cache ?

  // process model data
  class ColumnTypeVisitor : public ModelVisitor {
   public:
    ColumnTypeVisitor(int column) :
     column_(column) {
    }

    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
       QModelIndex ind = model->index(row, column_, parent);

      // if column can be integral, check if value is valid integer
      if (isInt_) {
        bool ok;

        (void) CQChartsUtil::modelInteger(model, ind, ok);

        if (ok)
          return State::SKIP;

        QString str = CQChartsUtil::modelString(model, ind, ok);

        if (! str.length())
          return State::SKIP;

        isInt_ = false;
      }

      // if column can be real, check if value is valid real
      if (isReal_) {
        bool ok;

        (void) CQChartsUtil::modelReal(model, ind, ok);

        if (ok)
          return State::SKIP;

        QString str = CQChartsUtil::modelString(model, ind, ok);

        if (! str.length())
          return State::SKIP;

        isReal_ = false;
      }

      // not value real or integer so assume string and we are done
      return State::TERMINATE;
    }

    ColumnType columnType() {
      if      (isInt_)
        return ColumnType::INTEGER;
      else if (isReal_)
        return ColumnType::REAL;
      else
        return ColumnType::STRING;
    }

   private:
    int  column_ { -1 };
    bool isInt_  { true };
    bool isReal_ { true };
  };

  // determine column value type
  ColumnTypeVisitor columnTypeVisitor(column);

  const_cast<CQChartsPlot *>(this)->visitModel(columnTypeVisitor);

  return columnTypeVisitor.columnType();
}

//------

bool
CQChartsPlot::
getHierColumnNames(int r, const Columns &nameColumns, const QString &separator,
                   QStringList &nameStrs, ModelIndices &nameInds)
{
  QAbstractItemModel *model = this->model();
  assert(model);

  // single column (seprated names)
  if (nameColumns.size() == 1) {
    int nameColumn = nameColumns[0];

    QModelIndex nameInd = model->index(r, nameColumn);

    //---

    bool ok;

    QString name = CQChartsUtil::modelString(model, nameInd, ok);

    if (ok && ! name.simplified().length())
      ok = false;

    if (ok) {
      if (separator.length())
        nameStrs = name.split(separator, QString::SkipEmptyParts);
      else
        nameStrs << name;
    }

    nameInds.push_back(nameInd);
  }
  else {
    for (auto &nameColumn : nameColumns) {
      QModelIndex nameInd = model->index(r, nameColumn);

      //---

      bool ok;

      QString name = CQChartsUtil::modelString(model, nameInd, ok);

      if (ok && ! name.simplified().length())
        ok = false;

      if (ok) {
        nameStrs << name;

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
  QAbstractItemModel *model = this->model();
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
  QAbstractItemModel *model = this->model();
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
  QAbstractItemModel *model = this->model();
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
  QAbstractItemModel *model = this->model();

  if (! model)
    return true;

  int nc = model->rowCount();

  for (int r = 0; r < nc; ++r) {
    QModelIndex index1 = model->index(r, 0);

    if (model->rowCount(index1) > 0)
      return true;
  }

  return false;
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

  CQChartsValueSet *valueSet = new CQChartsValueSet;

  valueSets_[name] = valueSet;

  return valueSet;
}

CQChartsValueSet *
CQChartsPlot::
addColorSet(const QString &name)
{
  assert(! getColorSet(name));

  CQChartsColorSet *colorSet = new CQChartsColorSet;

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

int
CQChartsPlot::
valueSetColumn(const QString &name) const
{
  CQChartsValueSet *valueSet = getValueSet(name);
  assert(valueSet);

  return valueSet->column();
}

bool
CQChartsPlot::
setValueSetColumn(const QString &name, int i)
{
  CQChartsValueSet *valueSet = getValueSet(name);
  assert(valueSet);

  if (valueSet->column() != i) {
    valueSet->setColumn(i);
    return true;
  }

  return false;
}

bool
CQChartsPlot::
isValueSetMapEnabled(const QString &name) const
{
  CQChartsValueSet *valueSet = getValueSet(name);
  assert(valueSet);

  return valueSet->isMapEnabled();
}

void
CQChartsPlot::
setValueSetMapEnabled(const QString &name, bool b)
{
  CQChartsValueSet *valueSet = getValueSet(name);
  assert(valueSet);

  valueSet->setMapEnabled(b);
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
    if (valueSet.second->column() >= 0) {
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
addValueSetRow(QAbstractItemModel *model, const QModelIndex &parent, int r)
{
  for (auto &valueSet : valueSets_) {
    int column = valueSet.second->column();

    if (column >= 0) {
      QModelIndex ind = model->index(r, column, parent);

      bool ok;

      QVariant value = CQChartsUtil::modelValue(model, ind, ok);

      valueSet.second->addValue(value); // always add some value
    }
  }
}

//------

void
CQChartsPlot::
initGroup(int groupColumn, const Columns &valueColumns, bool rowGrouping)
{
  groupBucket_.clear();

  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  // for row grouping we use the column header as the grouping id so all row
  // values in the column are added to the group
  if (valueColumns.size() > 1 && rowGrouping) {
    groupBucket_.setColumnType (ColumnType::INTEGER);
    groupBucket_.setDataType   (CQChartsColumnBucket::DataType::HEADER);
    groupBucket_.setRowGrouping(true);

    for (const auto &column : valueColumns) {
      bool ok;

      QString name = CQChartsUtil::modelHeaderValue(model, column, ok).toString();

      int ind = groupBucket_.addValue(column);

      groupBucket_.setIndName(ind, name);
    }

    return;
  }

  //---

  if (groupColumn >= 0) {
    ColumnType columnType = columnValueType(model, groupColumn);

    groupBucket_.setColumnType(columnType);
    groupBucket_.setColumn    (groupColumn);
    groupBucket_.setDataType  (CQChartsColumnBucket::DataType::COLUMN);
  }
  else {
    groupBucket_.setColumnType(ColumnType::STRING);
    groupBucket_.setDataType  (CQChartsColumnBucket::DataType::PATH);
  }

  // process model data
  class GroupVisitor : public ModelVisitor {
   public:
    GroupVisitor(CQChartsColumnBucket *bucket) :
     bucket_(bucket) {
    }

    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      if (bucket_->column() >= 0) {
        QModelIndex ind = model->index(row, bucket_->column(), parent);

        bool ok;

        QVariant value = CQChartsUtil::modelValue(model, ind, ok);

        bucket_->addValue(value);
      }
      else {
        QString path = CQChartsUtil::parentPath(model, parent);

        bucket_->addString(path);
      }

      return State::OK;
    }

   private:
    CQChartsColumnBucket *bucket_ { nullptr };
  };

  GroupVisitor groupVisitor(&groupBucket_);

  visitModel(groupVisitor);
}

int
CQChartsPlot::
rowGroupInd(QAbstractItemModel *model, const QModelIndex &parent, int row, int column) const
{
  // header has multiple groups (one per column)
  if (groupBucket_.dataType() == CQChartsColumnBucket::DataType::HEADER) {
    return groupBucket_.ind(column);
  }

  // get group id from value in group column
  if (groupBucket_.dataType() == CQChartsColumnBucket::DataType::COLUMN) {
    QModelIndex ind = model->index(row, groupBucket_.column(), parent);

    bool ok;

    QVariant value = CQChartsUtil::modelValue(model, ind, ok);

    return groupBucket_.ind(value);
  }

  // get group id from parent path name
  QString path = CQChartsUtil::parentPath(model, parent);

  return groupBucket_.ind(path);
}

//------

void
CQChartsPlot::
visitModel(ModelVisitor &visitor)
{
  visitor.setPlot(this);

  (void) CQChartsUtil::visitModel(model(), visitor);
}

//------

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
  QModelIndex ind1 = unnormalizeIndex(ind);

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

  QAbstractItemModel *model = this->model();
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

double
CQChartsPlot::
lengthPixelWidth(const CQChartsLength &len) const
{
  if      (len.units() == CQChartsLength::Units::PIXEL)
    return len.value();
  else if (len.units() == CQChartsLength::Units::PLOT)
    return windowToPixelWidth(len.value());
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

CQChartsUtil::ModelVisitor::State
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

//------

CQChartsHierPlot::
CQChartsHierPlot(CQChartsView *view, CQChartsPlotType *type, const ModelP &model) :
 CQChartsPlot(view, type, model)
{
  (void) addColorSet("color");

  nameColumns_.push_back(nameColumn_);
}

CQChartsHierPlot::
~CQChartsHierPlot()
{
}

//----

void
CQChartsHierPlot::
setNameColumn(int i)
{
  if (i != nameColumn_) {
    nameColumn_ = i;

    nameColumns_.clear();

    if (nameColumn_ >= 0)
      nameColumns_.push_back(nameColumn_);

    updateRangeAndObjs();
  }
}

void
CQChartsHierPlot::
setNameColumns(const Columns &nameColumns)
{
  nameColumns_ = nameColumns;

  if (! nameColumns_.empty())
    nameColumn_ = nameColumns_[0];
  else
    nameColumn_ = -1;

  updateRangeAndObjs();
}

QString
CQChartsHierPlot::
nameColumnsStr() const
{
  return CQChartsUtil::toString(nameColumns_);
}

bool
CQChartsHierPlot::
setNameColumnsStr(const QString &s)
{
  if (s != nameColumnsStr()) {
    std::vector<int> nameColumns;

    if (! CQChartsUtil::fromString(s, nameColumns))
      return false;

    setNameColumns(nameColumns);
  }

  return true;
}

void
CQChartsHierPlot::
setValueColumn(int i)
{
  if (i != valueColumn_) {
    valueColumn_ = i;

    updateRangeAndObjs();
  }
}

void
CQChartsHierPlot::
addProperties()
{
  addProperty("columns", this, "nameColumn" , "name" );
  addProperty("columns", this, "nameColumns", "names");
  addProperty("columns", this, "valueColumn", "value");
  addProperty("columns", this, "colorColumn", "color");

  addProperty("", this, "separator");

  addProperty("color", this, "colorMapEnabled", "mapEnabled");
  addProperty("color", this, "colorMapMin"    , "mapMin"    );
  addProperty("color", this, "colorMapMax"    , "mapMax"    );
}

//------

CQChartsResizeHandle::
CQChartsResizeHandle(CQChartsPlot *plot, Side side) :
 plot_(plot), side_(side)
{
}

void
CQChartsResizeHandle::
draw(QPainter *painter)
{
  double cs = 16;

  CQChartsGeom::BBox bbox = plot_->bbox();

  path_ = QPainterPath();

  if     (side() == Side::LL) {
    QPointF ll = CQChartsUtil::toQPoint(plot_->view()->windowToPixel(bbox.getLL()));

    path_.moveTo(ll.x()     , ll.y()     );
    path_.lineTo(ll.x() + cs, ll.y()     );
    path_.lineTo(ll.x()     , ll.y() - cs);
  }
  else if (side() == Side::LR) {
    QPointF lr = CQChartsUtil::toQPoint(plot_->view()->windowToPixel(bbox.getLR()));

    path_.moveTo(lr.x()     , lr.y()     );
    path_.lineTo(lr.x() - cs, lr.y()     );
    path_.lineTo(lr.x()     , lr.y() - cs);
  }
  else if (side() == Side::UL) {
    QPointF ul = CQChartsUtil::toQPoint(plot_->view()->windowToPixel(bbox.getUL()));

    path_.moveTo(ul.x()     , ul.y()     );
    path_.lineTo(ul.x() + cs, ul.y()     );
    path_.lineTo(ul.x()     , ul.y() + cs);
  }
  else if (side() == Side::UR) {
    QPointF ur = CQChartsUtil::toQPoint(plot_->view()->windowToPixel(bbox.getUR()));

    path_.moveTo(ur.x()     , ur.y()     );
    path_.lineTo(ur.x() - cs, ur.y()     );
    path_.lineTo(ur.x()     , ur.y() + cs);
  }
  else {
    return;
  }

  painter->setPen  (Qt::black);
  painter->setBrush(Qt::green);

  painter->drawPath(path_);
}

bool
CQChartsResizeHandle::
inside(const CQChartsGeom::Point &w) const
{
  QPointF p = plot_->windowToPixel(CQChartsUtil::toQPoint(w));

  return path_.contains(p);
}
