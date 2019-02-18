#ifndef CQChartsUtil_H
#define CQChartsUtil_H

#include <CQChartsColumn.h>
#include <CQChartsGeom.h>
#include <CQChartsColor.h>
#include <CQChartsTypes.h>
#include <CQChartsFillPattern.h>
#include <CQChartsNameValues.h>
#include <CMathUtil.h>

#include <QVariant>
#include <QPen>
#include <QStringList>
#include <QRectF>

class CQChartsLineDash;
class CQChartsStyle;
class CQChartsPath;
class CQStrParse;
class QPainter;

namespace CQChartsUtil {

double toReal(const QString &str, bool &ok);
bool   toReal(const QString &str, double &r);

//------

long toInt(const QString &str, bool &ok);
bool toInt(const QString &str, long &i);

long toInt(const char *str, bool &ok, const char **rstr);

//------

QString toString(double r, const QString &fmt="%g" );
QString toString(long   i, const QString &fmt="%ld");

//------

QString toString(const std::vector<CQChartsColumn> &columns);

bool fromString(const QString &str, std::vector<CQChartsColumn> &columns);

//------

inline QPointF toQPoint(const CQChartsGeom::Point &point) {
  return QPointF(point.x, point.y);
}

inline QPoint toQPointI(const CQChartsGeom::Point &point) {
  return QPoint(point.x, point.y);
}

inline CQChartsGeom::Point fromQPoint(const QPointF &point) {
  return CQChartsGeom::Point(point.x(), point.y());
}

inline CQChartsGeom::Point fromQPointF(const QPoint &point) {
  return CQChartsGeom::Point(point.x(), point.y());
}

inline QRectF toQRect(const CQChartsGeom::BBox &rect) {
  return QRectF(toQPoint(rect.getLL()), toQPoint(rect.getUR())).normalized();
}

inline QRectF toQRect(const CQChartsGeom::Range &range) {
  return QRectF(range.xmin(), range.ymin(), range.xsize(), range.ysize());
}

inline QRect toQRectI(const CQChartsGeom::BBox &rect) {
  return QRect(toQPointI(rect.getLL()), toQPointI(rect.getUR())).normalized();
}

inline CQChartsGeom::BBox fromQRect(const QRectF &rect) {
  return CQChartsGeom::BBox(fromQPoint(rect.bottomLeft()), fromQPoint(rect.topRight()));
}

inline CQChartsGeom::BBox rangeBBox(const CQChartsGeom::Range &range) {
  return CQChartsGeom::BBox(range.xmin(), range.ymin(), range.xmax(), range.ymax());
}

inline CQChartsGeom::Range bboxRange(const CQChartsGeom::BBox &bbox) {
  return CQChartsGeom::Range(bbox.getXMin(), bbox.getYMin(), bbox.getXMax(), bbox.getYMax());
}

//------

bool intersectLines(const QPointF &l1s, const QPointF &l1e,
                    const QPointF &l2s, const QPointF &l2e, QPointF &pi);
bool intersectLines(double x11, double y11, double x21, double y21,
                    double x12, double y12, double x22, double y22,
                    double &xi, double &yi);

//---

QColor bwColor(const QColor &c);

QColor invColor(const QColor &c);

QColor blendColors(const QColor &c1, const QColor &c2, double f);
QColor blendColors(const std::vector<QColor> &colors);

//------

void penSetLineDash(QPen &pen, const CQChartsLineDash &dash);

//------

inline CQChartsGeom::Point AngleToPoint(const CQChartsGeom::Point &c,
                                        double xr, double yr, double a) {
  double ra = CMathUtil::Deg2Rad(a);

  double x = c.x + xr*cos(ra);
  double y = c.y + yr*sin(ra);

  return CQChartsGeom::Point(x, y);
}

inline CQChartsGeom::Point AngleToPoint(const CQChartsGeom::Point &c, double r, double a) {
  return AngleToPoint(c, r, r, a);
}

//------

// distance between two points
double PointPointDistance(const CQChartsGeom::Point &point1, const CQChartsGeom::Point &point2);

// distance between point and line
bool PointLineDistance(const CQChartsGeom::Point &point, const CQChartsGeom::Point &lineStart,
                       const CQChartsGeom::Point &lineEnd, double *dist);

//------

bool fileToLines(const QString &filename, QStringList &lines, int maxLines=-1);

}

//------

namespace CQChartsUtil {

// compare reals with tolerance
struct RealCmp {
  bool operator()(const double &lhs, const double &rhs) const {
    if (CMathUtil::realEq(lhs, rhs))
      return false;

    return lhs < rhs;
  }
};

// compare colors
struct ColorCmp {
  bool operator()(const CQChartsColor &lhs, const CQChartsColor &rhs) const {
    return (CQChartsColor::cmp(lhs, rhs) < 0);
  }

  bool operator()(const QColor &lhs, const QColor &rhs) const {
    return (CQChartsColor::cmp(lhs, rhs) < 0);
  }
};

//------

inline bool stringToBool(const QString &str, bool *ok) {
  QString lstr = str.toLower();

  if (lstr == "0" || lstr == "false" || lstr == "no") {
    *ok = true;
    return false;
  }

  if (lstr == "1" || lstr == "true" || lstr == "yes") {
    *ok = true;
    return true;
  }

  *ok = false;

  return false;
}

//---

std::vector<double> stringToReals(const QString &str, bool &ok);

//---

int countLeadingBraces(const QString &str);

QString polygonListToString(const std::vector<QPolygonF> &polyList);
bool stringToPolygons(const QString &str, std::vector<QPolygonF> &polyList);

QString polygonToString(const QPolygonF &poly);
bool stringToPolygon(const QString &str, QPolygonF &poly );

bool parsePolygon(CQStrParse &parse, QPolygonF &poly);

QString rectToString(const QRectF &rect);
bool stringToRect(const QString &str, QRectF &rect);

QString pointToString(const QPointF &p);
bool stringToPoint(const QString &str, QPointF &point);

bool parseRect (CQStrParse &parse, QRectF &rect);
bool parsePoint(CQStrParse &parse, QPointF &point);

//------

QString pathToString(const CQChartsPath &path);
bool stringToPath(const QString &str, CQChartsPath &path);

//------

QString styleToString(const CQChartsStyle &style);
bool stringToStyle(const QString &str, CQChartsStyle &style);

//------

QString timeToString(const QString &fmt, double r);
bool    stringToTime(const QString &fmt, const QString &str, double &t);

//------

bool formatStringInRect(const QString &str, const QFont &font,
                        const QRectF &rect, QStringList &strs);

}

//------

#include <CQChartsLineDash.h>

namespace CQChartsUtil {

void drawContrastText(QPainter *painter, double x, double y, const QString &text, const QPen &pen);

void setPen(QPen &pen, bool stroked, const QColor &strokeColor, double strokeAlpha=1.0,
            double strokeWidth=0.0, const CQChartsLineDash &strokeDash=CQChartsLineDash());

void setBrush(QBrush &brush, bool filled,
              const QColor &fillColor=QColor(), double fillAlpha=1.0,
              const CQChartsFillPattern &pattern=CQChartsFillPattern::Type::SOLID);

double limitLineWidth(double w);

}

//------

namespace CQChartsUtil {

inline QString unitsString(const CQChartsUnits &units) {
  if      (units == CQChartsUnits::PIXEL  ) return "px";
  else if (units == CQChartsUnits::PERCENT) return "%" ;
  else if (units == CQChartsUnits::PLOT   ) return "P" ;
  else if (units == CQChartsUnits::VIEW   ) return "V" ;
  else                                      return ""  ;
}

bool decodeUnits(const QString &str, CQChartsUnits &units,
                 const CQChartsUnits &defUnits=CQChartsUnits::PLOT);

QStringList unitNames(bool includeNone=false);

QStringList unitTipNames(bool includeNone=false);

}

//------

#include <functional>

class CQChartsScopeGuard {
 public:
  template<class Callable>
  CQChartsScopeGuard(Callable &&f) :
   f_(std::forward<Callable>(f)) {
  }

  CQChartsScopeGuard(CQChartsScopeGuard &&other) :
   f_(std::move(other.f_)) {
    other.f_ = nullptr;
  }

 ~CQChartsScopeGuard() {
    if (f_)
      f_(); // must not throw
  }

  void dismiss() noexcept {
    f_ = nullptr;
  }

  CQChartsScopeGuard(const CQChartsScopeGuard&) = delete;
  void operator = (const CQChartsScopeGuard&) = delete;

 private:
  std::function<void()> f_;
};

//------

namespace CQChartsUtil {

template<typename T, typename NOTIFIER>
void testAndSet(T &t, const T &v, NOTIFIER &&notifier) {
  if (v != t) {
    t = v;

    notifier();
  }
}

}

//------

#endif
