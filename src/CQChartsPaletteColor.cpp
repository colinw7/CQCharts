#include <CQChartsPaletteColor.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>

QString
CQChartsPaletteColor::
colorStr() const
{
  if (type_ == Type::PALETTE)
    return "palette";

  if (type_ == Type::PALETTE_VALUE) {
    if (scale_)
      return QString("palette:%1:s").arg(value_);
    else
      return QString("palette:%1").arg(value_);
  }

  if (type_ == Type::THEME_VALUE)
    return QString("theme:%1").arg(value_);

  return color_.name();
}

void
CQChartsPaletteColor::
setColorStr(const QString &str)
{
  scale_ = false;

  if      (str == "palette") {
    type_ = Type::PALETTE;
  }
  else if (str.startsWith("palette:")) {
    type_ = Type::PALETTE_VALUE;

    QString paletteValueStr = str.mid(8);

    int pos = paletteValueStr.indexOf(':');

    if (pos >= 0) {
      QString rhs = paletteValueStr.mid(pos + 1);

      if (rhs == "s")
        scale_ = true;

      paletteValueStr = paletteValueStr.mid(0, pos);
    }

    bool ok;

    value_ = paletteValueStr.toDouble(&ok);
  }
  else if (str.startsWith("theme:")) {
    type_ = Type::THEME_VALUE;

    QString paletteValueStr = str.mid(6);

    bool ok;

    value_ = paletteValueStr.toDouble(&ok);
  }
  else {
    type_  = Type::COLOR;
    color_ = QColor(str);
  }
}

QColor
CQChartsPaletteColor::
interpColor(const CQChartsPlot *plot, int i, int n) const
{
  if (type_ == Type::THEME_VALUE) {
    double r = CQChartsUtil::norm(i, 0, n);

    return interpColor(plot, r);
  }
  else {
    double r = CQChartsUtil::norm(i + 1, 0, n + 1);

    return interpColor(plot, r);
  }
}

QColor
CQChartsPaletteColor::
interpColor(const CQChartsPlot *plot, double value) const
{
  if (type_ == Type::COLOR)
    return color_;

  if (type_ == Type::PALETTE)
    return plot->interpPaletteColor(value);

  if (type_ == Type::PALETTE_VALUE)
    return plot->interpPaletteColor(value_, scale_);

  if (type_ == Type::THEME_VALUE)
    return plot->interpThemeColor(value_);

  return QColor(0, 0, 0);
}
