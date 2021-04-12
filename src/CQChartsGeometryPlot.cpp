#include <CQChartsGeometryPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsPolygonList.h>
#include <CQChartsPath.h>
#include <CQChartsDataLabel.h>
#include <CQCharts.h>
#include <CQChartsTip.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QMenu>
#include <QAction>

CQChartsGeometryPlotType::
CQChartsGeometryPlotType()
{
}

void
CQChartsGeometryPlotType::
addParameters()
{
  startParameterGroup("Geometry");

  // required object geometry
  addColumnParameter("geometry", "Geometry", "geometryColumn").
   setRequired().setPropPath("columns.geometry").
   setTip("Polygon List, Polygon, Rect or Path geometry");

  // optional value, name, style
  addColumnParameter("name" , "Name" , "nameColumn" ).setPropPath("columns.name").setBasic();
  addColumnParameter("value", "Value", "valueColumn").setPropPath("columns.value").setBasic();
  addColumnParameter("style", "Style", "styleColumn").setPropPath("columns.style");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsGeometryPlotType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto BR  = []() { return CQChartsHtml::Str(CQChartsHtml::Str::Type::BR); };
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Geometry Plot").
    h3("Summary").
     p("Draws polygon list, polygon, rect or path shapes.").
    h3("Columns").
     p("The shape geometry is specified in the " + B("Geometry") + " column.").
     p("The optional shape name can be specified in the " + B("Name") + " column.").
     p("The optional shape value can be specified in the " + B("Value") + " column "
       "and can be used to color the shape by enabling the " + B("colorByValue") + " option." +
       BR() + "This value can be normalized using the " + B("minValue") + " and " +
       B("maxValue") + " values.").
     p("The optional style (fill, stroke) can be specified in the " + B("Style") + " column.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/geometryplot.png"));
}

bool
CQChartsGeometryPlotType::
isColumnForParameter(ColumnDetails *columnDetails, Parameter *parameter) const
{
  if (parameter->name() == "geometry") {
    if (columnDetails->type() == CQChartsPlot::ColumnType::RECT ||
        columnDetails->type() == CQChartsPlot::ColumnType::POLYGON ||
        columnDetails->type() == CQChartsPlot::ColumnType::POLYGON_LIST ||
        columnDetails->type() == CQChartsPlot::ColumnType::PATH)
      return true;

    return false;
  }

  return CQChartsPlotType::isColumnForParameter(columnDetails, parameter);
}

void
CQChartsGeometryPlotType::
analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData)
{
  bool hasGeometry = (analyzeModelData.parameterNameColumn.find("geometry") !=
                      analyzeModelData.parameterNameColumn.end());
  if (hasGeometry) return;

  auto *details = modelData->details();
  if (! details) return;

  auto *charts = modelData->charts();
  auto *model  = modelData->model().data();

  Column geometryColumn;

  int nc = details->numColumns();

  for (int c = 0; c < nc; ++c) {
    auto *columnDetails = details->columnDetails(Column(c));
    if (! columnDetails) continue;

    int         row = 0;
    QModelIndex parent;

    //ModelIndex ind(nullptr, row, columnDetails->column(), parent);

    if (columnDetails->type() == ColumnType::STRING) {
      bool ok;

      auto str = CQChartsModelUtil::modelString(charts, model, row, columnDetails->column(),
                                                parent, ok);
      if (! ok) continue;

      CQChartsGeometryShape shape(str);

      if (shape.type != CQChartsGeometryShape::Type::NONE) {
        geometryColumn = columnDetails->column();
        break;
      }
    }
  }

  if (geometryColumn.isValid())
    analyzeModelData.parameterNameColumn["geometry"] = geometryColumn;
}

CQChartsPlot *
CQChartsGeometryPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsGeometryPlot(view, model);
}

//------

CQChartsGeometryPlot::
CQChartsGeometryPlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("geometry"), model),
 CQChartsObjShapeData<CQChartsGeometryPlot>(this)
{
}

CQChartsGeometryPlot::
~CQChartsGeometryPlot()
{
  term();
}

//---

void
CQChartsGeometryPlot::
init()
{
  CQChartsPlot::init();

  //---

  NoUpdate noUpdate(this);

  dataLabel_ = new CQChartsDataLabel(this);

  setFillColor(Color(Color::Type::PALETTE));

  setFilled (true);
  setStroked(true);

  setLayerActive(CQChartsLayer::Type::FG_PLOT, true);

  //---

  addAxes();

  addTitle();

  //---

  addColorMapKey();
}

void
CQChartsGeometryPlot::
term()
{
  delete dataLabel_;
}

//---

void
CQChartsGeometryPlot::
setNameColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGeometryPlot::
setGeometryColumn(const Column &c)
{
  CQChartsUtil::testAndSet(geometryColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGeometryPlot::
setValueColumn(const Column &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGeometryPlot::
setStyleColumn(const Column &c)
{
  CQChartsUtil::testAndSet(styleColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

CQChartsColumn
CQChartsGeometryPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "geometry") c = this->geometryColumn();
  else if (name == "name"    ) c = this->nameColumn();
  else if (name == "value"   ) c = this->valueColumn();
  else if (name == "style"   ) c = this->styleColumn();
  else                         c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsGeometryPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "geometry") this->setGeometryColumn(c);
  else if (name == "name"    ) this->setNameColumn(c);
  else if (name == "value"   ) this->setValueColumn(c);
  else if (name == "style"   ) this->setStyleColumn(c);
  else                         CQChartsPlot::setNamedColumn(name, c);
}

//---

void
CQChartsGeometryPlot::
setColorByValue(bool b)
{
  setValueStyle(b ? ValueStyle::COLOR : ValueStyle::NONE);
}

//---

double
CQChartsGeometryPlot::
minValue() const
{
  return minValue_.realOr(valueRange_.min(0.0));
}

void
CQChartsGeometryPlot::
setMinValue(double r)
{
  if (! minValue_.isSet() || r != minValue_.value()) {
    minValue_ = OptReal(r); drawObjs();
  }
}

double
CQChartsGeometryPlot::
maxValue() const
{
  return maxValue_.realOr(valueRange_.max(1.0));
}

void
CQChartsGeometryPlot::
setMaxValue(double r)
{
  if (! maxValue_.isSet() || r != maxValue_.value()) {
    maxValue_ = OptReal(r); drawObjs();
  }
}

void
CQChartsGeometryPlot::
setValueStyle(const ValueStyle &valueStyle)
{
  CQChartsUtil::testAndSet(valueStyle_, valueStyle, [&]() { drawObjs(); } );
}

//---

void
CQChartsGeometryPlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns", "nameColumn"    , "name"    , "Name column");
  addProp("columns", "geometryColumn", "geometry", "Geometry column");
  addProp("columns", "valueColumn"   , "value"   , "Value column");
  addProp("columns", "styleColumn"   , "style"   , "Style column");

  // fill
  addProp("fill", "filled", "visible", "Fill visible");

  addFillProperties("fill", "fill", "");

  // stroke
  addProp("stroke", "stroked", "visible", "Stroke visible");

  addLineProperties("stroke", "stroke", "");

  // data label
  dataLabel_->addPathProperties("labels", "Labels");

  // selectable
  addProp("geometry", "geometrySelectable", "selectable", "Geometry selectable");

  // value balloon
  addProp("value", "valueStyle", "style", "Value Style");

  // value normalization
  addProp("value", "minValue", "min", "Min value for color map");
  addProp("value", "maxValue", "max", "Max value for color map");

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
}

void
CQChartsGeometryPlot::
getPropertyNames(QStringList &names, bool hidden) const
{
  CQChartsPlot::getPropertyNames(names, hidden);

  propertyModel()->objectNames(dataLabel_, names, hidden);
}

//---

CQChartsGeom::Range
CQChartsGeometryPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsGeometryPlot::calcRange");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsGeometryPlot *>(this);

  th->clearErrors();

  //---

  Range dataRange;

  auto *model = this->model().data();
  if (! model) return dataRange;

  //---

  th->geometries_.clear();

  th->valueRange_ = RMinMax();

  //---

  // check columns
  bool columnsValid = true;

  // geometry column required
  // value, color and style columns optional

  if (! checkColumn(geometryColumn(), "Geometry", th->geometryColumnType_, /*required*/true))
    columnsValid = false;

  if (! checkColumn(valueColumn(), "Value"))
    columnsValid = false;

  if (! checkColumn(colorColumn(), "Color", th->colorColumnType_))
    columnsValid = false;

  if (! checkColumn(styleColumn(), "Style", th->styleColumnType_))
    columnsValid = false;

  if (! columnsValid)
    return dataRange;

  //---

  // process model data
  class GeometryPlotVisitor : public ModelVisitor {
   public:
    GeometryPlotVisitor(const CQChartsGeometryPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *model, const VisitData &data) override {
      plot_->addRow(model, data, dataRange_);

      return State::OK;
    }

    const Range &dataRange() const { return dataRange_; }

   private:
    const CQChartsGeometryPlot *plot_ { nullptr };
    Range                       dataRange_;
  };

  GeometryPlotVisitor geometryPlotVisitor(this);

  visitModel(geometryPlotVisitor);

  return geometryPlotVisitor.dataRange();
}

void
CQChartsGeometryPlot::
addRow(const QAbstractItemModel *model, const ModelVisitor::VisitData &data,
       Range &dataRange) const
{
  auto *th = const_cast<CQChartsGeometryPlot *>(this);

  //---

  Geometry geometry;

  //---

  // get geometry name
  ModelIndex nameInd;

  if (nameColumn().isValid()) {
    nameInd = ModelIndex(th, data.row, nameColumn(), data.parent);

    bool ok1;

    geometry.name = modelString(nameInd, ok1);
  }

  //---

  // decode geometry column value into polygons
  ModelIndex geometryInd(th, data.row, geometryColumn(), data.parent);

  if (geometryColumnType_ == ColumnType::RECT ||
      geometryColumnType_ == ColumnType::POLYGON ||
      geometryColumnType_ == ColumnType::POLYGON_LIST ||
      geometryColumnType_ == ColumnType::PATH) {
    bool ok2;

    auto var = modelValue(geometryInd, ok2);

    bool converted;

    auto rvar = CQChartsModelUtil::columnUserData(charts(), model, geometryColumn(),
                                                  var, converted);

    bool ok3;

    if      (geometryColumnType_ == ColumnType::RECT) {
      auto bbox = CQChartsVariant::toBBox(rvar, ok3);

      geometry.polygons.push_back(Polygon(bbox));
    }
    else if (geometryColumnType_ == ColumnType::POLYGON) {
      auto poly = CQChartsVariant::toPolygon(rvar, ok3);

      if (poly.size() < 2 || ! poly.boundingBox().isValid()) {
        th->addDataError(geometryInd, "Too few points for polygon");
        return;
      }

      geometry.polygons.push_back(poly);
    }
    else if (geometryColumnType_ == ColumnType::POLYGON_LIST) {
      auto polyList = CQChartsVariant::toPolygonList(rvar, ok3);

      bool rc = true;

      for (const auto &poly : polyList.polygons()) {
        if (poly.size() > 2 && poly.boundingBox().isValid())
          geometry.polygons.push_back(poly);
        else
          rc = false;
      }

      if (! rc) {
        th->addDataError(geometryInd, "Too few points for polygon(s) in list");
        return;
      }
    }
    else if (geometryColumnType_ == ColumnType::PATH) {
      CQChartsPath path;

      if (! CQChartsVariant::toPath(rvar, path)) {
        th->addDataError(geometryInd, "Invalid path");
        return;
      }

      auto poly = Polygon(path.path().toFillPolygon());

      geometry.polygons.push_back(poly);
    }
    else {
      assert(false);
    }

    if (geometry.polygons.empty()) {
      th->addDataError(geometryInd, "Invalid geometry");
      return;
    }
  }
  else {
    bool ok2;

    auto geomStr = modelString(geometryInd, ok2);

    CQChartsGeometryShape shape(geomStr);

    if (shape.type == CQChartsGeometryShape::Type::NONE) {
      th->addDataError(geometryInd, "Invalid geometry '" + geomStr + "'");
      return;
    }

    if      (shape.type == CQChartsGeometryShape::Type::RECT)
      geometry.polygons.push_back(Polygon(shape.rect));
    else if (shape.type == CQChartsGeometryShape::Type::POLYGON) {
      if (shape.polygon.size() < 2) {
        th->addDataError(geometryInd, "Too few points for polygon '" + geomStr + "'");
        return;
      }

      geometry.polygons.push_back(shape.polygon);
    }
    else if (shape.type == CQChartsGeometryShape::Type::POLYGON_LIST) {
      geometry.polygons = shape.polygonList;
    }
    else if (shape.type == CQChartsGeometryShape::Type::PATH) {
      auto poly = Polygon(shape.path.qpoly());

      geometry.polygons.push_back(poly);
    }

    if (geometry.polygons.empty()) {
      th->addDataError(geometryInd, "Invalid geometry '" + geomStr + "'");
      return;
    }
  }

  //---

  // update range from polygons
  for (auto &poly : geometry.polygons) {
    for (int j = 0; j < poly.size(); ++j) {
      auto p = poly.point(j);

      dataRange.updateRange(p.x, p.y);

      geometry.bbox.add(p.x, p.y);
    }
  }

  //---

  // get geometry associated value
  if (valueColumn().isValid()) {
    ModelIndex valueInd(th, data.row, valueColumn(), data.parent);

    bool ok3;

    double value = modelReal(valueInd, ok3);

    if (! ok3) {
      if (! isSkipBad()) {
        th->addDataError(geometryInd, "Invalid value real");
        return;
      }

      value = 0;
    }

    if (! CMathUtil::isNaN(value))
      geometry.value = OptReal(value);

    // update value range
    if (geometry.value.isSet())
      th->valueRange_.add(geometry.value.real());
  }

  //---

#if 0
  // get geometry custom color
  // TODO: just call colorColumnColor (duplicate code in calcPenBrush)
  if (colorColumn().isValid()) {
    ModelIndex colorInd(th, data.row, colorColumn(), data.parent);

    if (colorColumnType_ == ColumnType::COLOR) {
      Color c;

      if (colorColumnColor(data.row, data.parent, c))
        geometry.color = c;
    }
#if 0
    else {
      bool ok4;

      auto str = modelString(colorInd, ok4);

      if (ok4)
        geometry.color = Color(str);
    }
#endif
  }
#endif

  //---

  // get geometry custom style
  if (styleColumn().isValid()) {
    ModelIndex styleInd(th, data.row, styleColumn(), data.parent);

    bool ok4;

    if (styleColumnType_ == ColumnType::STYLE) {
      auto str = modelString(styleInd, ok4);

      geometry.style = CQChartsStyle(str);
    }
  }

  //---

  // save model index for geometry
  auto geomInd  = modelIndex(geometryInd);
  auto geomInd1 = normalizeIndex(geomInd);

  geometry.ind = geomInd1;

  //---

  // add to list
  th->geometries_.push_back(geometry);
}

bool
CQChartsGeometryPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsGeometryPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  int n = geometries_.size();

  for (int i = 0; i < n; ++i) {
    const auto &geometry = geometries_[i];

    auto bbox = geometry.bbox;

    ColorInd iv(i, n);

    auto *geomObj = createGeometryObj(bbox, geometry.polygons, geometry.ind, iv);

    geomObj->setName (geometry.name);
//  geomObj->setColor(geometry.color);
    geomObj->setStyle(geometry.style);

    if (geometry.value.isSet())
      geomObj->setValue(geometry.value.real());

    objs.push_back(geomObj);
  }

  //---

  return true;
}

//---

bool
CQChartsGeometryPlot::
probe(ProbeData &probeData) const
{
  CQChartsPlotObj *obj;

  if (! objNearestPoint(probeData.p, obj))
    return false;

  auto c = obj->rect().getCenter();

  probeData.p    = c;
  probeData.both = true;

  probeData.xvals.emplace_back(c.x, "", "");
  probeData.yvals.emplace_back(c.y, "", "");

  return true;
}

//---

bool
CQChartsGeometryPlot::
addMenuItems(QMenu *menu)
{
  bool added = false;

  if (canDrawColorMapKey()) {
    auto *keysMenu = new QMenu("Keys", menu);

    addMenuCheckedAction(keysMenu, "Color Key", isColorMapKey(),
                         SLOT(setColorMapKey(bool)));

    menu->addMenu(keysMenu);

    added = true;
  }

  return added;
}

//---

bool
CQChartsGeometryPlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsGeometryPlot::
execDrawForeground(PaintDevice *device) const
{
  if (isColorMapKey())
    drawColorMapKey(device);
}

//---

void
CQChartsGeometryPlot::
write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
      const QString &viewVarName) const
{
  CQChartsPlot::write(os, plotVarName, modelVarName, viewVarName);

  dataLabel_->write(os, plotVarName);
}

//---

CQChartsGeometryObj *
CQChartsGeometryPlot::
createGeometryObj(const BBox &rect, const Polygons &polygons,
                  const QModelIndex &ind, const ColorInd &iv) const
{
  return new CQChartsGeometryObj(this, rect, polygons, ind, iv);
}

//---

CQChartsPlotCustomControls *
CQChartsGeometryPlot::
createCustomControls()
{
  auto *controls = new CQChartsGeometryPlotCustomControls(charts());

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsGeometryObj::
CQChartsGeometryObj(const CQChartsGeometryPlot *plot, const BBox &rect, const Polygons &polygons,
                    const QModelIndex &ind, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsGeometryPlot *>(plot), rect, ColorInd(), ColorInd(), iv),
 plot_(plot), polygons_(polygons)
{
  setDetailHint(DetailHint::MAJOR);

  setModelInd(ind);
}

QString
CQChartsGeometryObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(iv().i);
}

QString
CQChartsGeometryObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  //---

  auto addColumnRowValue = [&](const Column &column, const QString &label="",
                               const QString &defLabel="") {
    if (! column.isValid()) return;

    if (tableTip.hasColumn(column))
      return;

    ModelIndex columnInd(plot_, modelInd().row(), column, modelInd().parent());

    bool ok;
    auto name = plot_->modelString(columnInd, ok);
    if (! ok) return;

    QString label1 = label;

    if (label1 == "") {
      label1 = plot_->columnHeaderName(column, /*tip*/true);
      if (label1 == "") label1 = defLabel;
    }

    tableTip.addTableRow(label1, name);

    tableTip.addColumn(column);
  };

  //---

  tableTip.addTableRow("Name", name());

  if (hasValue())
    tableTip.addTableRow("Value", value());

  addColumnRowValue(plot_->colorColumn(), "Color");

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

bool
CQChartsGeometryObj::
inside(const Point &p) const
{
  if (! plot_->isGeometrySelectable())
    return false;

  auto p1 = p;

  for (const auto &poly : polygons_) {
    if (poly.containsPoint(p1, Qt::OddEvenFill))
      return true;
  }

  return false;
}

//---

void
CQChartsGeometryObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->nameColumn    ());
  addColumnSelectIndex(inds, plot_->geometryColumn());
  addColumnSelectIndex(inds, plot_->valueColumn   ());
  addColumnSelectIndex(inds, plot_->styleColumn   ());
}

//---

void
CQChartsGeometryObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  /// draw polygon
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  for (const auto &ppoly : polygons_)
    device->drawPolygon(Polygon(ppoly));

  device->resetColorNames();
}

void
CQChartsGeometryObj::
drawFg(PaintDevice *device) const
{
  plot_->dataLabel()->draw(device, rect(), name());

  //---

  // draw balloon for value
  if (plot_->valueStyle() == CQChartsGeometryPlot::ValueStyle::BALLOON && hasValue()) {
    auto safeSqrt = [](double r) {
      return (r > 0.0 ? std::sqrt(r) : 0.0);
    };

    auto prect = plot_->windowToPixel(rect());

    auto pbbox = plot_->calcPlotPixelRect();

    double minSize = plot_->minBalloonSize()*pbbox.getHeight();
    double maxSize = plot_->maxBalloonSize()*pbbox.getHeight();

    double v1    = safeSqrt(value());
    double minV1 = safeSqrt(plot_->minValue());
    double maxV1 = safeSqrt(plot_->maxValue());

    double s = CMathUtil::map(v1, minV1, maxV1, minSize, maxSize);

    //---

    // TODO: customize color
    auto colorInd = calcColorInd();

    auto pc = QColor(Qt::black);
    auto bc = QColor(Qt::red);

    if (plot_->colorColumn().isValid()) {
      auto ind1 = modelInd();

      ModelIndex ind2(plot_, ind1.row(), plot_->colorColumn(), ind1.parent());

      Color indColor;

      if (plot_->modelIndexColor(ind2, indColor))
        bc = plot_->interpColor(indColor, colorInd);
      else
        bc = QColor();
    }

    PenBrush penBrush;

    plot_->setPenBrush(penBrush,
      PenData  (/*stroke*/true, pc),
      BrushData(/*filled*/true, bc, Alpha(0.5)));

    plot_->updateObjPenBrushState(this, penBrush);

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    BBox ebbox(prect.getXMid() - s/2, prect.getYMid() - s/2,
               prect.getXMid() + s/2, prect.getYMid() + s/2);

    device->drawEllipse(plot_->pixelToWindow(ebbox));
  }
}

void
CQChartsGeometryObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  //---

  // calc fill color
  auto calcFillColor = [&]() {
    if (plot_->colorColumn().isValid()) {
      auto ind1 = modelInd();

      ModelIndex ind2(plot_, ind1.row(), plot_->colorColumn(), ind1.parent());

      Color indColor;

      if (plot_->modelIndexColor(ind2, indColor))
        return plot_->interpColor(indColor, colorInd);
    }

    //---

    if (hasValue() && plot_->valueStyle() == CQChartsGeometryPlot::ValueStyle::COLOR) {
      double dv = (value() - plot_->minValue())/(plot_->maxValue() - plot_->minValue());

#if 0
      if (color().isValid())
        return plot_->interpColor(color(), ColorInd(dv));
      else
        return plot_->interpColor(plot_->fillColor(), ColorInd(dv));
#endif
      return plot_->interpColor(plot_->fillColor(), ColorInd(dv));
    }

    //---

    //if  (color().isValid())
    //  return plot_->interpColor(color(), colorInd);

    return plot_->interpFillColor(colorInd);
  };

  auto fc = calcFillColor();

  //---

  // calc stroke color
  auto bc = plot_->interpStrokeColor(colorInd);

  //---

  // calc pen and brush
  plot_->setPenBrush(penBrush,
    PenData  (plot_->isStroked(), bc, plot_->strokeAlpha(),
              plot_->strokeWidth(), plot_->strokeDash()),
    BrushData(plot_->isFilled(), fc, plot_->fillAlpha(), plot_->fillPattern()));

  if (style().isValid()) {
    penBrush.pen   = style().pen  ();
    penBrush.brush = style().brush();
  }

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsGeometryObj::
writeScriptData(ScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  std::ostream &os = device->os();

  os << "\n";

  if (hasValue())
    os << "  this.value = " << value() << ";\n";
}

//------

CQChartsGeometryShape::
CQChartsGeometryShape(const QString &str)
{
  if      (CQChartsUtil::stringToPolygons(str, polygonList))
    type = Type::POLYGON_LIST;
  else if (CQChartsUtil::stringToPolygon(str, polygon))
    type = Type::POLYGON;
  else if (CQChartsUtil::stringToBBox(str, rect))
    type = Type::RECT;
  else if (CQChartsUtil::stringToPath(str, path))
    type = Type::PATH;
  else
    type = Type::NONE;
}

//------

CQChartsGeometryPlotCustomControls::
CQChartsGeometryPlotCustomControls(CQCharts *charts) :
 CQChartsPlotCustomControls(charts, "geometry")
{
   // options group
  auto optionsFrame = createGroupFrame("Options", "optionsFrame");

  //---

  static auto columnNames = QStringList() << "geometry" << "name" << "value" << "style";

  addColumnWidgets(columnNames, optionsFrame);

  //---

  addColorColumnWidgets();

  //---

  connectSlots(true);
}

void
CQChartsGeometryPlotCustomControls::
connectSlots(bool b)
{
  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsGeometryPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  plot_ = dynamic_cast<CQChartsGeometryPlot *>(plot);

  CQChartsPlotCustomControls::setPlot(plot);
}

void
CQChartsGeometryPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  CQChartsPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

CQChartsColor
CQChartsGeometryPlotCustomControls::
getColorValue()
{
  return plot_->fillColor();
}

void
CQChartsGeometryPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  plot_->setFillColor(c);
}
