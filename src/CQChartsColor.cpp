#include <CQChartsColor.h>
#include <CQCharts.h>
#include <CQChartsUtil.h>

CQUTIL_DEF_META_TYPE(CQChartsColor, toString, fromString)

int CQChartsColor::metaTypeId;

void
CQChartsColor::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsColor);
}

QString
CQChartsColor::
colorStr() const
{
  if (! isValid())
    return "none";

  if      (type() == Type::PALETTE) {
    if (ind() == 0)
      return "palette";

    return QString("palette#%1").arg(ind());
  }
  else if (type() == Type::PALETTE_VALUE) {
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
  else if (type() == Type::INTERFACE) {
    return "interface";
  }
  else if (type() == Type::INTERFACE_VALUE) {
    return QString("interface:%1").arg(value());
  }

  return color().name();
}

bool
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

      if (! ok)
        return false;

      int pos1 = rhs1.indexOf(':');

      if (pos1 >= 0) {
        QString rhs2 = rhs1.mid(pos1 + 1);

        if (rhs2 == "s")
          scale = true;

        rhs1 = rhs1.mid(0, pos);
      }

      bool ok1;

      double value = rhs1.toDouble(&ok1);

      if (! ok1)
        return false;

      setIndScaleValue(Type::PALETTE_VALUE, ind, value, scale);
    }
    else {
      bool ok;

      int ind = rhs.toInt(&ok);

      if (! ok)
        return false;

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

    if (! ok)
      return false;

    setScaleValue(Type::PALETTE_VALUE, value, scale);
  }
  else if (str == "interface") {
    setValue(Type::INTERFACE, 0.0);
  }
  else if (str.startsWith("interface:")) {
    QString rhs = str.mid(10);

    bool ok;

    double value = rhs.toDouble(&ok);

    if (! ok)
      return false;

    setValue(Type::INTERFACE_VALUE, value);
  }
  else {
    QColor c(str);

    if (! c.isValid())
      return false;

    setColor(c);
  }

  return true;
}

//------

QColor
CQChartsColor::
interpColor(const CQCharts *charts, int i, int n) const
{
  double r = CMathUtil::norm(i, 0, n - 1);

  return interpColor(charts, r);
}

QColor
CQChartsColor::
interpColor(const CQCharts *charts, double value) const
{
  assert(isValid());

  if      (type() == Type::COLOR)
    return color();
  else if (type() == Type::PALETTE) {
    if (ind() == 0)
      return charts->interpPaletteColor(value);
    else
      return charts->interpIndPaletteColor(ind(), value);
  }
  else if (type() == Type::PALETTE_VALUE) {
    if (ind() == 0)
      return charts->interpPaletteColor(this->value(), isScale());
    else
      return charts->interpIndPaletteColor(ind(), this->value(), isScale());
  }
  else if (type() == Type::INTERFACE)
    return charts->interpThemeColor(value);
  else if (type() == Type::INTERFACE_VALUE)
    return charts->interpThemeColor(this->value());

  return QColor(0, 0, 0);
}
