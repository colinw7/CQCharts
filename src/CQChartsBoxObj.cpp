#include <CQChartsBoxObj.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQPropertyViewModel.h>
#include <CQChartsRoundedPolygon.h>
#include <QPainter>

CQChartsBoxObj::
CQChartsBoxObj(CQChartsView *view) :
 CQChartsObj(view), view_(view)
{
}

CQChartsBoxObj::
CQChartsBoxObj(CQChartsPlot *plot) :
 CQChartsObj(plot), plot_(plot)
{
}

CQChartsView *
CQChartsBoxObj::
view() const
{
  if (plot())
    return plot()->view();
  else
    return view_;
}

QColor
CQChartsBoxObj::
interpBackgroundColor(int i, int n) const
{
  if      (plot())
    return backgroundColor().interpColor(plot(), i, n);
  else if (view())
    return backgroundColor().interpColor(view(), i, n);
  else
    return QColor();
}

QColor
CQChartsBoxObj::
interpBorderColor(int i, int n) const
{
  if      (plot())
    return borderColor().interpColor(plot(), i, n);
  else if (view())
    return borderColor().interpColor(view(), i, n);
  else
    return QColor();
}

void
CQChartsBoxObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "margin" );
  model->addProperty(path, this, "padding");

  QString bgPath = path + "/background";

  model->addProperty(bgPath, this, "background"       , "visible");
  model->addProperty(bgPath, this, "backgroundColor"  , "color"  );
  model->addProperty(bgPath, this, "backgroundAlpha"  , "alpha"  );
  model->addProperty(bgPath, this, "backgroundPattern", "pattern");

  QString borderPath = path + "/border";

  model->addProperty(borderPath, this, "border"     , "visible"   );
  model->addProperty(borderPath, this, "borderColor", "color"     );
  model->addProperty(borderPath, this, "borderAlpha", "alpha"     );
  model->addProperty(borderPath, this, "borderWidth", "width"     );
  model->addProperty(borderPath, this, "cornerSize" , "cornerSize");
  model->addProperty(borderPath, this, "borderSides", "sides"     );
}

void
CQChartsBoxObj::
draw(QPainter *painter, const QRectF &rect) const
{
  if (isBackground()) {
    QBrush brush;
    QPen   pen(Qt::NoPen);

    QColor bgColor = interpBackgroundColor(0, 1);

    if      (plot())
      plot()->setBrush(brush, true, bgColor, backgroundAlpha(), backgroundPattern());
    else if (view())
      view()->setBrush(brush, true, bgColor, backgroundAlpha(), backgroundPattern());

    if (plot())
      plot()->updateObjPenBrushState(this, pen, brush);

    painter->setPen  (pen);
    painter->setBrush(brush);

    double cxs = (plot() ? plot()->lengthPixelWidth (cornerSize()) : 0.0);
    double cys = (plot() ? plot()->lengthPixelHeight(cornerSize()) : 0.0);

    CQChartsRoundedPolygon::draw(painter, rect, cxs, cys);
  }

  if (isBorder()) {
    QPen   pen;
    QBrush brush(Qt::NoBrush);

    QColor borderColor = interpBorderColor(0, 1);

    if      (plot())
      plot()->setPen(pen, true, borderColor, borderAlpha(), borderWidth(), CQChartsLineDash());
    else if (view())
      view()->setPen(pen, true, borderColor, borderAlpha(), borderWidth(), CQChartsLineDash());

    if (plot())
      plot()->updateObjPenBrushState(this, pen, brush);

    painter->setPen  (pen);
    painter->setBrush(brush);

    double cxs = (plot() ? plot()->lengthPixelWidth (cornerSize()) : 0.0);
    double cys = (plot() ? plot()->lengthPixelHeight(cornerSize()) : 0.0);

    CQChartsRoundedPolygon::draw(painter, rect, cxs, cys);
  }
}

void
CQChartsBoxObj::
redrawBoxObj()
{
  if      (plot())
    plot()->invalidateLayers();
  else if (view())
    view()->update();
}

void
CQChartsBoxObj::
draw(QPainter *painter, const QPolygonF &poly) const
{
  if (isBackground()) {
    QBrush brush;
    QPen   pen(Qt::NoPen);

    QColor bgColor = interpBackgroundColor(0, 1);

    if      (plot())
      plot()->setBrush(brush, true, bgColor, backgroundAlpha(), backgroundPattern());
    else if (view())
      view()->setBrush(brush, true, bgColor, backgroundAlpha(), backgroundPattern());

    painter->setPen  (pen);
    painter->setBrush(brush);

    double cxs = (plot() ? plot()->lengthPixelWidth (cornerSize()) : 0.0);
    double cys = (plot() ? plot()->lengthPixelHeight(cornerSize()) : 0.0);

    CQChartsRoundedPolygon::draw(painter, poly, cxs, cys);
  }

  if (isBorder()) {
    QPen   pen;
    QBrush brush(Qt::NoBrush);

    QColor borderColor = interpBorderColor(0, 1);

    if      (plot())
      plot()->setPen(pen, true, borderColor, borderAlpha(), borderWidth(), CQChartsLineDash());
    else if (view())
      view()->setPen(pen, true, borderColor, borderAlpha(), borderWidth(), CQChartsLineDash());

    painter->setPen  (pen);
    painter->setBrush(brush);

    double cxs = (plot() ? plot()->lengthPixelWidth (cornerSize()) : 0.0);
    double cys = (plot() ? plot()->lengthPixelHeight(cornerSize()) : 0.0);

    CQChartsRoundedPolygon::draw(painter, poly, cxs, cys);
  }
}
