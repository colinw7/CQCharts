#ifndef CQChartsAxisSide_H
#define CQChartsAxisSide_H

#include <CQChartsEnum.h>

/*!
 * \brief Axis side
 * \ingroup Charts
 */
class CQChartsAxisSide : public CQChartsEnum<CQChartsAxisSide> {
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
  explicit CQChartsAxisSide(Type type=Type::BOTTOM_LEFT) :
   type_(type) {
  }

  explicit CQChartsAxisSide(const QString &str) {
    (void) setValue(str);
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

 private:
  bool setValue(const QString &s);

 private:
  Type type_ { Type::NONE };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsAxisSide)

#endif
