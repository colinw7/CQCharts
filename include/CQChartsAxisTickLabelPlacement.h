#ifndef CQChartsAxisTickLabelPlacement_H
#define CQChartsAxisTickLabelPlacement_H

#include <CQChartsEnum.h>

/*!
 * \brief class for axis tick label placement
 * \ingroup Charts
 */
class CQChartsAxisTickLabelPlacement : public CQChartsEnum {
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

 public:
  CQChartsAxisTickLabelPlacement(Type type=Type::MIDDLE) :
   type_(type) {
  }

  explicit CQChartsAxisTickLabelPlacement(const QString &str) {
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

  friend bool operator<(const CQChartsAxisTickLabelPlacement &lhs,
                        const CQChartsAxisTickLabelPlacement &rhs) {
    return (lhs.type_ < rhs.type_);
  }

  friend bool operator==(const CQChartsAxisTickLabelPlacement &lhs,
                         const CQChartsAxisTickLabelPlacement &rhs) {
    return (lhs.type_ == rhs.type_);
  }

  friend bool operator!=(const CQChartsAxisTickLabelPlacement &lhs,
                         const CQChartsAxisTickLabelPlacement &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  friend std::ostream &operator<<(std::ostream &os, const CQChartsAxisTickLabelPlacement &l) {
    l.print(os);
    return os;
  }

 private:
  Type type_ { Type::NONE };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsAxisTickLabelPlacement)

#endif
