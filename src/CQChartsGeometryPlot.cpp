#include <CQChartsGeometryPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelUtil.h>
#include <CQChartsPolygonList.h>
#include <CQChartsPath.h>
#include <CQChartsDataLabel.h>
#include <CQCharts.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

CQChartsGeometryPlotType::
CQChartsGeometryPlotType()
{
}

void
CQChartsGeometryPlotType::
addParameters()
{
  startParameterGroup("Geometry");

  addColumnParameter("geometry", "Geometry", "geometryColumn").
   setRequired().setTip("Polygon List, Polygon, Rect or Path geometry");
  addColumnParameter("value", "Value", "valueColumn").setRequired();

  addColumnParameter("name" , "Name" , "nameColumn" );
  addColumnParameter("style", "Style", "styleColumn");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsGeometryPlotType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto BR  = []() { return CQChartsHtml::Str(CQChartsHtml::Str::Type::BR); };
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Geometry Plot").
    h3("Summary").
     p("Draws polygon list, polygon, rect or path shapes.").
    h3("Columns").
     p("The shape geometry is specified in the " + B("Geometry") + " column.").
     p("The optional shape name can be specified in the " + B("Name") + " column.").
     p("The optional shape value can be specified in the " + B("Value") + " column "
       "and can be used to color the shape by enabling the " + B("colorByValue") + " option." +
       BR() + "This value can be normalized using the " + B("minValue") + " and " +
       B("maxValue") + " values.").
     p("The optional style (fill, stroke) can be specified in the " + B("Style") + " column.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/geometryplot.png"));
}

bool
CQChartsGeometryPlotType::
isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                     CQChartsPlotParameter *parameter) const
{
  if (parameter->name() == "geometry") {
    if (columnDetails->type() == CQChartsPlot::ColumnType::RECT ||
        columnDetails->type() == CQChartsPlot::ColumnType::POLYGON ||
        columnDetails->type() == CQChartsPlot::ColumnType::POLYGON_LIST ||
        columnDetails->type() == CQChartsPlot::ColumnType::PATH)
      return true;

    return false;
  }

  return CQChartsPlotType::isColumnForParameter(columnDetails, parameter);
}

CQChartsPlot *
CQChartsGeometryPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsGeometryPlot(view, model);
}

//------

CQChartsGeometryPlot::
CQChartsGeometryPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("geometry"), model),
 CQChartsObjShapeData<CQChartsGeometryPlot>(this)
{
  NoUpdate noUpdate(this);

  dataLabel_ = new CQChartsDataLabel(this);

  setFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setFilled (true);
  setStroked(true);

  setLayerActive(CQChartsLayer::Type::FG_PLOT, true);

  addAxes();

  addTitle();
}

CQChartsGeometryPlot::
~CQChartsGeometryPlot()
{
  delete dataLabel_;
}

//---

void
CQChartsGeometryPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGeometryPlot::
setGeometryColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(geometryColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGeometryPlot::
setValueColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGeometryPlot::
setStyleColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(styleColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsGeometryPlot::
setColorByValue(bool b)
{
  CQChartsUtil::testAndSet(colorByValue_, b, [&]() { drawObjs(); } );
}

//---

double
CQChartsGeometryPlot::
minValue() const
{
  return minValue_.value_or(valueRange_.min(0.0));
}

void
CQChartsGeometryPlot::
setMinValue(double r)
{
  if (! minValue_ || r != minValue_.value()) {
    minValue_ = r; drawObjs();
  }
}

double
CQChartsGeometryPlot::
maxValue() const
{
  return maxValue_.value_or(valueRange_.max(1.0));
}

void
CQChartsGeometryPlot::
setMaxValue(double r)
{
  if (! maxValue_ || r != maxValue_.value()) {
    maxValue_ = r; drawObjs();
  }
}

//---

void
CQChartsGeometryPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  CQChartsPlot::addProperties();

  // columns
  addProp("columns", "nameColumn"    , "name"    , "Name column");
  addProp("columns", "geometryColumn", "geometry", "Geometry column");
  addProp("columns", "valueColumn"   , "value"   , "Value column");
  addProp("columns", "styleColumn"   , "style"   , "Style column");

  // coloring
  addProp("coloring", "colorByValue", "colorByValue", "Color shapes by value");

  // fill
  addProp("fill", "filled", "visible", "Fill visible");

  addFillProperties("fill", "fill", "");

  // stroke
  addProp("stroke", "stroked", "visible", "Stroke visible");

  addLineProperties("stroke", "stroke", "");

  // data label
  dataLabel_->addPathProperties("labels", "Labels");

  // value normalization
  addProp("value", "minValue", "min", "Min value for color map");
  addProp("value", "maxValue", "max", "Max value for color map");
}

void
CQChartsGeometryPlot::
getPropertyNames(QStringList &names, bool hidden) const
{
  CQChartsPlot::getPropertyNames(names, hidden);

  propertyModel()->objectNames(dataLabel_, names, hidden);
}

//---

CQChartsGeom::Range
CQChartsGeometryPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsGeometryPlot::calcRange");

  CQChartsGeom::Range dataRange;

  QAbstractItemModel *model = this->model().data();

  if (! model)
    return dataRange;

  //---

  CQChartsGeometryPlot *th = const_cast<CQChartsGeometryPlot *>(this);

  th->geometries_.clear();

  th->valueRange_ = CQChartsGeom::RMinMax();

  //---

  th->geometryColumnType_ = columnValueType(geometryColumn());
  th->colorColumnType_    = columnValueType(colorColumn());
  th->styleColumnType_    = columnValueType(styleColumn());

  //---

  // process model data
  class GeometryPlotVisitor : public ModelVisitor {
   public:
    GeometryPlotVisitor(const CQChartsGeometryPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *model, const VisitData &data) override {
      plot_->addRow(model, data, dataRange_);

      return State::OK;
    }

    const CQChartsGeom::Range &dataRange() const { return dataRange_; }

   private:
    const CQChartsGeometryPlot *plot_ { nullptr };
    CQChartsGeom::Range         dataRange_;
  };

  GeometryPlotVisitor geometryPlotVisitor(this);

  visitModel(geometryPlotVisitor);

  return geometryPlotVisitor.dataRange();
}

void
CQChartsGeometryPlot::
addRow(const QAbstractItemModel *model, const ModelVisitor::VisitData &data,
       CQChartsGeom::Range &dataRange) const
{
  CQChartsGeometryPlot *th = const_cast<CQChartsGeometryPlot *>(this);

  //---

  Geometry geometry;

  //---

  // get geometry name
  bool ok1;

  geometry.name = modelString(data.row, nameColumn(), data.parent, ok1);

  //---

  // decode geometry column value into polygons
  if (geometryColumnType_ == ColumnType::RECT ||
      geometryColumnType_ == ColumnType::POLYGON ||
      geometryColumnType_ == ColumnType::POLYGON_LIST ||
      geometryColumnType_ == ColumnType::PATH) {
    bool ok2;

    QVariant var = modelValue(data.row, geometryColumn(), data.parent, ok2);

    bool converted;

    QVariant rvar =
      CQChartsModelUtil::columnUserData(charts(), model, geometryColumn(), var, converted);

    QPolygonF poly;

    if      (geometryColumnType_ == ColumnType::RECT) {
      QRectF r = rvar.value<QRectF>();

      poly = QPolygonF(r);

      geometry.polygons.push_back(poly);
    }
    else if (geometryColumnType_ == ColumnType::POLYGON) {
      poly = rvar.value<QPolygonF>();

      geometry.polygons.push_back(poly);
    }
    else if (geometryColumnType_ == ColumnType::POLYGON_LIST) {
      CQChartsPolygonList polyList = rvar.value<CQChartsPolygonList>();

      for (const auto &poly : polyList.polygons())
        geometry.polygons.push_back(poly);
    }
    else if (geometryColumnType_ == ColumnType::PATH) {
      CQChartsPath path = rvar.value<CQChartsPath>();

      poly = path.path().toFillPolygon();

      geometry.polygons.push_back(poly);
    }
    else {
      assert(false);
    }
  }
  else {
    bool ok2;

    QString geomStr = modelString(data.row, geometryColumn(), data.parent, ok2);

    if (! decodeGeometry(geomStr, geometry.polygons)) {
      if (! isPreview())
        charts()->errorMsg("Invalid geometry '" + geomStr + "' for '" + geometry.name + "'");
      return;
    }
  }

  //---

  // update range from polygons
  for (auto &poly : geometry.polygons) {
    for (int j = 0; j < poly.count(); ++j) {
      const QPointF &p = poly[j];

      dataRange.updateRange(p.x(), p.y());

      geometry.bbox.add(p.x(), p.y());
    }
  }

  //---

  // get geometry associated value
  bool ok3;

  double value = modelReal(data.row, valueColumn(), data.parent, ok3);

  if (ok3 && ! CMathUtil::isNaN(value))
    geometry.value = value;

  // update value range
  if (geometry.value)
    th->valueRange_.add(*geometry.value);

  //---

  // get geometry custom color
  if (colorColumn().isValid()) {
    if (colorColumnType_ == ColumnType::COLOR) {
      CQChartsColor c;

      if (columnColor(data.row, data.parent, c))
        geometry.color = c;
    }
    else {
      bool ok4;

      QString str = modelString(data.row, colorColumn(), data.parent, ok4);

      if (ok4)
        geometry.color = CQChartsColor(str);
    }
  }

  //---

  // get geometry custom style
  if (styleColumn().isValid()) {
    bool ok4;

    if (styleColumnType_ == ColumnType::STYLE) {
      QString str = modelString(data.row, styleColumn(), data.parent, ok4);

      geometry.style = CQChartsStyle(str);
    }
  }

  //---

  // save model index for geometry
  QModelIndex geomInd  = modelIndex(data.row, geometryColumn(), data.parent);
  QModelIndex geomInd1 = normalizeIndex(geomInd);

  geometry.ind = geomInd1;

  //---

  // add to list
  th->geometries_.push_back(geometry);
}

bool
CQChartsGeometryPlot::
decodeGeometry(const QString &geomStr, Polygons &polygons) const
{
  if (geomStr.simplified() == "")
    return true;

  // count leading braces
  int n = CQChartsUtil::countLeadingBraces(geomStr);

  //---

  // no braces - single polygon x1 y1 x2 y2 ...
  if      (n == 0) {
    QPolygonF poly;

    if (! CQChartsUtil::stringToPolygon("{{" + geomStr + "}}", poly))
      return false;

    polygons.push_back(poly);
  }
  // single brace - single polygon {x1 y1} {x2 y2} ...
  else if (n == 1) {
    QPolygonF poly;

    if (! CQChartsUtil::stringToPolygon("{" + geomStr + "}", poly))
      return false;

    polygons.push_back(poly);
  }
  // two braces - single polygon {{x1 y1} {x2 y2} ...}
  else if (n == 2) {
    QPolygonF poly;

    if (! CQChartsUtil::stringToPolygon(geomStr, poly))
      return false;

    polygons.push_back(poly);
  }
  // three braces - list of polygons {{{x1 y1} {x2 y2} ...} ... }
  else if (n == 3) {
    if (! CQChartsUtil::stringToPolygons(geomStr, polygons))
      return false;
  }
  else {
    return false;
  }

  return true;
}

bool
CQChartsGeometryPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsGeometryPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  int n = geometries_.size();

  for (int i = 0; i < n; ++i) {
    const Geometry &geometry = geometries_[i];

    CQChartsGeom::BBox bbox = geometry.bbox;

    ColorInd iv(i, n);

    CQChartsGeometryObj *geomObj =
      new CQChartsGeometryObj(this, bbox, geometry.polygons, geometry.ind, iv);

    geomObj->setName (geometry.name);
    geomObj->setColor(geometry.color);
    geomObj->setStyle(geometry.style);

    if (geometry.value)
      geomObj->setValue(*geometry.value);

    objs.push_back(geomObj);
  }

  //---

  return true;
}

//---

bool
CQChartsGeometryPlot::
probe(ProbeData &probeData) const
{
  CQChartsPlotObj *obj;

  if (! objNearestPoint(probeData.p, obj))
    return false;

  CQChartsGeom::Point c = obj->rect().getCenter();

  probeData.p    = c;
  probeData.both = true;

  probeData.xvals.push_back(c.x);
  probeData.yvals.push_back(c.y);

  return true;
}

//---

void
CQChartsGeometryPlot::
write(std::ostream &os, const QString &varName, const QString &modelName) const
{
  CQChartsPlot::write(os, varName, modelName);

  dataLabel_->write(os, varName);
}

//------

CQChartsGeometryObj::
CQChartsGeometryObj(const CQChartsGeometryPlot *plot, const CQChartsGeom::BBox &rect,
                    const Polygons &polygons, const QModelIndex &ind, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsGeometryPlot *>(plot), rect, ColorInd(), ColorInd(), iv),
 plot_(plot), polygons_(polygons)
{
  setDetailHint(DetailHint::MAJOR);

  setModelInd(ind);
}

QString
CQChartsGeometryObj::
calcId() const
{
  if (! plot_->valueColumn().isValid())
    return name();

  return QString("%1:%2:%3").arg(typeName()).arg(name()).arg(value());
}

QString
CQChartsGeometryObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name" , name ());
  tableTip.addTableRow("Value", value());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

bool
CQChartsGeometryObj::
inside(const CQChartsGeom::Point &p) const
{
  QPointF p1 = p.qpoint();

  for (const auto &poly : polygons_) {
    if (poly.containsPoint(p1, Qt::OddEvenFill))
      return true;
  }

  return false;
}

//---

void
CQChartsGeometryObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->nameColumn    ());
  addColumnSelectIndex(inds, plot_->geometryColumn());
  addColumnSelectIndex(inds, plot_->valueColumn   ());
  addColumnSelectIndex(inds, plot_->styleColumn   ());
}

//---

void
CQChartsGeometryObj::
draw(CQChartsPaintDevice *device)
{
  // set polygon pen/brush
  QPen   pen;
  QBrush brush;

  QColor fc;

  double dv = (value() - plot_->minValue())/(plot_->maxValue() - plot_->minValue());

  ColorInd colorInd = calcColorInd();

  if (color().isValid()) {
    if (! hasValue_ || ! plot_->isColorByValue())
      fc = plot_->interpColor(color(), colorInd);
    else
      fc = plot_->interpColor(color(), ColorInd(dv));
  }
  else {
    if (! hasValue_ || ! plot_->isColorByValue())
      fc = plot_->interpFillColor(colorInd);
    else
      fc = plot_->interpColor(plot_->fillColor(), ColorInd(dv));
  }

  QColor bc = plot_->interpStrokeColor(colorInd);

  plot_->setPenBrush(pen, brush,
    plot_->isStroked(), bc, plot_->strokeAlpha(), plot_->strokeWidth(), plot_->strokeDash(),
    plot_->isFilled(), fc, plot_->fillAlpha(), plot_->fillPattern());

  if (style().isValid()) {
    pen   = style().pen  ();
    brush = style().brush();
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  device->setPen  (pen);
  device->setBrush(brush);

  for (const auto &ppoly : polygons_) {
    QPainterPath path;

    int i = 0;

    for (const auto &ppoint : ppoly) {
      if (i == 0)
        path.moveTo(ppoint);
      else
        path.lineTo(ppoint);

      ++i;
    }

    path.closeSubpath();

    device->drawPath(path);
  }
}

void
CQChartsGeometryObj::
drawFg(CQChartsPaintDevice *device) const
{
  QRectF qrect = rect().qrect();

  plot_->dataLabel()->draw(device, qrect, name());
}
