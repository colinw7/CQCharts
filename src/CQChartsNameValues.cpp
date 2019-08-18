#include <CQChartsNameValues.h>

#include <CQChartsVariant.h>
#include <CQUtil.h>
#include <CQStrParse.h>

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
nameValueInteger(const QString &name, int &value) const
{
  QVariant var;

  if (! nameValue(name, var))
    return false;

  bool ok;

  value = CQChartsVariant::toInt(var, ok);

  return ok;
}

bool
CQChartsNameValues::
nameValueReal(const QString &name, double &value) const
{
  QVariant var;

  if (! nameValue(name, var))
    return false;

  bool ok;

  value = CQChartsVariant::toReal(var, ok);

  return ok;
}

bool
CQChartsNameValues::
nameValueBool(const QString &name, bool &value) const
{
  QVariant var;

  if (! nameValue(name, var))
    return false;

  bool ok;

  value = CQChartsVariant::toBool(var, ok);

  return ok;
}

bool
CQChartsNameValues::
nameValueColor(const QString &name, CQChartsColor &color) const
{
  QVariant var;

  if (! nameValue(name, var))
    return false;

  bool ok;

  color = CQChartsVariant::toColor(var, ok);

  return ok;
}

bool
CQChartsNameValues::
nameValueFont(const QString &name, CQChartsFont &font) const
{
  QVariant var;

  if (! nameValue(name, var))
    return false;

  bool ok;

  font = CQChartsVariant::toFont(var, ok);

  return ok;
}
