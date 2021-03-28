#include <CQChartsSVGUtil.h>
#include <CQChartsDrawUtil.h>
#include <CSVGUtil.h>
#include <CQStrParse.h>
#include <CXML.h>
#include <CXMLToken.h>

namespace {

struct PathData {
  QString d;
  QString style;
};

using PathDatas = std::vector<PathData>;

void processXmlTag(CXMLTag *tag, PathDatas &pathDatas) {
  bool isPath = (tag->getName() == "path");

  if (isPath) {
    // process options
    PathData pathData;

    for (const auto &option1 : tag->getOptions()) {
      const auto &optName  = option1->getName ();
      const auto &optValue = option1->getValue();

      if     (optName == "d") {
        pathData.d = QString(optValue.c_str());
      }
      else if (optName == "style") {
        pathData.style = QString(optValue.c_str());
      }
    }

    if (pathData.d != "")
      pathDatas.push_back(pathData);
  }

  //---

  const auto &children = tag->getChildren();

  for (const auto *child : children) {
    if (! child->isTag())
      continue;

    auto *childTag = child->getTag();

    processXmlTag(childTag, pathDatas);
  }
}

}

bool
CQChartsSVGUtil::
svgFileToPaths(const QString &fileName, Paths &paths, Styles &styles, BBox &bbox)
{
  CXML xml;

  CXMLTag *tag;

  if (! xml.read(fileName.toStdString(), &tag))
    return false;

  if (tag->getName() != "svg")
    return false;

  // process options
  for (const auto &option1 : tag->getOptions()) {
    const auto &optName  = option1->getName ();
    const auto &optValue = option1->getValue();

    if (optName == "viewBox") {
      auto strs = QString(optValue.c_str()).split(" ", QString::SkipEmptyParts);

      if (strs.size() == 4) {
        bool ok { false }, ok1 { true };
        int x = strs[0].toInt(&ok); if (! ok) ok1 = false;
        int y = strs[1].toInt(&ok); if (! ok) ok1 = false;
        int w = strs[2].toInt(&ok); if (! ok) ok1 = false;
        int h = strs[3].toInt(&ok); if (! ok) ok1 = false;

        if (ok1)
          bbox = BBox(x, y, x + w, y + h);
      }
    }
  }

  PathDatas pathDatas;

  processXmlTag(tag, pathDatas);

  for (const auto &pathData : pathDatas) {
    paths .emplace_back(pathData.d);
    styles.emplace_back(pathData.style);
  }

  return true;
}

//---

bool
CQChartsSVGUtil::
stringToPath(const QString &str, QPainterPath &path)
{
  class PathVisitor : public CSVGUtil::PathVisitor {
   public:
    PathVisitor() { }

    const QPainterPath &path() const { return path_; }

    void moveTo(double x, double y) override {
      path_.moveTo(x, y);
    }

    void lineTo(double x, double y) override {
      path_.lineTo(x, y);
    }

    void arcTo(double rx, double ry, double xa, int fa, int fs, double x2, double y2) override {
      bool unit_circle = false;

      //double cx, cy, rx1, ry1, theta, delta;

      //CSVGUtil::convertArcCoords(lastX(), lastY(), x2, y2, xa, rx, ry, fa, fs, unit_circle,
      //                           &cx, &cy, &rx1, &ry1, &theta, &delta);

      //path_.arcTo(QRectF(cx - rx1, cy - ry1, 2*rx1, 2*ry1), -theta, -delta);

      //double a1 = CMathUtil::Deg2Rad(theta);
      //double a2 = CMathUtil::Deg2Rad(theta + delta);

      CSVGUtil::BezierList beziers;

      CSVGUtil::arcToBeziers(lastX(), lastY(), x2, y2, xa, rx, ry, fa, fs, unit_circle, beziers);

      auto qpoint = [](const CPoint2D &p) { return QPointF(p.x, p.y); };

      if (! beziers.empty())
        path_.lineTo(qpoint(beziers[0].getFirstPoint()));

      for (const auto &bezier : beziers)
        path_.cubicTo(qpoint(bezier.getControlPoint1()),
                      qpoint(bezier.getControlPoint2()),
                      qpoint(bezier.getLastPoint    ()));
    }

    void bezier2To(double x1, double y1, double x2, double y2) override {
      path_.quadTo(QPointF(x1, y1), QPointF(x2, y2));
    }

    void bezier3To(double x1, double y1, double x2, double y2, double x3, double y3) override {
      path_.cubicTo(QPointF(x1, y1), QPointF(x2, y2), QPointF(x3, y3));
    }

    void closePath(bool /*relative*/) override {
      path_.closeSubpath();
    }

   private:
    QPainterPath path_;
  };

  PathVisitor visitor;

  if (! CSVGUtil::visitPath(str.toStdString(), visitor)) {
    //std::cerr << "Invalid path: " << str.toStdString() << "\n";
    return false;
  }

  path = visitor.path();

  return true;
}

QString
CQChartsSVGUtil::
pathToString(const QPainterPath &path)
{
  using Point = CQChartsGeom::Point;

  class PathVisitor : public CQChartsDrawUtil::PathVisitor {
   public:
    void moveTo(const Point &p) override {
      if (str_.length()) str_ += " ";

      str_ += QString("M %1 %2").arg(p.x).arg(p.y);
    }

    void lineTo(const Point &p) override {
      if (str_.length()) str_ += " ";

      str_ += QString("L %1 %2").arg(p.x).arg(p.y);
    }

    void quadTo(const Point &p1, const Point &p2) override {
      if (str_.length()) str_ += " ";

      str_ += QString("Q %1 %2 %3 %4").arg(p1.x).arg(p1.y).arg(p2.x).arg(p2.y);
    }

    void curveTo(const Point &p1, const Point &p2, const Point &p3) override {
      if (str_.length()) str_ += " ";

      str_ += QString("C %1 %2 %3 %4 %5 %6").
                arg(p1.x).arg(p1.y).arg(p2.x).arg(p2.y).arg(p3.x).arg(p3.y);
    }

    const QString &str() const { return str_; }

   private:
    QString str_;
  };

  PathVisitor visitor;

  CQChartsDrawUtil::visitPath(path, visitor);

  return visitor.str();
}

//---

bool
CQChartsSVGUtil::
stringToPenBrush(const QString &str, QPen &pen, QBrush &brush)
{
  bool valid = true;

  CQStrParse parse(str);

  while (! parse.eof()) {
    parse.skipSpace();

    QString name;

    while (! parse.eof()) {
      if (parse.isSpace() || parse.isChar(':'))
        break;

      name += parse.getChar();
    }

    parse.skipSpace();

    if (parse.isChar(':')) {
      parse.skipChar();

      parse.skipSpace();
    }

    QString value;

    while (! parse.eof()) {
      if (parse.isSpace() || parse.isChar(';'))
        break;

      value += parse.getChar();
    }

    parse.skipSpace();

    if (parse.isChar(';')) {
      parse.skipChar();

      parse.skipSpace();
    }

    if      (name == "fill") {
      QColor c(value);

      brush.setColor(c);
      brush.setStyle(Qt::SolidPattern);
    }
    else if (name == "fill-opacity") {
      bool ok;
      double a = value.toDouble(&ok);
      if (! ok) continue;

      auto c = brush.color();
      c.setAlphaF(a);
      brush.setColor(c);
    }
    else if (name == "stroke") {
      QColor c(value);

      pen.setColor(c);
    }
    else if (name == "stroke-width") {
      bool ok;
      double w = value.toDouble(&ok);
      if (! ok) continue;

      pen.setWidthF(w);
    }
    else {
      valid = false;

#if 0
      std::cerr << "Unsupported style: " <<
                   name.toStdString() << "=" << value.toStdString() << "\n";
#endif
    }
  }

  return valid;
}

QString
CQChartsSVGUtil::
penBrushToString(QPen &pen, QBrush &brush)
{
  QString str;

  str += QString("stroke: %1;"      ).arg(pen.color().name());
  str += " ";
  str += QString("stroke-width: %1;").arg(pen.widthF());
  str += " ";
  str += QString("fill: %1;"        ).arg(brush.color().name());
  str += " ";
  str += QString("fill-opacity: %1;").arg(brush.color().alphaF());

  return str;
}
