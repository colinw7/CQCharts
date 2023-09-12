#include <CQChartsSymbolSizeRangeSlider.h>
#include <CQChartsPointPlot.h>
#include <CQCharts.h>

#include <QPainter>

CQChartsSymbolSizeRangeSlider::
CQChartsSymbolSizeRangeSlider(QWidget *parent) :
 CQDoubleRangeSlider(parent)
{
  auto font = this->font();

  setTextFontScale(0.9);
}

void
CQChartsSymbolSizeRangeSlider::
setPlot(CQChartsPlot *plot, bool force)
{
  if (! force && plot == plot_)
    return;

  plot_ = plot;

  auto *pointPlot = dynamic_cast<CQChartsPointPlot *>(plot_.data());

  if (pointPlot) {
    setRangeMinMax(pointPlot->symbolSizeMapMin(), pointPlot->symbolSizeMapMax(),
                   /*reset*/false);
    setSliderMinMax(pointPlot->symbolSizeUserMapMin(), pointPlot->symbolSizeUserMapMax(),
                    /*reset*/false);
  }
  else
    setRangeMinMax(CQChartsSymbolSize::minValue(), CQChartsSymbolSize::maxValue(),
                   /*reset*/false);

  update();
}
