#ifndef CQChartsPlotParameter_H
#define CQChartsPlotParameter_H

#include <QString>
#include <QVariant>

class CQChartsPlotParameterAttributes {
 public:
  enum Flags {
    REQUIRED  = (1<<0),
    MONOTONIC = (1<<1),
    NUMERIC   = (1<<2),
    STRING    = (1<<3),
    COLOR     = (1<<4),
    GROUPABLE = (1<<5),
    MAPPED    = (1<<6)
  };

 public:
  CQChartsPlotParameterAttributes() { }

  // optional/required
  bool isOptional() const { return ! isRequired(); }
  CQChartsPlotParameterAttributes &setOptional () { flags_ &= ~REQUIRED ; return *this; }

  bool isRequired() const { return (flags_ & REQUIRED); }
  CQChartsPlotParameterAttributes &setRequired() { flags_ |= REQUIRED; return *this; }

  //---

  // monotonic
  bool isMonotonic() const { return (flags_ & MONOTONIC); }
  CQChartsPlotParameterAttributes &setMonotonic() { flags_ |= MONOTONIC; return *this; }

  //---

  // value types
  bool isNumeric() const { return (flags_ & NUMERIC); }
  CQChartsPlotParameterAttributes &setNumeric() { flags_ |= NUMERIC; return *this; }

  bool isString() const { return (flags_ & STRING); }
  CQChartsPlotParameterAttributes &setString() { flags_ |= STRING; return *this; }

  bool isColor() const { return (flags_ & COLOR); }
  CQChartsPlotParameterAttributes &setColor() { flags_ |= COLOR; return *this; }

  //---

  // groupable
  bool isGroupable() const { return (flags_ & GROUPABLE); }
  CQChartsPlotParameterAttributes &setGroupable() { flags_ |= GROUPABLE; return *this; }

  //---

  // mapping (normalize 0->1 to min/max)
  bool isMapped() const { return (flags_ & MAPPED); }
  CQChartsPlotParameterAttributes &setMapped() { flags_ |= MAPPED; return *this; }

  double mapMin() const { return mapMin_; }
  CQChartsPlotParameterAttributes &setMapMin(double r) { mapMin_ = r; return *this; }

  double mapMax() const { return mapMax_; }
  CQChartsPlotParameterAttributes &setMapMax(double r) { mapMax_ = r; return *this; }

  CQChartsPlotParameterAttributes &setMapMinMax(double min, double max) {
    mapMin_ = min; mapMax_ = max; return *this;
  }

  QString summary() const {
    QString str = (isOptional() ? "optional" : "required");

    str += (isMonotonic() ? "|monotonic" : "");

    str += (isNumeric() ? "|numeric" : "");
    str += (isString () ? "|string"  : "");
    str += (isColor  () ? "|color"   : "");

    str += (isGroupable() ? "|groupable" : "");

    return str;
  }

 private:
  unsigned int flags_  { 0 };   // flags
  double       mapMin_ { 0.0 }; // map min
  double       mapMax_ { 1.0 }; // map max
};

class CQChartsPlotParameter {
 public:
  using Attributes = CQChartsPlotParameterAttributes;

 public:
  CQChartsPlotParameter(const QString &name, const QString &desc, const QString &type,
                        const QString &propName, const Attributes &attributes=Attributes(),
                        const QVariant &defValue=QVariant()) :
   name_(name), desc_(desc), type_(type), propName_(propName), attributes_(attributes),
   defValue_(defValue) {
  }

  virtual ~CQChartsPlotParameter() { }

  const QString &name() const { return name_; }
  CQChartsPlotParameter &setName(const QString &s) { name_ = s; return *this; }

  const QString &desc() const { return desc_; }
  CQChartsPlotParameter &setDesc(const QString &s) { desc_ = s; return *this; }

  const QString &type() const { return type_; }
  CQChartsPlotParameter &setType(const QString &s) { type_ = s; return *this; }

  const QString &propName() const { return propName_; }
  CQChartsPlotParameter &setPropName(const QString &s) { propName_ = s; return *this; }

  const Attributes &attributes() const { return attributes_; }
  CQChartsPlotParameter &setAttributes(const Attributes &s) { attributes_ = s; return *this; }

  bool inGroup() const { return groupId_ > 0; }

  int groupId() const { return groupId_; }
  void setGroupId(int i) { groupId_ = i; }

  const QVariant &defValue() const { return defValue_; }
  CQChartsPlotParameter &setDefValue(const QVariant &v) { defValue_ = v; return *this; }

  const QString &tip() const { return tip_; }
  CQChartsPlotParameter &setTip(const QString &s) { tip_ = s; return *this; }

  CQChartsPlotParameter &setOptional () { attributes_.setOptional (); return *this; }
  CQChartsPlotParameter &setRequired () { attributes_.setRequired (); return *this; }
  CQChartsPlotParameter &setMonotonic() { attributes_.setMonotonic(); return *this; }
  CQChartsPlotParameter &setNumeric  () { attributes_.setNumeric  (); return *this; }
  CQChartsPlotParameter &setString   () { attributes_.setString   (); return *this; }
  CQChartsPlotParameter &setColor    () { attributes_.setColor    (); return *this; }
  CQChartsPlotParameter &setGroupable() { attributes_.setGroupable(); return *this; }
  CQChartsPlotParameter &setMapped   () { attributes_.setMapped   (); return *this; }

  CQChartsPlotParameter &setMapMin(double r) { attributes_.setMapMin(r); return *this; }
  CQChartsPlotParameter &setMapMax(double r) { attributes_.setMapMax(r); return *this; }

  CQChartsPlotParameter &setMapMinMax(double min, double max) {
    attributes_.setMapMinMax(min, max); return *this; }

  bool mapPropNames(QString &mappedName, QString &mapMinName, QString &mapMaxName) const {
    QString columnPropName = this->propName();

    int pos = columnPropName.indexOf("Column");
    if (pos < 0) return false;

    mappedName = columnPropName.mid(0, pos) + "Mapped";
    mapMinName = columnPropName.mid(0, pos) + "MapMin";
    mapMaxName = columnPropName.mid(0, pos) + "MapMax";

    return true;
  }

 private:
  QString    name_;           //! name
  QString    desc_;           //! description
  QString    type_;           //! type
  QString    propName_;       //! property name
  Attributes attributes_;     //! attributes
  int        groupId_ { -1 }; //! group id
  QVariant   defValue_;       //! default value
  QString    tip_;            //! tip
};

//---

class CQChartsColumnParameter : public CQChartsPlotParameter {
 public:
  CQChartsColumnParameter(const QString &name, const QString &desc, const QString &propName,
                          const Attributes &attributes=Attributes(), int defValue=-1) :
   CQChartsPlotParameter(name, desc, "column", propName, attributes,
                         (defValue >= 0 ? QVariant(defValue) : QVariant())) {
  }
};

//---

class CQChartsColumnsParameter : public CQChartsPlotParameter {
 public:
  CQChartsColumnsParameter(const QString &name, const QString &desc, const QString &propName,
                           const Attributes &attributes=Attributes(), const QString &defValue="") :
   CQChartsPlotParameter(name, desc, "columns", propName, attributes,
                         (defValue != "" ? QVariant(defValue) : QVariant())) {
  }
};

//---

class CQChartsStringParameter : public CQChartsPlotParameter {
 public:
  CQChartsStringParameter(const QString &name, const QString &desc, const QString &propName,
                          const Attributes &attributes=Attributes(), const QString &defValue="") :
   CQChartsPlotParameter(name, desc, "string", propName, attributes, QVariant(defValue)) {
  }
};

//---

class CQChartsRealParameter : public CQChartsPlotParameter {
 public:
  CQChartsRealParameter(const QString &name, const QString &desc, const QString &propName,
                        const Attributes &attributes=Attributes(), double defValue=0.0) :
   CQChartsPlotParameter(name, desc, "real", propName, attributes, QVariant(defValue)) {
  }
};

//---

class CQChartsIntParameter : public CQChartsPlotParameter {
 public:
  CQChartsIntParameter(const QString &name, const QString &desc, const QString &propName,
                       const Attributes &attributes=Attributes(), int defValue=0) :
   CQChartsPlotParameter(name, desc, "int", propName, attributes, QVariant(defValue)) {
  }
};

//---

class CQChartsBoolParameter : public CQChartsPlotParameter {
 public:
  CQChartsBoolParameter(const QString &name, const QString &desc, const QString &propName,
                        const Attributes &attributes=Attributes(), bool defValue=false) :
   CQChartsPlotParameter(name, desc, "bool", propName, attributes, QVariant(defValue)) {
  }
};

//------

class CQChartsPlotParameterGroup {
 public:
  CQChartsPlotParameterGroup(const QString &name="", int groupId=-1) :
   name_(name), groupId_(groupId) {
  }

  const QString &name() const { return name_; }
  void setName(const QString &v) { name_ = v; }

  int groupId() const { return groupId_; }
  void setGroupId(int i) { groupId_ = i; }

 private:
  QString name_;
  int     groupId_ { -1 };
};

#endif
