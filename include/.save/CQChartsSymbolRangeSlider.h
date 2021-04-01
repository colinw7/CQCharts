#ifndef CQChartsSymbolRangeSlider_H
#define CQChartsSymbolRangeSlider_H

#include <CQIntRangeSlider.h>

class CQChartsPlot;

class CQChartsSymbolRangeSlider : public CQIntRangeSlider {
  Q_OBJECT

  Q_PROPERTY(QColor fillColor READ fillColor WRITE setFillColor)

 public:
  CQChartsSymbolRangeSlider(QWidget *parent=nullptr);

  void setPlot(CQChartsPlot *plot);

  void setSymbolSetName(const QString &name);

  const QColor &fillColor() const { return fillColor_; }
  void setFillColor(const QColor &c) { fillColor_ = c; }

  void drawSliderLabels(QPainter *painter) override;

 public:
  CQChartsPlot *plot_          { nullptr };
  QString       symbolSetName_;
  QColor        fillColor_;
};

#endif
