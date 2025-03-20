#include <CQChartsNameValues.h>
#include <CQPropertyView.h>
#include <CQTclUtil.h>

#include <CQChartsVariant.h>
#include <CQUtil.h>

CQUTIL_DEF_META_TYPE(CQChartsNameValues, toString, fromString)

int CQChartsNameValues::metaTypeId;

void
CQChartsNameValues::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsNameValues);

  CQPropertyViewMgrInst->setUserName("CQChartsNameValues", "name_values");
}

CQChartsNameValues::
CQChartsNameValues()
{
}

CQChartsNameValues::
CQChartsNameValues(const QString &str) :
 CQModelNameValues(str)
{
}

bool
CQChartsNameValues::
nameValueInteger(const QString &name, long &value, bool &ok) const
{
  ok = true;

  QVariant var;
  if (! nameValue(name, var))
    return false;

  value = CQChartsVariant::toInt(var, ok);

  return true;
}

bool
CQChartsNameValues::
nameValueReal(const QString &name, double &value, bool &ok) const
{
  ok = true;

  QVariant var;
  if (! nameValue(name, var))
    return false;

  value = CQChartsVariant::toReal(var, ok);

  return true;
}

bool
CQChartsNameValues::
nameValueBool(const QString &name, bool &value, bool &ok) const
{
  ok = true;

  QVariant var;
  if (! nameValue(name, var))
    return false;

  value = CQChartsVariant::toBool(var, ok);

  return true;
}

bool
CQChartsNameValues::
nameValueColor(const QString &name, CQChartsColor &color, bool &ok) const
{
  ok = true;

  QVariant var;

  if (! nameValue(name, var))
    return false;

  color = CQChartsVariant::toColor(var, ok);

  return true;
}

bool
CQChartsNameValues::
nameValueFont(const QString &name, CQChartsFont &font, bool &ok) const
{
  QVariant var;

  if (! nameValue(name, var))
    return false;

  font = CQChartsVariant::toFont(var, ok);

  return true;
}

//---

CQChartsTclNameValues::
CQChartsTclNameValues(const QString &str)
{
  QStringList strs;
  valid_ = CQTcl::splitList(str, strs);

  for (const auto &str1 : strs) {
    QStringList strs1;
    if (! CQTcl::splitList(str1, strs1)) {
      valid_ = false;
      break;
    }

    if (strs1.length() != 2) {
      valid_ = false;
      break;
    }

    nameValues_[strs1[0]] = strs1[1];
  }
}

bool
CQChartsTclNameValues::
nameValueString(const QString &name, QString &value) const
{
  auto pn = nameValues_.find(name);
  if (pn == nameValues_.end()) return false;

  value = (*pn).second;

  return true;
}

bool
CQChartsTclNameValues::
nameValueInteger(const QString &name, long &value, bool &ok) const
{
  QString str;
  if (! nameValueString(name, str))
    return false;

  value = CQChartsVariant::toInt(str, ok);

  return true;
}

bool
CQChartsTclNameValues::
nameValueReal(const QString &name, double &value, bool &ok) const
{
  QString str;
  if (! nameValueString(name, str))
    return false;

  value = CQChartsVariant::toReal(str, ok);

  return true;
}

bool
CQChartsTclNameValues::
nameValueBool(const QString &name, bool &value, bool &ok) const
{
  QString str;
  if (! nameValueString(name, str))
    return false;

  value = CQChartsVariant::toBool(str, ok);

  return true;
}
