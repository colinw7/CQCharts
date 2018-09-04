#include <CQChartsGeometryPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsBoxObj.h>
#include <CQChartsTip.h>
#include <CQStrParse.h>
#include <QPainter>

CQChartsGeometryPlotType::
CQChartsGeometryPlotType()
{
}

void
CQChartsGeometryPlotType::
addParameters()
{
  startParameterGroup("Geometry");

  addColumnParameter("geometry", "Geometry", "geometryColumn", 0).setRequired();
  addColumnParameter("value"   , "Value"   , "valueColumn"   , 1).setRequired();

  addColumnParameter("name" , "Name" , "nameColumn" );
  addColumnParameter("color", "Color", "colorColumn").setTip("Custom shape color");
  addColumnParameter("style", "Style", "styleColumn");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsGeometryPlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draw polygon shapes.</p>\n";
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
 CQChartsPlotShapeData<CQChartsGeometryPlot>(this),
 dataLabel_(this)
{
  setFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setFilled(true);
  setBorder(true);

  setLayerActive(CQChartsLayer::Type::FG_PLOT, true);

  addAxes();

  addTitle();
}

CQChartsGeometryPlot::
~CQChartsGeometryPlot()
{
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
setColorColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(colorColumn_, c, [&]() { updateRangeAndObjs(); } );
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
setMinValue(double r)
{
  CQChartsUtil::testAndSet(minValue_, r, [&]() { invalidateLayers(); } );
}

void
CQChartsGeometryPlot::
setMaxValue(double r)
{
  CQChartsUtil::testAndSet(maxValue_, r, [&]() { invalidateLayers(); } );
}

//---

void
CQChartsGeometryPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nameColumn"    , "name"    );
  addProperty("columns", this, "geometryColumn", "geometry");
  addProperty("columns", this, "valueColumn"   , "value"   );
  addProperty("columns", this, "colorColumn"   , "color"   );
  addProperty("columns", this, "styleColumn"   , "style"   );

  addProperty("stroke", this, "border", "visible");

  addLineProperties("stroke", "border");

  addProperty("fill", this, "filled", "visible");

  addFillProperties("fill", "fill");

  dataLabel_.addPathProperties("dataLabel");

  addProperty("value", this, "minValue", "min");
  addProperty("value", this, "maxValue", "max");
}

void
CQChartsGeometryPlot::
calcRange()
{
  QAbstractItemModel *model = this->model().data();

  if (! model)
    return;

  dataRange_.reset();

  geometries_.clear();

  minValue_ = 0.0;
  maxValue_ = 0.0;

  //---

  geometryColumnType_ = columnValueType(geometryColumn());
  colorColumnType_    = columnValueType(colorColumn());
  styleColumnType_    = columnValueType(styleColumn());

  //---

  // process model data
  class GeometryPlotVisitor : public ModelVisitor {
   public:
    GeometryPlotVisitor(CQChartsGeometryPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *model, const VisitData &data) override {
      plot_->addRow(model, data);

      return State::OK;
    }

   private:
    CQChartsGeometryPlot *plot_ { nullptr };
  };

  GeometryPlotVisitor geometryPlotVisitor(this);

  visitModel(geometryPlotVisitor);
}

void
CQChartsGeometryPlot::
addRow(QAbstractItemModel *model, const ModelVisitor::VisitData &data)
{
  Geometry geometry;

  //---

  // get geometry name
  bool ok1;

  geometry.name = modelString(data.row, nameColumn(), data.parent, ok1);

  //---

  // decode geometry column value into polygons
  if (geometryColumnType_ == ColumnType::RECT ||
      geometryColumnType_ == ColumnType::POLYGON ||
      geometryColumnType_ == ColumnType::PATH) {
    bool ok2;

    QVariant var = modelValue(data.row, geometryColumn(), data.parent, ok2);

    bool converted;

    QVariant rvar =
      CQChartsUtil::columnUserData(charts(), model, geometryColumn(), var, converted);

    QPolygonF poly;

    if      (geometryColumnType_ == ColumnType::RECT) {
      QRectF r = rvar.value<QRectF>();

      poly = QPolygonF(r);
    }
    else if (geometryColumnType_ == ColumnType::POLYGON) {
      poly = rvar.value<QPolygonF>();
    }
    else if (geometryColumnType_ == ColumnType::PATH) {
      CQChartsPath path = rvar.value<CQChartsPath>();

      poly = path.path().toFillPolygon();
    }
    else {
      assert(false);
    }

    geometry.polygons.push_back(poly);
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

      dataRange_.updateRange(p.x(), p.y());

      geometry.bbox.add(p.x(), p.y());
    }
  }

  //---

  // get geometry associated value
  bool ok3;

  geometry.value = modelReal(data.row, valueColumn(), data.parent, ok3);

  if (! ok3)
    geometry.value = data.row;

  if (CMathUtil::isNaN(geometry.value))
    return;

  // update value range
  if (geometries_.empty()) {
    minValue_ = geometry.value;
    maxValue_ = geometry.value;
  }
  else {
    minValue_ = std::min(minValue_, geometry.value);
    maxValue_ = std::max(maxValue_, geometry.value);
  }

  //---

  // get geometry custom color
  if (colorColumn().isValid()) {
    bool ok4;

    if (colorColumnType_ == ColumnType::COLOR) {
      CQChartsColor c = modelColor(data.row, colorColumn(), data.parent, ok4);

      geometry.color = c;
    }
    else {
      QString str = modelString(data.row, colorColumn(), data.parent, ok4);

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
  geometries_.push_back(geometry);
}

bool
CQChartsGeometryPlot::
decodeGeometry(const QString &geomStr, Polygons &polygons)
{
  CQStrParse parse(geomStr);

  int n = 0;

  while (! parse.eof()) {
    parse.skipSpace();

    if (! parse.isChar('{'))
      break;

    parse.skipChar();

    ++n;
  }

  //---

  // single polygon x1 y1 x2 y2 ...
  if      (n == 0) {
    QPolygonF poly;

    if (! decodePolygon("{{" + geomStr + "}}", poly))
      return false;

    polygons.push_back(poly);
  }
  // single polygon {x1 y1} {x2 y2} ...
  else if (n == 1) {
    QPolygonF poly;

    if (! decodePolygon("{" + geomStr + "}", poly))
      return false;

    polygons.push_back(poly);
  }
  // single polygon {{x1 y1} {x2 y2} ...}
  else if (n == 2) {
    QPolygonF poly;

    if (! decodePolygon(geomStr, poly))
      return false;

    polygons.push_back(poly);
  }
  // list of polygons {{{x1 y1} {x2 y2} ...} ... }
  else if (n == 3) {
    if (! decodePolygons(geomStr, polygons))
      return false;
  }
  else {
    return false;
  }

  return true;
}

bool
CQChartsGeometryPlot::
decodePolygons(const QString &str, Polygons &polys)
{
  return CQChartsUtil::stringToPolygons(str, polys);
}

bool
CQChartsGeometryPlot::
decodePolygon(const QString &str, QPolygonF &poly)
{
  return CQChartsUtil::stringToPolygon(str, poly);
}

bool
CQChartsGeometryPlot::
initObjs()
{
  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return false;
  }

  //---

  if (! plotObjs_.empty())
    return false;

  //---

  int n = geometries_.size();

  for (int i = 0; i < n; ++i) {
    const Geometry &geometry = geometries_[i];

    CQChartsGeom::BBox bbox = geometry.bbox;

    CQChartsGeometryObj *geomObj;

    if (! valueColumn().isValid())
      geomObj = new CQChartsGeometryObj(this, bbox, geometry.polygons, geometry.ind, i, n);
    else
      geomObj = new CQChartsGeometryObj(this, bbox, geometry.polygons, geometry.ind, -1, -1);

    geomObj->setName (geometry.name);
    geomObj->setValue(geometry.value);
    geomObj->setColor(geometry.color);
    geomObj->setStyle(geometry.style);

    addPlotObject(geomObj);
  }

  //---

  return true;
}

//------

CQChartsGeometryObj::
CQChartsGeometryObj(CQChartsGeometryPlot *plot, const CQChartsGeom::BBox &rect,
                    const Polygons &polygons, const QModelIndex &ind, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), polygons_(polygons), ind_(ind), i_(i), n_(n)
{
}

QString
CQChartsGeometryObj::
calcId() const
{
  if (! plot_->valueColumn().isValid())
    return name();

  return QString("geom:%1:%2").arg(name()).arg(value());
}

QString
CQChartsGeometryObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name" , name ());
  tableTip.addTableRow("Value", value());

  return tableTip.str();
}

bool
CQChartsGeometryObj::
inside(const CQChartsGeom::Point &p) const
{
  QPointF p1 = CQChartsUtil::toQPoint(p);

  for (const auto &poly : polygons_) {
    if (poly.containsPoint(p1, Qt::OddEvenFill))
      return true;
  }

  return false;
}

void
CQChartsGeometryObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->nameColumn    ());
  addColumnSelectIndex(inds, plot_->geometryColumn());
  addColumnSelectIndex(inds, plot_->valueColumn   ());
  addColumnSelectIndex(inds, plot_->colorColumn   ());
  addColumnSelectIndex(inds, plot_->styleColumn   ());
}

void
CQChartsGeometryObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind_.row(), column, ind_.parent());
}

void
CQChartsGeometryObj::
draw(QPainter *painter)
{
  ppolygons_.clear();

  for (const auto &poly : polygons_) {
    QPolygonF ppoly;

    for (int i = 0; i < poly.count(); ++i) {
      double px, py;

      plot_->windowToPixel(poly[i].x(), poly[i].y(), px, py);

      ppoly << QPointF(px, py);
    }

    ppolygons_.push_back(ppoly);
  }

  //---

  // set polygon pen/brush
  QPen   pen;
  QBrush brush;

  QColor fc;

  double dv = (value() - plot_->minValue())/(plot_->maxValue() - plot_->minValue());

  if (color().isValid()) {
    if (n_ > 0)
      fc = color().interpColor(plot_, i_, n_);
    else
      fc = color().interpColor(plot_, dv);
  }
  else {
    if (n_ > 0)
      fc = plot_->interpFillColor(i_, n_);
    else
      fc = plot_->interpPaletteColor(dv);
  }

  plot_->setPenBrush(pen, brush,
                     plot_->isBorder(),
                     plot_->interpBorderColor(0, 1),
                     plot_->borderAlpha(),
                     plot_->borderWidth(),
                     plot_->borderDash(),
                     plot_->isFilled(),
                     fc,
                     plot_->fillAlpha(),
                     plot_->fillPattern());

  if (style().isValid()) {
    pen   = style().pen  ();
    brush = style().brush();
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  painter->setPen  (pen);
  painter->setBrush(brush);

  for (const auto &ppoly : ppolygons_) {
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

    painter->drawPath(path);
  }
}

void
CQChartsGeometryObj::
drawFg(QPainter *painter)
{
  CQChartsGeom::BBox prect;

  plot_->windowToPixel(rect(), prect);

  QRectF qrect = CQChartsUtil::toQRect(prect);

  plot_->dataLabel().draw(painter, qrect, name());
}
