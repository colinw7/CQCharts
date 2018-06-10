#include <CQChartsBubblePlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTextBoxObj.h>

#include <QPainter>

CQChartsBubblePlotType::
CQChartsBubblePlotType()
{
}

void
CQChartsBubblePlotType::
addParameters()
{
  addColumnParameter("name" , "Name" , "nameColumn" , 0).setRequired();
  addColumnParameter("value", "Value", "valueColumn");
  addColumnParameter("color", "Color", "colorColumn");

  CQChartsPlotType::addParameters();
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
 CQChartsPlot(view, view->charts()->plotType("bubble"), model)
{
  (void) addColorSet("color");

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
  for (auto &node : nodes_)
    delete node;
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
  CQChartsUtil::testAndSet(shapeData_.background.visible, b, [&]() { update(); } );
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
  CQChartsUtil::testAndSet(shapeData_.background.color, c, [&]() { update(); } );
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
  CQChartsUtil::testAndSet(shapeData_.background.alpha, a, [&]() { update(); } );
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

    update();
  }
}

//------

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
  CQChartsUtil::testAndSet(shapeData_.border.visible, b, [&]() { update(); } );
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
  CQChartsUtil::testAndSet(shapeData_.border.color, c, [&]() { update(); } );
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
  CQChartsUtil::testAndSet(shapeData_.border.alpha, a, [&]() { update(); } );
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
  CQChartsUtil::testAndSet(shapeData_.border.width, l, [&]() { update(); } );
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
  CQChartsUtil::testAndSet(textData_.font, f, [&]() { update(); } );
}

void
CQChartsBubblePlot::
setTextFontSize(double s)
{
  if (s != textData_.font.pointSizeF()) {
    textData_.font.setPointSizeF(s);

    update();
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
  CQChartsUtil::testAndSet(textData_.color, c, [&]() { update(); } );
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
  CQChartsUtil::testAndSet(textData_.alpha, a, [&]() { update(); } );
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
  CQChartsUtil::testAndSet(textData_.contrast, b, [&]() { update(); } );
}

//---

void
CQChartsBubblePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nameColumn" , "name" );
  addProperty("columns", this, "valueColumn", "value");
  addProperty("columns", this, "colorColumn", "color");

  addProperty("stroke", this, "border"     , "visible");
  addProperty("stroke", this, "borderColor", "color"  );
  addProperty("stroke", this, "borderAlpha", "alpha"  );
  addProperty("stroke", this, "borderWidth", "width"  );

  addProperty("fill", this, "filled"     , "visible");
  addProperty("fill", this, "fillColor"  , "color"  );
  addProperty("fill", this, "fillAlpha"  , "alpha"  );
  addProperty("fill", this, "fillPattern", "pattern");

  addProperty("text", this, "textFont"    , "font"    );
  addProperty("text", this, "textColor"   , "color"   );
  addProperty("text", this, "textAlpha"   , "alpha"   );
  addProperty("text", this, "textContrast", "contrast");

  addProperty("color", this, "colorMapped", "mapped");
  addProperty("color", this, "colorMapMin", "mapMin");
  addProperty("color", this, "colorMapMax", "mapMax");
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

  if (nodes_.empty())
    initNodes();

  //---

  initColorIds();

  for (const auto &node : nodes_)
    colorNode(node);

  //---

  initNodeObjs();

  //---

  return true;
}

void
CQChartsBubblePlot::
initNodeObjs()
{
  int i = 0;
  int n = nodes_.size();

  for (auto node : nodes_) {
    if (! node->placed()) continue;

    //---

    double r = node->radius();

    CQChartsGeom::BBox rect(node->x() - r, node->y() - r, node->x() + r, node->y() + r);

    CQChartsBubbleObj *obj = new CQChartsBubbleObj(this, node, rect, i, n);

    addPlotObject(obj);

    ++i;
  }
}

void
CQChartsBubblePlot::
resetNodes()
{
  for (auto &node : nodes_)
    delete node;

  nodes_.clear();
}

void
CQChartsBubblePlot::
initNodes()
{
  loadModel();

  //---

  placeNodes();
}

void
CQChartsBubblePlot::
placeNodes()
{
  pack_.reset();

  for (auto node : nodes_)
    pack_.addNode(node);

  //---

  double xc, yc, r;

  if (pack_.boundingCircle(xc, yc, r)) {
    offset_ = CQChartsGeom::Point(xc, yc);
    scale_  = (r > 0.0 ? 1.0/r : 1.0);
  }
  else {
    offset_ = CQChartsGeom::Point(0.0, 0.0);
    scale_  = 1.0;
  }

  //---

  for (auto node : nodes_) {
    node->setX((node->x() - offset_.x)*scale_);
    node->setY((node->y() - offset_.y)*scale_);

    node->setRadius(node->radius()*scale_);
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
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsBubblePlot *plot) :
     plot_(plot) {
      valueColumnType_ = plot_->columnValueType(plot_->valueColumn());
    }

    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      bool ok1;

      QString name = plot_->modelString(model, row, plot_->nameColumn(), parent, ok1);

      //---

      double size = 1.0;

      if (plot_->valueColumn().isValid()) {
        bool ok2 = true;

        if      (valueColumnType_ == ColumnType::REAL)
          size = plot_->modelReal(model, row, plot_->valueColumn(), parent, ok2);
        else if (valueColumnType_ == ColumnType::INTEGER)
          size = plot_->modelInteger(model, row, plot_->valueColumn(), parent, ok2);
        else
          ok2 = false;

        if (ok2 && size <= 0.0)
          ok2 = false;

        if (! ok2)
          return State::SKIP;
      }

      //---

      QModelIndex nameInd  = model->index(row, plot_->nameColumn().column(), parent);
      QModelIndex nameInd1 = plot_->normalizeIndex(nameInd);

      CQChartsBubbleNode *node = plot_->addNode(name, size, nameInd1);

      OptColor color;

      if (plot_->colorSetColor("color", row, color))
        node->setColor(*color);

      return State::OK;
    }

   private:
    CQChartsBubblePlot *plot_            { nullptr };
    ColumnType          valueColumnType_ { ColumnType::NONE };
  };

  RowVisitor visitor(this);

  visitModel(visitor);
}

CQChartsBubbleNode *
CQChartsBubblePlot::
addNode(const QString &name, double size, const QModelIndex &nameInd)
{
  CQChartsBubbleNode *node = new CQChartsBubbleNode(name, size, nameInd);

  nodes_.push_back(node);

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
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
}

void
CQChartsBubblePlot::
drawForeground(QPainter *painter)
{
  double xc = 0.0, yc = 0.0, r = 1.0;

  pack_.boundingCircle(xc, yc, r);

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

  //---

  CQChartsPlot::drawForeground(painter);
}

//------

CQChartsBubbleObj::
CQChartsBubbleObj(CQChartsBubblePlot *plot, CQChartsBubbleNode *node,
                  const CQChartsGeom::BBox &rect, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), node_(node), i_(i), n_(n)
{
}

QString
CQChartsBubbleObj::
calcId() const
{
  return QString("%1:%2").arg(node_->name()).arg(node_->size());
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
  const QModelIndex &ind = node_->ind();

  addSelectIndex(inds, ind.row(), plot_->nameColumn(), ind.parent());

  if (plot_->valueColumn().isValid())
    addSelectIndex(inds, ind.row(), plot_->valueColumn(), ind.parent());

  if (plot_->colorColumn().isValid())
    addSelectIndex(inds, ind.row(), plot_->colorColumn(), ind.parent());

  if (plot_->idColumn().isValid())
    addSelectIndex(inds, ind.row(), plot_->idColumn(), ind.parent());

}

void
CQChartsBubbleObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
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

  // set font
  QFont font = plot_->textFont();

  painter->setFont(font);

  QFontMetricsF fm(painter->font());

  //---

  // calc text size and position
  const QString &name = node_->name();

  plot_->windowToPixel(node_->x(), node_->y(), px1, py1);

  //---

  // draw label
  painter->setClipRect(qrect);

  CQChartsTextOptions textOptions;

  textOptions.contrast = plot_->isTextContrast();

  plot_->drawTextAtPoint(painter, QPointF(px1, py1), name, tpen, textOptions);

  //---

  painter->restore();
}

//------

CQChartsBubbleNode::
CQChartsBubbleNode(const QString &name, double size, const QModelIndex &ind) :
 id_(nextId()), name_(name), size_(size), ind_(ind)
{
  initRadius();
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
