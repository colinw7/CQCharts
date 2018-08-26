#ifndef CQChartsVariant_H
#define CQChartsVariant_H

#include <CQChartsUtil.h>
#include <CQUtil.h>

namespace CQChartsVariant {

inline bool toString(const QVariant &var, QString &str) {
  if (! var.isValid())
    return false;

  //---

  if      (var.type() == QVariant::String) {
    str = var.toString();
  }
  else if (var.type() == QVariant::Double) {
    str = CQChartsUtil::toString(var.toDouble());
  }
  else if (var.type() == QVariant::Int) {
    str = CQChartsUtil::toString((long) var.toInt());
  }
  else if (var.type() == QVariant::PointF) {
    QPointF point = var.value<QPointF>();

    str = CQChartsUtil::pointToString(point);
  }
  else if (var.type() == QVariant::RectF) {
    QRectF rect = var.value<QRectF>();

    str = CQChartsUtil::rectToString(rect);
  }
  else if (var.type() == QVariant::PolygonF) {
    QPolygonF poly = var.value<QPolygonF>();

    str = CQChartsUtil::polygonToString(poly);
  }
  else if (var.type() == QVariant::UserType) {
#if 0
    if      (var.userType() == CQChartsPath::metaType()) {
      CQChartsPath path = var.value<CQChartsPath>();

      str = path.toString();
    }
    else if (var.userType() == CQChartsStyle::metaType()) {
      CQChartsStyle style = var.value<CQChartsStyle>();

      str = style.toString();
    }
    else {
      assert(false);
    }
#else
    if (! CQUtil::userVariantToString(var, str))
      return false;
#endif
  }
  else if (var.type() == QVariant::List) {
    QList<QVariant> vars = var.toList();

    QStringList strs;

    for (int i = 0; i < vars.length(); ++i) {
      QString str1;

      if (toString(vars[i], str1))
        strs.push_back(str1);
    }

    str = "{" + strs.join(" ") + "}";

    return true;
  }
  else if (var.canConvert(QVariant::String)) {
    str = var.toString();
  }
  else {
    assert(false);

    return false;
  }

  return true;
}

inline QString toString(const QVariant &var, bool &ok) {
  QString str;

  ok = toString(var, str);

  return str;
}

//---

inline bool isReal(const QVariant &var) {
  return (var.type() == QVariant::Double);
}

inline double toReal(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Double)
    return var.value<double>();

  QString str = toString(var, ok);

  if (! ok)
    return CMathUtil::getNaN();

  return CQChartsUtil::toReal(str, ok);
}

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

inline long toInt(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Int)
    return var.value<int>();

  if (var.type() == QVariant::Double) {
    double r = var.value<double>();

    if (CMathUtil::isInteger(r))
      return int(r);
  }

  QString str = toString(var, ok);

  if (! ok)
    return 0;

  return CQChartsUtil::toInt(str, ok);
}

//---

inline bool isNumeric(const QVariant &var) {
  return isReal(var) || isInt(var);
}

//---

inline bool toBool(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Bool)
    return var.toBool();

  return false;
}

//---

inline bool isColor(const QVariant &var) {
  return (var.type() == QVariant::Color);
}

inline CQChartsColor toColor(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Color)
    return var.value<QColor>();

  QColor c(var.toString());

  if (c.isValid())
    return CQChartsColor(c);

  ok = false;

  return CQChartsColor();
}

//---

inline std::vector<double> toReals(const QVariant &var, bool &ok) {
  std::vector<double> reals;

  if      (var.type() == QVariant::List) {
    QList<QVariant> vars = var.toList();

    for (int i = 0; i < vars.length(); ++i) {
      bool ok1;

      double r = toReal(vars[i], ok1);

      if (! ok1) {
        ok = false;
        continue;
      }

      reals.push_back(r);
    }
  }
  else if (var.type() == QVariant::Double) {
    double r = var.toDouble();

    reals.push_back(r);
  }
  else {
    std::vector<double> reals;

    QString str;

    if (! toString(var, str))
      return reals;

    reals = CQChartsUtil::stringToReals(str, ok);
  }

  return reals;
}

}

#endif
