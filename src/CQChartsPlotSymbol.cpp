#include <CQChartsPlotSymbol.h>
#include <CQChartsPlot.h>
#include <QPainter>
#include <cmath>
#include <cassert>

class CQChartsPlotSymbolList {
 public:
  using Symbols = std::vector<CQChartsPlotSymbol>;

 public:
  CQChartsPlotSymbolList(std::initializer_list<CQChartsPlotSymbol> symbols) :
   symbols_(symbols) {
  }

  const Symbols &symbols() const { return symbols_; }

  bool isSymbol(CQChartsPlotSymbol::Type type) const {
    for (const auto &s : symbols_)
      if (s.type == type)
        return true;

    return false;
  }

  const CQChartsPlotSymbol &getSymbol(CQChartsPlotSymbol::Type type) const {
    for (const auto &s : symbols_)
      if (s.type == type)
        return s;

    assert(false);

    return symbols_[0];
  }

  void drawSymbol(CQChartsPlotSymbol::Type type, CQChartsPlotSymbolRenderer *painter) const {
    if (type == CQChartsPlotSymbol::Type::CIRCLE) {
      painter->strokeCircle(0, 0, 1);
      return;
    }

    //---

    const CQChartsPlotSymbol &s = getSymbol(type);

    double w = painter->lineWidth();

    bool connect = false;

    if (w <= 0.0) {
      for (const auto &l : s.lines) {
        if (! connect)
          painter->moveTo(l.x1, l.y1);
        else
          painter->lineTo(l.x1, l.y1);

        if      (l.connect == CQChartsPlotSymbol::Connect::CLOSE) {
          painter->closePath();

          painter->stroke();

          connect = false;
        }
        else if (l.connect == CQChartsPlotSymbol::Connect::STROKE) {
          painter->lineTo(l.x2, l.y2);

          painter->stroke();

          connect = false;
        }
        else
          connect = true;
      }
    }
    else {
      for (const auto &l : s.lines) {
        drawWideLine(painter, l, w);
      }
    }
  }

  void strokeSymbol(CQChartsPlotSymbol::Type type, CQChartsPlotSymbolRenderer *painter) const {
    fillStrokeSymbol(type, painter, false);
  }

  void fillSymbol(CQChartsPlotSymbol::Type type, CQChartsPlotSymbolRenderer *painter) const {
    fillStrokeSymbol(type, painter, true);
  }

  void fillStrokeSymbol(CQChartsPlotSymbol::Type type, CQChartsPlotSymbolRenderer *painter,
                        bool fill) const {
    if (type == CQChartsPlotSymbol::Type::CIRCLE) {
      if (fill)
        painter->fillCircle(0, 0, 1);
      else
        painter->strokeCircle(0, 0, 1);

      return;
    }

    //---

    const CQChartsPlotSymbol &s = getSymbol(type);

    bool connect = false;

    for (const auto &l : s.fillLines) {
      if (! connect)
        painter->moveTo(l.x1, l.y1);
      else
        painter->lineTo(l.x1, l.y1);

      if      (l.connect == CQChartsPlotSymbol::Connect::CLOSE) {
        painter->closePath();

        if (fill)
          painter->fill();
        else
          painter->stroke();

        connect = false;
      }
      else if (l.connect == CQChartsPlotSymbol::Connect::FILL) {
        painter->lineTo(l.x2, l.y2);

        if (fill)
          painter->fill();
        else
          painter->stroke();

        connect = false;
      }
      else
        connect = true;
    }
  }

  void drawWideLine(CQChartsPlotSymbolRenderer *painter, const CQChartsPlotSymbol::Line &l,
                    double w) const {
    auto addWidthToPoint= [&](const CQChartsPlotSymbol::Point &p, double a, double w,
                              CQChartsPlotSymbol::Point &p1, CQChartsPlotSymbol::Point &p2) {
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

    CQChartsPlotSymbol::Point p[4];

    addWidthToPoint(CQChartsPlotSymbol::Point(l.x1, l.y1), a, w, p[0], p[3]);
    addWidthToPoint(CQChartsPlotSymbol::Point(l.x2, l.y2), a, w, p[1], p[2]);

    painter->moveTo(p[0].x, p[0].y);
    painter->lineTo(p[1].x, p[1].y);
    painter->lineTo(p[2].x, p[2].y);
    painter->lineTo(p[3].x, p[3].y);

    painter->closePath();

    painter->fill();
  }

 private:
  Symbols symbols_;
};

static double cw1 =  0.2828430; // 0.4/sqrt(2)
static double cw2 =  0.7171570; // 1-0.4/sqrt(2)
static double cw3 = -0.0828427; // line intersect for Y

CQChartsPlotSymbolList symbols({
  { CQChartsPlotSymbol::Type::CROSS    ,
    {{-1.0, -1.0,  1.0,  1.0, CQChartsPlotSymbol::Connect::STROKE},
     {-1.0,  1.0,  1.0, -1.0, CQChartsPlotSymbol::Connect::STROKE}},
    {{-cw2, -1.0,  0.0, -cw1, CQChartsPlotSymbol::Connect::LINE  },
     { 0.0, -cw1,  cw2, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     { cw2, -1.0,  1.0, -cw2, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0, -cw2,  cw1,  0.0, CQChartsPlotSymbol::Connect::LINE  },
     { cw1,  0.0,  1.0,  cw2, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0,  cw2,  cw2,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     { cw2,  1.0,  0.0,  cw1, CQChartsPlotSymbol::Connect::LINE  },
     { 0.0,  cw1, -cw2,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     {-cw2,  1.0, -1.0,  cw2, CQChartsPlotSymbol::Connect::LINE  },
     {-1.0,  cw2, -cw1,  0.0, CQChartsPlotSymbol::Connect::LINE  },
     {-cw1,  0.0, -1.0, -cw2, CQChartsPlotSymbol::Connect::LINE  },
     {-1.0, -cw2, -cw2, -1.0, CQChartsPlotSymbol::Connect::FILL  }} },
  { CQChartsPlotSymbol::Type::PLUS     ,
    {{ 0.0, -1.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::STROKE},
     {-1.0,  0.0,  1.0,  0.0, CQChartsPlotSymbol::Connect::STROKE}},
    {{-0.2, -1.0,  0.2, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 0.2, -1.0,  0.2, -0.2, CQChartsPlotSymbol::Connect::LINE  },
     { 0.2, -0.2,  1.0, -0.2, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0, -0.2,  1.0,  0.2, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0,  0.2,  0.2,  0.2, CQChartsPlotSymbol::Connect::LINE  },
     { 0.2,  0.2,  0.2,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 0.2,  1.0, -0.2,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     {-0.2,  1.0, -0.2,  0.2, CQChartsPlotSymbol::Connect::LINE  },
     {-0.2,  0.2, -1.0,  0.2, CQChartsPlotSymbol::Connect::LINE  },
     {-1.0,  0.2, -1.0, -0.2, CQChartsPlotSymbol::Connect::LINE  },
     {-1.0, -0.2, -0.2, -0.2, CQChartsPlotSymbol::Connect::LINE  },
     {-0.2, -0.2, -0.2, -1.0, CQChartsPlotSymbol::Connect::FILL  }} },
  { CQChartsPlotSymbol::Type::Y        ,
    {{ 0.0,  0.0,  0.0, -1.0, CQChartsPlotSymbol::Connect::STROKE},
     { 0.0,  0.0,  1.0,  1.0, CQChartsPlotSymbol::Connect::STROKE},
     { 0.0,  0.0, -1.0,  1.0, CQChartsPlotSymbol::Connect::STROKE}},
    {{-0.2, -1.0,  0.2, -1.0, CQChartsPlotSymbol::Connect::STROKE},
     { 0.2, -1.0,  0.2,  cw3, CQChartsPlotSymbol::Connect::STROKE},
     { 0.2,  cw3,  1.0,  cw2, CQChartsPlotSymbol::Connect::STROKE},
     { 1.0,  cw2,  cw2,  1.0, CQChartsPlotSymbol::Connect::STROKE},
     { cw2,  1.0,  0.0,  cw1, CQChartsPlotSymbol::Connect::STROKE},
     { 0.0,  cw1, -cw2,  1.0, CQChartsPlotSymbol::Connect::STROKE},
     {-cw2,  1.0, -1.0,  cw2, CQChartsPlotSymbol::Connect::STROKE},
     {-1.0,  cw2, -0.2,  cw3, CQChartsPlotSymbol::Connect::STROKE},
     {-0.2,  cw3, -0.2, -1.0, CQChartsPlotSymbol::Connect::FILL  }} },
  { CQChartsPlotSymbol::Type::TRIANGLE ,
    {{ 0.0,  1.0, -1.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     {-1.0, -1.0,  1.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0, -1.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::CLOSE },
     { 0.0,  0.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::STROKE}},
    {{ 0.0,  1.0, -1.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     {-1.0, -1.0,  1.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0, -1.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::FILL  }} },
  { CQChartsPlotSymbol::Type::ITRIANGLE,
    {{ 0.0, -1.0, -1.0,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     {-1.0,  1.0,  1.0,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0,  1.0,  0.0, -1.0, CQChartsPlotSymbol::Connect::CLOSE },
     { 0.0,  0.0,  0.0, -1.0, CQChartsPlotSymbol::Connect::STROKE}},
    {{ 0.0, -1.0, -1.0,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     {-1.0,  1.0,  1.0,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0,  1.0,  0.0, -1.0, CQChartsPlotSymbol::Connect::FILL  }} },
  { CQChartsPlotSymbol::Type::BOX      ,
    {{-1.0,  1.0,  1.0,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0,  1.0,  1.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0, -1.0, -1.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     {-1.0, -1.0, -1.0,  1.0, CQChartsPlotSymbol::Connect::CLOSE },
     { 0.0,  0.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::STROKE}},
    {{-1.0,  1.0,  1.0,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0,  1.0,  1.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0, -1.0, -1.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     {-1.0, -1.0, -1.0,  1.0, CQChartsPlotSymbol::Connect::FILL  }} },
  { CQChartsPlotSymbol::Type::DIAMOND  ,
    {{-1.0,  0.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 0.0,  1.0,  1.0,  0.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0,  0.0,  0.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 0.0, -1.0, -1.0,  0.0, CQChartsPlotSymbol::Connect::CLOSE },
     { 0.0,  0.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::STROKE}},
    {{-1.0,  0.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 0.0,  1.0,  1.0,  0.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0,  0.0,  0.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 0.0, -1.0, -1.0,  0.0, CQChartsPlotSymbol::Connect::FILL  }} },
  { CQChartsPlotSymbol::Type::STAR     ,
    {{ 0.0,  0.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::STROKE},
     { 0.0,  0.0, -1.0,  0.4, CQChartsPlotSymbol::Connect::STROKE},
     { 0.0,  0.0,  1.0,  0.4, CQChartsPlotSymbol::Connect::STROKE},
     { 0.0,  0.0, -1.0, -1.0, CQChartsPlotSymbol::Connect::STROKE},
     { 0.0,  0.0,  1.0, -1.0, CQChartsPlotSymbol::Connect::STROKE}},
    {{ 0.0     ,  1       , -0.293893,  0.404508, CQChartsPlotSymbol::Connect::LINE},
     {-0.293893,  0.404508, -0.951057,  0.309017, CQChartsPlotSymbol::Connect::LINE},
     {-0.951057,  0.309017, -0.475528, -0.154508, CQChartsPlotSymbol::Connect::LINE},
     {-0.475528, -0.154508, -0.587785, -0.809017, CQChartsPlotSymbol::Connect::LINE},
     {-0.587785, -0.809017,  0.0     , -0.5     , CQChartsPlotSymbol::Connect::LINE},
     { 0.0     , -0.5     ,  0.587785, -0.809017, CQChartsPlotSymbol::Connect::LINE},
     { 0.587785, -0.809017,  0.475528, -0.154508, CQChartsPlotSymbol::Connect::LINE},
     { 0.475528, -0.154508,  0.951057,  0.309017, CQChartsPlotSymbol::Connect::LINE},
     { 0.951057,  0.309017,  0.293893,  0.404508, CQChartsPlotSymbol::Connect::LINE},
     { 0.293893,  0.404508,  0.0     ,  1       , CQChartsPlotSymbol::Connect::FILL}} },
  { CQChartsPlotSymbol::Type::STAR1    ,
    {{-1.0,  0.0,  1.0,  0.0, CQChartsPlotSymbol::Connect::STROKE},
     { 0.0, -1.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::STROKE},
     {-1.0, -1.0,  1.0,  1.0, CQChartsPlotSymbol::Connect::STROKE},
     {-1.0,  1.0,  1.0, -1.0, CQChartsPlotSymbol::Connect::STROKE}},
    {{ 0.0     ,  1.0     , -0.25    ,  0.433013, CQChartsPlotSymbol::Connect::LINE},
     {-0.25    ,  0.433013, -0.866025,  0.5     , CQChartsPlotSymbol::Connect::LINE},
     {-0.866025,  0.5     , -0.5     ,  0.0     , CQChartsPlotSymbol::Connect::LINE},
     {-0.5     ,  0.0     , -0.866025, -0.5     , CQChartsPlotSymbol::Connect::LINE},
     {-0.866025, -0.5     , -0.25    , -0.433013, CQChartsPlotSymbol::Connect::LINE},
     {-0.25    , -0.433013,  0.0     , -1.0     , CQChartsPlotSymbol::Connect::LINE},
     { 0.0     , -1.0     ,  0.25    , -0.433013, CQChartsPlotSymbol::Connect::LINE},
     { 0.25    , -0.433013,  0.866025, -0.5     , CQChartsPlotSymbol::Connect::LINE},
     { 0.866025, -0.5     ,  0.5     ,  0.0     , CQChartsPlotSymbol::Connect::LINE},
     { 0.5     ,  0.0     ,  0.866025,  0.5     , CQChartsPlotSymbol::Connect::LINE},
     { 0.866025,  0.5     ,  0.25    ,  0.433013, CQChartsPlotSymbol::Connect::LINE},
     { 0.25    ,  0.433013,  0.0     ,  1.0     , CQChartsPlotSymbol::Connect::FILL}} },
  { CQChartsPlotSymbol::Type::PENTAGON ,
    {{  0.000000, -1.000000,  0.951057, -0.309017, CQChartsPlotSymbol::Connect::LINE  },
     {  0.951057, -0.309017,  0.587785,  0.809017, CQChartsPlotSymbol::Connect::LINE  },
     {  0.587785,  0.809017, -0.587785,  0.809017, CQChartsPlotSymbol::Connect::LINE  },
     { -0.587785,  0.809017, -0.951057, -0.309017, CQChartsPlotSymbol::Connect::LINE  },
     { -0.951057, -0.309017,  0.000000, -1.000000, CQChartsPlotSymbol::Connect::CLOSE },
     {  0.000000, -1.000000,  0.000000,  0.000000, CQChartsPlotSymbol::Connect::STROKE}},
    {{  0.000000, -1.000000,  0.951057, -0.309017, CQChartsPlotSymbol::Connect::LINE  },
     {  0.951057, -0.309017,  0.587785,  0.809017, CQChartsPlotSymbol::Connect::LINE  },
     {  0.587785,  0.809017, -0.587785,  0.809017, CQChartsPlotSymbol::Connect::LINE  },
     { -0.587785,  0.809017, -0.951057, -0.309017, CQChartsPlotSymbol::Connect::LINE  },
     { -0.951057, -0.309017,  0.000000, -1.000000, CQChartsPlotSymbol::Connect::FILL  }} },
   { CQChartsPlotSymbol::Type::IPENTAGON ,
    {{  0.000000,  1.000000,  0.951057,  0.309017, CQChartsPlotSymbol::Connect::LINE  },
     {  0.951057,  0.309017,  0.587785, -0.809017, CQChartsPlotSymbol::Connect::LINE  },
     {  0.587785, -0.809017, -0.587785, -0.809017, CQChartsPlotSymbol::Connect::LINE  },
     { -0.587785, -0.809017, -0.951057,  0.309017, CQChartsPlotSymbol::Connect::LINE  },
     { -0.951057,  0.309017,  0.000000,  1.000000, CQChartsPlotSymbol::Connect::CLOSE },
     {  0.000000,  1.000000,  0.000000, -0.000000, CQChartsPlotSymbol::Connect::STROKE}},
    {{  0.000000,  1.000000,  0.951057,  0.309017, CQChartsPlotSymbol::Connect::LINE  },
     {  0.951057,  0.309017,  0.587785, -0.809017, CQChartsPlotSymbol::Connect::LINE  },
     {  0.587785, -0.809017, -0.587785, -0.809017, CQChartsPlotSymbol::Connect::LINE  },
     { -0.587785, -0.809017, -0.951057,  0.309017, CQChartsPlotSymbol::Connect::LINE  },
     { -0.951057,  0.309017,  0.000000,  1.000000, CQChartsPlotSymbol::Connect::FILL  }} }
});

//---

bool
CQChartsPlotSymbolMgr::
isSymbol(CQChartsPlotSymbol::Type type)
{
  return symbols.isSymbol(type);
}

const CQChartsPlotSymbol &
CQChartsPlotSymbolMgr::
getSymbol(CQChartsPlotSymbol::Type type)
{
  return symbols.getSymbol(type);
}

void
CQChartsPlotSymbolMgr::
drawSymbol(CQChartsPlotSymbol::Type type, CQChartsPlotSymbolRenderer *painter)
{
  return symbols.drawSymbol(type, painter);
}

void
CQChartsPlotSymbolMgr::
strokeSymbol(CQChartsPlotSymbol::Type type, CQChartsPlotSymbolRenderer *painter)
{
  return symbols.strokeSymbol(type, painter);
}

void
CQChartsPlotSymbolMgr::
fillSymbol(CQChartsPlotSymbol::Type type, CQChartsPlotSymbolRenderer *painter)
{
  return symbols.fillSymbol(type, painter);
}

QString
CQChartsPlotSymbolMgr::
typeToName(CQChartsPlotSymbol::Type type)
{
  switch (type) {
    case CQChartsPlotSymbol::Type::CROSS:     return "cross";
    case CQChartsPlotSymbol::Type::PLUS:      return "plus";
    case CQChartsPlotSymbol::Type::Y:         return "y";
    case CQChartsPlotSymbol::Type::TRIANGLE:  return "triangle";
    case CQChartsPlotSymbol::Type::ITRIANGLE: return "itriangle";
    case CQChartsPlotSymbol::Type::BOX:       return "box";
    case CQChartsPlotSymbol::Type::DIAMOND:   return "diamond";
    case CQChartsPlotSymbol::Type::STAR:      return "star";
    case CQChartsPlotSymbol::Type::STAR1:     return "star1";
    case CQChartsPlotSymbol::Type::CIRCLE:    return "circle";
    case CQChartsPlotSymbol::Type::PENTAGON:  return "pentagon";
    case CQChartsPlotSymbol::Type::IPENTAGON: return "ipentagon";
    default:                         return "none";
  }
}

CQChartsPlotSymbol::Type
CQChartsPlotSymbolMgr::
nameToType(const QString &str)
{
  QString lstr = str.toLower();

  if (lstr == "cross"    ) return CQChartsPlotSymbol::Type::CROSS;
  if (lstr == "plus"     ) return CQChartsPlotSymbol::Type::PLUS;
  if (lstr == "y"        ) return CQChartsPlotSymbol::Type::Y;
  if (lstr == "triangle" ) return CQChartsPlotSymbol::Type::TRIANGLE;
  if (lstr == "itriangle") return CQChartsPlotSymbol::Type::ITRIANGLE;
  if (lstr == "box"      ) return CQChartsPlotSymbol::Type::BOX;
  if (lstr == "diamond"  ) return CQChartsPlotSymbol::Type::DIAMOND;
  if (lstr == "star"     ) return CQChartsPlotSymbol::Type::STAR;
  if (lstr == "star1"    ) return CQChartsPlotSymbol::Type::STAR1;
  if (lstr == "circle"   ) return CQChartsPlotSymbol::Type::CIRCLE;
  if (lstr == "pentagon" ) return CQChartsPlotSymbol::Type::PENTAGON;
  if (lstr == "ipentagon") return CQChartsPlotSymbol::Type::IPENTAGON;

  return CQChartsPlotSymbol::Type::NONE;
}

//---

void
CQChartsPlotSymbolRenderer::
drawSymbol(CQChartsPlotSymbol::Type type)
{
  CQChartsPlotSymbolMgr::drawSymbol(type, this);
}

void
CQChartsPlotSymbolRenderer::
strokeSymbol(CQChartsPlotSymbol::Type type)
{
  CQChartsPlotSymbolMgr::strokeSymbol(type, this);
}

void
CQChartsPlotSymbolRenderer::
fillSymbol(CQChartsPlotSymbol::Type type)
{
  CQChartsPlotSymbolMgr::fillSymbol(type, this);
}

//------

CQChartsSymbol2DRenderer::
CQChartsSymbol2DRenderer(QPainter *painter, const CQChartsGeom::Point &p, double s) :
 renderer_(painter), p_(p), s_(s)
{
  strokePen_.setColor (renderer_->pen().color());
  strokePen_.setWidthF(renderer_->pen().widthF());

  fillBrush_ = QBrush(renderer_->brush().color());
}

void
CQChartsSymbol2DRenderer::
moveTo(double x, double y)
{
  path_.moveTo(p_.x + x*s_, p_.y - y*s_);
}

void
CQChartsSymbol2DRenderer::
lineTo(double x, double y)
{
  path_.lineTo(p_.x + x*s_, p_.y - y*s_);
}

void
CQChartsSymbol2DRenderer::
closePath()
{
  path_.closeSubpath();
}

void
CQChartsSymbol2DRenderer::
stroke()
{
  renderer_->strokePath(path_, strokePen_);
}

void
CQChartsSymbol2DRenderer::
fill()
{
  renderer_->fillPath(path_, fillBrush_);
}

void
CQChartsSymbol2DRenderer::
strokeCircle(double x, double y, double r)
{
  QRectF rect(p_.x + (x - r)*s_, p_.y + (y - r)*s_, 2*r*s_, 2*r*s_);

  renderer_->save();

  renderer_->setBrush(Qt::NoBrush);
  renderer_->setPen  (strokePen_);

  renderer_->drawEllipse(rect);

  renderer_->restore();
}

void
CQChartsSymbol2DRenderer::
fillCircle(double x, double y, double r)
{
  QRectF rect(p_.x + (x - r)*s_, p_.y + (y - r)*s_, 2*r*s_, 2*r*s_);

  renderer_->save();

  renderer_->setBrush(fillBrush_);
  renderer_->setPen  (Qt::NoPen);

  renderer_->drawEllipse(rect);

  renderer_->restore();
}

double
CQChartsSymbol2DRenderer::
lineWidth() const
{
  return w_;
}
