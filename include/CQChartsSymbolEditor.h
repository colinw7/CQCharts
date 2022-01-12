#ifndef CQChartsSymbolEditor_H
#define CQChartsSymbolEditor_H

#include <CQChartsDisplayRange.h>
#include <CQChartsSymbol.h>

#include <QFrame>

class CQChartsViewSettings;

class CQChartsSymbolEditor : public QFrame {
  Q_OBJECT

 public:
  using Symbol = CQChartsSymbol;

 public:
  CQChartsSymbolEditor(CQChartsViewSettings *viewSettings);

  void setSymbol(const Symbol &symbol);

  //--

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

  void drawGrid  (QPainter *painter);
  void drawSymbol(QPainter *painter);
  void drawGuides(QPainter *painter);

  void mousePressEvent  (QMouseEvent *me) override;
  void mouseMoveEvent   (QMouseEvent *me) override;
  void mouseReleaseEvent(QMouseEvent *me) override;

  void keyPressEvent(QKeyEvent *e) override;

  void updateMousePos(const QPoint &pos);

  QSize sizeHint() const override;

 private:
  void pixelToWindow(double px, double py, double &wx, double &wy);

 private:
  struct Point {
   QPointF p;
   bool    skip { false };

   Point() = default;

   Point(const QPointF &p) : p(p) { }
   Point(double x, double y) : p(x, y) { }

   double x() const { return p.x(); }
   double y() const { return p.y(); }
  };

  using Points      = std::vector<Point>;
  using PointsArray = std::vector<Points>;

  CQChartsViewSettings* viewSettings_  { nullptr };
  Symbol                symbol_;
  CQChartsDisplayRange  range_;
  PointsArray           pointsArray_;
  QPointF               pressPos_;
  QPointF               pointPos_;
  QPointF               mousePos_;
  int                   mouseArrayInd_ { 0 };
  int                   mouseInd_      { 0 };
  bool                  pressed_       { false };
  bool                  escape_        { false };
  Qt::MouseButton       button_        { Qt::LeftButton };
};

#endif
