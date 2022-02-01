#ifndef CQChartsVariant_H
#define CQChartsVariant_H

#include <CQChartsUtil.h>
#include <CQChartsFont.h>
#include <CQChartsSymbol.h>
#include <CQChartsImage.h>
#include <CQChartsLength.h>
#include <CQChartsUnits.h>
#include <CQModelUtil.h>
#include <CQUtil.h>

namespace CQChartsVariant {

int cmp(const QVariant &var1, const QVariant &var2);

//---

// string
inline bool isString(const QVariant &var) {
  return (var.type() == QVariant::String);
}

bool toString(const QVariant &var, QString &str);
QString toString(const QVariant &var, bool &ok);

inline QVariant fromString(const QString &s) {
  return CQModelUtil::stringVariant(s);
}

//---

// real
inline bool isReal(const QVariant &var) {
  return (var.type() == QVariant::Double);
}

bool toReal(const QVariant &var, double &r);
double toReal(const QVariant &var, bool &ok);

double toConvertedReal(const QVariant &var, bool &ok, bool &converted);

inline QVariant fromReal(double r) {
  return CQModelUtil::realVariant(r);
}

inline QVariant fromNaN() {
  return CQModelUtil::nanVariant();
}

//---

// int
inline bool isInt(const QVariant &var) {
  return (var.type() == QVariant::Bool || var.type() == QVariant::Char ||
          var.type() == QVariant::Int  || var.type() == QVariant::LongLong ||
          var.type() == QVariant::UInt);
}

long toInt(const QVariant &var, bool &ok);

inline QVariant fromInt(long i) {
  return CQModelUtil::intVariant(i);
}

//---

// bool
inline bool isBool(const QVariant &var) {
  return (var.type() == QVariant::Bool);
}

bool toBool(const QVariant &var, bool &ok);

inline QVariant fromBool(bool b) {
  return CQModelUtil::boolVariant(b);
}

//---

// numeric
inline bool isNumeric(const QVariant &var) {
  return isReal(var) || isInt(var) || isBool(var);
}

//---

// list
inline bool isList(const QVariant &var) {
  return (var.type() == QVariant::List);
}

// user type
inline bool isUserType(const QVariant &var) {
  return (var.type() == QVariant::UserType);
}

}

//---

// color
namespace CQChartsVariant {

inline bool isColor(const QVariant &var) {
  if (! var.isValid())
    return false;

  if (var.type() == QVariant::Color)
    return true;

  if (CQChartsColor::isVariantType(var))
    return true;

  return false;
}

CQChartsColor toColor(const QVariant &var, bool &ok);
QVariant fromColor(const CQChartsColor &c);

inline QVariant fromColor(const QColor &c) {
  return CQModelUtil::colorVariant(c);
}

}

//---

// font
namespace CQChartsVariant {

inline bool isFont(const QVariant &var) {
  if (! var.isValid())
    return false;

  if (var.type() == QVariant::Font)
    return true;

  if (CQChartsFont::isVariantType(var))
    return true;

  return false;
}

CQChartsFont toFont(const QVariant &var, bool &ok);
QVariant fromFont(const CQChartsFont &f);

}

//---

// symbol
namespace CQChartsVariant {

inline bool isSymbol(const QVariant &var) {
  if (! var.isValid())
    return false;

  if (var.type() == QVariant::UserType) {
    if (var.userType() == CQChartsSymbol::metaTypeId)
      return true;
  }

  return false;
}

CQChartsSymbol toSymbol(const QVariant &var, bool &ok);
QVariant fromSymbol(const CQChartsSymbol &symbol);

}

//---

// image
namespace CQChartsVariant {

inline bool isImage(const QVariant &var) {
  if (! var.isValid())
    return false;

  if (var.type() == QVariant::Image)
    return true;

  if (var.type() == QVariant::UserType) {
    if (var.userType() == CQChartsImage::metaTypeId)
      return true;
  }

  return false;
}

CQChartsImage toImage(const QVariant &var, bool &ok);
QVariant fromImage(const CQChartsImage &image);

}

//---

// units
namespace CQChartsVariant {

inline bool isUnits(const QVariant &var) {
  if (! var.isValid())
    return false;

  if (var.type() == QVariant::UserType) {
    if (var.userType() == CQChartsUnits::metaTypeId)
      return true;
  }

  return false;
}

CQChartsUnits toUnits(const QVariant &var, bool &ok);
QVariant fromUnits(const CQChartsUnits &units);

}

//---

// length
namespace CQChartsVariant {

inline bool isLength(const QVariant &var) {
  if (! var.isValid())
    return false;

  if (var.type() == QVariant::UserType) {
    if (var.userType() == CQChartsLength::metaTypeId)
      return true;
  }

  return false;
}

CQChartsLength toLength(const QVariant &var, bool &ok);
QVariant fromLength(const CQChartsLength &length);

}

//---

// reals
namespace CQChartsVariant {

std::vector<double> toReals(const QVariant &var, bool &ok);

}

//---

// point
namespace CQChartsVariant {

using Point = CQChartsGeom::Point;

Point toPoint(const QVariant &var, bool &ok);
QVariant fromPoint(const Point &point);

}

//---

// rect/bbox
namespace CQChartsVariant {

inline bool isRectF(const QVariant &var) {
  return (var.type() == QVariant::RectF);
}

using BBox = CQChartsGeom::BBox;

BBox toBBox(const QVariant &var, bool &ok);
QVariant fromBBox(const BBox &bbox);

}

//---

// polygon
namespace CQChartsVariant {

using Polygon = CQChartsGeom::Polygon;

inline bool isPolygonF(const QVariant &var) {
  return (var.type() == QVariant::PolygonF);
}

inline bool isPolygon(const QVariant &var) {
  if (! var.isValid())
    return false;

  if      (var.type() == QVariant::Polygon)
    return true;
  else if (var.type() == QVariant::PolygonF)
    return true;

  return false;
}

QPolygonF toQPolygon(const QVariant &var, bool &ok);
Polygon toPolygon(const QVariant &var, bool &ok);

QVariant fromQPolygon(const QPolygonF &poly);

}

//---

// path
class CQChartsPath;

namespace CQChartsVariant {

bool toPath(const QVariant &var, CQChartsPath &path);
QVariant fromPath(const CQChartsPath &path);

}

//---

#include <CQChartsPolygonList.h>

// polygon list
namespace CQChartsVariant {

CQChartsPolygonList toPolygonList(const QVariant &var, bool &ok);
QVariant fromPolygonList(const CQChartsPolygonList &polyList);

}

//---

#include <CQChartsAlpha.h>

// alpha
namespace CQChartsVariant {

CQChartsAlpha toAlpha(const QVariant &var, bool &ok);
QVariant fromAlpha(const CQChartsAlpha &a);

}

//---

#include <CQChartsAngle.h>

// angle
namespace CQChartsVariant {

CQChartsAngle toAngle(const QVariant &var, bool &ok);
QVariant fromAngle(const CQChartsAngle &a);

}

#endif
