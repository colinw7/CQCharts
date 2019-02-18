#include <CQChartsColor.h>
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
  auto startsWith = [](const QString &str, const QString &pattern, QString &rhs) {
    if (str.startsWith(pattern)) {
      rhs = str.mid(pattern.length());
      return true;
    }

    return false;
  };

  //---

  scale_ = false;

  QString rhs;

  if      (str == "palette") {
    setValue(Type::PALETTE, 0.0);
  }
  else if (startsWith(str, "palette#", rhs)) {
    int pos = rhs.indexOf(':');

    if (pos > 0) {
      bool scale = false;

      QString lhs1 = rhs.mid(0, pos);
      QString rhs1 = rhs.mid(pos + 1);

      bool ok;

      long ind = CQChartsUtil::toInt(lhs1, ok);

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

      double value = CQChartsUtil::toReal(rhs1, ok1);

      if (! ok1)
        return false;

      setIndScaleValue(Type::PALETTE_VALUE, ind, value, scale);
    }
    else {
      bool ok;

      long ind = CQChartsUtil::toInt(rhs, ok);

      if (! ok)
        return false;

      setIndValue(Type::PALETTE, ind, 0.0);
    }
  }
  else if (startsWith(str, "palette:", rhs)) {
    bool scale = false;

    int pos = rhs.indexOf(':');

    if (pos >= 0) {
      QString rhs1 = rhs.mid(pos + 1);

      if (rhs == "s")
        scale = true;

      rhs = rhs.mid(0, pos);
    }

    bool ok;

    double value = CQChartsUtil::toReal(rhs, ok);

    if (! ok)
      return false;

    setScaleValue(Type::PALETTE_VALUE, value, scale);
  }
  else if (str == "interface") {
    setValue(Type::INTERFACE, 0.0);
  }
  else if (startsWith(str, "interface:", rhs)) {
    bool ok;

    double value = CQChartsUtil::toReal(rhs, ok);

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
