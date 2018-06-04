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
  addColumnParameter("geometry", "Geometry", "geometryColumn", 0).setRequired();
  addColumnParameter("value"   , "Value"   , "valueColumn"   , 1).setRequired();

  addColumnParameter("name" , "Name" , "nameColumn" );
  addColumnParameter("color", "Color", "colorColumn");
  addColumnParameter("style", "Style", "styleColumn");

  CQChartsPlotType::addParameters();
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
 CQChartsPlot(view, view->charts()->plotType("geometry"), model), dataLabel_(this)
{
  setFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setFilled(true);
  setBorder(true);

  setLayerActive(Layer::FG, true);

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
  CQChartsUtil::testAndSet(minValue_, r, [&]() { update(); } );
}

void
CQChartsGeometryPlot::
setMaxValue(double r)
{
  CQChartsUtil::testAndSet(maxValue_, r, [&]() { update(); } );
}

//---

bool
CQChartsGeometryPlot::
isBorder() const
{
  return shapeData_.border.visible;
}

void
CQChartsGeometryPlot::
setBorder(bool b)
{
  CQChartsUtil::testAndSet(shapeData_.border.visible, b, [&]() { update(); } );
}

const CQChartsColor &
CQChartsGeometryPlot::
borderColor() const
{
  return shapeData_.border.color;
}

void
CQChartsGeometryPlot::
setBorderColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(shapeData_.border.color, c, [&]() { update(); } );
}

QColor
CQChartsGeometryPlot::
interpBorderColor(int i, int n) const
{
  return borderColor().interpColor(this, i, n);
}

double
CQChartsGeometryPlot::
borderAlpha() const
{
  return shapeData_.border.alpha;
}

void
CQChartsGeometryPlot::
setBorderAlpha(double a)
{
  CQChartsUtil::testAndSet(shapeData_.border.alpha, a, [&]() { update(); } );
}

const CQChartsLength &
CQChartsGeometryPlot::
borderWidth() const
{
  return shapeData_.border.width;
}

void
CQChartsGeometryPlot::
setBorderWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(shapeData_.border.width, l, [&]() { update(); } );
}

//------

bool
CQChartsGeometryPlot::
isFilled() const
{
  return shapeData_.background.visible;
}

void
CQChartsGeometryPlot::
setFilled(bool b)
{
  CQChartsUtil::testAndSet(shapeData_.background.visible, b, [&]() { update(); } );
}

const CQChartsColor &
CQChartsGeometryPlot::
fillColor() const
{
  return shapeData_.background.color;
}

void
CQChartsGeometryPlot::
setFillColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(shapeData_.background.color, c, [&]() { update(); } );
}

QColor
CQChartsGeometryPlot::
interpFillColor(int i, int n) const
{
  return fillColor().interpColor(this, i, n);
}

double
CQChartsGeometryPlot::
fillAlpha() const
{
  return shapeData_.background.alpha;
}

void
CQChartsGeometryPlot::
setFillAlpha(double a)
{
  CQChartsUtil::testAndSet(shapeData_.background.alpha, a, [&]() { update(); } );
}

CQChartsGeometryPlot::Pattern
CQChartsGeometryPlot::
fillPattern() const
{
  return (Pattern) shapeData_.background.pattern;
}

void
CQChartsGeometryPlot::
setFillPattern(Pattern pattern)
{
  if (pattern != (Pattern) shapeData_.background.pattern) {
    shapeData_.background.pattern = (CQChartsFillData::Pattern) pattern;

    update();
  }
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

  addProperty("stroke", this, "border"     , "visible");
  addProperty("stroke", this, "borderColor", "color"  );
  addProperty("stroke", this, "borderAlpha", "alpha"  );
  addProperty("stroke", this, "borderWidth", "width"  );

  addProperty("fill", this, "filled"     , "visible");
  addProperty("fill", this, "fillColor"  , "color"  );
  addProperty("fill", this, "fillAlpha"  , "alpha"  );
  addProperty("fill", this, "fillPattern", "pattern");

  dataLabel_.addProperties("dataLabel");

  addProperty("value", this, "minValue", "min");
  addProperty("value", this, "maxValue", "max");
}

void
CQChartsGeometryPlot::
updateRange(bool apply)
{
  QAbstractItemModel *model = this->model();

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

    State visit(QAbstractItemModel *model, const QModelIndex &ind, int row) override {
      plot_->addRow(model, ind, row);

      return State::OK;
    }

   private:
    CQChartsGeometryPlot *plot_ { nullptr };
  };

  GeometryPlotVisitor geometryPlotVisitor(this);

  visitModel(geometryPlotVisitor);

  //---

  if (apply)
    applyDataRange();
}

void
CQChartsGeometryPlot::
addRow(QAbstractItemModel *model, const QModelIndex &parent, int row)
{
  Geometry geometry;

  //---

  // get geometry name
  bool ok1;

  geometry.name = modelString(model, row, nameColumn(), parent, ok1);

  //---

  // decode geometry column value into polygons
  if (geometryColumnType_ == ColumnType::RECT ||
      geometryColumnType_ == ColumnType::POLYGON ||
      geometryColumnType_ == ColumnType::PATH) {
    bool ok2;

    QVariant var = modelValue(model, row, geometryColumn(), parent, ok2);

    QVariant rvar = CQChartsUtil::columnUserData(charts(), model, geometryColumn(), var);

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

    QString geomStr = modelString(model, row, geometryColumn(), parent, ok2);

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

  geometry.value = modelReal(model, row, valueColumn(), parent, ok3);

  if (! ok3)
    geometry.value = row;

  if (CQChartsUtil::isNaN(geometry.value))
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
      CQChartsColor c = modelColor(model, row, colorColumn(), parent, ok4);

      geometry.color = c;
    }
    else {
      QString str = modelString(model, row, colorColumn(), parent, ok4);

      geometry.color = CQChartsColor(str);
    }
  }

  //---

  // get geometry custom style
  if (styleColumn().isValid()) {
    bool ok4;

    if (styleColumnType_ == ColumnType::STYLE) {
      QString str = modelString(model, row, styleColumn(), parent, ok4);

      geometry.style = CQChartsStyle(str);
    }
  }

  //---

  // save model index for geometry
  QModelIndex geomInd  = model->index(row, geometryColumn().column(), parent);
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

void
CQChartsGeometryPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
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

  return QString("%1:%2").arg(name()).arg(value());
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
addSelectIndex()
{
  plot_->addSelectIndex(ind_.row(), plot_->nameColumn    (), ind_.parent());
  plot_->addSelectIndex(ind_.row(), plot_->geometryColumn(), ind_.parent());
  plot_->addSelectIndex(ind_.row(), plot_->valueColumn   (), ind_.parent());
  plot_->addSelectIndex(ind_.row(), plot_->colorColumn   (), ind_.parent());
  plot_->addSelectIndex(ind_.row(), plot_->styleColumn   (), ind_.parent());
}

bool
CQChartsGeometryObj::
isIndex(const QModelIndex &ind) const
{
  if (ind.row() != ind_.row()) return false;

  return (plot_->isSelectIndex(ind, ind_.row(), plot_->nameColumn    (), ind_.parent()) ||
          plot_->isSelectIndex(ind, ind_.row(), plot_->geometryColumn(), ind_.parent()) ||
          plot_->isSelectIndex(ind, ind_.row(), plot_->valueColumn   (), ind_.parent()) ||
          plot_->isSelectIndex(ind, ind_.row(), plot_->colorColumn   (), ind_.parent()) ||
          plot_->isSelectIndex(ind, ind_.row(), plot_->styleColumn   (), ind_.parent()));
}

void
CQChartsGeometryObj::
draw(QPainter *painter, const CQChartsPlot::Layer &layer)
{
  if (layer == CQChartsPlot::Layer::MID) {
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
    QPen pen;

    if (plot_->isBorder()) {
      QColor bc = plot_->interpBorderColor(0, 1);

      bc.setAlphaF(plot_->borderAlpha());

      double bw = plot_->lengthPixelWidth(plot_->borderWidth());

      pen.setColor (bc);
      pen.setWidthF(bw);
    }
    else {
      pen = QPen(Qt::NoPen);
    }

    QBrush brush;

    if (plot_->isFilled()) {
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

      fc.setAlphaF(plot_->fillAlpha());

      brush.setColor(fc);
      brush.setStyle(CQChartsFillPattern::toStyle(
        (CQChartsFillPattern::Type) plot_->fillPattern()));
    }
    else {
      brush.setStyle(Qt::NoBrush);
    }

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

  //---

  if (layer == CQChartsPlot::Layer::FG) {
    CQChartsGeom::BBox prect;

    plot_->windowToPixel(rect(), prect);

    QRectF qrect = CQChartsUtil::toQRect(prect);

    plot_->dataLabel().draw(painter, qrect, name());
  }
}
