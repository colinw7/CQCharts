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
  Q_PROPERTY(bool   exactBucketValue READ isExactBucketValue WRITE setExactBucketValue)
  Q_PROPERTY(bool   autoBucketRange  READ isAutoBucketRange  WRITE setAutoBucketRange )
  Q_PROPERTY(double startBucketValue READ startBucketValue   WRITE setStartBucketValue)
  Q_PROPERTY(double deltaBucketValue READ deltaBucketValue   WRITE setDeltaBucketValue)
  Q_PROPERTY(int    numAutoBuckets   READ numAutoBuckets     WRITE setNumAutoBuckets  )
  Q_PROPERTY(double minBucketValue   READ minBucketValue     WRITE setMinBucketValue  )
  Q_PROPERTY(double maxBucketValue   READ maxBucketValue     WRITE setMaxBucketValue  )
  Q_PROPERTY(bool   bucketUnderflow  READ isBucketUnderflow  WRITE setBucketUnderflow )
  Q_PROPERTY(bool   bucketOverflow   READ isBucketOverflow   WRITE setBucketOverflow  )

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

  //! get/set group column
  const Column &groupColumn() const { return groupColumn_; }
  virtual void setGroupColumn(const Column &c);

  //! get/set row grouping (groups use rwo values not column headers when multiple columns)
  bool isRowGrouping() const { return groupData_.rowGrouping; }
  void setRowGrouping(bool b);

  //! get/set use path for hierarchical names
  bool isUsePath() const { return groupData_.usePath; }
  void setUsePath(bool b);

  //! get/set use row number for group
  bool isUseRow() const { return groupData_.useRow; }
  void setUseRow(bool b);

  //---

  // bucketing

  //! get/set use exact (unique) value when grouping (not bucketing)
  bool isExactBucketValue() const { return groupData_.exactValue; }
  void setExactBucketValue(bool b);

  //! get/set use automatic bucket range (from number of auto buckets)
  bool isAutoBucketRange() const {
    return groupData_.bucketer.type() == CQBucketer::Type::REAL_AUTO; }
  void setAutoBucketRange(bool b);

  //! get/set start bucket value for explicit buckets
  double startBucketValue() const { return groupData_.bucketer.rstart(); }
  void setStartBucketValue(double r);

  //! get/set delta bucket value for explicit buckets
  double deltaBucketValue() const { return groupData_.bucketer.rdelta(); }
  void setDeltaBucketValue(double r);

  //! get/set number of (recommended) buckets for auto bucketing
  int numAutoBuckets() const { return groupData_.bucketer.numAuto(); }
  void setNumAutoBuckets(int i);

  //! get/set min bucketed value (for underflow)
  double minBucketValue() const { return groupData_.bucketer.rmin(); }
  void setMinBucketValue(double r);

  //! get/set max bucketed value (for overflow)
  double maxBucketValue() const { return groupData_.bucketer.rmax(); }
  void setMaxBucketValue(double r);

  //! get/set bucket has underflow bucket (values less than min)
  bool isBucketUnderflow() const { return groupData_.bucketer.isUnderflow(); }
  void setBucketUnderflow(bool b);

  //! get/set bucket has overflow bucket (values greater than max)
  bool isBucketOverflow() const { return groupData_.bucketer.isOverflow(); }
  void setBucketOverflow(bool b);

  //! get/set explicit bucket stop points (can be variable)
  const CQChartsReals &bucketStops() const { return groupData_.bucketStops; }
  void setBucketStops(const CQChartsReals &r);

  //! get/set bucket value type
  CQBucketer::Type bucketType() const;
  void setBucketType(const CQBucketer::Type &type);

  //---

  void addProperties() override;

  void addGroupingProperties();

  //---

  // group data
  void initGroupData(const Columns &dataColumns, const Column &nameColumn, bool hier=false) const;

  void initGroupBucketer();

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

  //---

  bool adjustedGroupColor(int ig, int ng, Color &color) const;

 signals:
  void groupCustomDataChanged();

 private:
  Bucket *initGroupData(const Columns &dataColumns, const Column &nameColumn,
                        bool hier, const GroupData &groupData) const;

  Bucket *initGroup(GroupData &data) const;

  bool rowGroupInds(const ModelIndex &ind, GroupInds &ids, bool hier) const;

  //---

  using PathInds = std::vector<int>;

  PathInds pathInds(const QString &path) const;

  QStringList pathStrs(const QString &path) const;

 private:
  Column             groupColumn_;             //!< group column
  GroupData          groupData_;               //!< grouping data
  Bucket*            groupBucket_ { nullptr }; //!< group column bucket
  mutable std::mutex mutex_;                   //!< mutex
};

#endif
