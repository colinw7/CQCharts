#ifndef CQChartsSymbolRangeSlider_H
#define CQChartsSymbolRangeSlider_H

#include <CQIntRangeSlider.h>

class CQChartsPlot;

class CQChartsSymbolRangeSlider : public CQIntRangeSlider {
 public:
  CQChartsSymbolRangeSlider(QWidget *parent=nullptr);

  void setPlot(CQChartsPlot *plot);

  void setSymbolSetName(const QString &name);

  void drawSliderLabels(QPainter *painter) override;

 public:
  CQChartsPlot *plot_          { nullptr };
  QString       symbolSetName_;
};

#endif
