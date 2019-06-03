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

  setFilled (false);
  setStroked(false);
}

CQChartsTextBoxObj::
CQChartsTextBoxObj(CQChartsPlot *plot) :
 CQChartsBoxObj(plot),
 CQChartsObjTextData<CQChartsTextBoxObj>(this)
{
  CQChartsColor themeFg(CQChartsColor::Type::INTERFACE_VALUE, 1);

  setTextColor(themeFg);

  setFilled (false);
  setStroked(false);
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
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(model->addProperty(path, this, name, alias)->setDesc(desc));
  };

  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc) {
    CQPropertyViewItem *item = addProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  QString desc1 = (desc.length() ? desc + " text" : "Text");

  if (addVisible)
    addProp(path, "textVisible", "visible", desc1 + " visible");

  addStyleProp(path, "textColor"   , "color"   , desc1 + " color");
  addStyleProp(path, "textAlpha"   , "alpha"   , desc1 + " alpha");
  addStyleProp(path, "textFont"    , "font"    , desc1 + " font");
  addStyleProp(path, "textAngle"   , "angle"   , desc1 + " angle");
  addStyleProp(path, "textContrast", "contrast", desc1 + " contrast");
  addStyleProp(path, "textHtml"    , "html"    , desc1 + " is HTML");
  addStyleProp(path, "textAlign"   , "align"   , desc1 + " alignment");
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
