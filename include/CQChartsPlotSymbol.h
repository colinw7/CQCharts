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
  enum class Connect {
    NONE,
    LINE,
    CLOSE,
    STROKE,
    FILL
  };

  struct Point {
    Point(double x=0.0, double y=0.0) :
     x(x), y(y) {
    }

    double x { 0.0 };
    double y { 0.0 };
  };

  struct Line {
    double  x1      { 0.0 }, y1 { 0.0 };
    double  x2      { 0.0 }, y2 { 0.0 };
    Connect connect { Connect::NONE };

    Line(double x11, double y11, double x21, double y21, Connect connect1=Connect::NONE) :
     x1(x11), y1(y11), x2(x21), y2(y21), connect(connect1) {
    }
  };

  using Lines = std::vector<Line>;

  CQChartsSymbol type { CQChartsSymbol::Type::NONE };
  Lines          lines;
  Lines          fillLines;

  CQChartsPlotSymbol(CQChartsSymbol type, std::initializer_list<Line> lines,
                     std::initializer_list<Line> fillLines) :
   type(type), lines(lines), fillLines(fillLines) {
  }
};

//---

namespace CQChartsPlotSymbolMgr {
  bool isSymbol(CQChartsSymbol type);

  const CQChartsPlotSymbol &getSymbol(CQChartsSymbol type);

  void drawSymbol  (CQChartsSymbol type, CQChartsPlotSymbolRenderer *renderer);
  void strokeSymbol(CQChartsSymbol type, CQChartsPlotSymbolRenderer *renderer);
  void fillSymbol  (CQChartsSymbol type, CQChartsPlotSymbolRenderer *renderer);
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
  CQChartsPlotSymbolRenderer(CQChartsPaintDevice *painter, const CQChartsGeom::Point &p,
                             const CQChartsLength &size);

  void drawSymbol  (CQChartsSymbol type);
  void strokeSymbol(CQChartsSymbol type);
  void fillSymbol  (CQChartsSymbol type);

  //---

  void moveTo(double x, double y);

  void lineTo(double x, double y);

  void closePath();

  void stroke();

  void fill();

  void drawPoint(double x, double y) const;

  void drawLine(double x1, double y1, double x2, double y2) const;

  void fillRect(double x1, double y1, double x2, double y2) const;

  void strokeCircle(double x, double y, double r) const;
  void fillCircle  (double x, double y, double r) const;

  void mapXY(double x, double y, double &x1, double &y1) const;

  double lineWidth() const;

  void save   () const;
  void restore() const;

 private:
  CQChartsPaintDevice* device_  { nullptr };  //!< device
  CQChartsGeom::Point  p_       { 0.0, 0.0 }; //!< symbol center
  double               s_       { 2.0 };      //!< size as pixel
  CQChartsLength       size_;                 //!< size as length
  double               w_       { 0.0 };      //!< line width
  QPainterPath         path_;                 //!< path
  QPen                 strokePen_;            //!< stroke pen
  QBrush               fillBrush_;            //!< fill brush
  mutable bool         saved_   { false };    //!< saved
};

#endif
