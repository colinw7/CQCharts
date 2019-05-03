#ifndef CQChartsGradientPaletteCanvas_H
#define CQChartsGradientPaletteCanvas_H

#include <QFrame>

class CQChartsGradientPalette;
class QPainter;
class QLabel;

//! \brief Gradient Plot
class CQChartsGradientPaletteCanvas : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool showPoints   READ isShowPoints   WRITE setShowPoints  )
  Q_PROPERTY(bool showLines    READ isShowLines    WRITE setShowLines   )
  Q_PROPERTY(bool showColorBar READ isShowColorBar WRITE setShowColorBar)
  Q_PROPERTY(bool gray         READ isGray         WRITE setGray        )

 public:
  struct Margin {
    double left   = 0.1;
    double bottom = 0.1;
    double right  = 0.2;
    double top    = 0.1;
  };

 public:
  CQChartsGradientPaletteCanvas(QWidget *parent, CQChartsGradientPalette *palette);
  CQChartsGradientPaletteCanvas(CQChartsGradientPalette *palette, QWidget *parent=0);

 ~CQChartsGradientPaletteCanvas();

  //---

  CQChartsGradientPalette *gradientPalette() { return palette_; }
  void setGradientPalette(CQChartsGradientPalette *pal);

  bool isShowPoints  () const { return showPoints_  ; }
  bool isShowLines   () const { return showLines_   ; }
  bool isShowColorBar() const { return showColorBar_; }
  bool isGray        () const { return gray_        ; }

  //---

  void enterEvent(QEvent *) override;
  void leaveEvent(QEvent *) override;

  void mousePressEvent  (QMouseEvent *) override;
  void mouseMoveEvent   (QMouseEvent *) override;
  void mouseReleaseEvent(QMouseEvent *) override;

  void contextMenuEvent(QContextMenuEvent *e) override;

  void paintEvent(QPaintEvent *) override;

  //---

  QSize sizeHint() const override;

 private:
  struct NearestColor {
    int    i = -1;
    double d = 0.0;
    int    c = 0;
    QColor color;
  };

 private:
  void init();

  void showTipText();
  void hideTipText();

  void drawAxis(QPainter *painter);

  void drawLine(QPainter *painter, double x1, double y1, double x2, double y2, const QPen &pen);

  void drawSymbol(QPainter *painter, double x, double y, const QPen &pen);

  void nearestDefinedColor(const QPointF &p, NearestColor &nearestColor);

  void moveNearestDefinedColor(const NearestColor &nearestColor, double dy);

  QPointF pixelToWindow(const QPoint &p);

  void windowToPixel(double wx, double wy, double &px, double &py) const;
  void pixelToWindow(double px, double py, double &wx, double &wy) const;

 public slots:
  void setShowPoints(bool b);
  void setShowLines(bool b);
  void setShowColorBar(bool b);
  void setGray(bool b);

 signals:
  void colorsChanged();

 private:
  struct MouseData {
    bool    pressed { false };
    QPointF pressPos;
    QPointF movePos;
  };

  CQChartsGradientPalette* palette_       { nullptr }; //!< palette to edit
  Margin                   margin_;                    //!< canvas margin
  QLabel*                  tipText_       { nullptr }; //!< tip text widget
  MouseData                mouseData_;                 //!< mouse state data
  NearestColor             nearestColor_;              //!< nearest color
  bool                     showPoints_    { true };    //!< show points on plot
  bool                     showLines_     { true };    //!< show lines on plot
  bool                     showColorBar_  { true };    //!< show color bar
  bool                     gray_          { false };   //!< is gray
};

#endif
