#ifndef CQChartsGroupPlot_H
#define CQChartsGroupPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <future>

class CQChartsColumnBucket;

//---

/*!
 * \brief Group Plot Type base class
 * \ingroup Charts
 */
class CQChartsGroupPlotType : public CQChartsPlotType {
 public:
  CQChartsGroupPlotType();

  //---

  bool isGroupType() const override { return true; }

  virtual bool isGroupRequired() const { return false; }

  virtual bool allowRowGrouping() const { return true; }

  virtual bool allowUsePath() const { return true; }

  virtual bool allowUseRow() const { return true; }

  //---

  void addParameters() override;
};

//---

/*!
 * \brief Group Plot base class
 * \ingroup Charts
 */
class CQChartsGroupPlot : public CQChartsPlot {
  Q_OBJECT

  // grouping
  Q_PROPERTY(CQChartsColumn groupColumn READ groupColumn   WRITE setGroupColumn)
  Q_PROPERTY(bool           rowGrouping READ isRowGrouping WRITE setRowGrouping)
  Q_PROPERTY(bool           usePath     READ isUsePath     WRITE setUsePath    )
  Q_PROPERTY(bool           useRow      READ isUseRow      WRITE setUseRow     )

  // bucketing
  Q_PROPERTY(bool   exactValue READ isExactValue WRITE setExactValue)
  Q_PROPERTY(bool   autoRange  READ isAutoRange  WRITE setAutoRange )
  Q_PROPERTY(double startValue READ startValue   WRITE setStartValue)
  Q_PROPERTY(double deltaValue READ deltaValue   WRITE setDeltaValue)
  Q_PROPERTY(int    numAuto    READ numAuto      WRITE setNumAuto   )

 public:
  using Bucket    = CQChartsColumnBucket;
  using GroupData = CQChartsGroupData;

 public:
  CQChartsGroupPlot(View *view, PlotType *plotType, const ModelP &model);
 ~CQChartsGroupPlot();

  //---

  void init() override;
  void term() override;

  //---

  // grouping
  const Column &groupColumn() const { return groupColumn_; }

  virtual void setGroupColumn(const Column &c);

  bool isRowGrouping() const { return groupData_.rowGrouping; }
  void setRowGrouping(bool b);

  bool isUsePath() const { return groupData_.usePath; }
  void setUsePath(bool b);

  bool isUseRow() const { return groupData_.useRow; }
  void setUseRow(bool b);

  //---

  // bucketing
  bool isExactValue() const { return groupData_.exactValue; }
  void setExactValue(bool b);

  bool isAutoRange() const { return groupData_.bucketer.type() == CQBucketer::Type::REAL_AUTO; }

  void setAutoRange(bool b);

  double startValue() const { return groupData_.bucketer.rstart(); }
  void setStartValue(double r);

  double deltaValue() const { return groupData_.bucketer.rdelta(); }
  void setDeltaValue(double r);

  int numAuto() const { return groupData_.bucketer.numAuto(); }
  void setNumAuto(int i);

  //---

  void addProperties() override;

  void addGroupingProperties();

  //---

  // group data
  void initGroupData(const Columns &dataColumns, const Column &nameColumn, bool hier=false) const;

  using GroupInds = std::vector<int>;

  GroupInds rowHierGroupInds(const ModelIndex &ind) const;

  int rowGroupInd(const ModelIndex &ind) const;

  bool getGroupInds(GroupInds &inds) const;

  QString groupIndName(int ind, bool hier=false) const;

  ColumnType groupType() const;

  QVariant groupIndValue(int ind) const;

  void setModelGroupInd(const ModelIndex &ind, int groupInd);

  const Bucket *groupBucket() const { return groupBucket_; }
  void setGroupBucket(Bucket *bucket);

  bool isGroupHeaders () const;
  bool isGroupPathType() const;

  int numGroups() const override;

  void printGroup() const;

 private:
  Bucket *initGroupData(const Columns &dataColumns, const Column &nameColumn,
                        bool hier, const GroupData &groupData) const;

  Bucket *initGroup(GroupData &data) const;

  bool rowGroupInds(const ModelIndex &ind, GroupInds &ids, bool hier) const;

  //---

  using PathInds = std::vector<int>;

  PathInds pathInds(const QString &path) const;

  QStringList pathStrs(const QString &path) const;

 protected: // TODO: make private
  Column             groupColumn_;             //!< group column
  GroupData          groupData_;               //!< grouping data
  Bucket*            groupBucket_ { nullptr }; //!< group column bucket
  mutable std::mutex mutex_;                   //!< mutex
};

//------

class CQChartsGroupPlotCustomControls : public CQChartsPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsGroupPlotCustomControls(CQCharts *charts, const QString &plotType);

  void setPlot(CQChartsPlot *plot) override;

  void addGroupColumnWidgets();

 public slots:
  void updateWidgets() override;

 protected slots:
  void groupColumnSlot();

 private:
  CQChartsGroupPlot*   plot_             { nullptr };
  CQChartsColumnCombo* groupColumnCombo_ { nullptr };
};

#endif
