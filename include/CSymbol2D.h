#ifndef CSYMBOL_2D_H
#define CSYMBOL_2D_H

#include <string>
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
    PENTAGON
  };

  enum class Connect {
    NONE,
    LINE,
    CLOSE,
    STROKE
  };

  struct Line {
    double  x1      { 0 }, y1 { 0 };
    double  x2      { 0 }, y2 { 0 };
    Connect connect { Connect::NONE };

    Line(double x11, double y11, double x21, double y21, Connect connect1=Connect::NONE) :
     x1(x11), y1(y11), x2(x21), y2(y21), connect(connect1) {
    }
  };

  typedef std::vector<Line> Lines;

  Type  type { Type::NONE };
  Lines lines;

  CSymbol2D(Type type1, std::initializer_list<Line> lines1) :
   type(type1), lines(lines1) {
  }
};

//---

namespace CSymbol2DMgr {
  bool isSymbol(CSymbol2D::Type type);

  const CSymbol2D &getSymbol(CSymbol2D::Type type);

  void drawSymbol(CSymbol2D::Type type, CSymbol2DRenderer *renderer);

  std::string typeToName(CSymbol2D::Type type);

  CSymbol2D::Type nameToType(const std::string &str);
}

//---

class CSymbol2DRenderer {
 public:
  CSymbol2DRenderer() { }

  virtual void moveTo(double x, double y) = 0;
  virtual void lineTo(double x, double y) = 0;

  virtual void closePath() = 0;

  virtual void stroke() = 0;

  void drawSymbol(CSymbol2D::Type type);
};

#endif
