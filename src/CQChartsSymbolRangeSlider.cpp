#include <CQChartsSymbolRangeSlider.h>
#include <CQChartsPointPlot.h>
#include <CQChartsSymbolSet.h>
#include <CQCharts.h>
#include <CQChartsViewPlotPaintDevice.h>

#include <QPainter>

CQChartsSymbolRangeSlider::
CQChartsSymbolRangeSlider(QWidget *parent) :
 CQIntRangeSlider(parent)
{
}

void
CQChartsSymbolRangeSlider::
setPlot(CQChartsPlot *plot)
{
  plot_ = plot;

  update();
}

void
CQChartsSymbolRangeSlider::
setSymbolSetName(const QString &name)
{
  symbolSetName_ = name;

  auto *symbolSetMgr = (plot_ ? plot_->charts()->symbolSetMgr() : nullptr);
  auto *symbolSet    = (symbolSetMgr ? symbolSetMgr->symbolSet(symbolSetName_) : nullptr);

  int typeVal1 = (symbolSet ?                           0 : CQChartsSymbol::minOutlineValue());
  int typeVal2 = (symbolSet ? symbolSet->numSymbols() - 1 : CQChartsSymbol::maxOutlineValue());

  setRangeMinMax(typeVal1, typeVal2);

  auto *pointPlot = dynamic_cast<CQChartsPointPlot *>(plot_);

  if (pointPlot)
    setSliderMinMax(pointPlot->symbolTypeMapMin(), pointPlot->symbolTypeMapMax());

  fixSliderValues();
}

void
CQChartsSymbolRangeSlider::
drawSliderLabels(QPainter *painter)
{
  QFontMetricsF fm(font());
  QFontMetricsF tfm(textFont());

  double ym = height()/2.0;
  double ss = std::min(tfm.height(), fm.height() - 4);

  double bs = fm.height()/3.0;

  //---

  auto sx1 = valueToPixel(sliderMin());
  auto sx2 = valueToPixel(sliderMax());

  auto x1 = sx1 - bs - ss/2.0;
  auto x2 = sx2 + bs + ss/2.0;

  if (x1 - ss/2.0 < xs1_)
    x1 = sx1 + bs + ss/2.0;

  if (x2 + ss/2.0 > xs2_)
    x2 = sx2 - bs - ss/2.0;

  //---

  auto *symbolSetMgr = (plot_ ? plot_->charts()->symbolSetMgr() : nullptr);
  auto *symbolSet    = (symbolSetMgr ? symbolSetMgr->symbolSet(symbolSetName_) : nullptr);

  CQChartsSymbol symbol1, symbol2;

  if (symbolSet) {
    symbol1 = symbolSet->symbol(sliderMin());
    symbol2 = symbolSet->symbol(sliderMax());
  }
  else {
    symbol1 = CQChartsSymbol((CQChartsSymbol::Type) sliderMin());
    symbol2 = CQChartsSymbol((CQChartsSymbol::Type) sliderMax());
  }

  //---

  CQChartsPixelPaintDevice device(painter);

  painter->setPen(Qt::black);

  bool filled1 = (symbolSet ? symbolSet->isFilled(sliderMin()) : false);
  bool filled2 = (symbolSet ? symbolSet->isFilled(sliderMax()) : false);

  auto drawSymbol = [&](const CQChartsSymbol &symbol, double x, bool filled) {
    if (filled)
      painter->setBrush(Qt::green);
    else
      painter->setBrush(Qt::NoBrush);

    CQChartsGeom::BBox bbox(x - ss/2.0, ym - ss/2, x + ss/2.0, ym + ss/2.0);

    CQChartsDrawUtil::drawSymbol(&device, symbol, bbox);
  };

  drawSymbol(symbol1, x1, filled1);
  drawSymbol(symbol2, x2, filled2);

}
