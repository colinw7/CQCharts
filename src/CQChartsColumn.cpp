#include <CQChartsColumn.h>

CQUTIL_DEF_META_TYPE(CQChartsColumn, toString, fromString)

void
CQChartsColumn::
registerMetaType()
{
  CQUTIL_REGISTER_META(CQChartsColumn);
}

CQChartsColumn::
CQChartsColumn(int column, int role) :
 type_(Type::DATA), column_(column), role_(role)
{
  if (column_ < 0)
    type_ = Type::NONE;
}

CQChartsColumn::
CQChartsColumn(const QString &s)
{
  if (! setValue(s))
    type_ = Type::NONE;
}

CQChartsColumn::
CQChartsColumn(const CQChartsColumn &rhs) :
 type_(rhs.type_), column_(rhs.column_), role_(rhs.role_), expr_(nullptr), mapped_(rhs.mapped_)
{
  if (rhs.type_ == Type::EXPR && rhs.expr_) {
    int len = strlen(rhs.expr_);

    expr_ = new char [len + 1];

    memcpy(expr_, rhs.expr_, len + 1);
  }
}

CQChartsColumn::
~CQChartsColumn()
{
  delete [] expr_;
}

CQChartsColumn &
CQChartsColumn::
operator=(const CQChartsColumn &rhs)
{
  delete [] expr_;

  type_   = rhs.type_;
  column_ = rhs.column_;
  role_   = rhs.role_;
  expr_   = nullptr;

  if (rhs.type_ == Type::EXPR && rhs.expr_) {
    int len = strlen(rhs.expr_);

    expr_ = new char [len + 1];

    memcpy(expr_, rhs.expr_, len + 1);
  }

  return *this;
}

bool
CQChartsColumn::
setValue(const QString &str)
{
  Type    type;
  int     column;
  int     role;
  QString expr;

  if (! decodeString(str, type, column, role, expr))
    return false;

  delete [] expr_;

  type_   = type;
  column_ = column;
  role_   = role;
  expr_   = nullptr;

  if (type == Type::EXPR) {
    int len = expr.length();

    expr_ = new char [len + 1];

    memcpy(expr_, expr.toStdString().c_str(), len + 1);
  }

  return true;
}

QString
CQChartsColumn::
toString() const
{
  if      (type_ == Type::DATA) {
    if (role_ >= 0)
      return QString("%1@%2").arg(column_).arg(role_);
    else
      return QString("%1").arg(column_);
  }
  else if (type_ == Type::EXPR)
    return QString("(%1)").arg(expr_);
  else if (type_ == Type::VHEADER)
    return "@VH";
  else if (type_ == Type::GROUP)
    return "@GROUP";

  return "";
}

void
CQChartsColumn::
fromString(const QString &s)
{
  if (! setValue(s))
    type_ = Type::NONE;
}

int
CQChartsColumn::
cmp(const CQChartsColumn &lhs, const CQChartsColumn &rhs)
{
  if (lhs.type_ < rhs.type_) return -1;
  if (lhs.type_ > rhs.type_) return  1;

  if (lhs.column_ < rhs.column_) return -1;
  if (lhs.column_ > rhs.column_) return  1;

  if (lhs.role_ < rhs.role_) return -1;
  if (lhs.role_ > rhs.role_) return  1;

  if (lhs.expr_ != rhs.expr_) {
    if (! lhs.expr_ &&   rhs.expr_) return -1;
    if (  lhs.expr_ && ! rhs.expr_) return  1;

    return strcmp(lhs.expr_, rhs.expr_);
  }

  return 0;
}

void
CQChartsColumn::
print(std::ostream &os) const
{
  os << toString().toStdString();
}

bool
CQChartsColumn::
stringToColumns(const QString &str, Columns &columns)
{
  bool rc = true;

  QStringList strs = str.split(" ", QString::SkipEmptyParts);

  for (int i = 0; i < strs.length(); ++i) {
    CQChartsColumn c(strs[i]);

    if (! c.isValid())
      rc = false;

    columns.push_back(c);
  }

  return rc;
}

QString
CQChartsColumn::
columnsToString(const Columns &columns)
{
  QString str;

  for (const auto &c : columns) {
    if (str.length())
      str += " ";

    str += c.toString();
  }

  return str;
}

bool
CQChartsColumn::
decodeString(const QString &str, Type &type, int &column, int &role, QString &expr)
{
  type   = Type::NONE;
  column = -1;
  role   = -1;
  expr   = "";

  std::string sstr = str.toStdString();

  const char *c_str = sstr.c_str();

  int i = 0;

  while (c_str[i] != 0 && ::isspace(c_str[i]))
    ++i;

  if (c_str[i] == '\0')
    return false;

  // expression
  if (c_str[i] == '(') {
    ++i;

    QString str;

    int nb = 1;

    while (c_str[i] != 0) {
      if      (c_str[i] == '(')
        ++nb;
      else if (c_str[i] == ')') {
        --nb;

        if (nb == 0)
          break;
      }

      str += c_str[i++];
    }

    expr = str.simplified();
    type = Type::EXPR;

    return true;
  }

  if (strcmp(&c_str[i], "@VH") == 0 || strcmp(&c_str[i], "@VHEADER") == 0) {
    type = Type::VHEADER;

    return true;
  }

  if (strcmp(&c_str[i], "@GROUP") == 0) {
    type = Type::GROUP;

    return true;
  }

  // TODO: support column name (need model)

  // integer column number
  const char *p;

  errno = 0;

  long value = strtol(&c_str[i], (char **) &p, 10);

  if (errno == ERANGE)
    return false;

  if (*p == '@') {
    ++p;

    const char *p1;

    errno = 0;

    long value1 = strtol(p, (char **) &p1, 10);

    if (errno == ERANGE)
      return false;

    while (*p1 != 0 && ::isspace(*p1))
      ++p1;

    if (*p1 != '\0')
      return false;

    if (value < 0 || value1 < 0)
      return false;

    type   = Type::DATA;
    column = value;
    role   = value1;
  }
  else {
    while (*p != 0 && ::isspace(*p))
      ++p;

    if (*p != '\0')
      return false;

    if (value < 0)
      return false;

    type   = Type::DATA;
    column = value;
  }

  return true;
}
