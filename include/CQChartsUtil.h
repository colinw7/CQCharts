#ifndef CQChartsUtil_H
#define CQChartsUtil_H

#include <CUnixFile.h>
#include <CLineDash.h>
#include <CBBox2D.h>
#include <CPoint2D.h>
#include <CVector2D.h>
#include <COSNaN.h>

#include <QAbstractItemModel>
#include <QVariant>
#include <QPen>
#include <QColor>
#include <QStringList>
#include <QRectF>

namespace CQChartsUtil {

inline double toReal(const QString &str, bool &ok) {
  ok = true;

  double r = 0.0;

  try {
    r = std::stod(str.toStdString());
  }
  catch (...) {
    ok = false;
  }

  return r;
}

inline double toReal(const QVariant &var, bool &ok) {
  return toReal(var.toString(), ok);
}

inline bool toReal(const QString &str, double &r) {
  bool ok;

  r = toReal(str, ok);

  return ok;
}

inline long toInt(const QString &str, bool &ok) {
  ok = true;

  long i = 0.0;

  try {
    i = std::stol(str.toStdString());
  }
  catch (...) {
    ok = false;
  }

  return i;
}

inline long toInt(const QVariant &var, bool &ok) {
  return toInt(var.toString(), ok);
}

inline bool toInt(const QString &str, long &i) {
  bool ok;

  i = toInt(str, ok);

  return ok;
}

inline QString toString(double r) {
  static char buffer[128];

#ifdef ALLOW_NAN
  if (COS::is_nan(real))
    strcpy(buffer, "NaN");
  else
    ::sprintf(buffer, "%g", r);
#else
  ::sprintf(buffer, "%g", r);
#endif

  return buffer;
}

inline QString toString(long i) {
  static char buffer[64];

  ::sprintf(buffer, "%lu", i);

  return buffer;
}

inline QString toString(const std::vector<int> &columns) {
  QString str;

  for (std::size_t i = 0; i < columns.size(); ++i) {
    if (str.length())
      str += " ";

    str += QString("%1").arg(columns[i]);
  }

  return str;
}

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

inline bool isInteger(double r) {
  return std::abs(r - int(r)) < 1E-3;
}

//------

inline QPointF toQPoint(const CPoint2D &point) {
  return QPointF(point.x, point.y);
}

inline QPoint toQPointI(const CPoint2D &point) {
  return QPoint(point.x, point.y);
}

inline CPoint2D fromQPoint(const QPointF &point) {
  return CPoint2D(point.x(), point.y());
}

inline CPoint2D fromQPointF(const QPoint &point) {
  return CPoint2D(point.x(), point.y());
}

inline QRectF toQRect(const CBBox2D &rect) {
  return QRectF(toQPoint(rect.getLL()), toQPoint(rect.getUR())).normalized();
}

inline QRect toQRectI(const CBBox2D &rect) {
  return QRect(toQPointI(rect.getLL()), toQPointI(rect.getUR())).normalized();
}

inline CBBox2D fromQRect(const QRectF &rect) {
  return CBBox2D(fromQPoint(rect.bottomLeft()), fromQPoint(rect.topRight()));
}

//------

inline double clamp(double val, double low, double high) {
  if (val < low ) return low;
  if (val > high) return high;
  return val;
}

inline double iclamp(int val, int low, int high) {
  if (val < low ) return low;
  if (val > high) return high;
  return val;
}

inline QColor blendColors(const QColor &c1, const QColor &c2, double f) {
  double f1 = 1.0 - f;

  double r = c1.redF  ()*f + c2.redF  ()*f1;
  double g = c1.greenF()*f + c2.greenF()*f1;
  double b = c1.blueF ()*f + c2.blueF ()*f1;

  return QColor(iclamp(255*r, 0, 255), iclamp(255*g, 0, 255), iclamp(255*b, 0, 255));
}

//------

inline void penSetLineDash(QPen &pen, const CLineDash &dash) {
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

//------

// distance between two points
inline double PointPointDistance(const CPoint2D &point1, const CPoint2D &point2) {
  double dx = point1.x - point2.x;
  double dy = point1.y - point2.y;

  return std::hypot(dx, dy);
}

// distance between point and line
inline bool PointLineDistance(const CPoint2D &point, const CPoint2D &lineStart,
                              const CPoint2D &lineEnd, double *dist) {
  CVector2D l (lineStart, lineEnd);
  CVector2D pl(lineStart, point  );

  double u1 = pl.dotProduct(l);
  double u2 = l .lengthSqr();

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
    CPoint2D intersection = lineStart + u*l;

    *dist = PointPointDistance(point, intersection);

    return true;
  }
}

//------

inline bool isNaN(double r) {
  return COSNaN::is_nan(r) && ! COSNaN::is_inf(r);
}

//------

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

inline QString modelString(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  QVariant var = model->data(ind, Qt::UserRole);

  if (! var.isValid())
    var = model->data(ind, Qt::DisplayRole);

  ok = var.isValid();

  return var.toString();
}

inline QString modelString(QAbstractItemModel *model, int row, int col, bool &ok) {
  QModelIndex ind = model->index(row, col);

  return modelString(model, ind, ok);
}

inline double modelReal(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  ok = true;

  QVariant var = model->data(ind, Qt::UserRole);

  if (! var.isValid())
    var = model->data(ind, Qt::DisplayRole);

  if (var.type() == QVariant::Double)
    return var.toReal();

  return toReal(var, ok);
}

inline double modelReal(QAbstractItemModel *model, int row, int col, bool &ok) {
  ok = true;

  QModelIndex ind = model->index(row, col);

  return modelReal(model, ind, ok);
}

inline long modelInteger(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  ok = true;

  QVariant var = model->data(ind, Qt::UserRole);

  if (! var.isValid())
    var = model->data(ind, Qt::DisplayRole);

  if (var.type() == QVariant::Int)
    return var.toInt();

  return toInt(var, ok);
}

inline long modelInteger(QAbstractItemModel *model, int row, int col, bool &ok) {
  ok = true;

  QModelIndex ind = model->index(row, col);

  return modelInteger(model, ind, ok);
}

}

#endif
