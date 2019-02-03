#include <CQChartsNameValues.h>

#include <CQChartsVariant.h>
#include <CQStrParse.h>

QString
CQChartsNameValues::
toString(const NameValues &nameValues)
{
  QString str;

  if (nameValues.empty())
    return str;

  for (const auto &nameValue : nameValues) {
    if (str.length())
      str += ",";

    str += nameValue.first + '=';

    QString value = nameValue.second.toString();

    for (int i = 0; i < value.length(); ++i) {
      if (value[i] == ',')
        str += '\\';

      str += value[i];
    }
  }

  return str;
}

bool
CQChartsNameValues::
fromString(const QString &str)
{
  NameValues nameValues;

  if (! fromString(str, nameValues))
    return false;

  nameValues_ = nameValues;

  return true;
}

bool
CQChartsNameValues::
fromString(const QString &str, NameValues &nameValues)
{
  if (! str.length())
    return true;

  QStringList strs = splitNameValues(str);

  for (int i = 0; i < strs.length(); ++i) {
    int pos1 = strs[i].indexOf("=");

    if (pos1 < 1) {
      nameValues[strs[i]] = "1";
    }
    else {
      QString name  = strs[i].mid(0, pos1 ).simplified();
      QString value = strs[i].mid(pos1 + 1).simplified();

      nameValues[name] = value;
    }
  }

  return true;
}

bool
CQChartsNameValues::
nameValue(const QString &name, QVariant &value) const
{
  auto p = nameValues_.find(name);

  if (p == nameValues_.end())
    return false;

  value = (*p).second;

  return true;
}

void
CQChartsNameValues::
setNameValue(const QString &name, const QVariant &value)
{
  nameValues_[name] = value;
}

bool
CQChartsNameValues::
nameValueString(const QString &name, QString &value) const
{
  QVariant var;

  if (! nameValue(name, var))
    return false;

  value = var.toString();

  return true;
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

QStringList
CQChartsNameValues::
splitNameValues(const QString &str)
{
  QStringList words;

  //words = str.split(",", QString::SkipEmptyParts);

  CQStrParse parse(str);

  QString word;

  while (! parse.eof()) {
    if      (parse.isChar('\\')) {
      parse.skipChar();

      if (! parse.eof())
        word += parse.getChar();
    }
    else if (parse.isChar(',')) {
      parse.skipChar();

      if (word.length())
        words.push_back(word);

      word = "";
    }
    else
      word += parse.getChar();
  }

  if (word.length())
    words.push_back(word);

  return words;
}
