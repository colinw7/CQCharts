#include <CQChartsDotPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsNamePair.h>
#include <CQChartsConnectionList.h>
#include <CQChartsValueSet.h>
#include <CQChartsVariant.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsArrow.h>
#include <CQChartsEditHandles.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQDot.h>
#include <CQPerfMonitor.h>
#include <CCommand.h>

#include <QMenu>
#include <QAction>

#include <fstream>

CQChartsDotPlotType::
CQChartsDotPlotType()
{
}

void
CQChartsDotPlotType::
addParameters()
{
  CQChartsConnectionPlotType::addParameters();
}

QString
CQChartsDotPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Dot Plot").
    h3("Summary").
     p("Draw connected objects as a connected graph.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/dot.png"));
}

CQChartsPlot *
CQChartsDotPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsDotPlot(view, model);
}

//------

CQChartsDotPlot::
CQChartsDotPlot(View *view, const ModelP &model) :
 CQChartsConnectionPlot(view, view->charts()->plotType("dot"), model),
 CQChartsObjTextData<CQChartsDotPlot>(this),
 CQChartsObjNodeShapeData<CQChartsDotPlot>(this),
 CQChartsObjEdgeShapeData<CQChartsDotPlot>(this)
{
}

CQChartsDotPlot::
~CQChartsDotPlot()
{
  term();
}

//---

void
CQChartsDotPlot::
init()
{
  CQChartsConnectionPlot::init();

  //---

  NoUpdate noUpdate(this);

  setLayerActive(Layer::Type::FG_PLOT, true);

  //---

  Color bg(Color::Type::PALETTE);

  setNodeFilled(true);
  setNodeFillColor(bg);

  setNodeStroked(true);
  setNodeStrokeAlpha(Alpha(0.2));

  //---

  setEdgeFilled(true);
  setEdgeFillColor(bg);
  setEdgeFillAlpha(Alpha(0.25));

  setEdgeStroked(true);
  setEdgeStrokeAlpha(Alpha(0.2));

  //---

  addTitle();

  //---

  addColorMapKey();

  //---

  bbox_ = targetBBox_;

  setFitMargin(PlotMargin(Length::percent(5), Length::percent(5),
                          Length::percent(5), Length::percent(5)));
}

void
CQChartsDotPlot::
term()
{
  // delete objects first to ensure link from edge/node to object reset
  clearPlotObjects();

  clearNodesAndEdges();
}

//---

void
CQChartsDotPlot::
clearNodesAndEdges()
{
  nameNodeMap_.clear();
  indNodeMap_ .clear();

  nodes_.clear();
  edges_.clear();

  groupValueInd_.clear();

  maxNodeDepth_ = 0;
}

//---

void
CQChartsDotPlot::
setNodeShape(const NodeShape &s)
{
  CQChartsUtil::testAndSet(nodeShape_, s, [&]() { updateObjs(); } );
}

void
CQChartsDotPlot::
setEdgeShape(const EdgeShape &s)
{
  CQChartsUtil::testAndSet(edgeShape_, s, [&]() { updateObjs(); } );
}

void
CQChartsDotPlot::
setEdgeScaled(bool b)
{
  CQChartsUtil::testAndSet(edgeScaled_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDotPlot::
setEdgeWidth(const Length &l)
{
  CQChartsUtil::testAndSet(edgeWidth_, l, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDotPlot::
setArrowWidth(double w)
{
  CQChartsUtil::testAndSet(arrowWidth_, w, [&]() { updateRangeAndObjs(); } );
}

//---

const Qt::Orientation &
CQChartsDotPlot::
orientation() const
{
  return orientation_;
}

void
CQChartsDotPlot::
setOrientation(const Qt::Orientation &o)
{
  CQChartsUtil::testAndSet(orientation_, o, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsDotPlot::
setBlendEdgeColor(bool b)
{
  CQChartsUtil::testAndSet(blendEdgeColor_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsDotPlot::
addProperties()
{
  CQChartsConnectionPlot::addProperties();

  //---

  // options
  addProp("options", "orientation", "orientation", "Plot orientation");

  // coloring
  addProp("coloring", "blendEdgeColor", "", "Blend Edge Node Colors");

  // node
  addProp("node", "nodeShape", "shapeType", "Node shape type");

  // node style
  addProp("node/stroke", "nodeStroked", "visible", "Node stroke visible");

  addLineProperties("node/stroke", "nodeStroke", "Node");

  addProp("node/fill", "nodeFilled", "visible", "Node fill visible");

  addFillProperties("node/fill", "nodeFill", "Node");

  //---

  // edge
  addProp("edge", "edgeShape" , "shapeType" , "Edge shape type");
  addProp("edge", "edgeScaled", "scaled"    , "Edge is scaled");
  addProp("edge", "edgeWidth" , "width"     , "Edge width");
  addProp("edge", "arrowWidth", "arrowWidth", "Arrow width");

  // edge style
  addProp("edge/stroke", "edgeStroked", "visible", "Edge stroke visible");

  addLineProperties("edge/stroke", "edgeStroke", "Edge");

  addProp("edge/fill", "edgeFilled", "visible", "Edit fill visible");

  addFillProperties("edge/fill", "edgeFill", "Edge");

  //---

  // text
  addProp("text", "textVisible", "visible", "Text label visible");

  addTextProperties("text", "text", "", CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::FORMATTED |
                    CQChartsTextOptions::ValueType::SCALED |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
}

//---

CQChartsGeom::Range
CQChartsDotPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsDotPlot::calcRange");

  auto *th = const_cast<CQChartsDotPlot *>(this);

//th->nodeYSet_ = false;

  Range dataRange;

  auto *model = this->model().data();

  if (! model)
    return dataRange;

  //---

  th->bbox_ = targetBBox_;

  dataRange.updateRange(bbox_.getLL());
  dataRange.updateRange(bbox_.getUR());

  //---

  return dataRange;
}

CQChartsGeom::Range
CQChartsDotPlot::
objTreeRange() const
{
  auto bbox = nodesBBox();

  if (! bbox.isValid())
    return Range(0, 0, 1, 1);

  return Range(bbox.getXMin(), bbox.getYMax(), bbox.getXMax(), bbox.getYMin());
}

CQChartsGeom::BBox
CQChartsDotPlot::
nodesBBox() const
{
  // calc bounding box of all nodes (all graphs)
  BBox bbox;

  for (auto *node : nodes_) {
    if (! node->isVisible()) continue;

    bbox += node->rect();
  }

  return bbox;
}

//------

bool
CQChartsDotPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsDotPlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsDotPlot *>(this);

  th->clearErrors();

  //---

  // check columns
  if (! checkColumns())
    return false;

  //---

  // init objects
  th->clearNodesAndEdges();

  auto *model = this->model().data();
  if (! model) return false;

  //---

  // create objects
  auto columnDataType = calcColumnDataType();

  bool rc = false;

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
  else if (columnDataType == ColumnDataType::TABLE)
    rc = initTableObjs();

  if (! rc)
    return false;

  //---

  th->filterObjs();

  //---

  writeGraph();

  addObjects(objs);

  return true;
}

void
CQChartsDotPlot::
addObjects(PlotObjs &objs) const
{
  if (! bbox_.isValid())
    return;

  for (const auto &node : nodes()) {
    auto *nodeObj = createObjFromNode(node);

    objs.push_back(nodeObj);
  }

  for (const auto &edge : edges()) {
    auto *edgeObj = addEdgeObj(edge);

    if (edgeObj)
      objs.push_back(edgeObj);
  }
}

void
CQChartsDotPlot::
writeGraph() const
{
  auto dotFilename  = std::string("/tmp/dot.gv");
  auto jsonFilename = std::string("/tmp/dot.json");

  {
  auto os = std::ofstream(dotFilename, std::ofstream::out);

  os << "digraph g {\n";

  using NodeSet = std::set<Node *>;

  NodeSet nodeSet;

  auto printAttr = [&](const QString &name, const QString &value, int &attrCount) {
    if (attrCount == 0)
      os << " [";
    else
      os << ",";

    os << name.toStdString() << "=\"" << value.toStdString() << "\"";

    ++attrCount;
  };

  auto endPrintAttr = [&](int attrCount) {
    if (attrCount > 0)
      os << "]";

    os << ";\n";
  };

  auto printNode = [&](Node *node) {
    auto p = nodeSet.find(node);
    if (p != nodeSet.end()) return;

    os << "\"" << node->name().toStdString() << "\"";

    int nodeAttrCount = 0;

    auto shapeStr = shapeTypeToString(node->shapeType());

    printAttr("shape", shapeStr, nodeAttrCount);

    if (node->label().length())
      printAttr("label", node->label(), nodeAttrCount);

    endPrintAttr(nodeAttrCount);

    nodeSet.insert(node);
  };

  for (const auto &edge : edges_) {
    auto *node1 = edge->srcNode ();
    auto *node2 = edge->destNode();

    printNode(node1);
    printNode(node2);

    os << "\"" << node1->name().toStdString() << "\" -> \"" << node2->name().toStdString() << "\"";

    int edgeAttrCount = 0;

    if (edge->label().length())
      printAttr("label", edge->label(), edgeAttrCount);

    endPrintAttr(edgeAttrCount);
  }

  os << "}\n";
  }

  CCommand::Args args;

  args.push_back("-Tjson");
  args.push_back(dotFilename);

  CCommand cmd("dot", "/usr/bin/dot", args);

  cmd.addFileDest(jsonFilename);

  cmd.start();

  cmd.wait();

  CQDot::App dot;

  dot.processFile(jsonFilename);

  BBox bbox;

  for (auto &object : dot.objects()) {
    auto *node = findNode(object->name());

    if (! node) {
      std::cerr << object->name().toStdString() << "Not Found\n";
      continue;
    }

    node->setDotId(object->id());

    BBox bbox1(object->rect());

    if (! bbox1.isValid()) {
      std::cerr << "No bbox for object " << object->name().toStdString() << "\n";
      continue;
    }

    node->setRect(bbox1);

    bbox += bbox1;
  }

  auto addPathPoint = [](QPainterPath &path, const Point &p) {
    if (path.elementCount() == 0)
      path.moveTo(p.qpoint());
    else
      path.lineTo(p.qpoint());
  };

  for (auto &dotEdge : dot.edges()) {
    int tailId = dotEdge->tailId(); // from
    int headId = dotEdge->headId(); // to

    auto *tailNode = findDotNode(tailId);
    auto *headNode = findDotNode(headId);
    if (! tailNode || ! headNode) {
      std::cerr << "Missing tail/head node for edge " << dotEdge->id() << "\n";
      continue;
    }

    auto *edge = tailNode->findDestEdge(headNode);
    if (! edge) edge = tailNode->findSrcEdge(headNode);
    if (! edge) {
      std::cerr << "Edge " << dotEdge->id() << " not Found\n";
      continue;
    }

    int nl = dotEdge->lines().size();

    if (nl == 1) {
      auto path = dotEdge->lines()[0].path;

      auto len = path.length();
      //std::cerr << "len=" << len << "\n";
      auto delta = len/100.0;
      //std::cerr << "delta=" << delta << "\n";

      auto points = CQChartsPath::pathPoints(path);

      int   it = -1, ih = -1;
      Point pit, pih;
      bool  pitValid = true, pihValid = true;

      if (tailNode->rect().isValid()) { // from
        (void) CQChartsUtil::nearestPointListPoint(points, tailNode->rect().getCenter(), it);

        if (tailNode->shapeType() == Node::ShapeType::CIRCLE ||
            tailNode->shapeType() == Node::ShapeType::OVAL)
          CQChartsGeom::lineIntersectCircle(tailNode->rect(),
            tailNode->rect().getCenter(), points[it], pit);
        else
          CQChartsGeom::lineIntersectRect(tailNode->rect(),
            tailNode->rect().getCenter(), points[it], pit);

        auto d = CQChartsUtil::PointPointDistance(points[it], pit);
        //std::cerr << "d=" << d << "\n";
        pitValid = (d > delta);
      }

      if (headNode->rect().isValid()) { // to
        (void) CQChartsUtil::nearestPointListPoint(points, headNode->rect().getCenter(), ih);

        if (headNode->shapeType() == Node::ShapeType::CIRCLE ||
            headNode->shapeType() == Node::ShapeType::OVAL)
          CQChartsGeom::lineIntersectCircle(headNode->rect(),
            headNode->rect().getCenter(), points[ih], pih);
        else
          CQChartsGeom::lineIntersectRect(headNode->rect(),
            headNode->rect().getCenter(), points[ih], pih);

        auto d = CQChartsUtil::PointPointDistance(points[ih], pih);
        //std::cerr << "d=" << d << "\n";
        pihValid = (d > delta);
      }

      QPainterPath path1;

      if (it >= 0 && ih >= 0) {
        if (it < ih) {
          //addPathPoint(path1, tailNode->rect().getCenter());

          if (pitValid)
            addPathPoint(path1, pit);

          for (int i = it; i <= ih; ++i)
            addPathPoint(path1, points[i]);

          if (pihValid)
            addPathPoint(path1, pih);

          //addPathPoint(path1, headNode->rect().getCenter());
        }
        else {
          //addPathPoint(path1, headNode->rect().getCenter());

          if (pihValid)
            addPathPoint(path1, pih);

          for (int i = ih; i <= it; ++i)
            addPathPoint(path1, points[i]);

          if (pitValid)
            addPathPoint(path1, pit);

          //addPathPoint(path1, tailNode->rect().getCenter());
        }
      }
      else
        path1 = path;

      auto str = CQChartsPath::pathToString(path1);

      //std::cerr << "Edge " << tailNode->name().toStdString() << "->" <<
      //                        headNode->name().toStdString() << " = " <<
      //                        str.toStdString() << "\n";

      edge->setPath(path1);
    }
    else {
      //std::cerr << "edge " << dotEdge->id() << " has " << nl << " lines\n";
    }

    if (dot.isDirected())
      edge->setDirected(true);
  }

  auto *th = const_cast<CQChartsDotPlot *>(this);

  th->bbox_ = bbox; // current

  th->fitToBBox(targetBBox_);
}

void
CQChartsDotPlot::
fitToBBox(const BBox &bbox)
{
  if (! bbox_.isValid() || ! bbox.isValid())
    return;

  // current
  double x1 = bbox_.getXMin  ();
  double y1 = bbox_.getYMin  ();
  double w1 = bbox_.getWidth ();
  double h1 = bbox_.getHeight();

  // target
  double x2 = bbox.getXMin  ();
  double y2 = bbox.getYMin  ();
  double w2 = bbox.getWidth ();
  double h2 = bbox.getHeight();

  double xf = w2/w1;
  double yf = h2/h1;

  double f = std::min(xf, yf);

  BBox bboxNew;

  //----

  using EdgeDelta = std::map<Edge *, QPointF>;

  EdgeDelta edgeDelta;

  for (auto *edge : edges_) {
    auto path = edge->path();

    if (path.isEmpty())
      continue;

    auto prect = path.boundingRect();

    auto *srcNode = edge->srcNode();
    auto  srcRect = srcNode->rect();

    if (! srcRect.isValid())
      continue;

    double ex = (prect.center().x() - srcRect.getXMid())*f;
    double ey = (prect.center().y() - srcRect.getYMid())*f;

    edgeDelta[edge] = QPointF(ex, ey);

    auto scaledPath = CQChartsPath::scalePath(path, f, f);

    edge->setPath(scaledPath);
  }

  //---

  // move scale nodes to match target bbox
  for (auto *node : nodes_) {
    if (! node->isVisible()) continue;

    if (! node->rect().isValid())
      continue;

    auto nodeRect = node->rect();

    double nx1 = x2 + (nodeRect.getXMin() - x1)*f;
    double ny1 = y2 + (nodeRect.getYMin() - y1)*f;

    node->scale(f, f);

    nodeRect = node->rect();

    double dx = nx1 - nodeRect.getXMin();
    double dy = ny1 - nodeRect.getYMin();

    node->moveBy(Point(dx, dy));

    bboxNew += node->rect();
  }

  //----

  // center at 0, 0 after fit
  double dx = bbox.getXMid() - bboxNew.getXMid();
  double dy = bbox.getYMid() - bboxNew.getYMid();

  for (auto *node : nodes_) {
    if (! node->isVisible()) continue;

    if (! node->rect().isValid())
      continue;

    node->moveBy(Point(dx, dy));
  }

  //---

  for (auto *edge : edges_) {
    if (edge->path().isEmpty())
      continue;

    auto path = edge->path();

    auto rect = path.boundingRect();

    auto d = edgeDelta[edge];

    auto *srcNode = edge->srcNode();

    if (! srcNode->rect().isValid())
      continue;

    auto srcRect = srcNode->rect();

    double ex = srcRect.getXMid() + d.x();
    double ey = srcRect.getYMid() + d.y();

    auto movedPath = CQChartsPath::movePath(path, ex - rect.center().x(), ey - rect.center().y());

    edge->setPath(movedPath);
  }
}

//------

bool
CQChartsDotPlot::
initHierObjs() const
{
  CQPerfTrace trace("CQChartsDotPlot::initHierObjs");

  return CQChartsConnectionPlot::initHierObjs();
}

void
CQChartsDotPlot::
initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                              const HierConnectionData &destHierData) const
{
  int srcDepth = srcHierData.linkStrs.size();

  Node *srcNode  = nullptr;
  Node *destNode = nullptr;

  initHierObjsAddConnection(srcHierData.parentStr, destHierData.parentStr, srcDepth,
                            destHierData.total, srcNode, destNode);

  if (srcNode) {
    QString srcStr;

    if (! srcHierData.linkStrs.empty())
      srcStr = srcHierData.linkStrs.back();

    srcNode->setValue(OptReal(destHierData.total));
    srcNode->setName (srcStr);
  }
}

void
CQChartsDotPlot::
initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                              const HierConnectionData &destHierData) const
{
  int srcDepth = srcHierData.linkStrs.size();

  Node *srcNode  = nullptr;
  Node *destNode = nullptr;

  initHierObjsAddConnection(srcHierData.parentStr, destHierData.parentStr, srcDepth,
                            destHierData.total, srcNode, destNode);

  if (destNode) {
    QString destStr;

    if (! destHierData.linkStrs.empty())
      destStr = destHierData.linkStrs.back();

    destNode->setValue(OptReal(destHierData.total));
    destNode->setName (destStr);
  }
}

void
CQChartsDotPlot::
initHierObjsAddConnection(const QString &srcStr, const QString &destStr, int srcDepth,
                          double value, Node* &srcNode, Node* &destNode) const
{
  int destDepth = srcDepth + 1;

  if (maxDepth() <= 0 || srcDepth <= maxDepth())
    srcNode = findNode(srcStr);

  if (maxDepth() <= 0 || destDepth <= maxDepth())
    destNode = findNode(destStr);

  //---

  // create edge and link src/dest nodes
  auto *edge = (srcNode && destNode ? createEdge(OptReal(value), srcNode, destNode) : nullptr);

  if (edge) {
    srcNode ->addDestEdge(edge);
    destNode->addSrcEdge (edge);
  }

  //---

  // set node depths
  if (srcNode)
    srcNode->setDepth(srcDepth);

  if (destNode)
    destNode->setDepth(destDepth);
}

//---

bool
CQChartsDotPlot::
initPathObjs() const
{
  CQPerfTrace trace("CQChartsDotPlot::initPathObjs");

  //---

  auto *th = const_cast<CQChartsDotPlot *>(this);

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
CQChartsDotPlot::
addPathValue(const PathData &pathData) const
{
  int n = pathData.pathStrs.length();
  assert(n > 0);

  auto *th = const_cast<CQChartsDotPlot *>(this);

  th->maxNodeDepth_ = std::max(maxNodeDepth_, n - 1);

  auto separator = calcSeparator();

  auto path1 = pathData.pathStrs[0];

  for (int i = 1; i < n; ++i) {
    auto path2 = path1 + separator + pathData.pathStrs[i];

    auto *srcNode  = findNode(path1);
    auto *destNode = findNode(path2);

    srcNode ->setLabel(pathData.pathStrs[i - 1]);
    destNode->setLabel(pathData.pathStrs[i    ]);

    srcNode ->setDepth(i - 1);
    destNode->setDepth(i    );

    if (i < n - 1) {
      bool hasEdge = srcNode->hasDestNode(destNode);

      if (! hasEdge) {
        // create edge and link src/dest nodes
        auto *edge = createEdge(OptReal(), srcNode, destNode);

        srcNode ->addDestEdge(edge);
        destNode->addSrcEdge (edge);
      }
    }
    else {
      // create edge and link src/dest nodes
      auto *edge = createEdge(OptReal(), srcNode, destNode);

      srcNode ->addDestEdge(edge);
      destNode->addSrcEdge (edge);

      //---

      // add model indices
      auto addModelInd = [&](const ModelIndex &modelInd) {
        if (modelInd.isValid())
          edge->addModelInd(modelInd);
      };

      addModelInd(pathData.pathModelInd );
      addModelInd(pathData.valueModelInd);

      //---

      // set destination node value (will be propagated up)
      destNode->setValue(pathData.value);
    }

    path1 = path2;
  }
}

void
CQChartsDotPlot::
propagatePathValues()
{
  // propagate node value up through edges and parent nodes
  for (int depth = maxNodeDepth_; depth >= 0; --depth) {
    for (const auto &node : nodes_) {
      if (node->depth() != depth) continue;

      // set node value from sum of dest values
      if (! node->hasValue()) {
        if (! node->destEdges().empty()) {
          OptReal sum;

          for (const auto &edge : node->destEdges()) {
            if (edge->hasValue()) {
              double value = edge->value().real();

              if (sum.isSet())
                sum = OptReal(sum.real() + value);
              else
                sum = OptReal(value);
            }
          }

          if (sum.isSet())
            node->setValue(sum);
        }
      }

      // propagate set node value up to source nodes
      if (node->hasValue()) {
        if (! node->srcEdges().empty()) {
          //int ns = node->srcEdges().size();

          double value = node->value().realOr(1.0);

          for (auto &srcEdge : node->srcEdges()) {
            if (! srcEdge->hasValue())
              srcEdge->setValue(OptReal(value));

            auto *srcNode = srcEdge->srcNode();

            for (const auto &edge : srcNode->destEdges()) {
              if (edge->destNode() == node)
                edge->setValue(OptReal(value));
            }
          }
        }
      }
    }
  }
}

//---

bool
CQChartsDotPlot::
initFromToObjs() const
{
  CQPerfTrace trace("CQChartsDotPlot::initFromToObjs");

  return CQChartsConnectionPlot::initFromToObjs();
}

void
CQChartsDotPlot::
addFromToValue(const FromToData &fromToData) const
{
  // get src node
  auto *srcNode = findNode(fromToData.fromStr);

  if (fromToData.depth > 0)
    srcNode->setDepth(fromToData.depth);

  //---

  // set group
  if (fromToData.groupData.ng > 1) {
    srcNode->setGroup(fromToData.groupData.ig, fromToData.groupData.ng);

    auto *th = const_cast<CQChartsDotPlot *>(this);

    th->numGroups_ = std::max(numGroups_, fromToData.groupData.ng);
  }
  else
    srcNode->setGroup(-1);

  //---

  // Just node
  if (fromToData.toStr == "") {
    // set node color (if color column specified)
    Color c;

    if (colorColumnColor(fromToData.fromModelInd.row(), fromToData.fromModelInd.parent(), c))
      srcNode->setFillColor(c);

    //---

    // set node name values (attribute column)
    processNodeNameValues(srcNode, fromToData.nameValues);

    //---

    auto groupNames = fromToData.groupData.value.toString().split("/");

    int graphId = -1, parentGraphId = -1;

    int ng = groupNames.length();

    if (ng > 0) {
      auto *th = const_cast<CQChartsDotPlot *>(this);

      graphId = th->groupValueInd_.calcId(groupNames[ng - 1]);

      if (ng > 1)
        parentGraphId = th->groupValueInd_.calcId(groupNames[ng - 2]);
    }

    if (graphId >= 0)
      srcNode->setGraphId(graphId);

    if (parentGraphId >= 0)
      srcNode->setParentGraphId(parentGraphId);
  }
  else {
    if (fromToData.fromStr == fromToData.toStr)
      return;

    auto *destNode = findNode(fromToData.toStr);

    if (fromToData.depth > 0)
      destNode->setDepth(fromToData.depth + 1);

    //---

    // create edge and link src/dest nodes
    auto *edge = createEdge(fromToData.value, srcNode, destNode);

    srcNode ->addDestEdge(edge, /*primary*/true );
    destNode->addSrcEdge (edge, /*primary*/false);

    //---

    // add model indices
    auto addModelInd = [&](const ModelIndex &modelInd) {
      if (modelInd.isValid())
        edge->addModelInd(modelInd);
    };

    addModelInd(fromToData.fromModelInd  );
    addModelInd(fromToData.toModelInd    );
    addModelInd(fromToData.valueModelInd );
    addModelInd(fromToData.depthModelInd );

    edge->setNamedColumn("From" , fromToData.fromModelInd  .column());
    edge->setNamedColumn("To"   , fromToData.toModelInd    .column());
    edge->setNamedColumn("Value", fromToData.valueModelInd .column());
    edge->setNamedColumn("Depth", fromToData.depthModelInd .column());

    //---

    // set edge color (if color column specified)
    Color c;

    if (colorColumnColor(fromToData.fromModelInd.row(), fromToData.fromModelInd.parent(), c))
      edge->setFillColor(c);

    //---

    // set edge name values (attribute column)
    processEdgeNameValues(edge, fromToData.nameValues);
  }
}

//---

bool
CQChartsDotPlot::
initLinkObjs() const
{
  CQPerfTrace trace("CQChartsDotPlot::initLinkObjs");

  return CQChartsConnectionPlot::initLinkObjs();
}

void
CQChartsDotPlot::
addLinkConnection(const LinkConnectionData &linkConnectionData) const
{
  // get src/dest nodes (TODO: allow single source node)
  auto *srcNode  = findNode(linkConnectionData.srcStr);
  auto *destNode = findNode(linkConnectionData.destStr);
//assert(srcNode != destNode);

  //---

  // create edge and link src/dest nodes
  auto *edge = createEdge(linkConnectionData.value, srcNode, destNode);

  srcNode ->addDestEdge(edge);
  destNode->addSrcEdge (edge);

  //---

  // add model indices
  auto addModelInd = [&](const ModelIndex &modelInd) {
    if (modelInd.isValid())
      edge->addModelInd(modelInd);
  };

  addModelInd(linkConnectionData.groupModelInd);
  addModelInd(linkConnectionData.linkModelInd );
  addModelInd(linkConnectionData.valueModelInd);
  addModelInd(linkConnectionData.nameModelInd );
  addModelInd(linkConnectionData.depthModelInd);

  //---

  // set edge color (if color column specified)
  Color c;

  if (colorColumnColor(linkConnectionData.linkModelInd.row(),
                       linkConnectionData.linkModelInd.parent(), c))
    edge->setFillColor(c);

  //---

  // set value on dest node (NEEDED ?)
  destNode->setValue(linkConnectionData.value);

  //---

  // set group
  if (linkConnectionData.groupData.isValid()) {
    srcNode->setGroup(linkConnectionData.groupData.ig, linkConnectionData.groupData.ng);

    auto *th = const_cast<CQChartsDotPlot *>(this);

    th->numGroups_ = std::max(numGroups_, linkConnectionData.groupData.ng);
  }
  else
    srcNode->setGroup(-1);

  //---

  // set name index
  if (linkConnectionData.nameModelInd.isValid()) {
    auto nameModelInd1 = normalizeIndex(linkConnectionData.nameModelInd);

    srcNode->setInd(nameModelInd1);
  }

  //---

  // set depth
  if (linkConnectionData.depth > 0) {
    srcNode ->setDepth(linkConnectionData.depth);
    destNode->setDepth(linkConnectionData.depth + 1);
  }

  //---

  // set edge name values (attribute column)
  processEdgeNameValues(edge, linkConnectionData.nameValues);
}

//---

bool
CQChartsDotPlot::
initConnectionObjs() const
{
  CQPerfTrace trace("CQChartsDotPlot::initConnectionObjs");

  return CQChartsConnectionPlot::initConnectionObjs();
}

void
CQChartsDotPlot::
addConnectionObj(int id, const ConnectionsData &connectionsData, const NodeIndex &) const
{
  // get src node
  auto srcStr = QString::number(id);

  auto *srcNode = findNode(srcStr);

  srcNode->setName(connectionsData.name);

  //---

  // set group
  if (connectionsData.groupData.isValid()) {
    srcNode->setGroup(connectionsData.groupData.ig, connectionsData.groupData.ng);

    auto *th = const_cast<CQChartsDotPlot *>(this);

    th->numGroups_ = std::max(numGroups_, connectionsData.groupData.ng);
  }
  else
    srcNode->setGroup(-1);

  //---

  // add model indices
  auto addModelInd = [&](const ModelIndex &modelInd) {
    if (modelInd.isValid())
      srcNode->addModelInd(modelInd);
  };

  addModelInd(connectionsData.nodeModelInd);
  addModelInd(connectionsData.connectionsModelInd);
  addModelInd(connectionsData.nameModelInd);
  addModelInd(connectionsData.groupModelInd);

  //---

  // set node name values (attribute column)
  processNodeNameValues(srcNode, connectionsData.nameValues);

  //---

  // process connections
  for (const auto &connection : connectionsData.connections) {
    // get destination node
    auto destStr = QString::number(connection.node);

    auto *destNode = findNode(destStr);

    //---

    // create edge and link src/dest nodes
    auto *edge = createEdge(OptReal(connection.value), srcNode, destNode);

    srcNode ->addDestEdge(edge);
    destNode->addSrcEdge (edge);

    //---

    // set value
    destNode->setValue(OptReal(connection.value));
  }
}

//---

bool
CQChartsDotPlot::
initTableObjs() const
{
  CQPerfTrace trace("CQChartsDotPlot::initTableObjs");

  //---

  TableConnectionDatas tableConnectionDatas;
  TableConnectionInfo  tableConnectionInfo;

  if (! processTableModel(tableConnectionDatas, tableConnectionInfo))
    return false;

  //---

  int nv = tableConnectionDatas.size();

  for (int row = 0; row < nv; ++row) {
    const auto &tableConnectionData = tableConnectionDatas[row];

    if (tableConnectionData.values().empty())
      continue;

    auto srcStr = QString::number(tableConnectionData.from());

    auto *srcNode = findNode(srcStr);

    srcNode->setName (tableConnectionData.name());
    srcNode->setGroup(tableConnectionData.group().ig, tableConnectionData.group().ng);

    for (const auto &value : tableConnectionData.values()) {
      auto destStr = QString::number(value.to);

      auto *destNode = findNode(destStr);

      // create edge and link src/dest nodes
      auto *edge = createEdge(OptReal(value.value), srcNode, destNode);

      srcNode ->addDestEdge(edge);
      destNode->addSrcEdge (edge);
    }
  }

  //---

  return true;
}

//---

void
CQChartsDotPlot::
processNodeNameValues(Node *node, const NameValues &nameValues) const
{
  for (const auto &nv : nameValues.nameValues()) {
    const auto &name  = nv.first;
    auto        value = nv.second.toString();

    processNodeNameValue(node, name, value);
  }
}

void
CQChartsDotPlot::
processNodeNameValue(Node *node, const QString &name, const QString &value) const
{
  // shape
  if      (name == "shape") {
    Node::ShapeType shapeType;

    stringToShapeType(value, shapeType);

    node->setShapeType(shapeType);
  }
  // num sides
  else if (name == "num_sides") {
    bool ok;

    int n = value.toInt(&ok);

    if (ok)
      node->setNumSides(n);
  }
  else if (name == "label") {
    node->setLabel(value);
  }
  else if (name == "fill_color" || name == "color") {
    node->setFillColor(CQChartsColor(value));
  }
  else if (name == "fill_alpha" || name == "alpha") {
    node->setFillAlpha(CQChartsAlpha(value));
  }
  else if (name == "fill_pattern" || name == "pattern") {
    node->setFillPattern(CQChartsFillPattern(value));
  }
  else if (name == "stroke_color") {
    node->setStrokeColor(CQChartsColor(value));
  }
  else if (name == "stroke_alpha") {
    node->setStrokeAlpha(CQChartsAlpha(value));
  }
  else if (name == "stroke_width" || name == "width") {
    node->setStrokeWidth(CQChartsLength(value));
  }
  else if (name == "stroke_dash" || name == "dash") {
    node->setStrokeDash(CQChartsLineDash(value));
  }
}

void
CQChartsDotPlot::
processEdgeNameValues(Edge *edge, const NameValues &nameValues) const
{
  auto *srcNode  = edge->srcNode ();
  auto *destNode = edge->destNode();

  for (const auto &nv : nameValues.nameValues()) {
    const auto &name  = nv.first;
    auto        value = nv.second.toString();

    if      (name == "shape") {
      Edge::ShapeType shapeType;

      stringToShapeType(value, shapeType);

      edge->setShapeType(shapeType);
    }
    else if (name == "label") {
      edge->setLabel(value);
    }
    else if (name == "color") {
      edge->setFillColor(CQChartsColor(value));
    }
    else if (name.left(4) == "src_") {
      processNodeNameValue(srcNode, name.mid(4), value);
    }
    else if (name.left(5) == "dest_") {
      processNodeNameValue(destNode, name.mid(5), value);
    }
  }
}

void
CQChartsDotPlot::
stringToShapeType(const QString &str, Node::ShapeType &shapeType)
{
  if      (str == "box"            ) shapeType = Node::ShapeType::BOX;
  else if (str == "polygon"        ) shapeType = Node::ShapeType::POLYGON;
  else if (str == "ellipse"        ) shapeType = Node::ShapeType::CIRCLE;
  else if (str == "oval"           ) shapeType = Node::ShapeType::CIRCLE;
  else if (str == "circle"         ) shapeType = Node::ShapeType::CIRCLE;
//else if (str == "point"          ) shapeType = Node::ShapeType::POINT;
//else if (str == "egg"            ) shapeType = Node::ShapeType::EGG;
  else if (str == "triangle"       ) shapeType = Node::ShapeType::TRIANGLE;
  else if (str == "plaintext"      ) shapeType = Node::ShapeType::PLAIN_TEXT;
//else if (str == "plain"          ) shapeType = Node::ShapeType::PLAIN;
  else if (str == "diamond"        ) shapeType = Node::ShapeType::DIAMOND;
//else if (str == "trapezium"      ) shapeType = Node::ShapeType::TRAPEZIUM;
//else if (str == "parallelogram"  ) shapeType = Node::ShapeType::PARALLELGRAM;
//else if (str == "house"          ) shapeType = Node::ShapeType::HOUSE;
//else if (str == "pentagon"       ) shapeType = Node::ShapeType::PENTAGON;
//else if (str == "hexagon"        ) shapeType = Node::ShapeType::HEXAGON;
//else if (str == "septagon"       ) shapeType = Node::ShapeType::SEPTAGON;
//else if (str == "octagon"        ) shapeType = Node::ShapeType::OCTAGON;
  else if (str == "doublecircle"   ) shapeType = Node::ShapeType::DOUBLE_CIRCLE;
//else if (str == "doubleoctagon"  ) shapeType = Node::ShapeType::DOUBLE_OCTAGON;
//else if (str == "tripleoctagon"  ) shapeType = Node::ShapeType::TRIPLE_OCTAGON;
//else if (str == "invtriangle"    ) shapeType = Node::ShapeType::INV_TRIANGLE;
//else if (str == "invtrapezium"   ) shapeType = Node::ShapeType::INV_TRAPEZIUM;
//else if (str == "invhouse"       ) shapeType = Node::ShapeType::INV_HOUSE;
//else if (str == "Mdiamond"       ) shapeType = Node::ShapeType::MDIAMOND;
//else if (str == "Msquare"        ) shapeType = Node::ShapeType::MSQUARE;
//else if (str == "Mcircle"        ) shapeType = Node::ShapeType::MCIRCLE;
  else if (str == "rect"           ) shapeType = Node::ShapeType::BOX;
  else if (str == "rectangle"      ) shapeType = Node::ShapeType::BOX;
  else if (str == "square"         ) shapeType = Node::ShapeType::BOX;
//else if (str == "star"           ) shapeType = Node::ShapeType::STAR;
//else if (str == "underline"      ) shapeType = Node::ShapeType::UNDERLINE;
//else if (str == "cylinder"       ) shapeType = Node::ShapeType::CYLINDER;
//else if (str == "note"           ) shapeType = Node::ShapeType::NOTE;
//else if (str == "tab"            ) shapeType = Node::ShapeType::TAB;
//else if (str == "folder"         ) shapeType = Node::ShapeType::FOLDER;
//else if (str == "box3d"          ) shapeType = Node::ShapeType::BOX3D;
//else if (str == "component"      ) shapeType = Node::ShapeType::COMPONENT;
//else if (str == "promoter"       ) shapeType = Node::ShapeType::PROMOTER;
//else if (str == "cds"            ) shapeType = Node::ShapeType::CDS;
//else if (str == "terminator"     ) shapeType = Node::ShapeType::TERMINATOR;
//else if (str == "utr"            ) shapeType = Node::ShapeType::UTR;
//else if (str == "primersite"     ) shapeType = Node::ShapeType::PRIMERSITE;
//else if (str == "restrictionsite") shapeType = Node::ShapeType::RESTRICTIONSITE;
//else if (str == "fivepoverhang"  ) shapeType = Node::ShapeType::FIVEPOVERHANG;
//else if (str == "threepoverhang" ) shapeType = Node::ShapeType::THREEPOVERHANG;
//else if (str == "noverhang"      ) shapeType = Node::ShapeType::NOVERHANG;
//else if (str == "assembly"       ) shapeType = Node::ShapeType::ASSEMBLY;
//else if (str == "signature"      ) shapeType = Node::ShapeType::SIGNATURE;
//else if (str == "insulator"      ) shapeType = Node::ShapeType::INSULATOR;
//else if (str == "ribosite"       ) shapeType = Node::ShapeType::RIBOSITE;
//else if (str == "rnastab"        ) shapeType = Node::ShapeType::RNASTAB;
//else if (str == "proteasesite"   ) shapeType = Node::ShapeType::PROTEASESITE;
//else if (str == "proteinstab"    ) shapeType = Node::ShapeType::PROTEINSTAB;
  else if (str == "record"         ) shapeType = Node::ShapeType::RECORD;
  else if (str == "rpromoter"      ) shapeType = Node::ShapeType::RPROMOTER;
  else if (str == "rarrow"         ) shapeType = Node::ShapeType::RARROW;
//else if (str == "larrow"         ) shapeType = Node::ShapeType::LARROW;
//else if (str == "lpromoter"      ) shapeType = Node::ShapeType::LPROMOTER;
  else if (str == "none"           ) shapeType = Node::ShapeType::NONE;
  else {
    std::cerr << "Unhandled shape type " << str.toStdString() << "\n";
    shapeType = Node::ShapeType::NONE;
  }
}

QString
CQChartsDotPlot::
shapeTypeToString(const Node::ShapeType &shapeType)
{
  switch (shapeType) {
    case Node::ShapeType::DIAMOND      : return "diamond";
    case Node::ShapeType::BOX          : return "box";
    case Node::ShapeType::POLYGON      : return "polygon";
    case Node::ShapeType::CIRCLE       : return "circle";
    case Node::ShapeType::DOUBLE_CIRCLE: return "doublecircle";
    case Node::ShapeType::RECORD       : return "record";
    case Node::ShapeType::PLAIN_TEXT   : return "plaintext";
    case Node::ShapeType::RARROW       : return "rarrow";
    case Node::ShapeType::RPROMOTER    : return "rpromoter";
    default                            : return "none";
  }
}

void
CQChartsDotPlot::
stringToShapeType(const QString &str, Edge::ShapeType &shapeType)
{
  if (str == "arrow" ) shapeType = Edge::ShapeType::ARROW;
  else                 shapeType = Edge::ShapeType::NONE;
}

QString
CQChartsDotPlot::
shapeTypeToString(const Edge::ShapeType &shapeType)
{
  switch (shapeType) {
    case Edge::ShapeType::ARROW: return "arrow";
    default                    : return "none";
  }
}

//---

void
CQChartsDotPlot::
filterObjs()
{
  // hide nodes below depth
  if (maxDepth() > 0) {
    for (const auto &node : nodes_) {
      if (node->depth() > maxDepth())
        node->setVisible(false);
    }
  }

  // hide nodes less than min value
  if (minValue() > 0) {
    for (const auto &node : nodes_) {
      if (! node->value().isSet() || node->value().real() < minValue())
        node->setVisible(false);
    }
  }
}

//---

bool
CQChartsDotPlot::
addMenuItems(QMenu *menu)
{
  if (canDrawColorMapKey()) {
    menu->addSeparator();

    addColorMapKeyItems(menu);
  }

  return true;
}

//---

CQChartsDotPlot::NodeObj *
CQChartsDotPlot::
createObjFromNode(Node *node) const
{
//int numNodes = graph->nodes().size(); // node id needs to be per graph
  int numNodes = this->numNodes();

  ColorInd iv(node->id(), numNodes);

  if (! node->rect().isValid())
    node->setRect(BBox(Point(0, 0), Point(1, 1)));

  auto *nodeObj = createNodeObj(node->rect(), node, iv);

  NodeObj::ShapeType shapeType = (NodeObj::ShapeType) node->shapeType();

  if (shapeType == NodeObj::ShapeType::NONE)
    shapeType = (NodeObj::ShapeType) nodeShape();

  nodeObj->setShapeType(shapeType);
  nodeObj->setHierName (node->str());

  for (const auto &modelInd : node->modelInds())
    nodeObj->addModelInd(normalizedModelIndex(modelInd));

  auto *pnode = dynamic_cast<CQChartsDotPlotNode *>(node);
  assert(pnode);

  pnode->setObj(nodeObj);

  return nodeObj;
}

CQChartsDotEdgeObj *
CQChartsDotPlot::
addEdgeObj(Edge *edge) const
{
  assert(bbox_.isValid());

  double xm = bbox_.getHeight()*edgeMargin_;
  double ym = bbox_.getWidth ()*edgeMargin_;

  BBox nodeRect;

  nodeRect += edge->srcNode ()->rect();
  nodeRect += edge->destNode()->rect();

  if (! nodeRect.isValid())
    return nullptr;

  BBox rect(nodeRect.getXMin() - xm, nodeRect.getYMin() - ym,
            nodeRect.getXMax() + xm, nodeRect.getYMax() + ym);

  auto *edgeObj = createEdgeObj(rect, edge);

  EdgeObj::ShapeType shapeType = (EdgeObj::ShapeType) edge->shapeType();

  if (shapeType == EdgeObj::ShapeType::NONE)
    shapeType = (EdgeObj::ShapeType) edgeShape();

  edgeObj->setShapeType(shapeType);

  auto *pedge = dynamic_cast<CQChartsDotPlotEdge *>(edge);
  assert(pedge);

  pedge->setObj(edgeObj);

  return edgeObj;
}

//---

CQChartsDotPlotNode *
CQChartsDotPlot::
findNode(const QString &name) const
{
  auto p = nameNodeMap_.find(name);

  if (p != nameNodeMap_.end())
    return (*p).second;

  return createNode(name);
}

CQChartsDotPlotNode *
CQChartsDotPlot::
findDotNode(int dotId) const
{
  for (const auto &node : nodes())
    if (node->dotId() == dotId)
     return node;

  return nullptr;
}

CQChartsDotPlotNode *
CQChartsDotPlot::
createNode(const QString &name) const
{
  auto *node = new CQChartsDotPlotNode(name);

  node->setName(name);

  node->setId(nameNodeMap_.size());

  auto *th = const_cast<CQChartsDotPlot *>(this);

  auto p1 = th->nameNodeMap_.insert(th->nameNodeMap_.end(),
              NameNodeMap::value_type(node->str(), node));
  assert(node == (*p1).second);

  th->indNodeMap_[node->id()] = node;

  //---

  th->nodes_.push_back(node);

  return node;
}

CQChartsDotPlotEdge *
CQChartsDotPlot::
createEdge(const OptReal &value, Node *srcNode, Node *destNode) const
{
  auto *edge = new CQChartsDotPlotEdge(value, srcNode, destNode);

  edge->setId(edges_.size());

  auto *th = const_cast<CQChartsDotPlot *>(this);

  th->edges_.push_back(edge);

  return edge;
}

//---

bool
CQChartsDotPlot::
keyPress(int key, int modifier)
{
  if (key == Qt::Key_F) {
    bbox_ = nodesBBox(); // current

    fitToBBox(targetBBox_);

    drawObjs();
  }
  else if (key == Qt::Key_S) {
    printStats();
  }
  else {
    return CQChartsPlot::keyPress(key, modifier);
  }

  return true;
}

void
CQChartsDotPlot::
printStats()
{
  using NameData = std::map<QString, QString>;
  using NodeData = std::map<Node *, NameData>;

  NodeData nodeData;

  for (auto *node : nodes_) {
    if (! node->isVisible()) continue;

    if (node->srcEdges().empty())
      nodeData[node]["No Src" ] = "";

    if (node->destEdges().empty())
      nodeData[node]["No Dest"] = "";
  }

  for (auto &pn : nodeData) {
    auto       *node     = pn.first;
    const auto &nameData = pn.second;

    for (auto &pd : nameData) {
      std::cerr << node->name().toStdString() << " " << pd.first.toStdString() << "\n";
    }
  }
}

//---

CQChartsDotNodeObj *
CQChartsDotPlot::
createNodeObj(const BBox &rect, Node *node, const ColorInd &ind) const
{
  return new NodeObj(this, rect, node, ind);
}

CQChartsDotEdgeObj *
CQChartsDotPlot::
createEdgeObj(const BBox &rect, Edge *edge) const
{
  return new EdgeObj(this, rect, edge);
}

//---

bool
CQChartsDotPlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsDotPlot::
execDrawForeground(PaintDevice *device) const
{
  if (isColorMapKey())
    drawColorMapKey(device);
}

//---

CQChartsPlotCustomControls *
CQChartsDotPlot::
createCustomControls()
{
  auto *controls = new CQChartsDotPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsDotNodeObj::
CQChartsDotNodeObj(const Plot *plot, const BBox &rect, Node *node, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect, ColorInd(), ColorInd(), iv),
 plot_(plot), node_(node)
{
  setDetailHint(DetailHint::MAJOR);

  setEditable(true);
}

CQChartsDotNodeObj::
~CQChartsDotNodeObj()
{
  auto *pnode = dynamic_cast<CQChartsDotPlotNode *>(node_);

  if (pnode)
    pnode->setObj(nullptr);
}

QString
CQChartsDotNodeObj::
name() const
{
  return node()->name();
}

void
CQChartsDotNodeObj::
setName(const QString &s)
{
  node()->setName(s);
}

double
CQChartsDotNodeObj::
value() const
{
  return node()->value().realOr(0.0);
}

void
CQChartsDotNodeObj::
setValue(double r)
{
  node()->setValue(CQChartsDotPlotNode::OptReal(r));
}

int
CQChartsDotNodeObj::
depth() const
{
  return node()->depth();
}

void
CQChartsDotNodeObj::
setDepth(int depth)
{
  node()->setDepth(depth);
}

CQChartsDotNodeObj::ShapeType
CQChartsDotNodeObj::
shapeType() const
{
  return (CQChartsDotNodeObj::ShapeType) node()->shapeType();
}

void
CQChartsDotNodeObj::
setShapeType(const ShapeType &s)
{
  node()->setShapeType((CQChartsDotPlotNode::ShapeType) s);
}

int
CQChartsDotNodeObj::
numSides() const
{
  return node()->numSides();
}

void
CQChartsDotNodeObj::
setNumSides(int n)
{
  node()->setNumSides(n);
}

CQChartsColor
CQChartsDotNodeObj::
fillColor() const
{
  return node()->fillColor();
}

void
CQChartsDotNodeObj::
setFillColor(const Color &c)
{
  node()->setFillColor(c);
}

QString
CQChartsDotNodeObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(node()->id());
}

QString
CQChartsDotNodeObj::
calcTipId() const
{
  Edge *edge = nullptr;

  if      (! node()->srcEdges().empty())
    edge = *node()->srcEdges().begin();
  else if (! node()->destEdges().empty())
    edge = *node()->destEdges().begin();

  auto namedColumn = [&](const QString &name, const QString &defName="") {
    if (edge && edge->hasNamedColumn(name))
      return plot_->columnHeaderName(edge->namedColumn(name));

    auto headerName = (defName.length() ? defName : name);

    return headerName;
  };

  //---

  CQChartsTableTip tableTip;

  auto name = this->name();

  if (name == "")
    name = this->id();

  auto hierName = this->hierName();

  if (hierName != name)
    tableTip.addTableRow("Hier Name", hierName);

  tableTip.addTableRow("Name", name);

  if (node()->hasValue())
    tableTip.addTableRow(namedColumn("Value"), value());

  if (depth() >= 0)
    tableTip.addTableRow(namedColumn("Depth"), depth());

  int ns = node()->srcEdges ().size();
  int nd = node()->destEdges().size();

  tableTip.addTableRow("Edges", QString("In:%1, Out:%2").arg(ns).arg(nd));

  if (plot_->groupColumn().isValid())
    tableTip.addTableRow("Group", node()->group());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

void
CQChartsDotNodeObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  auto path1 = (path.length() ? path + "/" : ""); path1 += propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);

  model->addProperty(path1, this, "hierName" )->setDesc("Hierarchical Name");
  model->addProperty(path1, this, "name"     )->setDesc("Name");
  model->addProperty(path1, this, "value"    )->setDesc("Value");
  model->addProperty(path1, this, "depth"    )->setDesc("Depth");
  model->addProperty(path1, this, "shapeType")->setDesc("Shape type");
  model->addProperty(path1, this, "numSides" )->setDesc("Number of Shape Sides");
  model->addProperty(path1, this, "color"    )->setDesc("Color");
}

//---

void
CQChartsDotNodeObj::
moveBy(const Point &delta)
{
  //std::cerr << "  Move " << node()->str().toStdString() << " by " << delta.y << "\n";

  rect_.moveBy(delta);
}

void
CQChartsDotNodeObj::
scale(double fx, double fy)
{
  rect_.scale(fx, fy);
}

//---

bool
CQChartsDotNodeObj::
editPress(const Point &p)
{
  editChanged_ = false;

  editHandles()->setDragPos(p);

  return true;
}

bool
CQChartsDotNodeObj::
editMove(const Point &p)
{
  const auto &dragPos  = editHandles()->dragPos();
  const auto &dragSide = editHandles()->dragSide();

  if (dragSide != CQChartsResizeSide::MOVE)
    return false;

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  editHandles()->updateBBox(dx, dy);

  setEditBBox(editHandles()->bbox(), dragSide);

  editHandles()->setDragPos(p);

  //auto *graph = node()->graph();

  //if (graph)
  //  graph->updateRect();

  editChanged_ = true;

  const_cast<CQChartsDotPlot *>(plot())->drawObjs();

  return true;
}

bool
CQChartsDotNodeObj::
editMotion(const Point &p)
{
  return editHandles()->selectInside(p);
}

bool
CQChartsDotNodeObj::
editRelease(const Point &)
{
  if (editChanged_)
    const_cast<CQChartsDotPlot *>(plot())->invalidateObjTree();

  return true;
}

void
CQChartsDotNodeObj::
setEditBBox(const BBox &bbox, const CQChartsResizeSide &)
{
  assert(bbox.isSet());

  double dx = bbox.getXMin() - rect_.getXMin();
  double dy = bbox.getYMin() - rect_.getYMin();

  node()->moveBy(Point(dx, dy));
}

//---

CQChartsDotNodeObj::PlotObjs
CQChartsDotNodeObj::
getConnected() const
{
  PlotObjs plotObjs;

  for (auto &pedge : node()->srcEdges())
    plotObjs.push_back(pedge->obj());

  for (auto &pedge : node()->destEdges())
    plotObjs.push_back(pedge->obj());

  return plotObjs;
}

//---

void
CQChartsDotNodeObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw node
  if (rect().isSet()) {
    if      (shapeType() == ShapeType::DIAMOND)
      device->drawDiamond(rect());
    else if (shapeType() == ShapeType::BOX)
      device->drawRect(rect());
    else if (shapeType() == ShapeType::POLYGON)
      device->drawPolygonSides(rect(), numSides());
    else if (shapeType() == ShapeType::CIRCLE)
      device->drawEllipse(rect());
    else if (shapeType() == ShapeType::DOUBLE_CIRCLE) {
      auto rect = this->rect();

      double dx = rect.getWidth ()/10.0;
      double dy = rect.getHeight()/10.0;

      auto rect1 = rect.expanded(dx, dy, -dx, -dy);

      device->drawEllipse(rect );
      device->drawEllipse(rect1);
    }
    else
      device->drawRect(rect());
  }

  //---

  device->resetColorNames();
}

void
CQChartsDotNodeObj::
drawFg(PaintDevice *device) const
{
  if (! plot_->isTextVisible())
    return;

  auto prect = plot_->windowToPixel(rect());

  //---

  // set font
  plot_->setPainterFont(device, plot_->textFont());

  QFontMetricsF fm(device->font());

  //---

  // set text pen
  auto ic = calcColorInd();

  PenBrush penBrush;

  auto c = plot_->interpTextColor(ic);

  plot_->setPen(penBrush, PenData(true, c, plot_->textAlpha()));

  device->setPen(penBrush.pen);

  //---

  double textMargin = 4; // pixels

  auto str = node()->label();

  if (! str.length())
    str = node()->name();

  //---

  double ptw = fm.width(str);

  double clipLength = plot_->lengthPixelWidth(plot()->textClipLength());

  if (clipLength > 0.0)
    ptw = std::min(ptw, clipLength);

  double tw = plot_->pixelToWindowWidth(ptw);

  //---

  auto range = plot_->getCalcDataRange();

  double xm = (range.isSet() ? range.xmid() : 0.0);

  double tx = (rect().getXMid() < xm - tw ?
    prect.getXMax() + textMargin : prect.getXMin() - textMargin - ptw);
  double ty = prect.getYMid() + (fm.ascent() - fm.descent())/2;

  auto pt = plot_->pixelToWindow(Point(tx, ty));

  // only support contrast
  auto textOptions = plot_->textOptions(device);

  textOptions.angle = Angle();
  textOptions.align = Qt::AlignLeft;
  textOptions.html  = false;

  if (shapeType() == ShapeType::DIAMOND || shapeType() == ShapeType::BOX ||
      shapeType() == ShapeType::POLYGON || shapeType() == ShapeType::CIRCLE ||
      shapeType() == ShapeType::DOUBLE_CIRCLE) {
    textOptions.align = Qt::AlignHCenter | Qt::AlignVCenter;

    if (rect().isValid())
      CQChartsDrawUtil::drawTextInBox(device, rect(), str, textOptions);
  }
  else {
    textOptions.align = Qt::AlignLeft;

    CQChartsDrawUtil::drawTextAtPoint(device, pt, str, textOptions);
  }
}

void
CQChartsDotNodeObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // set fill and stroke
  auto ic = calcColorInd();

  QColor bc;

  if (node()->strokeColor().isValid())
    bc = plot_->interpColor(node()->strokeColor(), ic);
  else
    bc = plot_->interpNodeStrokeColor(ic);

  auto fc = calcFillColor();

  auto fillAlpha   = (node()->fillAlpha  ().isValid() ?
    node()->fillAlpha() : plot_->nodeFillAlpha());
  auto fillPattern = (node()->fillPattern().isValid() ?
    node()->fillPattern() : plot_->nodeFillPattern());

  auto strokeAlpha = (node()->strokeAlpha().isValid() ?
    node()->strokeAlpha() : plot_->nodeStrokeAlpha());
  auto strokeWidth = (node()->strokeWidth().isValid() ?
    node()->strokeWidth() : plot_->nodeStrokeWidth());
  auto strokeDash  = (node()->strokeDash ().isValid() ?
    node()->strokeDash () : plot_->nodeStrokeDash());

  plot_->setPenBrush(penBrush,
    plot_->nodePenData  (bc, strokeAlpha, strokeWidth, strokeDash),
    plot_->nodeBrushData(fc, fillAlpha, fillPattern));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

QColor
CQChartsDotNodeObj::
calcFillColor() const
{
  QColor fc;

  auto ic = calcColorInd();

  if (fillColor().isValid())
    fc = plot_->interpColor(fillColor(), ic);
  else
    fc = plot_->interpNodeFillColor(ic);

  return fc;
}

void
CQChartsDotNodeObj::
writeScriptData(ScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);
}

//------

CQChartsDotEdgeObj::
CQChartsDotEdgeObj(const Plot *plot, const BBox &rect, Edge *edge) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect), plot_(plot), edge_(edge)
{
  //setDetailHint(DetailHint::MAJOR);
}

CQChartsDotEdgeObj::
~CQChartsDotEdgeObj()
{
  auto *pedge = dynamic_cast<CQChartsDotPlotEdge *>(edge_);

  if (pedge)
    pedge->setObj(nullptr);
}

//---

CQChartsDotEdgeObj::ShapeType
CQChartsDotEdgeObj::
shapeType() const
{
  return (CQChartsDotEdgeObj::ShapeType) edge()->shapeType();
}

void
CQChartsDotEdgeObj::
setShapeType(const ShapeType &s)
{
  edge()->setShapeType((CQChartsDotPlotEdge::ShapeType) s);
}

//---

QString
CQChartsDotEdgeObj::
calcId() const
{
  auto *srcNode  = dynamic_cast<CQChartsDotPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsDotPlotNode *>(edge()->destNode());
  assert(srcNode && destNode);

  auto *srcObj  = srcNode ->obj();
  auto *destObj = destNode->obj();

  if (! srcObj || ! destObj)
    return QString("%1:%4").arg(typeName()).arg(edge()->id());

  return QString("%1:%2:%3:%4").arg(typeName()).
           arg(srcObj->calcId()).arg(destObj->calcId()).arg(edge()->id());
}

QString
CQChartsDotEdgeObj::
calcTipId() const
{
  auto namedColumn = [&](const QString &name, const QString &defName="") {
    if (edge()->hasNamedColumn(name))
      return plot_->columnHeaderName(edge()->namedColumn(name));

    auto headerName = (defName.length() ? defName : name);

    return headerName;
  };

  //---

  CQChartsTableTip tableTip;

  auto *srcNode  = dynamic_cast<CQChartsDotPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsDotPlotNode *>(edge()->destNode());
  assert(srcNode && destNode);

  auto *srcObj  = srcNode ->obj();
  auto *destObj = destNode->obj();

  auto srcName  = srcObj ->hierName();
  auto destName = destObj->hierName();

  if (srcName  == "") srcName  = srcObj ->id();
  if (destName == "") destName = destObj->id();

  //---

  tableTip.addTableRow(namedColumn("From"), srcName);
  tableTip.addTableRow(namedColumn("To"  ), destName);

  if (edge()->hasValue())
    tableTip.addTableRow(namedColumn("Value"), edge()->value().real());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

void
CQChartsDotEdgeObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  auto path1 = (path.length() ? path + "/" : ""); path1 += propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);

  model->addProperty(path1, this, "shapeType")->setDesc("Shape");
}

//---

bool
CQChartsDotEdgeObj::
inside(const Point &p) const
{
  return path_.contains(p.qpoint());
}

//---

CQChartsDotEdgeObj::PlotObjs
CQChartsDotEdgeObj::
getConnected() const
{
  PlotObjs plotObjs;

  auto *srcNode  = dynamic_cast<CQChartsDotPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsDotPlotNode *>(edge()->destNode());
  assert(srcNode && destNode);

  auto *srcObj  = srcNode ->obj();
  auto *destObj = destNode->obj();

  plotObjs.push_back(srcObj);
  plotObjs.push_back(destObj);

  return plotObjs;
}

//---

void
CQChartsDotEdgeObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  device->setColorNames();

  //---

  // get connection rect of source and destination object
  auto *srcNode  = dynamic_cast<CQChartsDotPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsDotPlotNode *>(edge()->destNode());
  assert(srcNode && destNode);

  auto *srcObj  = srcNode ->obj();
  auto *destObj = destNode->obj();

  bool isSelf = (srcObj == destObj);

  auto srcRect  = edge()->srcNode ()->rect();
  auto destRect = edge()->destNode()->rect();

  if (shapeType() == ShapeType::ARROW || ! plot_->isEdgeScaled()) {
    if (edge()->srcNode()->shapeType() == Node::ShapeType::DIAMOND ||
        edge()->srcNode()->shapeType() == Node::ShapeType::POLYGON ||
        edge()->srcNode()->shapeType() == Node::ShapeType::CIRCLE ||
        edge()->srcNode()->shapeType() == Node::ShapeType::DOUBLE_CIRCLE) {
      srcRect = srcObj->rect();
    }

    if (edge()->destNode()->shapeType() == Node::ShapeType::DIAMOND ||
        edge()->destNode()->shapeType() == Node::ShapeType::POLYGON ||
        edge()->destNode()->shapeType() == Node::ShapeType::CIRCLE ||
        edge()->destNode()->shapeType() == Node::ShapeType::DOUBLE_CIRCLE) {
      destRect = destObj->rect();
    }
  }

  if (! srcRect.isSet() || ! destRect.isSet())
    return;

  //---

  double x1, y1, x2, y2;

  bool swapped = false;

  if (plot_->isHorizontal()) {
    // x from right of source rect to left of dest rect
    x1 = srcRect .getXMax();
    x2 = destRect.getXMin();

    if (x1 > x2) {
      x1 = destRect.getXMax(), x2 = srcRect.getXMin();
      swapped = true;
    }
  }
  else {
    // y from top of source rect to bottom of dest rect
    y1 = srcRect .getYMax();
    y2 = destRect.getYMin();

    if (y1 > y2) {
      y1 = destRect.getYMax(), y2 = srcRect.getYMin();
      swapped = true;
    }
  }

  //---

  // draw edge
  double lw = plot_->lengthPlotHeight(plot()->edgeWidth());

  auto epath = edge()->path();

  if (! epath.isEmpty()) {
    if (edge()->isDirected()) {
      CQChartsArrowData arrowData;

      arrowData.setTHeadType(CQChartsArrowData::HeadType::ARROW);

      QPainterPath path1;

      CQChartsArrow::pathAddArrows(epath, arrowData, lw,
                                   plot_->arrowWidth(), plot_->arrowWidth(), path_);

      //path_ = epath;
    }
    else {
      path_ = epath;

      device->setBrush(Qt::NoBrush);
    }

    device->drawPath(path_);
  }
  else {
    if (shapeType() == ShapeType::ARROW) {
      const_cast<CQChartsDotPlot *>(plot())->setUpdatesEnabled(false);

      if (! isSelf) {
        QPainterPath lpath;

        CQChartsDrawUtil::curvePath(lpath, srcRect, destRect,
                                    plot_->orientation(), /*rectilinear*/true);

        CQChartsArrowData arrowData;

        arrowData.setFHeadType(CQChartsArrowData::HeadType::ARROW);
        arrowData.setTHeadType(CQChartsArrowData::HeadType::ARROW);

        CQChartsArrow::pathAddArrows(lpath, arrowData, lw,
                                     plot_->arrowWidth(), plot_->arrowWidth(), path_);
      }
      else
        CQChartsArrow::selfPath(path_, srcRect, /*fhead*/true, /*thead*/true, lw);

      device->drawPath(path_);

      const_cast<CQChartsDotPlot *>(plot())->setUpdatesEnabled(true);
    }
    else {
      if (plot_->isEdgeScaled()) {
        CQChartsDrawUtil::edgePath(path_, srcRect, destRect,
                                   /*isLine*/false, plot_->orientation());
      }
      else {
        double lw = plot_->lengthPlotHeight(plot()->edgeWidth()); // TODO: config

        if (! isSelf) {
          if (plot_->isHorizontal()) {
            // start y range from source node, and end y range from dest node
            y1 = srcRect .getYMid();
            y2 = destRect.getYMid();

            if (swapped)
              std::swap(y1, y2);

            CQChartsDrawUtil::edgePath(path_, Point(x1, y1), Point(x2, y2),
                                       lw, plot_->orientation());
          }
          else {
            // start x range from source node, and end x range from dest node
            x1 = srcRect .getXMid();
            x2 = destRect.getXMid();

            if (swapped)
              std::swap(x1, x2);

            CQChartsDrawUtil::edgePath(path_, Point(x1, y1), Point(x2, y2),
                                       lw, plot_->orientation());
          }
        }
        else {
          CQChartsDrawUtil::selfEdgePath(path_, srcRect, lw);
        }
      }

      device->drawPath(path_);
    }
  }

  device->resetColorNames();
}

void
CQChartsDotEdgeObj::
drawFg(PaintDevice *device) const
{
  if (! plot_->isTextVisible())
    return;

  //---

  // get connection rect of source and destination object
  auto *srcNode  = dynamic_cast<CQChartsDotPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsDotPlotNode *>(edge()->destNode());
  assert(srcNode && destNode);

  auto *srcObj  = srcNode ->obj();
  auto *destObj = destNode->obj();

  bool isSelf = (srcObj == destObj);

  auto srcRect  = edge()->srcNode ()->rect();
  auto destRect = edge()->destNode()->rect();

  auto rect = (isSelf ? srcRect : srcRect + destRect);

  auto prect = plot_->windowToPixel(rect);

  //---

  // set font
  plot_->setPainterFont(device, plot_->textFont());

  QFontMetricsF fm(device->font());

  //---

  // set text pen
  auto ic = calcColorInd();

  PenBrush penBrush;

  auto c = plot_->interpTextColor(ic);

  plot_->setPen(penBrush, PenData(true, c, plot_->textAlpha()));

  device->setPen(penBrush.pen);

  //---

  double textMargin = 4; // pixels

  auto str = edge()->label();

  if (! str.length())
    return;

  double ptw = fm.width(str);

  double tx = prect.getXMid() - textMargin - ptw/2.0;
  double ty = prect.getYMid() + (fm.ascent() - fm.descent())/2;

  auto pt = plot_->pixelToWindow(Point(tx, ty));

  // only support contrast
  auto textOptions = plot_->textOptions(device);

  textOptions.angle     = Angle();
  textOptions.align     = Qt::AlignLeft;
  textOptions.formatted = false;
  textOptions.scaled    = false;
  textOptions.html      = false;

  CQChartsDrawUtil::drawTextAtPoint(device, pt, str, textOptions);
}

void
CQChartsDotEdgeObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // set fill and stroke
  auto *srcNode  = dynamic_cast<CQChartsDotPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsDotPlotNode *>(edge()->destNode());
  assert(srcNode && destNode);

  //---

  ColorInd colorInd;

  //---

  // calc fill color
  QColor fc;

  if (! edge()->fillColor().isValid()) {
    if (plot()->isBlendEdgeColor()) {
      auto fc1 = srcNode ->obj()->calcFillColor();
      auto fc2 = destNode->obj()->calcFillColor();

      fc = CQChartsUtil::blendColors(fc1, fc2, 0.5);
    }
    else
      fc = plot()->interpEdgeFillColor(colorInd);
  }
  else {
    fc = plot()->interpColor(edge()->fillColor(), colorInd);
  }

  //---

  // calc stroke color
  QColor sc;

  if (plot()->isBlendEdgeColor()) {
    int numNodes = plot_->numNodes();

    ColorInd ic1(srcNode ->id(), numNodes);
    ColorInd ic2(destNode->id(), numNodes);

    auto sc1 = plot()->interpEdgeStrokeColor(ic1);
    auto sc2 = plot()->interpEdgeStrokeColor(ic2);

    sc = CQChartsUtil::blendColors(sc1, sc2, 0.5);
  }
  else {
    sc = plot()->interpEdgeStrokeColor(colorInd);
  }

  //---

  plot_->setPenBrush(penBrush, plot_->edgePenData(sc), plot_->edgeBrushData(fc));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsDotEdgeObj::
writeScriptData(ScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  if (edge()->hasValue()) {
    std::ostream &os = device->os();

    os << "\n";
    os << "  this.value = " << edge()->value().real() << ";\n";
  }
}

//------

CQChartsDotPlotCustomControls::
CQChartsDotPlotCustomControls(CQCharts *charts) :
 CQChartsConnectionPlotCustomControls(charts, "graph")
{
}

void
CQChartsDotPlotCustomControls::
init()
{
  addWidgets();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsDotPlotCustomControls::
addWidgets()
{
  addConnectionColumnWidgets();

  addColorColumnWidgets("Cell Color");
}

void
CQChartsDotPlotCustomControls::
connectSlots(bool b)
{
  CQChartsConnectionPlotCustomControls::connectSlots(b);
}

void
CQChartsDotPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  plot_ = dynamic_cast<CQChartsDotPlot *>(plot);

  CQChartsConnectionPlotCustomControls::setPlot(plot);
}

void
CQChartsDotPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  CQChartsConnectionPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

CQChartsColor
CQChartsDotPlotCustomControls::
getColorValue()
{
  return plot_->nodeFillColor();
}

void
CQChartsDotPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  plot_->setNodeFillColor(c);
}

//---

CQChartsDotPlotNode::
CQChartsDotPlotNode(const QString &str) :
 str_(str)
{
}

CQChartsDotPlotNode::
~CQChartsDotPlotNode()
{
  assert(! obj_);
}

void
CQChartsDotPlotNode::
setObj(Obj *obj)
{
  obj_ = obj;
}

const CQChartsDotPlotNode::BBox &
CQChartsDotPlotNode::
rect() const
{
  return rect_;
}

void
CQChartsDotPlotNode::
setRect(const BBox &rect)
{
  rect_ = rect;

  if (obj_) // TODO: assert null or use move by
    obj_->setRect(rect);
}

void
CQChartsDotPlotNode::
addSrcEdge(Edge *edge, bool primary)
{
  assert(edge->destNode());

  edge->destNode()->parent_ = edge->srcNode();

  srcEdges_.push_back(edge);

  srcDepth_ = -1;

  if (! primary)
    nonPrimaryEdges_.push_back(edge);
}

void
CQChartsDotPlotNode::
addDestEdge(Edge *edge, bool primary)
{
  assert(edge->destNode());

  edge->destNode()->parent_ = edge->srcNode();

  destEdges_.push_back(edge);

  destDepth_ = -1;

  if (! primary)
    nonPrimaryEdges_.push_back(edge);
}

bool
CQChartsDotPlotNode::
hasDestNode(Node *destNode) const
{
  for (auto &destEdge : destEdges()) {
    if (destEdge->destNode() == destNode)
      return true;
  }

  return false;
}

CQChartsDotPlotEdge *
CQChartsDotPlotNode::
findSrcEdge(Node *node) const
{
  for (auto &srcEdge : srcEdges()) {
    if (srcEdge->srcNode() == node)
      return srcEdge;
  }

  return nullptr;
}

CQChartsDotPlotEdge *
CQChartsDotPlotNode::
findDestEdge(Node *node) const
{
  for (auto &destEdge : destEdges()) {
    if (destEdge->destNode() == node)
      return destEdge;
  }

  return nullptr;
}

//---

void
CQChartsDotPlotNode::
moveBy(const Point &delta)
{
  rect_.moveBy(delta);

  if (obj_)
    obj_->moveBy(delta);
}

void
CQChartsDotPlotNode::
scale(double fx, double fy)
{
  rect_.scale(fx, fy);

  if (obj_)
    obj_->scale(fx, fy);
}

//---

CQChartsDotPlotEdge::
CQChartsDotPlotEdge(const OptReal &value, Node *srcNode, Node *destNode) :
 value_(value), srcNode_(srcNode), destNode_(destNode)
{
}

CQChartsDotPlotEdge::
~CQChartsDotPlotEdge()
{
  assert(! obj_);
}

void
CQChartsDotPlotEdge::
setObj(Obj *obj)
{
  obj_ = obj;
}
