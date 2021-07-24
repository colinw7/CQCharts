#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsColumnType.h>
#include <CQChartsModelFilter.h>
#include <CQChartsModelVisitor.h>
#include <CQChartsModelUtil.h>
#include <CQChartsValueSet.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>

#include <CQPerfMonitor.h>
#include <CMathCorrelation.h>

#include <QAbstractItemModel>

CQChartsModelDetails::
CQChartsModelDetails(CQChartsModelData *data) :
 data_(data)
{
  if (charts())
    connect(charts(), SIGNAL(modelTypeChanged(int)), this, SLOT(modelTypeChangedSlot(int)));
}

CQChartsModelDetails::
~CQChartsModelDetails()
{
  resetValues();
}

int
CQChartsModelDetails::
numColumns() const
{
  initSimpleData();

  return numColumns_;
}

int
CQChartsModelDetails::
numRows() const
{
  initSimpleData();

  return numRows_;
}

bool
CQChartsModelDetails::
isHierarchical() const
{
  initSimpleData();

  return hierarchical_;
}

const CQChartsModelColumnDetails *
CQChartsModelDetails::
columnDetails(const CQChartsColumn &c) const
{
  auto *th = const_cast<CQChartsModelDetails *>(this);

  return th->columnDetails(c);
}

CQChartsModelColumnDetails *
CQChartsModelDetails::
columnDetails(const CQChartsColumn &c)
{
  // fail if column not in model
  if (c.hasColumn()) {
    int nc = numColumns();

    if (c.column() < 0 || c.column() >= nc)
      return nullptr;
  }

  //---

  std::unique_lock<std::mutex> columnLock(columnMutex_);

  auto p = columnDetails_.find(c);

  if (p == columnDetails_.end()) {
    auto *details = new CQChartsModelColumnDetails(this, c);

    p = columnDetails_.insert(p, ColumnDetailsMap::value_type(c, details));
  }

  auto *details = (*p).second;

  return details;
}

void
CQChartsModelDetails::
initSimpleData() const
{
  if (initialized_ == Initialized::NONE) {
    std::unique_lock<std::mutex> initLock(initMutex_);

    if (initialized_ == Initialized::NONE) {
      auto *th = const_cast<CQChartsModelDetails *>(this);

      th->updateSimple();
    }
  }
}

void
CQChartsModelDetails::
initFullData() const
{
  if (initialized_ != Initialized::FULL) {
    std::unique_lock<std::mutex> initLock(initMutex_);

    if (initialized_ != Initialized::FULL) {
      auto *th = const_cast<CQChartsModelDetails *>(this);

      th->updateFull();
    }
  }
}

void
CQChartsModelDetails::
reset()
{
  resetValues();

  emit detailsReset();
}

void
CQChartsModelDetails::
resetValues()
{
  std::unique_lock<std::mutex> initLock(initMutex_);

  initialized_  = Initialized::NONE;
  numColumns_   = 0;
  numRows_      = 0;
  hierarchical_ = false;

  //---

  std::unique_lock<std::mutex> columnLock(columnMutex_);

  for (auto &cd : columnDetails_)
    delete cd.second;

  columnDetails_.clear();
}

void
CQChartsModelDetails::
updateSimple()
{
  CQPerfTrace trace("CQChartsModelDetails::updateSimple");

  assert(initialized_ == Initialized::NONE);

  auto *model = this->model();

  hierarchical_ = CQChartsModelUtil::isHierarchical(model);

  numColumns_ = model->columnCount();
  numRows_    = model->rowCount   ();

  initialized_ = Initialized::SIMPLE;
}

void
CQChartsModelDetails::
updateFull()
{
  CQPerfTrace trace("CQChartsModelDetails::updateFull");

  assert(initialized_ != Initialized::FULL);

  if (initialized_ == Initialized::NONE)
    updateSimple();

  resetValues();

  for (int c = 0; c < numColumns_; ++c) {
    auto *columnDetails = this->columnDetails(CQChartsColumn(c));

    numRows_ = std::max(numRows_, columnDetails->numRows());
  }

  initialized_ = Initialized::FULL;
}

void
CQChartsModelDetails::
modelTypeChangedSlot(int modelInd)
{
  if (data_ && data_->ind() == modelInd) {
    for (int c = 0; c < numColumns_; ++c) {
      auto *columnDetails = this->columnDetails(CQChartsColumn(c));

      columnDetails->resetTypeInitialized();
    }
  }
}

CQChartsColumns
CQChartsModelDetails::
numericColumns() const
{
  initSimpleData();

  CQChartsColumns columns;

  for (int c = 0; c < numColumns_; ++c) {
    const auto *columnDetails = this->columnDetails(CQChartsColumn(c));

    if (columnDetails->isNumeric())
      columns.addColumn(columnDetails->column());
  }

  return columns;
}

CQChartsColumns
CQChartsModelDetails::
monotonicColumns() const
{
  initSimpleData();

  CQChartsColumns columns;

  for (int c = 0; c < numColumns_; ++c) {
    const auto *columnDetails = this->columnDetails(CQChartsColumn(c));

    if (columnDetails->isMonotonic())
      columns.addColumn(columnDetails->column());
  }

  return columns;
}

std::vector<int>
CQChartsModelDetails::
duplicates(const CQChartsColumn &column) const
{
  return columnDuplicates(column, false);
}

std::vector<int>
CQChartsModelDetails::
duplicates() const
{
  return columnDuplicates(CQChartsColumn(), true);
}

std::vector<int>
CQChartsModelDetails::
columnDuplicates(const CQChartsColumn &column, bool all) const
{
  initSimpleData();

  //---

  std::vector<int> rows;

  auto *charts = this->charts();
  if (! charts) return rows;

  auto *model = this->model();

  std::vector<QVariant> rowValues1, rowValues2;

  if (all)
    rowValues2.resize(numColumns_);
  else
    rowValues2.resize(1);

  for (int r = 0; r < numRows_; ++r) {
    bool match = true;

    if (all) {
      for (int c = 0; c < numColumns_; ++c) {
        QModelIndex parent;

        bool ok;

        auto var = CQChartsModelUtil::modelValue(charts, model, r, CQChartsColumn(c), parent, ok);

        rowValues2[c] = var;

        if (rowValues1.empty() || var != rowValues1[c])
          match = false;
      }
    }
    else {
      QModelIndex parent;

      bool ok;

      auto var = CQChartsModelUtil::modelValue(charts, model, r, column, parent, ok);

      rowValues2[0] = var;

      if (rowValues1.empty() || var != rowValues1[0])
        match = false;
    }

    if (match)
      rows.push_back(r);

    rowValues1 = rowValues2;
  }

  return rows;
}

double
CQChartsModelDetails::
correlation(const CQChartsColumn &column1, const CQChartsColumn &column2) const
{
  const auto *details1 = columnDetails(column1);
  const auto *details2 = columnDetails(column2);

  if (! details1 || ! details2)
    return 0.0;

  details1->initCache();
  details2->initCache();

  std::vector<double> r1, r2;

  details1->valueSet()->reals(r1);
  details2->valueSet()->reals(r2);

  if (r1.size() != r2.size())
    return 0.0;

  return CMathCorrelation::calc(r1, r2);
}

CQCharts *
CQChartsModelDetails::
charts() const
{
  return (data_ ? data_->charts() : nullptr);
}

QAbstractItemModel *
CQChartsModelDetails::
model() const
{
  return (data_ ? data_->currentModel().data() : nullptr);
}

//------

CQChartsModelColumnDetails::
CQChartsModelColumnDetails(CQChartsModelDetails *details, const CQChartsColumn &column) :
 details_(details), column_(column)
{
  assert(details_);
}

CQChartsModelColumnDetails::
~CQChartsModelColumnDetails()
{
  delete valueSet_;
  delete bucketer_;
}

bool
CQChartsModelColumnDetails::
isNamedValue(const QString &name)
{
  return getLongNamedValues ().contains(name) ||
         getShortNamedValues().contains(name);
}

const QStringList &
CQChartsModelColumnDetails::
getLongNamedValues()
{
  static auto namedValues = QStringList() <<
    "name" << "type" << "minimum" << "maximum" << "mean" << "standard_deviation" <<
    "monotonic" << "increasing" << "num_unique" << "unique_values" << "unique_counts" <<
    "num_null" << "median" << "lower_median" << "upper_median" <<
    "outliers";

  return namedValues;
}

const QStringList &
CQChartsModelColumnDetails::
getShortNamedValues()
{
  static auto namedValues = QStringList() <<
    "min" << "max" << "mean" << "sum" << "avg" << "stdev" << "stddev" << "std_dev";

  return namedValues;
}

QVariant
CQChartsModelColumnDetails::
getNamedValue(const QString &name) const
{
  if      (name == "name")
    return this->headerName();
  else if (name == "type")
    return this->typeName();
  else if (name == "min" || name == "minimum")
    return this->minValue();
  else if (name == "max" || name == "maximum")
    return this->maxValue();
  else if (name == "mean" || name == "avg" || name == "average")
    return this->meanValue();
  else if (name == "sum")
    return this->sumValue();
  else if (name == "stdev"   || name == "stddev" ||
           name == "std_dev" || name == "standard_deviation")
    return this->stdDevValue();

  else if (name == "monotonic")
    return this->isMonotonic();
  else if (name == "increasing")
    return this->isIncreasing();

  else if (name == "num_unique")
    return this->numUnique();
  else if (name == "unique_values")
    return this->uniqueValues();
  else if (name == "unique_counts")
    return this->uniqueCounts();

  else if (name == "num_null")
    return this->numNull();

  else if (name == "median")
    return this->medianValue();
  else if (name == "lower_median")
    return this->lowerMedianValue();
  else if (name == "upper_median")
    return this->upperMedianValue();

  else if (name == "outliers")
    return this->outlierValues();

  return QVariant();
}

QString
CQChartsModelColumnDetails::
headerName() const
{
  auto *model = details_->model();

  bool ok;

  return CQChartsModelUtil::modelHHeaderString(model, column_, ok);
}

bool
CQChartsModelColumnDetails::
isKey() const
{
  auto *model = details_->model();

  bool ok;

  auto value = CQChartsModelUtil::modelHeaderValue(model, column_,
                 static_cast<int>(CQBaseModelRole::Key), ok);

  return (ok && value.toBool());
}

QString
CQChartsModelColumnDetails::
typeName() const
{
  initType();

  return typeName_;
}

const CQChartsModelTypeData &
CQChartsModelColumnDetails::
typeData() const
{
  initType();

  return typeData_;
}

CQBaseModelType
CQChartsModelColumnDetails::
type() const
{
  return typeData().type;
}

void
CQChartsModelColumnDetails::
setType(CQBaseModelType type)
{
  typeData_.type = type;

  if (typeData_.baseType == CQBaseModelType::NONE)
    typeData_.baseType = typeData_.type;
}

CQBaseModelType
CQChartsModelColumnDetails::
baseType() const
{
  return typeData().baseType;
}

void
CQChartsModelColumnDetails::
setBaseType(CQBaseModelType type)
{
  typeData_.baseType = type;
}

const CQChartsNameValues &
CQChartsModelColumnDetails::
nameValues() const
{
  return typeData().nameValues;
}

QVariant
CQChartsModelColumnDetails::
minValue() const
{
  initCache();

  return minValue_;
}

QVariant
CQChartsModelColumnDetails::
maxValue() const
{
  initCache();

  return maxValue_;
}

QVariant
CQChartsModelColumnDetails::
meanValue(bool useNaN) const
{
  auto *valueSet = this->calcValueSet();

  if      (type() == CQBaseModelType::INTEGER) {
    return valueSet->ivals().mean();
  }
  else if (type() == CQBaseModelType::REAL) {
    return valueSet->rvals().mean();
  }
  else if (type() == CQBaseModelType::STRING) {
    return (useNaN ? QVariant(CMathUtil::getNaN()) : QVariant());
  }
  else if (type() == CQBaseModelType::TIME) {
    return valueSet->tvals().mean();
  }
  else if (type() == CQBaseModelType::COLOR) {
    return (useNaN ? QVariant(CMathUtil::getNaN()) : QVariant());
  }

  return QVariant();
}

QVariant
CQChartsModelColumnDetails::
sumValue(bool useNaN) const
{
  auto *valueSet = this->calcValueSet();

  if      (type() == CQBaseModelType::INTEGER) {
    return valueSet->ivals().sum();
  }
  else if (type() == CQBaseModelType::REAL) {
    return valueSet->rvals().sum();
  }
  else if (type() == CQBaseModelType::STRING) {
    return (useNaN ? QVariant(CMathUtil::getNaN()) : QVariant());
  }
  else if (type() == CQBaseModelType::TIME) {
    return valueSet->tvals().sum();
  }
  else if (type() == CQBaseModelType::COLOR) {
    return (useNaN ? QVariant(CMathUtil::getNaN()) : QVariant());
  }

  return QVariant();
}

QVariant
CQChartsModelColumnDetails::
stdDevValue(bool useNaN) const
{
  auto *valueSet = this->calcValueSet();

  if      (type() == CQBaseModelType::INTEGER) {
    return valueSet->ivals().stddev();
  }
  else if (type() == CQBaseModelType::REAL) {
    return valueSet->rvals().stddev();
  }
  else if (type() == CQBaseModelType::STRING) {
    return (useNaN ? QVariant(CMathUtil::getNaN()) : QVariant());
  }
  else if (type() == CQBaseModelType::TIME) {
    return valueSet->tvals().stddev();
  }
  else if (type() == CQBaseModelType::COLOR) {
    return (useNaN ? QVariant(CMathUtil::getNaN()) : QVariant());
  }

  return QVariant();
}

QVariant
CQChartsModelColumnDetails::
dataName(const QVariant &v) const
{
  initCache();

  auto *charts = details_->charts();
  if (! charts) return QVariant();

  const auto *columnType = this->columnType();
  if (! columnType) return v;

  auto *model = details_->model();

  bool converted;

  auto v1 = columnType->dataName(charts, model, column_, v, typeData(), converted);

  return v1;
}

int
CQChartsModelColumnDetails::
numRows() const
{
  initCache();

  return numRows_;
}

bool
CQChartsModelColumnDetails::
isNumeric() const
{
  if (type() == CQBaseModelType::INTEGER ||
      type() == CQBaseModelType::REAL)
    return true;

  return false;
}

bool
CQChartsModelColumnDetails::
isMonotonic() const
{
  if (column_.type() == Column::Type::DATA ||
      column_.type() == Column::Type::DATA_INDEX) {
    bool ok;

    auto *model = details_->model();

    auto var = CQChartsModelUtil::modelHeaderValue(
      model, column_, static_cast<int>(CQBaseModelRole::Sorted), ok);

    if (ok && var.isValid() && var.toBool())
      return true;
  }

  //---

  initCache();

  return monotonic_;
}

bool
CQChartsModelColumnDetails::
isIncreasing() const
{
  if (column_.type() == Column::Type::DATA ||
      column_.type() == Column::Type::DATA_INDEX) {
    bool ok;

    auto *model = details_->model();

    auto var = CQChartsModelUtil::modelHeaderValue(
      model, column_, static_cast<int>(CQBaseModelRole::Sorted), ok);

    if (ok && var.isValid() && var.toBool()) {
      auto var = CQChartsModelUtil::modelHeaderValue(
        model, column_, static_cast<int>(CQBaseModelRole::SortOrder), ok);

      if (ok && var.isValid()) {
        bool ok;

        long order = CQChartsVariant::toInt(var, ok);

        return (ok && (order == Qt::AscendingOrder));
      }
    }
  }

  //---

  initCache();

  return increasing_;
}

//------

int
CQChartsModelColumnDetails::
numUnique() const
{
  auto *valueSet = this->calcValueSet();

  if (column_.type() == Column::Type::ROW) {
    return details_->numRows();
  }

  if      (type() == CQBaseModelType::INTEGER) {
    return valueSet->ivals().numUnique();
  }
  else if (type() == CQBaseModelType::REAL) {
    return valueSet->rvals().numUnique();
  }
  else if (type() == CQBaseModelType::STRING) {
    return valueSet->svals().numUnique();
  }
  else if (type() == CQBaseModelType::TIME) {
    return valueSet->tvals().numUnique();
  }
  else if (type() == CQBaseModelType::COLOR) {
    return valueSet->cvals().numUnique();
  }
  else {
    return 0;
  }
}

QVariantList
CQChartsModelColumnDetails::
uniqueValues() const
{
  auto *valueSet = this->calcValueSet();

  QVariantList vars;

  if (column_.type() == Column::Type::ROW) {
    return vars; // TODO
  }

  if      (type() == CQBaseModelType::INTEGER) {
    CQChartsIValues::Values values;

    valueSet->ivals().uniqueValues(values);

    for (const auto &v : values)
      vars.push_back(v);
  }
  else if (type() == CQBaseModelType::REAL) {
    CQChartsRValues::Values values;

    valueSet->rvals().uniqueValues(values);

    for (const auto &v : values)
      vars.push_back(v);
  }
  else if (type() == CQBaseModelType::STRING) {
    CQChartsSValues::Values values;

    valueSet->svals().uniqueValues(values);

    for (const auto &v : values)
      vars.push_back(v);
  }
  else if (type() == CQBaseModelType::TIME) {
    CQChartsRValues::Values values;

    valueSet->tvals().uniqueValues(values);

    for (const auto &v : values)
      vars.push_back(v);
  }
  else if (type() == CQBaseModelType::COLOR) {
    CQChartsCValues::Values values;

    valueSet->cvals().uniqueValues(values);

    for (const auto &c : values)
      vars.push_back(CQChartsVariant::fromColor(c));
  }

  return vars;
}

QVariantList
CQChartsModelColumnDetails::
uniqueCounts() const
{
  auto *valueSet = this->calcValueSet();

  QVariantList vars;

  if (column_.type() == Column::Type::ROW) {
    return vars; // TODO
  }

  CQChartsIValues::Counts counts;

  if      (type() == CQBaseModelType::INTEGER) {
    valueSet->ivals().uniqueCounts(counts);
  }
  else if (type() == CQBaseModelType::REAL) {
    valueSet->rvals().uniqueCounts(counts);
  }
  else if (type() == CQBaseModelType::STRING) {
    valueSet->svals().uniqueCounts(counts);
  }
  else if (type() == CQBaseModelType::TIME) {
    valueSet->tvals().uniqueCounts(counts);
  }
  else if (type() == CQBaseModelType::COLOR) {
    valueSet->cvals().uniqueCounts(counts);
  }

  for (const auto &c : counts)
    vars.push_back(c);

  return vars;
}

CQChartsModelColumnDetails::ValueCounts
CQChartsModelColumnDetails::
uniqueValueCounts() const
{
  auto *valueSet = this->calcValueSet();

  ValueCounts valueCounts;

  if (column_.type() == Column::Type::ROW) {
    return valueCounts; // TODO
  }

  if      (type() == CQBaseModelType::INTEGER) {
    CQChartsIValues::ValueCounts ivalueCounts;

    valueSet->ivals().uniqueValueCounts(ivalueCounts);

    for (const auto &v : ivalueCounts)
      valueCounts.emplace_back(v.first, v.second);
  }
  else if (type() == CQBaseModelType::REAL) {
    CQChartsRValues::ValueCounts rvalueCounts;

    valueSet->rvals().uniqueValueCounts(rvalueCounts);

    for (const auto &v : rvalueCounts)
      valueCounts.emplace_back(v.first, v.second);
  }
  else if (type() == CQBaseModelType::STRING) {
    CQChartsSValues::ValueCounts svalueCounts;

    valueSet->svals().uniqueValueCounts(svalueCounts);

    for (const auto &v : svalueCounts)
      valueCounts.emplace_back(v.first, v.second);
  }
  else if (type() == CQBaseModelType::TIME) {
    CQChartsRValues::ValueCounts tvalueCounts;

    valueSet->tvals().uniqueValueCounts(tvalueCounts);

    for (const auto &v : tvalueCounts)
      valueCounts.emplace_back(v.first, v.second);
  }
  else if (type() == CQBaseModelType::COLOR) {
    CQChartsCValues::ValueCounts cvalueCounts;

    valueSet->cvals().uniqueValueCounts(cvalueCounts);

    for (const auto &v : cvalueCounts)
      valueCounts.emplace_back(CQChartsVariant::fromColor(v.first), v.second);
  }

  return valueCounts;
}

int
CQChartsModelColumnDetails::
uniqueId(const QVariant &var) const
{
  auto *valueSet = this->calcValueSet();

  //---

  bool ok;

  if (column_.type() == Column::Type::ROW) {
    long i = CQChartsVariant::toInt(var, ok);

    return (ok ? int(i) : -1);
  }

  //--

  if      (type() == CQBaseModelType::INTEGER) {
    long i = CQChartsVariant::toInt(var, ok);

    if (ok)
      return valueSet->ivals().id(int(i));
  }
  else if (type() == CQBaseModelType::REAL) {
    double r = CQChartsVariant::toReal(var, ok);

    if (ok)
      return valueSet->rvals().id(r);
  }
  else if (type() == CQBaseModelType::STRING) {
    return valueSet->svals().id(var.toString());
  }
  else if (type() == CQBaseModelType::TIME) {
    double r = CQChartsVariant::toReal(var, ok);

    if (ok)
      return valueSet->tvals().id(r);
  }
  else if (type() == CQBaseModelType::COLOR) {
    bool ok;

    auto color = CQChartsVariant::toColor(var, ok);

    if (ok)
      return valueSet->cvals().id(color);
  }

  return -1;
}

QVariant
CQChartsModelColumnDetails::
uniqueValue(int i) const
{
  if (column_.type() == Column::Type::ROW) {
    return QVariant(i);
  }

  auto uniqueValues = this->uniqueValues();

  if (i >= 0 && i < uniqueValues.size())
    return uniqueValues[i];

  return QVariant();
}

//---

int
CQChartsModelColumnDetails::
numBuckets() const
{
  return 20;
}

int
CQChartsModelColumnDetails::
bucket(const QVariant &var) const
{
  if (type() != CQBaseModelType::INTEGER && type() != CQBaseModelType::REAL)
    return uniqueId(var);

  initBucketer();

  return bucketer_->bucket(var);
}

void
CQChartsModelColumnDetails::
bucketRange(int i, QVariant &vmin, QVariant &vmax) const
{
  if (type() != CQBaseModelType::INTEGER && type() != CQBaseModelType::REAL) {
    vmin = uniqueValue(i);
    vmax = vmin;
    return;
  }

  initBucketer();

  if (type() == CQBaseModelType::REAL) {
    double rmin, rmax;

    bucketer_->bucketRValues(i, rmin, rmax);

    vmin = rmin;
    vmax = rmax;
  }
  else {
    int imin, imax;

    bucketer_->bucketIValues(i, imin, imax);

    vmin = imin;
    vmax = imax;
  }
}

//------

int
CQChartsModelColumnDetails::
numNull() const
{
  auto *valueSet = this->calcValueSet();

  if      (type() == CQBaseModelType::INTEGER) {
    return valueSet->ivals().numNull();
  }
  else if (type() == CQBaseModelType::REAL) {
    return valueSet->rvals().numNull();
  }
  else if (type() == CQBaseModelType::STRING) {
    return 0;
  }
  else if (type() == CQBaseModelType::TIME) {
    return valueSet->tvals().numNull();
  }
  else if (type() == CQBaseModelType::COLOR) {
    return valueSet->cvals().numNull();
  }

  return 0;
}

int
CQChartsModelColumnDetails::
valueInd(const QVariant &value) const
{
  auto p = valueInds_.find(value);

  if (p == valueInds_.end())
    return -1;

  return (*p).second;
}

QVariant
CQChartsModelColumnDetails::
medianValue(bool useNaN) const
{
  auto *valueSet = this->calcValueSet();

  if      (type() == CQBaseModelType::INTEGER) {
    return valueSet->ivals().median();
  }
  else if (type() == CQBaseModelType::REAL) {
    return valueSet->rvals().median();
  }
  else if (type() == CQBaseModelType::STRING) {
    return (useNaN ? QVariant(CMathUtil::getNaN()) : QVariant());
  }
  else if (type() == CQBaseModelType::TIME) {
    return valueSet->tvals().median();
  }
  else if (type() == CQBaseModelType::COLOR) {
    return (useNaN ? QVariant(CMathUtil::getNaN()) : QVariant());
  }

  return QVariant();
}

QVariant
CQChartsModelColumnDetails::
lowerMedianValue(bool useNaN) const
{
  auto *valueSet = this->calcValueSet();

  if      (type() == CQBaseModelType::INTEGER) {
    return valueSet->ivals().lowerMedian();
  }
  else if (type() == CQBaseModelType::REAL) {
    return valueSet->rvals().lowerMedian();
  }
  else if (type() == CQBaseModelType::STRING) {
    return (useNaN ? QVariant(CMathUtil::getNaN()) : QVariant());
  }
  else if (type() == CQBaseModelType::TIME) {
    return valueSet->tvals().lowerMedian();
  }
  else if (type() == CQBaseModelType::COLOR) {
    return (useNaN ? QVariant(CMathUtil::getNaN()) : QVariant());
  }

  return QVariant();
}

QVariant
CQChartsModelColumnDetails::
upperMedianValue(bool useNaN) const
{
  auto *valueSet = this->calcValueSet();

  if      (type() == CQBaseModelType::INTEGER) {
    return valueSet->ivals().upperMedian();
  }
  else if (type() == CQBaseModelType::REAL) {
    return valueSet->rvals().upperMedian();
  }
  else if (type() == CQBaseModelType::STRING) {
    return (useNaN ? QVariant(CMathUtil::getNaN()) : QVariant());
  }
  else if (type() == CQBaseModelType::TIME) {
    return valueSet->tvals().upperMedian();
  }
  else if (type() == CQBaseModelType::COLOR) {
    return (useNaN ? QVariant(CMathUtil::getNaN()) : QVariant());
  }

  return QVariant();
}

QVariantList
CQChartsModelColumnDetails::
outlierValues() const
{
  auto *valueSet = this->calcValueSet();

  QVariantList vars;

  if      (type() == CQBaseModelType::INTEGER) {
    const auto &outliers = valueSet->ivals().outliers();

    for (auto &o : outliers)
      vars.push_back(valueSet->ivals().svalue(o));
  }
  else if (type() == CQBaseModelType::REAL) {
    const auto &outliers = valueSet->rvals().outliers();

    for (auto &o : outliers)
      vars.push_back(valueSet->rvals().svalue(o));
  }
  else if (type() == CQBaseModelType::STRING) {
    return vars;
  }
  else if (type() == CQBaseModelType::TIME) {
    const auto &outliers = valueSet->tvals().outliers();

    for (auto &o : outliers)
      vars.push_back(valueSet->tvals().svalue(o));
  }
  else if (type() == CQBaseModelType::COLOR) {
    return vars;
  }

  return vars;
}

bool
CQChartsModelColumnDetails::
isOutlier(const QVariant &value) const
{
  auto *valueSet = this->calcValueSet();

  bool ok;

  if      (type() == CQBaseModelType::INTEGER) {
    long i = CQChartsVariant::toInt(value, ok);

    return (ok && valueSet->ivals().isOutlier(int(i)));
  }
  else if (type() == CQBaseModelType::REAL) {
    double r = CQChartsVariant::toReal(value, ok);

    return (ok && valueSet->rvals().isOutlier(r));
  }
  else if (type() == CQBaseModelType::STRING) {
    return false;
  }
  else if (type() == CQBaseModelType::TIME) {
    double r = CQChartsVariant::toReal(value, ok);

    return (ok && valueSet->tvals().isOutlier(r));
  }
  else if (type() == CQBaseModelType::COLOR) {
    return false;
  }

  return false;
}

double
CQChartsModelColumnDetails::
map(const QVariant &var) const
{
  auto *valueSet = this->calcValueSet();

  bool ok;

  if      (type() == CQBaseModelType::INTEGER) {
    long i = CQChartsVariant::toInt(var, ok);

    if (ok)
      return valueSet->ivals().map(int(i));
  }
  else if (type() == CQBaseModelType::REAL) {
    double r = CQChartsVariant::toReal(var, ok);

    return valueSet->rvals().map(r);
  }
  else if (type() == CQBaseModelType::STRING) {
    return 0;
  }
  else if (type() == CQBaseModelType::TIME) {
    double r = CQChartsVariant::toReal(var, ok);

    return valueSet->tvals().map(r);
  }
  else if (type() == CQBaseModelType::COLOR) {
    return 0;
  }

  return 0;
}

//---

int
CQChartsModelColumnDetails::
valueCount() const
{
  if      (type() == CQBaseModelType::INTEGER) {
    return valueSet_->ivals().size();
  }
  else if (type() == CQBaseModelType::REAL) {
    return valueSet_->rvals().size();
  }
  else if (type() == CQBaseModelType::STRING) {
    return valueSet_->svals().size();
  }
  else if (type() == CQBaseModelType::TIME) {
    return valueSet_->tvals().size();
  }
  else if (type() == CQBaseModelType::COLOR) {
    return valueSet_->cvals().size();
  }

  return 0;
}

QVariant
CQChartsModelColumnDetails::
value(int i) const
{
  if      (type() == CQBaseModelType::INTEGER) {
    const auto &value = valueSet_->ivals().value(i);

    return (value ? value.value() : QVariant());
  }
  else if (type() == CQBaseModelType::REAL) {
    const auto &value = valueSet_->rvals().value(i);

    return (value ? value.value() : QVariant());
  }
  else if (type() == CQBaseModelType::STRING) {
    const auto &value = valueSet_->svals().value(i);

    return (value ? value.value() : QVariant());
  }
  else if (type() == CQBaseModelType::TIME) {
    const auto &value = valueSet_->tvals().value(i);

    return (value ? value.value() : QVariant());
  }
  else if (type() == CQBaseModelType::COLOR) {
    const auto &value = valueSet_->cvals().value(i);

    return CQChartsVariant::fromColor(value);
  }

  return QVariant();
}

//---

void
CQChartsModelColumnDetails::
resetTypeInitialized()
{
  std::unique_lock<std::mutex> initLock(initMutex_);

  typeInitialized_ = false;
  initialized_     = false; // dependent on type
}

CQChartsValueSet *
CQChartsModelColumnDetails::
calcValueSet() const
{
  initCache();

  return valueSet_;
}

void
CQChartsModelColumnDetails::
initCache() const
{
  if (! initialized_) {
    if (initializing_.load())
      return;

    std::unique_lock<std::mutex> initLock(initMutex_);

    assert(! initializing_.load());

    if (! initialized_) {
      initializing_.store(true);

      initCache1();

      initializing_.store(false);
    }

    assert(! initializing_.load());
  }
}

void
CQChartsModelColumnDetails::
initCache1() const
{
  auto *th = const_cast<CQChartsModelColumnDetails *>(this);

  (void) th->calcCache();
}

bool
CQChartsModelColumnDetails::
calcCache()
{
  CQPerfTrace trace("CQChartsModelColumnDetails::calcCache");

  assert(! initialized_);

  //---

  if (! valueSet_)
    valueSet_ = new CQChartsValueSet;

  //---

  if (! initType1())
    return false;

  //---

  auto *model = details_->model();

  if (! model)
    return false;

  //---

  if (column_.type() == Column::Type::ROW) {
    initialized_ = true;

    return true;
  }

  //---

  CQChartsModelFilter *modelFilter = nullptr;

  if      (type() == CQBaseModelType::COLOR ||
           type() == CQBaseModelType::SYMBOL) {
    modelFilter = qobject_cast<CQChartsModelFilter *>(model);

    if (modelFilter)
      modelFilter->setMapping(false);
  }
  else if (type() == CQBaseModelType::SYMBOL_SIZE ||
           type() == CQBaseModelType::FONT_SIZE) {
    if (baseType() == CQBaseModelType::REAL ||
        baseType() == CQBaseModelType::INTEGER) {
      modelFilter = qobject_cast<CQChartsModelFilter *>(model);

      if (modelFilter)
        modelFilter->setMapping(false);
    }
  }

  //---

  // TODO: replace monotonic with sorted and sort dir
  // auto update sorted when model sorted

  class DetailVisitor : public CQChartsModelVisitor {
   public:
    DetailVisitor(CQChartsModelColumnDetails *details) :
     details_(details) {
      charts_ = details_->details()->charts();

      auto *columnTypeMgr = charts_->columnTypeMgr();

      const auto *columnType = columnTypeMgr->getType(details_->type());

      if (columnType) {
        min_ = columnType->minValue(details->nameValues()); // type custom min value
        max_ = columnType->maxValue(details->nameValues()); // type custom max value

        visitMin_ = ! min_.isValid();
        visitMax_ = ! max_.isValid();
      }

      monotonicSet_ = false;
      monotonic_    = true;
      increasing_   = true;
    }

    // visit row
    State visit(const QAbstractItemModel *model, const VisitData &data) override {
      bool ok;

      auto var = CQChartsModelUtil::modelValue(
        charts_, model, data.row, details_->column(), data.parent, ok);

      if (ok)
        details_->addValue(var);

      if      (details_->type() == CQBaseModelType::INTEGER) {
        long i = (ok ? CQChartsVariant::toInt(var, ok) : 0);

        if (ok && ! details_->checkRow(int(i)))
          return State::SKIP;

        details_->addInt(i, ok);

        if (ok)
          addInt(i);
      }
      else if (details_->type() == CQBaseModelType::REAL) {
        double r = (ok ? CQChartsVariant::toReal(var, ok) : 0.0);

        if (ok && ! details_->checkRow(r))
          return State::SKIP;

        details_->addReal(r, ok);

        if (ok)
          addReal(r);
      }
      else if (details_->type() == CQBaseModelType::STRING) {
        QString s;

        if (ok)
          ok = CQChartsVariant::toString(var, s);

        if (ok && ! details_->checkRow(s))
          return State::SKIP;

        details_->addString(s);

        addString(s);
      }
      else if (details_->type() == CQBaseModelType::TIME) {
        double t = (ok ? CQChartsVariant::toReal(var, ok) : 0.0);

        if (ok && ! details_->checkRow(t))
          return State::SKIP;

        details_->addTime(t, ok);

        if (ok)
          addReal(t);
      }
      else if (details_->type() == CQBaseModelType::COLOR) {
        CQChartsColor color;

        if (ok)
          ok = details_->columnColor(var, color);

        if (ok && ! details_->checkRow(CQChartsVariant::fromColor(color)))
          return State::SKIP;

        details_->addColor(color, ok);

        if (ok)
          addColor(color);
      }
      else if (details_->type() == CQBaseModelType::SYMBOL_SIZE) {
        double r = (ok ? CQChartsVariant::toReal(var, ok) : 0.0);

        if (ok && ! details_->checkRow(r))
          return State::SKIP;

        details_->addReal(r, ok);

        if (ok)
          addReal(r);
      }
      else if (details_->type() == CQBaseModelType::FONT_SIZE) {
        double r = (ok ? CQChartsVariant::toReal(var, ok) : 0.0);

        if (ok && ! details_->checkRow(r))
          return State::SKIP;

        details_->addReal(r, ok);

        if (ok)
          addReal(r);
      }
      else {
        QString s;

        if (ok)
          ok = CQChartsVariant::toString(var, s);

        if (ok && ! details_->checkRow(s))
          return State::SKIP;

        details_->addString(s);

        addString(s);
      }

      return State::OK;
    }

    void addInt(long i) {
      // if no type defined min, update min value
      if (visitMin_) {
        bool ok1;

        long imin = CQChartsVariant::toInt(min_, ok1);

        imin = (! ok1 ? i : std::min(imin, i));

        min_ = QVariant(int(imin));
      }

      // if no type defined max, update max value
      if (visitMax_) {
        bool ok1;

        long imax = CQChartsVariant::toInt(max_, ok1);

        imax = (! ok1 ? i : std::max(imax, i));

        max_ = QVariant(int(imax));
      }

      if (lastValue1_.isValid() && lastValue2_.isValid()) {
        bool ok1, ok2;

        long i1 = CQChartsVariant::toInt(lastValue1_, ok1);
        long i2 = CQChartsVariant::toInt(lastValue2_, ok2);

        if (! monotonicSet_) {
          if (i1 != i2) {
            increasing_   = (i2 > i1);
            monotonicSet_ = true;
          }
        }
        else {
          if (monotonic_) {
            if (increasing_) {
              if (i2 < i1)
                monotonic_ = false;
            }
            else {
              if (i2 > i1)
                monotonic_ = false;
            }
          }
        }
      }

      lastValue1_ = lastValue2_;
      lastValue2_ = int(i);
    }

    void addReal(double r) {
      // if no type defined min, update min value
      if (visitMin_) {
        bool ok1;

        double rmin = CQChartsVariant::toReal(min_, ok1);

        rmin = (! ok1 ? r : std::min(rmin, r));

        min_ = QVariant(rmin);
      }

      // if no type defined max, update max value
      if (visitMax_) {
        bool ok1;

        double rmax = CQChartsVariant::toReal(max_, ok1);

        rmax = (! ok1 ? r : std::max(rmax, r));

        max_ = QVariant(rmax);
      }

      if (lastValue1_.isValid() && lastValue2_.isValid()) {
        bool ok1, ok2;

        double r1 = CQChartsVariant::toReal(lastValue1_, ok1);
        double r2 = CQChartsVariant::toReal(lastValue2_, ok2);

        if (! monotonicSet_) {
          if (r1 != r2) {
            increasing_   = (r2 > r1);
            monotonicSet_ = true;
          }
        }
        else {
          if (monotonic_) {
            if (increasing_) {
              if (r2 < r1)
                monotonic_ = false;
            }
            else {
              if (r2 > r1)
                monotonic_ = false;
            }
          }
        }
      }

      lastValue1_ = lastValue2_;
      lastValue2_ = r;
    }

    void addString(const QString &s) {
      // if no type defined min, update min value
      if (visitMin_) {
        bool ok1;

        auto smin = CQChartsVariant::toString(min_, ok1);

        smin = (! ok1 ? s : std::min(smin, s));

        min_ = QVariant(smin);
      }

      // if no type defined max, update max value
      if (visitMax_) {
        bool ok1;

        auto smax = CQChartsVariant::toString(max_, ok1);

        smax = (! ok1 ? s : std::max(smax, s));

        max_ = QVariant(smax);
      }

      if (lastValue1_.isValid() && lastValue2_.isValid()) {
        bool ok1, ok2;

        auto s1 = CQChartsVariant::toString(lastValue1_, ok1);
        auto s2 = CQChartsVariant::toString(lastValue2_, ok2);

        if (! monotonicSet_) {
          if (s1 != s2) {
            increasing_   = (s2 > s1);
            monotonicSet_ = true;
          }
        }
        else {
          if (monotonic_) {
            if (increasing_) {
              if (s2 < s1)
                monotonic_ = false;
            }
            else {
              if (s2 > s1)
                monotonic_ = false;
            }
          }
        }
      }

      lastValue1_ = lastValue2_;
      lastValue2_ = s;
    }

    void addColor(const CQChartsColor &c) {
      // if no type defined min, update min value
      if (visitMin_) {
        CQChartsColor cmin;

        if (details_->columnColor(min_, cmin))
          cmin = std::min(cmin, c);
        else
          cmin = c;

        min_ = CQChartsVariant::fromColor(cmin);
      }

      // if no type defined max, update max value
      if (visitMax_) {
        CQChartsColor cmax;

        if (details_->columnColor(max_, cmax))
          cmax = std::max(cmax, c);
        else
          cmax = c;

        max_ = CQChartsVariant::fromColor(cmax);
      }

      lastValue1_ = lastValue2_;
      lastValue2_ = CQChartsVariant::fromColor(c);
    }

    QVariant minValue() const { return min_; }
    QVariant maxValue() const { return max_; }

    bool isMonotonic () const { return monotonicSet_ && monotonic_; }
    bool isIncreasing() const { return increasing_; }

   private:
    CQChartsModelColumnDetails* details_      { nullptr };
    CQCharts*                   charts_       { nullptr };
    QVariant                    min_;
    QVariant                    max_;
    bool                        visitMin_     { true };
    bool                        visitMax_     { true };
    QVariant                    lastValue1_;
    QVariant                    lastValue2_;
    bool                        monotonicSet_ { false };
    bool                        monotonic_    { true };
    bool                        increasing_   { true };
  };

  //---

  DetailVisitor detailVisitor(this);

  auto *charts = details()->charts();

  if (! charts)
    return false;

  CQChartsModelVisit::exec(charts, model, detailVisitor);

  //---

  minValue_   = detailVisitor.minValue();
  maxValue_   = detailVisitor.maxValue();
  numRows_    = detailVisitor.numRows();
  monotonic_  = detailVisitor.isMonotonic();
  increasing_ = detailVisitor.isIncreasing();

  if (modelFilter)
    modelFilter->setMapping(true);

  //---

  initialized_ = true;

  return true;
}

//---

bool
CQChartsModelColumnDetails::
initType() const
{
  bool rc = true;

  if (! typeInitialized_) {
    if (initializing_.load())
      return rc;

    std::unique_lock<std::mutex> initLock(initMutex_);

    assert(! initializing_.load());

    if (! typeInitialized_) {
      initializing_.store(true);

      rc = initType1();

      initializing_.store(false);
    }

    assert(! initializing_.load());
  }

  return rc;
}

bool
CQChartsModelColumnDetails::
initType1() const
{
  if (! typeInitialized_) {
    auto *th = const_cast<CQChartsModelColumnDetails *>(this);

    return th->calcType();
  }

  return true;
}

bool
CQChartsModelColumnDetails::
calcType()
{
  CQPerfTrace trace("CQChartsModelColumnDetails::calcType");

  assert(! typeInitialized_);

  //---

  auto *model = details_->model();
  if (! model) return false;

  if (! column_.isValid())
    return false;

  if (column_.type() == Column::Type::DATA ||
      column_.type() == Column::Type::DATA_INDEX) {
    int icolumn = column_.column();

    int numColumns = details_->numColumns();

    if (icolumn < 0 || icolumn >= numColumns)
      return false;
  }

  //---

  if (column_.type() == Column::Type::ROW) {
    typeData_.type     = CQBaseModelType::INTEGER;
    typeData_.baseType = CQBaseModelType::INTEGER;

    typeInitialized_ = true;

    return true;
  }

  //---

  // get column type and name values
  // TODO: calls CQChartsModelVisitor, integrate into this visitor
  auto *charts = details_->charts();
  if (! charts) return false;

  if (! CQChartsModelUtil::columnValueType(charts, model, column_, typeData_)) {
    typeData_.type     = CQBaseModelType::NONE;
    typeData_.baseType = CQBaseModelType::NONE;
  }

  //---

  const auto *columnType = this->columnType();

  if (columnType) {
    typeName_ = columnType->name();
  }
  else {
    typeData_.type     = CQBaseModelType::STRING;
    typeData_.baseType = CQBaseModelType::STRING;
    typeName_          = "string";
  }

  //---

  if (columnType) {
    const auto &nameValues = this->typeData_.nameValues;

    preferredWidth_ = columnType->preferredWidth(nameValues);

    nullValue_ = columnType->nullValue(nameValues);

    tableDrawColor_ = columnType->drawColor(nameValues);
    tableDrawType_  = columnType->drawType (nameValues);
    tableDrawStops_ = columnType->drawStops(nameValues);

  //namedColors_ = columnType->namedColors(nameValues);
  //namedImages_ = columnType->namedImages(nameValues);
  }

  //---

  typeInitialized_ = true;

  return true;
}

//---

void
CQChartsModelColumnDetails::
initBucketer() const
{
  if (! bucketer_) {
    if (bucketing_.load())
      return;

    std::unique_lock<std::mutex> initLock(bucketMutex_);

    assert(! bucketing_.load());

    if (! bucketer_) {
      bucketing_.store(true);

      auto *th = const_cast<CQChartsModelColumnDetails *>(this);

      th->calcBucketer();

      bucketing_.store(false);
    }

    assert(! bucketing_.load());
  }
}

void
CQChartsModelColumnDetails::
calcBucketer()
{
  bucketer_ = new CQBucketer;

  int num = numBuckets();

  if      (type() == CQBaseModelType::INTEGER) {
    bucketer_->setType(CQBucketer::Type::INTEGER_RANGE);

    int i1 = minValue().toInt();
    int i2 = maxValue().toInt();

    int n = i2 - i1 + 1;
    int d = n/num;

    if (d <= 0) d = 1;

    bucketer_->setIStart(i1);
    bucketer_->setIDelta(d);
    bucketer_->setNumAuto(num);
  }
  else if (type() == CQBaseModelType::REAL) {
    bucketer_->setType(CQBucketer::Type::REAL_AUTO);

    double r1 = minValue().toDouble();
    double r2 = maxValue().toDouble();

    bucketer_->setRMin(r1);
    bucketer_->setRMax(r2);
    bucketer_->setNumAuto(num);
  }
}

//---

void
CQChartsModelColumnDetails::
addInt(long i, bool ok)
{
  if (ok)
    valueSet_->ivals().addValue(i);
  else
    valueSet_->ivals().addValue(CQChartsIValues::OptInt());
}

void
CQChartsModelColumnDetails::
addReal(double r, bool ok)
{
  if (ok && ! CMathUtil::isNaN(r))
    valueSet_->rvals().addValue(r);
  else
    valueSet_->rvals().addValue(CQChartsRValues::OptReal());
}

void
CQChartsModelColumnDetails::
addString(const QString &s)
{
  valueSet_->svals().addValue(s);
}

void
CQChartsModelColumnDetails::
addTime(double t, bool ok)
{
  if (ok)
    valueSet_->tvals().addValue(t);
}

void
CQChartsModelColumnDetails::
addColor(const CQChartsColor &c, bool ok)
{
  if (ok)
    valueSet_->cvals().addValue(c);
}

void
CQChartsModelColumnDetails::
addValue(const QVariant &value)
{
  // add unique value
  auto p = valueInds_.find(value);

  if (p == valueInds_.end()) {
    int ind = valueInds_.size();

    (void) valueInds_.insert(p, VariantInds::value_type(value, ind));
  }
}

bool
CQChartsModelColumnDetails::
columnColor(const QVariant &var, CQChartsColor &color) const
{
  auto *charts = details_->charts();
  if (! charts) return false;

  auto *model = details_->model();

  auto *columnTypeMgr = charts->columnTypeMgr();

  const auto *colorType =
    dynamic_cast<const CQChartsColumnColorType *>(columnTypeMgr->getType(CQBaseModelType::COLOR));

  bool converted;

  auto cvar = colorType->userData(charts, model, column_, var, typeData(), converted);

  bool ok;

  color = CQChartsVariant::toColor(cvar, ok);

  if (! ok)
    return false;

  return true;
}

bool
CQChartsModelColumnDetails::
columnNameValue(const QString &name, QString &value) const
{
  const auto *columnType = this->columnType();
  if (! columnType) return false;

  if (! columnType->nameValueString(nameValues(), name, value))
    return false;

  return true;
}

const CQChartsColumnType *
CQChartsModelColumnDetails::
columnType() const
{
  auto *charts = details_->charts();
  if (! charts) return nullptr;

  auto *columnTypeMgr = charts->columnTypeMgr();

  return columnTypeMgr->getType(typeData_.type);
}

//---

QColor
CQChartsModelColumnDetails::
heatmapColor(double r, double min, double max, const QColor &bg) const
{
  initType();

  auto c = bg;

  auto *charts = details_->charts();
  if (! charts) return c;

  double norm = (max > min ? (r - min)/(max - min) : 0.0);

  const auto &drawColor = this->tableDrawColor();

  if (drawColor.isValid()) {
    if (drawColor.isDirect()) {
      // blend fixed color with background color using normalized value
      CQChartsUtil::ColorInd colorInd;

      auto bg1 = charts->interpColor(drawColor, colorInd);
      auto bg2 = bg;

      c = CQChartsUtil::blendColors(bg1, bg2, norm);
    }
    else {
      const auto &drawStops = this->tableDrawStops();

      bool hasStops = drawStops.isValid();
      bool relative = (hasStops ? drawStops.isPercent() : true);

      if (hasStops) {
        int ind = drawStops.ind(relative ? norm : r);

        CQChartsUtil::ColorInd colorInd(ind, drawStops.size() + 1);

        c = charts->interpColor(drawColor, colorInd);
      }
      else {
        // use interpolated color directly
        CQChartsUtil::ColorInd colorInd(norm);

        c = charts->interpColor(drawColor, colorInd);
      }
    }
  }
  else {
    // blend default color (red) with background color using normalized value
    auto bg1 = QColor(Qt::red);
    auto bg2 = bg;

    c = CQChartsUtil::blendColors(bg1, bg2, norm);
  }

  return c;
}

QColor
CQChartsModelColumnDetails::
barchartColor() const
{
  initType();

  auto c = QColor(160, 160, 160); // gray. TODO: config

  auto *charts = details_->charts();
  if (! charts) return c;

  const auto &drawColor = this->tableDrawColor();

  if (drawColor.isValid()) {
    CQChartsUtil::ColorInd colorInd;

    c = charts->interpColor(drawColor, colorInd);
  }

  return c;
}

bool
CQChartsModelColumnDetails::
namedColor(const QString &name, Color &color) const
{
  initType();

  const auto *columnType = this->columnType();
  if (! columnType) return false;

  const auto &nameValues  = typeData_.nameValues;
  const auto &namedColors = columnType->namedColors(nameValues);

  QVariant value;

  if (! namedColors.nameValue(name, value))
    return false;

  bool ok;
  auto color1 = CQChartsVariant::toColor(value, ok);
  if (! ok) return false;

  color = color1;

  return true;
}

bool
CQChartsModelColumnDetails::
namedImage(const QString &name, CQChartsImage &image) const
{
  initType();

  const auto *columnType = this->columnType();
  if (! columnType) return false;

  auto *charts = details_->charts();

  const auto &nameValues  = typeData_.nameValues;
  const auto &namedImages = columnType->namedImages(charts, nameValues);

  QVariant value;

  if (! namedImages.nameValue(name, value))
    return false;

  bool ok;
  auto image1 = CQChartsVariant::toImage(value, ok);
  if (! ok) return false;

  image = image1;

  return true;
}
