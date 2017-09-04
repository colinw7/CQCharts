#include <CQChartsDelaunayPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>
#include <CMathGeom2D.h>
#include <CDelaunay.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsDelaunayPlot::
CQChartsDelaunayPlot(CQChartsView *view, QAbstractItemModel *model) :
 CQChartsPlot(view, model)
{
  addAxes();

  xAxis_->setColumn(xColumn_);
  yAxis_->setColumn(yColumn_);
}

void
CQChartsDelaunayPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "xColumn"       , "x"     );
  addProperty("columns", this, "yColumn"       , "y"     );
  addProperty("points" , this, "points"        , "shown" );
  addProperty("points" , this, "pointsColor"   , "color" );
  addProperty("points" , this, "symbolName"    , "symbol");
  addProperty("points" , this, "symbolSize"    , "size"  );
  addProperty("lines"  , this, "lines"         , "shown" );
  addProperty("lines"  , this, "linesColor"    , "color" );
  addProperty(""       , this, "voronoi"                 );
}

QString
CQChartsDelaunayPlot::
symbolName() const
{
  return CSymbol2DMgr::typeToName(symbolType()).c_str();
}

void
CQChartsDelaunayPlot::
setSymbolName(const QString &s)
{
  CSymbol2D::Type type = CSymbol2DMgr::nameToType(s.toStdString());

  if (type != CSymbol2D::Type::NONE)
    setSymbolType(type);
}

void
CQChartsDelaunayPlot::
updateRange()
{
  int n = model_->rowCount(QModelIndex());

  dataRange_.reset();

  for (int i = 0; i < n; ++i) {
    bool ok1, ok2;

    double x = CQChartsUtil::modelReal(model_, i, xColumn_, ok1);
    double y = CQChartsUtil::modelReal(model_, i, yColumn_, ok2);

    if (! ok1) x = i;
    if (! ok2) y = i;

    dataRange_.updateRange(x, y);
  }

  //---

  xAxis_->setColumn(xColumn_);

  QString xname = model_->headerData(xColumn_, Qt::Horizontal).toString();

  xAxis_->setLabel(xname);

  yAxis_->setColumn(yColumn_);

  QString yname = model_->headerData(yColumn_, Qt::Horizontal).toString();

  yAxis_->setLabel(yname);

  //---

  applyDataRange();
}

void
CQChartsDelaunayPlot::
initObjs(bool force)
{
  if (force) {
    clearPlotObjects();

    dataRange_.reset();

    delete delaunay_;

    delaunay_ = nullptr;
  }

  //--

  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return;
  }

  //---

  if (! plotObjs_.empty())
    return;

  //--

  double sw = (dataRange_.xmax() - dataRange_.xmin())/100.0;
  double sh = (dataRange_.ymax() - dataRange_.ymin())/100.0;

  int n = model_->rowCount(QModelIndex());

  QString name = model_->headerData(yColumn_, Qt::Horizontal).toString();

  //---

  if (! delaunay_)
    delaunay_ = new CDelaunay;

  for (int i = 0; i < n; ++i) {
    bool ok1, ok2;

    double x = CQChartsUtil::modelReal(model_, i, xColumn_, ok1);
    double y = CQChartsUtil::modelReal(model_, i, yColumn_, ok2);

    if (! ok1) x = i;
    if (! ok2) y = i;

    delaunay_->addVertex(x, y);

    QString name1;

    if (nameColumn_ >= 0) {
      bool ok;

      name1 = CQChartsUtil::modelString(model_, i, nameColumn_, ok);
    }
    else
      name1 = name;

    CBBox2D bbox(x - sw/2, y - sh/2, x + sw/2, y + sh/2);

    CQChartsDelaunayPointObj *pointObj = new CQChartsDelaunayPointObj(this, bbox, x, y, i, n);

    if (name1.length())
      pointObj->setId(QString("%1:%2:%3").arg(name1).arg(x).arg(y));
    else
      pointObj->setId(QString("%1:%2:%3").arg(i).arg(x).arg(y));

    addPlotObject(pointObj);
  }

  delaunay_->calc();
}

void
CQChartsDelaunayPlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawBackground(p);

  //---

  drawObjs(p);

  //---

  if (! isVoronoi())
    drawDelaunay(p);
  else
    drawVoronoi(p);

  //---

  drawAxes(p);
}

void
CQChartsDelaunayPlot::
drawDelaunay(QPainter *p)
{
  if (isLines()) {
    QColor lc = linesColor();

    for (auto pf = delaunay_->facesBegin(); pf != delaunay_->facesEnd(); ++pf) {
      const CHull3D::Face *f = *pf;

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

      p->strokePath(path, QPen(lc));
    }
  }
}

void
CQChartsDelaunayPlot::
drawVoronoi(QPainter *p)
{
  if (isPoints()) {
    QColor pc = pointsColor();

    p->setPen(pc);

    for (auto pf = delaunay_->facesBegin(); pf != delaunay_->facesEnd(); ++pf) {
      const CHull3D::Face *f = *pf;

      CHull3D::Vertex *v = f->getVoronoi();
      if (! v) continue;

      double px, py;

      windowToPixel(v->x(), v->y(), px, py);

      int d = 2;

      QRectF rect(px - d, py - d, 2*d, 2*d);

      p->drawArc(rect, 0, 16*360);
    }
  }

  //---

  if (isLines()) {
    QColor lc = linesColor();

    p->setPen(lc);

    for (auto pve = delaunay_->voronoiEdgesBegin(); pve != delaunay_->voronoiEdgesEnd(); ++pve) {
      const CHull3D::Edge *e = *pve;

      auto *v1 = e->start();
      auto *v2 = e->end  ();

      double px1, py1, px2, py2;

      windowToPixel(v1->x(), v1->y(), px1, py1);
      windowToPixel(v2->x(), v2->y(), px2, py2);

      p->drawLine(px1, py1, px2, py2);
    }
  }
}

//------

CQChartsDelaunayPointObj::
CQChartsDelaunayPointObj(CQChartsDelaunayPlot *plot, const CBBox2D &rect, double x, double y,
                         int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), x_(x), y_(y), i_(i), n_(n)
{
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
inside(const CPoint2D &p) const
{
  double px, py;

  plot_->windowToPixel(x_, y_, px, py);

  double s = plot_->symbolSize();

  CBBox2D pbbox(px - s, py - s, px + s, py + s);

  CPoint2D pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsDelaunayPointObj::
draw(QPainter *p)
{
  if (plot_->isPoints()) {
    QColor c = plot_->objectStateColor(this, plot_->pointsColor());
    double s = plot_->symbolSize();

    plot_->drawSymbol(p, CPoint2D(x_, y_), plot_->symbolType(), s, c);
  }
}
