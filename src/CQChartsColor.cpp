#include <CQChartsColor.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
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

  if (type() == Type::PALETTE) {
    if (ind() == 0)
      return "palette";

    return QString("palette#%1").arg(ind());
  }

  if (type() == Type::PALETTE_VALUE) {
    if (ind() == 0) {
      if (isScale())
        return QString("palette:%1:s").arg(value());
      else
        return QString("palette:%1").arg(value());
    }
    else {
      if (isScale())
        return QString("palette#%1:%2:s").arg(ind()).arg(value());
      else
        return QString("palette#%1:%2").arg(ind()).arg(value());
    }
  }

  if (type() == Type::THEME_VALUE)
    return QString("theme:%1").arg(value());

  return color().name();
}

void
CQChartsColor::
setColorStr(const QString &str)
{
  scale_ = false;

  if      (str == "palette") {
    setValue(Type::PALETTE, 0.0);
  }
  else if (str.startsWith("palette#")) {
    QString rhs = str.mid(8);

    int pos = rhs.indexOf(':');

    if (pos > 0) {
      bool scale = false;

      QString lhs1 = rhs.mid(0, pos);
      QString rhs1 = rhs.mid(pos + 1);

      bool ok;

      int ind = lhs1.toInt(&ok);

      int pos1 = rhs1.indexOf(':');

      if (pos1 >= 0) {
        QString rhs2 = rhs1.mid(pos1 + 1);

        if (rhs2 == "s")
          scale = true;

        rhs1 = rhs1.mid(0, pos);
      }

      bool ok1;

      double value = rhs1.toDouble(&ok1);

      setIndScaleValue(Type::PALETTE_VALUE, ind, value, scale);
    }
    else {
      bool ok;

      int ind = rhs.toInt(&ok);

      setIndValue(Type::PALETTE, ind, 0.0);
    }
  }
  else if (str.startsWith("palette:")) {
    bool scale = false;

    QString rhs = str.mid(8);

    int pos = rhs.indexOf(':');

    if (pos >= 0) {
      QString rhs1 = rhs.mid(pos + 1);

      if (rhs == "s")
        scale = true;

      rhs = rhs.mid(0, pos);
    }

    bool ok;

    double value = rhs.toDouble(&ok);

    setScaleValue(Type::PALETTE_VALUE, value, scale);
  }
  else if (str.startsWith("theme:")) {
    QString rhs = str.mid(6);

    bool ok;

    double value = rhs.toDouble(&ok);

    setValue(Type::THEME_VALUE, value);
  }
  else {
    QColor c(str);

    if (c.isValid())
      setColor(c);
  }
}

QColor
CQChartsColor::
interpColor(const CQChartsPlot *plot, int i, int n) const
{
  assert(isValid());

  if (type() == Type::THEME_VALUE) {
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

  if (type() == Type::COLOR)
    return color();

  if (type() == Type::PALETTE) {
    if (ind() == 0)
      return plot->interpPaletteColor(value);
    else
      return plot->interpIndPaletteColor(ind(), value);
  }

  if (type() == Type::PALETTE_VALUE) {
    if (ind() == 0)
      return plot->interpPaletteColor(this->value(), isScale());
    else
      return plot->interpIndPaletteColor(ind(), this->value(), isScale());
  }

  if (type() == Type::THEME_VALUE)
    return plot->interpThemeColor(this->value());

  return QColor(0, 0, 0);
}

QColor
CQChartsColor::
interpColor(const CQChartsView *view, int i, int n) const
{
  assert(isValid());

  if (type() == Type::THEME_VALUE) {
    double r = CQChartsUtil::norm(i, 0, n);

    return interpColor(view, r);
  }
  else {
    double r = CQChartsUtil::norm(i + 1, 0, n + 1);

    return interpColor(view, r);
  }
}

QColor
CQChartsColor::
interpColor(const CQChartsView *view, double value) const
{
  assert(isValid());

  if (type() == Type::COLOR)
    return color();

  if (type() == Type::PALETTE) {
    if (ind() == 0)
      return view->interpPaletteColor(value);
    else
      return view->interpIndPaletteColor(ind(), value);
   }

  if (type() == Type::PALETTE_VALUE) {
    if (ind() == 0)
      return view->interpPaletteColor(this->value(), isScale());
    else
      return view->interpIndPaletteColor(ind(), this->value(), isScale());
  }

  if (type() == Type::THEME_VALUE)
    return view->interpThemeColor(this->value());

  return QColor(0, 0, 0);
}