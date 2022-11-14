#include <CQChartsSymbolTypeRangeSlider.h>
#include <CQChartsPointPlot.h>
#include <CQChartsSymbolSet.h>
#include <CQCharts.h>
#include <CQChartsUtil.h>
#include <CQChartsPixelPaintDevice.h>

#include <QPainter>

CQChartsSymbolTypeRangeSlider::
CQChartsSymbolTypeRangeSlider(QWidget *parent) :
 CQIntRangeSlider(parent)
{
  setSliderPos(SliderPos::BOTH);

  setFillColor  (QColor("#1155bc")); // TODO: config
  setStrokeColor(QColor("#aaaaaa")); // TODO: config

  setXBorder(12);
}

CQChartsPlot *
CQChartsSymbolTypeRangeSlider::
plot() const
{
  return plot_.data();
}

void
CQChartsSymbolTypeRangeSlider::
setPlot(Plot *plot)
{
  plot_ = plot;

  update();
}

void
CQChartsSymbolTypeRangeSlider::
setSymbolSetName(const QString &name)
{
  symbolSetName_ = name;

  auto *symbolSetMgr = (plot() ? plot()->charts()->symbolSetMgr() : nullptr);
  auto *symbolSet    = (symbolSetMgr ? symbolSetMgr->symbolSet(symbolSetName_) : nullptr);

  int typeVal1 = (symbolSet ?                           0 : CQChartsSymbolType::minOutlineValue());
  int typeVal2 = (symbolSet ? symbolSet->numSymbols() - 1 : CQChartsSymbolType::maxOutlineValue());

  setRangeMinMax(typeVal1, typeVal2);

  auto *pointPlot = dynamic_cast<CQChartsPointPlot *>(plot());

  if (pointPlot)
    setSliderMinMax(int(pointPlot->symbolTypeMapMin()), int(pointPlot->symbolTypeMapMax()));

  fixSliderValues();
}

void
CQChartsSymbolTypeRangeSlider::
drawSliderLabels(QPainter *painter)
{
  painter->setRenderHints(QPainter::Antialiasing);

  //---

  QFontMetricsF fm(font());
  QFontMetricsF tfm(textFont());

  double ym = height()/2.0;
  double ss = std::min(tfm.height(), fm.height() - 4);

  //---

  auto sx1 = valueToPixel(rangeMin());
  auto sx2 = valueToPixel(rangeMax());

  //---

  auto *symbolSetMgr = (plot() ? plot()->charts()->symbolSetMgr() : nullptr);
  auto *symbolSet    = (symbolSetMgr ? symbolSetMgr->symbolSet(symbolSetName_) : nullptr);

  //---

  CQChartsPixelPaintDevice device(painter);

  auto drawSymbol = [&](const CQChartsSymbol &symbol, double x, bool inside) {
    if (symbol.isFilled()) {
      auto bc = fillColor();
      auto ba = 1.0;

      if (! isEnabled()) {
        bc = CQChartsUtil::grayColor(bc);
        ba = 0.5;
      }

      auto pc = strokeColor();

      bc.setAlphaF(inside ? ba : ba*0.5);
      pc.setAlphaF(inside ? ba : ba*0.5);

      painter->setBrush(bc);
      painter->setPen  (pc);
    }
    else {
      auto pc = palette().color(QPalette::Text);

      if (! inside)
        pc.setAlphaF(0.5);

      painter->setBrush(Qt::NoBrush);
      painter->setPen  (pc);
    }

    CQChartsGeom::BBox bbox(x - ss/2.0, ym - ss/2, x + ss/2.0, ym + ss/2.0);

    CQChartsDrawUtil::drawSymbol(&device, symbol, bbox);
  };

  double x2 = sx1 - ss;

  painter->setPen(Qt::black);

  for (int i = rangeMin(); i <= rangeMax(); ++i) {
    double x = CMathUtil::map(i, rangeMin(), rangeMax(), sx1, sx2);

    double x1 = x - ss/2.0;

    if (x1 > x2) {
      bool inside = (i >= sliderMin() && i <= sliderMax());

      CQChartsSymbol symbol;

      if (symbolSet)
        symbol = symbolSet->symbol(i);
      else {
        auto type = static_cast<CQChartsSymbolType::Type>(i);

        if (! CQChartsSymbolType::isValidType(type))
          continue;

        symbol = CQChartsSymbol(type);
      }

      if (symbol.isValid())
        drawSymbol(symbol, x, inside);

      x2 = x + ss/2.0;
    }
  }
}
