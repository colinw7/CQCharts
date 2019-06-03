#include <CQChartsInterfaceTheme.h>
#include <CQColorsPalette.h>

CQChartsInterfaceTheme::
CQChartsInterfaceTheme()
{
  palette_ = new CQColorsPalette();
}

CQChartsInterfaceTheme::
~CQChartsInterfaceTheme()
{
  delete palette_;
}

void
CQChartsInterfaceTheme::
setDark(bool b)
{
  isDark_ = b;

  palette_->setColorType(CQColorsPalette::ColorType::DEFINED);

  palette_->resetDefinedColors();

  if (! isDark_) {
    palette_->addDefinedColor(0.0, lightBgColor_);
    palette_->addDefinedColor(1.0, lightFgColor_);
  }
  else {
    palette_->addDefinedColor(0.0, darkBgColor_);
    palette_->addDefinedColor(1.0, darkFgColor_);
  }
}

QColor
CQChartsInterfaceTheme::
interpColor(double r, bool scale) const
{
  return palette_->getColor(r, scale);
}
