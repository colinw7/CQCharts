#include <CQChartsVariant.h>
#include <CQModelUtil.h>

namespace CQChartsVariant {

using Point   = CQChartsGeom::Point;
using Size    = CQChartsGeom::Size;
using BBox    = CQChartsGeom::BBox;
using Polygon = CQChartsGeom::Polygon;

bool toString(const QVariant &var, QString &str) {
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
    str = CQChartsUtil::formatInteger(static_cast<long>(var.toInt()));
  }
  else if (var.type() == QVariant::LongLong) {
    str = CQChartsUtil::formatInteger(var.toLongLong());
  }
  else if (var.type() == QVariant::Bool) {
    str = CQChartsUtil::boolToString(var.toBool());
  }
  else if (var.type() == QVariant::Point) {
    auto point = var.value<QPoint>();

    str = CQChartsUtil::pointToString(Point(point));
  }
  else if (var.type() == QVariant::PointF) {
    auto point = var.value<QPointF>();

    str = CQChartsUtil::pointToString(Point(point));
  }
  else if (var.type() == QVariant::Size) {
    auto size = var.value<QSize>();

    str = CQChartsUtil::sizeToString(Size(QSizeF(size)));
  }
  else if (var.type() == QVariant::SizeF) {
    auto size = var.value<QSizeF>();

    str = CQChartsUtil::sizeToString(Size(size));
  }
  else if (var.type() == QVariant::Rect) {
    auto rect = var.value<QRect>();

    str = CQChartsUtil::bboxToString(BBox(QRectF(rect)));
  }
  else if (var.type() == QVariant::RectF) {
    auto rect = var.value<QRectF>();

    str = CQChartsUtil::bboxToString(BBox(rect));
  }
  else if (var.type() == QVariant::PolygonF) {
    auto poly = var.value<QPolygonF>();

    str = CQChartsUtil::polygonToString(Polygon(poly));
  }
  else if (var.type() == QVariant::Image) {
    auto image = var.value<QImage>();

    str = image.text("filename");
  }
  else if (var.type() == QVariant::UserType) {
#if 0
    if      (CQChartsPath::isVariantType(var)) {
      auto path = CQChartsPath::fromVariant(var);

      str = path.toString();
    }
    else if (CQChartsStyle::isVariantType(var)) {
      auto style = CQChartsStyle::fromVariant(var);

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
    auto vars = var.toList();

    QStringList strs;

    for (int i = 0; i < vars.length(); ++i) {
      QString str1;

      if (toString(vars[i], str1))
        strs.push_back(std::move(str1));
    }

    str = "{" + strs.join(" ") + "}";

    return true;
  }
  else if (var.canConvert(QVariant::String)) {
    str = var.toString();
  }
  else if (var.type() == QVariant::SizePolicy) {
    auto policyToString = [](QSizePolicy::Policy policy) {
      switch (policy) {
        case QSizePolicy::Fixed           : return QString("Fixed");
        case QSizePolicy::Minimum         : return QString("Minimum");
        case QSizePolicy::Maximum         : return QString("Maximum");
        case QSizePolicy::Preferred       : return QString("Preferred");
        case QSizePolicy::Expanding       : return QString("Expanding");
        case QSizePolicy::MinimumExpanding: return QString("MinimumExpanding");
        case QSizePolicy::Ignored         : return QString("Ignored");
        default                           : return QString::number(policy);
      }
    };

    auto sp = var.value<QSizePolicy>();

    str = QString("%1 %2 %3 %4").arg(policyToString(sp.horizontalPolicy())).
                                 arg(policyToString(sp.verticalPolicy  ())).
                                 arg(sp.horizontalStretch()).
                                 arg(sp.verticalStretch  ());

    return true;
  }
  else {
    assert(false);

    return false;
  }

  return true;
}

QString toString(const QVariant &var, bool &ok) {
  QString str;

  ok = toString(var, str);

  return str;
}

//---

int cmp(const QVariant &var1, const QVariant &var2) {
  auto cmpInt = [&]() {
    bool ok1, ok2;

    long i1 = toInt(var1, ok1); assert(ok1);
    long i2 = toInt(var2, ok2); assert(ok2);

    if (i1 < i2) return -1;
    if (i1 > i2) return  1;

    return 0;
  };

  auto cmpReal = [&]() {
    double r1 = var1.value<double>();
    double r2 = var2.value<double>();

    if (r1 < r2) return -1;
    if (r1 > r2) return  1;

    return 0;
  };

  bool isNumber1 = isNumeric(var1);
  bool isNumber2 = isNumeric(var2);

  if (isNumber1 && isNumber2) {
    if (var1.type() == var2.type()) {
      if      (isInt(var1))
        return cmpInt();
      else if (isReal(var1))
        return cmpReal();
    }

    //---

    bool ok1; double r1 = toReal(var1, ok1);
    bool ok2; double r2 = toReal(var2, ok2);

    if      (! ok1 && ! ok2) return  0;
    else if (! ok1 &&   ok2) return -1;
    else if (  ok1 && ! ok2) return  1;

    if (r1 < r2) return -1;
    if (r1 > r2) return  1;

    return 0;
  }

  //---

  if (var1.type() != var2.type()) {
    bool isNull1 = (var1.type() == QVariant::String && var1.toString().length() == 0);
    bool isNull2 = (var2.type() == QVariant::String && var2.toString().length() == 0);

    if      (  isNull1 &&   isNull2) return  0;
    else if (  isNull1 && ! isNull2) return -1;
    else if (! isNull1 &&   isNull2) return  1;

    if (var1.type() < var2.type()) return -1;
    if (var1.type() > var2.type()) return  1;

    assert(false);
  }

  //---

  if      (isInt(var1)) {
    return cmpInt();
  }
  else if (isReal(var1)) {
    return cmpReal();
  }
  else if (var1.type() == QVariant::UserType) {
    if (var1.userType() != var2.userType()) {
      if (var1.userType() < var2.userType()) return -1;
      if (var1.userType() > var2.userType()) return  1;

      assert(false);
    }

    if (CQChartsColor::isVariantType(var1)) {
      auto color1 = CQChartsColor::fromVariant(var1);
      auto color2 = CQChartsColor::fromVariant(var2);

      return cmp(color1, color2);
    }
  }

  //---

  auto str1 = var1.toString();
  auto str2 = var2.toString();

  if (str1 < str2) return -1;
  if (str1 > str2) return  1;

  return 0;
}

//---

double toConvertedReal(const QVariant &var, bool &ok, bool &converted) {
  ok        = true;
  converted = false;

  if (var.type() == QVariant::Double)
    return var.value<double>();

  if (var.type() == QVariant::Int)
    return var.value<int>();

  if (var.type() == QVariant::LongLong)
    return double(var.value<qlonglong>());

  converted = true;

  auto str = toString(var, ok);

  if (! ok)
    return CMathUtil::getNaN();

  return CQChartsUtil::toReal(str, ok);
}

bool toReal(const QVariant &var, double &r) {
  bool ok, converted;

  r = toConvertedReal(var, ok, converted);

  return ok;
}

double toReal(const QVariant &var, bool &ok) {
  double r;

  ok = toReal(var, r);

  return r;
}

//---

long toInt(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Invalid) {
    ok = false;
    return 0;
  }

  if (var.type() == QVariant::Int)
    return var.value<int>();

  if (var.type() == QVariant::LongLong)
    return var.value<qlonglong>();

  if (var.type() == QVariant::Double) {
    double r = var.value<double>();

    if (CQModelUtil::isInteger(r))
      return static_cast<long>(r);
  }

  if (var.type() == QVariant::Bool)
    return var.toBool();

  auto str = toString(var, ok);

  if (! ok)
    return 0;

  return CQChartsUtil::toInt(str, ok);
}

//---

bool toBool(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Bool)
    return var.toBool();

  if (var.type() == QVariant::Int)
    return var.toInt();

  if (var.type() == QVariant::LongLong)
    return var.toLongLong();

  if (var.type() == QVariant::String) {
    auto str = var.toString();

    bool b = CQChartsUtil::stringToBool(str, &ok);
    if (ok) return b;
  }

  ok = false;

  return var.toBool();
}

//---

CQChartsColor toColor(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Color) {
    auto color = var.value<QColor>();
    ok = color.isValid();
    return CQChartsColor(color);
  }

  if (CQChartsColor::isVariantType(var)) {
    auto color = CQChartsColor::fromVariant(var);
    ok = color.isValid();
    return color;
  }

  auto color = CQChartsColor(var.toString());
  ok = color.isValid();

  return color;
}

QVariant fromColor(const CQChartsColor &c) {
  return CQChartsColor::toVariant(c);
}

//---

CQChartsFont toFont(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Font) {
    auto font = var.value<QFont>();
    //ok = font.isValid();
    return CQChartsFont(font);
  }

  if (CQChartsFont::isVariantType(var)) {
    auto font = CQChartsFont::fromVariant(var);
    ok = font.isValid();
    return font;
  }

  auto font = CQChartsFont(var.toString());
  ok = font.isValid();

  return font;
}

QVariant fromFont(const CQChartsFont &f) {
  return CQChartsFont::toVariant(f);
}

//---

CQChartsSymbol toSymbol(const QVariant &var, bool &ok) {
  ok = true;

  if (CQChartsSymbol::isVariantType(var))
    return CQChartsSymbol::fromVariant(var);

  CQChartsSymbol symbol(var.toString());

  if (symbol.isValid())
    return symbol;

  ok = false;

  return CQChartsSymbol();
}

QVariant fromSymbol(const CQChartsSymbol &symbol) {
  return CQChartsSymbol::toVariant(symbol);
}

//---

CQChartsImage toImage(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Image) {
    auto image = var.value<QImage>();

    return CQChartsImage(image);
  }

  if (CQChartsImage::isVariantType(var))
    return CQChartsImage::fromVariant(var);

  CQChartsImage image(var.toString());

  if (image.isValid())
    return image;

  ok = false;

  return CQChartsImage();
}

QVariant fromImage(const CQChartsImage &image) {
  return CQChartsImage::toVariant(image);
}

//---

CQChartsUnits toUnits(const QVariant &var, bool &ok) {
  ok = true;

  if (CQChartsUnits::isVariantType(var))
    return CQChartsUnits::fromVariant(var);

  CQChartsUnits units(var.toString());

  if (units.isSet())
    return units;

  ok = false;

  return CQChartsUnits();
}

QVariant fromUnits(const CQChartsUnits &units) {
  return CQChartsUnits::toVariant(units);
}

//---

CQChartsLength toLength(const QVariant &var, bool &ok) {
  ok = true;

  if (CQChartsLength::isVariantType(var))
    return CQChartsLength::fromVariant(var);

  CQChartsLength length(var.toString());

  if (length.isValid())
    return length;

  ok = false;

  return CQChartsLength();
}

QVariant fromLength(const CQChartsLength &length) {
  return CQChartsLength::toVariant(length);
}

//---

std::vector<double> toReals(const QVariant &var, bool &ok) {
  std::vector<double> reals;

  if      (var.type() == QVariant::List) {
    auto vars = var.toList();

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

//---

Point toPoint(const QVariant &var, bool &ok) {
  Point point;

  ok = false;

  if (! var.isValid())
    return point;

  // TODO: handle QVariant::PolygonF ?
  if      (var.type() == QVariant::PointF) {
    ok = true;

    auto p = var.value<QPointF>();

    point = Point(p);
  }
  else if (var.type() == QVariant::Point) {
    ok = true;

    auto p = var.value<QPoint>();

    point = Point(p);
  }
  else if (var.type() == QVariant::UserType) {
    QString str;

    if (! CQUtil::userVariantToString(var, str))
      return point;

    ok = point.fromString(str);
  }
  else {
    auto str = var.toString();

    ok = point.fromString(str);
  }

  return point;
}

QVariant fromPoint(const Point &point) {
  return QVariant::fromValue<Point>(point);
}

//---

BBox toBBox(const QVariant &var, bool &ok) {
  BBox bbox;

  ok = false;

  if (! var.isValid())
    return bbox;

  // TODO: handle QVariant::PolygonF ?
  if      (var.type() == QVariant::RectF) {
    ok = true;

    auto r = var.value<QRectF>();

    bbox = BBox(r);
  }
  else if (var.type() == QVariant::Rect) {
    ok = true;

    auto r = var.value<QRect>();

    bbox = BBox(QRectF(r));
  }
  else if (var.type() == QVariant::UserType) {
    QString str;

    if (! CQUtil::userVariantToString(var, str))
      return bbox;

    ok = bbox.fromString(str);
  }
  else {
    auto str = var.toString();

    ok = bbox.fromString(str);
  }

  return bbox;
}

QVariant fromBBox(const BBox &bbox) {
  return QVariant::fromValue<BBox>(bbox);
}

//---

QPolygonF toQPolygon(const QVariant &var, bool &ok) {
  ok = false;

  // TODO: handle QVariant::RectF ?
  QPolygonF qpoly;

  if      (var.type() == QVariant::PolygonF)
    qpoly = var.value<QPolygonF>();
  else if (var.type() == QVariant::Polygon)
    qpoly = var.value<QPolygon>();
  else {
    ok = false;

    return qpoly;
  }

  return qpoly;
}

Polygon toPolygon(const QVariant &var, bool &ok) {
  auto qpoly = toQPolygon(var, ok);

  auto poly = Polygon(qpoly);

  return poly;
}

QVariant fromQPolygon(const QPolygonF &poly) {
  return QVariant::fromValue<QPolygonF>(poly);
}

//---

bool toPath(const QVariant &var, CQChartsPath &path) {
  bool ok = true;

  // TODO: other var formats
  if (CQChartsPath::isVariantType(var))
    path = CQChartsPath::fromVariant(var);
  else
    ok = false;

  // TODO: validate path

  return ok;
}

QVariant fromPath(const CQChartsPath &path) {
  return CQChartsPath::toVariant(path);
}

//---

CQChartsPolygonList toPolygonList(const QVariant &var, bool &ok) {
  ok = true;

  CQChartsPolygonList polys;

  if (CQChartsPolygonList::isVariantType(var))
    polys = CQChartsPolygonList::fromVariant(var);
  else
    ok = false;

  // TODO: validate polygon list

  return polys;
}

QVariant fromPolygonList(const CQChartsPolygonList &polyList) {
  return CQChartsPolygonList::toVariant(polyList);
}

//---

CQChartsAlpha toAlpha(const QVariant &var, bool &ok) {
  ok = true;

  CQChartsAlpha alpha;

  // TODO: other var formats
  if (CQChartsAlpha::isVariantType(var))
    alpha = CQChartsAlpha::fromVariant(var);
  else
    ok = false;

  // TODO: validate alpha

  return alpha;
}

QVariant fromAlpha(const CQChartsAlpha &a) {
  return CQChartsAlpha::toVariant(a);
}

//---

CQChartsAngle toAngle(const QVariant &var, bool &ok) {
  ok = true; // TODO: validate

  CQChartsAngle angle;

  // TODO: other var formats
  if (CQChartsAngle::isVariantType(var))
    angle = CQChartsAngle::fromVariant(var);
  else
    ok = false;

  // TODO: validate angle

  return angle;
}

QVariant fromAngle(const CQChartsAngle &a) {
  return CQChartsAngle::toVariant(a);
}

}
