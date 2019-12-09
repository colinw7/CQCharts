#include <CQChartsDelaunayPlot.h>
#include <CQChartsGrahamHull.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsDelaunay.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsTip.h>
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

  addColumnParameter("name" , "Name" , "nameColumn").
    setString().setTip("Optional Name Column");
  addColumnParameter("value", "Value", "valueColumn").
    setString().setTip("Optional Value Column");

  addBoolParameter("delaunay", "Delaunay", "delaunay").
    setTip("Draw Delaunay");

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
 CQChartsObjDelaunayLineData<CQChartsDelaunayPlot>(this),
 CQChartsObjVoronoiLineData <CQChartsDelaunayPlot>(this),
 CQChartsObjVoronoiPointData<CQChartsDelaunayPlot>(this),
 CQChartsObjVoronoiShapeData<CQChartsDelaunayPlot>(this),
 CQChartsObjPointData       <CQChartsDelaunayPlot>(this)
{
  NoUpdate noUpdate(this);

  CQChartsObjDelaunayLineData::setDelaunayReloadObj(false);
  CQChartsObjVoronoiLineData ::setVoronoiReloadObj (false);
  CQChartsObjVoronoiPointData::setVoronoiReloadObj (false);
  CQChartsObjVoronoiShapeData::setVoronoiReloadObj (false);

  setDelaunay(true);
  setVoronoi (false);

  setPoints(true);

  setSymbolStrokeColor(CQChartsColor(CQChartsColor::Type::PALETTE));
  setVoronoiFillColor (CQChartsColor(CQChartsColor::Type::PALETTE));

  addAxes();

  addTitle();
}

CQChartsDelaunayPlot::
~CQChartsDelaunayPlot()
{
  delete delaunayData_;
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

void
CQChartsDelaunayPlot::
setValueColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsDelaunayPlot::
setDelaunay(bool b)
{
  CQChartsUtil::testAndSet(delaunay_, b, [&]() { drawObjs(); } );
}

void
CQChartsDelaunayPlot::
setVoronoi(bool b)
{
  CQChartsUtil::testAndSet(voronoi_, b, [&]() { drawObjs(); } );
}

void
CQChartsDelaunayPlot::
setVoronoiCircles(bool b)
{
  CQChartsUtil::testAndSet(voronoiCircles_, b, [&]() { drawObjs(); } );
}

void
CQChartsDelaunayPlot::
setVoronoiPolygon(bool b)
{
  CQChartsUtil::testAndSet(voronoiPolygon_, b, [&]() { drawObjs(); } );
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

  addProp("columns", "valueColumn", "value", "Value column");

  // delaunay
  addProp("delaunay", "delaunay", "visible", "Show delaunay connections");

  addProp("delaunay/lines", "delaunayLines", "visible", "Connecting lines visible");

  addLineProperties("delaunay/lines/stroke", "delaunayLines", "Delaunay Stroke");

  // voronoi
  addProp("voronoi", "voronoi", "visible", "Show voronoi connections");

  addProp("voronoi/lines", "voronoiLines", "visible", "Connecting lines visible");

  addLineProperties("voronoi/lines/stroke", "voronoiLines", "Voronoi Stroke");

  addProp("voronoi/points", "voronoiPoints", "visible", "Draw Voronoi points");

  addSymbolProperties("voronoi/points/symbol", "voronoi", "Voronoi points symbol");

  addProp("voronoi/circle", "voronoiCircles", "visible", "Draw Voronoi circles");

  addProp("voronoi/polygon", "voronoiPolygon", "visible", "Draw Voronoi polygons");

  addFillProperties("voronoi/polygon/fill"  , "voronoiFill"  , "Voronoi polygon fill");
  addLineProperties("voronoi/polygon/stroke", "voronoiStroke", "Voronoi polygon stroke");

  // points
  addProp("points", "points", "visible", "Center points visible");

  addSymbolProperties("points/symbol", "", "Data points symbol");
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

  QString xname = modelHHeaderString(xColumn(), ok);

  xAxis_->setLabel(xname);

  yAxis_->setColumn(yColumn());

  QString yname = modelHHeaderString(yColumn(), ok);

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

  th->yname_ = modelHHeaderString(yColumn(), ok);

  //---

  delete th->delaunayData_;

  th->delaunayData_ = new CQChartsDelaunay;

  //---

  // create points for original data points
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsDelaunayPlot *plot, PlotObjs &objs) :
     plot_(plot), objs_(objs) {
      nr_ = numRows();
    }

    const CQChartsGeom::RMinMax &valueRange() const { return valueRange_; }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      bool ok1, ok2, ok3;

      double x = plot_->modelReal(data.row, plot_->xColumn    (), data.parent, ok1);
      double y = plot_->modelReal(data.row, plot_->yColumn    (), data.parent, ok2);

      if (! ok1) x = data.row;
      if (! ok2) y = data.row;

      double value = plot_->modelReal(data.row, plot_->valueColumn(), data.parent, ok3);

      if (ok3)
        valueRange_.add(value);
      else
        value = 0.0;

      if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
        return State::SKIP;

      QModelIndex xind = plot_->modelIndex(data.row, plot_->xColumn(), data.parent);

      plot_->addPointObj(x, y, value, xind, ModelVisitor::row(), nr_, objs_);

      return State::OK;
    }

   private:
    const CQChartsDelaunayPlot* plot_      { nullptr };
    PlotObjs&                   objs_;
    int                         nr_        { 0 };
    CQChartsGeom::RMinMax       valueRange_;
  };

  RowVisitor visitor(this, objs);

  visitModel(visitor);

  th->valueRange_ = visitor.valueRange();

  //---

  th->delaunayData_->calc();

  //---

  return true;
}

void
CQChartsDelaunayPlot::
addPointObj(double x, double y, double value, const QModelIndex &xind,
            int r, int nr, PlotObjs &objs) const
{
  assert(delaunayData_);

  CQChartsDelaunayPlot *th = const_cast<CQChartsDelaunayPlot *>(this);

  CQChartsDelaunay::PVertex v = th->delaunayData_->addVertex(x, y);

  v->setValue(value);

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
    new CQChartsDelaunayPointObj(this, bbox, x, y, value, xind1, iv);

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
hasBackground() const
{
  return isLayerActive(CQChartsLayer::Type::BACKGROUND);
}

bool
CQChartsDelaunayPlot::
hasForeground() const
{
  return false;
}

void
CQChartsDelaunayPlot::
execDrawBackground(CQChartsPaintDevice *device) const
{
  device->save();

  setClipRect(device);

  if (isDelaunay())
    drawDelaunay(device);

  if (isVoronoi())
    drawVoronoi(device);

  device->restore();
}

void
CQChartsDelaunayPlot::
execDrawForeground(CQChartsPaintDevice *) const
{
}

void
CQChartsDelaunayPlot::
drawDelaunay(CQChartsPaintDevice *device) const
{
  if (! delaunayData_)
    return;

  if (isDelaunayLines()) {
    QPen pen;

    setDelaunayLineDataPen(pen, ColorInd());

    //---

    // draw delaunay triangles
    for (auto pf = delaunayData_->facesBegin(); pf != delaunayData_->facesEnd(); ++pf) {
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
  if (! delaunayData_)
    return;

  //---

  // fill voronoi polygons
  if (isVoronoiPolygon()) {
    CQChartsPenBrush penBrush;

    QColor pc = interpVoronoiStrokeColor(ColorInd());
    QColor fc = interpVoronoiFillColor  (ColorInd());

    setPenBrush(penBrush,
      CQChartsPenData  (true, pc, voronoiStrokeAlpha(), voronoiStrokeWidth(), voronoiStrokeDash()),
      CQChartsBrushData(true, fc, voronoiFillAlpha(), voronoiFillPattern()));

    for (auto pv = delaunayData_->verticesBegin(); pv != delaunayData_->verticesEnd(); ++pv) {
      const CQChartsHull3D::Vertex *v1 = *pv;

      CQChartsGrahamHull hull;

      for (const auto &v2 : v1->voronoi()) {
        QPointF p(v2->x(), v2->y());

        hull.addPoint(p);
      }

      QPolygonF poly;

      hull.getHull(poly);

      QBrush brush = penBrush.brush;

      if (valueRange_.isSet()) {
        double v = CMathUtil::map(v1->value(), valueRange_.min(), valueRange_.max(), 0.0, 1.0);

        QColor fc1 = interpVoronoiFillColor(ColorInd(v));

        brush.setColor(fc1);
      }

      device->setPen  (penBrush.pen);
      device->setBrush(brush);

      device->drawPolygon(poly);
    }
  }

  //---

  // draw voronoi points
  if (isVoronoiPoints()) {
    CQChartsPenBrush penBrush;

    setVoronoiSymbolPenBrush(penBrush, ColorInd());

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    // get symbol type and size
    CQChartsSymbol symbolType = this->voronoiSymbolType();
    CQChartsLength symbolSize = this->voronoiSymbolSize();

    for (auto pf = delaunayData_->facesBegin(); pf != delaunayData_->facesEnd(); ++pf) {
      const CQChartsHull3D::Face *f = *pf;

      CQChartsHull3D::Vertex *v = f->getVoronoi();
      if (! v) continue;

      CQChartsGeom::Point p(v->x(), v->y());

      drawSymbol(device, p.qpoint(), symbolType, symbolSize, penBrush);
    }
  }

  //---

  // draw voronoi lines
  if (isVoronoiLines() || isVoronoiCircles()) {
    QPen pen;

    QColor lc = interpVoronoiLinesColor(ColorInd());

    setPen(pen, true, lc, voronoiLinesAlpha(), voronoiLinesWidth(), voronoiLinesDash());

    device->setPen  (pen);
    device->setBrush(Qt::NoBrush);

    for (auto pve = delaunayData_->voronoiEdgesBegin();
           pve != delaunayData_->voronoiEdgesEnd(); ++pve) {
      const CQChartsHull3D::Edge *e = *pve;

      auto *v1 = e->start();
      auto *v2 = e->end  ();

      if (isVoronoiLines()) {
        QPointF p1(v1->x(), v1->y());
        QPointF p2(v2->x(), v2->y());

        device->drawLine(p1, p2);
      }

      if (isVoronoiCircles()) {
        double r = v1->z();

        CQChartsGeom::BBox bbox(v1->x() - r, v1->y() - r, v1->x() + r, v1->y() + r);

        device->drawEllipse(bbox.qrect());
      }
    }
  }
}

//------

CQChartsDelaunayPointObj::
CQChartsDelaunayPointObj(const CQChartsDelaunayPlot *plot, const CQChartsGeom::BBox &rect,
                         double x, double y, double value, const QModelIndex &ind,
                         const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsDelaunayPlot *>(plot), rect, ColorInd(), ColorInd(), iv),
 plot_(plot), x_(x), y_(y), value_(value)
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

QString
CQChartsDelaunayPointObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  if (plot_->nameColumn().isValid()) {
    bool ok;

    QString name =
      plot_->modelString(modelInd().row(), plot_->nameColumn(), modelInd().parent(), ok);

    if (ok && name.length())
      tableTip.addTableRow("Name", name);
  }

  //---

  // add x, y columns
  QString xstr = plot()->xStr(x());
  QString ystr = plot()->yStr(y());

  tableTip.addTableRow("X", xstr);
  tableTip.addTableRow("Y", ystr);

  if (plot_->valueColumn().isValid())
    tableTip.addTableRow("Value", value());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
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

  CQChartsPenBrush penBrush;

  plot_->setSymbolPenBrush(penBrush, colorInd);

  plot_->updateObjPenBrushState(this, penBrush, CQChartsPlot::DrawType::SYMBOL);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // get symbol type and size
  CQChartsSymbol symbolType = plot_->symbolType();
  CQChartsLength symbolSize = plot_->symbolSize();

  //---

  // draw symbol
  CQChartsGeom::Point p(x_, y_);

  plot_->drawSymbol(device, p.qpoint(), symbolType, symbolSize, penBrush);
}
