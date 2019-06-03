#include <CQChartsColumn.h>
#include <CQChartsUtil.h>
#include <CQPropertyView.h>
#include <CQTclUtil.h>

CQUTIL_DEF_META_TYPE(CQChartsColumn, toString, fromString)

int CQChartsColumn::metaTypeId;

void
CQChartsColumn::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsColumn);

  CQPropertyViewMgrInst->setUserName("CQChartsColumn", "column");
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
CQChartsColumn(Type type, int column, const QString &s, int role) :
 type_(type), column_(column), role_(role)
{
  if (type_ == Type::EXPR || type_ == Type::DATA_INDEX) {
    int len = s.length();

    expr_ = new char [len + 1];

    memcpy(expr_, s.toLatin1().constData(), len + 1);
  }
}

CQChartsColumn::
CQChartsColumn(const CQChartsColumn &rhs) :
 type_(rhs.type_), column_(rhs.column_), role_(rhs.role_), expr_(nullptr)
{
  if (rhs.hasExpr() || rhs.hasIndex()) {
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

  if (rhs.hasExpr() || rhs.hasIndex()) {
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

  if (type == Type::EXPR || type == Type::DATA_INDEX) {
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
  else if (type_ == Type::DATA_INDEX) {
    if (role_ >= 0)
      return QString("%1@%2[%3]").arg(column_).arg(role_).arg(expr_);
    else
      return QString("%1[%2]").arg(column_).arg(expr_);
  }

  return "";
}

bool
CQChartsColumn::
fromString(const QString &s)
{
  if (! setValue(s))
    type_ = Type::NONE;

  return (type_ != Type::NONE);
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
    if (! lhs.expr_) return -1;
    if (! rhs.expr_) return  1;

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
  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  bool rc = true;

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

    while (c_str[i] != '\0') {
      if      (c_str[i] == '(')
        ++nb;
      else if (c_str[i] == ')') {
        --nb;

        if (nb == 0)
          break;
      }

      str += c_str[i++];
    }

    //std::string rhs = str.mid(i);

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

  //---

  // integer column number
  bool ok;

  const char *p;

  long column1 = CQChartsUtil::toInt(&c_str[i], ok, &p);

  if (! ok)
    return false;

  if (column1 < 0)
    return false;

  //---

  // optional role
  long role1 = -1;

  if (*p == '@') {
    ++p;

    bool ok1;

    const char *p1;

    role1 = CQChartsUtil::toInt(p, ok1, &p1);

    if (! ok1)
      return false;

    if (role1 < 0)
      return false;

    p = p1;
  }

  //---

  // optional index string
  QString indexStr;

  if (*p == '[') {
    ++p;

    while (*p != '\0' && *p != ']') {
      indexStr += *p;

      ++p;
    }

    if (*p == ']')
      ++p;

    while (*p != 0 && ::isspace(*p))
      ++p;
  }

  //---

  if (*p != '\0')
    return false;

  //---

  if (indexStr.length()) {
    type = Type::DATA_INDEX;
    expr = indexStr.simplified();
  }
  else
    type = Type::DATA;

  column = column1;
  role   = role1;

  return true;
}

//---

CQUTIL_DEF_META_TYPE(CQChartsColumns, toString, fromString)

int CQChartsColumns::metaTypeId;

void
CQChartsColumns::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsColumns);

  CQPropertyViewMgrInst->setUserName("CQChartsColumns", "column_list");
}
