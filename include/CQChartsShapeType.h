#ifndef CQChartsShapeType_H
#define CQChartsShapeType_H

#include <CQChartsTmpl.h>
#include <CQUtilMeta.h>

#include <QString>
#include <QStringList>
#include <cassert>

/*!
 * \brief Shape Type
 * \ingroup Charts
 */
class CQChartsShapeType :
  public CQChartsComparatorBase<CQChartsShapeType>,
  public CQChartsToStringBase<CQChartsShapeType> {
 public:
  // extra properties
  //  angle, num sides, double border, triple border, corner markers
  enum class Type {
    NONE,
    BOX,     // RECT, RECTANGLE, SQUARE
    POLYGON, // TODO: needed
    CIRCLE, // OVAL, ELLIPSE
    DOT,
    TRIANGLE,
    DIAMOND,
    TRAPEZIUM,
    PARALLELOGRAM,
    HOUSE,
    PENTAGON,
    HEXAGON,
    SEPTAGON,
    OCTAGON,
    DOUBLE_CIRCLE,
    STAR,
    UNDERLINE,
    CYLINDER,
    NOTE,
    TAB,
    FOLDER,
    BOX3D,
    COMPONENT,
    CDS,
    ARROW,
    LPROMOTER,
    RPROMOTER
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsShapeType, metaTypeId)

 public:
  static QString typeToName(Type type);
  static Type nameToType(const QString &str);

  static QStringList typeNames();

  //---

  static bool isValidType(Type type) { return (type >= Type::NONE && type <= Type::RPROMOTER); }

 public:
  CQChartsShapeType() = default;

  explicit CQChartsShapeType(Type type);

  explicit CQChartsShapeType(const QString &s);

  //---

  bool isValid() const { return type_ != Type::NONE; }

  const Type &type() const { return type_; }
  void setType(const Type &t) { type_ = t; assert(isValidType(type_)); }

  //---

  bool isRound() const {
    return (type_ == Type::CIRCLE || type_ == Type::DOUBLE_CIRCLE);
  }

  //---

  QString toString() const;

  bool fromString(const QString &s);

  //---

  //! compare for (==, !=, <, >, <=, >=)
  friend int cmp(const CQChartsShapeType &lhs, const CQChartsShapeType &rhs) {
    if (lhs.type_ > rhs.type_) return  1;
    if (lhs.type_ < rhs.type_) return -1;
    return 0;
  }

 private:
  Type type_ { Type::NONE };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsShapeType)

#endif
