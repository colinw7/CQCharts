#ifndef CQChartsUtil_H
#define CQChartsUtil_H

#include <CQChartsLineDash.h>
#include <CQChartsColumn.h>
#include <CQChartsGeom.h>
#include <CQChartsPath.h>
#include <CQChartsStyle.h>
#include <CQChartsColor.h>
#include <CQChartsExprModel.h>
#include <CQBaseModel.h>
#include <CMathUtil.h>

#include <QAbstractItemModel>
#include <QVariant>
#include <QPen>
#include <QStringList>
#include <QRectF>

using CQChartsNameValues = std::map<QString,QString>;

class CQCharts;
class CQChartsColumnType;
class CQDataModel;
class CQStrParse;

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

template<class T, class NOTIFIER>
void testAndSet(T &t, const T &v, NOTIFIER &&notifier) {
  if (v != t) {
    t = v;

    notifier();
  }
}

}

//------

namespace CQChartsUtil {

QString replaceModelExprVars(const QString &expr, QAbstractItemModel *model,
                             const QModelIndex &ind, int nr, int nc);

}

//------

#endif
