#include <CQChartsVennPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
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

CQChartsVennPlotType::
CQChartsVennPlotType()
{
}

void
CQChartsVennPlotType::
addParameters()
{
  startParameterGroup("Venn");

  addColumnParameter("name", "Name", "nameColumn").
    setRequired().setPropPath("columns.name").setTip("Name column");
  addColumnParameter("value", "Value", "valueColumn").
    setRequired().setPropPath("columns.value").setTip("Value column");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsVennPlotType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
    h2("Venn Diagram Plot").
     h3("Summary").
      p("Draws venn diagram of two or three columns.").
     h3("Columns").
     p("The " + B("Values") + " column specifies the values.").
     h3("Limitations").
      p("None.").
     h3("Example").
      p(IMG("images/venn.png"));
}

void
CQChartsVennPlotType::
analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData)
{
  auto *details = modelData->details();
  if (! details) return;

  Column nameColumn, valueColumn;

  for (int c = 0; c < details->numColumns(); ++c) {
    const auto *columnDetails = details->columnDetails(Column(c));
    if (! columnDetails) continue;

    if      (! nameColumn.isValid())
      nameColumn = Column(c);
    else if (! valueColumn.isValid())
      valueColumn = Column(c);
    else
      break;
  }

  if (nameColumn.isValid() && valueColumn.isValid()) {
    analyzeModelData.parameterNameColumn["name" ] = nameColumn;
    analyzeModelData.parameterNameColumn["value"] = valueColumn;
  }
}

CQChartsPlot *
CQChartsVennPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsVennPlot(view, model);
}

//------

CQChartsVennPlot::
CQChartsVennPlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("venn"), model),
 CQChartsObjShapeData<CQChartsVennPlot>(this),
 CQChartsObjTextData <CQChartsVennPlot>(this)
{
}

CQChartsVennPlot::
~CQChartsVennPlot()
{
  CQChartsVennPlot::term();
}

//---

void
CQChartsVennPlot::
init()
{
  CQChartsPlot::init();

  //---

  NoUpdate noUpdate(this);

  setFillColor(Color::makePalette());
  setFillAlpha(Alpha(0.5));

  setFilled (true);
  setStroked(true);

  setTextColor(Color::makeInterfaceValue(1.0));

  //---

  addTitle();

  //---

  addColorMapKey();
}

void
CQChartsVennPlot::
term()
{
}

//------

void
CQChartsVennPlot::
setNameColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsVennPlot::
setValueColumn(const Column &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

//---

void
CQChartsVennPlot::
setStartAngle(const Angle &a)
{
  CQChartsUtil::testAndSet(startAngle_, a, [&]() {
     updateObjs(); Q_EMIT customDataChanged();
  } );
}

//---

void
CQChartsVennPlot::
setLevel1LabelType(const LabelType &t)
{
  CQChartsUtil::testAndSet(level1LabelType_, t, [&]() {
     drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsVennPlot::
setLevel2LabelType(const LabelType &t)
{
  CQChartsUtil::testAndSet(level2LabelType_, t, [&]() {
     drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsVennPlot::
setLevel3LabelType(const LabelType &t)
{
  CQChartsUtil::testAndSet(level3LabelType_, t, [&]() {
     drawObjs(); Q_EMIT customDataChanged();
  } );
}

//------

CQChartsColumn
CQChartsVennPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "name" ) c = this->nameColumn();
  else if (name == "value") c = this->valueColumn();
  else                      c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsVennPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "name" ) this->setNameColumn(c);
  else if (name == "value") this->setValueColumn(c);
  else                      CQChartsPlot::setNamedColumn(name, c);
}

//------

void
CQChartsVennPlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns", "nameColumn" , "name" , "Name column");
  addProp("columns", "valueColumn", "value", "Value column");

  // optiosn
  addProp("options", "startAngle"     , "", "Start angle");
  addProp("options", "level1LabelType", "", "Level 1 label type");
  addProp("options", "level2LabelType", "", "Level 2 label type");
  addProp("options", "level3LabelType", "", "Level 3 label type");

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
CQChartsVennPlot::
calcRange() const
{
  PathDatas pathDatas;

  if (! getPathDatas(pathDatas))
    return Range(-1, -1, 1, 1);

  //---

  Range dataRange;

  for (const auto &pathData : pathDatas) {
    auto rect = BBox(pathData.poly.boundingRect());

    dataRange.updateRange(rect);
  }

  return dataRange;
}

//------

bool
CQChartsVennPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsVennPlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsVennPlot *>(this);

  th->clearErrors();

  //---

  PathDatas pathDatas;

  if (! getPathDatas(pathDatas))
    return false;

  //---

  int ind = 0;

  for (const auto &pathData : pathDatas) {
    auto rect = BBox(pathData.poly.boundingRect());

    auto *obj = th->createCircleObj(rect, pathData);

    objs.push_back(obj);

    ++ind;
  }

  return true;
}

bool
CQChartsVennPlot::
getPathDatas(PathDatas &pathDatas) const
{
  auto *th = const_cast<CQChartsVennPlot *>(this);

  // check columns
  bool columnsValid = true;

  // value column required, count column optional
  if (! checkColumn(nameColumn (), "Name" , /*required*/true) ||
      ! checkColumn(valueColumn(), "Value", /*required*/true))
    columnsValid = false;

  if (! columnsValid)
    return false;

  //---

  class VennModelVisitor : public ModelVisitor {
   public:
    using NameSet   = std::set<QString>;
    using NameValue = std::map<QString, double>;

   public:
    VennModelVisitor(const CQChartsVennPlot *vennPlot) :
     vennPlot_(vennPlot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      ModelIndex nameInd (vennPlot_, data.row, vennPlot_->nameColumn (), data.parent);
      ModelIndex valueInd(vennPlot_, data.row, vennPlot_->valueColumn(), data.parent);

      bool ok1;
      auto name = vennPlot_->modelString(nameInd, ok1);
      if (! ok1) return State::SKIP;

      bool ok2;
      auto r = vennPlot_->modelReal(valueInd, ok2);
      if (! ok2) return State::SKIP;

      //---

      auto names = name.split("/", Qt::KeepEmptyParts);

      names.sort();

      for (auto &name : names)
        names_.insert(name);

      name = names.join("/");

      nameValue_[name] = r;

      return State::OK;
    }

    QStringList names() const {
      QStringList names;
      for (const auto &name : names_)
        names.push_back(name);
      return names;
    }

    const NameValue &nameValue() const { return nameValue_; }

   private:
    const CQChartsVennPlot* vennPlot_ { nullptr };
    NameSet                 names_;
    NameValue               nameValue_;
  };

  VennModelVisitor visitor(this);

  visitModel(visitor);

  auto names = visitor.names();

  th->n_ = names.size();
  if (n_ > 3) return false;

  //---

  auto indPath = [&](int i) {
    double a1 = startAngle().radians();
    double a  = 2.0*M_PI/n_;

    auto dx = std::cos(i*a + a1);
    auto dy = std::sin(i*a + a1);

    double ir = 0.5/sqrt(2.0);
    double r  = 0.5;

    auto c = Point(dx*ir, dy*ir);

    QPainterPath path;

    BBox bbox(c.x - r, c.y - r, c.x + r, c.y + r);

    CQChartsDrawUtil::ellipsePath(path, bbox);

    return path;
  };

  //---

  struct PathName {
    QPainterPath path;
    QString      name;

    PathName(const QPainterPath &path, const QString &name) :
     path(path), name(name) {
    }
  };

  using PathNames = std::vector<PathName>;

  PathNames pathNames;

  //---

  int ind = 0;

  for (const auto &name : names) {
    auto path = indPath(ind);

    pathNames.push_back(PathName(path, name));

    ++ind;
  }

  //---

  const auto &nameValue = visitor.nameValue();

  //---

  ind = 0;

  auto pathPoly = [](const QPainterPath &path) {
    auto points = CQChartsPath::pathPoints(path, 1E-6);
    QPolygonF poly;
    for (const auto &p : points)
      poly << p.qpoint();
    return poly;
  };

  for (int i = 0; i < n_; ++i) {
    const auto &pathName1 = pathNames[i];

    auto poly1 = pathPoly(pathName1.path);

    for (int j = 0; j < n_; ++j) {
      if (i == j) continue;

      const auto &pathName2 = pathNames[j];

      poly1 = poly1.subtracted(pathPoly(pathName2.path));
    }

    CQChartsPathData pathData1(poly1);

    pathData1.names.push_back(pathName1.name);

    auto pn1 = nameValue.find(pathName1.name);

    if (pn1 != nameValue.end())
      pathData1.value = OptReal((*pn1).second);

    pathData1.inds.push_back(i);

    pathData1.level = 1;
    pathData1.ind   = ++ind;

    pathDatas.push_back(pathData1);
  }

  //---

  QPolygonF allPoly;

  if (n_ == 3) {
    allPoly = pathPoly(pathNames[0].path);

    for (int i = 1; i < n_; ++i)
      allPoly = allPoly.intersected(pathPoly(pathNames[i].path));

    CQChartsPathData allPathData(allPoly);

    for (int i = 0; i < n_; ++i)
      allPathData.names.push_back(pathNames[i].name);

    auto pn1 = nameValue.find(allPathData.names.join("/"));

    if (pn1 != nameValue.end())
      allPathData.value = OptReal((*pn1).second);

    for (int i = 0; i < n_; ++i)
      allPathData.inds.push_back(i);

    allPathData.level = 3;
    allPathData.ind   = ++ind;

    pathDatas.push_back(allPathData);
  }

  //---

  for (int i = 0; i < n_; ++i) {
    const auto &pathName1 = pathNames[i];

    for (int j = i + 1; j < n_; ++j) {
      const auto &pathName2 = pathNames[j];

      auto poly1 = pathPoly(pathName1.path);

      poly1 = poly1.intersected(pathPoly(pathName2.path));

      if (n_ == 3)
        poly1 = poly1.subtracted(allPoly);

      //---

      CQChartsPathData pathData1(poly1);

      pathData1.names.push_back(pathName1.name);
      pathData1.names.push_back(pathName2.name);

      auto pn1 = nameValue.find(pathData1.names.join("/"));

      if (pn1 != nameValue.end())
        pathData1.value = OptReal((*pn1).second);

      pathData1.inds.push_back(i);
      pathData1.inds.push_back(j);

      pathData1.level = 2;
      pathData1.ind   = ++ind;

      pathDatas.push_back(pathData1);
    }
  }

  //---

  return true;
}

//---

bool
CQChartsVennPlot::
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

CQChartsCircleObj *
CQChartsVennPlot::
createCircleObj(const BBox &rect, const CQChartsPathData &pathData)
{
  return new CQChartsCircleObj(this, rect, pathData);
}

//---

bool
CQChartsVennPlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsVennPlot::
execDrawForeground(PaintDevice *device) const
{
  CQChartsPlot::execDrawForeground(device);
}

//---

CQChartsPlotCustomControls *
CQChartsVennPlot::
createCustomControls()
{
  auto *controls = new CQChartsVennPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsCircleObj::
CQChartsCircleObj(const CQChartsVennPlot *vennPlot, const BBox &rect,
                  const CQChartsPathData &pathData) :
 CQChartsPlotObj(const_cast<CQChartsVennPlot *>(vennPlot), rect, ColorInd(),
                 ColorInd(), ColorInd()),
 vennPlot_(vennPlot), pathData_(pathData)
{
  setDetailHint(DetailHint::MAJOR);
}

QString
CQChartsCircleObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(pathData_.ind);
}

QString
CQChartsCircleObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name", pathData_.names.join(", "));

  if (pathData_.value.isSet())
    tableTip.addTableRow("Value", pathData_.value.real());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

void
CQChartsCircleObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  auto path1 = path + "/" + propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);

  model->addProperty(path1, this, "name")->setDesc("Name");
}

//---

void
CQChartsCircleObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, vennPlot_->nameColumn ());
  addColumnSelectIndex(inds, vennPlot_->valueColumn());
}

void
CQChartsCircleObj::
draw(PaintDevice *device) const
{
  // calc stroke and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawPolygon(Polygon(pathData_.poly));

  //---

  calcTextPenBrush(penBrush, updateState);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  auto textOptions = vennPlot_->textOptions();

  textOptions.angle            = Angle();
  textOptions.align            = Qt::AlignHCenter | Qt::AlignVCenter;
  textOptions.scaled           = true;
  textOptions.minScaleFontSize = 4;
  textOptions.maxScaleFontSize = 1000;

  auto labelType { CQChartsVennPlot::LabelType::NAME };

  if      (pathData_.level == 1) labelType = vennPlot_->level1LabelType();
  else if (pathData_.level == 2) labelType = vennPlot_->level2LabelType();
  else if (pathData_.level == 3) labelType = vennPlot_->level3LabelType();

  QStringList names;

  if (labelType == CQChartsVennPlot::LabelType::NAME ||
      labelType == CQChartsVennPlot::LabelType::NAME_VALUE)
    names << pathData_.names;

  if (labelType == CQChartsVennPlot::LabelType::VALUE ||
      labelType == CQChartsVennPlot::LabelType::NAME_VALUE)
    names << QString(pathData_.value.isSet() ? QString::number(pathData_.value.real()) : "0.0");

  CQChartsDrawUtil::drawTextsInCircle(device, rect(), names, textOptions);

  //---

  device->resetColorNames();
}

void
CQChartsCircleObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  std::vector<QColor> fillColors;

  for (int ind : pathData_.inds) {
    ColorInd colorInd(ind, vennPlot_->numNames());

    QColor fillColor;

    if (vennPlot_->colorColumn().isValid() &&
        vennPlot_->colorType() == CQChartsPlot::ColorType::AUTO) {
      auto ind1 = modelInd();

      Color indColor;

      if (vennPlot_->colorColumnColor(ind1.row(), ind1.parent(), indColor))
        fillColor = vennPlot_->interpColor(indColor, colorInd);
      else
        fillColor = vennPlot_->interpFillColor(colorInd);
    }
    else {
      fillColor = vennPlot_->interpFillColor(colorInd);
    }

    fillColors.push_back(fillColor);
  }

  auto fillColor = CQChartsUtil::blendColors(fillColors);

  auto strokeColor = vennPlot_->interpStrokeColor(ColorInd());

  vennPlot_->setPenBrush(penBrush, vennPlot_->penData(strokeColor),
                         vennPlot_->brushData(fillColor));

  if (updateState)
    vennPlot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsCircleObj::
calcTextPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  auto textColor = vennPlot_->interpTextColor(colorInd);

  vennPlot_->setPen(penBrush, PenData(true, textColor, vennPlot_->textAlpha()));

  if (updateState)
    vennPlot_->updateObjPenBrushState(this, penBrush);
}

//------

CQChartsVennPlotCustomControls::
CQChartsVennPlotCustomControls(CQCharts *charts) :
 CQChartsPlotCustomControls(charts, "venn")
{
}

void
CQChartsVennPlotCustomControls::
init()
{
  addWidgets();

  addOverview();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsVennPlotCustomControls::
addWidgets()
{
  addColumnWidgets();

  addColorColumnWidgets("Text Color");
}

void
CQChartsVennPlotCustomControls::
addColumnWidgets()
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  //---

  // value and count columns
  addNamedColumnWidgets(QStringList() << "name" << "value", columnsFrame);
}

void
CQChartsVennPlotCustomControls::
connectSlots(bool b)
{
  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsVennPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_ && vennPlot_)
    disconnect(vennPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  vennPlot_ = dynamic_cast<CQChartsVennPlot *>(plot);

  CQChartsPlotCustomControls::setPlot(plot);

  if (vennPlot_)
    connect(vennPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
}

void
CQChartsVennPlotCustomControls::
updateWidgets()
{
  CQChartsPlotCustomControls::updateWidgets();
}

CQChartsColor
CQChartsVennPlotCustomControls::
getColorValue()
{
  return vennPlot_->textColor();
}

void
CQChartsVennPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  vennPlot_->setTextColor(c);
}
