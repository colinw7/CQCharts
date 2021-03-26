#ifndef CQChartsTitleLocation_H
#define CQChartsTitleLocation_H

#include <CQChartsEnum.h>

/*!
 * \brief title location
 * \ingroup Charts
 */
class CQChartsTitleLocation : public CQChartsEnum<CQChartsTitleLocation> {
 public:
  enum Type {
    NONE,
    TOP,
    CENTER,
    BOTTOM,
    ABSOLUTE_POSITION,
    ABSOLUTE_RECTANGLE
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  explicit CQChartsTitleLocation(Type type=Type::TOP) :
   type_(type) {
  }

  explicit CQChartsTitleLocation(const QString &str) {
    (void) setValue(str);
  }

  //---

  const Type &type() const { return type_; }
  void setType(const Type &type) { type_ = type; }

  //---

  QString toString() const override;
  bool fromString(const QString &s) override;

  static bool decodeString(const QString &str, CQChartsTitleLocation::Type &type);

  //---

  QStringList enumNames() const override;

  //---

 private:
  bool setValue(const QString &s);

 private:
  Type type_ { Type::TOP };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsTitleLocation)

#endif
