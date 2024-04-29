#ifndef CQChartsAxisValueType_H
#define CQChartsAxisValueType_H

#include <CQChartsEnum.h>

/*!
 * \brief Axis value type
 * \ingroup Charts
 */
class CQChartsAxisValueType : public CQChartsEnum<CQChartsAxisValueType> {
 public:
  enum class Type {
    NONE,
    REAL,
    INTEGER,
    STRING,
    DATE,
    LOG
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsAxisValueType, metaTypeId)

 public:
  static CQChartsAxisValueType none   () { return CQChartsAxisValueType(Type::NONE); }
  static CQChartsAxisValueType real   () { return CQChartsAxisValueType(Type::REAL); }
  static CQChartsAxisValueType integer() { return CQChartsAxisValueType(Type::INTEGER); }
  static CQChartsAxisValueType string () { return CQChartsAxisValueType(Type::STRING); }
  static CQChartsAxisValueType log    () { return CQChartsAxisValueType(Type::LOG); }
  static CQChartsAxisValueType date   () { return CQChartsAxisValueType(Type::DATE); }

  //---

  explicit CQChartsAxisValueType(Type type=Type::REAL) :
   type_(type) {
  }

  explicit CQChartsAxisValueType(const QString &str) {
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
  bool setValue(const QString &str);

 private:
  Type type_ { Type::REAL };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsAxisValueType)

#endif
