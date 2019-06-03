#include <CQChartsColor.h>
#include <CQChartsUtil.h>
#include <CQPropertyView.h>
#include <CQTclUtil.h>

CQUTIL_DEF_META_TYPE(CQChartsColor, toString, fromString)

int CQChartsColor::metaTypeId;

void
CQChartsColor::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsColor);

  CQPropertyViewMgrInst->setUserName("CQChartsColor", "color");
}

QString
CQChartsColor::
colorStr() const
{
  if (! isValid())
    return "none";

  QStringList strs;

  if      (type() == Type::PALETTE) {
    if (ind() < 0)
      strs << "palette";
    else
      strs << QString("palette#%1").arg(ind());
  }
  else if (type() == Type::PALETTE_VALUE) {
    if (ind() < 0)
      strs << "palette";
    else
      strs << QString("palette#%1").arg(ind());

    strs << QString("%1").arg(value());

    if (isScale())
      strs << "s";
  }
  else if (type() == Type::INDEXED) {
    if (ind() < 0)
      strs << "ind_palette";
    else
      strs << QString("ind_palette#%1").arg(ind());
  }
  else if (type() == Type::INDEXED_VALUE) {
    if (ind() < 0)
      strs << "ind_palette";
    else
      strs << QString("ind_palette#%1").arg(ind());

    strs << QString("%1").arg(value());

    if (isScale())
      strs << "s";
  }
  else if (type() == Type::INTERFACE) {
    strs << "interface";
  }
  else if (type() == Type::INTERFACE_VALUE) {
    strs << "interface" << QString("%1").arg(value());
  }
  else {
    strs << color().name();
  }

  return CQTcl::mergeList(strs);
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

  // reset
  type_  = Type::NONE;
  ind_   = -1;
  value_ = 0.0;
  color_ = QColor();
  scale_ = false;

  //---

  // get strings
  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  if (strs.length() == 0 || (strs.length() == 1 && strs[0].simplified() == ""))
    return true;

  //---

  QString rhs;

  if      (strs[0] == "palette") {
    if (strs.length() > 1) {
      bool ok;

      double value = CQChartsUtil::toReal(strs[1], ok);
      if (! ok) return false;

      bool scale = false;

      if (strs.length() > 2) {
        if (strs[2][0] == 's')
          scale = true;
      }

      setScaleValue(Type::PALETTE_VALUE, value, scale);
    }
    else
      setValue(Type::PALETTE, 0.0);
  }
  else if (startsWith(strs[0], "palette#", rhs)) {
    bool ok;

    long ind = CQChartsUtil::toInt(rhs, ok);
    if (! ok) return false;

    if (strs.length() > 1) {
      bool ok;

      double value = CQChartsUtil::toReal(strs[1], ok);
      if (! ok) return false;

      bool scale = false;

      if (strs.length() > 2) {
        if (strs[2][0] == 's')
          scale = true;
      }

      setIndScaleValue(Type::PALETTE_VALUE, ind, value, scale);
    }
    else
      setIndValue(Type::PALETTE, ind, 0.0);
  }
  else if (strs[0] == "ind_palette") {
    if (strs.length() > 1) {
      bool ok;

      int value = CQChartsUtil::toInt(strs[1], ok);
      if (! ok) return false;

      setValue(Type::INDEXED_VALUE, value);
    }
    else
      setValue(Type::INDEXED, 0);
  }
  else if (startsWith(strs[0], "ind_palette#", rhs)) {
    bool ok;

    long ind = CQChartsUtil::toInt(rhs, ok);
    if (! ok) return false;

    if (strs.length() > 1) {
      bool ok;

      int value = CQChartsUtil::toInt(strs[1], ok);
      if (! ok) return false;

      setIndValue(Type::INDEXED_VALUE, ind, value);
    }
    else
      setIndValue(Type::INDEXED, ind, 0);
  }
  else if (strs[0] == "interface") {
    if (strs.length() > 1) {
      bool ok;

      double value = CQChartsUtil::toReal(strs[1], ok);
      if (! ok) return false;

      setValue(Type::INTERFACE_VALUE, value);
    }
    else
      setValue(Type::INTERFACE, 0.0);
  }
  else {
    QColor c(strs[0]);

    if (! c.isValid())
      return false;

    setColor(c);
  }

  return true;
}
