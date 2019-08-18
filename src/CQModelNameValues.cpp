#include <CQModelNameValues.h>
#include <CQStrParse.h>
#include <CQUtil.h>
#include <QColor>
#include <QFont>

QString
CQModelNameValues::
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
CQModelNameValues::
fromString(const QString &str)
{
  NameValues nameValues;

  if (! fromString(str, nameValues))
    return false;

  nameValues_ = nameValues;

  return true;
}

bool
CQModelNameValues::
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
CQModelNameValues::
nameValue(const QString &name, QVariant &value) const
{
  auto p = nameValues_.find(name);

  if (p == nameValues_.end())
    return false;

  value = (*p).second;

  assert(value.isValid());

  return true;
}

void
CQModelNameValues::
setNameValue(const QString &name, const QVariant &value)
{
  assert(value.isValid());

  nameValues_[name] = value;
}

bool
CQModelNameValues::
nameValueString(const QString &name, QString &value) const
{
  QVariant var;

  if (! nameValue(name, var))
    return false;

  value = var.toString();

  return true;
}

bool
CQModelNameValues::
nameValueInteger(const QString &name, int &value) const
{
  QVariant var;

  if (! nameValue(name, var))
    return false;

  bool ok;

  value = var.toInt(&ok);

  return ok;
}

bool
CQModelNameValues::
nameValueReal(const QString &name, double &value) const
{
  QVariant var;

  if (! nameValue(name, var))
    return false;

  bool ok;

  value = var.toDouble(&ok);

  return ok;
}

bool
CQModelNameValues::
nameValueBool(const QString &name, bool &value) const
{
  QVariant var;

  if (! nameValue(name, var))
    return false;

  value = var.toBool();

  return true;
}

bool
CQModelNameValues::
nameValueColor(const QString &name, QColor &color) const
{
  QVariant var;

  if (! nameValue(name, var))
    return false;

  if (var.type() == QVariant::Color)
    color = var.value<QColor>();
  else
    color = QColor(var.toString());

  return color.isValid();
}

bool
CQModelNameValues::
nameValueFont(const QString &name, QFont &font) const
{
  QVariant var;

  if (! nameValue(name, var))
    return false;

  if (var.type() == QVariant::Font)
    font = var.value<QFont>();
  else
    font = QFont(var.toString());

  return true;
}

bool
CQModelNameValues::
nameValueAlign(const QString &name, Qt::Alignment &align) const
{
  QVariant var;

  if (! nameValue(name, var))
    return false;

  if (! CQUtil::stringToAlign(var.toString(), align))
    return false;

  return true;
}

QStringList
CQModelNameValues::
splitNameValues(const QString &str)
{
  QStringList words;

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
