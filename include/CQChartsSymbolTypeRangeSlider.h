#ifndef CQChartsSymbolTypeRangeSlider_H
#define CQChartsSymbolTypeRangeSlider_H

#include <CQIntRangeSlider.h>

class CQChartsPlot;

class CQChartsSymbolTypeRangeSlider : public CQIntRangeSlider {
  Q_OBJECT

  Q_PROPERTY(QColor fillColor   READ fillColor   WRITE setFillColor  )
  Q_PROPERTY(QColor strokeColor READ strokeColor WRITE setStrokeColor)

 public:
  CQChartsSymbolTypeRangeSlider(QWidget *parent=nullptr);

  void setPlot(CQChartsPlot *plot);

  void setSymbolSetName(const QString &name);

  const QColor &fillColor() const { return fillColor_; }
  void setFillColor(const QColor &c) { fillColor_ = c; }

  const QColor &strokeColor() const { return strokeColor_; }
  void setStrokeColor(const QColor &c) { strokeColor_ = c; }

  void drawSliderLabels(QPainter *painter) override;

 public:
  CQChartsPlot *plot_          { nullptr };
  QString       symbolSetName_;
  QColor        fillColor_;
  QColor        strokeColor_;
};

#endif
