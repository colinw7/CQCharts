#include <CQChartsColor.h>
#include <CQChartsUtil.h>
#include <CQPropertyView.h>
#include <CQColors.h>
#include <CQColorsPalette.h>
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

  if      (type() == Type::PALETTE || type() == Type::PALETTE_VALUE) {
    if      (hasPaletteIndex())
      strs << QString("palette#%1").arg(ind());
    else if (hasPaletteName()) {
      QString name;

      if (getPaletteName(name))
        strs << QString("palette#%1").arg(name);
      else
        strs << "palette";
    }
    else
      strs << "palette";

    if (type() == Type::PALETTE_VALUE) {
      strs << QString::number(value());

      if (isScale())
        strs << "s";
    }
  }
  else if (type() == Type::INDEXED || type() == Type::INDEXED_VALUE) {
    if      (hasPaletteIndex())
      strs << QString("ind_palette#%1").arg(ind());
    else if (hasPaletteName()) {
      QString name;

      if (getPaletteName(name))
        strs << QString("ind_palette#%1").arg(name);
      else
        strs << "ind_palette";
    }
    else
      strs << "ind_palette";

    if (type() == Type::INDEXED_VALUE) {
      strs << QString::number(value());

      if (isScale())
        strs << "s";
    }
  }
  else if (type() == Type::INTERFACE || type() == Type::INTERFACE_VALUE) {
    strs << "interface";

    if (type() == Type::INTERFACE_VALUE)
      strs << QString::number(value());
  }
  else if (type() == Type::CONTRAST || type() == Type::CONTRAST_VALUE) {
    strs << "contrast";

    if (type() == Type::CONTRAST_VALUE)
      strs << QString::number(value());
  }
  else if (type() == Type::MODEL || type() == Type::MODEL_VALUE) {
    int r, g, b;

    decodeModelRGB(ind(), r, g, b);

    strs << "model" << QString::number(r) << QString::number(g) << QString::number(b);

    if (type() == Type::MODEL_VALUE)
      strs << QString::number(value());
  }
  else if (type() == Type::LIGHTER || type() == Type::LIGHTER_VALUE) {
    strs << "lighter";

    if (type() == Type::LIGHTER_VALUE)
      strs << QString::number(value());
  }
  else if (type() == Type::DARKER || type() == Type::DARKER_VALUE) {
    strs << "darker";

    if (type() == Type::DARKER_VALUE)
      strs << QString::number(value());
  }
  else {
    strs << color().name();
  }

  if (strs.size() == 1)
    return strs[0];
  else
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

    if (! ok || ind < 0) {
      if (! paletteNameInd(rhs, ind))
        return false;
    }

    if (strs.length() > 1) {
      bool ok;

      double value = CQChartsUtil::toReal(strs[1], ok);
      if (! ok) return false;

      bool scale = false;

      if (strs.length() > 2) {
        if (strs[2][0] == 's')
          scale = true;
      }

      setIndScaleValue(Type::PALETTE_VALUE, int(ind), value, scale);
    }
    else
      setIndValue(Type::PALETTE, int(ind), 0.0);
  }
  else if (strs[0] == "ind_palette") {
    if (strs.length() > 1) {
      bool ok;

      long value = CQChartsUtil::toInt(strs[1], ok);
      if (! ok) return false;

      setValue(Type::INDEXED_VALUE, int(value));
    }
    else
      setValue(Type::INDEXED, 0);
  }
  else if (startsWith(strs[0], "ind_palette#", rhs)) {
    bool ok;

    long ind = CQChartsUtil::toInt(rhs, ok);

    if (! ok || ind < 0) {
      if (! paletteNameInd(rhs, ind))
        return false;
    }

    if (strs.length() > 1) {
      bool ok;

      long value = CQChartsUtil::toInt(strs[1], ok);
      if (! ok) return false;

      setIndValue(Type::INDEXED_VALUE, int(ind), int(value));
    }
    else
      setIndValue(Type::INDEXED, int(ind), 0);
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
  else if (strs[0] == "contrast") {
    if (strs.length() > 1) {
      bool ok;

      double value = CQChartsUtil::toReal(strs[1], ok);
      if (! ok) return false;

      setValue(Type::CONTRAST_VALUE, value);
    }
    else
      setValue(Type::CONTRAST, 0.0);
  }
  else if (strs[0] == "model") {
    if (strs.length() < 4)
      return false;

    bool ok;

    long r = CQChartsUtil::toInt(strs[1], ok);
    if (! ok) return false;

    long g = CQChartsUtil::toInt(strs[2], ok);
    if (! ok) return false;

    long b = CQChartsUtil::toInt(strs[3], ok);
    if (! ok) return false;

    int rgb = encodeModelRGB(int(r), int(g), int(b));

    if (strs.length() > 4) {
      bool ok;

      double value = CQChartsUtil::toReal(strs[4], ok);
      if (! ok) return false;

      setIndValue(Type::MODEL_VALUE, rgb, value);
    }
    else
      setIndValue(Type::MODEL, rgb, 0.0);
  }
  else if (strs[0] == "lighter") {
    if (strs.length() > 1) {
      bool ok;

      double value = CQChartsUtil::toReal(strs[1], ok);
      if (! ok) return false;

      setValue(Type::LIGHTER_VALUE, value);
    }
    else
      setValue(Type::LIGHTER, 0.0);
  }
  else if (strs[0] == "darker") {
    if (strs.length() > 1) {
      bool ok;

      double value = CQChartsUtil::toReal(strs[1], ok);
      if (! ok) return false;

      setValue(Type::DARKER_VALUE, value);
    }
    else
      setValue(Type::DARKER, 0.0);
  }
  else {
    QColor c(strs[0]);

    if (! c.isValid())
      return false;

    setColor(c);
  }

  return true;
}

bool
CQChartsColor::
getPaletteName(QString &name) const
{
  int paletteInd = getNamedPaletteIndex();

  auto *palette = CQColorsMgrInst->getIndPalette(paletteInd);
  if (! palette) return false;

  name = palette->name();

  return true;
}

bool
CQChartsColor::
setPaletteName(const QString &name)
{
  assert(type_ == Type::PALETTE || type_ == Type::PALETTE_VALUE ||
         type_ == Type::INDEXED || type_ == Type::INDEXED_VALUE);

  long ind;

  if (! paletteNameInd(name, ind))
    return false;

  ind_ = int(ind);

  return true;
}

bool
CQChartsColor::
paletteNameInd(const QString &name, long &ind)
{
  int paletteInd = CQColorsMgrInst->getNamedPaletteInd(name);

  if (paletteInd < 0)
    return false;

  ind = -paletteInd - 2;

  return true;
}
