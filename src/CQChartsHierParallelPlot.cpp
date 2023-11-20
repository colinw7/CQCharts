#include <CQChartsHierParallelPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsUtil.h>
#include <CQChartsTip.h>
#include <CQCharts.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsPlotParameterEdit.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQTclUtil.h>
#include <CQPerfMonitor.h>
#include <CMathRound.h>

#include <QApplication>
#include <QMenu>
#include <QVBoxLayout>

CQChartsHierParallelPlotType::
CQChartsHierParallelPlotType()
{
}

void
CQChartsHierParallelPlotType::
addParameters()
{
  CQChartsHierPlotType::addHierParameters("Parallel");

  //---

  startParameterGroup("Parallel");

  addEnumParameter("orientation", "Orientation", "orientation").
    addNameValue("HORIZONTAL", static_cast<int>(Qt::Horizontal)).
    addNameValue("VERTICAL"  , static_cast<int>(Qt::Vertical  )).
    setTip("Draw orientation");

  addBoolParameter("normalized", "Normalized", "normalized").
    setPropPath("options.normalized").setTip("Normalize each depth range");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsHierParallelPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
    h2("Parallel Plot").
     h3("Summary").
      p("Draws lines through values of multiple column values for each row.").
     h3("Columns").
      p("An axis is drawn for each y column using a label from the x column").
     h3("Limitations").
      p("None.").
     h3("Example").
      p(IMG("images/parallelplot.png"));
}

void
CQChartsHierParallelPlotType::
analyzeModel(ModelData *, AnalyzeModelData &)
{
}

CQChartsPlot *
CQChartsHierParallelPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsHierParallelPlot(view, model);
}

//---

CQChartsHierParallelPlot::
CQChartsHierParallelPlot(View *view, const ModelP &model) :
 CQChartsHierPlot(view, view->charts()->plotType("hierparallel"), model),
 CQChartsObjLineData <CQChartsHierParallelPlot>(this),
 CQChartsObjPointData<CQChartsHierParallelPlot>(this)
{
}

CQChartsHierParallelPlot::
~CQChartsHierParallelPlot()
{
  CQChartsHierParallelPlot::term();

  delete root_;
}

//---

void
CQChartsHierParallelPlot::
init()
{
  CQChartsHierPlot::init();

  //---

  NoUpdate noUpdate(this);

  setLinesColor(Color::makePalette());

  setPoints(true);

  setSymbol(Symbol::circle());

  setSymbolStrokeAlpha(Alpha(0.25));
  setSymbolFilled     (true);
  setSymbolFillColor  (Color::makePalette());
  setSymbolFillAlpha  (Alpha(0.5));

  mainXAxis_ = std::make_unique<CQChartsAxis>(this, Qt::Horizontal, 0.0, 1.0);
  mainYAxis_ = std::make_unique<CQChartsAxis>(this, Qt::Vertical  , 0.0, 1.0);

  //--

  //addKey(); TODO

  addTitle();

  //---

  addColorMapKey();
}

void
CQChartsHierParallelPlot::
term()
{
}

//---

CQChartsColumn
CQChartsHierParallelPlot::
getNamedColumn(const QString &name) const
{
  return CQChartsHierPlot::getNamedColumn(name);
}

void
CQChartsHierParallelPlot::
setNamedColumn(const QString &name, const Column &c)
{
  return CQChartsHierPlot::setNamedColumn(name, c);
}

CQChartsColumns
CQChartsHierParallelPlot::
getNamedColumns(const QString &name) const
{
  return CQChartsHierPlot::getNamedColumns(name);
}

void
CQChartsHierParallelPlot::
setNamedColumns(const QString &name, const Columns &c)
{
  CQChartsHierPlot::setNamedColumns(name, c);
}

//---

void
CQChartsHierParallelPlot::
setHorizontal(bool b)
{
  setOrientation(b ? Qt::Horizontal : Qt::Vertical);
}

void
CQChartsHierParallelPlot::
setOrientation(const Qt::Orientation &orientation)
{
  CQChartsUtil::testAndSet(orientation_, orientation, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsHierParallelPlot::
setNormalized(bool b)
{
  CQChartsUtil::testAndSet(normalized_, b, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

//---

QString
CQChartsHierParallelPlot::
labels() const
{
  return CQTcl::mergeList(labels_);
}

void
CQChartsHierParallelPlot::
setLabels(const QString &s)
{
  if (s != labels()) {
    QStringList strs;
    if (! CQTcl::splitList(s, strs))
      return;

    labels_ = strs;

    updateRangeAndObjs(); Q_EMIT customDataChanged();
  }
}

//---

void
CQChartsHierParallelPlot::
setLinesSelectable(bool b)
{
  CQChartsUtil::testAndSet(linesSelectable_, b, [&]() { drawObjs(); } );
}

void
CQChartsHierParallelPlot::
setLinesNodeColored(bool b)
{
  CQChartsUtil::testAndSet(linesNodeColored_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsHierParallelPlot::
setRootVisible(bool b)
{
  CQChartsUtil::testAndSet(rootVisible_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsHierParallelPlot::
setAxisLabelPos(const AxisLabelPos &p)
{
  CQChartsUtil::testAndSet(axisLabelPos_, p, [&]() { drawObjs(); } );
}

void
CQChartsHierParallelPlot::
setAxisLocal(bool b)
{
  CQChartsUtil::testAndSet(axisLocal_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsHierParallelPlot::
setAxisSpread(bool b)
{
  CQChartsUtil::testAndSet(axisSpread_, b, [&]() { updateRangeAndObjs(); } );
}

//------

void
CQChartsHierParallelPlot::
addProperties()
{
  addHierProperties();

  // options
  addProp("options", "orientation", "", "Draw horizontal or vertical");
  addProp("options", "normalized" , "", "Is data normalized");
  addProp("options", "labels"     , "", "Depth labels");

  // points
  addProp("points", "points", "visible", "Points visible");

  addSymbolProperties("points/symbol", "", "Points");

  // lines
  addProp("lines", "lines"           , "visible"    , "Lines visible");
  addProp("lines", "linesSelectable" , "selectable" , "Lines selectable");
  addProp("lines", "linesNodeColored", "nodeColored", "Lines colored from nodes");

  addLineProperties("lines/stroke", "lines", "");

  //---

  // axes
  auto addAxisProp = [&](const QString &path, const QString &name, const QString &alias,
                         const QString &desc, bool hidden=false) {
    auto *item = propertyModel()->addProperty(path, mainYAxis_.get(), name, alias);
    item->setDesc(desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  auto addAxisStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                              const QString &desc, bool hidden=false) {
    auto *item = addAxisProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  addProp("root", "rootVisible", "visible", "Root is visible");

  addProp("axis", "axisLabelPos", "labelPos", "Axis label position");
  addProp("axis", "axisLocal"   , "local"   , "Local axis for each range");
  addProp("axis", "axisSpread"  , "spread"  , "Spread axis to global range");

  auto linePath = QString("axis/stroke");

  addAxisStyleProp(linePath, "axesLineData"  , "style"  , "Axis stroke style", true);
  addAxisStyleProp(linePath, "axesLines"     , "visible", "Axis stroke visible");
  addAxisStyleProp(linePath, "axesLinesColor", "color"  , "Axis stroke color");
  addAxisStyleProp(linePath, "axesLinesAlpha", "alpha"  , "Axis stroke alpha");
  addAxisStyleProp(linePath, "axesLinesWidth", "width"  , "Axis stroke width");
  addAxisStyleProp(linePath, "axesLinesDash" , "dash"   , "Axis stroke dash");
  addAxisStyleProp(linePath, "axesLinesCap"  , "cap"    , "Axis stroke cap");
//addAxisStyleProp(linePath, "axesLinesJoin" , "join"   , "Axis stroke join");

  addAxisProp("axis/grid", "gridLinesDisplayed", "visible", "Axis grid visible");

  //---

  auto ticksPath = QString("axis/ticks");

  addAxisProp(ticksPath, "ticksDisplayed", "lines", "Axis major and/or minor ticks visible");

  auto majorTicksPath = ticksPath + "/major";
  auto minorTicksPath = ticksPath + "/minor";

  addAxisProp(majorTicksPath, "majorTickLen", "length", "Axis major ticks pixel length");
  addAxisProp(minorTicksPath, "minorTickLen", "length", "Axis minor ticks pixel length");

  //---

  auto ticksLabelPath     = ticksPath + "/label";
  auto ticksLabelTextPath = ticksLabelPath + "/text";

  addAxisProp(ticksLabelPath, "tickLabelAutoHide" , "autoHide",
              "Axis tick label text is auto hide");
  addAxisProp(ticksLabelPath, "tickLabelPlacement", "placement",
              "Axis tick label text placement");

  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextData"         , "style",
                   "Axis tick label text style", true);
  addAxisProp     (ticksLabelTextPath, "axesTickLabelTextVisible"      , "visible",
                   "Axis tick label text visible");
  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextColor"        , "color",
                   "Axis tick label text color");
  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextAlpha"        , "alpha",
                   "Axis tick label text alpha");
  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextFont"         , "font",
                   "Axis tick label text font");
  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextAngle"        , "angle",
                   "Axis tick label text angle");
  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextContrast"     , "contrast",
                   "Axis tick label text contrast");
  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextContrastAlpha", "contrastAlpha",
                   "Axis tick label text contrast alpha");
  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextClipLength"   , "clipLength",
                   "Axis tick label text clip length");
  addAxisStyleProp(ticksLabelTextPath, "axesTickLabelTextClipElide"    , "clipElide",
                   "Axis tick label text clip elide");

  //---

  auto labelPath     = QString("axis/label");
  auto labelTextPath = labelPath + "/text";

  addAxisStyleProp(labelTextPath, "axesLabelTextData"         , "style",
                   "Axis label text style", true);
  addAxisProp     (labelTextPath, "axesLabelTextVisible"      , "visible",
                   "Axis label text visible");
  addAxisStyleProp(labelTextPath, "axesLabelTextColor"        , "color",
                   "Axis label text color");
  addAxisStyleProp(labelTextPath, "axesLabelTextAlpha"        , "alpha",
                   "Axis label text alpha");
  addAxisStyleProp(labelTextPath, "axesLabelTextFont"         , "font",
                   "Axis label text font");
  addAxisStyleProp(labelTextPath, "axesLabelTextContrast"     , "contrast",
                   "Axis label text contrast");
  addAxisStyleProp(labelTextPath, "axesLabelTextContrastAlpha", "contrastAlpha",
                   "Axis label text contrast alpha");
  addAxisStyleProp(labelTextPath, "axesLabelTextClipLength"   , "clipLength",
                   "Axis label text clip length");
  addAxisStyleProp(labelTextPath, "axesLabelTextClipElide"    , "clipElide",
                   "Axis label text clip elide");

//mainYAxis_->addProperties(propertyModel(), "axis");

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
}

//---

void
CQChartsHierParallelPlot::
initRange()
{
  if (! currentModelData())
    axes_.clear();
}

CQChartsGeom::Range
CQChartsHierParallelPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsHierParallelPlot::calcRange");

  //---

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsHierParallelPlot *>(this);

  th->clearErrors();

  //---

  // check columns
  bool columnsValid = true;

  if (! checkColumns      (nameColumns(), "Name" )) columnsValid = false;
  if (! checkNumericColumn(valueColumn(), "Value")) columnsValid = false;

  if (! columnsValid) {
    auto dataRange = Range(0.0, 0.0, 1.0, 1.0);

    th->dataRange_           = dataRange;
    th->normalizedDataRange_ = dataRange;

    return dataRange;
  }

  //---

  // calc range for each value column (set)
  class RowVisitor : public ModelVisitor {
   public:
    using HierParallelPlot = CQChartsHierParallelPlot;
    using Node             = CQChartsHierParallelNode;

   public:
    RowVisitor(const HierParallelPlot *parallelPlot, Node *root) :
     parallelPlot_(parallelPlot) {
      nodeStack_.push_back(root);
    }

    State hierVisit(const QAbstractItemModel *, const VisitData &data) override {
      QStringList names;
      QModelIndex nameInd;

      (void) getNames(data, names, nameInd);

      //---

      if (names.length() > 0) {
        Node *parentNode = this->parentNode();

        auto name = names[0];

        auto *childNode = parentNode->getChild(name);

        if (! childNode)
          childNode = parallelPlot_->addNode(parentNode, name, nameInd);

        nodeStack_.push_back(childNode);
      }

      return State::OK;
    }

    State hierPostVisit(const QAbstractItemModel *, const VisitData &) override {
      nodeStack_.pop_back();

      assert(! nodeStack_.empty());

      return State::OK;
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      QStringList names;
      QModelIndex nameInd;

      (void) getNames(data, names, nameInd);

      //---

      QModelIndex valueInd;

      ModelIndex valueModelInd(parallelPlot_, data.row, parallelPlot_->valueColumn(),
                               data.parent);

      valueInd = parallelPlot_->modelIndex(valueModelInd);

      bool ok;
      auto value = parallelPlot_->modelReal(valueModelInd, ok);
      if (! ok) return State::SKIP;

      //---

      Node *parentNode = this->parentNode();

      for (const auto &name : names) {
        auto *childNode = parentNode->getChild(name);

        if (! childNode)
          childNode = parallelPlot_->addNode(parentNode, name, nameInd);

        parentNode = childNode;
      }

      parentNode->setValue(value);

      return State::OK;
    }

   private:
    Node *parentNode() const {
      assert(! nodeStack_.empty());

      return nodeStack_.back();
    }

    bool getNames(const VisitData &data, QStringList &names, QModelIndex &nameInd) const {
      ModelIndex nameModelInd(parallelPlot_, data.row, parallelPlot_->nameColumns().column(),
                              data.parent);

      nameInd = parallelPlot_->modelIndex(nameModelInd);

      bool ok;
      auto name = parallelPlot_->modelString(nameModelInd, ok);
      if (! ok) return false;

      //---

      // split name into hier path elements
      int pos = name.indexOf('/');

      while (pos != -1) {
        auto lhs = name.mid(0, pos);
        auto rhs = name.mid(pos + 1);

        names.push_back(lhs);

        name = rhs;

        pos = name.indexOf('/');
      }

      names.push_back(name);

      return true;
    }

   private:
    using NodeStack = std::vector<Node *>;

    const HierParallelPlot* parallelPlot_ { nullptr };
    NodeStack               nodeStack_;
  };

  //---

  delete root_;

  root_ = new Node(this, nullptr, QString());

  RowVisitor visitor(this, root_);

  visitModel(visitor);

  depth_ = root_->childDepth() + 1;

  //---

  // update axes to match columns
  th->updateAxes();

  //---

  // set range from data
  th->dataRange_ = Range();

  int minDepth = (isRootVisible() ? 0 : 1);
  int maxDepth = depth_;

  th->depthRanges_.clear();

  CQChartsHierParallelNode::ValuesArray valuesArray;

  root_->valueArrays(valuesArray);

  for (const auto &values : valuesArray) {
    for (const auto &pv : values) {
      auto depth = pv.node->parentDepth();

      if (depth == 0 && ! isRootVisible())
        continue;

      auto &range = th->depthRanges_[depth];

      range.add(pv.value);
    }
  }

  for (int depth = minDepth; depth < maxDepth; ++depth) {
    auto &range = th->depthRanges_[depth];
    if (! range.isSet()) continue;

    auto updateRange = [&](double x, double y) {
      if (isVertical())
        th->dataRange_.updateRange(x, y);
      else
        th->dataRange_.updateRange(y, x);
    };

    updateRange(minDepth - 0.5, range.min());
    updateRange(maxDepth - 0.5, range.max());
  }

  //---

  // set plot range
  Range dataRange;

  auto updateRange = [&](double x, double y) {
    if (isVertical())
      dataRange.updateRange(x, y);
    else
      dataRange.updateRange(y, x);
  };

  updateRange(minDepth - 0.5, 0.0);
  updateRange(maxDepth - 0.5, 1.0);

  th->normalizedDataRange_ = dataRange;

  //---

  // set axes range and name
  if (isAxisLocal()) {
    for (int depth = minDepth; depth < maxDepth; ++depth) {
      auto *axis = this->axis(depth);

      const auto &range = depthRange(depth);
      if (! range.isSet()) continue;

      if (isVertical()) {
        if (isNormalized()) {
          axis->setRange(0.0, 1.0);
          axis->setValueRange(range.min(), range.max());
        }
        else {
          if (! isAxisSpread()) {
            axis->setRange     (range.min(), range.max());
            axis->setValueRange(range.min(), range.max());
          }
          else {
            axis->setRange     (dataRange_.ymin(), dataRange_.ymax());
            axis->setValueRange(dataRange_.ymin(), dataRange_.ymax());
          }
        }
      }
      else {
        if (isNormalized()) {
          axis->setRange(0.0, 1.0);
          axis->setValueRange(range.min(), range.max());
        }
        else {
          if (! isAxisSpread()) {
            axis->setRange     (range.min(), range.max());
            axis->setValueRange(range.min(), range.max());
          }
          else {
            axis->setRange     (dataRange_.xmin(), dataRange_.xmax());
            axis->setValueRange(dataRange_.xmin(), dataRange_.xmax());
          }
        }
      }
    }
  }
  else {
    if (isVertical()) {
      mainXAxis_->setRange(minDepth, maxDepth);
      mainYAxis_->setRange(dataRange_.ymin(), dataRange_.ymax());

      mainXAxis_->setValueType(CQChartsAxisValueType::integer());
      mainYAxis_->setValueType(CQChartsAxisValueType::real   ());
    }
    else {
      mainXAxis_->setRange(dataRange_.xmin(), dataRange_.xmax());
      mainYAxis_->setRange(minDepth, maxDepth);

      mainXAxis_->setValueType(CQChartsAxisValueType::real   ());
      mainYAxis_->setValueType(CQChartsAxisValueType::integer());
    }

    //---

    auto *axis = this->axis(0);

    if (isNormalized())
      axis->setRange(0.0, 1.0);
    else {
      if (isVertical())
        axis->setRange(dataRange_.ymin(), dataRange_.ymax());
      else
        axis->setRange(dataRange_.xmin(), dataRange_.xmax());
    }
  }

  //---

  if (isNormalized())
    return normalizedDataRange_;
  else
    return dataRange_;
}

CQChartsHierParallelNode *
CQChartsHierParallelPlot::
addNode(Node *parent, const QString &name, const QModelIndex &nameInd) const
{
  auto nameInd1 = normalizeIndex(nameInd);

  auto *node = new Node(this, parent, name);

  node->setModelInd(nameInd1);

  return node;
}

void
CQChartsHierParallelPlot::
updateAxes()
{
  if (isVertical()) {
    mainXAxis_->setDirection(Qt::Horizontal);
    mainYAxis_->setDirection(Qt::Vertical);
  }
  else {
    mainXAxis_->setDirection(Qt::Vertical);
    mainYAxis_->setDirection(Qt::Horizontal);
  }

  //---

  // create axes
  int maxDepth = depth_;

  auto adir = orientation();

  if (int(axes_.size()) != maxDepth || adir_ != adir) {
    adir_ = adir;

    axes_.clear();

    for (int depth = 0; depth < maxDepth; ++depth) {
      auto *axis = new CQChartsAxis(this, adir_, 0, 1);

      axis->moveToThread(this->thread());

      axis->setParent(this);
      axis->setPlot  (this);

      axis->setUpdatesEnabled(false);

      axes_.push_back(AxisP(axis));
    }
  }
}

//------

bool
CQChartsHierParallelPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsHierParallelPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

//auto *th = const_cast<CQChartsHierParallelPlot *>(this);

  double sx, sy;
  plotSymbolSize(symbolSize(), sx, sy);

  //---

  CQChartsHierParallelNode::ValuesArray valuesArray;

  root_->valueArrays(valuesArray);

  // create polyline for each value array
  struct PolyNodes {
    Polygon             poly;
    std::vector<Node *> nodes;
  };

  using Polygons = std::vector<PolyNodes>;

  Polygons polygons;

  for (const auto &values : valuesArray) {
    PolyNodes polyNodes;

    for (const auto &pv : values) {
      auto depth = pv.node->parentDepth();

      if (depth == 0 && ! isRootVisible())
        continue;

      Point p;

      if (isVertical())
        p = Point(depth, pv.value);
      else
        p = Point(pv.value, depth);

      polyNodes.poly .addPoint(p);
      polyNodes.nodes.push_back(pv.node);
    }

    polygons.push_back(polyNodes);
  }

  //---

  int i = 0;
  int n = polygons.size();

  for (const auto &polyNodes : polygons) {
    BBox bbox;

    if (isNormalized())
      bbox = BBox(normalizedDataRange_.xmin(), normalizedDataRange_.ymin(),
                  normalizedDataRange_.xmax(), normalizedDataRange_.ymax());
    else
      bbox = BBox(dataRange_.xmin(), dataRange_.ymin(),
                  dataRange_.xmax(), dataRange_.ymax());

    ColorInd is(i, n);

    QModelIndex xind;

    auto *lineObj = createLineObj(bbox, i, polyNodes.poly, is);

    for (auto *node : polyNodes.nodes) {
      if (node->modelInd().isValid())
        lineObj->addModelInd(node->modelInd());
    }

    lineObj->connectDataChanged(this, SLOT(updateSlot()));

    objs.push_back(lineObj);

    ++i;
  }

  //---

  using NodeSet = std::set<Node *>;

  NodeSet nodeSet;

  i = 0;
  n = valuesArray.size();

  for (const auto &values : valuesArray) {
    for (const auto &pv : values) {
      auto pn = nodeSet.find(pv.node);
      if (pn != nodeSet.end()) continue;

      nodeSet.insert(pv.node);

      //---

      auto depth = pv.node->parentDepth();

      if (depth == 0 && ! isRootVisible())
        continue;

      //---

      Point p;

      if (isVertical())
        p = Point(depth, pv.value);
      else
        p = Point(pv.value, depth);

      // create point object for value
      auto range = depthRange(depth);
      if (! range.isSet()) continue;

      auto pos  = (isVertical() ? p.y : p.x);
      auto pos1 = pos;

      if (isNormalized())
        pos1 = range.normalize(pos);

      double x, x1, y, y1;

      if (isVertical()) {
        x = depth; x1 = x; y = pos; y1 = pos1;
      }
      else {
        x = pos; x1 = pos1; y = depth; y1 = y;
      }

      BBox bbox(x1 - sx/2, y1 - sy/2, x1 + sx/2, y1 + sy/2);

      auto name     = pv.node->name();
      auto hierName = pv.node->hierName();

      ColorInd is(i, n);
      ColorInd iv(depth, depth_);

      auto ind = pv.node->modelInd();

      auto *pointObj = createPointObj(bbox, name, hierName, depth, pos, Point(x, y), ind, is, iv);

      pointObj->connectDataChanged(this, SLOT(updateSlot()));

      objs.push_back(pointObj);
    }

    ++i;
  }

  //---

  return true;
}

bool
CQChartsHierParallelPlot::
probe(ProbeData &probeData) const
{
  int n = 1;

  if (isVertical()) {
    int x = CMathRound::RoundNearest(probeData.p.x);

    x = std::min(std::max(x, 0), n - 1);

    auto range = depthRange(x);
    if (! range.isSet()) return false;

    probeData.p.x = x;

    probeData.yvals.emplace_back(probeData.p.y, "",
      QString::number(probeData.p.y*(range.max() - range.min()) + range.min()));
  }
  else {
    int y = CMathRound::RoundNearest(probeData.p.y);

    y = std::min(std::max(y, 0), n - 1);

    auto range = depthRange(y);
    if (! range.isSet()) return false;

    probeData.p.y = y;

    probeData.xvals.emplace_back(probeData.p.x, "",
      QString::number(probeData.p.x*(range.max() - range.min()) + range.min()));
  }

  return true;
}

//------

bool
CQChartsHierParallelPlot::
addMenuItems(QMenu *menu, const Point &)
{
  menu->addSeparator();

  CQUtil::addCheckedAction(menu, "Horizontal", isHorizontal(), this, SLOT(setHorizontal(bool)));

  //---

  if (canDrawColorMapKey())
    addColorMapKeyItems(menu);

  //---

  addRootMenuItems(menu);

  return true;
}

//---

void
CQChartsHierParallelPlot::
redrawAxis(CQChartsAxis *, bool wait)
{
  if (wait)
    drawObjs();
  else
    invalidateLayers();
}

CQChartsGeom::BBox
CQChartsHierParallelPlot::
axesFitBBox() const
{
  return axesBBox_;
}

CQChartsGeom::BBox
CQChartsHierParallelPlot::
calcExtraFitBBox() const
{
  CQPerfTrace trace("CQChartsHierParallelPlot::calcExtraFitBBox");

  auto range = (isNormalized() ? normalizedDataRange_ : dataRange_);
  if (! range.isSet()) return BBox();

  //--

  double tst = 0.0, tsb = 0.0;

  if (labels_.length()) {
    auto font = view()->plotFont(this, view()->font());

    QFontMetricsF fm(font);

    double tm = 4.0;

    double ts;

    if (isVertical())
      ts = pixelToWindowHeight(fm.height() + tm);
    else
      ts = pixelToWindowWidth(max_tw_ + tm);

    if (axisLabelPos() == AxisLabelPos::TOP    || axisLabelPos() == AxisLabelPos::ALTERNATE)
      tst = ts;
    if (axisLabelPos() == AxisLabelPos::BOTTOM || axisLabelPos() == AxisLabelPos::ALTERNATE)
      tsb = ts;
  }

  //---

  BBox bbox;

  if (isVertical())
    bbox = BBox(range.xmin(), range.ymin() - tsb, range.xmax(), range.ymax() + tst);
  else
    bbox = BBox(range.xmin() - tsb, range.ymin(), range.xmax() + tst, range.ymax());

  return bbox;
}

//---

bool
CQChartsHierParallelPlot::
hasFgAxes() const
{
  return true;
}

void
CQChartsHierParallelPlot::
drawFgAxes(PaintDevice *device) const
{
  if (axes_.empty())
    return;

  //--

  auto *th = const_cast<CQChartsHierParallelPlot *>(this);

  //---

  th->axesBBox_ = BBox();

  th->max_tw_ = 0.0;

  double tm = 4.0;

  double sx, sy;
  plotSymbolSize(symbolSize(), sx, sy);

  if (isVertical())
    tm = std::max(tm, sy);
  else
    tm = std::max(tm, sx);

  // draw axes
  int minDepth = (isRootVisible() ? 0 : 1);
  int maxDepth = depth_;

  if (isAxisLocal()) {
    for (int depth = minDepth; depth < maxDepth; ++depth) {
      assert(depth < int(axes_.size()));

      auto *axis = this->axis(depth);

      axis->setAxesLineData         (mainYAxis_->axesLineData());
      axis->setAxesLabelTextData    (mainYAxis_->axesLabelTextData());
      axis->setAxesTickLabelTextData(mainYAxis_->axesTickLabelTextData());

      axis->setAxesLabelTextVisible(axisLabelPos() == AxisLabelPos::AXIS);

      setPainterFont(device, mainYAxis_->axesLabelTextFont());

      //---

      const auto &range = depthRange(depth);
      if (! range.isSet()) continue;

      //---

      // draw depth axis
      axis->setPosition(CQChartsOptReal(depth));

      if (depth == minDepth && ! isNormalized()) {
        if (mainYAxis_->gridLinesDisplayed() != CQChartsAxis::GridLinesDisplayed::NONE) {
          axis->setGridLinesDisplayed(mainYAxis_->gridLinesDisplayed());

          axis->setGridStart(OptReal(minDepth - 0.5));
          axis->setGridEnd  (OptReal(maxDepth - 0.5));

          axis->drawGrid(this, device);
        }
      }

      axis->draw(this, device);

      //---

      th->axesBBox_ += windowToPixel(axis->fitBBox());
    }
  }
  else {
    auto *axis = this->axis(0);

    axis->setAxesLineData         (mainYAxis_->axesLineData());
    axis->setAxesLabelTextData    (mainYAxis_->axesLabelTextData());
    axis->setAxesTickLabelTextData(mainYAxis_->axesTickLabelTextData());

    axis->setAxesLabelTextVisible(axisLabelPos() == AxisLabelPos::AXIS);

    axis      ->setGridLinesDisplayed(mainYAxis_->gridLinesDisplayed());
    mainXAxis_->setGridLinesDisplayed(mainYAxis_->gridLinesDisplayed());

    setPainterFont(device, mainYAxis_->axesLabelTextFont());

    //---

    // draw set axis
    axis->setPosition(CQChartsOptReal(minDepth - 0.5));

    if (mainYAxis_->gridLinesDisplayed() != CQChartsAxis::GridLinesDisplayed::NONE) {
      axis->setGridStart(OptReal(minDepth - 0.5));
      axis->setGridEnd  (OptReal(maxDepth - 0.5));

      axis->drawGrid(this, device);
    }

    if (mainYAxis_->gridLinesDisplayed() != CQChartsAxis::GridLinesDisplayed::NONE)
      mainXAxis_->drawGrid(this, device);

    axis->draw(this, device);

    //---

    th->axesBBox_ += windowToPixel(axis->fitBBox());
  }

  //---

  // draw axes labels
  for (int depth = minDepth; depth < maxDepth; ++depth) {
    assert(depth < int(axes_.size()));
    auto *axis = this->axis(depth);

    //---

    // draw axis labels
    if (axisLabelPos() != AxisLabelPos::AXIS) {
      auto axisLabelPos = this->axisLabelPos();
      auto axisLabelLen = Length::plot(1.0);

      if (axisLabelPos == AxisLabelPos::ALTERNATE) {
        axisLabelPos = (depth & 1 ? AxisLabelPos::BOTTOM : AxisLabelPos::TOP);
        axisLabelLen = Length::plot(2.0);
      }

      //---

      QString label;

      if (depth < labels_.length())
        label = labels_[depth];

      axis->setLabelStr(label);

      //---

      Point p;

      auto textRange = (isNormalized() ? normalizedDataRange_ : dataRange_);

      if (textRange.isSet()) {
        if (axisLabelPos == AxisLabelPos::TOP) {
          if (isVertical())
            p = windowToPixel(Point(depth, textRange.ymax()));
          else
            p = windowToPixel(Point(textRange.xmax(), depth));
        }
        else {
          if (isVertical())
            p = windowToPixel(Point(depth, textRange.ymin()));
          else
            p = windowToPixel(Point(textRange.xmin(), depth));
        }
      }

      //---

      QFontMetricsF fm(device->font());

      double tw = fm.horizontalAdvance(label);
      double ta = fm.ascent();
      double td = fm.descent();

      th->max_tw_ = std::max(max_tw_, tw);

      PenBrush tpenBrush;

      auto tc = mainYAxis_->interpAxesLabelTextColor(ColorInd());

      setPen(tpenBrush, PenData(true, tc, mainYAxis_->axesLabelTextAlpha()));

      device->setPen(tpenBrush.pen);

      Point tp;

      if (axisLabelPos == AxisLabelPos::TOP) {
        if (isVertical()) {
          if (! isInvertY())
            tp = Point(p.x - tw/2.0, p.y - td - tm);
          else
            tp = Point(p.x - tw/2.0, p.y + ta + tm);
        }
        else {
          if (! isInvertX())
            tp = Point(p.x + tm, p.y - (ta - td)/2);
          else
            tp = Point(p.x - tw - tm, p.y - (ta - td)/2);
        }
      }
      else {
        if (isVertical()) {
          if (! isInvertY())
            tp = Point(p.x - tw/2.0, p.y + ta + tm);
          else
            tp = Point(p.x - tw/2.0, p.y - td - tm);
        }
        else {
          if (! isInvertX())
            tp = Point(p.x - tw - tm, p.y - (ta - td)/2);
          else
            tp = Point(p.x + tm, p.y - (ta - td)/2);
        }
      }

      auto textOptions = mainYAxis_->axesLabelTextOptions();

      textOptions.angle = Angle();
      textOptions.align = Qt::AlignLeft;

      if (textOptions.clipLength <= 0)
        textOptions.clipLength = lengthPixelWidth(axisLabelLen);

      CQChartsDrawUtil::drawTextAtPoint(device, pixelToWindow(tp), label,
                                        textOptions, /*centered*/false);
    }
  }

  //---

  if (axesBBox_.isValid())
    th->axesBBox_ = pixelToWindow(axesBBox_);
}

bool
CQChartsHierParallelPlot::
hasOverlay() const
{
  return true;
}

void
CQChartsHierParallelPlot::
execDrawOverlay(PaintDevice *device) const
{
  for (const auto &plotObj : plotObjects()) {
    auto *pointObj = dynamic_cast<CQChartsHierParallelPointObj *>(plotObj);
    if (! pointObj) continue;

    if (pointObj->isInside())
      pointObj->drawOverlay(device);
  }
}

//---

CQChartsHierParallelLineObj *
CQChartsHierParallelPlot::
createLineObj(const BBox &rect, int ind, const Polygon &poly, const ColorInd &is) const
{
  return new CQChartsHierParallelLineObj(this, rect, ind, poly, is);
}

CQChartsHierParallelPointObj *
CQChartsHierParallelPlot::
createPointObj(const BBox &rect, const QString &name, const QString &hierName, int depth,
               double value, const Point &p, const QModelIndex &modelInd,
               const ColorInd &is, const ColorInd &iv) const
{
  return new CQChartsHierParallelPointObj(this, rect, name, hierName, depth,
                                          value, p, modelInd, is, iv);
}

//---

const CQChartsHierParallelPointObj *
CQChartsHierParallelPlot::
getModelPointObj(const QModelIndex &ind) const
{
  for (const auto *obj : plotObjects()) {
    const auto *pointObj = dynamic_cast<const PointObj *>(obj);

    if (pointObj && pointObj->modelInd() == ind)
      return pointObj;
  }

  return nullptr;
}

//---

bool
CQChartsHierParallelPlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsHierParallelPlot::
execDrawForeground(PaintDevice *device) const
{
  CQChartsHierPlot::execDrawForeground(device);
}

//---

CQChartsHierPlotCustomControls *
CQChartsHierParallelPlot::
createCustomControls()
{
  auto *controls = new CQChartsHierParallelPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsHierParallelLineObj::
CQChartsHierParallelLineObj(const CQChartsHierParallelPlot *parallelPlot, const BBox &rect,
                            int ind, const Polygon &poly, const ColorInd &is) :
 CQChartsPlotObj(const_cast<CQChartsHierParallelPlot *>(parallelPlot),
                 rect, is, ColorInd(), ColorInd()),
 parallelPlot_(parallelPlot), ind_(ind), poly_(poly)
{
  setDetailHint(DetailHint::MAJOR);
}

QString
CQChartsHierParallelLineObj::
calcId() const
{
  return QString("line%1").arg(ind_);
}

QString
CQChartsHierParallelLineObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  std::map<int, QString> depthName;

  for (const auto &ind : modelInds()) {
    auto *pointObj = parallelPlot_->getModelPointObj(ind);
    if (! pointObj) continue;

    depthName[pointObj->depth()] = pointObj->name();
  }

  QStringList names;

  for (const auto &pn : depthName)
    names.push_back(pn.second);

  tableTip.addTableRow("Path", names.join("/"));

  plot()->addTipColumns(tableTip, modelInd());

  return tableTip.str();
}

bool
CQChartsHierParallelLineObj::
isVisible() const
{
  if (! parallelPlot_->isLines())
    return false;

  return CQChartsPlotObj::isVisible();
}

bool
CQChartsHierParallelLineObj::
inside(const Point &p) const
{
  if (! isVisible())
    return false;

  if (! parallelPlot_->isLinesSelectable())
    return false;

  // TODO: check as lines created, only need to create lines close to point

  // create unnormalized polygon
  Polygon poly;

  getPolyLine(poly);

  // check if close enough to each line to be inside
  for (int i = 1; i < poly.size(); ++i) {
    double x1 = poly.point(i - 1).x;
    double y1 = poly.point(i - 1).y;
    double x2 = poly.point(i    ).x;
    double y2 = poly.point(i    ).y;

    double d;

    Point pl1(x1, y1);
    Point pl2(x2, y2);

    if (! CQChartsUtil::PointLineDistance(p, pl1, pl2, &d))
      continue;

    auto pdx = parallelPlot_->windowToPixelWidth (d);
    auto pdy = parallelPlot_->windowToPixelHeight(d);

    if (pdx < 4 || pdy < 4)
      return true;
  }

  return false;
}

void
CQChartsHierParallelLineObj::
getObjSelectIndices(Indices &) const
{
}

void
CQChartsHierParallelLineObj::
draw(PaintDevice *device) const
{
  // set pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // draw polyline (using unnormalized polygon)
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  polyLine_ = Polygon();

  getPolyLine(polyLine_);

  for (int i = 1; i < polyLine_.size(); ++i)
    device->drawLine(polyLine_.point(i - 1), polyLine_.point(i));

  device->resetColorNames();
}

void
CQChartsHierParallelLineObj::
drawOverlay(PaintDevice *device) const
{
  device->setPen(QPen(Qt::red));

  for (int i = 1; i < polyLine_.size(); ++i)
    device->drawLine(polyLine_.point(i - 1), polyLine_.point(i));
}

void
CQChartsHierParallelLineObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  parallelPlot_->setLineDataPen(penBrush.pen, colorInd);

  if (parallelPlot_->colorColumn().isValid() && parallelPlot_->isLinesNodeColored()) {
    double alpha = CQChartsDrawUtil::penAlpha(penBrush.pen);

    std::vector<QColor> colors;

    for (const auto &ind : modelInds()) {
      Color indColor;

      if (parallelPlot_->colorColumnColor(ind.row(), ind.parent(), indColor))
        colors.push_back(parallelPlot_->interpColor(indColor, colorInd));
    }

    auto c = CQChartsUtil::blendColors(colors);
    penBrush.pen.setColor(c);

    CQChartsDrawUtil::setPenAlpha(penBrush.pen, alpha);
  }

  parallelPlot_->setBrush(penBrush, BrushData(false));

  if (updateState)
    parallelPlot_->updateObjPenBrushState(this, penBrush, CQChartsHierPlot::DrawType::LINE);
}

void
CQChartsHierParallelLineObj::
getPolyLine(Polygon &poly) const
{
  // create normalized polyline
  if (parallelPlot_->isNormalized()) {
    for (int i = 0; i < poly_.size(); ++i) {
      auto p = poly_.point(i);

      double x, y;

      if (parallelPlot_->isVertical()) {
        const auto &range = parallelPlot_->depthRange(int(p.x));
        if (! range.isSet()) continue;

        x = p.x;
        y = range.normalize(p.y);
      }
      else {
        const auto &range = parallelPlot_->depthRange(int(p.y));
        if (! range.isSet()) continue;

        x = range.normalize(p.x);
        y = p.y;
      }

      poly.addPoint(Point(x, y));
    }
  }
  else {
    poly = poly_;
  }
}

bool
CQChartsHierParallelLineObj::
hasIndex(const QModelIndex &ind) const
{
  for (const auto &ind1 : modelInds()) {
    if (ind == ind1)
      return true;
  }

  return false;
}

//------

CQChartsHierParallelPointObj::
CQChartsHierParallelPointObj(const HierParallelPlot *parallelPlot, const BBox &rect,
                             const QString &name, const QString &hierName, int depth,
                             double value, const Point &p, const QModelIndex &modelInd,
                             const ColorInd &is, const ColorInd &iv) :
 CQChartsPlotPointObj(const_cast<HierParallelPlot *>(parallelPlot), rect, p, is, ColorInd(), iv),
 parallelPlot_(parallelPlot), name_(name), hierName_(hierName), depth_(depth),
 value_(value), point_(p)
{
  p_ = calcPoint();

  if (modelInd.isValid())
    setModelInd(modelInd);
}

//---

CQChartsLength
CQChartsHierParallelPointObj::
calcSymbolSize() const
{
  return parallelPlot()->symbolSize();
}

//---

QString
CQChartsHierParallelPointObj::
calcId() const
{
  auto name = this->hierName();
  if (name == "") name = "root";

  return name;
}

QString
CQChartsHierParallelPointObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  auto hierName = this->hierName();

  tableTip.addBoldLine(hierName);

  tableTip.addTableRow("Name" , name());
  tableTip.addTableRow("Value", value_);

  //---

  parallelPlot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

bool
CQChartsHierParallelPointObj::
isVisible() const
{
  if (! parallelPlot_->isPoints())
    return false;

  return CQChartsPlotPointObj::isVisible();
}

//---

void
CQChartsHierParallelPointObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, Column(modelInd().column()));
}

//---

void
CQChartsHierParallelPointObj::
draw(PaintDevice *device) const
{
  auto symbol = parallelPlot()->symbol();

  if (! symbol.isValid())
    return;

  //---

  // get symbol size
  double sx, sy;

  calcSymbolPixelSize(sx, sy, /*square*/false, /*enforceMinSize*/false);

  //---

  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // draw symbol
  auto p = calcPoint();

  plot()->drawSymbol(device, p, symbol, sx, sy, penBrush, /*scaled*/false);
}

void
CQChartsHierParallelPointObj::
drawOverlay(PaintDevice *device) const
{
  for (const auto &plotObj : parallelPlot_->plotObjects()) {
    auto *lineObj = dynamic_cast<CQChartsHierParallelLineObj *>(plotObj);
    if (! lineObj) continue;

    if (lineObj->hasIndex(modelInd()))
      lineObj->drawOverlay(device);
  }
}

CQChartsGeom::Point
CQChartsHierParallelPointObj::
calcPoint() const
{
  // draw symbol
  auto p = point_;

  if (parallelPlot_->isNormalized()) {
    const auto &range = parallelPlot_->depthRange(depth_);

    if (range.isSet()) {
      double x, y;

      if (parallelPlot_->isVertical()) {
        x = p.x;
        y = range.normalize(p.y);
      }
      else {
        x = range.normalize(p.x);
        y = p.y;
      }

      p = Point(x, y);
    }
  }

  return p;
}

void
CQChartsHierParallelPointObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  parallelPlot()->setSymbolPenBrush(penBrush, colorInd);

  if (parallelPlot_->colorColumn().isValid() &&
      parallelPlot_->colorType() == CQChartsHierPlot::ColorType::AUTO) {
    auto ind1 = modelInd();

    Color indColor;

    if (parallelPlot_->colorColumnColor(ind1.row(), ind1.parent(), indColor)) {
      double alpha = CQChartsDrawUtil::brushAlpha(penBrush.brush);

      penBrush.brush.setColor(parallelPlot_->interpColor(indColor, colorInd));

      CQChartsDrawUtil::setBrushAlpha(penBrush.brush, alpha);
    }
  }

  if (updateState)
    plot()->updateObjPenBrushState(this, penBrush, drawType());
}

//------

CQChartsHierParallelPlotCustomControls::
CQChartsHierParallelPlotCustomControls(CQCharts *charts) :
 CQChartsHierPlotCustomControls(charts, "hierparallel")
{
}

void
CQChartsHierParallelPlotCustomControls::
init()
{
  addWidgets();

  addOverview();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsHierParallelPlotCustomControls::
addWidgets()
{
  addHierColumnWidgets();

  addOptionsWidgets();

  addColorColumnWidgets();
}

void
CQChartsHierParallelPlotCustomControls::
addColumnWidgets()
{
}

void
CQChartsHierParallelPlotCustomControls::
addOptionsWidgets()
{
  // options group
  optionsFrame_ = createGroupFrame("Options", "optionsFrame");

  //---

  orientationCombo_ = createEnumEdit("orientation");
  normalizedCheck_  = createBoolEdit("normalized", /*choice*/false);

  addFrameWidget(optionsFrame_, "Orientation", orientationCombo_);

  addFrameColWidget(optionsFrame_, normalizedCheck_);

  //addFrameRowStretch(optionsFrame_);

  // lines selectable
}

void
CQChartsHierParallelPlotCustomControls::
connectSlots(bool b)
{
  CQUtil::optConnectDisconnect(b,
    orientationCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(orientationSlot()));
  CQUtil::optConnectDisconnect(b,
    normalizedCheck_, SIGNAL(stateChanged(int)), this, SLOT(normalizedSlot()));

  CQChartsHierPlotCustomControls::connectSlots(b);
}

void
CQChartsHierParallelPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_ && parallelPlot_)
    disconnect(parallelPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  parallelPlot_ = dynamic_cast<CQChartsHierParallelPlot *>(plot);

  CQChartsHierPlotCustomControls::setPlot(plot);

  if (parallelPlot_)
    connect(parallelPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
}

void
CQChartsHierParallelPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  orientationCombo_->setCurrentValue(static_cast<int>(parallelPlot_->orientation()));

  normalizedCheck_->setChecked(parallelPlot_->isNormalized());

  //---

  connectSlots(true);

  //---

  CQChartsHierPlotCustomControls::updateWidgets();
}

void
CQChartsHierParallelPlotCustomControls::
orientationSlot()
{
  parallelPlot_->setOrientation(static_cast<Qt::Orientation>(orientationCombo_->currentValue()));
}

void
CQChartsHierParallelPlotCustomControls::
normalizedSlot()
{
  parallelPlot_->setNormalized(normalizedCheck_->isChecked());
}

CQChartsColor
CQChartsHierParallelPlotCustomControls::
getColorValue()
{
  return parallelPlot_->symbolFillColor();
}

void
CQChartsHierParallelPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  parallelPlot_->setSymbolFillColor(c);
}
