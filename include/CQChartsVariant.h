#ifndef CQChartsVariant_H
#define CQChartsVariant_H

#include <CQChartsUtil.h>
#include <CQChartsFont.h>
#include <CQChartsSymbol.h>
#include <CQChartsLength.h>
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
    str = CQChartsUtil::formatReal(var.toDouble());
  }
  else if (var.type() == QVariant::Int) {
    str = CQChartsUtil::formatInteger((long) var.toInt());
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
    if      (var.userType() == CQChartsPath::metaTypeId) {
      CQChartsPath path = var.value<CQChartsPath>();

      str = path.toString();
    }
    else if (var.userType() == CQChartsStyle::metaTypeId) {
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

inline bool isBool(const QVariant &var) {
  return (var.type() == QVariant::Bool);
}

inline bool toBool(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Bool)
    return var.toBool();

  if (var.type() == QVariant::String)
    return var.toBool();

  ok = false;

  return false;
}

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

inline CQChartsColor toColor(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Color) {
    CQChartsColor color = var.value<QColor>();
    ok = color.isValid();
    return color;
  }

  if (var.type() == QVariant::UserType && var.userType() == CQChartsColor::metaTypeId) {
    CQChartsColor color = var.value<CQChartsColor>();
    ok = color.isValid();
    return color;
  }

  CQChartsColor color = QColor(var.toString());
  ok = color.isValid();
  return color;
}

inline CQChartsFont toFont(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Font) {
    CQChartsFont font(var.value<QFont>());
    ok = font.isValid();
    return font;
  }

  if (var.type() == QVariant::UserType && var.userType() == CQChartsFont::metaTypeId) {
    CQChartsFont font = var.value<CQChartsFont>();
    ok = font.isValid();
    return font;
  }

  CQChartsFont font(var.toString());
  ok = font.isValid();
  return font;
}

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

inline CQChartsSymbol toSymbol(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::UserType) {
    if (var.userType() == CQChartsSymbol::metaTypeId)
      return var.value<CQChartsSymbol>();
  }

  CQChartsSymbol symbol(var.toString());

  if (symbol.isValid())
    return symbol;

  ok = false;

  return CQChartsSymbol();
}

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

inline CQChartsLength toLength(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::UserType) {
    if (var.userType() == CQChartsLength::metaTypeId)
      return var.value<CQChartsLength>();
  }

  CQChartsLength length(var.toString());

  if (length.isValid())
    return length;

  ok = false;

  return CQChartsLength();
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
    QString str;

    if (! toString(var, str))
      return reals;

    reals = CQChartsUtil::stringToReals(str, ok);
  }

  return reals;
}

}

#endif
