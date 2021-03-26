#ifndef CQChartsKeyPressBehavior_H
#define CQChartsKeyPressBehavior_H

#include <CQChartsEnum.h>

/*!
 * \brief key press behavior
 * \ingroup Charts
 */
class CQChartsKeyPressBehavior : public CQChartsEnum<CQChartsKeyPressBehavior> {
 public:
  enum class Type {
    NONE,
    SHOW,
    SELECT
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  explicit CQChartsKeyPressBehavior(Type type=Type::SHOW) :
   type_(type) {
  }

  explicit CQChartsKeyPressBehavior(const QString &str) {
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

CQUTIL_DCL_META_TYPE(CQChartsKeyPressBehavior)

#endif
