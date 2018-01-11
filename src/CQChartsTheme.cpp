#include <CQChartsTheme.h>
#include <cassert>

CQChartsThemeMgr *
CQChartsThemeMgr::
instance()
{
  static CQChartsThemeMgr *inst;

  if (! inst)
    inst = new CQChartsThemeMgr;

  return inst;
}

CQChartsThemeMgr::
CQChartsThemeMgr()
{
  addTheme("default", new CQChartsDefaultTheme);
  addTheme("light1" , new CQChartsLightTheme1 );
  addTheme("light2" , new CQChartsLightTheme2 );
  addTheme("dark1"  , new CQChartsDarkTheme1  );
  addTheme("dark2"  , new CQChartsDarkTheme2  );
}

CQChartsThemeMgr::
~CQChartsThemeMgr()
{
  for (auto &nameTheme : themes_)
    delete nameTheme.second;
}

void
CQChartsThemeMgr::
addTheme(const QString &name, CQChartsTheme *theme)
{
  auto p = themes_.find(name);
  assert(p == themes_.end());

  theme->setName(name);

  themes_[name] = theme;
}

CQChartsTheme *
CQChartsThemeMgr::
getTheme(const QString &name) const
{
  auto p = themes_.find(name);

  if (p == themes_.end())
    return nullptr;

  return (*p).second;
}

//------

CQChartsTheme::
CQChartsTheme()
{
  palette_ = new CGradientPalette;
  theme_   = new CGradientPalette;
}

CQChartsTheme::
~CQChartsTheme()
{
  delete palette_;
  delete theme_;
}

//---

CQChartsDefaultTheme::
CQChartsDefaultTheme()
{
  theme_->setColorType(CGradientPalette::ColorType::DEFINED);

  theme_->resetDefinedColors();

  theme_->addDefinedColor(0.0, QColor("#ffffff"));
  theme_->addDefinedColor(1.0, QColor("#000000"));

  //---

  selectColor_ = QColor("#71d852");
}

CQChartsLightTheme1::
CQChartsLightTheme1()
{
  theme_->setColorType(CGradientPalette::ColorType::DEFINED);

  theme_->resetDefinedColors();

  theme_->addDefinedColor(0.0, QColor("#ffffff"));
  theme_->addDefinedColor(1.0, QColor("#000000"));

  //---

  palette_->setRedModel  (1);
  palette_->setGreenModel(7);
  palette_->setBlueModel (4);

  palette_->setBlueNegative(true);

  palette_->setRedMax  (0.8);
  palette_->setGreenMax(0.4);

  palette_->setColorType(CGradientPalette::ColorType::DEFINED);

  palette_->resetDefinedColors();

  palette_->addDefinedColor(0.0, QColor("#6d78ad"));
  palette_->addDefinedColor(1.0, QColor("#51cda0"));
  palette_->addDefinedColor(2.0, QColor("#df7970"));

  //---

  selectColor_ = QColor("#b60000");
}

CQChartsLightTheme2::
CQChartsLightTheme2()
{
  theme_->setColorType(CGradientPalette::ColorType::DEFINED);

  theme_->resetDefinedColors();

  theme_->addDefinedColor(0.0, QColor("#ffffff"));
  theme_->addDefinedColor(1.0, QColor("#000000"));

  //---

  palette_->setRedModel  (1);
  palette_->setGreenModel(7);
  palette_->setBlueModel (4);

  palette_->setBlueNegative(true);

  palette_->setRedMax  (0.8);
  palette_->setGreenMax(0.4);

  palette_->setColorType(CGradientPalette::ColorType::DEFINED);

  palette_->resetDefinedColors();

  palette_->addDefinedColor(0.0, QColor("#4d81bc"));
  palette_->addDefinedColor(1.0, QColor("#c0504e"));
  palette_->addDefinedColor(2.0, QColor("#9bbb58"));

  //---

  selectColor_ = QColor("#cccc00");
}

//------

CQChartsDarkTheme1::
CQChartsDarkTheme1()
{
  theme_->setColorType(CGradientPalette::ColorType::DEFINED);

  theme_->resetDefinedColors();

  theme_->addDefinedColor(0.0, QColor("#222222"));
  theme_->addDefinedColor(1.0, QColor("#dddddd"));

  //---

  palette_->setRedModel  (1);
  palette_->setGreenModel(7);
  palette_->setBlueModel (4);

  palette_->setBlueNegative(true);

  palette_->setRedMax  (0.8);
  palette_->setGreenMax(0.4);

  palette_->setColorType(CGradientPalette::ColorType::DEFINED);

  palette_->resetDefinedColors();

  palette_->addDefinedColor(0.0, QColor("#6d78ad"));
  palette_->addDefinedColor(1.0, QColor("#51cda0"));
  palette_->addDefinedColor(2.0, QColor("#df7970"));

  //---

  selectColor_ = QColor("#b60000");
}

CQChartsDarkTheme2::
CQChartsDarkTheme2()
{
  theme_->setColorType(CGradientPalette::ColorType::DEFINED);

  theme_->resetDefinedColors();

  theme_->addDefinedColor(0.0, QColor("#222222"));
  theme_->addDefinedColor(1.0, QColor("#dddddd"));

  //---

  palette_->setRedModel  (1);
  palette_->setGreenModel(7);
  palette_->setBlueModel (4);

  palette_->setBlueNegative(true);

  palette_->setRedMax  (0.8);
  palette_->setGreenMax(0.4);

  palette_->setColorType(CGradientPalette::ColorType::DEFINED);

  palette_->resetDefinedColors();

  palette_->addDefinedColor(0.0, QColor("#4d81bc"));
  palette_->addDefinedColor(1.0, QColor("#c0504e"));
  palette_->addDefinedColor(2.0, QColor("#9bbb58"));

  //---

  selectColor_ = QColor("#cccc00");
}
