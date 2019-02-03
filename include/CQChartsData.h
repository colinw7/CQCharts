#ifndef CQChartsData_H
#define CQChartsData_H

#include <CQChartsColor.h>
#include <CQChartsLength.h>
#include <CQChartsPosition.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsLineDash.h>
#include <CQChartsFillPattern.h>
#include <CQChartsSides.h>
#include <CQUtilMeta.h>
#include <QFont>

//------

// Text Properties
//   visible, font, color, alpha, angle, contrast (shadow), formatted, scaled
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

//------

// Line Properties
//   visible, color, alpha, width, dash
class CQChartsLineData {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsLineData() = default;

  explicit CQChartsLineData(const QString &str) {
    fromString(str);
  }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &v) { color_ = v; }

  double alpha() const { return alpha_; }
  void setAlpha(double r) { alpha_ = r; }

  const CQChartsLength &width() const { return width_; }
  void setWidth(const CQChartsLength &v) { width_ = v; }

  const CQChartsLineDash &dash() const { return dash_; }
  void setDash(const CQChartsLineDash &v) { dash_ = v; }

  //---

  bool isValid() const { return true; }

  QString toString() const;

  void fromString(const QString &s);

 private:
  bool             visible_ { true };                                      // visible
  CQChartsColor    color_   { CQChartsColor::Type::INTERFACE_VALUE, 1.0 }; // color
  double           alpha_   { 1.0 };                                       // alpha
  CQChartsLength   width_   { "0px" };                                     // width
  CQChartsLineDash dash_    { };                                           // dash
};

CQUTIL_DCL_META_TYPE(CQChartsLineData)

//------

// Fill Properties
//   visible, color, alpha, pattern
class CQChartsFillData {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsFillData() = default;

  explicit CQChartsFillData(const QString &str) {
    fromString(str);
  }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &v) { color_ = v; }

  double alpha() const { return alpha_; }
  void setAlpha(double r) { alpha_ = r; }

  const CQChartsFillPattern &pattern() const { return pattern_; }
  void setPattern(const CQChartsFillPattern &v) { pattern_ = v; }

  //---

  bool isValid() const { return true; }

  QString toString() const;

  void fromString(const QString &s);

 private:
  bool                visible_ { true };                                      // draw bg
  CQChartsColor       color_   { CQChartsColor::Type::INTERFACE_VALUE, 0.0 }; // bg fill color
  double              alpha_   { 1.0 };                                       // bg fill color alpha
  CQChartsFillPattern pattern_ { CQChartsFillPattern::Type::SOLID };          // bg fill pattern
};

CQUTIL_DCL_META_TYPE(CQChartsFillData)

//------

// Stroke Properties
//   visible, color, alpha, width, dash, corner size
class CQChartsStrokeData {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsStrokeData() = default;

  explicit CQChartsStrokeData(const QString &str) {
    fromString(str);
  }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &v) { color_ = v; }

  double alpha() const { return alpha_; }
  void setAlpha(double r) { alpha_ = r; }

  const CQChartsLength &width() const { return width_; }
  void setWidth(const CQChartsLength &v) { width_ = v; }

  const CQChartsLineDash &dash() const { return dash_; }
  void setDash(const CQChartsLineDash &v) { dash_ = v; }

  const CQChartsLength &cornerSize() const { return cornerSize_; }
  void setCornerSize(const CQChartsLength &v) { cornerSize_ = v; }

  //---

  bool isValid() const { return true; }

  QString toString() const;

  void fromString(const QString &s);

 private:
  bool             visible_    { true };                                      // draw border
  CQChartsColor    color_      { CQChartsColor::Type::INTERFACE_VALUE, 1.0 }; // border color
  double           alpha_      { 1.0 };                                       // border color alpha
  CQChartsLength   width_      { "0px" };                                     // stroke width
  CQChartsLineDash dash_       { };                                           // stroke dash
  CQChartsLength   cornerSize_ { "0px" };                                     // border corner size
};

CQUTIL_DCL_META_TYPE(CQChartsStrokeData)

//------

// Shape Properties
//  background, border
class CQChartsShapeData {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsShapeData() = default;

  explicit CQChartsShapeData(const QString &str) {
    fromString(str);
  }

  CQChartsFillData &background() { return background_; }
  const CQChartsFillData &background() const { return background_; }
  void setBackground(const CQChartsFillData &v) { background_ = v; }

  CQChartsStrokeData &border() { return border_; }
  const CQChartsStrokeData &border() const { return border_; }
  void setBorder(const CQChartsStrokeData &v) { border_ = v; }

  //---

  bool isValid() const { return background_.isValid() && border_.isValid(); }

  QString toString() const;

  void fromString(const QString &s);

 private:
  CQChartsFillData   background_; // background data
  CQChartsStrokeData border_;     // border data
};

CQUTIL_DCL_META_TYPE(CQChartsShapeData)

//------

// Box Properties
//  visible, inner margin, outer margin, shape, sides
//
//  TODO: border corners
struct CQChartsBoxData {
  bool              visible     { true };   // draw box
  double            margin      { 4 };      // inside margin (pixels)
  double            padding     { 0 };      // outside margin (pixels)
  CQChartsShapeData shape;                  // shape data
  CQChartsSides     borderSides { "tlbr" }; // border sides to draw
};

//------

// Text in Box Properties
//   text, box
struct CQChartsTextBoxData {
  CQChartsTextData text;
  CQChartsBoxData  box;
};

//------

// Symbol Properties
//   visible, type, size, stroke, fill
struct CQChartsSymbolData {
  bool               visible { true };
  CQChartsSymbol     type;
  CQChartsLength     size    { "5px" };
  CQChartsStrokeData stroke;
  CQChartsFillData   fill;

  CQChartsSymbolData() {
    fill.setVisible(false);
  }
};

//------

// Arrow Properties
//   relative, length, angle, back angle, front head, tail head, line edits, line width
struct CQChartsArrowData {
  bool           relative  { false }; // to point relative to from
  CQChartsLength length    { "1V" };  // length
  double         angle     { -1 };    // arrow angle
  double         backAngle { -1 };    // arrow back angle
  bool           fhead     { false }; // draw arrow head at front
  bool           thead     { true };  // draw arrow head at tail
//bool           empty     { false }; // draw empty head (unfilled stroked)
  bool           lineEnds  { false }; // lines at end
  CQChartsLength lineWidth { -1 };    // connecting line width
};

#endif
