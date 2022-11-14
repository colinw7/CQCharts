#ifndef CQChartsConnectionPlot_H
#define CQChartsConnectionPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsConnectionList.h>
#include <CQChartsOptInt.h>
#include <CSafeIndex.h>

//---

/*!
 * \brief Connection Plot Type
 * \ingroup Charts
 */
class CQChartsConnectionPlotType : public CQChartsPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsConnectionPlotType();

  Category category() const override { return Category::CONNECTION; }

  void addParameters() override;

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  //---

  bool isColumnForParameter(ColumnDetails *columnDetails, Parameter *parameter) const override;

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;
};

//---

CQCHARTS_NAMED_SHAPE_DATA(Graph, graph)

/*!
 * \brief Connection Plot Base Class
 * \ingroup Charts
 *
 */
class CQChartsConnectionPlot : public CQChartsPlot {
  Q_OBJECT

  // columns
  //  hier :
  //    linkColumn, valueColumn
  //  link :
  //    groupColumn, linkColumn, valueColumn, nameColumn, depthColumn, attributesColumn
  //  connections :
  //    groupColumn, nodeColumn, connectionsColumn, nameColumn, attributesColumn
  //  path :
  //    pathColumn, valueColumn
  //  from/to :
  //    groupColumn, fromColumn, toColumn, valueColumn, depthColumn, attributesColumn,
  //    [pathIdColumn]
  //  table :
  //    groupColumn, linkColumn
  Q_PROPERTY(CQChartsColumn groupColumn READ groupColumn WRITE setGroupColumn)

  Q_PROPERTY(CQChartsColumn nodeColumn        READ nodeColumn        WRITE setNodeColumn       )
  Q_PROPERTY(CQChartsColumn connectionsColumn READ connectionsColumn WRITE setConnectionsColumn)

  Q_PROPERTY(CQChartsColumn linkColumn READ linkColumn WRITE setLinkColumn)

  Q_PROPERTY(CQChartsColumn pathColumn READ pathColumn WRITE setPathColumn)

  Q_PROPERTY(CQChartsColumn fromColumn READ fromColumn WRITE setFromColumn)
  Q_PROPERTY(CQChartsColumn toColumn   READ toColumn   WRITE setToColumn  )

  Q_PROPERTY(CQChartsColumn valueColumn READ valueColumn WRITE setValueColumn)
  Q_PROPERTY(CQChartsColumn depthColumn READ depthColumn WRITE setDepthColumn)
  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn  WRITE setNameColumn)

#ifdef CQCHARTS_GRAPH_PATH_ID
  Q_PROPERTY(CQChartsColumn pathIdColumn     READ pathIdColumn     WRITE setPathIdColumn    )
#endif
  Q_PROPERTY(CQChartsColumn attributesColumn READ attributesColumn WRITE setAttributesColumn)

  //---

  // link separator
  Q_PROPERTY(QString separator READ separator WRITE setSeparator)

  //---

  // options
  Q_PROPERTY(bool   symmetric READ isSymmetric WRITE setSymmetric)
  Q_PROPERTY(bool   sorted    READ isSorted    WRITE setSorted   )
  Q_PROPERTY(int    maxDepth  READ maxDepth    WRITE setMaxDepth )
  Q_PROPERTY(double minValue  READ minValue    WRITE setMinValue )
  Q_PROPERTY(bool   propagate READ isPropagate WRITE setPropagate)
  Q_PROPERTY(bool   hierName  READ isHierName  WRITE setHierName )

  //---

  Q_PROPERTY(ColumnDataType columnDataType READ calcColumnDataType WRITE setCalcColumnDataType)

  Q_ENUMS(ColumnDataType)

 public:
  enum ColumnDataType {
    HIER,
    LINK,
    CONNECTIONS,
    PATH,
    FROM_TO,
    TABLE
  };

  using ColumnArray = std::vector<Column>;
  using OptInt      = CQChartsOptInt;
  using Alpha       = CQChartsAlpha;

 public:
  CQChartsConnectionPlot(View *view, PlotType *plotType, const ModelP &model);
 ~CQChartsConnectionPlot();

  //---

  void init() override;
  void term() override;

  //---

  // get/set node column
  const Column &nodeColumn() const { return nodeColumn_; }
  void setNodeColumn(const Column &c);

  // get/set connections column
  const Column &connectionsColumn() const { return connectionsColumn_; }
  void setConnectionsColumn(const Column &c);

  //! get/set link column
  const Column &linkColumn() const { return linkColumn_; }
  void setLinkColumn(const Column &c);

  //! get/set path column
  const Column &pathColumn() const { return pathColumn_; }
  void setPathColumn(const Column &c);

  //! get/set from column
  const Column &fromColumn() const { return fromColumn_; }
  void setFromColumn(const Column &c);

  //! get/set to column
  const Column &toColumn() const { return toColumn_; }
  void setToColumn(const Column &c);

  //! get/set value column
  const Column &valueColumn() const { return valueColumn_; }
  void setValueColumn(const Column &c);

  //! get/set depth column
  const Column &depthColumn() const { return depthColumn_; }
  void setDepthColumn(const Column &c);

#ifdef CQCHARTS_GRAPH_PATH_ID
  //! get/set pathId column
  const Column &pathIdColumn() const { return pathIdColumn_; }
  void setPathIdColumn(const Column &c);
#endif

  //! get/set attributes column
  const Column &attributesColumn() const { return attributesColumn_; }
  void setAttributesColumn(const Column &c);

  //! get/set group column
  const Column &groupColumn() const { return groupColumn_; }
  void setGroupColumn(const Column &c);

  //! get/set name column
  const Column &nameColumn() const { return nameColumn_; }
  void setNameColumn(const Column &c);

  //---

  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  //---

  const ColumnArray &modelColumns() const { return modelColumns_; }

  //---

  //! get/set separator
  const QString &separator() const { return separator_; }
  void setSeparator(const QString &s);

  QString calcSeparator() const { return (separator().length() ? separator() : QString("/")); }

  //---

  //! get/set symmetric
  bool isSymmetric() const { return symmetric_; }
  void setSymmetric(bool b);

  //---

  //! get/set sorted
  bool isSorted() const { return sorted_; }
  void setSorted(bool b);

  //---

  //! get/set max depth
  int maxDepth() const { return maxDepth_; }
  void setMaxDepth(int d);

  //! get/set min value
  double minValue() const { return minValue_; }
  void setMinValue(double r);

  //---

  //! get/set propagate values
  bool isPropagate() const { return propagateData_.active; }
  void setPropagate(bool b);

  //---

  //! get/set name is hierarhical
  bool isHierName() const { return hierName_; }
  void setHierName(bool b);

  //---

  ColumnDataType calcColumnDataType() const;
  void setCalcColumnDataType(const ColumnDataType &columnDataType);

  //---

  const ColumnType &connectionsColumnType() const { return connectionsColumnType_; }

  const ColumnType &linkColumnType() const { return linkColumnType_; }

  //---

  //! add properties
  void addProperties() override;

  //---

  bool checkColumns() const;

  //---

  //! group data
  struct GroupData {
    QVariant value;
    int      ig { -1 };
    int      ng { 0 };

    GroupData() = default;

    GroupData(const QVariant &value, int ig, int ng) :
     value(value), ig(ig), ng(ng) {
    }

    bool isValid() const { return (ng > 0 && ig >= 0 && ig < ng); }

    double ivalue() const { return (isValid() ? double(ig)/ng : 0.0); }

    bool isNull() const { return value.isNull(); }
  };

  //---

  /*!
   * \brief Table Connection Data
   * \ingroup Charts
   */
  class TableConnectionData {
   public:
    enum class PrimaryType {
      ANY,
      PRIMARY,
      NON_PRIMARY
    };

    //! table value data
    struct Value {
      int         to      { -1 };
      OptReal     value;
      QModelIndex ind;
      bool        primary { true };

      Value() = default;

      Value(int to, const OptReal &value, const QModelIndex &ind, bool primary) :
       to(to), value(value), ind(ind), primary(primary) {
      }
    };

    using Values = std::vector<Value>;

   public:
    TableConnectionData() { }

    int from() const { return from_; }
    void setFrom(int i) { from_ = i; }

    const QString &name() const { return name_; }
    void setName(const QString &s) { name_ = s; }

    const QString &label() const { return label_; }
    void setLabel(const QString &s) { label_ = s; }

    const GroupData &group() const { return group_; }
    void setGroup(const GroupData &group) { group_ = group; }

    int depth() const { return depth_; }
    void setDepth(int depth) { depth_ = depth; }

    //---

    const Values &values() const { return values_; }

    const Value &ivalue(int i) const { return CUtil::safeIndex(values_, i); }

    void addValue(int to, const OptReal &value, bool primary) {
      addValue(to, value, QModelIndex(), primary);
    }

    void addValue(int to, const OptReal &value, const QModelIndex &ind, bool primary) {
      values_.emplace_back(to, value, ind, primary);

      totalValid_ = false;
    }

    bool hasTo(int to) const {
      for (const auto &v : values_) {
        if (v.to == to)
          return true;
      }

      return false;
    }

    void setToValue(int to, const OptReal &value) {
      for (auto &v : values_) {
        if (v.to == to) {
          v.value = value;
        }
      }
    }

    //---

    const QModelIndex &nameInd() const { return nameInd_; }
    void setNameInd(const QModelIndex &i) { nameInd_ = i; }

    const QModelIndex &groupInd() const { return groupInd_; }
    void setGroupInd(const QModelIndex &i) { groupInd_ = i; }

    double total(bool primaryOnly=false) const {
      if (totalValid_ && totalPrimary_ == primaryOnly)
        return total_;

      auto *th = const_cast<TableConnectionData *>(this);

      th->calcTotal(primaryOnly);

      return total_;
    }

    void calcTotal(bool primaryOnly) {
      total_ = 0.0;

      for (auto &value : values_) {
        if (primaryOnly && ! value.primary)
          continue;

        if (value.value.isSet())
          total_ += value.value.real();
      }

      totalValid_   = true;
      totalPrimary_ = primaryOnly;
    }

    void sort() {
      std::sort(values_.begin(), values_.end(),
        [](const Value &lhs, const Value &rhs) {
          return lhs.value.realOr(0.0) < rhs.value.realOr(0.0);
        });
    }

   protected:
    int         from_         { -1 };    //!< from node
    QString     name_;                   //!< value name
    QString     label_;                  //!< value label
    GroupData   group_;                  //!< group
    int         depth_        { 0 };     //!< depth
    Values      values_;                 //!< connection values
    QModelIndex nameInd_;                //!< name model index
    QModelIndex groupInd_;               //!< group model index
    bool        totalValid_   { false }; //!< is total valid
    bool        totalPrimary_ { false }; //!< is total for primary only
    double      total_        { 0.0 };   //!< value total
  };

  using TableConnectionDatas = std::vector<TableConnectionData>;

  //! table connection info
  struct TableConnectionInfo {
    int    numNonZero { 0 };
    double total      { 0.0 };
  };

 protected:
  //! hierarchical connection data
  struct HierConnectionData {
    ModelIndex  parentLinkInd;
    QStringList linkStrs;
    QString     parentStr;
    double      total      { 0.0 };
    double      childTotal { 0.0 };
  };

  using HierConnectionDataList = std::vector<HierConnectionData>;

  bool initHierObjs() const;

  virtual void initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                                             const HierConnectionData &destHierData) const = 0;
  virtual void initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                                             const HierConnectionData &destHierData) const = 0;

  //---

  using NameValues = CQChartsNameValues;

  //! link connection data
  //!  Columns are link (From/To Pair) and value define an edge and value (real)
  //!  Name column can be used for edge name
  //!  Depth column can be used for depth
  //!  Group column can be used to color group edges
  struct LinkConnectionData {
    ModelIndex linkModelInd;  //!< link model index
    QString    srcStr;        //!< source string from link
    QString    destStr;       //!< destination string from link
    ModelIndex valueModelInd; //!< value model index
    OptReal    value;         //!< optional value
    ModelIndex nameModelInd;  //!< name model index
    ModelIndex depthModelInd; //!< depth model index
    int        depth { -1 };  //!< depth
    NameValues nameValues;    //!< node/edge attributes
    ModelIndex groupModelInd; //!< group model index
    GroupData  groupData;     //!< group data
  };

  bool initLinkObjs() const;

  virtual void addLinkConnection(const LinkConnectionData &linkConnectionData) const = 0;

  //---

  using ConnectionList = CQChartsConnectionList;
  using Connections    = ConnectionList::Connections;
  using NodeIndex      = std::map<int, QModelIndex>;

  //! connections data
  //!  Columns are node (source node id) and connections (destination node id and value)
  //!  If no node column then row number is used.
  //!  name column is used for node name
  struct ConnectionsData {
    QModelIndex ind;                 //!< data model index (normalized)
    ModelIndex  nodeModelInd;        //!< node model index
    int         node    { -1 };      //!< node number
    ModelIndex  connectionsModelInd; //!< connections model index
    Connections connections;         //!< connection list
    ModelIndex  nameModelInd;        //!< name model index
    QString     name;                //!< name
    double      total   { 0.0 };     //!< sum of all connection values
    NameValues  nameValues;          //!< node attributes
    ModelIndex  groupModelInd;       //!< group model index
    GroupData   groupData;           //!< group data
  };

  using IdConnectionsData = std::map<int, ConnectionsData>;

  bool initConnectionObjs() const;

  virtual void addConnectionObj(int, const ConnectionsData &, const NodeIndex &) const { }

  //---

  //! path data
  //!  Columns are path (hierarchical edges) and value (to leaf)
  struct PathData {
    ModelIndex  pathModelInd;  //!< path model index
    QStringList pathStrs;      //!< path strings
    ModelIndex  valueModelInd; //!< value model index
    OptReal     value;         //!< optional value
  };

  bool initPathObjs() const;

  virtual void addPathValue(const PathData &) const { }

  //---

  //! from/to connection data
  //!  Columns are from and to (node names) and value
  //!  Depth column can be used for depth
  //!  Group column can be used to color group edges
  struct FromToData {
    ModelIndex fromModelInd;   //!< from model index
    QString    fromStr;        //!< from string
    ModelIndex toModelInd;     //!< to model index
    QString    toStr;          //!< to string
    ModelIndex valueModelInd;  //!< value model index
    OptReal    value;          //!< optional value
    ModelIndex depthModelInd;  //!< depth model index
    int        depth { -1 };   //!< source node depth
#ifdef CQCHARTS_GRAPH_PATH_ID
    ModelIndex pathIdModelInd; //!< pathId model index
    OptInt     pathId;         //!< optional path id
#endif
    NameValues nameValues;     //!< node/edge attributes
    GroupData  groupData;      //!< grouping data
  };

  bool initFromToObjs() const;

  virtual void addFromToValue(const FromToData &) const { }

  //---

  bool processTableModel(TableConnectionDatas &tableConnectionDatas,
                         TableConnectionInfo &tableConnectionInfo) const;

  //---

  bool groupColumnData(const ModelIndex &groupModelInd, GroupData &groupData) const;

 protected:
  //! value propagation data
  struct PropagateData {
    enum class Type {
      SUM,
      AVERAGE,
      MIN,
      MAX
    };

    bool active { true };
    Type type   { Type::SUM };
  };

  // columns
  Column     nodeColumn_;                                 //!< connection node column
  Column     connectionsColumn_;                          //!< connections column
  ColumnType connectionsColumnType_ { ColumnType::NONE }; //!< connection column type
  Column     linkColumn_;                                 //!< link column
  Column     pathColumn_;                                 //!< path column
  Column     fromColumn_;                                 //!< from column
  Column     toColumn_;                                   //!< to column
  Column     valueColumn_;                                //!< value column
  Column     depthColumn_;                                //!< depth column
#ifdef CQCHARTS_GRAPH_PATH_ID
  Column     pathIdColumn_;                               //!< pathId column
#endif
  Column     attributesColumn_;                           //!< attributes column
  Column     groupColumn_;                                //!< group column
  ColumnType linkColumnType_ { ColumnType::NONE };        //!< link column type
  Column     nameColumn_;                                 //!< name column

  ColumnDataType defaultColumnDataType_ { ColumnDataType::LINK };

  mutable ColumnArray modelColumns_; //!< used columns

  // options
  QString separator_;           //!< separator
  bool    symmetric_ { true };  //!< to/from values are symmetric (from/to auto implied)
  bool    sorted_    { false }; //!< is sorted
  int     maxDepth_  { -1 };    //!< user max depth
  double  minValue_  { -1 };    //!< user min value
  bool    hierName_  { false }; //!< name is hierarchical

  PropagateData propagateData_;
};

//------

#include <CQChartsPlotCustomControls.h>

class CQEnumCombo;

/*!
 * \brief Connection Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsConnectionPlotCustomControls : public CQChartsPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsConnectionPlotCustomControls(CQCharts *charts, const QString &plotType);

  void setPlot(Plot *plot) override;

  void addConnectionColumnWidgets();

 public Q_SLOTS:
  void updateWidgets() override;

 protected:
  void connectSlots(bool b) override;

 protected Q_SLOTS:
  void columnsTypeSlot();

 protected:
  CQChartsConnectionPlot* connectionPlot_   { nullptr };
  CQEnumCombo*            columnsTypeCombo_ { nullptr };
};

#endif
