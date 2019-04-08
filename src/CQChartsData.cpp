#include <CQChartsData.h>
#include <CQUtil.h>

// TODO: only save if value not default

CQUTIL_DEF_META_TYPE(CQChartsTextData, toString, fromString)

int CQChartsTextData::metaTypeId;

void
CQChartsTextData::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsTextData);
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
  background_.setNameValues(nameValues);
  border_    .setNameValues(nameValues);
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
  background_.getNameValues(nameValues);
  border_    .getNameValues(nameValues);

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
  if (isRelative())
    nameValues.setNameValue("relative", isRelative());

  if (angle() > 0)
    nameValues.setNameValue("angle", angle());

  if (backAngle() > 0)
    nameValues.setNameValue("back_angle", backAngle());

  nameValues.setNameValue("fhead", isFHead());
  nameValues.setNameValue("thead", isTHead());

  if (isLineEnds())
    nameValues.setNameValue("line_ends", isLineEnds());

  if (length().isValid())
    nameValues.setNameValueType<CQChartsLength>("length", length());

  if (lineWidth().value() > 0)
    nameValues.setNameValueType<CQChartsLength>("line_width", lineWidth());
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
  nameValues.nameValueBool("relative"  , relative_);
  nameValues.nameValueReal("angle"     , angle_);
  nameValues.nameValueReal("back_angle", backAngle_);
  nameValues.nameValueBool("fhead"     , fhead_);
  nameValues.nameValueBool("thead"     , thead_);
  nameValues.nameValueBool("line_ends" , lineEnds_);

  nameValues.nameValueType<CQChartsLength>("length", length_);
  nameValues.nameValueType<CQChartsLength>("line_width", lineWidth_);

  return true;
}
