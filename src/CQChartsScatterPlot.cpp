#include <CQChartsScatterPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsValueSet.h>
#include <CQChartsUtil.h>
#include <CQChartsTip.h>
#include <CQCharts.h>
#include <QPainter>

CQChartsScatterPlotType::
CQChartsScatterPlotType()
{
}

void
CQChartsScatterPlotType::
addParameters()
{
  startParameterGroup("Scatter");

  addColumnParameter("x", "X", "xColumn", 0).setTip("X Value").setRequired();
  addColumnParameter("y", "Y", "yColumn", 1).setTip("Y Value").setRequired();

  addColumnParameter("name", "Name", "nameColumn").setTip("Value Name");

  endParameterGroup();

  //---

  startParameterGroup("Points");

  addColumnParameter("symbolSize", "Symbol Size", "symbolSizeColumn").
   setTip("Point Size").setMapped().setMapMinMax(8, 64);

  addColumnParameter("fontSize", "Font Size", "fontSizeColumn"  ).
   setTip("Custom Font Size for Text Label").setMapped().setMapMinMax(8, 48);

  addColumnParameter("color", "Color", "colorColumn").
   setTip("Custom Point Fill color").setMapped();

  addBoolParameter("textLabels", "Text Labels", "textLabels").
   setTip("Show Text Label at Point");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsScatterPlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draws scatter plot x, y points with support for customization of"
         "point size, color and label font.\n";
}

CQChartsPlot *
CQChartsScatterPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsScatterPlot(view, model);
}

//---

CQChartsScatterPlot::
CQChartsScatterPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("scatter"), model), dataLabel_(this)
{
  // set mapped range
  (void) addValueSet("symbolSize", 8, 64);
  (void) addValueSet("fontSize"  , 8, 48);
  (void) addColorSet("color");

  symbolData_.type           = CQChartsSymbol::Type::CIRCLE;
  symbolData_.stroke.visible = true;
  symbolData_.fill.visible   = true;
  symbolData_.fill.color     = CQChartsColor(CQChartsColor::Type::PALETTE);

  addAxes();

  addKey();

  addTitle();
}

//------

void
CQChartsScatterPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setXColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setYColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(yColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setSymbolSize(const CQChartsLength &s)
{
  CQChartsUtil::testAndSet(symbolData_.size, s, [&]() { updateObjs(); } );
}

void
CQChartsScatterPlot::
setSymbolStroked(bool b)
{
  CQChartsUtil::testAndSet(symbolData_.stroke.visible, b, [&]() { update(); } );
}

void
CQChartsScatterPlot::
setSymbolStrokeColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(symbolData_.stroke.color, c, [&]() { update(); } );
}

void
CQChartsScatterPlot::
setSymbolStrokeAlpha(double a)
{
  CQChartsUtil::testAndSet(symbolData_.stroke.alpha, a, [&]() { update(); } );
}

void
CQChartsScatterPlot::
setSymbolStrokeWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(symbolData_.stroke.width, l, [&]() { update(); } );
}

void
CQChartsScatterPlot::
setSymbolFilled(bool b)
{
  CQChartsUtil::testAndSet(symbolData_.fill.visible, b, [&]() { update(); } );
}

void
CQChartsScatterPlot::
setSymbolFillColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(symbolData_.fill.color, c, [&]() { update(); } );
}

void
CQChartsScatterPlot::
setSymbolFillAlpha(double a)
{
  CQChartsUtil::testAndSet(symbolData_.fill.alpha, a, [&]() { update(); } );
}

CQChartsScatterPlot::Pattern
CQChartsScatterPlot::
symbolFillPattern() const
{
  return (Pattern) symbolData_.fill.pattern;
}

void
CQChartsScatterPlot::
setSymbolFillPattern(const Pattern &pattern)
{
  if (pattern != (Pattern) symbolData_.fill.pattern) {
    symbolData_.fill.pattern = (CQChartsFillData::Pattern) pattern;

    update();
  }
}

//---

void
CQChartsScatterPlot::
setFontSize(double s)
{
  CQChartsUtil::testAndSet(fontSize_, s, [&]() { updateObjs(); } );
}

//------

void
CQChartsScatterPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nameColumn"      , "name"      );
  addProperty("columns", this, "xColumn"         , "x"         );
  addProperty("columns", this, "yColumn"         , "y"         );
  addProperty("columns", this, "symbolSizeColumn", "symbolSize");
  addProperty("columns", this, "fontSizeColumn"  , "fontSize"  );
  addProperty("columns", this, "colorColumn"     , "color"     );

  addSymbolProperties("symbol");

  addProperty("symbol/map", this, "symbolSizeMapped", "enabled");
  addProperty("symbol/map", this, "symbolSizeMapMin", "min"    );
  addProperty("symbol/map", this, "symbolSizeMapMax", "max"    );

  addProperty("font"    , this, "fontSize"      , "size"   );
  addProperty("font/map", this, "fontSizeMapped", "enabled");
  addProperty("font/map", this, "fontSizeMapMin", "min"    );
  addProperty("font/map", this, "fontSizeMapMax", "max"    );

  dataLabel_.addProperties("dataLabel");

  addProperty("color/map", this, "colorMapped", "enabled");
  addProperty("color/map", this, "colorMapMin", "min"    );
  addProperty("color/map", this, "colorMapMax", "max"    );
}

void
CQChartsScatterPlot::
updateRange(bool apply)
{
  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsScatterPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      bool ok1, ok2;

      double x = plot_->modelReal(row, plot_->xColumn(), parent, ok1);
      double y = plot_->modelReal(row, plot_->yColumn(), parent, ok2);

      if (! ok1) x = row;
      if (! ok2) y = row;

      if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y))
        return State::SKIP;

      range_.updateRange(x, y);

      return State::OK;
    }

    const CQChartsGeom::Range &range() const { return range_; }

   private:
    CQChartsScatterPlot *plot_ { nullptr };
    CQChartsGeom::Range  range_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  dataRange_ = visitor.range();

  //---

  // update data range if unset
  if (dataRange_.isSet() && CQChartsUtil::isZero(dataRange_.xsize())) {
    double x = dataRange_.xmid();
    double y = dataRange_.ymid();

    dataRange_.updateRange(x - 1, y);
    dataRange_.updateRange(x + 1, y);
  }

  if (dataRange_.isSet() && CQChartsUtil::isZero(dataRange_.ysize())) {
    double x = dataRange_.xmid();
    double y = dataRange_.ymid();

    dataRange_.updateRange(x, y - 1);
    dataRange_.updateRange(x, y + 1);
  }

  //---

  xAxis_->setColumn(xColumn());
  yAxis_->setColumn(yColumn());

  bool ok;

  QString xname = (xLabel().length() ? xLabel() : modelHeaderString(xColumn(), ok));
  QString yname = (yLabel().length() ? yLabel() : modelHeaderString(yColumn(), ok));

  xAxis_->setLabel(xname);
  yAxis_->setLabel(yname);

  //---

  if (apply)
    applyDataRange();
}

int
CQChartsScatterPlot::
nameIndex(const QString &name) const
{
  int i = 0;

  for (const auto &nv : nameValues_) {
    if (nv.first == name)
      return i;

    ++i;
  }

  return 0;
}

//------

void
CQChartsScatterPlot::
updateObjs()
{
  nameValues_.clear();

  clearValueSets();

  CQChartsPlot::updateObjs();
}

bool
CQChartsScatterPlot::
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

  // init value set
  initValueSets();

  //---

  // init name values
  if (nameValues_.empty()) {
    class RowVisitor : public ModelVisitor {
     public:
      RowVisitor(CQChartsScatterPlot *plot) :
       plot_(plot) {
      }

      State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
        // get x, y value
        QModelIndex xInd  = plot_->modelIndex(row, plot_->xColumn(), parent);
        QModelIndex xInd1 = plot_->normalizeIndex(xInd);

        bool ok1, ok2;

        double x = plot_->modelReal(row, plot_->xColumn(), parent, ok1);
        double y = plot_->modelReal(row, plot_->yColumn(), parent, ok2);

        if (! ok1) x = row;
        if (! ok2) y = row;

        if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y))
          return State::SKIP;

        //---

        // get optional name
        bool ok;

        QString name = plot_->modelString(row, plot_->nameColumn(), parent, ok);

        //---

        // get symbol size, font size and color
        QString       symbolSizeStr;
        QString       fontSizeStr;
        CQChartsColor color;

        // get symbol size label (needed if not string ?)
        if (plot_->symbolSizeColumn().isValid()) {
          bool ok3;

          symbolSizeStr = plot_->modelString(row, plot_->symbolSizeColumn(), parent, ok3);
        }

        // get font size label (needed if not string ?)
        if (plot_->fontSizeColumn().isValid()) {
          bool ok4;

          fontSizeStr = plot_->modelString(row, plot_->fontSizeColumn(), parent, ok4);
        }

        // get color label (needed if not string ?)
        if (plot_->colorColumn().isValid()) {
          bool ok5;

          color = plot_->modelColor(row, plot_->colorColumn(), parent, ok5);
        }

        //---

        plot_->addNameValue(name, x, y, row, xInd1, symbolSizeStr, fontSizeStr, color);

        return State::OK;
      }

     private:
      CQChartsScatterPlot *plot_ { nullptr };
    };

    RowVisitor visitor(this);

    visitModel(visitor);
  }

  //---

  bool ok;

  xname_          = modelHeaderString(xColumn         (), ok);
  yname_          = modelHeaderString(yColumn         (), ok);
  symbolSizeName_ = modelHeaderString(symbolSizeColumn(), ok);
  fontSizeName_   = modelHeaderString(fontSizeColumn  (), ok);
  colorName_      = modelHeaderString(colorColumn     (), ok);

  if (! xname_         .length()) xname_          = "x";
  if (! yname_         .length()) yname_          = "y";
  if (! symbolSizeName_.length()) symbolSizeName_ = "symbolSize";
  if (! fontSizeName_  .length()) fontSizeName_   = "fontSize";
  if (! colorName_     .length()) colorName_      = "color";

  //---

  CQChartsValueSet *symbolSizeSet = getValueSet("symbolSize");
  CQChartsValueSet *fontSizeSet   = getValueSet("fontSize");

  assert(symbolSizeSet && fontSizeSet);

  //---

  //double sw = (dataRange_.xmax() - dataRange_.xmin())/100.0;
  //double sh = (dataRange_.ymax() - dataRange_.ymin())/100.0;

  int nv = nameValues_.size();

  int r = 0;

  for (const auto &nameValues : nameValues_) {
    bool hidden = isSetHidden(r);

    if (! hidden) {
      const QString &name   = nameValues.first;
      const Values  &values = nameValues.second;

      int nv1 = values.size();

      for (int j = 0; j < nv1; ++j) {
        const ValueData &valuePoint = values[j];

        const QPointF &p = valuePoint.p;

        CQChartsLength symbolSize = this->symbolSize();

        if (symbolSizeColumn().isValid() && symbolSizeSet->hasInd(valuePoint.i))
          symbolSize = CQChartsLength(symbolSizeSet->imap(valuePoint.i),
                                      CQChartsLength::Units::PIXEL);

        OptReal  fontSize = boost::make_optional(false, 0.0);
        OptColor color    = boost::make_optional(false, CQChartsColor());

        if (fontSizeColumn().isValid() && fontSizeSet->hasInd(valuePoint.i))
          fontSize = fontSizeSet->imap(valuePoint.i);

        if (colorColumn().isValid())
          (void) colorSetColor("color", valuePoint.i, color);

        double sw = lengthPlotWidth (symbolSize);
        double sh = lengthPlotHeight(symbolSize);

        CQChartsGeom::BBox bbox(p.x() - sw, p.y() - sh, p.x() + sw, p.y() + sh);

        CQChartsScatterPointObj *pointObj =
          new CQChartsScatterPointObj(this, bbox, p, symbolSize, fontSize, color, r, nv, j, nv1);

        //---

#if 0
        QString id = name;

        id += QString("\n  %1\t%2").arg(xname_).arg(p.x());
        id += QString("\n  %1\t%2").arg(yname_).arg(p.y());

        if (valuePoint.symbolSizeStr != "")
          id += QString("\n  %1\t%2").arg(symbolSizeName_).arg(valuePoint.symbolSizeStr);

        if (valuePoint.fontSizeStr != "")
          id += QString("\n  %1\t%2").arg(fontSizeName_).arg(valuePoint.fontSizeStr);

        if (valuePoint.color.isValid())
          id += QString("\n  %1\t%2").arg(colorName_).arg(valuePoint.color.colorStr());

        pointObj->setId(id);
#endif

        //---

        pointObj->setName(name);

        pointObj->setInd(valuePoint.ind);

        addPlotObject(pointObj);
      }
    }

    ++r;
  }

  //---

  resetKeyItems();

  //---

  return true;
}

void
CQChartsScatterPlot::
addKeyItems(CQChartsPlotKey *key)
{
  int nv = nameValues_.size();

  int i = 0;

  for (const auto &nameValue : nameValues_) {
    const QString &name   = nameValue.first;
    const Values  &values = nameValue.second;

    CQChartsScatterKeyColor *colorItem = new CQChartsScatterKeyColor(this, i, nv);
    CQChartsKeyText         *textItem  = new CQChartsKeyText        (this, name);

    key->addItem(colorItem, i, 0);
    key->addItem(textItem , i, 1);

    int nv1 = values.size();

    if (nv1 > 0) {
      const ValueData &valuePoint = values[0];

      OptColor color = boost::make_optional(false, CQChartsColor());

      if (colorColumn().isValid()) {
        (void) colorSetColor("color", valuePoint.i, color);

        if (color)
          colorItem->setColor(*color);
      }
    }

    ++i;
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsScatterPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
}

void
CQChartsScatterPlot::
drawForeground(QPainter *painter)
{
  // draw size key
  if (symbolSizeColumn().isValid()) {
    CQChartsValueSet *symbolSizeSet = getValueSet("symbolSize");

    OptReal min  = boost::make_optional(false, 0.0);
    OptReal mean = boost::make_optional(false, 0.0);
    OptReal max  = boost::make_optional(false, 0.0);

    if (symbolSizeSet->canMap()) {
      min  = symbolSizeSet->rmin();
      mean = symbolSizeSet->rmean();
      max  = symbolSizeSet->rmax();
    }

    QString  typeName;
    QVariant minValue, maxValue;

    columnDetails(symbolSizeColumn(), typeName, minValue, maxValue);

    bool ok;

    if (minValue.isValid())
      min = CQChartsUtil::toReal(minValue, ok);

    if (maxValue.isValid())
      max = CQChartsUtil::toReal(maxValue, ok);

    double px, py;

    double vx = view()->viewportRange();
    double vy = 0.0;

    view()->windowToPixel(vx, vy, px, py);

    int pm = 16;

    double pr1 = symbolSizeSet->mapMax();
    double pr3 = symbolSizeSet->mapMin();
    double pr2 = (pr1 + pr3)/2;

    QColor borderColor = interpThemeColor(1.0);

    painter->setPen(borderColor);

    double xm = px - pr1 - pm;
    double ym = py - pm;

    QRectF r1(xm - pr1, ym - 2*pr1, 2*pr1, 2*pr1);
    QRectF r2(xm - pr2, ym - 2*pr2, 2*pr2, 2*pr2);
    QRectF r3(xm - pr3, ym - 2*pr3, 2*pr3, 2*pr3);

    QColor fillColor1 = interpSymbolFillColor(1.0); fillColor1.setAlphaF(0.2);
    QColor fillColor2 = interpSymbolFillColor(0.5); fillColor2.setAlphaF(0.2);
    QColor fillColor3 = interpSymbolFillColor(0.0); fillColor3.setAlphaF(0.2);

    painter->setBrush(fillColor1); painter->drawEllipse(r1);
    painter->setBrush(fillColor2); painter->drawEllipse(r2);
    painter->setBrush(fillColor3); painter->drawEllipse(r3);

    auto drawText = [&](QPainter *painter, const QPointF &p, const QString &text) {
      QFontMetricsF fm(painter->font());

      painter->drawText(p.x() - fm.width(text)/2, p.y(), text);
    };

    if (max)
      drawText(painter, QPointF(r1.center().x(), r1.top()), QString("%1").arg(*max));

    if (mean)
      drawText(painter, QPointF(r2.center().x(), r2.top()), QString("%1").arg(*mean));

    if (min)
      drawText(painter, QPointF(r3.center().x(), r3.top()), QString("%1").arg(*min));
  }

  //---

  CQChartsPlot::drawForeground(painter);
}

#if 0
void
CQChartsScatterPlot::
drawDataLabel(QPainter *painter, const QRectF &qrect, const QString &str, double fontSize)
{
  if (fontSize > 0) {
    QFont font = dataLabel_.textFont();

    QFont font1 = font;

    font1.setPointSizeF(fontSize);

    dataLabel_.setTextFont(font1);

    dataLabel_.draw(painter, qrect, str);

    dataLabel_.setTextFont(font);
  }
  else {
    dataLabel_.draw(painter, qrect, str);
  }
}
#endif

//------

CQChartsScatterPointObj::
CQChartsScatterPointObj(CQChartsScatterPlot *plot, const CQChartsGeom::BBox &rect, const QPointF &p,
                        const CQChartsLength &symbolSize, const OptReal &fontSize,
                        const OptColor &color, int is, int ns, int iv, int nv) :
 CQChartsPlotObj(plot, rect), plot_(plot), p_(p), symbolSize_(symbolSize), fontSize_(fontSize),
 color_(color), is_(is), ns_(ns), iv_(iv), nv_(nv)
{
}

QString
CQChartsScatterPointObj::
calcId() const
{
  QString id = name_;

  id += QString(" %1=%2").arg(plot_->xname()).arg(p_.x());
  id += QString(" %1=%2").arg(plot_->yname()).arg(p_.y());

  return id;
}

QString
CQChartsScatterPointObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addBoldLine(name_);

  tableTip.addTableRow(plot_->xname(), p_.x());
  tableTip.addTableRow(plot_->yname(), p_.y());

  auto p = plot_->nameValues().find(name_);
  assert(p != plot_->nameValues().end());

  const CQChartsScatterPlot::Values &values = (*p).second;

  const CQChartsScatterPlot::ValueData &valuePoint = values[iv_];

  if (valuePoint.symbolSizeStr != "")
    tableTip.addTableRow(plot_->symbolSizeName(), valuePoint.symbolSizeStr);

  if (valuePoint.fontSizeStr != "")
    tableTip.addTableRow(plot_->fontSizeName(), valuePoint.fontSizeStr);

  if (valuePoint.color.isValid())
    tableTip.addTableRow(plot_->colorName(), valuePoint.color.colorStr());

  return tableTip.str();
}

bool
CQChartsScatterPointObj::
inside(const CQChartsGeom::Point &p) const
{
  const CQChartsLength &s = this->symbolSize(); // TODO: ensure not a crazy number

  double sx = plot_->lengthPixelWidth (s);
  double sy = plot_->lengthPixelHeight(s);

  double px, py;

  plot_->windowToPixel(p_.x(), p_.y(), px, py);

  CQChartsGeom::BBox pbbox(px - sx, py - sy, px + sx, py + sy);

  CQChartsGeom::Point pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsScatterPointObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->xColumn());
  addColumnSelectIndex(inds, plot_->yColumn());
}

void
CQChartsScatterPointObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind_.row(), column, ind_.parent());
}

void
CQChartsScatterPointObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  CQChartsSymbol        symbol  = plot_->symbolType();
  const CQChartsLength &size    = this->symbolSize();
  bool                  stroked = plot_->isSymbolStroked();
  bool                  filled  = plot_->isSymbolFilled();

  double sx = plot_->limitSymbolSize(plot_->lengthPixelWidth (size));
  double sy = plot_->limitSymbolSize(plot_->lengthPixelHeight(size));

  //---

  // calc stroke and brush
  QBrush brush;

  if (filled) {
    QColor c;

    if (color_)
      c = (*color_).interpColor(plot_, is_, ns_);
    else
      c = plot_->interpSymbolFillColor(is_, ns_);

    c.setAlphaF(plot_->symbolFillAlpha());

    brush.setColor(c);
    brush.setStyle(CQChartsFillPattern::toStyle(
      (CQChartsFillPattern::Type) plot_->symbolFillPattern()));
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen pen;

  if (stroked) {
    QColor c = plot_->interpSymbolStrokeColor(0, 1);

    c.setAlphaF(plot_->symbolStrokeAlpha());

    double bw = plot_->lengthPixelWidth(plot_->symbolStrokeWidth());

    pen.setColor (c);
    pen.setWidthF(bw);
  }
  else {
    pen.setStyle(Qt::NoPen);
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  double px, py;

  plot_->windowToPixel(p_.x(), p_.y(), px, py);

  //---

  // draw symbol
  painter->setPen  (pen);
  painter->setBrush(brush);

  QRectF erect(px - sx, py - sy, 2*sx, 2*sy);

  plot_->drawSymbol(painter, QPointF(px, py), symbol, CQChartsUtil::avg(sx, sy),
                    stroked, pen.color(), pen.widthF(), filled, brush.color());

  //---

  // draw text labels
  if (plot_->isTextLabels()) {
    double fontSize = (fontSize_ ? *fontSize_ : -1);

    CQChartsDataLabel &dataLabel = plot_->dataLabel();

    if (fontSize > 0) {
      fontSize = plot_->limitFontSize(fontSize);

      QFont font = dataLabel.textFont();

      QFont font1 = font;

      font1.setPointSizeF(fontSize);

      dataLabel.setTextFont(font1);

      dataLabel.draw(painter, erect, name_);

      dataLabel.setTextFont(font);
    }
    else {
      dataLabel.draw(painter, erect, name_);
    }

    //plot_->drawDataLabel(painter, erect, name_, (fontSize_ ? *fontSize_ : -1));
  }
}

//------

CQChartsScatterKeyColor::
CQChartsScatterKeyColor(CQChartsScatterPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

bool
CQChartsScatterKeyColor::
selectPress(const CQChartsGeom::Point &)
{
  CQChartsScatterPlot *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->updateObjs();

  return true;
}

QBrush
CQChartsScatterKeyColor::
fillBrush() const
{
  QColor c;

  if (color_.isValid())
    c = color_.interpColor(plot_, 0, 1);
  else
    c = CQChartsKeyColorBox::fillBrush().color();

  CQChartsScatterPlot *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  return c;
}
