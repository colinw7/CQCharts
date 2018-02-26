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
  int nc = columnCount();

  resetColumnTypes();

  // auto determine type for each column. Do column by column to allow early out
  for (int c = 0; c < nc; ++c) {
    ColumnTypeData columnTypeData;

    QModelIndex parent;

    (void) genColumnTypes(parent, c, columnTypeData);

    // if inderminate (no values or all reals or integers) then use real if any reals,
    // integer if any integers and string if no values.
    if (columnTypeData.type == Type::NONE) {
      if     (columnTypeData.numReal)
        columnTypeData.type = Type::REAL;
      else if (columnTypeData.numInt)
        columnTypeData.type = Type::INTEGER;
      else
        columnTypeData.type = Type::STRING;
    }

    setColumnType(c, columnTypeData.type);
  }
}

bool
CQBaseModel::
genColumnTypes(const QModelIndex &parent, int c, ColumnTypeData &columnTypeData)
{
  bool done = false;

  int nr = rowCount(parent);

  for (int r = 0; r < nr; ++r) {
    QModelIndex ind0 = index(r, 0, parent);

    if (rowCount(ind0) > 0) {
      if (genColumnTypes(ind0, c, columnTypeData)) {
        done = true;
        break;
      }
    }

    //---

    // get value
    QModelIndex ind = (c != 0 ? index(r, c, parent) : ind0);

    QVariant v = data(ind, Qt::DisplayRole);

    if      (v.type() == QVariant::Double) {
      ++columnTypeData.numReal;
    }
    else if (v.type() == QVariant::Int) {
      ++columnTypeData.numInt;
    }
    else {
      QString s = v.toString();

      if (! s.length())
        continue;

      //---

      // check if string can be converted to real
      bool ok;

      double real = toReal(s, ok);

      // if not real then assume string column and we are done
      if (! ok) {
        columnTypeData.type = Type::STRING;
        done = true;
        break;
      }

      // if real can also be an integer prefer integer
      if (isInteger(real))
        ++columnTypeData.numInt;
      else
        ++columnTypeData.numReal;
    }
  }

  return done;
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
  if      (type == (int) Type::BOOLEAN) return true;
  else if (type == (int) Type::INTEGER) return true;
  else if (type == (int) Type::REAL   ) return true;
  else if (type == (int) Type::STRING ) return true;
  else if (type == (int) Type::POINT  ) return true;
  else if (type == (int) Type::LINE   ) return true;
  else if (type == (int) Type::RECT   ) return true;
  else if (type == (int) Type::SIZE   ) return true;
  else if (type == (int) Type::POLYGON) return true;
  else if (type == (int) Type::COLOR  ) return true;
  else if (type == (int) Type::TIME   ) return true;

  return false;
}

QString
CQBaseModel::
typeName(Type type)
{
  if (! isType((int) type))
    return "none";

  if      (type == Type::BOOLEAN) return "boolean";
  else if (type == Type::INTEGER) return "integer";
  else if (type == Type::REAL   ) return "real";
  else if (type == Type::STRING ) return "string";
  else if (type == Type::POINT  ) return "point";
  else if (type == Type::LINE   ) return "line";
  else if (type == Type::RECT   ) return "rect";
  else if (type == Type::POLYGON) return "polygon";
  else if (type == Type::SIZE   ) return "size";
  else if (type == Type::COLOR  ) return "color";
  else if (type == Type::TIME   ) return "time";
  else                            assert(false);
}

CQBaseModel::Type
CQBaseModel::
nameType(const QString &name)
{
  if      (name == "boolean") return Type::BOOLEAN;
  else if (name == "integer") return Type::INTEGER;
  else if (name == "real"   ) return Type::REAL;
  else if (name == "string" ) return Type::STRING;
  else if (name == "point"  ) return Type::POINT;
  else if (name == "line"   ) return Type::LINE;
  else if (name == "rect"   ) return Type::RECT;
  else if (name == "polygon") return Type::POLYGON;
  else if (name == "size"   ) return Type::SIZE;
  else if (name == "color"  ) return Type::COLOR;
  else if (name == "time"   ) return Type::TIME;
  else                        return Type::NONE;
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
