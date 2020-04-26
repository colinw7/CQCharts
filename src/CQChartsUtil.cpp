#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsLineDash.h>
#include <CQChartsPath.h>
#include <CQChartsStyle.h>
#include <CQCharts.h>

#include <CQTclUtil.h>
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
      columns.push_back(CQChartsColumn(int(col)));
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

bool intersectLines(const CQChartsGeom::Point &l1s, const CQChartsGeom::Point &l1e,
                    const CQChartsGeom::Point &l2s, const CQChartsGeom::Point &l2e,
                    CQChartsGeom::Point &pi) {
  double dx1 = l1e.x - l1s.x;
  double dy1 = l1e.y - l1s.y;
  double dx2 = l2e.x - l2s.x;
  double dy2 = l2e.y - l2s.y;

  double delta = dx1*dy2 - dy1*dx2;

  if (fabs(delta) < 1E-6) // parallel
    return false;

  double idelta = 1.0/delta;

  double dx = l2s.x - l1s.x;
  double dy = l2s.y - l1s.y;

  double m1 = (dx*dy2 - dy*dx2)*idelta;
//double m2 = (dx*dy1 - dy*dx1)*idelta;

  double xi = l1s.x + m1*dx1;
  double yi = l1s.y + m1*dy1;

  pi = CQChartsGeom::Point(xi, yi);

  return true;
}

bool intersectLines(double x11, double y11, double x21, double y21,
                    double x12, double y12, double x22, double y22,
                    double &xi, double &yi) {
  CQChartsGeom::Point pi;

  bool rc = intersectLines(CQChartsGeom::Point(x11, y11), CQChartsGeom::Point(x21, y21),
                           CQChartsGeom::Point(x12, y12), CQChartsGeom::Point(x22, y22), pi);

  xi = pi.x;
  yi = pi.y;

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

QColor blendColors(const QColor &c1, const QColor &c2, const CQChartsAlpha &a) {
  return blendColors(c1, c2, a.value());
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
    auto intersection = lineStart + u*CQChartsGeom::Point(dx1, dy1);

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
      line += char(c);

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

    if (i > 0 && str[i].isPunct()) {
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
      const QChar &c1 = str[i - 1];
      const QChar &c2 = str[i];

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

bool stringToPolygons(const QString &str, std::vector<CQChartsGeom::Polygon> &polygons) {
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

bool parsePolygons(CQStrParse &parse, std::vector<CQChartsGeom::Polygon> &polygons) {
  while (! parse.eof()) {
    parse.skipSpace();

    if (! parse.isChar('{'))
      return false;

    QString polyStr;

    if (! parse.readBracedString(polyStr))
      return false;

    CQChartsGeom::Polygon poly;

    if (! stringToPolygon(polyStr, poly))
      return false;

    polygons.push_back(poly);

    parse.skipSpace();
  }

  return true;
}

QString polygonListToString(const std::vector<CQChartsGeom::Polygon> &polyList) {
  int np = polyList.size();

  QStringList strs;

  for (int i = 0; i < np; ++i) {
    const auto &poly = polyList[i];

    strs += polygonToString(poly);
  }

  return CQTcl::mergeList(strs);
}

//---

bool stringToPolygon(const QString &str, CQChartsGeom::Polygon &poly) {
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

bool parsePolygon(CQStrParse &parse, CQChartsGeom::Polygon &poly) {
  while (! parse.eof()) {
    parse.skipSpace();

    if (! parse.isChar('{'))
      return false;

    QString pointStr;

    if (! parse.readBracedString(pointStr))
      return false;

    CQChartsGeom::Point point;

    if (! stringToPoint(pointStr, point))
      return false;

    poly.addPoint(point);

    parse.skipSpace();
  }

  return true;
}

QString polygonToString(const CQChartsGeom::Polygon &poly) {
  int np = poly.size();

  QStringList strs;

  for (int i = 0; i < np; ++i) {
    auto p = poly.point(i);

    strs += pointToString(p);
  }

  return CQTcl::mergeList(strs);
}

//---

bool stringToBBox(const QString &str, CQChartsGeom::BBox &bbox) {
  CQStrParse parse(str);

  return parseBBox(parse, bbox, /*terminated*/ true);
}

bool parseBBox(CQStrParse &parse, CQChartsGeom::BBox &bbox, bool terminated) {
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
  CQChartsGeom::Point p1, p2;

  if (! parsePoint(parse, p1, /*terminated*/ false))
    return false;

  if (! parsePoint(parse, p2, /*terminated*/ true))
    return false;

  //---

  bbox = CQChartsGeom::BBox(p1.x, p1.y, p2.x, p2.y);

  return true;
}

QString bboxToString(const CQChartsGeom::BBox &bbox) {
  if (bbox.isSet()) {
    QStringList strs;

    const auto &pmin = bbox.getMin();
    const auto &pmax = bbox.getMax();

    strs += pointToString(pmin);
    strs += pointToString(pmax);

    return CQTcl::mergeList(strs);
  }
  else {
    return "{ }";
  }
}

//---

bool stringToPoint(const QString &str, CQChartsGeom::Point &point) {
  CQStrParse parse(str);

  return parsePoint(parse, point, /*terminated*/ true);
}

bool parsePoint(CQStrParse &parse, CQChartsGeom::Point &point, bool terminated) {
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
  point = CQChartsGeom::Point(x, y);

  return true;
}

QString pointToString(const CQChartsGeom::Point &p) {
  QStringList strs;

  strs += QString("%1").arg(p.x);
  strs += QString("%1").arg(p.y);

  return CQTcl::mergeList(strs);
}

}

//------

namespace CQChartsUtil {

QString sizeToString(const CQChartsGeom::Size &s) {
  QStringList strs;

  strs += QString("%1").arg(s.width ());
  strs += QString("%1").arg(s.height());

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

  auto *tm1 = localtime(&t);
  if (! tm1) return "<no_time>";

  (void) strftime(buffer, 512, fmt.toLatin1().constData(), tm1);

  return buffer;
}

bool stringToTime(const QString &fmt, const QString &str, double &t) {
  struct tm tm1; memset(&tm1, 0, sizeof(tm));

  char *p = strptime(str.toLatin1().constData(), fmt.toLatin1().constData(), &tm1);
  if (! p) return false;

  t = mktime(&tm1);

  return true;
}

}

//------

namespace CQChartsUtil {

bool
formatStringInRect(const QString &str, const QFont &font, const CQChartsGeom::BBox &bbox,
                   QStringList &strs, const FormatData &formatData) {
  auto addStr = [&](const QString &str) {
    assert(str.length());
    strs.push_back(str);
  };

  //---

  QString sstr = str.trimmed();

  if (! sstr.length()) { // empty
    addStr(sstr);
    return false;
  }

  //---

  // number of newlines
  std::size_t nl = str.count('\n');

  //---

  QFontMetricsF fm(font);

  double h = fm.height();

  if (nl == 0) {
    double w  = fm.width(sstr);
    double dw = (bbox.getWidth() - w);

    if (dw > 0 || CMathUtil::isZero(dw)) { // fits
      addStr(sstr);
      return false;
    }

    double dh = (bbox.getHeight() - h);

    if (dh < 0 || CMathUtil::isZero(dh)) { // bbox can only fit single line of text (TODO: factor)
      addStr(sstr);
      return false;
    }
  }

  //---

  // get potential split points
  std::vector<int> splits;

  if (formatData.isValid())
    findStringCustomSplits(sstr, splits, formatData);

  if (splits.size() < nl + 1)
    findStringSpaceSplits(sstr, splits);

  if (splits.size() < nl + 1)
    findStringPunctSplits(sstr, splits);

  if (splits.size() < nl + 1)
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

  QChar c = splits[split];

  QString str1 = sstr.mid(0, split).trimmed();

  QString str2;

  if (c == '\n')
    str2 = sstr.mid(split + 1).trimmed();
  else
    str2 = sstr.mid(split).trimmed();

  std::size_t nl1 = str1.count('\n');
  std::size_t nl2 = str2.count('\n');

  bool fit1 = false;
  bool fit2 = false;

  if (nl1 == 0) {
    double w1 = fm.width(str1);

    fit1 = (w1 <= bbox.getWidth());
  }

  if (nl2 == 0) {
    double w2 = fm.width(str2);

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

    CQChartsGeom::BBox bbox1(bbox.getXMin(), bbox.getYMin(),
                             bbox.getXMax(), bbox.getYMin() + splitHeight);
    CQChartsGeom::BBox bbox2(bbox.getXMin(), bbox.getYMin() + splitHeight,
                             bbox.getXMax(), bbox.getYMax() - splitHeight);

    QStringList strs1, strs2;

    formatStringInRect(str1, font, bbox1, strs1, formatData);
    formatStringInRect(str2, font, bbox2, strs2, formatData);

    strs += strs1;
    strs += strs2;
  }
  else if (! fit1) {
    double splitHeight = bbox.getHeight() - h;

    CQChartsGeom::BBox bbox1(bbox.getXMin(), bbox.getYMin(),
                             bbox.getXMax(), bbox.getYMin() + splitHeight);

    QStringList strs1;

    formatStringInRect(str1, font, bbox1, strs1, formatData);

    strs += strs1;

    addStr(str2);
  }
  else {
    CQChartsGeom::BBox bbox2(bbox.getXMin(), bbox.getYMin() + h,
                             bbox.getXMax(), bbox.getYMax());

    QStringList strs2;

    formatStringInRect(str2, font, bbox2, strs2, formatData);

    addStr(str1);

    strs += strs2;
  }

  return true;
}

}

//------

namespace CQChartsUtil {

void setPenBrush(CQChartsPenBrush &penBrush,
                 bool stroked, const QColor &strokeColor, const CQChartsAlpha &strokeAlpha,
                 double strokeWidth, const CQChartsLineDash &strokeDash,
                 bool filled, const QColor &fillColor, const CQChartsAlpha &fillAlpha,
                 const CQChartsFillPattern &pattern)
{
  setPen(penBrush.pen, stroked, strokeColor, strokeAlpha, strokeWidth, strokeDash);

  setBrush(penBrush.brush, filled, fillColor, fillAlpha, pattern);
}

void setPen(QPen &pen, bool stroked, const QColor &strokeColor, const CQChartsAlpha &strokeAlpha,
            double strokeWidth, const CQChartsLineDash &strokeDash) {
  double width = limitLineWidth(strokeWidth);

  // calc pen (stroke)
  if (stroked) {
    QColor color = strokeColor;

    color.setAlphaF(CMathUtil::clamp(strokeAlpha.value(), 0.0, 1.0));

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

void setBrush(QBrush &brush, bool filled, const QColor &fillColor, const CQChartsAlpha &fillAlpha,
              const CQChartsFillPattern &pattern) {
  // calc brush (fill)
  if (filled) {
    QColor color = fillColor;

    color.setAlphaF(CMathUtil::clamp(fillAlpha.value(), 0.0, 1.0));

    brush.setColor(color);

    brush.setStyle(pattern.style());

    if (pattern.scale() != 1.0)
      brush.setTransform(QTransform::fromScale(pattern.scale(), pattern.scale()));
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

//------

namespace CQChartsUtil {

QFont scaleFontSize(const QFont &font, double s, double minSize, double maxSize) {
  double fs = font.pointSizeF()*s;

  fs = CMathUtil::clamp(fs, minSize, maxSize);

  QFont font1 = font;

  if (fs > 0.0)
    font1.setPointSizeF(fs);

  return font1;
}

}
