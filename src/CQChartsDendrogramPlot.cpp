#include <CQChartsDendrogramPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQCharts.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsHtml.h>

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

  addColumnParameter("name", "name", "nameColumn").
   setStringColumn().setRequired().setPropPath("columns.name").setTip("Name column");
  addColumnParameter("value", "Value", "valueColumn").
   setNumericColumn().setRequired().setPropPath("columns.value").setTip("Value column");

  endParameterGroup();

  //---

  // options
  addEnumParameter("orientation", "Orientation", "orientation").
    addNameValue("HORIZONTAL", int(Qt::Horizontal)).
    addNameValue("VERTICAL"  , int(Qt::Vertical  )).
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
     p(IMG("images/dendogram.png"));
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
 CQChartsObjNodeShapeData<CQChartsDendrogramPlot>(this),
 CQChartsObjEdgeLineData <CQChartsDendrogramPlot>(this),
 CQChartsObjTextData     <CQChartsDendrogramPlot>(this)
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
  addProp("columns", "nameColumn" , "name" , "Name column");
  addProp("columns", "valueColumn", "value", "Value column");

  // node
  addProp("node", "circleSize", "circleSize", "Circle size in pixels")->setMinValue(1.0);

  addFillProperties("node/fill"  , "nodeFill"  , "Node");
  addLineProperties("node/stroke", "nodeStroke", "Node");
  addLineProperties("edge/stroke", "edgeLines" , "Edge");

  // label
//addProp("label/text", "textVisible", "visible", "Labels visible");

  addTextProperties("label/text", "text", "", CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  addProp("label", "textMargin", "margin", "Text margin in pixels")->setMinValue(1.0);

  addProp("options", "placeType"  , "placeType"  , "Place type");
  addProp("options", "orientation", "orientation", "Draw orientation");
}

//---

void
CQChartsDendrogramPlot::
setNameColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsDendrogramPlot::
setValueColumn(const Column &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

//---

CQChartsColumn
CQChartsDendrogramPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "name" ) c = this->nameColumn();
  else if (name == "value") c = this->valueColumn();
  else                      c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsDendrogramPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "name" ) this->setNameColumn(c);
  else if (name == "value") this->setValueColumn(c);
  else                      CQChartsPlot::setNamedColumn(name, c);
}

//---

void
CQChartsDendrogramPlot::
setCircleSize(double r)
{
  CQChartsUtil::testAndSet(circleSize_, r, [&]() { drawObjs(); } );
}

void
CQChartsDendrogramPlot::
setTextMargin(double r)
{
  CQChartsUtil::testAndSet(textMargin_, r, [&]() { drawObjs(); } );
}

void
CQChartsDendrogramPlot::
setOrientation(const Qt::Orientation &orient)
{
  CQChartsUtil::testAndSet(orientation_, orient, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDendrogramPlot::
setPlaceType(const PlaceType &t)
{
  CQChartsUtil::testAndSet(placeType_, t, [&]() { updateRangeAndObjs(); } );
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
  if (! checkNumericColumn(valueColumn(), "Value")) columnsValid = false;

  if (! columnsValid)
    return Range(0.0, 0.0, 1.0, 1.0);

  //---

  delete th->dendrogram_;

  th->dendrogram_ = new CQChartsDendrogram;

  //---

  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsDendrogramPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *model, const VisitData &data) override {
      auto path = CQChartsModelUtil::parentPath(model, data.parent);

      //---

      // get name
      ModelIndex nameModelInd(plot_, data.row, plot_->nameColumn (), data.parent);

    //auto nameInd  = modelIndex(nameModelInd);
    //auto nameInd1 = normalizeIndex(nameInd);

      bool ok1;
      auto name = plot_->modelString(nameModelInd, ok1);

      if (ok1 && path.length())
        name = path + "/" + name;

      //--

      // get value
      ModelIndex valueModelInd(plot_, data.row, plot_->valueColumn(), data.parent);

      bool ok2;
      double value = plot_->modelReal(valueModelInd, ok2);
      if (! ok2) return addDataError(valueModelInd, "Invalid Value");

      if (CMathUtil::isNaN(value))
        return State::SKIP;

      //---

      plot_->addNameValue(name, value);

      return State::OK;
    }

    const Range &range() const { return range_; }

   private:
    State addDataError(const ModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsDendrogramPlot *>(plot_)->addDataError(ind , msg);
      return State::SKIP;
    }

   private:
    const CQChartsDendrogramPlot* plot_ { nullptr };
    Range                         range_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  auto dataRange = visitor.range();

  //---

  auto *root = dendrogram_->root();

  if (root)
    root->setOpen(true);

  place();

  //---

  dataRange.updateRange(0, 0);
  dataRange.updateRange(1, 1);

  //---

  return dataRange;
}

void
CQChartsDendrogramPlot::
place() const
{
  if (placeType() == PlaceType::BUCHHEIM) {
    delete buchheimTree_;
    delete buchheimDrawTree_;

    buchheimTree_ = new Tree(dendrogram_->root());

    addBuchheimHierNode(buchheimTree_, dendrogram_->root());

    buchheimDrawTree_ = new CBuchHeim::DrawTree(buchheimTree_);

    buchheimDrawTree_->place();

    buchheimDrawTree_->normalize(/*equalScale*/false);

    //---

    dendrogram_->root()->resetPlaced();

    moveBuchheimHierNode(buchheimDrawTree_);
  }
  else {
    dendrogram_->placeNodes();
  }
}

void
CQChartsDendrogramPlot::
addBuchheimHierNode(CBuchHeim::Tree *tree, HierNode *hierNode) const
{
  if (! hierNode->isOpen())
    return;

  for (auto &hierNode : hierNode->getChildren()) {
    auto *childTree = new Tree(hierNode);

    tree->addChild(CBuchHeim::TreeP(childTree));

    addBuchheimHierNode(childTree, hierNode);
  }

  for (auto &node : hierNode->getNodes()) {
    auto *childTree = new Tree(node);

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

  if (orientation() == Qt::Horizontal)
    node1->setBBox(BBox(y1, x1, y2, x2));
  else
    node1->setBBox(BBox(x1, 1.0 - y1, x2, 1.0 - y2));

  node1->setPlaced(true);

  for (const auto &node : drawTree->children())
    moveBuchheimHierNode(node.get());
}

void
CQChartsDendrogramPlot::
addNameValue(const QString &name, double value) const
{
  QStringList names;

  auto name1 = name;

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

  // create nodes
  HierNode *hierNode = nullptr;

  for (const auto &n : names) {
    if (! hierNode) {
      if (! dendrogram_->root()) {
        hierNode = dendrogram_->createRootNode(n);

        hierNode->setOpen(false);
      }
      else
        hierNode = dendrogram_->root();
    }
    else {
      auto *hierNode1 = hierNode->findChild(n);

      if (! hierNode1) {
        hierNode = dendrogram_->createHierNode(hierNode, n);

        hierNode->setOpen(false);
      }
      else
        hierNode = hierNode1;
    }
  }

  if (! hierNode) {
    hierNode = dendrogram_->createRootNode(name1);

    hierNode->setOpen(false);
  }
  else {
    auto *node = dendrogram_->createNode(hierNode, name1, value);

    assert(node);

  //node->setInd(nameInd1);
  }
}

CQChartsGeom::BBox
CQChartsDendrogramPlot::
calcExtraFitBBox() const
{
  CQPerfTrace trace("CQChartsDendrogramPlot::calcExtraFitBBox");

  BBox bbox;

  for (const auto &plotObj : plotObjs_) {
    auto *nodeObj = dynamic_cast<NodeObj *>(plotObj);

    if (nodeObj)
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

  auto *root = dendrogram_->root();

  if (root) {
    addNodeObj(root, objs);

    addNodeObjs(root, 0, objs);
  }

  return true;
}

void
CQChartsDendrogramPlot::
addNodeObjs(HierNode *hier, int depth, PlotObjs &objs) const
{
  for (auto &hierNode : hier->getChildren()) {
    addNodeObj(hierNode, objs);

    addNodeObjs(hierNode, depth + 1, objs);
  }

  //------

  for (auto &node : hier->getNodes())
    addNodeObj(node, objs);
}

void
CQChartsDendrogramPlot::
addNodeObj(Node *node, PlotObjs &objs) const
{
  if (! node->isPlaced()) return;

  BBox rect = getBBox(node);

  auto *obj = createNodeObj(node, rect);

  objs.push_back(obj);
}

CQChartsGeom::BBox
CQChartsDendrogramPlot::
getBBox(Node *node) const
{
  if (placeType() == PlaceType::BUCHHEIM)
    return node->bbox();

  double cs = std::max(circleSize(), 1.0);
//double tm = std::max(textMargin(), 1.0);

  double cw = pixelToWindowWidth (cs);
  double ch = pixelToWindowHeight(cs);

//double mw = pixelToWindowWidth(tm);

//double xc = node->x() + mw;
  double xc = node->x();
  double yc = node->yc();

  return BBox(xc - cw/2.0, yc - ch/2.0, xc + cw/2.0, yc + ch/2.0);
}

//------

bool
CQChartsDendrogramPlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsDendrogramPlot::
execDrawForeground(PaintDevice *device) const
{
  auto *root = dendrogram_->root();

  if (root) {
    drawNode(device, 0, root);

    drawNodes(device, root, 0);
  }
}

void
CQChartsDendrogramPlot::
drawNodes(PaintDevice *device, HierNode *hier, int depth) const
{
  for (auto &hierNode : hier->getChildren()) {
    drawNode(device, hier, hierNode);

    drawNodes(device, hierNode, depth + 1);
  }

  //------

  for (auto &node : hier->getNodes())
    drawNode(device, hier, node);
}

void
CQChartsDendrogramPlot::
drawNode(PaintDevice *device, HierNode *hier, Node *node) const
{
  if (! node->isPlaced()) return;

  if (! hier) return;

  auto pbbox1 = windowToPixel(getBBox(node));
  auto pbbox2 = windowToPixel(getBBox(hier));

  //---

  PenBrush lPenBrush;

  setEdgeLineDataPen(lPenBrush.pen, ColorInd(0, 1));

  setBrush(lPenBrush, BrushData(false));

  CQChartsDrawUtil::setPenBrush(device, lPenBrush);

  //---

  // draw edge
  QPainterPath path;

  double x1, y1, x4, y4;

  if (orientation() == Qt::Horizontal) {
    x1 = pbbox2.getXMax(); y1 = pbbox2.getYMid();
    x4 = pbbox1.getXMin(); y4 = pbbox1.getYMid();
  }
  else {
    x1 = pbbox2.getXMid(); y1 = pbbox2.getYMax();
    x4 = pbbox1.getXMid(); y4 = pbbox1.getYMin();
  }

  auto p1 = pixelToWindow(Point(x1, y1));
  auto p4 = pixelToWindow(Point(x4, y4));

  CQChartsDrawUtil::curvePath(path, p1, p4);

  device->drawPath(path);
}

bool
CQChartsDendrogramPlot::
handleSelectPress(const Point &p, SelMod /*selMod*/)
{
  auto bbox = getBBox(dendrogram_->root());

  double r = bbox.getWidth()/2.0;

  auto *node = dendrogram_->getNodeAtPoint(p.x, p.y, r);
  if (! node) return false;

  node->setOpen(! node->isOpen());

  place();

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
 CQChartsPlotObj(const_cast<Plot *>(plot), rect), plot_(plot), node_(node)
{
}

QString
CQChartsDendrogramNodeObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(node_->name());
}

CQChartsGeom::BBox
CQChartsDendrogramNodeObj::
textRect() const
{
  auto pbbox = plot_->windowToPixel(rect());

  auto pc = pbbox.getCenter();
  auto pr = pbbox.getWidth()/2.0;

  //---

  auto font = plot_->view()->plotFont(plot_, plot_->textFont());

  QFontMetricsF fm(font);

  const auto &name = node_->name();

  bool is_hier = dynamic_cast<HierNode *>(node_);

  double dy = (fm.ascent() - fm.descent())/2;

  Point p;

  if (is_hier)
    p = Point(pc.x - pr - fm.width(name), pc.y + dy);
  else
    p = Point(pc.x + pr, pc.y + dy);

  Point p1(p.x                 , p.y - fm.ascent());
  Point p2(p.x + fm.width(name), p.y + fm.ascent());

  BBox ptbbox(p1, p2);

  auto tbbox = plot_->pixelToWindow(ptbbox);

  return tbbox;
}

void
CQChartsDendrogramNodeObj::
draw(PaintDevice *device) const
{
  if (! node_->isPlaced()) return;

  double tm = std::max(plot_->textMargin(), 1.0);

  BBox pbbox = plot_->windowToPixel(rect());

  //---

  // set pen and brush
  PenBrush penBrush;

  auto strokeColor = plot_->interpNodeStrokeColor(ColorInd());
  auto fillColor   = plot_->interpNodeFillColor  (ColorInd());

  bool filled = (node_->hasChildren() && ! node_->isOpen());

  plot_->setPenBrush(penBrush, plot_->nodePenData(strokeColor),
    filled ? plot_->nodeBrushData(fillColor) : BrushData(false));

  plot_->updateObjPenBrushState(this, penBrush);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw node
  device->drawEllipse(rect());

  //---

  // draw node text
  PenBrush tpenBrush;

  auto tc = plot_->interpTextColor(ColorInd());

  plot_->setPen(tpenBrush, PenData(/*stroked*/true, tc, plot_->textAlpha()));

  device->setPen(tpenBrush.pen);

  //---

  plot_->setPainterFont(device, plot_->textFont());

  //---

  const auto &name = node_->name();

  bool is_hier = dynamic_cast<HierNode *>(node_);

  QFontMetricsF fm(device->font());

  double dy = (fm.ascent() - fm.descent())/2;

  Point p;

  if (is_hier)
    p = Point(pbbox.getXMid() - pbbox.getWidth()/2.0 - fm.width(name) - tm,
              pbbox.getYMid() + dy); // align right
  else
    p = Point(pbbox.getXMid() + pbbox.getWidth()/2.0 + tm,
              pbbox.getYMid() + dy); // align left

  // only support contrast
  auto textOptions = plot_->textOptions(device);

  textOptions.angle     = Angle();
  textOptions.align     = Qt::AlignLeft;
  textOptions.formatted = false;
  textOptions.scaled    = false;
  textOptions.html      = false;

  CQChartsDrawUtil::drawTextAtPoint(device, plot_->pixelToWindow(p), name, textOptions);
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

  addColumnWidgets(QStringList() << "name" << "value", columnsFrame);
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
