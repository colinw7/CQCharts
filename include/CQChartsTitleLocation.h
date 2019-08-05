#ifndef CQChartsTitleLocation_H
#define CQChartsTitleLocation_H

#include <CQChartsEnum.h>

/*!
 * \brief title location
 * \ingroup Charts
 */
class CQChartsTitleLocation : public CQChartsEnum {
 public:
  enum Type {
    TOP,
    CENTER,
    BOTTOM,
    ABS_POS,
    ABS_RECT
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsTitleLocation(Type type=Type::TOP) :
   type_(type) {
  }

  explicit CQChartsTitleLocation(const QString &str) {
    (void) fromString(str);
  }

  //---

  const Type &type() const { return type_; }
  void setType(const Type &type) { type_ = type; }

  //---

  QString toString() const override;
  bool fromString(const QString &s) override;

  //---

  QStringList enumNames() const override;

  //---

  friend bool operator==(const CQChartsTitleLocation &lhs, const CQChartsTitleLocation &rhs) {
    return (lhs.type_ == rhs.type_);
  }

  friend bool operator!=(const CQChartsTitleLocation &lhs, const CQChartsTitleLocation &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  friend std::ostream &operator<<(std::ostream &os, const CQChartsTitleLocation &l) {
    l.print(os);
    return os;
  }

 private:
  Type type_ { Type::TOP };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsTitleLocation)

#endif
