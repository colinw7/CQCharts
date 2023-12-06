#ifndef CQChartsKeyLocation_H
#define CQChartsKeyLocation_H

#include <CQChartsEnum.h>

/*!
 * \brief key location
 * \ingroup Charts
 */
class CQChartsKeyLocation : public CQChartsEnum<CQChartsKeyLocation> {
 public:
  enum class Type {
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
    ABSOLUTE_POSITION,
    ABSOLUTE_RECTANGLE
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsKeyLocation, metaTypeId)

 public:
  explicit CQChartsKeyLocation(Type type=Type::TOP_RIGHT) :
   type_(type) {
  }

  explicit CQChartsKeyLocation(const QString &str) {
    (void) setValue(str);
  }

  //---

  bool isValid() const { return type_ != Type::NONE; }

  const Type &type() const { return type_; }
  void setType(const Type &type) { type_ = type; }

  //---

  QString toString() const override;
  bool fromString(const QString &s) override;

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

  static bool decodeString(const QString &str, Type &type);

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

 private:
  Type type_ { Type::NONE };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsKeyLocation)

#endif
