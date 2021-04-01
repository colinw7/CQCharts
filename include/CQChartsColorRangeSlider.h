#ifndef CQChartsColorRangeSlider_H
#define CQChartsColorRangeSlider_H

#include <CQChartsPaletteName.h>
#include <CQDoubleRangeSlider.h>

class CQChartsPlot;

class CQChartsColorRangeSlider : public CQDoubleRangeSlider {
  Q_OBJECT

 public:
  CQChartsColorRangeSlider(QWidget *parent=nullptr);

  void setPlot(CQChartsPlot *plot);

  void setPaletteName(const CQChartsPaletteName &paletteName);

 public:
  CQChartsPlot*       plot_ { nullptr };
  CQChartsPaletteName paletteName_;
};

#endif
