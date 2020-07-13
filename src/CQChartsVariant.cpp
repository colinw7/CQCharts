#include <CQChartsVariant.h>

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
    str = CQChartsUtil::formatInteger((long) var.toInt());
  }
  else if (var.type() == QVariant::Point) {
    QPoint point = var.value<QPoint>();

    str = CQChartsUtil::pointToString(Point(point));
  }
  else if (var.type() == QVariant::PointF) {
    QPointF point = var.value<QPointF>();

    str = CQChartsUtil::pointToString(Point(point));
  }
  else if (var.type() == QVariant::Size) {
    QSize size = var.value<QSize>();

    str = CQChartsUtil::sizeToString(Size(QSizeF(size)));
  }
  else if (var.type() == QVariant::SizeF) {
    QSizeF size = var.value<QSizeF>();

    str = CQChartsUtil::sizeToString(Size(size));
  }
  else if (var.type() == QVariant::Rect) {
    QRect rect = var.value<QRect>();

    str = CQChartsUtil::bboxToString(BBox(QRectF(rect)));
  }
  else if (var.type() == QVariant::RectF) {
    QRectF rect = var.value<QRectF>();

    str = CQChartsUtil::bboxToString(BBox(rect));
  }
  else if (var.type() == QVariant::PolygonF) {
    QPolygonF poly = var.value<QPolygonF>();

    str = CQChartsUtil::polygonToString(Polygon(poly));
  }
  else if (var.type() == QVariant::Image) {
    QImage image = var.value<QImage>();

    str = image.text("filename");
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

//---

int cmp(const QVariant &var1, const QVariant &var2) {
  bool isNumber1 = (var1.type() == QVariant::Int || var1.type() == QVariant::Double);
  bool isNumber2 = (var2.type() == QVariant::Int || var2.type() == QVariant::Double);

  if (isNumber1 && isNumber2) {
    bool ok1; double r1 = toReal(var1, ok1);
    bool ok2; double r2 = toReal(var2, ok2);

    if (r1 < r2) return -1;
    if (r1 > r2) return  1;

    return 0;
  }

  //---

  if (var1.type() != var2.type()) {
    if (var1.type() < var2.type()) return -1;
    if (var1.type() > var2.type()) return  1;

    assert(false);
  }

  if      (var1.type() == QVariant::Int) {
    int i1 = var1.value<int>();
    int i2 = var2.value<int>();

    if (i1 < i2) return -1;
    if (i1 > i2) return  1;

    return 0;
  }
  else if (var1.type() == QVariant::Double) {
    double r1 = var1.value<double>();
    double r2 = var2.value<double>();

    if (r1 < r2) return -1;
    if (r1 > r2) return  1;

    return 0;
  }
  else if (var1.type() == QVariant::UserType) {
    if (var1.userType() != var2.userType()) {
      if (var1.userType() < var2.userType()) return -1;
      if (var1.userType() > var2.userType()) return  1;

      assert(false);
    }

    if (var1.userType() == CQChartsColor::metaTypeId) {
      CQChartsColor color1 = var1.value<CQChartsColor>();
      CQChartsColor color2 = var2.value<CQChartsColor>();

      return CQChartsColor::cmp(color1, color2);
    }
  }

  //---

  QString str1 = var1.toString();
  QString str2 = var2.toString();

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

  converted = true;

  QString str = toString(var, ok);

  if (! ok)
    return CMathUtil::getNaN();

  return CQChartsUtil::toReal(str, ok);
}

double toReal(const QVariant &var, bool &ok) {
  bool converted;

  return toConvertedReal(var, ok, converted);
}

//---

long toInt(const QVariant &var, bool &ok) {
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

bool toBool(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Bool)
    return var.toBool();

  if (var.type() == QVariant::Int)
    return var.toInt();

  if (var.type() == QVariant::String) {
    QString str = var.toString();

    if (str == "yes" || str == "true"  || str == "1") { ok = true ; return true; }
    if (str == "no"  || str == "false" || str == "0") { ok = false; return true; }
  }

  ok = false;

  return false;
}

//---

CQChartsColor toColor(const QVariant &var, bool &ok) {
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

QVariant fromColor(const CQChartsColor &c) {
  return QVariant::fromValue<CQChartsColor>(c);
}

//---

CQChartsFont toFont(const QVariant &var, bool &ok) {
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

QVariant fromFont(const CQChartsFont &f) {
  return QVariant::fromValue<CQChartsFont>(f);
}

//---

CQChartsSymbol toSymbol(const QVariant &var, bool &ok) {
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

QVariant fromSymbol(const CQChartsSymbol &symbol) {
  return QVariant::fromValue<CQChartsSymbol>(symbol);
}

//---

CQChartsImage toImage(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Image) {
    QImage image = var.value<QImage>();

    return CQChartsImage(image);
  }

  if (var.type() == QVariant::UserType) {
    if (var.userType() == CQChartsImage::metaTypeId)
      return var.value<CQChartsImage>();
  }

  CQChartsImage image(var.toString());

  if (image.isValid())
    return image;

  ok = false;

  return CQChartsImage();
}

QVariant fromImage(const CQChartsImage &image) {
  return QVariant::fromValue<CQChartsImage>(image);
}

//---

CQChartsLength toLength(const QVariant &var, bool &ok) {
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

QVariant fromLength(const CQChartsLength &length) {
  return QVariant::fromValue<CQChartsLength>(length);
}

//---

std::vector<double> toReals(const QVariant &var, bool &ok) {
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

//---

Point toPoint(const QVariant &var, bool &ok) {
  Point point;

  ok = false;

  if (! var.isValid())
    return point;

  // TODO: handle QVariant::PolygonF ?
  if      (var.type() == QVariant::PointF) {
    ok = true;

    QPointF p = var.value<QPointF>();

    point = Point(p);
  }
  else if (var.type() == QVariant::Point) {
    ok = true;

    QPoint p = var.value<QPoint>();

    point = Point(p);
  }
  else if (var.type() == QVariant::UserType) {
    QString str;

    if (! CQUtil::userVariantToString(var, str))
      return point;

    ok = point.fromString(str);
  }
  else {
    QString str = var.toString();

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

    QRectF r = var.value<QRectF>();

    bbox = BBox(r);
  }
  else if (var.type() == QVariant::Rect) {
    ok = true;

    QRect r = var.value<QRect>();

    bbox = BBox(QRectF(r));
  }
  else if (var.type() == QVariant::UserType) {
    QString str;

    if (! CQUtil::userVariantToString(var, str))
      return bbox;

    ok = bbox.fromString(str);
  }
  else {
    QString str = var.toString();

    ok = bbox.fromString(str);
  }

  return bbox;
}

QVariant fromBBox(const BBox &bbox) {
  return QVariant::fromValue<BBox>(bbox);
}

//---

Polygon toPolygon(const QVariant &var, bool &ok) {
  Polygon poly;

  ok = false;

  // TODO: handle QVariant::RectF ?
  QPolygonF qpoly;

  if      (var.type() == QVariant::PolygonF)
    qpoly = var.value<QPolygonF>();
  else if (var.type() == QVariant::Polygon)
    qpoly = var.value<QPolygon>();
  else {
    ok = false;

    return poly;
  }


  return poly;
}

//---

CQChartsPath toPath(const QVariant &var, bool &ok) {
  ok = true; // TODO: validate

  CQChartsPath path;

  // TODO: other var formats
  if (var.userType() == CQChartsPath::metaTypeId)
    path = var.value<CQChartsPath>();
  else
    ok = false;

  return path;
}

QVariant fromPath(const CQChartsPath &path) {
  return QVariant::fromValue<CQChartsPath>(path);
}

//---

CQChartsPolygonList toPolygonList(const QVariant &var, bool &ok) {
  ok = true; // TODO: validate

  return var.value<CQChartsPolygonList>();
}

QVariant fromPolygonList(const CQChartsPolygonList &polyList) {
  return QVariant::fromValue<CQChartsPolygonList>(polyList);
}

//---

CQChartsAlpha toAlpha(const QVariant &var, bool &ok) {
  ok = true; // TODO: validate

  CQChartsAlpha alpha;

  // TODO: other var formats
  if (var.userType() == CQChartsAlpha::metaTypeId)
    alpha = var.value<CQChartsAlpha>();
  else
    ok = false;

  return alpha;
}

QVariant fromAlpha(const CQChartsAlpha &a) {
  return QVariant::fromValue<CQChartsAlpha>(a);
}

//---

CQChartsAngle toAngle(const QVariant &var, bool &ok) {
  ok = true; // TODO: validate

  CQChartsAngle angle;

  // TODO: other var formats
  if (var.userType() == CQChartsAngle::metaTypeId)
    angle = var.value<CQChartsAngle>();
  else
    ok = false;

  return angle;
}

QVariant fromAngle(const CQChartsAngle &a) {
  return QVariant::fromValue<CQChartsAngle>(a);
}

}
