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
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQPropertyViewModel.h>
#include <CGradientPalette.h>
#include <CQChartsDisplayTransform.h>
#include <CQChartsDisplayRange.h>

#include <QItemSelectionModel>
#include <QSortFilterProxyModel>
#include <QPainter>
#include <iostream>

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

void
CQChartsPlotType::
addParameters()
{
  addBoolParameter("key", "Key", "keyVisible", "optional");
}

//------

CQChartsPlot::
CQChartsPlot(CQChartsView *view, CQChartsPlotType *type, const ModelP &model) :
 view_(view), type_(type), model_(model)
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

  updateTimer_.setSingleShot(true);

  connect(&updateTimer_, SIGNAL(timeout()), this, SLOT(updateTimerSlot()));
}

CQChartsPlot::
~CQChartsPlot()
{
  clearPlotObjects();

  delete plotObjTree_;

  delete displayRange_;
  delete displayTransform_;

  delete borderObj_;
  delete dataBorderObj_;

  delete titleObj_;
  delete keyObj_;
  delete xAxis_;
  delete yAxis_;
}

//---

void
CQChartsPlot::
modelDataChangedSlot(const QModelIndex & /*tl*/, const QModelIndex & /*br*/)
{
  // TODO: check if model uses changed columns
  //int column1 = tl.column();
  //int column2 = br.column();

  updateTimer_.start(100);
}

void
CQChartsPlot::
modelLayoutChangedSlot()
{
  updateTimer_.start(100);
}

void
CQChartsPlot::
modelRowsInsertedSlot()
{
  updateTimer_.start(100);
}

void
CQChartsPlot::
modelRowsRemovedSlot()
{
  updateTimer_.start(100);
}

void
CQChartsPlot::
modelColumnsInsertedSlot()
{
  updateTimer_.start(100);
}

void
CQChartsPlot::
modelColumnsRemovedSlot()
{
  updateTimer_.start(100);
}

void
CQChartsPlot::
updateTimerSlot()
{
  updateRangeAndObjs();
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
setDataRange(const CQChartsGeom::Range &r)
{
  if (r != dataRange_) {
    dataRange_ = r;

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
    title()->setText(titleStr_);
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

  updateMargin();
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

  if (xAxis())
    xAxis()->addProperties(propertyModel(), id() + "/" + "X Axis");

  if (yAxis())
    yAxis()->addProperties(propertyModel(), id() + "/" + "Y Axis");

  if (key())
    key()->addProperties(propertyModel(), id() + "/" + "Key");

  if (title())
    title()->addProperties(propertyModel(), id() + "/" + "Title");
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
  keyObj_ = new CQChartsKey(this);
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

  titleObj_->setText(titleStr());
}

void
CQChartsPlot::
updateObjs()
{
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
calcDataRange() const
{
  CQChartsGeom::BBox bbox;

  if (dataRange_.isSet())
    bbox = CQChartsGeom::BBox(dataRange_.xmin(), dataRange_.ymin(),
                              dataRange_.xmax(), dataRange_.ymax());
  else
    bbox = CQChartsGeom::BBox(0, 0, 1, 1);

  // adjust by zoom data pan offset, zoom scale
  if (view_->isZoomData()) {
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
        plot1->setDataRange(CQChartsGeom::Range());

        plot1->updateRange(/*update*/false);

        dataRange += plot1->calcDataRange();

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
        //plot1->setDataRange (dataRange1  );
        //plot1->setDataScaleX(dataScaleX());
        //plot1->setDataScaleY(dataScaleY());
        //plot1->setDataOffset(dataOffset());

        //plot1->applyDataRange(/*propagate*/false);

        while (plot1) {
          plot1->setDataRange (dataRange1  );
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
        plot2->setDataRange(CQChartsGeom::Range());

        plot2->updateRange(/*update*/false);

        CQChartsGeom::BBox bbox2 = plot2->calcDataRange();

        CQChartsGeom::Range dataRange2 =
          CQChartsGeom::Range(dataRange1.left (), bbox2.getYMin(),
                              dataRange1.right(), bbox2.getYMax());

        plot2->setDataRange (dataRange2  );
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

  insidePlotObjs_.clear();
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

    insidePlotObjs_.clear();

    for (const auto &obj : plotObjs_)
      obj->setInside(false);

    for (const auto &obj : objs)
      insidePlotObjs_.insert(obj);

    setInsidePlotObject();
  }

  return changed;
}

CQChartsPlotObj *
CQChartsPlot::
insidePlotObject() const
{
  int i = 0;

  for (auto &obj : insidePlotObjs_) {
    if (i == insidePlotInd_)
      return obj;

    ++i;
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

  for (auto obj : insidePlotObjs_) {
    if (obj->isInside()) {
      if (objText != "")
        objText += " ";

      objText += obj->id();
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

    selectObj = *objs.begin();
  }

  //---

  // change selection depending on selection modifier
  if (selectObj) {
    if      (modSelect == ModSelect::TOGGLE)
      objsSelected[selectObj] = ! selectObj->isSelected();
    else if(modSelect == ModSelect::REPLACE)
      objsSelected[selectObj] = true;
    else if (modSelect == ModSelect::ADD)
      objsSelected[selectObj] = true;
    else if (modSelect == ModSelect::REMOVE)
      objsSelected[selectObj] = false;

    //---

    selectObj->mousePress();

    emit objPressed(selectObj);
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
mouseDragPress(const CQChartsGeom::Point &w)
{
  mouseData_.dragObj = DragObj::NONE;

  if (key() && key()->contains(w)) {
    if (key()->mouseDragPress(w)) {
      mouseData_.dragObj = DragObj::KEY;
      return true;
    }
  }

  if (xAxis() && xAxis()->contains(w)) {
    if (xAxis()->mouseDragPress(w)) {
      mouseData_.dragObj = DragObj::XAXIS;
      return true;
    }
  }

  if (yAxis() && yAxis()->contains(w)) {
    if (yAxis()->mouseDragPress(w)) {
      mouseData_.dragObj = DragObj::YAXIS;
      return true;
    }
  }

  if (title() && title()->contains(w)) {
    if (title()->mouseDragPress(w)) {
      mouseData_.dragObj = DragObj::TITLE;
      return true;
    }
  }

  return false;
}

bool
CQChartsPlot::
mouseDragMove(const CQChartsGeom::Point &w, bool /*first*/)
{
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

  return false;
}

void
CQChartsPlot::
mouseDragRelease(const CQChartsGeom::Point & /*w*/)
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
clickZoom(const CQChartsGeom::Point &w)
{
  PlotObjs objs;

  objsAtPoint(w, objs);

  for (auto obj : objs)
    obj->clickZoom(w);
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

  CQBaseModel::Type  columnType;
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
keyPress(int key)
{
  if      (key == Qt::Key_Left || key == Qt::Key_Right) {
    if (key == Qt::Key_Right)
      panLeft();
    else
      panRight();
  }
  else if (key == Qt::Key_Up || key == Qt::Key_Down) {
    if (key == Qt::Key_Up)
      panDown();
    else
      panUp();
  }
  else if (key == Qt::Key_Plus) {
    zoomIn();
  }
  else if (key == Qt::Key_Minus) {
    zoomOut();
  }
  else if (key == Qt::Key_Home) {
    zoomFull();
  }
  else if (key == Qt::Key_Tab) {
    if (! insidePlotObjs_.empty()) {
      nextInsidePlotInd();

      setInsidePlotObject();

      QString objText = insidePlotObjectText();

      view_->setStatusText(objText);

      update();
    }
  }
  else
    return;
}

void
CQChartsPlot::
panLeft()
{
  if (view_->isZoomData()) {
    CQChartsGeom::BBox dataRange = calcDataRange();

    dataOffset_.setX(dataOffset_.x - dataRange.getWidth()/8);

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
panRight()
{
  if (view_->isZoomData()) {
    CQChartsGeom::BBox dataRange = calcDataRange();

    dataOffset_.setX(dataOffset_.x + dataRange.getWidth()/8);

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
panUp()
{
  if (view_->isZoomData()) {
    CQChartsGeom::BBox dataRange = calcDataRange();

    dataOffset_.setY(dataOffset_.y + dataRange.getHeight()/8);

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
panDown()
{
  if (view_->isZoomData()) {
    CQChartsGeom::BBox dataRange = calcDataRange();

    dataOffset_.setY(dataOffset_.y - dataRange.getHeight()/8);

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

    dataOffset_.setX(dataOffset_.x + dx);
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
    dataScaleX_ *= f;
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
    dataScaleX_ /= f;
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
  if (bbox.getWidth() < 1E-50 || bbox.getHeight() < 1E-50)
    return;

  if (view_->isZoomData()) {
    if (! dataRange_.isSet())
      return;

    double w = bbox.getWidth ();
    double h = bbox.getHeight();

    CQChartsGeom::Point c = bbox.getCenter();

    double w1 = dataRange_.xsize();
    double h1 = dataRange_.ysize();

    double xscale = w1/w;
    double yscale = h1/h;

    dataScaleX_ = std::min(xscale, yscale);
    dataScaleY_ = std::min(xscale, yscale);

    CQChartsGeom::Point c1 = CQChartsGeom::Point(dataRange_.xmid(), dataRange_.ymid());

    dataOffset_ = CQChartsGeom::Point(c.x - c1.x, c.y - c1.y);

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
    dataScaleX_ = 1.0;
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
  for (auto &obj : plotObjs_)
    obj->handleResize();

  updateKeyPosition(/*force*/true);
}

void
CQChartsPlot::
updateKeyPosition(bool force)
{
  if (! key())
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

  updateMargin();
}

CQChartsGeom::BBox
CQChartsPlot::
fitBBox() const
{
  CQChartsGeom::BBox bbox;

  bbox += dataFitBBox ();
  bbox += axesFitBBox ();
  bbox += keyFitBBox  ();
  bbox += titleFitBBox();

  addExtraFitBBox(bbox);

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

  if (xAxis())
    bbox += xAxis()->bbox();

  if (yAxis())
    bbox += yAxis()->bbox();

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
keyFitBBox() const
{
  CQChartsGeom::BBox bbox;

  if (key())
    bbox += key()->bbox();

  return bbox;
}

CQChartsGeom::BBox
CQChartsPlot::
titleFitBBox() const
{
  CQChartsGeom::BBox bbox;

  if (title())
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

    drawWindowRedBox(painter, bbox);

    drawWindowRedBox(painter, dataFitBBox   ());
    drawWindowRedBox(painter, axesFitBBox   ());
    drawWindowRedBox(painter, keyFitBBox    ());
    drawWindowRedBox(painter, titleFitBBox  ());
    drawWindowRedBox(painter, annotationBBox());
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
drawContrastText(QPainter *painter, double x, double y, const QString &text, const QPen &pen)
{
  painter->setPen(CQChartsUtil::invColor(pen.color()));

  painter->drawText(QPointF(x + 1, y + 1), text);

  painter->setPen(pen);

  painter->drawText(QPointF(x, y), text);
}

void
CQChartsPlot::
drawWindowRedBox(QPainter *painter, const CQChartsGeom::BBox &bbox)
{
  if (! bbox.isSet())
    return;

  CQChartsGeom::BBox prect;

  windowToPixel(bbox, prect);

  drawRedBox(painter, prect);
}

void
CQChartsPlot::
drawRedBox(QPainter *painter, const CQChartsGeom::BBox &bbox)
{
  painter->setPen(QColor(Qt::red));
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
  QColor c = palette()->getColor(r, scale);

  return c;
}

QColor
CQChartsPlot::
groupPaletteColor(double r1, double r2, double dr) const
{
  // r1 is parent color and r2 is child color
  QColor c1 = palette()->getColor(r1 - dr/2.0);
  QColor c2 = palette()->getColor(r1 + dr/2.0);

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
  QColor c = theme()->getColor(r, /*scale*/true);

  return c;
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
  itemSelection_ = QItemSelection();

  selIndexColumnRows_.clear();
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

  itemSelection_.select(ind1, ind1);

  selIndexColumnRows_[ind1.parent()][ind1.column()].insert(ind1.row());
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

  for (const auto &p : selIndexColumnRows_) {
    const QModelIndex &parent     = p.first;
    const ColumnRows  &columnRows = p.second;

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

//std::cerr << "Opt: " << optItemSelection.length() << " Std: " << itemSelection_.length() << "\n";
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
