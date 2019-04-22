#include <CQChartsDelaunayPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsDelaunay.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

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

  addColumnParameter("x", "X", "xColumn").
    setRequired().setNumeric().setTip("X Value Column");
  addColumnParameter("y", "Y", "yColumn").
    setRequired().setNumeric().setTip("Y Value Column");

  addColumnParameter("name", "Name", "nameColumn").
    setString().setTip("Optional Name Column");

  addBoolParameter("voronoi", "Voronoi", "voronoi").
    setTip("Draw Voronoi");

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
 CQChartsObjLineData <CQChartsDelaunayPlot>(this),
 CQChartsObjPointData<CQChartsDelaunayPlot>(this)
{
  NoUpdate noUpdate(this);

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
  CQChartsUtil::testAndSet(voronoiPointSize_, r, [&]() { drawObjs(); } );
}

//---

void
CQChartsDelaunayPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "xColumn", "x")->setDesc("X column");
  addProperty("columns", this, "yColumn", "y")->setDesc("Y column");

  addProperty("voronoi", this, "voronoi"         , "enabled"  )->
    setDesc("Show voronoi connections");
  addProperty("voronoi", this, "voronoiPointSize", "pointSize")->
    setDesc("Voronoi point symbol size");

  // points
  addProperty("points", this, "points", "visible")->setDesc("Show center points");

  addSymbolProperties("points/symbol");

  // lines
  addProperty("lines", this, "lines", "visible")->setDesc("Show connecting lines");

  addLineProperties("lines", "lines");
}

CQChartsGeom::Range
CQChartsDelaunayPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsDelaunayPlot::calcRange");

  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsDelaunayPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
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
    const CQChartsDelaunayPlot* plot_ { nullptr };
    CQChartsGeom::Range         range_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  CQChartsGeom::Range dataRange = visitor.range();

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

    dataRange.equalScale(aspect);
  }

  //---

  return dataRange;
}

bool
CQChartsDelaunayPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsDelaunayPlot::createObjs");

  CQChartsDelaunayPlot *th = const_cast<CQChartsDelaunayPlot *>(this);

  NoUpdate noUpdate(th);

  //---

  bool ok;

  th->yname_ = modelHeaderString(yColumn(), ok);

  //---

  delete th->delaunay_;

  th->delaunay_ = new CQChartsDelaunay;

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsDelaunayPlot *plot, PlotObjs &objs) :
     plot_(plot), objs_(objs) {
      nr_ = numRows();
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      bool ok1, ok2;

      double x = plot_->modelReal(data.row, plot_->xColumn(), data.parent, ok1);
      double y = plot_->modelReal(data.row, plot_->yColumn(), data.parent, ok2);

      if (! ok1) x = data.row;
      if (! ok2) y = data.row;

      if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
        return State::SKIP;

      QModelIndex xind = plot_->modelIndex(data.row, plot_->xColumn(), data.parent);

      plot_->addPointObj(x, y, xind, ModelVisitor::row(), nr_, objs_);

      return State::OK;
    }

   private:
    const CQChartsDelaunayPlot* plot_ { nullptr };
    PlotObjs&                   objs_;
    int                         nr_   { 0 };
  };

  RowVisitor visitor(this, objs);

  visitModel(visitor);

  //---

  th->delaunay_->calc();

  //---

  return true;
}

void
CQChartsDelaunayPlot::
addPointObj(double x, double y, const QModelIndex &xind, int r, int nr, PlotObjs &objs) const
{
  assert(delaunay_);

  CQChartsDelaunayPlot *th = const_cast<CQChartsDelaunayPlot *>(this);

  th->delaunay_->addVertex(x, y);

  //---

  const CQChartsGeom::Range &dataRange = this->dataRange();

  double sw = (dataRange.xmax() - dataRange.xmin())/100.0;
  double sh = (dataRange.ymax() - dataRange.ymin())/100.0;

  QModelIndex xind1 = normalizeIndex(xind);

  CQChartsGeom::BBox bbox(x - sw/2.0, y - sh/2.0, x + sw/2.0, y + sh/2.0);

  CQChartsDelaunayPointObj *pointObj =
    new CQChartsDelaunayPointObj(this, bbox, x, y, xind1, r, nr);

  objs.push_back(pointObj);
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
execDrawForeground(QPainter *painter) const
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
drawDelaunay(QPainter *painter) const
{
  if (! delaunay_)
    return;

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

      CQChartsGeom::Point p1 = windowToPixel(CQChartsGeom::Point(v1->x(), v1->y()));
      CQChartsGeom::Point p2 = windowToPixel(CQChartsGeom::Point(v2->x(), v2->y()));
      CQChartsGeom::Point p3 = windowToPixel(CQChartsGeom::Point(v3->x(), v3->y()));

      QPainterPath path;

      path.moveTo(p1.x, p1.y);
      path.lineTo(p2.x, p2.y);
      path.lineTo(p3.x, p3.y);

      path.closeSubpath();

      painter->strokePath(path, pen);
    }
  }
}

void
CQChartsDelaunayPlot::
drawVoronoi(QPainter *painter) const
{
  if (! delaunay_)
    return;

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

      CQChartsGeom::Point p = windowToPixel(CQChartsGeom::Point(v->x(), v->y()));

      double d = voronoiPointSize();

      QRectF rect(p.x - d, p.y - d, 2.0*d, 2.0*d);

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

      CQChartsGeom::Point p1 = windowToPixel(CQChartsGeom::Point(v1->x(), v1->y()));
      CQChartsGeom::Point p2 = windowToPixel(CQChartsGeom::Point(v2->x(), v2->y()));

      painter->drawLine(QPointF(p1.x, p1.y), QPointF(p2.x, p2.y));
    }
  }
}

//------

CQChartsDelaunayPointObj::
CQChartsDelaunayPointObj(const CQChartsDelaunayPlot *plot, const CQChartsGeom::BBox &rect,
                         double x, double y, const QModelIndex &ind, int i, int n) :
 CQChartsPlotObj(const_cast<CQChartsDelaunayPlot *>(plot), rect), plot_(plot),
 x_(x), y_(y), ind_(ind), i_(i), n_(n)
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
    return QString("%1:%2:%3:%4").arg(typeName()).arg(name1).arg(x_).arg(y_);
  else
    return QString("%1:%2:%3:%4").arg(typeName()).arg(i_).arg(x_).arg(y_);
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

  CQChartsGeom::Point p1 = plot_->windowToPixel(CQChartsGeom::Point(x_, y_));

  double sx, sy;

  plot_->pixelSymbolSize(plot_->symbolSize(), sx, sy);

  CQChartsGeom::BBox pbbox(p1.x - sx, p1.y - sy, p1.x + sx, p1.y + sy);

  CQChartsGeom::Point pp = plot_->windowToPixel(p);

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

  // calc pen and brush
  QPen   pen;
  QBrush brush;

  plot_->setSymbolPenBrush(pen, brush, 0, 1);

  plot_->updateObjPenBrushState(this, pen, brush, CQChartsPlot::DrawType::SYMBOL);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  // get symbol type and size
  double sx, sy;

  plot_->pixelSymbolSize(plot_->symbolSize(), sx, sy);

  CQChartsSymbol symbol = plot_->symbolType();

  //---

  // draw symbol
  CQChartsGeom::Point p = plot_->windowToPixel(CQChartsGeom::Point(x_, y_));

  plot_->drawSymbol(painter, p.qpoint(), symbol, CMathUtil::avg(sx, sy), pen, brush);
}
