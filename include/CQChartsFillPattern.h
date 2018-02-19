#ifndef CQChartsFillPattern_H
#define CQChartsFillPattern_H

namespace CQChartsFillPattern {
  enum class Type {
    SOLID,
    HATCH,
    DENSE,
    HORIZ,
    VERT,
    FDIAG,
    BDIAG
  };

  inline Qt::BrushStyle toStyle(const Type &type) {
    switch (type) {
      case Type::SOLID: return Qt::SolidPattern;
      case Type::HATCH: return Qt::CrossPattern;
      case Type::DENSE: return Qt::Dense5Pattern;
      case Type::HORIZ: return Qt::HorPattern;
      case Type::VERT : return Qt::VerPattern;
      case Type::FDIAG: return Qt::FDiagPattern;
      case Type::BDIAG: return Qt::BDiagPattern;
      default         : return Qt::SolidPattern;
    }
  }
}

#endif
