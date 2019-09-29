#ifndef CQChartsPlotType_H
#define CQChartsPlotType_H

#include <CQChartsPlotParameter.h>
#include <QAbstractItemModel>
#include <QSharedPointer>
#include <QObject>
#include <QStringList>
#include <QString>
#include <map>
#include <vector>

class CQChartsPlotType;
class CQChartsView;
class CQChartsPlot;
class CQChartsModelColumnDetails;

/*!
 * \brief Plot Type manager
 * \ingroup Charts
 */
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

/*!
 * \brief Plot Type base class
 * \ingroup Charts
 */
class CQChartsPlotType : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString   name               READ name              )
  Q_PROPERTY(QString   desc               READ desc              )
  Q_PROPERTY(QString   htmlDesc           READ description       )
  Q_PROPERTY(Dimension dimension          READ dimension         )
  Q_PROPERTY(QString   xColumnName        READ xColumnName       )
  Q_PROPERTY(QString   yColumnName        READ yColumnName       )
  Q_PROPERTY(bool      customXRange       READ customXRange      )
  Q_PROPERTY(bool      customYRange       READ customYRange      )
  Q_PROPERTY(bool      hasAxes            READ hasAxes           )
  Q_PROPERTY(bool      hasXAxis           READ hasXAxis          )
  Q_PROPERTY(bool      hasYAxis           READ hasYAxis          )
  Q_PROPERTY(bool      hasKey             READ hasKey            )
  Q_PROPERTY(bool      hasTitle           READ hasTitle          )
  Q_PROPERTY(bool      hasColor           READ hasColor          )
  Q_PROPERTY(bool      allowXAxisIntegral READ allowXAxisIntegral)
  Q_PROPERTY(bool      allowYAxisIntegral READ allowYAxisIntegral)
  Q_PROPERTY(bool      allowXLog          READ allowXLog         )
  Q_PROPERTY(bool      allowYLog          READ allowYLog         )
  Q_PROPERTY(bool      isGroupType        READ isGroupType       )
  Q_PROPERTY(bool      isHierarchical     READ isHierarchical    )
  Q_PROPERTY(bool      canProbe           READ canProbe          )
  Q_PROPERTY(bool      canRectSelect      READ canRectSelect     )

  Q_ENUMS(Dimension)

 public:
  using Parameters          = std::vector<CQChartsPlotParameter *>;
  using ParameterGroups     = std::vector<CQChartsPlotParameterGroup *>;
  using ParameterGroupMap   = std::map<int,CQChartsPlotParameterGroup *>;
  using ParameterAttributes = CQChartsPlotParameterAttributes;
  using ModelP              = QSharedPointer<QAbstractItemModel>;

  enum Dimension {
    NONE,
    ONE_D,
    TWO_D,
  };

 public:
  CQChartsPlotType();

  virtual ~CQChartsPlotType();

  // type name and description
  virtual QString name() const = 0;
  virtual QString desc() const = 0;

  virtual Dimension dimension() const = 0;

  //---

  // properties
  virtual QString xColumnName() const { return ""; }
  virtual QString yColumnName() const { return ""; }

  virtual bool customXRange() const { return true; }
  virtual bool customYRange() const { return true; }

  virtual bool hasAxes () const { return true; }
  virtual bool hasXAxis() const { return hasAxes(); }
  virtual bool hasYAxis() const { return hasAxes(); }
  virtual bool hasKey  () const { return true; }
  virtual bool hasTitle() const { return true; }
  virtual bool hasColor() const { return true; }

  // TODO: use plot first then default to type
  virtual bool allowXAxisIntegral() const { return true; }
  virtual bool allowYAxisIntegral() const { return true; }

  virtual bool allowXLog() const { return true; }
  virtual bool allowYLog() const { return true; }

  virtual QString description() const { return QString(); }

  virtual bool isGroupType() const { return false; }

  virtual bool isHierarchical() const { return false; }

  virtual bool canProbe() const = 0;

  virtual bool canRectSelect() const { return true; }

  //---

  // plot parameters
  // (required/key options to initialize plot)
  virtual void addParameters() = 0;

  const Parameters &parameters() const { return parameters_; }

  bool hasParameter(const QString &name) const;

  const CQChartsPlotParameter &getParameter(const QString &name) const;

  //---

  // parameter groups
  const ParameterGroupMap &parameterGroups() const { return parameterGroups_; }

  Parameters groupParameters(int groupId) const;

  Parameters nonGroupParameters() const;

  ParameterGroups groupParameterGroups(int groupId) const;

  //--

  CQChartsPlotParameterGroup *startParameterGroup(const QString &name);
  void endParameterGroup();

  //---

  // type parameters
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

  CQChartsEnumParameter &
  addEnumParameter(const QString &name, const QString &desc, const QString &propName,
                   bool defValue=false);
  CQChartsEnumParameter &
  addEnumParameter(const QString &name, const QString &desc, const QString &propName,
                   const ParameterAttributes &attributes, bool defValue);

  CQChartsPlotParameter &
  addBoolParameter(const QString &name, const QString &desc, const QString &propName,
                   bool defValue=false);
  CQChartsPlotParameter &
  addBoolParameter(const QString &name, const QString &desc, const QString &propName,
                   const ParameterAttributes &attributes, bool defValue);

  CQChartsPlotParameter &addParameter(CQChartsPlotParameter *parameter);

  //---

  void addProperty(const QString &name, const QString &propName, const QString &desc);

  void propertyNames(QStringList &names) const;

  bool hasProperty(const QString &name) const;

  QVariant getPropertyValue(const QString &name) const;

  //---

  // is column suitable for parameter
  virtual bool isColumnForParameter(CQChartsModelColumnDetails *,
                                    CQChartsPlotParameter *) const { return true; }

  //---

  // create plot
  virtual CQChartsPlot *create(CQChartsView *view, const ModelP &model) const = 0;

 protected:
  struct PropertyData {
    QString name;
    QString propName;
    QString desc;

    PropertyData(const QString &name="", const QString &propName="", const QString &desc="") :
     name(name), propName(propName), desc(desc) {
    }
  };

  using Properties        = std::map<QString,PropertyData>;
  using ParameterGroupIds = std::vector<int>;

  Parameters        parameters_;        //!< parameters
  ParameterGroupMap parameterGroups_;   //!< parameter groups
  ParameterGroupIds parameterGroupIds_; //!< parameter group stack
  Properties        properties_;        //!< properties
};

#endif
