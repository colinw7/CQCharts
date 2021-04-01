#ifndef CQChartsSymbolSizeRangeSlider_H
#define CQChartsSymbolSizeRangeSlider_H

#include <CQDoubleRangeSlider.h>

class CQChartsPlot;

class CQChartsSymbolSizeRangeSlider : public CQDoubleRangeSlider {
  Q_OBJECT

 public:
  CQChartsSymbolSizeRangeSlider(QWidget *parent=nullptr);

  void setPlot(CQChartsPlot *plot);

 public:
  CQChartsPlot *plot_ { nullptr };
};

#endif
