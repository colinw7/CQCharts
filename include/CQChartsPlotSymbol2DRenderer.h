#ifndef CQChartsPlotSymbol2DRenderer_H
#define CQChartsPlotSymbol2DRenderer_H

#include <CSymbol2D.h>
#include <CPoint2D.h>
#include <QPainterPath>
#include <QColor>

class CQChartsPlot;
class QPainter;

class CQChartsPlotSymbol2DRenderer : public CSymbol2DRenderer {
 public:
  CQChartsPlotSymbol2DRenderer(CQChartsPlot *plot, QPainter *painter,
                               const CPoint2D &p, double s);

  void moveTo(double x, double y) override;

  void lineTo(double x, double y) override;

  void closePath() override;

  void stroke() override;

  void fill() override;

  void strokeCircle(double x, double y, double r) override;

  void fillCircle(double x, double y, double r) override;

  double lineWidth() const override;

 private:
  CQChartsPlot *plot_    { nullptr };
  QPainter     *painter_ { nullptr };
  CPoint2D      p_       { 0, 0 };
  CPoint2D      px_      { 0, 0 };
  double        s_       { 2.0 };
  double        w_       { 0.0 };
  QPainterPath  path_;
  QColor        pc_;
  QColor        fc_;
};

#endif
