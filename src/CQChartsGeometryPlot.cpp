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
#include <CQChartsDrawUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

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
   setRequired().setTip("Polygon List, Polygon, Rect or Path geometry");

  // optional value, name, style
  addColumnParameter("value", "Value", "valueColumn");
  addColumnParameter("name" , "Name" , "nameColumn" );
  addColumnParameter("style", "Style", "styleColumn");

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
isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                     CQChartsPlotParameter *parameter) const
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
analyzeModel(CQChartsModelData *modelData, CQChartsAnalyzeModelData &analyzeModelData)
{
  bool hasGeometry = (analyzeModelData.parameterNameColumn.find("geometry") !=
                      analyzeModelData.parameterNameColumn.end());
  if (hasGeometry) return;

  auto *details = modelData->details();
  if (! details) return;

  auto *charts = modelData->charts();
  auto *model  = modelData->model().data();

  CQChartsColumn geometryColumn;

  int nc = details->numColumns();

  for (int c = 0; c < nc; ++c) {
    auto columnDetails = details->columnDetails(CQChartsColumn(c));
    if (! columnDetails) continue;

    CQChartsModelIndex ind(/*row*/0, columnDetails->column(), /*parent*/QModelIndex());

    if (columnDetails->type() == CQBaseModelType::STRING) {
      bool ok;

      auto str = CQChartsModelUtil::modelString(charts, model, ind, ok);
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
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsGeometryPlot(view, model);
}

//------

CQChartsGeometryPlot::
CQChartsGeometryPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("geometry"), model),
 CQChartsObjShapeData<CQChartsGeometryPlot>(this)
{
  NoUpdate noUpdate(this);

  dataLabel_ = new CQChartsDataLabel(this);

  setFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setFilled (true);
  setStroked(true);

  setLayerActive(CQChartsLayer::Type::FG_PLOT, true);

  addAxes();

  addTitle();
}

CQChartsGeometryPlot::
~CQChartsGeometryPlot()
{
  delete dataLabel_;
}

//---

void
CQChartsGeometryPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGeometryPlot::
setGeometryColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(geometryColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGeometryPlot::
setValueColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGeometryPlot::
setStyleColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(styleColumn_, c, [&]() { updateRangeAndObjs(); } );
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
  return minValue_.value_or(valueRange_.min(0.0));
}

void
CQChartsGeometryPlot::
setMinValue(double r)
{
  if (! minValue_ || r != minValue_.value()) {
    minValue_ = r; drawObjs();
  }
}

double
CQChartsGeometryPlot::
maxValue() const
{
  return maxValue_.value_or(valueRange_.max(1.0));
}

void
CQChartsGeometryPlot::
setMaxValue(double r)
{
  if (! maxValue_ || r != maxValue_.value()) {
    maxValue_ = r; drawObjs();
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
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

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

  CQChartsGeom::Range dataRange;

  QAbstractItemModel *model = this->model().data();
  if (! model) return dataRange;

  //---

  auto *th = const_cast<CQChartsGeometryPlot *>(this);

  th->geometries_.clear();

  th->valueRange_ = CQChartsGeom::RMinMax();

  //---

  // check columns
  bool columnsValid = true;

  th->clearErrors();

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

    const CQChartsGeom::Range &dataRange() const { return dataRange_; }

   private:
    const CQChartsGeometryPlot *plot_ { nullptr };
    CQChartsGeom::Range         dataRange_;
  };

  GeometryPlotVisitor geometryPlotVisitor(this);

  visitModel(geometryPlotVisitor);

  return geometryPlotVisitor.dataRange();
}

void
CQChartsGeometryPlot::
addRow(const QAbstractItemModel *model, const ModelVisitor::VisitData &data,
       CQChartsGeom::Range &dataRange) const
{
  auto *th = const_cast<CQChartsGeometryPlot *>(this);

  //---

  Geometry geometry;

  //---

  // get geometry name
  CQChartsModelIndex nameInd;

  if (nameColumn().isValid()) {
    nameInd = CQChartsModelIndex(data.row, nameColumn(), data.parent);

    bool ok1;

    geometry.name = modelString(nameInd, ok1);
  }

  //---

  // decode geometry column value into polygons
  CQChartsModelIndex geometryInd(data.row, geometryColumn(), data.parent);

  if (geometryColumnType_ == ColumnType::RECT ||
      geometryColumnType_ == ColumnType::POLYGON ||
      geometryColumnType_ == ColumnType::POLYGON_LIST ||
      geometryColumnType_ == ColumnType::PATH) {
    bool ok2;

    QVariant var = modelValue(geometryInd, ok2);

    bool converted;

    QVariant rvar =
      CQChartsModelUtil::columnUserData(charts(), model, geometryColumn(), var, converted);

    bool ok3;

    if      (geometryColumnType_ == ColumnType::RECT) {
      auto bbox = CQChartsVariant::toBBox(rvar, ok3);

      geometry.polygons.push_back(CQChartsGeom::Polygon(bbox));
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
      CQChartsPolygonList polyList = CQChartsVariant::toPolygonList(rvar, ok3);

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
      CQChartsPath path = CQChartsVariant::toPath(rvar, ok3);

      auto poly = CQChartsGeom::Polygon(path.path().toFillPolygon());

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

    QString geomStr = modelString(geometryInd, ok2);

    CQChartsGeometryShape shape(geomStr);

    if (shape.type == CQChartsGeometryShape::Type::NONE) {
      th->addDataError(geometryInd, "Invalid geometry '" + geomStr + "'");
      return;
    }

    if      (shape.type == CQChartsGeometryShape::Type::RECT)
      geometry.polygons.push_back(CQChartsGeom::Polygon(shape.rect));
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
      auto poly = CQChartsGeom::Polygon(shape.path.qpoly());

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
    CQChartsModelIndex valueInd(data.row, valueColumn(), data.parent);

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
      geometry.value = value;

    // update value range
    if (geometry.value)
      th->valueRange_.add(*geometry.value);
  }

  //---

  // get geometry custom color
  if (colorColumn().isValid()) {
    CQChartsModelIndex colorInd(data.row, colorColumn(), data.parent);

    if (colorColumnType_ == ColumnType::COLOR) {
      CQChartsColor c;

      if (colorColumnColor(data.row, data.parent, c))
        geometry.color = c;
    }
    else {
      bool ok4;

      QString str = modelString(colorInd, ok4);

      if (ok4)
        geometry.color = CQChartsColor(str);
    }
  }

  //---

  // get geometry custom style
  if (styleColumn().isValid()) {
    CQChartsModelIndex styleInd(data.row, styleColumn(), data.parent);

    bool ok4;

    if (styleColumnType_ == ColumnType::STYLE) {
      QString str = modelString(styleInd, ok4);

      geometry.style = CQChartsStyle(str);
    }
  }

  //---

  // save model index for geometry
  QModelIndex geomInd  = modelIndex(geometryInd);
  QModelIndex geomInd1 = normalizeIndex(geomInd);

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
    const Geometry &geometry = geometries_[i];

    auto bbox = geometry.bbox;

    ColorInd iv(i, n);

    auto *geomObj = new CQChartsGeometryObj(this, bbox, geometry.polygons, geometry.ind, iv);

    geomObj->setName (geometry.name);
    geomObj->setColor(geometry.color);
    geomObj->setStyle(geometry.style);

    if (geometry.value)
      geomObj->setValue(*geometry.value);

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

  probeData.xvals.push_back(c.x);
  probeData.yvals.push_back(c.y);

  return true;
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

//------

CQChartsGeometryObj::
CQChartsGeometryObj(const CQChartsGeometryPlot *plot, const CQChartsGeom::BBox &rect,
                    const CQChartsGeom::Polygons &polygons, const QModelIndex &ind,
                    const ColorInd &iv) :
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
  if (! plot_->valueColumn().isValid())
    return name();

  return QString("%1:%2:%3").arg(typeName()).arg(name()).arg(value());
}

QString
CQChartsGeometryObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name" , name ());
  tableTip.addTableRow("Value", value());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

bool
CQChartsGeometryObj::
inside(const CQChartsGeom::Point &p) const
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
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->nameColumn    ());
  addColumnSelectIndex(inds, plot_->geometryColumn());
  addColumnSelectIndex(inds, plot_->valueColumn   ());
  addColumnSelectIndex(inds, plot_->styleColumn   ());
}

//---

void
CQChartsGeometryObj::
draw(CQChartsPaintDevice *device)
{
  // calc pen and brush
  CQChartsPenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  /// draw polygon
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  for (const auto &ppoly : polygons_)
    device->drawPolygon(CQChartsGeom::Polygon(ppoly));

  device->resetColorNames();
}

void
CQChartsGeometryObj::
drawFg(CQChartsPaintDevice *device) const
{
  plot_->dataLabel()->draw(device, rect(), name());

  //---

  if (plot_->valueStyle() == CQChartsGeometryPlot::ValueStyle::BALLOON && value() > 0) {
    auto prect = plot_->windowToPixel(rect());

    auto pbbox = plot_->calcPlotPixelRect();

    double minSize = plot_->minBalloonSize()*pbbox.getHeight();
    double maxSize = plot_->maxBalloonSize()*pbbox.getHeight();

    double v1    = sqrt(value());
    double minV1 = sqrt(plot_->minValue());
    double maxV1 = sqrt(plot_->maxValue());

    double s = CMathUtil::map(v1, minV1, maxV1, minSize, maxSize);

    //---

    ColorInd colorInd;

    QColor pc = QColor(Qt::black);
    QColor bc = QColor(Qt::red);

    CQChartsPenBrush penBrush;

    plot_->setPenBrush(penBrush,
      CQChartsPenData  (/*stroke*/true, pc),
      CQChartsBrushData(/*filled*/true, bc, CQChartsAlpha(0.5)));

    plot_->updateObjPenBrushState(this, penBrush);

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    CQChartsGeom::BBox ebbox(prect.getXMid() - s/2, prect.getYMid() - s/2,
                             prect.getXMid() + s/2, prect.getYMid() + s/2);

    device->drawEllipse(device->pixelToWindow(ebbox));
  }
}

void
CQChartsGeometryObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  // calc pen and brush
  QColor fc;

  double dv = (value() - plot_->minValue())/(plot_->maxValue() - plot_->minValue());

  ColorInd colorInd = calcColorInd();

  if (color().isValid()) {
    if (hasValue_ && plot_->valueStyle() == CQChartsGeometryPlot::ValueStyle::COLOR)
      fc = plot_->interpColor(color(), ColorInd(dv));
    else
      fc = plot_->interpColor(color(), colorInd);
  }
  else {
    if (hasValue_ && plot_->valueStyle() == CQChartsGeometryPlot::ValueStyle::COLOR)
      fc = plot_->interpColor(plot_->fillColor(), ColorInd(dv));
    else
      fc = plot_->interpFillColor(colorInd);
  }

  QColor bc = plot_->interpStrokeColor(colorInd);

  plot_->setPenBrush(penBrush,
    CQChartsPenData  (plot_->isStroked(), bc, plot_->strokeAlpha(),
                      plot_->strokeWidth(), plot_->strokeDash()),
    CQChartsBrushData(plot_->isFilled(), fc, plot_->fillAlpha(), plot_->fillPattern()));

  if (style().isValid()) {
    penBrush.pen   = style().pen  ();
    penBrush.brush = style().brush();
  }

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsGeometryObj::
writeScriptData(CQChartsScriptPainter *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  std::ostream &os = device->os();

  os << "\n";
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
