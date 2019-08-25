#include <CQChartsUtil.h>
#include <CQChartsEval.h>
#include <CQChartsVariant.h>
#include <CQChartsLineDash.h>
#include <CQChartsPath.h>
#include <CQChartsStyle.h>
#include <CQCharts.h>

#include <CQUtil.h>
#include <CQStrUtil.h>
#include <CQStrParse.h>
#include <CPrintF.h>

#include <QPainter>
#include <QFontMetricsF>

namespace CQChartsUtil {

double toReal(const QString &str, bool &ok) {
  return CQStrUtil::toReal(str, ok);
}

bool toReal(const QString &str, double &r) {
  bool ok;

  r = toReal(str, ok);

  return ok;
}

//------

long toInt(const QString &str, bool &ok) {
  return CQStrUtil::toInt(str, ok);
}

bool toInt(const QString &str, long &i) {
  bool ok;

  i = toInt(str, ok);

  return ok;
}

long toInt(const char *str, bool &ok, const char **rstr) {
  return CQStrUtil::toInt(str, ok, rstr);
}

//---

QString toString(const std::vector<CQChartsColumn> &columns) {
  QString str;

  for (std::size_t i = 0; i < columns.size(); ++i) {
    if (str.length())
      str += " ";

    str += QString("%1").arg(columns[i].toString());
  }

  return str;
}

bool fromString(const QString &str, std::vector<CQChartsColumn> &columns) {
  bool ok = true;

  columns.clear();

  QStringList strs = str.split(" ", QString::SkipEmptyParts);

  for (int i = 0; i < strs.size(); ++i) {
    bool ok1;

    long col = toInt(strs[i], ok1);

    if (ok1)
      columns.push_back(col);
    else
      ok = false;
  }

  return ok;
}

//------

QString formatVar(const QVariant &var, const QString &fmt) {
  class VarPrintF : public CPrintF {
   public:
    VarPrintF(const QVariant &var, const QString &fmt) :
     CPrintF(fmt.toStdString()), var_(var) {
    }

    int   getInt     () const { bool ok; return var_.toInt(&ok); }
    long  getLong    () const { bool ok; return var_.toInt(&ok); }
    LLong getLongLong() const { bool ok; return var_.toInt(&ok); }

    double getDouble() const {
      bool ok;
      double r = var_.toDouble(&ok);
      if (CMathUtil::isZero(r)) r = 0.0;
      return r;
    }

    std::string getString() const { return var_.toString().toStdString(); }

   private:
    QVariant var_;
  };

  VarPrintF p(var, fmt);

  return p.format().c_str();
}

QString formatReal(double r, const QString &fmt) {
#ifdef ALLOW_NAN
  if (COS::is_nan(real))
    return "NaN";
#endif

  if (fmt == "%g" && CMathUtil::isZero(r))
    return "0.0";

  static char buffer[128];

#if 0
  if (fmt == "%T") {
    // format real in buffer
    bool negative = (r < 0);

    if (negative)
      r = -r;

    ::sprintf(buffer, "%g", r);

    // find decimal point - if less than 4 digits to left then done
    QString res = buffer;

    int pos = res.indexOf('.');

    if (pos < 0)
      pos = res.length();

    if (pos <= 3) {
      if (negative)
        return "-" + res;
      else
        return res;
    }

    // add commas to value to left of decimal point
    QString rhs = res.mid(pos);

    int ncomma = pos/3; // number of comma to add

    QString lhs;

    if (negative)
      lhs = "-";

    // add digits before first comma
    int pos1 = pos - ncomma*3;

    lhs += res.mid(0, pos1);

    for (int i = 0; i < ncomma; ++i) {
      // add comma and next set of 3 digits
      lhs += "," + res.mid(pos1, 3);

      pos1 += 3;
    }

    return lhs + rhs;
  }
#endif

  // format real in buffer
  ::sprintf(buffer, fmt.toLatin1().constData(), r);

  return buffer;
}

QString formatInteger(long i, const QString &fmt) {
  static char buffer[64];

  ::sprintf(buffer, fmt.toLatin1().constData(), i);

  return buffer;
}

}

//---

namespace CQChartsUtil {

bool intersectLines(const QPointF &l1s, const QPointF &l1e,
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

bool intersectLines(double x11, double y11, double x21, double y21,
                    double x12, double y12, double x22, double y22,
                    double &xi, double &yi) {
  QPointF pi;

  bool rc = intersectLines(QPointF(x11, y11), QPointF(x21, y21),
                           QPointF(x12, y12), QPointF(x22, y22), pi);

  xi = pi.x();
  yi = pi.y();

  return rc;
}

}

//---

namespace CQChartsUtil {

QColor bwColor(const QColor &c) {
  int g = qGray(c.red(), c.green(), c.blue());

  return (g > 128 ? QColor(0,0,0) : QColor(255, 255, 255));
}

QColor invColor(const QColor &c) {
  return QColor(255 - c.red(), 255 - c.green(), 255 - c.blue());
}

QColor blendColors(const QColor &c1, const QColor &c2, double f) {
  double f1 = 1.0 - f;

  double r = c1.redF  ()*f + c2.redF  ()*f1;
  double g = c1.greenF()*f + c2.greenF()*f1;
  double b = c1.blueF ()*f + c2.blueF ()*f1;

  return rgbToColor(r, g, b);
}

QColor blendColors(const std::vector<QColor> &colors) {
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

  return rgbToColor(r, g, b);
}

QColor rgbToColor(double r, double g, double b) {
  return QColor(CMathUtil::clamp(int(255*r), 0, 255),
                CMathUtil::clamp(int(255*g), 0, 255),
                CMathUtil::clamp(int(255*b), 0, 255));
}

}

//------

namespace CQChartsUtil {

void penSetLineDash(QPen &pen, const CQChartsLineDash &dash) {
  int num = dash.getNumLengths();

  if (num > 0) {
    pen.setStyle(Qt::CustomDashLine);

    pen.setDashOffset(dash.getOffset());

    QVector<qreal> dashes;

    double w = pen.widthF();

    if (w <= 0.0) w = 1.0;

    for (int i = 0; i < num; ++i)
      dashes << dash.getLength(i);

    if (num & 1)
      dashes << dash.getLength(0);

    pen.setDashPattern(dashes);
  }
  else
    pen.setStyle(Qt::SolidLine);
}

}

//---

namespace CQChartsUtil {

// distance between two points
double PointPointDistance(const CQChartsGeom::Point &point1, const CQChartsGeom::Point &point2) {
  double dx = point1.x - point2.x;
  double dy = point1.y - point2.y;

  return std::hypot(dx, dy);
}

// distance between point and line
bool PointLineDistance(const CQChartsGeom::Point &point, const CQChartsGeom::Point &lineStart,
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

}

//------

namespace CQChartsUtil {

bool fileToLines(const QString &filename, QStringList &lines, int maxLines) {
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

}

//------

namespace {

void findStringSplits1(const QString &str, std::vector<int> &splits) {
  int len = str.length();
  assert(len);

  for (int i = 1; i < len; ++i) {
    if (str[i].isSpace())
      splits.push_back(i);
  }
}

void findStringSplits2(const QString &str, std::vector<int> &splits) {
  int len = str.length();
  assert(len);

  for (int i = 0; i < len; ++i) {
    if (str[i].isPunct()) {
      int i1 = i;

      // keep consecutive punctuation together (::, ..., etc)
      while (i < len - 1 && str[i].isPunct())
        ++i;

      if (i1 == 0 || i >= len) // don't break if at start or end
        continue;

      splits.push_back(i1);
    }
  }
}

void findStringSplits3(const QString &str, std::vector<int> &splits) {
  int len = str.length();
  assert(len);

  for (int i = 1; i < len; ++i) {
    if (str[i - 1].isLower() && str[i].isUpper())
      splits.push_back(i);
  }
}

}

//------

namespace CQChartsUtil {

std::vector<double> stringToReals(const QString &str, bool &ok) {
  std::vector<double> reals;

  CQStrParse parse(str);

  parse.skipSpace();

  if (parse.isChar('{')) {
    QString str1;

    if (! parse.readBracedString(str1)) {
      ok = false;
      return reals;
    }

    return stringToReals(str1, ok);
  }

  while (! parse.eof()) {
    parse.skipSpace();

    double r;

    if (! parse.readReal(&r))
      break;

    reals.push_back(r);
  }

  return reals;
}

}

//------

namespace CQChartsUtil {

int countLeadingBraces(const QString &str) {
  CQStrParse parse(str);

  int n = 0;

  while (! parse.eof()) {
    parse.skipSpace();

    if (! parse.isChar('{'))
      break;

    parse.skipChar();

    ++n;
  }

  return n;
}

bool stringToPolygons(const QString &str, std::vector<QPolygonF> &polygons) {
  CQStrParse parse(str);

  parse.skipSpace();

  int pos = parse.getPos();

  bool braced = false;

  if (parse.isChar('{')) {
    parse.skipChar();

    parse.skipSpace();

    if (parse.isChar('{')) {
      braced = true;
    }
    else
      parse.setPos(pos);
  }

  while (! parse.eof()) {
    parse.skipSpace();

    if (! parse.isChar('{'))
      return false;

    QString polyStr;

    if (! parse.readBracedString(polyStr))
      return false;

    QPolygonF poly;

    if (! stringToPolygon(polyStr, poly))
      return false;

    polygons.push_back(poly);

    parse.skipSpace();

    if (braced && parse.isChar('}')) {
      parse.skipChar();

      parse.skipSpace();

      break;
    }
  }

  return true;
}

bool stringToPolygon(const QString &str, QPolygonF &poly) {
  CQStrParse parse(str);

  return parsePolygon(parse, poly);
}

bool parsePolygon(CQStrParse &parse, QPolygonF &poly) {
  parse.skipSpace();

  int pos = parse.getPos();

  bool braced = false;

  if (parse.isChar('{')) {
    parse.skipChar();

    parse.skipSpace();

    if (parse.isChar('{')) {
      braced = true;
    }
    else
      parse.setPos(pos);
  }

  while (! parse.eof()) {
    parse.skipSpace();

    if (! parse.isChar('{'))
      return false;

    QString pointStr;

    if (! parse.readBracedString(pointStr))
      return false;

    QPointF point;

    if (! stringToPoint(pointStr, point))
      return false;

    poly.push_back(point);

    parse.skipSpace();

    if (braced && parse.isChar('}')) {
      parse.skipChar();

      parse.skipSpace();

      break;
    }
  }

  return true;
}

bool stringToRect(const QString &str, QRectF &rect) {
  CQStrParse parse(str);

  return parseRect(parse, rect);
}

bool parseRect(CQStrParse &parse, QRectF &rect) {
  // parse rect:
  //  x1 y1 x2 y2
  //  {x1 y1} {x2 y2}

  parse.skipSpace();

  if (parse.isChar('{')) {
    QString str1;

    if (! parse.readBracedString(str1))
      return false;

    CQStrParse parse1(str1);

    return parseRect(parse1, rect);
  }

  QPointF p1, p2;

  if (! parsePoint(parse, p1))
    return false;

  if (! parsePoint(parse, p2))
    return false;

  parse.skipSpace();

  // TODO: check for extra characters

  //---

  rect = QRectF(p1.x(), p1.y(), p2.x() - p1.x(), p2.y() - p1.y());

  return true;
}

bool stringToPoint(const QString &str, QPointF &point) {
  CQStrParse parse(str);

  return parsePoint(parse, point);
}

bool parsePoint(CQStrParse &parse, QPointF &point) {
  parse.skipSpace();

  if (parse.isChar('{')) {
    QString str1;

    if (! parse.readBracedString(str1))
      return false;

    CQStrParse parse1(str1);

    return parsePoint(parse1, point);
  }

  //---

  // read x y values
  double x = 0.0;

  if (! parse.readReal(&x))
    return false;

  parse.skipSpace();

  if (parse.isChar(',')) {
    parse.skipChar();

    parse.skipSpace();
  }

  double y = 0.0;

  if (! parse.readReal(&y))
    return false;

  // TODO: check for extra characters

  point = QPointF(x, y);

  return true;
}

}

//------

namespace CQChartsUtil {

QString pointToString(const QPointF &p) {
  return QString("%1 %2").arg(p.x()).arg(p.y());
}

QString rectToString(const QRectF &rect) {
  const QPointF &tl = rect.topLeft    ();
  const QPointF &br = rect.bottomRight();

  return QString("{%1 %2} {%3 %4}").arg(tl.x()).arg(tl.y()).arg(br.x()).arg(br.y());
}

QString polygonToString(const QPolygonF &poly) {
  int np = poly.size();

  QString str;

  for (int i = 0; i < np; ++i) {
    const QPointF &p = poly[i];

    str += QString("{%1}").arg(pointToString(p));
  }

  return str;
}

QString polygonListToString(const std::vector<QPolygonF> &polyList) {
  int np = polyList.size();

  QString str;

  for (int i = 0; i < np; ++i) {
    const QPolygonF &poly = polyList[i];

    str += QString("{%1}").arg(polygonToString(poly));
  }

  return str;
}

}

//------

namespace CQChartsUtil {

QString pathToString(const CQChartsPath &path) {
  return path.toString();
}

bool stringToPath(const QString &str, CQChartsPath &path) {
  path = CQChartsPath();

  if (! path.fromString(str))
    return false;

  return true;
}

}

//------

namespace CQChartsUtil {

QString styleToString(const CQChartsStyle &style) {
  return style.toString();
}

bool stringToStyle(const QString &str, CQChartsStyle &style) {
  style = CQChartsStyle();

  if (! style.fromString(str))
    return false;

  return true;
}

}

//------

namespace CQChartsUtil {

QString timeToString(const QString &fmt, double r) {
  static char buffer[512];

  time_t t(r);

  struct tm *tm1 = localtime(&t);

  if (! tm1)
    return "<no_time>";

  (void) strftime(buffer, 512, fmt.toLatin1().constData(), tm1);

  return buffer;
}

bool stringToTime(const QString &fmt, const QString &str, double &t) {
  struct tm tm1; memset(&tm1, 0, sizeof(tm));

  char *p = strptime(str.toLatin1().constData(), fmt.toLatin1().constData(), &tm1);

  if (! p)
    return false;

  t = mktime(&tm1);

  return true;
}

}

//------

namespace CQChartsUtil {

bool
formatStringInRect(const QString &str, const QFont &font, const QRectF &rect, QStringList &strs) {
  auto addStr = [&](const QString &str) {
    assert(str.length());
    strs.push_back(str);
  };

  //---

  QString sstr = str.simplified();

  if (! sstr.length()) { // empty
    addStr(sstr);
    return false;
  }

  //---

  QFontMetricsF fm(font);

  double w = fm.width(sstr);

  double dw = (rect.width() - w);

  if (dw > 0 || CMathUtil::isZero(dw)) { // fits
    addStr(sstr);
    return false;
  }

  double h = fm.height();

  double dh = (rect.height() - h);

  if (dh < 0 || CMathUtil::isZero(dh)) { // rect can only fit single line of text (TODO: factor)
    addStr(sstr);
    return false;
  }

  //---

  // get potential split points
  std::vector<int> splits;

  findStringSplits1(sstr, splits);

  if (splits.empty()) {
    findStringSplits2(sstr, splits);

    if (splits.empty())
      findStringSplits3(sstr, splits);

    if (splits.empty()) {
      addStr(sstr);
      return false;
    }
  }

  //---

  // get split closest to middle
  int target   = sstr.length()/2;
  int bestDist = target*2 + 1;
  int bestInd  = -1;

  for (std::size_t i = 0; i < splits.size(); ++i) {
    int dist = std::abs(splits[i] - target);

    if (bestInd < 0 || dist < bestDist) {
      bestDist = dist;
      bestInd  = i;
    }
  }

  if (bestInd < 0) {
    addStr(sstr);
    return false;
  }

  //---

  // split at best and measure
  int split = splits[bestInd];

  QString str1 = sstr.mid(0, split).simplified();
  QString str2 = sstr.mid(split   ).simplified();

  double w1 = fm.width(str1);
  double w2 = fm.width(str2);

  // both fit so we are done
  if (w1 <= rect.width() && w2 <= rect.width()) {
    addStr(str1);
    addStr(str2);

    return true;
  }

  //---

  // if one or both still wider then divide rect and refit
  if      (w1 > rect.width() && w2 > rect.width()) {
    double splitHeight = rect.height()/2.0;

    QRectF rect1(rect.left(), rect.top(), rect.width(), splitHeight);
    QRectF rect2(rect.left(), rect.top() + splitHeight, rect.width(), rect.height() - splitHeight);

    QStringList strs1, strs2;

    formatStringInRect(str1, font, rect1, strs1);
    formatStringInRect(str2, font, rect2, strs2);

    strs += strs1;
    strs += strs2;
  }
  else if (w1 > rect.width()) {
    double splitHeight = rect.height() - h;

    QRectF rect1(rect.left(), rect.top(), rect.width(), splitHeight);

    QStringList strs1;

    formatStringInRect(str1, font, rect1, strs1);

    strs += strs1;

    addStr(str2);
  }
  else {
    double splitHeight = rect.height() - h;

    QRectF rect2(rect.left(), rect.top() + h, rect.width(), splitHeight);

    QStringList strs2;

    formatStringInRect(str2, font, rect2, strs2);

    addStr(str1);

    strs += strs2;
  }

  return true;
}

}

//------

namespace CQChartsUtil {

void setPen(QPen &pen, bool stroked, const QColor &strokeColor, double strokeAlpha,
            double strokeWidth, const CQChartsLineDash &strokeDash) {
  double width = limitLineWidth(strokeWidth);

  // calc pen (stroke)
  if (stroked) {
    QColor color = strokeColor;

    color.setAlphaF(CMathUtil::clamp(strokeAlpha, 0.0, 1.0));

    pen.setColor(color);

    pen.setCosmetic(true);

    if (width > 0)
      pen.setWidthF(width);
    else
      pen.setWidthF(0.0);

    penSetLineDash(pen, strokeDash);
  }
  else {
    pen.setStyle(Qt::NoPen);
  }
}

void setBrush(QBrush &brush, bool filled, const QColor &fillColor, double fillAlpha,
              const CQChartsFillPattern &pattern) {
  // calc brush (fill)
  if (filled) {
    QColor color = fillColor;

    color.setAlphaF(CMathUtil::clamp(fillAlpha, 0.0, 1.0));

    brush.setColor(color);

    brush.setStyle(pattern.style());
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }
}

double limitLineWidth(double w) {
  // TODO: configuration setting
  return CMathUtil::clamp(w, 0.0, CQChartsLineWidth::maxPixelValue());
}

}

//------

namespace CQChartsUtil {

bool decodeUnits(const QString &str, CQChartsUnits &units, const CQChartsUnits &defUnits) {
  CQStrParse parse(str);

  parse.skipSpace();

  if      (parse.isString("px") || parse.isString("pixel"))
    units = CQChartsUnits::PIXEL;
  else if (parse.isString("%" ) || parse.isString("percent"))
    units = CQChartsUnits::PERCENT;
  else if (parse.isString("V" ) || parse.isString("view"))
    units = CQChartsUnits::VIEW;
  else if (parse.isString("P" ) || parse.isString("plot"))
    units = CQChartsUnits::PLOT;
  else if (parse.eof())
    units = defUnits;
  else
    return false;

  return true;
}

QStringList unitNames(bool includeNone) {
  static QStringList names      = QStringList() << "px" << "%" << "P" << "V";
  static QStringList none_names = QStringList() << "none" << names;

  return (includeNone ? none_names : names);
}

QStringList unitTipNames(bool includeNone) {
  static QStringList names      = QStringList() << "Pixel" << "Percent" << "Plot" << "View";
  static QStringList none_names = QStringList() << "None" << names;

  return (includeNone ? none_names : names);
}

}
