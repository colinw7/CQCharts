#include <CQChartsData.h>
#include <CQChartsNameValues.h>
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

  nameValues.setNameValueType<Color>("color", color());

  if (alpha() != Alpha())
    nameValues.setNameValue("alpha", alpha().value());

  nameValues.setNameValueType<Font>("font", font());

  if (angle() != Angle())
    nameValues.setNameValue("angle", angle().value());

  if (isContrast())
    nameValues.setNameValue("contrast", isContrast());

  if (contrastAlpha() != Alpha())
    nameValues.setNameValue("contrastAlpha", contrastAlpha().value());

  nameValues.setNameValue("align", CQUtil::alignToString(align()));

  if (isFormatted())
    nameValues.setNameValue("formatted", isFormatted());

  if (isScaled())
    nameValues.setNameValue("scaled", isScaled());

  if (isHtml())
    nameValues.setNameValue("html", isHtml());

  if (clipLength() != Length())
    nameValues.setNameValueType<Length>("clipLength", clipLength());

//if (clipElide() != Elide())
//  nameValues.setNameValueType<Length>("clipElide", clipElide());
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
  auto nameValueAlpha = [&](const QString &name, Alpha &alpha, bool &ok) {
    double r = 1.0; (void) nameValues.nameValueReal(name, r, ok); alpha = Alpha(r);
    return true;
  };

  auto nameValueAngle = [&](const QString &name, Angle &angle, bool &ok) {
    double r = 0.0; (void) nameValues.nameValueReal(name, r, ok); angle = Angle(r);
    return true;
  };

  bool ok = true, ok1 = true;

  (void) nameValues.nameValueBool ("visible"      , visible_      , ok1); if (! ok1) ok = false;
  (void) nameValues.nameValueColor("color"        , color_        , ok1); if (! ok1) ok = false;
  (void) nameValueAlpha           ("alpha"        , alpha_        , ok1); if (! ok1) ok = false;
  (void) nameValues.nameValueFont ("font"         , font_         , ok1); if (! ok1) ok = false;
  (void) nameValueAngle           ("angle"        , angle_        , ok1); if (! ok1) ok = false;
  (void) nameValues.nameValueBool ("contrast"     , contrast_     , ok1); if (! ok1) ok = false;
  (void) nameValueAlpha           ("contrastAlpha", contrastAlpha_, ok1); if (! ok1) ok = false;
  (void) nameValues.nameValueAlign("align"        , align_        , ok1); if (! ok1) ok = false;
  (void) nameValues.nameValueBool ("formatted"    , formatted_    , ok1); if (! ok1) ok = false;
  (void) nameValues.nameValueBool ("scaled"       , scaled_       , ok1); if (! ok1) ok = false;
  (void) nameValues.nameValueBool ("html"         , html_         , ok1); if (! ok1) ok = false;

  (void) nameValues.nameValueType<Length>("clipLength", clipLength_, ok1); if (! ok1) ok = false;

//(void) nameValues.nameValueElide>("clipElide", clipElide_, ok1); if (! ok1) ok = false;

  return ok;
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

  nameValues.setNameValueType<Color>("color", color());

  if (alpha() != Alpha())
    nameValues.setNameValue("alpha", alpha().value());

  nameValues.setNameValueType<Length>("width", width());

  if (dash() != LineDash())
    nameValues.setNameValueType<LineDash>("dash", dash());

  if (lineCap() != LineCap())
    nameValues.setNameValueType<LineCap>("cap", lineCap());

  if (lineJoin() != LineJoin())
    nameValues.setNameValueType<LineJoin>("join", lineJoin());
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
  auto nameValueAlpha = [&](const QString &name, Alpha &alpha, bool &ok) {
    double r = 1.0; (void) nameValues.nameValueReal(name, r, ok); alpha = Alpha(r);
    return true;
  };

  bool ok = true, ok1 = true;

  (void) nameValues.nameValueBool ("visible", visible_, ok1); if (! ok1) ok = false;
  (void) nameValues.nameValueColor("color"  , color_  , ok1); if (! ok1) ok = false;
  (void) nameValueAlpha           ("alpha"  , alpha_  , ok1); if (! ok1) ok = false;

  (void) nameValues.nameValueType<Length>  ("width", width_   , ok1); if (! ok1) ok = false;
  (void) nameValues.nameValueType<LineDash>("dash" , dash_    , ok1); if (! ok1) ok = false;
  (void) nameValues.nameValueType<LineCap> ("cap"  , lineCap_ , ok1); if (! ok1) ok = false;
  (void) nameValues.nameValueType<LineJoin>("join" , lineJoin_, ok1); if (! ok1) ok = false;

  return ok;
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
  bool ok = true;

  if (! fill_  .getNameValues(nameValues)) ok = false;
  if (! stroke_.getNameValues(nameValues)) ok = false;

  return ok;
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

  nameValues.setNameValue("margin" , margin ().toString());
  nameValues.setNameValue("padding", padding().toString());

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
  bool ok = true, ok1 = true;

  QString str;

  nameValues.nameValueBool("visible", visible_, ok1); if (! ok1) ok = false;

  if (nameValues.nameValueString("margin", str, ok1))
    margin_ = CQChartsMargin(str);

  if (nameValues.nameValueString("padding", str, ok1))
    padding_ = CQChartsMargin(str);

  if (! shape_.getNameValues(nameValues)) ok = false;

  if (nameValues.nameValueString("border_sides", str, ok1))
    borderSides_ = CQChartsSides(str);

  return ok;
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
  bool ok = true;

  if (! text_.getNameValues(nameValues)) ok = false;
  if (! box_ .getNameValues(nameValues)) ok = false;

  return ok;
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

  nameValues.setNameValueType<Color>("fillColor", color());

  if (alpha() != Alpha())
    nameValues.setNameValue("fillAlpha", alpha().value());

  if (pattern() != CQChartsFillPattern::makeSolid())
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
  auto nameValueAlpha = [&](const QString &name, Alpha &alpha, bool &ok) {
    double r = 1.0; (void) nameValues.nameValueReal(name, r, ok); alpha = Alpha(r);
    return true;
  };

  bool ok = true, ok1 = true;

  (void) nameValues.nameValueBool ("filled"   , visible_, ok1); if (! ok1) ok = false;
  (void) nameValues.nameValueColor("fillColor", color_  , ok1); if (! ok1) ok = false;
  (void) nameValueAlpha           ("fillAlpha", alpha_  , ok1); if (! ok1) ok = false;

  (void) nameValues.nameValueType<CQChartsFillPattern>("fillPattern", pattern_, ok1);
  if (! ok1) ok = false;

  return ok;
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

  nameValues.setNameValueType<Color>("strokeColor", color());

  if (alpha() != Alpha())
    nameValues.setNameValue("strokeAlpha", alpha().value());

  if (width() != Length("0px"))
    nameValues.setNameValueType<Length>("strokeWidth", width());

  if (dash() != LineDash())
    nameValues.setNameValueType<LineDash>("strokeDash" , dash());

  if (lineCap() != LineCap())
    nameValues.setNameValueType<LineCap>("cap", lineCap());

  if (lineJoin() != LineJoin())
    nameValues.setNameValueType<LineJoin>("join", lineJoin());

  if (cornerSize() != Length("0px"))
    nameValues.setNameValueType<Length>("strokeCornerSize", cornerSize());
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
  auto nameValueAlpha = [&](const QString &name, Alpha &alpha, bool &ok) {
    double r = 1.0; (void) nameValues.nameValueReal(name, r, ok); alpha = Alpha(r);
    return true;
  };

  bool ok = true, ok1 = true;

  (void) nameValues.nameValueBool ("stroked"    , visible_, ok1); if (! ok1) ok = false;
  (void) nameValues.nameValueColor("strokeColor", color_  , ok1); if (! ok1) ok = false;
  (void) nameValueAlpha           ("strokeAlpha", alpha_  , ok1); if (! ok1) ok = false;

  (void) nameValues.nameValueType<Length>("strokeWidth", width_, ok1);
  if (! ok1) ok = false;

  (void) nameValues.nameValueType<LineDash>("strokeDash", dash_, ok1);
  if (! ok1) ok = false;

  (void) nameValues.nameValueType<LineCap>("strokeCap", lineCap_, ok1);
  if (! ok1) ok = false;

  (void) nameValues.nameValueType<LineJoin>("strokeJoin", lineJoin_, ok1);
  if (! ok1) ok = false;

  (void) nameValues.nameValueType<Length>("strokeCornerSize", cornerSize_, ok1);
  if (! ok1) ok = false;

  return ok;
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

  nameValues.setNameValueType<CQChartsSymbol>("symbol", symbol());
  nameValues.setNameValueType<Length>("size", size());

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
  bool ok = true, ok1 = true;

  (void) nameValues.nameValueBool("visible", visible_, ok1); if (! ok1) ok = false;

  (void) nameValues.nameValueType<CQChartsSymbol>("symbol", symbol_, ok1); if (! ok1) ok = false;
  (void) nameValues.nameValueType<Length>("size", size_, ok1); if (! ok1) ok = false;

  if (! stroke_.getNameValues(nameValues)) ok = false;
  if (! fill_  .getNameValues(nameValues)) ok = false;

  return ok;
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

  if      (fheadData_.type == HeadType::ARROW    ||
           fheadData_.type == HeadType::TRIANGLE ||
           fheadData_.type == HeadType::STEALTH  ||
           fheadData_.type == HeadType::DIAMOND) {
    Angle angle, backAngle;

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

  if      (theadData_.type == HeadType::ARROW    ||
           theadData_.type == HeadType::TRIANGLE ||
           theadData_.type == HeadType::STEALTH  ||
           theadData_.type == HeadType::DIAMOND) {
    Angle angle, backAngle;

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

void
CQChartsArrowData::
updateFrontBackAngle()
{
  getTypeBackAngle(fheadData_.type, fheadData_.angle, fheadData_.backAngle);
}

void
CQChartsArrowData::
updateTailBackAngle()
{
  getTypeBackAngle(theadData_.type, theadData_.angle, theadData_.backAngle);
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
    nameValues.setNameValueType<Length>("line_width", lineWidth());

  //---

  // TODO: fhead type
  nameValues.setNameValue("front_visible", isFHead());

  if (frontAngle().value() > 0)
    nameValues.setNameValue("front_angle", frontAngle().value());

  if (frontBackAngle().value() > 0)
    nameValues.setNameValue("front_back_angle", frontBackAngle().value());

  if (frontLength().isValid())
    nameValues.setNameValueType<Length>("front_length", frontLength());

  if (isFrontLineEnds())
    nameValues.setNameValue("front_line_ends", isFrontLineEnds());

  //---

  // TODO: thead type
  nameValues.setNameValue("tail_visible", isTHead());

  if (tailAngle().value() > 0)
    nameValues.setNameValue("tail_angle", tailAngle().value());

  if (tailBackAngle().value() > 0)
    nameValues.setNameValue("tail_back_angle", tailBackAngle().value());

  if (tailLength().isValid())
    nameValues.setNameValueType<Length>("tail_length", tailLength());

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
  auto nameValueAngle = [&](const QString &name, Angle &angle, bool &ok) {
    double r = 0.0; (void) nameValues.nameValueReal(name, r, ok); angle = Angle(r);
    return true;
  };

  bool ok = true, ok1 = true;

//nameValues.nameValueBool("relative", relative_, ok1); if (! ok1) ok = false;

  (void) nameValues.nameValueType<Length>("line_width", lineWidth_, ok1);
  if (! ok1) ok = false;

  (void) nameValues.nameValueBool        ("front_visible"   , fheadData_.visible  , ok1);
  if (! ok1) ok = false;
  (void) nameValueAngle                  ("front_angle"     , fheadData_.angle    , ok1);
  if (! ok1) ok = false;
  (void) nameValueAngle                  ("front_back_angle", fheadData_.backAngle, ok1);
  if (! ok1) ok = false;
  (void) nameValues.nameValueType<Length>("front_length"    , fheadData_.length   , ok1);
  if (! ok1) ok = false;
  (void) nameValues.nameValueBool        ("front_line_ends" , fheadData_.lineEnds , ok1);
  if (! ok1) ok = false;

  (void) nameValues.nameValueBool        ("tail_visible"    , theadData_.visible  , ok1);
  if (! ok1) ok = false;
  (void) nameValueAngle                  ("tail_angle"      , theadData_.angle    , ok1);
  if (! ok1) ok = false;
  (void) nameValueAngle                  ("tail_back_angle" , theadData_.backAngle, ok1);
  if (! ok1) ok = false;
  (void) nameValues.nameValueType<Length>("tail_length"     , theadData_.length   , ok1);
  if (! ok1) ok = false;
  (void) nameValues.nameValueBool        ("tail_line_ends"  , theadData_.lineEnds , ok1);
  if (! ok1) ok = false;

  return ok;
}

bool
CQChartsArrowData::
getTypeAngles(const HeadType &type, Angle &angle, Angle &backAngle)
{
  if      (type == HeadType::ARROW   ) angle = Angle(45.0);
  else if (type == HeadType::TRIANGLE) angle = Angle(30.0);
  else if (type == HeadType::STEALTH ) angle = Angle(30.0);
  else if (type == HeadType::DIAMOND ) angle = Angle(30.0);
  else return false;

  getTypeBackAngle(type, angle, backAngle);

  return true;
}

bool
CQChartsArrowData::
getTypeBackAngle(const HeadType &type, const Angle &angle, Angle &backAngle)
{
  if      (type == HeadType::ARROW   ) backAngle = Angle(90.0);
  else if (type == HeadType::TRIANGLE) backAngle = Angle(90.0);
  else if (type == HeadType::STEALTH ) backAngle = Angle(90.0 - 2.0*angle.value()/3.0);
  else if (type == HeadType::DIAMOND ) backAngle = Angle(180.0 - angle.value());
  else return false;

  return true;
}

bool
CQChartsArrowData::
checkTypeAngles(const HeadType &type, const Angle &angle, const Angle &backAngle)
{
  Angle angle1, backAngle1;

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

  auto lstr = name.toLower();

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
           const Angle &angle, const Angle &backAngle, QString &name)
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
