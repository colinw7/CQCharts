#ifndef CQChartsPlotParameter_H
#define CQChartsPlotParameter_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

class CQChartsPlotParameterAttributes {
 public:
  enum Flags {
    REQUIRED     = (1<<0),
    MONOTONIC    = (1<<1),
    NUMERIC      = (1<<2),
    STRING       = (1<<3),
    COLOR        = (1<<4),
    GROUPABLE    = (1<<5),
    MAPPED       = (1<<6),
    DISCRIMIATOR = (1<<7)
  };

 public:
  CQChartsPlotParameterAttributes() { }

  // optional/required
  bool isOptional() const { return ! isRequired(); }
  CQChartsPlotParameterAttributes &setOptional() { flags_ &= ~REQUIRED ; return *this; }

  bool isRequired() const { return (flags_ & REQUIRED); }
  CQChartsPlotParameterAttributes &setRequired() { flags_ |= REQUIRED; return *this; }

  //---

  bool isDiscrimator() const { return (flags_ & DISCRIMIATOR); }
  CQChartsPlotParameterAttributes &setDiscrimator() { flags_ |= DISCRIMIATOR ; return *this; }

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

  bool hasTypeDetail() const { return isNumeric() || isString() || isColor(); }

  QString typeDetail() const {
    if (isNumeric()) return "numeric";
    if (isString ()) return "string";
    if (isColor  ()) return "color";

    return "generic";
  }

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

    str += (isMonotonic  () ? "|monotonic"       : "");
    str += (hasTypeDetail() ? "|" + typeDetail() : "");
    str += (isGroupable  () ? "|groupable"       : "");

    return str;
  }

 private:
  unsigned int flags_  { 0 };   // flags
  double       mapMin_ { 0.0 }; // map min
  double       mapMax_ { 1.0 }; // map max
};

class CQChartsPlotParameter : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString  name          READ name          WRITE setName    )
  Q_PROPERTY(QString  desc          READ desc          WRITE setDesc    )
  Q_PROPERTY(QString  type          READ type          WRITE setType    )
  Q_PROPERTY(QString  propName      READ propName      WRITE setPropName)
  Q_PROPERTY(int      groupId       READ groupId       WRITE setGroupId )
  Q_PROPERTY(QVariant defValue      READ defValue      WRITE setDefValue)
  Q_PROPERTY(QString  tip           READ tip           WRITE setTip     )
  Q_PROPERTY(bool     isColumn      READ isColumn                       )
  Q_PROPERTY(bool     isMultiple    READ isMultiple                     )
  Q_PROPERTY(bool     isOptional    READ isOptional                     )
  Q_PROPERTY(bool     isRequired    READ isRequired                     )
  Q_PROPERTY(bool     isDiscrimator READ isDiscrimator                  )
  Q_PROPERTY(bool     isMonotonic   READ isMonotonic                    )
  Q_PROPERTY(bool     isNumeric     READ isNumeric                      )
  Q_PROPERTY(bool     isString      READ isString                       )
  Q_PROPERTY(bool     isColor       READ isColor                        )
  Q_PROPERTY(bool     hasTypeDetail READ hasTypeDetail                  )
  Q_PROPERTY(QString  typeDetail    READ typeDetail                     )
  Q_PROPERTY(bool     isGroupable   READ isGroupable                    )
  Q_PROPERTY(bool     isMapped      READ isMapped                       )
  Q_PROPERTY(double   mapMin        READ mapMin                         )
  Q_PROPERTY(double   mapMax        READ mapMax                         )

 public:
  using Attributes = CQChartsPlotParameterAttributes;

 public:
  CQChartsPlotParameter(const QString &name, const QString &desc, const QString &type,
                        const QString &propName, const Attributes &attributes=Attributes(),
                        const QVariant &defValue=QVariant());

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

  virtual bool isColumn() const { return false; }

  virtual bool isMultiple() const { return false; }

  bool isOptional   () const { return attributes_.isOptional   (); }
  bool isRequired   () const { return attributes_.isRequired   (); }
  bool isDiscrimator() const { return attributes_.isDiscrimator(); }
  bool isMonotonic  () const { return attributes_.isMonotonic  (); }

  bool isNumeric() const { return attributes_.isNumeric    (); }
  bool isString () const { return attributes_.isString     (); }
  bool isColor  () const { return attributes_.isColor      (); }

  bool hasTypeDetail() const { return attributes_.hasTypeDetail(); }

  QString typeDetail() const { return attributes_.typeDetail(); }

  bool isGroupable() const { return attributes_.isGroupable  (); }
  bool isMapped   () const { return attributes_.isMapped     (); }

  CQChartsPlotParameter &setOptional   () { attributes_.setOptional   (); return *this; }
  CQChartsPlotParameter &setRequired   () { attributes_.setRequired   (); return *this; }
  CQChartsPlotParameter &setDiscrimator() { attributes_.setDiscrimator(); return *this; }
  CQChartsPlotParameter &setMonotonic  () { attributes_.setMonotonic  (); return *this; }
  CQChartsPlotParameter &setNumeric    () { attributes_.setNumeric    (); return *this; }
  CQChartsPlotParameter &setString     () { attributes_.setString     (); return *this; }
  CQChartsPlotParameter &setColor      () { attributes_.setColor      (); return *this; }
  CQChartsPlotParameter &setGroupable  () { attributes_.setGroupable  (); return *this; }
  CQChartsPlotParameter &setMapped     () { attributes_.setMapped     (); return *this; }

  double mapMin() const { return attributes_.mapMin(); }
  double mapMax() const { return attributes_.mapMax(); }

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

  //---

  void addProperty(const QString &name, const QString &propName, const QString &desc);

  void propertyNames(QStringList &names) const;

  bool hasProperty(const QString &name) const;

  QVariant getPropertyValue(const QString &name) const;

 private:
  CQChartsPlotParameter(CQChartsPlotParameter &p);
  CQChartsPlotParameter &operator=(const CQChartsPlotParameter &);

 private:
  struct PropertyData {
    QString name;
    QString propName;
    QString desc;

    PropertyData(const QString &name="", const QString &propName="", const QString &desc="") :
     name(name), propName(propName), desc(desc) {
    }
  };

  using Properties = std::map<QString,PropertyData>;

  QString    name_;           //! name
  QString    desc_;           //! description
  QString    type_;           //! type
  QString    propName_;       //! property name
  Attributes attributes_;     //! attributes
  int        groupId_ { -1 }; //! group id
  QVariant   defValue_;       //! default value
  QString    tip_;            //! tip
  Properties properties_;     //! properties
};

//---

class CQChartsColumnParameter : public CQChartsPlotParameter {
 public:
  CQChartsColumnParameter(const QString &name, const QString &desc, const QString &propName,
                          const Attributes &attributes=Attributes(), int defValue=-1) :
   CQChartsPlotParameter(name, desc, "column", propName, attributes,
                         (defValue >= 0 ? QVariant(defValue) : QVariant())) {
  }

  bool isColumn() const override { return true; }
};

//---

class CQChartsColumnsParameter : public CQChartsPlotParameter {
 public:
  CQChartsColumnsParameter(const QString &name, const QString &desc, const QString &propName,
                           const Attributes &attributes=Attributes(), const QString &defValue="") :
   CQChartsPlotParameter(name, desc, "columns", propName, attributes,
                         (defValue != "" ? QVariant(defValue) : QVariant())) {
  }

  bool isColumn() const override { return true; }

  bool isMultiple() const override { return true; }
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

class CQChartsEnumParameter : public CQChartsPlotParameter {
 public:
  CQChartsEnumParameter(const QString &name, const QString &desc, const QString &propName,
                        const Attributes &attributes=Attributes(), int defValue=0) :
   CQChartsPlotParameter(name, desc, "enum", propName, attributes, QVariant(defValue)) {
  }

  CQChartsEnumParameter &addNameValue(const QString &name, int value) {
    nameValues_[name ] = value;
    valueNames_[value] = name;

    return *this;
  }

  QStringList names() const {
    QStringList names;

    for (const auto &nv : nameValues_)
      names.push_back(nv.first);

    return names;
  }

  int nameValue(const QString &name) const {
    auto p = nameValues_.find(name);
    if (p == nameValues_.end()) return -1;

    return (*p).second;
  }

  QString valueName(int value) const {
    auto p = valueNames_.find(value);
    if (p == valueNames_.end()) return "";

    return (*p).second;
  }

 private:
  using NameValues = std::map<QString,int>;
  using ValueNames = std::map<int,QString>;

  NameValues nameValues_;
  ValueNames valueNames_;
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
  void setName(const QString &s) { name_ = s; }

  int groupId() const { return groupId_; }
  void setGroupId(int i) { groupId_ = i; }

 private:
  QString name_;
  int     groupId_ { -1 };
};

#endif
