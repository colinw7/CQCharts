#include <CQChartsColor.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>

CQUTIL_DEF_META_TYPE(CQChartsColor, toString, fromString)

void
CQChartsColor::
registerMetaType()
{
  CQUTIL_REGISTER_META(CQChartsColor);
}

QString
CQChartsColor::
colorStr() const
{
  if (! isValid())
    return "none";

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
CQChartsColor::
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
CQChartsColor::
interpColor(const CQChartsPlot *plot, int i, int n) const
{
  assert(isValid());

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
CQChartsColor::
interpColor(const CQChartsPlot *plot, double value) const
{
  assert(isValid());

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
