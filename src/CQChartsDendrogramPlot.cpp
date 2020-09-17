#include <CQChartsDendrogramPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQCharts.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

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
   setString().setRequired().setTip("Name column");

  addColumnParameter("value", "Value", "valueColumn").
   setNumeric().setRequired().setTip("Value column");

  endParameterGroup();

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
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

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
}

//---

void
CQChartsDendrogramPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDendrogramPlot::
setValueColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
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

  if (! checkColumn(nameColumn (), "Name" )) columnsValid = false;
  if (! checkColumn(valueColumn(), "Value")) columnsValid = false;

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
      QString path = CQChartsModelUtil::parentPath(model, data.parent);

      //---

      auto *plot = const_cast<CQChartsDendrogramPlot *>(plot_);

      ModelIndex nameModelInd (plot, data.row, plot_->nameColumn (), data.parent);
      ModelIndex valueModelInd(plot, data.row, plot_->valueColumn(), data.parent);

    //QModelIndex nameInd  = modelIndex(nameModelInd);
    //QModelIndex nameInd1 = normalizeIndex(nameInd);

      //---

      bool ok1;

      QString name = plot_->modelString(nameModelInd, ok1);

      if (path.length())
        name = path + "/" + name;

      //--

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

  dendrogram_->placeNodes();

  //---

  dataRange.updateRange(0, 0);
  dataRange.updateRange(1, 1);

  //---

  return dataRange;
}

void
CQChartsDendrogramPlot::
addNameValue(const QString &name, double value) const
{
  QStringList names;

  QString name1 = name;

  int pos = name1.indexOf('/');

  if (pos != -1) {
    while (pos != -1) {
      QString lhs = name1.mid(0, pos);
      QString rhs = name1.mid(pos + 1);

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
  CQChartsDendrogram::HierNode *hierNode = nullptr;

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
calcAnnotationBBox() const
{
  CQPerfTrace trace("CQChartsDendrogramPlot::calcAnnotationBBox");

  BBox bbox;

  for (const auto &plotObj : plotObjs_) {
    auto *nodeObj = dynamic_cast<CQChartsDendrogramNodeObj *>(plotObj);

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
addNodeObjs(CQChartsDendrogram::HierNode *hier, int depth, PlotObjs &objs) const
{
  const auto &children = hier->getChildren();

  for (auto &hierNode : children) {
    addNodeObj(hierNode, objs);

    addNodeObjs(hierNode, depth + 1, objs);
  }

  //------

  const auto &nodes = hier->getNodes();

  for (auto &node : nodes)
    addNodeObj(node, objs);
}

void
CQChartsDendrogramPlot::
addNodeObj(CQChartsDendrogram::Node *node, PlotObjs &objs) const
{
  if (! node->isPlaced()) return;

  double cs = std::max(circleSize(), 1.0);
//double tm = std::max(textMargin(), 1.0);

  double cw = pixelToWindowWidth (cs);
  double ch = pixelToWindowHeight(cs);

//double mw = pixelToWindowWidth(tm);

//double xc = node->x() + mw;
  double xc = node->x();
  double yc = node->yc();

  BBox rect(xc - cw/2.0, yc - ch/2.0, xc + cw/2.0, yc + ch/2.0);

  auto *obj = createNodeObj(node, rect);

  objs.push_back(obj);
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
execDrawForeground(CQChartsPaintDevice *device) const
{
  auto *root = dendrogram_->root();

  if (root) {
    drawNode(device, 0, root);

    drawNodes(device, root, 0);
  }
}

void
CQChartsDendrogramPlot::
drawNodes(CQChartsPaintDevice *device, CQChartsDendrogram::HierNode *hier, int depth) const
{
  const auto &children = hier->getChildren();

  for (auto &hierNode : children) {
    drawNode(device, hier, hierNode);

    drawNodes(device, hierNode, depth + 1);
  }

  //------

  const auto &nodes = hier->getNodes();

  for (auto &node : nodes)
    drawNode(device, hier, node);
}

void
CQChartsDendrogramPlot::
drawNode(CQChartsPaintDevice *device, CQChartsDendrogram::HierNode *hier,
         CQChartsDendrogram::Node *node) const
{
  if (! node->isPlaced()) return;

  auto pn = windowToPixel(Point(node->x(), node->yc()));

  double cs = std::max(circleSize(), 1.0);
//double tm = std::max(textMargin(), 1.0);

  //---

  // draw edge
  if (hier) {
    auto ph = windowToPixel(Point(hier->x(), hier->yc()));

//  double x1 = ph.x + tm + cs/2.0; double y1 = ph.y;
    double x1 = ph.x + cs/2.0;      double y1 = ph.y;
    double x4 = pn.x - cs/2.0;      double y4 = pn.y;
    double x2 = x1 + (x4 - x1)/3.0; double y2 = y1;
    double x3 = x2 + (x4 - x1)/3.0; double y3 = y4;

    //---

    PenBrush lPenBrush;

    setEdgeLineDataPen(lPenBrush.pen, ColorInd(0, 1));

    setBrush(lPenBrush, BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, lPenBrush);

    //---

    QPainterPath path;

    auto p1 = pixelToWindow(Point(x1, y1));
    auto p2 = pixelToWindow(Point(x2, y2));
    auto p3 = pixelToWindow(Point(x3, y3));
    auto p4 = pixelToWindow(Point(x4, y4));

    path.moveTo (p1.qpoint());
    path.cubicTo(p2.qpoint(), p3.qpoint(), p4.qpoint());

    device->drawPath(path);
  }
}

bool
CQChartsDendrogramPlot::
selectPress(const Point &p, SelMod /*selMod*/)
{
  double cs = circleSize();

  double ps = pixelToWindowWidth(cs);

  auto *node = dendrogram_->getNodeAtPoint(p.x, p.y, ps);

  if (! node)
    return false;

  node->setOpen(! node->isOpen());

  dendrogram_->placeNodes();

  updateObjs();

  return true;
}

CQChartsDendrogramNodeObj *
CQChartsDendrogramPlot::
createNodeObj(CQChartsDendrogram::Node *node, const BBox &rect) const
{
  return new CQChartsDendrogramNodeObj(this, node, rect);
}

//------

CQChartsDendrogramNodeObj::
CQChartsDendrogramNodeObj(const CQChartsDendrogramPlot *plot, CQChartsDendrogram::Node *node,
                          const BBox &rect) :
 CQChartsPlotObj(const_cast<CQChartsDendrogramPlot *>(plot), rect), plot_(plot), node_(node)
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
  auto pn = plot_->windowToPixel(Point(node_->x(), node_->yc()));

  double cs = plot_->circleSize();

  //---

  QFont font = plot_->view()->plotFont(plot_, plot_->textFont());

  QFontMetricsF fm(font);

  const auto &name = node_->name();

  bool is_hier = dynamic_cast<CQChartsDendrogram::HierNode *>(node_);

  double dy = (fm.ascent() - fm.descent())/2;

  Point p;

  if (is_hier)
    p = Point(pn.x - cs - fm.width(name), pn.y + dy);
  else
    p = Point(pn.x + cs, pn.y + dy);

  Point p1(p.x                 , p.y - fm.ascent());
  Point p2(p.x + fm.width(name), p.y + fm.ascent());

  BBox pbbox(p1, p2);

  auto wbbox = plot_->pixelToWindow(pbbox);

  return wbbox;
}

void
CQChartsDendrogramNodeObj::
draw(CQChartsPaintDevice *device)
{
  if (! node_->isPlaced()) return;

  auto p1 = plot_->windowToPixel(Point(node_->x(), node_->yc()));

  double cs = std::max(plot_->circleSize(), 1.0);
  double tm = std::max(plot_->textMargin(), 1.0);

//p1.x += tm;

  BBox bbox(p1.x - cs/2.0, p1.y - cs/2.0, p1.x + cs/2.0, p1.y + cs/2.0);

  //---

  // set pen and brush
  PenBrush penBrush;

  QColor strokeColor = plot_->interpNodeStrokeColor(ColorInd());
  QColor fillColor   = plot_->interpNodeFillColor  (ColorInd());

  bool filled = (node_->hasChildren() && ! node_->isOpen());

  plot_->setPenBrush(penBrush,
    PenData  (plot_->isNodeStroked(), strokeColor, plot_->nodeStrokeAlpha(),
              plot_->plotStrokeWidth(), plot_->nodeStrokeDash()),
    BrushData(filled, fillColor, plot_->nodeFillAlpha(), plot_->nodeFillPattern()));

  plot_->updateObjPenBrushState(this, penBrush);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw node
  device->drawEllipse(plot_->pixelToWindow(bbox));

  //---

  // draw node text
  PenBrush tpenBrush;

  QColor tc = plot_->interpTextColor(ColorInd());

  plot_->setPen(tpenBrush, PenData(/*stroked*/true, tc, plot_->textAlpha()));

  device->setPen(tpenBrush.pen);

  //---

  plot_->view()->setPlotPainterFont(plot_, device, plot_->textFont());

  //---

  const auto &name = node_->name();

  bool is_hier = dynamic_cast<CQChartsDendrogram::HierNode *>(node_);

  QFontMetricsF fm(device->font());

  double dy = (fm.ascent() - fm.descent())/2;

  Point p;

  if (is_hier)
    p = Point(p1.x - cs - fm.width(name) - tm, p1.y + dy); // align right
  else
    p = Point(p1.x + cs + tm, p1.y + dy); // align left

  // only support contrast
  CQChartsTextOptions options;

  options.angle         = CQChartsAngle(0);
  options.align         = Qt::AlignLeft;
  options.contrast      = plot_->isTextContrast();
  options.contrastAlpha = plot_->textContrastAlpha();
  options.clipLength    = plot_->textClipLength();
  options.clipElide     = plot_->textClipElide();

  CQChartsDrawUtil::drawTextAtPoint(device, plot_->pixelToWindow(p), name, options);
}
