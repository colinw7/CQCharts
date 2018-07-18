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
  resetColumnTypes();

  // auto determine type for each column. Do column by column to allow early out
  int nc = columnCount();

  for (int column = 0; column < nc; ++column)
    genColumnType(column);
}

void
CQBaseModel::
genColumnType(int column)
{
  ColumnData &columnData = getColumnData(column);

  genColumnType(columnData);
}

void
CQBaseModel::
genColumnType(ColumnData &columnData)
{
  ColumnTypeData columnTypeData;

  QModelIndex parent;

  (void) genColumnType(parent, columnData.column, columnTypeData);

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

  if (columnTypeData.type != columnData.type) {
    columnData.type = columnTypeData.type;

    emit columnTypeChanged(columnData.column);
  }
}

bool
CQBaseModel::
genColumnType(const QModelIndex &parent, int c, ColumnTypeData &columnTypeData)
{
  bool done = false;

  int nr = rowCount(parent);

  for (int r = 0; r < nr && r < maxTypeRows(); ++r) {
    QModelIndex ind0 = index(r, 0, parent);

    if (rowCount(ind0) > 0) {
      if (genColumnType(ind0, c, columnTypeData)) {
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
  if (column < 0 || column >= columnCount())
    return Type::NONE;

  ColumnData &columnData = getColumnData(column);

  if (columnData.type == Type::NONE) {
    CQBaseModel *th = const_cast<CQBaseModel *>(this);

    th->genColumnType(columnData);
  }

  return columnData.type;
}

bool
CQBaseModel::
setColumnType(int column, Type type)
{
  if (column < 0 || column >= columnCount())
    return false;

  ColumnData &columnData = getColumnData(column);

  if (type != columnData.type) {
    columnData.type = type;

    emit columnTypeChanged(column);
  }

  return true;
}

QString
CQBaseModel::
columnTypeValues(int column) const
{
  if (column < 0 || column >= columnCount())
    return QString();

  ColumnData &columnData = getColumnData(column);

  return columnData.typeValues;
}

bool
CQBaseModel::
setColumnTypeValues(int column, const QString &str)
{
  if (column < 0 || column >= columnCount())
    return false;

  ColumnData &columnData = getColumnData(column);

  columnData.typeValues = str;

  emit columnTypeChanged(column);

  return true;
}

QVariant
CQBaseModel::
columnMin(int column) const
{
  if (column < 0 || column >= columnCount())
    return QVariant();

  ColumnData &columnData = getColumnData(column);

  return columnData.min;
}

bool
CQBaseModel::
setColumnMin(int column, const QVariant &v)
{
  if (column < 0 || column >= columnCount())
    return false;

  ColumnData &columnData = getColumnData(column);

  columnData.min = v;

  emit columnRangeChanged(column);

  return true;
}

QVariant
CQBaseModel::
columnMax(int column) const
{
  if (column < 0 || column >= columnCount())
    return QVariant();

  ColumnData &columnData = getColumnData(column);

  return columnData.max;
}

bool
CQBaseModel::
setColumnMax(int column, const QVariant &v)
{
  if (column < 0 || column >= columnCount())
    return false;

  ColumnData &columnData = getColumnData(column);

  columnData.max = v;

  emit columnRangeChanged(column);

  return true;
}

CQBaseModel::ColumnData &
CQBaseModel::
getColumnData(int column) const
{
  auto p = columnDatas_.find(column);

  if (p != columnDatas_.end()) {
    const ColumnData &columnData = (*p).second;

    assert(columnData.column == column);

    return const_cast<CQBaseModel::ColumnData &>(columnData);
  }

  assert(column >= 0 || column < columnCount());

  CQBaseModel *th = const_cast<CQBaseModel *>(this);

  auto p1 = th->columnDatas_.insert(p, ColumnDatas::value_type(column, ColumnData(column)));

  return (*p1).second;
}

void
CQBaseModel::
resetColumnTypes()
{
  for (auto &p : columnDatas_) {
    ColumnData &columnData = p.second;

    columnData.type = Type::NONE;
  }
}

//------

QVariant
CQBaseModel::
rowGroup(int row) const
{
  if (row < 0 || row >= rowCount())
    return QVariant();

  RowData &rowData = getRowData(row);

  return rowData.group;
}

bool
CQBaseModel::
setRowGroup(int row, const QVariant &v)
{
  if (row < 0 || row >= rowCount())
    return false;

  RowData &rowData = getRowData(row);

  rowData.group = v;

  return true;
}

CQBaseModel::RowData &
CQBaseModel::
getRowData(int row) const
{
  auto p = rowDatas_.find(row);

  if (p != rowDatas_.end()) {
    const RowData &rowData = (*p).second;

    assert(rowData.row == row);

    return const_cast<CQBaseModel::RowData &>(rowData);
  }

  assert(row >= 0 || row < rowCount());

  CQBaseModel *th = const_cast<CQBaseModel *>(this);

  auto p1 = th->rowDatas_.insert(p, RowDatas::value_type(row, RowData(row)));

  return (*p1).second;
}

//------

QVariant
CQBaseModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  // generic column data
  if      (orientation == Qt::Horizontal) {
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
    else {
      return QAbstractItemModel::headerData(section, orientation, role);
    }
  }
  // generic row data
  else if (orientation == Qt::Vertical) {
    if (role == static_cast<int>(Role::Group)) {
      return rowGroup(section);
    }
    else {
      return QAbstractItemModel::headerData(section, orientation, role);
    }
  }
  else {
    assert(false);
  }

  return QVariant();
}

bool
CQBaseModel::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  // generic column data
  if      (orientation == Qt::Horizontal) {
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
    else {
      return QAbstractItemModel::setHeaderData(section, orientation, role);
    }
  }
  // generic row data
  else if (orientation == Qt::Vertical) {
    if (role == static_cast<int>(Role::Group)) {
      return setRowGroup(section, value);
    }
    else {
      return QAbstractItemModel::setHeaderData(section, orientation, role);
    }
  }
  else {
    assert(false);
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

#if 0
  if (type == Type::TIME && var.type() == QVariant::Double)
    return true;
#endif

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
#if 0
  else if (type == Type::TIME) {
    bool ok;

    double real = toReal(str, ok);

    if (ok)
      return QVariant(real);
  }
#endif

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
