#include <CQChartsBoxObj.h>
#include <CQChartsPlot.h>
#include <CQPropertyViewModel.h>
#include <CQRoundedPolygon.h>
#include <QPainter>

CQChartsBoxObj::
CQChartsBoxObj(CQChartsPlot *plot) :
 plot_(plot)
{
  CQChartsPaletteColor themeBg(CQChartsPaletteColor::Type::THEME_VALUE, 0);
  CQChartsPaletteColor themeFg(CQChartsPaletteColor::Type::THEME_VALUE, 1);

  setBackgroundColor(themeBg);
  setBorderColor    (themeFg);
}

QColor
CQChartsBoxObj::
interpBackgroundColor(int i, int n) const
{
  return backgroundColor_.interpColor(plot_, i, n);
}

QColor
CQChartsBoxObj::
interpBorderColor(int i, int n) const
{
  return borderColor_.interpColor(plot_, i, n);
}

void
CQChartsBoxObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "margin" );
  model->addProperty(path, this, "padding");

  QString bgPath = path + "/background";

  model->addProperty(bgPath, this, "background"     , "visible");
  model->addProperty(bgPath, this, "backgroundColor", "color"  );
  model->addProperty(bgPath, this, "backgroundAlpha", "alpha"  );

  QString borderPath = path + "/border";

  model->addProperty(borderPath, this, "border"          , "visible"   );
  model->addProperty(borderPath, this, "borderColor"     , "color"     );
  model->addProperty(borderPath, this, "borderAlpha"     , "alpha"     );
  model->addProperty(borderPath, this, "borderWidth"     , "width"     );
  model->addProperty(borderPath, this, "borderCornerSize", "cornerSize");
}

void
CQChartsBoxObj::
draw(QPainter *p, const QRectF &rect) const
{
  if (isBackground()) {
    QColor bgColor = interpBackgroundColor(0, 1);

    bgColor.setAlphaF(backgroundAlpha());

    QBrush brush(bgColor);

    p->setBrush(brush);
    p->setPen  (Qt::NoPen);

    CQRoundedPolygon::draw(p, rect, borderCornerSize());
  }

  if (isBorder()) {
    QColor borderColor = interpBorderColor(0, 1);

    borderColor.setAlphaF(borderAlpha());

    QPen pen(borderColor);

    pen.setWidthF(borderWidth());

    p->setPen  (pen);
    p->setBrush(Qt::NoBrush);

    CQRoundedPolygon::draw(p, rect, borderCornerSize());
  }
}

void
CQChartsBoxObj::
redrawBoxObj()
{
  plot_->update();
}

void
CQChartsBoxObj::
draw(QPainter *p, const QPolygonF &poly) const
{
  if (isBackground()) {
    QColor bgColor = interpBackgroundColor(0, 1);

    bgColor.setAlphaF(backgroundAlpha());

    QBrush brush(bgColor);

    p->setBrush(brush);
    p->setPen  (Qt::NoPen);

    CQRoundedPolygon::draw(p, poly, borderCornerSize());
  }

  if (isBorder()) {
    QColor borderColor = interpBorderColor(0, 1);

    borderColor.setAlphaF(borderAlpha());

    QPen pen(borderColor);

    pen.setWidthF(borderWidth());

    p->setPen  (pen);
    p->setBrush(Qt::NoBrush);

    CQRoundedPolygon::draw(p, poly, borderCornerSize());
  }
}
