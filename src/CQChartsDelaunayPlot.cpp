#include <CQChartsDelaunayPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsDelaunay.h>
#include <QPainter>

CQChartsDelaunayPlotType::
CQChartsDelaunayPlotType()
{
}

void
CQChartsDelaunayPlotType::
addParameters()
{
  addColumnParameter("x", "X", "xColumn", "", 0);
  addColumnParameter("y", "Y", "yColumn", "", 1);

  addColumnParameter("name", "Name", "nameColumn", "optional");
}

CQChartsPlot *
CQChartsDelaunayPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsDelaunayPlot(view, model);
}

//------

CQChartsDelaunayPlot::
CQChartsDelaunayPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("delaunay"), model)
{
  pointObj_ = new CQChartsPointObj(this);
  lineObj_  = new CQChartsLineObj(this);

  CQChartsPaletteColor linesColor (CQChartsPaletteColor::Type::THEME_VALUE, 1);
  CQChartsPaletteColor pointsColor(CQChartsPaletteColor::Type::PALETTE);

  lineObj_ ->setColor      (linesColor);
  pointObj_->setStrokeColor(pointsColor);

  addAxes();

  addTitle();
}

CQChartsDelaunayPlot::
~CQChartsDelaunayPlot()
{
  delete pointObj_;
  delete lineObj_;

  delete delaunay_;
}

//---

QString
CQChartsDelaunayPlot::
pointsStrokeColorStr() const
{
  return pointObj_->strokeColorStr();
}

void
CQChartsDelaunayPlot::
setPointsStrokeColorStr(const QString &s)
{
  pointObj_->setStrokeColorStr(s);

  update();
}

QColor
CQChartsDelaunayPlot::
interpPointStrokeColor(int i, int n) const
{
  return pointObj_->interpStrokeColor(i, n);
}

double
CQChartsDelaunayPlot::
pointsStrokeAlpha() const
{
  return pointObj_->strokeAlpha();
}

void
CQChartsDelaunayPlot::
setPointsStrokeAlpha(double a)
{
  pointObj_->setStrokeAlpha(a);

  update();
}

QString
CQChartsDelaunayPlot::
pointsFillColorStr() const
{
  return pointObj_->fillColorStr();
}

void
CQChartsDelaunayPlot::
setPointsFillColorStr(const QString &str)
{
  pointObj_->setFillColorStr(str);

  update();
}

QColor
CQChartsDelaunayPlot::
interpPointFillColor(int i, int n) const
{
  return pointObj_->interpFillColor(i, n);
}

double
CQChartsDelaunayPlot::
pointsFillAlpha() const
{
  return pointObj_->fillAlpha();
}

void
CQChartsDelaunayPlot::
setPointsFillAlpha(double a)
{
  pointObj_->setFillAlpha(a);

  update();
}

//---

QString
CQChartsDelaunayPlot::
linesColorStr() const
{
  return lineObj_->colorStr();
}

void
CQChartsDelaunayPlot::
setLinesColorStr(const QString &str)
{
  lineObj_->setColorStr(str);

  update();
}

QColor
CQChartsDelaunayPlot::
interpLinesColor(int i, int n) const
{
  return lineObj_->interpColor(i, n);
}

//---

void
CQChartsDelaunayPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "xColumn", "x");
  addProperty("columns", this, "yColumn", "y");

  addProperty("points"       , this, "points"           , "visible");
  addProperty("points"       , this, "symbolName"       , "symbol" );
  addProperty("points"       , this, "symbolSize"       , "size"   );
  addProperty("points/stroke", this, "symbolStroked"    , "visible");
  addProperty("points/stroke", this, "pointsStrokeColor", "color"  );
  addProperty("points/stroke", this, "pointsStrokeAlpha", "alpha"  );
  addProperty("points/stroke", this, "symbolLineWidth"  , "width"  );
  addProperty("points/fill"  , this, "symbolFilled"     , "visible");
  addProperty("points/fill"  , this, "pointsFillColor"  , "color"  );
  addProperty("points/fill"  , this, "pointsFillAlpha"  , "alpha"  );

  addProperty("lines", this, "lines"     , "visible");
  addProperty("lines", this, "linesColor", "color"  );
  addProperty("lines", this, "linesAlpha", "alpha"  );
  addProperty("lines", this, "linesWidth", "width"  );

  addProperty("voronoi", this, "voronoi"         , "enabled"  );
  addProperty("voronoi", this, "voronoiPointSize", "pointSize");
}

void
CQChartsDelaunayPlot::
updateRange(bool apply)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int nr = model->rowCount(QModelIndex());

  dataRange_.reset();

  for (int r = 0; r < nr; ++r) {
    QModelIndex xind = model->index(r, xColumn());
    QModelIndex yind = model->index(r, yColumn());

    //---

    bool ok1, ok2;

    double x = CQChartsUtil::modelReal(model, xind, ok1);
    double y = CQChartsUtil::modelReal(model, yind, ok2);

    if (! ok1) x = r;
    if (! ok2) y = r;

    if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y))
      continue;

    dataRange_.updateRange(x, y);
  }

  //---

  bool ok;

  xAxis_->setColumn(xColumn());

  QString xname = CQChartsUtil::modelHeaderString(model, xColumn(), ok);

  xAxis_->setLabel(xname);

  yAxis_->setColumn(yColumn());

  QString yname = CQChartsUtil::modelHeaderString(model, yColumn(), ok);

  yAxis_->setLabel(yname);

  //---

  if (apply)
    applyDataRange();
}

bool
CQChartsDelaunayPlot::
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

  QAbstractItemModel *model = this->model();

  if (! model)
    return false;

  //---

  double sw = (dataRange_.xmax() - dataRange_.xmin())/100.0;
  double sh = (dataRange_.ymax() - dataRange_.ymin())/100.0;

  int nr = model->rowCount(QModelIndex());

  bool ok;

  yname_ = CQChartsUtil::modelHeaderString(model, yColumn(), ok);

  //---

  delete delaunay_;

  delaunay_ = new CQChartsDelaunay;

  for (int r = 0; r < nr; ++r) {
    QModelIndex xind = model->index(r, xColumn());
    QModelIndex yind = model->index(r, yColumn());

    bool ok1, ok2;

    double x = CQChartsUtil::modelReal(model, xind, ok1);
    double y = CQChartsUtil::modelReal(model, yind, ok2);

    if (! ok1) x = r;
    if (! ok2) y = r;

    if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y))
      continue;

    delaunay_->addVertex(x, y);

    //---

    QModelIndex xind1 = normalizeIndex(xind);

    CQChartsGeom::BBox bbox(x - sw/2.0, y - sh/2.0, x + sw/2.0, y + sh/2.0);

    CQChartsDelaunayPointObj *pointObj =
      new CQChartsDelaunayPointObj(this, bbox, x, y, xind1, r, nr);

    addPlotObject(pointObj);
  }

  delaunay_->calc();

  //---

  return true;
}

void
CQChartsDelaunayPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
}

void
CQChartsDelaunayPlot::
drawForeground(QPainter *painter)
{
  setClipRect(painter);

  if (! isVoronoi())
    drawDelaunay(painter);
  else
    drawVoronoi(painter);
}

void
CQChartsDelaunayPlot::
drawDelaunay(QPainter *painter)
{
  if (isLines()) {
    QColor lc = interpLinesColor(0, 1);

    lc.setAlphaF(linesAlpha());

    QPen pen(lc);

    pen.setWidthF(linesWidth());

    for (auto pf = delaunay_->facesBegin(); pf != delaunay_->facesEnd(); ++pf) {
      const CQChartsHull3D::Face *f = *pf;

      if (! f->isLower()) continue;

      auto *v1 = f->vertex(0);
      auto *v2 = f->vertex(1);
      auto *v3 = f->vertex(2);

      double px1, py1, px2, py2, px3, py3;

      windowToPixel(v1->x(), v1->y(), px1, py1);
      windowToPixel(v2->x(), v2->y(), px2, py2);
      windowToPixel(v3->x(), v3->y(), px3, py3);

      QPainterPath path;

      path.moveTo(px1, py1);
      path.lineTo(px2, py2);
      path.lineTo(px3, py3);

      path.closeSubpath();

      painter->strokePath(path, pen);
    }
  }
}

void
CQChartsDelaunayPlot::
drawVoronoi(QPainter *painter)
{
  if (isPoints()) {
    QPen   pen;
    QBrush brush;

    if (isSymbolStroked()) {
      QColor pc = interpPointStrokeColor(0, 1);

      pc.setAlphaF(pointsStrokeAlpha());

      pen.setColor(pc);
    }
    else
      pen.setStyle(Qt::NoPen);

    pen.setWidthF(symbolLineWidth());

    if (isSymbolFilled()) {
      QColor bc = interpPointFillColor(0, 1);

      bc.setAlphaF(pointsFillAlpha());

      brush.setStyle(Qt::SolidPattern);
      brush.setColor(bc);
    }
    else
      brush.setStyle(Qt::NoBrush);

    painter->setPen  (pen);
    painter->setBrush(brush);

    for (auto pf = delaunay_->facesBegin(); pf != delaunay_->facesEnd(); ++pf) {
      const CQChartsHull3D::Face *f = *pf;

      CQChartsHull3D::Vertex *v = f->getVoronoi();
      if (! v) continue;

      double px, py;

      windowToPixel(v->x(), v->y(), px, py);

      double d = voronoiPointSize();

      QRectF rect(px - d, py - d, 2.0*d, 2.0*d);

      painter->drawArc(rect, 0, 16*360);
    }
  }

  //---

  if (isLines()) {
    QColor lc = interpLinesColor(0, 1);

    lc.setAlphaF(linesAlpha());

    double lw = linesWidth();

    CQChartsLineDash ld;

    for (auto pve = delaunay_->voronoiEdgesBegin(); pve != delaunay_->voronoiEdgesEnd(); ++pve) {
      const CQChartsHull3D::Edge *e = *pve;

      auto *v1 = e->start();
      auto *v2 = e->end  ();

      double px1, py1, px2, py2;

      windowToPixel(v1->x(), v1->y(), px1, py1);
      windowToPixel(v2->x(), v2->y(), px2, py2);

      CQChartsLineObj::draw(painter, QPointF(px1, py1), QPointF(px2, py2), lc, lw, ld);
    }
  }
}

//------

CQChartsDelaunayPointObj::
CQChartsDelaunayPointObj(CQChartsDelaunayPlot *plot, const CQChartsGeom::BBox &rect,
                         double x, double y, const QModelIndex &ind, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), x_(x), y_(y), ind_(ind), i_(i), n_(n)
{
}

QString
CQChartsDelaunayPointObj::
calcId() const
{
  QString name1;

  if (plot_->nameColumn() >= 0) {
    QModelIndex nameInd = plot_->model()->index(i_, plot_->nameColumn());

    bool ok;

    name1 = CQChartsUtil::modelString(plot_->model(), nameInd, ok);
  }
  else
    name1 = plot_->yname();

  if (name1.length())
    return QString("%1:%2:%3").arg(name1).arg(x_).arg(y_);
  else
    return QString("%1:%2:%3").arg(i_).arg(x_).arg(y_);
}

bool
CQChartsDelaunayPointObj::
visible() const
{
  if (! plot_->isPoints())
    return false;

  return isVisible();
}

bool
CQChartsDelaunayPointObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  double px, py;

  plot_->windowToPixel(x_, y_, px, py);

  double s = plot_->symbolSize();

  CQChartsGeom::BBox pbbox(px - s, py - s, px + s, py + s);

  CQChartsGeom::Point pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsDelaunayPointObj::
addSelectIndex()
{
  plot_->addSelectIndex(ind_.row(), plot_->xColumn(), ind_.parent());
  plot_->addSelectIndex(ind_.row(), plot_->yColumn(), ind_.parent());
}

bool
CQChartsDelaunayPointObj::
isIndex(const QModelIndex &ind) const
{
  return (ind == ind_);
}

void
CQChartsDelaunayPointObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  if (! visible())
    return;

  QPen   pen;
  QBrush brush;

  if (plot_->isSymbolStroked()) {
    QColor pc = plot_->interpPointStrokeColor(0, 1);

    pc.setAlphaF(plot_->pointsStrokeAlpha());

    pen.setColor(pc);
  }
  else
    pen.setStyle(Qt::NoPen);

  pen.setWidthF(plot_->symbolLineWidth());

  if (plot_->isSymbolFilled()) {
    QColor bc = plot_->interpPointFillColor(0, 1);

    bc.setAlphaF(plot_->pointsFillAlpha());

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(bc);
  }
  else
    brush.setStyle(Qt::NoBrush);

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  double s = plot_->symbolSize();

  CQChartsPlotSymbol::Type symbol = plot_->symbolType();

  //---

  double px, py;

  plot_->windowToPixel(x_, y_, px, py);

  CQChartsPointObj::draw(painter, QPointF(px, py), symbol, s);
}
