#ifndef CQChartsUtil_H
#define CQChartsUtil_H

#include <CQChartsLineDash.h>
#include <CQChartsGeom.h>
#include <COSNaN.h>

#include <QAbstractItemModel>
#include <QVariant>
#include <QPen>
#include <QColor>
#include <QStringList>
#include <QRectF>

class CQCharts;

namespace CQChartsUtil {

class ModelVisitor {
 public:
  enum class State {
    OK,
    SKIP,
    TERMINATE
  };

 public:
  ModelVisitor() { }

  virtual ~ModelVisitor() { }

  int numCols() const { return numCols_; }

  int row() const { return row_; }

  int numRows() const { return numRows_; }
  void setNumRows(int nr) { numRows_ = nr; }

  void init(int nc) { numCols_ = nc; row_ = 0; numRows_ = 0; }

  void step() { ++row_; }

  void term() { numRows_ = row_; }

  //---

  virtual State hierVisit(QAbstractItemModel *, const QModelIndex &, int) { return State::OK; }

  virtual State hierPostVisit(QAbstractItemModel *, const QModelIndex &, int) { return State::OK; }

  //---

  virtual State preVisit(QAbstractItemModel *, const QModelIndex &, int) { return State::OK; }

  virtual State visit(QAbstractItemModel *, const QModelIndex &, int) { return State::OK; }

  //virtual State postVisit(QAbstractItemModel *, const QModelIndex &, int) { return State::OK; }

 protected:
  int numCols_ { 0 };
  int row_     { 0 };
  int numRows_ { 0 };
};

bool isHierarchical(QAbstractItemModel *model);

int hierRowCount(QAbstractItemModel *model);

bool visitModel(QAbstractItemModel *model, ModelVisitor &visitor);

bool visitModel(QAbstractItemModel *model, const QModelIndex &parent, int r,
                ModelVisitor &visitor);

ModelVisitor::State visitModelIndex(QAbstractItemModel *model, const QModelIndex &parent,
                                    ModelVisitor &visitor);

ModelVisitor::State visitModelRow(QAbstractItemModel *model, const QModelIndex &parent,
                                  int r, ModelVisitor &visitor);

QString parentPath(QAbstractItemModel *model, const QModelIndex &parent);

//------

class ModelColumnDetails {
 public:
  ModelColumnDetails(CQCharts *charts, QAbstractItemModel *model, int column);

  virtual ~ModelColumnDetails() { }

  CQCharts *charts() const { return charts_; }

  QAbstractItemModel *model() const { return model_; }

  int column() const { return column_; }

  QString typeName() const;

  QVariant minValue() const;
  QVariant maxValue() const;

  int numRows() const;

  virtual bool checkRow(const QVariant &) { return true; }

 private:
  bool init();

 private:
  CQCharts           *charts_ { nullptr };
  QAbstractItemModel *model_ { nullptr };
  int                 column_ { 0 };
  QString             typeName_;
  QVariant            minValue_;
  QVariant            maxValue_;
  int                 numRows_;
  bool                initialized_ { false };
};

//------

inline bool isNaN(double r) {
  return COSNaN::is_nan(r) && ! COSNaN::is_inf(r);
}

inline bool isInf(double r) {
  return COSNaN::is_inf(r);
}

inline double getNaN() {
  double r;

  COSNaN::set_nan(r);

  return r;
}

//------

inline bool isInteger(double r) {
  if (isNaN(r))
    return false;

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

inline double toReal(const QString &str, bool &ok) {
  ok = true;

  double r = 0.0;

  //---

  std::string sstr = str.toStdString();

  const char *c_str = sstr.c_str();

  int i = 0;

  while (c_str[i] != 0 && ::isspace(c_str[i]))
    ++i;

  if (c_str[i] == '\0') {
    ok = false;
    return r;
  }

  const char *p;

#ifdef ALLOW_NAN
  if (COS::has_nan() && strncmp(&c_str[i], "NaN", 3) == 0)
    p = &c_str[i + 3];
  else {
    errno = 0;

    r = strtod(&c_str[i], (char **) &p);

    if (errno == ERANGE) {
      ok = false;
      return r;
    }
  }
#else
  errno = 0;

  r = strtod(&c_str[i], (char **) &p);

  if (errno == ERANGE) {
    ok = false;
    return r;
  }
#endif

  while (*p != 0 && ::isspace(*p))
    ++p;

  if (*p != '\0')
    ok = false;

  return r;
}

inline double toReal(const QVariant &var, bool &ok) {
  if (! var.isValid()) {
    ok = false;

    return getNaN();
  }

  return toReal(var.toString(), ok);
}

inline bool toReal(const QString &str, double &r) {
  bool ok;

  r = toReal(str, ok);

  return ok;
}

//------

inline long toInt(const QString &str, bool &ok) {
  ok = true;

  long integer = 0;

  std::string sstr = str.toStdString();

  const char *c_str = sstr.c_str();

  int i = 0;

  while (c_str[i] != 0 && ::isspace(c_str[i]))
    ++i;

  if (c_str[i] == '\0') {
    ok = false;
    return integer;
  }

  const char *p;

  errno = 0;

  integer = strtol(&c_str[i], (char **) &p, 10);

  if (errno == ERANGE) {
    ok = false;
    return integer;
  }

  while (*p != 0 && ::isspace(*p))
    ++p;

  if (*p == '.') {
    ++p;

    while (*p == '0')
      ++p;
  }

  if (*p != '\0') {
    ok = false;

    return integer;
  }

  return integer;
}

inline long toInt(const QVariant &var, bool &ok) {
  if (! var.isValid()) {
    ok = false;

    return 0;
  }

  return toInt(var.toString(), ok);
}

inline bool toInt(const QString &str, long &i) {
  bool ok;

  i = toInt(str, ok);

  return ok;
}

//------

QString toString(double r, const QString &fmt="%g" );
QString toString(long   i, const QString &fmt="%ld");

QString toString(const std::vector<int> &columns);

//------

inline bool fromString(const QString &str, std::vector<int> &columns) {
  bool ok = true;

  columns.clear();

  QStringList strs = str.split(" ", QString::SkipEmptyParts);

  for (int i = 0; i < strs.size(); ++i) {
    bool ok1;

    int col = strs[i].toInt(&ok1);

    if (ok1)
      columns.push_back(col);
    else
      ok = false;
  }

  return ok;
}

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

inline bool intersectLines(const QPointF &l1s, const QPointF &l1e,
                           const QPointF &l2s, const QPointF &l2e, QPointF &pi) {
  double dx1 = l1e.x() - l1s.x();
  double dy1 = l1e.y() - l1s.y();
  double dx2 = l2e.x() - l2s.x();
  double dy2 = l2e.y() - l2s.y();

  double delta = dx1*dy2 - dy1*dx2;

  if (fabs(delta) < 1E-6) // parallel
    return false;

  double idelta = 1.0/delta;

  double dx = l2s.x() - l1s.x();
  double dy = l2s.y() - l1s.y();

  double m1 = (dx*dy2 - dy*dx2)*idelta;
//double m2 = (dx*dy1 - dy*dx1)*idelta;

  double xi = l1s.x() + m1*dx1;
  double yi = l1s.y() + m1*dy1;

  pi = QPointF(xi, yi);

  return true;
}

inline bool intersectLines(double x11, double y11, double x21, double y21,
                           double x12, double y12, double x22, double y22,
                           double &xi, double &yi)
{
  QPointF pi;

  bool rc = intersectLines(QPointF(x11, y11), QPointF(x21, y21),
                           QPointF(x12, y12), QPointF(x22, y22), pi);

  xi = pi.x();
  yi = pi.y();

  return rc;
}

//---

inline QColor bwColor(const QColor &c) {
  int g = qGray(c.red(), c.green(), c.blue());

  return (g > 128 ? QColor(0,0,0) : QColor(255, 255, 255));
}

inline QColor invColor(const QColor &c) {
  return QColor(255 - c.red(), 255 - c.green(), 255 - c.blue());
}

inline QColor blendColors(const QColor &c1, const QColor &c2, double f) {
  double f1 = 1.0 - f;

  double r = c1.redF  ()*f + c2.redF  ()*f1;
  double g = c1.greenF()*f + c2.greenF()*f1;
  double b = c1.blueF ()*f + c2.blueF ()*f1;

  return QColor(iclamp(255*r, 0, 255), iclamp(255*g, 0, 255), iclamp(255*b, 0, 255));
}

inline QColor blendColors(const std::vector<QColor> &colors) {
  if (colors.empty())
    return QColor();

  double f = 1.0/colors.size();

  double r = 0.0;
  double g = 0.0;
  double b = 0.0;

  for (const auto &c : colors) {
    r += c.redF  ()*f;
    g += c.greenF()*f;
    b += c.blueF ()*f;
  }

  return QColor(iclamp(255*r, 0, 255), iclamp(255*g, 0, 255), iclamp(255*b, 0, 255));
}

//------

inline void penSetLineDash(QPen &pen, const CQChartsLineDash &dash) {
  int num = dash.getNumLengths();

  if (num > 0) {
    pen.setStyle(Qt::CustomDashLine);

    pen.setDashOffset(dash.getOffset());

    QVector<qreal> dashes;

    double w = pen.widthF();

    if (w <= 0.0) w = 1.0;

    for (int i = 0; i < num; ++i)
      dashes << dash.getLength(i)*w;

    if (num & 1)
      dashes << dash.getLength(0)*w;

    pen.setDashPattern(dashes);
  }
  else
    pen.setStyle(Qt::SolidLine);
}

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
inline double PointPointDistance(const CQChartsGeom::Point &point1,
                                 const CQChartsGeom::Point &point2) {
  double dx = point1.x - point2.x;
  double dy = point1.y - point2.y;

  return std::hypot(dx, dy);
}

// distance between point and line
inline bool PointLineDistance(const CQChartsGeom::Point &point,
                              const CQChartsGeom::Point &lineStart,
                              const CQChartsGeom::Point &lineEnd, double *dist) {
  double dx1 = lineEnd.x - lineStart.x;
  double dy1 = lineEnd.y - lineStart.y;

  double dx2 = point.x - lineStart.x;
  double dy2 = point.y - lineStart.y;

  double u1 = dx2*dx1 + dy2*dy1;
  double u2 = dx1*dx1 + dy1*dy1;

  if (u2 <= 0.0) {
    *dist = PointPointDistance(point, lineStart);
    return false;
  }

  double u = u1/u2;

  if      (u < 0.0) {
    *dist = PointPointDistance(point, lineStart);
    return false;
  }
  else if (u > 1.0) {
    *dist = PointPointDistance(point, lineEnd);
    return false;
  }
  else {
    CQChartsGeom::Point intersection = lineStart + u*CQChartsGeom::Point(dx1, dy1);

    *dist = PointPointDistance(point, intersection);

    return true;
  }
}

//------

#if 0
#include <CUnixFile.h>

inline bool fileToLines(const QString &filename, QStringList &lines) {
  // open file
  CUnixFile file(filename.toStdString());

  if (! file.open())
    return false;

  // read lines
  std::string line;

  while (file.readLine(line))
    lines.push_back(line.c_str());

  return true;
}
#endif

//------

inline QVariant modelHeaderValue(QAbstractItemModel *model, int column, bool &ok) {
  ok = false;

  if (column < 0)
    return QVariant();

  QVariant var = model->headerData(column, Qt::Horizontal);

  ok = var.isValid();

  return var;
}

inline QString modelHeaderString(QAbstractItemModel *model, int column, bool &ok) {
  QVariant var = modelHeaderValue(model, column, ok);

  return var.toString();
}

//------

inline double varToReal(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Double)
    return var.toReal();

  return toReal(var, ok);
}

inline long varToInt(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Int)
    return var.toInt();

  return toInt(var, ok);
}

//---

inline QVariant modelValue(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  ok = true;

  QVariant var = model->data(ind, Qt::EditRole);

  if (! var.isValid())
    var = model->data(ind, Qt::DisplayRole);

  ok = var.isValid();

  return var;
}

#if 0
inline QVariant modelValue(QAbstractItemModel *model, int row, int col, bool &ok) {
  QModelIndex ind = model->index(row, col);

  return modelValue(model, ind, ok);
}
#endif

inline QString modelString(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  QVariant var = modelValue(model, ind, ok);
  if (! ok) return "";

  return var.toString();
}

#if 0
inline QString modelString(QAbstractItemModel *model, int row, int col, bool &ok) {
  QModelIndex ind = model->index(row, col);

  return modelString(model, ind, ok);
}
#endif

inline double modelReal(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  QVariant var = modelValue(model, ind, ok);
  if (! ok) return 0.0;

  return varToReal(var, ok);
}

#if 0
inline double modelReal(QAbstractItemModel *model, int row, int col, bool &ok) {
  QModelIndex ind = model->index(row, col);

  return modelReal(model, ind, ok);
}
#endif

inline long modelInteger(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  QVariant var = modelValue(model, ind, ok);
  if (! ok) return 0;

  return varToInt(var, ok);
}

#if 0
inline long modelInteger(QAbstractItemModel *model, int row, int col, bool &ok) {
  QModelIndex ind = model->index(row, col);

  return modelInteger(model, ind, ok);
}
#endif

//------

inline bool isValidModelColumn(QAbstractItemModel *model, int column) {
  return (column >= 0 && column < model->columnCount());
}

inline int modelColumnNameToInd(QAbstractItemModel *model, const QString &name) {
  for (int i = 0; i < model->columnCount(); ++i) {
    QString name1 = model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();

    if (name == name1)
      return i;
  }

  bool ok;

  int column = name.toInt(&ok);

  if (ok)
    return column;

  return -1;
}

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

  column = CQChartsUtil::modelColumnNameToInd(model, strs[0]);

  if (column < 0)
    return false;

  filter1 = strs[1];

  return true;
}

//------

// compare reals with tolerance
struct RealCmp {
  bool operator()(const double &lhs, const double &rhs) const {
    if (CQChartsUtil::realEq(lhs, rhs))
      return false;

    return lhs < rhs;
  }
};

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

#if 0
template<typename KEY>
class CQChartsOrderedSet {
 public:
  CQChartsOrderedSet() { }

  void add(const KEY &key) {
    auto p = keySet_.find(key);

    if (p == keySet_.end()) {
      keySet_.insert(key);

      keys_.push_back(key);
    }
  }

 private:
  using KeySet = std::set<KEY>;
  using Keys   = std::vector<KEY>;

  KeySet keySet_;
  Keys   keys_;
};

template<typename KEY, typename VALUE>
class CQChartsOrderedMap {
 public:
  CQChartsOrderedMap() { }

  void add(const KEY &key, const VALUE &value) {
    auto p = keyValues_.find(key);

    if (p == keyValues_.end())
      p = keyValues_.insert(p, KeyValues::value_type(key, value));

    p.second.add(value);
  }

 private:
  using ValueSet = CQChartsOrderedSet<VALUE>;
  using KeyMap   = std::map<KEY,ValueSet>;

  KeyMap keyMap_;
  Keys   keys_;
};
#endif

#endif
