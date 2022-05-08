#include <CQChartsData.h>
#include <CQChartsNameValues.h>
#include <CQChartsUtil.h>
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

  if (width() != Length::pixel(0))
    nameValues.setNameValueType<Length>("strokeWidth", width());

  if (dash() != LineDash())
    nameValues.setNameValueType<LineDash>("strokeDash" , dash());

  if (lineCap() != LineCap())
    nameValues.setNameValueType<LineCap>("cap", lineCap());

  if (lineJoin() != LineJoin())
    nameValues.setNameValueType<LineJoin>("join", lineJoin());

  if (cornerSize() != Length::pixel(0))
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
