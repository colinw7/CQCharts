#include <CQChartsFillUnder.h>
#include <CQChartsUtil.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsFillUnderSide, toString, fromString)

int CQChartsFillUnderSide::metaTypeId;

void
CQChartsFillUnderSide::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsFillUnderSide);

  CQPropertyViewMgrInst->setUserName("CQChartsFillUnderSide", "fill_under_side");
}

QString
CQChartsFillUnderSide::
toString() const
{
  switch (type_) {
    case Type::BOTH:  return "both" ;
    case Type::ABOVE: return "above";
    case Type::BELOW: return "below";
    default:          return "below";
  }
}

bool
CQChartsFillUnderSide::
decodeString(const QString &str, Type &type)
{
  QString lstr = str.toLower();

  if      (lstr == "both" ) type = Type::BOTH;
  else if (lstr == "above") type = Type::ABOVE;
  else if (lstr == "below") type = Type::BELOW;
  else                      return false;

  return true;
}

QStringList
CQChartsFillUnderSide::
sideNames()
{
  static QStringList names = QStringList() <<
    "both" << "above" << "below";

  return names;
}

//---

CQUTIL_DEF_META_TYPE(CQChartsFillUnderPos, toString, fromString)

int CQChartsFillUnderPos::metaTypeId;

void
CQChartsFillUnderPos::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsFillUnderPos);

  CQPropertyViewMgrInst->setUserName("CQChartsFillUnderPos", "fill_under_position");
}

QString
CQChartsFillUnderPos::
toString() const
{
  QString xstr;

  switch (xtype_) {
    case Type::MIN: xstr += "xmin"; break;
    case Type::MAX: xstr += "xmax"; break;
    case Type::POS: xstr += QString("%1").arg(xpos_); break;
    default: break;
  }

  QString ystr;

  switch (ytype_) {
    case Type::MIN: ystr += "ymin"; break;
    case Type::MAX: ystr += "ymax"; break;
    case Type::POS: ystr += QString("%1").arg(ypos_); break;
    default: break;
  }

  if      (xstr.length() && ystr.length())
    return xstr + " " + ystr;
  else if (xstr.length())
    return xstr;
  else
    return ystr;
}

bool
CQChartsFillUnderPos::
decodeString(const QString &str, Type &xtype, double &xpos, Type &ytype, double &ypos)
{
  xtype = Type::NONE;
  ytype = Type::NONE;

  QStringList strs = str.split(" ", QString::KeepEmptyParts);

  if (! strs.length())
    return true;

  if (strs.length() > 1) {
    const QString &xstr = strs[0];
    const QString &ystr = strs[1];

    if      (xstr == "min" || xstr == "xmin") {
      xtype = Type::MIN;
    }
    else if (xstr == "max" || xstr == "xmax") {
      xtype = Type::MAX;
    }
    else {
      bool ok;

      double x1 = CQChartsUtil::toReal(xstr, ok);

      if (! ok)
        return false;

      xtype = Type::POS;
      xpos  = x1;
    }

    if      (ystr == "min" || ystr == "ymin") {
      ytype = Type::MIN;
    }
    else if (ystr == "max" || ystr == "ymax") {
      ytype = Type::MAX;
    }
    else {
      bool ok;

      double y1 = CQChartsUtil::toReal(ystr, ok);

      if (! ok)
        return false;

      ytype = Type::POS;
      ypos  = y1;
    }
  }
  else {
    const QString &str = strs[0];

    if      (str == "xmin") {
      xtype = Type::MIN;
    }
    else if (str == "xmax") {
      xtype = Type::MAX;
    }
    else if (str == "min" || str == "ymin") {
      ytype = Type::MIN;
    }
    else if (str == "max" || str == "ymax") {
      ytype = Type::MAX;
    }
    else {
      bool ok;

      double y1 = CQChartsUtil::toReal(str, ok);

      if (! ok)
        return false;

      ytype = Type::POS;
      ypos  = y1;
    }
  }

  return true;
}
