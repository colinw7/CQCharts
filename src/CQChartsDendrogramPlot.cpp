#include <CQChartsDendrogramPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQCharts.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QPainter>

CQChartsDendrogramPlotType::
CQChartsDendrogramPlotType()
{
}

void
CQChartsDendrogramPlotType::
addParameters()
{
  startParameterGroup("Dendogram");

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
  return "<h2>Summary</h2>\n"
         "<p>Draw hierarchical data using collapsible tree.</p>\n";
}

CQChartsPlot *
CQChartsDendrogramPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsDendrogramPlot(view, model);
}

//------

CQChartsDendrogramPlot::
CQChartsDendrogramPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("dendrogram"), model),
 CQChartsObjNodeShapeData<CQChartsDendrogramPlot>(this),
 CQChartsObjEdgeLineData <CQChartsDendrogramPlot>(this),
 CQChartsObjTextData     <CQChartsDendrogramPlot>(this)
{
  NoUpdate noUpdate(this);

  setNodeFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  //---

  addTitle();
}

CQChartsDendrogramPlot::
~CQChartsDendrogramPlot()
{
  delete dendrogram_;
}

//---

void
CQChartsDendrogramPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nameColumn" , "name" )->setDesc("Name column");
  addProperty("columns", this, "valueColumn", "value")->setDesc("Value column");

  addProperty("options", this, "textMargin", "textMargin")->setDesc("Text margin");

  addProperty("node", this, "circleSize", "circleSize")->setDesc("Circle size");

  addLineProperties("node/stroke", "nodeBorder");
  addFillProperties("node/fill"  , "nodeFill"  );
  addLineProperties("edge/stroke", "edgeLines" );

  addTextProperties("text", "text");
}

//---

void
CQChartsDendrogramPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDendrogramPlot::
setValueColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsDendrogramPlot::
setCircleSize(double r)
{
  CQChartsUtil::testAndSet(circleSize_, r, [&]() { queueDrawObjs(); } );
}

void
CQChartsDendrogramPlot::
setTextMargin(double r)
{
  CQChartsUtil::testAndSet(textMargin_, r, [&]() { queueDrawObjs(); } );
}

//---

CQChartsGeom::Range
CQChartsDendrogramPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsDendrogramPlot::calcRange");

  CQChartsDendrogramPlot *th = const_cast<CQChartsDendrogramPlot *>(this);

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

      //QModelIndex nameInd  = modelIndex(data.row, plot_->nameColumn(), data.parent);
      //QModelIndex nameInd1 = normalizeIndex(nameInd);

      //---

      bool ok1;

      QString name = plot_->modelString(data.row, plot_->nameColumn(), data.parent, ok1);

      if (path.length())
        name = path + "/" + name;

      //--

      bool ok2;

      double value = plot_->modelReal(data.row, plot_->valueColumn(), data.parent, ok2);

      if (CMathUtil::isNaN(value))
        return State::SKIP;

      //---

      plot_->addNameValue(name, value);

      return State::OK;
    }

    const CQChartsGeom::Range &range() const { return range_; }

   private:
    const CQChartsDendrogramPlot* plot_ { nullptr };
    CQChartsGeom::Range           range_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  CQChartsGeom::Range dataRange = visitor.range();

  //---

  CQChartsDendrogram::HierNode *root = dendrogram_->root();

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
  CQChartsDendrogram::HierNode *hierNode = 0;

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
      CQChartsDendrogram::HierNode *hierNode1 = hierNode->findChild(n);

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
    CQChartsDendrogram::Node *node = dendrogram_->createNode(hierNode, name1, value);

    assert(node);

  //node->setInd(nameInd1);
  }
}

CQChartsGeom::BBox
CQChartsDendrogramPlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  for (const auto &plotObj : plotObjs_) {
    CQChartsDendrogramNodeObj *nodeObj = dynamic_cast<CQChartsDendrogramNodeObj *>(plotObj);

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

  NoUpdate noUpdate(const_cast<CQChartsDendrogramPlot *>(this));

  //---

  CQChartsDendrogram::HierNode *root = dendrogram_->root();

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
  const CQChartsDendrogram::HierNode::Children &children = hier->getChildren();

  for (auto &hierNode : children) {
    addNodeObj(hierNode, objs);

    addNodeObjs(hierNode, depth + 1, objs);
  }

  //------

  const CQChartsDendrogram::Nodes &nodes = hier->getNodes();

  for (auto &node : nodes)
    addNodeObj(node, objs);
}

void
CQChartsDendrogramPlot::
addNodeObj(CQChartsDendrogram::Node *node, PlotObjs &objs) const
{
  if (! node->isPlaced()) return;

  double cs = circleSize();
  double tm = textMargin();

  double cw = pixelToWindowWidth (cs);
  double ch = pixelToWindowHeight(cs);

  double mw = pixelToWindowWidth(tm);

  double xc = node->x() + mw;
  double yc = node->yc();

  CQChartsGeom::BBox rect(xc - cw/2.0, yc - ch/2.0, xc + cw/2.0, yc + ch/2.0);

  CQChartsDendrogramNodeObj *obj = new CQChartsDendrogramNodeObj(this, node, rect);

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
drawForeground(QPainter *painter) const
{
  CQChartsDendrogram::HierNode *root = dendrogram_->root();

  if (root) {
    drawNode(painter, 0, root);

    drawNodes(painter, root, 0);
  }
}

void
CQChartsDendrogramPlot::
drawNodes(QPainter *painter, CQChartsDendrogram::HierNode *hier, int depth) const
{
  const CQChartsDendrogram::HierNode::Children &children = hier->getChildren();

  for (auto &hierNode : children) {
    drawNode(painter, hier, hierNode);

    drawNodes(painter, hierNode, depth + 1);
  }

  //------

  const CQChartsDendrogram::Nodes &nodes = hier->getNodes();

  for (auto &node : nodes)
    drawNode(painter, hier, node);
}

void
CQChartsDendrogramPlot::
drawNode(QPainter *painter, CQChartsDendrogram::HierNode *hier,
         CQChartsDendrogram::Node *node) const
{
  if (! node->isPlaced()) return;

  CQChartsGeom::Point p1 = windowToPixel(CQChartsGeom::Point(node->x(), node->yc()));

  double cs = circleSize();
  double tm = textMargin();

  //---

  // draw edge
  if (hier) {
    CQChartsGeom::Point p2 = windowToPixel(CQChartsGeom::Point(hier->x(), hier->yc()));

    double x1 = p2.x + tm + cs/2.0; double y1 = p2.y;
    double x4 = p1.x - cs/2.0     ; double y4 = p1.y;
    double x2 = x1 + (x4 - x1)/3.0; double y2 = y1;
    double x3 = x2 + (x4 - x1)/3.0; double y3 = y4;

    //---

    QPen   lpen;
    QBrush lbrush;

    setEdgeLineDataPen(lpen, 0, 1);

    setBrush(lbrush, false);

    painter->setPen  (lpen);
    painter->setBrush(lbrush);

    //---

    QPainterPath path;

    path.moveTo (QPointF(x1, y1));
    path.cubicTo(QPointF(x2, y2), QPointF(x3, y3), QPointF(x4, y4));

    painter->drawPath(path);
  }
}

bool
CQChartsDendrogramPlot::
selectPress(const CQChartsGeom::Point &p, SelMod /*selMod*/)
{
  double cs = circleSize();

  double ps = pixelToWindowWidth(cs);

  CQChartsDendrogram::Node *node = dendrogram_->getNodeAtPoint(p.x, p.y, ps);

  if (! node)
    return false;

  node->setOpen(! node->isOpen());

  dendrogram_->placeNodes();

  queueUpdateObjs();

  return true;
}

//------

CQChartsDendrogramNodeObj::
CQChartsDendrogramNodeObj(const CQChartsDendrogramPlot *plot, CQChartsDendrogram::Node *node,
                          const CQChartsGeom::BBox &rect) :
 CQChartsPlotObj(const_cast<CQChartsDendrogramPlot *>(plot), rect), plot_(plot), node_(node)
{
}

QString
CQChartsDendrogramNodeObj::
calcId() const
{
  return QString("node:%1").arg(node_->name());
}

CQChartsGeom::BBox
CQChartsDendrogramNodeObj::
textRect() const
{
  CQChartsGeom::Point pn = plot_->windowToPixel(CQChartsGeom::Point(node_->x(), node_->yc()));

  double cs = plot_->circleSize();

  //---

  QFont font = plot_->view()->plotFont(plot_, plot_->textFont());

  QFontMetricsF fm(font);

  const QString &name = node_->name();

  bool is_hier = dynamic_cast<CQChartsDendrogram::HierNode *>(node_);

  CQChartsGeom::BBox bbox;

  CQChartsGeom::Point p;

  double dy = (fm.ascent() - fm.descent())/2;

  if (is_hier)
    p = CQChartsGeom::Point(pn.x - cs - fm.width(name), pn.y + dy);
  else
    p = CQChartsGeom::Point(pn.x + cs, pn.y + dy);

  CQChartsGeom::Point p1(p.x                 , p.y - fm.ascent());
  CQChartsGeom::Point p2(p.x + fm.width(name), p.y + fm.ascent());

  CQChartsGeom::BBox pbbox(p1, p2);

  CQChartsGeom::BBox wbbox = plot_->pixelToWindow(pbbox);

  return wbbox;
}

void
CQChartsDendrogramNodeObj::
getSelectIndices(Indices &) const
{
}

void
CQChartsDendrogramNodeObj::
draw(QPainter *painter)
{
  if (! node_->isPlaced()) return;

  CQChartsGeom::Point p1 = plot_->windowToPixel(CQChartsGeom::Point(node_->x(), node_->yc()));

  double cs = plot_->circleSize();
  double tm = plot_->textMargin();

  p1.x += tm;

  QRectF qrect(p1.x - cs/2.0, p1.y - cs/2.0, cs, cs);

  //---

  // set pen and brush
  QPen   pen;
  QBrush brush;

  QColor borderColor = plot_->interpNodeBorderColor(0, 1);
  QColor fillColor   = plot_->interpNodeFillColor  (0, 1);

  plot_->setPen(pen, plot_->isNodeBorder(), borderColor, plot_->nodeBorderAlpha(),
                plot_->plotBorderWidth(), plot_->nodeBorderDash());

  bool filled = (node_->hasChildren() && ! node_->isOpen());

  plot_->setBrush(brush, filled, fillColor, plot_->nodeFillAlpha(), plot_->nodeFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  // draw node
  painter->drawEllipse(qrect);

  //---

  // draw node text
  QPen tpen;

  QColor tc = plot_->interpTextColor(0, 1);

  plot_->setPen(tpen, /*stroked*/true, tc, plot_->textAlpha());

  painter->setPen(tpen);

  //---

  plot_->view()->setPlotPainterFont(plot_, painter, plot_->textFont());

  //---

  const QString &name = node_->name();

  bool is_hier = dynamic_cast<CQChartsDendrogram::HierNode *>(node_);

  QFontMetricsF fm(painter->font());

  double dy = (fm.ascent() - fm.descent())/2;

  QPointF p;

  if (is_hier)
    p = QPointF(p1.x - cs - fm.width(name), p1.y + dy);
  else
    p = QPointF(p1.x + cs, p1.y + dy);

  CQChartsDrawUtil::drawSimpleText(painter, p, name);
}
