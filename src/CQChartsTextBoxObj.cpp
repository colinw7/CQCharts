#include <CQChartsTextBoxObj.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQPropertyViewModel.h>
#include <QPainter>

CQChartsTextBoxObj::
CQChartsTextBoxObj(CQChartsView *view) :
 CQChartsBoxObj(view)
{
  CQChartsColor themeFg(CQChartsColor::Type::INTERFACE_VALUE, 1);

  setTextColor(themeFg);

  setBorder(false);
  setFilled(false);
}

CQChartsTextBoxObj::
CQChartsTextBoxObj(CQChartsPlot *plot) :
 CQChartsBoxObj(plot)
{
  CQChartsColor themeFg(CQChartsColor::Type::INTERFACE_VALUE, 1);

  setTextColor(themeFg);

  setBorder(false);
  setFilled(false);
}

QColor
CQChartsTextBoxObj::
interpTextColor(int i, int n) const
{
  return charts()->interpColor(textColor(), i, n);
}

void
CQChartsTextBoxObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  CQChartsBoxObj::addProperties(model, path);

  QString textPath = path + "/text";

  model->addProperty(textPath, this, "textStr"     , "text"    );
  model->addProperty(textPath, this, "textFont"    , "font"    );
  model->addProperty(textPath, this, "textColor"   , "color"   );
  model->addProperty(textPath, this, "textAlpha"   , "alpha"   );
  model->addProperty(textPath, this, "textAngle"   , "angle"   );
  model->addProperty(textPath, this, "textContrast", "contrast");
  model->addProperty(textPath, this, "textAlign"   , "align"   );
}

void
CQChartsTextBoxObj::
draw(QPainter *painter, const QRectF &rect) const
{
  if (! isTextVisible())
    return;

  CQChartsBoxObj::draw(painter, rect);

  //---

  drawText(painter, rect, textStr());
}

void
CQChartsTextBoxObj::
draw(QPainter *painter, const QPolygonF &poly) const
{
  CQChartsBoxObj::draw(painter, poly);

  //---

  QRectF rect = poly.boundingRect();

  drawText(painter, rect, textStr());
}

void
CQChartsTextBoxObj::
drawText(QPainter *painter, const QRectF &rect, const QString &text) const
{
  if      (plot())
    view()->setPlotPainterFont(plot(), painter, textFont());
  else if (view())
    view()->setPainterFont(painter, textFont());

  QFontMetricsF fm(painter->font());

  QColor c = interpTextColor(0, 1);

  QPen pen;

  if      (plot())
    plot()->setPen(pen, true, c, textAlpha());
  else if (view())
    view()->setPen(pen, true, c, textAlpha());

  painter->setPen(pen);

  painter->drawText(QPointF(rect.left() + margin(),
                            rect.bottom() - margin() - fm.descent()), text);
}
