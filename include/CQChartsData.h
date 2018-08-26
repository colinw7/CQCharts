#ifndef CQChartsData_H
#define CQChartsData_H

#include <CQChartsColor.h>
#include <CQChartsLength.h>
#include <CQChartsPosition.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsLineDash.h>
#include <CQChartsFillPattern.h>
#include <QFont>

// Text Properties
//   font, color, alpha, angle, contrast (shadow), formatted, scaled
//
// TODO shadow properties
struct CQChartsTextData {
  bool          visible   { true };
  QFont         font;
  CQChartsColor color     { CQChartsColor::Type::INTERFACE_VALUE, 1.0 };
  double        alpha     { 1.0 };
  double        angle     { 0.0 };
  bool          contrast  { false };
  Qt::Alignment align     { Qt::AlignLeft | Qt::AlignVCenter };
  bool          formatted { false };
  bool          scaled    { false };
  bool          html      { false };
};

// Line Properties
struct CQChartsLineData {
  bool             visible { true };
  CQChartsColor    color   { CQChartsColor::Type::INTERFACE_VALUE, 1.0 };
  double           alpha   { 1.0 };
  CQChartsLength   width   { "0px" };
  CQChartsLineDash dash    { };
};

// Fill Properties
struct CQChartsFillData {
  bool                visible { true };                                      // draw bg
  CQChartsColor       color   { CQChartsColor::Type::INTERFACE_VALUE, 0.0 }; // bg fill color
  double              alpha   { 1.0 };                                       // bg fill color alpha
  CQChartsFillPattern pattern { CQChartsFillPattern::Type::SOLID };          // bg fill pattern
};

// Stroke Properties
struct CQChartsStrokeData {
  bool             visible { true };                                      // draw border
  CQChartsColor    color   { CQChartsColor::Type::INTERFACE_VALUE, 1.0 }; // border color
  double           alpha   { 1.0 };                                       // border color alpha
  CQChartsLength   width   { "0px" };                                     // stroke width
  CQChartsLineDash dash    { };                                           // stroke dash
};

// Shape Properties
struct CQChartsShapeData {
  CQChartsFillData   background; // background data
  CQChartsStrokeData border;     // border data
};

// Box Properties
//  TODO: border corners
struct CQChartsBoxData {
  bool              visible     { true };   // draw box
  double            margin      { 4 };      // inside margin (pixels)
  double            padding     { 0 };      // outside margin (pixels)
  CQChartsShapeData shape;                  // shape data
  CQChartsLength    cornerSize  { "0px" };  // border rounded corner size
  QString           borderSides { "tlbr" }; // border sides to draw
};

// Text in Box Properties
struct CQChartsTextBoxData {
  CQChartsTextData text;
  CQChartsBoxData  box;
};

struct CQChartsSymbolData {
  bool               visible { true };
  CQChartsSymbol     type;
  CQChartsLength     size    { "5px" };
  CQChartsStrokeData stroke;
  CQChartsFillData   fill;

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
