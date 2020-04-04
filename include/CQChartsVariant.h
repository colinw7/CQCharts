#ifndef CQChartsVariant_H
#define CQChartsVariant_H

#include <CQChartsUtil.h>
#include <CQChartsFont.h>
#include <CQChartsSymbol.h>
#include <CQChartsImage.h>
#include <CQChartsLength.h>
#include <CQUtil.h>

namespace CQChartsVariant {

bool toString(const QVariant &var, QString &str);

inline QString toString(const QVariant &var, bool &ok) {
  QString str;

  ok = toString(var, str);

  return str;
}

//---

int cmp(const QVariant &var1, const QVariant &var2);

//---

inline bool isReal(const QVariant &var) {
  return (var.type() == QVariant::Double);
}

double toReal(const QVariant &var, bool &ok);

inline bool toReal(const QVariant &var, double &r) {
  bool ok;

  r = toReal(var, ok);

  return ok;
}

//---

inline bool isInt(const QVariant &var) {
  return (var.type() == QVariant::Bool || var.type() == QVariant::Char ||
          var.type() == QVariant::Int  || var.type() == QVariant::LongLong ||
          var.type() == QVariant::UInt);
}

long toInt(const QVariant &var, bool &ok);

//---

inline bool isBool(const QVariant &var) {
  return (var.type() == QVariant::Bool);
}

bool toBool(const QVariant &var, bool &ok);

//---

inline bool isNumeric(const QVariant &var) {
  return isReal(var) || isInt(var) || isBool(var);
}

//---

inline bool isColor(const QVariant &var) {
  if (! var.isValid())
    return false;

  if (var.type() == QVariant::Color)
    return true;

  if (var.type() == QVariant::UserType && var.userType() == CQChartsColor::metaTypeId)
    return true;

  return false;
}

CQChartsColor toColor(const QVariant &var, bool &ok);

//---

CQChartsFont toFont(const QVariant &var, bool &ok);

//---

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

//---

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

//---

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

//---

std::vector<double> toReals(const QVariant &var, bool &ok);

//---

CQChartsGeom::Point toPoint(const QVariant &var, bool &ok);
QVariant            fromPoint(const CQChartsGeom::Point &point);

//---

CQChartsGeom::BBox toBBox  (const QVariant &var, bool &ok);
QVariant           fromBBox(const CQChartsGeom::BBox &bbox);

//---

CQChartsGeom::Polygon toPolygon(const QVariant &var, bool &ok);

}

//---

#include <CQChartsPath.h>

namespace CQChartsVariant {

CQChartsPath toPath(const QVariant &var, bool &ok);

}

//---

#include <CQChartsPolygonList.h>

namespace CQChartsVariant {

CQChartsPolygonList toPolygonList(const QVariant &var, bool &ok);

}

#endif
