#include <CQChartsGeometryPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>
#include <CGradientPalette.h>
#include <CStrParse.h>

#include <QPainter>

CQChartsGeometryPlot::
CQChartsGeometryPlot(CQChartsView *view, QAbstractItemModel *model) :
 CQChartsPlot(view, model)
{
}

void
CQChartsGeometryPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nameColumn"    , "name"    );
  addProperty("columns", this, "geometryColumn", "geometry");
  addProperty("columns", this, "valueColumn"   , "value"   );

  addProperty("", this, "lineColor");
  addProperty("", this, "fillColor");
}

void
CQChartsGeometryPlot::
updateRange()
{
  int n = model_->rowCount(QModelIndex());

  dataRange_.reset();

  geometries_.clear();

  minValue_ = 0.0;
  maxValue_ = 0.0;

  for (int i = 0; i < n; ++i) {
    Geometry geometry;

    bool ok1;

    geometry.name = CQChartsUtil::modelString(model_, i, nameColumn_, ok1);

    //--

    bool ok2;

    QString geomStr = CQChartsUtil::modelString(model_, i, geometryColumn_, ok2);

    if (! decodeGeometry(geomStr, geometry.polygons)) {
      std::cerr << "Invalid state geom : " << geometry.name.toStdString() << " : " <<
                   geomStr.toStdString() << std::endl;
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

    geometry.value = CQChartsUtil::modelReal(model_, i, valueColumn_, ok);

    if (! ok)
      geometry.value = i;

    if (i == 0) {
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

  //displayRange_.setEqualScale(true);

  applyDataRange();
}

bool
CQChartsGeometryPlot::
decodeGeometry(const QString &geomStr, Polygons &polygons)
{
  CStrParse parse(geomStr.toStdString());

  int n = 0;

  while (! parse.eof()) {
    parse.skipSpace();

    if (! parse.isChar('{'))
      break;

    parse.skipChar();

    ++n;
  }

  //---

  // single polygon {{x1 y1} {x2 y2} ...}
  if (n == 2) {
    QPolygonF poly;

    if (! decodePolygon(geomStr, poly))
      return false;

    polygons.push_back(poly);
  }
  // list of polygons {{{x1 y1} {x2 y2} ...}}
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
  CStrParse parse(polysStr.toStdString());

  parse.skipSpace();

  if (! parse.isChar('{'))
    return false;

  parse.skipChar();

  while (! parse.isChar('}')) {
    parse.skipSpace();

    std::string polyStr;

    if (! parse.readBracedString(polyStr, /*includeBraces*/true))
      return false;

    QPolygonF poly;

    if (! decodePolygon(polyStr.c_str(), poly))
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
  CStrParse parse(polyStr.toStdString());

  parse.skipSpace();

  if (! parse.isChar('{'))
    return false;

  parse.skipChar();

  while (! parse.isChar('}')) {
    parse.skipSpace();

    std::string pointStr;

    if (! parse.readBracedString(pointStr, /*includeBraces*/false))
      return false;

    QPointF point;

    if (! decodePoint(pointStr.c_str(), point))
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
  CStrParse parse(pointStr.toStdString());

  parse.skipSpace();

  std::string xstr;

  if (! parse.readNonSpace(xstr))
    return false;

  parse.skipSpace();

  std::string ystr;

  if (! parse.readNonSpace(ystr))
    return false;

  double x, y;

  if (! CQChartsUtil::toReal(xstr.c_str(), x))
    return false;

  if (! CQChartsUtil::toReal(ystr.c_str(), y))
    return false;

  point = QPointF(x, y);

  return true;
}

void
CQChartsGeometryPlot::
initObjs()
{
  if (! plotObjs_.empty())
    return;

  int n = geometries_.size();

  for (int i = 0; i < n; ++i) {
    const Geometry &geometry = geometries_[i];

    CBBox2D bbox = geometry.bbox;

    CQChartsGeometryObj *geomObj;

    if (valueColumn_ < 0) {
      geomObj = new CQChartsGeometryObj(this, bbox, geometry.polygons, 0.0, i, n);

      geomObj->setId(geometry.name);
    }
    else {
      geomObj = new CQChartsGeometryObj(this, bbox, geometry.polygons, geometry.value, -1, -1);

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

  drawBackground(p);

  //---

  drawObjs(p);
}

//------

CQChartsGeometryObj::
CQChartsGeometryObj(CQChartsGeometryPlot *plot, const CBBox2D &rect, const Polygons &polygons,
                    double value, int ind, int n) :
 CQChartsPlotObj(rect), plot_(plot), polygons_(polygons), value_(value), ind_(ind), n_(n)
{
}

bool
CQChartsGeometryObj::
inside(const CPoint2D &p) const
{
  QPointF p1 = CQUtil::toQPoint(p);

  for (const auto &poly : polygons_) {
    if (poly.containsPoint(p1, Qt::OddEvenFill))
      return true;
  }

  return false;
}

void
CQChartsGeometryObj::
handleResize()
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
}

void
CQChartsGeometryObj::
draw(QPainter *p)
{
  if (ppolygons_.empty())
    handleResize();

  p->setPen(plot_->lineColor());

  QColor c;

  if (n_ > 0) {
    c = plot_->objectColor(this, ind_, n_, Qt::black);
  }
  else {
    double v = (value_ - plot_->minValue())/(plot_->maxValue() - plot_->minValue());

    c = CQUtil::toQColor(plot_->palette()->getColor(v).rgba());
  }

  p->setBrush(c);

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
