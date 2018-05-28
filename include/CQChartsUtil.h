#ifndef CQChartsUtil_H
#define CQChartsUtil_H

#include <CQChartsLineDash.h>
#include <CQChartsColumn.h>
#include <CQChartsGeom.h>
#include <CQChartsPath.h>
#include <CQChartsStyle.h>
#include <CQChartsModelVisitor.h>
#include <CQChartsColor.h>
#include <CQBaseModel.h>
#include <CQCsvModel.h>
#include <CQTsvModel.h>
#include <CQStrParse.h>
#include <COSNaN.h>

#include <QAbstractItemModel>
#include <QVariant>
#include <QPen>
#include <QStringList>
#include <QRectF>

using CQChartsNameValues = std::map<QString,QString>;

class CQCharts;

namespace CQChartsUtil {

bool isHierarchical(QAbstractItemModel *model);

int hierRowCount(QAbstractItemModel *model);

bool visitModel(QAbstractItemModel *model, CQChartsModelVisitor &visitor);

bool visitModel(QAbstractItemModel *model, const QModelIndex &parent, int r,
                CQChartsModelVisitor &visitor);

CQChartsModelVisitor::State visitModelIndex(QAbstractItemModel *model, const QModelIndex &parent,
                                            CQChartsModelVisitor &visitor);

CQChartsModelVisitor::State visitModelRow(QAbstractItemModel *model, const QModelIndex &parent,
                                          int r, CQChartsModelVisitor &visitor);

QString parentPath(QAbstractItemModel *model, const QModelIndex &parent);

//------

bool columnValueType(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                     CQBaseModel::Type &columnType, CQChartsNameValues &nameValues);

bool formatColumnTypeValue(CQCharts *charts, const QString &typeStr, double value, QString &str);

bool formatColumnValue(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                       double value, QString &str);

QVariant columnDisplayData(CQCharts *charts, QAbstractItemModel *model,
                           const CQChartsColumn &column, const QVariant &var);

QVariant columnUserData(CQCharts *charts, QAbstractItemModel *model,
                        const CQChartsColumn &column, const QVariant &var);

bool columnTypeStr(CQCharts *charts, QAbstractItemModel *model,
                   const CQChartsColumn &column, QString &typeStr);

bool setColumnTypeStr(CQCharts *charts, QAbstractItemModel *model,
                      const CQChartsColumn &column, const QString &typeStr);

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

  if (*p == '.') {
    ++p;

    while (*p == '0')
      ++p;
  }

  while (*p != 0 && ::isspace(*p))
    ++p;

  if (*p != '\0') {
    ok = false;
    return integer;
  }

  return integer;
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

//------

bool evalExpr(int row, const QString &expr, double &r);

//------

inline int nameToRole(const QString &name) {
  if      (name == "display"       ) return Qt::DisplayRole;
  else if (name == "user"          ) return Qt::UserRole;
  else if (name == "edit"          ) return Qt::EditRole;
  else if (name == "font"          ) return Qt::FontRole;
  else if (name == "size_hint"     ) return Qt::SizeHintRole;
  else if (name == "tool_tip"      ) return Qt::ToolTipRole;
  else if (name == "background"    ) return Qt::BackgroundRole;
  else if (name == "foreground"    ) return Qt::ForegroundRole;
  else if (name == "text_alignment") return Qt::TextAlignmentRole;
  else if (name == "text_color"    ) return Qt::TextColorRole;
  else if (name == "decoration"    ) return Qt::DecorationRole;
  else if (name == "type"          ) return (int) CQBaseModel::Role::Type;
  else if (name == "type_values"   ) return (int) CQBaseModel::Role::TypeValues;

  return -1;
}

//------

inline QVariant modelHeaderValue(QAbstractItemModel *model, int column, int role, bool &ok) {
  ok = false;

  if (column < 0)
    return QVariant();

  QVariant var = model->headerData(column, Qt::Horizontal, role);

  ok = var.isValid();

  return var;
}

inline QVariant modelHeaderValue(QAbstractItemModel *model, int column, bool &ok) {
  return modelHeaderValue(model, column, Qt::DisplayRole, ok);
}

inline QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                                 int role, bool &ok) {
  if (column.type() != CQChartsColumn::Type::DATA)
    return QVariant();

  return modelHeaderValue(model, column.column(), role, ok);
}

inline QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                                 bool &ok) {
  return modelHeaderValue(model, column, Qt::DisplayRole, ok);
}

//--

inline QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                                 int role, bool &ok) {
  if (! column.isValid())
    return "";

  QVariant var = modelHeaderValue(model, column, role, ok);
  if (! var.isValid()) return "";

  QString str;

  bool rc = variantToString(var, str);
  assert(rc);

  return str;
}

inline QString modelHeaderString(QAbstractItemModel *model, int column, int role, bool &ok) {
  return modelHeaderString(model, CQChartsColumn(column), role, ok);
}

inline QString modelHeaderString(QAbstractItemModel *model, int column, bool &ok) {
  return modelHeaderString(model, CQChartsColumn(column), Qt::DisplayRole, ok);
}

inline QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                                 bool &ok) {
  return modelHeaderString(model, column, Qt::DisplayRole, ok);
}

//--

inline bool setModelHeaderValue(QAbstractItemModel *model, int column,
                                const QVariant &var, int role)
{
  return model->setHeaderData(column, Qt::Horizontal, var, role);
}

inline bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                                const QVariant &var, int role)
{
  if (column.type() != CQChartsColumn::Type::DATA)
    return false;

  return setModelHeaderValue(model, column.column(), var, role);
}

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

inline bool isReal(const QVariant &var) {
  return (var.type() == QVariant::Double);
}

inline double varToReal(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Double)
    return var.toReal();

  return toReal(var, ok);
}

inline bool isInt(const QVariant &var) {
  return (var.type() == QVariant::Int);
}

inline long varToInt(const QVariant &var, bool &ok) {
  ok = true;

  if (var.type() == QVariant::Int)
    return var.toInt();

  return toInt(var, ok);
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

inline QVariant modelValue(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok) {
  if (! ind.isValid()) {
    ok = false;

    return QVariant();
  }

  QVariant var = model->data(ind, role);

  ok = var.isValid();

  return var;
}

inline QVariant modelValue(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  if (! ind.isValid()) {
    ok = false;

    return QVariant();
  }

  QVariant var = model->data(ind, Qt::EditRole);

  if (! var.isValid())
    var = model->data(ind, Qt::DisplayRole);

  ok = var.isValid();

  return var;
}

inline QVariant modelValue(QAbstractItemModel *model, int row, const CQChartsColumn &col,
                           const QModelIndex &parent, int role, bool &ok) {
  if (! col.isValid()) {
    ok = false;

    return QVariant();
  }

  if      (col.type() == CQChartsColumn::Type::DATA) {
    QModelIndex ind = model->index(row, col.column(), parent);

    return modelValue(model, ind, role, ok);
  }
  else if (col.type() == CQChartsColumn::Type::VHEADER) {
    ok = true;

    QVariant var = model->headerData(row, Qt::Vertical, role);

    return var;
  }
  else {
    ok = false;

    return QVariant();
  }
}

inline QVariant modelValue(QAbstractItemModel *model, int row, const CQChartsColumn &col,
                           const QModelIndex &parent, bool &ok) {
  if (! col.isValid()) {
    ok = false;

    return QVariant();
  }

  if      (col.type() == CQChartsColumn::Type::DATA) {
    QModelIndex ind = model->index(row, col.column(), parent);

    return modelValue(model, ind, ok);
  }
  else if (col.type() == CQChartsColumn::Type::VHEADER) {
    ok = true;

    QVariant var = model->headerData(row, Qt::Vertical);

    return var;
  }
  else {
    ok = false;

    return QVariant();
  }
}

//---

inline QString modelString(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  QVariant var = modelValue(model, ind, ok);
  if (! ok) return "";

  QString str;

  bool rc = variantToString(var, str);
  assert(rc);

  return str;
}

inline QString modelString(QAbstractItemModel *model, int row, const CQChartsColumn &col,
                           const QModelIndex &parent, bool &ok) {
  if      (col.type() == CQChartsColumn::Type::DATA) {
    QModelIndex ind = model->index(row, col.column(), parent);

    return modelString(model, ind, ok);
  }
  else if (col.type() == CQChartsColumn::Type::VHEADER) {
    ok = true;

    QVariant var = model->headerData(row, Qt::Vertical);

    QString str;

    bool rc = variantToString(var, str);
    assert(rc);

    return str;
  }
  else {
    ok = false;

    return "";
  }
}

//---

inline double modelReal(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  QVariant var = modelValue(model, ind, ok);
  if (! ok) return 0.0;

  return varToReal(var, ok);
}

inline double modelReal(QAbstractItemModel *model, int row, const CQChartsColumn &col,
                        const QModelIndex &parent, bool &ok) {
  if      (col.type() == CQChartsColumn::Type::DATA) {
    QModelIndex ind = model->index(row, col.column(), parent);

    return modelReal(model, ind, ok);
  }
  else if (col.type() == CQChartsColumn::Type::EXPR) {
    double r = 0.0;

    ok = CQChartsUtil::evalExpr(row, col.expr(), r);

    return r;
  }
  else {
    ok = false;

    return 0.0;
  }
}

//---

inline long modelInteger(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  QVariant var = modelValue(model, ind, ok);
  if (! ok) return 0;

  return varToInt(var, ok);
}

inline long modelInteger(QAbstractItemModel *model, int row, const CQChartsColumn &col,
                         const QModelIndex &parent, bool &ok) {
  if      (col.type() == CQChartsColumn::Type::DATA) {
    QModelIndex ind = model->index(row, col.column(), parent);

    return modelInteger(model, ind, ok);
  }
  else if (col.type() == CQChartsColumn::Type::EXPR) {
    double r = 0.0;

    ok = CQChartsUtil::evalExpr(row, col.expr(), r);

    return r;
  }
  else {
    ok = false;

    return 0;
  }
}

//---

inline CQChartsColor modelColor(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  QVariant var = modelValue(model, ind, ok);
  if (! ok) return CQChartsColor();

  if (isColor(var))
    return CQChartsColor(var.value<QColor>());

  CQChartsColor color;

  if (isReal(var)) {
    double r;

    if (toReal(var, r))
      color = CQChartsColor(CQChartsColor::Type::PALETTE, r);
  }
  else {
    QString str;

    if (CQChartsUtil::toString(var, str))
      color = CQChartsColor(str);
  }

  return color;
}

inline CQChartsColor modelColor(QAbstractItemModel *model, int row, const CQChartsColumn &col,
                                const QModelIndex &parent, bool &ok) {
  if (! col.isValid()) {
    ok = false;

    return CQChartsColor();
  }

  if      (col.type() == CQChartsColumn::Type::DATA) {
    QModelIndex ind = model->index(row, col.column(), parent);

    return modelColor(model, ind, ok);
  }
  else {
    ok = false;

    return CQChartsColor();
  }
}

//------

inline bool isValidModelColumn(QAbstractItemModel *model, int column) {
  return (column >= 0 && column < model->columnCount());
}

inline int modelColumnNameToInd(QAbstractItemModel *model, const QString &name) {
  int role = Qt::DisplayRole;

  for (int i = 0; i < model->columnCount(); ++i) {
    QVariant var1 = model->headerData(i, Qt::Horizontal, role);

    if (! var1.isValid())
      continue;

    QString name1;

    bool rc = variantToString(var1, name1);
    assert(rc);

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

inline bool fileToLines(const QString &filename, std::vector<QString> &lines, int maxLines=-1) {
  auto open = [&](FILE* &fp, const QString &filename) -> bool {
    fp = fopen(filename.toStdString().c_str(), "r");
    if (! fp) return false;

    return true;
  };

  auto readLine = [](FILE *fp, QString &line) {
    line = "";

    if (feof(fp)) return false;

    int c = fgetc(fp);

    if (c == EOF)
      return false;

    while (! feof(fp) && c != '\n') {
      line += c;

      c = fgetc(fp);
    }

    return true;
  };

  auto close = [](FILE* &fp) {
    if (fp)
      fclose(fp);

    fp = 0;
  };

  //---

  FILE *fp = nullptr;

  if (! open(fp, filename))
    return false;

  QString line;

  while (readLine(fp, line)) {
    lines.push_back(line);

    if (maxLines >= 0 && int(lines.size()) > maxLines)
      break;
  }

  close(fp);

  //---

  return true;
}

inline bool analyzeFile(const QString &filename, CQBaseModel::DataType &dataType,
                        bool &commentHeader, bool &firstLineHeader,
                        bool &firstColumnHeader) {
  dataType = CQBaseModel::DataType::NONE;

  commentHeader     = false;
  firstLineHeader   = false;
  firstColumnHeader = false;

  //---

  int maxLines = 10;

  std::vector<QString> lines;

  if (! fileToLines(filename, lines, maxLines))
    return false;

  //---

  std::size_t lineNum = 0;

  // check for comment first line
  if (lineNum < lines.size()) {
    QString line = lines[lineNum];

    CQStrParse parse(line);

    parse.skipSpace();

    if (parse.getChar() == '#') {
      commentHeader = true;

      ++lineNum;

      // skip subsequent comment lines
      while (true) {
        QString line = lines[lineNum];

        CQStrParse parse(line);

        parse.skipSpace();

        if (parse.getChar() != '#')
          break;

        ++lineNum;
      }
    }
  }

  // check line for comma, tab, space separators
  // TODO: combine multiple lines
  if (lineNum < lines.size()) {
    QString line = lines[lineNum];

    int commaPos = line.indexOf(',');
    int tabPos   = line.indexOf('\t');
    int spacePos = line.indexOf(' ');

    CQStrParse parse(line);

    QStringList commaStrs, tabStrs, spaceStrs;

    if (commaPos >= 0) commaStrs = line.split(',' , QString::KeepEmptyParts);
    if (tabPos   >= 0) tabStrs   = line.split('\t', QString::KeepEmptyParts);
    if (spacePos >= 0) spaceStrs = line.split(' ' , QString::SkipEmptyParts);

    int nc = commaStrs.length();
    int nt = tabStrs  .length();
    int ns = spaceStrs.length();

    if      (nc > 0 && nc > nt)
      dataType = CQBaseModel::DataType::CSV;
    else if (nt > 0 && nt > nc)
      dataType = CQBaseModel::DataType::TSV;
    else if (ns > 0)
      dataType = CQBaseModel::DataType::GNUPLOT;
  }

  // TODO: auto determine column type from first few lines ?

  // TODO: if no header then check first line again (using data type) for column heade like
  // line (string and different type from other lines)

  return true;
}

//------

inline void exportModel(QAbstractItemModel *model, CQBaseModel::DataType type,
                        bool hheader=true, bool vheader=false,
                        std::ostream &os=std::cout) {
  if      (type == CQBaseModel::DataType::CSV) {
    CQCsvModel csv;

    csv.setFirstLineHeader  (hheader);
    csv.setFirstColumnHeader(vheader);

    csv.save(model, os);
  }
  else if (type == CQBaseModel::DataType::TSV) {
    CQTsvModel tsv;

    tsv.setFirstLineHeader  (hheader);
    tsv.setFirstColumnHeader(vheader);

    tsv.save(model, os);
  }
  else {
    assert(false);
  }
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
