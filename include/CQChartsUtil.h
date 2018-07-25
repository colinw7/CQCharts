#ifndef CQChartsUtil_H
#define CQChartsUtil_H

#include <CQChartsLineDash.h>
#include <CQChartsColumn.h>
#include <CQChartsGeom.h>
#include <CQChartsPath.h>
#include <CQChartsStyle.h>
#include <CQChartsColor.h>
#include <CQChartsExprModel.h>
#include <CQChartsEval.h>
#include <CQBaseModel.h>
#include <COSNaN.h>

#include <QAbstractItemModel>
#include <QVariant>
#include <QPen>
#include <QStringList>
#include <QRectF>

using CQChartsNameValues = std::map<QString,QString>;

class CQCharts;
class CQChartsColumnType;
class CQDataModel;

namespace CQChartsUtil {

bool isHierarchical(QAbstractItemModel *model);

int hierRowCount(QAbstractItemModel *model);

QString parentPath(QAbstractItemModel *model, const QModelIndex &parent);

//------

bool columnValueType(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                     CQBaseModel::Type &columnType, CQChartsNameValues &nameValues);

bool formatColumnTypeValue(CQCharts *charts, const QString &typeStr, double value, QString &str);

bool formatColumnValue(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                       double value, QString &str);

bool formatColumnTypeValue(CQChartsColumnType *typeData, const CQChartsNameValues &nameValues,
                           double value, QString &str);

QVariant columnDisplayData(CQCharts *charts, QAbstractItemModel *model,
                           const CQChartsColumn &column, const QVariant &var, bool &converted);

QVariant columnUserData(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                        const QVariant &var, bool &converted);

bool columnTypeStr(CQCharts *charts, QAbstractItemModel *model,
                   const CQChartsColumn &column, QString &typeStr);

bool setColumnTypeStrs(CQCharts *charts, QAbstractItemModel *model, const QString &columnTypes);

bool setColumnTypeStr(CQCharts *charts, QAbstractItemModel *model,
                      const CQChartsColumn &column, const QString &typeStr);

#if 0
void remapColumnTime(QAbstractItemModel *model, const CQChartsColumn &column,
                     CQChartsColumnType *typeData, const CQChartsNameValues &nameValues);
#endif

//------

inline bool isNaN(double r) { return COSNaN::is_nan(r) && ! COSNaN::is_inf(r); }
inline bool isInf(double r) { return COSNaN::is_inf(r); }

inline double getNaN() { double r; COSNaN::set_nan(r); return r; }

//------

inline bool isInteger(double r) {
  if (isNaN(r)) return false;

  return std::abs(r - int(r)) < 1E-3;
}

inline bool realEq(double r1, double r2) {
  if (isNaN(r1) || isNaN(r2)) return false;

  return std::fabs(r2 - r1) < 1E-5;
}

inline bool isZero(double r) {
  if (isNaN(r)) return false;

  return realEq(r, 0.0);
}

//------

double toReal(const QString &str, bool &ok);
bool   toReal(const QString &str, double &r);

//------

long toInt(const QString &str, bool &ok);
bool toInt(const QString &str, long &i);

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

inline QRect toQRectI(const CQChartsGeom::BBox &rect) {
  return QRect(toQPointI(rect.getLL()), toQPointI(rect.getUR())).normalized();
}

inline CQChartsGeom::BBox fromQRect(const QRectF &rect) {
  return CQChartsGeom::BBox(fromQPoint(rect.bottomLeft()), fromQPoint(rect.topRight()));
}

//------

// sign of value
template<typename T>
int sign(T v) {
  return (T(0) < v) - (v < T(0));
}

// average of two reals
inline double avg(double x1, double x2) {
  return (x1 + x2)/2.0;
}

// map x in low->high to 0->1
inline double norm(double x, double low, double high) {
  if (high != low)
    return (x - low)/(high - low);
  else
    return 0;
}

// map x in 0->1 to low->high
inline double lerp(double x, double low, double high) {
  return low + (high - low)*x;
}

// map value in range low1->high2 to low2->high2
inline double map(double value, double low1, double high1, double low2, double high2) {
  return lerp(norm(value, low1, high1), low2, high2);
}

// clamp real value to range
inline double clamp(double val, double low, double high) {
  if (val < low ) return low;
  if (val > high) return high;
  return val;
}

// clamp integer value to range
inline double iclamp(int val, int low, int high) {
  if (val < low ) return low;
  if (val > high) return high;
  return val;
}

//---

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

enum Rounding {
  ROUND_DOWN,
  ROUND_UP,
  ROUND_NEAREST
};

inline int RoundNearest(double x) {
  double x1;

  if (x <= 0.0)
    x1 = (x - 0.499999);
  else
    x1 = (x + 0.500001);

  if (x1 < INT_MIN || x1 > INT_MAX)
    errno = ERANGE;

  return int(x1);
}

inline int RoundUp(double x) {
  double x1;

  if (x <= 0.0)
    x1 = (x       - 1E-6);
  else
    x1 = (x + 1.0 - 1E-6);

  if (x1 < INT_MIN || x1 > INT_MAX)
    errno = ERANGE;

  return int(x1);
}

inline int RoundDown(double x) {
  double x1;

  if (x >= 0.0)
    x1 = (x       + 1E-6);
  else
    x1 = (x - 1.0 + 1E-6);

  if (x1 < INT_MIN || x1 > INT_MAX)
    errno = ERANGE;

  return int(x1);
}

inline int Round(double x, Rounding rounding=ROUND_NEAREST) {
  switch (rounding) {
    case ROUND_UP  : return RoundUp(x);
    case ROUND_DOWN: return RoundDown(x);
    default        : return RoundNearest(x);
  }
}

inline double RoundNearestF(double x) {
  double x1;

  if (x <= 0.0)
    x1 = (x - 0.499999);
  else
    x1 = (x + 0.500001);

  return std::trunc(x1);
}

inline double RoundUpF(double x) {
  double x1;

  if (x <= 0.0)
    x1 = (x       - 1E-6);
  else
    x1 = (x + 1.0 - 1E-6);

  return std::trunc(x1);
}

inline double RoundDownF(double x) {
  double x1;

  if (x >= 0.0)
    x1 = (x       + 1E-6);
  else
    x1 = (x - 1.0 + 1E-6);

  return std::trunc(x1);
}

inline double RoundF(double x, Rounding rounding=ROUND_NEAREST) {
  switch (rounding) {
    case ROUND_UP  : return RoundUpF(x);
    case ROUND_DOWN: return RoundDownF(x);
    default        : return RoundNearestF(x);
  }
}

//------

inline double Deg2Rad(double d) { return M_PI*d/180.0; }
inline double Rad2Deg(double r) { return 180.0*r/M_PI; }

inline double normalizeAngle(double a, bool isEnd=false) {
  while (a < 0.0) a += 360.0;

  if (! isEnd) {
    while (a >= 360.0) a -= 360.0;
  }
  else {
    while (a > 360.0) a -= 360.0;
  }

  return a;
}

inline CQChartsGeom::Point AngleToPoint(const CQChartsGeom::Point &c,
                                        double xr, double yr, double a) {
  double ra = CQChartsUtil::Deg2Rad(a);

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

//------

QString pointToString  (const QPointF   &point);
QString rectToString   (const QRectF    &rect);
QString polygonToString(const QPolygonF &poly);

bool variantToString(const QVariant &var, QString &str);

inline QString toString(const QVariant &var, bool &ok) {
  ok = true;

  if (! var.isValid()) {
    ok = false;

    return "";
  }

  QString str;

  bool rc = variantToString(var, str);
  assert(rc);

  return str;
}

inline bool toString(const QVariant &var, QString &str) {
  bool ok;

  str = toString(var, ok);

  return ok;
}

inline bool isReal(const QVariant &var) {
  return (var.type() == QVariant::Double);
}

inline double toReal(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Double)
    return var.value<double>();

  QString str = toString(var, ok);

  if (! ok)
    return getNaN();

  return toReal(str, ok);
}

inline bool toReal(const QVariant &var, double &r) {
  bool ok;

  r = toReal(var, ok);

  return ok;
}

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

    if (isInteger(r))
      return int(r);
  }

  QString str = toString(var, ok);

  if (! ok)
    return 0;

  return toInt(str, ok);
}

inline bool isNumeric(const QVariant &var) {
  return isReal(var) || isInt(var);
}

//------

int nameToRole(const QString &name);

//------

QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orientation, int role, bool &ok);
QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orientation, bool &ok);

QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                          int role, bool &ok);
QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column, bool &ok);

//--

QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orient, int role, bool &ok);
QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orient, bool &ok);

QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                          int role, bool &ok);
QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column, bool &ok);

//--

bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         Qt::Orientation orient, const QVariant &var, int role);
bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         Qt::Orientation orientation, const QVariant &var);

bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         const QVariant &var, int role);
bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         const QVariant &var);

//--

bool setModelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                   const QVariant &var, int role);
bool setModelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                   const QVariant &var);

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

//------

inline bool varToBool(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Bool)
    return var.toBool();

  return false;
}

inline bool isColor(const QVariant &var) {
  return (var.type() == QVariant::Color);
}

inline CQChartsColor varToColor(const QVariant &var, bool &ok) {
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

inline QString varToString(const QVariant &var, bool &ok) {
  QString str;

  ok = variantToString(var, str);

  return str;
}

std::vector<double> varToReals(const QVariant &var, bool &ok);

std::vector<double> stringToReals(const QString &str, bool &ok);

//---

QVariant modelValue(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
QVariant modelValue(QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

QVariant modelValue(CQCharts *charts, QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
QVariant modelValue(CQCharts *charts, QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

//---

QString modelString(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
QString modelString(QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

QString modelString(CQCharts *charts, QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
QString modelString(CQCharts *charts, QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

QString modelHierString(CQCharts *charts, QAbstractItemModel *model, int row,
                        const CQChartsColumn &column, const QModelIndex &parent,
                        int role, bool &ok);
QString modelHierString(CQCharts *charts, QAbstractItemModel *model, int row,
                        const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

//---

double modelReal(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
double modelReal(QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

double modelReal(CQCharts *charts, QAbstractItemModel *model, int row,
                 const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
double modelReal(CQCharts *charts, QAbstractItemModel *model, int row,
                 const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

double modelHierReal(CQCharts *charts, QAbstractItemModel *model, int row,
                     const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
double modelHierReal(CQCharts *charts, QAbstractItemModel *model, int row,
                     const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

//---

long modelInteger(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
long modelInteger(QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

long modelInteger(CQCharts *charts, QAbstractItemModel *model, int row,
                  const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
long modelInteger(CQCharts *charts, QAbstractItemModel *model, int row,
                  const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

long modelHierInteger(CQCharts *charts, QAbstractItemModel *model, int row,
                      const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
long modelHierInteger(CQCharts *charts, QAbstractItemModel *model, int row,
                      const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

//---

CQChartsColor modelColor(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
CQChartsColor modelColor(QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

CQChartsColor modelColor(CQCharts *charts, QAbstractItemModel *model, int row,
                         const CQChartsColumn &column, const QModelIndex &parent,
                         int role, bool &ok);
CQChartsColor modelColor(CQCharts *charts, QAbstractItemModel *model, int row,
                         const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

//------

bool isValidModelColumn(QAbstractItemModel *model, int column);

int modelColumnNameToInd(const QAbstractItemModel *model, const QString &name);

bool stringToColumn(const QAbstractItemModel *model, const QString &str, CQChartsColumn &column);

bool stringToColumns(const QAbstractItemModel *model, const QString &str,
                     std::vector<CQChartsColumn> &columns);

//------

inline bool decodeModelFilterStr(QAbstractItemModel *model, const QString &filter,
                                 QString &filter1, int &column) {
  filter1 = filter;
  column  = -1;

  if (! filter1.length())
    return false;

  QStringList strs = filter1.split(':', QString::KeepEmptyParts);

  if (strs.size() != 2)
    return false;

  column = modelColumnNameToInd(model, strs[0]);

  if (column < 0)
    return false;

  filter1 = strs[1];

  return true;
}

//------

void exportModel(QAbstractItemModel *model, CQBaseModel::DataType type,
                 bool hheader=true, bool vheader=false,
                 std::ostream &os=std::cout);

}

//------

namespace CQChartsUtil {

void processAddExpression(QAbstractItemModel *model, const QString &exprStr);

int processExpression(QAbstractItemModel *model, const QString &exprStr);

int processExpression(QAbstractItemModel *model, CQChartsExprModel::Function function,
                      const CQChartsColumn &column, const QString &expr);

CQChartsExprModel *getExprModel(QAbstractItemModel *model);

CQDataModel *getDataModel(QAbstractItemModel *model);

}

//------

namespace CQChartsUtil {

// compare reals with tolerance
struct RealCmp {
  bool operator()(const double &lhs, const double &rhs) const {
    if (CQChartsUtil::realEq(lhs, rhs))
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

bool stringToPolygons(const QString &str, std::vector<QPolygonF> &polys);

bool stringToPolygon (const QString &str, QPolygonF &poly );
bool stringToRect    (const QString &str, QRectF    &rect );
bool stringToPoint   (const QString &str, QPointF   &point);

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

template<class T, class Callable>
void testAndSet(T &t, const T &v, Callable &&f) {
  if (v != t) {
    t = v;

    f();
  }
}

}

//------

namespace CQChartsUtil {

bool getBoolEnv(const char *name, bool def=false);

}

//------

#include <CQChartsModelVisitor.h>

namespace CQChartsUtil {

bool visitModel(QAbstractItemModel *model, CQChartsModelVisitor &visitor);

bool visitModel(QAbstractItemModel *model, const QModelIndex &parent, int r,
                CQChartsModelVisitor &visitor);

CQChartsModelVisitor::State visitModelIndex(QAbstractItemModel *model, const QModelIndex &parent,
                                            CQChartsModelVisitor &visitor);

CQChartsModelVisitor::State visitModelRow(QAbstractItemModel *model, const QModelIndex &parent,
                                          int r, CQChartsModelVisitor &visitor);

}

//------

#endif
