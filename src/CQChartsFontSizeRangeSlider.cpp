#include <CQChartsFontSizeRangeSlider.h>
#include <CQChartsPointPlot.h>
#include <CQCharts.h>

#include <QPainter>

CQChartsFontSizeRangeSlider::
CQChartsFontSizeRangeSlider(QWidget *parent) :
 CQDoubleRangeSlider(parent)
{
  auto font = this->font();

  setTextFontScale(0.9);
}

void
CQChartsFontSizeRangeSlider::
setPlot(CQChartsPlot *plot)
{
  plot_ = plot;

  auto *pointPlot = dynamic_cast<CQChartsPointPlot *>(plot_.data());

  if (pointPlot) {
    setRangeMinMax(pointPlot->fontSizeMapMin(), pointPlot->fontSizeMapMax(),
                   /*reset*/false);
    setSliderMinMax(pointPlot->fontSizeUserMapMin(), pointPlot->fontSizeUserMapMax(),
                    /*reset*/false);
  }
  else
    setRangeMinMax(CQChartsFontSize::minValue(), CQChartsFontSize::maxValue(),
                   /*reset*/false);

  update();
}
