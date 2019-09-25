#include <CQChartsBubblePlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTip.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsHtml.h>

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
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Bubble Plot").
    h3("Summary").
     p("Draws circles represent a set of data values and packs then into the "
       "smallest enclosing circle.").
    h3("Columns").
     p("The values are taken from the value column. The value name can be specified "
       "in the name column.").
    h3("Options").
     p("The value can be displayed at the center of the bubble along with the name.").
     p("Bubbles can be sorted by value or displayed in model order.").
    h3("Customization").
     p("The bubble style (fill and stroke) and text style can be specified.").
    h3("Limitations").
     p("A user defined range cannot be specified, no axes or key are displayed, "
       "logarithmic values are not supported and probing is not allowed.").
    h3("Example").
     p(IMG("images/bubbleplot.png"));
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

  setStroked(true);
  setFilled (true);

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
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  CQChartsPlot::addProperties();

  // columns
  addProp("columns", "nameColumn" , "name" , "Name column");
  addProp("columns", "valueColumn", "value", "Value column");

  CQChartsGroupPlot::addProperties();

  // options
  addProp("options", "valueLabel", "", "Show value label");
  addProp("options", "sorted"    , "", "Sort values by size");

  // coloring
  addProp("coloring", "colorById", "colorById", "Color by id");

  // fill
  addProp("fill", "filled", "visible", "Fill visible");

  addFillProperties("fill", "fill", "");

  // stroke
  addProp("stroke", "stroked", "visible", "Stroke visible");

  addLineProperties("stroke", "stroke", "");

  // text
  addProp("text", "textVisible", "visible", "Text visible");

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

  NoUpdate noUpdate(this);

  //---

  // init value sets
//initValueSets();

  //---

  if (! nodeData_.root)
    initNodes();

  //---

  CQChartsBubblePlot *th = const_cast<CQChartsBubblePlot *>(this);

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
      QString     name;
      QModelIndex nameInd;

      (void) getName(data, name, nameInd);

      //---

      double size = 1.0;

      if (! getSize(data, size))
        return State::SKIP;

      //---

      QModelIndex nameInd1 = plot_->normalizeIndex(nameInd);

      CQChartsBubbleNode *node = plot_->addNode(parentHier(data), name, size, nameInd1);

      if (node) {
        CQChartsColor color;

        if (plot_->columnColor(data.row, data.parent, color))
          node->setColor(color);
      }

      return State::OK;
    }

   private:
    CQChartsBubbleHierNode *parentHier(const VisitData &data) const {
      CQChartsModelIndex ind(data.row, plot_->valueColumn(), data.parent);

      std::vector<int> groupInds = plot_->rowHierGroupInds(ind);

      CQChartsBubbleHierNode *hierNode = plot_->currentRoot();

      for (std::size_t i = 0; i < groupInds.size(); ++i) {
        int groupInd = groupInds[i];

        hierNode = plot_->groupHierNode(hierNode, groupInd);
      }

      return hierNode;
    }

    bool getName(const VisitData &data, QString &name, QModelIndex &nameInd) const {
      if (plot_->nameColumn().isValid())
        nameInd = plot_->modelIndex(data.row, plot_->nameColumn(), data.parent);
      else
        nameInd = plot_->modelIndex(data.row, plot_->idColumn(), data.parent);

      bool ok;

      if (plot_->nameColumn().isValid())
        name = plot_->modelString(data.row, plot_->nameColumn(), data.parent, ok);
      else
        name = plot_->modelString(data.row, plot_->idColumn(), data.parent, ok);

      return ok;
    }

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
execDrawForeground(CQChartsPaintDevice *device) const
{
  drawBounds(device, currentRoot());
}

void
CQChartsBubblePlot::
drawBounds(CQChartsPaintDevice *device, CQChartsBubbleHierNode *hier) const
{
  double xc = hier->x();
  double yc = hier->y();
  double r  = hier->radius();

  //---

  CQChartsGeom::Point p1(xc - r, yc - r);
  CQChartsGeom::Point p2(xc + r, yc + r);

  QRectF qrect(p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);

  //---

  // draw bubble
  QColor bc = interpStrokeColor(ColorInd());

  device->setPen  (bc);
  device->setBrush(Qt::NoBrush);

  QPainterPath path;

  path.addEllipse(qrect);

  device->drawPath(path);
}

//------

CQChartsBubbleHierObj::
CQChartsBubbleHierObj(const CQChartsBubblePlot *plot, CQChartsBubbleHierNode *hier,
                      CQChartsBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                      const ColorInd &is) :
 CQChartsBubbleNodeObj(plot, hier, hierObj, rect, is), hier_(hier)
{
  setModelInd(hier_->ind());
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
draw(CQChartsPaintDevice *device)
{
  CQChartsBubbleHierNode *root = hier_->parent();

  if (! root)
    root = hier_;

  //---

  double r = hier_->radius();

  CQChartsGeom::Point p1(hier_->x() - r, hier_->y() - r);
  CQChartsGeom::Point p2(hier_->x() + r, hier_->y() + r);

  QRectF qrect = CQChartsGeom::BBox(p1.x, p2.y, p2.x, p1.y).qrect();

  //---

  // calc stroke and brush
  ColorInd colorInd = calcColorInd();

  QPen   pen;
  QBrush brush;

  QColor bc = plot_->interpStrokeColor(colorInd);
  QColor fc = hier_->interpColor(plot_, plot_->fillColor(), colorInd, plot_->numColorIds());

  plot_->setPenBrush(pen, brush,
    plot_->isStroked(), bc, plot_->strokeAlpha(), plot_->strokeWidth(), plot_->strokeDash(),
    plot_->isFilled(), fc, plot_->fillAlpha(), plot_->fillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  // draw bubble
  QPainterPath path;

  path.addEllipse(qrect);

  device->drawPath(path);
}

//------

CQChartsBubbleNodeObj::
CQChartsBubbleNodeObj(const CQChartsBubblePlot *plot, CQChartsBubbleNode *node,
                      CQChartsBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                      const ColorInd &is) :
 CQChartsPlotObj(const_cast<CQChartsBubblePlot *>(plot), rect, is, ColorInd(), ColorInd()),
 plot_(plot), node_(node), hierObj_(hierObj)
{
  setDetailHint(DetailHint::MAJOR);

  setModelInd(node_->ind());
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

  //---

  const QModelIndex &ind = node_->ind();

  plot()->addTipColumns(tableTip, ind);

  //---

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
draw(CQChartsPaintDevice *device)
{
  double r = node_->radius();

  CQChartsGeom::Point p1(node_->x() - r, node_->y() - r);
  CQChartsGeom::Point p2(node_->x() + r, node_->y() + r);

  QRectF qrect = CQChartsGeom::BBox(p1.x, p2.y, p2.x, p1.y).qrect();

  //---

  // check if small enough to draw as point
  QPointF pp1 = device->windowToPixel(p1.qpoint());
  QPointF pp2 = device->windowToPixel(p2.qpoint());

  double pw = std::abs(pp2.x() - pp1.x()) - 2;
  double ph = std::abs(pp2.y() - pp1.y()) - 2;

  QPointF qpoint;

  bool isPoint = (pw <= 1.5 || ph <= 1.5);

  if (isPoint)
    qpoint = QPointF((p1.x + p2.x)/2.0, (p1.y + p2.y)/2.0);

  //---

  // calc stroke and brush
  ColorInd colorInd = calcColorInd();

  QPen   pen;
  QBrush brush;

  QColor bc = plot_->interpStrokeColor(colorInd);
  QColor fc = node_->interpColor(plot_, plot_->fillColor(), colorInd, plot_->numColorIds());

  if (isPoint) {
    if      (plot_->isFilled())
      plot_->setPenBrush(pen, brush,
        true, fc, plot_->fillAlpha(), 0.0, CQChartsLineDash(),
        true, fc, plot_->fillAlpha(), plot_->fillPattern());
    else if (plot_->isStroked())
      plot_->setPenBrush(pen, brush,
        true, bc, plot_->strokeAlpha(), plot_->strokeWidth(), plot_->strokeDash(),
        true, bc, plot_->strokeAlpha(), CQChartsFillPattern());
  }
  else {
    plot_->setPenBrush(pen, brush,
      plot_->isStroked(), bc, plot_->strokeAlpha(), plot_->strokeWidth(), plot_->strokeDash(),
      plot_->isFilled(), fc, plot_->fillAlpha(), plot_->fillPattern());
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  // draw bubble
  device->setPen  (pen);
  device->setBrush(brush);

  if (isPoint)
    device->drawPoint(qpoint);
  else {
    QPainterPath path;

    path.addEllipse(qrect);

    device->drawPath(path);
  }

  //---

  if (isPoint)
    return;

  if (! plot_->isTextVisible())
    return;

  //---

  // get labels (name and optional size)
  QStringList strs;

  QString name = (! node_->isFiller() ? node_->name() : node_->parent()->name());

  strs.push_back(name);

  if (plot_->isValueLabel() && ! node_->isFiller()) {
    strs.push_back(QString("%1").arg(node_->size()));
  }

  //---

  // calc text pen
  QPen tpen;

  QColor tc = plot_->interpTextColor(colorInd);

  plot_->setPen(tpen, true, tc, plot_->textAlpha());

  plot_->updateObjPenBrushState(this, tpen, brush);

  //---

  device->save();

  //---

  // set font
  plot_->view()->setPlotPainterFont(plot_, device, plot_->textFont());

  if (plot_->isTextScaled()) {
    // calc text size
    QFontMetricsF fm(device->font());

    double tw = 0;

    for (int i = 0; i < strs.size(); ++i)
      tw = std::max(tw, fm.width(strs[i]));

    double th = strs.size()*fm.height();

    //---

    // calc scale factor
    QRectF prect = device->windowToPixel(qrect);

    double sx = (tw > 0 ? prect.width ()/tw : 1.0);
    double sy = (th > 0 ? prect.height()/th : 1.0);

    double s = std::min(sx, sy);

    //---

    // scale font
    double fs = device->font().pointSizeF()*s;

    QFont font1 = device->font();

    font1.setPointSizeF(fs);

    device->setFont(font1);
  }

  //---

  // calc text position
  CQChartsGeom::Point pc = plot_->windowToPixel(CQChartsGeom::Point(node_->x(), node_->y()));

  //---

  // draw label
  device->setClipRect(qrect);

  CQChartsTextOptions textOptions;

  textOptions.contrast  = plot_->isTextContrast ();
  textOptions.formatted = plot_->isTextFormatted();
  textOptions.html      = plot_->isTextHtml();
//textOptions.align     = plot_->textAlign();

  textOptions = plot_->adjustTextOptions(textOptions);

  device->setPen(tpen);

  QPointF tp = pc.qpoint();

  if      (strs.size() == 1) {
    CQChartsDrawUtil::drawTextAtPoint(device, device->pixelToWindow(tp), name, textOptions);
  }
  else if (strs.size() == 2) {
    QFontMetricsF fm(device->font());

    double th = fm.height();

    QPointF tp1 = device->pixelToWindow(QPointF(tp.x(), tp.y() - th/2));
    QPointF tp2 = device->pixelToWindow(QPointF(tp.x(), tp.y() + th/2));

    CQChartsDrawUtil::drawTextAtPoint(device, tp1, strs[0], textOptions);
    CQChartsDrawUtil::drawTextAtPoint(device, tp2, strs[1], textOptions);
  }
  else {
    assert(false);
  }

  //---

  device->restore();
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
interpColor(const CQChartsBubblePlot *plot, const CQChartsColor &c,
            const ColorInd &colorInd, int n) const
{
  using Colors = std::vector<QColor>;

  Colors colors;

  for (auto &child : children_)
    colors.push_back(child->interpColor(plot, c, colorInd, n));

  for (auto &node : nodes_)
    colors.push_back(node->interpColor(plot, c, colorInd, n));

  if (colors.empty())
    return plot->interpColor(c, colorInd);

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
interpColor(const CQChartsBubblePlot *plot, const CQChartsColor &c,
            const ColorInd &colorInd, int n) const
{
  if      (color().isValid())
    return plot->interpColor(color(), ColorInd());
  else if (colorId() >= 0 && plot_->isColorById())
    return plot->interpColor(c, ColorInd(colorId(), n));
  else
    return plot->interpColor(c, colorInd);
}
