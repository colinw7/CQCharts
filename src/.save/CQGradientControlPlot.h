#ifndef CQGradientControlPlot_H
#define CQGradientControlPlot_H

#include <CGradientPalette.h>
#include <QFrame>

class QPainter;

// Gradient Plot
class CQGradientControlPlot : public QFrame {
  Q_OBJECT

 public:
  struct Margin {
    double left   = 0.1;
    double bottom = 0.1;
    double right  = 0.2;
    double top    = 0.1;
  };

 public:
  CQGradientControlPlot(QWidget *parent, CGradientPalette *palette);
  CQGradientControlPlot(CGradientPalette *palette, QWidget *parent=0);

 ~CQGradientControlPlot();

  CGradientPalette *gradientPalette() { return palette_; }
  void setGradientPalette(CGradientPalette *pal);

  void paintEvent(QPaintEvent *);

  QSize sizeHint() const;

 private:
  void init();

  void drawAxis(QPainter *painter);

  void drawLine(QPainter *painter, double x1, double y1, double x2, double y2, const QPen &pen);

  void drawSymbol(QPainter *painter, double x, double y, const QPen &pen);

  void windowToPixel(double wx, double wy, double &px, double &py) const;
  void pixelToWindow(double px, double py, double &wx, double &wy) const;

 private:
  CGradientPalette* palette_ { nullptr };
  Margin            margin_;
};

#endif
