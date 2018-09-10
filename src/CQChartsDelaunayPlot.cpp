#include <CQChartsDelaunayPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsDelaunay.h>
#include <QPainter>
#include <QMenu>

CQChartsDelaunayPlotType::
CQChartsDelaunayPlotType()
{
}

void
CQChartsDelaunayPlotType::
addParameters()
{
  startParameterGroup("Delaunay");

  addColumnParameter("x", "X", "xColumn", 0).setRequired();
  addColumnParameter("y", "Y", "yColumn", 1).setRequired();

  addColumnParameter("name", "Name", "nameColumn");

  addBoolParameter("voronoi", "Voronoi", "voronoi");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsDelaunayPlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draws delaunay triangulation for a set of points.<p>\n";
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
 CQChartsPlot(view, view->charts()->plotType("delaunay"), model),
 CQChartsPlotLineData <CQChartsDelaunayPlot>(this),
 CQChartsPlotPointData<CQChartsDelaunayPlot>(this)
{
  setPoints(true);

  setSymbolStrokeColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  addAxes();

  addTitle();
}

CQChartsDelaunayPlot::
~CQChartsDelaunayPlot()
{
  delete delaunay_;
}

//---

void
CQChartsDelaunayPlot::
setXColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDelaunayPlot::
setYColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(yColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDelaunayPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsDelaunayPlot::
setVoronoi(bool b)
{
  CQChartsUtil::testAndSet(voronoi_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDelaunayPlot::
setVoronoiPointSize(double r)
{
  CQChartsUtil::testAndSet(voronoiPointSize_, r, [&]() { invalidateLayers(); } );
}

//---

void
CQChartsDelaunayPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "xColumn", "x");
  addProperty("columns", this, "yColumn", "y");

  addProperty("voronoi", this, "voronoi"         , "enabled"  );
  addProperty("voronoi", this, "voronoiPointSize", "pointSize");

  // points
  addProperty("points", this, "points", "visible");

  addSymbolProperties("points/symbol");

  // lines
  addProperty("lines", this, "lines", "visible");

  addLineProperties("lines", "lines");
}

void
CQChartsDelaunayPlot::
calcRange()
{
  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsDelaunayPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const VisitData &data) override {
      bool ok1, ok2;

      double x = plot_->modelReal(data.row, plot_->xColumn(), data.parent, ok1);
      double y = plot_->modelReal(data.row, plot_->yColumn(), data.parent, ok2);

      if (! ok1) x = data.row;
      if (! ok2) y = data.row;

      if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
        return State::SKIP;

      range_.updateRange(x, y);

      return State::OK;
    }

    const CQChartsGeom::Range &range() const { return range_; }

   private:
    CQChartsDelaunayPlot *plot_ { nullptr };
    CQChartsGeom::Range   range_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  dataRange_ = visitor.range();

  nr_ = visitor.numRows();

  //---

  bool ok;

  xAxis_->setColumn(xColumn());

  QString xname = modelHeaderString(xColumn(), ok);

  xAxis_->setLabel(xname);

  yAxis_->setColumn(yColumn());

  QString yname = modelHeaderString(yColumn(), ok);

  yAxis_->setLabel(yname);

  //---

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange_.equalScale(aspect);
  }
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

  bool ok;

  yname_ = modelHeaderString(yColumn(), ok);

  //---

  delete delaunay_;

  delaunay_ = new CQChartsDelaunay;

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsDelaunayPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const VisitData &data) override {
      bool ok1, ok2;

      double x = plot_->modelReal(data.row, plot_->xColumn(), data.parent, ok1);
      double y = plot_->modelReal(data.row, plot_->yColumn(), data.parent, ok2);

      if (! ok1) x = data.row;
      if (! ok2) y = data.row;

      if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
        return State::SKIP;

      QModelIndex xind = plot_->modelIndex(data.row, plot_->xColumn(), data.parent);

      plot_->addPointObj(x, y, xind, ModelVisitor::row());

      return State::OK;
    }

   private:
    CQChartsDelaunayPlot *plot_ { nullptr };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  delaunay_->calc();

  //---

  return true;
}

void
CQChartsDelaunayPlot::
addPointObj(double x, double y, const QModelIndex &xind, int r)
{
  delaunay_->addVertex(x, y);

  //---

  double sw = (dataRange_.xmax() - dataRange_.xmin())/100.0;
  double sh = (dataRange_.ymax() - dataRange_.ymin())/100.0;

  QModelIndex xind1 = normalizeIndex(xind);

  CQChartsGeom::BBox bbox(x - sw/2.0, y - sh/2.0, x + sw/2.0, y + sh/2.0);

  CQChartsDelaunayPointObj *pointObj =
    new CQChartsDelaunayPointObj(this, bbox, x, y, xind1, r, nr_);

  addPlotObject(pointObj);
}

//------

bool
CQChartsDelaunayPlot::
addMenuItems(QMenu *menu)
{
  QAction *voronoiAction = new QAction("Voronoi", menu);

  voronoiAction->setCheckable(true);
  voronoiAction->setChecked(isVoronoi());

  connect(voronoiAction, SIGNAL(triggered(bool)), this, SLOT(setVoronoi(bool)));

  menu->addSeparator();

  menu->addAction(voronoiAction);

  return true;
}

//------

bool
CQChartsDelaunayPlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsDelaunayPlot::
drawForeground(QPainter *painter)
{
  painter->save();

  setClipRect(painter);

  if (! isVoronoi())
    drawDelaunay(painter);
  else
    drawVoronoi(painter);

  painter->restore();
}

void
CQChartsDelaunayPlot::
drawDelaunay(QPainter *painter)
{
  if (isLines()) {
    QPen pen;

    setLineDataPen(pen, 0, 1);

    //---

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

    setSymbolPenBrush(pen, brush, 0, 1);

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
    QPen pen;

    QColor lc = interpLinesColor(0, 1);

    setPen(pen, true, lc, linesAlpha(), linesWidth(), linesDash());

    painter->setPen(pen);

    for (auto pve = delaunay_->voronoiEdgesBegin(); pve != delaunay_->voronoiEdgesEnd(); ++pve) {
      const CQChartsHull3D::Edge *e = *pve;

      auto *v1 = e->start();
      auto *v2 = e->end  ();

      double px1, py1, px2, py2;

      windowToPixel(v1->x(), v1->y(), px1, py1);
      windowToPixel(v2->x(), v2->y(), px2, py2);

      painter->drawLine(QPointF(px1, py1), QPointF(px2, py2));
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

  if (plot_->nameColumn().isValid()) {
    bool ok;

    name1 = plot_->modelString(ind_.row(), plot_->nameColumn(), ind_.parent(), ok);
  }
  else
    name1 = plot_->yname();

  if (name1.length())
    return QString("point:%1:%2:%3").arg(name1).arg(x_).arg(y_);
  else
    return QString("point:%1:%2:%3").arg(i_).arg(x_).arg(y_);
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

  double sx, sy;

  plot_->pixelSymbolSize(plot_->symbolSize(), sx, sy);

  CQChartsGeom::BBox pbbox(px - sx, py - sy, px + sx, py + sy);

  CQChartsGeom::Point pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsDelaunayPointObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->xColumn());
  addColumnSelectIndex(inds, plot_->yColumn());
}

void
CQChartsDelaunayPointObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind_.row(), column, ind_.parent());
}

void
CQChartsDelaunayPointObj::
draw(QPainter *painter)
{
  if (! visible())
    return;

  QPen   pen;
  QBrush brush;

  plot_->setSymbolPenBrush(pen, brush, 0, 1);

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  double sx, sy;

  plot_->pixelSymbolSize(plot_->symbolSize(), sx, sy);

  CQChartsSymbol symbol = plot_->symbolType();

  //---

  double px, py;

  plot_->windowToPixel(x_, y_, px, py);

  plot_->drawSymbol(painter, QPointF(px, py), symbol, CMathUtil::avg(sx, sy), pen, brush);
}
