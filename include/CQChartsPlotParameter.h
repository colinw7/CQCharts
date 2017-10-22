#ifndef CQChartsPlotParameter_H
#define CQChartsPlotParameter_H

#include <QString>
#include <QVariant>

class CQChartsPlotParameter {
 public:
  CQChartsPlotParameter(const QString &name, const QString &desc, const QString &type,
                        const QString &propName, const QString &attributes="",
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

  const QString &attributes() const { return attributes_; }
  CQChartsPlotParameter &setAttributes(const QString &s) { attributes_ = s; return *this; }

  const QVariant &defValue() const { return defValue_; }
  CQChartsPlotParameter &setDefValue(const QVariant &v) { defValue_ = v; return *this; }

  const QString &tip() const { return tip_; }
  CQChartsPlotParameter &setTip(const QString &s) { tip_ = s; return *this; }

 private:
  QString  name_;       //! name
  QString  desc_;       //! description
  QString  type_;       //! type
  QString  propName_;   //! property name
  QString  attributes_; //! attributes
  QVariant defValue_;   //! default value
  QString  tip_;        //! tip
};

//---

class CQChartsColumnParameter : public CQChartsPlotParameter {
 public:
  CQChartsColumnParameter(const QString &name, const QString &desc, const QString &propName,
                          const QString &attributes="", int defValue=-1) :
   CQChartsPlotParameter(name, desc, "column", propName, attributes,
                         (defValue >= 0 ? QVariant(defValue) : QVariant())) {
  }
};

//---

class CQChartsColumnsParameter : public CQChartsPlotParameter {
 public:
  CQChartsColumnsParameter(const QString &name, const QString &desc, const QString &propName,
                           const QString &attributes="", const QString &defValue="") :
   CQChartsPlotParameter(name, desc, "columns", propName, attributes,
                         (defValue != "" ? QVariant(defValue) : QVariant())) {
  }
};

//---

class CQChartsBoolParameter : public CQChartsPlotParameter {
 public:
  CQChartsBoolParameter(const QString &name, const QString &desc, const QString &propName,
                        const QString &attributes="", bool defValue=false) :
   CQChartsPlotParameter(name, desc, "bool", propName, attributes, QVariant(defValue)) {
  }
};

#endif
