#include <CQChartsTextBoxObj.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

CQChartsTextBoxObj::
CQChartsTextBoxObj(View *view) :
 CQChartsBoxObj(view), CQChartsObjTextData<CQChartsTextBoxObj>(this)
{
  init();
}

CQChartsTextBoxObj::
CQChartsTextBoxObj(Plot *plot) :
 CQChartsBoxObj(plot), CQChartsObjTextData<CQChartsTextBoxObj>(this)
{
  init();
}

void
CQChartsTextBoxObj::
init()
{
  CQChartsColor themeFg(CQChartsColor::Type::CONTRAST);

  textData_.setColor(themeFg); //setTextColor(themeFg);

  setFilled (false);
  setStroked(false);
}

void
CQChartsTextBoxObj::
addProperties(PropertyView *model, const QString &path, const QString &desc)
{
  addTypeProperties(model, path, desc);
}

void
CQChartsTextBoxObj::
addTypeProperties(PropertyView *model, const QString &path, const QString &desc, uint addTypes)
{
  addBoxProperties(model, path, desc);

  auto textPath = path + "/text";

  if (addTypes & static_cast<int>(PropertyType::TEXT)) {
    auto desc1 = (desc.length() ? desc + " text" : "Text");

    model->addProperty(textPath, this, "textStr", "string")->setDesc(desc1 + " string");
  }

  addTextDataProperties(model, textPath, desc, addTypes);
}

void
CQChartsTextBoxObj::
addTextDataProperties(PropertyView *model, const QString &path, const QString &desc,
                      uint addTypes)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(model->addProperty(path, this, name, alias)->setDesc(desc));
  };

  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc) {
    auto *item = addProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  auto desc1 = (desc.length() ? desc + " text" : "Text");

  if (addTypes & static_cast<int>(PropertyType::VISIBLE))
    addProp(path, "textVisible", "visible", desc1 + " visible");

  if (addTypes & static_cast<int>(PropertyType::COLOR))
    addStyleProp(path, "textColor", "color", desc1 + " color");

  if (addTypes & static_cast<int>(PropertyType::ALPHA))
    addStyleProp(path, "textAlpha", "alpha", desc1 + " alpha");

  if (addTypes & static_cast<int>(PropertyType::FONT))
    addStyleProp(path, "textFont" , "font" , desc1 + " font");

  if (addTypes & static_cast<int>(PropertyType::ANGLE))
    addStyleProp(path, "textAngle", "angle", desc1 + " angle");

  if (addTypes & static_cast<int>(PropertyType::CONTRAST))
    addStyleProp(path, "textContrast", "contrast", desc1 + " contrast");

  if (addTypes & static_cast<int>(PropertyType::CONTRAST_ALPHA))
    addStyleProp(path, "textContrastAlpha", "contrastAlpha", desc1 + " contrast alpha");

  if (addTypes & static_cast<int>(PropertyType::HTML))
    addStyleProp(path, "textHtml", "html", desc1 + " is HTML");

  if (addTypes & static_cast<int>(PropertyType::ALIGN))
    addStyleProp(path, "textAlign", "align", desc1 + " alignment");
}

//---

CQChartsTextOptions
CQChartsTextBoxObj::
textOptions() const
{
  CQChartsTextOptions textOptions;

  textOptions.angle         = textAngle();
  textOptions.align         = textAlign();
  textOptions.contrast      = isTextContrast();
  textOptions.contrastAlpha = textContrastAlpha();
  textOptions.formatted     = isTextFormatted();
  textOptions.scaled        = isTextScaled();
  textOptions.html          = isTextHtml();
  textOptions.clipLength    = lengthPixelWidth(textClipLength());
  textOptions.clipElide     = textClipElide();
//textOptions.clipped       = isTextClipped();

  return textOptions;
}

//---

void
CQChartsTextBoxObj::
textBoxObjInvalidate()
{
  boxObjInvalidate();
}

//---

void
CQChartsTextBoxObj::
write(std::ostream &os, const QString &varName) const
{
  assert(plot());

  auto plotName = [&]() {
    return (varName != "" ? varName : "plot");
  };

  PropertyView::NameValues nameValues;

  plot()->propertyModel()->getChangedNameValues(this, nameValues, /*tcl*/true);

  if (! nameValues.empty())
    os << "\n";

  for (const auto &nv : nameValues) {
    QString str;

    if (! CQChartsVariant::toString(nv.second, str))
      str.clear();

    os << "set_charts_property -plot $" << plotName().toStdString();

    os << " -name " << nv.first.toStdString() << " -value {" << str.toStdString() << "}\n";
  }
}
