#include <CQChartsScatterPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
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
  addColumnParameter("x"   , "X"   , "xColumn"   , "", 0);
  addColumnParameter("y"   , "Y"   , "yColumn"   , "", 1);
  addColumnParameter("name", "Name", "nameColumn", "optional");

  addColumnParameter("symbolSize", "Symbol Size", "symbolSizeColumn", "optional");
  addColumnParameter("fontSize"  , "Font Size"  , "fontSizeColumn"  , "optional");
  addColumnParameter("color"     , "Color"      , "colorColumn"     , "optional");

  addBoolParameter("textLabels", "Text Labels", "textLabels", "optional");

  CQChartsPlotType::addParameters();
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
  (void) addValueSet("symbolSize", 16, 64);
  (void) addValueSet("fontSize"  , 16, 64);
  (void) addColorSet("color");

  symbolBorderColor_ = CQChartsPaletteColor(CQChartsPaletteColor::Type::THEME_VALUE, 1);
  symbolFillColor_   = CQChartsPaletteColor(CQChartsPaletteColor::Type::PALETTE);

  addAxes();

  addKey();

  addTitle();
}

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

  addProperty("symbol/stroke", this, "symbolBorder"     , "visible");
  addProperty("symbol/stroke", this, "symbolBorderColor", "color"  );
  addProperty("symbol/stroke", this, "symbolBorderAlpha", "alpha"  );
  addProperty("symbol/stroke", this, "symbolBorderWidth", "width"  );

  addProperty("symbol/fill", this, "symbolFilled"   , "visible");
  addProperty("symbol/fill", this, "symbolFillColor", "color"  );
  addProperty("symbol/fill", this, "symbolFillAlpha", "alpha"  );

  addProperty("symbol", this, "symbolSize"          , "size"      );
  addProperty("symbol", this, "symbolSizeMapEnabled", "mapEnabled");
  addProperty("symbol", this, "symbolSizeMapMin"    , "mapMin"    );
  addProperty("symbol", this, "symbolSizeMapMax"    , "mapMax"    );

  addProperty("font", this, "fontSize"          , "font"      );
  addProperty("font", this, "fontSizeMapEnabled", "mapEnabled");
  addProperty("font", this, "fontSizeMapMin"    , "mapMin"    );
  addProperty("font", this, "fontSizeMapMax"    , "mapMax"    );

  dataLabel_.addProperties("dataLabel");

  addProperty("color", this, "colorMapEnabled", "mapEnabled");
  addProperty("color", this, "colorMapMin"    , "mapMin"    );
  addProperty("color", this, "colorMapMax"    , "mapMax"    );
}

void
CQChartsScatterPlot::
updateRange(bool apply)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsScatterPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      QModelIndex xInd = model->index(row, plot_->xColumn(), parent);
      QModelIndex yInd = model->index(row, plot_->yColumn(), parent);

      bool ok1, ok2;

      double x = CQChartsUtil::modelReal(model, xInd, ok1);
      double y = CQChartsUtil::modelReal(model, yInd, ok2);

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
  if (CQChartsUtil::isZero(dataRange_.xsize())) {
    double x = dataRange_.xmid();
    double y = dataRange_.ymid();

    dataRange_.updateRange(x - 1, y);
    dataRange_.updateRange(x + 1, y);
  }

  if (CQChartsUtil::isZero(dataRange_.ysize())) {
    double x = dataRange_.xmid();
    double y = dataRange_.ymid();

    dataRange_.updateRange(x, y - 1);
    dataRange_.updateRange(x, y + 1);
  }

  //---

  xAxis_->setColumn(xColumn());
  yAxis_->setColumn(yColumn());

  bool ok;

  QString xname = CQChartsUtil::modelHeaderString(model, xColumn(), ok);
  QString yname = CQChartsUtil::modelHeaderString(model, yColumn(), ok);

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
    QAbstractItemModel *model = this->model();

    if (! model)
      return false;

    class RowVisitor : public ModelVisitor {
     public:
      RowVisitor(CQChartsScatterPlot *plot) :
       plot_(plot) {
      }

      State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
        // get x, y value
        QModelIndex xInd = model->index(row, plot_->xColumn(), parent);
        QModelIndex yInd = model->index(row, plot_->yColumn(), parent);

        QModelIndex xInd1 = plot_->normalizeIndex(xInd);

        bool ok1, ok2;

        double x = CQChartsUtil::modelReal(model, xInd, ok1);
        double y = CQChartsUtil::modelReal(model, yInd, ok2);

        if (! ok1) x = row;
        if (! ok2) y = row;

        if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y))
          return State::SKIP;

        //---

        // get optional name
        QModelIndex nameInd = model->index(row, plot_->nameColumn(), parent);

        bool ok;

        QString name = CQChartsUtil::modelString(model, nameInd, ok);

        //---

        // get symbol size, font size and color
        QModelIndex symbolSizeInd = model->index(row, plot_->symbolSizeColumn());
        QModelIndex fontSizeInd   = model->index(row, plot_->fontSizeColumn  ());
        QModelIndex colorInd      = model->index(row, plot_->colorColumn     ());

        bool ok3, ok4, ok5;

        // get symbol size label (needed if not string ?)
        QString symbolSizeStr = CQChartsUtil::modelString(model, symbolSizeInd, ok3);

        // get font size label (needed if not string ?)
        QString fontSizeStr = CQChartsUtil::modelString(model, fontSizeInd, ok4);

        // get color label (needed if not string ?)
        QString colorStr = CQChartsUtil::modelString(model, colorInd, ok5);

        //---

        plot_->addNameValue(name, x, y, row, xInd1, symbolSizeStr, fontSizeStr, colorStr);

        return State::OK;
      }

     private:
      CQChartsScatterPlot *plot_ { nullptr };
    };

    RowVisitor visitor(this);

    visitModel(visitor);
  }

  //---

  QAbstractItemModel *model = this->model();

  //---

  if (model) {
    bool ok;

    xname_          = CQChartsUtil::modelHeaderString(model, xColumn         (), ok);
    yname_          = CQChartsUtil::modelHeaderString(model, yColumn         (), ok);
    symbolSizeName_ = CQChartsUtil::modelHeaderString(model, symbolSizeColumn(), ok);
    fontSizeName_   = CQChartsUtil::modelHeaderString(model, fontSizeColumn  (), ok);
    colorName_      = CQChartsUtil::modelHeaderString(model, colorColumn     (), ok);
  }
  else {
    xname_          = "";
    yname_          = "";
    symbolSizeName_ = "";
    fontSizeName_   = "";
    colorName_      = "";
  }

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
        const Point &valuePoint = values[j];

        const QPointF &p = valuePoint.p;

        double symbolSize = this->symbolSize();

        if (symbolSizeColumn() >= 0)
          symbolSize = symbolSizeSet->imap(valuePoint.i);

        OptReal  fontSize = boost::make_optional(false, 0.0);
        OptColor color    = boost::make_optional(false, CQChartsPaletteColor());

        if (fontSizeColumn() >= 0)
          fontSize = fontSizeSet->imap(valuePoint.i);

        if (colorColumn() >= 0)
          (void) colorSetColor("color", valuePoint.i, color);

        double sw = pixelToWindowWidth (symbolSize);
        double sh = pixelToWindowHeight(symbolSize);

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

        if (valuePoint.colorStr != "")
          id += QString("\n  %1\t%2").arg(colorName_).arg(valuePoint.colorStr);

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

  for (const auto &nameValue: nameValues_) {
    const QString &name = nameValue.first;

    CQChartsScatterKeyColor *color = new CQChartsScatterKeyColor(this, i, nv);
    CQChartsKeyText         *text  = new CQChartsKeyText        (this, name);

    key->addItem(color, i, 0);
    key->addItem(text , i, 1);

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
  if (symbolSizeColumn() >= 0) {
    CQChartsValueSet *symbolSizeSet = getValueSet("symbolSize");

    double min  = symbolSizeSet->rmin();
    double mean = symbolSizeSet->rmean();
    double max  = symbolSizeSet->rmax();

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
      QFontMetrics fm(painter->font());

      painter->drawText(p.x() - fm.width(text)/2, p.y(), text);
    };

    drawText(painter, QPointF(r1.center().x(), r1.top()), QString("%1").arg(max));
    drawText(painter, QPointF(r2.center().x(), r2.top()), QString("%1").arg(mean));
    drawText(painter, QPointF(r3.center().x(), r3.top()), QString("%1").arg(min));
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
                        double symbolSize, const OptReal &fontSize, const OptColor &color,
                        int is, int ns, int iv, int nv) :
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

  const CQChartsScatterPlot::Point &valuePoint = values[iv_];

  if (valuePoint.symbolSizeStr != "")
    tableTip.addTableRow(plot_->symbolSizeName(), valuePoint.symbolSizeStr);

  if (valuePoint.fontSizeStr != "")
    tableTip.addTableRow(plot_->fontSizeName(), valuePoint.fontSizeStr);

  if (valuePoint.colorStr != "")
    tableTip.addTableRow(plot_->colorName(), valuePoint.colorStr);

  return tableTip.str();
}

bool
CQChartsScatterPointObj::
inside(const CQChartsGeom::Point &p) const
{
  double s = this->symbolSize(); // TODO: ensure not a crazy number

  double px, py;

  plot_->windowToPixel(p_.x(), p_.y(), px, py);

  CQChartsGeom::BBox pbbox(px - s, py - s, px + s, py + s);

  CQChartsGeom::Point pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsScatterPointObj::
addSelectIndex()
{
  plot_->addSelectIndex(ind_.row(), plot_->xColumn());
  plot_->addSelectIndex(ind_.row(), plot_->yColumn());
}

bool
CQChartsScatterPointObj::
isIndex(const QModelIndex &ind) const
{
  return (ind == ind_);
}

void
CQChartsScatterPointObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  double s = this->symbolSize(); // TODO: ensure not a crazy number

  //---

  // calc stroke and brush
  QBrush brush;

  if (plot_->isSymbolFilled()) {
    QColor c;

    if (color_)
      c = (*color_).interpColor(plot_, is_, ns_);
    else
      c = plot_->interpSymbolFillColor(is_, ns_);

    c.setAlphaF(plot_->symbolFillAlpha());

    brush.setColor(c);
    brush.setStyle(Qt::SolidPattern);
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen pen;

  if (plot_->isSymbolBorder()) {
    QColor c = plot_->interpSymbolBorderColor(0, 1);

    c.setAlphaF(plot_->symbolBorderAlpha());

    pen.setColor (c);
    pen.setWidthF(plot_->symbolBorderWidth());
  }
  else {
    pen.setStyle(Qt::NoPen);
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  double px, py;

  plot_->windowToPixel(p_.x(), p_.y(), px, py);

  painter->setPen  (pen);
  painter->setBrush(brush);

  QRectF erect(px - s, py - s, 2*s, 2*s);

  painter->drawEllipse(erect);

  if (plot_->isTextLabels()) {
    int fontSize = (fontSize_ ? *fontSize_ : -1);

    CQChartsDataLabel &dataLabel = plot_->dataLabel();

    if (fontSize > 0) {
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
mousePress(const CQChartsGeom::Point &)
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
  QColor c = CQChartsKeyColorBox::fillBrush().color();

  CQChartsScatterPlot *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  return c;
}
