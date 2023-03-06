#include <CQChartsArrowData.h>
#include <CQChartsUtil.h>
#include <CQPropertyView.h>

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
setMidHeadType(HeadType type)
{
  midHeadData_.type = type;

  setMidLineEnds(false);
  setMidHead    (true);

  if      (midHeadData_.type == HeadType::ARROW    ||
           midHeadData_.type == HeadType::TRIANGLE ||
           midHeadData_.type == HeadType::STEALTH  ||
           midHeadData_.type == HeadType::DIAMOND) {
    Angle angle, backAngle;

    getTypeAngles(midHeadData_.type, angle, backAngle);

    setMidAngle(angle); setMidBackAngle(backAngle);
  }
  else if (midHeadData_.type == HeadType::LINE) {
    setMidLineEnds(true);
  }
  else if (midHeadData_.type == HeadType::NONE) {
    setMidHead(false);
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

void
CQChartsArrowData::
updateMidBackAngle()
{
  getTypeBackAngle(midHeadData_.type, midHeadData_.angle, midHeadData_.backAngle);
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

  //---

  // TODO: midHead type
  nameValues.setNameValue("mid_visible", isMidHead());

  if (midAngle().value() > 0)
    nameValues.setNameValue("mid_angle", midAngle().value());

  if (midBackAngle().value() > 0)
    nameValues.setNameValue("mid_back_angle", midBackAngle().value());

  if (midLength().isValid())
    nameValues.setNameValueType<Length>("mid_length", midLength());

  if (isMidLineEnds())
    nameValues.setNameValue("mid_line_ends", isMidLineEnds());
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

  (void) nameValues.nameValueBool        ("tail_visible"   , theadData_.visible  , ok1);
  if (! ok1) ok = false;
  (void) nameValueAngle                  ("tail_angle"     , theadData_.angle    , ok1);
  if (! ok1) ok = false;
  (void) nameValueAngle                  ("tail_back_angle", theadData_.backAngle, ok1);
  if (! ok1) ok = false;
  (void) nameValues.nameValueType<Length>("tail_length"    , theadData_.length   , ok1);
  if (! ok1) ok = false;
  (void) nameValues.nameValueBool        ("tail_line_ends" , theadData_.lineEnds , ok1);
  if (! ok1) ok = false;

  (void) nameValues.nameValueBool        ("mid_visible"   , midHeadData_.visible  , ok1);
  if (! ok1) ok = false;
  (void) nameValueAngle                  ("mid_angle"     , midHeadData_.angle    , ok1);
  if (! ok1) ok = false;
  (void) nameValueAngle                  ("mid_back_angle", midHeadData_.backAngle, ok1);
  if (! ok1) ok = false;
  (void) nameValues.nameValueType<Length>("mid_length"    , midHeadData_.length   , ok1);
  if (! ok1) ok = false;
  (void) nameValues.nameValueBool        ("mid_line_ends" , midHeadData_.lineEnds , ok1);
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

  bool ok;
  bool b = CQChartsUtil::stringToBool(lstr, &ok);
  if (ok) { visible = b; return true; }

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
