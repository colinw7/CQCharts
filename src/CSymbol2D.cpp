#include <CSymbol2D.h>
#include <CStrUtil.h>
#include <cassert>

class CSymbol2DList {
 public:
  typedef std::vector<CSymbol2D> Symbols;

 public:
  CSymbol2DList(std::initializer_list<CSymbol2D> symbols) :
   symbols_(symbols) {
  }

  const Symbols &symbols() const { return symbols_; }

  bool isSymbol(CSymbol2D::Type type) const {
    for (const auto &s : symbols_)
      if (s.type == type)
        return true;

    return false;
  }

  const CSymbol2D &getSymbol(CSymbol2D::Type type) const {
    for (const auto &s : symbols_)
      if (s.type == type)
        return s;

    assert(false);

    return symbols_[0];
  }

  void drawSymbol(CSymbol2D::Type type, CSymbol2DRenderer *renderer) const {
    const CSymbol2D &s = getSymbol(type);

    bool connect = false;

    for (const auto &l : s.lines) {
      if (! connect)
        renderer->moveTo(l.x1, l.y1);
      else
        renderer->lineTo(l.x1, l.y1);

      if      (l.connect == CSymbol2D::Connect::CLOSE) {
        renderer->closePath();

        renderer->stroke();

        connect = false;
      }
      else if (l.connect == CSymbol2D::Connect::STROKE) {
        renderer->lineTo(l.x2, l.y2);

        renderer->stroke();

        connect = false;
      }
      else
        connect = true;
    }
  }

 private:
  Symbols symbols_;
};

CSymbol2DList symbols({
  { CSymbol2D::Type::CROSS    , {{-1.0, -1.0,  1.0,  1.0, CSymbol2D::Connect::STROKE},
                                 {-1.0,  1.0,  1.0, -1.0, CSymbol2D::Connect::STROKE}} },
  { CSymbol2D::Type::PLUS     , {{ 0.0, -1.0,  0.0,  1.0, CSymbol2D::Connect::STROKE},
                                 {-1.0,  0.0,  1.0,  0.0, CSymbol2D::Connect::STROKE}} },
  { CSymbol2D::Type::Y        , {{ 0.0,  0.0,  0.0, -1.0, CSymbol2D::Connect::STROKE},
                                 { 0.0,  0.0,  1.0,  1.0, CSymbol2D::Connect::STROKE},
                                 { 0.0,  0.0, -1.0,  1.0, CSymbol2D::Connect::STROKE}} },
  { CSymbol2D::Type::TRIANGLE , {{ 0.0,  1.0, -1.0, -1.0, CSymbol2D::Connect::LINE  },
                                 {-1.0, -1.0,  1.0, -1.0, CSymbol2D::Connect::LINE  },
                                 { 1.0, -1.0,  0.0,  1.0, CSymbol2D::Connect::CLOSE },
                                 { 0.0,  0.0,  0.0,  1.0, CSymbol2D::Connect::STROKE}} },
  { CSymbol2D::Type::ITRIANGLE, {{ 0.0, -1.0, -1.0,  1.0, CSymbol2D::Connect::LINE  },
                                 {-1.0,  1.0,  1.0,  1.0, CSymbol2D::Connect::LINE  },
                                 { 1.0,  1.0,  0.0, -1.0, CSymbol2D::Connect::CLOSE },
                                 { 0.0,  0.0,  0.0, -1.0, CSymbol2D::Connect::STROKE}} },
  { CSymbol2D::Type::BOX      , {{-1.0,  1.0,  1.0,  1.0, CSymbol2D::Connect::LINE  },
                                 { 1.0,  1.0,  1.0, -1.0, CSymbol2D::Connect::LINE  },
                                 { 1.0, -1.0, -1.0, -1.0, CSymbol2D::Connect::LINE  },
                                 {-1.0, -1.0, -1.0,  1.0, CSymbol2D::Connect::CLOSE },
                                 { 0.0,  0.0,  0.0,  1.0, CSymbol2D::Connect::STROKE}} },
  { CSymbol2D::Type::DIAMOND  , {{-1.0,  0.0,  0.0,  1.0, CSymbol2D::Connect::LINE  },
                                 { 0.0,  1.0,  1.0,  0.0, CSymbol2D::Connect::LINE  },
                                 { 1.0,  0.0,  0.0, -1.0, CSymbol2D::Connect::LINE  },
                                 { 0.0, -1.0, -1.0,  0.0, CSymbol2D::Connect::CLOSE },
                                 { 0.0,  0.0,  0.0,  1.0, CSymbol2D::Connect::STROKE}} },
  { CSymbol2D::Type::STAR     , {{ 0.0,  0.0,  0.0,  1.0, CSymbol2D::Connect::STROKE},
                                 { 0.0,  0.0, -1.0,  0.0, CSymbol2D::Connect::STROKE},
                                 { 0.0,  0.0,  1.0,  0.0, CSymbol2D::Connect::STROKE},
                                 { 0.0,  0.0, -1.0, -1.0, CSymbol2D::Connect::STROKE},
                                 { 0.0,  0.0,  1.0, -1.0, CSymbol2D::Connect::STROKE}} },
  { CSymbol2D::Type::STAR1    , {{-1.0,  0.0,  1.0,  0.0, CSymbol2D::Connect::STROKE},
                                 { 0.0, -1.0,  0.0,  1.0, CSymbol2D::Connect::STROKE},
                                 {-1.0, -1.0,  1.0,  1.0, CSymbol2D::Connect::STROKE},
                                 {-1.0,  1.0,  1.0, -1.0, CSymbol2D::Connect::STROKE}} },
  { CSymbol2D::Type::PENTAGON , {{  0.000000, -1.000000,  0.951057, -0.309017,
                                  CSymbol2D::Connect::LINE  },
                                 {  0.951057, -0.309017,  0.587785,  0.809017,
                                  CSymbol2D::Connect::LINE  },
                                 {  0.587785,  0.809017, -0.587785,  0.809017,
                                  CSymbol2D::Connect::LINE  },
                                 { -0.587785,  0.809017, -0.951057, -0.309017,
                                  CSymbol2D::Connect::LINE  },
                                 { -0.951057, -0.309017,  0.000000, -1.000000,
                                  CSymbol2D::Connect::CLOSE },
                                 {  0.000000, -1.000000,  0.000000,  0.000000,
                                  CSymbol2D::Connect::STROKE}} }
});

//---

bool
CSymbol2DMgr::
isSymbol(CSymbol2D::Type type)
{
  return symbols.isSymbol(type);
}

const CSymbol2D &
CSymbol2DMgr::
getSymbol(CSymbol2D::Type type)
{
  return symbols.getSymbol(type);
}

void
CSymbol2DMgr::
drawSymbol(CSymbol2D::Type type, CSymbol2DRenderer *renderer)
{
  return symbols.drawSymbol(type, renderer);
}

std::string
CSymbol2DMgr::
typeToName(CSymbol2D::Type type)
{
  switch (type) {
    case CSymbol2D::Type::CROSS:     return "cross";
    case CSymbol2D::Type::PLUS:      return "plus";
    case CSymbol2D::Type::Y:         return "y";
    case CSymbol2D::Type::TRIANGLE:  return "triangle";
    case CSymbol2D::Type::ITRIANGLE: return "itriangle";
    case CSymbol2D::Type::BOX:       return "box";
    case CSymbol2D::Type::DIAMOND:   return "diamond";
    case CSymbol2D::Type::STAR:      return "star";
    case CSymbol2D::Type::STAR1:     return "star1";
    case CSymbol2D::Type::PENTAGON:  return "pentagon";
    default:                         return "none";
  }
}

CSymbol2D::Type
CSymbol2DMgr::
nameToType(const std::string &str)
{
  std::string lstr = CStrUtil::toLower(str);

  if (lstr == "cross"    ) return CSymbol2D::Type::CROSS;
  if (lstr == "plus"     ) return CSymbol2D::Type::PLUS;
  if (lstr == "y"        ) return CSymbol2D::Type::Y;
  if (lstr == "triangle" ) return CSymbol2D::Type::TRIANGLE;
  if (lstr == "itriangle") return CSymbol2D::Type::ITRIANGLE;
  if (lstr == "box"      ) return CSymbol2D::Type::BOX;
  if (lstr == "diamond"  ) return CSymbol2D::Type::DIAMOND;
  if (lstr == "star"     ) return CSymbol2D::Type::STAR;
  if (lstr == "star1"    ) return CSymbol2D::Type::STAR1;
  if (lstr == "pentagon" ) return CSymbol2D::Type::PENTAGON;

  return CSymbol2D::Type::NONE;
}

//---

void
CSymbol2DRenderer::
drawSymbol(CSymbol2D::Type type)
{
  CSymbol2DMgr::drawSymbol(type, this);
}
