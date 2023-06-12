#ifndef CQChartsSymbolSizeRangeSlider_H
#define CQChartsSymbolSizeRangeSlider_H

#include <CQDoubleRangeSlider.h>

#include <QPointer>

class CQChartsPlot;

class CQChartsSymbolSizeRangeSlider : public CQDoubleRangeSlider {
  Q_OBJECT

 public:
  using Plot = CQChartsPlot;

 public:
  CQChartsSymbolSizeRangeSlider(QWidget *parent=nullptr);

  void setPlot(Plot *plot, bool force=true);

 public:
  using PlotP = QPointer<Plot>;

  PlotP plot_;
};

#endif
