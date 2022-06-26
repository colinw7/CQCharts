#include <CQChartsRadarPlot.h>
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

CQChartsRadarPlotType::
CQChartsRadarPlotType()
{
}

void
CQChartsRadarPlotType::
addParameters()
{
  startParameterGroup("Radar");

  addColumnParameter("name", "Name", "nameColumn").
    setStringColumn().setBasic().setPropPath("columns.name").setTip("Name column");
  addColumnsParameter("values", "Value", "valueColumns").
   setNumericColumn().setRequired().setPropPath("columns.values").setTip("Value columns");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsRadarPlotType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Radar Plot").
    h3("Summary").
     p("Draws polygon for each row with a point for each value column.").
    h3("Columns").
    p("The " + B("Name") + " column specifies the name for the value set.").
    p("The column headers specify the name of the indiviidual values.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/radar.png"));
}

void
CQChartsRadarPlotType::
analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData)
{
  auto *details = modelData->details();
  if (! details) return;

  Column  nameColumn;
  Columns numericColumns;

  for (int c = 0; c < details->numColumns(); ++c) {
    const auto *columnDetails = details->columnDetails(Column(c));
    if (! columnDetails) continue;

    if      (columnDetails->isNumeric()) {
      numericColumns.addColumn(columnDetails->column());
    }
    else if (columnDetails->type() == ColumnType::STRING) {
      if (! nameColumn.isValid())
        nameColumn = columnDetails->column();
    }
  }

  if (nameColumn.isValid())
    analyzeModelData.parameterNameColumn["name"] = nameColumn;

  if (numericColumns.count())
    analyzeModelData.parameterNameColumns["value"] = numericColumns;
}

CQChartsPlot *
CQChartsRadarPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsRadarPlot(view, model);
}

//------

CQChartsRadarPlot::
CQChartsRadarPlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("radar"), model),
 CQChartsObjShapeData   <CQChartsRadarPlot>(this),
 CQChartsObjTextData    <CQChartsRadarPlot>(this),
 CQChartsObjGridLineData<CQChartsRadarPlot>(this)
{
}

CQChartsRadarPlot::
~CQChartsRadarPlot()
{
  term();
}

//---

void
CQChartsRadarPlot::
init()
{
  CQChartsPlot::init();

  //---

  NoUpdate noUpdate(this);

  setGridLinesColor(Color::makeInterfaceValue(0.5));

  setFillColor(Color::makePalette());
  setFillAlpha(Alpha(0.5));

  setFilled (true);
  setStroked(true);

  setTextColor(Color::makeInterfaceValue(1.0));

  //---

  addKey();

  addTitle();

  //---

  addColorMapKey();
}

void
CQChartsRadarPlot::
term()
{
}

//------

void
CQChartsRadarPlot::
setNameColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsRadarPlot::
setValueColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(valueColumns_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

//---

CQChartsColumn
CQChartsRadarPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if (name == "name") c = this->nameColumn();
  else                c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsRadarPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if (name == "name") this->setNameColumn(c);
  else                CQChartsPlot::setNamedColumn(name, c);
}

CQChartsColumns
CQChartsRadarPlot::
getNamedColumns(const QString &name) const
{
  Columns c;
  if (name == "values") c = this->valueColumns();
  else             c = CQChartsPlot::getNamedColumns(name);

  return c;
}

void
CQChartsRadarPlot::
setNamedColumns(const QString &name, const Columns &c)
{
  if (name == "values") this->setValueColumns(c);
  else                  CQChartsPlot::setNamedColumns(name, c);
}

//---

void
CQChartsRadarPlot::
setAngleStart(const Angle &a)
{
  CQChartsUtil::testAndSet(angleStart_, a, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsRadarPlot::
setAngleExtent(const Angle &a)
{
  CQChartsUtil::testAndSet(angleExtent_, a, [&]() { updateRangeAndObjs(); } );
}

//----

void
CQChartsRadarPlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns", "nameColumn"  , "name"  , "Name column");
  addProp("columns", "valueColumns", "values", "Value columns");

  // options
  addProp("options", "angleStart" , "", "Angle start");
  addProp("options", "angleExtent", "", "Angle extent");

  // grid
  addProp("grid", "gridLines", "visible", "Grid lines visible");

  addLineProperties("grid/stroke", "gridLines", "Grid");

  // fill
  addProp("fill", "filled", "visible", "Fill visible");

  addFillProperties("fill", "fill", "");

  // stroke
  addProp("stroke", "stroked", "visible", "Stroke visible");

  addLineProperties("stroke", "stroke", "");

  // text
//addProp("text", "textVisible", "visible", "Text visible");

  addTextProperties("text", "text", "", CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
}

CQChartsGeom::Range
CQChartsRadarPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsRadarPlot::calcRange");

  auto *th = const_cast<CQChartsRadarPlot *>(this);

  // get values for each row
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsRadarPlot *plot) :
     plot_(plot) {
      nv_ = plot_->valueColumns().count();
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      for (int iv = 0; iv < nv_; ++iv) {
        const auto &column = plot_->valueColumns().getColumn(iv);

        ModelIndex ind(plot_, data.row, column, data.parent);

        double value;

        if (! plot_->columnValue(ind, value))
          continue;

        valueDatas_[iv].add(value);
      }

      return State::OK;
    }

    const ValueDatas &valueDatas() const { return valueDatas_; }

   private:
    const CQChartsRadarPlot *plot_ { nullptr };
    int                      nv_   { 0 };
    ValueDatas               valueDatas_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  th->valueDatas_ = visitor.valueDatas();

  //---

  // calc max radius (normalized values)
  th->valueRadius_ = 0.0;

  int nv = valueColumns().count();

  for (int iv = 0; iv < nv; ++iv) {
    auto pd = valueDatas_.find(iv);
    if (pd == valueDatas_.end()) continue;

    const auto &valueData = (*pd).second;

    th->valueRadius_ = std::max(valueRadius_, valueData.max()/valueData.sum());
  }

  //---

  // set range
  double r = valueRadius_;

  Range dataRange;

  if (r > 0.0) {
    dataRange.updateRange(-r, -r);
    dataRange.updateRange( r,  r);
  }
  else {
    dataRange.updateRange(-1, -1);
    dataRange.updateRange( 1,  1);
  }

  return dataRange;
}

//------

CQChartsGeom::BBox
CQChartsRadarPlot::
calcExtraFitBBox() const
{
  CQPerfTrace trace("CQChartsRadarPlot::calcExtraFitBBox");

  BBox bbox;

  int nv = valueColumns().count();

  // add corner labels
  if (nv > 2) {
    auto *th = const_cast<CQChartsRadarPlot *>(this);

    CQChartsPlotPaintDevice device(th, nullptr);

    auto font = view()->plotFont(this, textFont());

    //---

    double alen = CQChartsUtil::clampDegrees(angleExtent().value());

    double da = alen/nv;
    double r  = valueRadius_;

    //---

    double a = angleStart().value();

    for (int iv = 0; iv < nv; ++iv) {
      double ra = CMathUtil::Deg2Rad(a);

      double x = r*std::cos(ra);
      double y = r*std::sin(ra);

      //---

      const auto &valueColumn = valueColumns().getColumn(iv);

      bool ok;

      auto name = modelHHeaderString(valueColumn, ok);

      if (name.length()) {
        auto align = alignForPosition(x, y);

        BBox tbbox =
          CQChartsDrawUtil::calcAlignedTextRect(&device, font, Point(x, y), name, align, 2, 2);

        bbox += tbbox;
      }

      //---

      a -= da;
    }
  }

  //---

  // add objects
  for (const auto &plotObj : plotObjs_) {
    auto *obj = dynamic_cast<CQChartsRadarObj *>(plotObj);

    if (obj)
      bbox += obj->extraFitBBox();
  }

  return bbox;
}

//------

bool
CQChartsRadarPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsRadarPlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsRadarPlot *>(this);

  th->clearErrors();

  //---

  // check columns
  bool columnsValid = true;

  // value column required
  // name column optional

  if (! checkNumericColumns(valueColumns(), "Values", /*required*/true))
    columnsValid = false;

  if (! checkColumn(nameColumn(), "Name"))
    columnsValid = false;

  if (! columnsValid)
    return false;

  //---

  // process model data
  class RadarPlotVisitor : public ModelVisitor {
   public:
    RadarPlotVisitor(const CQChartsRadarPlot *plot, PlotObjs &objs) :
     plot_(plot), objs_(objs) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      if (! plot_->addRow(data, numRows(), objs_))
        return State::SKIP;

      return State::OK;
    }

   private:
    const CQChartsRadarPlot* plot_ { nullptr };
    PlotObjs&                objs_;
  };

  RadarPlotVisitor radarPlotVisitor(this, objs);

  visitModel(radarPlotVisitor);

  //---

  return true;
}

bool
CQChartsRadarPlot::
addRow(const ModelVisitor::VisitData &data, int nr, PlotObjs &objs) const
{
  bool hidden = isSetHidden(data.row);

  if (hidden)
    return false;

  auto *th = const_cast<CQChartsRadarPlot *>(this);

  //---

  // get row name
  ModelIndex nameInd;

  QString name;

  if (nameColumn().isValid()) {
    nameInd = ModelIndex(th, data.row, nameColumn(), data.parent);

    bool ok;

    name = modelString(nameInd, ok);

    if (! ok)
      return th->addDataError(nameInd, "Invalid name");
  }

  //---

  // calc polygon angle
  int nv = valueColumns().count();

  double alen = CQChartsUtil::clampDegrees(angleExtent().value());

  double da = (nv > 2 ? alen/nv : 90.0);

  //---

  // calc polygon points
  Polygon                      poly;
  CQChartsRadarObj::NameValues nameValues;

  double a = (nv > 2 ? angleStart().value() : 0.0);

  for (int iv = 0; iv < nv; ++iv) {
    const auto &valueColumn = valueColumns().getColumn(iv);

    //---

    // get column value
    ModelIndex valueInd(th, data.row, valueColumn, data.parent);

    double value;

    if (! columnValue(valueInd, value))
      return th->addDataError(valueInd, "Invalid value");

    //---

    // get column name (unique ?)
    bool ok;

    auto name = modelHHeaderString(valueColumn, ok);

    //---

    auto pd = valueDatas_.find(iv);
    assert(pd != valueDatas_.end());

    const auto &valueData = (*pd).second;

    //---

    // set point
    double scale = valueData.sum();

    double ra = CMathUtil::Deg2Rad(a);

    double x = value*std::cos(ra)/scale;
    double y = value*std::sin(ra)/scale;

    poly.addPoint(Point(x, y));

    //---

    nameValues[name] = value;

    //---

    a -= da;
  }

  //---

  // create object
  QModelIndex nameInd1;

  if (nameInd.isValid())
    nameInd1 = normalizeIndex(modelIndex(nameInd));

  BBox bbox(-1, -1, 1, 1); // poly bounding rect ?

  ColorInd is(data.row, nr);

  auto *radarObj = th->createObj(bbox, name, poly, nameValues, nameInd1, is);

  objs.push_back(radarObj);

  return true;
}

bool
CQChartsRadarPlot::
columnValue(const ModelIndex &ind, double &value) const
{
  auto columnType = columnValueType(ind.column());

  value = 1.0;

  if (columnType == ColumnType::INTEGER || columnType == ColumnType::REAL) {
    bool ok;

    value = modelReal(ind, ok);

    if (! ok || CMathUtil::isNaN(value))
      return false;

    if (value <= 0.0)
      return false;
  }
  else {
    bool ok;

    value = modelReal(ind, ok);

    if (! ok)
      value = 1.0; // string non-real -> 1.0

    if (value <= 0.0)
      value = 1.0;
  }

  return true;
}

void
CQChartsRadarPlot::
addKeyItems(PlotKey *key)
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsRadarPlot *plot, CQChartsPlotKey *key) :
     plot_(plot), key_(key) {
      row_ = (! key_->isHorizontal() ? key_->maxRow() : 0);
      col_ = (! key_->isHorizontal() ? 0 : key_->maxCol());
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      QString name;

      if (plot_->nameColumn().isValid()) {
        ModelIndex nameInd(plot_, data.row, plot_->nameColumn(), data.parent);

        bool ok;

        name = plot_->modelString(nameInd, ok);
      }

      //---

      ColorInd ic(data.row, numRows());

      addKeyItem(name, ic);

      return State::OK;
    }

    void addKeyItem(const QString &name, const ColorInd &ic) {
      auto *plot = const_cast<CQChartsRadarPlot *>(plot_);

      auto *colorItem = new CQChartsColorBoxKeyItem(plot, ColorInd(), ColorInd(), ic);
      auto *textItem  = new CQChartsTextKeyItem    (plot, name, ic);

      auto *groupItem = new CQChartsGroupKeyItem(plot);

      groupItem->addRowItems(colorItem, textItem);

      colorItem->setClickable(true);

      key_->addItem(groupItem, row_, col_);

      key_->nextRowCol(row_, col_);
    }

   private:
    const CQChartsRadarPlot* plot_ { nullptr };
    CQChartsPlotKey*         key_  { nullptr };
    int                      row_  { 0 };
    int                      col_  { 0 };
  };

  RowVisitor visitor(this, key);

  visitModel(visitor);

  //---

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsRadarPlot::
postResize()
{
  CQChartsPlot::postResize();

  resetDataRange(/*updateRange*/true, /*updateObjs*/false);
}

bool
CQChartsRadarPlot::
hasBackground() const
{
  return true;
}

void
CQChartsRadarPlot::
execDrawBackground(PaintDevice *device) const
{
  int nv = valueColumns().count();

  if (! nv)
    return;

  //---

  if      (nv == 1) {
    // TODO
  }
  else if (nv == 2) {
    // TODO
  }
  else if (nv > 2) {
    double alen = CQChartsUtil::clampDegrees(angleExtent().value());

    double da = alen/nv;

    //---

    // draw grid spokes
    if (isGridLines()) {
      QPen gpen1;

      setGridLineDataPen(gpen1, ColorInd(0, 1));

      device->setPen(gpen1);

      //---

      auto p1 = windowToPixel(Point(0.0, 0.0));

      double a = angleStart().value();

      for (int iv = 0; iv < nv; ++iv) {
        double ra = CMathUtil::Deg2Rad(a);

        double x = valueRadius_*std::cos(ra);
        double y = valueRadius_*std::sin(ra);

        auto p2 = windowToPixel(Point(x, y));

        device->drawLine(pixelToWindow(p1), pixelToWindow(p2));

        a -= da;
      }
    }

    //---

    QPen gpen2;

    setGridLineDataPen(gpen2, ColorInd(0, 1));

    //---

    PenBrush tpenBrush;

    auto tc = interpTextColor(ColorInd());

    setPen(tpenBrush, PenData(true, tc, textAlpha()));

    //---

    setPainterFont(device, textFont());

    int    nl = 5;
    double dr = valueRadius_/nl;

    for (int i = 0; i <= nl; ++i) {
      double r = dr*i;

      double a = angleStart().value();

      Polygon poly;

      for (int iv = 0; iv < nv; ++iv) {
        double ra = CMathUtil::Deg2Rad(a);

        double x = r*std::cos(ra);
        double y = r*std::sin(ra);

        Point p1(x, y);

        poly.addPoint(p1);

        //---

        if (i == nl) {
          device->setPen(tpenBrush.pen);

          //---

          const auto &valueColumn = valueColumns().getColumn(iv);

          bool ok;

          auto name = modelHHeaderString(valueColumn, ok);

          if (name.length()) {
            auto align = alignForPosition(x, y);

            // only contrast support (custom align, zero angle)
            auto textOptions = this->textOptions();

            textOptions.angle = Angle();
            textOptions.align = align;

            CQChartsDrawUtil::drawTextAtPoint(device, p1, name, textOptions,
                                              /*centered*/false, 2, 2);
          }
        }

        //---

        a -= da;
      }

      poly.addPoint(poly.point(0));

      //---

      // draw grid polygon
      if (isGridLines()) {
        device->setPen(gpen2);

        device->drawPolygon(poly);
      }
    }
  }
}

Qt::Alignment
CQChartsRadarPlot::
alignForPosition(double x, double y) const
{
  Qt::Alignment align = 0;

  if      (CMathUtil::isZero(x)) align |= Qt::AlignHCenter;
  else if (x > 0)                align |= Qt::AlignLeft;
  else if (x < 0)                align |= Qt::AlignRight;

  if      (CMathUtil::isZero(y)) align |= Qt::AlignVCenter;
  else if (y > 0)                align |= Qt::AlignBottom;
  else if (y < 0)                align |= Qt::AlignTop;

  return align;
}

//---

bool
CQChartsRadarPlot::
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

CQChartsRadarObj *
CQChartsRadarPlot::
createObj(const BBox &rect, const QString &name, const Polygon &poly,
          const CQChartsRadarObj::NameValues &nameValues, const QModelIndex &ind,
          const ColorInd &iv)
{
  return new CQChartsRadarObj(this, rect, name, poly, nameValues, ind, iv);
}

//---

bool
CQChartsRadarPlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsRadarPlot::
execDrawForeground(PaintDevice *device) const
{
  if (isColorMapKey())
    drawColorMapKey(device);
}

//---

CQChartsPlotCustomControls *
CQChartsRadarPlot::
createCustomControls()
{
  auto *controls = new CQChartsRadarPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsRadarObj::
CQChartsRadarObj(const CQChartsRadarPlot *plot, const BBox &rect, const QString &name,
                 const Polygon &poly, const NameValues &nameValues, const QModelIndex &ind,
                 const ColorInd &is) :
 CQChartsPlotObj(const_cast<CQChartsRadarPlot *>(plot), rect, is, ColorInd(), ColorInd()),
 plot_(plot), name_(name), poly_(poly), nameValues_(nameValues)
{
  setDetailHint(DetailHint::MAJOR);

  if (ind.isValid())
    setModelInd(ind);
}

QString
CQChartsRadarObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(is_.i);
}

QString
CQChartsRadarObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name", name_);

  for (const auto &nameValue : nameValues_)
    tableTip.addTableRow(nameValue.first, nameValue.second);

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

void
CQChartsRadarObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  auto path1 = path + "/" + propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);

  model->addProperty(path1, this, "rect"    )->setDesc("Bounding box");
//model->addProperty(path1, this, "selected")->setDesc("Is selected");

  model->addProperty(path1, this, "name")->setDesc("Name");
}

//---

bool
CQChartsRadarObj::
inside(const Point &p) const
{
  if (! isVisible())
    return false;

  // point
  if      (poly_.size() == 1) {
    auto p1 = poly_.point(0); // circle radius p1.x

    double r  = std::hypot(p .x, p .y);
    double r1 = std::hypot(p1.x, p1.x);

    return (r < r1);
  }
  // line
  else if (poly_.size() == 2) {
    auto p1 = poly_.point(0); // circle radius p1.x and p2.y
    auto p2 = poly_.point(1);

    double r  = std::hypot(p .x, p .y);
    double r1 = std::hypot(p1.x, p2.y);

    return (r < r1);
  }
  // polygon
  else if (poly_.size() >= 3) {
    return poly_.containsPoint(p, Qt::OddEvenFill);
  }
  else
    return false;
}

CQChartsGeom::BBox
CQChartsRadarObj::
extraFitBBox() const
{
  BBox bbox;

  if (! isVisible())
    return bbox;

  return bbox;
}

void
CQChartsRadarObj::
getObjSelectIndices(Indices &inds) const
{
  for (const auto &valueColumn : plot_->valueColumns())
    addColumnSelectIndex(inds, valueColumn);

  addColumnSelectIndex(inds, CQChartsColumn(modelInd().column()));
}

void
CQChartsRadarObj::
draw(PaintDevice *device) const
{
  if (! poly_.size())
    return;

  //---

  // get pixel origin
  auto po = plot_->windowToPixel(Point(0.0, 0.0));

  //---

  // create pixel polygon
  auto ppoly = plot_->windowToPixel(poly_);

  ppoly.addPoint(ppoly.point(0)); // close

  //---

  // calc stroke and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  // draw point
  if      (poly_.size() == 1) {
    auto p1 = ppoly.point(0); // circle radius p1.x

    double r = p1.x - po.x;

    BBox pbbox(po.x - r, po.y - r, po.x + r, po.y + r);

    device->drawEllipse(plot_->pixelToWindow(pbbox));
  }
  // draw line
  else if (poly_.size() == 2) {
    auto p1 = ppoly.point(0); // circle radius p1.x and p2.y
    auto p2 = ppoly.point(1);

    double xr = p1.x - po.x;
    double yr = p2.y - po.y;

    BBox pbbox(po.x - xr, po.y - yr, po.x + xr, po.y + yr);

    device->drawEllipse(plot_->pixelToWindow(pbbox));
  }
  // draw polygon
  else if (poly_.size() >= 3) {
    device->drawPolygon(poly_);
  }

  device->resetColorNames();
}

void
CQChartsRadarObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  QColor fillColor;

  if (plot_->colorColumn().isValid() && plot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    auto ind1 = modelInd();

    Color indColor;

    if (plot_->colorColumnColor(ind1.row(), ind1.parent(), indColor))
      fillColor = plot_->interpColor(indColor, colorInd);
    else
      fillColor = plot_->interpFillColor(colorInd);
  }
  else
    fillColor = plot_->interpFillColor(colorInd);

  auto strokeColor = plot_->interpStrokeColor(colorInd);

  plot_->setPenBrush(penBrush, plot_->penData(strokeColor), plot_->brushData(fillColor));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

//------

CQChartsRadarPlotCustomControls::
CQChartsRadarPlotCustomControls(CQCharts *charts) :
 CQChartsPlotCustomControls(charts, "radar")
{
}

void
CQChartsRadarPlotCustomControls::
init()
{
  addWidgets();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsRadarPlotCustomControls::
addWidgets()
{
  addColumnWidgets();

  addColorColumnWidgets("Fill Color");

  addKeyList();
}

void
CQChartsRadarPlotCustomControls::
addColumnWidgets()
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  //---

  // name and values columns
  addNamedColumnWidgets(QStringList() << "name" << "values", columnsFrame);
}

void
CQChartsRadarPlotCustomControls::
connectSlots(bool b)
{
  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsRadarPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_)
    disconnect(plot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  plot_ = dynamic_cast<CQChartsRadarPlot *>(plot);

  CQChartsPlotCustomControls::setPlot(plot);

  if (plot_)
    connect(plot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
}

void
CQChartsRadarPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  CQChartsPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

CQChartsColor
CQChartsRadarPlotCustomControls::
getColorValue()
{
  return plot_->fillColor();
}

void
CQChartsRadarPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  plot_->setFillColor(c);
}
