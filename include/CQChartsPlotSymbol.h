#ifndef CQChartsPlotSymbol_H
#define CQChartsPlotSymbol_H

#include <CQChartsSymbol.h>
#include <QString>
#include <QBrush>
#include <QPen>
#include <vector>

class CQChartsPlotSymbolRenderer;

//---

//! \brief plot symbol
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

//! \brief plot symbol renderer
class CQChartsPlotSymbolRenderer {
 public:
  CQChartsPlotSymbolRenderer() { }

  virtual void moveTo(double x, double y) = 0;
  virtual void lineTo(double x, double y) = 0;

  virtual void closePath() = 0;

  virtual void stroke() = 0;

  virtual void fill() = 0;

  virtual void drawPoint(double x, double y) const = 0;

  virtual void drawLine(double x1, double y1, double x2, double y2) const = 0;

  virtual void strokeCircle(double x, double y, double r) const = 0;
  virtual void fillCircle  (double x, double y, double r) const = 0;

  virtual double lineWidth() const { return 0.0; }

  void drawSymbol  (CQChartsSymbol type);
  void strokeSymbol(CQChartsSymbol type);
  void fillSymbol  (CQChartsSymbol type);
};

//------

#include <CQChartsGeom.h>
#include <QPainterPath>

class CQChartsPlot;
class QPainter;

//! \brief plot symbol 2d renderer
class CQChartsSymbol2DRenderer : public CQChartsPlotSymbolRenderer {
 public:
  CQChartsSymbol2DRenderer(QPainter *painter, const CQChartsGeom::Point &p, double s);

  void moveTo(double x, double y) override;

  void lineTo(double x, double y) override;

  void closePath() override;

  void stroke() override;

  void fill() override;

  void drawPoint(double x, double y) const override;

  void drawLine(double x1, double y1, double x2, double y2) const override;

  void strokeCircle(double x, double y, double r) const override;
  void fillCircle  (double x, double y, double r) const override;

  double lineWidth() const override;

 private:
  QPainter*           painter_ { nullptr };  //!< painter
  CQChartsGeom::Point p_       { 0.0, 0.0 }; //!< symbol center
  double              s_       { 2.0 };      //!< size
  double              w_       { 0.0 };      //!< line width
  QPainterPath        path_;                 //!< path
  QPen                strokePen_;            //!< stroke pen
  QBrush              fillBrush_;            //!< fill brush
};

#endif
