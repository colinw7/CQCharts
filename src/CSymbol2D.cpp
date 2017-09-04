#include <CSymbol2D.h>
#include <CStrUtil.h>
#include <cmath>
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
    if (type == CSymbol2D::Type::CIRCLE) {
      renderer->strokeCircle(0, 0, 1);
      return;
    }

    //---

    const CSymbol2D &s = getSymbol(type);

    double w = renderer->lineWidth();

    bool connect = false;

    if (w <= 0.0) {
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
    else {
      for (const auto &l : s.lines) {
        drawWideLine(renderer, l, w);
      }
    }
  }

  void strokeSymbol(CSymbol2D::Type type, CSymbol2DRenderer *renderer) const {
    fillStrokeSymbol(type, renderer, false);
  }

  void fillSymbol(CSymbol2D::Type type, CSymbol2DRenderer *renderer) const {
    fillStrokeSymbol(type, renderer, true);
  }

  void fillStrokeSymbol(CSymbol2D::Type type, CSymbol2DRenderer *renderer, bool fill) const {
    if (type == CSymbol2D::Type::CIRCLE) {
      if (fill)
        renderer->fillCircle(0, 0, 1);
      else
        renderer->strokeCircle(0, 0, 1);

      return;
    }

    //---

    const CSymbol2D &s = getSymbol(type);

    bool connect = false;

    for (const auto &l : s.fillLines) {
      if (! connect)
        renderer->moveTo(l.x1, l.y1);
      else
        renderer->lineTo(l.x1, l.y1);

      if      (l.connect == CSymbol2D::Connect::CLOSE) {
        renderer->closePath();

        if (fill)
          renderer->fill();
        else
          renderer->stroke();

        connect = false;
      }
      else if (l.connect == CSymbol2D::Connect::FILL) {
        renderer->lineTo(l.x2, l.y2);

        if (fill)
          renderer->fill();
        else
          renderer->stroke();

        connect = false;
      }
      else
        connect = true;
    }
  }

  void drawWideLine(CSymbol2DRenderer *renderer, const CSymbol2D::Line &l, double w) const {
    auto addWidthToPoint= [&](const CSymbol2D::Point &p, double a, double w,
                              CSymbol2D::Point &p1, CSymbol2D::Point &p2) {
      double dx = w*sin(a)/2.0;
      double dy = w*cos(a)/2.0;

      p1.x = p.x + dx;
      p1.y = p.y - dy;
      p2.x = p.x - dx;
      p2.y = p.y + dy;
    };

    double x21 = l.x2 - l.x1;
    double y21 = l.y2 - l.y1;

    if (x21 == 0.0 && y21 == 0.0)
      return;

    double a = atan2(y21, x21);

    CSymbol2D::Point p[4];

    addWidthToPoint(CSymbol2D::Point(l.x1, l.y1), a, w, p[0], p[3]);
    addWidthToPoint(CSymbol2D::Point(l.x2, l.y2), a, w, p[1], p[2]);

    renderer->moveTo(p[0].x, p[0].y);
    renderer->lineTo(p[1].x, p[1].y);
    renderer->lineTo(p[2].x, p[2].y);
    renderer->lineTo(p[3].x, p[3].y);

    renderer->closePath();

    renderer->fill();
  }

 private:
  Symbols symbols_;
};

static double cw1 =  0.2828430; // 0.4/sqrt(2)
static double cw2 =  0.7171570; // 1-0.4/sqrt(2)
static double cw3 = -0.0828427; // line intersect for Y

CSymbol2DList symbols({
  { CSymbol2D::Type::CROSS    ,
    {{-1.0, -1.0,  1.0,  1.0, CSymbol2D::Connect::STROKE},
     {-1.0,  1.0,  1.0, -1.0, CSymbol2D::Connect::STROKE}},
    {{-cw2, -1.0,  0.0, -cw1, CSymbol2D::Connect::LINE  },
     { 0.0, -cw1,  cw2, -1.0, CSymbol2D::Connect::LINE  },
     { cw2, -1.0,  1.0, -cw2, CSymbol2D::Connect::LINE  },
     { 1.0, -cw2,  cw1,  0.0, CSymbol2D::Connect::LINE  },
     { cw1,  0.0,  1.0,  cw2, CSymbol2D::Connect::LINE  },
     { 1.0,  cw2,  cw2,  1.0, CSymbol2D::Connect::LINE  },
     { cw2,  1.0,  0.0,  cw1, CSymbol2D::Connect::LINE  },
     { 0.0,  cw1, -cw2,  1.0, CSymbol2D::Connect::LINE  },
     {-cw2,  1.0, -1.0,  cw2, CSymbol2D::Connect::LINE  },
     {-1.0,  cw2, -cw1,  0.0, CSymbol2D::Connect::LINE  },
     {-cw1,  0.0, -1.0, -cw2, CSymbol2D::Connect::LINE  },
     {-1.0, -cw2, -cw2, -1.0, CSymbol2D::Connect::FILL  }} },
  { CSymbol2D::Type::PLUS     ,
    {{ 0.0, -1.0,  0.0,  1.0, CSymbol2D::Connect::STROKE},
     {-1.0,  0.0,  1.0,  0.0, CSymbol2D::Connect::STROKE}},
    {{-0.2, -1.0,  0.2, -1.0, CSymbol2D::Connect::LINE  },
     { 0.2, -1.0,  0.2, -0.2, CSymbol2D::Connect::LINE  },
     { 0.2, -0.2,  1.0, -0.2, CSymbol2D::Connect::LINE  },
     { 1.0, -0.2,  1.0,  0.2, CSymbol2D::Connect::LINE  },
     { 1.0,  0.2,  0.2,  0.2, CSymbol2D::Connect::LINE  },
     { 0.2,  0.2,  0.2,  1.0, CSymbol2D::Connect::LINE  },
     { 0.2,  1.0, -0.2,  1.0, CSymbol2D::Connect::LINE  },
     {-0.2,  1.0, -0.2,  0.2, CSymbol2D::Connect::LINE  },
     {-0.2,  0.2, -1.0,  0.2, CSymbol2D::Connect::LINE  },
     {-1.0,  0.2, -1.0, -0.2, CSymbol2D::Connect::LINE  },
     {-1.0, -0.2, -0.2, -0.2, CSymbol2D::Connect::LINE  },
     {-0.2, -0.2, -0.2, -1.0, CSymbol2D::Connect::FILL  }} },
  { CSymbol2D::Type::Y        ,
    {{ 0.0,  0.0,  0.0, -1.0, CSymbol2D::Connect::STROKE},
     { 0.0,  0.0,  1.0,  1.0, CSymbol2D::Connect::STROKE},
     { 0.0,  0.0, -1.0,  1.0, CSymbol2D::Connect::STROKE}},
    {{-0.2, -1.0,  0.2, -1.0, CSymbol2D::Connect::STROKE},
     { 0.2, -1.0,  0.2,  cw3, CSymbol2D::Connect::STROKE},
     { 0.2,  cw3,  1.0,  cw2, CSymbol2D::Connect::STROKE},
     { 1.0,  cw2,  cw2,  1.0, CSymbol2D::Connect::STROKE},
     { cw2,  1.0,  0.0,  cw1, CSymbol2D::Connect::STROKE},
     { 0.0,  cw1, -cw2,  1.0, CSymbol2D::Connect::STROKE},
     {-cw2,  1.0, -1.0,  cw2, CSymbol2D::Connect::STROKE},
     {-1.0,  cw2, -0.2,  cw3, CSymbol2D::Connect::STROKE},
     {-0.2,  cw3, -0.2, -1.0, CSymbol2D::Connect::FILL  }} },
  { CSymbol2D::Type::TRIANGLE ,
    {{ 0.0,  1.0, -1.0, -1.0, CSymbol2D::Connect::LINE  },
     {-1.0, -1.0,  1.0, -1.0, CSymbol2D::Connect::LINE  },
     { 1.0, -1.0,  0.0,  1.0, CSymbol2D::Connect::CLOSE },
     { 0.0,  0.0,  0.0,  1.0, CSymbol2D::Connect::STROKE}},
    {{ 0.0,  1.0, -1.0, -1.0, CSymbol2D::Connect::LINE  },
     {-1.0, -1.0,  1.0, -1.0, CSymbol2D::Connect::LINE  },
     { 1.0, -1.0,  0.0,  1.0, CSymbol2D::Connect::FILL  }} },
  { CSymbol2D::Type::ITRIANGLE,
    {{ 0.0, -1.0, -1.0,  1.0, CSymbol2D::Connect::LINE  },
     {-1.0,  1.0,  1.0,  1.0, CSymbol2D::Connect::LINE  },
     { 1.0,  1.0,  0.0, -1.0, CSymbol2D::Connect::CLOSE },
     { 0.0,  0.0,  0.0, -1.0, CSymbol2D::Connect::STROKE}},
    {{ 0.0, -1.0, -1.0,  1.0, CSymbol2D::Connect::LINE  },
     {-1.0,  1.0,  1.0,  1.0, CSymbol2D::Connect::LINE  },
     { 1.0,  1.0,  0.0, -1.0, CSymbol2D::Connect::FILL  }} },
  { CSymbol2D::Type::BOX      ,
    {{-1.0,  1.0,  1.0,  1.0, CSymbol2D::Connect::LINE  },
     { 1.0,  1.0,  1.0, -1.0, CSymbol2D::Connect::LINE  },
     { 1.0, -1.0, -1.0, -1.0, CSymbol2D::Connect::LINE  },
     {-1.0, -1.0, -1.0,  1.0, CSymbol2D::Connect::CLOSE },
     { 0.0,  0.0,  0.0,  1.0, CSymbol2D::Connect::STROKE}},
    {{-1.0,  1.0,  1.0,  1.0, CSymbol2D::Connect::LINE  },
     { 1.0,  1.0,  1.0, -1.0, CSymbol2D::Connect::LINE  },
     { 1.0, -1.0, -1.0, -1.0, CSymbol2D::Connect::LINE  },
     {-1.0, -1.0, -1.0,  1.0, CSymbol2D::Connect::FILL  }} },
  { CSymbol2D::Type::DIAMOND  ,
    {{-1.0,  0.0,  0.0,  1.0, CSymbol2D::Connect::LINE  },
     { 0.0,  1.0,  1.0,  0.0, CSymbol2D::Connect::LINE  },
     { 1.0,  0.0,  0.0, -1.0, CSymbol2D::Connect::LINE  },
     { 0.0, -1.0, -1.0,  0.0, CSymbol2D::Connect::CLOSE },
     { 0.0,  0.0,  0.0,  1.0, CSymbol2D::Connect::STROKE}},
    {{-1.0,  0.0,  0.0,  1.0, CSymbol2D::Connect::LINE  },
     { 0.0,  1.0,  1.0,  0.0, CSymbol2D::Connect::LINE  },
     { 1.0,  0.0,  0.0, -1.0, CSymbol2D::Connect::LINE  },
     { 0.0, -1.0, -1.0,  0.0, CSymbol2D::Connect::FILL  }} },
  { CSymbol2D::Type::STAR     ,
    {{ 0.0,  0.0,  0.0,  1.0, CSymbol2D::Connect::STROKE},
     { 0.0,  0.0, -1.0,  0.4, CSymbol2D::Connect::STROKE},
     { 0.0,  0.0,  1.0,  0.4, CSymbol2D::Connect::STROKE},
     { 0.0,  0.0, -1.0, -1.0, CSymbol2D::Connect::STROKE},
     { 0.0,  0.0,  1.0, -1.0, CSymbol2D::Connect::STROKE}},
    {{ 0.0     ,  1       , -0.293893,  0.404508, CSymbol2D::Connect::LINE},
     {-0.293893,  0.404508, -0.951057,  0.309017, CSymbol2D::Connect::LINE},
     {-0.951057,  0.309017, -0.475528, -0.154508, CSymbol2D::Connect::LINE},
     {-0.475528, -0.154508, -0.587785, -0.809017, CSymbol2D::Connect::LINE},
     {-0.587785, -0.809017,  0.0     , -0.5     , CSymbol2D::Connect::LINE},
     { 0.0     , -0.5     ,  0.587785, -0.809017, CSymbol2D::Connect::LINE},
     { 0.587785, -0.809017,  0.475528, -0.154508, CSymbol2D::Connect::LINE},
     { 0.475528, -0.154508,  0.951057,  0.309017, CSymbol2D::Connect::LINE},
     { 0.951057,  0.309017,  0.293893,  0.404508, CSymbol2D::Connect::LINE},
     { 0.293893,  0.404508,  0.0     ,  1       , CSymbol2D::Connect::FILL}} },
  { CSymbol2D::Type::STAR1    ,
    {{-1.0,  0.0,  1.0,  0.0, CSymbol2D::Connect::STROKE},
     { 0.0, -1.0,  0.0,  1.0, CSymbol2D::Connect::STROKE},
     {-1.0, -1.0,  1.0,  1.0, CSymbol2D::Connect::STROKE},
     {-1.0,  1.0,  1.0, -1.0, CSymbol2D::Connect::STROKE}},
    {{ 0.0     ,  1.0     , -0.25    ,  0.433013, CSymbol2D::Connect::LINE},
     {-0.25    ,  0.433013, -0.866025,  0.5     , CSymbol2D::Connect::LINE},
     {-0.866025,  0.5     , -0.5     ,  0.0     , CSymbol2D::Connect::LINE},
     {-0.5     ,  0.0     , -0.866025, -0.5     , CSymbol2D::Connect::LINE},
     {-0.866025, -0.5     , -0.25    , -0.433013, CSymbol2D::Connect::LINE},
     {-0.25    , -0.433013,  0.0     , -1.0     , CSymbol2D::Connect::LINE},
     { 0.0     , -1.0     ,  0.25    , -0.433013, CSymbol2D::Connect::LINE},
     { 0.25    , -0.433013,  0.866025, -0.5     , CSymbol2D::Connect::LINE},
     { 0.866025, -0.5     ,  0.5     ,  0.0     , CSymbol2D::Connect::LINE},
     { 0.5     ,  0.0     ,  0.866025,  0.5     , CSymbol2D::Connect::LINE},
     { 0.866025,  0.5     ,  0.25    ,  0.433013, CSymbol2D::Connect::LINE},
     { 0.25    ,  0.433013,  0.0     ,  1.0     , CSymbol2D::Connect::FILL}} },
  { CSymbol2D::Type::PENTAGON ,
    {{  0.000000, -1.000000,  0.951057, -0.309017, CSymbol2D::Connect::LINE  },
     {  0.951057, -0.309017,  0.587785,  0.809017, CSymbol2D::Connect::LINE  },
     {  0.587785,  0.809017, -0.587785,  0.809017, CSymbol2D::Connect::LINE  },
     { -0.587785,  0.809017, -0.951057, -0.309017, CSymbol2D::Connect::LINE  },
     { -0.951057, -0.309017,  0.000000, -1.000000, CSymbol2D::Connect::CLOSE },
     {  0.000000, -1.000000,  0.000000,  0.000000, CSymbol2D::Connect::STROKE}},
    {{  0.000000, -1.000000,  0.951057, -0.309017, CSymbol2D::Connect::LINE  },
     {  0.951057, -0.309017,  0.587785,  0.809017, CSymbol2D::Connect::LINE  },
     {  0.587785,  0.809017, -0.587785,  0.809017, CSymbol2D::Connect::LINE  },
     { -0.587785,  0.809017, -0.951057, -0.309017, CSymbol2D::Connect::LINE  },
     { -0.951057, -0.309017,  0.000000, -1.000000, CSymbol2D::Connect::FILL  }} },
   { CSymbol2D::Type::IPENTAGON ,
    {{  0.000000,  1.000000,  0.951057,  0.309017, CSymbol2D::Connect::LINE  },
     {  0.951057,  0.309017,  0.587785, -0.809017, CSymbol2D::Connect::LINE  },
     {  0.587785, -0.809017, -0.587785, -0.809017, CSymbol2D::Connect::LINE  },
     { -0.587785, -0.809017, -0.951057,  0.309017, CSymbol2D::Connect::LINE  },
     { -0.951057,  0.309017,  0.000000,  1.000000, CSymbol2D::Connect::CLOSE },
     {  0.000000,  1.000000,  0.000000, -0.000000, CSymbol2D::Connect::STROKE}},
    {{  0.000000,  1.000000,  0.951057,  0.309017, CSymbol2D::Connect::LINE  },
     {  0.951057,  0.309017,  0.587785, -0.809017, CSymbol2D::Connect::LINE  },
     {  0.587785, -0.809017, -0.587785, -0.809017, CSymbol2D::Connect::LINE  },
     { -0.587785, -0.809017, -0.951057,  0.309017, CSymbol2D::Connect::LINE  },
     { -0.951057,  0.309017,  0.000000,  1.000000, CSymbol2D::Connect::FILL  }} }
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

void
CSymbol2DMgr::
strokeSymbol(CSymbol2D::Type type, CSymbol2DRenderer *renderer)
{
  return symbols.strokeSymbol(type, renderer);
}

void
CSymbol2DMgr::
fillSymbol(CSymbol2D::Type type, CSymbol2DRenderer *renderer)
{
  return symbols.fillSymbol(type, renderer);
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
    case CSymbol2D::Type::CIRCLE:    return "circle";
    case CSymbol2D::Type::PENTAGON:  return "pentagon";
    case CSymbol2D::Type::IPENTAGON: return "ipentagon";
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
  if (lstr == "circle"   ) return CSymbol2D::Type::CIRCLE;
  if (lstr == "pentagon" ) return CSymbol2D::Type::PENTAGON;
  if (lstr == "ipentagon") return CSymbol2D::Type::IPENTAGON;

  return CSymbol2D::Type::NONE;
}

//---

void
CSymbol2DRenderer::
drawSymbol(CSymbol2D::Type type)
{
  CSymbol2DMgr::drawSymbol(type, this);
}

void
CSymbol2DRenderer::
strokeSymbol(CSymbol2D::Type type)
{
  CSymbol2DMgr::strokeSymbol(type, this);
}

void
CSymbol2DRenderer::
fillSymbol(CSymbol2D::Type type)
{
  CSymbol2DMgr::fillSymbol(type, this);
}
