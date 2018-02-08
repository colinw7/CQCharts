#include <CQChartsBubblePlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsFillObj.h>

#include <QPainter>

CQChartsBubblePlotType::
CQChartsBubblePlotType()
{
}

void
CQChartsBubblePlotType::
addParameters()
{
  addColumnParameter("name" , "Name" , "nameColumn" , "", 0);
  addColumnParameter("value", "Value", "valueColumn", "optional");
  addColumnParameter("color", "Color", "colorColumn", "optional");
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

  textBoxObj_ = new CQChartsTextBoxObj(this);

  textBoxObj_->setBackgroundColor(CQChartsPaletteColor(CQChartsPaletteColor::Type::PALETTE));

  setBorder(true);
  setFilled(true);

  setTextContrast(true);

  textBoxObj_->setTextFontSize(12.0);

  CQChartsPaletteColor textColor(CQChartsPaletteColor::Type::THEME_VALUE, 1);

  textBoxObj_->setTextColor(textColor);

  setMargins(1, 1, 1, 1);

  addTitle();
}

CQChartsBubblePlot::
~CQChartsBubblePlot()
{
  delete textBoxObj_;

  for (auto &node : nodes_)
    delete node;
}

//------

void
CQChartsBubblePlot::
setNameColumn(int i)
{
  if (i != nameColumn_) {
    nameColumn_ = i;

    updateRangeAndObjs();
  }
}

void
CQChartsBubblePlot::
setValueColumn(int i)
{
  if (i != valueColumn_) {
    valueColumn_ = i;

    updateRangeAndObjs();
  }
}

//----

bool
CQChartsBubblePlot::
isFilled() const
{
  return textBoxObj_->isBackground();
}

void
CQChartsBubblePlot::
setFilled(bool b)
{
  textBoxObj_->setBackground(b);

  update();
}

QString
CQChartsBubblePlot::
fillColorStr() const
{
  return textBoxObj_->backgroundColorStr();
}

void
CQChartsBubblePlot::
setFillColorStr(const QString &s)
{
  textBoxObj_->setBackgroundColorStr(s);

  update();
}

QColor
CQChartsBubblePlot::
interpFillColor(int i, int n) const
{
  return textBoxObj_->interpBackgroundColor(i, n);
}

double
CQChartsBubblePlot::
fillAlpha() const
{
  return textBoxObj_->backgroundAlpha();
}

void
CQChartsBubblePlot::
setFillAlpha(double a)
{
  textBoxObj_->setBackgroundAlpha(a);

  update();
}

CQChartsBubblePlot::Pattern
CQChartsBubblePlot::
fillPattern() const
{
  return (Pattern) textBoxObj_->backgroundPattern();
}

void
CQChartsBubblePlot::
setFillPattern(Pattern pattern)
{
  textBoxObj_->setBackgroundPattern((CQChartsBoxObj::Pattern) pattern);

  update();
}

//---

bool
CQChartsBubblePlot::
isBorder() const
{
  return textBoxObj_->isBorder();
}

void
CQChartsBubblePlot::
setBorder(bool b)
{
  textBoxObj_->setBorder(b);

  update();
}

QString
CQChartsBubblePlot::
borderColorStr() const
{
  return textBoxObj_->borderColorStr();
}

void
CQChartsBubblePlot::
setBorderColorStr(const QString &str)
{
  textBoxObj_->setBorderColorStr(str);

  update();
}

QColor
CQChartsBubblePlot::
interpBorderColor(int i, int n) const
{
  return textBoxObj_->interpBorderColor(i, n);
}

double
CQChartsBubblePlot::
borderAlpha() const
{
  return textBoxObj_->borderAlpha();
}

void
CQChartsBubblePlot::
setBorderAlpha(double a)
{
  textBoxObj_->setBorderAlpha(a);

  update();
}

double
CQChartsBubblePlot::
borderWidth() const
{
  return textBoxObj_->borderWidth();
}

void
CQChartsBubblePlot::
setBorderWidth(double r)
{
  textBoxObj_->setBorderWidth(r);

  update();
}

//---

const QFont &
CQChartsBubblePlot::
textFont() const
{
  return textBoxObj_->textFont();
}

void
CQChartsBubblePlot::
setTextFont(const QFont &f)
{
  textBoxObj_->setTextFont(f);

  update();
}

QString
CQChartsBubblePlot::
textColorStr() const
{
  return textBoxObj_->textColorStr();
}

void
CQChartsBubblePlot::
setTextColorStr(const QString &s)
{
  textBoxObj_->setTextColorStr(s);

  update();
}

QColor
CQChartsBubblePlot::
interpTextColor(int i, int n) const
{
  return textBoxObj_->interpTextColor(i, n);
}

bool
CQChartsBubblePlot::
isTextContrast() const
{
  return textBoxObj_->isTextContrast();
}

void
CQChartsBubblePlot::
setTextContrast(bool b)
{
  textBoxObj_->setTextContrast(b);

  update();
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
  addProperty("text", this, "textContrast", "contrast");

  addProperty("color", this, "colorMapEnabled", "mapEnabled");
  addProperty("color", this, "colorMapMin"    , "mapMin"    );
  addProperty("color", this, "colorMapMax"    , "mapMax"    );
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
      QAbstractItemModel *model = plot_->model();
      assert(model);

      valueColumnType_ = plot_->columnValueType(model, plot_->valueColumn());
    }

    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      QModelIndex nameInd = model->index(row, plot_->nameColumn(), parent);

      QModelIndex nameInd1 = plot_->normalizeIndex(nameInd);

      bool ok1;

      QString name = CQChartsUtil::modelString(model, nameInd, ok1);

      //---

      double size = 1.0;

      QModelIndex valueInd = model->index(row, plot_->valueColumn(), parent);

      if (valueInd.isValid()) {
        bool ok2 = true;

        if      (valueColumnType_ == ColumnType::REAL)
          size = CQChartsUtil::modelReal(model, valueInd, ok2);
        else if (valueColumnType_ == ColumnType::INTEGER)
          size = CQChartsUtil::modelInteger(model, valueInd, ok2);
        else
          ok2 = false;

        if (ok2 && size <= 0.0)
          ok2 = false;

        if (! ok2)
          return State::SKIP;
      }

      //---

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
addSelectIndex()
{
  const QModelIndex &ind = node_->ind();

  QModelIndex nameInd  = plot_->selectIndex(ind.row(), plot_->nameColumn (), ind.parent());
  QModelIndex valueInd = plot_->selectIndex(ind.row(), plot_->valueColumn(), ind.parent());
  QModelIndex colorInd = plot_->selectIndex(ind.row(), plot_->colorColumn(), ind.parent());

  plot_->addSelectIndex(nameInd);

  if (valueInd.isValid())
    plot_->addSelectIndex(valueInd);

  if (colorInd.isValid())
    plot_->addSelectIndex(colorInd);
}

bool
CQChartsBubbleObj::
isIndex(const QModelIndex &ind) const
{
  const QModelIndex &nind = node_->ind();

  return (ind == nind);
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
    brush.setStyle(CQChartsFillObj::patternToStyle(
      (CQChartsFillObj::Pattern) plot_->fillPattern()));
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen pen;

  if (plot_->isBorder()) {
    QColor bc = plot_->interpBorderColor(0, 1);

    bc.setAlphaF(plot_->borderAlpha());

    pen.setColor (bc);
    pen.setWidthF(plot_->borderWidth());
  }
  else {
    pen.setStyle(Qt::NoPen);
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  QColor tc = plot_->interpTextColor(0, 1);

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

  double tw = fm.width(name);

  //double fdy = (fm.ascent() - fm.descent())/2;
  double fdy = fm.descent();

  //---

  // draw label
  painter->setClipRect(qrect);

  if (plot_->isTextContrast())
    plot_->drawContrastText(painter, px1 - tw/2, py1 + fdy, name, tpen);
  else {
    painter->setPen(tpen);

    painter->drawText(px1 - tw/2, py1 + fdy, name);
  }

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
