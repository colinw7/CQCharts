#ifndef CQChartsFillUnder_H
#define CQChartsFillUnder_H

#include <CMathUtil.h>
#include <QString>
#include <QStringList>
#include <iostream>

/*!
 * \brief fill under side
 * \ingroup Charts
 */
class CQChartsFillUnderSide {
 public:
  enum Type {
    BOTH,
    ABOVE,
    BELOW
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

  static QStringList sideNames();

 public:
  CQChartsFillUnderSide(Type type=Type::BOTH) :
   type_(type) {
  }

  CQChartsFillUnderSide(const QString &s) {
    setValue(s);
  }

  CQChartsFillUnderSide(const CQChartsFillUnderSide &rhs) :
    type_(rhs.type_) {
  }

  CQChartsFillUnderSide &operator=(const CQChartsFillUnderSide &rhs) {
    type_ = rhs.type_;

    return *this;
  }

  const Type &type() const { return type_; }

  bool setValue(const QString &str) {
    Type type;

    if (! decodeString(str, type))
      return false;

    type_ = type;

    return true;
  }

  //---

  QString toString() const;

  bool fromString(const QString &s) {
    return setValue(s);
  }

  //---

  friend bool operator==(const CQChartsFillUnderSide &lhs, const CQChartsFillUnderSide &rhs) {
    if (lhs.type_ != rhs.type_) return false;

    return true;
  }

  friend bool operator!=(const CQChartsFillUnderSide &lhs, const CQChartsFillUnderSide &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsFillUnderSide &l) {
    l.print(os);

    return os;
  }

  //---

 private:
  bool decodeString(const QString &str, Type &type);

 private:
  Type type_ { Type::BOTH };
};

//---

/*!
 * \brief fill under position
 * \ingroup Charts
 */
class CQChartsFillUnderPos {
 public:
  enum Type {
    NONE,
    MIN,
    MAX,
    POS
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsFillUnderPos() { }

  CQChartsFillUnderPos(Type xtype, double xpos, Type ytype, double ypos) :
   xtype_(xtype), xpos_(xpos), ytype_(ytype), ypos_(ypos) {
  }

  CQChartsFillUnderPos(const QString &s) {
    setValue(s);
  }

  CQChartsFillUnderPos(const CQChartsFillUnderPos &rhs) :
    xtype_(rhs.xtype_), xpos_(rhs.xpos_), ytype_(rhs.ytype_), ypos_(rhs.ypos_) {
  }

  CQChartsFillUnderPos &operator=(const CQChartsFillUnderPos &rhs) {
    xtype_ = rhs.xtype_;
    xpos_  = rhs.xpos_;
    ytype_ = rhs.ytype_;
    ypos_  = rhs.ypos_;

    return *this;
  }

  const Type &xtype() const { return xtype_; }
  double      xpos () const { return xpos_ ; }

  const Type &ytype() const { return ytype_; }
  double      ypos () const { return ypos_ ; }

  bool setValue(const QString &str) {
    Type xtype { Type::NONE }; double xpos { 0.0 };
    Type ytype { Type::NONE }; double ypos { 0.0 };

    if (! decodeString(str, xtype, xpos, ytype, ypos))
      return false;

    xtype_ = xtype;
    xpos_  = xpos;
    ytype_ = ytype;
    ypos_  = ypos;

    return true;
  }

  bool isValid() const { return xtype_ != Type::NONE || ytype_ != Type::NONE; }

  //---

  QString toString() const;

  bool fromString(const QString &s) {
    return setValue(s);
  }

  //---

  friend bool operator==(const CQChartsFillUnderPos &lhs, const CQChartsFillUnderPos &rhs) {
    if (lhs.xtype_ != rhs.xtype_) return false;
    if (lhs.ytype_ != rhs.ytype_) return false;

    if (lhs.xtype_ == Type::POS) {
      if (! CMathUtil::realEq(lhs.xpos_, rhs.xpos_))
        return false;
    }

    if (lhs.ytype_ == Type::POS) {
      if (! CMathUtil::realEq(lhs.ypos_, rhs.ypos_))
        return false;
    }

    return true;
  }

  friend bool operator!=(const CQChartsFillUnderPos &lhs, const CQChartsFillUnderPos &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsFillUnderPos &l) {
    l.print(os);

    return os;
  }

  //---

 private:
  bool decodeString(const QString &str, Type &xtype, double &xpos, Type &ytype, double &ypos);

 private:
  Type   xtype_ { Type::NONE };
  double xpos_  { 0.0 };
  Type   ytype_ { Type::MIN };
  double ypos_  { 0.0 };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsFillUnderSide)
CQUTIL_DCL_META_TYPE(CQChartsFillUnderPos)

#endif
