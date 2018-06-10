#include <CQChartsDendrogramPlot.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <QPainter>

CQChartsDendrogramPlotType::
CQChartsDendrogramPlotType()
{
}

void
CQChartsDendrogramPlotType::
addParameters()
{
  addColumnParameter("name" , "name" , "nameColumn" , 0).setRequired();
  addColumnParameter("value", "Value", "valueColumn", 1).setRequired();

  CQChartsPlotType::addParameters();
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
 CQChartsPlot(view, view->charts()->plotType("dendrogram"), model)
{
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

  addProperty("columns", this, "nameColumn" , "name" );
  addProperty("columns", this, "valueColumn", "value");

  addProperty("", this, "textMargin", "textMargin");

  addProperty("node", this, "circleSize", "circleSize");

  addProperty("node/stroke", this, "nodeBorderColor", "color");
  addProperty("node/stroke", this, "nodeBorderAlpha", "alpha");
  addProperty("node/stroke", this, "nodeBorderWidth", "width");

  addProperty("node/fill", this, "nodeFillColor", "color");
  addProperty("node/fill", this, "nodeFillAlpha", "alpha");

  addProperty("edge/stroke", this, "edgeLineColor", "color");
  addProperty("edge/stroke", this, "edgeLineAlpha", "alpha");
  addProperty("edge/stroke", this, "edgeLineWidth", "width");

  addProperty("text", this, "textFont" , "font" );
  addProperty("text", this, "textColor", "color");
  addProperty("text", this, "textAlpha", "alpha");
}

//---

const CQChartsColor &
CQChartsDendrogramPlot::
nodeBorderColor() const
{
  return nodeData_.border.color;
}

void
CQChartsDendrogramPlot::
setNodeBorderColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(nodeData_.border.color, c, [&]() { update(); } );
}

QColor
CQChartsDendrogramPlot::
interpNodeBorderColor(int i, int n) const
{
  return nodeBorderColor().interpColor(this, i, n);
}

double
CQChartsDendrogramPlot::
nodeBorderAlpha() const
{
  return nodeData_.border.alpha;
}

void
CQChartsDendrogramPlot::
setNodeBorderAlpha(double a)
{
  CQChartsUtil::testAndSet(nodeData_.border.alpha, a, [&]() { update(); } );
}

const CQChartsLength &
CQChartsDendrogramPlot::
nodeBorderWidth() const
{
  return nodeData_.border.width;
}

void
CQChartsDendrogramPlot::
setNodeBorderWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(nodeData_.border.width, l, [&]() { update(); } );
}

//---

const CQChartsColor &
CQChartsDendrogramPlot::
nodeFillColor() const
{
  return nodeData_.background.color;
}

void
CQChartsDendrogramPlot::
setNodeFillColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(nodeData_.background.color, c, [&]() { update(); } );
}

QColor
CQChartsDendrogramPlot::
interpNodeFillColor(int i, int n) const
{
  return nodeFillColor().interpColor(this, i, n);
}

double
CQChartsDendrogramPlot::
nodeFillAlpha() const
{
  return nodeData_.background.alpha;
}

void
CQChartsDendrogramPlot::
setNodeFillAlpha(double a)
{
  CQChartsUtil::testAndSet(nodeData_.background.alpha, a, [&]() { update(); } );
}

//---

const CQChartsColor &
CQChartsDendrogramPlot::
edgeLineColor() const
{
  return edgeData_.color;
}

void
CQChartsDendrogramPlot::
setEdgeLineColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(edgeData_.color, c, [&]() { update(); } );
}

QColor
CQChartsDendrogramPlot::
interpEdgeLineColor(int i, int n) const
{
  return edgeLineColor().interpColor(this, i, n);
}

double
CQChartsDendrogramPlot::
edgeLineAlpha() const
{
  return edgeData_.alpha;
}

void
CQChartsDendrogramPlot::
setEdgeLineAlpha(double a)
{
  CQChartsUtil::testAndSet(edgeData_.alpha, a, [&]() { update(); } );
}

const CQChartsLength &
CQChartsDendrogramPlot::
edgeLineWidth() const
{
  return edgeData_.width;
}

void
CQChartsDendrogramPlot::
setEdgeLineWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(edgeData_.width, l, [&]() { update(); } );
}

//---

const QFont &
CQChartsDendrogramPlot::
textFont() const
{
  return textData_.font;
}

void
CQChartsDendrogramPlot::
setTextFont(const QFont &f)
{
  CQChartsUtil::testAndSet(textData_.font, f, [&]() { update(); } );
}

const CQChartsColor &
CQChartsDendrogramPlot::
textColor() const
{
  return textData_.color;
}

void
CQChartsDendrogramPlot::
setTextColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(textData_.color, c, [&]() { update(); } );
}

QColor
CQChartsDendrogramPlot::
interpTextColor(int i, int n) const
{
  return textColor().interpColor(this, i, n);
}

double
CQChartsDendrogramPlot::
textAlpha() const
{
  return textData_.alpha;
}

void
CQChartsDendrogramPlot::
setTextAlpha(double a)
{
  CQChartsUtil::testAndSet(textData_.alpha, a, [&]() { update(); } );
}

//---

void
CQChartsDendrogramPlot::
updateRange(bool apply)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  delete dendrogram_;

  dendrogram_ = new CQChartsDendrogram;

  //---

  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsDendrogramPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      QString path = CQChartsUtil::parentPath(model, parent);

      //---

      //QModelIndex nameInd  = model->index(row, plot_->nameColumn().column(), parent);
      //QModelIndex nameInd1 = normalizeIndex(nameInd);

      //---

      bool ok1;

      QString name = plot_->modelString(model, row, plot_->nameColumn(), parent, ok1);

      if (path.length())
        name = path + "/" + name;

      //--

      bool ok2;

      double value = plot_->modelReal(model, row, plot_->valueColumn(), parent, ok2);

      if (CQChartsUtil::isNaN(value))
        return State::SKIP;

      //---

      plot_->addNameValue(name, value);

      return State::OK;
    }

    const CQChartsGeom::Range &range() const { return range_; }

   private:
    CQChartsDendrogramPlot *plot_ { nullptr };
    CQChartsGeom::Range     range_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  dataRange_ = visitor.range();

  //---

  CQChartsDendrogram::HierNode *root = dendrogram_->root();

  root->setOpen(true);

  dendrogram_->placeNodes();

  //---

  dataRange_.updateRange(0, 0);
  dataRange_.updateRange(1, 1);

  //---

  if (apply)
    applyDataRange();
}

void
CQChartsDendrogramPlot::
addNameValue(const QString &name, double value)
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
initObjs()
{
  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return false;
  }

  //---

  if (! plotObjs_.empty())
    return false;

  //---

  CQChartsDendrogram::HierNode *root = dendrogram_->root();

  if (root) {
    addNodeObj(root);

    addNodeObjs(root, 0);
  }

  return true;
}

void
CQChartsDendrogramPlot::
addNodeObjs(CQChartsDendrogram::HierNode *hier, int depth)
{
  const CQChartsDendrogram::HierNode::Children &children = hier->getChildren();

  for (auto hierNode : children) {
    addNodeObj(hierNode);

    addNodeObjs(hierNode, depth + 1);
  }

  //------

  const CQChartsDendrogram::Nodes &nodes = hier->getNodes();

  for (auto node : nodes)
    addNodeObj(node);
}

void
CQChartsDendrogramPlot::
addNodeObj(CQChartsDendrogram::Node *node)
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

  addPlotObject(obj);
}

//------

void
CQChartsDendrogramPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);

  //---

  CQChartsDendrogram::HierNode *root = dendrogram_->root();

  if (root) {
    drawNode(painter, 0, root);

    drawNodes(painter, root, 0);
  }
}

void
CQChartsDendrogramPlot::
drawNodes(QPainter *painter, CQChartsDendrogram::HierNode *hier, int depth)
{
  const CQChartsDendrogram::HierNode::Children &children = hier->getChildren();

  for (auto hierNode : children) {
    drawNode(painter, hier, hierNode);

    drawNodes(painter, hierNode, depth + 1);
  }

  //------

  const CQChartsDendrogram::Nodes &nodes = hier->getNodes();

  for (auto node : nodes)
    drawNode(painter, hier, node);
}

void
CQChartsDendrogramPlot::
drawNode(QPainter *painter, CQChartsDendrogram::HierNode *hier, CQChartsDendrogram::Node *node)
{
  if (! node->isPlaced()) return;

  double px1, py1;

  windowToPixel(node->x(), node->yc(), px1, py1);

  double cs = circleSize();
  double tm = textMargin();

  //---

  // draw edge
  if (hier) {
    double px2, py2;

    windowToPixel(hier->x(), hier->yc(), px2, py2);

    double x1 = px2 + tm + cs/2.0 ; double y1 = py2;
    double x4 = px1 - cs/2.0      ; double y4 = py1;
    double x2 = x1 + (x4 - x1)/3.0; double y2 = y1;
    double x3 = x2 + (x4 - x1)/3.0; double y3 = y4;

    //---

    QColor edgeColor = interpEdgeLineColor(0, 1);

    edgeColor.setAlphaF(edgeLineAlpha());

    QPen   lpen;
    QBrush lbrush;

    double lw = lengthPixelWidth(edgeLineWidth());

    lpen.setColor (edgeColor);
    lpen.setWidthF(lw);

    painter->setPen(lpen);

    lbrush.setStyle(Qt::NoBrush);

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
selectPress(const CQChartsGeom::Point &p, ModSelect /*modSelect*/)
{
  double cs = circleSize();

  double ps = pixelToWindowWidth(cs);

  CQChartsDendrogram::Node *node = dendrogram_->getNodeAtPoint(p.x, p.y, ps);

  if (! node)
    return false;

  node->setOpen(! node->isOpen());

  dendrogram_->placeNodes();

  updateObjs();

  return true;
}

//------

CQChartsDendrogramNodeObj::
CQChartsDendrogramNodeObj(CQChartsDendrogramPlot *plot, CQChartsDendrogram::Node *node,
                          const CQChartsGeom::BBox &rect) :
 CQChartsPlotObj(plot, rect), plot_(plot), node_(node)
{
}

QString
CQChartsDendrogramNodeObj::
calcId() const
{
  return node_->name();
}

CQChartsGeom::BBox
CQChartsDendrogramNodeObj::
textRect() const
{
  double px1, py1;

  plot_->windowToPixel(node_->x(), node_->yc(), px1, py1);

  double cs = plot_->circleSize();

  //---

  QFont font = plot_->textFont();

  QFontMetricsF fm(font);

  const QString &name = node_->name();

  bool is_hier = dynamic_cast<CQChartsDendrogram::HierNode *>(node_);

  CQChartsGeom::BBox bbox;

  CQChartsGeom::Point p;

  double dy = (fm.ascent() - fm.descent())/2;

  if (is_hier)
    p = CQChartsGeom::Point(px1 - cs - fm.width(name), py1 + dy);
  else
    p = CQChartsGeom::Point(px1 + cs, py1 + dy);

  CQChartsGeom::Point p1(p.x                 , p.y - fm.ascent());
  CQChartsGeom::Point p2(p.x + fm.width(name), p.y + fm.ascent());

  CQChartsGeom::BBox pbbox(p1, p2);

  CQChartsGeom::BBox wbbox;

  plot_->pixelToWindow(pbbox, wbbox);

  return wbbox;
}

void
CQChartsDendrogramNodeObj::
getSelectIndices(Indices &) const
{
}

void
CQChartsDendrogramNodeObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  if (! node_->isPlaced()) return;

  double px1, py1;

  plot_->windowToPixel(node_->x(), node_->yc(), px1, py1);

  double cs = plot_->circleSize();
  double tm = plot_->textMargin();

  px1 += tm;

  QRectF qrect(px1 - cs/2.0, py1 - cs/2.0, cs, cs);

  //---

  // draw node
  QPen   pen;
  QBrush brush;

  QColor borderColor = plot_->interpNodeBorderColor(0, 1);

  borderColor.setAlphaF(plot_->nodeBorderAlpha());

  double bw = plot_->lengthPixelWidth(plot_->borderWidth());

  pen.setColor (borderColor);
  pen.setWidthF(bw);

  QColor fillColor = plot_->interpNodeFillColor(0, 1);

  fillColor.setAlphaF(plot_->nodeFillAlpha  ());

  if (node_->hasChildren() && ! node_->isOpen()) {
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(fillColor);
  }
  else
    brush.setStyle(Qt::NoBrush);

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  painter->drawEllipse(qrect);

  //---

  // draw node text
  QFont font = plot_->textFont();

  QFontMetricsF fm(font);

  QColor tc = plot_->interpTextColor(0, 1);

  tc.setAlphaF(plot_->textAlpha());

  QPen tpen;

  tpen.setColor(tc);

  painter->setPen (tpen);
  painter->setFont(font);

  const QString &name = node_->name();

  bool is_hier = dynamic_cast<CQChartsDendrogram::HierNode *>(node_);

  double dy = (fm.ascent() - fm.descent())/2;

  QPointF p;

  if (is_hier)
    p = QPointF(px1 - cs - fm.width(name), py1 + dy);
  else
    p = QPointF(px1 + cs, py1 + dy);

  painter->drawText(p, name);
}
