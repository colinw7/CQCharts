#include <CQChartsBubblePlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsTip.h>

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

  addColumnParameter("name" , "Name" , "nameColumn");
  addColumnParameter("value", "Value", "valueColumn", 0).setRequired();
  addColumnParameter("color", "Color", "colorColumn").setTip("Custom bubble color");

  endParameterGroup();

  //---

  CQChartsGroupPlotType::addParameters();

  //---

  CQChartsPlotType::addParameters();
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
 CQChartsGroupPlot(view, view->charts()->plotType("bubble"), model)
{
  (void) addColorSet("color");

  setExactValue(false);

  //---

  setFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setBorder(true);
  setFilled(true);

  setTextContrast(true);
  setTextFontSize(12.0);

  setTextColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 1));

  setMargins(1, 1, 1, 1);

  addTitle();
}

CQChartsBubblePlot::
~CQChartsBubblePlot()
{
  delete root_;
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

bool
CQChartsBubblePlot::
isFilled() const
{
  return shapeData_.background.visible;
}

void
CQChartsBubblePlot::
setFilled(bool b)
{
  CQChartsUtil::testAndSet(shapeData_.background.visible, b, [&]() { invalidateLayers(); } );
}

const CQChartsColor &
CQChartsBubblePlot::
fillColor() const
{
  return shapeData_.background.color;
}

void
CQChartsBubblePlot::
setFillColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(shapeData_.background.color, c, [&]() { invalidateLayers(); } );
}

QColor
CQChartsBubblePlot::
interpFillColor(int i, int n) const
{
  return fillColor().interpColor(this, i, n);
}

double
CQChartsBubblePlot::
fillAlpha() const
{
  return shapeData_.background.alpha;
}

void
CQChartsBubblePlot::
setFillAlpha(double a)
{
  CQChartsUtil::testAndSet(shapeData_.background.alpha, a, [&]() { invalidateLayers(); } );
}

CQChartsBubblePlot::Pattern
CQChartsBubblePlot::
fillPattern() const
{
  return (Pattern) shapeData_.background.pattern;
}

void
CQChartsBubblePlot::
setFillPattern(Pattern pattern)
{
  if (pattern != (Pattern) shapeData_.background.pattern) {
    shapeData_.background.pattern = (CQChartsFillData::Pattern) pattern;

    invalidateLayers();
  }
}

//------

void
CQChartsBubblePlot::
setValueLabel(bool b)
{
  CQChartsUtil::testAndSet(valueLabel_, b, [&]() { invalidateLayers(); } );
}

//---

bool
CQChartsBubblePlot::
isBorder() const
{
  return shapeData_.border.visible;
}

void
CQChartsBubblePlot::
setBorder(bool b)
{
  CQChartsUtil::testAndSet(shapeData_.border.visible, b, [&]() { invalidateLayers(); } );
}

const CQChartsColor &
CQChartsBubblePlot::
borderColor() const
{
  return shapeData_.border.color;
}

void
CQChartsBubblePlot::
setBorderColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(shapeData_.border.color, c, [&]() { invalidateLayers(); } );
}

QColor
CQChartsBubblePlot::
interpBorderColor(int i, int n) const
{
  return borderColor().interpColor(this, i, n);
}

double
CQChartsBubblePlot::
borderAlpha() const
{
  return shapeData_.border.alpha;
}

void
CQChartsBubblePlot::
setBorderAlpha(double a)
{
  CQChartsUtil::testAndSet(shapeData_.border.alpha, a, [&]() { invalidateLayers(); } );
}

const CQChartsLength &
CQChartsBubblePlot::
borderWidth() const
{
  return shapeData_.border.width;
}

void
CQChartsBubblePlot::
setBorderWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(shapeData_.border.width, l, [&]() { invalidateLayers(); } );
}

const CQChartsLineDash &
CQChartsBubblePlot::
borderDash() const
{
  return shapeData_.border.dash;
}

void
CQChartsBubblePlot::
setBorderDash(const CQChartsLineDash &d)
{
  CQChartsUtil::testAndSet(shapeData_.border.dash, d, [&]() { invalidateLayers(); } );
}

//---

const QFont &
CQChartsBubblePlot::
textFont() const
{
  return textData_.font;
}

void
CQChartsBubblePlot::
setTextFont(const QFont &f)
{
  CQChartsUtil::testAndSet(textData_.font, f, [&]() { invalidateLayers(); } );
}

void
CQChartsBubblePlot::
setTextFontSize(double s)
{
  if (s != textData_.font.pointSizeF()) {
    textData_.font.setPointSizeF(s);

    invalidateLayers();
  }
}

const CQChartsColor &
CQChartsBubblePlot::
textColor() const
{
  return textData_.color;
}

void
CQChartsBubblePlot::
setTextColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(textData_.color, c, [&]() { invalidateLayers(); } );
}

QColor
CQChartsBubblePlot::
interpTextColor(int i, int n) const
{
  return textColor().interpColor(this, i, n);
}

double
CQChartsBubblePlot::
textAlpha() const
{
  return textData_.alpha;
}

void
CQChartsBubblePlot::
setTextAlpha(double a)
{
  CQChartsUtil::testAndSet(textData_.alpha, a, [&]() { invalidateLayers(); } );
}

bool
CQChartsBubblePlot::
isTextContrast() const
{
  return textData_.contrast;
}

void
CQChartsBubblePlot::
setTextContrast(bool b)
{
  CQChartsUtil::testAndSet(textData_.contrast, b, [&]() { invalidateLayers(); } );
}

bool
CQChartsBubblePlot::
isTextScaled() const
{
  return textData_.scaled;
}

void
CQChartsBubblePlot::
setTextScaled(bool b)
{
  CQChartsUtil::testAndSet(textData_.scaled, b, [&]() { invalidateLayers(); } );
}

//---

void
CQChartsBubblePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  // columns
  addProperty("columns", this, "nameColumn" , "name" );
  addProperty("columns", this, "valueColumn", "value");
  addProperty("columns", this, "colorColumn", "color");

  CQChartsGroupPlot::addProperties();

  // options
  addProperty("options", this, "valueLabel");

  // stroke
  addProperty("stroke", this, "border", "visible");

  addLineProperties("stroke", "border");

  // fill
  addProperty("fill", this, "filled", "visible");

  addFillProperties("fill", "fill");

  // text
  addProperty("text", this, "textFont"    , "font"    );
  addProperty("text", this, "textColor"   , "color"   );
  addProperty("text", this, "textAlpha"   , "alpha"   );
  addProperty("text", this, "textContrast", "contrast");

  // color
  addProperty("color", this, "colorMapped", "mapped");
  addProperty("color", this, "colorMapMin", "mapMin");
  addProperty("color", this, "colorMapMax", "mapMax");
}

//---

CQChartsBubbleHierNode *
CQChartsBubblePlot::
currentRoot() const
{
  return root_;
}

//---

void
CQChartsBubblePlot::
updateRange(bool apply)
{
  double r = 1.0;

  dataRange_.reset();

  dataRange_.updateRange(-r, -r);
  dataRange_.updateRange( r,  r);

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange_.equalScale(aspect);
  }

  //---

  initGroupData(Columns(), nameColumn(), /*hier*/true);

  //---

  if (apply)
    applyDataRange();
}

//------

void
CQChartsBubblePlot::
updateObjs()
{
  clearValueSets();

  resetNodes();

  CQChartsPlot::updateObjs();
}

bool
CQChartsBubblePlot::
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

  // init value sets
  initValueSets();

  //---

  if (! root_)
    initNodes();

  //---

  initColorIds();

  colorNodes(root_);

  //---

  initNodeObjs(currentRoot(), nullptr, 0);

  //---

  return true;
}

void
CQChartsBubblePlot::
initNodeObjs(CQChartsBubbleHierNode *hier, CQChartsBubbleHierObj *parentObj, int depth)
{
  CQChartsBubbleHierObj *hierObj = 0;

  if (hier != root_) {
    double r = hier->radius();

    CQChartsGeom::BBox rect(hier->x() - r, hier->y() - r, hier->x() + r, hier->y() + r);

    hierObj = new CQChartsBubbleHierObj(this, hier, parentObj, rect, hier->depth(), maxDepth());

    addPlotObject(hierObj);
  }

  //---

  for (auto &hierNode : hier->getChildren()) {
    initNodeObjs(hierNode, hierObj, depth + 1);
  }

  //---

  for (auto &node : hier->getNodes()) {
    if (! node->placed()) continue;

    //---

    double r = node->radius();

    CQChartsGeom::BBox rect(node->x() - r, node->y() - r, node->x() + r, node->y() + r);

    CQChartsBubbleObj *obj =
      new CQChartsBubbleObj(this, node, parentObj, rect, node->depth(), maxDepth());

    addPlotObject(obj);
  }
}

void
CQChartsBubblePlot::
resetNodes()
{
  delete root_;

  root_ = nullptr;

  groupHierNodes_.clear();
}

void
CQChartsBubblePlot::
initNodes()
{
  hierInd_ = 0;

  root_ = new CQChartsBubbleHierNode(this, 0, "<root>");

  root_->setDepth(0);
  root_->setHierInd(hierInd_++);

  //---

  loadModel();

  //---

  replaceNodes();
}

void
CQChartsBubblePlot::
replaceNodes()
{
  placeNodes(root_);
}

void
CQChartsBubblePlot::
placeNodes(CQChartsBubbleHierNode *hier)
{
  initNodes(hier);

  //---

  hier->packNodes();

  offset_ = CQChartsGeom::Point(hier->x(), hier->y());
  scale_  = (hier->radius() > 0.0 ? 1.0/hier->radius() : 1.0);

  //---

  hier->setX((hier->x() - offset_.x)*scale_);
  hier->setY((hier->y() - offset_.y)*scale_);

  hier->setRadius(1.0);

  transformNodes(hier);
}

void
CQChartsBubblePlot::
initNodes(CQChartsBubbleHierNode *hier)
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
transformNodes(CQChartsBubbleHierNode *hier)
{
  for (auto &hierNode : hier->getChildren()) {
    hierNode->setX((hierNode->x() - offset_.x)*scale_);
    hierNode->setY((hierNode->y() - offset_.y)*scale_);

    hierNode->setRadius(hierNode->radius()*scale_);

    transformNodes(hierNode);
  }

   //---

  for (auto &node : hier->getNodes()) {
    node->setX((node->x() - offset_.x)*scale_);
    node->setY((node->y() - offset_.y)*scale_);

    node->setRadius(node->radius()*scale_);
  }
}

void
CQChartsBubblePlot::
colorNodes(CQChartsBubbleHierNode *hier)
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
colorNode(CQChartsBubbleNode *node)
{
  if (! node->color().isValid())
    node->setColorId(nextColorId());
}

void
CQChartsBubblePlot::
loadModel()
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsBubblePlot *plot) :
     plot_(plot) {
      valueColumnType_ = plot_->columnValueType(plot_->valueColumn());
    }

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      CQChartsModelIndex ind(row, plot_->valueColumn(), parent);

      std::vector<int> groupInds = plot_->rowHierGroupInds(ind);

      CQChartsBubbleHierNode *hierNode = plot_->currentRoot();

      for (std::size_t i = 0; i < groupInds.size(); ++i) {
        int groupInd = groupInds[i];

        hierNode = plot_->groupHierNode(hierNode, groupInd);
      }

      //---

      bool ok1;

      QString name = plot_->modelString(row, plot_->nameColumn(), parent, ok1);

      //---

      double size = 1.0;

      if (! getSize(parent, row, size))
        return State::SKIP;

      //---

      QModelIndex nameInd  = plot_->modelIndex(row, plot_->nameColumn(), parent);
      QModelIndex nameInd1 = plot_->normalizeIndex(nameInd);

      CQChartsBubbleNode *node = plot_->addNode(hierNode, name, size, nameInd1);

      if (node) {
        CQChartsColor color;

        if (plot_->colorSetColor("color", row, color))
          node->setColor(color);
      }

      return State::OK;
    }

   private:
    bool getSize(const QModelIndex &parent, int row, double &size) const {
      size = 1.0;

      if (! plot_->valueColumn().isValid())
        return true;

      bool ok = true;

      if      (valueColumnType_ == ColumnType::REAL)
        size = plot_->modelReal(row, plot_->valueColumn(), parent, ok);
      else if (valueColumnType_ == ColumnType::INTEGER)
        size = plot_->modelInteger(row, plot_->valueColumn(), parent, ok);
      else if (valueColumnType_ == ColumnType::STRING)
        size = 1.0;
      else
        ok = false;

      if (ok && size <= 0.0)
        ok = false;

      return ok;
    }

   private:
    CQChartsBubblePlot *plot_            { nullptr };
    ColumnType          valueColumnType_ { ColumnType::NONE };
  };

  RowVisitor visitor(this);

  visitModel(visitor);
}

CQChartsBubbleHierNode *
CQChartsBubblePlot::
groupHierNode(CQChartsBubbleHierNode *parent, int groupInd)
{
  if (groupInd < 0)
    return parent;

  auto p = groupHierNodes_.find(groupInd);

  if (p == groupHierNodes_.end()) {
    QString name = groupIndName(groupInd, /*hier*/true);

    QModelIndex ind;

    CQChartsBubbleHierNode *hierNode = addHierNode(parent, name, ind);

    p = groupHierNodes_.insert(p, GroupHierNodes::value_type(groupInd, hierNode));
  }

  return (*p).second;
}

CQChartsBubbleHierNode *
CQChartsBubblePlot::
addHierNode(CQChartsBubbleHierNode *hier, const QString &name, const QModelIndex &nameInd)
{
  int depth1 = hier->depth() + 1;

  QModelIndex nameInd1 = normalizeIndex(nameInd);

  CQChartsBubbleHierNode *hier1 = new CQChartsBubbleHierNode(this, hier, name, nameInd1);

  hier1->setDepth(depth1);

  hier1->setHierInd(hierInd_++);

  maxDepth_ = std::max(maxDepth_, depth1);

  return hier1;
}

CQChartsBubbleNode *
CQChartsBubblePlot::
addNode(CQChartsBubbleHierNode *hier, const QString &name, double size,
        const QModelIndex &nameInd)
{
  int depth1 = hier->depth() + 1;

  QModelIndex nameInd1 = normalizeIndex(nameInd);

  CQChartsBubbleNode *node = new CQChartsBubbleNode(this, hier, name, size, nameInd1);

  node->setDepth(depth1);

  hier->addNode(node);

  maxDepth_ = std::max(maxDepth_, depth1);

  return node;
}

//------

void
CQChartsBubblePlot::
handleResize()
{
  CQChartsPlot::handleResize();

  dataRange_.reset();
}

void
CQChartsBubblePlot::
drawForeground(QPainter *painter)
{
  drawBounds(painter, currentRoot());

  //---

  CQChartsPlot::drawForeground(painter);
}

void
CQChartsBubblePlot::
drawBounds(QPainter *painter, CQChartsBubbleHierNode *hier)
{
  double xc = hier->x();
  double yc = hier->y();
  double r  = hier->radius();

  //---

  double px1, py1, px2, py2;

  windowToPixel(xc - r, yc + r, px1, py1);
  windowToPixel(xc + r, yc - r, px2, py2);

  QRectF qrect(px1, py1, px2 - px1, py2 - py1);

  //---

  // draw bubble
  QColor bc = interpBorderColor(0, 1);

  painter->setPen  (bc);
  painter->setBrush(Qt::NoBrush);

  QPainterPath path;

  path.addEllipse(qrect);

  painter->drawPath(path);
}

//------

CQChartsBubbleHierObj::
CQChartsBubbleHierObj(CQChartsBubblePlot *plot, CQChartsBubbleHierNode *hier,
                      CQChartsBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                      int i, int n) :
 CQChartsBubbleObj(plot, hier, hierObj, rect, i, n), hier_(hier)
{
}

QString
CQChartsBubbleHierObj::
calcId() const
{
  //return QString("%1:%2").arg(hier_->name()).arg(hier_->hierSize());
  return CQChartsBubbleObj::calcId();
}

QString
CQChartsBubbleHierObj::
calcTipId() const
{
  //return QString("%1:%2").arg(hier_->hierName()).arg(hier_->hierSize());
  return CQChartsBubbleObj::calcTipId();
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

  double px1, py1, px2, py2;

  plot_->windowToPixel(hier_->x() - r, hier_->y() + r, px1, py1);
  plot_->windowToPixel(hier_->x() + r, hier_->y() - r, px2, py2);

  QRectF qrect = CQChartsUtil::toQRect(CQChartsGeom::BBox(px1, py2, px2, py1));

  //---

  // calc stroke and brush
  QBrush brush;

  if (plot_->isFilled()) {
    QColor c = hier_->interpColor(plot_, plot_->numColorIds());

    c.setAlphaF(plot_->fillAlpha());

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(c);

    brush.setStyle(CQChartsFillPattern::toStyle(
     (CQChartsFillPattern::Type) plot_->fillPattern()));
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen pen;

  if (plot_->isBorder()) {
    QColor bc = plot_->interpBorderColor(0, 1);

    bc.setAlphaF(plot_->borderAlpha());

    double bw = plot_->lengthPixelWidth(plot_->borderWidth());

    pen.setColor (bc);
    pen.setWidthF(bw);
  }
  else {
    pen.setStyle(Qt::NoPen);
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  // draw bubble
  painter->setPen  (pen);
  painter->setBrush(brush);

  QPainterPath path;

  path.addEllipse(qrect);

  painter->drawPath(path);
}

//------

CQChartsBubbleObj::
CQChartsBubbleObj(CQChartsBubblePlot *plot, CQChartsBubbleNode *node,
                  CQChartsBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                  int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), node_(node), hierObj_(hierObj), i_(i), n_(n)
{
}

QString
CQChartsBubbleObj::
calcId() const
{
  if (node_->isFiller())
    return hierObj_->calcId();

  return QString("bubble::%1:%2").arg(node_->name()).arg(node_->hierSize());
}

QString
CQChartsBubbleObj::
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
CQChartsBubbleObj::
inside(const CQChartsGeom::Point &p) const
{
  if (CQChartsUtil::PointPointDistance(p,
        CQChartsGeom::Point(node_->x(), node_->y())) < node_->radius())
    return true;

  return false;
}

void
CQChartsBubbleObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->nameColumn ());
  addColumnSelectIndex(inds, plot_->valueColumn());
  addColumnSelectIndex(inds, plot_->colorColumn());
}

void
CQChartsBubbleObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    const QModelIndex &ind = node_->ind();

    addSelectIndex(inds, ind.row(), column, ind.parent());
  }
}

void
CQChartsBubbleObj::
draw(QPainter *painter)
{
  double r = node_->radius();

  double px1, py1, px2, py2;

  plot_->windowToPixel(node_->x() - r, node_->y() + r, px1, py1);
  plot_->windowToPixel(node_->x() + r, node_->y() - r, px2, py2);

  QRectF qrect = CQChartsUtil::toQRect(CQChartsGeom::BBox(px1, py2, px2, py1));

  //---

  // calc stroke and brush
  QBrush brush;

  if (plot_->isFilled()) {
    QColor c = node_->interpColor(plot_, plot_->numColorIds());

    c.setAlphaF(plot_->fillAlpha());

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(c);

    brush.setStyle(CQChartsFillPattern::toStyle(
      (CQChartsFillPattern::Type) plot_->fillPattern()));
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen pen;

  if (plot_->isBorder()) {
    QColor bc = plot_->interpBorderColor(0, 1);

    bc.setAlphaF(plot_->borderAlpha());

    double bw = plot_->lengthPixelWidth(plot_->borderWidth());

    pen.setColor (bc);
    pen.setWidthF(bw);
  }
  else {
    pen.setStyle(Qt::NoPen);
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  QColor tc = plot_->interpTextColor(0, 1);

  tc.setAlphaF(plot_->textAlpha());

  QPen tpen(tc);

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
  QFont font = plot_->textFont();

  painter->setFont(font);

  if (plot_->isTextScaled()) {
    QFontMetricsF fm(painter->font());

    double tw = 0;

    for (int i = 0; i < strs.size(); ++i)
      tw = std::max(tw, fm.width(strs[i]));

    double th = strs.size()*fm.height();

    double sx = (tw > 0 ? qrect.width ()/tw : 1.0);
    double sy = (th > 0 ? qrect.height()/th : 1.0);

    double s = std::min(sx, sy);

    double fs = painter->font().pointSizeF()*s;

    QFont font1 = painter->font();

    font1.setPointSizeF(fs);

    painter->setFont(font1);
  }

  //---

  // calc text size and position
  plot_->windowToPixel(node_->x(), node_->y(), px1, py1);

  //---

  // draw label
  painter->setClipRect(qrect);

  CQChartsTextOptions textOptions;

  textOptions.contrast = plot_->isTextContrast();

  if      (strs.size() == 1)
    plot_->drawTextAtPoint(painter, QPointF(px1, py1), name, tpen, textOptions);
  else if (strs.size() == 2) {
    QFontMetricsF fm(painter->font());

    double th = fm.height();

    plot_->drawTextAtPoint(painter, QPointF(px1, py1 - th/2), strs[0], tpen, textOptions);
    plot_->drawTextAtPoint(painter, QPointF(px1, py1 + th/2), strs[1], tpen, textOptions);
  }
  else {
    assert(false);
  }

  //---

  painter->restore();
}

//------

CQChartsBubbleHierNode::
CQChartsBubbleHierNode(CQChartsBubblePlot *plot, CQChartsBubbleHierNode *parent,
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
interpColor(CQChartsBubblePlot *plot, int n) const
{
  using Colors = std::vector<QColor>;

  Colors colors;

  for (auto &child : children_)
    colors.push_back(child->interpColor(plot, n));

  for (auto &node : nodes_)
    colors.push_back(node->interpColor(plot, n));

  if (colors.empty())
    return plot->interpPaletteColor(0, 1);

  return CQChartsUtil::blendColors(colors);
}

//------

CQChartsBubbleNode::
CQChartsBubbleNode(CQChartsBubblePlot *plot, CQChartsBubbleHierNode *parent,
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
interpColor(CQChartsBubblePlot *plot, int n) const
{
  if      (colorId() >= 0)
    return plot->interpPaletteColor(colorId(), n);
  else if (color().isValid())
    return color().interpColor(plot, 0, 1);
  else
    return plot->interpPaletteColor(0, 1);
}
