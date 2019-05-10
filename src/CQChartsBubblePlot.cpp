#include <CQChartsBubblePlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTip.h>
#include <CQChartsDrawUtil.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QPainter>

CQChartsBubblePlotType::
CQChartsBubblePlotType()
{
}

void
CQChartsBubblePlotType::
addParameters()
{
  startParameterGroup("Bubble");

  addColumnParameter("name", "Name", "nameColumn").
   setString().setTip("Name Column");

  addColumnParameter("value", "Value", "valueColumn").
   setNumeric().setRequired().setTip("Value Column");

  endParameterGroup();

  //---

  CQChartsGroupPlotType::addParameters();
}

QString
CQChartsBubblePlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draws circles represent a set of data values and packs then into the "
         "smallest enclosing circle.</p>\n";
}

CQChartsPlot *
CQChartsBubblePlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsBubblePlot(view, model);
}

//------

CQChartsBubblePlot::
CQChartsBubblePlot(CQChartsView *view, const ModelP &model) :
 CQChartsGroupPlot(view, view->charts()->plotType("bubble"), model),
 CQChartsObjShapeData<CQChartsBubblePlot>(this),
 CQChartsObjTextData <CQChartsBubblePlot>(this)
{
  NoUpdate noUpdate(this);

  setExactValue(false);

  //---

  setFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setBorder(true);
  setFilled(true);

  setTextContrast(true);
  setTextFontSize(12.0);

  setTextColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 1));

  setOuterMargin(CQChartsLength("4px"), CQChartsLength("4px"),
                 CQChartsLength("4px"), CQChartsLength("4px"));

  addTitle();
}

CQChartsBubblePlot::
~CQChartsBubblePlot()
{
  delete nodeData_.root;
}

//------

void
CQChartsBubblePlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBubblePlot::
setValueColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//------

void
CQChartsBubblePlot::
setValueLabel(bool b)
{
  CQChartsUtil::testAndSet(valueLabel_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsBubblePlot::
setTextFontSize(double s)
{
  if (s != textData_.font().pointSizeF()) {
    CQChartsFont f = textData_.font(); f.setPointSizeF(s); textData_.setFont(f);

    drawObjs();
  }
}

//---

void
CQChartsBubblePlot::
setColorById(bool b)
{
  CQChartsUtil::testAndSet(colorById_, b, [&]() { drawObjs(); } );
}

//----

void
CQChartsBubblePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  // columns
  addProperty("columns", this, "nameColumn" , "name" )->setDesc("Name column");
  addProperty("columns", this, "valueColumn", "value")->setDesc("Value column");

  CQChartsGroupPlot::addProperties();

  // options
  addProperty("options", this, "valueLabel")->setDesc("Show value label");
  addProperty("options", this, "sorted"    )->setDesc("Sort values by size");

  // color
  addProperty("color", this, "colorById", "colorById")->setDesc("Color by id");

  // fill
  addProperty("fill", this, "filled", "visible")->setDesc("Fill visible");

  addFillProperties("fill", "fill", "");

  // stroke
  addProperty("stroke", this, "border", "visible")->setDesc("Stroke visible");

  addLineProperties("stroke", "border", "");

  // text
  addAllTextProperties("text", "text", "");

  // color map
  addColorMapProperties();
}

//---

CQChartsBubbleHierNode *
CQChartsBubblePlot::
currentRoot() const
{
  return nodeData_.root;
}

//---

CQChartsGeom::Range
CQChartsBubblePlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsBubblePlot::calcRange");

  double r = 1.0;

  CQChartsGeom::Range dataRange;

  dataRange.updateRange(-r, -r);
  dataRange.updateRange( r,  r);

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange.equalScale(aspect);
  }

  //---

  initGroupData(CQChartsColumns(), nameColumn(), /*hier*/true);

  //---

  return dataRange;
}

//------

void
CQChartsBubblePlot::
clearPlotObjects()
{
  resetNodes();

  CQChartsPlot::clearPlotObjects();
}

bool
CQChartsBubblePlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsBubblePlot::createObjs");

  CQChartsBubblePlot *th = const_cast<CQChartsBubblePlot *>(this);

  NoUpdate noUpdate(th);

  //---

  // init value sets
//initValueSets();

  //---

  if (! nodeData_.root)
    initNodes();

  //---

  th->initColorIds();

  colorNodes(nodeData_.root);

  //---

  initNodeObjs(currentRoot(), nullptr, 0, objs);

  //---

  int ig = 0, in = 0;

  for (auto &obj : objs) {
    CQChartsBubbleHierObj *hierObj = dynamic_cast<CQChartsBubbleHierObj *>(obj);
    CQChartsBubbleNodeObj *nodeObj = dynamic_cast<CQChartsBubbleNodeObj *>(obj);

    if      (hierObj) { hierObj->setInd(ig); ++ig; }
    else if (nodeObj) { nodeObj->setInd(in); ++in; }
  }

  for (auto &obj : objs) {
    CQChartsBubbleHierObj *hierObj = dynamic_cast<CQChartsBubbleHierObj *>(obj);
    CQChartsBubbleNodeObj *nodeObj = dynamic_cast<CQChartsBubbleNodeObj *>(obj);

    if      (hierObj) {
      if (hierObj->parent())
        hierObj->setIg(ColorInd(hierObj->parent()->ind(), ig));

      hierObj->setIv(ColorInd(hierObj->ind(), ig));
    }
    else if (nodeObj) {
      if (nodeObj->parent())
        nodeObj->setIg(ColorInd(nodeObj->parent()->ind(), ig));

      nodeObj->setIv(ColorInd(nodeObj->ind(), in));
    }
  }

  return true;
}

void
CQChartsBubblePlot::
initNodeObjs(CQChartsBubbleHierNode *hier, CQChartsBubbleHierObj *parentObj,
             int depth, PlotObjs &objs) const
{
  CQChartsBubbleHierObj *hierObj = nullptr;

  if (hier != nodeData_.root) {
    double r = hier->radius();

    CQChartsGeom::BBox rect(hier->x() - r, hier->y() - r, hier->x() + r, hier->y() + r);

    ColorInd is(hier->depth(), maxDepth() + 1);

    hierObj = new CQChartsBubbleHierObj(this, hier, parentObj, rect, is);

    objs.push_back(hierObj);
  }

  //---

  for (auto &hierNode : hier->getChildren()) {
    initNodeObjs(hierNode, hierObj, depth + 1, objs);
  }

  //---

  for (auto &node : hier->getNodes()) {
    if (! node->placed()) continue;

    //---

    double r = node->radius();

    CQChartsGeom::BBox rect(node->x() - r, node->y() - r, node->x() + r, node->y() + r);

    ColorInd is(node->depth(), maxDepth() + 1);

    CQChartsBubbleNodeObj *obj = new CQChartsBubbleNodeObj(this, node, parentObj, rect, is);

    objs.push_back(obj);
  }
}

void
CQChartsBubblePlot::
resetNodes()
{
  delete nodeData_.root;

  nodeData_.root = nullptr;

  groupHierNodes_.clear();
}

void
CQChartsBubblePlot::
initNodes() const
{
  CQChartsBubblePlot *th = const_cast<CQChartsBubblePlot *>(this);

  th->nodeData_.hierInd = 0;

  th->nodeData_.root = new CQChartsBubbleHierNode(this, 0, "<root>");

  th->nodeData_.root->setDepth(0);
  th->nodeData_.root->setHierInd(th->nodeData_.hierInd++);

  //---

  loadModel();

  //---

  replaceNodes();
}

void
CQChartsBubblePlot::
replaceNodes() const
{
  placeNodes(nodeData_.root);
}

void
CQChartsBubblePlot::
placeNodes(CQChartsBubbleHierNode *hier) const
{
  CQChartsBubblePlot *th = const_cast<CQChartsBubblePlot *>(this);

  initNodes(hier);

  //---

  hier->packNodes();

  th->placeData_.offset = CQChartsGeom::Point(hier->x(), hier->y());
  th->placeData_.scale  = (hier->radius() > 0.0 ? 1.0/hier->radius() : 1.0);

  //---

  hier->setX((hier->x() - offset().x)*scale());
  hier->setY((hier->y() - offset().y)*scale());

  hier->setRadius(1.0);

  transformNodes(hier);
}

void
CQChartsBubblePlot::
initNodes(CQChartsBubbleHierNode *hier) const
{
  for (auto &hierNode : hier->getChildren()) {
    hierNode->initRadius();

    initNodes(hierNode);
  }

  //---

  for (auto &node : hier->getNodes())
    node->initRadius();
}

void
CQChartsBubblePlot::
transformNodes(CQChartsBubbleHierNode *hier) const
{
  for (auto &hierNode : hier->getChildren()) {
    hierNode->setX((hierNode->x() - offset().x)*scale());
    hierNode->setY((hierNode->y() - offset().y)*scale());

    hierNode->setRadius(hierNode->radius()*scale());

    transformNodes(hierNode);
  }

   //---

  for (auto &node : hier->getNodes()) {
    node->setX((node->x() - offset().x)*scale());
    node->setY((node->y() - offset().y)*scale());

    node->setRadius(node->radius()*scale());
  }
}

void
CQChartsBubblePlot::
colorNodes(CQChartsBubbleHierNode *hier) const
{
  if (! hier->hasNodes() && ! hier->hasChildren()) {
    colorNode(hier);
  }
  else {
    for (const auto &node : hier->getNodes())
      colorNode(node);

    for (const auto &child : hier->getChildren())
      colorNodes(child);
  }
}

void
CQChartsBubblePlot::
colorNode(CQChartsBubbleNode *node) const
{
  if (! node->color().isValid()) {
    CQChartsBubblePlot *th = const_cast<CQChartsBubblePlot *>(this);

    node->setColorId(th->nextColorId());
  }
}

void
CQChartsBubblePlot::
loadModel() const
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsBubblePlot *plot) :
     plot_(plot) {
      valueColumnType_ = plot_->columnValueType(plot_->valueColumn());
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      CQChartsModelIndex ind(data.row, plot_->valueColumn(), data.parent);

      std::vector<int> groupInds = plot_->rowHierGroupInds(ind);

      CQChartsBubbleHierNode *hierNode = plot_->currentRoot();

      for (std::size_t i = 0; i < groupInds.size(); ++i) {
        int groupInd = groupInds[i];

        hierNode = plot_->groupHierNode(hierNode, groupInd);
      }

      //---

      bool ok1;

      QString name = plot_->modelString(data.row, plot_->nameColumn(), data.parent, ok1);

      //---

      double size = 1.0;

      if (! getSize(data, size))
        return State::SKIP;

      //---

      QModelIndex nameInd  = plot_->modelIndex(data.row, plot_->nameColumn(), data.parent);
      QModelIndex nameInd1 = plot_->normalizeIndex(nameInd);

      CQChartsBubblePlot *plot = const_cast<CQChartsBubblePlot *>(plot_);

      CQChartsBubbleNode *node = plot->addNode(hierNode, name, size, nameInd1);

      if (node) {
        CQChartsColor color;

        if (plot->columnColor(data.row, data.parent, color))
          node->setColor(color);
      }

      return State::OK;
    }

   private:
    bool getSize(const VisitData &data, double &size) const {
      size = 1.0;

      if (! plot_->valueColumn().isValid())
        return true;

      bool ok = true;

      if      (valueColumnType_ == ColumnType::REAL)
        size = plot_->modelReal(data.row, plot_->valueColumn(), data.parent, ok);
      else if (valueColumnType_ == ColumnType::INTEGER)
        size = plot_->modelInteger(data.row, plot_->valueColumn(), data.parent, ok);
      else if (valueColumnType_ == ColumnType::STRING)
        size = 1.0;
      else
        ok = false;

      if (ok && size <= 0.0)
        ok = false;

      return ok;
    }

   private:
    const CQChartsBubblePlot* plot_            { nullptr };
    ColumnType                valueColumnType_ { ColumnType::NONE };
  };

  RowVisitor visitor(this);

  visitModel(visitor);
}

CQChartsBubbleHierNode *
CQChartsBubblePlot::
groupHierNode(CQChartsBubbleHierNode *parent, int groupInd) const
{
  if (groupInd < 0)
    return parent;

  auto p = groupHierNodes_.find(groupInd);

  if (p != groupHierNodes_.end())
    return (*p).second;

  CQChartsBubblePlot *th = const_cast<CQChartsBubblePlot *>(this);

  QString name = groupIndName(groupInd, /*hier*/true);

  QModelIndex ind;

  CQChartsBubbleHierNode *hierNode = th->addHierNode(parent, name, ind);

  auto p1 = th->groupHierNodes_.insert(th->groupHierNodes_.end(),
              GroupHierNodes::value_type(groupInd, hierNode));

  return (*p1).second;
}

CQChartsBubbleHierNode *
CQChartsBubblePlot::
addHierNode(CQChartsBubbleHierNode *hier, const QString &name, const QModelIndex &nameInd) const
{
  CQChartsBubblePlot *th = const_cast<CQChartsBubblePlot *>(this);

  int depth1 = hier->depth() + 1;

  QModelIndex nameInd1 = normalizeIndex(nameInd);

  CQChartsBubbleHierNode *hier1 = new CQChartsBubbleHierNode(this, hier, name, nameInd1);

  hier1->setDepth(depth1);

  hier1->setHierInd(th->nodeData_.hierInd++);

  th->nodeData_.maxDepth = std::max(nodeData_.maxDepth, depth1);

  return hier1;
}

CQChartsBubbleNode *
CQChartsBubblePlot::
addNode(CQChartsBubbleHierNode *hier, const QString &name, double size,
        const QModelIndex &nameInd) const
{
  CQChartsBubblePlot *th = const_cast<CQChartsBubblePlot *>(this);

  int depth1 = hier->depth() + 1;

  QModelIndex nameInd1 = normalizeIndex(nameInd);

  CQChartsBubbleNode *node = new CQChartsBubbleNode(this, hier, name, size, nameInd1);

  node->setDepth(depth1);

  hier->addNode(node);

  th->nodeData_.maxDepth = std::max(nodeData_.maxDepth, depth1);

  return node;
}

//------

void
CQChartsBubblePlot::
postResize()
{
  CQChartsPlot::postResize();

  resetDataRange(/*updateRange*/true, /*updateObjs*/false);
}

//------

bool
CQChartsBubblePlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsBubblePlot::
execDrawForeground(QPainter *painter) const
{
  drawBounds(painter, currentRoot());
}

void
CQChartsBubblePlot::
drawBounds(QPainter *painter, CQChartsBubbleHierNode *hier) const
{
  double xc = hier->x();
  double yc = hier->y();
  double r  = hier->radius();

  //---

  CQChartsGeom::Point p1 = windowToPixel(CQChartsGeom::Point(xc - r, yc + r));
  CQChartsGeom::Point p2 = windowToPixel(CQChartsGeom::Point(xc + r, yc - r));

  QRectF qrect(p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);

  //---

  // draw bubble
  QColor bc = interpBorderColor(ColorInd());

  painter->setPen  (bc);
  painter->setBrush(Qt::NoBrush);

  QPainterPath path;

  path.addEllipse(qrect);

  painter->drawPath(path);
}

//------

CQChartsBubbleHierObj::
CQChartsBubbleHierObj(const CQChartsBubblePlot *plot, CQChartsBubbleHierNode *hier,
                      CQChartsBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                      const ColorInd &is) :
 CQChartsBubbleNodeObj(plot, hier, hierObj, rect, is), hier_(hier)
{
}

QString
CQChartsBubbleHierObj::
calcId() const
{
  //return QString("%1:%2").arg(hier_->name()).arg(hier_->hierSize());
  return CQChartsBubbleNodeObj::calcId();
}

QString
CQChartsBubbleHierObj::
calcTipId() const
{
  //return QString("%1:%2").arg(hier_->hierName()).arg(hier_->hierSize());
  return CQChartsBubbleNodeObj::calcTipId();
}

bool
CQChartsBubbleHierObj::
inside(const CQChartsGeom::Point &p) const
{
  if (CQChartsUtil::PointPointDistance(p,
        CQChartsGeom::Point(hier_->x(), hier_->y())) < hier_->radius())
    return true;

  return false;
}

void
CQChartsBubbleHierObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->nameColumn ());
  addColumnSelectIndex(inds, plot_->valueColumn());
}

void
CQChartsBubbleHierObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    const QModelIndex &ind = hier_->ind();

    addSelectIndex(inds, ind.row(), column, ind.parent());
  }
}

void
CQChartsBubbleHierObj::
draw(QPainter *painter)
{
  CQChartsBubbleHierNode *root = hier_->parent();

  if (! root)
    root = hier_;

  //---

  double r = hier_->radius();

  CQChartsGeom::Point p1 =
    plot_->windowToPixel(CQChartsGeom::Point(hier_->x() - r, hier_->y() + r));
  CQChartsGeom::Point p2 =
    plot_->windowToPixel(CQChartsGeom::Point(hier_->x() + r, hier_->y() - r));

  QRectF qrect = CQChartsUtil::toQRect(CQChartsGeom::BBox(p1.x, p2.y, p2.x, p1.y));

  //---

  // calc stroke and brush
  ColorInd colorInd = calcColorInd();

  QPen   pen;
  QBrush brush;

  QColor bc = plot_->interpBorderColor(colorInd);
  QColor fc = hier_->interpColor(plot_, colorInd, plot_->numColorIds());

  plot_->setPenBrush(pen, brush,
    plot_->isBorder(), bc, plot_->borderAlpha(), plot_->borderWidth(), plot_->borderDash(),
    plot_->isFilled(), fc, plot_->fillAlpha(), plot_->fillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  // draw bubble
  QPainterPath path;

  path.addEllipse(qrect);

  painter->drawPath(path);
}

//------

CQChartsBubbleNodeObj::
CQChartsBubbleNodeObj(const CQChartsBubblePlot *plot, CQChartsBubbleNode *node,
                      CQChartsBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                      const ColorInd &is) :
 CQChartsPlotObj(const_cast<CQChartsBubblePlot *>(plot), rect, is, ColorInd(), ColorInd()),
 plot_(plot), node_(node), hierObj_(hierObj)
{
}

QString
CQChartsBubbleNodeObj::
calcId() const
{
  if (node_->isFiller())
    return hierObj_->calcId();

  return QString("%1:%2:%3").arg(typeName()).arg(node_->name()).arg(node_->hierSize());
}

QString
CQChartsBubbleNodeObj::
calcTipId() const
{
  if (node_->isFiller())
    return hierObj_->calcTipId();

  CQChartsTableTip tableTip;

  //return QString("%1:%2").arg(name).arg(node_->hierSize());

  tableTip.addTableRow("Name", node_->hierName());
  tableTip.addTableRow("Size", node_->hierSize());

  if (plot_->colorColumn().isValid()) {
    QModelIndex ind1 = plot_->unnormalizeIndex(node_->ind());

    bool ok;

    QString colorStr = plot_->modelString(ind1.row(), plot_->colorColumn(), ind1.parent(), ok);

    tableTip.addTableRow("Color", colorStr);
  }

  return tableTip.str();
}

bool
CQChartsBubbleNodeObj::
inside(const CQChartsGeom::Point &p) const
{
  if (CQChartsUtil::PointPointDistance(p,
        CQChartsGeom::Point(node_->x(), node_->y())) < node_->radius())
    return true;

  return false;
}

void
CQChartsBubbleNodeObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->nameColumn ());
  addColumnSelectIndex(inds, plot_->valueColumn());
}

void
CQChartsBubbleNodeObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    const QModelIndex &ind = node_->ind();

    addSelectIndex(inds, ind.row(), column, ind.parent());
  }
}

void
CQChartsBubbleNodeObj::
draw(QPainter *painter)
{
  double r = node_->radius();

  CQChartsGeom::Point p1 =
    plot_->windowToPixel(CQChartsGeom::Point(node_->x() - r, node_->y() + r));
  CQChartsGeom::Point p2 =
    plot_->windowToPixel(CQChartsGeom::Point(node_->x() + r, node_->y() - r));

  QRectF qrect = CQChartsUtil::toQRect(CQChartsGeom::BBox(p1.x, p2.y, p2.x, p1.y));

  //---

  // calc stroke and brush
  ColorInd colorInd = calcColorInd();

  QPen   pen;
  QBrush brush;

  QColor bc = plot_->interpBorderColor(colorInd);
  QColor fc = node_->interpColor(plot_, colorInd, plot_->numColorIds());

  plot_->setPenBrush(pen, brush,
    plot_->isBorder(), bc, plot_->borderAlpha(), plot_->borderWidth(), plot_->borderDash(),
    plot_->isFilled(), fc, plot_->fillAlpha(), plot_->fillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  // calc text pen
  QPen tpen;

  QColor tc = plot_->interpTextColor(ColorInd());

  plot_->setPen(tpen, true, tc, plot_->textAlpha());

  plot_->updateObjPenBrushState(this, tpen, brush);

  //---

  painter->save();

  //---

  // draw bubble
  painter->setPen  (pen);
  painter->setBrush(brush);

  QPainterPath path;

  path.addEllipse(qrect);

  painter->drawPath(path);

  //---

  QStringList strs;

  QString name = (! node_->isFiller() ? node_->name() : node_->parent()->name());

  strs.push_back(name);

  if (plot_->isValueLabel() && ! node_->isFiller()) {
    strs.push_back(QString("%1").arg(node_->size()));
  }

  //---

  // set font
  plot_->view()->setPlotPainterFont(plot_, painter, plot_->textFont());

  if (plot_->isTextScaled()) {
    // calc text size
    QFontMetricsF fm(painter->font());

    double tw = 0;

    for (int i = 0; i < strs.size(); ++i)
      tw = std::max(tw, fm.width(strs[i]));

    double th = strs.size()*fm.height();

    //---

    // calc scale factor
    double sx = (tw > 0 ? qrect.width ()/tw : 1.0);
    double sy = (th > 0 ? qrect.height()/th : 1.0);

    double s = std::min(sx, sy);

    //---

    // scale font
    double fs = painter->font().pointSizeF()*s;

    QFont font1 = painter->font();

    font1.setPointSizeF(fs);

    painter->setFont(font1);
  }

  //---

  // calc text size and position
  CQChartsGeom::Point pc = plot_->windowToPixel(CQChartsGeom::Point(node_->x(), node_->y()));

  //---

  // draw label
  painter->setClipRect(qrect);

  CQChartsTextOptions textOptions;

  textOptions.contrast  = plot_->isTextContrast ();
  textOptions.formatted = plot_->isTextFormatted();

  textOptions = plot_->adjustTextOptions(textOptions);

  painter->setPen(tpen);

  if      (strs.size() == 1) {
    QPointF tp(pc.x, pc.y);

    CQChartsDrawUtil::drawTextAtPoint(painter, tp, name, textOptions);
  }
  else if (strs.size() == 2) {
    QFontMetricsF fm(painter->font());

    double th = fm.height();

    QPointF tp1(pc.x, pc.y - th/2);
    QPointF tp2(pc.x, pc.y + th/2);

    CQChartsDrawUtil::drawTextAtPoint(painter, tp1, strs[0], textOptions);
    CQChartsDrawUtil::drawTextAtPoint(painter, tp2, strs[1], textOptions);
  }
  else {
    assert(false);
  }

  //---

  painter->restore();
}

//------

CQChartsBubbleHierNode::
CQChartsBubbleHierNode(const CQChartsBubblePlot *plot, CQChartsBubbleHierNode *parent,
                       const QString &name, const QModelIndex &ind) :
 CQChartsBubbleNode(plot, parent, name, 0.0, ind)
{
  if (parent_)
    parent_->children_.push_back(this);
}

CQChartsBubbleHierNode::
~CQChartsBubbleHierNode()
{
  for (auto &child : children_)
    delete child;

  for (auto &node : nodes_)
    delete node;
}

double
CQChartsBubbleHierNode::
hierSize() const
{
  double s = size();

  for (auto &child : children_)
    s += child->hierSize();

  for (auto &node : nodes_)
    s += node->hierSize();

  return s;
}

void
CQChartsBubbleHierNode::
packNodes()
{
  pack_.reset();

  for (auto &node : nodes_)
    node->resetPosition();

  //---

  // pack child hier nodes first
  for (auto &child : children_)
    child->packNodes();

  //---

  // make single list of nodes to pack
  Nodes packNodes;

  for (auto &child : children_)
    packNodes.push_back(child);

  for (auto &node : nodes_)
    packNodes.push_back(node);

  // sort nodes
  if (plot_->isSorted())
    std::sort(packNodes.begin(), packNodes.end(), CQChartsBubbleNodeCmp());

  // pack nodes
  for (auto &packNode : packNodes)
    pack_.addNode(packNode);

  //---

  // get bounding circle
  double xc { 0.0 }, yc { 0.0 }, r { 1.0 };

  pack_.boundingCircle(xc, yc, r);

  // set center and radius
  x_ = xc;
  y_ = yc;

  setRadius(r);

  //setRadius(std::max(std::max(fabs(xmin), xmax), std::max(fabs(ymin), ymax)));
}

void
CQChartsBubbleHierNode::
addNode(CQChartsBubbleNode *node)
{
  nodes_.push_back(node);
}

void
CQChartsBubbleHierNode::
removeNode(CQChartsBubbleNode *node)
{
  int n = nodes_.size();

  int i = 0;

  for ( ; i < n; ++i) {
    if (nodes_[i] == node)
      break;
  }

  assert(i < n);

  ++i;

  for ( ; i < n; ++i)
    nodes_[i - 1] = nodes_[i];

  nodes_.pop_back();
}

void
CQChartsBubbleHierNode::
setPosition(double x, double y)
{
  double dx = x - this->x();
  double dy = y - this->y();

  CQChartsBubbleNode::setPosition(x, y);

  for (auto &node : nodes_)
    node->setPosition(node->x() + dx, node->y() + dy);

  for (auto &child : children_)
    child->setPosition(child->x() + dx, child->y() + dy);
}

QColor
CQChartsBubbleHierNode::
interpColor(const CQChartsBubblePlot *plot, const ColorInd &colorInd, int n) const
{
  using Colors = std::vector<QColor>;

  Colors colors;

  for (auto &child : children_)
    colors.push_back(child->interpColor(plot, colorInd, n));

  for (auto &node : nodes_)
    colors.push_back(node->interpColor(plot, colorInd, n));

  if (colors.empty())
    return plot->interpPaletteColor(colorInd);

  return CQChartsUtil::blendColors(colors);
}

//------

CQChartsBubbleNode::
CQChartsBubbleNode(const CQChartsBubblePlot *plot, CQChartsBubbleHierNode *parent,
                   const QString &name, double size, const QModelIndex &ind) :
 plot_(plot), parent_(parent), id_(nextId()), name_(name), size_(size), ind_(ind)
{
  initRadius();
}

CQChartsBubbleNode::
~CQChartsBubbleNode()
{
}

void
CQChartsBubbleNode::
initRadius()
{
  r_ = sqrt(hierSize()/(2*M_PI));
}

QString
CQChartsBubbleNode::
hierName() const
{
  if (parent() && parent() != plot()->root())
    return parent()->hierName() + "/" + name();
  else
    return name();
}

void
CQChartsBubbleNode::
setPosition(double x, double y)
{
  CQChartsCircleNode::setPosition(x, y);

  placed_ = true;
}

QColor
CQChartsBubbleNode::
interpColor(const CQChartsBubblePlot *plot, const ColorInd &colorInd, int n) const
{
  if      (color().isValid())
    return plot->interpColor(color(), ColorInd());
  else if (colorId() >= 0 && plot_->isColorById())
    return plot->interpPaletteColor(ColorInd(colorId(), n));
  else
    return plot->interpPaletteColor(colorInd);
}
