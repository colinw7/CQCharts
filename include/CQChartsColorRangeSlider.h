#ifndef CQChartsColorRangeSlider_H
#define CQChartsColorRangeSlider_H

#include <CQChartsPaletteName.h>
#include <CQDoubleRangeSlider.h>

#include <QPointer>

class CQChartsPlot;

class CQChartsColorRangeSlider : public CQDoubleRangeSlider {
  Q_OBJECT

 public:
  using Plot = CQChartsPlot;

 public:
  CQChartsColorRangeSlider(QWidget *parent=nullptr);

  void setPlot(Plot *plot);

  void setPaletteName(const CQChartsPaletteName &paletteName);

 public:
  using PlotP = QPointer<Plot>;

  PlotP               plot_;
  CQChartsPaletteName paletteName_;
};

#endif
