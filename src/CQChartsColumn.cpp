#include <CQChartsColumn.h>
#include <CQChartsUtil.h>
#include <CQPropertyView.h>
#include <CQTclUtil.h>
#include <CQStrParse.h>

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
  updateType();
}

CQChartsColumn::
CQChartsColumn(const QString &s)
{
  if (! setValue(s))
    type_ = Type::NONE;

  updateType();
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

  updateType();
}

CQChartsColumn::
CQChartsColumn(const CQChartsColumn &rhs) :
 type_(rhs.type_), column_(rhs.column_), role_(rhs.role_)
{
  if (rhs.hasExpr() || rhs.hasIndex()) {
    int len = strlen(rhs.expr_);

    expr_ = new char [len + 1];

    memcpy(expr_, rhs.expr_, len + 1);
  }

  if (rhs.hasName()) {
    int len = strlen(rhs.name_);

    name_ = new char [len + 1];

    memcpy(name_, rhs.name_, len + 1);
  }

  updateType();
}

CQChartsColumn::
CQChartsColumn(CQChartsColumn &&rhs)
{
  std::swap(type_  , rhs.type_  );
  std::swap(column_, rhs.column_);
  std::swap(role_  , rhs.role_  );
  std::swap(expr_  , rhs.expr_  );
  std::swap(name_  , rhs.name_  );
}

CQChartsColumn::
~CQChartsColumn()
{
  delete [] expr_;
  delete [] name_;
}

CQChartsColumn &
CQChartsColumn::
operator=(const CQChartsColumn &rhs)
{
  if (&rhs != this) {
    delete [] expr_;
    delete [] name_;

    type_   = rhs.type_;
    column_ = rhs.column_;
    role_   = rhs.role_;
    expr_   = nullptr;
    name_   = nullptr;

    if (rhs.hasExpr() || rhs.hasIndex()) {
      int len = strlen(rhs.expr_);

      expr_ = new char [len + 1];

      memcpy(expr_, rhs.expr_, len + 1);
    }

    if (rhs.hasName()) {
      int len = strlen(rhs.name_);

      name_ = new char [len + 1];

      memcpy(name_, rhs.name_, len + 1);
    }
  }

  updateType();

  return *this;
}

CQChartsColumn &
CQChartsColumn::
operator=(CQChartsColumn &&rhs)
{
  if (&rhs != this) {
    delete [] expr_;
    delete [] name_;

    type_   = Type::NONE;
    column_ = -1;
    role_   = -1;
    expr_   = nullptr;
    name_   = nullptr;

    std::swap(type_  , rhs.type_  );
    std::swap(column_, rhs.column_);
    std::swap(role_  , rhs.role_  );
    std::swap(expr_  , rhs.expr_  );
    std::swap(name_  , rhs.name_  );
  }

  return *this;
}

void
CQChartsColumn::
setName(const QString &name)
{
  delete [] name_;

  int len = name.length();

  if (len) {
    name_ = new char [len + 1];

    memcpy(name_, name.toStdString().c_str(), len + 1);
  }
  else
    name_ = nullptr;
}

bool
CQChartsColumn::
setValue(const QString &str)
{
  Type    type;
  int     column;
  int     role;
  QString expr;
  QString name;

  if (! decodeString(str, type, column, role, expr, name))
    return false;

  delete [] expr_;
  delete [] name_;

  type_   = type;
  column_ = column;
  role_   = role;
  expr_   = nullptr;
  name_   = nullptr;

  if (type == Type::EXPR || type == Type::DATA_INDEX) {
    int len = expr.length();

    expr_ = new char [len + 1];

    memcpy(expr_, expr.toStdString().c_str(), len + 1);
  }

  if (name != "")
    setName(name);

  updateType();

  return true;
}

QString
CQChartsColumn::
toString() const
{
  QString str;

  if (hasName())
    str = "#" + name() + " ";

  if      (type_ == Type::DATA) {
    str += QString("%1").arg(column_);

    if (role_ >= 0)
      str += QString("@%1").arg(role_);
  }
  else if (type_ == Type::EXPR)
    str += QString("(%1)").arg(expr_);
  else if (type_ == Type::ROW) {
    if (role_ > 0)
      str += "@ROW1";
    else
      str += "@ROW";
  }
  else if (type_ == Type::COLUMN) {
    str += "@COLUMN";
  }
  else if (type_ == Type::CELL) {
    str += "@CELL";
  }
  else if (type_ == Type::HHEADER)
    str += "@HH";
  else if (type_ == Type::VHEADER)
    str += "@VH";
  else if (type_ == Type::GROUP)
    str += "@GROUP";
  else if (type_ == Type::DATA_INDEX) {
    str += QString("%1").arg(column_);

    if (role_ >= 0)
      str += QString("@%1").arg(role_);

    str += QString("[%1]").arg(expr_);
  }

  return str;
}

bool
CQChartsColumn::
fromString(const QString &s)
{
  if (! setValue(s))
    return false;

  return true;
}

int
CQChartsColumn::
cmp(const CQChartsColumn &c) const
{
  if (type_ < c.type_) return -1;
  if (type_ > c.type_) return  1;

  if (column_ < c.column_) return -1;
  if (column_ > c.column_) return  1;

  if (role_ < c.role_) return -1;
  if (role_ > c.role_) return  1;

  if (expr_ != c.expr_) {
    if (!   expr_) return -1;
    if (! c.expr_) return  1;

    return strcmp(expr_, c.expr_);
  }

  if (name_ != c.name_) {
    if (!   name_) return -1;
    if (! c.name_) return  1;

    return strcmp(name_, c.name_);
  }

  return 0;
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
  QStringList strs;

  for (const auto &c : columns) {
    strs += c.toString();
  }

  return CQTcl::mergeList(strs);
}

bool
CQChartsColumn::
decodeString(const QString &str, Type &type, int &column, int &role, QString &expr, QString &name)
{
  type   = Type::NONE;
  column = -1;
  role   = -1;
  expr   = "";
  name   = "";

  CQStrParse parse(str);

  parse.skipSpace();

  if (parse.eof())
    return false;

  //---

  // get optional name
  if (parse.isChar('#')) {
    parse.skipChar();

    int pos = parse.getPos();

    while (! parse.eof()) {
      if      (parse.isChar('\\')) {
        parse.skipChar();

        if (! parse.eof())
          parse.skipChar();
      }
      else if (parse.isSpace())
        break;
      else
        parse.skipChar();
    }

    name = parse.getBefore(pos);

    parse.skipSpace();
  }

  //---

  // expression
  if (parse.isChar('(')) {
    parse.skipChar();

    int nb = 1;

    int pos = parse.getPos();

    while (! parse.eof()) {
      if      (parse.isChar('('))
        ++nb;
      else if (parse.isChar(')')) {
        --nb;

        if (nb == 0)
          break;
      }

      parse.skipChar();
    }

    auto str = parse.getBefore(pos);

    if (parse.isChar(')'))
      parse.skipChar();

    expr = str.trimmed();
    type = Type::EXPR;

    //parse.skipSpace();

    //if (! parse.eof())
    //  return false;

    return true;
  }

  //--

  if (parse.isChar('@')) {
    // row or row offset
    if (parse.isWord("@R") || parse.isWord("@ROW")) {
      type = Type::ROW; return true;
    }

    if (parse.isWord("@R1") || parse.isWord("@ROW1")) {
      role = 1; type = Type::ROW; return true;
    }

    if (parse.isWord("@COL") || parse.isWord("@COLUMN")) {
      type = Type::COLUMN; return true;
    }

    if (parse.isWord("@CELL")) {
      type = Type::CELL; return true;
    }

    //---

    // horizontal header
    if (parse.isWord("@HH") || parse.isWord("@HHEADER")) {
      type = Type::HHEADER; return true;
    }

    // vertical header
    if (parse.isWord("@VH") || parse.isWord("@VHEADER")) {
      type = Type::VHEADER; return true;
    }

    // group
    if (parse.isWord("@GROUP")) {
      type = Type::GROUP; return true;
    }
  }

  //---

  // integer column number
  long column1 { 0 };

  if (parse.readInteger(&column1)) {
    // allow special value for unset
    if (column1 == -1) {
      type = Type::NONE; return true;
    }

    if (column1 < 0)
      return false;
  }

  //---

  // optional role (directly after)
  long role1 = -1;

  if (parse.isChar('@')) {
    parse.skipChar();

    if (! parse.readInteger(&role1))
      return false;

    if (role1 < 0)
      return false;
  }

  //---

  // optional index string (directly after)
  QString indexStr;

  if (parse.isChar('[')) {
    parse.skipChar();

    int pos = parse.getPos();

    while (! parse.eof()) {
      if (parse.isChar(']'))
        break;

      parse.skipChar();
    }

    indexStr = parse.getBefore(pos);

    if (parse.isChar(']'))
      parse.skipChar();
  }

  //---

  parse.skipSpace();

  if (! parse.eof())
    return false;

  //---

  if (indexStr.length()) {
    type = Type::DATA_INDEX;
    expr = indexStr.trimmed();
  }
  else
    type = Type::DATA;

  //---

  column = int(column1);
  role   = int(role1);

  return true;
}

void
CQChartsColumn::
updateType()
{
  if (type_ == Type::DATA && column_ < 0)
    type_ = Type::NONE;
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
