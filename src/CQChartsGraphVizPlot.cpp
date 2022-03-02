#include <CQChartsGraphVizPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsNamePair.h>
#include <CQChartsConnectionList.h>
#include <CQChartsValueSet.h>
#include <CQChartsVariant.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsArrow.h>
#include <CQChartsEditHandles.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>
#include <CQChartsEnv.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQGraphViz.h>
#include <CQPerfMonitor.h>
//#include <CCommand.h>

#include <QMenu>
#include <QAction>
#include <QProcess>
#include <QTemporaryFile>
#include <QDir>

#include <fstream>

CQChartsGraphVizPlotType::
CQChartsGraphVizPlotType()
{
}

void
CQChartsGraphVizPlotType::
addParameters()
{
  CQChartsConnectionPlotType::addParameters();
}

QString
CQChartsGraphVizPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Graphviz Plot").
    h3("Summary").
     p("Draw connected objects as a connected graph.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/graphviz.png"));
}

CQChartsPlot *
CQChartsGraphVizPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsGraphVizPlot(view, model);
}

//------

CQChartsGraphVizPlot::
CQChartsGraphVizPlot(View *view, const ModelP &model) :
 CQChartsConnectionPlot(view, view->charts()->plotType("graphviz"), model),
 CQChartsObjNodeShapeData<CQChartsGraphVizPlot>(this),
 CQChartsObjNodeTextData <CQChartsGraphVizPlot>(this),
 CQChartsObjEdgeShapeData<CQChartsGraphVizPlot>(this),
 CQChartsObjEdgeTextData <CQChartsGraphVizPlot>(this)
{
}

CQChartsGraphVizPlot::
~CQChartsGraphVizPlot()
{
  term();
}

//---

void
CQChartsGraphVizPlot::
init()
{
  CQChartsConnectionPlot::init();

  //---

  NoUpdate noUpdate(this);

  setLayerActive(Layer::Type::FG_PLOT, true);

  //---

  auto bg = Color::makePalette();

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
CQChartsGraphVizPlot::
term()
{
  // delete objects first to ensure link from edge/node to object reset
  clearPlotObjects();

  clearNodesAndEdges();
}

//---

void
CQChartsGraphVizPlot::
clearNodesAndEdges()
{
  for (auto *node : nodes_)
    delete node;

  for (auto *edge : edges_)
    delete edge;

  nameNodeMap_.clear();
  indNodeMap_ .clear();

  nodes_.clear();
  edges_.clear();

  groupValueInd_.clear();

  maxNodeDepth_ = 0;
}

//---

void
CQChartsGraphVizPlot::
setNodeShape(const NodeShape &s)
{
  CQChartsUtil::testAndSet(nodeShape_, s, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGraphVizPlot::
setNodeScaled(bool b)
{
  CQChartsUtil::testAndSet(nodeScaled_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGraphVizPlot::
setNodeSize(const Length &s)
{
  CQChartsUtil::testAndSet(nodeSize_, s, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGraphVizPlot::
setNodeTextSingleScale(bool b)
{
  CQChartsUtil::testAndSet(nodeTextSingleScale_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsGraphVizPlot::
setEdgeShape(const EdgeShape &s)
{
  CQChartsUtil::testAndSet(edgeShape_, s, [&]() { drawObjs(); } );
}

void
CQChartsGraphVizPlot::
setEdgeScaled(bool b)
{
  CQChartsUtil::testAndSet(edgeScaled_, b, [&]() { drawObjs(); } );
}

void
CQChartsGraphVizPlot::
setEdgeArrow(bool b)
{
  CQChartsUtil::testAndSet(edgeArrow_, b, [&]() { drawObjs(); } );
}

void
CQChartsGraphVizPlot::
setEdgeWidth(const Length &l)
{
  CQChartsUtil::testAndSet(edgeWidth_, l, [&]() { drawObjs(); } );
}

void
CQChartsGraphVizPlot::
setEdgeCentered(bool b)
{
  CQChartsUtil::testAndSet(edgeCentered_, b, [&]() { drawObjs(); } );
}

void
CQChartsGraphVizPlot::
setEdgePath(bool b)
{
  CQChartsUtil::testAndSet(edgePath_, b, [&]() { drawObjs(); } );
}

void
CQChartsGraphVizPlot::
setArrowWidth(double w)
{
  CQChartsUtil::testAndSet(arrowWidth_, w, [&]() { drawObjs(); } );
}

void
CQChartsGraphVizPlot::
setEdgeWeighted(bool b)
{
  CQChartsUtil::testAndSet(edgeWeighted_, b, [&]() { updateRangeAndObjs(); } );
}

//---

const Qt::Orientation &
CQChartsGraphVizPlot::
orientation() const
{
  return orientation_;
}

void
CQChartsGraphVizPlot::
setOrientation(const Qt::Orientation &o)
{
  CQChartsUtil::testAndSet(orientation_, o, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsGraphVizPlot::
setBlendEdgeColor(bool b)
{
  CQChartsUtil::testAndSet(blendEdgeColor_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsGraphVizPlot::
setFdpK(double r)
{
  CQChartsUtil::testAndSet(fdpK_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGraphVizPlot::
setFdpMaxIter(int i)
{
  CQChartsUtil::testAndSet(fdpMaxIter_, i, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGraphVizPlot::
setFdpStart(int i)
{
  CQChartsUtil::testAndSet(fdpStart_, i, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGraphVizPlot::
setFdpEdgeLen(double r)
{
  CQChartsUtil::testAndSet(fdpEdgeLen_, r, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsGraphVizPlot::
setPlotType(const PlotType &t)
{
  CQChartsUtil::testAndSet(plotType_, t, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGraphVizPlot::
setOutputFormat(const OutputFormat &f)
{
  CQChartsUtil::testAndSet(outputFormat_, f, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsGraphVizPlot::
addProperties()
{
  CQChartsConnectionPlot::addProperties();

  //---

  // options
  addProp("options", "orientation" , "orientation" , "Plot orientation");
  addProp("options", "plotType"    , "plotType"    , "Plot type");
  addProp("options", "outputFormat", "outputFormat", "Output format");

  // coloring
  addProp("coloring", "blendEdgeColor", "", "Blend Edge Node Colors");

  // node
  addProp("node", "nodeShape" , "shapeType", "Node shape type");
  addProp("node", "nodeScaled", "scaled"   , "Node is scaled");
  addProp("node", "nodeSize"  , "size"     , "Node size (ignore if <= 0)");

  // node style
  addProp("node/stroke", "nodeStroked", "visible", "Node stroke visible");

  addLineProperties("node/stroke", "nodeStroke", "Node");

  addProp("node/fill", "nodeFilled", "visible", "Node fill visible");

  addFillProperties("node/fill", "nodeFill", "Node");

  //---

  // edge
  addProp("edge", "edgeShape"   , "shapeType" , "Edge shape type");
  addProp("edge", "edgeArrow"   , "arrow"     , "Edge arrow");
  addProp("edge", "edgeScaled"  , "scaled"    , "Edge is scaled");
  addProp("edge", "edgeWidth"   , "width"     , "Edge width");
  addProp("edge", "edgeCentered", "centered"  , "Edge is cenetered");
  addProp("edge", "edgePath"    , "usePath"   , "Use Edge path");
  addProp("edge", "arrowWidth"  , "arrowWidth", "Directed edge arrow width factor");
  addProp("edge", "edgeWeighted", "weighted"  , "Edge is weighted bt value for placement");

  // edge style
  addProp("edge/stroke", "edgeStroked", "visible", "Edge stroke visible");

  addLineProperties("edge/stroke", "edgeStroke", "Edge");

  addProp("edge/fill", "edgeFilled", "visible", "Edit fill visible");

  addFillProperties("edge/fill", "edgeFill", "Edge");

  //---

  // text
  addProp("node/text", "nodeTextVisible", "visible", "Node text label visible");

  addTextProperties("node/text", "nodeText", "Node Text",
                    CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::FORMATTED |
                    CQChartsTextOptions::ValueType::SCALED |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  addProp("node/text", "nodeTextSingleScale", "singleScale",
          "Node text single scale (when scaled)");

  //--

  addProp("edge/text", "edgeTextVisible", "visible", "Edge text label visible");

  addTextProperties("edge/text", "edgeText", "Edge Text",
                    CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::FORMATTED |
                    CQChartsTextOptions::ValueType::SCALED |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  //---

  // placement
  addProp("placement/fdp", "fdpK"      , "k"      , "Ideal node separation");
  addProp("placement/fdp", "fdpMaxIter", "maxIter", "Max iterations");
  addProp("placement/fdp", "fdpStart"  , "start"  , "Start iteration");
  addProp("placement/fdp", "fdpEdgeLen", "edgeLen", "Optimal edge len");

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
}

//---

CQChartsGeom::Range
CQChartsGraphVizPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsGraphVizPlot::calcRange");

  auto *th = const_cast<CQChartsGraphVizPlot *>(this);

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
CQChartsGraphVizPlot::
objTreeRange() const
{
  auto bbox = nodesBBox();

  if (! bbox.isValid())
    return Range(0, 0, 1, 1);

  return Range(bbox.getXMin(), bbox.getYMax(), bbox.getXMax(), bbox.getYMin());
}

CQChartsGeom::BBox
CQChartsGraphVizPlot::
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
CQChartsGraphVizPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsGraphVizPlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsGraphVizPlot *>(this);

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

  if (! writeGraph(isEdgeWeighted()))
    return false;

  addObjects(objs);

  return true;
}

void
CQChartsGraphVizPlot::
addObjects(PlotObjs &objs) const
{
  if (! bbox_.isValid())
    return;

  auto *th = const_cast<CQChartsGraphVizPlot *>(this);

  //---

  th->maxNodeValue_ = OptReal();

  for (const auto &node : nodes()) {
    auto *nodeObj = createObjFromNode(node);

    objs.push_back(nodeObj);

    if (node->hasValue())
      th->maxNodeValue_ = OptReal(std::max(maxNodeValue_.realOr(0.0), node->value().real()));
  }

  //---

  th->maxEdgeValue_ = OptReal();

  for (const auto &edge : edges()) {
    auto *edgeObj = addEdgeObj(edge);
    if (! edgeObj) continue;

    objs.push_back(edgeObj);

    if (edge->hasValue())
      th->maxEdgeValue_ = OptReal(std::max(maxEdgeValue_.realOr(0.0), edge->value().real()));
  }
}

bool
CQChartsGraphVizPlot::
writeGraph(bool weighted) const
{
  auto graphVizFilename = CQChartsEnv::getString("CQ_CHARTS_GRAPHVIZ_INPUT_FILE");

  if (graphVizFilename == "") {
    // create graphviz input file from data
    QTemporaryFile graphVizFile(QDir::tempPath() + "/XXXXXX.gv");

    if (! graphVizFile.open())
      return false;

    graphVizFilename = graphVizFile.fileName();

    if (! writeGraph(graphVizFile, graphVizFilename, weighted))
      return false;
  }
  else {
    QFile graphVizFile(graphVizFilename);

    if (! graphVizFile.open(QIODevice::WriteOnly))
      return false;

    if (! writeGraph(graphVizFile, graphVizFilename, weighted))
      return false;
  }

  return true;
}

bool
CQChartsGraphVizPlot::
writeGraph(QFile &graphVizFile, const QString &graphVizFilename, bool weighted) const
{
  auto writeGraphViz = [&](const QString &str) {
    graphVizFile.write(str.toLatin1().constData());
  };

  writeGraphViz("digraph g {\n");

  //---

  using EdgeSet    = std::set<Edge *>;
  using GroupEdges = std::map<int, EdgeSet>;
  using EdgeGroup  = std::map<Edge *, int>;

  GroupEdges groupEdges;
  EdgeGroup  edgeGroup;

  for (const auto &edge : edges_) {
    auto *node1 = edge->srcNode ();
  //auto *node2 = edge->destNode();

    if (node1->group() < 0)
      continue;

    groupEdges[node1->group()].insert(edge);

    edgeGroup[edge] = node1->group();
  }

  //---

  using NodeSet = std::set<Node *>;

  NodeSet nodeSet;

  //---

  auto printAttr = [&](const QString &name, const QString &value, int &attrCount) {
    if (attrCount == 0)
      writeGraphViz(" [");
    else
      writeGraphViz(",");

    writeGraphViz(name + "=\"" + value + "\"");

    ++attrCount;
  };

  auto endPrintAttr = [&](int attrCount) {
    if (attrCount > 0)
      writeGraphViz("]");

    writeGraphViz(";\n");
  };

  //---

  // print node (if not already output)
  auto printNode = [&](Node *node) {
    auto p = nodeSet.find(node);
    if (p != nodeSet.end()) return;

    writeGraphViz("\"" + node->name() + "\"");

    int nodeAttrCount = 0;

    auto shapeStr = shapeTypeToString(node->shapeType());

    printAttr("shape", shapeStr, nodeAttrCount);

    if (node->label().length())
      printAttr("label", node->label(), nodeAttrCount);

    bool isNodeScaled = (this->isNodeScaled() && node->hasValue());

    if (isNodeScaled) {
      auto maxValue = maxNodeValue().realOr(0.0);

      auto nodeScale = (maxValue > 0.0 ? node->value().real()/maxValue : 1.0);

      auto size = nodeScale*lengthPlotWidth(nodeSize());

      printAttr("width" , QString::number(size), nodeAttrCount);
      printAttr("height", QString::number(size), nodeAttrCount);

      printAttr("fixedsize", "true", nodeAttrCount);
    }

    endPrintAttr(nodeAttrCount);

    nodeSet.insert(node);
  };

  auto printEdge = [&](Edge *edge) {
    auto *node1 = edge->srcNode ();
    auto *node2 = edge->destNode();

    printNode(node1);
    printNode(node2);

    writeGraphViz("\"" + node1->name() + "\" -> \"" + node2->name() + "\"");

    if (weighted) {
      if (edge->hasValue())
        writeGraphViz(" [weight=" + QString::number(edge->value().real()) + "]");
    }

    if (fdpEdgeLen() > 0.0)
      writeGraphViz(" [len=" + QString::number(fdpEdgeLen()) + "]\n");

    int edgeAttrCount = 0;

    if (edge->label().length())
      printAttr("label", edge->label(), edgeAttrCount);

    endPrintAttr(edgeAttrCount);
  };

  if (plotType() == PlotType::FDP) {
    if (fdpK() > 0.0)
      writeGraphViz(" K=" + QString::number(fdpK()) + ";\n");

    if (fdpMaxIter() > 0.0)
      writeGraphViz(" maxiter=" + QString::number(fdpMaxIter()) + ";\n");

    if (fdpStart() > 0.0)
      writeGraphViz(" start=" + QString::number(fdpStart()) + ";\n");
  }

  for (const auto &ge : groupEdges) {
    writeGraphViz("subgraph " + QString::number(ge.first) + " {\n");

    for (const auto &edge : ge.second) {
      printEdge(edge);
    }

    writeGraphViz("}\n");
  }

  for (const auto &edge : edges_) {
    auto p = edgeGroup.find(edge);

    if (p == edgeGroup.end())
      printEdge(edge);
  }

  writeGraphViz("}\n");

  graphVizFile.close();

  //---

  QString typeName;

  if      (outputFormat() == OutputFormat::XDOT)
    typeName = QString("xdot");
  else if (outputFormat() == OutputFormat::DOT)
    typeName = QString("dot");
  else if (outputFormat() == OutputFormat::JSON)
    typeName = QString("json");

  // get temporary file for output
  auto outFilename = CQChartsEnv::getString("CQ_CHARTS_GRAPHVIZ_OUTPUT_FILE");

  if (outFilename == "") {
    QTemporaryFile outFile(QDir::tempPath() + "/XXXXXX." + typeName);

    if (! outFile.open())
      return false;

    outFilename = outFile.fileName();

    if (! processGraph(graphVizFilename, outFile, outFilename, typeName))
      return false;
  }
  else {
    QFile outFile(outFilename);

    if (! outFile.open(QIODevice::WriteOnly))
      return false;

    if (! processGraph(graphVizFilename, outFile, outFilename, typeName))
      return false;
  }

  return true;
}

bool
CQChartsGraphVizPlot::
processGraph(const QString &graphVizFilename, QFile & /*outFile*/,
             const QString &outFilename, const QString &typeName) const
{
  QString cmd { "dot" };

  switch (plotType()) {
    case PlotType::DOT       : cmd = "dot"; break;
    case PlotType::NEATO     : cmd = "neato"; break;
    case PlotType::TWOPI     : cmd = "twopi"; break;
    case PlotType::CIRCO     : cmd = "circo"; break;
    case PlotType::FDP       : cmd = "fdp"; break;
    case PlotType::OSAGE     : cmd = "osage"; break;
    case PlotType::PATCHWORK : cmd = "patchwork"; break;
    case PlotType::SFDP      : cmd = "sfdp"; break;
  }

  // run dot on graph file
  auto dot_path = CQChartsEnv::getString("CQCHARTS_DOT_PATH", "/usr/bin");
  auto dot_file = dot_path + "/" + cmd;

#if 0
  CCommand::Args args;

  args.push_back(QString("-T" + typeName).toStdString());
  args.push_back(graphVizFilename.toStdString());

  CCommand cmd(cmd, dot_file, args);

  cmd.addFileDest(outFilename.toStdString());

  cmd.start();

  cmd.wait();
#else
  QProcess process;

  process.setStandardOutputFile(outFilename);

  auto typeArg = "-T" + typeName;
  auto cmdArgs = QStringList() << typeArg << graphVizFilename;

  process.start(dot_file, cmdArgs);

  if (! process.waitForFinished()) {
    std::cerr << "Command failed : " << dot_file.toStdString() << " " <<
                 cmdArgs.join(" ").toStdString() << " > " << outFilename.toStdString() << "\n";
    return false;
  }
#endif

  //---

  // parse output file
  CQGraphViz::App dot;

  if      (outputFormat() == OutputFormat::XDOT || outputFormat() == OutputFormat::DOT)
    dot.processDot(outFilename.toStdString());
  else if (outputFormat() == OutputFormat::JSON)
    dot.processJson(outFilename.toStdString());

  //---

  // process placement

  BBox bbox;

  // create nodes
  for (auto &object : dot.objects()) {
    auto *node = findNode(object->name());

    if (! node) {
      charts()->errorMsg(object->name() + " Not Found");
      continue;
    }

    node->setDotId(object->id());

    BBox bbox1(object->rect());

    if (! bbox1.isValid()) {
      charts()->errorMsg("No bbox for object " + object->name());
      continue;
    }

    node->setRect(bbox1);

    bbox += bbox1;
  }

  // create edges
  for (auto &dotEdge : dot.edges()) {
    int tailId = dotEdge->tailId(); // from
    int headId = dotEdge->headId(); // to

    auto *tailNode = findIdNode(tailId);
    auto *headNode = findIdNode(headId);
    if (! tailNode || ! headNode) {
      charts()->errorMsg("Missing tail/head node for edge " + QString::number(dotEdge->id()));
      continue;
    }

    auto *edge = tailNode->findDestEdge(headNode);
    if (! edge) edge = tailNode->findSrcEdge(headNode);
    if (! edge) {
      charts()->errorMsg("Edge " + QString::number(dotEdge->id()) + " not Found");
      continue;
    }

    //---

    auto spanRect = tailNode->rect() + headNode->rect();

    auto normalizeRectPoint = [&](const BBox &rect, const Point &p) {
      auto x = CMathUtil::map(p.x, rect.getXMin(), spanRect.getXMax(), 0, 1);
      auto y = CMathUtil::map(p.y, rect.getYMin(), spanRect.getYMax(), 0, 1);

      return Point(x, y);
    };

    auto normalizePathPoint = [&](const Point &p) {
      return normalizeRectPoint(spanRect, p);
    };

    auto addPathPoint = [&](QPainterPath &path, const Point &p) {
      auto p1 = normalizePathPoint(p).qpoint();

      if (path.elementCount() == 0)
        path.moveTo(p1);
      else
        path.lineTo(p1);
    };

    //---

    int nl = int(dotEdge->lines().size());

    if (nl > 0) {
      if (nl != 1) {
        charts()->errorMsg("Error: edge " + QString::number(dotEdge->id()) +
                           " has " + QString::number(nl) + " lines");
        continue;
      }

      auto path = dotEdge->lines()[0].path;

      auto len = path.length();
      //charts()->errorMsg("len=" + len);

      auto delta = len/100.0;
      //charts()->errorMsg("delta=" + delta);

      auto points = CQChartsPath::pathPoints(path);

      int   it = -1, ih = -1;
      Point pit, pih;
    //bool  pitValid = true, pihValid = true;

      // get edge point on tail node
      if (tailNode->rect().isValid()) { // from
        // get nearest points to center of node
        (void) CQChartsUtil::nearestPointListPoint(points, tailNode->rect().getCenter(), it);

        // intersect line from center to node
        if (tailNode->shapeType() == Node::ShapeType::CIRCLE ||
            tailNode->shapeType() == Node::ShapeType::OVAL)
          CQChartsGeom::lineIntersectCircle(tailNode->rect(),
            tailNode->rect().getCenter(), points[size_t(it)], pit);
        else
          CQChartsGeom::lineIntersectRect(tailNode->rect(),
            tailNode->rect().getCenter(), points[size_t(it)], pit);

        tailNode->setEdgePoint(edge, pit);

        //auto d = CQChartsUtil::PointPointDistance(points[size_t(it)], pit);
        //charts()->errorMsg("d=" + d);
        //pitValid = (d > delta);
      }

      // get edge point on head node
      if (headNode->rect().isValid()) { // to
        // get nearest points to center of node
        (void) CQChartsUtil::nearestPointListPoint(points, headNode->rect().getCenter(), ih);

        // intersect line from center to node
        if (headNode->shapeType() == Node::ShapeType::CIRCLE ||
            headNode->shapeType() == Node::ShapeType::OVAL)
          CQChartsGeom::lineIntersectCircle(headNode->rect(),
            headNode->rect().getCenter(), points[size_t(ih)], pih);
        else
          CQChartsGeom::lineIntersectRect(headNode->rect(),
            headNode->rect().getCenter(), points[size_t(ih)], pih);

        headNode->setEdgePoint(edge, pih);

        //auto d = CQChartsUtil::PointPointDistance(points[size_t(ih)], pih);
        //charts()->errorMsg("d=" + d);
        //pihValid = (d > delta);
      }

#if 0
      edge->setLine(CQChartsGeom::Line(normalizeRectPoint(tailNode->rect(), points[size_t(it)]),
                                       normalizeRectPoint(headNode->rect(), points[size_t(ih)])));
#endif

      QPainterPath path1;

      if (it >= 0 && ih >= 0) {
        // points from tail to head
        if (it < ih) {
          //addPathPoint(path1, tailNode->rect().getCenter());

          //if (pitValid)
          //  addPathPoint(path1, pit);

          for (int i = it; i <= ih; ++i) {
            if (headNode->rect().inside(points[size_t(i)]) ||
                tailNode->rect().inside(points[size_t(i)]))
              continue;

            if (CQChartsUtil::PointPointDistance(points[size_t(i)], pih) < delta ||
                CQChartsUtil::PointPointDistance(points[size_t(i)], pit) < delta)
              continue;

            addPathPoint(path1, points[size_t(i)]);
          }

          //if (pihValid)
          //  addPathPoint(path1, pih);

          //addPathPoint(path1, headNode->rect().getCenter());
        }
        // points from head to tail
        else {
          //addPathPoint(path1, headNode->rect().getCenter());

          //if (pihValid)
          //  addPathPoint(path1, pih);

          for (int i = ih; i <= it; ++i) {
            if (headNode->rect().inside(points[size_t(i)]) ||
                tailNode->rect().inside(points[size_t(i)]))
              continue;

            if (CQChartsUtil::PointPointDistance(points[size_t(i)], pih) < delta ||
                CQChartsUtil::PointPointDistance(points[size_t(i)], pit) < delta)
              continue;

            addPathPoint(path1, points[size_t(i)]);
          }

          //if (pitValid)
          //  addPathPoint(path1, pit);

          //addPathPoint(path1, tailNode->rect().getCenter());
        }
      }
      else
        path1 = path;

      //auto str = CQChartsPath::pathToString(path1);
      //charts()->errorMsg("Edge " + tailNode->name() + "->" + headNode->name() + " = " + str);

      edge->setEdgePath(path1);
    }
    else {
    }

    //---

    if (dot.isDirected())
      edge->setDirected(true);
  }

  auto *th = const_cast<CQChartsGraphVizPlot *>(this);

  th->bbox_ = bbox; // current

  th->fitToBBox(targetBBox_);

  return true;
}

//---

void
CQChartsGraphVizPlot::
preDrawFgObjs(PaintDevice *) const
{
  CQChartsDrawUtil::resetScaledFontSize();

  auto *th = const_cast<CQChartsGraphVizPlot *>(this);

  th->drawTextDatas_.clear();
}

void
CQChartsGraphVizPlot::
postDrawFgObjs(PaintDevice *device) const
{
  if (isNodeTextSingleScale()) {
    auto s = CQChartsDrawUtil::scaledFontSize();

    auto *th = const_cast<CQChartsGraphVizPlot *>(this);

    setPainterFont(device, nodeTextFont());

    for (auto &data : th->drawTextDatas_) {
      // set text pen
      PenBrush penBrush;

      setPen(penBrush, PenData(true, data.c, nodeTextAlpha()));

      device->setPen(penBrush.pen);

      // set scale and draw
      data.textOptions.scale = s;

      if (data.isRect)
        CQChartsDrawUtil::drawTextInBox(device, data.rect, data.str, data.textOptions);
      else
        CQChartsDrawUtil::drawTextAtPoint(device, data.p, data.str, data.textOptions);
    }
  }
}

//---

void
CQChartsGraphVizPlot::
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
}

//------

bool
CQChartsGraphVizPlot::
initHierObjs() const
{
  CQPerfTrace trace("CQChartsGraphVizPlot::initHierObjs");

  return CQChartsConnectionPlot::initHierObjs();
}

void
CQChartsGraphVizPlot::
initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                              const HierConnectionData &destHierData) const
{
  int srcDepth = int(srcHierData.linkStrs.size());

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
CQChartsGraphVizPlot::
initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                              const HierConnectionData &destHierData) const
{
  int srcDepth = int(srcHierData.linkStrs.size());

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
CQChartsGraphVizPlot::
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
CQChartsGraphVizPlot::
initPathObjs() const
{
  CQPerfTrace trace("CQChartsGraphVizPlot::initPathObjs");

  //---

  auto *th = const_cast<CQChartsGraphVizPlot *>(this);

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
CQChartsGraphVizPlot::
addPathValue(const PathData &pathData) const
{
  int n = pathData.pathStrs.length();
  assert(n > 0);

  auto *th = const_cast<CQChartsGraphVizPlot *>(this);

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
CQChartsGraphVizPlot::
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
          //int ns = int(node->srcEdges().size());

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
CQChartsGraphVizPlot::
initFromToObjs() const
{
  CQPerfTrace trace("CQChartsGraphVizPlot::initFromToObjs");

  return CQChartsConnectionPlot::initFromToObjs();
}

void
CQChartsGraphVizPlot::
addFromToValue(const FromToData &fromToData) const
{
  // get src node
  auto *srcNode = findNode(fromToData.fromStr);

  if (fromToData.depth > 0)
    srcNode->setDepth(fromToData.depth);

  //---

  // set group
  if (fromToData.groupData.isValid() && ! fromToData.groupData.isNull()) {
    srcNode->setGroup(fromToData.groupData.ig, fromToData.groupData.ng);

    auto *th = const_cast<CQChartsGraphVizPlot *>(this);

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
      auto *th = const_cast<CQChartsGraphVizPlot *>(this);

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
    Node *destNode = nullptr;

    if (fromToData.fromStr != fromToData.toStr)
      destNode = findNode(fromToData.toStr);
    else
      destNode = srcNode;

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

    auto fromModelIndex  = modelIndex(fromToData.fromModelInd);
    auto fromModelIndex1 = normalizeIndex(fromModelIndex);

    edge->setModelInd(fromModelIndex1);

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
    // Note: from and to are same row so we can use either
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
CQChartsGraphVizPlot::
initLinkObjs() const
{
  CQPerfTrace trace("CQChartsGraphVizPlot::initLinkObjs");

  return CQChartsConnectionPlot::initLinkObjs();
}

void
CQChartsGraphVizPlot::
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
  if (linkConnectionData.groupData.isValid() && ! linkConnectionData.groupData.isNull()) {
    srcNode->setGroup(linkConnectionData.groupData.ig, linkConnectionData.groupData.ng);

    auto *th = const_cast<CQChartsGraphVizPlot *>(this);

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
CQChartsGraphVizPlot::
initConnectionObjs() const
{
  CQPerfTrace trace("CQChartsGraphVizPlot::initConnectionObjs");

  return CQChartsConnectionPlot::initConnectionObjs();
}

void
CQChartsGraphVizPlot::
addConnectionObj(int id, const ConnectionsData &connectionsData, const NodeIndex &) const
{
  // get src node
  auto srcStr = QString::number(id);

  auto *srcNode = findNode(srcStr);

  srcNode->setName(connectionsData.name);

  //---

  // set group
  if (connectionsData.groupData.isValid() && ! connectionsData.groupData.isNull()) {
    srcNode->setGroup(connectionsData.groupData.ig, connectionsData.groupData.ng);

    auto *th = const_cast<CQChartsGraphVizPlot *>(this);

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
CQChartsGraphVizPlot::
initTableObjs() const
{
  CQPerfTrace trace("CQChartsGraphVizPlot::initTableObjs");

  //---

  TableConnectionDatas tableConnectionDatas;
  TableConnectionInfo  tableConnectionInfo;

  if (! processTableModel(tableConnectionDatas, tableConnectionInfo))
    return false;

  //---

  auto nv = tableConnectionDatas.size();

  for (size_t row = 0; row < nv; ++row) {
    const auto &tableConnectionData = tableConnectionDatas[row];

    if (tableConnectionData.values().empty())
      continue;

    auto srcStr = QString::number(tableConnectionData.from());

    auto *srcNode = findNode(srcStr);

    srcNode->setName(tableConnectionData.name());

    if (tableConnectionData.group().isValid() && ! tableConnectionData.group().isNull())
      srcNode->setGroup(tableConnectionData.group().ig, tableConnectionData.group().ng);
    else
      srcNode->setGroup(-1);

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
CQChartsGraphVizPlot::
processNodeNameValues(Node *node, const NameValues &nameValues) const
{
  for (const auto &nv : nameValues.nameValues()) {
    const auto &name  = nv.first;
    auto        value = nv.second.toString();

    processNodeNameValue(node, name, value);
  }
}

void
CQChartsGraphVizPlot::
processNodeNameValue(Node *node, const QString &name, const QString &valueStr) const
{
  // shape
  if      (name == "shape") {
    Node::ShapeType shapeType;

    stringToShapeType(valueStr, shapeType);

    node->setShapeType(shapeType);
  }
  // num sides
  else if (name == "num_sides") {
    bool ok;

    long n = CQChartsUtil::toInt(valueStr, ok);

    if (ok)
      node->setNumSides(int(n));
  }
  else if (name == "label") {
    node->setLabel(valueStr);
  }
  else if (name == "value") {
    bool ok;

    auto r = CQChartsUtil::toReal(valueStr, ok);

    if (ok)
      node->setValue(OptReal(r));
  }
  else if (name == "fill_color" || name == "color") {
    node->setFillColor(CQChartsColor(valueStr));
  }
  else if (name == "fill_alpha" || name == "alpha") {
    node->setFillAlpha(CQChartsAlpha(valueStr));
  }
  else if (name == "fill_pattern" || name == "pattern") {
    node->setFillPattern(CQChartsFillPattern(valueStr));
  }
  else if (name == "stroke_color") {
    node->setStrokeColor(CQChartsColor(valueStr));
  }
  else if (name == "stroke_alpha") {
    node->setStrokeAlpha(CQChartsAlpha(valueStr));
  }
  else if (name == "stroke_width" || name == "width") {
    node->setStrokeWidth(CQChartsLength(valueStr));
  }
  else if (name == "stroke_dash" || name == "dash") {
    node->setStrokeDash(CQChartsLineDash(valueStr));
  }
}

void
CQChartsGraphVizPlot::
processEdgeNameValues(Edge *edge, const NameValues &nameValues) const
{
  auto *srcNode  = edge->srcNode ();
  auto *destNode = edge->destNode();

  for (const auto &nv : nameValues.nameValues()) {
    const auto &name     = nv.first;
    auto        valueStr = nv.second.toString();

    if      (name == "shape") {
      Edge::ShapeType shapeType;

      stringToShapeType(valueStr, shapeType);

      edge->setShapeType(shapeType);
    }
    else if (name == "label") {
      edge->setLabel(valueStr);
    }
    else if (name == "color") {
      edge->setFillColor(CQChartsColor(valueStr));
    }
    else if (name.left(4) == "src_") {
      processNodeNameValue(srcNode, name.mid(4), valueStr);
    }
    else if (name.left(5) == "dest_") {
      processNodeNameValue(destNode, name.mid(5), valueStr);
    }
  }
}

void
CQChartsGraphVizPlot::
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
    //charts()->errorMsg("Unhandled shape type " + str);
    shapeType = Node::ShapeType::NONE;
  }
}

QString
CQChartsGraphVizPlot::
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
CQChartsGraphVizPlot::
stringToShapeType(const QString &str, Edge::ShapeType &shapeType)
{
  if (str == "arrow" ) shapeType = Edge::ShapeType::ARROW;
  else                 shapeType = Edge::ShapeType::NONE;
}

QString
CQChartsGraphVizPlot::
shapeTypeToString(const Edge::ShapeType &shapeType)
{
  switch (shapeType) {
    case Edge::ShapeType::ARROW: return "arrow";
    default                    : return "none";
  }
}

//---

void
CQChartsGraphVizPlot::
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
CQChartsGraphVizPlot::
addMenuItems(QMenu *menu)
{
  if (canDrawColorMapKey()) {
    menu->addSeparator();

    addColorMapKeyItems(menu);
  }

  return true;
}

//---

CQChartsGraphVizPlot::NodeObj *
CQChartsGraphVizPlot::
createObjFromNode(Node *node) const
{
//int numNodes = int(graph->nodes().size()); // node id needs to be per graph
  int numNodes = this->numNodes();

  ColorInd iv(node->id(), numNodes);

  if (! node->rect().isValid())
    node->setRect(BBox(Point(0, 0), Point(1, 1)));

  auto *nodeObj = createNodeObj(node->rect(), node, iv);

  //--

  // set shape type
  NodeObj::ShapeType shapeType = static_cast<NodeObj::ShapeType>(node->shapeType());

  if (shapeType == NodeObj::ShapeType::NONE)
    shapeType = static_cast<NodeObj::ShapeType>(nodeShape());

  nodeObj->setShapeType(shapeType);

  //---

  nodeObj->setHierName(node->str());

  //---

  // add model indices
  for (const auto &modelInd : node->modelInds())
    nodeObj->addModelInd(normalizedModelIndex(modelInd));

  //---

  auto *pnode = dynamic_cast<CQChartsGraphVizPlotNode *>(node);
  assert(pnode);

  pnode->setObj(nodeObj);

  return nodeObj;
}

CQChartsGraphVizEdgeObj *
CQChartsGraphVizPlot::
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

  //---

  // set shape type
  EdgeObj::ShapeType shapeType = static_cast<EdgeObj::ShapeType>(edge->shapeType());

  if (shapeType == EdgeObj::ShapeType::NONE)
    shapeType = static_cast<EdgeObj::ShapeType>(edgeShape());

  edgeObj->setShapeType(shapeType);

  //---

  // add model indices
  for (const auto &modelInd : edge->modelInds())
    edgeObj->addModelInd(normalizedModelIndex(modelInd));

  //---

  auto *pedge = dynamic_cast<CQChartsGraphVizPlotEdge *>(edge);
  assert(pedge);

  pedge->setObj(edgeObj);

  return edgeObj;
}

//---

CQChartsGraphVizPlotNode *
CQChartsGraphVizPlot::
findNode(const QString &name) const
{
  auto p = nameNodeMap_.find(name);

  if (p != nameNodeMap_.end())
    return (*p).second;

  return createNode(name);
}

CQChartsGraphVizPlotNode *
CQChartsGraphVizPlot::
findIdNode(int dotId) const
{
  for (const auto &node : nodes())
    if (node->dotId() == dotId)
     return node;

  return nullptr;
}

CQChartsGraphVizPlotNode *
CQChartsGraphVizPlot::
createNode(const QString &name) const
{
  auto *node = new CQChartsGraphVizPlotNode(name);

  node->setName(name);

  node->setId(int(nameNodeMap_.size()));

  auto *th = const_cast<CQChartsGraphVizPlot *>(this);

  auto p1 = th->nameNodeMap_.insert(th->nameNodeMap_.end(),
              NameNodeMap::value_type(node->str(), node));
  assert(node == (*p1).second);

  th->indNodeMap_[node->id()] = node;

  //---

  th->nodes_.push_back(node);

  return node;
}

CQChartsGraphVizPlotEdge *
CQChartsGraphVizPlot::
createEdge(const OptReal &value, Node *srcNode, Node *destNode) const
{
  auto *edge = new CQChartsGraphVizPlotEdge(value, srcNode, destNode);

  edge->setId(int(edges_.size()));

  auto *th = const_cast<CQChartsGraphVizPlot *>(this);

  th->edges_.push_back(edge);

  return edge;
}

//---

bool
CQChartsGraphVizPlot::
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
CQChartsGraphVizPlot::
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

CQChartsGraphVizNodeObj *
CQChartsGraphVizPlot::
createNodeObj(const BBox &rect, Node *node, const ColorInd &ind) const
{
  return new NodeObj(this, rect, node, ind);
}

CQChartsGraphVizEdgeObj *
CQChartsGraphVizPlot::
createEdgeObj(const BBox &rect, Edge *edge) const
{
  return new EdgeObj(this, rect, edge);
}

//---

bool
CQChartsGraphVizPlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsGraphVizPlot::
execDrawForeground(PaintDevice *device) const
{
  if (isColorMapKey())
    drawColorMapKey(device);
}

//---

CQChartsPlotCustomControls *
CQChartsGraphVizPlot::
createCustomControls()
{
  auto *controls = new CQChartsGraphVizPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsGraphVizNodeObj::
CQChartsGraphVizNodeObj(const Plot *plot, const BBox &rect, Node *node, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect, ColorInd(), ColorInd(), iv),
 plot_(plot), node_(node)
{
  setDetailHint(DetailHint::MAJOR);

  setEditable(true);
}

CQChartsGraphVizNodeObj::
~CQChartsGraphVizNodeObj()
{
  auto *pnode = dynamic_cast<CQChartsGraphVizPlotNode *>(node_);

  if (pnode)
    pnode->setObj(nullptr);
}

QString
CQChartsGraphVizNodeObj::
name() const
{
  return node()->name();
}

void
CQChartsGraphVizNodeObj::
setName(const QString &s)
{
  node()->setName(s);
}

double
CQChartsGraphVizNodeObj::
value() const
{
  return node()->value().realOr(0.0);
}

void
CQChartsGraphVizNodeObj::
setValue(double r)
{
  node()->setValue(CQChartsGraphVizPlotNode::OptReal(r));
}

int
CQChartsGraphVizNodeObj::
depth() const
{
  return node()->depth();
}

void
CQChartsGraphVizNodeObj::
setDepth(int depth)
{
  node()->setDepth(depth);
}

CQChartsGraphVizNodeObj::ShapeType
CQChartsGraphVizNodeObj::
shapeType() const
{
  return static_cast<CQChartsGraphVizNodeObj::ShapeType>(node()->shapeType());
}

void
CQChartsGraphVizNodeObj::
setShapeType(const ShapeType &s)
{
  node()->setShapeType(static_cast<CQChartsGraphVizPlotNode::ShapeType>(s));
}

int
CQChartsGraphVizNodeObj::
numSides() const
{
  return node()->numSides();
}

void
CQChartsGraphVizNodeObj::
setNumSides(int n)
{
  node()->setNumSides(n);
}

CQChartsColor
CQChartsGraphVizNodeObj::
fillColor() const
{
  return node()->fillColor();
}

void
CQChartsGraphVizNodeObj::
setFillColor(const Color &c)
{
  node()->setFillColor(c);
}

QString
CQChartsGraphVizNodeObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(node()->id());
}

QString
CQChartsGraphVizNodeObj::
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

  int ns = int(node()->srcEdges ().size());
  int nd = int(node()->destEdges().size());

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
CQChartsGraphVizNodeObj::
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
CQChartsGraphVizNodeObj::
moveBy(const Point &delta)
{
  //charts()->errorMsg("  Move " + node()->str() + " by " + delta.y);

  rect_.moveBy(delta);
}

void
CQChartsGraphVizNodeObj::
scale(double fx, double fy)
{
  rect_.scale(fx, fy);
}

//---

bool
CQChartsGraphVizNodeObj::
editPress(const Point &p)
{
  editChanged_ = false;

  editHandles()->setDragPos(p);

  return true;
}

bool
CQChartsGraphVizNodeObj::
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

  const_cast<CQChartsGraphVizPlot *>(plot())->drawObjs();

  return true;
}

bool
CQChartsGraphVizNodeObj::
editMotion(const Point &p)
{
  return editHandles()->selectInside(p);
}

bool
CQChartsGraphVizNodeObj::
editRelease(const Point &)
{
  if (editChanged_)
    const_cast<CQChartsGraphVizPlot *>(plot())->invalidateObjTree();

  return true;
}

void
CQChartsGraphVizNodeObj::
setEditBBox(const BBox &bbox, const CQChartsResizeSide &)
{
  assert(bbox.isSet());

  double dx = bbox.getXMin() - rect_.getXMin();
  double dy = bbox.getYMin() - rect_.getYMin();

  node()->moveBy(Point(dx, dy));
}

//---

void
CQChartsGraphVizNodeObj::
getObjSelectIndices(Indices &inds) const
{
  for (const auto &c : plot_->modelColumns())
    addColumnSelectIndex(inds, c);
}

CQChartsGraphVizNodeObj::PlotObjs
CQChartsGraphVizNodeObj::
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
CQChartsGraphVizNodeObj::
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
    if      (shapeType() == ShapeType::BOX)
      device->drawRect(rect());
    else if (shapeType() == ShapeType::DIAMOND)
      device->drawDiamond(rect());
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
CQChartsGraphVizNodeObj::
drawFg(PaintDevice *device) const
{
  if (! plot_->isNodeTextVisible())
    return;

  auto prect = plot_->windowToPixel(rect());

  //---

  // set font
  plot_->setPainterFont(device, plot_->nodeTextFont());

  QFontMetricsF fm(device->font());

  //---

  // set text pen
  auto ic = calcColorInd();

  PenBrush penBrush;

  auto c = plot_->interpNodeTextColor(ic);

  plot_->setPen(penBrush, PenData(true, c, plot_->nodeTextAlpha()));

  device->setPen(penBrush.pen);

  //---

  double textMargin = 4; // pixels

  auto str = node()->label();

  if (! str.length())
    str = node()->name();

  //---

  double ptw = fm.horizontalAdvance(str);

  double clipLength = plot_->lengthPixelWidth(plot()->nodeTextClipLength());

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

  auto *plot = const_cast<CQChartsGraphVizPlot *>(plot_);

  if (plot->isNodeTextSingleScale())
    device->setNull(true);

  // only support contrast
  auto textOptions = plot_->nodeTextOptions(device);

  textOptions.angle = Angle();
  textOptions.align = Qt::AlignLeft;
  textOptions.html  = false;

  if (shapeType() == ShapeType::DIAMOND || shapeType() == ShapeType::BOX ||
      shapeType() == ShapeType::POLYGON || shapeType() == ShapeType::CIRCLE ||
      shapeType() == ShapeType::DOUBLE_CIRCLE) {
    if (rect().isValid()) {
      textOptions.align = Qt::AlignHCenter | Qt::AlignVCenter;

      CQChartsDrawUtil::drawTextInBox(device, rect(), str, textOptions);

      if (plot->isNodeTextSingleScale())
        plot->addDrawTextData(CQChartsGraphVizPlot::DrawTextData(rect(), str, c, textOptions));
    }
  }
  else {
    textOptions.align = Qt::AlignLeft;

    CQChartsDrawUtil::drawTextAtPoint(device, pt, str, textOptions);

    if (plot->isNodeTextSingleScale())
      plot->addDrawTextData(CQChartsGraphVizPlot::DrawTextData(pt, str, c, textOptions));
  }

  if (plot->isNodeTextSingleScale())
    device->setNull(false);
}

void
CQChartsGraphVizNodeObj::
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
CQChartsGraphVizNodeObj::
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

//------

CQChartsGraphVizEdgeObj::
CQChartsGraphVizEdgeObj(const Plot *plot, const BBox &rect, Edge *edge) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect), plot_(plot), edge_(edge)
{
  //setDetailHint(DetailHint::MAJOR);

  if (edge->modelInd().isValid())
    setModelInd(edge->modelInd());
}

CQChartsGraphVizEdgeObj::
~CQChartsGraphVizEdgeObj()
{
  auto *pedge = dynamic_cast<CQChartsGraphVizPlotEdge *>(edge_);

  if (pedge)
    pedge->setObj(nullptr);
}

//---

CQChartsGraphVizEdgeObj::ShapeType
CQChartsGraphVizEdgeObj::
shapeType() const
{
  return static_cast<CQChartsGraphVizEdgeObj::ShapeType>(edge()->shapeType());
}

void
CQChartsGraphVizEdgeObj::
setShapeType(const ShapeType &s)
{
  edge()->setShapeType(static_cast<CQChartsGraphVizPlotEdge::ShapeType>(s));
}

//---

QString
CQChartsGraphVizEdgeObj::
calcId() const
{
  auto *srcNode  = dynamic_cast<CQChartsGraphVizPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsGraphVizPlotNode *>(edge()->destNode());
  assert(srcNode && destNode);

  auto *srcObj  = srcNode ->obj();
  auto *destObj = destNode->obj();

  if (! srcObj || ! destObj)
    return QString("%1:%4").arg(typeName()).arg(edge()->id());

  return QString("%1:%2:%3:%4").arg(typeName()).
           arg(srcObj->calcId()).arg(destObj->calcId()).arg(edge()->id());
}

QString
CQChartsGraphVizEdgeObj::
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

  auto *srcNode  = dynamic_cast<CQChartsGraphVizPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsGraphVizPlotNode *>(edge()->destNode());
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
CQChartsGraphVizEdgeObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  auto path1 = (path.length() ? path + "/" : ""); path1 += propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);

  model->addProperty(path1, this, "shapeType")->setDesc("Shape");
}

//---

bool
CQChartsGraphVizEdgeObj::
inside(const Point &p) const
{
  return path_.contains(p.qpoint());
}

//---

void
CQChartsGraphVizEdgeObj::
getObjSelectIndices(Indices &inds) const
{
  for (const auto &c : plot()->modelColumns())
    addColumnSelectIndex(inds, c);
}

CQChartsGraphVizEdgeObj::PlotObjs
CQChartsGraphVizEdgeObj::
getConnected() const
{
  PlotObjs plotObjs;

  auto *srcNode  = dynamic_cast<CQChartsGraphVizPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsGraphVizPlotNode *>(edge()->destNode());
  assert(srcNode && destNode);

  auto *srcObj  = srcNode ->obj();
  auto *destObj = destNode->obj();

  plotObjs.push_back(srcObj);
  plotObjs.push_back(destObj);

  return plotObjs;
}

//---

void
CQChartsGraphVizEdgeObj::
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
  auto *srcNode  = dynamic_cast<CQChartsGraphVizPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsGraphVizPlotNode *>(edge()->destNode());
  assert(srcNode && destNode);

  auto *srcObj  = srcNode ->obj();
  auto *destObj = destNode->obj();

  bool isSelf = (srcObj == destObj);

  auto srcRect  = srcNode ->rect();
  auto destRect = destNode->rect();

  bool isArrow      = plot()->isEdgeArrow();
  bool isEdgeScaled = (plot_->isEdgeScaled() && edge()->hasValue());

  if (! srcRect.isSet() || ! destRect.isSet())
    return;

  //---

  bool isCentered = plot()->isEdgeCentered();
  bool usePath    = plot()->isEdgePath();

  auto orient = plot_->orientation();

  //---

  // get default connection line (no path)
  Point           p1, p2;
  Qt::Orientation orient1 = orient, orient2 = orient;

  CQChartsDrawUtil::rectConnectionPoints(srcRect, destRect, p1, p2, orient1, orient2,
                                         /*cornerPoints*/false);

  //---

  Point srcPoint, destPoint;

  if (! isCentered && srcNode->hasEdgePoint(edge()))
    srcPoint = srcNode->edgePoint(edge());
  else
    srcPoint = p1;

  if (! isCentered && destNode->hasEdgePoint(edge()))
    destPoint = destNode->edgePoint(edge());
  else
    destPoint = p2;

  //---

  // calc edge path
  QPainterPath epath1;

  if (plot()->edgeShape() == Plot::EdgeShape::NONE) {
    // add start and end node points to edge path
    auto epath = edge()->edgePath();

    class PathVisitor : public CQChartsDrawUtil::PathVisitor {
     public:
      PathVisitor(QPainterPath &path, const BBox &srcRect, const BBox &destRect) :
       path_(path) {
        rect_ = srcRect + destRect;
      }

      void moveTo(const Point &p) override { path_.lineTo(mapPoint(p)); }
      void lineTo(const Point &p) override { path_.lineTo(mapPoint(p)); }

      void quadTo(const Point &p1, const Point &p2) override {
        path_.quadTo(mapPoint(p1), mapPoint(p2)); }

      void curveTo(const Point &p1, const Point &p2, const Point &p3) override {
        path_.cubicTo(mapPoint(p1), mapPoint(p2), mapPoint(p3)); }

      const QPainterPath &path() const { return path_; }

     private:
      QPointF mapPoint(const Point &p) {
        auto x = CMathUtil::map(p.x, 0, 1, rect_.getXMin(), rect_.getXMax());
        auto y = CMathUtil::map(p.y, 0, 1, rect_.getYMin(), rect_.getYMax());

        return QPointF(x, y);
      }

     private:
      QPainterPath &path_;
      BBox          rect_;
    };

    epath1.moveTo(srcPoint.qpoint());

    PathVisitor visitor(epath1, srcRect, destRect);

    CQChartsDrawUtil::visitPath(epath, visitor);

    epath1.lineTo(destPoint.qpoint());
  }
  else {
    if (srcNode->hasEdgePoint(edge()))
      epath1.moveTo(srcPoint.qpoint());

    if (destNode->hasEdgePoint(edge()))
      epath1.lineTo(destPoint.qpoint());

    usePath = (epath1.elementCount() > 1);
  }

  //---

  // calc edge width
  double edgeScale { 1.0 };

  if (isEdgeScaled) {
    auto maxValue = plot_->maxEdgeValue().realOr(0.0);

    edgeScale = (maxValue > 0.0 ? edge()->value().real()/maxValue : 0.0);
  }

  double lw;

  if (orient == Qt::Horizontal)
    lw = plot_->lengthPlotHeight(plot()->edgeWidth());
  else
    lw = plot_->lengthPlotWidth(plot()->edgeWidth());

  lw *= edgeScale;

  bool isLine = (plot()->edgeWidth().value() <= 0.0);

  //---

  // calc edge path
  path_  = QPainterPath();
  epath_ = QPainterPath();

  auto edgeType = static_cast<CQChartsEdgeType>(plot()->edgeShape());

  if (! epath1.isEmpty() && usePath) {
    epath_ = epath1;

    CQChartsArrowData arrowData;

    if (isArrow && edge()->isDirected())
      arrowData.setTHeadType(CQChartsArrowData::HeadType::ARROW);

    QPainterPath path1;

    CQChartsArrow::pathAddArrows(epath_, arrowData, lw,
                                 plot_->arrowWidth(), plot_->arrowWidth(), path_);
  }
  else {
    if (isArrow) {
      const_cast<CQChartsGraphVizPlot *>(plot())->setUpdatesEnabled(false);

      if (! isSelf) {
        CQChartsDrawUtil::curvePath(epath_, srcPoint, destPoint, edgeType, orient1, orient2);

        CQChartsArrowData arrowData;

        arrowData.setFHeadType(CQChartsArrowData::HeadType::ARROW);
        arrowData.setTHeadType(CQChartsArrowData::HeadType::ARROW);

        CQChartsArrow::pathAddArrows(epath_, arrowData, lw,
                                     plot_->arrowWidth(), plot_->arrowWidth(), path_);
      }
      else {
        CQChartsDrawUtil::selfCurvePath(epath_, srcRect, edgeType, orient1);

        CQChartsArrow::selfPath(path_, srcRect, /*fhead*/true, /*thead*/true, lw);
      }

      const_cast<CQChartsGraphVizPlot *>(plot())->setUpdatesEnabled(true);
    }
    else {
      if (! isSelf) {
        CQChartsDrawUtil::curvePath(epath_, srcPoint, destPoint, edgeType, orient1, orient2);

        CQChartsDrawUtil::edgePath(path_, srcPoint, destPoint, lw, edgeType, orient1, orient2);
      }
      else {
        CQChartsDrawUtil::selfCurvePath(epath_, srcRect, edgeType, orient1);

        CQChartsDrawUtil::selfEdgePath(path_, srcRect, lw, edgeType, orient1);
      }
    }
  }

  //---

  // draw edge
  if (isLine)
    device->drawPath(epath_);
  else {
    if (edgeType == CQChartsDrawUtil::EdgeType::RECTILINEAR)
      device->drawPath(path_.simplified());
    else
      device->drawPath(path_);
  }

  device->resetColorNames();
}

void
CQChartsGraphVizEdgeObj::
drawFg(PaintDevice *device) const
{
  if (! plot_->isEdgeTextVisible())
    return;

  auto str = edge()->label();
  if (! str.length()) return;

  //---

  // set font
  plot_->setPainterFont(device, plot_->edgeTextFont());

  QFontMetricsF fm(device->font());

  //---

  // set text pen
  auto ic = calcColorInd();

  PenBrush penBrush;

  auto c = plot_->interpEdgeTextColor(ic);

  plot_->setPen(penBrush, PenData(true, c, plot_->edgeTextAlpha()));

  device->setPen(penBrush.pen);

  //---

#if 0
  double textMargin = 4; // pixels

  double ptw = fm.horizontalAdvance(str);

  //---

  // get connection rect of source and destination object
  auto *srcNode  = dynamic_cast<CQChartsGraphVizPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsGraphVizPlotNode *>(edge()->destNode());
  assert(srcNode && destNode);

  auto srcRect  = edge()->srcNode ()->rect();
  auto destRect = edge()->destNode()->rect();

  auto *srcObj  = srcNode ->obj();
  auto *destObj = destNode->obj();

  bool isSelf = (srcObj == destObj);

  auto rect = (isSelf ? srcRect : srcRect + destRect);

  auto prect = plot_->windowToPixel(rect);

  //---

  double tx = prect.getXMid() - textMargin - ptw/2.0;
  double ty = prect.getYMid() + (fm.ascent() - fm.descent())/2;

  auto pt = plot_->pixelToWindow(Point(tx, ty));
#else
  auto pt = Point(CQChartsDrawUtil::pathMidPoint(epath_));
#endif

  // only support contrast
  auto textOptions = plot_->edgeTextOptions(device);

  textOptions.angle     = Angle();
  textOptions.align     = Qt::AlignCenter;
  textOptions.formatted = false;
  textOptions.scaled    = false;
  textOptions.html      = false;

  CQChartsDrawUtil::drawTextAtPoint(device, pt, str, textOptions);
}

void
CQChartsGraphVizEdgeObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // set fill and stroke
  auto *srcNode  = dynamic_cast<CQChartsGraphVizPlotNode *>(edge()->srcNode ());
  auto *destNode = dynamic_cast<CQChartsGraphVizPlotNode *>(edge()->destNode());
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

  bool isLine = (plot()->edgeWidth().value() <= 0.0);

  if (isLine) {
    penBrush.pen  .setColor(fc);
    penBrush.brush.setStyle(Qt::NoBrush);
  }

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

//------

CQChartsGraphVizPlotCustomControls::
CQChartsGraphVizPlotCustomControls(CQCharts *charts) :
 CQChartsConnectionPlotCustomControls(charts, "graphviz")
{
}

void
CQChartsGraphVizPlotCustomControls::
init()
{
  addWidgets();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsGraphVizPlotCustomControls::
addWidgets()
{
  addConnectionColumnWidgets();

  addColorColumnWidgets("Cell Color");
}

void
CQChartsGraphVizPlotCustomControls::
connectSlots(bool b)
{
  CQChartsConnectionPlotCustomControls::connectSlots(b);
}

void
CQChartsGraphVizPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  plot_ = dynamic_cast<CQChartsGraphVizPlot *>(plot);

  CQChartsConnectionPlotCustomControls::setPlot(plot);
}

void
CQChartsGraphVizPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  CQChartsConnectionPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

CQChartsColor
CQChartsGraphVizPlotCustomControls::
getColorValue()
{
  return plot_->nodeFillColor();
}

void
CQChartsGraphVizPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  plot_->setNodeFillColor(c);
}

//---

CQChartsGraphVizPlotNode::
CQChartsGraphVizPlotNode(const QString &str) :
 str_(str)
{
}

CQChartsGraphVizPlotNode::
~CQChartsGraphVizPlotNode()
{
  assert(! obj_);
}

void
CQChartsGraphVizPlotNode::
setObj(Obj *obj)
{
  obj_ = obj;
}

const CQChartsGraphVizPlotNode::BBox &
CQChartsGraphVizPlotNode::
rect() const
{
  return rect_;
}

void
CQChartsGraphVizPlotNode::
setRect(const BBox &rect)
{
  rect_ = rect;

  if (obj_) // TODO: assert null or use move by
    obj_->setRect(rect);
}

void
CQChartsGraphVizPlotNode::
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
CQChartsGraphVizPlotNode::
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
CQChartsGraphVizPlotNode::
hasDestNode(Node *destNode) const
{
  for (auto &destEdge : destEdges()) {
    if (destEdge->destNode() == destNode)
      return true;
  }

  return false;
}

CQChartsGraphVizPlotEdge *
CQChartsGraphVizPlotNode::
findSrcEdge(Node *node) const
{
  for (auto &srcEdge : srcEdges()) {
    if (srcEdge->srcNode() == node)
      return srcEdge;
  }

  return nullptr;
}

CQChartsGraphVizPlotEdge *
CQChartsGraphVizPlotNode::
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
CQChartsGraphVizPlotNode::
moveBy(const Point &delta)
{
  rect_.moveBy(delta);

  if (obj_)
    obj_->moveBy(delta);
}

void
CQChartsGraphVizPlotNode::
scale(double fx, double fy)
{
  rect_.scale(fx, fy);

  if (obj_)
    obj_->scale(fx, fy);
}

//---

bool
CQChartsGraphVizPlotNode::
hasEdgePoint(Edge *edge) const
{
  auto p = edgePoints_.find(edge->id());

  return (p != edgePoints_.end());
}

void
CQChartsGraphVizPlotNode::
setEdgePoint(Edge *edge, const Point &p)
{
  auto x = CMathUtil::map(p.x, rect_.getXMin(), rect_.getXMax(), 0, 1);
  auto y = CMathUtil::map(p.y, rect_.getYMin(), rect_.getYMax(), 0, 1);

  edgePoints_[edge->id()] = Point(x, y);
}

CQChartsGeom::Point
CQChartsGraphVizPlotNode::
edgePoint(Edge *edge) const
{
  auto p = edgePoints_.find(edge->id());
  assert(p != edgePoints_.end());

  auto x = CMathUtil::map((*p).second.x, 0, 1, rect_.getXMin(), rect_.getXMax());
  auto y = CMathUtil::map((*p).second.y, 0, 1, rect_.getYMin(), rect_.getYMax());

  return Point(x, y);
}

//---

CQChartsGraphVizPlotEdge::
CQChartsGraphVizPlotEdge(const OptReal &value, Node *srcNode, Node *destNode) :
 value_(value), srcNode_(srcNode), destNode_(destNode)
{
}

CQChartsGraphVizPlotEdge::
~CQChartsGraphVizPlotEdge()
{
  assert(! obj_);
}

void
CQChartsGraphVizPlotEdge::
setObj(Obj *obj)
{
  obj_ = obj;
}
