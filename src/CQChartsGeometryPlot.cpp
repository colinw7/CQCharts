#include <CQChartsGeometryPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsBoxObj.h>
#include <CQChartsFillObj.h>
#include <CGradientPalette.h>
#include <CQStrParse.h>
#include <QPainter>

CQChartsGeometryPlotType::
CQChartsGeometryPlotType()
{
  addParameters();
}

void
CQChartsGeometryPlotType::
addParameters()
{
  addColumnParameter("geometry", "Geometry", "geometryColumn", "", 0);
  addColumnParameter("value"   , "Value"   , "valueColumn"   , "", 1);

  addColumnParameter("name", "Name", "nameColumn", "optional");
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
  boxObj_ = new CQChartsBoxObj(this);

  boxObj_->setBackgroundColor(CQChartsPaletteColor(CQChartsPaletteColor::Type::PALETTE));

  setFilled(true);
  setBorder(true);

  setLayerActive(Layer::FG, true);

  addTitle();
}

CQChartsGeometryPlot::
~CQChartsGeometryPlot()
{
  delete boxObj_;
}

//---

bool
CQChartsGeometryPlot::
isBorder() const
{
  return boxObj_->isBorder();
}

void
CQChartsGeometryPlot::
setBorder(bool b)
{
  boxObj_->setBorder(b);

  update();
}

QString
CQChartsGeometryPlot::
borderColorStr() const
{
  return boxObj_->borderColorStr();
}

void
CQChartsGeometryPlot::
setBorderColorStr(const QString &str)
{
  boxObj_->setBorderColorStr(str);

  update();
}

QColor
CQChartsGeometryPlot::
interpBorderColor(int i, int n) const
{
  return boxObj_->interpBorderColor(i, n);
}

double
CQChartsGeometryPlot::
borderAlpha() const
{
  return boxObj_->borderAlpha();
}

void
CQChartsGeometryPlot::
setBorderAlpha(double a)
{
  boxObj_->setBorderAlpha(a);

  update();
}

double
CQChartsGeometryPlot::
borderWidth() const
{
  return boxObj_->borderWidth();
}

void
CQChartsGeometryPlot::
setBorderWidth(double r)
{
  boxObj_->setBorderWidth(r);

  update();
}

//------

bool
CQChartsGeometryPlot::
isFilled() const
{
  return boxObj_->isBackground();
}

void
CQChartsGeometryPlot::
setFilled(bool b)
{
  boxObj_->setBackground(b);

  update();
}

QString
CQChartsGeometryPlot::
fillColorStr() const
{
  return boxObj_->backgroundColorStr();
}

void
CQChartsGeometryPlot::
setFillColorStr(const QString &s)
{
  boxObj_->setBackgroundColorStr(s);

  update();
}

QColor
CQChartsGeometryPlot::
interpFillColor(int i, int n) const
{
  return boxObj_->interpBackgroundColor(i, n);
}

double
CQChartsGeometryPlot::
fillAlpha() const
{
  return boxObj_->backgroundAlpha();
}

void
CQChartsGeometryPlot::
setFillAlpha(double a)
{
  boxObj_->setBackgroundAlpha(a);

  update();
}

CQChartsGeometryPlot::Pattern
CQChartsGeometryPlot::
fillPattern() const
{
  return (Pattern) boxObj_->backgroundPattern();
}

void
CQChartsGeometryPlot::
setFillPattern(Pattern pattern)
{
  boxObj_->setBackgroundPattern((CQChartsBoxObj::Pattern) pattern);

  update();
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

  int nr = model->rowCount(QModelIndex());

  dataRange_.reset();

  geometries_.clear();

  minValue_ = 0.0;
  maxValue_ = 0.0;

  for (int r = 0; r < nr; ++r) {
    Geometry geometry;

    QModelIndex nameInd  = model->index(r, nameColumn    ());
    QModelIndex geomInd  = model->index(r, geometryColumn());
    QModelIndex valueInd = model->index(r, valueColumn   ());

    QModelIndex geomInd1 = normalizeIndex(geomInd);

    //---

    bool ok1;

    geometry.name = CQChartsUtil::modelString(model, nameInd, ok1);

    //--

    bool ok2;

    QString geomStr = CQChartsUtil::modelString(model, geomInd, ok2);

    if (! decodeGeometry(geomStr, geometry.polygons)) {
      charts()->errorMsg("Invalid geometry '" + geomStr + "' for '" + geometry.name + "'");
      continue;
    }

    for (auto &poly : geometry.polygons) {
      for (int j = 0; j < poly.count(); ++j) {
        const QPointF &p = poly[j];

        dataRange_.updateRange(p.x(), p.y());

        geometry.bbox.add(p.x(), p.y());
      }
    }

    //---

    bool ok;

    geometry.value = CQChartsUtil::modelReal(model, valueInd, ok);

    if (! ok)
      geometry.value = r;

    if (CQChartsUtil::isNaN(geometry.value))
      continue;

    if (r == 0) {
      minValue_ = geometry.value;
      maxValue_ = geometry.value;
    }
    else {
      minValue_ = std::min(minValue_, geometry.value);
      maxValue_ = std::max(maxValue_, geometry.value);
    }

    //---

    geometry.ind = geomInd1;

    //---

    geometries_.push_back(geometry);
  }

  //---

  if (apply)
    applyDataRange();
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

  // single polygon {x1 y1} {x2 y2} ...
  if      (n == 1) {
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
decodePolygons(const QString &polysStr, Polygons &polygons)
{
  CQStrParse parse(polysStr);

  parse.skipSpace();

  if (! parse.isChar('{'))
    return false;

  parse.skipChar();

  while (! parse.isChar('}')) {
    parse.skipSpace();

    QString polyStr;

    if (! parse.readBracedString(polyStr, /*includeBraces*/true))
      return false;

    QPolygonF poly;

    if (! decodePolygon(polyStr, poly))
      return false;

    polygons.push_back(poly);

    parse.skipSpace();
  }

  if (parse.isChar('}'))
    parse.skipChar();

  return true;
}

bool
CQChartsGeometryPlot::
decodePolygon(const QString &polyStr, QPolygonF &poly)
{
  CQStrParse parse(polyStr);

  parse.skipSpace();

  if (! parse.isChar('{'))
    return false;

  parse.skipChar();

  while (! parse.isChar('}')) {
    parse.skipSpace();

    QString pointStr;

    if (! parse.readBracedString(pointStr, /*includeBraces*/false))
      return false;

    QPointF point;

    if (! decodePoint(pointStr, point))
      return false;

    poly.push_back(point);

    parse.skipSpace();
  }

  if (parse.isChar('}'))
    parse.skipChar();

  return true;
}

bool
CQChartsGeometryPlot::
decodePoint(const QString &pointStr, QPointF &point)
{
  CQStrParse parse(pointStr);

  parse.skipSpace();

  QString xstr;

  if (! parse.readNonSpace(xstr))
    return false;

  parse.skipSpace();

  QString ystr;

  if (! parse.readNonSpace(ystr))
    return false;

  double x, y;

  if (! CQChartsUtil::toReal(xstr, x))
    return false;

  if (! CQChartsUtil::toReal(ystr, y))
    return false;

  point = QPointF(x, y);

  return true;
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

    if (valueColumn() < 0)
      geomObj = new CQChartsGeometryObj(this, bbox, geometry.polygons,
                                        0.0, geometry.name, geometry.ind, i, n);
    else
      geomObj = new CQChartsGeometryObj(this, bbox, geometry.polygons,
                                        geometry.value, geometry.name, geometry.ind, -1, -1);

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
                    const Polygons &polygons, double value, const QString &name,
                    const QModelIndex &ind, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), polygons_(polygons), value_(value),
 name_(name), ind_(ind), i_(i), n_(n)
{
}

QString
CQChartsGeometryObj::
calcId() const
{
  if (plot_->valueColumn() < 0)
    return name_;

  return QString("%1:%2").arg(name_).arg(value_);
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
  QModelIndex nameInd     = plot_->selectIndex(ind_.row(), plot_->nameColumn    (), ind_.parent());
  QModelIndex geometryInd = plot_->selectIndex(ind_.row(), plot_->geometryColumn(), ind_.parent());
  QModelIndex valueInd    = plot_->selectIndex(ind_.row(), plot_->valueColumn   (), ind_.parent());

  if (nameInd.isValid())
    plot_->addSelectIndex(nameInd);

  if (geometryInd.isValid())
    plot_->addSelectIndex(geometryInd);

  if (valueInd.isValid())
    plot_->addSelectIndex(valueInd);
}

bool
CQChartsGeometryObj::
isIndex(const QModelIndex &ind) const
{
  if (ind.row() != ind_.row()) return false;

  return (ind == plot_->selectIndex(ind_.row(), plot_->nameColumn    (), ind_.parent()) ||
          ind == plot_->selectIndex(ind_.row(), plot_->geometryColumn(), ind_.parent()) ||
          ind == plot_->selectIndex(ind_.row(), plot_->valueColumn   (), ind_.parent()));
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

      pen.setColor (bc);
      pen.setWidthF(plot_->borderWidth());
    }
    else {
      pen = QPen(Qt::NoPen);
    }

    QBrush brush;

    if (plot_->isFilled()) {
      QColor fc;

      if (n_ > 0) {
        fc = plot_->interpFillColor(i_, n_);
      }
      else {
        double v = (value_ - plot_->minValue())/(plot_->maxValue() - plot_->minValue());

        fc = plot_->palette()->getColor(v);
      }

      fc.setAlphaF(plot_->fillAlpha());

      brush.setColor(fc);
      brush.setStyle(CQChartsFillObj::patternToStyle(
        (CQChartsFillObj::Pattern) plot_->fillPattern()));
    }
    else {
      brush.setStyle(Qt::NoBrush);
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

    plot_->dataLabel().draw(painter, qrect, name_);
  }
}
