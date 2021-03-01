#include <CQChartsDelaunayPlot.h>
#include <CQChartsGrahamHull.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsDelaunay.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>
#include <CQChartsWidgetUtil.h>

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
    setRequired().setNumeric().setPropPath("columns.x").setTip("X Value Column");
  addColumnParameter("y", "Y", "yColumn").
    setRequired().setNumeric().setPropPath("columns.y").setTip("Y Value Column");

  addColumnParameter("name" , "Name" , "nameColumn").
    setString().setPropPath("columns.name").setTip("Optional Name Column");
  addColumnParameter("value", "Value", "valueColumn").
    setString().setPropPath("columns.value").setTip("Optional Value Column");

  addBoolParameter("delaunay", "Delaunay", "delaunay").setTip("Draw Delaunay");
  addBoolParameter("voronoi" , "Voronoi" , "voronoi" ).setTip("Draw Voronoi" );

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

void
CQChartsDelaunayPlotType::
analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData)
{
  auto *details = modelData->details();
  if (! details) return;

  CQChartsColumn xColumn, yColumn;

  int nc = details->numColumns();

  for (int i = 0; i < nc; ++i) {
    auto *columnDetails = details->columnDetails(Column(i));

    if (columnDetails && columnDetails->isNumeric()) {
      if      (! xColumn.isValid())
        xColumn = columnDetails->column();
      else if (! yColumn.isValid())
        yColumn = columnDetails->column();
    }
  }

  if (xColumn.isValid())
    analyzeModelData.parameterNameColumn["x"] = xColumn;

  if (yColumn.isValid())
    analyzeModelData.parameterNameColumn["y"] = yColumn;
}

CQChartsPlot *
CQChartsDelaunayPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsDelaunayPlot(view, model);
}

//------

CQChartsDelaunayPlot::
CQChartsDelaunayPlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("delaunay"), model),
 CQChartsObjDelaunayLineData<CQChartsDelaunayPlot>(this),
 CQChartsObjVoronoiLineData <CQChartsDelaunayPlot>(this),
 CQChartsObjVoronoiPointData<CQChartsDelaunayPlot>(this),
 CQChartsObjVoronoiShapeData<CQChartsDelaunayPlot>(this),
 CQChartsObjPointData       <CQChartsDelaunayPlot>(this)
{
}

CQChartsDelaunayPlot::
~CQChartsDelaunayPlot()
{
  term();
}

//---

void
CQChartsDelaunayPlot::
init()
{
  CQChartsPlot::init();

  //---

  NoUpdate noUpdate(this);

  CQChartsObjDelaunayLineData::setDelaunayReloadObj(false);
  CQChartsObjVoronoiLineData ::setVoronoiReloadObj (false);
  CQChartsObjVoronoiPointData::setVoronoiReloadObj (false);
  CQChartsObjVoronoiShapeData::setVoronoiReloadObj (false);

  setDelaunay(true);
  setVoronoi (false);

  setPoints(true);

  setSymbolStrokeColor(Color(Color::Type::PALETTE));
  setVoronoiFillColor (Color(Color::Type::PALETTE));

  addAxes();

  addTitle();
}

void
CQChartsDelaunayPlot::
term()
{
  delete delaunayData_;
}

//---

void
CQChartsDelaunayPlot::
setXColumn(const Column &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDelaunayPlot::
setYColumn(const Column &c)
{
  CQChartsUtil::testAndSet(yColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDelaunayPlot::
setNameColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDelaunayPlot::
setValueColumn(const Column &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

CQChartsColumn
CQChartsDelaunayPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "x"    ) c = this->xColumn();
  else if (name == "y"    ) c = this->yColumn();
  else if (name == "name" ) c = this->nameColumn();
  else if (name == "value") c = this->valueColumn();
  else                      c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsDelaunayPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "x"    ) this->setXColumn(c);
  else if (name == "y"    ) this->setYColumn(c);
  else if (name == "name" ) this->setNameColumn(c);
  else if (name == "value") this->setValueColumn(c);
  else                      CQChartsPlot::setNamedColumn(name, c);
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

  addBaseProperties();

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

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsDelaunayPlot *>(this);

  th->clearErrors();

  //---

  // check columns
  bool columnsValid = true;

  if (! checkColumn(xColumn    (), "X"    )) columnsValid = false;
  if (! checkColumn(yColumn    (), "Y"    )) columnsValid = false;
  if (! checkColumn(nameColumn (), "Name" )) columnsValid = false;
  if (! checkColumn(valueColumn(), "Value")) columnsValid = false;

  if (! columnsValid)
    return Range(0.0, 0.0, 1.0, 1.0);

  //---

  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsDelaunayPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      ModelIndex xInd(plot_, data.row, plot_->xColumn(), data.parent);
      ModelIndex yInd(plot_, data.row, plot_->yColumn(), data.parent);

      bool ok1, ok2;

      double x = plot_->modelReal(xInd, ok1);
      double y = plot_->modelReal(yInd, ok2);

      if (! ok1) { addDataError(xInd, "Bad X Value"); x = data.row; }
      if (! ok2) { addDataError(yInd, "Bad Y Value"); y = data.row; }

      if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
        return State::SKIP;

      //---

      range_.updateRange(x, y);

      return State::OK;
    }

    const Range &range() const { return range_; }

   private:
    void addDataError(const ModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsDelaunayPlot *>(plot_)->addDataError(ind, msg);
    }

   private:
    const CQChartsDelaunayPlot* plot_ { nullptr };
    Range                       range_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  auto dataRange = visitor.range();

  //---

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange.equalScale(aspect);
  }

  //---

  return dataRange;
}

void
CQChartsDelaunayPlot::
postCalcRange()
{
  bool ok;

  xAxis_->setColumn(xColumn());

  auto xname = modelHHeaderString(xColumn(), ok);

  xAxis_->setDefLabel(xname);

  yAxis_->setColumn(yColumn());

  auto yname = modelHHeaderString(yColumn(), ok);

  yAxis_->setDefLabel(yname);
}

bool
CQChartsDelaunayPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsDelaunayPlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsDelaunayPlot *>(this);

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

    const RMinMax &valueRange() const { return valueRange_; }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      ModelIndex xInd(plot_, data.row, plot_->xColumn(), data.parent);
      ModelIndex yInd(plot_, data.row, plot_->yColumn(), data.parent);

      bool ok1, ok2;

      double x = plot_->modelReal(xInd, ok1);
      double y = plot_->modelReal(yInd, ok2);

      if (! ok1) { addDataError(xInd, "Bad X Value"); x = data.row; }
      if (! ok2) { addDataError(yInd, "Bad Y Value"); y = data.row; }

      if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
        return State::SKIP;

      //---

      double value = 0.0;

      if (plot_->valueColumn().isValid()) {
        ModelIndex valueInd(plot_, data.row, plot_->valueColumn(), data.parent);

        bool ok3;

        value = plot_->modelReal(valueInd, ok3);

        if (ok3)
          valueRange_.add(value);
        else {
          addDataError(valueInd, "Bad Value");
          value = 0.0;
        }
      }

      //---

      auto xInd1 = plot_->modelIndex(xInd);

      plot_->addPointObj(x, y, value, xInd1, ModelVisitor::row(), nr_, objs_);

      return State::OK;
    }

   private:
    void addDataError(const ModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsDelaunayPlot *>(plot_)->addDataError(ind, msg);
    }

   private:
    const CQChartsDelaunayPlot* plot_      { nullptr };
    PlotObjs&                   objs_;
    int                         nr_        { 0 };
    RMinMax                     valueRange_;
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

  auto *th = const_cast<CQChartsDelaunayPlot *>(this);

  auto v = th->delaunayData_->addVertex(x, y);

  v->setValue(value);

  //---

  const auto &dataRange = this->dataRange();

  double sw = (dataRange.xmax() - dataRange.xmin())/100.0;
  double sh = (dataRange.ymax() - dataRange.ymin())/100.0;

  auto xind1 = normalizeIndex(xind);

  BBox bbox(x - sw/2.0, y - sh/2.0, x + sw/2.0, y + sh/2.0);

  ColorInd iv;

  if (nr > 0)
    iv = ColorInd(r, nr);

  auto *pointObj = createPointObj(bbox, x, y, value, xind1, iv);

  objs.push_back(pointObj);
}

//------

bool
CQChartsDelaunayPlot::
addMenuItems(QMenu *menu)
{
  auto *voronoiAction = new QAction("Voronoi", menu);

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
execDrawBackground(PaintDevice *device) const
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
execDrawForeground(PaintDevice *) const
{
}

void
CQChartsDelaunayPlot::
drawDelaunay(PaintDevice *device) const
{
  if (! delaunayData_)
    return;

  if (isDelaunayLines()) {
    QPen pen;

    setDelaunayLineDataPen(pen, ColorInd());

    //---

    // draw delaunay triangles
    for (auto pf = delaunayData_->facesBegin(); pf != delaunayData_->facesEnd(); ++pf) {
      const auto *f = *pf;

      if (! f->isLower()) continue;

      auto *v1 = f->vertex(0);
      auto *v2 = f->vertex(1);
      auto *v3 = f->vertex(2);

      Point p1(v1->x(), v1->y());
      Point p2(v2->x(), v2->y());
      Point p3(v3->x(), v3->y());

      QPainterPath path;

      CQChartsDrawUtil::trianglePath(path, p1, p2, p3);

      device->strokePath(path, pen);
    }
  }
}

void
CQChartsDelaunayPlot::
drawVoronoi(PaintDevice *device) const
{
  if (! delaunayData_)
    return;

  //---

  // fill voronoi polygons
  if (isVoronoiPolygon()) {
    PenBrush penBrush;

    auto pc = interpVoronoiStrokeColor(ColorInd());
    auto fc = interpVoronoiFillColor  (ColorInd());

    setPenBrush(penBrush,
      PenData  (true, pc, voronoiStrokeAlpha(), voronoiStrokeWidth(), voronoiStrokeDash()),
      BrushData(true, fc, voronoiFillAlpha(), voronoiFillPattern()));

    for (auto pv = delaunayData_->verticesBegin(); pv != delaunayData_->verticesEnd(); ++pv) {
      const auto *v1 = *pv;

      CQChartsGrahamHull hull;

      for (const auto &v2 : v1->voronoi()) {
        Point p(v2->x(), v2->y());

        hull.addPoint(p);
      }

      Polygon poly;

      hull.getHull(poly);

      auto penBrush1 = penBrush;

      if (valueRange_.isSet()) {
        double v = CMathUtil::map(v1->value(), valueRange_.min(), valueRange_.max(), 0.0, 1.0);

        auto fc1 = interpVoronoiFillColor(ColorInd(v));

        penBrush1.brush.setColor(fc1);
      }

      CQChartsDrawUtil::setPenBrush(device, penBrush1);

      device->drawPolygon(poly);
    }
  }

  //---

  // draw voronoi points
  if (isVoronoiPoints()) {
    PenBrush penBrush;

    setVoronoiSymbolPenBrush(penBrush, ColorInd());

    // get symbol type and size
    auto symbolType = this->voronoiSymbolType();
    auto symbolSize = this->voronoiSymbolSize();

    for (auto pf = delaunayData_->facesBegin(); pf != delaunayData_->facesEnd(); ++pf) {
      const auto *f = *pf;

      auto *v = f->getVoronoi();
      if (! v) continue;

      Point p(v->x(), v->y());

      CQChartsDrawUtil::drawSymbol(device, penBrush, symbolType, p, symbolSize);
    }
  }

  //---

  // draw voronoi lines
  if (isVoronoiLines() || isVoronoiCircles()) {
    PenBrush penBrush;

    auto lc = interpVoronoiLinesColor(ColorInd());

    setPenBrush(penBrush,
      PenData(true, lc, voronoiLinesAlpha(), voronoiLinesWidth(), voronoiLinesDash()),
      BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    for (auto pve = delaunayData_->voronoiEdgesBegin();
           pve != delaunayData_->voronoiEdgesEnd(); ++pve) {
      const auto *e = *pve;

      auto *v1 = e->start();
      auto *v2 = e->end  ();

      if (isVoronoiLines()) {
        Point p1(v1->x(), v1->y());
        Point p2(v2->x(), v2->y());

        device->drawLine(p1, p2);
      }

      if (isVoronoiCircles()) {
        double r = v1->z();

        BBox bbox(v1->x() - r, v1->y() - r, v1->x() + r, v1->y() + r);

        device->drawEllipse(bbox);
      }
    }
  }
}

//---

CQChartsDelaunayPointObj *
CQChartsDelaunayPlot::
createPointObj(const BBox &rect, double x, double y, double value, const QModelIndex &ind,
               const ColorInd &iv) const
{
  return new CQChartsDelaunayPointObj(this, rect, x, y, value, ind, iv);
}

//---

CQChartsPlotCustomControls *
CQChartsDelaunayPlot::
createCustomControls(CQCharts *charts)
{
  auto *controls = new CQChartsDelaunayPlotCustomControls(charts);

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsDelaunayPointObj::
CQChartsDelaunayPointObj(const CQChartsDelaunayPlot *plot, const BBox &rect,
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
    ModelIndex nameInd(plot_, modelInd().row(), plot_->nameColumn(), modelInd().parent());

    bool ok;
    name1 = plot_->modelString(nameInd, ok);
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
    ModelIndex nameInd(plot_, modelInd().row(), plot_->nameColumn(), modelInd().parent());

    bool ok;
    auto name = plot_->modelString(nameInd, ok);

    if (ok && name.length())
      tableTip.addTableRow("Name", name);
  }

  //---

  // add x, y columns
  auto xstr = plot()->xStr(x());
  auto ystr = plot()->yStr(y());

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
isVisible() const
{
  if (! plot_->isPoints())
    return false;

  return CQChartsPlotObj::isVisible();
}

bool
CQChartsDelaunayPointObj::
inside(const Point &p) const
{
  if (! isVisible())
    return false;

  auto p1 = plot_->windowToPixel(Point(x_, y_));

  double sx, sy;

  plot_->pixelSymbolSize(plot_->symbolSize(), sx, sy);

  BBox pbbox(p1.x - sx, p1.y - sy, p1.x + sx, p1.y + sy);

  auto pp = plot_->windowToPixel(p);

  return pbbox.inside(pp);
}

void
CQChartsDelaunayPointObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->xColumn());
  addColumnSelectIndex(inds, plot_->yColumn());
}

void
CQChartsDelaunayPointObj::
draw(PaintDevice *device) const
{
  if (! isVisible())
    return;

  // calc pen and brush
  auto colorInd = calcColorInd();

  PenBrush penBrush;

  plot_->setSymbolPenBrush(penBrush, colorInd);

  plot_->updateObjPenBrushState(this, penBrush, CQChartsPlot::DrawType::SYMBOL);

  //---

  // get symbol type and size
  auto symbolType = plot_->symbolType();
  auto symbolSize = plot_->symbolSize();

  //---

  // draw symbol
  Point p(x_, y_);

  CQChartsDrawUtil::drawSymbol(device, penBrush, symbolType, p, symbolSize);
}

//------

CQChartsDelaunayPlotCustomControls::
CQChartsDelaunayPlotCustomControls(CQCharts *charts) :
 CQChartsPlotCustomControls(charts, "delaunay")
{
  // options group
  auto optionsFrame = createGroupFrame("Options");

  //---

  static auto columnNames = QStringList() << "x" << "y" << "name" << "value";

  addColumnWidgets(columnNames, optionsFrame);

  //---

  delaunayCheck_ = createBoolEdit("delaunay");
  voronoiCheck_  = createBoolEdit("voronoi" );

  addFrameWidget(optionsFrame, "Delaunay", delaunayCheck_);
  addFrameWidget(optionsFrame, "Voronoi" , voronoiCheck_ );

  //---

  connectSlots(true);
}

void
CQChartsDelaunayPlotCustomControls::
connectSlots(bool b)
{
  CQChartsWidgetUtil::connectDisconnect(b,
    delaunayCheck_, SIGNAL(stateChanged(int)), this, SLOT(delaunaySlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    voronoiCheck_, SIGNAL(stateChanged(int)), this, SLOT(voronoiSlot()));

  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsDelaunayPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  plot_ = dynamic_cast<CQChartsDelaunayPlot *>(plot);

  CQChartsPlotCustomControls::setPlot(plot);
}

void
CQChartsDelaunayPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  delaunayCheck_->setChecked(plot_->isDelaunay());
  voronoiCheck_ ->setChecked(plot_->isVoronoi ());

  CQChartsPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

void
CQChartsDelaunayPlotCustomControls::
delaunaySlot()
{
  plot_->setDelaunay(delaunayCheck_->isChecked());
}

void
CQChartsDelaunayPlotCustomControls::
voronoiSlot()
{
  plot_->setVoronoi(voronoiCheck_->isChecked());
}
