#include <CQChartsDelaunayPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsDelaunay.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

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
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Delaunay Plot").
    h3("Summary").
     p("Draws delaunay triangulation for a set of points.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/delaunay.png"));
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
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  CQChartsPlot::addProperties();

  // columns
  addProp("columns", "xColumn", "x", "X column");
  addProp("columns", "yColumn", "y", "Y column");

  // voronoi
  addProp("voronoi", "voronoi"         , "visible"  , "Show voronoi connections");
  addProp("voronoi", "voronoiPointSize", "pointSize", "Voronoi point symbol size");

  // points
  addProp("points", "points", "visible", "Center points visible");

  addSymbolProperties("points/symbol", "", "Center points");

  // lines
  addProp("lines", "lines", "visible", "Connecting lines visible");

  addLineProperties("lines/stroke", "lines", "Lines");
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

  NoUpdate noUpdate(this);

  CQChartsDelaunayPlot *th = const_cast<CQChartsDelaunayPlot *>(this);

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

  ColorInd iv;

  if (nr > 0)
    iv = ColorInd(r, nr);

  CQChartsDelaunayPointObj *pointObj =
    new CQChartsDelaunayPointObj(this, bbox, x, y, xind1, iv);

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
execDrawForeground(CQChartsPaintDevice *device) const
{
  device->save();

  setClipRect(device);

  if (! isVoronoi())
    drawDelaunay(device);
  else
    drawVoronoi(device);

  device->restore();
}

void
CQChartsDelaunayPlot::
drawDelaunay(CQChartsPaintDevice *device) const
{
  if (! delaunay_)
    return;

  if (isLines()) {
    QPen pen;

    setLineDataPen(pen, ColorInd());

    //---

    for (auto pf = delaunay_->facesBegin(); pf != delaunay_->facesEnd(); ++pf) {
      const CQChartsHull3D::Face *f = *pf;

      if (! f->isLower()) continue;

      auto *v1 = f->vertex(0);
      auto *v2 = f->vertex(1);
      auto *v3 = f->vertex(2);

      CQChartsGeom::Point p1(v1->x(), v1->y());
      CQChartsGeom::Point p2(v2->x(), v2->y());
      CQChartsGeom::Point p3(v3->x(), v3->y());

      QPainterPath path;

      path.moveTo(p1.x, p1.y);
      path.lineTo(p2.x, p2.y);
      path.lineTo(p3.x, p3.y);

      path.closeSubpath();

      device->strokePath(path, pen);
    }
  }
}

void
CQChartsDelaunayPlot::
drawVoronoi(CQChartsPaintDevice *device) const
{
  if (! delaunay_)
    return;

  if (isPoints()) {
    QPen   pen;
    QBrush brush;

    setSymbolPenBrush(pen, brush, ColorInd());

    device->setPen  (pen);
    device->setBrush(brush);

    for (auto pf = delaunay_->facesBegin(); pf != delaunay_->facesEnd(); ++pf) {
      const CQChartsHull3D::Face *f = *pf;

      CQChartsHull3D::Vertex *v = f->getVoronoi();
      if (! v) continue;

      CQChartsGeom::Point p = windowToPixel(CQChartsGeom::Point(v->x(), v->y()));

      double d = voronoiPointSize();

      QRectF prect(p.x - d, p.y - d, 2.0*d, 2.0*d);

      device->drawArc(device->pixelToWindow(prect), 0, 360); // circle
    }
  }

  //---

  if (isLines()) {
    QPen pen;

    QColor lc = interpLinesColor(ColorInd());

    setPen(pen, true, lc, linesAlpha(), linesWidth(), linesDash());

    device->setPen(pen);

    for (auto pve = delaunay_->voronoiEdgesBegin(); pve != delaunay_->voronoiEdgesEnd(); ++pve) {
      const CQChartsHull3D::Edge *e = *pve;

      auto *v1 = e->start();
      auto *v2 = e->end  ();

      QPointF p1(v1->x(), v1->y());
      QPointF p2(v2->x(), v2->y());

      device->drawLine(p1, p2);
    }
  }
}

//------

CQChartsDelaunayPointObj::
CQChartsDelaunayPointObj(const CQChartsDelaunayPlot *plot, const CQChartsGeom::BBox &rect,
                         double x, double y, const QModelIndex &ind, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsDelaunayPlot *>(plot), rect, ColorInd(), ColorInd(), iv),
 plot_(plot), x_(x), y_(y)
{
  setModelInd(ind);
}

QString
CQChartsDelaunayPointObj::
calcId() const
{
  QString name1;

  if (plot_->nameColumn().isValid()) {
    bool ok;

    name1 = plot_->modelString(modelInd().row(), plot_->nameColumn(), modelInd().parent(), ok);
  }
  else
    name1 = plot_->yname();

  if (name1.length())
    return QString("%1:%2:%3:%4").arg(typeName()).arg(name1).arg(x_).arg(y_);
  else
    return QString("%1:%2:%3:%4").arg(typeName()).arg(iv_.i).arg(x_).arg(y_);
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
draw(CQChartsPaintDevice *device)
{
  if (! visible())
    return;

  // calc pen and brush
  ColorInd colorInd = calcColorInd();

  QPen   pen;
  QBrush brush;

  plot_->setSymbolPenBrush(pen, brush, colorInd);

  plot_->updateObjPenBrushState(this, pen, brush, CQChartsPlot::DrawType::SYMBOL);

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  // get symbol type and size
  CQChartsSymbol symbolType = plot_->symbolType();
  CQChartsLength symbolSize = plot_->symbolSize();

  //---

  // draw symbol
  CQChartsGeom::Point p(x_, y_);

  plot_->drawSymbol(device, p.qpoint(), symbolType, symbolSize, pen, brush);
}
