#ifndef CQGradientControlPlot_H
#define CQGradientControlPlot_H

#include <CGradientPalette.h>
#include <QFrame>

class QPainter;

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
#ifdef CGRADIENT_EXPR
  CQGradientControlPlot(QWidget *parent=0, CExpr *expr=0);
  CQGradientControlPlot(CExpr *expr=0, QWidget *parent=0);
#else
  CQGradientControlPlot(QWidget *parent=0);
#endif

  CGradientPalette *gradientPalette() { return pal_; }
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
#ifdef CGRADIENT_EXPR
  CExpr*            expr_ { 0 };
#endif
  CGradientPalette* pal_ { 0 };
  Margin            margin_;
};

#endif
