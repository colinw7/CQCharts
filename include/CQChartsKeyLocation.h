#ifndef CQChartsKeyLocation_H
#define CQChartsKeyLocation_H

#include <QString>
#include <QStringList>
#include <iostream>

class CQChartsKeyLocation {
 public:
  enum Type {
    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGHT,
    CENTER_LEFT,
    CENTER_CENTER,
    CENTER_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT,
    ABS_POS
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

  static QStringList locationNames();

 public:
  CQChartsKeyLocation(Type type=Type::TOP_RIGHT) :
   type_(type) {
  }

  CQChartsKeyLocation(const QString &s) {
    setValue(s);
  }

  CQChartsKeyLocation(const CQChartsKeyLocation &rhs) :
    type_(rhs.type_) {
  }

  CQChartsKeyLocation &operator=(const CQChartsKeyLocation &rhs) {
    type_ = rhs.type_;

    return *this;
  }

  const Type &type() const { return type_; }

  bool setValue(const QString &str) {
    Type type { Type::TOP_RIGHT };

    if (! decodeString(str, type))
      return false;

    type_ = type;

    return true;
  }

  //---

  bool onLeft() const {
    return (type_ == Type::TOP_LEFT ||
            type_ == Type::CENTER_LEFT ||
            type_ == Type::BOTTOM_LEFT);
  }
  bool onHCenter() const {
    return (type_ == Type::TOP_CENTER ||
            type_ == Type::CENTER_CENTER ||
            type_ == Type::BOTTOM_CENTER);
  }
  bool onRight() const {
    return (type_ == Type::TOP_RIGHT ||
            type_ == Type::CENTER_RIGHT ||
            type_ == Type::BOTTOM_RIGHT);
  }

  bool onTop() const {
    return (type_ == Type::TOP_LEFT ||
            type_ == Type::TOP_CENTER ||
            type_ == Type::TOP_RIGHT);
  }
  bool onVCenter() const {
    return (type_ == Type::CENTER_LEFT ||
            type_ == Type::CENTER_CENTER ||
            type_ == Type::CENTER_RIGHT);
  }
  bool onBottom() const {
    return (type_ == Type::BOTTOM_LEFT ||
            type_ == Type::BOTTOM_CENTER ||
            type_ == Type::BOTTOM_RIGHT);
  }

  //---

  QString toString() const;

  bool fromString(const QString &s) {
    return setValue(s);
  }

  //---

  friend bool operator==(const CQChartsKeyLocation &lhs, const CQChartsKeyLocation &rhs) {
    if (lhs.type_ != rhs.type_) return false;

    return true;
  }

  friend bool operator!=(const CQChartsKeyLocation &lhs, const CQChartsKeyLocation &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsKeyLocation &l) {
    l.print(os);

    return os;
  }

  //---

 private:
  bool decodeString(const QString &str, Type &type);

 private:
  Type type_ { Type::TOP_RIGHT };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsKeyLocation)

#endif
