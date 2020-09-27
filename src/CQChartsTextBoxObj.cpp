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
  addBoxProperties(model, path, desc);

  auto textPath = path + "/text";

  auto desc1 = (desc.length() ? desc + " text" : "Text");

  model->addProperty(textPath, this, "textStr", "string")->setDesc(desc1 + " string");

  addTextDataProperties(model, textPath, desc);
}

void
CQChartsTextBoxObj::
addTextDataProperties(PropertyView *model, const QString &path, const QString &desc,
                      bool addVisible)
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
textBoxDataInvalidate()
{
  boxDataInvalidate();
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
      str = "";

    os << "set_charts_property -plot $" << plotName().toStdString();

    os << " -name " << nv.first.toStdString() << " -value {" << str.toStdString() << "}\n";
  }
}
