#include <CQChartsScatterPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsTitle.h>
#include <CQChartsValueSet.h>
#include <CQChartsUtil.h>
#include <CQChartsTip.h>
#include <CQCharts.h>
#include <CLeastSquaresFit.h>

#include <QPainter>
#include <QMenu>

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

  CQChartsGroupPlotType::addParameters();

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
 CQChartsGroupPlot(view, view->charts()->plotType("scatter"), model), dataLabel_(this)
{
  // set mapped range
  (void) addValueSet("symbolSize", 8, 64);
  (void) addValueSet("fontSize"  , 8, 48);
  (void) addColorSet("color");

  setSymbolType(CQChartsSymbol::Type::CIRCLE);
  setSymbolStroked(true);
  setSymbolFilled (true);
  setSymbolFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

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

//---

void
CQChartsScatterPlot::
setSymbolSize(const CQChartsLength &s)
{
  CQChartsUtil::testAndSet(symbolData_.size, s, [&]() { updateObjs(); } );
}

void
CQChartsScatterPlot::
setSymbolType(const CQChartsSymbol &t)
{
  CQChartsUtil::testAndSet(symbolData_.type, t, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolStroked(bool b)
{
  CQChartsUtil::testAndSet(symbolData_.stroke.visible, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolStrokeColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(symbolData_.stroke.color, c, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolStrokeAlpha(double a)
{
  CQChartsUtil::testAndSet(symbolData_.stroke.alpha, a, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolStrokeWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(symbolData_.stroke.width, l, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolFilled(bool b)
{
  CQChartsUtil::testAndSet(symbolData_.fill.visible, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolFillColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(symbolData_.fill.color, c, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolFillAlpha(double a)
{
  CQChartsUtil::testAndSet(symbolData_.fill.alpha, a, [&]() { invalidateLayers(); } );
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

    invalidateLayers();
  }
}

//---

void
CQChartsScatterPlot::
setSymbolMapKey(bool b)
{
  CQChartsUtil::testAndSet(symbolMapKey_, b, [&]() { invalidateLayers(); } );
}

void
CQChartsScatterPlot::
setSymbolSizeColumn(const CQChartsColumn &c)
{
  if (setValueSetColumn("symbolSize", c))
    updateRangeAndObjs();
}

void
CQChartsScatterPlot::
setSymbolSizeMapped(bool b)
{
  setValueSetMapped("symbolSize", b);

  updateObjs();
}

void
CQChartsScatterPlot::
setSymbolSizeMapMin(double r)
{
  setValueSetMapMin("symbolSize", r); updateObjs(); }

void
CQChartsScatterPlot::
setSymbolSizeMapMax(double r)
{
  setValueSetMapMax("symbolSize", r);

  updateObjs();
}

//---

void
CQChartsScatterPlot::
setColorColumn(const CQChartsColumn &c)
{
  if (setValueSetColumn("color", c))
    updateRangeAndObjs();
}

void
CQChartsScatterPlot::
setColorMapped(bool b)
{
  setValueSetMapped("color", b);

  updateObjs();
}

void
CQChartsScatterPlot::
setColorMapMin(double r)
{
  setValueSetMapMin("color", r);

  updateObjs();
}

void
CQChartsScatterPlot::
setColorMapMax(double r)
{
  setValueSetMapMax("color", r);

  updateObjs();
}

//---

void
CQChartsScatterPlot::
setTextLabels(bool b)
{
  dataLabel_.setVisible(b);

  invalidateLayers();
}

//---

void
CQChartsScatterPlot::
setFontSizeColumn(const CQChartsColumn &c)
{
  if (setValueSetColumn("fontSize", c))

  updateRangeAndObjs();
}

void
CQChartsScatterPlot::
setFontSize(double s)
{
  CQChartsUtil::testAndSet(fontSize_, s, [&]() { updateObjs(); } );
}

void
CQChartsScatterPlot::
setFontSizeMapped(bool b)
{
  setValueSetMapped("fontSize", b);

  updateObjs();
}

void
CQChartsScatterPlot::
setFontSizeMapMin(double r)
{
  setValueSetMapMin("fontSize", r);

  updateObjs();
}

void
CQChartsScatterPlot::
setFontSizeMapMax(double r)
{
  setValueSetMapMax("fontSize", r);

  updateObjs();
}

void
CQChartsScatterPlot::
setBestFit(bool b)
{
  CQChartsUtil::testAndSet(bestFit_, b, [&]() { invalidateLayers(); } );
}

//------

void
CQChartsScatterPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "xColumn", "x");
  addProperty("columns", this, "yColumn", "y");

  addProperty("columns", this, "nameColumn"      , "name"      );
  addProperty("columns", this, "symbolSizeColumn", "symbolSize");
  addProperty("columns", this, "fontSizeColumn"  , "fontSize"  );
  addProperty("columns", this, "colorColumn"     , "color"     );

  addProperty("options", this, "bestFit", "bestFit");

  CQChartsGroupPlot::addProperties();

  addSymbolProperties("symbol");

  dataLabel_.addProperties("dataLabel");

  addProperty("symbol/map", this, "symbolSizeMapped", "enabled");
  addProperty("symbol/map", this, "symbolSizeMapMin", "min"    );
  addProperty("symbol/map", this, "symbolSizeMapMax", "max"    );

  addProperty("font"    , this, "fontSize"      , "size"   );
  addProperty("font/map", this, "fontSizeMapped", "enabled");
  addProperty("font/map", this, "fontSizeMapMin", "min"    );
  addProperty("font/map", this, "fontSizeMapMax", "max"    );

  addProperty("color/map", this, "colorMapped", "enabled");
  addProperty("color/map", this, "colorMapMin", "min"    );
  addProperty("color/map", this, "colorMapMax", "max"    );
}

//---

void
CQChartsScatterPlot::
updateRange(bool apply)
{
  initGroupData(Columns(), CQChartsColumn());

  //---

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

//------

void
CQChartsScatterPlot::
updateObjs()
{
  groupNameValues_.clear();

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
  if (groupNameValues_.empty())
    addNameValues();

  fitData_.reset();

  points_.clear();

  //---

  // get column titles
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

  int ig = 0;
  int ng = groupNameValues_.size();

  for (const auto &groupNameValue : groupNameValues_) {
    bool hidden = (ng > 1 && isSetHidden(ig));

    if (hidden) { ++ig; continue; }

    //---

    int               groupInd   = groupNameValue.first;
    const NameValues &nameValues = groupNameValue.second;

    int is = 0;
    int ns = nameValues.size();

    for (const auto &nameValue : nameValues) {
      bool hidden = (ng == 1 && isSetHidden(is));

      if (hidden) { ++is; continue; }

      //---

      const QString &name   = nameValue.first;
      const Values  &values = nameValue.second;

      int nv = values.size();

      for (int iv = 0; iv < nv; ++iv) {
        const ValueData &valuePoint = values[iv];

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
          new CQChartsScatterPointObj(this, groupInd, bbox, p, symbolSize, fontSize, color,
                                      ig, ng, is, ns, iv, nv);

        //---

        pointObj->setName(name);

        pointObj->setInd(valuePoint.ind);

        addPlotObject(pointObj);

        //---

        points_.push_back(p);
      }

      ++is;
    }

    ++ig;
  }

  //---

  resetKeyItems();

  //---

  return true;
}

void
CQChartsScatterPlot::
addNameValues()
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsScatterPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      CQChartsModelIndex ind(row, plot_->xColumn(), parent);

      // get group
      int groupInd = plot_->rowGroupInd(ind);

      //---

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
      QString name;

      if (plot_->nameColumn().isValid()) {
        bool ok;

        name = plot_->modelString(row, plot_->nameColumn(), parent, ok);
      }

      if (! name.length())
        name = plot_->title()->textStr();

      if (! name.length())
        name = plot_->xAxis()->label();

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

      plot_->addNameValue(groupInd, name, x, y, row, xInd1, symbolSizeStr, fontSizeStr, color);

      return State::OK;
    }

   private:
    CQChartsScatterPlot *plot_ { nullptr };
  };

  RowVisitor visitor(this);

  visitModel(visitor);
}

void
CQChartsScatterPlot::
addNameValue(int groupInd, const QString &name, double x, double y, int row,
             const QModelIndex &xind, const QString &symbolSizeStr, const QString &fontSizeStr,
             const CQChartsColor &color)
{
  groupNameValues_[groupInd][name].emplace_back(x, y, row, xind, symbolSizeStr, fontSizeStr, color);
}

void
CQChartsScatterPlot::
addKeyItems(CQChartsPlotKey *key)
{
  int ng = groupNameValues_.size();

  // multiple group - key item per group
  if (ng > 1) {
    int ig = 0;

    for (const auto &groupNameValue : groupNameValues_) {
      int groupInd = groupNameValue.first;

      QString groupName = groupIndName(groupInd);

      CQChartsScatterKeyColor *colorItem = new CQChartsScatterKeyColor(this, ig, ng);
      CQChartsKeyText         *textItem  = new CQChartsKeyText        (this, groupName);

      key->addItem(colorItem, ig, 0);
      key->addItem(textItem , ig, 1);

      ++ig;
    }
  }
  // single group - key item per value set
  else {
    const NameValues &nameValues = (*groupNameValues_.begin()).second;

    int is = 0;
    int ns = nameValues.size();

    for (const auto &nameValue : nameValues) {
      const QString &name   = nameValue.first;
      const Values  &values = nameValue.second;

      CQChartsScatterKeyColor *colorItem = new CQChartsScatterKeyColor(this, is, ns);
      CQChartsKeyText         *textItem  = new CQChartsKeyText        (this, name);

      key->addItem(colorItem, is, 0);
      key->addItem(textItem , is, 1);

      int nv = values.size();

      if (nv > 0) {
        const ValueData &valuePoint = values[0];

        OptColor color = boost::make_optional(false, CQChartsColor());

        if (colorColumn().isValid()) {
          (void) colorSetColor("color", valuePoint.i, color);

          if (color)
            colorItem->setColor(*color);
        }
      }

      ++is;
    }
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

//------

bool
CQChartsScatterPlot::
addMenuItems(QMenu *menu)
{
  QAction *bestFitAction = new QAction("Best Fit", menu);

  bestFitAction->setCheckable(true);
  bestFitAction->setChecked(isBestFit());

  connect(bestFitAction, SIGNAL(triggered(bool)), this, SLOT(setBestFit(bool)));

  menu->addSeparator();

  menu->addAction(bestFitAction);

  return true;
}

//------

void
CQChartsScatterPlot::
drawBackground(QPainter *painter)
{
  CQChartsPlot::drawBackground(painter);

  drawBestFit(painter);
}

void
CQChartsScatterPlot::
drawForeground(QPainter *painter)
{
  drawSymbolMapKey(painter);

  CQChartsPlot::drawForeground(painter);
}

void
CQChartsScatterPlot::
drawBestFit(QPainter *painter)
{
  if (isBestFit()) {
    if (! fitData_.fitted) {
      int np = points_.size();;

      std::vector<double> x, y;

      for (const auto &p : points_) {
        x.push_back(p.x());
        y.push_back(p.y());
      }

      double deviation;
      int    return_code;

      CLeastSquaresFit::leastSquaresFit(&x[0], &y[0], np,
                                        fitData_.coeffs, fitData_.coeffs_free, fitData_.num_coeffs,
                                        &deviation, &return_code);

      fitData_.fitted = true;
    }

    //---

    CQChartsGeom::BBox pbbox = calcDataPixelRect();

    double py1 = 0.0;

    for (int px = pbbox.getXMin(); px <= pbbox.getXMax(); ++px) {
      double x, y;

      pixelToWindow(px, 0.0, x, y);

      double y2 = fitData_.coeffs[0] + x*(fitData_.coeffs[1] + x*fitData_.coeffs[2]);

      double px2, py2;

      windowToPixel(x, y2, px2, py2);

      if (x > pbbox.getXMin())
        painter->drawLine(QPointF(px - 1, py1), QPointF(px, py2));
      else
        painter->drawPoint(QPointF(px, py2));

      py1 = py2;
    }
  }
}

void
CQChartsScatterPlot::
drawSymbolMapKey(QPainter *painter)
{
  // draw size key
  if (symbolSizeColumn().isValid() && isSymbolMapKey()) {
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
}

//------

CQChartsScatterPointObj::
CQChartsScatterPointObj(CQChartsScatterPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                        const QPointF &p, const CQChartsLength &symbolSize, const OptReal &fontSize,
                        const OptColor &color, int ig, int ng, int is, int ns, int iv, int nv) :
 CQChartsPlotObj(plot, rect), plot_(plot), groupInd_(groupInd), p_(p), symbolSize_(symbolSize),
 fontSize_(fontSize), color_(color), ig_(ig), ng_(ng), is_(is), ns_(ns), iv_(iv), nv_(nv)
{
  assert(ig >= 0 && ig < ng);
  assert(is >= 0 && is < ns);
  assert(iv >= 0 && iv < nv);
}

QString
CQChartsScatterPointObj::
calcId() const
{
  return QString("point:%1:%2:%3").arg(ig_).arg(is_).arg(iv_);
}

QString
CQChartsScatterPointObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addBoldLine(name_);

  if (ng_ > 1) {
    QString groupName = plot_->groupIndName(groupInd_);

    tableTip.addTableRow("Group", groupName);
  }

  tableTip.addTableRow(plot_->xname(), p_.x());
  tableTip.addTableRow(plot_->yname(), p_.y());

  auto pg = plot_->groupNameValues().find(groupInd_);
  assert(pg != plot_->groupNameValues().end());

  auto p = (*pg).second.find(name_);
  assert(p != (*pg).second.end());

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
draw(QPainter *painter)
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

    if (ng_ > 0) {
      if (color_)
        c = (*color_).interpColor(plot_, ig_, ng_);
      else
        c = plot_->interpSymbolFillColor(ig_, ng_);
    }
    else {
      if (color_)
        c = (*color_).interpColor(plot_, is_, ns_);
      else
        c = plot_->interpSymbolFillColor(is_, ns_);
    }

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

  plot_->drawSymbol(painter, QPointF(px, py), symbol, CQChartsUtil::avg(sx, sy), pen, brush);

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
  CQChartsScatterPlot *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

  QColor c;

  if (color_.isValid())
    c = color_.interpColor(plot_, 0, 1);
  else {
    c = plot->interpSymbolFillColor(i_, n_);

    //c = CQChartsKeyColorBox::fillBrush().color();
  }

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  return c;
}
