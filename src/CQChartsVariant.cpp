#include <CQChartsVariant.h>

namespace CQChartsVariant {

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

    str = CQChartsUtil::pointToString(CQChartsGeom::Point(QPointF(point)));
  }
  else if (var.type() == QVariant::PointF) {
    QPointF point = var.value<QPointF>();

    str = CQChartsUtil::pointToString(CQChartsGeom::Point(point));
  }
  else if (var.type() == QVariant::Size) {
    QSize size = var.value<QSize>();

    str = CQChartsUtil::sizeToString(CQChartsGeom::Size(QSizeF(size)));
  }
  else if (var.type() == QVariant::SizeF) {
    QSizeF size = var.value<QSizeF>();

    str = CQChartsUtil::sizeToString(CQChartsGeom::Size(size));
  }
  else if (var.type() == QVariant::Rect) {
    QRect rect = var.value<QRect>();

    str = CQChartsUtil::bboxToString(CQChartsGeom::BBox(QRectF(rect)));
  }
  else if (var.type() == QVariant::RectF) {
    QRectF rect = var.value<QRectF>();

    str = CQChartsUtil::bboxToString(CQChartsGeom::BBox(rect));
  }
  else if (var.type() == QVariant::PolygonF) {
    QPolygonF poly = var.value<QPolygonF>();

    str = CQChartsUtil::polygonToString(CQChartsGeom::Polygon(poly));
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

double toReal(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Double)
    return var.value<double>();

  QString str = toString(var, ok);

  if (! ok)
    return CMathUtil::getNaN();

  return CQChartsUtil::toReal(str, ok);
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

CQChartsGeom::Point toPoint(const QVariant &var, bool &ok) {
  CQChartsGeom::Point point;

  ok = false;

  if (! var.isValid())
    return point;

  // TODO: handle QVariant::PolygonF ?
  if      (var.type() == QVariant::PointF) {
    ok = true;

    QPointF p = var.value<QPointF>();

    point = CQChartsGeom::Point(p);
  }
  else if (var.type() == QVariant::Point) {
    ok = true;

    QPoint p = var.value<QPoint>();

    point = CQChartsGeom::Point(QPointF(p));
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

QVariant fromPoint(const CQChartsGeom::Point &point) {
  return QVariant::fromValue<CQChartsGeom::Point>(point);
}

//---

CQChartsGeom::BBox toBBox(const QVariant &var, bool &ok) {
  CQChartsGeom::BBox bbox;

  ok = false;

  if (! var.isValid())
    return bbox;

  // TODO: handle QVariant::PolygonF ?
  if      (var.type() == QVariant::RectF) {
    ok = true;

    QRectF r = var.value<QRectF>();

    bbox = CQChartsGeom::BBox(r);
  }
  else if (var.type() == QVariant::Rect) {
    ok = true;

    QRect r = var.value<QRect>();

    bbox = CQChartsGeom::BBox(QRectF(r));
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

QVariant fromBBox(const CQChartsGeom::BBox &bbox) {
  return QVariant::fromValue<CQChartsGeom::BBox>(bbox);
}

//---

CQChartsGeom::Polygon toPolygon(const QVariant &var, bool &ok) {
  CQChartsGeom::Polygon poly;

  ok = false;

  // TODO: handle QVariant::RectF ?
  if (var.type() != QVariant::PolygonF) {
    ok = false;

    return poly;
  }

  QPolygonF qpoly = var.value<QPolygonF>();

  poly = CQChartsGeom::Polygon(qpoly);

  return poly;
}

//---

CQChartsPath toPath(const QVariant &var, bool &ok) {
  ok = true; // TODO: validate

  return var.value<CQChartsPath>();
}

//---

CQChartsPolygonList toPolygonList(const QVariant &var, bool &ok) {
  ok = true; // TODO: validate

  return var.value<CQChartsPolygonList>();
}

}
