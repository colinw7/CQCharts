#ifndef CQChartsFontSizeRangeSlider_H
#define CQChartsFontSizeRangeSlider_H

#include <CQDoubleRangeSlider.h>

class CQChartsPlot;

class CQChartsFontSizeRangeSlider : public CQDoubleRangeSlider {
  Q_OBJECT

 public:
  CQChartsFontSizeRangeSlider(QWidget *parent=nullptr);

  void setPlot(CQChartsPlot *plot);

 public:
  CQChartsPlot *plot_ { nullptr };
};

#endif
