#ifndef CQChartsPlotType_H
#define CQChartsPlotType_H

#include <CQChartsPlotParameter.h>
#include <QAbstractItemModel>
#include <QSharedPointer>
#include <QString>
#include <map>
#include <vector>

class CQChartsPlotType;
class CQChartsView;
class CQChartsPlot;

class CQChartsPlotTypeMgr {
 public:
  using Types = std::vector<CQChartsPlotType*>;

 public:
  CQChartsPlotTypeMgr();
 ~CQChartsPlotTypeMgr();

  void addType(const QString &name, CQChartsPlotType *type);

  bool isType(const QString &name) const;

  CQChartsPlotType *type(const QString &name) const;

  void getTypeNames(QStringList &names, QStringList &descs) const;

  void getTypes(Types &types) const;

 private:
  using NameTypes = std::map<QString,CQChartsPlotType*>;

  NameTypes nameTypes_;
};

//----

class CQChartsPlotType {
 public:
  using Parameters          = std::vector<CQChartsPlotParameter>;
  using ParameterGroups     = std::map<int,CQChartsPlotParameterGroup>;
  using ParameterAttributes = CQChartsPlotParameterAttributes;
  using ModelP              = QSharedPointer<QAbstractItemModel>;

  enum Dimension {
    NONE,
    ONE_D,
    TWO_D,
  };

 public:
  CQChartsPlotType();

  virtual ~CQChartsPlotType() { }

  // type name and description
  virtual QString name() const = 0;
  virtual QString desc() const = 0;

  virtual Dimension dimension() const = 0;

  // plot parameters
  // (required/key options to initialize plot)
  virtual void addParameters() = 0;

  const Parameters &parameters() const { return parameters_; }

  const ParameterGroups &parameterGroups() const { return parameterGroups_; }

  Parameters groupParameters(int groupId) const;

  Parameters nonGroupParameters() const;

  //---

  void startParameterGroup(const QString &name);
  void endParameterGroup();

  //---

  CQChartsPlotParameter &
  addColumnParameter(const QString &name, const QString &desc, const QString &propName,
                     int defValue=-1);
  CQChartsPlotParameter &
  addColumnParameter(const QString &name, const QString &desc, const QString &propName,
                     const ParameterAttributes &attributes, int defValue=-1);

  CQChartsPlotParameter &
  addColumnsParameter(const QString &name, const QString &desc, const QString &propName,
                      const QString &defValue="");
  CQChartsPlotParameter &
  addColumnsParameter(const QString &name, const QString &desc, const QString &propName,
                      const ParameterAttributes &attributes, const QString &defValue);

  CQChartsPlotParameter &
  addStringParameter(const QString &name, const QString &desc, const QString &propName,
                     const QString &defValue="");
  CQChartsPlotParameter &
  addStringParameter(const QString &name, const QString &desc, const QString &propName,
                     const ParameterAttributes &attributes, const QString &defValue);

  CQChartsPlotParameter &
  addRealParameter(const QString &name, const QString &desc, const QString &propName,
                   double defValue=0.0);
  CQChartsPlotParameter &
  addRealParameter(const QString &name, const QString &desc, const QString &propName,
                   const ParameterAttributes &attributes, double defValue);

  CQChartsPlotParameter &
  addIntParameter(const QString &name, const QString &desc, const QString &propName,
                  int defValue=0);
  CQChartsPlotParameter &
  addIntParameter(const QString &name, const QString &desc, const QString &propName,
                  const ParameterAttributes &attributes, int defValue);

  CQChartsPlotParameter &
  addBoolParameter(const QString &name, const QString &desc, const QString &propName,
                   bool defValue=false);
  CQChartsPlotParameter &
  addBoolParameter(const QString &name, const QString &desc, const QString &propName,
                   const ParameterAttributes &attributes, bool defValue);

  CQChartsPlotParameter &addParameter(const CQChartsPlotParameter &parameter);

  //---

  virtual const char *xColumnName() const { return nullptr; }
  virtual const char *yColumnName() const { return nullptr; }

  virtual bool customXRange() const { return true; }
  virtual bool customYRange() const { return true; }

  virtual bool hasAxes () const { return true; }
  virtual bool hasKey  () const { return true; }
  virtual bool hasTitle() const { return true; }

  // TODO: use plot first then default to type
  virtual bool allowXAxisIntegral() const { return true; }
  virtual bool allowYAxisIntegral() const { return true; }

  virtual bool allowXLog() const { return true; }
  virtual bool allowYLog() const { return true; }

  virtual QString description() const { return QString(); }

  virtual bool isHierarchical() const { return false; }

  //---

  // create plot
  virtual CQChartsPlot *create(CQChartsView *view, const ModelP &model) const = 0;

 protected:
  Parameters      parameters_;
  ParameterGroups parameterGroups_;
  int             parameterGroupId_ { -1 };
};

#endif
