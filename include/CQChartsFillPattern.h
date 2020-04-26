#ifndef CQChartsFillPattern_H
#define CQChartsFillPattern_H

#include <CQChartsEnum.h>

/*!
 * \brief fill pattern
 * \ingroup Charts
 */
class CQChartsFillPattern : public CQChartsEnum<CQChartsFillPattern> {
 public:
  enum class Type {
    NONE,
    SOLID,
    HATCH,
    DENSE,
    HORIZ,
    VERT,
    FDIAG,
    BDIAG
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsFillPattern(Type type=Type::SOLID) :
   type_(type) {
  }

  explicit CQChartsFillPattern(const QString &str) {
    (void) setValue(str);
  }

  //---

  Type type() const { return type_; }
  void setType(Type type) { type_  = type; }

  bool isValid() const { return type() != Type::NONE; }

  //---

  double scale() const { return scale_; }
  void setScale(double r) { scale_ = r; }

  //---

  QString toString() const override;
  bool fromString(const QString &s) override;

  //---

  QStringList enumNames() const override;

  //---

  static Type stringToType(const QString &str);
  static QString typeToString(const Type &type);

  static Type styleToType(const Qt::BrushStyle &style);
  static Qt::BrushStyle typeToStyle(const Type &type);

  //---

  Qt::BrushStyle style() const { return typeToStyle(type_); }

  //---

 private:
  bool setValue(const QString &str);

 private:
  Type   type_  { Type::NONE };
  double scale_ { 1.0 };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsFillPattern)

#endif
