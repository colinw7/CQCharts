#include <CQBaseModel.h>
#include <cmath>
#include <cassert>

CQBaseModel::
CQBaseModel()
{
}

void
CQBaseModel::
genColumnTypes()
{
  // auto determine types
  int nc = columnCount();
  int nr = rowCount   ();

  resetColumnTypes();

  for (int c = 0; c < nc; ++c) {
    Type type = Type::NONE;

    int numInt = 0, numReal = 0;

    for (int r = 0; r < nr; ++r) {
      QModelIndex ind = index(r, c);

      QString s = data(ind, Qt::DisplayRole).toString();

      if (! s.length())
        continue;

      bool ok;

      double real = toReal(s, ok);

      if (ok) {
        if (isInteger(real))
          ++numInt;
        else
          ++numReal;
      }
      else {
        type = Type::STRING;
        break;
      }
    }

    if (type == Type::NONE) {
      if      (numReal == 0 && numInt == 0)
        type = Type::STRING;
      else if (numReal == 0)
        type = Type::INTEGER;
      else
        type = Type::REAL;
    }

    setColumnType(c, type);
  }
}

CQBaseModel::Type
CQBaseModel::
columnType(int column) const
{
  auto p = columnDatas_.find(column);

  if (p == columnDatas_.end())
    return Type::NONE;

  return (*p).second.type;
}

void
CQBaseModel::
setColumnType(int column, Type type)
{
  columnDatas_[column].type = type;
}

QString
CQBaseModel::
columnTypeValues(int column) const
{
  auto p = columnDatas_.find(column);

  if (p == columnDatas_.end())
    return QString();

  return (*p).second.typeValues;
}

void
CQBaseModel::
setColumnTypeValues(int column, const QString &str)
{
  columnDatas_[column].typeValues = str;
}

void
CQBaseModel::
resetColumnTypes()
{
  columnDatas_.clear();
}

//------

QVariant
CQBaseModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal)
    return QAbstractItemModel::headerData(section, orientation, role);

  if (role == static_cast<int>(Role::Type)) {
    Type type = columnType(section);

    return QVariant((int) type);
  }
  else if (role == static_cast<int>(Role::TypeValues)) {
    QString str = columnTypeValues(section);

    return QVariant(str);
  }

  return QVariant();
}

bool
CQBaseModel::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  if (orientation != Qt::Horizontal)
    return false;

  if (role == static_cast<int>(Role::Type)) {
    bool ok;

    Type type = variantToType(value, &ok);

    if (ok) {
      setColumnType(section, type);

      return true;
    }
  }
  else if (role == static_cast<int>(Role::TypeValues)) {
    QString str = value.toString();

    setColumnTypeValues(section, str);

    return true;
  }

  return false;
}

QVariant
CQBaseModel::
data(const QModelIndex &index, int role) const
{
  if (role == Qt::TextAlignmentRole) {
    Type type = columnType(index.column());

    if (type == Type::INTEGER || type == Type::REAL)
      return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    else
      return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
  }

  //return QAbstractItemModel::data(index, role);
  return QVariant();
}

//------

CQBaseModel::Type
CQBaseModel::
variantToType(const QVariant &var, bool *ok)
{
  Type type = Type::NONE;

  if (var.type() == QVariant::Int) {
    type = (Type) var.toInt(ok);
  }
  else {
    QString str = var.toString();

    type = nameType(str);
  }

  if (! isType((int) type)) {
    if (ok)
      *ok = false;
  }

  return type;
}

QVariant
CQBaseModel::
typeToVariant(Type type)
{
  return QVariant((int) type);
}

bool
CQBaseModel::
isSameType(const QVariant &var, Type type)
{
  if (type == Type::REAL && var.type() == QVariant::Double)
    return true;

  if (type == Type::INTEGER && var.type() == QVariant::Int)
    return true;

  return false;
}

QVariant
CQBaseModel::
typeStringToVariant(const QString &str, Type type)
{
  if      (type == Type::REAL) {
    bool ok;

    double real = toReal(str, ok);

    if (ok)
      return QVariant(real);
  }
  else if (type == Type::INTEGER) {
    bool ok;

    long integer = toInt(str, ok);

    if (ok)
      return QVariant(int(integer));
  }

  return QVariant(str);
}

bool
CQBaseModel::
isType(int type)
{
  if      (type == (int) Type::REAL   ) return true;
  else if (type == (int) Type::INTEGER) return true;
  else if (type == (int) Type::STRING ) return true;
  else if (type == (int) Type::TIME   ) return true;

  return false;
}

QString
CQBaseModel::
typeName(Type type)
{
  if (! isType((int) type))
    return "none";

  if      (type == Type::REAL)
    return "real";
  else if (type == Type::INTEGER)
    return "integer";
  else if (type == Type::STRING)
    return "string";
  else if (type == Type::TIME)
    return "time";
  else
    assert(false);
}

CQBaseModel::Type
CQBaseModel::
nameType(const QString &name)
{
  if      (name == "real")
    return Type::REAL;
  else if (name == "integer")
    return Type::INTEGER;
  else if (name == "string")
    return Type::STRING;
  else if (name == "time")
    return Type::TIME;
  else
    return Type::NONE;
}

double
CQBaseModel::
toReal(const QString &str, bool &ok)
{
  ok = true;

  double r = 0.0;

  //---

  std::string sstr = str.toStdString();

  const char *c_str = sstr.c_str();

  int i = 0;

  while (c_str[i] != 0 && ::isspace(c_str[i]))
    ++i;

  if (c_str[i] == '\0') {
    ok = false;
    return r;
  }

  const char *p;

#ifdef ALLOW_NAN
  if (COS::has_nan() && strncmp(&c_str[i], "NaN", 3) == 0)
    p = &c_str[i + 3];
  else {
    errno = 0;

    r = strtod(&c_str[i], (char **) &p);

    if (errno == ERANGE) {
      ok = false;
      return r;
    }
  }
#else
  errno = 0;

  r = strtod(&c_str[i], (char **) &p);

  if (errno == ERANGE) {
    ok = false;
    return r;
  }
#endif

  while (*p != 0 && ::isspace(*p))
    ++p;

  if (*p != '\0')
    ok = false;

  return r;
}

long
CQBaseModel::
toInt(const QString &str, bool &ok)
{
  ok = true;

  long integer = 0;

  std::string sstr = str.toStdString();

  const char *c_str = sstr.c_str();

  int i = 0;

  while (c_str[i] != 0 && ::isspace(c_str[i]))
    ++i;

  if (c_str[i] == '\0') {
    ok = false;
    return integer;
  }

  const char *p;

  errno = 0;

  integer = strtol(&c_str[i], (char **) &p, 10);

  if (errno == ERANGE) {
    ok = false;
    return integer;
  }

  while (*p != 0 && ::isspace(*p))
    ++p;

  if (*p != '\0') {
    ok = false;
    return integer;
  }

  return integer;
}

bool
CQBaseModel::
isInteger(double r)
{
  return std::abs(r - int(r)) < 1E-3;
}
