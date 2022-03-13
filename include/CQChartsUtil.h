#ifndef CQChartsUtil_H
#define CQChartsUtil_H

#include <CQChartsColumn.h>
#include <CQChartsGeom.h>
#include <CQChartsColor.h>
#include <CQChartsTypes.h>
#include <CQChartsFillPattern.h>
#include <CQChartsNameValues.h>
#include <CQChartsLineCap.h>
#include <CQChartsLineJoin.h>
#include <CMathUtil.h>

#include <QVariant>
#include <QPen>
#include <QStringList>

#include <cassert>

class CQChartsLineDash;
class CQChartsStyle;
class CQChartsPath;
class CQStrParse;

//---

#ifndef NDEBUG
# define CQCHARTS_ASSERT(condition, message) \
  do { \
    if (! (condition)) { \
      std::cerr << "Assertion `" #condition "` failed in " << __FILE__ << \
                   " line " << __LINE__ << ": " << message << "\n"; \
      std::terminate(); \
    } \
  } while (false)

# define CQCHARTS_QASSERT(condition, message) \
  do { \
    if (! (condition)) { \
      std::cerr << "Assertion `" #condition "` failed in " << __FILE__ << \
                   " line " << __LINE__ << ": " << message.toStdString() << "\n"; \
      std::terminate(); \
    } \
  } while (false)
#else
# define CQCHARTS_ASSERT(condition, message) do { } while (false)
# define CQCHARTS_QASSERT(condition, message) do { } while (false)
#endif

//---

// conversion

namespace CQChartsUtil {

enum class TriState {
  FALSE,
  TRUE,
  UNKNOWN
};

//---

template<typename FLAGS, typename FLAG>
bool isFlagSet(const FLAGS &flags, const FLAG &flag) {
  return (static_cast<int>(flags) & static_cast<int>(flag));
}

//---

using BBox  = CQChartsGeom::BBox;
using Range = CQChartsGeom::Range;

//---

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

inline BBox rangeBBox(const Range &range) {
  if (range.isSet())
    return BBox(range.xmin(), range.ymin(), range.xmax(), range.ymax());
  else
    return BBox();
}

inline Range bboxRange(const BBox &bbox) {
  if (bbox.isSet())
    return Range(bbox.getXMin(), bbox.getYMin(), bbox.getXMax(), bbox.getYMax());
  else
    return Range();
}

}

//------

// formatting

namespace CQChartsUtil {

QString formatVar    (const QVariant &var, const QString &fmt);
QString formatReal   (double r, const QString &fmt="%g" );
QString formatInteger(long   i, const QString &fmt="%ld");

bool scanReal   (const QString &fmt, const QString &str, double &r);
bool scanInteger(const QString &fmt, const QString &str, long   &i);

}

//------

// geometry

namespace CQChartsUtil {

using Point = CQChartsGeom::Point;
using BBox  = CQChartsGeom::BBox;

// intersect lines
bool intersectLines(const Point &l1s, const Point &l1e, const Point &l2s, const Point &l2e,
                    Point &pi, double &mu1, double &mu2);
bool intersectLines(double x11, double y11, double x21, double y21,
                    double x12, double y12, double x22, double y22,
                    double &xi, double &yi, double &mu1, double &mu2);

// distance between two points
double PointPointDistance(const Point &point1, const Point &point2);

// distance between point and line
bool PointLineDistance(const Point &point, const Point &lineStart, const Point &lineEnd,
                       double *dist);

inline Qt::Orientation lineOrient(const Point &p1, const Point &p2) {
  double dx = std::abs(p1.x - p2.x);
  double dy = std::abs(p1.y - p2.y);

  return (dx > dy ? Qt::Horizontal : Qt::Vertical);
}

}

//---

// colors

namespace CQChartsUtil {

QColor grayColor(const QColor &c);

QColor bwColor(const QColor &c);

QColor invColor(const QColor &c);

QColor blendColors(const QColor &c1, const QColor &c2, const CQChartsAlpha &a);
QColor blendColors(const QColor &c1, const QColor &c2, double f);
QColor blendColors(const std::vector<QColor> &colors);

QColor rgbToColor(double r, double g, double b);
QColor rgbaToColor(double r, double g, double b, double a);

QColor stringToColor(const QString &str);

}

//------

namespace CQChartsUtil {

bool fileToLines(const QString &filename, QStringList &lines, int maxLines=-1);

}

//------

namespace CQChartsUtil {

//! compare reals with tolerance
struct RealCmp {
  bool operator()(const double &lhs, const double &rhs) const {
    if (CMathUtil::realEq(lhs, rhs))
      return false;

    return lhs < rhs;
  }
};

//! compare colors
struct ColorCmp {
  bool operator()(const CQChartsColor &lhs, const CQChartsColor &rhs) const {
    return (lhs < rhs);
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

inline QString boolToString(bool b) {
  return (b ? "true" : "false");
}

//---

std::vector<double> stringToReals(const QString &str, bool &ok);

//---

int countLeadingBraces(const QString &str);
int countLeadingBraces(CQStrParse &parse);

}

//---

namespace CQChartsUtil {

QString realToString(double r, int ndp=-1);

}

//---

namespace CQChartsUtil {

using BBox     = CQChartsGeom::BBox;
using Size     = CQChartsGeom::Size;
using Point    = CQChartsGeom::Point;
using Point3D  = CQChartsGeom::Point3D;
using Polygon  = CQChartsGeom::Polygon;
using Polygons = CQChartsGeom::Polygons;

QString polygonListToString(const Polygons &polyList);
bool stringToPolygons(const QString &str, Polygons &polyList);
bool parsePolygons(CQStrParse &parse, Polygons &polyList);

QString polygonToString(const Polygon &poly);
bool stringToPolygon(const QString &str, Polygon &poly);
bool parsePolygon(CQStrParse &parse, Polygon &poly);

QString bboxToString(const BBox &bbox);
bool stringToBBox(const QString &str, BBox &bbox);

QString sizeToString(const Size &s);

QString pointToString(const Point &p);
bool stringToPoint(const QString &str, Point &point);

QString point3DToString(const Point3D &p);
bool stringToPoint3D(const QString &str, Point3D &point);

bool parseBBox   (CQStrParse &parse, BBox    &rect , bool terminated=true);
bool parsePoint  (CQStrParse &parse, Point   &point, bool terminated=true);
bool parsePoint3D(CQStrParse &parse, Point3D &point, bool terminated=true);

//------

QString pathToString(const CQChartsPath &path);
bool stringToPath(const QString &str, CQChartsPath &path);

//------

QString styleToString(const CQChartsStyle &style);
bool stringToStyle(const QString &str, CQChartsStyle &style);

//------

QString timeToString(const QString &fmt, double r);
bool    stringToTime(const QString &fmt, const QString &str, double &t);

}

//------

namespace CQChartsUtil {

using BBox = CQChartsGeom::BBox;

//! format string data
struct FormatData {
  QString seps;
  bool    continued { false };

  FormatData() { }

  FormatData(const QString &s) :
   seps(s) {
  }

  bool isValid() const {
    return (seps.length() > 0);
  }

  bool isSep(const QChar &c) const {
    return seps.contains(c);
  }
};

bool formatStringInRect(const QString &str, const QFont &font, const BBox &rect,
                        QStringList &strs, const FormatData &formatData=FormatData());

void findStringCustomSplits(const QString &str, std::vector<int> &splits,
                            const CQChartsUtil::FormatData &formatData);

void findStringSpaceSplits(const QString &str, std::vector<int> &splits);

void findStringPunctSplits(const QString &str, std::vector<int> &splits);

void findStringCaseSplits(const QString &str, std::vector<int> &splits);

}

//------

#include <CQChartsLineDash.h>

namespace CQChartsUtil {

using Alpha       = CQChartsAlpha;
using LineDash    = CQChartsLineDash;
using LineCap     = CQChartsLineCap;
using LineJoin    = CQChartsLineJoin;
using FillPattern = CQChartsFillPattern;

void penSetLineDash(QPen &pen, const LineDash &dash);

void setPen(QPen &pen, bool stroked, const QColor &strokeColor=QColor(),
            const Alpha &strokeAlpha=Alpha(), double strokeWidth=0.0,
            const LineDash &strokeDash=LineDash(), const LineCap &strokeCap=LineCap(),
            const LineJoin &strokeJoin=LineJoin());

void setBrush(QBrush &brush, bool filled, const QColor &fillColor=QColor(),
              const Alpha &fillAlpha=Alpha(),
              const FillPattern &pattern=FillPattern(FillPattern::Type::SOLID));

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
  explicit CQChartsScopeGuard(Callable &&f) :
   f_(std::forward<Callable>(f)) {
  }

  explicit CQChartsScopeGuard(CQChartsScopeGuard &&other) :
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

//! color index data
struct ColorInd {
  ColorInd() = default;

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
  QColor c;
};

}

//------

namespace CQChartsUtil {

inline double areaToRadius(double a) {
  // area = PI*r*r; r = sqrt(area/PI)
  return sqrt(a/M_PI);
}

inline double clampDegrees(double a) {
  return CMathUtil::clamp(a, -360.0, 360.0);
}

inline double circleArea(double r) {
  return M_PI*r*r;
}

inline double ellipseArea(double xr, double yr) {
  return M_PI*xr*yr;
}

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

namespace CQChartsUtil {

using PointList = std::vector<Point>;

Point nearestRectPoint(const BBox &bbox, const Point &p, double &angle, bool useCorners=true);

Point nearestPointListPoint(const PointList &points, const Point &pos, int &i);

}

//------

namespace CQChartsUtil {

inline int realStringNdp(const QString &s) {
  auto i = s.indexOf('.');
  if (i == -1) return 0;

  auto ndp = s.size() - i - 1;

  while (s[ndp + i] == '0')
    --ndp;

  return ndp;
}

inline int valuesNdp(const std::vector<double> &values) {
  int ndp = 0;

  for (const auto &r : values) {
    auto rstr = QString("%1").arg(r, 0, 'f', 6);

    ndp = std::max(ndp, realStringNdp(rstr));
  }

  return ndp;
}

}

//------

namespace CQChartsUtil {

bool encodeUtf(const QString &s, QString &res);

}

//------

namespace CQChartsUtil {

QImage disabledImage(const QImage &image, const QColor &bg, double f=0.5);

}

//------

namespace CQChartsUtil {

inline CQChartsSelMod modifiersToSelMod(Qt::KeyboardModifiers modifiers) {
  if      ((modifiers & Qt::ControlModifier) && (modifiers & Qt::ShiftModifier))
    return CQChartsSelMod::TOGGLE;
  else if (modifiers & Qt::ControlModifier)
    return CQChartsSelMod::ADD;
  else if (modifiers & Qt::ShiftModifier)
    return CQChartsSelMod::REMOVE;
  else
    return CQChartsSelMod::REPLACE;
}

inline CQChartsSelMod modifiersToClickMod(Qt::KeyboardModifiers modifiers) {
  if      ((modifiers & Qt::ControlModifier) && (modifiers & Qt::ShiftModifier))
    return CQChartsSelMod::ADD;
  else if (modifiers & Qt::ControlModifier)
    return CQChartsSelMod::REPLACE;
  else if (modifiers & Qt::ShiftModifier)
    return CQChartsSelMod::REMOVE;
  else
    return CQChartsSelMod::TOGGLE;
}

}

//------

#endif
