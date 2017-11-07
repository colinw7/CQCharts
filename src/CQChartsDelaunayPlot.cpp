#include <CQChartsDelaunayPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CDelaunay.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsDelaunayPlotType::
CQChartsDelaunayPlotType()
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
  setLinesColor (QColor(40, 40, 200));
  setPointsColor(QColor(200, 40, 40));

  addAxes();

  addTitle();
}

void
CQChartsDelaunayPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "xColumn"       , "x"      );
  addProperty("columns", this, "yColumn"       , "y"      );
  addProperty("points" , this, "points"        , "visible");
  addProperty("points" , this, "pointsColor"   , "color"  );
  addProperty("points" , this, "symbolName"    , "symbol" );
  addProperty("points" , this, "symbolSize"    , "size"   );
  addProperty("points" , this, "symbolFilled"  , "filled" );
  addProperty("lines"  , this, "lines"         , "visible");
  addProperty("lines"  , this, "linesColor"    , "color"  );
  addProperty("lines"  , this, "linesWidth"    , "width"  );
  addProperty(""       , this, "voronoi"                  );
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

  xAxis_->setColumn(xColumn());

  QString xname = model->headerData(xColumn(), Qt::Horizontal).toString();

  xAxis_->setLabel(xname);

  yAxis_->setColumn(yColumn());

  QString yname = model->headerData(yColumn(), Qt::Horizontal).toString();

  yAxis_->setLabel(yname);

  //---

  if (apply)
    applyDataRange();
}

void
CQChartsDelaunayPlot::
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

  //--

  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  double sw = (dataRange_.xmax() - dataRange_.xmin())/100.0;
  double sh = (dataRange_.ymax() - dataRange_.ymin())/100.0;

  int nr = model->rowCount(QModelIndex());

  QString name = model->headerData(yColumn(), Qt::Horizontal).toString();

  //---

  delete delaunay_;

  delaunay_ = new CDelaunay;

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

    QString name1;

    if (nameColumn() >= 0) {
      QModelIndex nameInd = model->index(r, nameColumn());

      bool ok;

      name1 = CQChartsUtil::modelString(model, nameInd, ok);
    }
    else
      name1 = name;

    //---

    QModelIndex xind1 = normalizeIndex(xind);

    CBBox2D bbox(x - sw/2, y - sh/2, x + sw/2, y + sh/2);

    CQChartsDelaunayPointObj *pointObj =
      new CQChartsDelaunayPointObj(this, bbox, x, y, xind1, r, nr);

    if (name1.length())
      pointObj->setId(QString("%1:%2:%3").arg(name1).arg(x).arg(y));
    else
      pointObj->setId(QString("%1:%2:%3").arg(r).arg(x).arg(y));

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

  drawParts(p);
}

void
CQChartsDelaunayPlot::
drawForeground(QPainter *p)
{
  if (! isVoronoi())
    drawDelaunay(p);
  else
    drawVoronoi(p);
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
    QColor    lc = linesColor();
    double    lw = linesWidth();
    CLineDash ld;

    for (auto pve = delaunay_->voronoiEdgesBegin(); pve != delaunay_->voronoiEdgesEnd(); ++pve) {
      const CHull3D::Edge *e = *pve;

      auto *v1 = e->start();
      auto *v2 = e->end  ();

      double px1, py1, px2, py2;

      windowToPixel(v1->x(), v1->y(), px1, py1);
      windowToPixel(v2->x(), v2->y(), px2, py2);

      CQChartsLineObj::draw(p, QPointF(px1, py1), QPointF(px2, py2), lc, lw, ld);
    }
  }
}

//------

CQChartsDelaunayPointObj::
CQChartsDelaunayPointObj(CQChartsDelaunayPlot *plot, const CBBox2D &rect, double x, double y,
                         const QModelIndex &ind, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), x_(x), y_(y), ind_(ind), i_(i), n_(n)
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
mousePress(const CPoint2D &)
{
  plot_->beginSelect();

  plot_->addSelectIndex(ind_.row(), plot_->xColumn(), ind_.parent());
  plot_->addSelectIndex(ind_.row(), plot_->yColumn(), ind_.parent());

  plot_->endSelect();
}

void
CQChartsDelaunayPointObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  if (plot_->isPoints()) {
    QColor          c      = plot_->objectStateColor(this, plot_->pointsColor());
    double          s      = plot_->symbolSize();
    CSymbol2D::Type symbol = plot_->symbolType();
    bool            filled = plot_->isSymbolFilled();

    double px, py;

    plot_->windowToPixel(x_, y_, px, py);

    CQChartsPointObj::draw(p, QPointF(px, py), symbol, s, c, filled);
  }
}
