#ifndef CQChartsFontSizeRangeSlider_H
#define CQChartsFontSizeRangeSlider_H

#include <CQDoubleRangeSlider.h>

#include <QPointer>

class CQChartsPlot;

class CQChartsFontSizeRangeSlider : public CQDoubleRangeSlider {
  Q_OBJECT

 public:
  using Plot = CQChartsPlot;

 public:
  CQChartsFontSizeRangeSlider(QWidget *parent=nullptr);

  void setPlot(Plot *plot);

 public:
  using PlotP = QPointer<Plot>;

  PlotP plot_;
};

#endif
