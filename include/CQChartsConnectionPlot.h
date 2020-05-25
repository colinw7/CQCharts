#ifndef CQChartsConnectionPlot_H
#define CQChartsConnectionPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>

#include <CQPerfMonitor.h>

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

  void addParameters() override;

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  //---

  bool isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                            CQChartsPlotParameter *parameter) const override;

  void analyzeModel(CQChartsModelData *modelData,
                    CQChartsAnalyzeModelData &analyzeModelData) override;
};

//---

/*!
 * \brief Connection Plot Base Class
 * \ingroup Charts
 *
 */
class CQChartsConnectionPlot : public CQChartsPlot {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn nodeColumn        READ nodeColumn        WRITE setNodeColumn       )
  Q_PROPERTY(CQChartsColumn connectionsColumn READ connectionsColumn WRITE setConnectionsColumn)

  Q_PROPERTY(CQChartsColumn linkColumn  READ linkColumn  WRITE setLinkColumn )
  Q_PROPERTY(CQChartsColumn valueColumn READ valueColumn WRITE setValueColumn)

  Q_PROPERTY(CQChartsColumn groupColumn READ groupColumn WRITE setGroupColumn)
  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn  WRITE setNameColumn)

  // link separator
  Q_PROPERTY(QString separator READ separator WRITE setSeparator)

  // options
  Q_PROPERTY(bool symmetric READ isSymmetric WRITE setSymmetric)
  Q_PROPERTY(bool sorted    READ isSorted    WRITE setSorted   )

 public:
  CQChartsConnectionPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model);

  virtual ~CQChartsConnectionPlot();

  //---

  // get/set node column
  const CQChartsColumn &nodeColumn() const { return nodeColumn_; }
  void setNodeColumn(const CQChartsColumn &c);

  // get/set connections column
  const CQChartsColumn &connectionsColumn() const { return connectionsColumn_; }
  void setConnectionsColumn(const CQChartsColumn &c);

  //! get/set link column
  const CQChartsColumn &linkColumn() const { return linkColumn_; }
  void setLinkColumn(const CQChartsColumn &c);

  //! get/set value column
  const CQChartsColumn &valueColumn() const { return valueColumn_; }
  void setValueColumn(const CQChartsColumn &c);

  //! get/set group column
  const CQChartsColumn &groupColumn() const { return groupColumn_; }
  void setGroupColumn(const CQChartsColumn &c);

  //! get/set name column
  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  //---

  //! get/set separator
  const QString &separator() const { return separator_; }
  void setSeparator(const QString &s) { separator_ = s; }

  //! get/set symmetric
  bool isSymmetric() const { return symmetric_; }
  void setSymmetric(bool b);

  //---

  //! get/set sorted
  bool isSorted() const { return sorted_; }
  void setSorted(bool b);

  //---

  const ColumnType &connectionsColumnType() const { return connectionsColumnType_; }

  const ColumnType &linkColumnType() const { return linkColumnType_; }

  //---

  //! add properties
  void addProperties();

  //---

  bool checkColumns() const;

  //---

  struct HierConnectionData {
    CQChartsModelIndex parentLinkInd;
    QStringList        linkStrs;
    QString            parentStr;
    double             total      { 0.0 };
    double             childTotal { 0.0 };
  };

  using HierConnectionDataList = std::vector<HierConnectionData>;

  class TableConnectionData {
   public:
    enum class PrimaryType {
      ANY,
      PRIMARY,
      NON_PRIMARY
    };

    struct Value {
      int    to        { -1 };
      double fromValue { 0.0 };
      double toValue   { 0.0 };
      bool   primary   { true };

      Value() = default;

      Value(int to, double fromValue, double toValue, bool primary) :
       to(to), fromValue(fromValue), toValue(toValue), primary(primary) {
      }
    };

    struct Group {
      QString str;
      int     i { 0 };
      int     n { 0 };

      Group() = default;

      Group(const QString &str, int i, int n) :
       str(str), i(i), n(n) {
      }

      Group(const Group &group) = default;

      bool isValid() const { return n > 0; }

      double value() const { return (isValid() ? double(i)/n : 0.0); }
    };

    using Values = std::vector<Value>;

   public:
    TableConnectionData() { }

    int from() const { return from_; }
    void setFrom(int i) { from_ = i; }

    const QString &name() const { return name_; }
    void setName(const QString &s) { name_ = s; }

    const Group &group() const { return group_; }
    void setGroup(const Group &group) { group_ = group; }

    const Values &values() const { return values_; }

    void addValue(int to, double fromValue, double toValue, bool primary=true) {
      values_.emplace_back(to, fromValue, toValue, primary);

      totalValid_ = false;
    }

    const QModelIndex &linkInd() const { return linkInd_; }
    void setLinkInd(const QModelIndex &i) { linkInd_ = i; }

    const QModelIndex &groupInd() const { return groupInd_; }
    void setGroupInd(const QModelIndex &i) { groupInd_ = i; }

    double toTotal(bool primaryOnly=false) const {
      if (totalValid_ && totalPrimary_ == primaryOnly)
        return toTotal_;

      auto *th = const_cast<TableConnectionData *>(this);

      th->calcTotal(primaryOnly);

      return toTotal_;
    }

    double fromTotal(bool primaryOnly=false) const {
      if (totalValid_ && totalPrimary_ == primaryOnly)
        return fromTotal_;

      auto *th = const_cast<TableConnectionData *>(this);

      th->calcTotal(primaryOnly);

      return fromTotal_;
    }

    void calcTotal(bool primaryOnly) {
      fromTotal_ = 0.0;
      toTotal_   = 0.0;

      for (auto &value : values_) {
        if (primaryOnly && ! value.primary)
          continue;

        fromTotal_ += value.fromValue;
        toTotal_   += value.toValue;
      }

      totalValid_   = true;
      totalPrimary_ = primaryOnly;
    }

    void sort() {
      std::sort(values_.begin(), values_.end(),
        [](const Value &lhs, const Value &rhs) {
          return lhs.toValue < rhs.toValue;
        });
    }

   protected:
    int           from_         { -1 };    //!< from node
    QString       name_;                   //!< value name
    Group         group_;                  //!< group
    Values        values_;                 //!< connection values
    QModelIndex   linkInd_;                //!< link model index
    QModelIndex   groupInd_;               //!< group model index
    double        totalValid_   { false }; //!< is total valid
    bool          totalPrimary_ { false }; //!< is total for primary only
    double        fromTotal_    { 0.0 };   //!< from value total
    double        toTotal_      { 0.0 };   //!< to value total
  };

  using TableConnectionDatas = std::vector<TableConnectionData>;

  struct TableConnectionInfo {
    int    numNonZero { 0 };
    double total      { 0.0 };
  };

 protected:
  bool initHierObjs() const;

  virtual void initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                                             const HierConnectionData &destHierData) const = 0;
  virtual void initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                                             const HierConnectionData &destHierData) const = 0;

  //---

  bool processTableModel(TableConnectionDatas &tableConnectionDatas,
                         TableConnectionInfo &tableConnectionInfo) const;

 protected:
  // columns
  CQChartsColumn nodeColumn_;                                 //!< connection node column
  CQChartsColumn connectionsColumn_;                          //!< connections column
  ColumnType     connectionsColumnType_ { ColumnType::NONE }; //!< connection column type
  CQChartsColumn linkColumn_;                                 //!< link column
  CQChartsColumn valueColumn_;                                //!< value column
  CQChartsColumn groupColumn_;                                //!< group column
  ColumnType     linkColumnType_ { ColumnType::NONE };        //!< link column type
  CQChartsColumn nameColumn_;                                 //!< name column

  // options
  QString separator_;           //!< separator
  bool    symmetric_ { true };  //!< to/from values are symmetric (from/to auto implied)
  bool    sorted_    { false }; //!< is sorted
};

#endif
