#ifndef CQChartsKeyPressBehavior_H
#define CQChartsKeyPressBehavior_H

#include <CQChartsEnum.h>

/*!
 * \brief key press behavior
 * \ingroup Charts
 */
class CQChartsKeyPressBehavior : public CQChartsEnum {
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
  CQChartsKeyPressBehavior(Type type=Type::SHOW) :
   type_(type) {
  }

  explicit CQChartsKeyPressBehavior(const QString &str) {
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

  friend bool operator<(const CQChartsKeyPressBehavior &lhs, const CQChartsKeyPressBehavior &rhs) {
    return (lhs.type_ < rhs.type_);
  }

  friend bool operator==(const CQChartsKeyPressBehavior &lhs, const CQChartsKeyPressBehavior &rhs) {
    return (lhs.type_ == rhs.type_);
  }

  friend bool operator!=(const CQChartsKeyPressBehavior &lhs, const CQChartsKeyPressBehavior &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  friend std::ostream &operator<<(std::ostream &os, const CQChartsKeyPressBehavior &l) {
    l.print(os);
    return os;
  }

 private:
  Type type_ { Type::NONE };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsKeyPressBehavior)

#endif
