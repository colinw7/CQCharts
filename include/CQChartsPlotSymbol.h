#ifndef CQChartsPlotSymbol_H
#define CQChartsPlotSymbol_H

#include <QString>
#include <vector>

class CSymbol2DRenderer;

//---

struct CSymbol2D {
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

  CSymbol2D(Type type, std::initializer_list<Line> lines, std::initializer_list<Line> fillLines) :
   type(type), lines(lines), fillLines(fillLines) {
  }
};

//---

namespace CSymbol2DMgr {
  bool isSymbol(CSymbol2D::Type type);

  const CSymbol2D &getSymbol(CSymbol2D::Type type);

  void drawSymbol  (CSymbol2D::Type type, CSymbol2DRenderer *renderer);
  void strokeSymbol(CSymbol2D::Type type, CSymbol2DRenderer *renderer);
  void fillSymbol  (CSymbol2D::Type type, CSymbol2DRenderer *renderer);

  QString typeToName(CSymbol2D::Type type);

  CSymbol2D::Type nameToType(const QString &str);
}

//---

class CSymbol2DRenderer {
 public:
  CSymbol2DRenderer() { }

  virtual void moveTo(double x, double y) = 0;
  virtual void lineTo(double x, double y) = 0;

  virtual void closePath() = 0;

  virtual void stroke() = 0;

  virtual void fill() = 0;

  virtual void strokeCircle(double x, double y, double r) = 0;
  virtual void fillCircle  (double x, double y, double r) = 0;

  virtual double lineWidth() const { return 0.0; }

  void drawSymbol  (CSymbol2D::Type type);
  void strokeSymbol(CSymbol2D::Type type);
  void fillSymbol  (CSymbol2D::Type type);
};

//------

#include <CQChartsGeom.h>
#include <QPainterPath>
#include <QColor>

class CQChartsPlot;
class QPainter;

class CQChartsSymbol2DRenderer : public CSymbol2DRenderer {
 public:
  CQChartsSymbol2DRenderer(QPainter *painter, const CQChartsGeom::Point &p, double s);

  void moveTo(double x, double y) override;

  void lineTo(double x, double y) override;

  void closePath() override;

  void stroke() override;

  void fill() override;

  void strokeCircle(double x, double y, double r) override;

  void fillCircle(double x, double y, double r) override;

  double lineWidth() const override;

 private:
  QPainter*    painter_ { nullptr };
  CQChartsGeom::Point     p_       { 0.0, 0.0 };
  double       s_       { 2.0 };
  double       w_       { 0.0 };
  QPainterPath path_;
  QColor       pc_;
  QColor       fc_;
};

#endif
