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

/*!
 * \brief Text Properties
 *   visible, color, alpha, font, angle, contrast (shadow), formatted, scaled
 */
class CQChartsTextData {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsTextData() = default;

  explicit CQChartsTextData(const QString &str) {
    (void) fromString(str);
  }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &v) { color_ = v; }

  double alpha() const { return alpha_; }
  void setAlpha(double r) { alpha_ = r; }

  const QFont &font() const { return font_; }
  void setFont(const QFont &v) { font_ = v; }

  double angle() const { return angle_; }
  void setAngle(double r) { angle_ = r; }

  bool isContrast() const { return contrast_; }
  void setContrast(bool b) { contrast_ = b; }

  const Qt::Alignment &align() const { return align_; }
  void setAlign(const Qt::Alignment &v) { align_ = v; }

  bool isFormatted() const { return formatted_; }
  void setFormatted(bool b) { formatted_ = b; }

  bool isScaled() const { return scaled_; }
  void setScaled(bool b) { scaled_ = b; }

  bool isHtml() const { return html_; }
  void setHtml(bool b) { html_ = b; }

  //---

  bool isValid() const { return true; }

  QString toString() const;

  bool fromString(const QString &s);

 private:
  bool          visible_   { true };                                      //! is visible
  CQChartsColor color_     { CQChartsColor::Type::INTERFACE_VALUE, 1.0 }; //! color
  double        alpha_     { 1.0 };                                       //! alpha
  QFont         font_;                                                    //! font
  double        angle_     { 0.0 };                                       //! angle
  bool          contrast_  { false };                                     //! contrast
  Qt::Alignment align_     { Qt::AlignLeft | Qt::AlignVCenter };          //! align
  bool          formatted_ { false };                                     //! formatted
  bool          scaled_    { false };                                     //! scaled
  bool          html_      { false };                                     //! html
};

CQUTIL_DCL_META_TYPE(CQChartsTextData)

//------

/*!
 * \brief Line Properties
 *   visible, color, alpha, width, dash
 */
class CQChartsLineData {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsLineData() = default;

  explicit CQChartsLineData(const QString &str) {
    (void) fromString(str);
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

  bool fromString(const QString &s);

 private:
  bool             visible_ { true };                                      //! visible
  CQChartsColor    color_   { CQChartsColor::Type::INTERFACE_VALUE, 1.0 }; //! color
  double           alpha_   { 1.0 };                                       //! alpha
  CQChartsLength   width_   { "0px" };                                     //! width
  CQChartsLineDash dash_    { };                                           //! dash
};

CQUTIL_DCL_META_TYPE(CQChartsLineData)

//------

/*!
 * \brief Fill Properties
 *   visible, color, alpha, pattern
 */
class CQChartsFillData {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsFillData() = default;

  explicit CQChartsFillData(const QString &str) {
    (void) fromString(str);
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

  bool fromString(const QString &s);

 private:
  bool                visible_ { true };                             //! draw bg
  CQChartsColor       color_   { CQChartsColor::Type::INTERFACE_VALUE, 0.0 }; //! bg fill color
  double              alpha_   { 1.0 };                              //! bg fill color alpha
  CQChartsFillPattern pattern_ { CQChartsFillPattern::Type::SOLID }; //! bg fill pattern
};

CQUTIL_DCL_META_TYPE(CQChartsFillData)

//------

/*!
 * \brief Stroke Properties
 *   visible, color, alpha, width, dash, corner size
 */
class CQChartsStrokeData {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsStrokeData() = default;

  explicit CQChartsStrokeData(const QString &str) {
    (void) fromString(str);
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

  bool fromString(const QString &s);

 private:
  bool             visible_    { true };  //! draw border
  CQChartsColor    color_      { CQChartsColor::Type::INTERFACE_VALUE, 1.0 }; //! border color
  double           alpha_      { 1.0 };   //! border color alpha
  CQChartsLength   width_      { "0px" }; //! stroke width
  CQChartsLineDash dash_       { };       //! stroke dash
  CQChartsLength   cornerSize_ { "0px" }; //! border corner size
};

CQUTIL_DCL_META_TYPE(CQChartsStrokeData)

//------

/*!
 * \brief Shape Properties
 *  background, border
 */
class CQChartsShapeData {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsShapeData() = default;

  explicit CQChartsShapeData(const QString &str) {
    (void) fromString(str);
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

  bool fromString(const QString &s);

 private:
  CQChartsFillData   background_; //! background data
  CQChartsStrokeData border_;     //! border data
};

CQUTIL_DCL_META_TYPE(CQChartsShapeData)

//------

/*!
 * \brief Box Properties
 *  visible, inner margin, outer margin, shape, sides
 *
 *  \todo border corners
 */
struct CQChartsBoxData {
  bool              visible     { true };   //! draw box
  double            margin      { 4 };      //! inside margin (pixels)
  double            padding     { 0 };      //! outside margin (pixels)
  CQChartsShapeData shape;                  //! shape data
  CQChartsSides     borderSides { "tlbr" }; //! border sides to draw
};

//------

/*!
 * \brief Text in Box Properties
 *   text, box
 */
struct CQChartsTextBoxData {
  CQChartsTextData text; //! text data
  CQChartsBoxData  box;  //! box data
};

//------

/*!
 * \brief Symbol Properties
 *   visible, type, size, stroke, fill
 */
struct CQChartsSymbolData {
  bool               visible { true };  //! visible
  CQChartsSymbol     type;              //! symbol type
  CQChartsLength     size    { "5px" }; //! symbol size
  CQChartsStrokeData stroke;            //! symbol stroke
  CQChartsFillData   fill;              //! symbol fill

  CQChartsSymbolData() {
    fill.setVisible(false);
  }
};

//------

/*!
 * \brief Arrow Properties
 *   relative, length, angle, back angle, front head, tail head, line edits, line width
 */
struct CQChartsArrowData {
  bool           relative  { false }; //! to point relative to from
  CQChartsLength length    { "1V" };  //! length
  double         angle     { -1 };    //! arrow angle
  double         backAngle { -1 };    //! arrow back angle
  bool           fhead     { false }; //! draw arrow head at front
  bool           thead     { true };  //! draw arrow head at tail
//bool           empty     { false }; //! draw empty head (unfilled stroked)
  bool           lineEnds  { false }; //! lines at end
  CQChartsLength lineWidth { -1 };    //! connecting line width
};

#endif
