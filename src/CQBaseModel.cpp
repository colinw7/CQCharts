#include <CQBaseModel.h>
#include <cmath>
#include <cassert>

namespace {

using TypeName = std::map<CQBaseModel::Type,QString>;
using NameType = std::map<QString,CQBaseModel::Type>;

static TypeName s_typeName;
static NameType s_nameType;

void addType(CQBaseModel::Type type, const QString &name) {
  s_typeName[type] = name;
  s_nameType[name] = type;
}

void initTypes() {
  if (s_typeName.empty()) {
    addType(CQBaseModel::Type::BOOLEAN, "boolean");
    addType(CQBaseModel::Type::INTEGER, "integer");
    addType(CQBaseModel::Type::REAL   , "real"   );
    addType(CQBaseModel::Type::STRING , "string" );
    addType(CQBaseModel::Type::STRINGS, "strings");
    addType(CQBaseModel::Type::POINT  , "point"  );
    addType(CQBaseModel::Type::LINE   , "line"   );
    addType(CQBaseModel::Type::RECT   , "rect"   );
    addType(CQBaseModel::Type::SIZE   , "size"   );
    addType(CQBaseModel::Type::POLYGON, "polygon");
    addType(CQBaseModel::Type::COLOR  , "color"  );
    addType(CQBaseModel::Type::PEN    , "pen"    );
    addType(CQBaseModel::Type::BRUSH  , "brush"  );
    addType(CQBaseModel::Type::IMAGE  , "image"  );
    addType(CQBaseModel::Type::TIME   , "time"   );
    addType(CQBaseModel::Type::PATH   , "path"   );
    addType(CQBaseModel::Type::STYLE  , "style"  );
  }
}

};

//---

CQBaseModel::
CQBaseModel()
{
  initTypes();
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
      if      (columnTypeData.numReal)
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

bool
CQBaseModel::
setColumnType(int column, Type type)
{
  columnDatas_[column].type = type;

  return true;
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

bool
CQBaseModel::
setColumnTypeValues(int column, const QString &str)
{
  columnDatas_[column].typeValues = str;

  return true;
}

QVariant
CQBaseModel::
columnMin(int column) const
{
  auto p = columnDatas_.find(column);

  if (p == columnDatas_.end())
    return QVariant();

  return (*p).second.min;
}

bool
CQBaseModel::
setColumnMin(int column, const QVariant &v)
{
  columnDatas_[column].min = v;

  return true;
}

QVariant
CQBaseModel::
columnMax(int column) const
{
  auto p = columnDatas_.find(column);

  if (p == columnDatas_.end())
    return QVariant();

  return (*p).second.max;
}

bool
CQBaseModel::
setColumnMax(int column, const QVariant &v)
{
  columnDatas_[column].max = v;

  return true;
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

  if      (role == static_cast<int>(Role::Type)) {
    return QVariant((int) columnType(section));
  }
  else if (role == static_cast<int>(Role::TypeValues)) {
    return QVariant(columnTypeValues(section));
  }
  else if (role == static_cast<int>(Role::Min)) {
    return columnMin(section);
  }
  else if (role == static_cast<int>(Role::Max)) {
    return columnMax(section);
  }

  return QVariant();
}

bool
CQBaseModel::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  if (orientation != Qt::Horizontal)
    return false;

  if      (role == static_cast<int>(Role::Type)) {
    bool ok;

    Type type = variantToType(value, &ok);
    if (! ok) return false;

    return setColumnType(section, type);
  }
  else if (role == static_cast<int>(Role::TypeValues)) {
    QString str = value.toString();

    return setColumnTypeValues(section, str);
  }
  else if (role == static_cast<int>(Role::Min)) {
    return setColumnMin(section, value);
  }
  else if (role == static_cast<int>(Role::Max)) {
    return setColumnMax(section, value);
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

  if (type == Type::TIME && var.type() == QVariant::Double)
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
  else if (type == Type::TIME) {
    bool ok;

    double real = toReal(str, ok);

    if (ok)
      return QVariant(real);
  }

  return QVariant(str);
}

bool
CQBaseModel::
isType(int type)
{
  return (s_typeName.find((CQBaseModel::Type) type) != s_typeName.end());
}

QString
CQBaseModel::
typeName(Type type)
{
  auto p = s_typeName.find(type);

  if (p == s_typeName.end())
    return "none";

  return (*p).second;
}

CQBaseModel::Type
CQBaseModel::
nameType(const QString &name)
{
  auto p = s_nameType.find(name);

  if (p == s_nameType.end())
    return Type::NONE;

  return (*p).second;
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
