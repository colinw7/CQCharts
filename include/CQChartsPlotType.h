#ifndef CQChartsPlotType_H
#define CQChartsPlotType_H

#include <CQChartsPlotParameter.h>
#include <QAbstractItemModel>
#include <QSharedPointer>
#include <QString>
#include <map>

class CQChartsPlotType;
class CQChartsView;
class CQChartsPlot;

class CQChartsPlotTypeMgr {
 public:
  CQChartsPlotTypeMgr();
 ~CQChartsPlotTypeMgr();

  void addType(const QString &name, CQChartsPlotType *type);

  bool isType(const QString &name) const;

  CQChartsPlotType *type(const QString &name) const;

  void getTypeNames(QStringList &names, QStringList &descs) const;

 private:
  using Types = std::map<QString,CQChartsPlotType*>;

  Types types_;
};

//----

class CQChartsPlotType {
 public:
  using Parameters          = std::vector<CQChartsPlotParameter>;
  using ParameterAttributes = CQChartsPlotParameterAttributes;
  using ModelP              = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsPlotType();

  virtual ~CQChartsPlotType() { }

  // type name and description
  virtual QString name() const = 0;
  virtual QString desc() const = 0;

  // plot parameters
  // (required/key options to initialize plot)
  virtual void addParameters() = 0;

  const Parameters &parameters() const { return parameters_; }

  //---

  CQChartsPlotParameter &
  addColumnParameter(const QString &name, const QString &desc, const QString &propName,
                     int defValue=-1) {
    return addColumnParameter(name, desc, propName, ParameterAttributes(), defValue);
  }

  CQChartsPlotParameter &
  addColumnParameter(const QString &name, const QString &desc, const QString &propName,
                     const ParameterAttributes &attributes, int defValue=-1) {
    return addParameter(CQChartsColumnParameter(name, desc, propName, attributes, defValue));
  }

  CQChartsPlotParameter &
  addColumnsParameter(const QString &name, const QString &desc, const QString &propName,
                      const QString &defValue="") {
    return addParameter(CQChartsColumnsParameter(name, desc, propName, ParameterAttributes(),
                                                 defValue));
  }

  CQChartsPlotParameter &
  addColumnsParameter(const QString &name, const QString &desc, const QString &propName,
                      const ParameterAttributes &attributes, const QString &defValue) {
    return addParameter(CQChartsColumnsParameter(name, desc, propName, attributes, defValue));
  }

  CQChartsPlotParameter &
  addStringParameter(const QString &name, const QString &desc, const QString &propName,
                     const QString &defValue="") {
    return addParameter(CQChartsStringParameter(name, desc, propName, ParameterAttributes(),
                                                defValue));
  }

  CQChartsPlotParameter &
  addStringParameter(const QString &name, const QString &desc, const QString &propName,
                     const ParameterAttributes &attributes, const QString &defValue) {
    return addParameter(CQChartsStringParameter(name, desc, propName, attributes, defValue));
  }

  CQChartsPlotParameter &
  addRealParameter(const QString &name, const QString &desc, const QString &propName,
                   double defValue=0.0) {
    return addParameter(CQChartsRealParameter(name, desc, propName, ParameterAttributes(),
                                              defValue));
  }

  CQChartsPlotParameter &
  addRealParameter(const QString &name, const QString &desc, const QString &propName,
                   const ParameterAttributes &attributes, double defValue) {
    return addParameter(CQChartsRealParameter(name, desc, propName, attributes, defValue));
  }

  CQChartsPlotParameter &
  addBoolParameter(const QString &name, const QString &desc, const QString &propName,
                   bool defValue=false) {
    return addParameter(CQChartsBoolParameter(name, desc, propName, ParameterAttributes(),
                                              defValue));
  }

  CQChartsPlotParameter &
  addBoolParameter(const QString &name, const QString &desc, const QString &propName,
                   const ParameterAttributes &attributes, bool defValue) {
    return addParameter(CQChartsBoolParameter(name, desc, propName, attributes, defValue));
  }

  CQChartsPlotParameter &addParameter(const CQChartsPlotParameter &parameter) {
    parameters_.push_back(parameter);

    return parameters_.back();
  }

  //---

  virtual const char *xColumnName() const { return nullptr; }
  virtual const char *yColumnName() const { return nullptr; }

  virtual bool customXRange() const { return true; }
  virtual bool customYRange() const { return true; }

  virtual bool hasAxes () const { return true; }
  virtual bool hasKey  () const { return true; }
  virtual bool hasTitle() const { return true; }

  virtual bool allowXAxisIntegral() const { return true; }
  virtual bool allowYAxisIntegral() const { return true; }

  virtual bool allowXLog() const { return true; }
  virtual bool allowYLog() const { return true; }

  virtual QString description() const { return QString(); }

  //---

  // create plot
  virtual CQChartsPlot *create(CQChartsView *view, const ModelP &model) const = 0;

 protected:
  Parameters parameters_;
};

#endif
