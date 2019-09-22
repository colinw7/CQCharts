#ifndef CQChartsPlotParameter_H
#define CQChartsPlotParameter_H

#include <CQBaseModelTypes.h>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

/*!
 * \brief plot parameter attributes
 * \ingroup Charts
 */
class CQChartsPlotParameterAttributes {
 public:
  enum Flags {
    REQUIRED      = (1<<0),
    MONOTONIC     = (1<<1),
    NUMERIC       = (1<<2),
    STRING        = (1<<3),
    BOOL          = (1<<4),
    COLOR         = (1<<5),
    GROUPABLE     = (1<<6),
    MAPPED        = (1<<7),
    DISCRIMINATOR = (1<<8)
  };

 public:
  CQChartsPlotParameterAttributes() { }

  // optional/required
  bool isOptional() const { return ! isRequired(); }
  CQChartsPlotParameterAttributes &setOptional() { flags_ &= ~REQUIRED ; return *this; }

  bool isRequired() const { return (flags_ & REQUIRED); }
  CQChartsPlotParameterAttributes &setRequired() { flags_ |= REQUIRED; return *this; }

  //---

  bool isDiscriminator() const { return (flags_ & DISCRIMINATOR); }
  CQChartsPlotParameterAttributes &setDiscriminator() { flags_ |= DISCRIMINATOR ; return *this; }

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

  bool isBool() const { return (flags_ & BOOL); }
  CQChartsPlotParameterAttributes &setBool() { flags_ |= BOOL; return *this; }

  bool isColor() const { return (flags_ & COLOR); }
  CQChartsPlotParameterAttributes &setColor() { flags_ |= COLOR; return *this; }

  bool hasTypeDetail() const { return isNumeric() || isString() || isBool() || isColor(); }

  QString typeDetail() const {
    if (isNumeric()) return "numeric";
    if (isString ()) return "string";
    if (isBool   ()) return "bool";
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
  unsigned int flags_  { 0 };   //!< flags
  double       mapMin_ { 0.0 }; //!< map min
  double       mapMax_ { 1.0 }; //!< map max
};

//---

/*!
 * \brief plot parameter object
 * \ingroup Charts
 */
class CQChartsPlotParameter : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString  name            READ name            WRITE setName    )
  Q_PROPERTY(QString  desc            READ desc            WRITE setDesc    )
  Q_PROPERTY(QString  type            READ typeName        WRITE setTypeName)
  Q_PROPERTY(QString  propName        READ propName        WRITE setPropName)
  Q_PROPERTY(int      groupId         READ groupId         WRITE setGroupId )
  Q_PROPERTY(QVariant defValue        READ defValue        WRITE setDefValue)
  Q_PROPERTY(QString  tip             READ tip             WRITE setTip     )
  Q_PROPERTY(bool     hidden          READ isHidden        WRITE setHidden  )
  Q_PROPERTY(bool     isColumn        READ isColumn                         )
  Q_PROPERTY(bool     isMultiple      READ isMultiple                       )
  Q_PROPERTY(bool     isOptional      READ isOptional                       )
  Q_PROPERTY(bool     isRequired      READ isRequired                       )
  Q_PROPERTY(bool     isDiscriminator READ isDiscriminator                  )
  Q_PROPERTY(bool     isMonotonic     READ isMonotonic                      )
  Q_PROPERTY(bool     isNumeric       READ isNumeric                        )
  Q_PROPERTY(bool     isString        READ isString                         )
  Q_PROPERTY(bool     isBool          READ isBool                           )
  Q_PROPERTY(bool     isColor         READ isColor                          )
  Q_PROPERTY(bool     hasTypeDetail   READ hasTypeDetail                    )
  Q_PROPERTY(QString  typeDetail      READ typeDetail                       )
  Q_PROPERTY(bool     isGroupable     READ isGroupable                      )
  Q_PROPERTY(bool     isMapped        READ isMapped                         )
  Q_PROPERTY(double   mapMin          READ mapMin                           )
  Q_PROPERTY(double   mapMax          READ mapMax                           )

 public:
  using Attributes = CQChartsPlotParameterAttributes;
  using Type       = CQBaseModelType;

 public:
  CQChartsPlotParameter(const QString &name, const QString &desc, const Type &type,
                        const QString &propName, const Attributes &attributes=Attributes(),
                        const QVariant &defValue=QVariant());

  virtual ~CQChartsPlotParameter() { }

  //! get/set name
  const QString &name() const { return name_; }
  CQChartsPlotParameter &setName(const QString &s) { name_ = s; return *this; }

  //! get/set description
  const QString &desc() const { return desc_; }
  CQChartsPlotParameter &setDesc(const QString &s) { desc_ = s; return *this; }

  //! get/set type
  const Type &type() const { return type_; }
  CQChartsPlotParameter &setType(const Type &s) { type_ = s; return *this; }

  //! get/set type name
  QString typeName() const;
  CQChartsPlotParameter &setTypeName(const QString &name);

  //! get/set property name
  const QString &propName() const { return propName_; }
  CQChartsPlotParameter &setPropName(const QString &s) { propName_ = s; return *this; }

  //! get/set attributes
  const Attributes &attributes() const { return attributes_; }
  CQChartsPlotParameter &setAttributes(const Attributes &s) { attributes_ = s; return *this; }

  //! is in group
  bool inGroup() const { return groupId_ > 0; }

  //! get/set associated group id
  int groupId() const { return groupId_; }
  void setGroupId(int i) { groupId_ = i; }

  //! get/set default value
  const QVariant &defValue() const { return defValue_; }
  CQChartsPlotParameter &setDefValue(const QVariant &v) { defValue_ = v; return *this; }

  //! get/set tip
  const QString &tip() const { return tip_; }
  CQChartsPlotParameter &setTip(const QString &s) { tip_ = s; return *this; }

  //! get/set is hidden
  bool isHidden() const { return hidden_; }
  void setHidden(bool b) { hidden_ = b; }

  //! is a column parameter
  virtual bool isColumn() const { return false; }

  //! does support multiple values
  virtual bool isMultiple() const { return false; }

  //! get/set optional
  bool isOptional() const { return attributes_.isOptional(); }
  CQChartsPlotParameter &setOptional() { attributes_.setOptional(); return *this; }

  //! get/set required
  bool isRequired() const { return attributes_.isRequired(); }
  CQChartsPlotParameter &setRequired() { attributes_.setRequired(); return *this; }

  //! get/set is discriminator (for analyze)
  bool isDiscriminator() const { return attributes_.isDiscriminator(); }
  CQChartsPlotParameter &setDiscriminator() { attributes_.setDiscriminator(); return *this; }

  //! get/set is monotonic (for analyze)
  bool isMonotonic() const { return attributes_.isMonotonic(); }
  CQChartsPlotParameter &setMonotonic() { attributes_.setMonotonic(); return *this; }

  //! get/set is numeric (real or integer)
  bool isNumeric() const { return attributes_.isNumeric(); }
  CQChartsPlotParameter &setNumeric() { attributes_.setNumeric(); return *this; }

  //! get/set is string
  bool isString() const { return attributes_.isString(); }
  CQChartsPlotParameter &setString() { attributes_.setString(); return *this; }

  //! get/set is boolean
  bool isBool() const { return attributes_.isBool(); }
  CQChartsPlotParameter &setBool() { attributes_.setBool(); return *this; }

  //! get/set is color
  bool isColor() const { return attributes_.isColor(); }
  CQChartsPlotParameter &setColor() { attributes_.setColor(); return *this; }

  //! has type detail
  bool hasTypeDetail() const { return attributes_.hasTypeDetail(); }

  //! type detail string
  QString typeDetail() const { return attributes_.typeDetail(); }

  //! get/set is groupable
  bool isGroupable() const { return attributes_.isGroupable  (); }
  CQChartsPlotParameter &setGroupable() { attributes_.setGroupable(); return *this; }

  //! get/set is mapped value
  bool isMapped() const { return attributes_.isMapped(); }
  CQChartsPlotParameter &setMapped() { attributes_.setMapped(); return *this; }

  //! get/set is map minimum
  double mapMin() const { return attributes_.mapMin(); }
  CQChartsPlotParameter &setMapMin(double r) { attributes_.setMapMin(r); return *this; }

  //! get/set is map maximum
  double mapMax() const { return attributes_.mapMax(); }
  CQChartsPlotParameter &setMapMax(double r) { attributes_.setMapMax(r); return *this; }

  //! set map maximum and maximum
  CQChartsPlotParameter &setMapMinMax(double min, double max) {
    attributes_.setMapMinMax(min, max); return *this; }

  //! get map property names
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

  //! add parameter property (for tcl command)
  void addProperty(const QString &name, const QString &propName, const QString &desc);

  //! get parameter property names (for tcl command)
  void propertyNames(QStringList &names) const;

  //! has specified property name
  bool hasProperty(const QString &name) const;

  QVariant getPropertyValue(const QString &name) const;

 private:
  CQChartsPlotParameter(CQChartsPlotParameter &p);
  CQChartsPlotParameter &operator=(const CQChartsPlotParameter &);

 private:
  //! property data
  struct PropertyData {
    QString name;
    QString propName;
    QString desc;

    PropertyData(const QString &name="", const QString &propName="", const QString &desc="") :
     name(name), propName(propName), desc(desc) {
    }
  };

  using Properties = std::map<QString,PropertyData>;

  QString    name_;                    //!< name
  QString    desc_;                    //!< description
  Type       type_     { Type::NONE }; //!< type
  QString    propName_;                //!< property name
  Attributes attributes_;              //!< attributes
  int        groupId_ { -1 };          //!< parent group id
  QVariant   defValue_;                //!< default value
  QString    tip_;                     //!< tip
  bool       hidden_  { false };       //!< is hidden
  Properties properties_;              //!< properties
};

//---

/*!
 * \brief column parameter
 * \ingroup Charts
 */
class CQChartsColumnParameter : public CQChartsPlotParameter {
  Q_OBJECT

 public:
  CQChartsColumnParameter(const QString &name, const QString &desc, const QString &propName,
                          const Attributes &attributes=Attributes(), int defValue=-1) :
   CQChartsPlotParameter(name, desc, Type::COLUMN, propName, attributes,
                         (defValue >= 0 ? QVariant(defValue) : QVariant())) {
  }

  bool isColumn() const override { return true; }
};

//---

/*!
 * \brief columns parameter
 * \ingroup Charts
 */
class CQChartsColumnsParameter : public CQChartsPlotParameter {
  Q_OBJECT

 public:
  CQChartsColumnsParameter(const QString &name, const QString &desc, const QString &propName,
                           const Attributes &attributes=Attributes(), const QString &defValue="") :
   CQChartsPlotParameter(name, desc, Type::COLUMN_LIST, propName, attributes,
                         (defValue != "" ? QVariant(defValue) : QVariant())) {
  }

  bool isColumn() const override { return true; }

  bool isMultiple() const override { return true; }
};

//---

/*!
 * \brief string parameter
 * \ingroup Charts
 */
class CQChartsStringParameter : public CQChartsPlotParameter {
  Q_OBJECT

 public:
  CQChartsStringParameter(const QString &name, const QString &desc, const QString &propName,
                          const Attributes &attributes=Attributes(), const QString &defValue="") :
   CQChartsPlotParameter(name, desc, Type::STRING, propName, attributes, QVariant(defValue)) {
  }
};

//---

/*!
 * \brief real parameter
 * \ingroup Charts
 */
class CQChartsRealParameter : public CQChartsPlotParameter {
  Q_OBJECT

 public:
  CQChartsRealParameter(const QString &name, const QString &desc, const QString &propName,
                        const Attributes &attributes=Attributes(), double defValue=0.0) :
   CQChartsPlotParameter(name, desc, Type::REAL, propName, attributes, QVariant(defValue)) {
  }
};

//---

/*!
 * \brief integer parameter
 * \ingroup Charts
 */
class CQChartsIntParameter : public CQChartsPlotParameter {
  Q_OBJECT

 public:
  CQChartsIntParameter(const QString &name, const QString &desc, const QString &propName,
                       const Attributes &attributes=Attributes(), int defValue=0) :
   CQChartsPlotParameter(name, desc, Type::INTEGER, propName, attributes, QVariant(defValue)) {
  }
};

//---

/*!
 * \brief enum parameter
 * \ingroup Charts
 */
class CQChartsEnumParameter : public CQChartsPlotParameter {
  Q_OBJECT

 public:
  CQChartsEnumParameter(const QString &name, const QString &desc, const QString &propName,
                        const Attributes &attributes=Attributes(), int defValue=0) :
   CQChartsPlotParameter(name, desc, Type::ENUM, propName, attributes, QVariant(defValue)) {
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

  NameValues nameValues_; //!< name values
  ValueNames valueNames_; //!< value names
};

//---

/*!
 * \brief boolean parameter
 * \ingroup Charts
 */
class CQChartsBoolParameter : public CQChartsPlotParameter {
  Q_OBJECT

 public:
  CQChartsBoolParameter(const QString &name, const QString &desc, const QString &propName,
                        const Attributes &attributes=Attributes(), bool defValue=false) :
   CQChartsPlotParameter(name, desc, Type::BOOLEAN, propName, attributes, QVariant(defValue)) {
  }
};

//------

/*!
 * \brief plot parameter group
 * \ingroup Charts
 */
class CQChartsPlotParameterGroup : public QObject {
  Q_OBJECT

  Q_PROPERTY(Type    type         READ type)
  Q_PROPERTY(QString name         READ name         WRITE setName        )
  Q_PROPERTY(int     groupId      READ groupId      WRITE setGroupId     )
  Q_PROPERTY(int     otherGroupId READ otherGroupId WRITE setOtherGroupId)

 public:
  enum Type {
    NONE,
    PRIMARY,
    SECONDARY
  };

 public:
  CQChartsPlotParameterGroup(const QString &name="", int groupId=-1) :
   name_(name), groupId_(groupId) {
  }

  //! get/set type
  const Type &type() const { return type_; }
  void setType(const Type &t) { type_ = t; }

  //! get/set name
  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  //! get/set group id
  int groupId() const { return groupId_; }
  void setGroupId(int i) { groupId_ = i; }

  //! get/set other group id (for primary or secondary)
  int otherGroupId() const { return otherGroupId_; }
  void setOtherGroupId(int i) { otherGroupId_ = i; }

  //! get/set parent group id
  int parentGroupId() const { return parentGroupId_; }
  void setParentGroupId(int i) { parentGroupId_ = i; }

  //! get/set is hidden
  bool isHidden() const { return hidden_; }
  void setHidden(bool b) { hidden_ = b; }

 private:
  Type    type_          { Type::NONE }; //!< group type
  QString name_;                         //!< group name
  int     groupId_       { -1 };         //!< group id
  int     otherGroupId_  { -1 };         //!< other group id
  int     parentGroupId_ { -1 };         //!< parent group id
  bool    hidden_        { false };      //!< is hidden
};

#endif
