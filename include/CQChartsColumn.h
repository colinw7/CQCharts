#ifndef CQChartsColumn_H
#define CQChartsColumn_H

#include <QString>
#include <QStringList>
#include <iostream>

class CQChartsColumn {
 public:
  enum class Type {
    NONE,
    DATA,
    VHEADER,
    EXPR
  };

 public:
  static void registerMetaType();

 public:
  CQChartsColumn() = default;

  CQChartsColumn(int column) :
   type_(Type::DATA), column_(column) {
    if (column_ < 0)
      type_ = Type::NONE;
  }

  CQChartsColumn(const QString &s) {
    setValue(s);
  }

  CQChartsColumn(const CQChartsColumn &rhs) :
   type_(rhs.type_), column_(rhs.column_), expr_(nullptr) {
    if (rhs.type_ == Type::EXPR && rhs.expr_) {
      int len = strlen(rhs.expr_);

      expr_ = new char [len + 1];

      memcpy(expr_, rhs.expr_, len + 1);
    }
  }

 ~CQChartsColumn() {
    delete [] expr_;
  }

  CQChartsColumn &operator=(const CQChartsColumn &rhs) {
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

  Type type() const { return type_; }

  int column() const { return (type_ == Type::DATA ? column_ : -1); }

  QString expr() const { return (type_ == Type::EXPR && expr_ ? QString(expr_) : QString()); }

  bool isValid() const { return type_ != Type::NONE; }

  bool setValue(const QString &str) {
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

  //---

  QString toString() const {
    if      (type_ == Type::DATA)
      return QString("%1").arg(column_);
    else if (type_ == Type::EXPR)
      return QString("(%1)").arg(expr_);
    else if (type_ == Type::VHEADER)
      return "@VH";

    return "";
  }

  void fromString(const QString &s) {
    setValue(s);
  }

  //---

  friend bool operator==(const CQChartsColumn &lhs, const CQChartsColumn &rhs) {
    if (lhs.type_   != rhs.type_  ) return false;
    if (lhs.column_ != rhs.column_) return false;

    if (lhs.expr_ != rhs.expr_) {
      if ((lhs.expr_ && ! rhs.expr_) || (! lhs.expr_ && rhs.expr_))
        return false;

      if (! lhs.expr_ || ! rhs.expr_)
        return false;

      if (strcmp(lhs.expr_, rhs.expr_) != 0)
        return false;
    }

    return true;
  }

  friend bool operator!=(const CQChartsColumn &lhs, const CQChartsColumn &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsColumn &l) {
    l.print(os);

    return os;
  }

  //---

 public:
  static bool stringToColumns(const QString &str, std::vector<CQChartsColumn> &columns) {
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

  static QString columnsToString(const std::vector<CQChartsColumn> &columns) {
    QString str;

    for (const auto &c : columns) {
      if (str.length())
        str += " ";

      str += c.toString();
    }

    return str;
  }

 private:
  bool decodeString(const QString &str, Type &type, int &column, QString &expr) {
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

 private:
  Type   type_   { Type::NONE };
  int    column_ { -1 };
  char*  expr_   { nullptr };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsColumn)

#endif
