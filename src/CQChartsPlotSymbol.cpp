#include <CQChartsPlotSymbol.h>
#include <CQChartsPaintDevice.h>

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

  bool isSymbol(CQChartsSymbol type) const {
    for (const auto &s : symbols_)
      if (s.type == type)
        return true;

    return false;
  }

  const CQChartsPlotSymbol &getSymbol(CQChartsSymbol type) const {
    for (const auto &s : symbols_)
      if (s.type == type.type())
        return s;

    assert(false);

    return symbols_[0];
  }

  void drawSymbol(CQChartsSymbol type, CQChartsPlotSymbolRenderer *renderer) const {
    if (type == CQChartsSymbol::Type::DOT) {
      renderer->drawPoint(0, 0);
      return;
    }

    if (type == CQChartsSymbol::Type::CIRCLE) {
      renderer->strokeCircle(0, 0, 1);
      return;
    }

    if (type == CQChartsSymbol::Type::HLINE) {
      renderer->drawLine(-1, 0, 1, 0);
      return;
    }

    if (type == CQChartsSymbol::Type::VLINE) {
      renderer->drawLine(0, -1, 0, 1);
      return;
    }

    //---

    const CQChartsPlotSymbol &s = getSymbol(type);

    double w = renderer->lineWidth();

    if (w <= 0.0) {
      bool connect = false;

      renderer->save();

      for (const auto &l : s.lines) {
        if (! connect)
          renderer->moveTo(l.x1, l.y1);
        else
          renderer->lineTo(l.x1, l.y1);

        if      (l.connect == CQChartsPlotSymbol::Connect::CLOSE) {
          renderer->closePath();

          renderer->stroke();

          connect = false;
        }
        else if (l.connect == CQChartsPlotSymbol::Connect::STROKE) {
          renderer->lineTo(l.x2, l.y2);

          renderer->stroke();

          connect = false;
        }
        else
          connect = true;
      }

      renderer->restore();
    }
    else {
      for (const auto &l : s.lines) {
        drawWideLine(renderer, l, w);
      }
    }
  }

  void strokeSymbol(CQChartsSymbol type, CQChartsPlotSymbolRenderer *renderer) const {
    fillStrokeSymbol(type, renderer, false);
  }

  void fillSymbol(CQChartsSymbol type, CQChartsPlotSymbolRenderer *renderer) const {
    fillStrokeSymbol(type, renderer, true);
  }

  void fillStrokeSymbol(CQChartsSymbol type, CQChartsPlotSymbolRenderer *renderer,
                        bool fill) const {
    if (type == CQChartsSymbol::Type::DOT) {
      if (fill)
        renderer->fillCircle(0, 0, 0.1);
      else
        renderer->drawPoint(0, 0);

      return;
    }

    if (type == CQChartsSymbol::Type::CIRCLE) {
      if (fill)
        renderer->fillCircle(0, 0, 1);
      else
        renderer->strokeCircle(0, 0, 1);

      return;
    }

    if (type == CQChartsSymbol::Type::HLINE) {
      if (fill)
        renderer->fillRect(-1, -0.1, 1, 0.1);
      else
        renderer->drawLine(-1, 0, 1, 0);

      return;
    }

    if (type == CQChartsSymbol::Type::VLINE) {
      if (fill)
        renderer->fillRect(-0.1, -1, 0.1, 1);
      else
        renderer->drawLine(0, -1, 0, 1);

      return;
    }

    //---

    const CQChartsPlotSymbol &s = getSymbol(type);

    renderer->save();

    bool connect = false;

    for (const auto &l : s.fillLines) {
      if (! connect)
        renderer->moveTo(l.x1, l.y1);
      else
        renderer->lineTo(l.x1, l.y1);

      if      (l.connect == CQChartsPlotSymbol::Connect::CLOSE) {
        renderer->closePath();

        if (fill)
          renderer->fill();
        else
          renderer->stroke();

        connect = false;
      }
      else if (l.connect == CQChartsPlotSymbol::Connect::FILL) {
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

    renderer->restore();
  }

  void drawWideLine(CQChartsPlotSymbolRenderer *renderer, const CQChartsPlotSymbol::Line &l,
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

CQChartsPlotSymbolList symbols({
  { CQChartsSymbol::Type::CROSS    ,
    {{-cw2, -cw2,  cw2,  cw2, CQChartsPlotSymbol::Connect::STROKE},
     {-cw2,  cw2,  cw2, -cw2, CQChartsPlotSymbol::Connect::STROKE}},
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
  { CQChartsSymbol::Type::PLUS     ,
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
  { CQChartsSymbol::Type::Y        ,
    {{ 0.0,  0.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::STROKE},
     { 0.0,  0.0,  cw2, -cw2, CQChartsPlotSymbol::Connect::STROKE},
     { 0.0,  0.0, -cw2, -cw2, CQChartsPlotSymbol::Connect::STROKE}},
    {{-0.2,  1.0,  0.2,  1.0, CQChartsPlotSymbol::Connect::STROKE},
     { 0.2,  1.0,  0.2, -cw3, CQChartsPlotSymbol::Connect::STROKE},
     { 0.2, -cw3,  1.0, -cw2, CQChartsPlotSymbol::Connect::STROKE},
     { 1.0, -cw2,  cw2, -1.0, CQChartsPlotSymbol::Connect::STROKE},
     { cw2, -1.0,  0.0, -cw1, CQChartsPlotSymbol::Connect::STROKE},
     { 0.0, -cw1, -cw2, -1.0, CQChartsPlotSymbol::Connect::STROKE},
     {-cw2, -1.0, -1.0, -cw2, CQChartsPlotSymbol::Connect::STROKE},
     {-1.0, -cw2, -0.2, -cw3, CQChartsPlotSymbol::Connect::STROKE},
     {-0.2, -cw3, -0.2,  1.0, CQChartsPlotSymbol::Connect::FILL  }} },
  { CQChartsSymbol::Type::TRIANGLE ,
    {{ 0.0, -1.0, -1.0,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     {-1.0,  1.0,  1.0,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0,  1.0,  0.0, -1.0, CQChartsPlotSymbol::Connect::CLOSE },
     { 0.0,  0.0,  0.0, -1.0, CQChartsPlotSymbol::Connect::STROKE}},
    {{ 0.0, -1.0, -1.0,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     {-1.0,  1.0,  1.0,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0,  1.0,  0.0, -1.0, CQChartsPlotSymbol::Connect::FILL  }} },
  { CQChartsSymbol::Type::ITRIANGLE,
    {{ 0.0,  1.0, -1.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     {-1.0, -1.0,  1.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0, -1.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::CLOSE },
     { 0.0,  0.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::STROKE}},
    {{ 0.0,  1.0, -1.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     {-1.0, -1.0,  1.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0, -1.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::FILL  }} },
  { CQChartsSymbol::Type::BOX      ,
    {{-1.0,  1.0,  1.0,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0,  1.0,  1.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0, -1.0, -1.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     {-1.0, -1.0, -1.0,  1.0, CQChartsPlotSymbol::Connect::CLOSE },
     { 0.0,  0.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::STROKE}},
    {{-1.0,  1.0,  1.0,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0,  1.0,  1.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0, -1.0, -1.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     {-1.0, -1.0, -1.0,  1.0, CQChartsPlotSymbol::Connect::FILL  }} },
  { CQChartsSymbol::Type::DIAMOND  ,
    {{-1.0,  0.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 0.0,  1.0,  1.0,  0.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0,  0.0,  0.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 0.0, -1.0, -1.0,  0.0, CQChartsPlotSymbol::Connect::CLOSE },
     { 0.0,  0.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::STROKE}},
    {{-1.0,  0.0,  0.0,  1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 0.0,  1.0,  1.0,  0.0, CQChartsPlotSymbol::Connect::LINE  },
     { 1.0,  0.0,  0.0, -1.0, CQChartsPlotSymbol::Connect::LINE  },
     { 0.0, -1.0, -1.0,  0.0, CQChartsPlotSymbol::Connect::FILL  }} },
  { CQChartsSymbol::Type::STAR5    ,
    {{ 0.0,  0.0,  0.0     , -1.0     , CQChartsPlotSymbol::Connect::STROKE},
     { 0.0,  0.0, -0.951057, -0.309017, CQChartsPlotSymbol::Connect::STROKE},
     { 0.0,  0.0,  0.951057, -0.309017, CQChartsPlotSymbol::Connect::STROKE},
     { 0.0,  0.0, -0.587785,  0.809017, CQChartsPlotSymbol::Connect::STROKE},
     { 0.0,  0.0,  0.587785,  0.809017, CQChartsPlotSymbol::Connect::STROKE}},
    {{ 0.0     , -1       , -0.293893, -0.404508, CQChartsPlotSymbol::Connect::LINE},
     {-0.293893, -0.404508, -0.951057, -0.309017, CQChartsPlotSymbol::Connect::LINE},
     {-0.951057, -0.309017, -0.475528,  0.154508, CQChartsPlotSymbol::Connect::LINE},
     {-0.475528,  0.154508, -0.587785,  0.809017, CQChartsPlotSymbol::Connect::LINE},
     {-0.587785,  0.809017,  0.0     ,  0.5     , CQChartsPlotSymbol::Connect::LINE},
     { 0.0     ,  0.5     ,  0.587785,  0.809017, CQChartsPlotSymbol::Connect::LINE},
     { 0.587785,  0.809017,  0.475528,  0.154508, CQChartsPlotSymbol::Connect::LINE},
     { 0.475528,  0.154508,  0.951057, -0.309017, CQChartsPlotSymbol::Connect::LINE},
     { 0.951057, -0.309017,  0.293893, -0.404508, CQChartsPlotSymbol::Connect::LINE},
     { 0.293893, -0.404508,  0.0     , -1       , CQChartsPlotSymbol::Connect::FILL}} },
  { CQChartsSymbol::Type::STAR6    ,
    {{ 0.0     , -1.0     ,  0.0     ,  1.0     , CQChartsPlotSymbol::Connect::STROKE},
     {-0.866025, -0.5     ,  0.866025,  0.5     , CQChartsPlotSymbol::Connect::STROKE},
     {-0.866025,  0.5     ,  0.866025, -0.5     , CQChartsPlotSymbol::Connect::STROKE}},
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
   { CQChartsSymbol::Type::PENTAGON ,
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
  { CQChartsSymbol::Type::IPENTAGON ,
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
isSymbol(CQChartsSymbol type)
{
  return symbols.isSymbol(type);
}

const CQChartsPlotSymbol &
CQChartsPlotSymbolMgr::
getSymbol(CQChartsSymbol type)
{
  return symbols.getSymbol(type);
}

void
CQChartsPlotSymbolMgr::
drawSymbol(CQChartsSymbol type, CQChartsPlotSymbolRenderer *renderer)
{
  return symbols.drawSymbol(type, renderer);
}

void
CQChartsPlotSymbolMgr::
strokeSymbol(CQChartsSymbol type, CQChartsPlotSymbolRenderer *renderer)
{
  return symbols.strokeSymbol(type, renderer);
}

void
CQChartsPlotSymbolMgr::
fillSymbol(CQChartsSymbol type, CQChartsPlotSymbolRenderer *renderer)
{
  return symbols.fillSymbol(type, renderer);
}

//------

CQChartsPlotSymbolRenderer::
CQChartsPlotSymbolRenderer(CQChartsPaintDevice *device, const CQChartsGeom::Point &p,
                           const CQChartsLength &size) :
 device_(device), p_(p), size_(size)
{
  strokePen_ = device_->pen  ();
  fillBrush_ = device_->brush();
}

//---

void
CQChartsPlotSymbolRenderer::
drawSymbol(CQChartsSymbol type)
{
  CQChartsPlotSymbolMgr::drawSymbol(type, this);
}

void
CQChartsPlotSymbolRenderer::
strokeSymbol(CQChartsSymbol type)
{
  CQChartsPlotSymbolMgr::strokeSymbol(type, this);
}

void
CQChartsPlotSymbolRenderer::
fillSymbol(CQChartsSymbol type)
{
  CQChartsPlotSymbolMgr::fillSymbol(type, this);
}

//---

void
CQChartsPlotSymbolRenderer::
moveTo(double x, double y)
{
  double sx, sy;

  mapXY(x, y, sx, sy);

  path_.moveTo(p_.x + sx, p_.y - sy);
}

void
CQChartsPlotSymbolRenderer::
lineTo(double x, double y)
{
  double sx, sy;

  mapXY(x, y, sx, sy);

  path_.lineTo(p_.x + sx, p_.y - sy);
}

void
CQChartsPlotSymbolRenderer::
closePath()
{
  path_.closeSubpath();
}

void
CQChartsPlotSymbolRenderer::
stroke()
{
  assert(saved_);

  device_->strokePath(path_, strokePen_);
}

void
CQChartsPlotSymbolRenderer::
fill()
{
  assert(saved_);

  device_->fillPath(path_, fillBrush_);
}

void
CQChartsPlotSymbolRenderer::
drawPoint(double x, double y) const
{
  double sx, sy;

  mapXY(x, y, sx, sy);

  QPointF p(p_.x + sx, p_.y + sy);

  save();

  device_->setPen(strokePen_);

  device_->drawPoint(p);

  restore();
}

void
CQChartsPlotSymbolRenderer::
drawLine(double x1, double y1, double x2, double y2) const
{
  double sx1, sy1, sx2, sy2;

  mapXY(x1, y1, sx1, sy1);
  mapXY(x2, y2, sx2, sy2);

  QPointF p1(p_.x + sx1, p_.y + sy1);
  QPointF p2(p_.x + sx2, p_.y + sy2);

  save();

  device_->setPen(strokePen_);

  device_->drawLine(p1, p2);

  restore();
}

void
CQChartsPlotSymbolRenderer::
fillRect(double x1, double y1, double x2, double y2) const
{
  double sx1, sy1, sx2, sy2;

  mapXY(x1, y1, sx1, sy1);
  mapXY(x2, y2, sx2, sy2);

  QRectF rect(p_.x + sx1, p_.y + sy1, sx2 - sx1, sy2 - sy1);

  save();

  device_->setBrush(fillBrush_);
  device_->setPen  (Qt::NoPen);

  device_->drawRect(rect);

  restore();
}

void
CQChartsPlotSymbolRenderer::
strokeCircle(double x, double y, double r) const
{
  double sx1, sy1, sx2, sy2;

  mapXY(x - r, y - r, sx1, sy1);
  mapXY(x + r, y + r, sx2, sy2);

  QRectF rect(p_.x + sx1, p_.y + sy1, sx2 - sx1, sy2 - sy1);

  save();

  device_->setBrush(Qt::NoBrush);
  device_->setPen  (strokePen_);

  device_->drawEllipse(rect);

  restore();
}

void
CQChartsPlotSymbolRenderer::
fillCircle(double x, double y, double r) const
{
  double sx1, sy1, sx2, sy2;

  mapXY(x - r, y - r, sx1, sy1);
  mapXY(x + r, y + r, sx2, sy2);

  QRectF rect(p_.x + sx1, p_.y + sy1, sx2 - sx1, sy2 - sy1);

  save();

  device_->setBrush(fillBrush_);
  device_->setPen  (Qt::NoPen);

  device_->drawEllipse(rect);

  restore();
}

void
CQChartsPlotSymbolRenderer::
mapXY(double x, double y, double &x1, double &y1) const
{
  x1 = x*device_->lengthWindowWidth (size_);
  y1 = y*device_->lengthWindowHeight(size_);

  if (device_->invertY())
    y1 = -y1;
}

double
CQChartsPlotSymbolRenderer::
lineWidth() const
{
  return w_;
}

void
CQChartsPlotSymbolRenderer::
save() const
{
  assert(! saved_);

  device_->save();

  saved_ = true;
}

void
CQChartsPlotSymbolRenderer::
restore() const
{
  assert(saved_);

  device_->restore();

  saved_ = false;
}
