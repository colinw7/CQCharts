#include <CQChartsDendrogramPlot.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <QPainter>

CQChartsDendrogramPlotType::
CQChartsDendrogramPlotType()
{
  addParameters();
}

void
CQChartsDendrogramPlotType::
addParameters()
{
  addColumnParameter("name" , "name" , "nameColumn" , "", 0);
  addColumnParameter("value", "Value", "valueColumn", "", 1);
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
  nodeBoxObj_ = new CQChartsTextBoxObj(this);
  edgeBoxObj_ = new CQChartsTextBoxObj(this);

  nodeBoxObj_->setBackgroundColor(CQChartsPaletteColor(CQChartsPaletteColor::Type::PALETTE));
}

CQChartsDendrogramPlot::
~CQChartsDendrogramPlot()
{
  delete nodeBoxObj_;
  delete edgeBoxObj_;

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

QString
CQChartsDendrogramPlot::
nodeBorderColorStr() const
{
  return nodeBoxObj_->borderColorStr();
}

void
CQChartsDendrogramPlot::
setNodeBorderColorStr(const QString &str)
{
  nodeBoxObj_->setBorderColorStr(str);

  update();
}

QColor
CQChartsDendrogramPlot::
interpNodeBorderColor(int i, int n) const
{
  return nodeBoxObj_->interpBorderColor(i, n);
}

double
CQChartsDendrogramPlot::
nodeBorderAlpha() const
{
  return nodeBoxObj_->borderAlpha();
}

void
CQChartsDendrogramPlot::
setNodeBorderAlpha(double a)
{
  nodeBoxObj_->setBorderAlpha(a);

  update();
}

double
CQChartsDendrogramPlot::
nodeBorderWidth() const
{
  return nodeBoxObj_->borderWidth();
}

void
CQChartsDendrogramPlot::
setNodeBorderWidth(double r)
{
  nodeBoxObj_->setBorderWidth(r);

  update();
}

//---

QString
CQChartsDendrogramPlot::
nodeFillColorStr() const
{
  return nodeBoxObj_->backgroundColorStr();
}

void
CQChartsDendrogramPlot::
setNodeFillColorStr(const QString &s)
{
  nodeBoxObj_->setBackgroundColorStr(s);

  update();
}

QColor
CQChartsDendrogramPlot::
interpNodeFillColor(int i, int n) const
{
  return nodeBoxObj_->interpBackgroundColor(i, n);
}

double
CQChartsDendrogramPlot::
nodeFillAlpha() const
{
  return nodeBoxObj_->backgroundAlpha();
}

void
CQChartsDendrogramPlot::
setNodeFillAlpha(double a)
{
  nodeBoxObj_->setBackgroundAlpha(a);

  update();
}

//---

QString
CQChartsDendrogramPlot::
edgeLineColorStr() const
{
  return edgeBoxObj_->borderColorStr();
}

void
CQChartsDendrogramPlot::
setEdgeLineColorStr(const QString &str)
{
  edgeBoxObj_->setBorderColorStr(str);

  update();
}

QColor
CQChartsDendrogramPlot::
interpEdgeLineColor(int i, int n) const
{
  return edgeBoxObj_->interpBorderColor(i, n);
}

double
CQChartsDendrogramPlot::
edgeLineAlpha() const
{
  return edgeBoxObj_->borderAlpha();
}

void
CQChartsDendrogramPlot::
setEdgeLineAlpha(double a)
{
  edgeBoxObj_->setBorderAlpha(a);

  update();
}

double
CQChartsDendrogramPlot::
edgeLineWidth() const
{
  return edgeBoxObj_->borderWidth();
}

void
CQChartsDendrogramPlot::
setEdgeLineWidth(double r)
{
  edgeBoxObj_->setBorderWidth(r);

  update();
}

//---

const QFont &
CQChartsDendrogramPlot::
textFont() const
{
  return nodeBoxObj_->textFont();
}

void
CQChartsDendrogramPlot::
setTextFont(const QFont &f)
{
  nodeBoxObj_->setTextFont(f);

  update();
}

QString
CQChartsDendrogramPlot::
textColorStr() const
{
  return nodeBoxObj_->textColorStr();
}

void
CQChartsDendrogramPlot::
setTextColorStr(const QString &s)
{
  nodeBoxObj_->setTextColorStr(s);

  update();
}

QColor
CQChartsDendrogramPlot::
interpTextColor(int i, int n) const
{
  return nodeBoxObj_->interpTextColor(i, n);
}

double
CQChartsDendrogramPlot::
textAlpha() const
{
  return nodeBoxObj_->textAlpha();
}

void
CQChartsDendrogramPlot::
setTextAlpha(double a)
{
  nodeBoxObj_->setTextAlpha(a);

  update();
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

  int nr = model->rowCount(QModelIndex());

  dataRange_.reset();

  for (int r = 0; r < nr; ++r) {
    QModelIndex nameInd  = model->index(r, nameColumn ());
    QModelIndex valueInd = model->index(r, valueColumn());

    //QModelIndex nameInd1 = normalizeIndex(nameInd);

    //---

    bool ok1;

    QString name = CQChartsUtil::modelString(model, nameInd, ok1);

    //--

    bool ok2;

    double value = CQChartsUtil::modelReal(model, valueInd, ok2);

    if (CQChartsUtil::isNaN(value))
      continue;

    //---

    QStringList names;

    int pos = name.indexOf('/');

    while (pos != -1) {
      QString lhs = name.mid(0, pos);
      QString rhs = name.mid(pos + 1);

      names.push_back(lhs);

      name = rhs;

      pos = name.indexOf('/');
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
      hierNode = dendrogram_->createRootNode(name);

      hierNode->setOpen(false);
    }
    else {
      CQChartsDendrogram::Node *node = dendrogram_->createNode(hierNode, name, value);

      assert(node);
      //node->setInd(nameInd1);
    }
  }

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

    lpen.setColor (edgeColor);
    lpen.setWidthF(edgeLineWidth());

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
mousePress(const CQChartsGeom::Point &p, ModSelect /*modSelect*/)
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

  QFontMetrics fm(font);

  const QString &name = node_->name();

  bool is_hier = dynamic_cast<CQChartsDendrogram::HierNode *>(node_);

  CQChartsGeom::BBox bbox;

  CQChartsGeom::Point p;

  int dy = (fm.ascent() - fm.descent())/2;

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
addSelectIndex()
{
}

bool
CQChartsDendrogramNodeObj::
isIndex(const QModelIndex &) const
{
  return false;
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

  pen.setColor (borderColor);
  pen.setWidthF(plot_->borderWidth());

  QColor fillColor = plot_->interpNodeFillColor  (0, 1);

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

  QFontMetrics fm(font);

  QColor textColor = plot_->interpTextColor(0, 1);

  textColor.setAlphaF(plot_->textAlpha());

  QPen tpen;

  tpen.setColor(textColor);

  painter->setPen (tpen);
  painter->setFont(font);

  const QString &name = node_->name();

  bool is_hier = dynamic_cast<CQChartsDendrogram::HierNode *>(node_);

  int dy = (fm.ascent() - fm.descent())/2;

  QPointF p;

  if (is_hier)
    p = QPointF(px1 - cs - fm.width(name), py1 + dy);
  else
    p = QPointF(px1 + cs, py1 + dy);

  painter->drawText(p, name);
}
