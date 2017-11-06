#include <CQChartsGeometryPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CGradientPalette.h>
#include <CQStrParse.h>

#include <QPainter>

CQChartsGeometryPlotType::
CQChartsGeometryPlotType()
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
  setLayerActive(Layer::FG, true);

  addTitle();
}

void
CQChartsGeometryPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nameColumn"    , "name"    );
  addProperty("columns", this, "geometryColumn", "geometry");
  addProperty("columns", this, "valueColumn"   , "value"   );
  addProperty("value"  , this, "minValue"      , "min"     );
  addProperty("value"  , this, "maxValue"      , "max"     );

  addProperty("", this, "lineColor");

  dataLabel_.addProperties("dataLabel");
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

    bool ok1;

    geometry.name = CQChartsUtil::modelString(model, r, nameColumn(), ok1);

    //--

    bool ok2;

    QString geomStr = CQChartsUtil::modelString(model, r, geometryColumn(), ok2);

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

    geometry.value = CQChartsUtil::modelReal(model, r, valueColumn_, ok);

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

    geometries_.push_back(geometry);
  }

  //---

  //setEqualScale(true);

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

void
CQChartsGeometryPlot::
initObjs()
{
  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return;
  }

  //---

  if (! plotObjs_.empty())
    return;

  //---

  int n = geometries_.size();

  for (int i = 0; i < n; ++i) {
    const Geometry &geometry = geometries_[i];

    CBBox2D bbox = geometry.bbox;

    CQChartsGeometryObj *geomObj;

    if (valueColumn_ < 0) {
      geomObj = new CQChartsGeometryObj(this, bbox, geometry.polygons,
                                        0.0, geometry.name, i, n);

      geomObj->setId(geometry.name);
    }
    else {
      geomObj = new CQChartsGeometryObj(this, bbox, geometry.polygons,
                                        geometry.value, geometry.name, -1, -1);

      geomObj->setId(QString("%1:%2").arg(geometry.name).arg(geometry.value));
    }

    addPlotObject(geomObj);
  }
}

void
CQChartsGeometryPlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawParts(p);
}

void
CQChartsGeometryPlot::
drawDataLabel(QPainter *p, const QRectF &qrect, const QString &str)
{
  dataLabel_.draw(p, qrect, str);
}

//------

CQChartsGeometryObj::
CQChartsGeometryObj(CQChartsGeometryPlot *plot, const CBBox2D &rect, const Polygons &polygons,
                    double value, const QString &name, int ind, int n) :
 CQChartsPlotObj(rect), plot_(plot), polygons_(polygons), value_(value),
 name_(name), ind_(ind), n_(n)
{
}

bool
CQChartsGeometryObj::
inside(const CPoint2D &p) const
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
draw(QPainter *p, const CQChartsPlot::Layer &layer)
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

    p->setPen(plot_->lineColor());

    QColor c;

    if (n_ > 0) {
      c = plot_->objectColor(this, ind_, n_, Qt::black);
    }
    else {
      double v = (value_ - plot_->minValue())/(plot_->maxValue() - plot_->minValue());

      c = plot_->palette()->getColor(v);
    }

    QColor c1 = plot_->objectStateColor(this, c);

    p->setBrush(c1);

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

      p->drawPath(path);
    }
  }

  //---

  if (layer == CQChartsPlot::Layer::FG) {
    CBBox2D prect;

    plot_->windowToPixel(rect(), prect);

    QRectF qrect = CQChartsUtil::toQRect(prect);

    plot_->drawDataLabel(p, qrect, name_);
  }
}
