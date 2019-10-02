#include <CQChartsData.h>
#include <CQPropertyView.h>
#include <CQUtil.h>

// TODO: only save if value not default

CQUTIL_DEF_META_TYPE(CQChartsTextData, toString, fromString)

int CQChartsTextData::metaTypeId;

void
CQChartsTextData::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsTextData);

  CQPropertyViewMgrInst->setUserName("CQChartsTextData", "text_data");
}

QString
CQChartsTextData::
toString() const
{
  CQChartsNameValues nameValues;

  setNameValues(nameValues);

  return nameValues.toString();
}

void
CQChartsTextData::
setNameValues(CQChartsNameValues &nameValues) const
{
  if (! isVisible())
    nameValues.setNameValue("visible", isVisible());

  nameValues.setNameValueType<CQChartsColor>("color", color());

  if (alpha() != 1.0)
    nameValues.setNameValue("alpha", alpha());

  nameValues.setNameValueType<CQChartsFont>("font", font());

  if (angle() != 0.0)
    nameValues.setNameValue("angle", angle());

  if (isContrast())
    nameValues.setNameValue("contrast", isContrast());

  nameValues.setNameValue("align", CQUtil::alignToString(align()));

  if (isFormatted())
    nameValues.setNameValue("formatted", isFormatted());

  if (isScaled())
    nameValues.setNameValue("scaled", isScaled());

  if (isHtml())
    nameValues.setNameValue("html", isHtml());
}

bool
CQChartsTextData::
fromString(const QString &str)
{
  CQChartsNameValues nameValues(str);

  return getNameValues(nameValues);
}

bool
CQChartsTextData::
getNameValues(const CQChartsNameValues &nameValues)
{
  nameValues.nameValueBool ("visible"  , visible_);
  nameValues.nameValueColor("color"    , color_);
  nameValues.nameValueReal ("alpha"    , alpha_);
  nameValues.nameValueFont ("font"     , font_);
  nameValues.nameValueReal ("angle"    , angle_);
  nameValues.nameValueBool ("contrast" , contrast_);
  nameValues.nameValueAlign("align"    , align_);
  nameValues.nameValueBool ("formatted", formatted_);
  nameValues.nameValueBool ("scaled"   , scaled_);
  nameValues.nameValueBool ("html"     , html_);

  return true;
}

//------

CQUTIL_DEF_META_TYPE(CQChartsLineData, toString, fromString)

int CQChartsLineData::metaTypeId;

void
CQChartsLineData::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsLineData);

  CQPropertyViewMgrInst->setUserName("CQChartsLineData", "line_data");
}

QString
CQChartsLineData::
toString() const
{
  CQChartsNameValues nameValues;

  setNameValues(nameValues);

  return nameValues.toString();
}

void
CQChartsLineData::
setNameValues(CQChartsNameValues &nameValues) const
{
  if (! isVisible())
    nameValues.setNameValue("visible", isVisible());

  nameValues.setNameValueType<CQChartsColor>("color", color());

  if (alpha() != 1.0)
    nameValues.setNameValue("alpha", alpha());

  nameValues.setNameValueType<CQChartsLength>("width", width());

  if (dash() != CQChartsLineDash())
    nameValues.setNameValueType<CQChartsLineDash>("dash", dash());
}

bool
CQChartsLineData::
fromString(const QString &str)
{
  CQChartsNameValues nameValues(str);

  return getNameValues(nameValues);
}

bool
CQChartsLineData::
getNameValues(const CQChartsNameValues &nameValues)
{
  nameValues.nameValueBool ("visible", visible_);
  nameValues.nameValueColor("color"  , color_);
  nameValues.nameValueReal ("alpha"  , alpha_);

  nameValues.nameValueType<CQChartsLength>  ("width", width_);
  nameValues.nameValueType<CQChartsLineDash>("dash" , dash_);

  return true;
}

//------

CQUTIL_DEF_META_TYPE(CQChartsShapeData, toString, fromString)

int CQChartsShapeData::metaTypeId;

void
CQChartsShapeData::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsShapeData);

  CQPropertyViewMgrInst->setUserName("CQChartsShapeData", "shape_data");
}

QString
CQChartsShapeData::
toString() const
{
  CQChartsNameValues nameValues;

  setNameValues(nameValues);

  return nameValues.toString();
}

void
CQChartsShapeData::
setNameValues(CQChartsNameValues &nameValues) const
{
  fill_  .setNameValues(nameValues);
  stroke_.setNameValues(nameValues);
}

bool
CQChartsShapeData::
fromString(const QString &str)
{
  CQChartsNameValues nameValues(str);

  return getNameValues(nameValues);
}

bool
CQChartsShapeData::
getNameValues(const CQChartsNameValues &nameValues)
{
  fill_  .getNameValues(nameValues);
  stroke_.getNameValues(nameValues);

  return true;
}

//------

CQUTIL_DEF_META_TYPE(CQChartsBoxData, toString, fromString)

int CQChartsBoxData::metaTypeId;

void
CQChartsBoxData::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsBoxData);

  CQPropertyViewMgrInst->setUserName("CQChartsBoxData", "box_data");
}

QString
CQChartsBoxData::
toString() const
{
  CQChartsNameValues nameValues;

  setNameValues(nameValues);

  return nameValues.toString();
}

void
CQChartsBoxData::
setNameValues(CQChartsNameValues &nameValues) const
{
  if (! isVisible())
    nameValues.setNameValue("visible", isVisible());

  nameValues.setNameValue("margin" , margin ());
  nameValues.setNameValue("padding", padding());

  shape_.setNameValues(nameValues);

  nameValues.setNameValue("border_sides", borderSides().toString());
}

bool
CQChartsBoxData::
fromString(const QString &str)
{
  CQChartsNameValues nameValues(str);

  return getNameValues(nameValues);
}

bool
CQChartsBoxData::
getNameValues(const CQChartsNameValues &nameValues)
{
  nameValues.nameValueBool("visible", visible_);
  nameValues.nameValueReal("margin" , margin_ );
  nameValues.nameValueReal("padding", padding_);

  shape_.getNameValues(nameValues);

  QString str;

  if (nameValues.nameValueString("border_sides", str))
    borderSides_ = CQChartsSides(str);

  return true;
}

//------

CQUTIL_DEF_META_TYPE(CQChartsTextBoxData, toString, fromString)

int CQChartsTextBoxData::metaTypeId;

void
CQChartsTextBoxData::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsTextBoxData);

  CQPropertyViewMgrInst->setUserName("CQChartsTextBoxData", "text_box_data");
}

QString
CQChartsTextBoxData::
toString() const
{
  CQChartsNameValues nameValues;

  setNameValues(nameValues);

  return nameValues.toString();
}

void
CQChartsTextBoxData::
setNameValues(CQChartsNameValues &nameValues) const
{
  text_.setNameValues(nameValues);
  box_ .setNameValues(nameValues);
}

bool
CQChartsTextBoxData::
fromString(const QString &str)
{
  CQChartsNameValues nameValues(str);

  return getNameValues(nameValues);
}

bool
CQChartsTextBoxData::
getNameValues(const CQChartsNameValues &nameValues)
{
  text_.getNameValues(nameValues);
  box_ .getNameValues(nameValues);

  return true;
}

//------

CQUTIL_DEF_META_TYPE(CQChartsFillData, toString, fromString)

int CQChartsFillData::metaTypeId;

void
CQChartsFillData::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsFillData);

  CQPropertyViewMgrInst->setUserName("CQChartsFillData", "fill_data");
}

QString
CQChartsFillData::
toString() const
{
  CQChartsNameValues nameValues;

  setNameValues(nameValues);

  return nameValues.toString();
}

void
CQChartsFillData::
setNameValues(CQChartsNameValues &nameValues) const
{
  nameValues.setNameValue("filled", isVisible());

  nameValues.setNameValueType<CQChartsColor>("fillColor", color());

  if (alpha() != 1.0)
    nameValues.setNameValue("fillAlpha", alpha());

  if (pattern() != CQChartsFillPattern(CQChartsFillPattern::Type::SOLID))
    nameValues.setNameValueType<CQChartsFillPattern>("fillPattern", pattern());
}

bool
CQChartsFillData::
fromString(const QString &str)
{
  CQChartsNameValues nameValues(str);

  return getNameValues(nameValues);
}

bool
CQChartsFillData::
getNameValues(const CQChartsNameValues &nameValues)
{
  nameValues.nameValueBool ("filled", visible_);
  nameValues.nameValueColor("fillColor", color_);
  nameValues.nameValueReal ("fillAlpha", alpha_);

  nameValues.nameValueType<CQChartsFillPattern>("fillPattern", pattern_);

  return true;
}

//------

CQUTIL_DEF_META_TYPE(CQChartsStrokeData, toString, fromString)

int CQChartsStrokeData::metaTypeId;

void
CQChartsStrokeData::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsStrokeData);

  CQPropertyViewMgrInst->setUserName("CQChartsStrokeData", "stroke_data");
}

QString
CQChartsStrokeData::
toString() const
{
  CQChartsNameValues nameValues;

  setNameValues(nameValues);

  return nameValues.toString();
}

void
CQChartsStrokeData::
setNameValues(CQChartsNameValues &nameValues) const
{
  nameValues.setNameValue("stroked", isVisible());

  nameValues.setNameValueType<CQChartsColor>("strokeColor", color());

  if (alpha() != 1.0)
    nameValues.setNameValue("strokeAlpha", alpha());

  if (width() != CQChartsLength("0px"))
    nameValues.setNameValueType<CQChartsLength>("strokeWidth", width());

  if (dash() != CQChartsLineDash())
    nameValues.setNameValueType<CQChartsLineDash>("strokeDash" , dash());

  if (cornerSize() != CQChartsLength("0px"))
    nameValues.setNameValueType<CQChartsLength>("strokeCornerSize", cornerSize());
}

bool
CQChartsStrokeData::
fromString(const QString &str)
{
  CQChartsNameValues nameValues(str);

  return getNameValues(nameValues);
}

bool
CQChartsStrokeData::
getNameValues(const CQChartsNameValues &nameValues)
{
  nameValues.nameValueBool ("stroked"    , visible_);
  nameValues.nameValueColor("strokeColor", color_);
  nameValues.nameValueReal ("strokeAlpha", alpha_);

  nameValues.nameValueType<CQChartsLength>("strokeWidth", width_);

  nameValues.nameValueType<CQChartsLineDash>("strokeDash", dash_);

  nameValues.nameValueType<CQChartsLength>("strokeCornerSize", cornerSize_);

  return true;
}

//------

CQUTIL_DEF_META_TYPE(CQChartsSymbolData, toString, fromString)

int CQChartsSymbolData::metaTypeId;

void
CQChartsSymbolData::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsSymbolData);

  CQPropertyViewMgrInst->setUserName("CQChartsSymbolData", "symbol_data");
}

QString
CQChartsSymbolData::
toString() const
{
  CQChartsNameValues nameValues;

  setNameValues(nameValues);

  return nameValues.toString();
}

void
CQChartsSymbolData::
setNameValues(CQChartsNameValues &nameValues) const
{
  nameValues.setNameValue("visible", isVisible());

  nameValues.setNameValueType<CQChartsSymbol>("type", type());
  nameValues.setNameValueType<CQChartsLength>("size", size());

  stroke_.setNameValues(nameValues);
  fill_  .setNameValues(nameValues);
}

bool
CQChartsSymbolData::
fromString(const QString &str)
{
  CQChartsNameValues nameValues(str);

  return getNameValues(nameValues);
}

bool
CQChartsSymbolData::
getNameValues(const CQChartsNameValues &nameValues)
{
  nameValues.nameValueBool("visible", visible_);

  nameValues.nameValueType<CQChartsSymbol>("type", type_);
  nameValues.nameValueType<CQChartsLength>("size", size_);

  stroke_.getNameValues(nameValues);
  fill_  .getNameValues(nameValues);

  return true;
}

//------

CQUTIL_DEF_META_TYPE(CQChartsArrowData, toString, fromString)

int CQChartsArrowData::metaTypeId;

void
CQChartsArrowData::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsArrowData);

  CQPropertyViewMgrInst->setUserName("CQChartsArrowData", "arrow_data");
}

void
CQChartsArrowData::
setFHeadType(HeadType type)
{
  fheadData_.type = type;

  setFrontLineEnds(false);
  setFHead        (true);

  if      (fheadData_.type == HeadType::TRIANGLE ||
           fheadData_.type == HeadType::STEALTH  ||
           fheadData_.type == HeadType::DIAMOND) {
    double angle, backAngle;

    getTypeAngles(fheadData_.type, angle, backAngle);

    setFrontAngle(angle); setFrontBackAngle(backAngle);
  }
  else if (fheadData_.type == HeadType::LINE) {
    setFrontLineEnds(true);
  }
  else if (fheadData_.type == HeadType::NONE) {
    setFHead(false);
  }
}

void
CQChartsArrowData::
setTHeadType(HeadType type)
{
  theadData_.type = type;

  setTailLineEnds(false);
  setTHead       (true);

  if      (theadData_.type == HeadType::TRIANGLE ||
           theadData_.type == HeadType::STEALTH  ||
           theadData_.type == HeadType::DIAMOND) {
    double angle, backAngle;

    getTypeAngles(theadData_.type, angle, backAngle);

    setTailAngle(angle); setTailBackAngle(backAngle);
  }
  else if (theadData_.type == HeadType::LINE) {
    setTailLineEnds(true);
  }
  else if (theadData_.type == HeadType::NONE) {
    setTHead(false);
  }
}

QString
CQChartsArrowData::
toString() const
{
  CQChartsNameValues nameValues;

  setNameValues(nameValues);

  return nameValues.toString();
}

void
CQChartsArrowData::
setNameValues(CQChartsNameValues &nameValues) const
{
#if 0
  if (isRelative())
    nameValues.setNameValue("relative", isRelative());
#endif

  if (lineWidth().value() > 0)
    nameValues.setNameValueType<CQChartsLength>("line_width", lineWidth());

  //---

  nameValues.setNameValue("front_visible", isFHead());

  if (frontAngle    () > 0) nameValues.setNameValue("front_angle", frontAngle());
  if (frontBackAngle() > 0) nameValues.setNameValue("front_back_angle", frontBackAngle());

  if (frontLength().isValid())
    nameValues.setNameValueType<CQChartsLength>("front_length", frontLength());

  if (isFrontLineEnds())
    nameValues.setNameValue("front_line_ends", isFrontLineEnds());

  //---

  nameValues.setNameValue("tail_visible", isTHead());

  if (tailAngle    () > 0) nameValues.setNameValue("tail_angle", tailAngle());
  if (tailBackAngle() > 0) nameValues.setNameValue("tail_back_angle", tailBackAngle());

  if (tailLength().isValid())
    nameValues.setNameValueType<CQChartsLength>("tail_length", tailLength());

  if (isTailLineEnds())
    nameValues.setNameValue("tail_line_ends", isTailLineEnds());
}

bool
CQChartsArrowData::
fromString(const QString &str)
{
  CQChartsNameValues nameValues(str);

  return getNameValues(nameValues);
}

bool
CQChartsArrowData::
getNameValues(const CQChartsNameValues &nameValues)
{
//nameValues.nameValueBool("relative", relative_);

  nameValues.nameValueType<CQChartsLength>("line_width", lineWidth_);

  nameValues.nameValueBool                ("front_visible"   , fheadData_.visible);
  nameValues.nameValueReal                ("front_angle"     , fheadData_.angle);
  nameValues.nameValueReal                ("front_back_angle", fheadData_.backAngle);
  nameValues.nameValueType<CQChartsLength>("front_length"    , fheadData_.length);
  nameValues.nameValueBool                ("front_line_ends" , fheadData_.lineEnds);

  nameValues.nameValueBool                ("tail_visible"    , theadData_.visible);
  nameValues.nameValueReal                ("tail_angle"      , theadData_.angle);
  nameValues.nameValueReal                ("tail_back_angle" , theadData_.backAngle);
  nameValues.nameValueType<CQChartsLength>("tail_length"     , theadData_.length);
  nameValues.nameValueBool                ("tail_line_ends"  , theadData_.lineEnds);

  return true;
}

bool
CQChartsArrowData::
getTypeAngles(const HeadType &type, double &angle, double &backAngle)
{
  if      (type == HeadType::TRIANGLE) {
    angle = 30.0; backAngle = 90.0;
  }
  else if (type == HeadType::STEALTH) {
    angle = 30.0; backAngle = 45.0;
  }
  else if (type == HeadType::DIAMOND) {
    angle = 30.0; backAngle = 130.0;
  }
  else {
    return false;
  }

  return true;
}

bool
CQChartsArrowData::
checkTypeAngles(const HeadType &type, double angle, double backAngle)
{
  double angle1, backAngle1;

  if (! getTypeAngles(type, angle1, backAngle1))
    return false;

  return (angle == angle1 && backAngle == backAngle1);
}

bool
CQChartsArrowData::
nameToData(const QString &name, HeadType &type, bool &lineEnds, bool &visible)
{
  lineEnds = false;
  visible  = true;
  type     = CQChartsArrowData::HeadType::STEALTH;

  QString lstr = name.toLower();

  if (lstr == "yes" || lstr == "true"  || lstr == "1") { visible = true ; return true; }
  if (lstr == "no"  || lstr == "false" || lstr == "0") { visible = false; return true; }

  if      (lstr == "triangle") type = CQChartsArrowData::HeadType::TRIANGLE;
  else if (lstr == "stealth" ) type = CQChartsArrowData::HeadType::STEALTH;
  else if (lstr == "diamond" ) type = CQChartsArrowData::HeadType::DIAMOND;
  else if (lstr == "line"    ) { lineEnds = true ; type = CQChartsArrowData::HeadType::LINE; }
  else if (lstr == "none"    ) { visible  = false; type = CQChartsArrowData::HeadType::NONE; }
  else                         return false;

  return true;
}

bool
CQChartsArrowData::
dataToName(const HeadType &type, bool lineEnds, bool visible,
           double angle, double backAngle, QString &name)
{
  if (! visible) { name = "none"; return true; }
  if (lineEnds ) { name = "line"; return true; }

  if (type == CQChartsArrowData::HeadType::TRIANGLE) {
    name = "triangle"; return checkTypeAngles(type, angle, backAngle); }
  if (type == CQChartsArrowData::HeadType::STEALTH) {
    name = "stealth" ; return checkTypeAngles(type, angle, backAngle); }
  if (type == CQChartsArrowData::HeadType::DIAMOND) {
    name = "diamond" ; return checkTypeAngles(type, angle, backAngle); }

  if (type == CQChartsArrowData::HeadType::LINE) { name = "line"; return true; }

  return false;
}
