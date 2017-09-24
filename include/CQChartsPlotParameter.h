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
  void setName(const QString &s) { name_ = s; }

  const QString &desc() const { return desc_; }
  void setDesc(const QString &s) { desc_ = s; }

  const QString &type() const { return type_; }
  void setType(const QString &s) { type_ = s; }

  const QString &propName() const { return propName_; }
  void setPropName(const QString &s) { propName_ = s; }

  const QString &attributes() const { return attributes_; }
  void setAttributes(const QString &s) { attributes_ = s; }

  const QVariant &defValue() const { return defValue_; }
  void setDefValue(const QVariant &v) { defValue_ = v; }

 private:
  QString  name_;
  QString  desc_;
  QString  type_;
  QString  propName_;
  QString  attributes_;
  QVariant defValue_;
};

class CQChartsColumnParameter : public CQChartsPlotParameter {
 public:
  CQChartsColumnParameter(const QString &name, const QString &desc, const QString &propName,
                          const QString &attributes="", int defValue=-1) :
   CQChartsPlotParameter(name, desc, "column", propName, attributes,
                         (defValue >= 0 ? QVariant(defValue) : QVariant())) {
  }
};

class CQChartsColumnsParameter : public CQChartsPlotParameter {
 public:
  CQChartsColumnsParameter(const QString &name, const QString &desc, const QString &propName,
                           const QString &attributes="", const QString &defValue="") :
   CQChartsPlotParameter(name, desc, "columns", propName, attributes,
                         (defValue != "" ? QVariant(defValue) : QVariant())) {
  }
};

class CQChartsBoolParameter : public CQChartsPlotParameter {
 public:
  CQChartsBoolParameter(const QString &name, const QString &desc, const QString &propName,
                        const QString &attributes="", bool defValue=false) :
   CQChartsPlotParameter(name, desc, "bool", propName, attributes, QVariant(defValue)) {
  }
};

#endif
