#include <CQChartsFontSizeRangeSlider.h>
#include <CQChartsPointPlot.h>
#include <CQCharts.h>

#include <QPainter>

CQChartsFontSizeRangeSlider::
CQChartsFontSizeRangeSlider(QWidget *parent) :
 CQDoubleRangeSlider(parent)
{
  auto font = this->font();

  font.setPointSizeF(font.pointSizeF()*0.9);

  setTextFont(font);
}

void
CQChartsFontSizeRangeSlider::
setPlot(CQChartsPlot *plot)
{
  plot_ = plot;

  setRangeMinMax(CQChartsFontSize::minValue(), CQChartsFontSize::maxValue());

  auto *pointPlot = dynamic_cast<CQChartsPointPlot *>(plot_);

  if (pointPlot)
    setSliderMinMax(pointPlot->fontSizeMapMin(), pointPlot->fontSizeMapMax());

  update();
}
