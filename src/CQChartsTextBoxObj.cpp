#include <CQChartsTextBoxObj.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

#include <QPainter>

CQChartsTextBoxObj::
CQChartsTextBoxObj(CQChartsView *view) :
 CQChartsBoxObj(view),
 CQChartsObjTextData<CQChartsTextBoxObj>(this)
{
  CQChartsColor themeFg(CQChartsColor::Type::INTERFACE_VALUE, 1);

  setTextColor(themeFg);

  setBorder(false);
  setFilled(false);
}

CQChartsTextBoxObj::
CQChartsTextBoxObj(CQChartsPlot *plot) :
 CQChartsBoxObj(plot),
 CQChartsObjTextData<CQChartsTextBoxObj>(this)
{
  CQChartsColor themeFg(CQChartsColor::Type::INTERFACE_VALUE, 1);

  setTextColor(themeFg);

  setBorder(false);
  setFilled(false);
}

void
CQChartsTextBoxObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  CQChartsBoxObj::addProperties(model, path);

  QString textPath = path + "/text";

  model->addProperty(textPath, this, "textStr", "text");

  addTextDataProperties(model, textPath);
}

void
CQChartsTextBoxObj::
addTextDataProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "textColor"   , "color"   )->setDesc("Text color");
  model->addProperty(path, this, "textAlpha"   , "alpha"   )->setDesc("Text alpha");
  model->addProperty(path, this, "textFont"    , "font"    )->setDesc("Text font");
  model->addProperty(path, this, "textAngle"   , "angle"   );
  model->addProperty(path, this, "textContrast", "contrast");
  model->addProperty(path, this, "textHtml"    , "html"    );
  model->addProperty(path, this, "textAlign"   , "align"   );
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

  //---

  // set text options
  CQChartsTextOptions textOptions;

  textOptions.contrast  = isTextContrast();
  textOptions.formatted = isTextFormatted();
  textOptions.scaled    = isTextScaled();
  textOptions.html      = isTextHtml();
  textOptions.align     = textAlign();

  if (plot())
    textOptions = plot()->adjustTextOptions(textOptions);

  //---

  QPointF tp(rect.left() + margin(), rect.bottom() - margin() - fm.descent());

  CQChartsDrawUtil::drawTextAtPoint(painter, tp, text, textOptions);
}

void
CQChartsTextBoxObj::
textBoxDataInvalidate()
{
  boxDataInvalidate();
}
