#ifndef CQChartsSymbolTypeRangeSlider_H
#define CQChartsSymbolTypeRangeSlider_H

#include <CQIntRangeSlider.h>

#include <QPointer>

class CQChartsPlot;

class CQChartsSymbolTypeRangeSlider : public CQIntRangeSlider {
  Q_OBJECT

  Q_PROPERTY(QColor fillColor   READ fillColor   WRITE setFillColor  )
  Q_PROPERTY(QColor strokeColor READ strokeColor WRITE setStrokeColor)

 public:
  using Plot = CQChartsPlot;

 public:
  CQChartsSymbolTypeRangeSlider(QWidget *parent=nullptr);

  Plot *plot() const;
  void setPlot(Plot *plot);

  void setSymbolSetName(const QString &name);

  const QColor &fillColor() const { return fillColor_; }
  void setFillColor(const QColor &c) { fillColor_ = c; }

  const QColor &strokeColor() const { return strokeColor_; }
  void setStrokeColor(const QColor &c) { strokeColor_ = c; }

  void drawSliderLabels(QPainter *painter) override;

 public:
  using PlotP = QPointer<Plot>;

  PlotP   plot_;
  QString symbolSetName_;
  QColor  fillColor_;
  QColor  strokeColor_;
};

#endif
