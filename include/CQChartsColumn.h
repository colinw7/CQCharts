#ifndef CQChartsColumn_H
#define CQChartsColumn_H

#include <QString>
#include <QStringList>
#include <vector>
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
  CQChartsColumn(int column);
  CQChartsColumn(const QString &s);

  CQChartsColumn(const CQChartsColumn &rhs);

 ~CQChartsColumn();

  CQChartsColumn &operator=(const CQChartsColumn &rhs);

  Type type() const { return type_; }

  int column() const { return (type_ == Type::DATA ? column_ : -1); }

  QString expr() const { return (type_ == Type::EXPR && expr_ ? QString(expr_) : QString()); }

  bool isValid() const { return type_ != Type::NONE; }

  bool setValue(const QString &str);

  bool isMapped() const { return mapped_; }
  void setMapped(bool b) { mapped_ = b; }

  double mapMin() const { return mapMin_; }
  void setMapMin(double r) { mapMin_ = r; }

  double mapMax() const { return mapMax_; }
  void setMapMax(double r) { mapMax_ = r; }

  //---

  QString toString() const;

  void fromString(const QString &s);

  //---

  static int cmp(const CQChartsColumn &lhs, const CQChartsColumn &rhs);

  friend bool operator==(const CQChartsColumn &lhs, const CQChartsColumn &rhs) {
    return cmp(lhs, rhs) == 0;
  }

  friend bool operator!=(const CQChartsColumn &lhs, const CQChartsColumn &rhs) {
    return ! operator==(lhs, rhs);
  }

  friend bool operator<(const CQChartsColumn &lhs, const CQChartsColumn &rhs) {
    return cmp(lhs, rhs) < 0;
  }

  //---

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CQChartsColumn &l) {
    l.print(os);

    return os;
  }

  //---

 public:
  static bool stringToColumns(const QString &str, std::vector<CQChartsColumn> &columns);

  static QString columnsToString(const std::vector<CQChartsColumn> &columns);

 private:
  bool decodeString(const QString &str, Type &type, int &column, QString &expr);

 private:
  Type   type_   { Type::NONE };
  int    column_ { -1 };
  char*  expr_   { nullptr };
  bool   mapped_ { false };
  double mapMin_ { 0.0 };
  double mapMax_ { 1.0 };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsColumn)

#endif
