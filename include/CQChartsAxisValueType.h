#ifndef CQChartsAxisValueType_H
#define CQChartsAxisValueType_H

#include <CQChartsEnum.h>

/*!
 * \brief Axis value type
 * \ingroup Charts
 */
class CQChartsAxisValueType : public CQChartsEnum {
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

 public:
  CQChartsAxisValueType(Type type=Type::REAL) :
   type_(type) {
  }

  explicit CQChartsAxisValueType(const QString &str) {
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

  friend bool operator<(const CQChartsAxisValueType &lhs, const CQChartsAxisValueType &rhs) {
    return (lhs.type_ < rhs.type_);
  }

  friend bool operator==(const CQChartsAxisValueType &lhs, const CQChartsAxisValueType &rhs) {
    return (lhs.type_ == rhs.type_);
  }

  friend bool operator!=(const CQChartsAxisValueType &lhs, const CQChartsAxisValueType &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  friend std::ostream &operator<<(std::ostream &os, const CQChartsAxisValueType &l) {
    l.print(os);
    return os;
  }

 private:
  Type type_ { Type::REAL };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsAxisValueType)

#endif
