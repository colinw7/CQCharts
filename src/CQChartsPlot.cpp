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
#include <CQChartsQuadTree.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQPropertyViewModel.h>
#include <CGradientPalette.h>
#include <CQChartsDisplayTransform.h>
#include <CQChartsDisplayRange.h>

#include <QItemSelectionModel>
#include <QSortFilterProxyModel>
#include <QPainter>

using PlotObjTree = CQChartsQuadTree<CQChartsPlotObj,CQChartsGeom::BBox>;

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
  assert(name == type->name());

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
  displayRange_     = new CQChartsDisplayRange;
  displayTransform_ = new CQChartsDisplayTransform(displayRange_);

  borderObj_     = new CQChartsBoxObj;
  dataBorderObj_ = new CQChartsBoxObj;

  plotObjTree_ = new PlotObjTree;

  setBackground    (true);
  setDataBackground(true);

  double vr = CQChartsView::viewportRange();

  bbox_ = CQChartsGeom::BBox(0, 0, vr, vr);

  displayRange_->setPixelRange(0, vr, vr, 0);

  displayRange_->setWindowRange(0, 0, 1, 1);
}

CQChartsPlot::
~CQChartsPlot()
{
  clearPlotObjects();

  delete static_cast<PlotObjTree *>(plotObjTree_);

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
setTitle(const QString &s)
{
  title_ = s;

  if (titleObj_)
    titleObj_->setText(title_);
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

const QColor &
CQChartsPlot::
backgroundColor() const
{
  return borderObj_->backgroundColor();
}

void
CQChartsPlot::
setBackgroundColor(const QColor &c)
{
  borderObj_->setBackgroundColor(c); update();
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

const QColor &
CQChartsPlot::
borderColor() const
{
  return borderObj_->borderColor();
}

void
CQChartsPlot::
setBorderColor(const QColor &c)
{
  borderObj_->setBorderColor(c); update();
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

const QColor &
CQChartsPlot::
dataBackgroundColor() const
{
  return dataBorderObj_->backgroundColor();
}

void
CQChartsPlot::
setDataBackgroundColor(const QColor &c)
{
  dataBorderObj_->setBackgroundColor(c); update();
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

const QColor &
CQChartsPlot::
dataBorderColor() const
{
  return dataBorderObj_->borderColor();
}

void
CQChartsPlot::
setDataBorderColor(const QColor &c)
{
  dataBorderObj_->setBorderColor(c); update();
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
  return (keyObj_ ? keyObj_->isVisible() : false);
}

void
CQChartsPlot::
setKeyVisible(bool b)
{
  if (keyObj_)
    keyObj_->setVisible(b);
}

//---

void
CQChartsPlot::
setEqualScale(bool b)
{
  equalScale_ = b;

  //dataRange_.reset();
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

  updateTitlePosition();

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
  assert(dataScale_ == 1.0);

  CQChartsGeom::BBox bbox = CQChartsUtil::fromQRect(r);

  CQChartsGeom::Range range(bbox.getXMin(), bbox.getYMin(), bbox.getXMax(), bbox.getYMax());

  //dataRange_.set(range);
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
  addProperty("", this, "equalScale" );
  addProperty("", this, "followMouse");
  addProperty("", this, "overlay"    );
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

  if (xAxis_)
    xAxis_->addProperties(propertyModel(), id() + "/" + "X Axis");

  if (yAxis_)
    yAxis_->addProperties(propertyModel(), id() + "/" + "Y Axis");

  if (keyObj_)
    keyObj_->addProperties(propertyModel(), id() + "/" + "Key");

  if (titleObj_)
    titleObj_->addProperties(propertyModel(), id() + "/" + "Title");
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
    keyObj_->clearItems();

    addKeyItems(keyObj_);

    CQChartsPlot *plot1 = nextPlot();

    while (plot1) {
      plot1->addKeyItems(keyObj_);

      plot1 = plot1->nextPlot();
    }
  }
}

void
CQChartsPlot::
addTitle()
{
  titleObj_ = new CQChartsTitle(this);

  titleObj_->setText(title_);
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

  CQChartsGeom::Point c = bbox.getCenter();
  double              w = 0.5*bbox.getWidth ()/dataScale_;
  double              h = 0.5*bbox.getHeight()/dataScale_;

  double x = c.x + dataOffset_.x;
  double y = c.y + dataOffset_.y;

  return CQChartsGeom::BBox(x - w, y - h, x + w, y + h);
}

void
CQChartsPlot::
applyDataRange(bool propagate)
{
  CQChartsGeom::BBox dataRange;

  if (propagate) {
    CQChartsPlot *plot1 = firstPlot();

    if (isOverlay()) {
      while (plot1) {
        plot1->setDataRange(CQChartsGeom::Range());

        plot1->updateRange(/*update*/false);

        dataRange += plot1->calcDataRange();

        plot1 = plot1->nextPlot();
      }
    }
    else
      dataRange = plot1->calcDataRange();
  }
  else {
    dataRange = calcDataRange();
  }

  displayRange_->setWindowRange(dataRange.getXMin(), dataRange.getYMin(),
                                dataRange.getXMax(), dataRange.getYMax());

  if (xAxis_) {
    xAxis_->setRange(dataRange.getXMin(), dataRange.getXMax());
    yAxis_->setRange(dataRange.getYMin(), dataRange.getYMax());
  }

  if (propagate) {
    CQChartsPlot *plot1 = firstPlot();

    if (isOverlay()) {
      CQChartsGeom::Range dataRange1 =
        CQChartsGeom::Range(dataRange.getXMin(), dataRange.getYMin(),
                            dataRange.getXMax(), dataRange.getYMax());

      if (plot1) {
        plot1->setDataRange (dataRange1 );
        plot1->setDataScale (dataScale_ );
        plot1->setDataOffset(dataOffset_);

        plot1->applyDataRange(/*propagate*/false);

        while (plot1) {
          plot1->setDataRange (dataRange1 );
          plot1->setDataScale (dataScale_ );
          plot1->setDataOffset(dataOffset_);

          plot1->applyDataRange(/*propagate*/false);

          plot1 = plot1->nextPlot();
        }
      }
    }
    else {
      if (plot1) {
#if 0
        CQChartsGeom::BBox bbox1(dataRange_.xmin(), dataRange_.ymin(),
                                 dataRange_.xmax(), dataRange_.ymax());
#endif

        while (plot1) {
          if (plot1 != this) {

#if 0
            CQChartsGeom::BBox bbox2;

            windowToPixel(bbox1, bbox2);

            CQChartsGeom::BBox bbox3;

            plot1->pixelToWindow(bbox2, bbox3);

            CQChartsGeom::Range dataRange(bbox3.getXMin(), bbox3.getYMin(),
                                          bbox3.getXMax(), bbox3.getYMax());

            plot1->setDataRange(dataRange);
#endif

            plot1->setDataScale (dataScale_ );
            plot1->setDataOffset(dataOffset_);

            plot1->applyDataRange(/*propagate*/false);
          }

          plot1 = plot1->nextPlot();
        }
      }
    }
  }

  updateKeyPosition(/*force*/true);

  updateTitlePosition();
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
addPlotObject(CQChartsPlotObj *obj)
{
  if (! obj->rect().isSet())
    return;

  static_cast<PlotObjTree *>(plotObjTree_)->add(obj);

  plotObjs_.push_back(obj);
}

void
CQChartsPlot::
clearPlotObjects()
{
  for (auto &plotObj : plotObjs_)
    delete plotObj;

  plotObjs_.clear();

  static_cast<PlotObjTree *>(plotObjTree_)->reset();
}

bool
CQChartsPlot::
mousePress(const CQChartsGeom::Point &w)
{
  if (keyObj_ && keyObj_->contains(w)) {
    CQChartsKeyItem *item = keyObj_->getItemAt(w);

    bool handled = false;

    if (item)
      handled = item->mousePress(w);

    if (! handled)
      handled = keyObj_->mousePress(w);

    if (handled)
      return true;
  }

  //---

  if (titleObj_ && titleObj_->contains(w)) {
    if (titleObj_->mousePress(w))
      return true;
  }

  //---

  // init all objects to unselected
  using ObjsSelected = std::map<CQChartsPlotObj*,bool>;

  ObjsSelected objsSelected;

  for (auto &plotObj : plotObjs_)
    objsSelected[plotObj] = false;

  //---

  // get selected objects and toggle selected
  PlotObjTree::DataList dataList;

  objsAtPoint(w, dataList);

  for (auto obj : dataList) {
    objsSelected[obj] = ! obj->isSelected();

    obj->mousePress(w);

    emit objPressed(obj);
  }

  //---

  bool changed = false;

  for (const auto &objSelected : objsSelected) {
    if (objSelected.first->isSelected() == objSelected.second)
      continue;

    objSelected.first->setSelected(objSelected.second);

    changed = true;
  }

  if (changed)
    update();

  //---

  return ! dataList.empty();
}

bool
CQChartsPlot::
mouseMove(const CQChartsGeom::Point &w, bool first)
{
  if (keyObj_) {
    if (keyObj_->contains(w)) {
      CQChartsKeyItem *item = keyObj_->getItemAt(w);

      bool handled = false;
      bool changed = false;

      if (item) {
        changed = keyObj_->setInside(item);

        handled = item->mouseMove(w);
      }

      if (! handled)
        handled = keyObj_->mouseMove(w);

      if (changed)
        update();

      if (handled)
        return true;
    }
    else {
      bool changed = keyObj_->setInside(nullptr);

      if (changed)
        update();
    }
  }

  //---

  QString posText = xStr(w.x) + " " + yStr(w.y);

  //---

  QString objText;

  PlotObjTree::DataList dataList;

  if (isFollowMouse()) {
    objsAtPoint(w, dataList);

    for (auto obj : dataList) {
      if (objText != "")
        objText += " ";

      objText += obj->id();
    }
  }

  //---

  if (first) {
    if (objText != "")
      view_->setStatusText(objText + " : " + posText);
    else
      view_->setStatusText(posText);
  }

  //---

  if (isFollowMouse()) {
    bool changed = false;

    auto resetInside = [&] (CQChartsPlotObj *obj) -> void {
      for (const auto &obj1 : dataList) {
        if (obj1 == obj) {
          if (! obj->isInside()) {
            obj->setInside(true);

            changed = true;
          }

          return;
        }
      }

      if (obj->isInside()) {
        obj->setInside(false);

        changed = true;
      }
    };

    static_cast<PlotObjTree *>(plotObjTree_)->process(resetInside);

    if (changed)
      update();
  }

  //---

  return false;
}

void
CQChartsPlot::
mouseRelease(const CQChartsGeom::Point &)
{
}

void
CQChartsPlot::
clickZoom(const CQChartsGeom::Point &w)
{
  PlotObjTree::DataList dataList;

  objsAtPoint(w, dataList);

  for (auto obj : dataList)
    obj->clickZoom(w);
}

void
CQChartsPlot::
setXValueColumn(int column)
{
  xValueColumn_ = column;

  if (xAxis_)
    xAxis_->setColumn(xValueColumn_);
}

void
CQChartsPlot::
setYValueColumn(int column)
{
  yValueColumn_ = column;

  if (yAxis_)
    yAxis_->setColumn(yValueColumn_);
}

QString
CQChartsPlot::
xStr(double x) const
{
  return columnStr(xValueColumn(), x);
}

QString
CQChartsPlot::
yStr(double y) const
{
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
zoomIn(double f)
{
  if (view_->isZoomData()) {
    dataScale_ *= 1.5;

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
    dataScale_ /= 1.5;

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

    dataScale_ = std::min(xscale, yscale);

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
    dataScale_  = 1.0;
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
  PlotObjTree::DataList dataList;

  objsAtPoint(p, dataList);

  if (dataList.empty())
    return false;

  for (auto obj : dataList) {
    if (! obj->visible())
      continue;

    if (tip != "")
      tip += " ";

    tip += obj->id();
  }

  return tip.length();
}

void
CQChartsPlot::
objsAtPoint(const CQChartsGeom::Point &p, std::list<CQChartsPlotObj *> &objs) const
{
  PlotObjTree::DataList dataList;

  static_cast<PlotObjTree *>(plotObjTree_)->dataAtPoint(p.x, p.y, dataList);

  for (const auto &obj : dataList) {
    if (obj->inside(p))
      objs.push_back(obj);
  }
}

void
CQChartsPlot::
handleResize()
{
  updateKeyPosition(/*force*/true);

  updateTitlePosition();

  for (auto &obj : plotObjs_)
    obj->handleResize();
}

void
CQChartsPlot::
updateKeyPosition(bool force)
{
  if (! keyObj_)
    return;

  if (force)
    keyObj_->invalidateLayout();

  CQChartsGeom::BBox bbox = calcDataRange();

  if (! bbox.isSet())
    return;

  QSizeF ks = keyObj_->calcSize();

  CQChartsKey::Location location = keyObj_->location();

  double xm = pixelToWindowWidth (8);
  double ym = pixelToWindowHeight(8);

  double kx { 0.0 }, ky { 0.0 };

  if      (location == CQChartsKey::Location::TOP_LEFT ||
           location == CQChartsKey::Location::CENTER_LEFT ||
           location == CQChartsKey::Location::BOTTOM_LEFT) {
    if (keyObj_->isInsideX())
      kx = bbox.getXMin() + xm;
    else
      kx = bbox.getXMin() - ks.width() - xm;
  }
  else if (location == CQChartsKey::Location::TOP_CENTER ||
           location == CQChartsKey::Location::CENTER_CENTER ||
           location == CQChartsKey::Location::BOTTOM_CENTER) {
    kx = bbox.getXMid() - ks.width()/2;
  }
  else if (location == CQChartsKey::Location::TOP_RIGHT ||
           location == CQChartsKey::Location::CENTER_RIGHT ||
           location == CQChartsKey::Location::BOTTOM_RIGHT) {
    if (keyObj_->isInsideX())
      kx = bbox.getXMax() - ks.width() - xm;
    else
      kx = bbox.getXMax() + xm;
  }

  if      (location == CQChartsKey::Location::TOP_LEFT ||
           location == CQChartsKey::Location::TOP_CENTER ||
           location == CQChartsKey::Location::TOP_RIGHT) {
    if (keyObj_->isInsideY())
      ky = bbox.getYMax() - ym;
    else
      ky = bbox.getYMax() + ks.height() + ym;
  }
  else if (location == CQChartsKey::Location::CENTER_LEFT ||
           location == CQChartsKey::Location::CENTER_CENTER ||
           location == CQChartsKey::Location::CENTER_RIGHT) {
    ky = bbox.getYMid() - ks.height()/2;
  }
  else if (location == CQChartsKey::Location::BOTTOM_LEFT ||
           location == CQChartsKey::Location::BOTTOM_CENTER ||
           location == CQChartsKey::Location::BOTTOM_RIGHT) {
    if (keyObj_->isInsideY())
      ky = bbox.getYMin() + ks.height() + ym;
    else
      ky = bbox.getYMin() - ym;
  }

  keyObj_->setPosition(QPointF(kx, ky));
}

void
CQChartsPlot::
updateTitlePosition()
{
  if (! titleObj_)
    return;

  CQChartsGeom::BBox bbox = calcDataRange();

  if (! bbox.isSet())
    return;

  QSizeF ts = titleObj_->calcSize();

  CQChartsTitle::Location location = titleObj_->location();

//double xm = pixelToWindowWidth (8);
  double ym = pixelToWindowHeight(8);

  double kx = bbox.getXMid() - ts.width()/2;

  double ky = 0.0;

  if      (location == CQChartsTitle::Location::TOP) {
    if (! titleObj_->isInside()) {
      ky = bbox.getYMax() + ym;

      if (xAxis_ && xAxis_->side() == CQChartsAxis::Side::TOP_RIGHT)
        ky += xAxis_->bbox().getHeight();
    }
    else
      ky = bbox.getYMax() - ts.height() - ym;
  }
  else if (location == CQChartsTitle::Location::CENTER) {
    ky = bbox.getYMid() - ts.height()/2;
  }
  else if (location == CQChartsTitle::Location::BOTTOM) {
    if (! titleObj_->isInside()) {
      ky = bbox.getYMin() - ts.height() - ym;

      if (xAxis_ && xAxis_->side() == CQChartsAxis::Side::BOTTOM_LEFT)
        ky -= xAxis_->bbox().getHeight();
    }
    else
      ky = bbox.getYMin() + ym;
  }
  else {
    ky = bbox.getYMid() - ts.height()/2;
  }

  titleObj_->setPosition(QPointF(kx, ky));
}

void
CQChartsPlot::
drawBackground(QPainter *painter)
{
  QRectF plotRect = CQChartsUtil::toQRect(calcPixelRect());
  QRectF dataRect = calcRect();

  if (isBackground())
    painter->fillRect(plotRect, QBrush(backgroundColor()));

  if (isBorder()) {
    drawSides(painter, plotRect, borderSides(), borderWidth(), borderColor());
  }

  if (isDataBackground())
    painter->fillRect(dataRect, QBrush(dataBackgroundColor()));

  if (isDataBorder()) {
    drawSides(painter, dataRect, dataBorderSides(), dataBorderWidth(), dataBorderColor());
  }
}

void
CQChartsPlot::
drawSides(QPainter *painter, const QRectF &rect, const QString &sides,
          double width, const QColor &color)
{
  QPen pen(color);

  pen.setWidth(width);

  painter->setPen(pen);
  painter->setBrush(Qt::NoBrush);

  if (sides.indexOf('t') >= 0) painter->drawLine(rect.topLeft   (), rect.topRight   ());
  if (sides.indexOf('l') >= 0) painter->drawLine(rect.topLeft   (), rect.bottomLeft ());
  if (sides.indexOf('b') >= 0) painter->drawLine(rect.bottomLeft(), rect.bottomRight());
  if (sides.indexOf('r') >= 0) painter->drawLine(rect.topRight  (), rect.bottomRight());
}

QRectF
CQChartsPlot::
calcRect() const
{
  double xmin, ymin, xmax, ymax;

  displayRange_->getWindowRange(&xmin, &ymin, &xmax, &ymax);

  double pxmin, pymin, pxmax, pymax;

  windowToPixel(xmin, ymin, pxmin, pymax);
  windowToPixel(xmax, ymax, pxmax, pymin);

  return QRectF(pxmin, pymin, pxmax - pxmin - 1, pymax - pymin - 1);
}

CQChartsGeom::BBox
CQChartsPlot::
calcPixelRect() const
{
  //double xmin, ymin, xmax, ymax;

  //displayRange_->getPixelRange(&xmin, &ymin, &xmax, &ymax);

  //double px1, py1, px2, py2;

  //view_->windowToPixel(xmin, ymin, px1, py1);
  //view_->windowToPixel(xmax, ymax, px2, py2);

  //return CQChartsGeom::BBox(px1, py1, px2, py2);

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
  double xmin, ymin, xmax, ymax;

  displayRange_->getWindowRange(&xmin, &ymin, &xmax, &ymax);

  CQChartsGeom::BBox pbbox(xmin, ymin, xmax, ymax);

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
  double xmin, ymin, xmax, ymax;

  displayRange_->getWindowRange(&xmin, &ymin, &xmax, &ymax);

  CQChartsGeom::BBox pbbox(xmin, ymin, xmax, ymax);

  CQChartsGeom::BBox bbox = pbbox;

  if (xAxis_)
    bbox += xAxis_->bbox();

  if (yAxis_)
    bbox += yAxis_->bbox();

  if (keyObj_)
    bbox += keyObj_->bbox();

  if (titleObj_)
    bbox += titleObj_->bbox();

  double xm = pixelToWindowWidth (8);
  double ym = pixelToWindowHeight(8);

  bbox.expand(-xm, -ym, xm, ym);

  return bbox;
}

//------

void
CQChartsPlot::
drawParts(QPainter *p)
{
  drawBackground(p);

  //---

  drawBgAxes(p);
  drawBgKey(p);

  drawObjs(p, Layer::BG );
  drawObjs(p, Layer::MID);
  drawObjs(p, Layer::FG );

  drawFgAxes(p);
  drawFgKey(p);

  //---

  drawTitle(p);

  //---

  drawForeground(p);
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

  CQChartsPlot *plot1 = firstPlot();

  if      (plot1->isDataClip()) {
    QRectF dataRect = calcRect();

    painter->setClipRect(dataRect, Qt::ReplaceClip);
  }
  else if (plot1->isClip()) {
    QRectF plotRect = CQChartsUtil::toQRect(calcPixelRect());

    painter->setClipRect(plotRect, Qt::ReplaceClip);
  }

  for (const auto &plotObj : plotObjs_)
    plotObj->draw(painter, layer);

  painter->restore();
}

void
CQChartsPlot::
drawBgAxes(QPainter *painter)
{
  if (xAxis_ && xAxis_->isVisible() && ! xAxis_->isGridAbove())
    xAxis_->drawGrid(this, painter);

  if (yAxis_ && yAxis_->isVisible() && ! yAxis_->isGridAbove())
    yAxis_->drawGrid(this, painter);
}

void
CQChartsPlot::
drawFgAxes(QPainter *painter)
{
  if (xAxis_ && xAxis_->isVisible() && xAxis_->isGridAbove())
    xAxis_->drawGrid(this, painter);

  if (yAxis_ && yAxis_->isVisible() && yAxis_->isGridAbove())
    yAxis_->drawGrid(this, painter);

  //---

  if (xAxis_ && xAxis_->isVisible())
    xAxis_->draw(this, painter);

  if (yAxis_ && yAxis_->isVisible())
    yAxis_->draw(this, painter);
}

void
CQChartsPlot::
drawBgKey(QPainter *painter)
{
  if (keyObj_ && ! keyObj_->isAbove())
    drawKey(painter);
}

void
CQChartsPlot::
drawFgKey(QPainter *painter)
{
  if (keyObj_ && keyObj_->isAbove())
    drawKey(painter);
}

void
CQChartsPlot::
drawKey(QPainter *painter)
{
  CQChartsPlot *plot1 = firstPlot();

  if (! plot1->keyObj_)
    return;

  // draw key under first plot
  if (! plot1->keyObj_->isAbove()) {
    if (plot1 == this)
      plot1->keyObj_->draw(painter);
  }
  // draw key above last plot
  else {
    if (lastPlot() == this)
      plot1->keyObj_->draw(painter);
  }
}

void
CQChartsPlot::
drawTitle(QPainter *painter)
{
  if (titleObj_)
    titleObj_->draw(painter);
}

void
CQChartsPlot::
drawContrastText(QPainter *p, double x, double y, const QString &text, const QPen &pen)
{
  p->setPen(CQChartsUtil::invColor(pen.color()));

  p->drawText(x + 1, y + 1, text);

  p->setPen(pen);

  p->drawText(x, y, text);
}

void
CQChartsPlot::
drawWindowRedBox(QPainter *painter, const CQChartsGeom::BBox &bbox)
{
  CQChartsGeom::BBox prect;

  windowToPixel(bbox, prect);

  drawRedBox(painter, prect);
}

void
CQChartsPlot::
drawRedBox(QPainter *painter, const CQChartsGeom::BBox &bbox)
{
  painter->setPen(Qt::red);
  painter->setBrush(Qt::NoBrush);

  painter->drawRect(CQChartsUtil::toQRect(bbox));
}

//------

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
  // stroke and fill
  if (brush.style() != Qt::NoBrush) {
    QColor pc = pen  .color();
    QColor bc = brush.color();

    // inside first (low priorty)
    if (obj->isInside()) {
      if (view()->insideMode() == CQChartsView::InsideMode::OUTLINE) {
        pen.setColor(CQChartsUtil::invColor(pc));
        pen.setWidth(1);
      }
      else
        brush.setColor(insideColor(bc));
    }

    // selected last (high priority)
    if (obj->isSelected()) {
      if (view()->selectedMode() == CQChartsView::SelectedMode::OUTLINE) {
        pen.setColor(CQChartsUtil::invColor(pc));
        pen.setWidth(2);
      }
      else
        brush.setColor(CQChartsUtil::invColor(bc));
    }
  }
  // just stroke
  else {
    QColor pc = pen.color();

    // inside first (low priorty)
    if (obj->isInside()) {
      pen.setColor(CQChartsUtil::invColor(pc));
      pen.setWidth(2);
    }

    // selected last (high priority)
    if (obj->isSelected()) {
      pen.setColor(CQChartsUtil::invColor(pc));
      pen.setWidth(4);
    }
  }
}

#if 0
QColor
CQChartsPlot::
objectColor(CQChartsPlotObj *obj, int i, int n, const QColor &def) const
{
  QColor c = paletteColor(i, n, def);

  return objectStateColor(obj, c);
}
#endif

#if 0
QColor
CQChartsPlot::
objectStateColor(CQChartsPlotObj *obj, const QColor &c) const
{
  QColor c1 = c;

  if (obj->isInside())
    c1 = insideColor(c1);

  return c1;
}
#endif

QColor
CQChartsPlot::
insideColor(const QColor &c) const
{
  return CQChartsUtil::blendColors(c, CQChartsUtil::bwColor(c), 0.8);
}

QColor
CQChartsPlot::
paletteColor(int i, int n, const QColor &def) const
{
  double r = CQChartsUtil::norm(i + 1, 0, n + 1);

  return interpPaletteColor(r, def);
}

QColor
CQChartsPlot::
interpPaletteColor(double r, const QColor &def) const
{
  if (! palette())
    return def;

  QColor c = palette()->getColor(r);

  return c;
}

QColor
CQChartsPlot::
groupPaletteColor(double r1, double r2, double dr, const QColor &def) const
{
  if (! palette())
    return def;

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
}

void
CQChartsPlot::
endSelect()
{
  QItemSelectionModel *sm = this->selectionModel();
  if (! sm) return;

  disconnect(sm, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
             this, SLOT(selectionSlot()));

  sm->select(itemSelection_, QItemSelectionModel::ClearAndSelect);

  connect(sm, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this, SLOT(selectionSlot()));
}

//------

void
CQChartsPlot::
windowToPixel(double wx, double wy, double &px, double &py) const
{
  double wx1, wy1;

  displayTransform_->getMatrix().multiplyPoint(wx, wy, &wx1, &wy1);

  double wx2, wy2;

  displayRange_->windowToPixel(wx1, wy1, &wx2, &wy2);

  view_->windowToPixel(wx2, wy2, px, py);
}

void
CQChartsPlot::
pixelToWindow(double px, double py, double &wx, double &wy) const
{
  double wx1, wy1;

  view_->pixelToWindow(px, py, wx1, wy1);

  double wx2, wy2;

  displayRange_->pixelToWindow(wx1, wy1, &wx2, &wy2);

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
