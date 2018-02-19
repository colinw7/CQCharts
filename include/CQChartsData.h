#ifndef CQChartsData_H
#define CQChartsData_H

#include <CQChartsColor.h>
#include <CQChartsLength.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsLineDash.h>
#include <CQChartsFillPattern.h>
#include <QFont>

// Text Properties
//   font, color, alpha, angle, contrast (shadow)
//
// TODO shadow properties
struct CQChartsTextData {
  bool          visible  { true };
  QFont         font;
  CQChartsColor color    { CQChartsColor::Type::THEME_VALUE, 1.0 };
  double        alpha    { 1.0 };
  double        angle    { 0.0 };
  bool          contrast { false };
  Qt::Alignment align    { Qt::AlignLeft | Qt::AlignVCenter };
};

// Line Properties
struct CQChartsLineData {
  bool             visible { true };
  CQChartsColor    color   { CQChartsColor::Type::THEME_VALUE, 1.0 };
  double           alpha   { 1.0 };
  CQChartsLength   width   { "0px" };
  CQChartsLineDash dash    { };
};

// Fill Properties
struct CQChartsFillData {
  using Pattern = CQChartsFillPattern::Type;

  bool          visible { true };                                  // draw bg
  CQChartsColor color   { CQChartsColor::Type::THEME_VALUE, 0.0 }; // bg fill color
  double        alpha   { 1.0 };                                   // bg fill color alpha
  Pattern       pattern { Pattern::SOLID };                        // bg fill pattern
};

// Stroke Properties
struct CQChartsStrokeData {
  bool             visible { true };                                  // draw border
  CQChartsColor    color   { CQChartsColor::Type::THEME_VALUE, 1.0 }; // border color
  double           alpha   { 1.0 };                                   // border color alpha
  CQChartsLength   width   { "0px" };                                 // stroke width
  CQChartsLineDash dash    { };                                       // stroke dash
};

// Shape Properties
struct CQChartsShapeData {
  CQChartsFillData   background;
  CQChartsStrokeData border;
};

// Box Properties
//  TODO: border corners
struct CQChartsBoxData {
  double             margin      { 4 };      // inside margin (pixels)
  double             padding     { 0 };      // outside margin (pixels)
  CQChartsFillData   background;             // background data
  CQChartsStrokeData border;                 // border data
  double             cornerSize  { 0.0 };    // border rounded corner size
  QString            borderSides { "tlbr" }; // border sides to draw
};

struct CQChartsSymbolData {
  bool                     visible { true };
  CQChartsPlotSymbol::Type type    { CQChartsPlotSymbol::Type::CROSS };
  double                   size    { 5.0 };
  CQChartsStrokeData       stroke;
  CQChartsFillData         fill;

  CQChartsSymbolData() {
    fill.visible = false;
  }
};

struct CQChartsArrowData {
  bool               relative    { false };     // to point relative to from
  CQChartsLength     length      { "1V" };      // length
  double             angle       { -1 };        // arrow angle
  double             backAngle   { -1 };        // arrow back angle
  bool               fhead       { false };     // draw arrow head at front
  bool               thead       { true };      // draw arrow head at tail
  bool               empty       { false };     // draw empty head (unfilled stroked)
  CQChartsStrokeData stroke;                    // line stroke
  CQChartsFillData   fill;                      // head fill
  bool               labels      { false };     // show debug labels
  QColor             labelColor  { 255, 0, 0 }; // debug label color
};

#endif
