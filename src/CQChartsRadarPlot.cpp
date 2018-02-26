#include <CQChartsRadarPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsTextBoxObj.h>
#include <QPainter>

CQChartsRadarPlotType::
CQChartsRadarPlotType()
{
}

void
CQChartsRadarPlotType::
addParameters()
{
  addColumnParameter ("name" , "Name" , "nameColumn"  , "optional");
  addColumnsParameter("value", "Value", "valueColumns", "", "1");
}

CQChartsPlot *
CQChartsRadarPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsRadarPlot(view, model);
}

//------

CQChartsRadarPlot::
CQChartsRadarPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("radar"), model)
{
  gridData_.color = CQChartsColor(CQChartsColor::Type::THEME_VALUE, 0.5);

  setFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));
  setFillAlpha(0.5);

  setBorder(true);
  setFilled(true);

  setTextColor(CQChartsColor(CQChartsColor::Type::THEME_VALUE, 1));

  addKey();

  addTitle();
}

CQChartsRadarPlot::
~CQChartsRadarPlot()
{
}

//------

QString
CQChartsRadarPlot::
valueColumnsStr() const
{
  return CQChartsColumn::columnsToString(valueColumns());
}

bool
CQChartsRadarPlot::
setValueColumnsStr(const QString &s)
{
  Columns valueColumns;

  if (! CQChartsColumn::stringToColumns(s, valueColumns))
    return false;

  setValueColumns(valueColumns);

  return true;
}

//------

bool
CQChartsRadarPlot::
isFilled() const
{
  return shapeData_.background.visible;
}

void
CQChartsRadarPlot::
setFilled(bool b)
{
  CQChartsUtil::testAndSet(shapeData_.background.visible, b, [&]() { update(); } );
}

const CQChartsColor &
CQChartsRadarPlot::
fillColor() const
{
  return shapeData_.background.color;
}

void
CQChartsRadarPlot::
setFillColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(shapeData_.background.color, c, [&]() { update(); } );
}

QColor
CQChartsRadarPlot::
interpFillColor(int i, int n)
{
  return fillColor().interpColor(this, i, n);
}

double
CQChartsRadarPlot::
fillAlpha() const
{
  return shapeData_.background.alpha;
}

void
CQChartsRadarPlot::
setFillAlpha(double a)
{
  CQChartsUtil::testAndSet(shapeData_.background.alpha, a, [&]() { update(); } );
}

CQChartsRadarPlot::Pattern
CQChartsRadarPlot::
fillPattern() const
{
  return (Pattern) shapeData_.background.pattern;
}

void
CQChartsRadarPlot::
setFillPattern(Pattern pattern)
{
  if (pattern != (Pattern) shapeData_.background.pattern) {
    shapeData_.background.pattern = (CQChartsFillData::Pattern) pattern;

    update();
  }
}

//------

bool
CQChartsRadarPlot::
isBorder() const
{
  return shapeData_.border.visible;
}

void
CQChartsRadarPlot::
setBorder(bool b)
{
  CQChartsUtil::testAndSet(shapeData_.border.visible, b, [&]() { update(); } );
}

const CQChartsColor &
CQChartsRadarPlot::
borderColor() const
{
  return shapeData_.border.color;
}

void
CQChartsRadarPlot::
setBorderColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(shapeData_.border.color, c, [&]() { update(); } );
}

QColor
CQChartsRadarPlot::
interpBorderColor(int i, int n) const
{
  return borderColor().interpColor(this, i, n);
}

double
CQChartsRadarPlot::
borderAlpha() const
{
  return shapeData_.border.alpha;
}

void
CQChartsRadarPlot::
setBorderAlpha(double a)
{
  CQChartsUtil::testAndSet(shapeData_.border.alpha, a, [&]() { update(); } );
}

const CQChartsLength &
CQChartsRadarPlot::
borderWidth() const
{
  return shapeData_.border.width;
}

void
CQChartsRadarPlot::
setBorderWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(shapeData_.border.width, l, [&]() { update(); } );
}

//---

const QFont &
CQChartsRadarPlot::
textFont() const
{
  return textData_.font;
}

void
CQChartsRadarPlot::
setTextFont(const QFont &f)
{
  CQChartsUtil::testAndSet(textData_.font, f, [&]() { update(); } );
}

const CQChartsColor &
CQChartsRadarPlot::
textColor() const
{
  return textData_.color;
}

void
CQChartsRadarPlot::
setTextColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(textData_.color, c, [&]() { update(); } );
}

QColor
CQChartsRadarPlot::
interpTextColor(int i, int n) const
{
  return textColor().interpColor(this, i, n);
}

double
CQChartsRadarPlot::
textAlpha() const
{
  return textData_.alpha;
}

void
CQChartsRadarPlot::
setTextAlpha(double a)
{
  CQChartsUtil::testAndSet(textData_.alpha, a, [&]() { update(); } );
}

//----

void
CQChartsRadarPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nameColumn"  , "name"     );
  addProperty("columns", this, "valueColumn" , "value"    );
  addProperty("columns", this, "valueColumns", "valuesSet");

  addProperty("", this, "angleStart" );
  addProperty("", this, "angleExtent");

  addProperty("grid", this, "gridColor", "color");
  addProperty("grid", this, "gridAlpha", "alpha");

  addProperty("stroke", this, "border"     , "visible");
  addProperty("stroke", this, "borderColor", "color"  );
  addProperty("stroke", this, "borderAlpha", "alpha"  );
  addProperty("stroke", this, "borderWidth", "width"  );

  addProperty("fill", this, "filled"     , "visible");
  addProperty("fill", this, "fillColor"  , "color"  );
  addProperty("fill", this, "fillAlpha"  , "alpha"  );
  addProperty("fill", this, "fillPattern", "pattern");

  addProperty("text", this, "textFont" , "font" );
  addProperty("text", this, "textColor", "color");
  addProperty("text", this, "textAlpha", "alpha");
}

void
CQChartsRadarPlot::
updateRange(bool apply)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsRadarPlot *plot) :
     plot_(plot) {
      nv_ = plot_->numValueColumns();
    }

    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      for (int iv = 0; iv < nv_; ++iv) {
        const CQChartsColumn &column = plot_->valueColumn(iv);

        bool ok;

        double value = CQChartsUtil::modelReal(model, row, column, parent, ok);

        valueDatas_[iv].add(value);
      }

      return State::OK;
    }

    const ValueDatas &valueDatas() const { return valueDatas_; }

   private:
    CQChartsRadarPlot *plot_ { nullptr };
    int                nv_   { 0 };
    ValueDatas         valueDatas_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  valueDatas_ = visitor.valueDatas();

  //---

  valueRadius_ = 0.0;

  int nv = numValueColumns();

  for (int iv = 0; iv < nv; ++iv)
    valueRadius_ = std::max(valueRadius_, valueDatas_[iv].max()/valueDatas_[iv].sum());

  //---

  double r = valueRadius_;

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

bool
CQChartsRadarPlot::
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

  // process model data
  class RadarPlotVisitor : public ModelVisitor {
   public:
    RadarPlotVisitor(CQChartsRadarPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *model, const QModelIndex &ind, int row) override {
      plot_->addRow(model, ind, row, numRows());

      return State::OK;
    }

   private:
    CQChartsRadarPlot *plot_ { nullptr };
  };

  RadarPlotVisitor radarPlotVisitor(this);

  visitModel(radarPlotVisitor);

  //---

  resetKeyItems();

  //---

  return true;
}

void
CQChartsRadarPlot::
addRow(QAbstractItemModel *model, const QModelIndex &parent, int row, int nr)
{
  bool hidden = isSetHidden(row);

  if (hidden)
    return;

  //---

  bool ok;

  QString name = CQChartsUtil::modelString(model, row, nameColumn(), parent, ok);

  //---

  int nv = valueColumns().size();

  double alen = std::min(std::max(angleExtent(), -360.0), 360.0);

  double da = (nv > 0 ? alen/nv : 0.0);

  //---

  QPolygonF poly;

  double a = angleStart();

  for (int iv = 0; iv < nv; ++iv) {
    bool ok1;

    double value = CQChartsUtil::modelReal(model, row, valueColumns()[iv], parent, ok1);

    double scale = valueDatas_[iv].sum();

    double ra = CQChartsUtil::Deg2Rad(a);

    double x = value*cos(ra)/scale;
    double y = value*sin(ra)/scale;

    poly << QPointF(x, y);

    a -= da;
  }

  //---

  QModelIndex nameInd  = model->index(row, nameColumn().column(), parent);
  QModelIndex nameInd1 = normalizeIndex(nameInd);

  CQChartsGeom::BBox bbox(-1, -1, 1, 1);

  CQChartsRadarObj *radarObj =
    new CQChartsRadarObj(this, bbox, name, poly, nameInd1, row, nr);

  addPlotObject(radarObj);
}

void
CQChartsRadarPlot::
addKeyItems(CQChartsPlotKey *key)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsRadarPlot *plot, CQChartsPlotKey *key) :
     plot_(plot), key_(key) {
    }

    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      bool ok;

      QString name = CQChartsUtil::modelString(model, row, plot_->nameColumn(), parent, ok);

      //---

      CQChartsKeyColorBox *color = new CQChartsKeyColorBox(plot_, row, numRows());
      CQChartsKeyText     *text  = new CQChartsKeyText(plot_, name);

      color->setClickHide(true);

      key_->addItem(color, row, 0);
      key_->addItem(text , row, 1);

      return State::OK;
    }

   private:
    CQChartsRadarPlot *plot_ { nullptr };
    CQChartsPlotKey   *key_  { nullptr };
  };

  RowVisitor visitor(this, key);

  visitModel(visitor);

  //---

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsRadarPlot::
handleResize()
{
  CQChartsPlot::handleResize();

  dataRange_.reset();
}

void
CQChartsRadarPlot::
drawBackground(QPainter *painter)
{
  CQChartsPlot::drawBackground(painter);

  //---

  int nv = valueColumns().size();

  if (! nv)
    return;

  double alen = std::min(std::max(angleExtent(), -360.0), 360.0);

  double da = alen/nv;

  //---

  // draw grid spokes
  QColor gridColor1 = interpGridColor(0, 1);

  gridColor1.setAlphaF(gridAlpha());

  QPen gpen1(gridColor1);

  painter->setPen(gpen1);

  double px1, py1;

  windowToPixel(0.0, 0.0, px1, py1);

  double a = angleStart();

  for (int iv = 0; iv < nv; ++iv) {
    double ra = CQChartsUtil::Deg2Rad(a);

    double x = valueRadius_*cos(ra);
    double y = valueRadius_*sin(ra);

    double px2, py2;

    windowToPixel(x, y, px2, py2);

    painter->drawLine(QPointF(px1, py1), QPointF(px2, py2));

    a -= da;
  }

  //---

  // draw grid polygons
  QColor gridColor2 = interpGridColor(0, 1);

  QPen gpen2(gridColor2);

  //---

  QColor tc = interpTextColor(0, 1);

  tc.setAlphaF(textAlpha());

  QPen tpen(tc);

  //---

  painter->setFont(textFont());

  int    nl = 5;
  double dr = valueRadius_/nl;

  for (int i = 0; i <= nl; ++i) {
    double r = dr*i;

    double a = angleStart();

    QPolygonF poly;

    for (int iv = 0; iv < nv; ++iv) {
      double ra = CQChartsUtil::Deg2Rad(a);

      double x = r*cos(ra);
      double y = r*sin(ra);

      double px, py;

      windowToPixel(x, y, px, py);

      poly << QPointF(px, py);

      //---

      if (i == nl) {
        painter->setPen(tpen);

        //---

        bool ok;

        QString name = CQChartsUtil::modelHeaderString(model(), valueColumns()[iv], ok);

        Qt::Alignment align = 0;

        if      (CQChartsUtil::isZero(x)) align |= Qt::AlignHCenter;
        else if (x > 0)                   align |= Qt::AlignLeft;
        else if (x < 0)                   align |= Qt::AlignRight;

        if      (CQChartsUtil::isZero(y)) align |= Qt::AlignVCenter;
        else if (y > 0)                   align |= Qt::AlignBottom;
        else if (y < 0)                   align |= Qt::AlignTop;

        CQChartsDrawUtil::drawAlignedText(painter, px, py, name, align, 2, 2);
      }

      //---

      a -= da;
    }

    poly << poly[0];

    painter->setPen(gpen2);

    painter->drawPolygon(poly);
  }
}

void
CQChartsRadarPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
}

//------

CQChartsRadarObj::
CQChartsRadarObj(CQChartsRadarPlot *plot, const CQChartsGeom::BBox &rect, const QString &name,
                 const QPolygonF &poly, const QModelIndex &ind, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), name_(name), poly_(poly), ind_(ind), i_(i), n_(n)
{
}

QString
CQChartsRadarObj::
calcId() const
{
  return QString("%1").arg(name_);
}

bool
CQChartsRadarObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  return poly_.containsPoint(CQChartsUtil::toQPoint(p), Qt::OddEvenFill);
}

void
CQChartsRadarObj::
addSelectIndex()
{
  plot_->addSelectIndex(ind_);
}

bool
CQChartsRadarObj::
isIndex(const QModelIndex &ind) const
{
  return (ind == ind_);
}

void
CQChartsRadarObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  if (! poly_.size())
    return;

  QPolygonF ppoly;

  for (int i = 0; i < poly_.size(); ++i) {
    double x = poly_[i].x();
    double y = poly_[i].y();

    double px, py;

    plot_->windowToPixel(x, y, px, py);

    ppoly << QPointF(px, py);
  }

  ppoly << ppoly[0];

  //---

  // calc stroke and brush
  QBrush brush;

  if (plot_->isFilled()) {
    QColor c = plot_->interpFillColor(i_, n_);

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

  painter->setPen  (pen);
  painter->setBrush(brush);

  painter->drawPolygon(ppoly);
}
