#include <CQChartsSymbolSizeRangeSlider.h>
#include <CQChartsPointPlot.h>
#include <CQCharts.h>

#include <QPainter>

CQChartsSymbolSizeRangeSlider::
CQChartsSymbolSizeRangeSlider(QWidget *parent) :
 CQDoubleRangeSlider(parent)
{
  auto font = this->font();

  font.setPointSizeF(font.pointSizeF()*0.9);

  setTextFont(font);
}

void
CQChartsSymbolSizeRangeSlider::
setPlot(CQChartsPlot *plot)
{
  plot_ = plot;

  setRangeMinMax(CQChartsSymbolSize::minValue(), CQChartsSymbolSize::maxValue());

  auto *pointPlot = dynamic_cast<CQChartsPointPlot *>(plot_);

  if (pointPlot)
    setSliderMinMax(pointPlot->symbolSizeMapMin(), pointPlot->symbolSizeMapMax());

  update();
}
