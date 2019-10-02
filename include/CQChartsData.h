#ifndef CQChartsData_H
#define CQChartsData_H

#include <CQChartsColor.h>
#include <CQChartsFont.h>
#include <CQChartsLength.h>
#include <CQChartsPosition.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsLineDash.h>
#include <CQChartsFillPattern.h>
#include <CQChartsSides.h>
#include <CQUtilMeta.h>

//------

/*!
 * \brief Text Properties
 * \ingroup Charts
 *
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

  const CQChartsFont &font() const { return font_; }
  void setFont(const CQChartsFont &v) { font_ = v; }

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

  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

 private:
  bool          visible_   { true };                                      //!< is visible
  CQChartsColor color_     { CQChartsColor::Type::INTERFACE_VALUE, 1.0 }; //!< color
  double        alpha_     { 1.0 };                                       //!< alpha
  CQChartsFont  font_;                                                    //!< font
  double        angle_     { 0.0 };                                       //!< angle
  bool          contrast_  { false };                                     //!< contrast
  Qt::Alignment align_     { Qt::AlignLeft | Qt::AlignVCenter };          //!< align
  bool          formatted_ { false };                                     //!< formatted
  bool          scaled_    { false };                                     //!< scaled
  bool          html_      { false };                                     //!< html
};

CQUTIL_DCL_META_TYPE(CQChartsTextData)

//------

/*!
 * \brief Line Properties
 * \ingroup Charts
 *
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

  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

 private:
  bool             visible_ { true };                                      //!< visible
  CQChartsColor    color_   { CQChartsColor::Type::INTERFACE_VALUE, 1.0 }; //!< color
  double           alpha_   { 1.0 };                                       //!< alpha
  CQChartsLength   width_   { "0px" };                                     //!< width
  CQChartsLineDash dash_    { };                                           //!< dash
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

  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

 private:
  bool                visible_ { true };                             //!< draw background
  CQChartsColor       color_   {
                        CQChartsColor::Type::INTERFACE_VALUE, 0.0 }; //!< background fill color
  double              alpha_   { 1.0 };                              //!< background fill alpha
  CQChartsFillPattern pattern_ {
                        CQChartsFillPattern::Type::SOLID };          //!< background fill pattern
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

  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

 private:
  bool             visible_    { true };  //!< draw stroke
  CQChartsColor    color_      { CQChartsColor::Type::INTERFACE_VALUE, 1.0 }; //!< stroke color
  double           alpha_      { 1.0 };   //!< stroke color alpha
  CQChartsLength   width_      { "0px" }; //!< stroke width
  CQChartsLineDash dash_       { };       //!< stroke dash
  CQChartsLength   cornerSize_ { "0px" }; //!< corner size
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

 public:
  CQChartsShapeData() = default;

  explicit CQChartsShapeData(const QString &str) {
    (void) fromString(str);
  }

  CQChartsFillData &fill() { return fill_; }
  const CQChartsFillData &fill() const { return fill_; }
  void setFill(const CQChartsFillData &v) { fill_ = v; }

  CQChartsStrokeData &stroke() { return stroke_; }
  const CQChartsStrokeData &stroke() const { return stroke_; }
  void setStroke(const CQChartsStrokeData &v) { stroke_ = v; }

  //---

  bool isValid() const { return fill_.isValid() && stroke_.isValid(); }

  QString toString() const;
  bool fromString(const QString &s);

  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

 private:
  CQChartsFillData   fill_;   //!< fill data
  CQChartsStrokeData stroke_; //!< stroke data
};

CQUTIL_DCL_META_TYPE(CQChartsShapeData)

//------

/*!
 * \brief Box Properties
 * \ingroup Charts
 *
 *  visible, inner margin, outer margin, shape, sides
 *
 *  \todo border corners
 */
class CQChartsBoxData {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsBoxData() = default;

  explicit CQChartsBoxData(const QString &str) {
    (void) fromString(str);
  }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  double margin() const { return margin_; }
  void setMargin(double r) { margin_ = r; }

  double padding() const { return padding_; }
  void setPadding(double r) { padding_ = r; }

  CQChartsShapeData &shape() { return shape_; }
  const CQChartsShapeData &shape() const { return shape_; }
  void setShape(const CQChartsShapeData &v) { shape_ = v; }

  const CQChartsSides &borderSides() const { return borderSides_; }
  void setBorderSides(const CQChartsSides &v) { borderSides_ = v; }

  //---

  bool isValid() const { return true; }

  QString toString() const;
  bool fromString(const QString &s);

  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

 private:
  bool              visible_     { true };   //!< draw box
  double            margin_      { 4 };      //!< inside margin (pixels)
  double            padding_     { 0 };      //!< outside margin (pixels)
  CQChartsShapeData shape_;                  //!< shape data
  CQChartsSides     borderSides_ { "tlbr" }; //!< border sides to draw
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

 public:
  CQChartsTextBoxData() = default;

  explicit CQChartsTextBoxData(const QString &str) {
    (void) fromString(str);
  }

  CQChartsTextData &text() { return text_; }
  const CQChartsTextData &text() const { return text_; }
  void setText(const CQChartsTextData &v) { text_ = v; }

  CQChartsBoxData &box() { return box_; }
  const CQChartsBoxData &box() const { return box_; }
  void setBox(const CQChartsBoxData &v) { box_ = v; }

  //---

  bool isValid() const { return true; }

  QString toString() const;
  bool fromString(const QString &s);

  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

 private:
  CQChartsTextData text_; //!< text data
  CQChartsBoxData  box_;  //!< box data
};

CQUTIL_DCL_META_TYPE(CQChartsTextBoxData)

//------

/*!
 * \brief Symbol Properties
 * \ingroup Charts
 *
 *   visible, type, size, stroke, fill
 */
class CQChartsSymbolData {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsSymbolData() {
    fill_.setVisible(false);
  }

  explicit CQChartsSymbolData(const QString &str) {
    fill_.setVisible(false);

    (void) fromString(str);
  }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  const CQChartsSymbol &type() const { return type_; }
  void setType(const CQChartsSymbol &v) { type_ = v; }

  const CQChartsLength &size() const { return size_; }
  void setSize(const CQChartsLength &v) { size_ = v; }

  CQChartsStrokeData &stroke() { return stroke_; }
  const CQChartsStrokeData &stroke() const { return stroke_; }
  void setStroke(const CQChartsStrokeData &v) { stroke_ = v; }

  CQChartsFillData &fill() { return fill_; }
  const CQChartsFillData &fill() const { return fill_; }
  void setFill(const CQChartsFillData &v) { fill_ = v; }

  //---

  bool isValid() const { return true; }

  QString toString() const;
  bool fromString(const QString &s);

  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

 private:
  bool               visible_ { true };  //!< visible
  CQChartsSymbol     type_;              //!< symbol type
  CQChartsLength     size_    { "5px" }; //!< symbol size
  CQChartsStrokeData stroke_;            //!< symbol stroke
  CQChartsFillData   fill_;              //!< symbol fill
};

CQUTIL_DCL_META_TYPE(CQChartsSymbolData)

//------

/*!
 * \brief Arrow Properties
 * \ingroup Charts
 *
 *   line width, front head, tail head, angle, back angle, length and line ends
 */
class CQChartsArrowData {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  enum class HeadType {
    NONE,
    TRIANGLE,
    STEALTH,
    DIAMOND,
    LINE
  };

 public:
  CQChartsArrowData() {
    theadData_.visible = true;
  }

  explicit CQChartsArrowData(const QString &str) {
    theadData_.visible = true;

    (void) fromString(str);
  }

//bool isRelative() const { return relative_; }
//void setRelative(bool b) { relative_ = b; }

  const CQChartsLength &lineWidth() const { return lineWidth_; }
  void setLineWidth(const CQChartsLength &v) { lineWidth_ = v; }

  //---

  // front head data
  bool isFHead() const { return fheadData_.visible; }
  void setFHead(bool b) { fheadData_.visible = b; }

  HeadType fheadType() const { return fheadData_.type; }
  void setFHeadType(HeadType type);

  double frontAngle() const { return fheadData_.angle; }
  void setFrontAngle(double a) { fheadData_.angle = a; }

  double frontBackAngle() const { return fheadData_.backAngle; }
  void setFrontBackAngle(double a) { fheadData_.backAngle = a; }

  const CQChartsLength &frontLength() const { return fheadData_.length; }
  void setFrontLength(const CQChartsLength &l) { fheadData_.length = l; }

  bool isFrontLineEnds() const { return fheadData_.lineEnds; }
  void setFrontLineEnds(bool b) { fheadData_.lineEnds = b; }

  //---

  // tail head data
  bool isTHead() const { return theadData_.visible; }
  void setTHead(bool b) { theadData_.visible = b; }

  HeadType theadType() const { return theadData_.type; }
  void setTHeadType(HeadType type);

  double tailAngle() const { return theadData_.angle; }
  void setTailAngle(double a) { theadData_.angle = a; }

  double tailBackAngle() const { return theadData_.backAngle; }
  void setTailBackAngle(double a) { theadData_.backAngle = a; }

  const CQChartsLength &tailLength() const { return theadData_.length; }
  void setTailLength(const CQChartsLength &l) { theadData_.length = l; }

  bool isTailLineEnds() const { return theadData_.lineEnds; }
  void setTailLineEnds(bool b) { theadData_.lineEnds = b; }

  //---

  double angle() const { return tailAngle(); }
  void setAngle(double a) { setFrontAngle(a); setTailAngle(a); }

  double backAngle() const { return tailBackAngle(); }
  void setBackAngle(double a) { setFrontBackAngle(a); setTailBackAngle(a); }

  const CQChartsLength &length() const { return tailLength(); }
  void setLength(const CQChartsLength &l) { setFrontLength(l); setTailLength(l); }

  bool isLineEnds() const { return isTailLineEnds(); }
  void setLineEnds(bool b) { setFrontLineEnds(b); setTailLineEnds(b); }

  //---

  bool isValid() const { return true; }

  QString toString() const;
  bool fromString(const QString &s);

  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

  //---

  static bool getTypeAngles(const HeadType &type, double &angle, double &backAngle);
  static bool checkTypeAngles(const HeadType &type, double angle, double backAngle);

  static bool nameToData(const QString &name, HeadType &type, bool &lineEnds, bool &visible);

  static bool dataToName(const HeadType &type, bool lineEnds, bool visible,
                         double angle, double backAngle, QString &name);

 private:
  struct HeadData {
    bool           visible   { false };          //!< draw arrow head
    HeadType       type      { HeadType::NONE }; //!< arrow head type
    double         angle     { -1 };             //!< arrow angle (default 45 if <= 0)
    double         backAngle { -1 };             //!< back angle (default 90 if <= 0)
    CQChartsLength length    { "1V" };           //!< arrow length
    bool           lineEnds  { false };          //!< lines at end
  };

//bool           relative_   { false }; //!< to point relative to from
  CQChartsLength lineWidth_  { -1 };    //!< connecting line width
  HeadData       fheadData_;            //!< front head data
  HeadData       theadData_;            //!< tail head data
};

CQUTIL_DCL_META_TYPE(CQChartsArrowData)

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
  bool                     visible      { true };
  bool                     horizontal   { false };
  bool                     autoHide     { false };
  bool                     clipped      { false };
  bool                     above        { false };
  CQChartsKeyLocation      location;
  bool                     insideX      { false };
  bool                     insideY      { false };
  QPointF                  absolutePosition;
  double                   hiddenAlpha  { 0.3 };
  int                      maxRows;
  bool                     interactive  { false };
  CQChartsKeyPressBehavior pressBehavior;
  int                      spacing      { 2 };
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

#endif
