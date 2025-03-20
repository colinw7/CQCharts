#include <CQChartsSVGUtil.h>
#include <CQChartsPlot.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsUtil.h>

#include <CSVGUtil.h>
#include <CQStrParse.h>
#include <CStrUtil.h>

#ifdef CXML_PARSER
#include <CXML.h>
#include <CXMLToken.h>
#else
#include <QXmlStreamReader>
#include <QFile>
#endif

namespace {

struct PathData {
  QString d;
  QString style;
};

using PathDatas = std::vector<PathData>;

#ifdef CXML_PARSER
void processXmlTag(CXMLTag *tag, PathDatas &pathDatas) {
  if      (tag->getName() == "path") {
    // process options
    PathData pathData;

    for (const auto &option1 : tag->getOptions()) {
      const auto &optName  = option1->getName ();
      const auto &optValue = option1->getValue();

      if     (optName == "d") {
        pathData.d = QString::fromStdString(optValue);
      }
      else if (optName == "style") {
        pathData.style = QString::fromStdString(optValue);
      }
    }

    if (pathData.d != "")
      pathDatas.push_back(std::move(pathData));
  }
  else if (tag->getName() == "circle") {
    // TODO ?
  }
  else if (tag->getName() == "ellipse") {
    // TODO ?
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
#endif

}

#ifndef CXML_PARSER
class CQChartsSVGParserHandler {
 public:
  using BBox = CQChartsGeom::BBox;

 public:
  CQChartsSVGParserHandler(BBox &bbox, PathDatas &pathDatas) :
    bbox_(bbox), pathDatas_(pathDatas) {
  }

  bool parse(QXmlStreamReader &xml) {
    while (! xml.atEnd()) {
      xml.readNext();

      switch (xml.tokenType()) {
        case QXmlStreamReader::StartDocument: {
          break;
        }
        case QXmlStreamReader::EndDocument: {
          break;
        }
        case QXmlStreamReader::StartElement: {
          if (! startElement(xml.qualifiedName().toString(), xml.attributes()))
            return false;

          break;
        }
        case QXmlStreamReader::EndElement: {
         if (! endElement(xml.qualifiedName().toString()))
            return false;

          break;
        }
        case QXmlStreamReader::Comment: {
          break;
        }
        case QXmlStreamReader::Characters: {
          break;
        }
        default:
          break;
      }
    }

    return xml.hasError();
  }

 private:
  bool startElement(const QString &qName, const QXmlStreamAttributes &attributes) {
    // save bounding box
    if      (qName == "svg") {
      for (int i = 0; i < attributes.length(); ++i) {
        auto &attr = attributes[i];

        auto name  = attr.qualifiedName();
        auto value = attr.value().toString();

        if (name == "viewBox") {
          auto strs = QString(value).split(" ", Qt::SkipEmptyParts);

          if (strs.size() == 4) {
            bool ok { false }, ok1 { true };
            int x = strs[0].toInt(&ok); if (! ok) ok1 = false;
            int y = strs[1].toInt(&ok); if (! ok) ok1 = false;
            int w = strs[2].toInt(&ok); if (! ok) ok1 = false;
            int h = strs[3].toInt(&ok); if (! ok) ok1 = false;

            if (ok1)
              bbox_ = BBox(x, y, x + w, y + h);
          }
        }
      }
    }
    // save path
    else if (qName == "path") {
      PathData pathData;

      for (int i = 0; i < attributes.length(); ++i) {
        auto &attr = attributes[i];

        auto name  = attr.qualifiedName();
        auto value = attr.value().toString();

        if      (name == "d")
          pathData.d = value;
        else if (name == "style")
          pathData.style = value;
      }

      if (pathData.d != "")
        pathDatas_.push_back(std::move(pathData));
    }

    return true;
  }

  bool endElement(const QString &) {
    return true;
  }

 private:
  BBox      &bbox_;
  PathDatas &pathDatas_;
};
#endif

bool
CQChartsSVGUtil::
svgFileToPaths(const QString &filename, Paths &paths, Styles &styles, BBox &bbox)
{
  PathDatas pathDatas;

#ifdef CXML_PARSER
  CXML xml;

  CXMLTag *tag;

  if (! xml.read(filename.toStdString(), &tag))
    return false;

  if (tag->getName() != "svg")
    return false;

  // process options
  for (const auto &option1 : tag->getOptions()) {
    const auto &optName  = option1->getName ();
    const auto &optValue = option1->getValue();

    if (optName == "viewBox") {
      auto strs = QString::fromStdString(optValue).split(" ", Qt::SkipEmptyParts);

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

  processXmlTag(tag, pathDatas);
#else
  QFile file(filename);

  if (! file.open(QIODevice::ReadOnly))
    return false;

  CQChartsSVGParserHandler handler(bbox, pathDatas);

  QXmlStreamReader xml(&file);

  handler.parse(xml);
#endif

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
  CQChartsSVGStyleData data;

  if (! stringToStyle(nullptr, str, data))
    return false;

  pen   = data.pen;
  brush = data.brush;

  return false;
}

bool
CQChartsSVGUtil::
stringToStyle(CQChartsPlot *plot, const QString &str, StyleData &data)
{
  data.arrowData.setFHeadType(CQChartsArrowData::HeadType::ARROW);
  data.arrowData.setTHeadType(CQChartsArrowData::HeadType::ARROW);

#if 0
  auto TODO = [](const QString &name, const QString &value) {
    std::cerr << "TODO style: " <<
                 name.toStdString() << "=" << value.toStdString() << "\n";
  };
#endif

  auto UNHANDLED = [](const QString &name, const QString &value) {
    std::cerr << "Unsupported style: " <<
                 name.toStdString() << "=" << value.toStdString() << "\n";
  };

  auto parseLength = [&](const QString &value, double &w) {
    if (plot) {
      auto l = CQChartsLength(value);
      if (! l.isValid()) return false;

      w = plot->lengthPixelWidth(l);
    }
    else {
      bool ok;
      w = CQChartsUtil::toReal(value, ok);
      if (! ok) return false;
    }

    return true;
  };

  auto parseColor = [&](const QString &value, QColor &c) {
    CQChartsColor color(value);

    if (plot)
      c = plot->interpColor(color, CQChartsPlot::ColorInd());
    else
      c = color.color();

    return true;
  };

  //---

  if (str.trimmed() == "")
    return false;

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
      if (parse.isChar(';'))
        break;

      value += parse.getChar();
    }

    value = value.trimmed();

    parse.skipSpace();

    if (parse.isChar(';')) {
      parse.skipChar();

      parse.skipSpace();
    }

    data.nameValues[name] = value;

    if      (name == "fill") {
      QColor c;
      (void) parseColor(value, c);

      data.brush.setColor(c);
      data.brush.setStyle(Qt::SolidPattern);
    }
    else if (name == "fill-opacity") {
      bool ok;
      double a = CQChartsUtil::toReal(value, ok);
      if (! ok) continue;

      auto c = data.brush.color();
      c.setAlphaF(a);
      data.brush.setColor(c);
    }
    else if (name == "stroke") {
      QColor c;
      (void) parseColor(value, c);

      data.pen.setColor(c);
    }
    else if (name == "stroke-width") {
      double w;
      if (parseLength(value, w))
        data.pen.setWidthF(w);
    }
    else if (name == "stroke-dasharray") {
      CQChartsLineDash dash(value);

      CQChartsUtil::penSetLineDash(data.pen, dash);
    }
    else if (name == "arrow-start") {
      bool ok;
      data.arrowData.setFHead(CQChartsUtil::stringToBool(value, &ok));
    }
    else if (name == "arrow-end") {
      bool ok;
      data.arrowData.setTHead(CQChartsUtil::stringToBool(value, &ok));
    }
    else if (name == "arrow-start-length") {
      double w;
      if (parseLength(value, w))
        data.arrowData.setFrontLength(CQChartsLength::pixel(w));
    }
    else if (name == "arrow-end-length") {
      double w;
      if (parseLength(value, w))
        data.arrowData.setTailLength(CQChartsLength::pixel(w));
    }
    else if (name == "arrow-width") {
      double w;
      if (parseLength(value, w))
        data.arrowData.setLineWidth(CQChartsLength::pixel(w));
    }
    else if (name == "arc-start") {
      data.arcStart = CQChartsAngle(value);
    }
    else if (name == "arc-delta" || name == "arc-start-delta") {
      data.arcDelta = CQChartsAngle(value);
    }
    else if (name == "arc-end") {
      data.arcEnd = CQChartsAngle(value);
    }
    else if (name == "arc-end-delta") {
      data.arcEndDelta = CQChartsAngle(value);
    }
    else if (name == "inner-radius") {
      (void) parseLength(value, data.innerRadius);
    }
    else if (name == "radius" || name == "outer-radius") {
      (void) parseLength(value, data.outerRadius);
    }
    else if (name == "direction") {
      data.direction = value;
    }
    else if (name == "draw-type") {
      data.drawType = value;
    }
    else if (name == "font" || name == "font-size") {
      data.textFont = value;
    }
    else if (name == "halign" || name == "text-halign") {
      data.textAlign &= ~0x0F;

      if      (value == "left")
        data.textAlign |= Qt::AlignLeft;
      else if (value == "right")
        data.textAlign |= Qt::AlignRight;
      else if (value == "center")
        data.textAlign |= Qt::AlignHCenter;
    }
    else if (name == "valign" || name == "text-valign") {
      data.textAlign &= ~0x1F0;

      if      (value == "top")
        data.textAlign |= Qt::AlignTop;
      else if (value == "bottom")
        data.textAlign |= Qt::AlignBottom;
      else if (value == "center")
        data.textAlign |= Qt::AlignVCenter;
    }
    else if (name == "text-angle") {
      data.textAngle = value;
    }
    else if (name == "text-scaled") {
      bool ok;
      data.textScaled = CQChartsUtil::stringToBool(value, &ok);
    }
    else if (name == "symbol-type") {
      data.symbolType = value;
    }
    else if (name == "symbol-size") {
      (void) parseLength(value, data.symbolSize);
    }
    else if (name == "tip") {
      data.tip = value;
    }
    else {
      UNHANDLED(name, value);

      valid = false;
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
