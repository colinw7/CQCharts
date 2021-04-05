#ifndef CQChartsPlotSymbol_H
#define CQChartsPlotSymbol_H

#include <CQChartsSymbol.h>
#include <CQChartsLength.h>
#include <QString>
#include <QBrush>
#include <QPen>
#include <vector>

class CQChartsPlotSymbolRenderer;
class CQChartsPaintDevice;

//---

/*!
 * \brief plot symbol
 * \ingroup Charts
 */
struct CQChartsPlotSymbol {
  using SymbolType = CQChartsSymbolType;

  enum class Connect {
    NONE,
    LINE,   // add point to path
    CLOSE,  // close path
    STROKE, // add point, stroke and close path
    FILL,   // add point, fill and close path
    BREAK   // move to new point
  };

  //! point data
  struct Point {
    Point(double x=0.0, double y=0.0) :
     x(x), y(y) {
    }

    double x { 0.0 };
    double y { 0.0 };
  };

  //! line data
  struct Line {
    double  x1      { 0.0 }, y1 { 0.0 };
    double  x2      { 0.0 }, y2 { 0.0 };
    Connect connect { Connect::NONE };

    Line(double x11, double y11, double x21, double y21, Connect connect1=Connect::NONE) :
     x1(x11), y1(y11), x2(x21), y2(y21), connect(connect1) {
    }
  };

  using Lines = std::vector<Line>;

  SymbolType::Type type { SymbolType::Type::NONE };
  Lines            lines;
  Lines            fillLines;

  CQChartsPlotSymbol(SymbolType::Type type, std::initializer_list<Line> lines,
                     std::initializer_list<Line> fillLines) :
   type(type), lines(lines), fillLines(fillLines) {
  }
};

//---

namespace CQChartsPlotSymbolMgr {
  using SymbolRenderer = CQChartsPlotSymbolRenderer;
  using Symbol         = CQChartsSymbol;

  bool isSymbol(const Symbol &symbol);

  const CQChartsPlotSymbol &getSymbol(const Symbol &symbol);

  void setSymbolLines(const Symbol &symbol, const CQChartsPlotSymbol::Lines &lines);
  void setSymbolFillLines(const Symbol &symbol, const CQChartsPlotSymbol::Lines &lines);

  void drawSymbol  (const Symbol &symbol, SymbolRenderer *renderer);
  void strokeSymbol(const Symbol &symbol, SymbolRenderer *renderer);
  void fillSymbol  (const Symbol &symbol, SymbolRenderer *renderer);
}

//---

#include <CQChartsGeom.h>
#include <QPainterPath>

class CQChartsPlot;

/*!
 * \brief plot symbol renderer
 * \ingroup Charts
 */
class CQChartsPlotSymbolRenderer {
 public:
  using PaintDevice = CQChartsPaintDevice;
  using Length      = CQChartsLength;
  using Symbol      = CQChartsSymbol;
  using Point       = CQChartsGeom::Point;
  using BBox        = CQChartsGeom::BBox;

 public:
  CQChartsPlotSymbolRenderer(PaintDevice *device, const Point &p, const Length &size);

  void drawSymbol  (Symbol symbol);
  void strokeSymbol(Symbol symbol);
  void fillSymbol  (Symbol symbol);

  //---

  PaintDevice *device() const { return device_; }

  void moveTo(double x, double y);

  void lineTo(double x, double y);

  void closePath();

  void stroke();

  void fill();

  void drawFillPoint(double x, double y, double r, bool fill) const;

  void drawPoint(double x, double y) const;

  void drawFillHLine(double x1, double x2, double y, double w, bool fill);
  void drawFillVLine(double x, double y1, double y2, double w, bool fill);

  void drawLine(double x1, double y1, double x2, double y2) const;

  void fillRect(double x1, double y1, double x2, double y2) const;
  void drawRect(double x1, double y1, double x2, double y2) const;

  void drawFillCircle(double x, double y, double r, bool fill) const;

  void strokeCircle(double x, double y, double r) const;
  void fillCircle  (double x, double y, double r) const;

  void drawChar(const QString &s) const;

  void drawPaths(const std::vector<CQChartsPath> &paths,
                 const std::vector<CQChartsStyle> &styles) const;
  void drawPath (const CQChartsPath &path, const CQChartsStyle &style) const;

  Point mapXY(const Point &p) const;

  void mapXY(double x, double y, double &x1, double &y1) const;

  double lineWidth() const;

  void save   () const;
  void restore() const;

 private:
  PaintDevice*   device_  { nullptr };  //!< device
  Point          p_       { 0.0, 0.0 }; //!< symbol center
  Length         size_;                 //!< size as length
  double         w_       { 0.0 };      //!< line width
  QPainterPath   path_;                 //!< path
  QPen           strokePen_;            //!< stroke pen
  QBrush         fillBrush_;            //!< fill brush
  mutable bool   saved_   { false };    //!< saved
};

#endif
