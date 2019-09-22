#include <CQChartsViewPlotObj.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>

CQChartsViewPlotObj::
CQChartsViewPlotObj(CQChartsView *view) :
 CQChartsObj(view->charts()), view_(view)
{
}

CQChartsViewPlotObj::
CQChartsViewPlotObj(CQChartsPlot *plot) :
 CQChartsObj(plot->charts()), plot_(plot)
{
}

CQCharts *
CQChartsViewPlotObj::
charts() const
{
  return view()->charts();
}

CQChartsView *
CQChartsViewPlotObj::
view() const
{
  return (plot_ ? plot_->view() : view_);
}

//---

void
CQChartsViewPlotObj::
setPen(QPen &pen, bool stroked, const QColor &strokeColor, double strokeAlpha,
       const CQChartsLength &strokeWidth, const CQChartsLineDash &strokeDash) const
{
  if      (plot())
    plot()->setPen(pen, stroked, strokeColor, strokeAlpha, strokeWidth, strokeDash);
  else if (view())
    view()->setPen(pen, stroked, strokeColor, strokeAlpha, strokeWidth, strokeDash);
}

void
CQChartsViewPlotObj::
setBrush(QBrush &brush, bool filled, const QColor &fillColor, double fillAlpha,
         const CQChartsFillPattern &pattern) const
{
  if      (plot())
    plot()->setBrush(brush, filled, fillColor, fillAlpha, pattern);
  else if (view())
    view()->setBrush(brush, filled, fillColor, fillAlpha, pattern);
}

void
CQChartsViewPlotObj::
updatePenBrushState(QPen &pen, QBrush &brush, DrawType drawType) const
{
  if      (plot())
    plot()->updateObjPenBrushState(this, pen, brush, drawType);
  else if (view())
    view()->updateObjPenBrushState(this, pen, brush, drawType);
}

//---

QFont
CQChartsViewPlotObj::
calcFont(const CQChartsFont &font) const
{
  QFont font1;

  if      (plot())
    font1 = view()->plotFont(plot(), font);
  else if (view())
    font1 = view()->viewFont(font);

  return font1;
}

void
CQChartsViewPlotObj::
setPainterFont(CQChartsPaintDevice *device, const CQChartsFont &font) const
{
  if      (plot())
    view()->setPlotPainterFont(plot(), device, font);
  else if (view())
    view()->setPainterFont(device, font);
}

#if 0
void
CQChartsViewPlotObj::
setPainterFont(QPainter *painter, const CQChartsFont &font) const
{
  if      (plot())
    view()->setPlotPainterFont(plot(), painter, font);
  else if (view())
    view()->setPainterFont(painter, font);
}
#endif

void
CQChartsViewPlotObj::
adjustTextOptions(CQChartsTextOptions &textOptions) const
{
  if (plot())
    textOptions = plot()->adjustTextOptions(textOptions);
}

//---

QPointF
CQChartsViewPlotObj::
positionToParent(const CQChartsPosition &pos) const
{
  QPointF p;

  if      (plot())
    p = plot()->positionToPlot(pos);
  else if (view())
    p = view()->positionToView(pos);

  return p;
}

double
CQChartsViewPlotObj::
lengthParentWidth(const CQChartsLength &len) const
{
  double w = 1.0;

  if      (plot())
    w = plot()->lengthPlotWidth(len);
  else if (view())
    w = view()->lengthViewWidth(len);

  return w;
}

double
CQChartsViewPlotObj::
lengthParentHeight(const CQChartsLength &len) const
{
  double h = 1.0;

  if      (plot())
    h = plot()->lengthPlotHeight(len);
  else if (view())
    h = view()->lengthViewHeight(len);

  return h;
}

double
CQChartsViewPlotObj::
lengthPixelWidth(const CQChartsLength &len) const
{
  double w = 1.0;

  if      (plot())
    w = plot()->lengthPixelWidth(len);
  else if (view())
    w = view()->lengthPixelWidth(len);

  return w;
}

double
CQChartsViewPlotObj::
lengthPixelHeight(const CQChartsLength &len) const
{
  double h = 1.0;

  if      (plot())
    h = plot()->lengthPixelHeight(len);
  else if (view())
    h = view()->lengthPixelHeight(len);

  return h;
}

CQChartsGeom::Point
CQChartsViewPlotObj::
windowToPixel(const CQChartsGeom::Point &w) const
{
  CQChartsGeom::Point p;

  if      (plot())
    p = plot()->windowToPixel(w);
  else if (view())
    p = view()->windowToPixel(w);

  return p;
}

CQChartsGeom::BBox
CQChartsViewPlotObj::
windowToPixel(const CQChartsGeom::BBox &w) const
{
  CQChartsGeom::BBox p;

  if      (plot())
    p = plot()->windowToPixel(w);
  else if (view())
    p = view()->windowToPixel(w);

  return p;
}

CQChartsGeom::Point
CQChartsViewPlotObj::
pixelToWindow(const CQChartsGeom::Point &w) const
{
  CQChartsGeom::Point p;

  if      (plot())
    p = plot()->pixelToWindow(w);
  else if (view())
    p = view()->pixelToWindow(w);

  return p;
}

CQChartsGeom::BBox
CQChartsViewPlotObj::
pixelToWindow(const CQChartsGeom::BBox &w) const
{
  CQChartsGeom::BBox p;

  if      (plot())
    p = plot()->pixelToWindow(w);
  else if (view())
    p = view()->pixelToWindow(w);

  return p;
}

double
CQChartsViewPlotObj::
pixelToWindowWidth(double pw) const
{
  double w = 0.0;

  if      (plot())
    w = plot()->pixelToWindowWidth(pw);
  else if (view())
    w = view()->pixelToWindowWidth(pw);

  return w;
}

double
CQChartsViewPlotObj::
pixelToWindowHeight(double ph) const
{
  double h = 0.0;

  if      (plot())
    h = plot()->pixelToWindowHeight(ph);
  else if (view())
    h = view()->pixelToWindowHeight(ph);

  return h;
}

QColor
CQChartsViewPlotObj::
backgroundColor() const
{
  CQChartsUtil::ColorInd ci;

  QColor bg;

  if      (plot())
    bg = plot()->view()->interpBackgroundFillColor(ci);
  else if (view())
    bg = view()->interpBackgroundFillColor(ci);

  return bg;
}
