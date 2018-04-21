#include <CQChartsBoxObj.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQPropertyViewModel.h>
#include <CQChartsRoundedPolygon.h>
#include <QPainter>

CQChartsBoxObj::
CQChartsBoxObj(CQChartsView *view) :
 QObject(view), view_(view)
{
}

CQChartsBoxObj::
CQChartsBoxObj(CQChartsPlot *plot) :
 QObject(plot), plot_(plot)
{
}

QColor
CQChartsBoxObj::
interpBackgroundColor(int i, int n) const
{
  if      (plot_)
    return backgroundColor().interpColor(plot_, i, n);
  else if (view_)
    return backgroundColor().interpColor(view_, i, n);
  else
    return QColor();
}

QColor
CQChartsBoxObj::
interpBorderColor(int i, int n) const
{
  if      (plot_)
    return borderColor().interpColor(plot_, i, n);
  else if (view_)
    return borderColor().interpColor(view_, i, n);
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
    QColor bgColor = interpBackgroundColor(0, 1);

    bgColor.setAlphaF(backgroundAlpha());

    QBrush brush(bgColor);

    brush.setStyle(CQChartsFillPattern::toStyle(
     (CQChartsFillPattern::Type) backgroundPattern()));

    painter->setBrush(brush);
    painter->setPen  (Qt::NoPen);

    double cxs = (plot_ ? plot_->lengthPixelWidth (cornerSize()) : 0.0);
    double cys = (plot_ ? plot_->lengthPixelHeight(cornerSize()) : 0.0);

    CQChartsRoundedPolygon::draw(painter, rect, cxs, cys);
  }

  if (isBorder()) {
    QColor borderColor = interpBorderColor(0, 1);

    borderColor.setAlphaF(borderAlpha());

    QPen pen(borderColor);

    double bw = (plot_ ? plot_->lengthPixelWidth(borderWidth()) : 0.0);

    pen.setWidthF(bw);

    painter->setPen  (pen);
    painter->setBrush(Qt::NoBrush);

    double cxs = (plot_ ? plot_->lengthPixelWidth (cornerSize()) : 0.0);
    double cys = (plot_ ? plot_->lengthPixelHeight(cornerSize()) : 0.0);

    CQChartsRoundedPolygon::draw(painter, rect, cxs, cys);
  }
}

void
CQChartsBoxObj::
redrawBoxObj()
{
  if      (plot_)
    plot_->update();
  else if (view_)
    view_->update();
}

void
CQChartsBoxObj::
draw(QPainter *painter, const QPolygonF &poly) const
{
  if (isBackground()) {
    QColor bgColor = interpBackgroundColor(0, 1);

    bgColor.setAlphaF(backgroundAlpha());

    QBrush brush(bgColor);

    painter->setBrush(brush);
    painter->setPen  (Qt::NoPen);

    double cxs = (plot_ ? plot_->lengthPixelWidth (cornerSize()) : 0.0);
    double cys = (plot_ ? plot_->lengthPixelHeight(cornerSize()) : 0.0);

    CQChartsRoundedPolygon::draw(painter, poly, cxs, cys);
  }

  if (isBorder()) {
    QColor borderColor = interpBorderColor(0, 1);

    borderColor.setAlphaF(borderAlpha());

    QPen pen(borderColor);

    double bw = (plot_ ? plot_->lengthPixelWidth(borderWidth()) : 0.0);

    pen.setWidthF(bw);

    painter->setPen  (pen);
    painter->setBrush(Qt::NoBrush);

    double cxs = (plot_ ? plot_->lengthPixelWidth (cornerSize()) : 0.0);
    double cys = (plot_ ? plot_->lengthPixelHeight(cornerSize()) : 0.0);

    CQChartsRoundedPolygon::draw(painter, poly, cxs, cys);
  }
}
