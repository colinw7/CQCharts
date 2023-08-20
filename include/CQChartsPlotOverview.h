#ifndef CQChartsPlotOverview_H
#define CQChartsPlotOverview_H

#include <CQChartsGeom.h>

#include <QFrame>
#include <QPointer>

class CQChartsPlot;

class CQChartsPlotOverview : public QFrame {
  Q_OBJECT

 public:
  using Point = CQChartsGeom::Point;

 public:
  CQChartsPlotOverview(CQChartsPlot *controls);

  CQChartsPlot *plot() const;
  void setPlot(CQChartsPlot *plot);

  void mousePressEvent  (QMouseEvent *) override;
  void mouseMoveEvent   (QMouseEvent *) override;
  void mouseReleaseEvent(QMouseEvent *) override;

  void wheelEvent(QWheelEvent *e) override;

  void contextMenuEvent(QContextMenuEvent *) override;

  void paintEvent(QPaintEvent *) override;

  QSize sizeHint() const override;

 private:
  Point pixelToPlot(const Point &pp) const;

 protected Q_SLOTS:
  void overviewChanged();

 private:
  using PlotP = QPointer<CQChartsPlot>;

  PlotP plot_;
  bool  pressed_     { false };
  int   pressButton_ { -1 };

  int overviewSize_  { 256 }; //!< overview size
};

#endif
