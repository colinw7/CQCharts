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
  CQChartsGroupPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model);

 ~CQChartsGroupPlot();

  //---

  // grouping
  const CQChartsColumn &groupColumn() const { return groupColumn_; }

  virtual void setGroupColumn(const CQChartsColumn &c);

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

  //---

  void initGroupData(const CQChartsColumns &dataColumns, const CQChartsColumn &nameColumn,
                     bool hier=false) const;

  std::vector<int> rowHierGroupInds(const CQChartsModelIndex &ind) const;

  int rowGroupInd(const CQChartsModelIndex &ind) const;

  void getGroupInds(std::vector<int> &inds) const;

  QString groupIndName(int ind, bool hier=false) const;

  void setModelGroupInd(const CQChartsModelIndex &ind, int groupInd);

  const CQChartsColumnBucket *groupBucket() const { return groupBucket_; }
  void setGroupBucket(CQChartsColumnBucket *bucket);

  bool isGroupHeaders () const;
  bool isGroupPathType() const;

  int numGroups() const override;

  void printGroup() const;

 private:
  CQChartsColumnBucket *initGroupData(const CQChartsColumns &dataColumns,
                                      const CQChartsColumn &nameColumn, bool hier,
                                      const CQChartsGroupData &groupData) const;

  CQChartsColumnBucket *initGroup(CQChartsGroupData &data) const;

  bool rowGroupInds(const CQChartsModelIndex &ind, std::vector<int> &ids, bool hier) const;

  std::vector<int> pathInds(const QString &path) const;

  QStringList pathStrs(const QString &path) const;

 protected: // TODO: make private
  CQChartsColumn        groupColumn_;             //!< group column
  CQChartsGroupData     groupData_;               //!< grouping data
  CQChartsColumnBucket* groupBucket_ { nullptr }; //!< group column bucket
  mutable std::mutex    mutex_;                   //!< mutex
};

#endif
