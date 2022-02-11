#include <CQChartsDendrogramPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQCharts.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsHtml.h>
#include <CQChartsTip.h>
#include <CQChartsNamePair.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <CBuchHeim.h>

namespace {

class Tree : public CBuchHeim::Tree {
 public:
  using Node = CQChartsDendrogram::Node;

 public:
  Tree(Node *node) :
   CBuchHeim::Tree(node->name().toStdString()), node_(node) {
  }

  Node *node() const { return node_; }

 private:
  Node *node_ { nullptr };
};

class PlotDendrogram : public CQChartsDendrogram {
 public:
  using ModelIndex = CQChartsModelIndex;
  using OptReal    = CQChartsOptReal;

 public:
  class PlotNode : public CQChartsDendrogram::Node {
   public:
    PlotNode(CQChartsDendrogram::Node *parent, const QString &name="", double size=1.0) :
     CQChartsDendrogram::Node(parent, name, size) {
    }

    const ModelIndex &modelInd() const { return modelInd_; }
    void setModelInd(const ModelIndex &v) { modelInd_ = v; }

    const OptReal &colorValue() const { return colorValue_; }
    void setColorValue(const OptReal &r) { colorValue_ = r; }

    const OptReal &sizeValue() const { return sizeValue_; }
    void setSizeValue(const OptReal &v) { sizeValue_ = v; }

   private:
    ModelIndex modelInd_;
    OptReal    colorValue_;
    OptReal    sizeValue_;
  };

 public:
  PlotDendrogram() :
   CQChartsDendrogram() {
  }

  Node *createNode(CQChartsDendrogram::Node *hier, const QString &name,
                   double size) const override {
    return new PlotNode(hier, name, size);
  }
};

}

//---

CQChartsDendrogramPlotType::
CQChartsDendrogramPlotType()
{
}

void
CQChartsDendrogramPlotType::
addParameters()
{
  startParameterGroup("Dendrogram");

  addColumnParameter("name" , "name" , "nameColumn").
   setStringColumn().setRequired().setPropPath("columns.name"  ).setTip("Name column");
  addColumnParameter("link" , "link" , "linkColumn").
   setStringColumn().setRequired().setPropPath("columns.line"  ).setTip("Link column");
  addColumnParameter("value", "Value", "valueColumn").
   setNumericColumn().setRequired().setPropPath("columns.value").setTip("Value column");
  addColumnParameter("size" , "Size" , "sizeColumn").
   setNumericColumn().setPropPath("columns.size" ).setTip("Size column");

  endParameterGroup();

  //---

  // options
  addEnumParameter("orientation", "Orientation", "orientation").
    addNameValue("HORIZONTAL", static_cast<int>(Qt::Horizontal)).
    addNameValue("VERTICAL"  , static_cast<int>(Qt::Vertical  )).
    setTip("Draw orientation");

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsDendrogramPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Dendrogram Plot").
    h3("Summary").
     p("Draw hierarchical data using collapsible tree.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/dendrogram.png"));
}

CQChartsPlot *
CQChartsDendrogramPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsDendrogramPlot(view, model);
}

//------

CQChartsDendrogramPlot::
CQChartsDendrogramPlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("dendrogram"), model),
 CQChartsObjNodeShapeData    <CQChartsDendrogramPlot>(this),
 CQChartsObjEdgeShapeData    <CQChartsDendrogramPlot>(this),
 CQChartsObjHierLabelTextData<CQChartsDendrogramPlot>(this),
 CQChartsObjLeafLabelTextData<CQChartsDendrogramPlot>(this)
{
}

CQChartsDendrogramPlot::
~CQChartsDendrogramPlot()
{
  term();
}

//---

void
CQChartsDendrogramPlot::
init()
{
  CQChartsPlot::init();

  //---

  NoUpdate noUpdate(this);

  setNodeFillColor(Color(Color::Type::PALETTE));

  //---

  addTitle();

  //---

  registerSlot("expand"      , QStringList());
  registerSlot("expand_all"  , QStringList());
  registerSlot("collapse_all", QStringList());
}

void
CQChartsDendrogramPlot::
term()
{
  delete dendrogram_;
}

//---

void
CQChartsDendrogramPlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns", "nameColumn" , "name" , "Name column" );
  addProp("columns", "linkColumn" , "link" , "Link column" );
  addProp("columns", "valueColumn", "value", "Value column");
  addProp("columns", "sizeColumn" , "size" , "Size column" );

  // node
  addProp("node", "symbolSize", "symbolSize", "Node Symbol size");

  addFillProperties("node/fill"  , "nodeFill"  , "Node");
  addLineProperties("node/stroke", "nodeStroke", "Node");

  // edge
  addProp("edge", "edgeScaled", "scaled", "Edge is scaled");
  addProp("edge", "edgeWidth" , "width" , "Edge width");

  addFillProperties("edge/fill"  , "edgeFill"  , "Edge");
  addLineProperties("edge/stroke", "edgeStroke", "Edge");

  // hier label
  addProp("label/hier", "hierLabelTextVisible", "visible", "Hier Labels visible");

  addTextProperties("label/hier", "hierLabelText", "Hier Label",
                    CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  addProp("label/leaf", "leafLabelTextVisible", "visible", "Leaf Labels visible");

  addTextProperties("label/leaf", "leafLabelText", "Leaf Label",
                    CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  addProp("label", "textMargin" , "margin" , "Text margin in pixels")->setMinValue(1.0);
  addProp("label", "rotatedText", "rotated", "Rotate text");

  // options
  addProp("options", "placeType"  , "placeType"  , "Place type");
  addProp("options", "orientation", "orientation", "Draw orientation");
}

//---

void
CQChartsDendrogramPlot::
setNameColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() {
    needsReload_ = true; updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsDendrogramPlot::
setLinkColumn(const Column &c)
{
  CQChartsUtil::testAndSet(linkColumn_, c, [&]() {
    needsReload_ = true; updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsDendrogramPlot::
setValueColumn(const Column &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() {
    needsReload_ = true; updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsDendrogramPlot::
setColorColumn(const Column &c)
{
  CQChartsUtil::testAndSet(colorColumnData_.column, c, [&]() {
    needsReload_ = true; updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsDendrogramPlot::
setSizeColumn(const Column &c)
{
  CQChartsUtil::testAndSet(sizeColumn_, c, [&]() {
    needsReload_ = true; updateRangeAndObjs(); emit customDataChanged();
  } );
}

//---

CQChartsColumn
CQChartsDendrogramPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "name" ) c = this->nameColumn();
  else if (name == "link" ) c = this->linkColumn();
  else if (name == "value") c = this->valueColumn();
  else if (name == "size" ) c = this->sizeColumn();
  else                      c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsDendrogramPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "name" ) this->setNameColumn(c);
  else if (name == "link" ) this->setLinkColumn(c);
  else if (name == "value") this->setValueColumn(c);
  else if (name == "size" ) this->setSizeColumn(c);
  else                      CQChartsPlot::setNamedColumn(name, c);
}

//---

void
CQChartsDendrogramPlot::
setSymbolSize(const Length &s)
{
  CQChartsUtil::testAndSet(symbolSize_, s, [&]() {
    needsPlace_ = true; updateRangeAndObjs();
  } );
}

double
CQChartsDendrogramPlot::
calcSymbolSize() const
{
  auto ss = lengthPixelWidth(symbolSize_);

  if (ss <= 0) {
    auto hfont = view()->plotFont(this, hierLabelTextFont());
    auto lfont = view()->plotFont(this, leafLabelTextFont());

    QFontMetricsF hfm(hfont);
    QFontMetricsF lfm(lfont);

    ss = std::max(hfm.height(), lfm.height()) + 2.0;
  }

  return ss;
}

//---

void
CQChartsDendrogramPlot::
setEdgeScaled(bool b)
{
  CQChartsUtil::testAndSet(edgeScaled_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDendrogramPlot::
setEdgeWidth(const Length &l)
{
  CQChartsUtil::testAndSet(edgeWidth_, l, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsDendrogramPlot::
setTextMargin(double r)
{
  CQChartsUtil::testAndSet(textMargin_, r, [&]() {
    drawObjs();
  } );
}

void
CQChartsDendrogramPlot::
setRotatedText(bool b)
{
  CQChartsUtil::testAndSet(rotatedText_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsDendrogramPlot::
setOrientation(const Qt::Orientation &orient)
{
  CQChartsUtil::testAndSet(orientation_, orient, [&]() {
    needsPlace_ = true; updateRangeAndObjs();
  } );
}

void
CQChartsDendrogramPlot::
setPlaceType(const PlaceType &t)
{
  CQChartsUtil::testAndSet(placeType_, t, [&]() {
    needsPlace_ = true; updateRangeAndObjs();
  } );
}

//---

CQChartsGeom::Range
CQChartsDendrogramPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsDendrogramPlot::calcRange");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsDendrogramPlot *>(this);

  th->clearErrors();

  //---

  // check columns
  bool columnsValid = true;

  if (! checkColumn       (nameColumn (), "Name" )) columnsValid = false;
  if (! checkColumn       (linkColumn (), "Link" )) columnsValid = false;
  if (! checkNumericColumn(valueColumn(), "Value")) columnsValid = false;
  if (! checkNumericColumn(sizeColumn (), "Size" )) columnsValid = false;

  if (! columnsValid)
    return Range(0.0, 0.0, 1.0, 1.0);

  //---

  Range dataRange;

  if (placeType() == PlaceType::CIRCULAR) {
    dataRange.updateRange(-1, -1);
    dataRange.updateRange( 1,  1);
  }
  else {
    dataRange.updateRange(0, 0);
    dataRange.updateRange(1, 1);
  }

  //---

  applyEqualScale(dataRange);

  //---

  return dataRange;
}

void
CQChartsDendrogramPlot::
placeModel() const
{
  auto *th = const_cast<CQChartsDendrogramPlot *>(this);

  for (const auto &plotObj : plotObjects()) {
    auto *nodeObj = dynamic_cast<NodeObj *>(plotObj);

    if (nodeObj)
      nodeObj->resetNode();
  }

  delete th->dendrogram_;

  th->dendrogram_ = new PlotDendrogram;

  //---

  if (linkColumn().isValid()) {
    th->tempRoot_ = th->dendrogram_->createNode(nullptr, "tempRoot", 0.0);
  }

  //---

  // add name values
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsDendrogramPlot *plot, Edges &edges) :
     plot_(plot), edges_(edges) {
    }

    State visit(const QAbstractItemModel *model, const VisitData &data) override {
      auto path = CQChartsModelUtil::parentPath(model, data.parent);

      //---

      QString          nameStr;
      CQChartsNamePair namePair;
      ModelIndex       nameModelInd;

      // get name
      if      (plot_->nameColumn().isValid()) {
        nameModelInd = ModelIndex(plot_, data.row, plot_->nameColumn(), data.parent);

      //auto nameInd  = modelIndex(nameModelInd);
      //auto nameInd1 = normalizeIndex(nameInd);

        bool ok1;
        nameStr = plot_->modelString(nameModelInd, ok1);

        if (ok1 && path.length())
          nameStr = path + "/" + nameStr;
      }
      else if (plot_->linkColumn().isValid()) {
        ModelIndex linkModelInd(plot_, data.row, plot_->linkColumn(), data.parent);

        bool ok1;
        auto linkStr = plot_->modelString(linkModelInd, ok1);

        namePair = CQChartsNamePair(linkStr, "/");
      }

      //--

      // get value
      OptReal value;

      ModelIndex valueModelInd(plot_, data.row, plot_->valueColumn(), data.parent);

      bool ok2;
      double rvalue = plot_->modelReal(valueModelInd, ok2);

      if (ok2) {
        if (CMathUtil::isNaN(rvalue))
          return State::SKIP;

        value = OptReal(rvalue);
      }
      else {
        if (! plot_->isSkipBad())
          return addDataError(valueModelInd, "Invalid Value");
      }

      //---

      // get color
      OptReal colorValue;

      if (plot_->colorColumn().isValid()) {
        ModelIndex colorModelInd(plot_, data.row, plot_->colorColumn(), data.parent);

        bool ok3;
        double color = plot_->modelReal(colorModelInd, ok3);

        if (! ok3) {
          if (! plot_->isSkipBad())
            return addDataError(valueModelInd, "Invalid Color");

          color = 0.0;
        }

        if (! CMathUtil::isNaN(color))
          colorValue = OptReal(color);
      }

      //---

      // get size
      OptReal sizeValue;

      if (plot_->sizeColumn().isValid()) {
        ModelIndex sizeModelInd(plot_, data.row, plot_->sizeColumn(), data.parent);

        bool ok3;
        double size = plot_->modelReal(sizeModelInd, ok3);

        if (! ok3) {
          if (! plot_->isSkipBad())
            return addDataError(valueModelInd, "Invalid Size");

          size = 0.0;
        }

        if (! CMathUtil::isNaN(size))
          sizeValue = OptReal(size);
      }

      //---

      plot_->addNameValue(nameStr, namePair, value, nameModelInd, colorValue, sizeValue, edges_);

      //--

      return State::OK;
    }

   private:
    State addDataError(const ModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsDendrogramPlot *>(plot_)->addDataError(ind , msg);
      return State::SKIP;
    }

   private:
    const CQChartsDendrogramPlot*  plot_ { nullptr };
    CQChartsDendrogramPlot::Edges& edges_;
  };

  Edges edges;

  RowVisitor visitor(this, edges);

  visitModel(visitor);

  //---

  auto *root = (dendrogram_ ? dendrogram_->root() : nullptr);

  if (root)
    root->setOpen(true);

  //---

  if (linkColumn().isValid()) {
    th->tempRoot_->clear();

    for (const auto &edge : edges) {
      if (edge.to->parent() == tempRoot_)
        edge.from->addChild(edge.to, edge.value.value());
    }

    for (const auto &edge : edges) {
      if (edge.from->parent() == tempRoot_)
        root->addChild(edge.from, CQChartsDendrogram::OptReal());
    }

    delete th->tempRoot_;
  }

  //---

  place();
}

void
CQChartsDendrogramPlot::
place() const
{
  if (! dendrogram_)
    return;

  if      (placeType() == PlaceType::BUCHHEIM) {
    placeBuchheim();
  }
  else if (placeType() == PlaceType::CIRCULAR) {
    placeCircular();
  }
  else {
    dendrogram_->placeNodes();
  }
}

void
CQChartsDendrogramPlot::
placeBuchheim() const
{
  delete buchheimTree_;
  delete buchheimDrawTree_;

  buchheimTree_ = new Tree(dendrogram_->root());

  addBuchheimHierNode(buchheimTree_, dendrogram_->root());

  buchheimDrawTree_ = new CBuchHeim::DrawTree(buchheimTree_);

  buchheimDrawTree_->place();

  buchheimDrawTree_->fixOverlaps();

  buchheimDrawTree_->normalize(/*equalScale*/false);

  //---

  dendrogram_->root()->resetPlaced();

  moveBuchheimHierNode(buchheimDrawTree_);
}

void
CQChartsDendrogramPlot::
addBuchheimHierNode(CBuchHeim::Tree *tree, Node *hierNode) const
{
  if (! hierNode->isOpen())
    return;

  for (auto &hierNode1 : hierNode->getChildren()) {
    auto *childTree = new Tree(hierNode1.node);

    tree->addChild(CBuchHeim::TreeP(childTree));

    addBuchheimHierNode(childTree, hierNode1.node);
  }

  for (auto &node : hierNode->getNodes()) {
    auto *childTree = new Tree(node.node);

    tree->addChild(CBuchHeim::TreeP(childTree));
  }
}

void
CQChartsDendrogramPlot::
moveBuchheimHierNode(CBuchHeim::DrawTree *drawTree) const
{
  auto *tree = static_cast<Tree *>(drawTree->tree());

  auto *node1 = tree->node();

  double x1 = drawTree->x1();
  double y1 = drawTree->y1();
  double x2 = drawTree->x2();
  double y2 = drawTree->y2();

  double xc = CMathUtil::avg(x1, x2);
  double yc = CMathUtil::avg(y1, y2);
  double r  = 0.4*std::min(x2 - x1, y2 - y1);

  if (orientation() == Qt::Horizontal)
    node1->setBBox(BBox(yc - r, xc - r, yc + r, xc + r));
  else
    node1->setBBox(BBox(xc - r, 1.0 - (yc - r), xc + r, 1.0 - (yc + r)));

  node1->setPlaced(true);

  for (const auto &node : drawTree->children())
    moveBuchheimHierNode(node.get());
}

void
CQChartsDendrogramPlot::
placeCircular() const
{
  dendrogram_->root()->resetPlaced();

  //---

  CircularDepth circularDepth;
  NodeAngles    nodeAngles;

  int maxDepth = 0;

  initCircularDepth(dendrogram_->root(), circularDepth, 0, maxDepth);

  auto r  = 0.0;
  auto dr = (maxDepth > 0 ? 1.0/maxDepth : 0.0);

  double cs = calcSymbolSize();

  double cw = pixelToWindowWidth (cs);
  double ch = pixelToWindowHeight(cs);

  for (const auto &pc : circularDepth) {
    //auto depth = pc.first;

    const auto &cnode = pc.second;

    Node *pnode { nullptr };

    int ind = 0;

    auto a1  = 0.0;
    auto da1 = 2.0*M_PI;

    for (auto &node : cnode.nodes) {
      auto *pnode1 = node->parent();

      if (pnode1 != pnode) {
        pnode = pnode1;
        ind   = 0;

        auto a  = 0.0;
        auto da = 2.0*M_PI;

        auto pa = nodeAngles.find(pnode);

        if (pa != nodeAngles.end()) {
          a  = (*pa).second.a;
          da = (*pa).second.da;
        }

        auto n = pnode->getChildren().size() + pnode->getNodes().size();

        if (n > 1) {
          a1  = a;
          da1 = da/double(n);
        }
        else {
          a1  = a + da/2.0;
          da1 = 0.0;
        }
      }
      else
        ++ind;

      auto a2 = a1 + ind*da1;

      AnglePair ap;

      ap.a  = a2 - da1/2.0;
      ap.da = da1;

      nodeAngles[node] = ap;

      auto x = std::cos(a2)*r;
      auto y = std::sin(a2)*r;

      auto bbox = BBox(x - cw/2.0, y - ch/2.0, x + cw/2.0, y + ch/2.0);

      node->setBBox(bbox);

      node->setPlaced(true);
    }

    r += dr;
  }
}

void
CQChartsDendrogramPlot::
initCircularDepth(Node *hierNode, CircularDepth &circularDepth, int depth, int &maxDepth) const
{
  maxDepth = std::max(maxDepth, depth);

  circularDepth[depth].nodes.push_back(hierNode);

  if (hierNode->isOpen()) {
    maxDepth = std::max(maxDepth, depth + 1);

    for (const auto &child : hierNode->getChildren())
      initCircularDepth(child.node, circularDepth, depth + 1, maxDepth);

    for (const auto &node : hierNode->getNodes())
      circularDepth[depth + 1].nodes.push_back(node.node);
  }
}

void
CQChartsDendrogramPlot::
addNameValue(const QString &nameStr, const CQChartsNamePair &namePair, const OptReal &value,
             const ModelIndex &modelInd, const OptReal &colorValue, OptReal &sizeValue,
             Edges &edges) const
{
  if (! dendrogram_->root()) {
    auto *hierNode = dendrogram_->addRootNode("root");

    hierNode->setOpen(false);
  }

  //---

  QStringList names;

  if (namePair.isValid()) {
    auto *fromNode = tempRoot_->findChild(namePair.name1());
    auto *toNode   = tempRoot_->findChild(namePair.name2());

    if (! fromNode)
      fromNode = dendrogram_->addHierNode(tempRoot_, namePair.name1());

    if (! toNode)
      toNode = dendrogram_->addHierNode(tempRoot_, namePair.name2());

    edges.push_back(Edge(fromNode, toNode, value));
  }
  else {
    // split name into hier path elements
    auto name1 = nameStr;

    int pos = name1.indexOf('/');

    if (pos != -1) {
      while (pos != -1) {
        auto lhs = name1.mid(0, pos);
        auto rhs = name1.mid(pos + 1);

        names.push_back(lhs);

        name1 = rhs;

        pos = name1.indexOf('/');
      }
    }
    else {
      names.push_back(name1);
    }

    //---

    // create nodes for hierarchy
    Node *hierNode = nullptr;

    for (const auto &n : names) {
      if (! hierNode) {
        hierNode = dendrogram_->root();

        hierNode->setName(n);
      }
      else {
        auto *hierNode1 = hierNode->findChild(n);

        if (! hierNode1) {
          hierNode = dendrogram_->addHierNode(hierNode, n);

          hierNode->setOpen(false);
        }
        else
          hierNode = hierNode1;
      }
    }

    auto *node = dendrogram_->addNode(hierNode, name1, value.realOr(0.0));
    assert(node);

    auto *node1 = dynamic_cast<PlotDendrogram::PlotNode *>(node); assert(node1);

    node1->setModelInd(modelInd);

    if (colorValue.isSet())
      node1->setColorValue(colorValue);

    if (sizeValue.isSet())
      node1->setSizeValue(sizeValue);
  }
}

CQChartsGeom::BBox
CQChartsDendrogramPlot::
calcExtraFitBBox() const
{
  CQPerfTrace trace("CQChartsDendrogramPlot::calcExtraFitBBox");

  BBox bbox;

  for (const auto &plotObj : plotObjects()) {
    auto *nodeObj = dynamic_cast<NodeObj *>(plotObj);
    if (! nodeObj) continue;

    bool is_hier = nodeObj->isHier();

    bool textVisible = (is_hier ? isHierLabelTextVisible() : isLeafLabelTextVisible());

    if (textVisible)
      bbox += nodeObj->textRect();
  }

  return bbox;
}

//------

bool
CQChartsDendrogramPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("QChartsDendrogramPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  if (needsReload_) {
    placeModel();

    needsReload_ = false;
  }
  else if (needsPlace_) {
    place();

    needsPlace_ = false;
  }

  //---

  auto *th = const_cast<CQChartsDendrogramPlot *>(this);

  auto *root = dendrogram_->root();

  if (root) {
    th->rootNodeObj_ = addNodeObj(root, objs, /*hier*/true);

    th->rootNodeObj_->setRoot(true);

    addNodeObjs(root, 0, rootNodeObj_, objs);
  }
  else
    th->rootNodeObj_ = nullptr;

  //---

  // init sum of child values for all hier nodes (including root)
  for (const auto &plotObj : objs) {
    auto *nodeObj = dynamic_cast<NodeObj *>(plotObj);
    if (! nodeObj) continue;

    auto *hierNode = dynamic_cast<const Node *>(nodeObj->node());

    if (hierNode) {
      nodeObj->setHierColor(calcHierColor(hierNode));
      nodeObj->setHierSize (calcHierSize (hierNode));
    }
  }

  return true;
}

double
CQChartsDendrogramPlot::
calcHierColor(const Node *hierNode) const
{
  double color = 0.0;

  for (const auto &child : hierNode->getChildren())
    color += calcHierColor(child.node);

  for (const auto &node : hierNode->getNodes()) {
    auto *node1 = dynamic_cast<const PlotDendrogram::PlotNode *>(node.node); assert(node1);

    color += node1->colorValue().realOr(0.0);
  }

  return color;
}

double
CQChartsDendrogramPlot::
calcHierSize(const Node *hierNode) const
{
  double size = 0.0;

  for (const auto &child : hierNode->getChildren())
    size += calcHierSize(child.node);

  for (const auto &node : hierNode->getNodes()) {
    auto *node1 = dynamic_cast<const PlotDendrogram::PlotNode *>(node.node); assert(node1);

    size += node1->sizeValue().realOr(0.0);
  }

  return size;
}

void
CQChartsDendrogramPlot::
addNodeObjs(Node *hier, int depth, NodeObj *parentObj, PlotObjs &objs) const
{
  for (auto &hierData : hier->getChildren()) {
    auto       *hierNode = hierData.node;
    const auto &value    = hierData.value;

    auto *hierNodeObj = addNodeObj(hierNode, objs, /*hier*/true);
    if (! hierNodeObj) continue;

    if (parentObj) {
      hierNodeObj->setParent(parentObj);

      parentObj->addChild(hierNodeObj, value);
    }

    addNodeObjs(hierNode, depth + 1, hierNodeObj, objs);
  }

  //------

  for (auto &nodeData : hier->getNodes()) {
    auto       *node  = nodeData.node;
    const auto &value = nodeData.value;

    auto *nodeObj = addNodeObj(node, objs, /*hier*/false);
    if (! nodeObj) continue;

    if (parentObj) {
      nodeObj->setParent(parentObj);

      parentObj->addChild(nodeObj, value);
    }
  }
}

CQChartsDendrogramNodeObj *
CQChartsDendrogramPlot::
addNodeObj(Node *node, PlotObjs &objs, bool isHier) const
{
  if (! node->isPlaced())
    return nullptr;

  BBox rect = getBBox(node);

  auto *obj = createNodeObj(node, rect);

  obj->setHier(isHier);

  objs.push_back(obj);

  return obj;
}

//---

void
CQChartsDendrogramPlot::
preDrawObjs(PaintDevice *device) const
{
  for (const auto &plotObj : plotObjects()) {
    auto *nodeObj = dynamic_cast<NodeObj *>(plotObj);
    if (! nodeObj) continue;

    nodeObj->drawEdges(device);
  }
}

//---

CQChartsGeom::BBox
CQChartsDendrogramPlot::
getBBox(Node *node) const
{
  if (placeType() == PlaceType::BUCHHEIM)
    return node->bbox();

  if (placeType() == PlaceType::CIRCULAR)
    return node->bbox();

  //---

  double cs = std::max(calcSymbolSize(), 1.0);
//double tm = std::max(textMargin(), 1.0);

  double cw = pixelToWindowWidth (cs);
  double ch = pixelToWindowHeight(cs);

//double mw = pixelToWindowWidth(tm);

//double xc = node->x() + mw;
  double xc = node->x();
  double yc = node->yc();

  if (orientation() == Qt::Horizontal)
    return BBox(xc - cw/2.0, yc - ch/2.0, xc + cw/2.0, yc + ch/2.0);
  else
    return BBox(yc - cw/2.0, 1.0 - (xc - ch/2.0), yc + cw/2.0, 1.0 - (xc + ch/2.0));
}

//------

bool
CQChartsDendrogramPlot::
handleSelectPress(const Point &p, SelMod /*selMod*/)
{
  Node *node = nullptr;

  PlotObjs plotObjs;

  plotObjsAtPoint(p, plotObjs, Constraints::SELECTABLE);

  if (plotObjs.size() == 1) {
    auto *nodeObj = dynamic_cast<NodeObj *>(plotObjs[0]);

    node = const_cast<Node *>(nodeObj->node());
  }

  if (! node) return false;

  node->setOpen(! node->isOpen());

  needsPlace_ = true;

  updateObjs();

  return true;
}

CQChartsDendrogramNodeObj *
CQChartsDendrogramPlot::
createNodeObj(Node *node, const BBox &rect) const
{
  return new CQChartsDendrogramNodeObj(this, node, rect);
}

//---

bool
CQChartsDendrogramPlot::
addMenuItems(QMenu *menu)
{
  menu->addSeparator();

  //---

  addMenuAction(menu, "Expand", SLOT(expandSlot()));
  addMenuAction(menu, "Expand All", SLOT(expandAllSlot()));
  addMenuAction(menu, "Collapse All", SLOT(collapseAllSlot()));

  return true;
}

void
CQChartsDendrogramPlot::
expandSlot()
{
  if (! dendrogram_)
    return;

  auto *root = dendrogram_->root();
  if (! root) return;

  expandNode(root, /*all*/false);

  needsPlace_ = true;

  updateObjs();
}

void
CQChartsDendrogramPlot::
expandAllSlot()
{
  if (! dendrogram_)
    return;

  auto *root = dendrogram_->root();
  if (! root) return;

  expandNode(root, /*all*/true);

  needsPlace_ = true;

  updateObjs();
}

void
CQChartsDendrogramPlot::
collapseAllSlot()
{
  if (! dendrogram_)
    return;

  auto *root = dendrogram_->root();
  if (! root) return;

  collapseNode(root, /*all*/true);

  needsPlace_ = true;

  updateObjs();
}

void
CQChartsDendrogramPlot::
expandNode(Node *hierNode, bool all)
{
  bool expandChildren = (all || hierNode->isOpen());

  hierNode->setOpen(true);

  if (expandChildren) {
    for (const auto &child : hierNode->getChildren())
      expandNode(child.node, all);
  }
}

void
CQChartsDendrogramPlot::
collapseNode(Node *hierNode, bool all)
{
  bool collapseChildren = (all || ! hierNode->isOpen());

  if (hierNode->isOpen())
    hierNode->setOpen(false);

  if (collapseChildren) {
    for (const auto &child : hierNode->getChildren())
      collapseNode(child.node, all);
  }
}

//---

CQChartsPlotCustomControls *
CQChartsDendrogramPlot::
createCustomControls()
{
  auto *controls = new CQChartsDendrogramPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsDendrogramNodeObj::
CQChartsDendrogramNodeObj(const CQChartsDendrogramPlot *plot, Node *node, const BBox &rect) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect), plot_(plot), node_(node), name_(node->name())
{
  setValue(node->size());
  setOpen (node->isOpen());

  auto *node1 = dynamic_cast<PlotDendrogram::PlotNode *>(node);

  if (node1) {
    setModelIndex(node1->modelInd());

    setColor(node1->colorValue());
    setSize (node1->sizeValue());
  }
}

void
CQChartsDendrogramNodeObj::
addChild(NodeObj *node, const OptReal &value)
{
  children_.push_back(Child(node, value));

  if (value.isSet())
    childTotal_ += value.real();
}

//---

QString
CQChartsDendrogramNodeObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(name());
}

QString
CQChartsDendrogramNodeObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  plot()->addNoTipColumns(tableTip);

  //---

  tableTip.addTableRow("Name", name());

  if (plot()->valueColumn().isValid())
    tableTip.addTableRow("Value", value());

  if (color().isSet())
    tableTip.addTableRow("Color", color().real());

  if (size().isSet())
    tableTip.addTableRow("Size", size().real());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

bool
CQChartsDendrogramNodeObj::
inside(const Point &p) const
{
  auto rect = displayRect();

  auto pbbox = plot_->windowToPixel(rect);

  pbbox.expand(2);

  auto pp = plot_->windowToPixel(p);

  return pbbox.inside(pp);
}

//---

CQChartsGeom::BBox
CQChartsDendrogramNodeObj::
textRect() const
{
  // get font
  bool is_hier = this->isHier();

  auto font  = (is_hier ? plot()->hierLabelTextFont() : plot()->leafLabelTextFont());
  auto qfont = plot()->view()->plotFont(plot(), font);

  //---

  // calc position
  Point         p;
  Angle         angle;
  Qt::Alignment align;
  bool          centered;

  calcTextPos(p, qfont, angle, align, centered);

  //---

  CQChartsPlotPaintDevice device(const_cast<CQChartsDendrogramPlot *>(plot()), nullptr);

  device.setFont(qfont);

  auto textOptions = (is_hier ?
    plot()->hierLabelTextOptions(&device) : plot()->leafLabelTextOptions(&device));

  textOptions.angle     = angle;
  textOptions.align     = align;
  textOptions.formatted = false;
  textOptions.scaled    = false;
  textOptions.html      = false;

  const auto &name = this->name();

  auto tbbox = CQChartsDrawUtil::calcTextAtPointRect(&device, p, name, textOptions,
                                                     centered, 0.0, 0.0);

#if 0
  QFontMetricsF fm(qfont);

  double dy = (fm.ascent() - fm.descent())/2.0;
  double dx = fm.width(name);

  auto pbbox = plot()->windowToPixel(rect());

  auto pc = pbbox.getCenter();
  auto pr = pbbox.getWidth()/2.0;

  Point p;

  if (is_hier)
    p = Point(pc.x - pr - dx, pc.y + dy);
  else
    p = Point(pc.x + pr, pc.y + dy);

  Point p1(p.x     , p.y - fm.ascent());
  Point p2(p.x + dx, p.y + fm.ascent());

  BBox ptbbox(p1, p2);

  auto tbbox = plot()->pixelToWindow(ptbbox);
#endif

  return tbbox;
}

void
CQChartsDendrogramNodeObj::
draw(PaintDevice *device) const
{
  auto rect1 = displayRect();

  //---

  // set pen and brush
  ColorInd colorInd;

  PenBrush penBrush;

  auto strokeColor = plot()->interpNodeStrokeColor(colorInd);
  auto fillColor   = plot()->interpNodeFillColor  (colorInd);

  bool is_hier = this->isHier();

  bool closed = (is_hier && ! isOpen());

  bool colored = false;

  if (plot()->colorColumn().isValid()) {
    auto maxColor = (parent() ? parent()->hierColor() : this->color().realOr(0.0));

    double color = 0.0;

    if (is_hier)
      color = hierColor();
    else
      color = this->color().realOr(0.0);

    auto c = plot()->colorFromColorMapPaletteValue(maxColor > 0 ? color/maxColor : color);

    fillColor = plot()->interpColor(c, colorInd);
    colored   = true;
  }

  plot()->setPenBrush(penBrush, plot()->nodePenData(strokeColor),
    (closed || colored) ? plot()->nodeBrushData(fillColor) : BrushData(false));

  plot()->updateObjPenBrushState(this, penBrush);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw node
  auto ss = calcSymbolSize();

  auto p = rect1.getCenter();

  auto symbol = (is_hier ? CQChartsSymbol::diamond() : CQChartsSymbol::circle());

  plot()->drawSymbol(device, p, symbol, ss, ss);

  //---

  drawText(device);

  //---

  if (plot()->showBoxes()) {
    auto bbox = textRect();

    drawDebugRect(device, bbox);
  }
}

void
CQChartsDendrogramNodeObj::
drawText(PaintDevice *device) const
{
  bool is_hier = this->isHier();

  bool textVisible = (is_hier ? plot_->isHierLabelTextVisible() : plot_->isLeafLabelTextVisible());
  if (! textVisible) return;

  //---

  // set pen
  ColorInd colorInd;
  PenBrush tpenBrush;

  auto tc = (is_hier ? plot()->interpHierLabelTextColor(colorInd) :
                       plot()->interpLeafLabelTextColor(colorInd));

  auto ta = (is_hier ? plot()->hierLabelTextAlpha() : plot()->leafLabelTextAlpha());

  plot()->setPen(tpenBrush, PenData(/*stroked*/true, tc, ta));

  device->setPen(tpenBrush.pen);

  //---

  // set font
  auto font = (is_hier ? plot()->hierLabelTextFont() : plot()->leafLabelTextFont());

  plot()->setPainterFont(device, font);

  //---

  // calc position
  Point         p;
  Angle         angle;
  Qt::Alignment align;
  bool          centered;

  calcTextPos(p, device->font(), angle, align, centered);

  //---

  // draw node text
  // only support contrast and clip
  const auto &name = this->name();

  auto textOptions = (is_hier ?
    plot()->hierLabelTextOptions(device) : plot()->leafLabelTextOptions(device));

  textOptions.angle     = angle;
  textOptions.align     = align;
  textOptions.formatted = false;
  textOptions.scaled    = false;
  textOptions.html      = false;

  CQChartsDrawUtil::drawTextAtPoint(device, p, name, textOptions, centered);
}

void
CQChartsDendrogramNodeObj::
calcTextPos(Point &p, const QFont &font, Angle &angle, Qt::Alignment &align, bool &centered) const
{
  auto rect1 = displayRect();

  const auto &name = this->name();

  QFontMetricsF fm(font);

//double dy = (fm.ascent() - fm.descent())/2.0;
  double dy = 0.0;
  double dx = fm.width(name);

  BBox pbbox = plot()->windowToPixel(rect1);

  double tm = std::max(plot()->textMargin(), 1.0);

  align    = Qt::AlignLeft | Qt::AlignVCenter;
  centered = false;

  Point pp;

  if (plot()->placeType() != Plot::PlaceType::CIRCULAR) {
    if (isRoot()) {
      pp = pbbox.getCenter();

      align = Qt::AlignCenter;
    }
    else {
      bool is_hier = this->isHier();

      if (plot()->orientation() == Qt::Horizontal) {
        if (is_hier)
          pp = Point(pbbox.getXMin() - dx - tm, pbbox.getYMid() + dy); // align right
        else
          pp = Point(pbbox.getXMax()      + tm, pbbox.getYMid() + dy); // align left
      }
      else {
        if (is_hier)
          pp = Point(pbbox.getXMid() - dx/2.0, pbbox.getYMin() - tm + dy); // align top
        else
          pp = Point(pbbox.getXMid() - dx/2.0, pbbox.getYMax() + tm + dy); // align bottom
      }
    }
  }
  else {
    pp = pbbox.getCenter();

    align = Qt::AlignCenter;

    if (! isRoot()) {
      auto p1 = plot()->pixelToWindow(pp);

      auto angle1 = Angle::radians(std::atan2(p1.y, p1.x));

      //---

      double dx1 = 0.0;

      if (plot()->isRotatedText()) {
        dx1 = plot()->pixelToWindowWidth (dx/2.0);
      //dy1 = plot()->pixelToWindowHeight(dy);
      }
      else {
        dx1 = plot()->pixelToWindowWidth(tm);
      }

      auto x1 = p1.x + dx1*angle1.cos();
      auto y1 = p1.y + dx1*angle1.sin();

      pp = plot()->windowToPixel(Point(x1, y1));

      //---

      if (plot()->isRotatedText()) {
        if (angle1.cos() < 0.0)
          angle1.flipX();

        angle = angle1;

        centered = true;
      }
      else {
        align = Qt::Alignment();

        if      (pp.x < 0.0) align |= Qt::AlignRight;
        else if (pp.x > 0.0) align |= Qt::AlignLeft;

        if      (pp.y < 0.0) align |= Qt::AlignTop;
        else if (pp.y < 0.0) align |= Qt::AlignBottom;
      }
    }
  }

  p = plot()->pixelToWindow(pp);
}

void
CQChartsDendrogramNodeObj::
drawEdges(PaintDevice *device) const
{
  for (const auto &child : children_) {
    drawEdge(device, child.node, child.value);
  }
}

void
CQChartsDendrogramNodeObj::
drawEdge(PaintDevice *device, const NodeObj *child, const OptReal &value) const
{
  auto rect1 = displayRect();
  auto rect2 = child->displayRect();

  auto pbbox1 = plot()->windowToPixel(rect1); // from
  auto pbbox2 = plot()->windowToPixel(rect2); // to

  //---

  bool edgeFilled = value.isSet();

  // set pen and brush
  ColorInd colorInd;

  PenBrush lPenBrush;

  auto strokeColor = plot()->interpEdgeStrokeColor(colorInd);

  if (edgeFilled) {
    auto fillColor = plot()->interpEdgeFillColor(colorInd);

    plot()->setPenBrush(lPenBrush,
      plot()->edgePenData(strokeColor), plot()->edgeBrushData(fillColor));
  }
  else {
    plot()->setPenBrush(lPenBrush,
      plot()->edgePenData(strokeColor), BrushData(false));
  }

  //plot()->updateObjPenBrushState(this, penBrush);

  CQChartsDrawUtil::setPenBrush(device, lPenBrush);

  //---

  // draw edge
  double x1, y1, x4, y4;

  if (plot()->placeType() != Plot::PlaceType::CIRCULAR) {
    if (plot()->orientation() == Qt::Horizontal) {
      x1 = pbbox1.getXMax(); y1 = pbbox1.getYMid();
      x4 = pbbox2.getXMin(); y4 = pbbox2.getYMid();
    }
    else {
      x1 = pbbox1.getXMid(); y1 = pbbox1.getYMax();
      x4 = pbbox2.getXMid(); y4 = pbbox2.getYMin();
    }
  }
  else {
    x1 = pbbox1.getXMid(); y1 = pbbox1.getYMid();
    x4 = pbbox2.getXMid(); y4 = pbbox2.getYMid();
  }

  auto p1 = plot()->pixelToWindow(Point(x1, y1));
  auto p4 = plot()->pixelToWindow(Point(x4, y4));

  auto lw = plot_->lengthPlotWidth(plot_->edgeWidth());

  if (plot_->isEdgeScaled()) {
    if (value.isSet())
      lw *= value.real()/childTotal();
  }

  if (plot()->placeType() != Plot::PlaceType::CIRCULAR) {
    if (edgeFilled)
      CQChartsDrawUtil::drawEdgePath(device, p1, p4, lw, CQChartsDrawUtil::EdgeType::ARC,
                                     plot()->orientation());
    else
      CQChartsDrawUtil::drawCurvePath(device, p1, p4, CQChartsDrawUtil::EdgeType::ARC,
                                      plot()->orientation());
  }
  else {
    if (edgeFilled)
      CQChartsDrawUtil::drawRoundedLine(device, p1, p4, lw);
    else
      device->drawLine(p1, p4);
  }
}

CQChartsGeom::BBox
CQChartsDendrogramNodeObj::
displayRect() const
{
  auto rect1 = rect();

  auto symbolSize = plot_->pixelToWindowWidth(calcSymbolSize());

  auto f = std::min(symbolSize/rect1.getWidth(), symbolSize/rect1.getHeight());

  rect1 = rect1.centerScaled(f, f);

  return rect1;
}

double
CQChartsDendrogramNodeObj::
calcSymbolSize() const
{
  double symbolSize = plot()->calcSymbolSize();

  if (parent() && plot()->sizeColumn().isValid()) {
    auto maxSize = parent()->hierSize();

    double size = 0.0;

    bool is_hier = this->isHier();

    if (is_hier)
      size = hierSize();
    else
      size = this->size().real();

    symbolSize = CMathUtil::mapSqr(size, 0.0, maxSize, 0.0, symbolSize);
  }

  return symbolSize;
}

//---

bool
CQChartsDendrogramPlot::
executeSlotFn(const QString &name, const QVariantList &args, QVariant &res)
{
  if      (name == "expand")
    expandSlot();
  else if (name == "expand_all")
    expandAllSlot();
  else if (name == "collapse_all")
    collapseAllSlot();
  else
    return CQChartsPlot::executeSlotFn(name, args, res);

  return true;
}

//------

CQChartsDendrogramPlotCustomControls::
CQChartsDendrogramPlotCustomControls(CQCharts *charts) :
 CQChartsPlotCustomControls(charts, "dendrogram")
{
}

void
CQChartsDendrogramPlotCustomControls::
init()
{
  addWidgets();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsDendrogramPlotCustomControls::
addWidgets()
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  //---

  addColumnWidgets(QStringList() << "name" << "value" << "color" << "size", columnsFrame);
}

void
CQChartsDendrogramPlotCustomControls::
connectSlots(bool b)
{
  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsDendrogramPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  plot_ = dynamic_cast<CQChartsDendrogramPlot *>(plot);

  CQChartsPlotCustomControls::setPlot(plot);
}

void
CQChartsDendrogramPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  CQChartsPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}
