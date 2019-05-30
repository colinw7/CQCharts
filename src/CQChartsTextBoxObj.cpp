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
addProperties(CQPropertyViewModel *model, const QString &path, const QString &desc)
{
  CQChartsBoxObj::addProperties(model, path, desc);

  QString textPath = path + "/text";

  QString desc1 = (desc.length() ? desc + " text" : "Text");

  model->addProperty(textPath, this, "textStr", "string")->setDesc(desc1 + " string");

  addTextDataProperties(model, textPath, desc);
}

void
CQChartsTextBoxObj::
addTextDataProperties(CQPropertyViewModel *model, const QString &path, const QString &desc,
                      bool addVisible)
{
  QString desc1 = (desc.length() ? desc + " text" : "Text");

  if (addVisible)
    model->addProperty(path, this, "textVisible", "visible")->setDesc(desc1 + " visible");

  model->addProperty(path, this, "textColor"   , "color"   )->setDesc(desc1 + " color");
  model->addProperty(path, this, "textAlpha"   , "alpha"   )->setDesc(desc1 + " alpha");
  model->addProperty(path, this, "textFont"    , "font"    )->setDesc(desc1 + " font");
  model->addProperty(path, this, "textAngle"   , "angle"   )->setDesc(desc1 + " angle");
  model->addProperty(path, this, "textContrast", "contrast")->setDesc(desc1 + " contrast");
  model->addProperty(path, this, "textHtml"    , "html"    )->setDesc(desc1 + " is HTML");
  model->addProperty(path, this, "textAlign"   , "align"   )->setDesc(desc1 + " alignment");
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

  QColor c = interpTextColor(ColorInd());

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
