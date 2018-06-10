#include <CQChartsColumn.h>

CQUTIL_DEF_META_TYPE(CQChartsColumn, toString, fromString)

void
CQChartsColumn::
registerMetaType()
{
  CQUTIL_REGISTER_META(CQChartsColumn);
}

CQChartsColumn::
CQChartsColumn(int column) :
 type_(Type::DATA), column_(column)
{
  if (column_ < 0)
    type_ = Type::NONE;
}

CQChartsColumn::
CQChartsColumn(const QString &s)
{
  setValue(s);
}

CQChartsColumn::
CQChartsColumn(const CQChartsColumn &rhs) :
 type_(rhs.type_), column_(rhs.column_), expr_(nullptr), mapped_(rhs.mapped_)
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
  QString expr;

  if (! decodeString(str, type, column, expr))
    return false;

  delete [] expr_;

  type_   = type;
  column_ = column;
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
  if      (type_ == Type::DATA)
    return QString("%1").arg(column_);
  else if (type_ == Type::EXPR)
    return QString("(%1)").arg(expr_);
  else if (type_ == Type::VHEADER)
    return "@VH";

  return "";
}

void
CQChartsColumn::
fromString(const QString &s)
{
  setValue(s);
}

int
CQChartsColumn::
cmp(const CQChartsColumn &lhs, const CQChartsColumn &rhs)
{
  if (lhs.type_ < rhs.type_) return -1;
  if (lhs.type_ > rhs.type_) return  1;

  if (lhs.column_ < rhs.column_) return -1;
  if (lhs.column_ > rhs.column_) return  1;

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
stringToColumns(const QString &str, std::vector<CQChartsColumn> &columns)
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
columnsToString(const std::vector<CQChartsColumn> &columns)
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
decodeString(const QString &str, Type &type, int &column, QString &expr)
{
  type   = Type::NONE;
  column = -1;
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

  if (strcmp(&c_str[i], "@VH") == 0) {
    type = Type::VHEADER;

    return true;
  }

  // TODO: support column name

  // integer column number
  const char *p;

  errno = 0;

  long value = strtol(&c_str[i], (char **) &p, 10);

  if (errno == ERANGE)
    return false;

  while (*p != 0 && ::isspace(*p))
    ++p;

  if (*p == '\0') {
    if (value < 0)
      return false;

    type   = Type::DATA;
    column = value;
  }
  else
    return false;

  return true;
}
