#include <CQChartsTernaryPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsTip.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QMenu>
#include <QAction>

CQChartsTernaryPlotType::
CQChartsTernaryPlotType()
{
}

void
CQChartsTernaryPlotType::
addParameters()
{
  startParameterGroup("Ternary");

  addColumnParameter("x", "X", "xColumn").
    setStringColumn().setBasic().setPropPath("columns.x").setTip("X column");
  addColumnParameter("y", "Y", "yColumn").
    setStringColumn().setBasic().setPropPath("columns.y").setTip("Y column");
  addColumnParameter("z", "Z", "zColumn").
    setStringColumn().setBasic().setPropPath("columns.z").setTip("Z column");

  addColumnParameter("name", "Name", "nameColumn").
    setStringColumn().setPropPath("columns.name").setTip("Group Name Column");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsTernaryPlotType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
    h2("Ternary Plot").
     h3("Summary").
      p("Draws three axis scatter plot.").
     h3("Columns").
     p("The " + B("X") + ", " + B("Y")  + ", " + " and " + B("Z") + "columns specify values.").
     p("The column headers specify the name of the individual values.").
     h3("Limitations").
      p("None.").
     h3("Example").
      p(IMG("images/ternary.png"));
}

void
CQChartsTernaryPlotType::
analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData)
{
  auto *details = modelData->details();
  if (! details) return;

  Column  nameColumn;
  Columns numericColumns;

  for (int c = 0; c < details->numColumns(); ++c) {
    const auto *columnDetails = details->columnDetails(Column(c));
    if (! columnDetails) continue;

    if (columnDetails->isNumeric()) {
      numericColumns.addColumn(columnDetails->column());
    }
  }

  if (numericColumns.count() >= 3) {
    analyzeModelData.parameterNameColumn["x"] = numericColumns.getColumn(0);
    analyzeModelData.parameterNameColumn["y"] = numericColumns.getColumn(1);
    analyzeModelData.parameterNameColumn["z"] = numericColumns.getColumn(2);
  }
}

CQChartsPlot *
CQChartsTernaryPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsTernaryPlot(view, model);
}

//------

CQChartsTernaryPlot::
CQChartsTernaryPlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("ternary"), model),
 CQChartsObjPointData<CQChartsTernaryPlot>(this)
{
}

CQChartsTernaryPlot::
~CQChartsTernaryPlot()
{
  CQChartsTernaryPlot::term();
}

//---

void
CQChartsTernaryPlot::
init()
{
  CQChartsPlot::init();

  //---

  NoUpdate noUpdate(this);

  setSymbol(Symbol::circle());

  setSymbolStrokeAlpha(Alpha(0.25));
  setSymbolFilled     (true);
  setSymbolFillColor  (Color::makePalette());
  setSymbolFillAlpha  (Alpha(0.5));
  setSymbolSize       (Length::pixel(8));

  //---

  height_ = std::sqrt(3.0)/2.0;

  xAxis_ = std::make_unique<CQChartsAxis>(this, Qt::Horizontal, 0.0, 1.0);
  yAxis_ = std::make_unique<CQChartsAxis>(this, Qt::Vertical  , 0.0, 1.0);
  zAxis_ = std::make_unique<CQChartsAxis>(this, Qt::Vertical  , 0.0, 1.0);

  auto xPath = CQChartsPath().moveTo(Point( 1.0, -height_)).lineTo(Point(-1.0, -height_));
  auto yPath = CQChartsPath().moveTo(Point(-1.0, -height_)).lineTo(Point( 0.0,  height_));
  auto zPath = CQChartsPath().moveTo(Point( 0.0,  height_)).lineTo(Point( 1.0, -height_));

  xAxis_->setPosition(-1.0);
  xAxis_->setRange   (-1.0, 1.0);
  xAxis_->setPath    (xPath);
  xAxis_->setNumMajor(10);

  yAxis_->setPosition(-1.0);
  yAxis_->setRange   (-1.0, 1.0);
  yAxis_->setPath    (yPath);
  yAxis_->setNumMajor(10);

  zAxis_->setPosition(-1.0);
  zAxis_->setRange   (-1.0, 1.0);
  zAxis_->setPath    (zPath);
  zAxis_->setNumMajor(10);

  //---

  addKey();

  addTitle();

  //---

  addColorMapKey();
}

void
CQChartsTernaryPlot::
term()
{
}

//------

void
CQChartsTernaryPlot::
setXColumn(const Column &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsTernaryPlot::
setYColumn(const Column &c)
{
  CQChartsUtil::testAndSet(yColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsTernaryPlot::
setZColumn(const Column &c)
{
  CQChartsUtil::testAndSet(zColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsTernaryPlot::
setNameColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged(); } );
}

//---

CQChartsColumn
CQChartsTernaryPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "x"   ) c = this->xColumn();
  else if (name == "y"   ) c = this->yColumn();
  else if (name == "z"   ) c = this->zColumn();
  else if (name == "name") c = this->nameColumn();
  else                     c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsTernaryPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "x"   ) this->setXColumn(c);
  else if (name == "y"   ) this->setYColumn(c);
  else if (name == "z"   ) this->setZColumn(c);
  else if (name == "name") this->setNameColumn(c);
  else                     CQChartsPlot::setNamedColumn(name, c);
}

//---

void
CQChartsTernaryPlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns", "xColumn"   , "x"   , "X column");
  addProp("columns", "yColumn"   , "y"   , "Y column");
  addProp("columns", "zColumn"   , "z"   , "Z column");
  addProp("columns", "nameColumn", "name", "Name column");

  //---

  // points
  addSymbolProperties("symbol", "", "");

  //---

  // axes
  auto addXAxisProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc, bool hidden=false) {
    auto *item = propertyModel()->addProperty(path, xAxis_.get(), name, alias);
    item->setDesc(desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  auto addXAxisStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                              const QString &desc, bool hidden=false) {
    auto *item = addXAxisProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

//addXAxisProp("xaxis", "side", "", "X Axis plot side");

  auto xlabelPath     = QString("xaxis/label");
  auto xlabelTextPath = xlabelPath + "/text";

  addXAxisStyleProp(xlabelTextPath, "axesLabelTextData"   , "style",
                    "Axis label text style", true);
  addXAxisProp     (xlabelTextPath, "axesLabelTextVisible", "visible",
                    "X Axis label text visible");

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
}

CQChartsGeom::Range
CQChartsTernaryPlot::
calcRange() const
{
  Range dataRange;

  dataRange.updateRange(-1, -1);
  dataRange.updateRange( 1,  1);

  return dataRange;
}

//------

bool
CQChartsTernaryPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsTernaryPlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsTernaryPlot *>(this);

  th->clearErrors();

  //---

  // check columns
  bool columnsValid = true;

  // value column required
  // name column optional

  if (! checkNumericColumn(xColumn(), "X", /*required*/true) ||
      ! checkNumericColumn(yColumn(), "Y", /*required*/true) ||
      ! checkNumericColumn(zColumn(), "Z", /*required*/true))
    columnsValid = false;

  if (! checkColumn(nameColumn(), "Name"))
    columnsValid = false;

  if (! columnsValid)
    return false;

  //---

  bool ok;
  th->xLabel_ = modelHHeaderString(xColumn(), ok);
  th->yLabel_ = modelHHeaderString(yColumn(), ok);
  th->zLabel_ = modelHHeaderString(zColumn(), ok);

  //---

  struct TPoint {
    double      x { 0.0 };
    double      y { 0.0 };
    double      z { 0.0 };
    QString     name;
    int         nameInd { -1 };
    Color       color;
    QModelIndex ind;
  };

  // process model data
  class TernaryPlotVisitor : public ModelVisitor {
   public:
    using Points = std::vector<TPoint>;

   public:
    TernaryPlotVisitor(CQChartsTernaryPlot *ternaryPlot) :
     ternaryPlot_(ternaryPlot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      ModelIndex xInd(ternaryPlot_, data.row, ternaryPlot_->xColumn(), data.parent);
      ModelIndex yInd(ternaryPlot_, data.row, ternaryPlot_->yColumn(), data.parent);
      ModelIndex zInd(ternaryPlot_, data.row, ternaryPlot_->zColumn(), data.parent);

      bool ok1, ok2, ok3;

      TPoint p;

      p.x = ternaryPlot_->modelNumericValue(xInd, ok1);
      p.y = ternaryPlot_->modelNumericValue(yInd, ok2);
      p.z = ternaryPlot_->modelNumericValue(zInd, ok3);

      if (! ok1) { addDataError(xInd, "Bad X Value"); p.x = 0.0; }
      if (! ok2) { addDataError(yInd, "Bad Y Value"); p.y = 0.0; }
      if (! ok3) { addDataError(zInd, "Bad Z Value"); p.z = 0.0; }

      if (CMathUtil::isNaN(p.x) || CMathUtil::isNaN(p.y) || CMathUtil::isNaN(p.z))
        return State::SKIP;

      xrange_.add(p.x);
      yrange_.add(p.y);
      zrange_.add(p.z);

      //---

      if (ternaryPlot_->nameColumn().isValid()) {
        ModelIndex nameInd(ternaryPlot_, data.row, ternaryPlot_->nameColumn(), data.parent);

        bool ok;
        p.name = ternaryPlot_->modelString(nameInd, ok);

        CQChartsModelColumnDetails *nameDetails { nullptr };

        if (! nameDetails_) {
          nameDetails = ternaryPlot_->columnDetails(ternaryPlot_->nameColumn());

          nameDetails_ = nameDetails;
          nameCount_   = (nameDetails ? nameDetails->numUnique() : 0);
        }
        else
          nameDetails = nameDetails_.value();

        p.nameInd = (nameDetails ? nameDetails->uniqueId(p.name) : -1);
      }

      //---

      if (ternaryPlot_->colorColumn().isValid()) {
        ModelIndex colorInd(ternaryPlot_, data.row, ternaryPlot_->colorColumn(), data.parent);

        bool ok;
        auto var = ternaryPlot_->modelValue(colorInd, ok);

        auto c = CQChartsVariant::toColor(var, ok);

        if (ok)
          p.color = c;
      }

      //---

      p.ind = ternaryPlot_->normalizeIndex(ternaryPlot_->modelIndex(xInd));

      points_.push_back(p);

      return State::OK;
    }

    const RMinMax &xrange() const { return xrange_; }
    const RMinMax &yrange() const { return yrange_; }
    const RMinMax &zrange() const { return zrange_; }

    const Points &points() const { return points_; }

    int nameCount() const { return nameCount_; }

   private:
    void addDataError(const ModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsTernaryPlot *>(ternaryPlot_)->addDataError(ind, msg);
    }

   private:
    CQChartsTernaryPlot* ternaryPlot_ { nullptr };
    Points               points_;
    RMinMax              xrange_;
    RMinMax              yrange_;
    RMinMax              zrange_;

    std::optional<CQChartsModelColumnDetails*> nameDetails_;
    int                                        nameCount_ { 0 };
  };

  TernaryPlotVisitor ternaryPlotVisitor(th);

  visitModel(ternaryPlotVisitor);

#if 0
  auto adjustRange = [&](const RMinMax &minMax) {
    auto minMax1 = minMax;

    for (int i = 0; i < 10; ++i) {
      CInterval interval(minMax1.min(), minMax1.max()); interval.setNumMajor(10);

      auto mid   = (interval.calcStart() + interval.calcEnd())/2.0;
      auto start = mid - 5.5*interval.calcIncrement();
      auto end   = mid + 5.5*interval.calcIncrement();

      minMax1 = RMinMax(start, end);

      if (interval.calcNumMajor() == 10)
        break;
    }

    return minMax1;
  };

  th->xrange_ = adjustRange(ternaryPlotVisitor.xrange());
  th->yrange_ = adjustRange(ternaryPlotVisitor.yrange());
  th->zrange_ = adjustRange(ternaryPlotVisitor.zrange());
#else
  th->xrange_ = ternaryPlotVisitor.xrange();
  th->yrange_ = ternaryPlotVisitor.yrange();
  th->zrange_ = ternaryPlotVisitor.zrange();
#endif

  xAxis_->setRange(0, 100.0);
  yAxis_->setRange(0, 100.0);
  zAxis_->setRange(0, 100.0);

//xAxis_->setSide(CQChartsAxis::AxisSide(CQChartsAxis::AxisSide::Type::BOTTOM_LEFT));
//yAxis_->setSide(CQChartsAxis::AxisSide(CQChartsAxis::AxisSide::Type::BOTTOM_LEFT));
//zAxis_->setSide(CQChartsAxis::AxisSide(CQChartsAxis::AxisSide::Type::TOP_RIGHT));

  //---

  auto ss = symbolSize();
  auto sx = lengthPlotWidth (ss);
  auto sy = lengthPlotHeight(ss);

  const auto &points = ternaryPlotVisitor.points();

  int  ip = 0;
  auto np = points.size();
  int  nn = ternaryPlotVisitor.nameCount();

  for (const auto &p : points) {
    auto pos = mapPoint(p.x, p.y, p.z);

    BBox bbox(pos.x - sx/2.0, pos.y - sy/2.0, pos.x + sx/2.0, pos.y + sy/2.0);

    ColorInd iv(ip++, np);
    ColorInd ig((nn > 0 ? p.nameInd : 0), (nn > 0 ? nn : 1));

    auto *pointObj = th->createPointObj(bbox, p.x, p.y, p.z, p.ind, ig, iv);

    if (p.name != "")
      pointObj->setName(p.name);

    if (p.color.isValid())
      pointObj->setColor(p.color);

    objs.push_back(pointObj);
  }

  //---

  return true;
}

CQChartsGeom::Point
CQChartsTernaryPlot::
mapPoint(double x, double y, double z) const
{
  const auto &xrange = this->xrange();
  const auto &yrange = this->yrange();
  const auto &zrange = this->zrange();

#if 0
  auto x1 = CMathUtil::map(x, xrange.min(), xrange.max(), -1, 1);
  auto y1 = CMathUtil::map(y, yrange.min(), yrange.max(), -1, 1);
  auto z1 = CMathUtil::map(z, zrange.min(), zrange.max(), -1, 1);

  auto xy1 = CMathUtil::map(y1, -1, 1, -1, 0);
  auto yy1 = CMathUtil::map(y1, -1, 1, -1, 1);

  auto xz1 = CMathUtil::map(z1, -1, 1, 0,  1);
  auto yz1 = CMathUtil::map(z1, -1, 1, 1, -1);

  Point point1( x1,  -1);
  Point point2(xy1, yy1);
  Point point3(xz1, yz1);

  return (point1 + point2 + point3)/3.0;
#else
  auto valuePoint = [&](double value, const RMinMax &range,
                        const AxisP &axis, const Angle &angle) {
    auto v = angle.vector();

    return axis->valueToPoint(50) +
      v*CMathUtil::map(value, range.min(), range.max(), 0.0, 2.0*height_);
  };

  auto p0 = valuePoint(x, xrange, xAxis_, Angle( 90.0));
  auto p1 = valuePoint(y, yrange, zAxis_, Angle(210.0));
  auto p2 = valuePoint(z, zrange, yAxis_, Angle(-30.0));

  auto p = (p0 + p1 + p2)/3.0;

  return p;
#endif
}

//---

bool
CQChartsTernaryPlot::
hasFgAxes() const
{
  return true;
}

void
CQChartsTernaryPlot::
drawFgAxes(PaintDevice *device) const
{
  xAxis_->draw(this, device);
  yAxis_->draw(this, device);
  zAxis_->draw(this, device);

//xAxis_->setDefLabel(xLabel());
//yAxis_->setDefLabel(yLabel());
//zAxis_->setDefLabel(zLabel());

  //---

  // draw grid
  auto xTicks = xAxis_->majorTickPoints();
  auto yTicks = yAxis_->majorTickPoints();
  auto zTicks = zAxis_->majorTickPoints();

  int n1 = std::min(xTicks.size(), yTicks.size());
  int n2 = std::min(xTicks.size(), zTicks.size());
  int n3 = std::min(yTicks.size(), zTicks.size());

  PenBrush penBrush;

  xAxis_->setAxesMinorGridLineDataPen(penBrush.pen, ColorInd());

  device->setPen(penBrush.pen);

  for (int i = 1; i < n1; ++i)
    device->drawLine(xTicks[i], yTicks[n1 - i]);

  for (int i = 1; i < n2; ++i)
    device->drawLine(xTicks[i], zTicks[n2 - i]);

  for (int i = 1; i < n3; ++i)
    device->drawLine(yTicks[i], zTicks[n3 - i]);

  //---

  auto labelPoint = [&](const AxisP &axis, const Angle &angle) {
    auto v = angle.vector();

    return axis->valueToPoint(50) + v*2.0*height_;
  };

  auto p0 = labelPoint(xAxis_, Angle( 90.0));
  auto p1 = labelPoint(zAxis_, Angle(210.0));
  auto p2 = labelPoint(yAxis_, Angle(-30.0));

  xAxis_->setAxesLineDataPen(penBrush.pen, ColorInd());

  device->setPen(penBrush.pen);

  auto textOptions = xAxis_->axesLabelTextOptions();

  CQChartsDrawUtil::drawTextAtPoint(device, p0, xLabel(), textOptions);
  CQChartsDrawUtil::drawTextAtPoint(device, p1, yLabel(), textOptions);
  CQChartsDrawUtil::drawTextAtPoint(device, p2, zLabel(), textOptions);
}

//---

void
CQChartsTernaryPlot::
addKeyItems(PlotKey *)
{
}

//---

bool
CQChartsTernaryPlot::
addMenuItems(QMenu *menu, const Point &)
{
  bool added = false;

  if (canDrawColorMapKey()) {
    addColorMapKeyItems(menu);

    added = true;
  }

  return added;
}

//---

CQChartsTernaryPointObj *
CQChartsTernaryPlot::
createPointObj(const BBox &rect, double x, double y, double z, const QModelIndex &ind,
               const ColorInd &ig, const ColorInd &iv)
{
  return new CQChartsTernaryPointObj(this, rect, x, y, z, ind, ig, iv);
}

//---

CQChartsPlotCustomControls *
CQChartsTernaryPlot::
createCustomControls()
{
  auto *controls = new CQChartsTernaryPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsTernaryPointObj::
CQChartsTernaryPointObj(const CQChartsTernaryPlot *ternaryPlot, const BBox &rect, double x,
                        double y, double z, const QModelIndex &ind, const ColorInd &ig,
                        const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsTernaryPlot *>(ternaryPlot), rect, ColorInd(), ig, iv),
 ternaryPlot_(ternaryPlot), x_(x), y_(y), z_(z)
{
  setDetailHint(DetailHint::MAJOR);

  if (ind.isValid())
    setModelInd(ind);
}

QString
CQChartsTernaryPointObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(iv_.i);
}

QString
CQChartsTernaryPointObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  if (name_ != "")
    tableTip.addTableRow("Name", name_);

  tableTip.addTableRow("X", x_);
  tableTip.addTableRow("Y", y_);
  tableTip.addTableRow("Z", z_);

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

void
CQChartsTernaryPointObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  auto path1 = path + "/" + propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);

  model->addProperty(path1, this, "rect"    )->setDesc("Bounding box");
}

//---

bool
CQChartsTernaryPointObj::
inside(const Point &p) const
{
  if (! isVisible())
    return false;

  return rect().inside(p);
}

void
CQChartsTernaryPointObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, ternaryPlot_->xColumn());
  addColumnSelectIndex(inds, ternaryPlot_->yColumn());
  addColumnSelectIndex(inds, ternaryPlot_->zColumn());

  addColumnSelectIndex(inds, CQChartsColumn(modelInd().column()));
}

void
CQChartsTernaryPointObj::
draw(PaintDevice *device) const
{
  // calc stroke and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  auto p = mapPoint();

  auto ss     = ternaryPlot_->symbolSize();
  auto symbol = ternaryPlot_->symbol();

  CQChartsDrawUtil::drawSymbol(device, penBrush, symbol, p, ss, /*scale*/false);

  device->resetColorNames();
}

void
CQChartsTernaryPointObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  QColor fillColor;

  if (ternaryPlot_->colorColumn().isValid() &&
      ternaryPlot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    auto ind1 = modelInd();

    Color indColor;

    if (ternaryPlot_->colorColumnColor(ind1.row(), ind1.parent(), indColor))
      fillColor = ternaryPlot_->interpColor(indColor, colorInd);
    else
      fillColor = ternaryPlot_->interpPaletteColor(colorInd);
  }
  else
    fillColor = ternaryPlot_->interpPaletteColor(colorInd);

  if (color_.isValid())
    fillColor = ternaryPlot_->interpColor(color_, colorInd);

  auto strokeColor = ternaryPlot_->interpPaletteColor(colorInd);

  ternaryPlot_->setPenBrush(penBrush, PenData(true, strokeColor),
                            BrushData(true, fillColor));

  if (updateState)
    ternaryPlot_->updateObjPenBrushState(this, penBrush);
}

CQChartsGeom::Point
CQChartsTernaryPointObj::
mapPoint() const
{
  return ternaryPlot_->mapPoint(x_, y_, z_);
}

//------

CQChartsTernaryPlotCustomControls::
CQChartsTernaryPlotCustomControls(CQCharts *charts) :
 CQChartsPlotCustomControls(charts, "ternary")
{
}

void
CQChartsTernaryPlotCustomControls::
init()
{
  addWidgets();

  addOverview();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsTernaryPlotCustomControls::
addWidgets()
{
  addColumnWidgets();

  addColorColumnWidgets("Fill Color");

  addKeyList();
}

void
CQChartsTernaryPlotCustomControls::
addColumnWidgets()
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  //---

  // name and values columns
  addNamedColumnWidgets(QStringList() << "x" << "y" << "z" << "name", columnsFrame);
}

void
CQChartsTernaryPlotCustomControls::
connectSlots(bool b)
{
  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsTernaryPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_ && ternaryPlot_)
    disconnect(ternaryPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  ternaryPlot_ = dynamic_cast<CQChartsTernaryPlot *>(plot);

  CQChartsPlotCustomControls::setPlot(plot);

  if (ternaryPlot_)
    connect(ternaryPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
}

void
CQChartsTernaryPlotCustomControls::
updateWidgets()
{
  CQChartsPlotCustomControls::updateWidgets();
}
