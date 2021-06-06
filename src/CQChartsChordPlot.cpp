#include <CQChartsChordPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsRotatedTextBoxObj.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsNamePair.h>
#include <CQChartsConnectionList.h>
#include <CQChartsTip.h>
#include <CQChartsValueSet.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQChartsArrow.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

//---

CQChartsChordPlotType::
CQChartsChordPlotType()
{
}

void
CQChartsChordPlotType::
addParameters()
{
  CQChartsConnectionPlotType::addParameters();
}

QString
CQChartsChordPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Chord Plot").
    h3("Summary").
     p("Draw connections using radial plot with sized path arcs.").
     p("The size of each arc is equivalent to the number of connections from that section.").
    h3("Columns").
     p("The link column specifies the node name and index (row) and the group column "
       "specifies the parent group. The remaining columns contain the connection value for "
       "each connected node i.e. there should be N rows and N + 1 or N + 2 columns depending "
       "on whether a group is specified").
    h3("Options").
     p("The inner radius (0-1) can be specified to adjust the width of the ring and connection "
       "area. The radius for the label can be specified ((0-1) inside, >1 outside) and the "
       "nodes can be sorted by value or use the original model order").
    h3("Customization").
     p("The stroke style, segment fill alpha, and arc fill alpha can be specified. The start angle "
       "and gap between nodes (in degress) can be specified. The label text style can be "
       "specified.").
    h3("Limitations").
     p("A user defined range cannot be specified, no axes or key is supported, logarithmic "
       "values are not allowed and probing is not available.").
    h3("Example").
     p(IMG("images/chord_plot.png"));
}

bool
CQChartsChordPlotType::
isColumnForParameter(ColumnDetails *columnDetails, Parameter *parameter) const
{
  return CQChartsConnectionPlotType::isColumnForParameter(columnDetails, parameter);
}

void
CQChartsChordPlotType::
analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData)
{
  CQChartsConnectionPlotType::analyzeModel(modelData, analyzeModelData);
}

CQChartsPlot *
CQChartsChordPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsChordPlot(view, model);
}

//------

CQChartsChordPlot::
CQChartsChordPlot(CQChartsView *view, const ModelP &model) :
 CQChartsConnectionPlot(view, view->charts()->plotType("chord"), model),
 CQChartsObjSegmentShapeData<CQChartsChordPlot>(this),
 CQChartsObjArcShapeData    <CQChartsChordPlot>(this)
{
}

CQChartsChordPlot::
~CQChartsChordPlot()
{
  term();
}

//---

void
CQChartsChordPlot::
init()
{
  CQChartsConnectionPlot::init();

  //---

  NoUpdate noUpdate(this);

  //---

  textBox_ = std::make_unique<RotatedTextBoxObj>(this);

  textBox_->setTextColor(Color(Color::Type::INTERFACE_VALUE, 1));

  //---

  setLayerActive(CQChartsLayer::Type::FG_PLOT, true);

  setSegmentFillAlpha(Alpha(0.7));

  setArcFillAlpha  (Alpha(0.3));
  setArcStrokeAlpha(Alpha(0.3));

  addTitle();

  //---

  addColorMapKey();
}

void
CQChartsChordPlot::
term()
{
}

//---

void
CQChartsChordPlot::
setInnerRadius(double r)
{
  CQChartsUtil::testAndSet(innerRadius_, r, [&]() { drawObjs(); } );
}

void
CQChartsChordPlot::
setLabelRadius(double r)
{
  CQChartsUtil::testAndSet(labelRadius_, r, [&]() { drawObjs(); } );
}

void
CQChartsChordPlot::
setRotatedText(bool b)
{
  CQChartsUtil::testAndSet(rotatedText_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsChordPlot::
setStartAngle(const Angle &a)
{
  CQChartsUtil::testAndSet(startAngle_, a, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsChordPlot::
setGapAngle(const Angle &a)
{
  CQChartsUtil::testAndSet(gapAngle_, a, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsChordPlot::
addProperties()
{
  CQChartsConnectionPlot::addProperties();

  //---

  // options
  addProp("options", "innerRadius", "", "Radius of inside of outer strip (0-1)")->
    setMinValue(0.0).setMaxValue(1.0);

  //---

  // segment
  addProp("segment/stroke", "segmentStroked", "visible", "Segment stroke visible");

  addLineProperties("segment/stroke", "segmentStroke", "Segment");

  addProp("segment/fill", "segmentFilled", "visible", "Segment fill visible");

  addFillProperties("segment/fill", "segmentFill", "Segment",
                    uint(CQChartsFillDataTypes::NO_COLOR));

  //---

  // arc
  addProp("arc/stroke", "arcStroked", "visible", "Arc stroke visible");

  addLineProperties("arc/stroke", "arcStroke", "Arc");

  addProp("arc/fill", "arcFilled", "visible", "Arc fill visible");

  addFillProperties("arc/fill", "arcFill", "Arc",
                    uint(CQChartsFillDataTypes::NO_COLOR));

  addProp("arc", "startAngle", "startAngle", "Angle for first strip segment");
  addProp("arc", "gapAngle"  , "gapAngle"  , "Angle for gap between strip segements");

  //---

  // labels
  textBox_->addTextDataProperties(propertyModel(), "labels", "Labels");

  addProp("labels", "labelRadius", "radius", "Radius for segment label (>= 1.0)")->
    setMinValue(1.0);

  addProp("labels", "rotatedText", "rotated", "Rotate labels to segment angle");

  auto labelBoxPath = QString("labels/box");

  textBox_->addBoxProperties(propertyModel(), labelBoxPath, "Labels");

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
}

CQChartsGeom::Range
CQChartsChordPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsChordPlot::calcRange");

  double lr = std::max(labelRadius(), 1.0);

  Range dataRange;

  dataRange.updateRange(-lr, -lr);
  dataRange.updateRange( lr,  lr);

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange.equalScale(aspect);
  }

  return dataRange;
}

CQChartsGeom::BBox
CQChartsChordPlot::
calcExtraFitBBox() const
{
  CQPerfTrace trace("CQChartsChordPlot::calcExtraFitBBox");

  BBox bbox;

  for (const auto &plotObj : plotObjs_) {
    auto *obj = dynamic_cast<CQChartsChordSegmentObj *>(plotObj);
    if (! obj) continue;

    bbox += obj->textBBox();
  }

  return bbox;
}

//------

double
CQChartsChordPlot::
calcInnerRadius() const
{
  return std::max(innerRadius() - maxHierDepth_*0.1, 0.1);
}

double
CQChartsChordPlot::
calcOuterRadius() const
{
  return std::min(1.0 - maxHierDepth_*0.1, 1.0);
}

//------

bool
CQChartsChordPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsChordPlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsChordPlot *>(this);

  th->clearErrors();

  //---

  // check columns
  if (! checkColumns())
    return false;

  //---

  // create objects
  auto columnDataType = calcColumnDataType();

  th->nameDataMap_.clear();

  bool rc      = false;
  bool addObjs = true;

  if      (columnDataType == ColumnDataType::HIER)
    rc = initHierObjs();
  else if (columnDataType == ColumnDataType::LINK)
    rc = initLinkObjs();
  else if (columnDataType == ColumnDataType::CONNECTIONS)
    rc = initConnectionObjs();
  else if (columnDataType == ColumnDataType::PATH)
    rc = initPathObjs();
  else if (columnDataType == ColumnDataType::FROM_TO)
    rc = initFromToObjs();
  else if (columnDataType == ColumnDataType::TABLE) {
    rc = initTableObjs(objs);

    addObjs = false;
  }

  if (! rc)
    return false;

  //---

  if (addObjs) {
    th->filterObjs();

    th->addNameDataMap(nameDataMap_, objs);
  }

  //---

  return true;
}

bool
CQChartsChordPlot::
initHierObjs() const
{
  CQPerfTrace trace("CQChartsChordPlot::initHierObjs");

  return CQChartsConnectionPlot::initHierObjs();
}

void
CQChartsChordPlot::
initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                              const HierConnectionData &destHierData) const
{
  initHierObjsConnection(srcHierData .parentStr, srcHierData .parentLinkInd, srcHierData .total,
                         destHierData.parentStr, destHierData.parentLinkInd, destHierData.total);
}

void
CQChartsChordPlot::
initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                              const HierConnectionData &destHierData) const
{
  initHierObjsConnection(srcHierData .parentStr, srcHierData .parentLinkInd, srcHierData .total,
                         destHierData.parentStr, destHierData.parentLinkInd, destHierData.total);
}

void
CQChartsChordPlot::
initHierObjsConnection(const QString &srcStr, const ModelIndex &srcLinkInd, double /*srcValue*/,
                       const QString &destStr, const ModelIndex &destLinkInd,
                       double destValue) const
{
  // find src (create if doesn't exist)
  auto srcModelIndex  = modelIndex(srcLinkInd);
  auto srcModelIndex1 = normalizeIndex(srcModelIndex);

  auto &srcData = findNameData(srcStr, srcModelIndex1);

  // find dest (create if doesn't exist)
  auto destModelIndex  = modelIndex(destLinkInd);
  auto destModelIndex1 = normalizeIndex(destModelIndex);

  auto &destData = findNameData(destStr, destModelIndex1);

  // create link from src to dest for values
  // (hier always symmetric - edge is from child to parent)
  addEdge(srcData, srcModelIndex1, destData, destModelIndex1, destValue, /*symmetric*/true);
}

//---

bool
CQChartsChordPlot::
initPathObjs() const
{
  CQPerfTrace trace("CQChartsChordPlot::initPathObjs");

  //---

  auto *th = const_cast<CQChartsChordPlot *>(this);

  th->maxNodeDepth_ = 0;

  //--

  if (! CQChartsConnectionPlot::initPathObjs())
    return false;

  //---

  if (isPropagate())
    th->propagatePathValues();

  return true;
}

void
CQChartsChordPlot::
addPathValue(const PathData &pathData) const
{
  int n = pathData.pathStrs.length();
  assert(n > 0);

  auto *th = const_cast<CQChartsChordPlot *>(this);

  th->maxNodeDepth_ = std::max(maxNodeDepth_, n - 1);

  auto pathModelIndex  = modelIndex(pathData.pathModelInd); // leaf index
  auto pathModelIndex1 = normalizeIndex(pathModelIndex);

  auto separator = calcSeparator();

  auto path1 = pathData.pathStrs[0]; // parent

  for (int i = 1; i < n; ++i) {
    auto path2 = path1 + separator + pathData.pathStrs[i]; // child

    auto &srcData  = findNameData(path1, pathModelIndex1);
    auto &destData = findNameData(path2, pathModelIndex1);

    srcData .setLabel(pathData.pathStrs[i - 1]);
    destData.setLabel(pathData.pathStrs[i    ]);

    srcData .setDepth(i - 1);
    destData.setDepth(i);

    if (i < n - 1) {
      // parent to non-leaf
      if (! srcData.hasTo(destData.from())) {
        addEdge(srcData, QModelIndex(), destData, QModelIndex(),
                pathData.value.realOr(1.0), /*symmetric*/true);

        destData.setParent(srcData.from());
      }
    }
    else {
      // parent to leaf
      addEdge(srcData, pathModelIndex1.parent(), destData, pathModelIndex1,
              pathData.value.realOr(1.0), /*symmetric*/true);

      destData.setParent(srcData.from());
      destData.setValue (OptReal(pathData.value));
    }

    path1 = path2; // update parent
  }
}

void
CQChartsChordPlot::
propagatePathValues()
{
  // propagate node value up through edges and parent nodes
  for (int depth = maxNodeDepth_; depth >= 0; --depth) {
    for (auto &p : nameDataMap_) {
      auto &chordData = p.second;
      if (chordData.depth() != depth) continue;

      ChordData::Group group("", depth, maxNodeDepth_ + 1);

      chordData.setGroup(group);

      // set node value from sum of dest values
      if (! chordData.hasValue()) {
        if (! chordData.values().empty()) {
          OptReal sum;

          for (const auto &value : chordData.values()) {
            if (value.value.isSet()) {
              double v = value.value.real();

              if (sum.isSet())
                sum = OptReal(sum.real() + v);
              else
                sum = OptReal(v);
            }
          }

          if (sum.isSet())
            chordData.setValue(sum);
        }
      }

      // propagate set node value up to source nodes
      if (chordData.hasValue()) {
        int parent = chordData.parent();

        if (parent > 0) {
          auto &parentChordData = getIndData(parent);

          parentChordData.setToValue(chordData      .from(), chordData.value().real());
          chordData      .setToValue(parentChordData.from(), chordData.value().real());
        }
      }
    }
  }
}

//---

bool
CQChartsChordPlot::
initFromToObjs() const
{
  CQPerfTrace trace("CQChartsChordPlot::initFromToObjs");

  return CQChartsConnectionPlot::initFromToObjs();
}

void
CQChartsChordPlot::
addFromToValue(const FromToData &fromToData) const
{
  auto fromModelIndex  = modelIndex(fromToData.fromModelInd);
  auto fromModelIndex1 = normalizeIndex(fromModelIndex);

  auto &srcData = findNameData(fromToData.fromStr, fromModelIndex1);

  if (fromToData.depth > 0)
    srcData.setDepth(fromToData.depth);

  // Just node
  if (fromToData.toStr == "") {
    for (const auto &nv : fromToData.nameValues.nameValues()) {
      auto value = nv.second.toString();

      if      (nv.first == "label") {
        srcData.setLabel(value);
      }
      else if (nv.first == "color") {
        //srcData.setColor(CQChartsUtil::stringToColor(value));
      }
    }
  }
  else {
    // self
    if (fromToData.fromStr == fromToData.toStr)
      return;

    auto toModelIndex  = modelIndex(fromToData.toModelInd);
    auto toModelIndex1 = normalizeIndex(toModelIndex);

    auto &destData = findNameData(fromToData.toStr, toModelIndex1);

    if (fromToData.depth > 0)
      destData.setDepth(fromToData.depth + 1);

    addEdge(srcData, fromModelIndex1, destData, toModelIndex1,
            fromToData.value.realOr(1.0), /*symmetric*/true);

    for (const auto &nv : fromToData.nameValues.nameValues()) {
      auto value = nv.second.toString();

      if      (nv.first == "label") {
      }
      else if (nv.first == "color") {
      }
    }
  }
}

//---

bool
CQChartsChordPlot::
initLinkObjs() const
{
  CQPerfTrace trace("CQChartsChordPlot::initLinkObjs");

  return CQChartsConnectionPlot::initLinkObjs();
}

void
CQChartsChordPlot::
addLinkConnection(const LinkConnectionData &linkConnectionData) const
{
  // Get group value
  GroupData groupData;

  if (groupColumn().isValid()) {
    if (! groupColumnData(linkConnectionData.groupModelInd, groupData)) {
      auto *th = const_cast<CQChartsChordPlot *>(this);
      th->addDataError(linkConnectionData.groupModelInd, "Invalid group value");
      return;
    }
  }

  //---

  // Get link value
  auto linkInd  = modelIndex(linkConnectionData.linkModelInd);
  auto linkInd1 = normalizeIndex(linkInd);

  //---

  // find src (create if doesn't exist)
  auto &srcData = findNameData(linkConnectionData.srcStr, linkInd1);

  // find dest (create if doesn't exist)
  auto &destData = findNameData(linkConnectionData.destStr, linkInd1);

  // create link from src to dest for value
  // (src and dest are same row of model)
  addEdge(srcData, linkInd1, destData, linkInd1,
          linkConnectionData.value.realOr(1.0), /*symmetric*/true);

  //---

  // set group if specified
  if (groupData.isValid())
    srcData.setGroup(groupData);

  //---

  if (linkConnectionData.depth > 0) {
    srcData .setDepth(linkConnectionData.depth);
    destData.setDepth(linkConnectionData.depth + 1);
  }
}

//------

bool
CQChartsChordPlot::
initConnectionObjs() const
{
  CQPerfTrace trace("CQChartsChordPlot::initConnectionObjs");

  return CQChartsConnectionPlot::initConnectionObjs();
}

void
CQChartsChordPlot::
addConnectionObj(int id, const ConnectionsData &connectionsData, const NodeIndex &nodeIndex) const
{
  auto srcStr = QString::number(id);

  // find src (create if doesn't exist)
  auto &srcData = findNameData(srcStr, connectionsData.ind);

  srcData.setName (connectionsData.name);
  srcData.setGroup(connectionsData.groupData);

  for (const auto &connection : connectionsData.connections) {
    auto destStr = QString::number(connection.node);

    auto &destData = findNameData(destStr, connectionsData.ind);

    auto p = nodeIndex.find(connection.node);
    assert(p != nodeIndex.end());
    auto ind = (*p).second;

    // create link from src to dest for value
    addEdge(srcData, connectionsData.ind, destData, ind, connection.value, /*symmetric*/false);
  }
}

//------

bool
CQChartsChordPlot::
initTableObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsChordPlot::initTableObjs");

  //---

  TableConnectionDatas tableConnectionDatas;
  TableConnectionInfo  tableConnectionInfo;

  if (! processTableModel(tableConnectionDatas, tableConnectionInfo))
    return false;

  int nv  = tableConnectionDatas.size();
  int nv1 = tableConnectionInfo.numNonZero;

  double total = tableConnectionInfo.total;

  //---

  using ChordDatas = std::vector<ChordData>;

  ChordDatas datas;

  datas.resize(nv);

  for (int row = 0; row < nv; ++row) {
    auto &tableConnectionData = const_cast<TableConnectionData &>(tableConnectionDatas[row]);

    datas[row].setData(tableConnectionData);
  }

  //---

  // 360 degree circle, minus gap angle degrees per set
  double gap = std::max(this->gapAngle().value(), 0.0);

  double drange = 360 - nv1*gap;

  while (drange <= 180) {
    gap /= 2.0;

    drange = 360 - nv1*gap;
  }

  // divide remaining degrees by total to get value->degrees factor
  auto *th = const_cast<CQChartsChordPlot *>(this);

  th->valueToDegrees_ = (total > 0 ? drange/total : 1);

  //---

  double angle1 = this->startAngle().value();

  for (int row = 0; row < nv; ++row) {
    auto &data = datas[row];

    // get total (skip if zero)
    // when symmetric use max as we overlay from/to connections
    double total1;

    if (! isSymmetric())
      total1 = data.maxTotal();
    else
      total1 = data.total();

    if (CMathUtil::isZero(total1))
      continue;

    double dangle = valueToDegrees(total1);

    double angle2 = angle1 + dangle;

    data.setAngles(Angle(angle1), Angle(dangle));

    angle1 = angle2 + gap;
  }

  //---

  th->segmentObjs_.clear();
  th->edgeObjs_   .clear();
  th->hierObjs_   .clear();

  for (int row = 0; row < nv; ++row) {
    const auto &data = datas[row];

    BBox rect(-1, -1, 1, 1);

    ColorInd ig;

    if (data.group().isValid())
      ig = ColorInd(data.group().ig, data.group().ng);

    ColorInd iv(row, nv);

    auto *segmentObj = createSegmentObj(rect, data, ig, iv);

    objs.push_back(segmentObj);

    th->segmentObjs_.push_back(segmentObj);

    //---

    int ind = 0;

    for (const auto &value : data.values()) {
      if (! value.primary)
        continue;

      auto *edgeObj = createEdgeObj(rect, data, value.to, value.value, value.ind);

      objs.push_back(edgeObj);

      th->edgeObjs_.push_back(edgeObj);

      segmentObj->addEdgeObj(edgeObj);

      ++ind;
    }
  }

  return true;
}

//---

CQChartsChordPlot::ChordData &
CQChartsChordPlot::
findNameData(const QString &name, const QModelIndex &nameInd) const
{
  auto *th = const_cast<CQChartsChordPlot *>(this);

  return findNameData(th->nameDataMap_, name, nameInd, /*global*/true);
}

CQChartsChordPlot::ChordData &
CQChartsChordPlot::
findNameData(NameDataMap &nameDataMap, const QString &name,
             const QModelIndex &nameInd, bool global) const
{
  auto p = nameDataMap.find(name);

  if (p == nameDataMap.end()) {
    p = nameDataMap.insert(p, NameDataMap::value_type(name, ChordData()));

    int ind = nameDataMap.size();

    (*p).second.setFrom(ind);
    (*p).second.setName(name);
    (*p).second.setNameInd(nameInd);

    if (isHierName()) {
      auto strs = name.split(calcSeparator(), QString::SkipEmptyParts);

      if (strs.size() > 1) {
        strs.pop_back();

        (*p).second.setPath(strs);
      }
    }

    if (global) {
      auto *th = const_cast<CQChartsChordPlot *>(this);

      th->indName_[ind] = name;
    }
  }

  return (*p).second;
}

//---

void
CQChartsChordPlot::
addEdge(ChordData &srcData, const QModelIndex &srcInd, ChordData &destData,
        const QModelIndex &destInd, double value, bool symmetric) const
{
  srcData.addValue(destData.from(), value, destInd, /*primary*/true );

  if (symmetric)
    destData.addValue(srcData.from(), value, srcInd, /*primary*/false);
}

//---

CQChartsChordPlot::ChordData &
CQChartsChordPlot::
getIndData(int ind)
{
  auto pi = indName_.find(ind);
  assert(pi != indName_.end());

  auto pn = nameDataMap_.find((*pi).second);
  assert(pn != nameDataMap_.end());

  return (*pn).second;
}

//---

void
CQChartsChordPlot::
filterObjs()
{
  // hide nodes below depth
  if (maxDepth() > 0) {
    for (auto &p : nameDataMap_) {
      auto &chordData = p.second;

      if (chordData.depth() > maxDepth())
        chordData.setVisible(false);
    }
  }

  // hide nodes less than min value
  if (minValue() > 0) {
    for (auto &p : nameDataMap_) {
      auto &chordData = p.second;

      if (! chordData.value().isSet() || chordData.value().real() < minValue())
        chordData.setVisible(false);
    }
  }
}

//---

bool
CQChartsChordPlot::
addMenuItems(QMenu *menu)
{
  bool added = false;

  if (canDrawColorMapKey()) {
    addColorMapKeyItems(menu);

    added = true;
  }

  return added;
}

//---

void
CQChartsChordPlot::
addNameDataMap(const NameDataMap &nameDataMap, PlotObjs &objs)
{
  using Datas = std::vector<ChordData>;

  Datas datas;

  double total = 0.0;

  for (const auto &nameData : nameDataMap) {
    const auto &data = nameData.second;
    if (! data.isVisible()) continue;

    // get total (skip if zero)
    // when symmetric use max as we overlay from/to connections
    double total1;

    if (! isSymmetric())
      total1 = data.maxTotal();
    else
      total1 = data.total();

    if (CMathUtil::isZero(total1))
      continue;

    datas.push_back(data);

    total += total1;
  }

  int nv = datas.size();

  //---

  auto cmpPaths = [](const QStringList &lhs, const QStringList &rhs) {
    int nl = lhs.length();
    int nr = rhs.length();

    if (nl != nr) return (nl - nr);

    for (int i = 0; i < nl; ++i) {
      if (lhs[i] != rhs[i])
        return (lhs[i] > rhs[i] ? 1 : -1);
    }

    return 0;
  };

  if (isSorted()) {
    // sort by value/total
    std::sort(datas.begin(), datas.end(),
      [&](const ChordData &lhs, const ChordData &rhs) {
        if (lhs.group().ivalue() != rhs.group().ivalue())
          return lhs.group().ivalue() < rhs.group().ivalue();

        if (lhs.path().length() || rhs.path().length()) {
          int cmp = cmpPaths(lhs.path(), rhs.path());
          if (cmp != 0) return (cmp < 0);
        }

        return lhs.total(/*primaryOnly*/ ! isSymmetric()) <
               rhs.total(/*primaryOnly*/ ! isSymmetric());
      });

    for (auto &data : datas)
      data.sort();
  }
  else {
    // sort by value/from
    std::sort(datas.begin(), datas.end(),
      [&](const ChordData &lhs, const ChordData &rhs) {
        if (lhs.group().ivalue() != rhs.group().ivalue())
          return lhs.group().ivalue() < rhs.group().ivalue();

        if (lhs.path().length() || rhs.path().length()) {
          int cmp = cmpPaths(lhs.path(), rhs.path());
          if (cmp != 0) return (cmp < 0);
        }

        return lhs.from() < rhs.from();
      });

    for (auto &data : datas)
      data.sort();
  }

  //---

  // 360 degree circle, minus gap angle degrees per set
  double gap = std::max(this->gapAngle().value(), 0.0);

  double drange = 360 - nv*gap;

  while (drange <= 180) {
    gap /= 2.0;

    drange = 360 - nv*gap;
  }

  // divide remaining degrees by total to get value->degrees factor
  auto *th = const_cast<CQChartsChordPlot *>(this);

  th->valueToDegrees_ = (total > 0 ? drange/total : 1);

  //---

  double angle1 = this->startAngle().value();

  for (int row = 0; row < nv; ++row) {
    auto &data = datas[row];

    double total1;

    if (! isSymmetric())
      total1 = data.maxTotal();
    else
      total1 = data.total();

    double dangle = valueToDegrees(total1);

    double angle2 = angle1 + dangle;

    data.setAngles(Angle(angle1), Angle(dangle));

    angle1 = angle2 + gap;
  }

  //---

  maxHierDepth_ = 0;

  th->segmentObjs_.clear();
  th->edgeObjs_   .clear();
  th->hierObjs_   .clear();

  for (int row = 0; row < nv; ++row) {
    const auto &data = datas[row];

    BBox rect(-1, -1, 1, 1);

    ColorInd ig;

    if (data.group().isValid())
      ig = ColorInd(data.group().ig, data.group().ng);

    ColorInd iv(row, nv);

    auto *segmentObj = createSegmentObj(rect, data, ig, iv);

    objs.push_back(segmentObj);

    th->segmentObjs_.push_back(segmentObj);

    //---

    int ind = 0;

    for (const auto &value : data.values()) {
      if (! value.primary)
        continue;

      auto *edgeObj = createEdgeObj(rect, data, value.to, value.value, value.ind);

      objs.push_back(edgeObj);

      th->edgeObjs_.push_back(edgeObj);

      ++ind;
    }

    //---

    if (isHierName()) {
      auto getHierObj = [&](const QString &name) {
        for (auto *hierObj : hierObjs_)
          if (hierObj->name() == name)
            return hierObj;

        return (CQChartsChordHierObj *) nullptr;
      };

      CQChartsChordHierObj *hierObj = nullptr;

      auto path = data.path();

      int np = path.length();

      for (int i = 0; i < np; ++i) {
        auto parentName = (path.length() > 1 ? path.join(calcSeparator()) : path[0]);

        auto *parentHierObj = getHierObj(parentName);

        if (! parentHierObj) {
          parentHierObj = createHierObj(parentName, rect);

          objs.push_back(parentHierObj);

          th->hierObjs_.push_back(parentHierObj);
        }

        if (i == 0) {
          segmentObj->setHierObj(parentHierObj);

          if (! parentHierObj->hasChildObj(segmentObj))
            parentHierObj->addChildObj(segmentObj);
        }
        else {
          hierObj->setHierObj(parentHierObj);

          if (! parentHierObj->hasChildObj(hierObj))
            parentHierObj->addChildObj(hierObj);
        }

        hierObj = parentHierObj;

        path.pop_back();
      }
    }
  }

  for (auto &edgeObj : edgeObjs_) {
    edgeObj->fromObj()->addEdgeObj(edgeObj);

    if (edgeObj->toObj() && edgeObj->toObj() != edgeObj->fromObj())
      edgeObj->toObj()->addEdgeObj(edgeObj);
  }

  for (auto *hierObj : hierObjs_)
    maxHierDepth_ = std::max(maxHierDepth_, hierObj->depth());
}

//---

CQChartsChordSegmentObj *
CQChartsChordPlot::
segmentObject(int ind) const
{
  for (const auto &obj : segmentObjs_) {
    if (obj->from() == ind)
      return obj;
  }

  return nullptr;
}

CQChartsChordEdgeObj *
CQChartsChordPlot::
edgeObject(int from, int to) const
{
  for (const auto &obj : edgeObjs_) {
    if (obj->from() == from && obj->to() == to)
      return obj;
  }

  return nullptr;
}

//---

void
CQChartsChordPlot::
preDrawObjs(PaintDevice *) const
{
  maxTotal_ = 0.0;

  for (const auto &plotObj : plotObjs_) {
    auto *hierObj    = dynamic_cast<CQChartsChordHierObj    *>(plotObj);
    auto *segmentObj = dynamic_cast<CQChartsChordSegmentObj *>(plotObj);

    if      (hierObj)
      maxTotal_ = std::max(maxTotal_, hierObj->calcTotal());
    else if (segmentObj)
      maxTotal_ = std::max(maxTotal_, segmentObj->calcTotal());
  }
}

//---

void
CQChartsChordPlot::
postResize()
{
  CQChartsPlot::postResize();

  resetDataRange(/*updateRange*/true, /*updateObjs*/false);
}

//---

void
CQChartsChordPlot::
write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
      const QString &viewVarName) const
{
  CQChartsPlot::write(os, plotVarName, modelVarName, viewVarName);

  textBox_->write(os, plotVarName);
}

//---

CQChartsChordSegmentObj *
CQChartsChordPlot::
createSegmentObj(const BBox &rect, const ChordData &data,
                 const ColorInd &ig, const ColorInd &iv) const
{
  return new CQChartsChordSegmentObj(this, rect, data, ig, iv);
}

CQChartsChordEdgeObj *
CQChartsChordPlot::
createEdgeObj(const BBox &rect, const ChordData &data, int to, const OptReal &value,
              const QModelIndex &ind) const
{
  return new CQChartsChordEdgeObj(this, rect, data, to, value, ind);
}

CQChartsChordHierObj *
CQChartsChordPlot::
createHierObj(const QString &name, const BBox &rect) const
{
  return new CQChartsChordHierObj(this, name, rect);
}

//---

CQChartsPlotCustomControls *
CQChartsChordPlot::
createCustomControls()
{
  auto *controls = new CQChartsChordPlotCustomControls(charts());

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsChordSegmentObj::
CQChartsChordSegmentObj(const CQChartsChordPlot *plot, const BBox &rect, const ChordData &data,
                        const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsChordPlot *>(plot), rect, ColorInd(), ig, iv),
 plot_(plot), data_(data)
{
  setDetailHint(DetailHint::MAJOR);

  if (data.nameInd().isValid())
    setModelInd(data.nameInd());
}

QString
CQChartsChordSegmentObj::
calcId() const
{
  auto name = data_.group().value.toString();

  if (name != "")
    return QString("%1:%2:%3:%4").arg(typeName()).arg(dataName()).
             arg(name).arg(iv_.i);
  else
    return QString("%1:%2:%3").arg(typeName()).arg(dataName()).arg(iv_.i);
}

QString
CQChartsChordSegmentObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name", dataName());

  auto name = data_.group().value.toString();

  if (name != "")
    tableTip.addTableRow("Group", name);

  if (! plot_->isSymmetric()) {
    tableTip.addTableRow("Total (Out)", calcTotal());
    tableTip.addTableRow("Total (In)" , calcAltTotal());
  }
  else
    tableTip.addTableRow("Total", calcTotal());

  if (! plot_->isSymmetric()) {
    int numIn { 0 }, numOut { 0 };

    for (const auto &obj : edgeObjs_) {
      if      (obj->fromObj() == this)
        ++numOut;
      else if (obj->toObj() == this)
        ++numIn;
      else
        assert(false);
    }

    tableTip.addTableRow("Count (Out)", numOut);
    tableTip.addTableRow("Count (In)" , numIn);
  }
  else {
    tableTip.addTableRow("Count", calcNumValues());
  }

  //---

#if 0
  plot()->addTipColumns(tableTip, modelInd());
#else
  for (const auto &c : plot_->tipColumns().columns()) {
    if (! c.isValid()) continue;

    if (tableTip.hasColumn(c))
      continue;

    bool ok1;
    auto name = plot_->modelHHeaderString(c, ok1);
    if (! ok1) continue;

    auto *columnDetails = plot_->columnDetails(c);
    if (! columnDetails) continue;

    CQChartsRValues rivals, rovals;
    CQChartsSValues sivals, sovals;

    for (const auto &obj : edgeObjs_) {
      auto ind  = obj->modelInd();
      auto ind1 = plot_->unnormalizeIndex(ind);

      ModelIndex tipModelInd(plot_, ind1.row(), c, ind1.parent());

      bool ok2;

      if (columnDetails->isNumeric()) {
        auto r = plot_->modelReal(tipModelInd, ok2);
        if (! ok2) continue;

        if (obj->toObj() != this)
          rovals.addValue(r);
        else
          rivals.addValue(r);
      }
      else {
        auto str = plot_->modelString(tipModelInd, ok2);
        if (! ok2) continue;

        if (obj->toObj() != this)
          sovals.addValue(str);
        else
          sivals.addValue(str);
      }
    }

    if (columnDetails->isNumeric()) {
      if (! plot_->isSymmetric()) {
        tableTip.addTableRow(QString("%1 (Out)").arg(name), rovals.sum());
        tableTip.addTableRow(QString("%1 (In)").arg(name), rivals.sum());
      }
      else
        tableTip.addTableRow(name, rovals.sum());
    }
    else {
      if (! plot_->isSymmetric()) {
        tableTip.addTableRow(QString("%1 (Out)").arg(name), sovals.numUnique());
        tableTip.addTableRow(QString("%1 (In)").arg(name), sivals.numUnique());
      }
      else
        tableTip.addTableRow(name, sovals.numUnique());
    }

    tableTip.addColumn(c);
  }
#endif

  //---

  return tableTip.str();
}

bool
CQChartsChordSegmentObj::
inside(const Point &p) const
{
  return arcData().inside(p);
}

//---

CQChartsArcData
CQChartsChordSegmentObj::
arcData() const
{
  CQChartsArcData arcData;

  arcData.setInnerRadius(calcInnerRadius());
  arcData.setOuterRadius(calcOuterRadius());

  double a1, da;

  dataAngles(a1, da);

  double a2 = a1 + da;

  arcData.setAngle1(Angle(a1));
  arcData.setAngle2(Angle(a2));

  return arcData;
}

//---

void
CQChartsChordSegmentObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->linkColumn ());
  addColumnSelectIndex(inds, plot_->groupColumn());
}

CQChartsChordSegmentObj::PlotObjs
CQChartsChordSegmentObj::
getConnected() const
{
  PlotObjs plotObjs;

  for (const auto &value : data_.values()) {
    if (! value.primary)
      continue;

    auto *toObj = plot_->segmentObject(value.to);
    if (! toObj) continue;

    plotObjs.push_back(toObj);
  }

  return plotObjs;
}

//---

void
CQChartsChordSegmentObj::
draw(PaintDevice *device) const
{
  // calc inner outer arc rectangles
  double ri = calcInnerRadius();
  double ro = calcOuterRadius();

  Point o1(-ro, -ro);
  Point o2( ro,  ro);
  Point i1(-ri, -ri);
  Point i2( ri,  ri);

  BBox obbox(o1, o2);
  BBox ibbox(i1, i2);

  //---

  double angle1, dangle;

  dataAngles(angle1, dangle);

  //---

  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // draw value set segment arc path
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  CQChartsDrawUtil::drawArcSegment(device, ibbox, obbox, Angle(angle1), Angle(dangle));

  device->resetColorNames();

  //---

  if (plot_->drawLayerType() == CQChartsLayer::Type::MOUSE_OVER) {
    plot_->view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER_EXTRA);

    for (const auto &value : data_.values()) {
      if (! value.primary)
        continue;

      auto *toObj = plot_->edgeObject(data_.from(), value.to);
      if (! toObj) continue;

      toObj->setNotificationsEnabled(false);
      toObj->setInside(true); toObj->draw(device); toObj->setInside(false);
      toObj->setNotificationsEnabled(true);
    }

    plot_->view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER);
  }
}

void
CQChartsChordSegmentObj::
drawFg(PaintDevice *device) const
{
  if (dataName() == "")
    return;

  if (! plot_->textBox()->isTextVisible())
    return;

  // get total (skip if zero)
  // when symmetric use max as we overlay from/to connections
  double total;

  if (! plot_->isSymmetric())
    total = data_.maxTotal();
  else
    total = data_.total();

  if (CMathUtil::isZero(total))
    return;

  // calc name and label radius
  auto name = dataName();

  double ri = calcInnerRadius();
  double ro = calcOuterRadius();

  double lr = 0.0;

  if (hierObj()) {
    lr = (ri + ro)/2.0;

    auto strs = name.split(plot_->calcSeparator(), QString::SkipEmptyParts);

    if (strs.size() > 1)
      name = strs[strs.size() - 1];
  }
  else {
    double lr1 = labelRadius();

    lr = ri + lr1*(ro - ri);
  }

  lr = std::max(lr, 0.01);

  // calc label angle
  double angle1, dangle;

  dataAngles(angle1, dangle);

  double angle2 = angle1 + dangle;

  double ta = CMathUtil::avg(angle1, angle2);

  //---

  // set connection line pen
  // TODO: separate text and line pen control
  auto colorInd = calcColorInd();

  PenBrush lpenBrush;

  auto bg = plot_->interpPaletteColor(colorInd);

  plot_->setPen(lpenBrush, PenData(true, bg, Alpha()));

  //---

  // draw text using line pen
  Point center(0, 0);

  plot_->textBox()->drawConnectedRadialText(device, center, ro, lr, ta, name,
                                            lpenBrush.pen, plot_->isRotatedText());

  //---

  if (plot_->drawLayerType() == CQChartsLayer::Type::MOUSE_OVER) {
    plot_->view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER_EXTRA);

    for (const auto &obj : edgeObjs_) {
      obj->setNotificationsEnabled(false);
      obj->setInside(true); obj->draw(device); obj->setInside(false);
      obj->setNotificationsEnabled(true);
    }

    plot_->view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER);
  }
}

void
CQChartsChordSegmentObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // set fill and stroke
  // TODO: separate segment stroke/fill control
  auto segmentStrokeColor = plot_->interpSegmentStrokeColor(ColorInd());

  auto  fromColor = calcFromColor();
  Alpha fromAlpha;

  if (! isInside() && ! isSelected())
    fromAlpha = plot_->segmentFillAlpha();

  plot_->setPenBrush(penBrush,
    PenData  (plot_->isSegmentStroked(), segmentStrokeColor, plot_->segmentStrokeAlpha(),
              plot_->segmentStrokeWidth(), plot_->segmentStrokeDash()),
    BrushData(plot_->isSegmentFilled(), fromColor, fromAlpha,
              plot_->segmentFillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

QColor
CQChartsChordSegmentObj::
calcFromColor() const
{
  auto colorInd = calcColorInd();

  if (plot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    if (plot_->colorColumn().isValid() && plot_->colorColumn() == plot_->valueColumn()) {
      double r = CMathUtil::map(calcTotal(), 0, plot_->maxTotal(), 0.0, 1.0);

      auto color = plot_->normalizedColorMapRealColor(r);

      return plot_->interpColor(color, colorInd);
    }

    if (plot_->colorColumn().isValid()) {
      double total = 0.0;

      for (const auto &obj : edgeObjs_) {
        auto ind  = obj->modelInd();
        auto ind1 = plot_->unnormalizeIndex(ind);

        ModelIndex colorModelInd(plot_, ind1.row(), plot_->colorColumn(), ind1.parent());

        bool ok;
        double r = plot_->modelReal(colorModelInd, ok);
        if (! ok) continue;

        total += r;
      }

      auto color = plot_->colorMapRealColor(total);

      return plot_->interpColor(color, colorInd);
    }

    if (data_.group().isValid())
      return plot_->blendGroupPaletteColor(data_.group().ivalue(), iv_.value(), 0.1);
    else
      return plot_->interpPaletteColor(colorInd);
  }
  else
    return plot_->interpPaletteColor(colorInd);
}

void
CQChartsChordSegmentObj::
writeScriptData(ScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  std::ostream &os = device->os();

  os << "\n";
  os << "  this.total = " << calcTotal() << ";\n";
}

CQChartsGeom::BBox
CQChartsChordSegmentObj::
textBBox() const
{
  if (! dataName().length())
    return BBox();

  //---

  double ri = calcInnerRadius();
  double ro = calcOuterRadius();

  //---

  double angle1, dangle;

  dataAngles(angle1, dangle);

  double angle2 = angle1 + dangle;

  //---

  if (! plot_->textBox()->isTextVisible())
    return BBox();

  double total = calcTotal();

  if (CMathUtil::isZero(total))
    return BBox();

  //---

  // calc box of text on arc center line
  double lr1 = labelRadius();
  double lr  = ri + lr1*(ro - ri);

  lr = std::max(lr, 0.01);

  double ta = CMathUtil::avg(angle1, angle2);

  Point center(0, 0);

  BBox tbbox;

  plot_->textBox()->calcConnectedRadialTextBBox(center, ro, lr, ta, dataName(),
                                                plot_->isRotatedText(), tbbox);

  return tbbox;
}

double
CQChartsChordSegmentObj::
calcInnerRadius() const
{
  return std::min(std::max(plot_->calcInnerRadius(), 0.01), 1.0);
}

double
CQChartsChordSegmentObj::
calcOuterRadius() const
{
  return std::min(std::max(plot_->calcOuterRadius(), 0.01), 1.0);
}

double
CQChartsChordSegmentObj::
labelRadius() const
{
  return std::max(plot_->labelRadius(), 1.0);
}

void
CQChartsChordSegmentObj::
dataAngles(double &a, double &da) const
{
  a  = data_.angle ().value();
  da = data_.dangle().value();
}

void
CQChartsChordSegmentObj::
valueAngles(int ind, double &a, double &da, ChordData::PrimaryType primaryType) const
{
  a = data_.angle().value();

  for (const auto &value : data_.values()) {
    if (primaryType == ChordData::PrimaryType::PRIMARY && ! value.primary)
      continue;

    if (primaryType == ChordData::PrimaryType::NON_PRIMARY && value.primary)
      continue;

    da = plot_->valueToDegrees(value.value.realOr(0.0));

    if (ind == value.to)
      return;

    a += da;
  }

  da = 0.0;
}

double
CQChartsChordSegmentObj::
calcTotal() const
{
  return data_.total(/*primaryOnly*/ ! plot_->isSymmetric());
}

double
CQChartsChordSegmentObj::
calcAltTotal() const
{
  assert(! plot_->isSymmetric());

  double t1 = data_.total(/*primaryOnly*/ false);
  double t2 = data_.total(/*primaryOnly*/ true );

  return t1 - t2;
}

int
CQChartsChordSegmentObj::
calcNumValues() const
{
  return data_.values().size();
}

//------

CQChartsChordEdgeObj::
CQChartsChordEdgeObj(const CQChartsChordPlot *plot, const BBox &rect, const ChordData &data,
                     int to, const OptReal &value, const QModelIndex &ind) :
 CQChartsPlotObj(const_cast<CQChartsChordPlot *>(plot), rect, ColorInd(), ColorInd(), ColorInd()),
 plot_(plot), data_(data), to_(to), value_(value)
{
  setDetailHint(DetailHint::MAJOR);

  if (ind.isValid())
    setModelInd(ind);
}

QString
CQChartsChordEdgeObj::
calcId() const
{
  auto *fromObj = this->fromObj();
  auto *toObj   = this->toObj  ();

  auto fromName = (fromObj ? fromObj->dataName() : QString::number(from()));
  auto toName   = (toObj   ? toObj  ->dataName() : QString::number(to  ()));

  return QString("%1:%2").arg(fromName).arg(toName);
}

QString
CQChartsChordEdgeObj::
calcTipId() const
{
  auto *fromObj = this->fromObj();
  auto *toObj   = this->toObj  ();

  CQChartsTableTip tableTip;

  if (fromObj)
    tableTip.addTableRow("From", fromObj->dataName());

  if (toObj)
    tableTip.addTableRow("To", toObj->dataName());

  if (value_.isSet())
    tableTip.addTableRow("Value", value_.real());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

bool
CQChartsChordEdgeObj::
inside(const Point &p) const
{
  return path_.contains(p.qpoint());
}

//---

void
CQChartsChordEdgeObj::
getObjSelectIndices(Indices &) const
{
}

CQChartsChordEdgeObj::PlotObjs
CQChartsChordEdgeObj::
getConnected() const
{
  PlotObjs plotObjs;

  auto *fromObj = this->fromObj();
  auto *toObj   = this->toObj  ();

  if (fromObj)
    plotObjs.push_back(fromObj);

  if (toObj)
    plotObjs.push_back(toObj);

  return plotObjs;
}

//---

void
CQChartsChordEdgeObj::
draw(PaintDevice *device) const
{
  auto *fromObj = this->fromObj();
  auto *toObj   = this->toObj  ();

  if (! fromObj || ! toObj)
    return;

  //---

  // calc inner outer arc rectangles
  double ri = fromObj->calcInnerRadius();

  Point i1(-ri, -ri);
  Point i2( ri,  ri);

  BBox ibbox(i1, i2);

  //---

  // get from/to angles
  double a1, da1, a2, da2;

  if (! plot_->isSymmetric()) {
    fromObj->valueAngles(to  (), a1, da1, ChordData::PrimaryType::PRIMARY    );
    toObj  ->valueAngles(from(), a2, da2, ChordData::PrimaryType::NON_PRIMARY);
  }
  else {
    fromObj->valueAngles(to  (), a1, da1);
    toObj  ->valueAngles(from(), a2, da2);
  }

  if (CMathUtil::isZero(da1))
    return;
//if (CMathUtil::isZero(da2))
//  return;

  //---

  // set arc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw arcs between value sets
  bool isSelf = (from() == to());

  path_ = QPainterPath();

  CQChartsDrawUtil::arcsConnectorPath(path_, ibbox,
    Angle(a1), Angle(da1), Angle(a2), Angle(da2), isSelf);

  device->drawPath(path_);

  //---

  if (plot_->drawLayerType() == CQChartsLayer::Type::MOUSE_OVER) {
    plot_->view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER_EXTRA);

    if (fromObj) {
      fromObj->setNotificationsEnabled(false);
      fromObj->setInside(true); fromObj->draw(device); fromObj->setInside(false);
      fromObj->setNotificationsEnabled(true);
    }

    if (toObj) {
      toObj->setNotificationsEnabled(false);
      toObj->setInside(true); toObj->draw(device); toObj->setInside(false);
      toObj->setNotificationsEnabled(true);
    }

    plot_->view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER);
  }

  //---

  if (plot_->drawLayerType() == CQChartsLayer::Type::MOUSE_OVER) {
    QPainterPath path;

    double a1c = a1 + da1/2.0;
    double a2c = a2 + da2/2.0;

    CQChartsDrawUtil::arcsConnectorPath(path, ibbox,
      Angle(a1c - 0.05), Angle(0.1), Angle(a2c - 0.05), Angle(0.1), isSelf);

    auto p1 = path.pointAtPercent(0.23);
    auto p2 = path.pointAtPercent(0.27);

    CQChartsArrowData arrowData;

    if (p1.x() < p2.x()) {
      arrowData.setFHeadType(CQChartsArrowData::HeadType::NONE );
      arrowData.setTHeadType(CQChartsArrowData::HeadType::ARROW);
    }
    else {
      arrowData.setFHeadType(CQChartsArrowData::HeadType::ARROW);
      arrowData.setTHeadType(CQChartsArrowData::HeadType::NONE );
    }

    auto c = CQChartsUtil::bwColor(penBrush.pen.color());

    PenBrush arrowPenBrush;

    plot_->setPenBrush(arrowPenBrush,
      PenData(false), BrushData(true, c));

    auto strokeWidth = CQChartsLength::pixel(1);

    CQChartsArrow::drawArrow(device, Point(p1), Point(p2), arrowData, strokeWidth,
                             /*rectilinear*/false, arrowPenBrush);
  }
}

void
CQChartsChordEdgeObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto *fromObj = this->fromObj();
  auto *toObj   = this->toObj  ();

  if (! fromObj || ! toObj) return;

  //---

  auto colorInd = calcColorInd();

  //---

  // set pen and brush
  // TODO: separate arc stroke/fill control
  auto arcStrokeColor = plot_->interpArcStrokeColor(colorInd);

  //---

  // set fill color
  QColor fillColor;

  if (plot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    if      (plot_->colorColumn().isValid() && plot_->colorColumn() == plot_->valueColumn()) {
      double r = CMathUtil::map(value_.realOr(0.0), 0, plot_->maxTotal(), 0.0, 1.0);

      auto color = plot_->normalizedColorMapRealColor(r);

      fillColor = plot_->interpColor(color, colorInd);
    }
    else if (modelInd().isValid() && plot_->colorColumn().isValid()) {
      Color color;

      auto ind  = modelInd();
      auto ind1 = plot_->unnormalizeIndex(ind);

      ModelIndex colorModelInd(plot_, ind1.row(), plot_->colorColumn(), ind1.parent());

      if (plot_->modelIndexColor(colorModelInd, color))
        fillColor = plot_->interpColor(color, colorInd);
    }
  }

  if (! fillColor.isValid()) {
    auto fromColor = fromObj->calcFromColor();
    auto toColor   = toObj  ->calcFromColor();

    fillColor = CQChartsUtil::blendColors(fromColor, toColor, 0.5);
  }

  //---

  // set fill alpha
  Alpha fillAlpha;

  if (! isInside() && ! isSelected())
    fillAlpha = plot_->arcFillAlpha();

  //---

  plot_->setPenBrush(penBrush,
    PenData  (plot_->isArcStroked(), arcStrokeColor, plot_->arcStrokeAlpha(),
              plot_->arcStrokeWidth(), plot_->arcStrokeDash()),
    BrushData(plot_->isArcFilled(), fillColor, fillAlpha,
              plot_->arcFillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

CQChartsChordSegmentObj *
CQChartsChordEdgeObj::
fromObj() const
{
  return plot_->segmentObject(from());
}

CQChartsChordSegmentObj *
CQChartsChordEdgeObj::
toObj() const
{
  return plot_->segmentObject(to());
}

//------

CQChartsChordHierObj::
CQChartsChordHierObj(const CQChartsChordPlot *plot, const QString &name, const BBox &rect) :
 CQChartsPlotObj(const_cast<CQChartsChordPlot *>(plot), rect, ColorInd(), ColorInd(), ColorInd()),
 plot_(plot), name_(name)
{
  setDetailHint(DetailHint::MINOR);
}

QString
CQChartsChordHierObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(name());
}

QString
CQChartsChordHierObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name", name());

  if (! plot_->isSymmetric()) {
    tableTip.addTableRow("Total (Out)", calcTotal());
    tableTip.addTableRow("Total (In)" , calcAltTotal());
  }
  else
    tableTip.addTableRow("Total", calcTotal());

  tableTip.addTableRow("Count", calcNumValues());

  return tableTip.str();
}

bool
CQChartsChordHierObj::
inside(const Point &p) const
{
  return path_.contains(p.qpoint());
}

//---

void
CQChartsChordHierObj::
draw(PaintDevice *device) const
{
  // calc inner outer arc rectangles
  double ri = calcInnerRadius();
  double ro = calcOuterRadius();

  Point o1(-ro, -ro);
  Point o2( ro,  ro);
  Point i1(-ri, -ri);
  Point i2( ri,  ri);

  BBox obbox(o1, o2);
  BBox ibbox(i1, i2);

  //---

  double angle1 = 0.0, angle2 = 0.0;

  dataAngles(angle1, angle2);

  //---

  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // draw value set segment arc path
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  QPainterPath path;

  CQChartsDrawUtil::arcSegmentPath(path, ibbox, obbox, Angle(angle1), Angle(angle2 - angle1));

  path_ = path;

  device->drawPath(path);

  device->resetColorNames();
}

void
CQChartsChordHierObj::
drawFg(PaintDevice *device) const
{
  if (name() == "")
    return;

  if (! plot_->textBox()->isTextVisible())
    return;

  // calc label radius
  double ri = calcInnerRadius();
  double ro = calcOuterRadius();

//double lr1 = labelRadius();
//double lr  = ri + lr1*(ro - ri);
  double lr  = (ri + ro)/2.0;

  lr = std::max(lr, 0.01);

  // calc label angle
  double angle1, angle2;

  dataAngles(angle1, angle2);

  double ta = CMathUtil::avg(angle1, angle2);

  //---

  auto name = this->name();

  auto strs = name.split(plot_->calcSeparator(), QString::SkipEmptyParts);

  if (strs.size() > 1)
    name = strs[strs.size() - 1];

  //---

  // set connection line pen
  // TODO: separate text and line pen control
  auto colorInd = calcColorInd();

  PenBrush lpenBrush;

  auto bg = plot_->interpPaletteColor(colorInd);

  plot_->setPen(lpenBrush, PenData(true, bg, Alpha()));

  //---

  // draw text using line pen
  Point center(0, 0);

  plot_->textBox()->drawConnectedRadialText(device, center, ro, lr, ta, name,
                                            lpenBrush.pen, plot_->isRotatedText());
}

void
CQChartsChordHierObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // set pen and brush
  // TODO: separate arc stroke/fill control
  auto segmentStrokeColor = plot_->interpSegmentStrokeColor(ColorInd());

  auto fillColor = this->fillColor();

  plot_->setPenBrush(penBrush,
    PenData  (plot_->isSegmentFilled(), segmentStrokeColor, plot_->segmentStrokeAlpha(),
              plot_->segmentStrokeWidth(), plot_->segmentStrokeDash()),
    BrushData(plot_->isSegmentStroked(), fillColor, plot_->segmentFillAlpha(),
              plot_->segmentFillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

QColor
CQChartsChordHierObj::
fillColor() const
{
  if (plot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    if (plot_->colorColumn().isValid() && plot_->colorColumn() == plot_->valueColumn()) {
      double r = CMathUtil::map(calcTotal(), 0, plot_->maxTotal(), 0.0, 1.0);

      auto color = plot_->normalizedColorMapRealColor(r);

      return plot_->interpColor(color, ColorInd());
    }
  }

  //---

  std::vector<QColor> colors;

  for (auto *obj : childObjs_) {
    auto *hierObj    = dynamic_cast<CQChartsChordHierObj    *>(obj);
    auto *segmentObj = dynamic_cast<CQChartsChordSegmentObj *>(obj);

    if      (hierObj)
      colors.push_back(hierObj->fillColor());
    else if (segmentObj)
      colors.push_back(segmentObj->calcFromColor());
  }

  return CQChartsUtil::blendColors(colors);
}

double
CQChartsChordHierObj::
calcInnerRadius() const
{
  return plot_->calcOuterRadius() + (childDepth() - 1)*0.1;
}

double
CQChartsChordHierObj::
calcOuterRadius() const
{
  return calcInnerRadius() + 0.1;
}

double
CQChartsChordHierObj::
labelRadius() const
{
  return std::max(plot_->labelRadius(), 1.0);
}

void
CQChartsChordHierObj::
dataAngles(double &angle1, double &angle2) const
{
  angle1 = 0.0;
  angle2 = 0.0;

  bool angleSet = false;

  for (auto *obj : childObjs_) {
    auto *hierObj    = dynamic_cast<CQChartsChordHierObj    *>(obj);
    auto *segmentObj = dynamic_cast<CQChartsChordSegmentObj *>(obj);

    if      (hierObj) {
      double angle11 = 0.0, angle21 = 0.0;

      hierObj->dataAngles(angle11, angle21);

      if (! angleSet) {
        angle1 = angle11;
        angle2 = angle21;

        angleSet = true;
      }
      else {
        angle1 = std::min(angle1, angle11);
        angle2 = std::max(angle2, angle21);
      }
    }
    else if (segmentObj) {
      double angle = 0.0, dangle = 0.0;

      segmentObj->dataAngles(angle, dangle);

      if (! angleSet) {
        angle1 = angle;
        angle2 = angle + dangle;

        angleSet = true;
      }
      else {
        angle1 = std::min(angle1, angle);
        angle2 = std::max(angle2, angle + dangle);
      }
    }
    else
      continue;
  }
}

double
CQChartsChordHierObj::
calcTotal() const
{
  double total = 0.0;

  for (auto *obj : childObjs_) {
    auto *hierObj    = dynamic_cast<CQChartsChordHierObj    *>(obj);
    auto *segmentObj = dynamic_cast<CQChartsChordSegmentObj *>(obj);

    if      (hierObj)
      total += hierObj->calcTotal();
    else if (segmentObj)
      total += segmentObj->calcTotal();
  }

  return total;
}

double
CQChartsChordHierObj::
calcAltTotal() const
{
  double total = 0.0;

  for (auto *obj : childObjs_) {
    auto *hierObj    = dynamic_cast<CQChartsChordHierObj    *>(obj);
    auto *segmentObj = dynamic_cast<CQChartsChordSegmentObj *>(obj);

    if      (hierObj)
      total += hierObj->calcAltTotal();
    else if (segmentObj)
      total += segmentObj->calcAltTotal();
  }

  return total;
}

int
CQChartsChordHierObj::
calcNumValues() const
{
  int n = 0;

  for (auto *obj : childObjs_) {
    auto *hierObj    = dynamic_cast<CQChartsChordHierObj    *>(obj);
    auto *segmentObj = dynamic_cast<CQChartsChordSegmentObj *>(obj);

    if      (hierObj)
      n += hierObj->calcNumValues();
    else if (segmentObj)
      n += segmentObj->calcNumValues();
  }

  return n;
}

int
CQChartsChordHierObj::
depth() const
{
  return (hierObj() ? hierObj()->depth() : 0) + 1;
}

int
CQChartsChordHierObj::
childDepth() const
{
  if (childObjs_.empty())
    return 0;

  int depth = 1;

  for (auto *obj : childObjs_) {
    auto *hierObj = dynamic_cast<CQChartsChordHierObj *>(obj);

    if (hierObj)
      depth = std::max(depth, hierObj->childDepth() + 1);
  }

  return depth;
}

//------

CQChartsChordPlotCustomControls::
CQChartsChordPlotCustomControls(CQCharts *charts) :
 CQChartsConnectionPlotCustomControls(charts, "chord")
{
  addConnectionColumnWidgets();

  addColorColumnWidgets("Chord Color");

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsChordPlotCustomControls::
connectSlots(bool b)
{
  CQChartsConnectionPlotCustomControls::connectSlots(b);
}

void
CQChartsChordPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  plot_ = dynamic_cast<CQChartsChordPlot *>(plot);

  CQChartsConnectionPlotCustomControls::setPlot(plot);
}

void
CQChartsChordPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  CQChartsConnectionPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}
