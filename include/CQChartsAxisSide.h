#ifndef CQChartsAxisSide_H
#define CQChartsAxisSide_H

#include <CQChartsEnum.h>

/*!
 * \brief Axis side
 * \ingroup Charts
 */
class CQChartsAxisSide : public CQChartsEnum {
 public:
  enum class Type {
    NONE,
    BOTTOM_LEFT,
    TOP_RIGHT
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsAxisSide(Type type=Type::BOTTOM_LEFT) :
   type_(type) {
  }

  explicit CQChartsAxisSide(const QString &str) {
    (void) fromString(str);
  }

  //---

  bool isValid() const { return type_ != Type::NONE; }

  Type type() const { return type_; }
  void setType(Type type) { type_  = type; }

  //---

  QString toString() const override;
  bool fromString(const QString &s) override;

  //---

  QStringList enumNames() const override;

  //---

  friend bool operator<(const CQChartsAxisSide &lhs, const CQChartsAxisSide &rhs) {
    return (lhs.type_ < rhs.type_);
  }

  friend bool operator==(const CQChartsAxisSide &lhs, const CQChartsAxisSide &rhs) {
    return (lhs.type_ == rhs.type_);
  }

  friend bool operator!=(const CQChartsAxisSide &lhs, const CQChartsAxisSide &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  friend std::ostream &operator<<(std::ostream &os, const CQChartsAxisSide &l) {
    l.print(os);
    return os;
  }

 private:
  Type type_ { Type::NONE };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsAxisSide)

#endif
