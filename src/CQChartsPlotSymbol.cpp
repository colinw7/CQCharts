#include <CQChartsPlotSymbol.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsPlot.h>
#include <CQChartsDrawUtil.h>
#include <CQCharts.h>
#include <CQChartsSymbolSet.h>

#include <cmath>
#include <cassert>

class CQChartsPlotSymbolList {
 public:
  using SymbolRenderer = CQChartsPlotSymbolRenderer;
  using PlotSymbol     = CQChartsPlotSymbol;
  using Symbols        = std::vector<PlotSymbol>;

 public:
  CQChartsPlotSymbolList(std::initializer_list<PlotSymbol> symbols) :
   symbols_(symbols) {
  }

  const Symbols &symbols() const { return symbols_; }

  bool isSymbol(CQChartsSymbolType type) const {
    if (! type.isValid()) return false;

    for (const auto &s : symbols_)
      if (s.type == type.type())
        return true;

    return false;
  }

  const PlotSymbol &getSymbol(CQChartsSymbolType type) const {
    for (const auto &s : symbols_)
      if (s.type == type.type())
        return s;

    assert(false);

    return symbols_[0];
  }

  void setSymbolLines(CQChartsSymbolType type, const CQChartsPlotSymbol::Lines &lines) {
    assert(type.isValid());

    for (auto &s : symbols_) {
      if (s.type == type.type()) {
        s.lines = lines;
        break;
      }
    }
  }

  void setSymbolFillLines(CQChartsSymbolType type, const CQChartsPlotSymbol::Lines &lines) {
    assert(type.isValid());

    for (auto &s : symbols_) {
      if (s.type == type.type()) {
        s.fillLines = lines;
        break;
      }
    }
  }

  // draw unfilled symbol (just lines)
  void drawSymbol(CQChartsSymbolType type, SymbolRenderer *renderer) const {
    if (! type.isValid()) return;

    auto type1 = type.type();

    if (type1 == CQChartsSymbolType::Type::DOT)
      return renderer->drawPoint(0.0, 0.0);

    if (type1 == CQChartsSymbolType::Type::PAW) {
      renderer->drawPoint(-0.5,  0.5);
      renderer->drawPoint(-0.5, -0.5);
      renderer->drawPoint( 0.5,  0.5);
      renderer->drawPoint( 0.5, -0.5);
      return;
    }

    if (type1 == CQChartsSymbolType::Type::CIRCLE) {
      renderer->drawLine(0.0, 0.0, 0.0, 0.875);
      renderer->drawFillCircle(0.0, 0.0, 0.875, /*fill*/false);
      return;
    }

    if (type1 == CQChartsSymbolType::Type::HLINE)
      return renderer->drawLine(-0.875, 0, 0.875, 0);

    if (type1 == CQChartsSymbolType::Type::VLINE)
      return renderer->drawLine(0, -0.875, 0, 0.875);

#if 0
    if (type1 == CQChartsSymbolType::Type::HASH) {
      renderer->drawLine(-0.875,  0.5  ,  0.875,  0.5  );
      renderer->drawLine(-0.875, -0.5  ,  0.875, -0.5  );
      renderer->drawLine(-0.5  , -0.875, -0.5  ,  0.875);
      renderer->drawLine( 0.5  , -0.875,  0.5  ,  0.875);
      return;
    }
#endif

    //---

    const auto &s = getSymbol(type);

    double w = renderer->lineWidth();

    if (w <= 0.0) {
      bool connect = false;

      renderer->save();

      for (const auto &l : s.lines) {
        if (! connect)
          renderer->moveTo(l.x1, l.y1);
        else
          renderer->lineTo(l.x1, l.y1);

        // close path
        if      (l.connect == PlotSymbol::Connect::CLOSE) {
          renderer->closePath();

          renderer->stroke();

          connect = false;
        }
        // add line and stroke
        else if (l.connect == PlotSymbol::Connect::STROKE) {
          renderer->lineTo(l.x2, l.y2);

          renderer->stroke();

          connect = false;
        }
        // line to
        else if (l.connect == PlotSymbol::Connect::LINE) {
          connect = true;
        }
        else {
          assert(false);
        }
      }

      renderer->restore();
    }
    else {
      for (const auto &l : s.lines) {
        drawWideLine(renderer, l, w);
      }
    }
  }

  //---

  void strokeSymbol(CQChartsSymbolType type, SymbolRenderer *renderer) const {
    if (! type.isValid()) return;

    fillStrokeSymbol(type, renderer, false);
  }

  void fillSymbol(CQChartsSymbolType type, SymbolRenderer *renderer) const {
    if (! type.isValid()) return;

    fillStrokeSymbol(type, renderer, true);
  }

  // draw solid symbol with optional fill
  void fillStrokeSymbol(CQChartsSymbolType type, SymbolRenderer *renderer, bool fill) const {
    if (! type.isValid()) return;

    auto type1 = type.type();

    if (type1 == CQChartsSymbolType::Type::DOT)
      return renderer->drawFillPoint(0.0, 0.0, 0.1, fill);

    if (type1 == CQChartsSymbolType::Type::PAW) {
      renderer->drawFillCircle(-0.5,  0.5, 0.1, fill);
      renderer->drawFillCircle(-0.5, -0.5, 0.1, fill);
      renderer->drawFillCircle( 0.5,  0.5, 0.1, fill);
      renderer->drawFillCircle( 0.5, -0.5, 0.1, fill);
      return;
    }

    if (type1 == CQChartsSymbolType::Type::CIRCLE)
      return renderer->drawFillCircle(0, 0, 1.0, fill);

    if (type1 == CQChartsSymbolType::Type::HLINE)
      return renderer->drawFillHLine(-1.0, 1.0, 0.0, 0.2, fill);

    if (type1 == CQChartsSymbolType::Type::VLINE)
      return renderer->drawFillVLine(0.0, -1.0, 1.0, 0.2, fill);

#if 0
    if (type1 == CQChartsSymbolType::Type::HASH) {
      renderer->drawFillHLine(-1.0,  1.0,  0.5, 0.2, fill);
      renderer->drawFillHLine(-1.0,  1.0, -0.5, 0.2, fill);
      renderer->drawFillVLine(-0.5, -1.0,  1.0, 0.2, fill);
      renderer->drawFillVLine( 0.5, -1.0,  1.0, 0.2, fill);
      return;
    }
#endif

    //---

    const auto &s = getSymbol(type);

    renderer->save();

    bool connect = false;

    for (const auto &l : s.fillLines) {
      if (! connect)
        renderer->moveTo(l.x1, l.y1);
      else
        renderer->lineTo(l.x1, l.y1);

      if      (l.connect == PlotSymbol::Connect::CLOSE) {
        renderer->closePath();

        if (fill)
          renderer->fill();
        else
          renderer->stroke();

        connect = false;
      }
      else if (l.connect == PlotSymbol::Connect::FILL) {
        renderer->lineTo(l.x2, l.y2);

        if (fill)
          renderer->fill();
        else
          renderer->stroke();

        connect = false;
      }
      else if (l.connect == PlotSymbol::Connect::BREAK)
        connect = false;
      else
        connect = true;
    }

    renderer->restore();
  }

  void drawWideLine(SymbolRenderer *renderer, const PlotSymbol::Line &l, double w) const {
    auto addWidthToPoint= [&](const PlotSymbol::Point &p, double a, double w,
                              PlotSymbol::Point &p1, PlotSymbol::Point &p2) {
      double dx = w*std::sin(a)/2.0;
      double dy = w*std::cos(a)/2.0;

      p1.x = p.x + dx;
      p1.y = p.y - dy;
      p2.x = p.x - dx;
      p2.y = p.y + dy;
    };

    double x21 = l.x2 - l.x1;
    double y21 = l.y2 - l.y1;

    if (x21 == 0.0 && y21 == 0.0)
      return;

    double a = std::atan2(y21, x21);

    PlotSymbol::Point p[4];

    addWidthToPoint(PlotSymbol::Point(l.x1, l.y1), a, w, p[0], p[3]);
    addWidthToPoint(PlotSymbol::Point(l.x2, l.y2), a, w, p[1], p[2]);

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

constexpr double cw1 =  0.2828430; // 0.4/sqrt(2)
constexpr double cw2 =  0.7171570; // 1-0.4/sqrt(2)
constexpr double cw3 = -0.0828427; // line intersect for Y

using PlotSymbol = CQChartsPlotSymbol;

CQChartsPlotSymbolList symbols({
  { CQChartsSymbolType::Type::CROSS,
    {{-0.875, -0.875, 0.875,  0.875, PlotSymbol::Connect::STROKE},
     {-0.875,  0.875, 0.875, -0.875, PlotSymbol::Connect::STROKE}},
    {{-cw2 , -1.0 ,  0.0 , -cw1 , PlotSymbol::Connect::STROKE},
     { 0.0 , -cw1 ,  cw2 , -1.0 , PlotSymbol::Connect::STROKE},
     { cw2 , -1.0 ,  1.0 , -cw2 , PlotSymbol::Connect::STROKE},
     { 1.0 , -cw2 ,  cw1 ,  0.0 , PlotSymbol::Connect::STROKE},
     { cw1 ,  0.0 ,  1.0 ,  cw2 , PlotSymbol::Connect::STROKE},
     { 1.0 ,  cw2 ,  cw2 ,  1.0 , PlotSymbol::Connect::STROKE},
     { cw2 ,  1.0 ,  0.0 ,  cw1 , PlotSymbol::Connect::STROKE},
     { 0.0 ,  cw1 , -cw2 ,  1.0 , PlotSymbol::Connect::STROKE},
     {-cw2 ,  1.0 , -1.0 ,  cw2 , PlotSymbol::Connect::STROKE},
     {-1.0 ,  cw2 , -cw1 ,  0.0 , PlotSymbol::Connect::STROKE},
     {-cw1 ,  0.0 , -1.0 , -cw2 , PlotSymbol::Connect::STROKE},
     {-1.0 , -cw2 , -cw2 , -1.0 , PlotSymbol::Connect::FILL  }} },
  { CQChartsSymbolType::Type::PLUS,
    {{ 0.0  , -0.875,  0.0  ,  0.875, PlotSymbol::Connect::STROKE},
     {-0.875,  0.0  ,  0.875,  0.0  , PlotSymbol::Connect::STROKE}},
    {{-0.2  , -1.0  ,  0.2  , -1.0  , PlotSymbol::Connect::STROKE},
     { 0.2  , -1.0  ,  0.2  , -0.2  , PlotSymbol::Connect::STROKE},
     { 0.2  , -0.2  ,  1.0  , -0.2  , PlotSymbol::Connect::STROKE},
     { 1.0  , -0.2  ,  1.0  ,  0.2  , PlotSymbol::Connect::STROKE},
     { 1.0  ,  0.2  ,  0.2  ,  0.2  , PlotSymbol::Connect::STROKE},
     { 0.2  ,  0.2  ,  0.2  ,  1.0  , PlotSymbol::Connect::STROKE},
     { 0.2  ,  1.0  , -0.2  ,  1.0  , PlotSymbol::Connect::STROKE},
     {-0.2  ,  1.0  , -0.2  ,  0.2  , PlotSymbol::Connect::STROKE},
     {-0.2  ,  0.2  , -1.0  ,  0.2  , PlotSymbol::Connect::STROKE},
     {-1.0  ,  0.2  , -1.0  , -0.2  , PlotSymbol::Connect::STROKE},
     {-1.0  , -0.2  , -0.2  , -0.2  , PlotSymbol::Connect::STROKE},
     {-0.2  , -0.2  , -0.2  , -1.0  , PlotSymbol::Connect::FILL  }} },
  { CQChartsSymbolType::Type::Y,
    {{ 0.0,  0.0,  0.0  ,  0.875, PlotSymbol::Connect::STROKE},
     { 0.0,  0.0,  0.875, -0.875, PlotSymbol::Connect::STROKE},
     { 0.0,  0.0, -0.875, -0.875, PlotSymbol::Connect::STROKE}},
    {{-0.2,  1.0,  0.2  ,  1.0  , PlotSymbol::Connect::STROKE},
     { 0.2,  1.0,  0.2  , -cw3  , PlotSymbol::Connect::STROKE},
     { 0.2, -cw3,  1.0  , -cw2  , PlotSymbol::Connect::STROKE},
     { 1.0, -cw2,  cw2  , -1.0  , PlotSymbol::Connect::STROKE},
     { cw2, -1.0,  0.0  , -cw1  , PlotSymbol::Connect::STROKE},
     { 0.0, -cw1, -cw2  , -1.0  , PlotSymbol::Connect::STROKE},
     {-cw2, -1.0, -1.0  , -cw2  , PlotSymbol::Connect::STROKE},
     {-1.0, -cw2, -0.2  , -cw3  , PlotSymbol::Connect::STROKE},
     {-0.2, -cw3, -0.2  ,  1.0  , PlotSymbol::Connect::FILL  }} },
  { CQChartsSymbolType::Type::Z,
    {{-0.875, -0.875,  0.875, -0.875, PlotSymbol::Connect::STROKE},
     { 0.875, -0.875, -0.875,  0.875, PlotSymbol::Connect::STROKE},
     {-0.875,  0.875,  0.875,  0.875, PlotSymbol::Connect::STROKE}},
    {{-1.0     , -1.0     ,  1.0     , -1.0     , PlotSymbol::Connect::STROKE},
     { 1.0     , -1.0     ,  1.0     , -0.578125, PlotSymbol::Connect::STROKE},
     { 1.0     , -0.578125, -0.515625,  0.625   , PlotSymbol::Connect::STROKE},
     {-0.515625,  0.625   ,  1.0     ,  0.625   , PlotSymbol::Connect::STROKE},
     { 1.0     ,  0.625   ,  1.0     ,  1.0     , PlotSymbol::Connect::STROKE},
     { 1.0     ,  1.0     , -1.0     ,  1.0     , PlotSymbol::Connect::STROKE},
     {-1.0     ,  1.0     , -1.0     ,  0.5625  , PlotSymbol::Connect::STROKE},
     {-1.0     ,  0.5625  ,  0.5     , -0.625   , PlotSymbol::Connect::STROKE},
     { 0.5     , -0.625   , -1.0     , -0.625   , PlotSymbol::Connect::STROKE},
     {-1.0     , -0.625   , -1.0     , -1.0     , PlotSymbol::Connect::STROKE},
     {-1.0     , -1.0     , -1.0     , -1.0     , PlotSymbol::Connect::FILL  }} },

/*
    {{-0.8  , -0.9  ,  0.8  , -0.9  , PlotSymbol::Connect::STROKE},
     { 0.8  , -0.9  ,  0.8  , -0.5  , PlotSymbol::Connect::STROKE},
     { 0.8  , -0.5  , -0.4  ,  0.575, PlotSymbol::Connect::STROKE},
     {-0.4  ,  0.575,  0.8  ,  0.575, PlotSymbol::Connect::STROKE},
     { 0.8  ,  0.575,  0.8  ,  0.9  , PlotSymbol::Connect::STROKE},
     { 0.8  ,  0.9  , -0.8  ,  0.9  , PlotSymbol::Connect::STROKE},
     {-0.8  ,  0.9  , -0.8  ,  0.5  , PlotSymbol::Connect::STROKE},
     {-0.8  ,  0.5  ,  0.4  , -0.575, PlotSymbol::Connect::STROKE},
     { 0.4  , -0.575, -0.8  , -0.575, PlotSymbol::Connect::STROKE},
     {-0.8  , -0.575, -0.8  , -0.9  , PlotSymbol::Connect::FILL  }},
*/
  { CQChartsSymbolType::Type::TRIANGLE,
    {{ 0.0  , -0.875, -0.875,  0.875, PlotSymbol::Connect::LINE  },
     {-0.875,  0.875,  0.875,  0.875, PlotSymbol::Connect::LINE  },
     { 0.875,  0.875,  0.0  , -0.875, PlotSymbol::Connect::CLOSE },
     { 0.0  ,  0.0  ,  0.0  , -0.875, PlotSymbol::Connect::STROKE}},
    {{ 0.0  , -1.0  , -1.0  ,  1.0  , PlotSymbol::Connect::STROKE},
     {-1.0  ,  1.0  ,  1.0  ,  1.0  , PlotSymbol::Connect::STROKE},
     { 1.0  ,  1.0  ,  0.0  , -1.0  , PlotSymbol::Connect::FILL  }} },
  { CQChartsSymbolType::Type::ITRIANGLE,
    {{ 0.0  ,  0.875, -0.875, -0.875, PlotSymbol::Connect::LINE  },
     {-0.875, -0.875,  0.875, -0.875, PlotSymbol::Connect::LINE  },
     { 0.875, -0.875,  0.0  ,  0.875, PlotSymbol::Connect::CLOSE },
     { 0.0  ,   0.0 ,  0.0  ,  0.875, PlotSymbol::Connect::STROKE}},
    {{ 0.0  ,   1.0 , -1.0  , -1.0  , PlotSymbol::Connect::STROKE},
     {-1.0  ,  -1.0 ,  1.0  , -1.0  , PlotSymbol::Connect::STROKE},
     { 1.0  ,  -1.0 ,  0.0  ,  1.0  , PlotSymbol::Connect::FILL  }} },
  { CQChartsSymbolType::Type::BOX,
    {{-0.875,  0.875,  0.875,  0.875, PlotSymbol::Connect::LINE  },
     { 0.875,  0.875,  0.875, -0.875, PlotSymbol::Connect::LINE  },
     { 0.875, -0.875, -0.875, -0.875, PlotSymbol::Connect::LINE  },
     {-0.875, -0.875, -0.875,  0.875, PlotSymbol::Connect::CLOSE },
     { 0.0  ,  0.0  ,  0.0  ,  0.875, PlotSymbol::Connect::STROKE}},
    {{-1.0  ,  1.0  ,  1.0  ,  1.0  , PlotSymbol::Connect::STROKE},
     { 1.0  ,  1.0  ,  1.0  , -1.0  , PlotSymbol::Connect::STROKE},
     { 1.0  , -1.0  , -1.0  , -1.0  , PlotSymbol::Connect::STROKE},
     {-1.0  , -1.0  , -1.0  ,  1.0  , PlotSymbol::Connect::FILL  }} },
  { CQChartsSymbolType::Type::DIAMOND,
    {{-0.875,  0.0  ,  0.0  ,  0.875, PlotSymbol::Connect::LINE  },
     { 0.0  ,  0.875,  0.875,  0.0  , PlotSymbol::Connect::LINE  },
     { 0.875,  0.0  ,  0.0  , -0.875, PlotSymbol::Connect::LINE  },
     { 0.0  , -0.875, -0.875,  0.0  , PlotSymbol::Connect::CLOSE },
     { 0.0  ,  0.0  ,  0.0  ,  0.875, PlotSymbol::Connect::STROKE}},
    {{-1.0  ,  0.0  ,  0.0  ,  1.0  , PlotSymbol::Connect::STROKE},
     { 0.0  ,  1.0  ,  1.0  ,  0.0  , PlotSymbol::Connect::STROKE},
     { 1.0  ,  0.0  ,  0.0  , -1.0  , PlotSymbol::Connect::STROKE},
     { 0.0  , -1.0  , -1.0  ,  0.0  , PlotSymbol::Connect::FILL  }} },
  { CQChartsSymbolType::Type::STAR5,
    {{ 0.0     ,  0.0     ,  0.0     , -0.875   , PlotSymbol::Connect::STROKE},
     { 0.0     ,  0.0     , -0.828125, -0.28125 , PlotSymbol::Connect::STROKE},
     { 0.0     ,  0.0     ,  0.828125, -0.28125 , PlotSymbol::Connect::STROKE},
     { 0.0     ,  0.0     , -0.53125 ,  0.6875  , PlotSymbol::Connect::STROKE},
     { 0.0     ,  0.0     ,  0.53125 ,  0.6875  , PlotSymbol::Connect::STROKE}},
    {{ 0.0     , -1       , -0.293893, -0.404508, PlotSymbol::Connect::STROKE},
     {-0.293893, -0.404508, -0.951057, -0.309017, PlotSymbol::Connect::STROKE},
     {-0.951057, -0.309017, -0.475528,  0.154508, PlotSymbol::Connect::STROKE},
     {-0.475528,  0.154508, -0.587785,  0.809017, PlotSymbol::Connect::STROKE},
     {-0.587785,  0.809017,  0.0     ,  0.5     , PlotSymbol::Connect::STROKE},
     { 0.0     ,  0.5     ,  0.587785,  0.809017, PlotSymbol::Connect::STROKE},
     { 0.587785,  0.809017,  0.475528,  0.154508, PlotSymbol::Connect::STROKE},
     { 0.475528,  0.154508,  0.951057, -0.309017, PlotSymbol::Connect::STROKE},
     { 0.951057, -0.309017,  0.293893, -0.404508, PlotSymbol::Connect::STROKE},
     { 0.293893, -0.404508,  0.0     , -1       , PlotSymbol::Connect::FILL  }} },
  { CQChartsSymbolType::Type::STAR6,
    {{ 0.0     , -0.875   ,  0.0     ,  0.875   , PlotSymbol::Connect::STROKE},
     {-0.75    , -0.4375  ,  0.75    ,  0.453125, PlotSymbol::Connect::STROKE},
     {-0.75    ,  0.453125,  0.75    , -0.4375  , PlotSymbol::Connect::STROKE}},
    {{ 0.0     ,  1.0     , -0.25    ,  0.433013, PlotSymbol::Connect::STROKE},
     {-0.25    ,  0.433013, -0.866025,  0.5     , PlotSymbol::Connect::STROKE},
     {-0.866025,  0.5     , -0.5     ,  0.0     , PlotSymbol::Connect::STROKE},
     {-0.5     ,  0.0     , -0.866025, -0.5     , PlotSymbol::Connect::STROKE},
     {-0.866025, -0.5     , -0.25    , -0.433013, PlotSymbol::Connect::STROKE},
     {-0.25    , -0.433013,  0.0     , -1.0     , PlotSymbol::Connect::STROKE},
     { 0.0     , -1.0     ,  0.25    , -0.433013, PlotSymbol::Connect::STROKE},
     { 0.25    , -0.433013,  0.866025, -0.5     , PlotSymbol::Connect::STROKE},
     { 0.866025, -0.5     ,  0.5     ,  0.0     , PlotSymbol::Connect::STROKE},
     { 0.5     ,  0.0     ,  0.866025,  0.5     , PlotSymbol::Connect::STROKE},
     { 0.866025,  0.5     ,  0.25    ,  0.433013, PlotSymbol::Connect::STROKE},
     { 0.25    ,  0.433013,  0.0     ,  1.0     , PlotSymbol::Connect::FILL  }} },
  { CQChartsSymbolType::Type::PENTAGON,
    {{  0.00    , -0.875   ,  0.828125, -0.28125 , PlotSymbol::Connect::LINE  },
     {  0.828125, -0.28125 ,  0.50    ,  0.71875 , PlotSymbol::Connect::LINE  },
     {  0.50    ,  0.71875 , -0.50    ,  0.71875 , PlotSymbol::Connect::LINE  },
     { -0.50    ,  0.71875 , -0.828125, -0.28125 , PlotSymbol::Connect::LINE  },
     { -0.828125, -0.28125 ,  0.00    , -0.875   , PlotSymbol::Connect::CLOSE },
     {  0.00    , -0.875   ,  0.00    ,  0.00    , PlotSymbol::Connect::STROKE}},
    {{  0.000000, -1.000000,  0.951057, -0.309017, PlotSymbol::Connect::STROKE},
     {  0.951057, -0.309017,  0.587785,  0.809017, PlotSymbol::Connect::STROKE},
     {  0.587785,  0.809017, -0.587785,  0.809017, PlotSymbol::Connect::STROKE},
     { -0.587785,  0.809017, -0.951057, -0.309017, PlotSymbol::Connect::STROKE},
     { -0.951057, -0.309017,  0.000000, -1.000000, PlotSymbol::Connect::FILL  }} },
  { CQChartsSymbolType::Type::IPENTAGON,
    {{  0.00    ,  0.875   ,  0.84375 ,  0.25    , PlotSymbol::Connect::LINE  },
     {  0.84375 ,  0.25    ,  0.50    , -0.71875 , PlotSymbol::Connect::LINE  },
     {  0.50    , -0.71875 , -0.50    , -0.71875 , PlotSymbol::Connect::LINE  },
     { -0.50    , -0.71875 , -0.84375 ,  0.25    , PlotSymbol::Connect::LINE  },
     { -0.84375 ,  0.25    ,  0.00    ,  0.875   , PlotSymbol::Connect::CLOSE },
     {  0.00    ,  0.875   ,  0.00    ,  0.00    , PlotSymbol::Connect::STROKE}},
    {{  0.000000,  1.000000,  0.951057,  0.309017, PlotSymbol::Connect::STROKE},
     {  0.951057,  0.309017,  0.587785, -0.809017, PlotSymbol::Connect::STROKE},
     {  0.587785, -0.809017, -0.587785, -0.809017, PlotSymbol::Connect::STROKE},
     { -0.587785, -0.809017, -0.951057,  0.309017, PlotSymbol::Connect::STROKE},
     { -0.951057,  0.309017,  0.000000,  1.000000, PlotSymbol::Connect::FILL  }} },
  { CQChartsSymbolType::Type::HEXAGON,
    {{  0.866025,  0.5     ,  0.000000,  1.0     , PlotSymbol::Connect::LINE  },
     {  0.000000,  1.0     , -0.866025,  0.5     , PlotSymbol::Connect::LINE  },
     { -0.866025,  0.5     , -0.866025, -0.5     , PlotSymbol::Connect::LINE  },
     { -0.866025, -0.5     ,  0.00000 , -1.0     , PlotSymbol::Connect::LINE  },
     {  0.00000 , -1.0     ,  0.866025, -0.5     , PlotSymbol::Connect::LINE  },
     {  0.866025, -0.5     ,  0.866025,  0.5     , PlotSymbol::Connect::CLOSE },
     {  0.00    ,  1.000000,  0.00    ,  0.00    , PlotSymbol::Connect::STROKE}},
    {{  0.866025,  0.5     ,  0.000000,  1.0     , PlotSymbol::Connect::STROKE},
     {  0.000000,  1.0     , -0.866025,  0.5     , PlotSymbol::Connect::STROKE},
     { -0.866025,  0.5     , -0.866025, -0.5     , PlotSymbol::Connect::STROKE},
     { -0.866025, -0.5     ,  0.00000 , -1.0     , PlotSymbol::Connect::STROKE},
     {  0.00000 , -1.0     ,  0.866025, -0.5     , PlotSymbol::Connect::STROKE},
     {  0.866025, -0.5     ,  0.866025,  0.5     , PlotSymbol::Connect::FILL  }} },
  { CQChartsSymbolType::Type::OCTAGON,
    {{  0.923880,  0.382683,  0.382683,  0.923880, PlotSymbol::Connect::LINE  },
     {  0.382683,  0.923880, -0.382683,  0.92388 , PlotSymbol::Connect::LINE  },
     { -0.382683,  0.923880, -0.923880,  0.382683, PlotSymbol::Connect::LINE  },
     { -0.923880,  0.382683, -0.923880, -0.382683, PlotSymbol::Connect::LINE  },
     { -0.923880, -0.382683, -0.382683, -0.923880, PlotSymbol::Connect::LINE  },
     { -0.382683, -0.923880,  0.382683, -0.923880, PlotSymbol::Connect::LINE  },
     {  0.382683, -0.923880,  0.923880, -0.382683, PlotSymbol::Connect::LINE  },
     {  0.923880, -0.382683,  0.923880,  0.382683, PlotSymbol::Connect::CLOSE },
     {  0.00    ,  1.000000,  0.00    ,  0.00    , PlotSymbol::Connect::STROKE}},
    {{  0.923880,  0.382683,  0.382683,  0.923880, PlotSymbol::Connect::STROKE},
     {  0.382683,  0.923880, -0.382683,  0.92388 , PlotSymbol::Connect::STROKE},
     { -0.382683,  0.923880, -0.923880,  0.382683, PlotSymbol::Connect::STROKE},
     { -0.923880,  0.382683, -0.923880, -0.382683, PlotSymbol::Connect::STROKE},
     { -0.923880, -0.382683, -0.382683, -0.923880, PlotSymbol::Connect::STROKE},
     { -0.382683, -0.923880,  0.382683, -0.923880, PlotSymbol::Connect::STROKE},
     {  0.382683, -0.923880,  0.923880, -0.382683, PlotSymbol::Connect::STROKE},
     {  0.923880, -0.382683,  0.923880,  0.382683, PlotSymbol::Connect::FILL  }} },
  { CQChartsSymbolType::Type::HASH,
    {{ -0.875, -0.5  ,  0.875, -0.5  , PlotSymbol::Connect::STROKE},
     { -0.875,  0.5  ,  0.875,  0.5  , PlotSymbol::Connect::STROKE},
     { -0.5  , -0.875, -0.5  ,  0.875, PlotSymbol::Connect::STROKE},
     {  0.5  , -0.875,  0.5  ,  0.875, PlotSymbol::Connect::STROKE}},
    {{-0.625, -1.0  , -0.375, -1.0  , PlotSymbol::Connect::STROKE},
     {-0.375, -1.0  , -0.375, -0.625, PlotSymbol::Connect::STROKE},
     {-0.375, -0.625,  0.375, -0.625, PlotSymbol::Connect::STROKE},
     { 0.375, -0.625,  0.375, -1.0  , PlotSymbol::Connect::STROKE},
     { 0.375, -1.0  ,  0.625, -1.0  , PlotSymbol::Connect::STROKE},
     { 0.625, -1.0  ,  0.625, -0.625, PlotSymbol::Connect::STROKE},
     { 0.625, -0.625,  1.0  , -0.625, PlotSymbol::Connect::STROKE},
     { 1.0  , -0.625,  1.0  , -0.375, PlotSymbol::Connect::STROKE},
     { 1.0  , -0.375,  0.625, -0.375, PlotSymbol::Connect::STROKE},
     { 0.625, -0.375,  0.625,  0.375, PlotSymbol::Connect::STROKE},
     { 0.625,  0.375,  1.0  ,  0.375, PlotSymbol::Connect::STROKE},
     { 1.0  ,  0.375,  1.0  ,  0.625, PlotSymbol::Connect::STROKE},
     { 1.0  ,  0.625,  0.625,  0.625, PlotSymbol::Connect::STROKE},
     { 0.625,  0.625,  0.625,  1.0  , PlotSymbol::Connect::STROKE},
     { 0.625,  1.0  ,  0.375,  1.0  , PlotSymbol::Connect::STROKE},
     { 0.375,  1.0  ,  0.375,  0.625, PlotSymbol::Connect::STROKE},
     { 0.375,  0.625, -0.375,  0.625, PlotSymbol::Connect::STROKE},
     {-0.375,  0.625, -0.375,  1.0  , PlotSymbol::Connect::STROKE},
     {-0.375,  1.0  , -0.625,  1.0  , PlotSymbol::Connect::STROKE},
     {-0.625,  1.0  , -0.625,  0.625, PlotSymbol::Connect::STROKE},
     {-0.625,  0.625, -1.0  ,  0.625, PlotSymbol::Connect::STROKE},
     {-1.0  ,  0.625, -1.0  ,  0.375, PlotSymbol::Connect::STROKE},
     {-1.0  ,  0.375, -0.625,  0.375, PlotSymbol::Connect::STROKE},
     {-0.625,  0.375, -0.625, -0.375, PlotSymbol::Connect::STROKE},
     {-0.625, -0.375, -1.0  , -0.375, PlotSymbol::Connect::STROKE},
     {-1.0  , -0.375, -1.0  , -0.625, PlotSymbol::Connect::STROKE},
     {-1.0  , -0.625, -0.625, -0.625, PlotSymbol::Connect::STROKE},
     {-0.625, -0.625, -0.625, -1.0  , PlotSymbol::Connect::BREAK },
     {-0.375, -0.375,  0.375, -0.375, PlotSymbol::Connect::STROKE},
     { 0.375, -0.375,  0.375,  0.375, PlotSymbol::Connect::STROKE},
     { 0.375,  0.375, -0.375,  0.375, PlotSymbol::Connect::STROKE},
     {-0.375,  0.375, -0.375, -0.375, PlotSymbol::Connect::FILL  }} }
});

//---

bool
CQChartsPlotSymbolMgr::
isSymbol(const CQChartsSymbol &symbol)
{
  if (symbol.type() == CQChartsSymbol::Type::SYMBOL)
    return symbols.isSymbol(symbol.symbolType());
  else
    return false;
}

const CQChartsPlotSymbol &
CQChartsPlotSymbolMgr::
getSymbol(const CQChartsSymbol &symbol)
{
  assert(symbol.type() == CQChartsSymbol::Type::SYMBOL);

  return symbols.getSymbol(symbol.symbolType());
}

void
CQChartsPlotSymbolMgr::
setSymbolLines(const CQChartsSymbol &symbol, const CQChartsPlotSymbol::Lines &lines)
{
  assert(symbol.type() == CQChartsSymbol::Type::SYMBOL);

  symbols.setSymbolLines(symbol.symbolType(), lines);
}

void
CQChartsPlotSymbolMgr::
setSymbolFillLines(const CQChartsSymbol &symbol, const CQChartsPlotSymbol::Lines &lines)
{
  assert(symbol.type() == CQChartsSymbol::Type::SYMBOL);

  symbols.setSymbolFillLines(symbol.symbolType(), lines);
}

void
CQChartsPlotSymbolMgr::
drawSymbol(const CQChartsSymbol &symbol, SymbolRenderer *renderer)
{
  assert(symbol.isValid());

  if      (symbol.type() == CQChartsSymbol::Type::CHAR)
    renderer->drawChar(symbol.charStr());
  else if (symbol.type() == CQChartsSymbol::Type::PATH ||
           symbol.type() == CQChartsSymbol::Type::SVG)
    renderer->drawPaths(symbol.paths(), symbol.styles());
  else if (symbol.type() == CQChartsSymbol::Type::SYMBOL)
    symbols.drawSymbol(symbol.symbolType(), renderer);
  else if (symbol.type() == CQChartsSymbol::Type::SYMBOL_SET)
    drawSymbol(getSymbolSetSymbol(symbol, renderer), renderer);
}

void
CQChartsPlotSymbolMgr::
strokeSymbol(const CQChartsSymbol &symbol, SymbolRenderer *renderer)
{
  assert(symbol.isValid());

  if      (symbol.type() == CQChartsSymbol::Type::CHAR)
    renderer->drawChar(symbol.charStr());
  else if (symbol.type() == CQChartsSymbol::Type::PATH ||
           symbol.type() == CQChartsSymbol::Type::SVG)
    renderer->drawPaths(symbol.paths(), symbol.styles());
  else if (symbol.type() == CQChartsSymbol::Type::SYMBOL)
    symbols.strokeSymbol(symbol.symbolType(), renderer);
  else if (symbol.type() == CQChartsSymbol::Type::SYMBOL_SET)
    strokeSymbol(getSymbolSetSymbol(symbol, renderer), renderer);
}

void
CQChartsPlotSymbolMgr::
fillSymbol(const CQChartsSymbol &symbol, SymbolRenderer *renderer)
{
  assert(symbol.isValid());

  if      (symbol.type() == CQChartsSymbol::Type::CHAR)
    renderer->drawChar(symbol.charStr());
  else if (symbol.type() == CQChartsSymbol::Type::PATH ||
           symbol.type() == CQChartsSymbol::Type::SVG)
    renderer->drawPaths(symbol.paths(), symbol.styles());
  else if (symbol.type() == CQChartsSymbol::Type::SYMBOL)
    symbols.fillSymbol(symbol.symbolType(), renderer);
  else if (symbol.type() == CQChartsSymbol::Type::SYMBOL_SET)
    fillSymbol(getSymbolSetSymbol(symbol, renderer), renderer);
}

CQChartsSymbol
CQChartsPlotSymbolMgr::
getSymbolSetSymbol(const CQChartsSymbol &symbol, SymbolRenderer *renderer)
{
  auto *charts = renderer->charts();
  assert(charts);

  auto *symbolSetMgr = charts->symbolSetMgr();

  auto *symbolSet = symbolSetMgr->symbolSet(symbol.setName());
  if (! symbolSet) CQChartsSymbol();

  return symbolSet->symbol(symbol.setInd());
}

//------

CQChartsPlotSymbolRenderer::
CQChartsPlotSymbolRenderer(PaintDevice *device, const Point &p, const Length &size) :
 device_(device), p_(p), xsize_(size), ysize_(size)
{
  strokePen_ = device_->pen  ();
  fillBrush_ = device_->brush();
}

CQChartsPlotSymbolRenderer::
CQChartsPlotSymbolRenderer(PaintDevice *device, const Point &p,
                           const Length &xsize, const Length &ysize) :
 device_(device), p_(p), xsize_(xsize), ysize_(ysize)
{
  strokePen_ = device_->pen  ();
  fillBrush_ = device_->brush();
}

//---

CQCharts *
CQChartsPlotSymbolRenderer::
charts() const
{
  return device_->calcCharts();
}

//---

void
CQChartsPlotSymbolRenderer::
drawSymbol(CQChartsSymbol symbol)
{
  CQChartsPlotSymbolMgr::drawSymbol(symbol, this);
}

void
CQChartsPlotSymbolRenderer::
strokeSymbol(CQChartsSymbol symbol)
{
  CQChartsPlotSymbolMgr::strokeSymbol(symbol, this);
}

void
CQChartsPlotSymbolRenderer::
fillSymbol(CQChartsSymbol symbol)
{
  CQChartsPlotSymbolMgr::fillSymbol(symbol, this);
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

  Point p(p_.x + sx, p_.y + sy);

  save();

  device_->setPen(strokePen_);

  device_->drawPoint(p);

  restore();
}

void
CQChartsPlotSymbolRenderer::
drawFillHLine(double x1, double x2, double y, double w, bool fill)
{
  if (fill)
    fillRect(x1, y - w/2.0, x2, y + w/2.0);
  else
    drawRect(x1, y - w/2.0, x2, y + w/2.0);
}

void
CQChartsPlotSymbolRenderer::
drawFillVLine(double x, double y1, double y2, double w, bool fill)
{
  if (fill)
    fillRect(x - w/2.0, y1, x + w/2.0, y2);
  else
    drawRect(x - w/2.0, y1, x + w/2.0, y2);
}

void
CQChartsPlotSymbolRenderer::
drawLine(double x1, double y1, double x2, double y2) const
{
  double sx1, sy1, sx2, sy2;

  mapXY(x1, y1, sx1, sy1);
  mapXY(x2, y2, sx2, sy2);

  Point p1(p_.x + sx1, p_.y + sy1);
  Point p2(p_.x + sx2, p_.y + sy2);

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

  BBox bbox(p_.x + sx1, p_.y + sy1, p_.x + sx2, p_.y + sy2);
  if (! bbox.isValid()) return;

  save();

  device_->setBrush(fillBrush_);
  device_->setPen  (Qt::NoPen);

  device_->drawRect(bbox);

  restore();
}

void
CQChartsPlotSymbolRenderer::
drawRect(double x1, double y1, double x2, double y2) const
{
  double sx1, sy1, sx2, sy2;

  mapXY(x1, y1, sx1, sy1);
  mapXY(x2, y2, sx2, sy2);

  BBox bbox(p_.x + sx1, p_.y + sy1, p_.x + sx2, p_.y + sy2);
  if (! bbox.isValid()) return;

  save();

  device_->setBrush(Qt::NoBrush);
  device_->setPen  (strokePen_);

  device_->drawRect(bbox);

  restore();
}

void
CQChartsPlotSymbolRenderer::
drawFillPoint(double x, double y, double r, bool fill) const
{
  if (fill)
    fillCircle(x, y, r);
  else
    drawPoint(x, y);
}

void
CQChartsPlotSymbolRenderer::
drawFillCircle(double x, double y, double r, bool fill) const
{
  if (fill)
    fillCircle(x, y, r);
  else
    strokeCircle(x, y, r);
}

void
CQChartsPlotSymbolRenderer::
strokeCircle(double x, double y, double r) const
{
  double sx1, sy1, sx2, sy2;

  mapXY(x - r, y - r, sx1, sy1);
  mapXY(x + r, y + r, sx2, sy2);

  BBox bbox(p_.x + sx1, p_.y + sy1, p_.x + sx2, p_.y + sy2);
  if (! bbox.isValid()) return;

  save();

  device_->setBrush(Qt::NoBrush);
  device_->setPen  (strokePen_);

  device_->drawEllipse(bbox);

  restore();
}

void
CQChartsPlotSymbolRenderer::
fillCircle(double x, double y, double r) const
{
  double sx1, sy1, sx2, sy2;

  mapXY(x - r, y - r, sx1, sy1);
  mapXY(x + r, y + r, sx2, sy2);

  BBox bbox(p_.x + sx1, p_.y + sy1, p_.x + sx2, p_.y + sy2);
  if (! bbox.isValid()) return;

  save();

  device_->setBrush(fillBrush_);
  device_->setPen  (Qt::NoPen);

  device_->drawEllipse(bbox);

  restore();
}

void
CQChartsPlotSymbolRenderer::
drawChar(const QString &str) const
{
  double sx1, sy1, sx2, sy2;

  mapXY(-1, -1, sx1, sy1);
  mapXY( 1,  1, sx2, sy2);

  BBox bbox(p_.x + sx1, p_.y + sy1, p_.x + sx2, p_.y + sy2);
  if (! bbox.isValid()) return;

  CQChartsTextOptions options;

  options.scaled  = true;
  options.clipped = false;

  CQChartsDrawUtil::drawTextInBox(device_, bbox, str, options);
}

void
CQChartsPlotSymbolRenderer::
drawPaths(const std::vector<CQChartsPath> &paths, const std::vector<CQChartsStyle> &styles) const
{
  auto np = paths.size();

  assert(np == styles.size());

  for (size_t i = 0; i < np; ++i)
    drawPath(paths[i], styles[i]);
}

void
CQChartsPlotSymbolRenderer::
drawPath(const CQChartsPath &path, const CQChartsStyle &style) const
{
  const auto &ppath = path.path();

  class PathVisitor : public CQChartsDrawUtil::PathVisitor {
   public:
    PathVisitor(const CQChartsPlotSymbolRenderer *renderer, const Point &p) :
     renderer_(renderer), p_(p) {
    };

    void moveTo(const Point &p) override {
      auto mp = renderer_->mapXY(p) + p_;

      path_.moveTo(mp.qpoint());
    }

    void lineTo(const Point &p) override {
      auto mp = renderer_->mapXY(p) + p_;

      path_.lineTo(mp.qpoint());
    }

    void quadTo(const Point &p1, const Point &p2) override {
      auto mp1 = renderer_->mapXY(p1) + p_;
      auto mp2 = renderer_->mapXY(p2) + p_;

      path_.quadTo(mp1.qpoint(), mp2.qpoint());
    }

    void curveTo(const Point &p1, const Point &p2, const Point &p3) override {
      auto mp1 = renderer_->mapXY(p1) + p_;
      auto mp2 = renderer_->mapXY(p2) + p_;
      auto mp3 = renderer_->mapXY(p3) + p_;

      path_.cubicTo(mp1.qpoint(), mp2.qpoint(), mp3.qpoint());
    }

    const QPainterPath &path() const { return path_; }

   private:
    const CQChartsPlotSymbolRenderer *renderer_ { nullptr };
    Point                             p_;
    QPainterPath                      path_;
  };

  PathVisitor visitor(this, p_);

  CQChartsDrawUtil::visitPath(ppath, visitor);

  save();

  if (style.isValid()) {
    device_->setBrush(style.brush());
    device_->setPen  (style.pen());
  }
  else {
    device_->setBrush(fillBrush_);
    device_->setPen  (strokePen_);
  }

  device_->drawPath(visitor.path());

  restore();
}

CQChartsGeom::Point
CQChartsPlotSymbolRenderer::
mapXY(const Point &p) const
{
  double x1, y1;

  mapXY(p.x, p.y, x1, y1);

  return Point(x1, y1);
}

void
CQChartsPlotSymbolRenderer::
mapXY(double x, double y, double &x1, double &y1) const
{
  auto *plot = device_->plot();

  double sx, sy;

  if (plot && isScale()) {
    plot->plotSymbolSize(xsize_, ysize_, sx, sy);
  }
  else {
    sx = device_->lengthWindowWidth (xsize_);
    sy = device_->lengthWindowHeight(ysize_);
  }

  x1 = x*sx;
  y1 = y*sy;

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
