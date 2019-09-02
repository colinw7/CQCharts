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
  CQChartsModelDetails *th = const_cast<CQChartsModelDetails *>(this);

  return th->columnDetails(c);
}

CQChartsModelColumnDetails *
CQChartsModelDetails::
columnDetails(const CQChartsColumn &c)
{
  std::unique_lock<std::mutex> lock(mutex_);

  auto p = columnDetails_.find(c);

  if (p == columnDetails_.end()) {
    CQChartsModelColumnDetails *details = new CQChartsModelColumnDetails(this, c);

    p = columnDetails_.insert(p, ColumnDetails::value_type(c, details));
  }

  CQChartsModelColumnDetails *details = (*p).second;

  return details;
}

void
CQChartsModelDetails::
initSimpleData() const
{
  if (initialized_ == Initialized::NONE) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (initialized_ == Initialized::NONE) {
      CQChartsModelDetails *th = const_cast<CQChartsModelDetails *>(this);

      th->updateSimple();
    }
  }
}

void
CQChartsModelDetails::
initFullData() const
{
  if (initialized_ != Initialized::FULL) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (initialized_ != Initialized::FULL) {
      CQChartsModelDetails *th = const_cast<CQChartsModelDetails *>(this);

      th->updateFull();
    }
  }
}

void
CQChartsModelDetails::
reset()
{
  {
  std::unique_lock<std::mutex> lock(mutex_);

  resetValues();
  }

  emit detailsReset();
}

void
CQChartsModelDetails::
resetValues()
{
  initialized_  = Initialized::NONE;
  numColumns_   = 0;
  numRows_      = 0;
  hierarchical_ = false;

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

  QAbstractItemModel *model = this->model();

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

  resetValues();

  if (initialized_ == Initialized::NONE)
    updateSimple();

  for (int c = 0; c < numColumns_; ++c) {
    CQChartsModelColumnDetails *columnDetails = this->columnDetails(c);

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
      CQChartsModelColumnDetails *columnDetails = this->columnDetails(c);

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
    const CQChartsModelColumnDetails *columnDetails = this->columnDetails(c);

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
    const CQChartsModelColumnDetails *columnDetails = this->columnDetails(c);

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

  CQCharts *charts = this->charts();
  if (! charts) return rows;

  QAbstractItemModel *model = this->model();

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

        QVariant var = CQChartsModelUtil::modelValue(charts, model, r, c, parent, ok);

        rowValues2[c] = var;

        if (rowValues1.empty() || var != rowValues1[c])
          match = false;
      }
    }
    else {
      QModelIndex parent;

      bool ok;

      QVariant var = CQChartsModelUtil::modelValue(charts, model, r, column, parent, ok);

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

  valueSet_ = new CQChartsValueSet;
}

CQChartsModelColumnDetails::
~CQChartsModelColumnDetails()
{
  delete valueSet_;
}

bool
CQChartsModelColumnDetails::
isNamedValue(const QString &name)
{
  return getLongNamedValues().contains(name) ||
         getShortNamedValues().contains(name);
}

const QStringList &
CQChartsModelColumnDetails::
getLongNamedValues()
{
  static QStringList namedValues = QStringList() <<
   "name" << "type" << "minimum" << "maximum" << "average" << "standard_deviation" <<
   "monotonic" << "increasing" << "num_unique" << "unique_values" << "unique_counts" <<
   "num_null" << "median" << "lower_median" << "upper_median" <<
   "outliers";

  return namedValues;
}

const QStringList &
CQChartsModelColumnDetails::
getShortNamedValues()
{
  static QStringList namedValues = QStringList() <<
   "min" << "max" << "mean" << "avg" << "stddev" << "std_dev";

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
  else if (name == "stddev" || name == "std_dev" || name == "standard_deviation")
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
  QAbstractItemModel *model = details_->model();

  bool ok;

  return CQChartsModelUtil::modelHeaderString(model, column_, ok);
}

bool
CQChartsModelColumnDetails::
isKey() const
{
  QAbstractItemModel *model = details_->model();

  bool ok;

  QVariant value =
    CQChartsModelUtil::modelHeaderValue(
      model, column_, static_cast<int>(CQBaseModelRole::Key), ok);

  return (ok && value.toBool());
}

QString
CQChartsModelColumnDetails::
typeName() const
{
  initType();

  return typeName_;
}

CQBaseModelType
CQChartsModelColumnDetails::
type() const
{
  initType();

  return type_;
}

void
CQChartsModelColumnDetails::
setType(CQBaseModelType type)
{
  type_ = type;

  if (baseType_ == CQBaseModelType::NONE)
    baseType_ = type_;
}

CQBaseModelType
CQChartsModelColumnDetails::
baseType() const
{
  initType();

  return baseType_;
}

void
CQChartsModelColumnDetails::
setBaseType(CQBaseModelType type)
{
  baseType_ = type;
}

const CQChartsNameValues &
CQChartsModelColumnDetails::
nameValues() const
{
  initType();

  return nameValues_;
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
  initCache();

  if      (type() == CQBaseModelType::INTEGER) {
    return valueSet_->ivals().mean();
  }
  else if (type() == CQBaseModelType::REAL) {
    return valueSet_->rvals().mean();
  }
  else if (type() == CQBaseModelType::STRING) {
    return (useNaN ? QVariant(CMathUtil::getNaN()) : QVariant());
  }
  else if (type() == CQBaseModelType::TIME) {
    return valueSet_->tvals().mean();
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
  initCache();

  if      (type() == CQBaseModelType::INTEGER) {
    return valueSet_->ivals().stddev();
  }
  else if (type() == CQBaseModelType::REAL) {
    return valueSet_->rvals().stddev();
  }
  else if (type() == CQBaseModelType::STRING) {
    return (useNaN ? QVariant(CMathUtil::getNaN()) : QVariant());
  }
  else if (type() == CQBaseModelType::TIME) {
    return valueSet_->tvals().stddev();
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

  CQCharts *charts = details_->charts();
  if (! charts) return QVariant();

  const CQChartsColumnType *columnType = this->columnType();

  if (! columnType)
    return v;

  QAbstractItemModel *model = details_->model();

  bool converted;

  QVariant v1 = columnType->dataName(charts, model, column_, v, nameValues_, converted);

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
  if (column_.type() == CQChartsColumn::Type::DATA ||
      column_.type() == CQChartsColumn::Type::DATA_INDEX) {
    int icolumn = column_.column();

    bool ok;

    QAbstractItemModel *model = details_->model();

    QVariant var = CQChartsModelUtil::modelHeaderValue(
      model, icolumn, static_cast<int>(CQBaseModelRole::Sorted), ok);

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
  if (column_.type() == CQChartsColumn::Type::DATA ||
      column_.type() == CQChartsColumn::Type::DATA_INDEX) {
    int icolumn = column_.column();

    bool ok;

    QAbstractItemModel *model = details_->model();

    QVariant var = CQChartsModelUtil::modelHeaderValue(
      model, icolumn, static_cast<int>(CQBaseModelRole::Sorted), ok);

    if (ok && var.isValid() && var.toBool()) {
      QVariant var = CQChartsModelUtil::modelHeaderValue(
        model, icolumn, static_cast<int>(CQBaseModelRole::SortOrder), ok);

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

int
CQChartsModelColumnDetails::
numUnique() const
{
  initCache();

  if      (type() == CQBaseModelType::INTEGER) {
    return valueSet_->ivals().numUnique();
  }
  else if (type() == CQBaseModelType::REAL) {
    return valueSet_->rvals().numUnique();
  }
  else if (type() == CQBaseModelType::STRING) {
    return valueSet_->svals().numUnique();
  }
  else if (type() == CQBaseModelType::TIME) {
    return valueSet_->tvals().numUnique();
  }
  else if (type() == CQBaseModelType::COLOR) {
    return valueSet_->cvals().numUnique();
  }
  else {
    return 0;
  }
}

CQChartsModelColumnDetails::VariantList
CQChartsModelColumnDetails::
uniqueValues() const
{
  initCache();

  VariantList vars;

  if      (type() == CQBaseModelType::INTEGER) {
    CQChartsIValues::Values values;

    valueSet_->ivals().uniqueValues(values);

    for (const auto &v : values)
      vars.push_back(v);
  }
  else if (type() == CQBaseModelType::REAL) {
    CQChartsRValues::Values values;

    valueSet_->rvals().uniqueValues(values);

    for (const auto &v : values)
      vars.push_back(v);
  }
  else if (type() == CQBaseModelType::STRING) {
    CQChartsSValues::Values values;

    valueSet_->svals().uniqueValues(values);

    for (const auto &v : values)
      vars.push_back(v);
  }
  else if (type() == CQBaseModelType::TIME) {
    CQChartsRValues::Values values;

    valueSet_->tvals().uniqueValues(values);

    for (const auto &v : values)
      vars.push_back(v);
  }
  else if (type() == CQBaseModelType::COLOR) {
    CQChartsCValues::Values values;

    valueSet_->cvals().uniqueValues(values);

    for (const auto &v : values)
      vars.push_back(QVariant::fromValue<CQChartsColor>(v));
  }

  return vars;
}

CQChartsModelColumnDetails::VariantList
CQChartsModelColumnDetails::
uniqueCounts() const
{
  initCache();

  CQChartsIValues::Counts counts;

  if      (type() == CQBaseModelType::INTEGER) {
    valueSet_->ivals().uniqueCounts(counts);
  }
  else if (type() == CQBaseModelType::REAL) {
    valueSet_->rvals().uniqueCounts(counts);
  }
  else if (type() == CQBaseModelType::STRING) {
    valueSet_->svals().uniqueCounts(counts);
  }
  else if (type() == CQBaseModelType::TIME) {
    valueSet_->tvals().uniqueCounts(counts);
  }
  else if (type() == CQBaseModelType::COLOR) {
    valueSet_->cvals().uniqueCounts(counts);
  }

  VariantList vars;

  for (const auto &c : counts)
    vars.push_back(c);

  return vars;
}

int
CQChartsModelColumnDetails::
uniqueId(const QVariant &var) const
{
  initCache();

  bool ok;

  if      (type() == CQBaseModelType::INTEGER) {
    long i = CQChartsVariant::toInt(var, ok);

    if (ok)
      return valueSet_->ivals().id(i);
  }
  else if (type() == CQBaseModelType::REAL) {
    double r = CQChartsVariant::toReal(var, ok);

    return valueSet_->rvals().id(r);
  }
  else if (type() == CQBaseModelType::STRING) {
    return valueSet_->svals().id(var.toString());
  }
  else if (type() == CQBaseModelType::TIME) {
    double r = CQChartsVariant::toReal(var, ok);

    return valueSet_->tvals().id(r);
  }
  else if (type() == CQBaseModelType::COLOR) {
    bool ok;

    CQChartsColor color = CQChartsVariant::toColor(var, ok);

    if (ok)
      return valueSet_->cvals().id(color);
  }

  return -1;
}

QVariant
CQChartsModelColumnDetails::
uniqueValue(int i) const
{
  VariantList uniqueValues = this->uniqueValues();

  if (i >= 0 && i < uniqueValues.size())
    return uniqueValues[i];

  return QVariant();
}

int
CQChartsModelColumnDetails::
numNull() const
{
  initCache();

  if      (type() == CQBaseModelType::INTEGER) {
    return valueSet_->ivals().numNull();
  }
  else if (type() == CQBaseModelType::REAL) {
    return valueSet_->rvals().numNull();
  }
  else if (type() == CQBaseModelType::STRING) {
    return 0;
  }
  else if (type() == CQBaseModelType::TIME) {
    return valueSet_->tvals().numNull();
  }
  else if (type() == CQBaseModelType::COLOR) {
    return valueSet_->cvals().numNull();
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
  initCache();

  if      (type() == CQBaseModelType::INTEGER) {
    return valueSet_->ivals().median();
  }
  else if (type() == CQBaseModelType::REAL) {
    return valueSet_->rvals().median();
  }
  else if (type() == CQBaseModelType::STRING) {
    return (useNaN ? QVariant(CMathUtil::getNaN()) : QVariant());
  }
  else if (type() == CQBaseModelType::TIME) {
    return valueSet_->tvals().median();
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
  initCache();

  if      (type() == CQBaseModelType::INTEGER) {
    return valueSet_->ivals().lowerMedian();
  }
  else if (type() == CQBaseModelType::REAL) {
    return valueSet_->rvals().lowerMedian();
  }
  else if (type() == CQBaseModelType::STRING) {
    return (useNaN ? QVariant(CMathUtil::getNaN()) : QVariant());
  }
  else if (type() == CQBaseModelType::TIME) {
    return valueSet_->tvals().lowerMedian();
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
  initCache();

  if      (type() == CQBaseModelType::INTEGER) {
    return valueSet_->ivals().upperMedian();
  }
  else if (type() == CQBaseModelType::REAL) {
    return valueSet_->rvals().upperMedian();
  }
  else if (type() == CQBaseModelType::STRING) {
    return (useNaN ? QVariant(CMathUtil::getNaN()) : QVariant());
  }
  else if (type() == CQBaseModelType::TIME) {
    return valueSet_->tvals().upperMedian();
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
  initCache();

  QVariantList vars;

  if      (type() == CQBaseModelType::INTEGER) {
    const CQChartsIValues::Indices &outliers = valueSet_->ivals().outliers();

    for (auto &o : outliers)
      vars.push_back(valueSet_->ivals().svalue(o));
  }
  else if (type() == CQBaseModelType::REAL) {
    const CQChartsRValues::Indices &outliers = valueSet_->rvals().outliers();

    for (auto &o : outliers)
      vars.push_back(valueSet_->rvals().svalue(o));
  }
  else if (type() == CQBaseModelType::STRING) {
    return vars;
  }
  else if (type() == CQBaseModelType::TIME) {
    const CQChartsIValues::Indices &outliers = valueSet_->tvals().outliers();

    for (auto &o : outliers)
      vars.push_back(valueSet_->tvals().svalue(o));
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
  initCache();

  bool ok;

  if      (type() == CQBaseModelType::INTEGER) {
    long i = CQChartsVariant::toInt(value, ok);

    return (ok && valueSet_->ivals().isOutlier(i));
  }
  else if (type() == CQBaseModelType::REAL) {
    double r = CQChartsVariant::toReal(value, ok);

    return (ok && valueSet_->rvals().isOutlier(r));
  }
  else if (type() == CQBaseModelType::STRING) {
    return false;
  }
  else if (type() == CQBaseModelType::TIME) {
    double r = CQChartsVariant::toReal(value, ok);

    return (ok && valueSet_->tvals().isOutlier(r));
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
  initCache();

  bool ok;

  if      (type() == CQBaseModelType::INTEGER) {
    long i = CQChartsVariant::toInt(var, ok);

    if (ok)
      return valueSet_->ivals().map(i);
  }
  else if (type() == CQBaseModelType::REAL) {
    double r = CQChartsVariant::toReal(var, ok);

    return valueSet_->rvals().map(r);
  }
  else if (type() == CQBaseModelType::STRING) {
    return 0;
  }
  else if (type() == CQBaseModelType::TIME) {
    double r = CQChartsVariant::toReal(var, ok);

    return valueSet_->tvals().map(r);
  }
  else if (type() == CQBaseModelType::COLOR) {
    return 0;
  }

  return 0;
}

void
CQChartsModelColumnDetails::
initCache() const
{
  if (! initialized_) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (! initialized_) {
      CQChartsModelColumnDetails *th = const_cast<CQChartsModelColumnDetails *>(this);

      (void) th->initData();
    }
  }
}

bool
CQChartsModelColumnDetails::
initData()
{
  CQPerfTrace trace("CQChartsModelColumnDetails::initData");

  assert(! initialized_);

  //---

  if (! typeInitialized_) {
    if (! calcType())
      return false;
  }

  //---

  QAbstractItemModel *model = details_->model();
  if (! model) return false;

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

      CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

      const CQChartsColumnType *columnType = columnTypeMgr->getType(details_->type());

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

      QVariant var = CQChartsModelUtil::modelValue(
        charts_, model, data.row, details_->column(), data.parent, ok);
      if (! ok) return State::SKIP;

      details_->addValue(var);

      if      (details_->type() == CQBaseModelType::INTEGER) {
        long i = CQChartsVariant::toInt(var, ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(int(i)))
          return State::SKIP;

        details_->addInt((int) i);

        addInt(i);
      }
      else if (details_->type() == CQBaseModelType::REAL) {
        double r = CQChartsVariant::toReal(var, ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(r))
          return State::SKIP;

        details_->addReal(r);

        addReal(r);
      }
      else if (details_->type() == CQBaseModelType::STRING) {
        QString s;

        ok = CQChartsVariant::toString(var, s);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(s))
          return State::SKIP;

        details_->addString(s);

        addString(s);
      }
      else if (details_->type() == CQBaseModelType::TIME) {
        double t = CQChartsVariant::toReal(var, ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(t))
          return State::SKIP;

        details_->addTime(t);

        addReal(t);
      }
      else if (details_->type() == CQBaseModelType::COLOR) {
        CQChartsColor color;

        if (! details_->columnColor(var, color))
          return State::SKIP;

        if (! details_->checkRow(QVariant::fromValue<CQChartsColor>(color)))
          return State::SKIP;

        details_->addColor(color);

        addColor(color);
      }
      else if (details_->type() == CQBaseModelType::SYMBOL_SIZE) {
        double r = CQChartsVariant::toReal(var, ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(r))
          return State::SKIP;

        details_->addReal(r);

        addReal(r);
      }
      else if (details_->type() == CQBaseModelType::FONT_SIZE) {
        double r = CQChartsVariant::toReal(var, ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(r))
          return State::SKIP;

        details_->addReal(r);

        addReal(r);
      }
      else {
        QString s;

        ok = CQChartsVariant::toString(var, s);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(s))
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

        QString smin = CQChartsVariant::toString(min_, ok1);

        smin = (! ok1 ? s : std::min(smin, s));

        min_ = QVariant(smin);
      }

      // if no type defined max, update max value
      if (visitMax_) {
        bool ok1;

        QString smax = CQChartsVariant::toString(max_, ok1);

        smax = (! ok1 ? s : std::max(smax, s));

        max_ = QVariant(smax);
      }

      if (lastValue1_.isValid() && lastValue2_.isValid()) {
        bool ok1, ok2;

        QString s1 = CQChartsVariant::toString(lastValue1_, ok1);
        QString s2 = CQChartsVariant::toString(lastValue2_, ok2);

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

        min_ = QVariant::fromValue<CQChartsColor>(cmin);
      }

      // if no type defined max, update max value
      if (visitMax_) {
        CQChartsColor cmax;

        if (details_->columnColor(max_, cmax))
          cmax = std::max(cmax, c);
        else
          cmax = c;

        max_ = QVariant::fromValue<CQChartsColor>(cmax);
      }

      lastValue1_ = lastValue2_;
      lastValue2_ = QVariant::fromValue<CQChartsColor>(c);
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

  CQCharts *charts = details()->charts();
  if (! charts) return false;

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

void
CQChartsModelColumnDetails::
initType() const
{
  if (! typeInitialized_) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (! typeInitialized_) {
      CQChartsModelColumnDetails *th = const_cast<CQChartsModelColumnDetails *>(this);

      (void) th->calcType();
    }
  }
}

bool
CQChartsModelColumnDetails::
calcType()
{
  CQPerfTrace trace("CQChartsModelColumnDetails::calcType");

  assert(! typeInitialized_);

  //---

  QAbstractItemModel *model = details_->model();
  if (! model) return false;

  if (! column_.isValid())
    return false;

  if (column_.type() == CQChartsColumn::Type::DATA ||
      column_.type() == CQChartsColumn::Type::DATA_INDEX) {
    int icolumn = column_.column();

    int numColumns = details_->numColumns();

    if (icolumn < 0 || icolumn >= numColumns)
      return false;
  }

  //---

  // get column type and name values
  // TODO: calls CQChartsModelVisitor, integrate into this visitor
  CQCharts *charts = details_->charts();
  if (! charts) return false;

  if (! CQChartsModelUtil::columnValueType(charts, model, column_, type_,
                                           baseType_, nameValues_)) {
    type_     = CQBaseModelType::NONE;
    baseType_ = CQBaseModelType::NONE;
  }

  //---

  const CQChartsColumnType *columnType = this->columnType();

  if (columnType) {
    typeName_ = columnType->name();
  }
  else {
    type_     = CQBaseModelType::STRING;
    baseType_ = CQBaseModelType::STRING;
    typeName_ = "string";
  }

  //---

  tableDrawColor_ = columnType->drawColor(nameValues_);
  tableDrawType_  = columnType->drawType (nameValues_);
  tableDrawStops_ = columnType->drawStops(nameValues_);

  //---

  typeInitialized_ = true;

  return true;
}

void
CQChartsModelColumnDetails::
addInt(int i)
{
  valueSet_->ivals().addValue(i);
}

void
CQChartsModelColumnDetails::
addReal(double r)
{
  if (! CMathUtil::isNaN(r))
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
addTime(double t)
{
  valueSet_->tvals().addValue(t);
}

void
CQChartsModelColumnDetails::
addColor(const CQChartsColor &c)
{
  valueSet_->cvals().addValue(c);
}

void
CQChartsModelColumnDetails::
addValue(const QVariant &value)
{
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
  CQCharts *charts = details_->charts();
  if (! charts) return false;

  QAbstractItemModel *model = details_->model();

  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  const CQChartsColumnColorType *colorType =
    dynamic_cast<const CQChartsColumnColorType *>(columnTypeMgr->getType(CQBaseModelType::COLOR));

  bool converted;

  QVariant cvar = colorType->userData(charts, model, column_, var, nameValues_, converted);

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
  const CQChartsColumnType *columnType = this->columnType();

  if (! columnType)
    return false;

  if (! columnType->nameValueString(nameValues_, name, value))
    return false;

  return true;
}

const CQChartsColumnType *
CQChartsModelColumnDetails::
columnType() const
{
  CQCharts *charts = details_->charts();
  if (! charts) return nullptr;

  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  return columnTypeMgr->getType(type_);
}
