#ifndef CQChartsAxisTickLabelPlacement_H
#define CQChartsAxisTickLabelPlacement_H

#include <CQChartsEnum.h>

/*!
 * \brief class for axis tick label placement
 * \ingroup Charts
 */
class CQChartsAxisTickLabelPlacement : public CQChartsEnum<CQChartsAxisTickLabelPlacement> {
 public:
  enum class Type {
    NONE,
    MIDDLE,
    BOTTOM_LEFT,
    TOP_RIGHT,
    BETWEEN
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsAxisTickLabelPlacement, metaTypeId)

 public:
  explicit CQChartsAxisTickLabelPlacement(Type type=Type::MIDDLE) :
   type_(type) {
  }

  explicit CQChartsAxisTickLabelPlacement(const QString &str) {
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

CQUTIL_DCL_META_TYPE(CQChartsAxisTickLabelPlacement)

#endif
