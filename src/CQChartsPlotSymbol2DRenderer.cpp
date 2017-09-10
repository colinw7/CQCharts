#include <CQChartsPlotSymbol2DRenderer.h>
#include <CQChartsPlot.h>
#include <QPainter>

CQChartsPlotSymbol2DRenderer::
CQChartsPlotSymbol2DRenderer(CQChartsPlot *plot, QPainter *painter, const CPoint2D &p, double s) :
 plot_(plot), painter_(painter), p_(p), s_(s)
{
  pc_ = painter_->pen  ().color();
  fc_ = painter_->brush().color();

  plot_->windowToPixel(p_, px_);
}

void
CQChartsPlotSymbol2DRenderer::
moveTo(double x, double y)
{
  path_.moveTo(px_.x + x*s_, px_.y - y*s_);
}

void
CQChartsPlotSymbol2DRenderer::
lineTo(double x, double y)
{
  path_.lineTo(px_.x + x*s_, px_.y - y*s_);
}

void
CQChartsPlotSymbol2DRenderer::
closePath()
{
  path_.closeSubpath();
}

void
CQChartsPlotSymbol2DRenderer::
stroke()
{
  painter_->strokePath(path_, QPen(pc_));
}

void
CQChartsPlotSymbol2DRenderer::
fill()
{
  painter_->fillPath(path_, QBrush(fc_));
}

void
CQChartsPlotSymbol2DRenderer::
strokeCircle(double x, double y, double r)
{
  QRectF rect(px_.x + (x - r)*s_, px_.y + (y - r)*s_, 2*r*s_, 2*r*s_);

  painter_->save();

  painter_->setBrush(Qt::NoBrush);
  painter_->setPen  (pc_);

  painter_->drawEllipse(rect);

  painter_->restore();
}

void
CQChartsPlotSymbol2DRenderer::
fillCircle(double x, double y, double r)
{
  QRectF rect(px_.x + (x - r)*s_, px_.y + (y - r)*s_, 2*r*s_, 2*r*s_);

  painter_->save();

  painter_->setBrush(fc_);
  painter_->setPen  (Qt::NoPen);

  painter_->drawEllipse(rect);

  painter_->restore();
}

double
CQChartsPlotSymbol2DRenderer::
lineWidth() const
{
  return w_;
}
