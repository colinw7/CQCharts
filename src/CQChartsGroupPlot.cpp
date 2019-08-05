#include <CQChartsGroupPlot.h>
#include <CQChartsColumnBucket.h>
#include <CQChartsModelUtil.h>
#include <CQChartsVariant.h>

#include <CQPropertyViewItem.h>
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
    addColumnParameter("group", "Group", "groupColumn").
     setRequired().setGroupable().setTip("Group column");
  else
    addColumnParameter("group", "Group", "groupColumn").
     setGroupable().setTip("Group column");

  if (allowRowGrouping())
    addBoolParameter("rowGrouping", "Row Grouping", "rowGrouping").
      setTip("Group by rows instead of column headers");

  if (allowUsePath())
    addBoolParameter("usePath", "Use Path", "usePath", true).
      setTip("Use hierarchical path as group");

  if (allowUseRow())
    addBoolParameter("useRow", "Use Row", "useRow").
      setTip("Use row number for group");

  addBoolParameter("exactValue", "Exact Value", "exactValue", true).
   setTip("Use exact value for grouping");

  addBoolParameter("autoRange", "Auto Range", "autoRange", true).
   setTip("Automatically determine value range");

  addRealParameter("start", "Start", "startValue", 0.0).
    setRequired().setTip("Start value for manual range");
  addRealParameter("delta", "Delta", "deltaValue", 1.0).
    setRequired().setTip("Delta value for manual range");

  addIntParameter("numAuto", "Num Auto", "numAuto", 10).
    setRequired().setTip("Number of auto groups");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

//---

CQChartsGroupPlot::
CQChartsGroupPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model) :
 CQChartsPlot(view, plotType, model)
{
}

CQChartsGroupPlot::
~CQChartsGroupPlot()
{
  delete groupBucket_;
}

void
CQChartsGroupPlot::
setGroupColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(groupColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsGroupPlot::
setRowGrouping(bool b)
{
  CQChartsUtil::testAndSet(groupData_.rowGrouping, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGroupPlot::
setUsePath(bool b)
{
  CQChartsUtil::testAndSet(groupData_.usePath, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGroupPlot::
setUseRow(bool b)
{
  CQChartsUtil::testAndSet(groupData_.useRow, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsGroupPlot::
setExactValue(bool b)
{
  CQChartsUtil::testAndSet(groupData_.exactValue, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGroupPlot::
setAutoRange(bool b)
{
  CQBucketer::Type type = (b ? CQBucketer::Type::REAL_AUTO : CQBucketer::Type::REAL_RANGE);

  if (type != groupData_.bucketer.type()) {
    groupData_.bucketer.setType(type);

    updateRangeAndObjs();
  }
}

void
CQChartsGroupPlot::
setStartValue(double r)
{
  if (r != groupData_.bucketer.rstart()) {
    groupData_.bucketer.setRStart(r);

    updateRangeAndObjs();
  }
}

void
CQChartsGroupPlot::
setDeltaValue(double r)
{
  if (r != groupData_.bucketer.rdelta()) {
    groupData_.bucketer.setRDelta(r);

    updateRangeAndObjs();
  }
}

void
CQChartsGroupPlot::
setNumAuto(int i)
{
  if (i != groupData_.bucketer.numAuto()) {
    groupData_.bucketer.setNumAuto(i);

    updateRangeAndObjs();
  }
}

//---

void
CQChartsGroupPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  CQChartsGroupPlotType *type = dynamic_cast<CQChartsGroupPlotType *>(this->type());
  assert(type);

  addProp("dataGrouping", "groupColumn", "group", "Group column");

  if (type->allowRowGrouping())
    addProp("dataGrouping", "rowGrouping", "rowGroups", "Group by rows instead of column headers");

  if (type->allowUsePath())
    addProp("dataGrouping", "usePath", "path", "Use path for group");

  if (type->allowUseRow())
    addProp("dataGrouping", "useRow", "row", "Use row number for grouping");

  addProp("dataGrouping/bucket", "exactValue", "exact"  , "Use exact value");
  addProp("dataGrouping/bucket", "autoRange" , "auto"   , "Bucket auto range");
  addProp("dataGrouping/bucket", "startValue", "start"  , "Bucket start value");
  addProp("dataGrouping/bucket", "deltaValue", "delta"  , "Bucket delta value");
  addProp("dataGrouping/bucket", "numAuto"   , "numAuto", "Number of automatic buckets");
}

void
CQChartsGroupPlot::
setGroupBucket(CQChartsColumnBucket *groupBucket)
{
  delete groupBucket_;

  groupBucket_ = groupBucket;
}

void
CQChartsGroupPlot::
initGroupData(const CQChartsColumns &dataColumns,
              const CQChartsColumn &nameColumn, bool hier) const
{
  CQPerfTrace trace("CQChartsGroupPlot::initGroupData");

  std::unique_lock<std::mutex> lock(mutex_);

  // given columns and current grouping settings cache group buckets
  CQChartsColumnBucket *groupBucket = initGroupData(dataColumns, nameColumn, hier, groupData_);

  const_cast<CQChartsGroupPlot *>(this)->setGroupBucket(groupBucket);
}

CQChartsColumnBucket *
CQChartsGroupPlot::
initGroupData(const CQChartsColumns &dataColumns, const CQChartsColumn &nameColumn, bool hier,
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
  CQChartsColumnBucket *columnBucket = new CQChartsColumnBucket;

  //---

  // when not row grouping we use the column header as the grouping id so all row
  // values in the column are added to the group
  if (data.columns.count() > 1 && ! data.rowGrouping) {
    columnBucket->setDataType   (CQChartsColumnBucket::DataType::HEADER);
    columnBucket->setColumnType (ColumnType::INTEGER);
    columnBucket->setRowGrouping(false);

    for (const auto &column : data.columns) {
      bool ok;

      QString name = modelHeaderString(column, ok);

      if (! name.length())
        name = QString("%1").arg(column.column());

      int ind = columnBucket->addValue(column.column());

      columnBucket->setIndName(ind, name);
    }

    return columnBucket;
  }

  //---

  // for specified grouping columns, set column and column type
  if      (data.columns.count() > 1) {
    CQChartsColumn column;

    if (data.column.isValid())
      column = data.column;

    if (! data.column.isValid()) {
      // TODO: combine multiple column values
      column = data.columns.column();
      assert(column.isValid());
    }

    ColumnType columnType = CQBaseModelType::STRING;

    if (column.type() == CQChartsColumn::Type::DATA ||
        column.type() == CQChartsColumn::Type::DATA_INDEX)
      columnType = columnValueType(column);

    if (isHierarchical())
      columnBucket->setDataType(CQChartsColumnBucket::DataType::COLUMN_ROOT);
    else
      columnBucket->setDataType(CQChartsColumnBucket::DataType::COLUMN);

    columnBucket->setColumnType(columnType);
    columnBucket->setColumn    (column);
  }
  // for specified grouping column, set column and column type
  else if (data.column.isValid()) {
    ColumnType columnType = CQBaseModelType::STRING;

    if (data.column.type() == CQChartsColumn::Type::DATA ||
        data.column.type() == CQChartsColumn::Type::DATA_INDEX)
      columnType = columnValueType(data.column);

    if (isHierarchical())
      columnBucket->setDataType(CQChartsColumnBucket::DataType::COLUMN_ROOT);
    else
      columnBucket->setDataType(CQChartsColumnBucket::DataType::COLUMN);

    columnBucket->setColumnType(columnType);
    columnBucket->setColumn    (data.column);
  }
  // no group column then use parent path (hierarchical)
  else if (data.usePath) {
    assert(isHierarchical());

    columnBucket->setDataType  (CQChartsColumnBucket::DataType::PATH);
    columnBucket->setColumnType(ColumnType::STRING);
  }
  else {
    columnBucket->setDataType  (CQChartsColumnBucket::DataType::NONE);
    columnBucket->setColumnType(data.useRow ? ColumnType::INTEGER : ColumnType::STRING);
  }

  columnBucket->setUseRow    (data.useRow);
  columnBucket->setExactValue(data.exactValue);
  columnBucket->setBucketer  (data.bucketer);

  //---

  // process model data
  class GroupVisitor : public ModelVisitor {
   public:
    GroupVisitor(const CQChartsGroupPlot *plot, CQChartsColumnBucket *bucket, bool hier) :
     plot_(plot), bucket_(bucket), hier_(hier) {
    }

    State visit(const QAbstractItemModel *model, const VisitData &data) override {
      // add column value
      if      (bucket_->dataType() == CQChartsColumnBucket::DataType::COLUMN ||
               bucket_->dataType() == CQChartsColumnBucket::DataType::COLUMN_ROOT) {
        bool ok;

        QVariant value;

        if (bucket_->dataType() == CQChartsColumnBucket::DataType::COLUMN_ROOT)
          value = plot_->modelRootValue(data.row, bucket_->column(), data.parent, ok);
        else
          value = plot_->modelHierValue(data.row, bucket_->column(), data.parent, ok);

        if (value.isValid())
          bucket_->addValue(value);

        if (hier_) {
          QStringList paths = plot_->pathStrs(value.toString());

          for (int i = 0; i < paths.length(); ++i)
            bucket_->addValue(paths[i]);
        }
      }
      // add parent path (hierarchical)
      else if (bucket_->dataType() == CQChartsColumnBucket::DataType::PATH) {
        QString path = CQChartsModelUtil::parentPath(model, data.parent);

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
    const CQChartsGroupPlot* plot_   { nullptr };
    CQChartsColumnBucket*    bucket_ { nullptr };
    bool                     hier_   { false };
  };

  GroupVisitor groupVisitor(this, columnBucket, data.hier);

  visitModel(groupVisitor);

  return columnBucket;
}

//---

std::vector<int>
CQChartsGroupPlot::
rowHierGroupInds(const CQChartsModelIndex &ind) const
{
  std::vector<int> inds;

  if (! rowGroupInds(ind, inds, /*hier*/true))
    return inds;

  return inds;
}

int
CQChartsGroupPlot::
rowGroupInd(const CQChartsModelIndex &ind) const
{
  std::vector<int> inds;

  if (! rowGroupInds(ind, inds, /*hier*/false))
    return -1;

  assert(inds.size() == 1);

  int groupInd = inds[0];

  {
  std::unique_lock<std::mutex> lock(mutex_);

  CQChartsGroupPlot *th = const_cast<CQChartsGroupPlot *>(this);

  th->setModelGroupInd(ind, groupInd);
  }

  return groupInd;
}

bool
CQChartsGroupPlot::
rowGroupInds(const CQChartsModelIndex &ind, std::vector<int> &inds, bool hier) const
{
  QAbstractItemModel *model = this->model().data();
  if (! model) return false;

  if (! groupBucket_)
    return false;

  // header has multiple groups (one per column)
  if      (groupBucket_->dataType() == CQChartsColumnBucket::DataType::HEADER) {
    int ind1 = groupBucket_->ind(ind.column.column());

    inds.push_back(ind1);
  }
  // get group id from value in group column
  else if (groupBucket_->dataType() == CQChartsColumnBucket::DataType::COLUMN ||
           groupBucket_->dataType() == CQChartsColumnBucket::DataType::COLUMN_ROOT) {
    bool ok;

    QVariant value;

    if (groupBucket_->dataType() == CQChartsColumnBucket::DataType::COLUMN_ROOT)
      value = modelRootValue(ind.row, groupBucket_->column(), ind.parent, ok);
    else
      value = modelHierValue(ind.row, groupBucket_->column(), ind.parent, ok);

    if (! value.isValid())
      return false;

    int ind1 = -1;

    if      (groupBucket_->isExactValue()) {
      ind1 = groupBucket_->ind(value);

      inds.push_back(ind1);
    }
    else if (CQChartsVariant::isNumeric(value)) {
      bool ok;

      double r = CQChartsVariant::toReal(value, ok);

      ind1 = groupBucket_->bucket(r);

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
  else if (groupBucket_->dataType() == CQChartsColumnBucket::DataType::PATH) {
    QString path = CQChartsModelUtil::parentPath(model, ind.parent);

    if (hier) {
      inds = pathInds(path);
    }
    else {
      int ind = groupBucket_->ind(path);

      inds.push_back(ind);
    }
  }
  else if (groupBucket_->isUseRow()) {
    int ind1 = ind.row; // default to row

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

  QStringList paths = pathStrs(path);

  for (int i = 0; i < paths.length(); ++i) {
    const QString &path1 = paths[i];

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

  QStringList strs = path.split("/", QString::KeepEmptyParts);
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
setModelGroupInd(const CQChartsModelIndex &ind, int groupInd)
{
  QAbstractItemModel *model = this->model().data();
  assert(model);

  QVariant var(groupInd);

  int role = (int) CQBaseModelRole::Group;

  model->setHeaderData(ind.row, Qt::Vertical, var, role);
}

bool
CQChartsGroupPlot::
isGroupHeaders() const
{
  return (groupBucket()->dataType() == CQChartsColumnBucket::DataType::HEADER);
}

bool
CQChartsGroupPlot::
isGroupPathType() const
{
  return (groupBucket()->dataType() == CQChartsColumnBucket::DataType::PATH);
}

int
CQChartsGroupPlot::
numGroups() const
{
  return (groupBucket_ ? groupBucket_->numUnique() : 1);
}

//---

void
CQChartsGroupPlot::
getGroupInds(std::vector<int> &inds) const
{
  if (groupBucket_ && groupBucket_->dataType() != CQChartsColumnBucket::DataType::NONE) {
    for (int groupInd = groupBucket_->imin(); groupInd <= groupBucket_->imax(); ++groupInd)
      inds.push_back(groupInd);
  }
  else {
    inds.push_back(-1);
  }
}

//---

QString
CQChartsGroupPlot::
groupIndName(int ind, bool hier) const
{
  if (! groupBucket_)
    return "";

  if (groupBucket_->dataType() == CQChartsColumnBucket::DataType::COLUMN ||
      groupBucket_->dataType() == CQChartsColumnBucket::DataType::COLUMN_ROOT) {
    if      (groupBucket_->isExactValue()) {
      return groupBucket_->indName(ind);
    }
    else if (groupBucket_->columnType() == ColumnType::REAL ||
             groupBucket_->columnType() == ColumnType::INTEGER) {
      return groupBucket_->bucketName(ind);
    }
    else {
      if (hier)
        return groupBucket_->iname(ind);
      else
        return groupBucket_->buckets(ind);
    }
  }
  else {
    return groupBucket_->indName(ind);
  }
}

//---

void
CQChartsGroupPlot::
printGroup() const
{
  if (groupBucket_)
    groupBucket_->print(std::cerr);
}
