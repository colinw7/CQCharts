#include <CQChartsScatterPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <QPainter>

CQChartsScatterPlotType::
CQChartsScatterPlotType()
{
  addParameters();
}

void
CQChartsScatterPlotType::
addParameters()
{
  addColumnParameter("x", "X", "xColumn", "", 0);
  addColumnParameter("y", "Y", "yColumn", "", 1);

  addColumnParameter("name"      , "Name"       , "nameColumn"      , "optional");
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
  symbolSizeSet_.setMapMin(16);
  symbolSizeSet_.setMapMax(64);

  fontSizeSet_.setMapMin(16);
  fontSizeSet_.setMapMax(64);

  symbolBorderColor_ = CQChartsPaletteColor(CQChartsPaletteColor::Type::THEME_VALUE, 1);

  addAxes();

  addKey();

  addTitle();
}

void
CQChartsScatterPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nameColumn"       , "name"       );
  addProperty("columns", this, "xColumn"          , "x"          );
  addProperty("columns", this, "yColumn"          , "y"          );
  addProperty("columns", this, "symbolSizeColumn" , "symbolSize" );
  addProperty("columns", this, "fontSizeColumn"   , "fontSize"   );
  addProperty("columns", this, "colorColumn"      , "color"      );

  addProperty("symbol", this, "symbolBorderColor"   , "borderColor");
  addProperty("symbol", this, "symbolSize"          , "size"       );
  addProperty("symbol", this, "symbolSizeMapEnabled", "mapEnabled" );
  addProperty("symbol", this, "symbolSizeMapMin"    , "mapMin"     );
  addProperty("symbol", this, "symbolSizeMapMax"    , "mapMax"     );

  addProperty("color", this, "colorMapEnabled", "mapEnabled" );
  addProperty("color", this, "colorMapMin"    , "mapMin"     );
  addProperty("color", this, "colorMapMax"    , "mapMax"     );

  addProperty("font", this, "fontSize"          , "font"      );
  addProperty("font", this, "fontSizeMapEnabled", "mapEnabled");
  addProperty("font", this, "fontSizeMapMin"    , "mapMin"    );
  addProperty("font", this, "fontSizeMapMax"    , "mapMax"    );

  dataLabel_.addProperties("dataLabel");
}

void
CQChartsScatterPlot::
updateRange(bool apply)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int nr = numRows();

  dataRange_.reset();

  for (int r = 0; r < nr; ++r) {
    QModelIndex xInd = model->index(r, xColumn());
    QModelIndex yInd = model->index(r, yColumn());

    //---

    bool ok1, ok2;

    double x = CQChartsUtil::modelReal(model, xInd, ok1);
    double y = CQChartsUtil::modelReal(model, yInd, ok2);

    if (! ok1) x = r;
    if (! ok2) y = r;

    if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y))
      continue;

    dataRange_.updateRange(x, y);
  }

  //---

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
numRows() const
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return 0;

  return model->rowCount(QModelIndex());
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
initSymbolSizeSet()
{
  symbolSizeSet_.clear();

  if (symbolSizeColumn() < 0)
    return;

  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int nr = model->rowCount(QModelIndex());

  for (int i = 0; i < nr; ++i) {
    bool ok;

    QVariant value = CQChartsUtil::modelValue(model, i, symbolSizeColumn(), ok);

    symbolSizeSet_.addValue(value); // always add some value
  }
}

void
CQChartsScatterPlot::
initFontSizeSet()
{
  fontSizeSet_.clear();

  if (fontSizeColumn() < 0)
    return;

  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int nr = model->rowCount(QModelIndex());

  for (int i = 0; i < nr; ++i) {
    bool ok;

    QVariant value = CQChartsUtil::modelValue(model, i, fontSizeColumn(), ok);

    fontSizeSet_.addValue(value); // always add some value
  }
}

void
CQChartsScatterPlot::
initColorSet()
{
  colorSet_.clear();

  if (colorColumn() < 0)
    return;

  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int nr = model->rowCount(QModelIndex());

  for (int i = 0; i < nr; ++i) {
    bool ok;

    QVariant value = CQChartsUtil::modelValue(model, i, colorColumn(), ok);

    colorSet_.addValue(value); // always add some value
  }
}

bool
CQChartsScatterPlot::
colorSetColor(int i, OptColor &color)
{
  if (i < 0)
    return false;

  if (colorSet_.empty())
    return false;

  // color can be actual color value (string) or value used to map into palette
  // (map enabled or disabled)
  if (colorSet_.type() != CQChartsValueSet::Type::STRING) {
    double value = colorSet_.imap(i);

    color = CQChartsPaletteColor(CQChartsPaletteColor::Type::PALETTE, value);
  }
  else {
    QVariant colorVar = colorSet_.value(i);

    color = QColor(colorVar.toString());
  }

  return true;
}

//------

void
CQChartsScatterPlot::
updateObjs()
{
  nameValues_.clear();

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

  // init name values
  if (nameValues_.empty()) {
    initSymbolSizeSet();
    initFontSizeSet  ();
    initColorSet     ();

    //---

    QAbstractItemModel *model = this->model();

    if (! model)
      return false;

    int nr = numRows();

    for (int r = 0; r < nr; ++r) {
      QModelIndex nameInd = model->index(r, nameColumn());

      //---

      bool ok;

      QString name = CQChartsUtil::modelString(model, nameInd, ok);

      //---

      QModelIndex xInd = model->index(r, xColumn());
      QModelIndex yInd = model->index(r, yColumn());

      QModelIndex xInd1 = normalizeIndex(xInd);

      //---

      bool ok1, ok2;

      double x = CQChartsUtil::modelReal(model, xInd, ok1);
      double y = CQChartsUtil::modelReal(model, yInd, ok2);

      if (! ok1) x = r;
      if (! ok2) y = r;

      if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y))
        continue;

      //---

      QModelIndex symbolSizeInd = model->index(r, symbolSizeColumn());
      QModelIndex fontSizeInd   = model->index(r, fontSizeColumn  ());
      QModelIndex colorInd      = model->index(r, colorColumn     ());

      bool ok3;

      // get symbol size label (needed if not string ?)
      QString symbolSizeStr = CQChartsUtil::modelString(model, symbolSizeInd, ok3);

      // get font size label (needed if not string ?)
      QString fontSizeStr = CQChartsUtil::modelString(model, fontSizeInd, ok3);

      // get color label (needed if not string ?)
      QString colorStr = CQChartsUtil::modelString(model, colorInd, ok3);

      //---

      nameValues_[name].emplace_back(x, y, r, xInd1, symbolSizeStr, fontSizeStr, colorStr);
    }
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
          symbolSize = symbolSizeSet_.imap(valuePoint.i);

        OptReal  fontSize = boost::make_optional(false, 0.0);
        OptColor color    = boost::make_optional(false, CQChartsPaletteColor());

        if (fontSizeColumn() >= 0)
          fontSize = fontSizeSet_.imap(valuePoint.i);

        (void) colorSetColor(valuePoint.i, color);

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
addKeyItems(CQChartsKey *key)
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
  QString id = "<b>" + name_ + "</b>\n";

  id += "<table>\n";

  id += QString("<tr><td>%1</td><td>%2</td></tr>\n").arg(plot_->xname()).arg(p_.x());
  id += QString("<tr><td>%1</td><td>%2</td></tr>\n").arg(plot_->yname()).arg(p_.y());

  auto p = plot_->nameValues().find(name_);
  assert(p != plot_->nameValues().end());

  const CQChartsScatterPlot::Values &values = (*p).second;

  const CQChartsScatterPlot::Point &valuePoint = values[iv_];

  if (valuePoint.symbolSizeStr != "")
    id += QString("<tr><td>%1</td><td>%2</td></tr>\n").
            arg(plot_->symbolSizeName()).arg(valuePoint.symbolSizeStr);

  if (valuePoint.fontSizeStr != "")
    id += QString("<tr><td>%1</td><td>%2</td></tr>\n").
            arg(plot_->fontSizeName()).arg(valuePoint.fontSizeStr);

  if (valuePoint.colorStr != "")
    id += QString("<tr><td>%1</td><td>%2</td></tr>\n").
            arg(plot_->colorName()).arg(valuePoint.colorStr);

  return id;
}

bool
CQChartsScatterPointObj::
inside(const CQChartsGeom::Point &p) const
{
  double s = symbolSize_; // TODO: ensure not a crazy number

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
  double s = symbolSize_; // TODO: ensure not a crazy number

  QColor color;

  if (color_)
    color = (*color_).interpColor(plot_, is_, ns_);
  else
    color = plot_->interpPaletteColor(is_, ns_);

  QBrush brush(color);
  QPen   pen  (plot_->interpSymbolBorderColor(0, 1));

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
