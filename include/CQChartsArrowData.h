#ifndef CQChartsArrowData_H
#define CQChartsArrowData_H

#include <CQChartsLength.h>
#include <CQChartsAngle.h>
#include <CQUtilMeta.h>

class CQChartsNameValues;

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

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsArrowData, metaTypeId)

 public:
  enum class HeadType {
    NONE,
    ARROW,
    TRIANGLE,
    STEALTH,
    DIAMOND,
    LINE
  };

  using Length = CQChartsLength;
  using Angle  = CQChartsAngle;
  using Units  = CQChartsUnits::Type;

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

  const Length &lineWidth() const { return lineWidth_; }
  void setLineWidth(const Length &l) { lineWidth_ = l; }

  //---

  // front head data
  bool isFHead() const { return fheadData_.visible; }
  void setFHead(bool b) { fheadData_.visible = b; }

  HeadType fheadType() const { return fheadData_.type; }
  void setFHeadType(HeadType type);

  bool calcIsFHead() const { return (fheadType() != HeadType::NONE); }

  const Angle &frontAngle() const { return fheadData_.angle; }
  void setFrontAngle(const Angle &a) { fheadData_.angle = a; updateFrontBackAngle(); }

  Angle calcFrontAngle() const { return (frontAngle().value() > 0 ? frontAngle() : Angle(45)); }

  const Angle &frontBackAngle() const { return fheadData_.backAngle; }
  void setFrontBackAngle(const Angle &a) { fheadData_.backAngle = a; }

  Angle calcFrontBackAngle() const {
    return (frontBackAngle().value() > 0 ? frontBackAngle() : Angle(90)); }

  const Length &frontLength() const { return fheadData_.length; }
  void setFrontLength(const Length &l) { fheadData_.length = l; }

  Length calcFrontLength() const {
    return (frontLength().value() > 0 ? frontLength() : Length::pixel(8)); }

  bool isFrontLineEnds() const { return fheadData_.lineEnds; }
  void setFrontLineEnds(bool b) { fheadData_.lineEnds = b; }

  //---

  // tail head data
  bool isTHead() const { return theadData_.visible; }
  void setTHead(bool b) { theadData_.visible = b; }

  HeadType theadType() const { return theadData_.type; }
  void setTHeadType(HeadType type);

  bool calcIsTHead() const { return (theadType() != HeadType::NONE); }

  const Angle &tailAngle() const { return theadData_.angle; }
  void setTailAngle(const Angle &a) { theadData_.angle = a; updateTailBackAngle(); }

  Angle calcTailAngle() const { return (tailAngle().value() > 0 ? tailAngle() : Angle(45)); }

  const Angle &tailBackAngle() const { return theadData_.backAngle; }
  void setTailBackAngle(const Angle &a) { theadData_.backAngle = a; }

  Angle calcTailBackAngle() const {
    return (tailBackAngle().value() > 0 ? tailBackAngle() : Angle(90));
  }

  const Length &tailLength() const { return theadData_.length; }
  void setTailLength(const Length &l) { theadData_.length = l; }

  Length calcTailLength() const {
    return (tailLength().value() > 0 ? tailLength() : Length::pixel(8)); }

  bool isTailLineEnds() const { return theadData_.lineEnds; }
  void setTailLineEnds(bool b) { theadData_.lineEnds = b; }

  //---

  // consistent tail+head angles
  const Angle &angle() const { return tailAngle(); }
  void setAngle(const Angle &a) { setFrontAngle(a); setTailAngle(a); }

  const Angle &backAngle() const { return tailBackAngle(); }
  void setBackAngle(const Angle &a) { setFrontBackAngle(a); setTailBackAngle(a); }

  const Length &length() const { return tailLength(); }
  void setLength(const Length &l) { setFrontLength(l); setTailLength(l); }

  bool isLineEnds() const { return isTailLineEnds(); }
  void setLineEnds(bool b) { setFrontLineEnds(b); setTailLineEnds(b); }

  //---

  bool isValid() const { return true; }

  QString toString() const;
  bool fromString(const QString &s);

  void setNameValues(CQChartsNameValues &nameValues) const;
  bool getNameValues(const CQChartsNameValues &nameValues);

  //---

  static bool getTypeAngles(const HeadType &type, Angle &angle, Angle &backAngle);
  static bool checkTypeAngles(const HeadType &type, const Angle &angle, const Angle &backAngle);

  static bool nameToData(const QString &name, HeadType &type, bool &lineEnds, bool &visible);

  static bool dataToName(const HeadType &type, bool lineEnds, bool visible,
                         const Angle &angle, const Angle &backAngle, QString &name);

 private:
  void updateFrontBackAngle();
  void updateTailBackAngle ();

  static bool getTypeBackAngle(const HeadType &type, const Angle &angle, Angle &backAngle);

 private:
  struct HeadData {
    bool     visible   { false };          //!< draw arrow head
    HeadType type      { HeadType::NONE }; //!< arrow head type
    Angle    angle     { -1 };             //!< arrow angle (default 45 if <= 0)
    Angle    backAngle { -1 };             //!< back angle (default 90 if <= 0)
    Length   length    { "1V" };           //!< arrow length
    bool     lineEnds  { false };          //!< lines at end
  };

//bool     relative_   { false };            //!< to point relative to from
  Length   lineWidth_  { Length::plot(-1) }; //!< connecting line width
  HeadData fheadData_;                       //!< front head data
  HeadData theadData_;                       //!< tail head data
};

CQUTIL_DCL_META_TYPE(CQChartsArrowData)

#endif
