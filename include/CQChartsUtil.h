#ifndef CQChartsUtil_H
#define CQChartsUtil_H

#include <CQChartsColumn.h>
#include <CQChartsGeom.h>
#include <CQChartsColor.h>
#include <CQChartsTypes.h>
#include <CQChartsFillPattern.h>
#include <CQChartsNameValues.h>
#include <CQChartsDrawUtil.h>
#include <CMathUtil.h>

#include <QVariant>
#include <QPen>
#include <QStringList>

class CQChartsLineDash;
class CQChartsStyle;
class CQChartsPath;
class CQStrParse;

// conversion

namespace CQChartsUtil {

double toReal(const QString &str, bool &ok);
bool   toReal(const QString &str, double &r);

//---

long toInt(const QString &str, bool &ok);
bool toInt(const QString &str, long &i);

long toInt(const char *str, bool &ok, const char **rstr);

//---

QString toString(const std::vector<CQChartsColumn> &columns);

bool fromString(const QString &str, std::vector<CQChartsColumn> &columns);

//------

inline CQChartsGeom::BBox rangeBBox(const CQChartsGeom::Range &range) {
  if (range.isSet())
    return CQChartsGeom::BBox(range.xmin(), range.ymin(), range.xmax(), range.ymax());
  else
    return CQChartsGeom::BBox();
}

inline CQChartsGeom::Range bboxRange(const CQChartsGeom::BBox &bbox) {
  if (bbox.isSet())
    return CQChartsGeom::Range(bbox.getXMin(), bbox.getYMin(), bbox.getXMax(), bbox.getYMax());
  else
    return CQChartsGeom::Range();
}

}

//------

// formatting

namespace CQChartsUtil {

QString formatVar    (const QVariant &var, const QString &fmt);
QString formatReal   (double r, const QString &fmt="%g" );
QString formatInteger(long   i, const QString &fmt="%ld");

}

//------

// geometry

namespace CQChartsUtil {

// intersect lines
bool intersectLines(const CQChartsGeom::Point &l1s, const CQChartsGeom::Point &l1e,
                    const CQChartsGeom::Point &l2s, const CQChartsGeom::Point &l2e,
                    CQChartsGeom::Point &pi);
bool intersectLines(double x11, double y11, double x21, double y21,
                    double x12, double y12, double x22, double y22,
                    double &xi, double &yi);

// distance between two points
double PointPointDistance(const CQChartsGeom::Point &point1, const CQChartsGeom::Point &point2);

// distance between point and line
bool PointLineDistance(const CQChartsGeom::Point &point, const CQChartsGeom::Point &lineStart,
                       const CQChartsGeom::Point &lineEnd, double *dist);

}

//---

// colors

namespace CQChartsUtil {

QColor bwColor(const QColor &c);

QColor invColor(const QColor &c);

QColor blendColors(const QColor &c1, const QColor &c2, const CQChartsAlpha &a);
QColor blendColors(const QColor &c1, const QColor &c2, double f);
QColor blendColors(const std::vector<QColor> &colors);

QColor rgbToColor(double r, double g, double b);

}

//------

namespace CQChartsUtil {

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

}

//------

namespace CQChartsUtil {

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

QString polygonListToString(const std::vector<CQChartsGeom::Polygon> &polyList);
bool stringToPolygons(const QString &str, std::vector<CQChartsGeom::Polygon> &polyList);

QString polygonToString(const CQChartsGeom::Polygon &poly);
bool stringToPolygon(const QString &str, CQChartsGeom::Polygon &poly );

bool parsePolygon(CQStrParse &parse, CQChartsGeom::Polygon &poly);

QString bboxToString(const CQChartsGeom::BBox &bbox);
bool stringToBBox(const QString &str, CQChartsGeom::BBox &bbox);

QString sizeToString(const CQChartsGeom::Size &s);

QString pointToString(const CQChartsGeom::Point &p);
bool stringToPoint(const QString &str, CQChartsGeom::Point &point);

bool parseBBox (CQStrParse &parse, CQChartsGeom::BBox  &rect , bool terminated=true);
bool parsePoint(CQStrParse &parse, CQChartsGeom::Point &point, bool terminated=true);

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
                        const CQChartsGeom::BBox &rect, QStringList &strs);

}

//------

#include <CQChartsLineDash.h>

namespace CQChartsUtil {

void penSetLineDash(QPen &pen, const CQChartsLineDash &dash);

void setPenBrush(CQChartsPenBrush &penBrush,
                 bool stroked, const QColor &strokeColor, const CQChartsAlpha &strokeAlpha,
                 double strokeWidth, const CQChartsLineDash &strokeDash,
                 bool filled, const QColor &fillColor, const CQChartsAlpha &fillAlpha,
                 const CQChartsFillPattern &pattern);

void setPen(QPen &pen, bool stroked, const QColor &strokeColor=QColor(),
            const CQChartsAlpha &strokeAlpha=CQChartsAlpha(),
            double strokeWidth=0.0, const CQChartsLineDash &strokeDash=CQChartsLineDash());

void setBrush(QBrush &brush, bool filled, const QColor &fillColor=QColor(),
              const CQChartsAlpha &fillAlpha=CQChartsAlpha(),
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

/*!
 * \brief Class to run function at end of scope
 * \ingroup Charts
 */
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

namespace CQChartsUtil {

struct ColorInd {
  ColorInd() { }

  ColorInd(int i, int n) :
   isInt(true), i(i), n(n) {
  }

  explicit ColorInd(double r) :
   isInt(false), r(r) {
  }

  bool isValid() const {
    if (isInt) return (i >= 0 && i < n);
    // TODO (r >= 0.0 && r <= 1.0);
    return true;
  }

  double value() const {
    if (! isInt)
      return r;
    else
      return CMathUtil::map(i, 0, n - 1, 0.0, 1.0);
  }

  friend bool operator==(const ColorInd &lhs, const ColorInd &rhs) {
    return (lhs.isInt == rhs.isInt &&
            lhs.i     == rhs.i     &&
            lhs.n     == rhs.n     &&
            lhs.r     == rhs.r);
  }

  bool   isInt { true };
  int    i     { 0 };
  int    n     { 1 };
  double r     { 0.0 };
};

}

//------

namespace CQChartsUtil {

inline QImage *newImage(const QSize &size) {
//return new QImage(size, QImage::Format_ARGB32);
  return new QImage(size, QImage::Format_ARGB32_Premultiplied);
}

inline QImage initImage(const QSize &size) {
//return QImage(size, QImage::Format_ARGB32);
  return QImage(size, QImage::Format_ARGB32_Premultiplied);
}

}

//------

namespace CQChartsUtil {

inline QString encodeScriptColor(const QColor &c) {
  if (! c.isValid())
    return "none";

  if (c.alpha() >= 255)
    return QString("rgb(%1,%2,%3)").arg(c.red()).arg(c.green()).arg(c.blue());
  else
    return QString("rgba(%1,%2,%3,%4)").
             arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha()/255.0);
}

inline QString encodeSVGColor(const QColor &c) {
  if (! c.isValid())
    return "rgb(0,0,0)";

  return QString("rgb(%1,%2,%3)").arg(c.red()).arg(c.green()).arg(c.blue());
}

}

//------

namespace CQChartsUtil {

QFont scaleFontSize(const QFont &font, double s, double minSize=4.0, double maxSize=999.0);

}

//------

#endif
