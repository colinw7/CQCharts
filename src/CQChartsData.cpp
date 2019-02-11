#include <CQChartsData.h>

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

  nameValues.setNameValue("visible"  , isVisible());
  nameValues.setNameValue("color"    , color().toString());
  nameValues.setNameValue("alpha"    , alpha());
  nameValues.setNameValue("font"     , font());
  nameValues.setNameValue("angle"    , angle());
  nameValues.setNameValue("contrast" , isContrast());
  nameValues.setNameValue("align"    , int(align()));
  nameValues.setNameValue("formatted", isFormatted());
  nameValues.setNameValue("scaled"   , isScaled());
  nameValues.setNameValue("html"     , isHtml());

  return nameValues.toString();
}

bool
CQChartsTextData::
fromString(const QString &str)
{
  CQChartsNameValues nameValues(str);

  nameValues.nameValueBool ("visible"  , visible_);
  nameValues.nameValueColor("color"    , color_);
  nameValues.nameValueReal ("alpha"    , alpha_);
//nameValues.nameValueFont ("font"     , font_);
  nameValues.nameValueReal ("angle"    , angle_);
  nameValues.nameValueBool ("contrast" , contrast_);
//nameValues.nameValueAlign("align"    , align_);
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

  nameValues.setNameValue("visible", isVisible());
  nameValues.setNameValue("color"  , color().toString());
  nameValues.setNameValue("alpha"  , alpha());
  nameValues.setNameValue("width"  , width().toString());
  nameValues.setNameValue("dash"   , dash().toString());

  return nameValues.toString();
}

bool
CQChartsLineData::
fromString(const QString &str)
{
  CQChartsNameValues nameValues(str);

  nameValues.nameValueBool    ("visible", visible_);
  nameValues.nameValueColor   ("color"  , color_);
  nameValues.nameValueReal    ("alpha"  , alpha_);
//nameValues.nameValueLength  ("width"  , width_);
//nameValues.nameValueLineDash("dash"   , dash_);

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
  QString str1 = background_.toString();
  QString str2 = border_.toString();

  if      (str1 != "" && str2 != "")
    return str1 + "," + str2;
  else if (str1 != "")
    return str1;
  else
    return str2;
}

bool
CQChartsShapeData::
fromString(const QString &str)
{
  background_.fromString(str);
  border_    .fromString(str);

  return true;
}

//---

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

  nameValues.setNameValue("filled"     , isVisible());
  nameValues.setNameValue("fillColor"  , color().toString());
  nameValues.setNameValue("fillAlpha"  , alpha());
  nameValues.setNameValue("fillPattern", pattern().toString());

  return nameValues.toString();
}

bool
CQChartsFillData::
fromString(const QString &str)
{
  CQChartsNameValues nameValues(str);

  nameValues.nameValueBool       ("filled"     , visible_);
  nameValues.nameValueColor      ("fillColor"  , color_);
  nameValues.nameValueReal       ("fillAlpha"  , alpha_);
//nameValues.nameValueFillPattern("fillPattern", pattern_);

  return true;
}

//---

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

  nameValues.setNameValue("stroked"         , isVisible());
  nameValues.setNameValue("strokeColor"     , color().toString());
  nameValues.setNameValue("strokeAlpha"     , alpha());
  nameValues.setNameValue("strokeWidth"     , width().toString());
  nameValues.setNameValue("strokeDash"      , dash().toString());
  nameValues.setNameValue("strokeCornerSize", cornerSize().toString());

  return nameValues.toString();
}

bool
CQChartsStrokeData::
fromString(const QString &str)
{
  CQChartsNameValues nameValues(str);

  nameValues.nameValueBool    ("stroked"         , visible_);
  nameValues.nameValueColor   ("strokeColor"     , color_);
  nameValues.nameValueReal    ("strokeAlpha"     , alpha_);
//nameValues.nameValueLength  ("strokeWidth"     , width_);
//nameValues.nameValueLineDash("strokeDash"      , dash_);
//nameValues.nameValueLength  ("strokeCornerSize", cornerSize_);

  return true;
}
