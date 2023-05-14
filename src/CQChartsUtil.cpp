#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsLineDash.h>
#include <CQChartsPath.h>
#include <CQChartsStyle.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsQuadTree.h>
#include <CQCharts.h>

#include <CQColors.h>
#include <CQColorsPalette.h>

#include <CQTclUtil.h>
#include <CQUtil.h>
#include <CQStrUtil.h>
#include <CQStrParse.h>
#include <CPrintF.h>
#include <CScanF.h>

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
  QStringList strs;

  for (std::size_t i = 0; i < columns.size(); ++i) {
    strs += columns[i].toString();
  }

  return CQTcl::mergeList(strs);
}

bool fromString(const QString &str, std::vector<CQChartsColumn> &columns) {
  bool ok = true;

  columns.clear();

  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  for (int i = 0; i < strs.size(); ++i) {
    bool ok1;

    long col = toInt(strs[i], ok1);

    if (ok1)
      columns.emplace_back(int(col));
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

    int   getInt     () const override { bool ok; return var_.toInt(&ok); }
    long  getLong    () const override { bool ok; return var_.toInt(&ok); }
    LLong getLongLong() const override { bool ok; return var_.toInt(&ok); }

    double getDouble() const override {
      bool ok;
      double r = var_.toDouble(&ok);
      if (CMathUtil::isZero(r)) r = 0.0;
      return r;
    }

    std::string getString() const override { return var_.toString().toStdString(); }

   private:
    QVariant var_;
  };

  VarPrintF p(var, fmt);

  return QString::fromStdString(p.format());
}

QString formatReal(double r, const QString &fmt) {
#ifdef ALLOW_NAN
  if (CMathUtil::isNaN(real))
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
    auto res = QString(buffer);

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
    auto rhs = res.mid(pos);

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

bool scanReal(const QString &fmt, const QString &str, double &r) {
  CScanF scanf(fmt.toStdString());

  if (! scanf.scan(str.toStdString()))
    return false;

  if (scanf.numValues() < 1)
    return false;

  const auto &v = scanf.value(0);

  if      (v.type() == CScanF::ValueType::REAL)
    r = v.real();
  else if (v.type() == CScanF::ValueType::INTEGER)
    r = double(v.integer());
  else
    return false;

  return true;
}

bool scanInteger(const QString &fmt, const QString &str, long &i) {
  CScanF scanf(fmt.toStdString());

  if (! scanf.scan(str.toStdString()))
    return false;

  if (scanf.numValues() < 1)
    return false;

  const auto &v = scanf.value(0);

  if      (v.type() == CScanF::ValueType::REAL)
    i = long(v.real());
  else if (v.type() == CScanF::ValueType::INTEGER)
    i = v.integer();
  else
    return false;

  return true;
}

}

//---

namespace CQChartsUtil {

bool intersectLines(const Point &l1s, const Point &l1e, const Point &l2s, const Point &l2e,
                    Point &pi, double &mu1, double &mu2) {
  double dx1 = l1e.x - l1s.x;
  double dy1 = l1e.y - l1s.y;
  double dx2 = l2e.x - l2s.x;
  double dy2 = l2e.y - l2s.y;

  double delta = dx1*dy2 - dy1*dx2;

  if (fabs(delta) < 1E-15) // parallel
    return false;

  double idelta = 1.0/delta;

  double dx = l2s.x - l1s.x;
  double dy = l2s.y - l1s.y;

  mu1 = (dx*dy2 - dy*dx2)*idelta;
  mu2 = (dx*dy1 - dy*dx1)*idelta;

  double xi = l1s.x + mu1*dx1;
  double yi = l1s.y + mu1*dy1;

  pi = Point(xi, yi);

  return true;
}

bool intersectLines(double x11, double y11, double x21, double y21,
                    double x12, double y12, double x22, double y22,
                    double &xi, double &yi, double &mu1, double &mu2) {
  Point pi;

  bool rc = intersectLines(Point(x11, y11), Point(x21, y21),
                           Point(x12, y12), Point(x22, y22), pi, mu1, mu2);

  xi = pi.x;
  yi = pi.y;

  return rc;
}

}

//---

namespace CQChartsUtil {

// get gray value of color (integer 0-255) (ignoring alpha)
int grayValue(const QColor &c) {
  return qGray(c.red(), c.green(), c.blue());
}

// get gray color of color (QColor) (ignoring alpha)
QColor grayColor(const QColor &c) {
  int g = grayValue(c);

  return QColor(g, g, g);
}

// get black or white color nearest to color (ignoring alpha)
QColor bwColor(const QColor &c) {
  int g = qGray(c.red(), c.green(), c.blue());

  return (g > 128 ? QColor(Qt::black) : QColor(Qt::white));
}

// get inverted color (r,g,b only)
QColor invColor(const QColor &c) {
  return QColor(255 - c.red(), 255 - c.green(), 255 - c.blue(), c.alpha());
}

// blend two colors using specified alpha as factor
QColor blendColors(const QColor &c1, const QColor &c2, const CQChartsAlpha &a) {
  return blendColors(c1, c2, a.value());
}

// blend two colors using specified factor (0-1) (first color is f, second is 1-f)
QColor blendColors(const QColor &c1, const QColor &c2, double f) {
  double f1 = 1.0 - f;

  double r = c1.redF  ()*f + c2.redF  ()*f1;
  double g = c1.greenF()*f + c2.greenF()*f1;
  double b = c1.blueF ()*f + c2.blueF ()*f1;
  double a = c1.alphaF()*f + c2.alphaF()*f1;

  return rgbaToColor(r, g, b, a);
}

// blend list of colors (no alpha), factor is evenly spread
QColor blendColors(const std::vector<QColor> &colors) {
  if (colors.empty())
    return QColor();

  double f = 1.0/double(colors.size());

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

// QColor from r, g, b (0-1)
QColor rgbToColor(double r, double g, double b) {
  return QColor(CMathUtil::clamp(int(255*r), 0, 255),
                CMathUtil::clamp(int(255*g), 0, 255),
                CMathUtil::clamp(int(255*b), 0, 255));
}

// QColor from r, g, b, a (0-1)
QColor rgbaToColor(double r, double g, double b, double a) {
  return QColor(CMathUtil::clamp(int(255*r), 0, 255),
                CMathUtil::clamp(int(255*g), 0, 255),
                CMathUtil::clamp(int(255*b), 0, 255),
                CMathUtil::clamp(int(255*a), 0, 255));
}

// QColor from string (HTML color names)
QColor stringToColor(const QString &str) {
  // TODO: X11 colors ?
  return QColor(str);
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

    //double w = pen.widthF();
    //if (w <= 0.0) w = 1.0;

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
double PointPointDistance(const Point &point1, const Point &point2) {
  double dx = point1.x - point2.x;
  double dy = point1.y - point2.y;

  return std::hypot(dx, dy);
}

// distance between point and line
bool PointLineDistance(const Point &point, const Point &lineStart, const Point &lineEnd,
                       double *dist) {
  double dx1 = lineEnd.x - lineStart.x;
  double dy1 = lineEnd.y - lineStart.y;

  double u2 = dx1*dx1 + dy1*dy1;

  if (u2 <= 0.0) {
    *dist = PointPointDistance(point, lineStart);
    return false;
  }

  double dx2 = point.x - lineStart.x;
  double dy2 = point.y - lineStart.y;

  double u1 = dx2*dx1 + dy2*dy1;

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
    auto intersection = lineStart + u*Point(dx1, dy1);

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
    line.clear();

    if (feof(fp)) return false;

    int c = fgetc(fp);

    if (c == EOF)
      return false;

    while (! feof(fp) && c != '\n') {
      line += char(c);

      c = fgetc(fp);
    }

    return true;
  };

  auto close = [](FILE* &fp) {
    if (fp)
      fclose(fp);

    fp = nullptr;
  };

  //---

  FILE *fp = nullptr;

  if (! open(fp, filename))
    return false;

  QString line;

  while (readLine(fp, line)) {
    lines.push_back(std::move(line));

    if (maxLines >= 0 && int(lines.size()) > maxLines)
      break;
  }

  close(fp);

  //---

  return true;
}

}

//------

namespace CQChartsUtil {

// split at spaces
void findStringSpaceSplits(const QString &str, std::vector<int> &splits) {
  int len = str.length();
  assert(len);

  for (int i = 0; i < len; ++i) {
    if (str[i] == '\n') {
      splits.push_back(i);
      continue;
    }

    if (i > 0 && str[i].isSpace()) {
      // skip consecutive spaces
      while (i < len - 1 && str[i + 1].isSpace())
        ++i;

      int i1 = i + 1;

      if (i1 >= len - 1) // don't break if at start or end
        continue;

      splits.push_back(i1);
    }
  }
}

// split at format separators
void findStringCustomSplits(const QString &str, std::vector<int> &splits,
                            const FormatData &formatData) {
  int len = str.length();
  assert(len);

  for (int i = 0; i < len; ++i) {
    if (str[i] == '\n') {
      splits.push_back(i);
      continue;
    }

    if (i > 0 && formatData.isSep(str[i])) {
      // keep consecutive separators together
      while (i < len - 1 && formatData.isSep(str[i + 1]))
        ++i;

      int i1 = i + 1;

      if (i1 >= len - 1) // don't break if at start or end
        continue;

      splits.push_back(i1);
    }
  }
}

// split at punctuation
void findStringPunctSplits(const QString &str, std::vector<int> &splits) {
  int len = str.length();
  assert(len);

  for (int i = 0; i < len; ++i) {
    if (str[i] == '\n') {
      splits.push_back(i);
      continue;
    }

    bool split = false;

    if (i > 0 && i < len - 1 && str[i].isPunct()) {
      split = true;

      // don't split at '.' in number
      if (str[i] == '.' && str[i - 1].isNumber() && str[i + 1].isNumber())
        split = false;
    }

    if (split) {
      // keep consecutive punctuation together (::, ..., etc)
      while (i < len - 1 && str[i + 1].isPunct())
        ++i;

      int i1 = i + 1;

      if (i1 >= len - 1) // don't break if at start or end
        continue;

      splits.push_back(i1);
    }
  }
}

// lower to upper
void findStringCaseSplits(const QString &str, std::vector<int> &splits) {
  int len = str.length();
  assert(len);

  for (int i = 0; i < len; ++i) {
    if (str[i] == '\n') {
      splits.push_back(i);
      continue;
    }

    if (i > 0) {
      const auto &c1 = str[i - 1];
      const auto &c2 = str[i];

      if (c1.isLower() && c2.isUpper())
        splits.push_back(i);
    }
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

  return countLeadingBraces(parse);
}

int countLeadingBraces(CQStrParse &parse) {
  int pos = parse.getPos();

  int n = 0;

  while (! parse.eof()) {
    parse.skipSpace();

    if (! parse.isChar('{'))
      break;

    parse.skipChar();

    ++n;
  }

  parse.setPos(pos);

  return n;
}

//---

QString realToString(double r, int ndp) {
  if (ndp < 0)
    return QString::number(r);
  else
    return QString("%1").arg(r, 0, 'f', ndp);
}

//---

bool stringToPolygons(const QString &str, std::vector<Polygon> &polygons) {
  CQStrParse parse(str);

  //---

  // count if brace around list of polygons (max 3 braces but allow more, min 2 braces)
  int nbraces = countLeadingBraces(parse);

  if      (nbraces > 3) {
    QString polyStr;

    if (! parse.readBracedString(polyStr))
      return false;

    return stringToPolygons(polyStr, polygons);
  }

  //---

  if (nbraces == 3) {
    int pos = parse.getPos();

    if (parsePolygons(parse, polygons))
      return true;

    parse.setPos(pos);

    QString polyStr;

    if (! parse.readBracedString(polyStr))
      return false;

    return stringToPolygons(polyStr, polygons);
  }
  else {
    return parsePolygons(parse, polygons);
  }
}

bool parsePolygons(CQStrParse &parse, std::vector<Polygon> &polygons) {
  while (! parse.eof()) {
    parse.skipSpace();

    if (! parse.isChar('{'))
      return false;

    QString polyStr;

    if (! parse.readBracedString(polyStr))
      return false;

    Polygon poly;

    if (! stringToPolygon(polyStr, poly))
      return false;

    polygons.push_back(std::move(poly));

    parse.skipSpace();
  }

  return true;
}

QString polygonListToString(const std::vector<Polygon> &polyList) {
  auto np = polyList.size();

  QStringList strs;

  for (size_t i = 0; i < np; ++i) {
    const auto &poly = polyList[i];

    strs += polygonToString(poly);
  }

  return CQTcl::mergeList(strs);
}

//---

bool stringToPolygon(const QString &str, Polygon &poly) {
  CQStrParse parse(str);

  //---

  // count if brace around polygons (max 2 braces but allow more, min 1 brace)
  int nbraces = countLeadingBraces(parse);

  if      (nbraces > 2) {
    QString polyStr;

    if (! parse.readBracedString(polyStr))
      return false;

    return stringToPolygon(polyStr, poly);
  }

  //---

  if (nbraces == 2) {
    int pos = parse.getPos();

    if (parsePolygon(parse, poly))
      return true;

    parse.setPos(pos);

    QString polyStr;

    if (! parse.readBracedString(polyStr))
      return false;

    return stringToPolygon(polyStr, poly);
  }
  else {
    return parsePolygon(parse, poly);
  }
}

bool parsePolygon(CQStrParse &parse, Polygon &poly) {
  while (! parse.eof()) {
    parse.skipSpace();

    if (! parse.isChar('{'))
      return false;

    QString pointStr;

    if (! parse.readBracedString(pointStr))
      return false;

    Point point;

    if (! stringToPoint(pointStr, point))
      return false;

    poly.addPoint(point);

    parse.skipSpace();
  }

  return true;
}

QString polygonToString(const Polygon &poly) {
  int np = poly.size();

  QStringList strs;

  for (int i = 0; i < np; ++i) {
    auto p = poly.point(i);

    strs += pointToString(p);
  }

  return CQTcl::mergeList(strs);
}

//---

bool stringToBBox(const QString &str, BBox &bbox) {
  CQStrParse parse(str);

  return parseBBox(parse, bbox, /*terminated*/ true);
}

bool parseBBox(CQStrParse &parse, BBox &bbox, bool terminated) {
  // parse bbox:
  //  x1 y1 x2 y2
  //  { x1 y1 x2 y2 }
  //  {x1 y1} {x2 y2}

  parse.skipSpace();

  // { x1 y1 x2 y2 }
  if (parse.isChar('{')) {
    QString str1;

    int pos = parse.getPos();

    if (! parse.readBracedString(str1))
      return false;

    CQStrParse parse1(str1);

    if (parseBBox(parse1, bbox, terminated)) {
      if (terminated) {
        parse.skipSpace();

        if (parse.eof())
          return true;
      }
      else
        return true;
    }

    parse.setPos(pos);
  }

  //--

  // { x1 y1 } { x2 y2 }
  Point p1, p2;

  if (! parsePoint(parse, p1, /*terminated*/ false))
    return false;

  if (! parsePoint(parse, p2, terminated))
    return false;

  //---

  bbox = BBox(p1.x, p1.y, p2.x, p2.y);

  return true;
}

QString bboxToString(const BBox &bbox) {
  if (bbox.isSet()) {
    QStringList strs;

    const auto &pmin = bbox.getMin();
    const auto &pmax = bbox.getMax();

    strs += pointToString(pmin);
    strs += pointToString(pmax);

    return CQTcl::mergeList(strs);
  }
  else {
    return "";
  }
}

//---

bool stringToSize(const QString &str, Size &size) {
  CQStrParse parse(str);

  return parseSize(parse, size, /*terminated*/ true);
}

bool parseSize(CQStrParse &parse, Size &size, bool terminated) {
  // parse bbox:
  //  w h
  //  { w h }

  parse.skipSpace();

  // { w h }
  if (parse.isChar('{')) {
    QString str1;

    if (! parse.readBracedString(str1))
      return false;

    CQStrParse parse1(str1);

    if (parseSize(parse1, size, terminated)) {
      if (terminated) {
        parse.skipSpace();

        if (parse.eof())
          return true;
      }
      else
        return true;
    }
  }

  //--

  // read width and height values
  double w = 0.0;

  if (! parse.readReal(&w))
    return false;

  parse.skipSpace();

  if (parse.isChar(',')) {
    parse.skipChar();

    parse.skipSpace();
  }

  double h = 0.0;

  if (! parse.readReal(&h))
    return false;

  if (terminated) {
    parse.skipSpace();

    if (! parse.eof())
      return false;
  }

  //---

  // return size
  size = Size(w, h);

  return true;
}

//---

bool stringToPoint(const QString &str, Point &point) {
  CQStrParse parse(str);

  return parsePoint(parse, point, /*terminated*/ true);
}

bool parsePoint(CQStrParse &parse, Point &point, bool terminated) {
  // parse point:
  //  x y
  //  { x y }

  parse.skipSpace();

  // { x y }
  if (parse.isChar('{')) {
    QString str1;

    if (! parse.readBracedString(str1))
      return false;

    CQStrParse parse1(str1);

    if (parsePoint(parse1, point, terminated)) {
      if (terminated) {
        parse.skipSpace();

        if (parse.eof())
          return true;
      }
      else
        return true;
    }
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

  if (terminated) {
    parse.skipSpace();

    if (! parse.eof())
      return false;
  }

  //---

  // return point
  point = Point(x, y);

  return true;
}

QString pointToString(const Point &p) {
  QStringList strs;

  strs += QString::number(p.x);
  strs += QString::number(p.y);

  return CQTcl::mergeList(strs);
}

//---

bool stringToPoint3D(const QString &str, Point3D &point) {
  CQStrParse parse(str);

  return parsePoint3D(parse, point, /*terminated*/ true);
}

bool parsePoint3D(CQStrParse &parse, Point3D &point, bool terminated) {
  // parse point:
  //  x y z
  //  { x y z }

  parse.skipSpace();

  // { x y z }
  if (parse.isChar('{')) {
    QString str1;

    if (! parse.readBracedString(str1))
      return false;

    CQStrParse parse1(str1);

    if (parsePoint3D(parse1, point, terminated)) {
      if (terminated) {
        parse.skipSpace();

        if (parse.eof())
          return true;
      }
      else
        return true;
    }
  }

  //---

  // read x y z values
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

  parse.skipSpace();

  if (parse.isChar(',')) {
    parse.skipChar();

    parse.skipSpace();
  }

  double z = 0.0;

  if (! parse.readReal(&z))
    return false;

  if (terminated) {
    parse.skipSpace();

    if (! parse.eof())
      return false;
  }

  //---

  // return point
  point = Point3D(x, y, z);

  return true;
}

QString point3DToString(const Point3D &p) {
  QStringList strs;

  strs += QString::number(p.x);
  strs += QString::number(p.y);
  strs += QString::number(p.z);

  return CQTcl::mergeList(strs);
}

}

//------

namespace CQChartsUtil {

QString sizeToString(const Size &s) {
  QStringList strs;

  strs += QString::number(s.width ());
  strs += QString::number(s.height());

  return CQTcl::mergeList(strs);
}

}

//------

namespace CQChartsUtil {

QString pathToString(const CQChartsPath &path) {
  return path.toString();
}

bool stringToPath(const QString &str, CQChartsPath &path) {
  path = CQChartsPath();

  return path.fromString(str);
}

}

//------

namespace CQChartsUtil {

QString styleToString(const CQChartsStyle &style) {
  return style.toString();
}

bool stringToStyle(const QString &str, CQChartsStyle &style) {
  style = CQChartsStyle();

  return style.fromString(str);
}

}

//------

namespace CQChartsUtil {

QString timeToString(const QString &fmt, double r) {
  static char buffer[512];

  auto t = time_t(r);

  auto *tm1 = localtime(&t);
  if (! tm1) return "<no_time>";

  (void) strftime(buffer, 512, fmt.toLatin1().constData(), tm1);

  return buffer;
}

bool stringToTime(const QString &fmt, const QString &str, double &t) {
  struct tm tm1; memset(&tm1, 0, sizeof(tm));

  char *p = strptime(str.toLatin1().constData(), fmt.toLatin1().constData(), &tm1);
  if (! p) return false;

  t = double(mktime(&tm1));

  return true;
}

}

//------

namespace CQChartsUtil {

bool
formatStringInRect(const QString &str, const QFont &font, const BBox &bbox,
                   QStringList &strs, const FormatData &formatData) {
  auto addStr = [&](const QString &str) {
    //assert(str.length());
    strs.push_back(str);
  };

  auto formatData1 = formatData;

  //---

  auto sstr = str.trimmed();

  if (! sstr.length()) { // empty
    addStr(sstr);
    return false;
  }

  //---

  // number of newlines
  int nl = str.count('\n');

  //---

  QFontMetricsF fm(font);

  double h = fm.height();

  if (nl == 0) {
    double w  = fm.horizontalAdvance(sstr);
    double dw = (bbox.getWidth() - w);

    if (dw > 0 || CMathUtil::isZero(dw)) { // fits
      addStr(sstr);
      return false;
    }

    double dh = (bbox.getHeight() - h);

    // bbox can only fit single line of text (TODO: factor)
    if (dh < 0 || CMathUtil::isZero(dh)) {
      if (! formatData1.continued) {
        addStr(sstr);
        return false;
      }
    }
  }

  //---

  // get potential split points
  std::vector<int> splits;

  if (formatData1.isValid())
    findStringCustomSplits(sstr, splits, formatData1);

  if (splits.size() < size_t(nl + 1))
    findStringSpaceSplits(sstr, splits);

  if (splits.size() < size_t(nl + 1))
    findStringPunctSplits(sstr, splits);

  if (splits.size() < size_t(nl + 1))
    findStringCaseSplits(sstr, splits);

  if (splits.empty()) {
    addStr(sstr);
    return false;
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
      bestInd  = int(i);
    }
  }

  if (bestInd < 0) {
    addStr(sstr);
    return false;
  }

  //---

  // split at best and measure
  int split = splits[size_t(bestInd)];

  auto str1 = sstr.mid(0, split).trimmed();

  QString str2;

  if (sstr[split] == '\n')
    str2 = sstr.mid(split + 1).trimmed();
  else
    str2 = sstr.mid(split).trimmed();

  int nl1 = str1.count('\n');
  int nl2 = str2.count('\n');

  bool fit1 = false;
  bool fit2 = false;

  if (nl1 == 0) {
    double w1 = fm.horizontalAdvance(str1);

    fit1 = (w1 <= bbox.getWidth());
  }

  if (nl2 == 0) {
    double w2 = fm.horizontalAdvance(str2);

    fit2 = (w2 <= bbox.getWidth());
  }

  // both fit so we are done
  if (fit1 && fit2) {
    addStr(str1);
    addStr(str2);

    return true;
  }

  //---

  // if one or both still wider then divide bbox and refit
  if      (! fit1 && ! fit2) {
    double splitHeight = bbox.getHeight()/2.0;

    BBox bbox1(bbox.getXMin(), bbox.getYMin(),
               bbox.getXMax(), bbox.getYMin() + splitHeight);
    BBox bbox2(bbox.getXMin(), bbox.getYMin() + splitHeight,
               bbox.getXMax(), bbox.getYMax());

    QStringList strs1, strs2;

    formatData1.continued = true;

    formatStringInRect(str1, font, bbox1, strs1, formatData1);
    formatStringInRect(str2, font, bbox2, strs2, formatData1);

    strs += strs1;
    strs += strs2;
  }
  else if (! fit1) {
    double splitHeight = bbox.getHeight() - h;

    BBox bbox1(bbox.getXMin(), bbox.getYMin(),
               bbox.getXMax(), bbox.getYMin() + splitHeight);

    QStringList strs1;

    formatData1.continued = true;

    formatStringInRect(str1, font, bbox1, strs1, formatData1);

    strs += strs1;

    addStr(str2);
  }
  else {
    BBox bbox2(bbox.getXMin(), bbox.getYMin() + h, bbox.getXMax(), bbox.getYMax());

    QStringList strs2;

    formatData1.continued = true;

    formatStringInRect(str2, font, bbox2, strs2, formatData1);

    addStr(str1);

    strs += strs2;
  }

  return true;
}

}

//------

namespace CQChartsUtil {

void setPen(QPen &pen, bool stroked, const QColor &strokeColor, const Alpha &strokeAlpha,
            double strokeWidth, const LineDash &strokeDash, const LineCap &lineCap,
            const LineJoin &lineJoin) {
  //strokeWidth = limitLineWidth(strokeWidth);

  // calc pen (stroke)
  if (stroked) {
    auto color = strokeColor;

    pen.setColor(CQChartsDrawUtil::setColorAlpha(color, strokeAlpha));

    pen.setCosmetic(true);

    if (strokeWidth > 0)
      pen.setWidthF(strokeWidth);
    else
      pen.setWidthF(0.0);

    penSetLineDash(pen, strokeDash);

    pen.setCapStyle (lineCap .cap ());
    pen.setJoinStyle(lineJoin.join());
  }
  else {
    pen.setStyle(Qt::NoPen);
  }
}

void setBrush(QBrush &brush, bool filled, const QColor &fillColor, const Alpha &fillAlpha,
              const FillPattern &pattern) {
  auto color = fillColor;

  CQChartsDrawUtil::setColorAlpha(color, fillAlpha);

  // calc brush (fill)
  if (filled) {
    // Linear gradient
    if      (pattern.type() == FillPattern::Type::LGRADIENT) {
      double c = pattern.angle().cos();
      double s = pattern.angle().sin();

      QLinearGradient lg(std::abs(std::min(0.0, c)), std::abs(std::min(0.0, s)),
                         std::abs(std::max(0.0, c)), std::abs(std::max(0.0, s)));

      lg.setCoordinateMode(QGradient::ObjectBoundingMode);

      lg.setColorAt(0, color);

      if (pattern.altColor().isValid()) {
        auto altColor = pattern.altColor().color();

        CQChartsDrawUtil::setColorAlpha(altColor, pattern.altAlpha());

        lg.setColorAt(1, altColor);
      }
      else
        lg.setColorAt(1, Qt::white);

      brush = QBrush(lg);
    }
    // Radial gradient
    else if (pattern.type() == FillPattern::Type::RGRADIENT) {
      double c = pattern.angle().cos();
      double s = pattern.angle().sin();

      QPointF center(0.5, 0.5);
      QPointF focus (c, s);

      double radius = 0.5;

      QRadialGradient rg(center, radius, focus);

      rg.setCoordinateMode(QGradient::ObjectBoundingMode);

      rg.setColorAt(0, color);

      if (pattern.altColor().isValid()) {
        auto altColor = pattern.altColor().color();

        CQChartsDrawUtil::setColorAlpha(altColor, pattern.altAlpha());

        rg.setColorAt(1, altColor);
      }
      else
        rg.setColorAt(1, Qt::white);

      brush = QBrush(rg);
    }
    // Palette
    else if (pattern.type() == FillPattern::Type::PALETTE) {
      auto *palette = pattern.palette().palette();
      if (! palette) return;

      auto a1 = -pattern.angle();

      // (0, 0) Top Left -> (1, 1) Bottom Right

      // cos/sin from (-1, -1, 1, 1)
      double x1 = 0.0;
      double y1 = 0.0;
      double x2 = a1.cos();
      double y2 = a1.sin();

      if (x2 < 0.0) { x1 += 1.0; x2 += 1.0; }
      if (y2 < 0.0) { y1 += 1.0; y2 += 1.0; }

      QLinearGradient lg(x1, y1, x2, y2);

      lg.setCoordinateMode(QGradient::ObjectBoundingMode);

      // store palette in linear gradient
      palette->setLinearGradient(lg, 1.0, pattern.palette().min(), pattern.palette().max());

      brush = QBrush(lg);
    }
    // Image
    else if (pattern.type() == FillPattern::Type::IMAGE ||
             pattern.type() == FillPattern::Type::TEXTURE ||
             pattern.type() == FillPattern::Type::MASK) {
      brush.setColor(color);

      auto image = pattern.image().image();

      image.setText("altColor", pattern.altColor().toString());
      image.setText("altAlpha", pattern.altAlpha().toString());

      brush.setTextureImage(image);
    }
    // Custom Patterns
    else if (int(pattern.type()) >= int(FillPattern::Type::PATTERN_HATCH) &&
             int(pattern.type()) <= int(FillPattern::Type::PATTERN_SQUARES)) {
      brush.setColor(color);
    }
    // SOLID, HATCH, DENSE, HORIZ, VERT, FDIAG, BDIAG
    else {
      brush.setStyle(pattern.style());
      brush.setColor(color);
    }

    if (pattern.scale() != 1.0)
      brush.setTransform(QTransform::fromScale(pattern.scale(), pattern.scale()));
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }
}

#if 0
double limitLineWidth(double w) {
  // TODO: configuration setting
  return CMathUtil::clamp(w, 0.0, CQChartsLineWidth::maxPixelValue());
}
#endif

}

//------

namespace CQChartsUtil {

QFont scaleFontSize(const QFont &font, double s, double minSize, double maxSize) {
  double fs = font.pointSizeF()*s;

  fs = CMathUtil::clamp(fs, minSize, maxSize);

  auto font1 = font;

  if (fs > 0.0)
    font1.setPointSizeF(fs);

  return font1;
}

}

//------

namespace CQChartsUtil {

Point nearestRectPoint(const BBox &rect, const Point &pos, double &angle, bool useCorners) {
  PointList pointList;

  pointList.resize(useCorners ? 8 : 4);

  size_t np = 0;

  if (useCorners) {
    pointList[np++] = Point(rect.getXMin(), rect.getYMin()); // bottom left
    pointList[np++] = Point(rect.getXMin(), rect.getYMax()); // top left
    pointList[np++] = Point(rect.getXMax(), rect.getYMin()); // bottom right
    pointList[np++] = Point(rect.getXMax(), rect.getYMax()); // top right
  }

  pointList[np++] = Point(rect.getXMin(), rect.getYMid()); // left mid
  pointList[np++] = Point(rect.getXMid(), rect.getYMin()); // bottom mid
  pointList[np++] = Point(rect.getXMid(), rect.getYMax()); // top mid
  pointList[np++] = Point(rect.getXMax(), rect.getYMid()); // right mid

//pointList[np++] = Point(rect.getXMid(), rect.getYMid());

  int i;

  auto p = nearestPointListPoint(pointList, pos, i);

  int i1 = i;

  if (useCorners)
    i1 -= 4;

  if      (i1 == 0) angle = 180;
  else if (i1 == 1) angle = -90;
  else if (i1 == 2) angle = 90;
  else if (i1 == 3) angle = 0;

  else if (i  == 0) angle = -135;
  else if (i  == 1) angle = 135;
  else if (i  == 2) angle = -45;
  else if (i  == 3) angle = 45;

  else assert(false);

  return p;
}

Point nearestPointListPoint(const PointList &points, const Point &pos, int &i) {
  auto pointPointDist = [](const Point &p1, const Point &p2) {
    return std::hypot(p1.x - p2.x, p1.y - p2.y);
  };

  auto np = points.size();
  assert(np > 0);

  int  i1 = -1;
  auto d  = pointPointDist(points[0], pos);

  for (size_t ip = 0; ip < np; ++ip) {
    auto d1 = pointPointDist(points[ip], pos);

    if (i1 < 0 || d1 < d) {
      i1 = int(ip);
      d  = d1;
    }
  }

  i = i1;

  return points[size_t(i1)];
}

}

//------

namespace CQChartsUtil {

bool encodeUtf(const QString &s, QString &res) {
  auto hexCharValue = [](char c) {
    if (isdigit(c)) return (c - '0');

    return (tolower(c) - 'a' + 10);
  };

  std::wstring str;

  int i   = 0;
  int len = s.length();

  while (i < len) {
    if (i < len - 5 && s[i] == '\\' && s[i + 1].toLower() == 'u') {
      i += 2;

      long u = 0;

      for (int i1 = 0; i1 < 4; ++i1) {
        auto c = s[i + i1].toLatin1();

        if (! isxdigit(c))
          return false;

        u = (u << 4) | (hexCharValue(c) & 0xF);
      }

      str += wchar_t(u);

      i += 4;
    }
    else {
      str += wchar_t(s[i].toLatin1());

      ++i;
    }
  }

  res = QString::fromStdWString(str);

  return true;
}

}

//------

namespace CQChartsUtil {

QImage disabledImage(const QImage &image, const QColor &bg, double f) {
  int iw = image.width ();
  int ih = image.height();

  auto image1 = initImage(QSize(iw, ih));

  for (int y = 0; y < ih; ++y) {
    for (int x = 0; x < iw; ++x) {
      auto rgba = image.pixel(x, y);

      int r = qRed  (rgba);
      int g = qGreen(rgba);
      int b = qBlue (rgba);
      int a = qAlpha(rgba);

      QColor c(r, g, b, a);

      auto c1 = blendColors(bg, c, f);

    //c1.setAlpha(a);

      image1.setPixel(x, y, c1.rgba());
    }
  }

  return image1;
}

}

//------

namespace CQChartsUtil {

bool checkOverlaps(const std::vector<BBox> &rects)
{
  if (rects.size() <= 1)
    return false;

  class RectData {
   public:
    RectData(const BBox &rect) :
     rect_(rect) {
    }

    BBox rect() const { return rect_; }

   private:
    BBox rect_;
  };

  using QuadTree  = CQChartsQuadTree<RectData, BBox>;
  using RectDatas = std::vector<RectData>;

  BBox      bbox;
  RectDatas rectDatas;

  for (auto &r : rects) {
    bbox += r;

    rectDatas.emplace_back(r);
  }

  QuadTree quad(bbox);

  for (auto &r : rectDatas) {
    if (quad.isDataTouchingRect(r.rect()))
      return true;

    quad.add(&r);
  }

  return false;
}

bool adjustRectsToOriginal(const std::vector<BBox> &oldRects, std::vector<BBox> &newRects)
{
  assert(oldRects.size() == newRects.size());

  bool changed = true;

  while (changed) {
    changed = false;

    uint i = 0;

    for (auto &r : newRects) {
      std::cerr << "Adjust rect: " << i << "\n";

      assert(r.isSet());

      // build list of sorted unique yvals (for intervals);
      uint j = 0;

      std::set<double> yvals;

      for (auto &r : newRects) {
        if (i != j) {
          auto ymin = r.getYMin();
          auto ymax = r.getYMax();

          yvals.insert(ymin);
          yvals.insert(ymax);
        }

        ++j;
      }

      // create y range intervals (each pair of unqiue yvals)
      auto ny = yvals.size();

      struct YRange {
        double        ymin { 0.0 };
        double        ymax { 0.0 };
        double        h    { 0.0 };
        std::set<int> used;
      };

      using YRanges = std::vector<YRange>;

      YRanges yranges;

      yranges.resize(ny - 1);

      j = 0;

      for (const auto &y : yvals) {
        if      (j > 0) {
          yranges[j - 1].ymax = y;
          yranges[j - 1].h    = y - yranges[j - 1].ymin;
        }
        else if (j < ny - 1) {
          yranges[j].ymin = y;
          yranges[j].ymax = y;
        }

        ++j;
      }

      //---

      // set inside for ranges
      j = 0;

      for (auto &r : newRects) {
        if (i != j) {
          auto ymin = r.getYMin();
          auto ymax = r.getYMax();

          for (auto &yrange : yranges) {
            // check if range center inside rect
            auto ym = (yrange.ymin + yrange.ymax)/2.0;

            if (ym > ymin && ym < ymax)
              yrange.used.insert(j);
          }
        }

        ++j;
      }

      //---

      for (auto &yrange : yranges) {
        std::cerr << " " << yrange.ymin << " " << yrange.ymax << " " << yrange.used.size() << "\n";
      }

      //---

      // get ideal pos
      auto y2 = newRects[i].getYMid();
      auto h2 = newRects[i].getHeight();

      auto   y1 = oldRects[i].getYMid();
      double dy = y1 - y2;

      bool foundGap = false;

      std::cerr << "  Check gap " << dy << "\n";

      // check ranges for fit
      for (auto &yrange : yranges) {
        if (! yrange.used.empty())
          continue;

        if (y2 < yrange.ymin || y2 > yrange.ymax || h2 > yrange.h)
          continue;

        auto ym = (yrange.ymin + yrange.ymax)/2.0;

        double y3;

        if (y1 >= ym)
          y3 = yrange.ymax - h2/2.0;
        else
          y3 = yrange.ymin + h2/2.0;

        double dy1 = (y1 - y3);

        if (abs(dy1) < abs(dy))
          dy = dy1;

        foundGap = true;

        std::cerr << "  Found gap " << dy1 << "\n";
      }

      if (foundGap) {
        newRects[i].moveBy(Point(0, dy));

        changed = false;
      }

      //---

      ++i;
    }
  }

  return true;
}

}
