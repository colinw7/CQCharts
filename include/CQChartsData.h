#ifndef CQChartsData_H
#define CQChartsData_H

#include <CQChartsColor.h>
#include <CQChartsAlpha.h>
#include <CQChartsFont.h>
#include <CQChartsMargin.h>
#include <CQChartsPosition.h>
#include <CQChartsSymbol.h>
#include <CQChartsLineDash.h>
#include <CQChartsLineCap.h>
#include <CQChartsLineJoin.h>
#include <CQChartsFillPattern.h>
#include <CQChartsSides.h>
#include <CQChartsAngle.h>
#include <CQUtilMeta.h>

class CQChartsNameValues;

//------

/*!
 * \brief Text Properties
 * \ingroup Charts
 *
 * visible, color, alpha, font, angle, contrast (shadow), contrast alpha,
 * alignment, formatted, scaled, html, clip length
 */
class CQChartsTextData {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsTextData, metaTypeId)

 public:
  using Color  = CQChartsColor;
  using Alpha  = CQChartsAlpha;
  using Font   = CQChartsFont;
  using Angle  = CQChartsAngle;
  using Length = CQChartsLength;

 public:
  CQChartsTextData() = default;

  //! create from string
  explicit CQChartsTextData(const QString &str) {
    (void) fromString(str);
  }

  //---

  //! get/set visible
  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  //! get/set color
  const Color &color() const { return color_; }
  void setColor(const Color &c) { color_ = c; }

  //! get/set alpha
  const Alpha &alpha() const { return alpha_; }
  void setAlpha(const Alpha &a) { alpha_ = a; }

  //! get/set font
  const Font &font() const { return font_; }
  void setFont(const Font &f) { font_ = f; }

  //! get/set angle
  const Angle &angle() const { return angle_; }
  void setAngle(const Angle &a) { angle_ = a; }

  //! get/set draw contrast background
  bool isContrast() const { return contrast_; }
  void setContrast(bool b) { contrast_ = b; }

  //! get/set contrast alpha
  const Alpha &contrastAlpha() const { return contrastAlpha_; }
  void setContrastAlpha(const Alpha &a) { contrastAlpha_ = a; }

  //! get/set text alignment
  const Qt::Alignment &align() const { return align_; }
  void setAlign(const Qt::Alignment &a) { align_ = a; }

  //! get/set is formatted in rectangle (lines broken to fit)
  bool isFormatted() const { return formatted_; }
  void setFormatted(bool b) { formatted_ = b; }

  //! get/set is scaled
  bool isScaled() const { return scaled_; }
  void setScaled(bool b) { scaled_ = b; }

  //! get/set text is html
  bool isHtml() const { return html_; }
  void setHtml(bool b) { html_ = b; }

  //! get/set clip length
  const Length &clipLength() const { return clipLength_; }
  void setClipLength(const Length &l) { clipLength_ = l; }

  //! get/set clip elide
  const Qt::TextElideMode &clipElide() const { return clipElide_; }
  void setClipElide(const Qt::TextElideMode &e) { clipElide_ = e; }

  //---

  //! get is valid
  bool isValid() const { return true; }

  //---

  //! convert to/from string representation (property)
  QString toString() const;
  bool fromString(const QString &s);

  //---

  /*! get/set properties from name value pairs
   *    "visible"       : is visible
   *    "color"         : color
   *    "alpha"         : alpha
   *    "font"          : font
   *    "angle"         : angle
   *    "contrast"      : is contrast
   *    "contrastAlpha" : contrast alpha
   *    "align"         : alignment
   *    "formatted"     : formatted
   *    "scaled"        : scaled
   *    "html"          : is html
   *    "clipLength"    : clip length
   *    "clipElide"     : clip elide
   */
  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

 private:
  bool              visible_       { true };                             //!< is visible
  Color             color_         { Color::makeInterfaceValue(1.0) };   //!< color
  Alpha             alpha_;                                              //!< alpha
  Font              font_;                                               //!< font
  Angle             angle_;                                              //!< angle
  bool              contrast_      { false };                            //!< contrast
  Alpha             contrastAlpha_ { 0.5 };                              //!< contrast alpha
  Qt::Alignment     align_         { Qt::AlignLeft | Qt::AlignVCenter }; //!< align
  bool              formatted_     { false };                            //!< formatted
  bool              scaled_        { false };                            //!< scaled
  bool              html_          { false };                            //!< html
  Length            clipLength_;                                         //!< clip length
  Qt::TextElideMode clipElide_     { Qt::ElideRight };                   //!< clip elide
};

CQUTIL_DCL_META_TYPE(CQChartsTextData)

//------

/*!
 * \brief Line Properties
 * \ingroup Charts
 *
 *   visible, color, alpha, width, dash, line cap, line join
 */
class CQChartsLineData {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsLineData, metaTypeId)

 public:
  using Color    = CQChartsColor;
  using Alpha    = CQChartsAlpha;
  using Length   = CQChartsLength;
  using LineDash = CQChartsLineDash;
  using LineCap  = CQChartsLineCap;
  using LineJoin = CQChartsLineJoin;

 public:
  CQChartsLineData() = default;

  explicit CQChartsLineData(const QString &str) {
    (void) fromString(str);
  }

  explicit CQChartsLineData(bool visible, const Color &color=Color(), const Alpha &alpha=Alpha(),
                            const Length &width=Length(), const LineDash &dash=LineDash(),
                            const LineCap &lineCap=LineCap(), const LineJoin &lineJoin=LineJoin()) :
   visible_(visible), color_(color), alpha_(alpha), width_(width),
   dash_(dash), lineCap_(lineCap), lineJoin_(lineJoin) {
  }

  //---

  //! get/set visible
  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  //! get/set color
  const Color &color() const { return color_; }
  void setColor(const Color &c) { color_ = c; }

  //! get/set alpha
  const Alpha &alpha() const { return alpha_; }
  void setAlpha(const Alpha &a) { alpha_ = a; }

  //! get/set width
  const Length &width() const { return width_; }
  void setWidth(const Length &l) { width_ = l; }

  //! get/set line dash
  const LineDash &dash() const { return dash_; }
  void setDash(const LineDash &d) { dash_ = d; }

  //! get/set line cap
  const LineCap &lineCap() const { return lineCap_; }
  void setLineCap(const LineCap &c) { lineCap_ = c; }

  //! get/set line join
  const LineJoin &lineJoin() const { return lineJoin_; }
  void setLineJoin(const LineJoin &j) { lineJoin_ = j; }

  //---

  bool isValid() const { return true; }

  QString toString() const;
  bool fromString(const QString &s);

  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

 private:
  bool     visible_  { true };                           //!< line visible
  Color    color_    { Color::makeInterfaceValue(1.0) }; //!< line color
  Alpha    alpha_;                                       //!< line alpha
  Length   width_    { Length::pixel(0) };               //!< line width
  LineDash dash_;                                        //!< line dash
  LineCap  lineCap_;                                     //!< line cap
  LineJoin lineJoin_;                                    //!< line join
};

CQUTIL_DCL_META_TYPE(CQChartsLineData)

//------

/*!
 * \brief Fill Properties
 * \ingroup Charts
 *
 *   visible, color, alpha, pattern
 */
class CQChartsFillData {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsFillData, metaTypeId)

 public:
  using Color       = CQChartsColor;
  using Alpha       = CQChartsAlpha;
  using FillPattern = CQChartsFillPattern;

 public:
  CQChartsFillData() = default;

  explicit CQChartsFillData(const QString &str) {
    (void) fromString(str);
  }

  explicit CQChartsFillData(bool visible, const Color &color=Color(), const Alpha &alpha=Alpha(),
                            const FillPattern &pattern=FillPattern::makeSolid()) :
   visible_(visible), color_(color), alpha_(alpha), pattern_(pattern) {
  }

  //---

  //! get/set visible
  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  //! get/set color
  const Color &color() const { return color_; }
  void setColor(const Color &c) { color_ = c; }

  //! get/set alpha
  const Alpha &alpha() const { return alpha_; }
  void setAlpha(const Alpha &a) { alpha_ = a; }

  const FillPattern &pattern() const { return pattern_; }
  void setPattern(const FillPattern &p) { pattern_ = p; }

  //---

  bool isValid() const { return true; }

  QString toString() const;
  bool fromString(const QString &s);

  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

 private:
  bool        visible_  { true };                           //!< visible
  Color       color_    { Color::makeInterfaceValue(0.0) }; //!< fill color
  Alpha       alpha_;                                       //!< fill alpha
  FillPattern pattern_  { FillPattern::Type::SOLID };       //!< fill pattern
};

CQUTIL_DCL_META_TYPE(CQChartsFillData)

//------

/*!
 * \brief Stroke Properties
 * \ingroup Charts
 *
 *   visible, color, alpha, width, dash, corner size
 */
class CQChartsStrokeData {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsStrokeData, metaTypeId)

 public:
  using Color    = CQChartsColor;
  using Alpha    = CQChartsAlpha;
  using Length   = CQChartsLength;
  using LineDash = CQChartsLineDash;
  using LineCap  = CQChartsLineCap;
  using LineJoin = CQChartsLineJoin;

 public:
  CQChartsStrokeData() = default;

  explicit CQChartsStrokeData(const QString &str) {
    (void) fromString(str);
  }

  //---

  //! get/set visible
  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  //! get/set color
  const Color &color() const { return color_; }
  void setColor(const Color &c) { color_ = c; }

  //! get/set alpha
  const Alpha &alpha() const { return alpha_; }
  void setAlpha(const Alpha &a) { alpha_ = a; }

  //! get/set width
  const Length &width() const { return width_; }
  void setWidth(const Length &w) { width_ = w; }

  //! get/set line dash
  const LineDash &dash() const { return dash_; }
  void setDash(const LineDash &d) { dash_ = d; }

  //! get/set line cap
  const LineCap &lineCap() const { return lineCap_; }
  void setLineCap(const LineCap &c) { lineCap_ = c; }

  //! get/set line join
  const LineJoin &lineJoin() const { return lineJoin_; }
  void setLineJoin(const LineJoin &j) { lineJoin_ = j; }

  //! get/set corner size
  const Length &cornerSize() const { return cornerSize_; }
  void setCornerSize(const Length &l) { cornerSize_ = l; }

  //---

  bool isValid() const { return true; }

  QString toString() const;
  bool fromString(const QString &s);

  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

 private:
  bool     visible_    { true };                           //!< draw stroke
  Color    color_      { Color::makeInterfaceValue(1.0) }; //!< stroke color
  Alpha    alpha_;                                         //!< stroke color alpha
  Length   width_      { Length::pixel(0) };               //!< stroke width
  LineDash dash_;                                          //!< stroke line dash
  LineCap  lineCap_;                                       //!< stroke line cap
  LineJoin lineJoin_;                                      //!< stroke line join
  Length   cornerSize_ { Length::pixel(0) };               //!< corner size (for border)
};

CQUTIL_DCL_META_TYPE(CQChartsStrokeData)

//------

/*!
 * \brief Shape Properties
 * \ingroup Charts
 *
 *  fill, stroke
 */
class CQChartsShapeData {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsShapeData, metaTypeId)

 public:
  using FillData   = CQChartsFillData;
  using StrokeData = CQChartsStrokeData;

 public:
  CQChartsShapeData() = default;

  explicit CQChartsShapeData(const QString &str) {
    (void) fromString(str);
  }

  FillData &fill() { return fill_; }
  const FillData &fill() const { return fill_; }
  void setFill(const FillData &f) { fill_ = f; }

  StrokeData &stroke() { return stroke_; }
  const StrokeData &stroke() const { return stroke_; }
  void setStroke(const StrokeData &s) { stroke_ = s; }

  //---

  bool isValid() const { return fill_.isValid() || stroke_.isValid(); }

  QString toString() const;
  bool fromString(const QString &s);

  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

 private:
  FillData   fill_;   //!< fill data
  StrokeData stroke_; //!< stroke data
};

CQUTIL_DCL_META_TYPE(CQChartsShapeData)

//------

/*!
 * \brief Box Properties
 * \ingroup Charts
 *
 *  visible, inner padding, outer margin, shape, sides
 *
 *  \todo border corners
 */
class CQChartsBoxData {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsBoxData, metaTypeId)

 public:
  using Margin    = CQChartsMargin;
  using ShapeData = CQChartsShapeData;
  using Sides     = CQChartsSides;

 public:
  CQChartsBoxData() = default;

  explicit CQChartsBoxData(const QString &str) {
    (void) fromString(str);
  }

  //---

  //! get/set visible
  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  const Margin &margin() const { return margin_; }
  void setMargin(const Margin &m) { margin_ = m; }

  const Margin &padding() const { return padding_; }
  void setPadding(const Margin &m) { padding_ = m; }

  ShapeData &shape() { return shape_; }
  const ShapeData &shape() const { return shape_; }
  void setShape(const ShapeData &s) { shape_ = s; }

  const Sides &borderSides() const { return borderSides_; }
  void setBorderSides(const Sides &s) { borderSides_ = s; }

  //---

  bool isValid() const { return true; }

  QString toString() const;
  bool fromString(const QString &s);

  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

 private:
  bool      visible_     { true };                      //!< draw box
  Margin    margin_      { Margin::pixel(4, 0, 4, 0) }; //!< outer margin
  Margin    padding_     { Margin::pixel(0, 0, 0, 0) }; //!< inner padding
  ShapeData shape_;                                     //!< shape data
  Sides     borderSides_ { "tlbr" };                    //!< border sides to draw
};

CQUTIL_DCL_META_TYPE(CQChartsBoxData)

//------

/*!
 * \brief Text in Box Properties
 * \ingroup Charts
 *
 *   text, box
 */
class CQChartsTextBoxData {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsTextBoxData, metaTypeId)

 public:
  using TextData = CQChartsTextData;
  using BoxData  = CQChartsBoxData;

 public:
  CQChartsTextBoxData() = default;

  explicit CQChartsTextBoxData(const QString &str) {
    (void) fromString(str);
  }

  TextData &text() { return text_; }
  const TextData &text() const { return text_; }
  void setText(const TextData &t) { text_ = t; }

  BoxData &box() { return box_; }
  const BoxData &box() const { return box_; }
  void setBox(const BoxData &b) { box_ = b; }

  //---

  bool isValid() const { return true; }

  QString toString() const;
  bool fromString(const QString &s);

  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

 private:
  TextData text_; //!< text data
  BoxData  box_;  //!< box data
};

CQUTIL_DCL_META_TYPE(CQChartsTextBoxData)

//------

/*!
 * \brief Symbol Properties
 * \ingroup Charts
 *
 *   visible, symbol, size, stroke, fill
 */
class CQChartsSymbolData {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsSymbolData, metaTypeId)

 public:
  using Symbol     = CQChartsSymbol;
  using Length     = CQChartsLength;
  using StrokeData = CQChartsStrokeData;
  using FillData   = CQChartsFillData;

 public:
  CQChartsSymbolData() {
    fill_.setVisible(false);
  }

  explicit CQChartsSymbolData(const QString &str) {
    fill_.setVisible(false);

    (void) fromString(str);
  }

  //---

  //! get/set visible
  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  const Symbol &symbol() const { return symbol_; }
  void setSymbol(const Symbol &s) { symbol_ = s; }

  const Length &size() const { return size_; }
  void setSize(const Length &l) { size_ = l; }

  StrokeData &stroke() { return stroke_; }
  const StrokeData &stroke() const { return stroke_; }
  void setStroke(const StrokeData &s) { stroke_ = s; }

  FillData &fill() { return fill_; }
  const FillData &fill() const { return fill_; }
  void setFill(const FillData &f) { fill_ = f; }

  //---

  bool isValid() const { return true; }

  QString toString() const;
  bool fromString(const QString &s);

  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

 private:
  bool       visible_ { true };             //!< visible
  Symbol     symbol_;                       //!< symbol
  Length     size_    { Length::pixel(5) }; //!< symbol size
  StrokeData stroke_;                       //!< symbol stroke
  FillData   fill_;                         //!< symbol fill
};

CQUTIL_DCL_META_TYPE(CQChartsSymbolData)

//------

#include <CQChartsRect.h>
#include <CQChartsTitleLocation.h>

/*!
 * \brief title data
 * \ingroup Charts
 */
struct CQChartsTitleData {
  bool                  visible   { true };
  CQChartsTitleLocation location;
  CQChartsPosition      position;
  CQChartsRect          rect;
  bool                  insidePlot { false };
  CQChartsTextData      textData;
};

//------

#include <CQChartsKeyLocation.h>
#include <CQChartsKeyPressBehavior.h>
#include <CQChartsOptLength.h>

/*!
 * \brief key data
 * \ingroup Charts
 */
struct CQChartsKeyData {
  using Point = CQChartsGeom::Point;
  using Alpha = CQChartsAlpha;

  bool                     visible      { true };
  bool                     horizontal   { false };
  bool                     autoHide     { false };
  bool                     clipped      { false };
  bool                     above        { false };
  CQChartsKeyLocation      location;
  bool                     insideX      { false };
  bool                     insideY      { false };
  Point                    absolutePosition;
  Alpha                    hiddenAlpha  { 0.3 };
  int                      maxRows;
  bool                     interactive  { false };
  CQChartsKeyPressBehavior pressBehavior;
  double                   spacing      { 2 }; // pixels
  bool                     flipped      { false };
  QString                  header;
  CQChartsTextData         headerTextData;
  CQChartsOptLength        scrollWidth;
  CQChartsOptLength        scrollHeight;
  CQChartsTextBoxData      textBoxData;
};

//------

#include <CQChartsAxisSide.h>
#include <CQChartsAxisValueType.h>
#include <CQChartsOptReal.h>

/*!
 * \brief axis data
 * \ingroup Charts
 */
struct CQChartsAxisData {
  bool                  visible        { true };
  Qt::Orientation       direction      { Qt::Horizontal };
  CQChartsAxisSide      side;
  CQChartsAxisValueType valueType;
  QString               format         { false };
  int                   tickIncrement  { 0 };
  int                   majorIncrement { 0 };
  double                start          { 0.0 };
  double                end            { 0.0 };
  bool                  includeZero;
  CQChartsOptReal       position;
  CQChartsLineData      lineData;
  CQChartsTextData      tickLabelTextData;
  CQChartsTextData      labelTextData;
  CQChartsLineData      majorGridLineData;
  CQChartsLineData      minorGridLineData;
  CQChartsFillData      gridFillData;
};

//------

enum class CQChartsShapeType {
  NONE,
  TRIANGLE,
  DIAMOND,
  BOX,
  POLYGON,
  CIRCLE,
  DOUBLE_CIRCLE,
  DOT_LINE,
  RECORD,
  PLAIN_TEXT,
  RARROW,
  OVAL,
  RPROMOTER
};

struct CQChartsShapeTypeData {
  CQChartsShapeType shapeType { CQChartsShapeType::NONE }; //!< shape type
  int               numSides  { -1 };                      //!< number of polygon sides (< 0 unset)
  double            aspect    { -1.0 };                    //!< aspect (< 0 unset)
};

#endif
