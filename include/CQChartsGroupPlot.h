#ifndef CQChartsGroupPlot_H
#define CQChartsGroupPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsColumnBucket.h>

//---

// box plot type
class CQChartsGroupPlotType : public CQChartsPlotType {
 public:
  CQChartsGroupPlotType();

  //---

  virtual bool isGroupRequired() const { return false; }

  virtual bool allowRowGrouping() const { return true; }

  virtual bool allowUsePath() const { return true; }

  virtual bool allowUseRow() const { return true; }

  //---

  void addParameters() override;
};

//---

class CQChartsGroupPlot : public CQChartsPlot {
  Q_OBJECT

  // grouping
  Q_PROPERTY(CQChartsColumn groupColumn READ groupColumn   WRITE setGroupColumn)
  Q_PROPERTY(bool           rowGrouping READ isRowGrouping WRITE setRowGrouping)
  Q_PROPERTY(bool           usePath     READ isUsePath     WRITE setUsePath    )
  Q_PROPERTY(bool           useRow      READ isUseRow      WRITE setUseRow     )
  Q_PROPERTY(bool           exactValue  READ isExactValue  WRITE setExactValue )
  Q_PROPERTY(bool           autoRange   READ isAutoRange   WRITE setAutoRange  )
  Q_PROPERTY(double         startValue  READ startValue    WRITE setStartValue )
  Q_PROPERTY(double         deltaValue  READ deltaValue    WRITE setDeltaValue )
  Q_PROPERTY(int            numAuto     READ numAuto       WRITE setNumAuto    )

 public:
  CQChartsGroupPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model);

 ~CQChartsGroupPlot();

  //---

  const CQChartsColumn &groupColumn() const { return groupColumn_; }

  virtual void setGroupColumn(const CQChartsColumn &c);

  //---

  bool isRowGrouping() const { return groupData_.rowGrouping; }
  void setRowGrouping(bool b) { groupData_.rowGrouping = b; updateRangeAndObjs(); }

  bool isUsePath() const { return groupData_.usePath; }
  void setUsePath(bool b) { groupData_.usePath = b; updateRangeAndObjs(); }

  bool isUseRow() const { return groupData_.useRow; }
  void setUseRow(bool b) { groupData_.useRow = b; updateRangeAndObjs(); }

  bool isExactValue() const { return groupData_.exactValue; }
  void setExactValue(bool b) { groupData_.exactValue = b; updateRangeAndObjs(); }

  bool isAutoRange() const {
    return groupData_.bucketer.type() == CQBucketer::Type::REAL_AUTO; }

  void setAutoRange(bool b) {
    groupData_.bucketer.setType(b ? CQBucketer::Type::REAL_AUTO :
                                    CQBucketer::Type::REAL_RANGE);
    updateRangeAndObjs();
  }

  double startValue() const { return groupData_.bucketer.rstart(); }
  void setStartValue(double r) { groupData_.bucketer.setRStart(r); updateRangeAndObjs(); }

  double deltaValue() const { return groupData_.bucketer.rdelta(); }
  void setDeltaValue(double r) { groupData_.bucketer.setRDelta(r); updateRangeAndObjs(); }

  int numAuto() const { return groupData_.bucketer.numAuto(); }
  void setNumAuto(int i) { groupData_.bucketer.setNumAuto(i); updateRangeAndObjs(); }

  //---

  void addProperties();

  //---

  void initGroupData(const Columns &dataColumns, const CQChartsColumn &nameColumn,
                     bool hier=false);

  void initGroup(const CQChartsGroupData &data);

  std::vector<int> rowHierGroupInds(const CQChartsModelIndex &ind) const;

  int rowGroupInd(const CQChartsModelIndex &ind) const;

  void getGroupInds(std::vector<int> &inds) const;

  QString groupIndName(int ind, bool hier=false) const;

  void setModelGroupInd(const CQChartsModelIndex &ind, int groupInd);

  void printGroup() const;

  const CQChartsColumnBucket &groupBucket() const { return groupBucket_; }

  bool isGroupHeaders() const {
    return (groupBucket().dataType() == CQChartsColumnBucket::DataType::HEADER);
  }

  bool isGroupPathType() const {
    return (groupBucket().dataType() == CQChartsColumnBucket::DataType::PATH);
  }

  int numGroups() const { return groupBucket_.numUnique(); }

 private:
  bool rowGroupInds(const CQChartsModelIndex &ind, std::vector<int> &ids, bool hier) const;

  std::vector<int> pathInds(const QString &path) const;

  QStringList pathStrs(const QString &path) const;

 protected: // TODO: make private
  CQChartsColumn       groupColumn_; // group column
  CQChartsGroupData    groupData_;   // grouping data
  CQChartsColumnBucket groupBucket_; // group value bucket
};

#endif
