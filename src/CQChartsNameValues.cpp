#include <CQChartsNameValues.h>

#include <CQChartsVariant.h>
#include <CQUtil.h>

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
nameValueInteger(const QString &name, int &value, bool &ok) const
{
  ok = true;

  QVariant var;

  if (! nameValue(name, var))
    return false;

  value = (int) CQChartsVariant::toInt(var, ok);

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
