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
  else if (var.type() == QVariant::PointF) {
    QPointF point = var.value<QPointF>();

    str = CQChartsUtil::pointToString(CQChartsGeom::Point(point));
  }
  else if (var.type() == QVariant::Size) {
    QSize size = var.value<QSize>();

    str = CQChartsUtil::sizeToString(size);
  }
  else if (var.type() == QVariant::RectF) {
    QRectF rect = var.value<QRectF>();

    CQChartsGeom::BBox bbox(rect);

    str = bbox.toString();
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

  if (var.type() == QVariant::String)
    return var.toBool();

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
