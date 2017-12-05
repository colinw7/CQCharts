#ifndef CQChartsPlotSymbol_H
#define CQChartsPlotSymbol_H

#include <QString>
#include <QBrush>
#include <QPen>
#include <vector>

class CQChartsPlotSymbolRenderer;

//---

struct CQChartsPlotSymbol {
  enum class Type {
    NONE,
    CROSS,
    PLUS,
    Y,
    TRIANGLE,
    ITRIANGLE,
    BOX,
    DIAMOND,
    STAR,
    STAR1,
    CIRCLE,
    PENTAGON,
    IPENTAGON
  };

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

  Type  type { Type::NONE };
  Lines lines;
  Lines fillLines;

  CQChartsPlotSymbol(Type type, std::initializer_list<Line> lines,
                     std::initializer_list<Line> fillLines) :
   type(type), lines(lines), fillLines(fillLines) {
  }
};

//---

namespace CQChartsPlotSymbolMgr {
  bool isSymbol(CQChartsPlotSymbol::Type type);

  const CQChartsPlotSymbol &getSymbol(CQChartsPlotSymbol::Type type);

  void drawSymbol  (CQChartsPlotSymbol::Type type, CQChartsPlotSymbolRenderer *renderer);
  void strokeSymbol(CQChartsPlotSymbol::Type type, CQChartsPlotSymbolRenderer *renderer);
  void fillSymbol  (CQChartsPlotSymbol::Type type, CQChartsPlotSymbolRenderer *renderer);

  QString typeToName(CQChartsPlotSymbol::Type type);

  CQChartsPlotSymbol::Type nameToType(const QString &str);
}

//---

class CQChartsPlotSymbolRenderer {
 public:
  CQChartsPlotSymbolRenderer() { }

  virtual void moveTo(double x, double y) = 0;
  virtual void lineTo(double x, double y) = 0;

  virtual void closePath() = 0;

  virtual void stroke() = 0;

  virtual void fill() = 0;

  virtual void strokeCircle(double x, double y, double r) = 0;
  virtual void fillCircle  (double x, double y, double r) = 0;

  virtual double lineWidth() const { return 0.0; }

  void drawSymbol  (CQChartsPlotSymbol::Type type);
  void strokeSymbol(CQChartsPlotSymbol::Type type);
  void fillSymbol  (CQChartsPlotSymbol::Type type);
};

//------

#include <CQChartsGeom.h>
#include <QPainterPath>

class CQChartsPlot;
class CQChartsRenderer;

class CQChartsSymbol2DRenderer : public CQChartsPlotSymbolRenderer {
 public:
  CQChartsSymbol2DRenderer(CQChartsRenderer *renderer, const CQChartsGeom::Point &p, double s);

  void moveTo(double x, double y) override;

  void lineTo(double x, double y) override;

  void closePath() override;

  void stroke() override;

  void fill() override;

  void strokeCircle(double x, double y, double r) override;

  void fillCircle(double x, double y, double r) override;

  double lineWidth() const override;

 private:
  CQChartsRenderer*   renderer_ { nullptr };
  CQChartsGeom::Point p_        { 0.0, 0.0 };
  double              s_        { 2.0 };
  double              w_        { 0.0 };
  QPainterPath        path_;
  QPen                strokePen_;
  QBrush              fillBrush_;
};

#endif
