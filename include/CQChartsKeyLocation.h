#ifndef CQChartsKeyLocation_H
#define CQChartsKeyLocation_H

#include <CQChartsEnum.h>

/*!
 * \brief key location
 * \ingroup Charts
 */
class CQChartsKeyLocation : public CQChartsEnum {
 public:
  enum Type {
    NONE,
    AUTO,
    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGHT,
    CENTER_LEFT,
    CENTER_CENTER,
    CENTER_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT,
    ABS_POSITION,
    ABS_RECT
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsKeyLocation(Type type=Type::TOP_RIGHT) :
   type_(type) {
  }

  CQChartsKeyLocation(const QString &str) {
    (void) fromString(str);
  }

  //---

  bool isValid() const { return type_ != Type::NONE; }

  const Type &type() const { return type_; }
  void setType(const Type &type) { type_ = type; }

  //---

  QString toString() const override;
  bool fromString(const QString &s) override { return setValue(s); }

  //---

  QStringList enumNames() const override;

  //---

  bool setValue(const QString &str) {
    Type type { Type::TOP_RIGHT };

    if (! decodeString(str, type))
      return false;

    type_ = type;

    return true;
  }

  //---

  bool isAuto() const { return (type_ == Type::AUTO); }

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

  friend bool operator==(const CQChartsKeyLocation &lhs, const CQChartsKeyLocation &rhs) {
    if (lhs.type_ != rhs.type_) return false;

    return true;
  }

  friend bool operator!=(const CQChartsKeyLocation &lhs, const CQChartsKeyLocation &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

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

CQUTIL_DCL_META_TYPE(CQChartsKeyLocation)

#endif
