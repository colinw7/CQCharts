#include <CQChartsGroupPlot.h>
#include <CQChartsColumnBucket.h>
#include <CQChartsModelUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>

#include <CQPropertyViewItem.h>
#include <CQModelUtil.h>
#include <CQPerfMonitor.h>

CQChartsGroupPlotType::
CQChartsGroupPlotType() :
 CQChartsPlotType()
{
}

void
CQChartsGroupPlotType::
addParameters()
{
  startParameterGroup("Grouping");

  if (isGroupRequired())
    addColumnParameter("group", "Group", "groupColumn").setRequired().
     setGroupable().setBasic().setPropPath("dataGrouping.group").setTip("Group column");
  else
    addColumnParameter("group", "Group", "groupColumn").
     setGroupable().setBasic().setPropPath("dataGrouping.group").setTip("Group column");

  if (allowRowGrouping())
    addBoolParameter("rowGrouping", "Row Grouping", "rowGrouping").
      setTip("Group by rows instead of column headers");

  if (allowUsePath())
    addBoolParameter("usePath", "Use Path", "usePath", true).
      setTip("Use hierarchical path as group");

  if (allowUseRow())
    addBoolParameter("useRow", "Use Row", "useRow").
      setTip("Use row number for group");

  addBoolParameter("exactValue", "Exact Value", "exactBucketValue", true).
   setTip("Use exact value for grouping");

  addBoolParameter("autoRange", "Auto Range", "autoBucketRange", true).
   setTip("Automatically determine value range");

  addRealParameter("start", "Start", "startBucketValue", 0.0).
    setRequired().setTip("Start value for manual range");
  addRealParameter("delta", "Delta", "deltaBucketValue", 1.0).
    setRequired().setTip("Delta value for manual range");

  addIntParameter("numAuto", "Num Auto", "numAutoBuckets", 10).
    setRequired().setTip("Number of auto groups");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

//---

CQChartsGroupPlot::
CQChartsGroupPlot(View *view, PlotType *plotType, const ModelP &model) :
 CQChartsPlot(view, plotType, model)
{
}

CQChartsGroupPlot::
~CQChartsGroupPlot()
{
  CQChartsGroupPlot::term();
}

//---

void
CQChartsGroupPlot::
init()
{
  CQChartsPlot::init();
}

void
CQChartsGroupPlot::
term()
{
  delete groupBucket_;
}

//---

void
CQChartsGroupPlot::
setGroupColumn(const Column &c)
{
  CQChartsUtil::testAndSet(groupColumn_, c, [&]() {
    resetSetHidden(); updateRangeAndObjs(); Q_EMIT groupCustomDataChanged();
  } );
}

//---

void
CQChartsGroupPlot::
setRowGrouping(bool b)
{
  CQChartsUtil::testAndSet(groupData_.rowGrouping, b, [&]() {
    updateRangeAndObjs(); Q_EMIT groupCustomDataChanged();
  } );
}

void
CQChartsGroupPlot::
setUsePath(bool b)
{
  CQChartsUtil::testAndSet(groupData_.usePath, b, [&]() {
    updateRangeAndObjs(); Q_EMIT groupCustomDataChanged();
  } );
}

void
CQChartsGroupPlot::
setUseRow(bool b)
{
  CQChartsUtil::testAndSet(groupData_.useRow, b, [&]() {
    updateRangeAndObjs(); Q_EMIT groupCustomDataChanged();
  } );
}

//---

void
CQChartsGroupPlot::
setExactBucketValue(bool b)
{
  CQChartsUtil::testAndSet(groupData_.exactValue, b, [&]() {
    updateRangeAndObjs(); Q_EMIT groupCustomDataChanged();
  } );
}

void
CQChartsGroupPlot::
setAutoBucketRange(bool b)
{
  setBucketType(b ? CQBucketer::Type::REAL_AUTO : CQBucketer::Type::REAL_RANGE);
}

void
CQChartsGroupPlot::
setStartBucketValue(double r)
{
  if (r != groupData_.bucketer.rstart()) {
    groupData_.bucketer.setRStart(r); updateRangeAndObjs(); Q_EMIT groupCustomDataChanged();
  }
}

void
CQChartsGroupPlot::
setDeltaBucketValue(double r)
{
  if (r != groupData_.bucketer.rdelta()) {
    groupData_.bucketer.setRDelta(r); updateRangeAndObjs(); Q_EMIT groupCustomDataChanged();
  }
}

void
CQChartsGroupPlot::
setNumAutoBuckets(int i)
{
  if (i != groupData_.bucketer.numAuto()) {
    groupData_.bucketer.setNumAuto(i); updateRangeAndObjs(); Q_EMIT groupCustomDataChanged();
  }
}

void
CQChartsGroupPlot::
setMinBucketValue(double r)
{
  if (r != groupData_.bucketer.rmin()) {
    groupData_.bucketer.setRMin(r); updateRangeAndObjs(); Q_EMIT groupCustomDataChanged();
  }
}

void
CQChartsGroupPlot::
setMaxBucketValue(double r)
{
  if (r != groupData_.bucketer.rmax()) {
    groupData_.bucketer.setRMax(r); updateRangeAndObjs(); Q_EMIT groupCustomDataChanged();
  }
}

void
CQChartsGroupPlot::
setBucketUnderflow(bool b)
{
  if (b != groupData_.bucketer.isUnderflow()) {
    groupData_.bucketer.setUnderflow(b); updateRangeAndObjs(); Q_EMIT groupCustomDataChanged();
  }
}

void
CQChartsGroupPlot::
setBucketOverflow(bool b)
{
  if (b != groupData_.bucketer.isOverflow()) {
    groupData_.bucketer.setOverflow(b); updateRangeAndObjs(); Q_EMIT groupCustomDataChanged();
  }
}

void
CQChartsGroupPlot::
setBucketStops(const CQChartsReals &r)
{
  if (r != groupData_.bucketStops) {
    groupData_.bucketStops = r;

    CQBucketer::RStops rstops;

    for (const auto &r1 : groupData_.bucketStops.reals())
      rstops.insert(r1);

    groupData_.bucketer.setRStops(rstops); updateRangeAndObjs(); Q_EMIT groupCustomDataChanged();
  }
}

CQBucketer::Type
CQChartsGroupPlot::
bucketType() const
{
  return groupData_.bucketer.type();
}

void
CQChartsGroupPlot::
setBucketType(const CQBucketer::Type &type)
{
  if (type != groupData_.bucketer.type()) {
    groupData_.bucketer.setType(type); updateRangeAndObjs(); Q_EMIT groupCustomDataChanged();
  }
}

//---

void
CQChartsGroupPlot::
addProperties()
{
  addGroupingProperties();
}

void
CQChartsGroupPlot::
addGroupingProperties()
{
  auto *type = dynamic_cast<CQChartsGroupPlotType *>(this->type());
  assert(type);

  addProp("dataGrouping", "groupColumn", "group", "Group column");

  if (type->allowRowGrouping())
    addProp("dataGrouping", "rowGrouping", "rowGroups", "Group by rows instead of column headers");

  if (type->allowUsePath())
    addProp("dataGrouping", "usePath", "path", "Use path for group");

  if (type->allowUseRow())
    addProp("dataGrouping", "useRow", "row", "Use row number for grouping");

  addProp("dataGrouping/bucket", "exactBucketValue", "exact"   , "Use exact value");
  addProp("dataGrouping/bucket", "autoBucketRange" , "auto"    , "Bucket auto range");
  addProp("dataGrouping/bucket", "startBucketValue", "start"   , "Bucket start value");
  addProp("dataGrouping/bucket", "deltaBucketValue", "delta"   , "Bucket delta value");
  addProp("dataGrouping/bucket", "numAutoBuckets"  , "numAuto" , "Number of automatic buckets");
  addProp("dataGrouping/bucket", "minBucketValue"  , "minValue", "Min bucket value");
  addProp("dataGrouping/bucket", "maxBucketValue"  , "maxValue", "Max bucket value");
}

void
CQChartsGroupPlot::
setGroupBucket(Bucket *groupBucket)
{
  delete groupBucket_;

  groupBucket_ = groupBucket;
}

void
CQChartsGroupPlot::
initGroupData(const Columns &dataColumns, const Column &nameColumn, bool hier) const
{
  CQPerfTrace trace("CQChartsGroupPlot::initGroupData");

  // given columns and current grouping settings cache group buckets
  auto *groupBucket = initGroupData(dataColumns, nameColumn, hier, groupData_);

  const_cast<CQChartsGroupPlot *>(this)->setGroupBucket(groupBucket);
}

CQChartsColumnBucket *
CQChartsGroupPlot::
initGroupData(const Columns &dataColumns, const Column &nameColumn, bool hier,
              const CQChartsGroupData &groupData) const
{
  // if multiple data columns then use name column and data labels
  //   if row grouping we are creating a value set per row (1 value per data column)
  //   if column grouping we are creating a value set per data column (1 value per row)
  // if group column defined use that
  // otherwise (single data column) just use name column (if any)
  CQChartsGroupData groupData1;

  groupData1.exactValue = groupData.exactValue;
  groupData1.bucketer   = groupData.bucketer;
  groupData1.usePath    = false;
  groupData1.hier       = hier;

  // use multiple group columns
  if      (dataColumns.count() > 1) {
    groupData1.columns     = dataColumns;
    groupData1.rowGrouping = groupData.rowGrouping; // only used for multiple columns

    if      (groupColumn().isValid())
      groupData1.column = groupColumn();
    else if (nameColumn.isValid())
      groupData1.column = nameColumn;
  }
  // use single group column
  else if (groupColumn().isValid()) {
    groupData1.column = groupColumn();
  }
  // use path and hierarchical
  else if (groupData.usePath && isHierarchical()) {
    groupData1.usePath = true;
  }
  // use row
  else if (groupData.useRow) {
    groupData1.useRow = true;
  }
#if 0
  // default use name column if defined (?? enables grouping when no grouping wanted)
  else if (nameColumn.isValid()) {
    groupData1.column = nameColumn;
  }
#endif

  return initGroup(groupData1);
}

//---

// init group buckets depending on:
//  group column
//  multiple value columns
//  row grouping
CQChartsColumnBucket *
CQChartsGroupPlot::
initGroup(CQChartsGroupData &data) const
{
  auto *columnBucket = new Bucket;

  //---

  // when not row grouping we use the column header as the grouping id so all row
  // values in the column are added to the group
  if (data.columns.count() > 1 && ! data.rowGrouping) {
    columnBucket->setDataType   (Bucket::DataType::HEADER);
    columnBucket->setColumnType (ColumnType::INTEGER);
    columnBucket->setExactValue (true);
    columnBucket->setRowGrouping(false);

    for (const auto &column : data.columns) {
      bool ok;

      auto name = modelHHeaderString(column, ok);

      if (! name.length())
        name = QString::number(column.column());

      int ind = columnBucket->addValue(column.column());

      columnBucket->setIndName(ind, name);
    }

    return columnBucket;
  }

  //---

  bool initRange = false;

  // for specified grouping columns, set column and column type
  if      (data.columns.count() > 1) {
    Column column;

    if (data.column.isValid())
      column = data.column;

    if (! data.column.isValid()) {
      // TODO: combine multiple column values
      column = data.columns.column();
      assert(column.isValid());
    }

    ColumnType columnType = ColumnType::STRING;

    if (column.type() == Column::Type::DATA ||
        column.type() == Column::Type::DATA_INDEX)
      columnType = columnValueType(column);

    if (isHierarchical())
      columnBucket->setDataType(Bucket::DataType::COLUMN_ROOT);
    else
      columnBucket->setDataType(Bucket::DataType::COLUMN);

    columnBucket->setColumnType(columnType);
    columnBucket->setColumn    (column);
  }
  // for specified grouping column, set column and column type
  else if (data.column.isValid()) {
    ColumnType columnType = ColumnType::STRING;

    if (data.column.type() == Column::Type::DATA ||
        data.column.type() == Column::Type::DATA_INDEX) {
      columnType = columnValueType(data.column);

      if (columnType == ColumnType::REAL || columnType == ColumnType::INTEGER) {
        if      (data.bucketer.type() == CQBucketer::Type::STRING) {
          if (! data.exactValue)
            initRange = true;
        }
        else if (columnType == ColumnType::INTEGER &&
                 data.bucketer.type() == CQBucketer::Type::REAL_AUTO) {
          if (! data.exactValue) {
            auto *details = columnDetails(data.column);

            if (details && details->numUnique() < bucketMaxExact())
              data.exactValue = true;
          }
        }
      }
    }

    if (isHierarchical())
      columnBucket->setDataType(Bucket::DataType::COLUMN_ROOT);
    else
      columnBucket->setDataType(Bucket::DataType::COLUMN);

    columnBucket->setColumnType(columnType);
    columnBucket->setColumn    (data.column);
  }
  // no group column then use parent path (hierarchical)
  else if (data.usePath) {
    assert(isHierarchical());

    columnBucket->setDataType  (Bucket::DataType::PATH);
    columnBucket->setColumnType(ColumnType::STRING);
  }
  else {
    columnBucket->setDataType  (Bucket::DataType::NONE);
    columnBucket->setColumnType(data.useRow ? ColumnType::INTEGER : ColumnType::STRING);
  }

  columnBucket->setUseRow    (data.useRow);
  columnBucket->setExactValue(data.exactValue);
  columnBucket->setBucketer  (data.bucketer);

  //---

  // process model data
  class GroupVisitor : public ModelVisitor {
   public:
    GroupVisitor(const CQChartsGroupPlot *groupPlot, Bucket *bucket, bool hier) :
     groupPlot_(groupPlot), bucket_(bucket), hier_(hier) {
    }

    State visit(const QAbstractItemModel *model, const VisitData &data) override {
      // add column value
      if      (bucket_->dataType() == Bucket::DataType::COLUMN ||
               bucket_->dataType() == Bucket::DataType::COLUMN_ROOT) {
        ModelIndex rootInd(groupPlot_, data.row, bucket_->column(), data.parent);

        bool ok;

        QVariant value;

        if (bucket_->dataType() == Bucket::DataType::COLUMN_ROOT)
          value = groupPlot_->modelRootValue(rootInd, ok);
        else
          value = groupPlot_->modelHierValue(rootInd, ok);

        if (value.isValid())
          bucket_->addValue(value);

        if (hier_) {
          auto paths = groupPlot_->pathStrs(value.toString());

          for (int i = 0; i < paths.length(); ++i)
            bucket_->addValue(paths[i]);
        }
      }
      // add parent path (hierarchical)
      else if (bucket_->dataType() == Bucket::DataType::PATH) {
        auto path = CQChartsModelUtil::parentPath(model, data.parent);

        bucket_->addString(path);
      }
      // use row number
      else if (bucket_->isUseRow()) {
        bucket_->addValue(data.row); // default to row
      }
      else {
        bucket_->addString(""); // no bucket
      }

      return State::OK;
    }

   private:
    const CQChartsGroupPlot* groupPlot_ { nullptr };
    Bucket*                  bucket_    { nullptr };
    bool                     hier_      { false };
  };

  GroupVisitor groupVisitor(this, columnBucket, data.hier);

  visitModel(groupVisitor);

  //---

  if (initRange) {
    auto *th = const_cast<CQChartsGroupPlot *>(this);

    th->initGroupBucketer();
  }

  //---

  return columnBucket;
}

//---

void
CQChartsGroupPlot::
initGroupBucketer()
{
  // update default grouping from group column details
  auto *details = (groupColumn().isValid() ? columnDetails(groupColumn()) : nullptr);
  if (! details) return;

  setUpdatesEnabled(false);

  if (details->isNumeric()) {
    auto minValue = details->minValue();
    auto maxValue = details->maxValue();

    bool ok;

    setExactBucketValue(false);
    setBucketType      (CQBucketer::Type::REAL_AUTO);
    setMinBucketValue  (CQChartsVariant::toReal(minValue, ok));
    setMaxBucketValue  (CQChartsVariant::toReal(maxValue, ok));

    setBucketUnderflow(true);
    setBucketOverflow (true);
  }
  else {
    setExactBucketValue(false);
    setBucketType      (CQBucketer::Type::STRING);
  }

  setUpdatesEnabled(true);
}

//---

std::vector<int>
CQChartsGroupPlot::
rowHierGroupInds(const ModelIndex &ind) const
{
  std::vector<int> inds;

  if (! rowGroupInds(ind, inds, /*hier*/true))
    return inds;

  return inds;
}

int
CQChartsGroupPlot::
rowGroupInd(const ModelIndex &ind) const
{
  std::vector<int> inds;

  if (! rowGroupInds(ind, inds, /*hier*/false))
    return -1;

  assert(inds.size() == 1);

  int groupInd = inds[0];

  auto *th = const_cast<CQChartsGroupPlot *>(this);

  th->setModelGroupInd(ind, groupInd);

  return groupInd;
}

bool
CQChartsGroupPlot::
rowGroupInds(const ModelIndex &ind, std::vector<int> &inds, bool hier) const
{
  auto *model = this->currentModel().data();
  if (! model) return false;

  if (! groupBucket_)
    return false;

  // header has multiple groups (one per column)
  if      (groupBucket_->dataType() == Bucket::DataType::HEADER) {
    int ind1 = groupBucket_->ind(ind.column().column());

    inds.push_back(ind1);
  }
  // get group id from value in group column
  else if (groupBucket_->dataType() == Bucket::DataType::COLUMN ||
           groupBucket_->dataType() == Bucket::DataType::COLUMN_ROOT) {
    ModelIndex rootInd(this, ind.row(), groupBucket_->column(), ind.parent());

    bool ok;

    QVariant value;

    if (groupBucket_->dataType() == Bucket::DataType::COLUMN_ROOT)
      value = modelRootValue(rootInd, ok);
    else
      value = modelHierValue(rootInd, ok);

    if (! value.isValid())
      return false;

    int ind1 = -1;

    if      (groupBucket_->isExactValue()) {
      ind1 = groupBucket_->ind(value);

      inds.push_back(ind1);
    }
    else if (CQChartsVariant::isNumeric(value)) {
      ind1 = groupBucket_->bucket(value);

      inds.push_back(ind1);
    }
    else {
      if (hier) {
        inds = pathInds(value.toString());
      }
      else {
        ind1 = groupBucket_->sbucket(value);

        inds.push_back(ind1);
      }
    }
  }
  // get group id from parent path name
  else if (groupBucket_->dataType() == Bucket::DataType::PATH) {
    auto path = CQChartsModelUtil::parentPath(model, ind.parent());

    if (hier) {
      inds = pathInds(path);
    }
    else {
      int ind1 = groupBucket_->ind(path);

      inds.push_back(ind1);
    }
  }
  else if (groupBucket_->isUseRow()) {
    int ind1 = ind.row(); // default to row

    inds.push_back(ind1);
  }
  else {
    return false; // no bucket
  }

  return true;
}

std::vector<int>
CQChartsGroupPlot::
pathInds(const QString &path) const
{
  std::vector<int> inds;

  if (! groupBucket_)
    return inds;

  auto paths = pathStrs(path);

  for (int i = 0; i < paths.length(); ++i) {
    const auto &path1 = paths[i];

    int ind = groupBucket_->ind(path1);

    inds.push_back(ind);
  }

  return inds;
}

QStringList
CQChartsGroupPlot::
pathStrs(const QString &path) const
{
  QStringList paths;

  auto strs = path.split("/", Qt::KeepEmptyParts);
  assert(strs.length() > 0);

  QString path1;

  for (int i = 0; i < strs.length(); ++i) {
    if (path1.length())
      path1 += "/";

    path1 += strs[i];

    paths.push_back(path1);
  }

  return paths;
}

void
CQChartsGroupPlot::
setModelGroupInd(const ModelIndex &ind, int groupInd)
{
  const auto &model = this->currentModel();

  assert(model.data());

  QVariant var(groupInd);

  int role = CQModelUtil::roleCast(CQBaseModelRole::Group);

  auto *modelData = this->getModelData(model);

  // disable model notification change for internal data
  if (modelData)
    modelData->connectModel(false);

  model->setHeaderData(ind.row(), Qt::Vertical, var, role);

  if (modelData)
    modelData->connectModel(true);
}

bool
CQChartsGroupPlot::
isGroupHeaders() const
{
  return (groupBucket()->dataType() == Bucket::DataType::HEADER);
}

bool
CQChartsGroupPlot::
isGroupPathType() const
{
  return (groupBucket()->dataType() == Bucket::DataType::PATH);
}

int
CQChartsGroupPlot::
numGroups() const
{
  if (groupBucket() && groupBucket()->isValid())
    return groupBucket()->numBuckets();
  else
    return 1;
}

//---

bool
CQChartsGroupPlot::
getGroupInds(GroupInds &inds) const
{
  if (groupBucket_ && groupBucket_->isValid()) {
    groupBucket_->bucketInds(inds);

    return true;
  }
  else {
    inds.push_back(-1);

    return false;
  }
}

//---

QString
CQChartsGroupPlot::
groupIndName(int ind, bool hier) const
{
  if (! groupBucket_)
    return "";

  if (groupBucket_->dataType() == Bucket::DataType::COLUMN ||
      groupBucket_->dataType() == Bucket::DataType::COLUMN_ROOT) {
     return groupBucket_->bucketIndName(ind, hier);
  }
  else {
    return groupBucket_->indName(ind);
  }
}

CQChartsColumn
CQChartsGroupPlot::
groupIndColumn() const
{
  return groupBucket_->column();
}

CQChartsPlot::ColumnType
CQChartsGroupPlot::
groupType() const
{
  if (! groupBucket_)
    return ColumnType::STRING;

  if (groupBucket_->dataType() == Bucket::DataType::COLUMN ||
      groupBucket_->dataType() == Bucket::DataType::COLUMN_ROOT)
    return groupBucket_->columnType();

  return ColumnType::STRING;
}

QVariant
CQChartsGroupPlot::
groupIndValue(int ind) const
{
  if (! groupBucket_)
    return QVariant();

  if (groupBucket_->dataType() == Bucket::DataType::COLUMN ||
      groupBucket_->dataType() == Bucket::DataType::COLUMN_ROOT) {
    return groupBucket_->bucketValue(ind);
  }

  return QVariant();
}

//---

bool
CQChartsGroupPlot::
adjustedGroupColor(int ig, int ng, Color &color) const
{
  // use color column and color map data if column is valid and is the grouping column
  if (! colorColumn().isValid())
    return false;

  if (! colorColumn().isGroup() && colorColumn() != groupColumn())
    return false;

  if (! isColorMapped())
    return false;

  double r = CMathUtil::map(ig, 0, ng - 1, colorMapMin(), colorMapMax());

  color = colorFromColorMapPaletteValue(r);

  return color.isValid();
}

//---

void
CQChartsGroupPlot::
printGroup() const
{
  if (groupBucket_)
    groupBucket_->print(std::cerr);
}
