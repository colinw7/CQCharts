#include <CQChartsColorRangeSlider.h>
#include <CQChartsPlot.h>
#include <CQColorsPalette.h>

#include <QPainter>

CQChartsColorRangeSlider::
CQChartsColorRangeSlider(QWidget *parent) :
 CQDoubleRangeSlider(parent)
{
  auto font = this->font();

  font.setPointSizeF(font.pointSizeF()*0.9);

  setTextFont(font);
}

void
CQChartsColorRangeSlider::
setPlot(CQChartsPlot *plot)
{
  plot_ = plot;

  setRangeMinMax(0.0, 1.0);
  setSliderMinMax(plot_->colorMapMin(), plot_->colorMapMax());

  update();
}

void
CQChartsColorRangeSlider::
setPaletteName(const CQChartsPaletteName &paletteName)
{
  paletteName_ = paletteName;

  if (! paletteName_.isValid())
    paletteName_ = plot_->defaultPalette();

  auto *colorPalette = paletteName.palette();

  if (colorPalette) {
    QLinearGradient lg(0, 0.5, 1, 0.5);

    lg.setCoordinateMode(QGradient::ObjectBoundingMode);

    colorPalette->setLinearGradient(lg, 1.0, paletteName.min(), paletteName.max(), isEnabled());

    setLinearGradient(lg);
  }
  else
    clearLinearGradient();

  update();
}
