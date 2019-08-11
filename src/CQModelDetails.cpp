#include <CQModelDetails.h>
#include <CQModelVisitor.h>
#include <CQBaseModel.h>
#include <CQModelUtil.h>
#include <CQValueSet.h>
//#include <CQPerfMonitor.h>

#include <QAbstractItemModel>

CQModelDetails::
CQModelDetails(QAbstractItemModel *model) :
 model_(model)
{
}

CQModelDetails::
~CQModelDetails()
{
  resetValues();
}

int
CQModelDetails::
numColumns() const
{
  initSimpleData();

  return numColumns_;
}

int
CQModelDetails::
numRows() const
{
  initSimpleData();

  return numRows_;
}

bool
CQModelDetails::
isHierarchical() const
{
  initSimpleData();

  return hierarchical_;
}

const CQModelColumnDetails *
CQModelDetails::
columnDetails(int c) const
{
  CQModelDetails *th = const_cast<CQModelDetails *>(this);

  return th->columnDetails(c);
}

CQModelColumnDetails *
CQModelDetails::
columnDetails(int c)
{
  std::unique_lock<std::mutex> lock(mutex_);

  auto p = columnDetails_.find(c);

  if (p == columnDetails_.end()) {
    CQModelColumnDetails *details = new CQModelColumnDetails(this, c);

    p = columnDetails_.insert(p, ColumnDetails::value_type(c, details));
  }

  CQModelColumnDetails *details = (*p).second;

  return details;
}

void
CQModelDetails::
initSimpleData() const
{
  if (initialized_ == Initialized::NONE) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (initialized_ == Initialized::NONE) {
      CQModelDetails *th = const_cast<CQModelDetails *>(this);

      th->updateSimple();
    }
  }
}

void
CQModelDetails::
initFullData() const
{
  if (initialized_ != Initialized::FULL) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (initialized_ != Initialized::FULL) {
      CQModelDetails *th = const_cast<CQModelDetails *>(this);

      th->updateFull();
    }
  }
}

void
CQModelDetails::
reset()
{
  {
  std::unique_lock<std::mutex> lock(mutex_);

  resetValues();
  }

  emit detailsReset();
}

void
CQModelDetails::
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
CQModelDetails::
updateSimple()
{
  //CQPerfTrace trace("CQModelDetails::updateSimple");

  assert(initialized_ == Initialized::NONE);

  hierarchical_ = CQModelUtil::isHierarchical(model());

  numColumns_ = model()->columnCount();
  numRows_    = model()->rowCount   ();

  initialized_ = Initialized::SIMPLE;
}

void
CQModelDetails::
updateFull()
{
  //CQPerfTrace trace("CQModelDetails::updateFull");

  assert(initialized_ != Initialized::FULL);

  resetValues();

  if (initialized_ == Initialized::NONE)
    updateSimple();

  for (int c = 0; c < numColumns_; ++c) {
    CQModelColumnDetails *columnDetails = this->columnDetails(c);

    numRows_ = std::max(numRows_, columnDetails->numRows());
  }

  initialized_ = Initialized::FULL;
}

CQModelDetails::Columns
CQModelDetails::
numericColumns() const
{
  initSimpleData();

  CQModelDetails::Columns columns;

  for (int c = 0; c < numColumns_; ++c) {
    const CQModelColumnDetails *columnDetails = this->columnDetails(c);

    if (columnDetails->isNumeric())
      columns.push_back(columnDetails->column());
  }

  return columns;
}

CQModelDetails::Columns
CQModelDetails::
monotonicColumns() const
{
  initSimpleData();

  CQModelDetails::Columns columns;

  for (int c = 0; c < numColumns_; ++c) {
    const CQModelColumnDetails *columnDetails = this->columnDetails(c);

    if (columnDetails->isMonotonic())
      columns.push_back(columnDetails->column());
  }

  return columns;
}

std::vector<int>
CQModelDetails::
duplicates(int column) const
{
  return columnDuplicates(column, false);
}

std::vector<int>
CQModelDetails::
duplicates() const
{
  return columnDuplicates(-1, true);
}

std::vector<int>
CQModelDetails::
columnDuplicates(int column, bool all) const
{
  initSimpleData();

  //---

  std::vector<QVariant> rowValues1, rowValues2;

  if (all)
    rowValues2.resize(numColumns_);
  else
    rowValues2.resize(1);

  std::vector<int> rows;

  for (int r = 0; r < numRows_; ++r) {
    bool match = true;

    if (all) {
      for (int c = 0; c < numColumns_; ++c) {
        QModelIndex parent;

        bool ok;

        QVariant var = CQModelUtil::modelValue(model(), r, c, parent, ok);

        rowValues2[c] = var;

        if (rowValues1.empty() || var != rowValues1[c])
          match = false;
      }
    }
    else {
      QModelIndex parent;

      bool ok;

      QVariant var = CQModelUtil::modelValue(model(), r, column, parent, ok);

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

//------

CQModelColumnDetails::
CQModelColumnDetails(CQModelDetails *details, int column) :
 details_(details), column_(column)
{
  assert(details_);

  valueSet_ = new CQValueSet;
}

CQModelColumnDetails::
~CQModelColumnDetails()
{
  delete valueSet_;
}

QAbstractItemModel *
CQModelColumnDetails::
model() const
{
  return details()->model();
}

bool
CQModelColumnDetails::
isNamedValue(const QString &name)
{
  return getLongNamedValues().contains(name) ||
         getShortNamedValues().contains(name);
}

const QStringList &
CQModelColumnDetails::
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
CQModelColumnDetails::
getShortNamedValues()
{
  static QStringList namedValues = QStringList() <<
   "min" << "max" << "mean" << "avg" << "stddev" << "std_dev";

  return namedValues;
}

QVariant
CQModelColumnDetails::
getNamedValue(const QString &name) const
{
  if      (name == "name")
    return this->headerName();
  else if (name == "type")
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
CQModelColumnDetails::
headerName() const
{
  bool ok;

  return CQModelUtil::modelHeaderString(model(), column_, ok);
}

bool
CQModelColumnDetails::
isKey() const
{
  bool ok;

  QVariant value =
    CQModelUtil::modelHeaderValue(model(), column_, CQBaseModelRole::Key, ok);

  return (ok && value.toBool());
}

CQBaseModelType
CQModelColumnDetails::
type() const
{
  initType();

  return type_;
}

void
CQModelColumnDetails::
setType(CQBaseModelType type)
{
  type_ = type;
}

QVariant
CQModelColumnDetails::
minValue() const
{
  initCache();

  return minValue_;
}

QVariant
CQModelColumnDetails::
maxValue() const
{
  initCache();

  return maxValue_;
}

QVariant
CQModelColumnDetails::
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

  return QVariant();
}

QVariant
CQModelColumnDetails::
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

  return QVariant();
}

int
CQModelColumnDetails::
numRows() const
{
  initCache();

  return numRows_;
}

bool
CQModelColumnDetails::
isNumeric() const
{
  if (type() == CQBaseModelType::INTEGER ||
      type() == CQBaseModelType::REAL)
    return true;

  return false;
}

bool
CQModelColumnDetails::
isMonotonic() const
{
  bool ok;

  QVariant var = CQModelUtil::modelHeaderValue(model(), column_, CQBaseModelRole::Sorted, ok);

  if (ok && var.isValid() && var.toBool())
    return true;

  //---

  initCache();

  return monotonic_;
}

bool
CQModelColumnDetails::
isIncreasing() const
{
  bool ok;

  QVariant var = CQModelUtil::modelHeaderValue(model(), column_, CQBaseModelRole::Sorted, ok);

  if (ok && var.isValid() && var.toBool()) {
    QVariant var = CQModelUtil::modelHeaderValue(model(), column_, CQBaseModelRole::SortOrder, ok);

    if (ok && var.isValid()) {
      bool ok;

      long order = var.toInt(&ok);

      return (ok && (order == Qt::AscendingOrder));
    }
  }

  //---

  initCache();

  return increasing_;
}

int
CQModelColumnDetails::
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
  else {
    return 0;
  }
}

CQModelColumnDetails::VariantList
CQModelColumnDetails::
uniqueValues() const
{
  initCache();

  VariantList vars;

  if      (type() == CQBaseModelType::INTEGER) {
    CQIValues::Values values;

    valueSet_->ivals().uniqueValues(values);

    for (const auto &v : values)
      vars.push_back(v);
  }
  else if (type() == CQBaseModelType::REAL) {
    CQRValues::Values values;

    valueSet_->rvals().uniqueValues(values);

    for (const auto &v : values)
      vars.push_back(v);
  }
  else if (type() == CQBaseModelType::STRING) {
    CQSValues::Values values;

    valueSet_->svals().uniqueValues(values);

    for (const auto &v : values)
      vars.push_back(v);
  }

  return vars;
}

CQModelColumnDetails::VariantList
CQModelColumnDetails::
uniqueCounts() const
{
  initCache();

  CQIValues::Counts counts;

  if      (type() == CQBaseModelType::INTEGER) {
    valueSet_->ivals().uniqueCounts(counts);
  }
  else if (type() == CQBaseModelType::REAL) {
    valueSet_->rvals().uniqueCounts(counts);
  }
  else if (type() == CQBaseModelType::STRING) {
    valueSet_->svals().uniqueCounts(counts);
  }

  VariantList vars;

  for (const auto &c : counts)
    vars.push_back(c);

  return vars;
}

int
CQModelColumnDetails::
uniqueId(const QVariant &var) const
{
  initCache();

  bool ok;

  if      (type() == CQBaseModelType::INTEGER) {
    long i = var.toInt(&ok);

    if (ok)
      return valueSet_->ivals().id(i);
  }
  else if (type() == CQBaseModelType::REAL) {
    double r = var.toDouble(&ok);

    return valueSet_->rvals().id(r);
  }
  else if (type() == CQBaseModelType::STRING) {
    return valueSet_->svals().id(var.toString());
  }

  return -1;
}

QVariant
CQModelColumnDetails::
uniqueValue(int i) const
{
  VariantList uniqueValues = this->uniqueValues();

  if (i >= 0 && i < uniqueValues.size())
    return uniqueValues[i];

  return QVariant();
}

int
CQModelColumnDetails::
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

  return 0;
}

int
CQModelColumnDetails::
valueInd(const QVariant &value) const
{
  auto p = valueInds_.find(value);

  if (p == valueInds_.end())
    return -1;

  return (*p).second;
}

QVariant
CQModelColumnDetails::
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

  return QVariant();
}

QVariant
CQModelColumnDetails::
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

  return QVariant();
}

QVariant
CQModelColumnDetails::
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

  return QVariant();
}

QVariantList
CQModelColumnDetails::
outlierValues() const
{
  initCache();

  QVariantList vars;

  if      (type() == CQBaseModelType::INTEGER) {
    const CQIValues::Indices &outliers = valueSet_->ivals().outliers();

    for (auto &o : outliers)
      vars.push_back(valueSet_->ivals().svalue(o));
  }
  else if (type() == CQBaseModelType::REAL) {
    const CQRValues::Indices &outliers = valueSet_->rvals().outliers();

    for (auto &o : outliers)
      vars.push_back(valueSet_->rvals().svalue(o));
  }
  else if (type() == CQBaseModelType::STRING) {
    return vars;
  }

  return vars;
}

bool
CQModelColumnDetails::
isOutlier(const QVariant &value) const
{
  initCache();

  bool ok;

  if      (type() == CQBaseModelType::INTEGER) {
    long i = value.toInt(&ok);

    return (ok && valueSet_->ivals().isOutlier(i));
  }
  else if (type() == CQBaseModelType::REAL) {
    double r = value.toDouble(&ok);

    return (ok && valueSet_->rvals().isOutlier(r));
  }
  else if (type() == CQBaseModelType::STRING) {
    return false;
  }

  return false;
}

double
CQModelColumnDetails::
map(const QVariant &var) const
{
  initCache();

  bool ok;

  if      (type() == CQBaseModelType::INTEGER) {
    long i = var.toInt(&ok);

    if (ok)
      return valueSet_->ivals().map(i);
  }
  else if (type() == CQBaseModelType::REAL) {
    double r = var.toDouble(&ok);

    return valueSet_->rvals().map(r);
  }
  else if (type() == CQBaseModelType::STRING) {
    return 0;
  }

  return 0;
}

void
CQModelColumnDetails::
initCache() const
{
  if (! initialized_) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (! initialized_) {
      CQModelColumnDetails *th = const_cast<CQModelColumnDetails *>(this);

      (void) th->initData();
    }
  }
}

bool
CQModelColumnDetails::
initData()
{
  //CQPerfTrace trace("CQModelColumnDetails::initData");

  assert(! initialized_);

  //---

  if (! typeInitialized_) {
    if (! calcType())
      return false;
  }

  //---

  // TODO: replace monotonic with sorted and sort dir
  // auto update sorted when model sorted

  class DetailVisitor : public CQModelVisitor {
   public:
    DetailVisitor(CQModelColumnDetails *details) :
     details_(details) {
      monotonicSet_ = false;
      monotonic_    = true;
      increasing_   = true;
    }

    // visit row
    State visit(const QAbstractItemModel *model, const VisitData &data) override {
      bool ok;

      QVariant var = CQModelUtil::modelValue(model, data.row, details_->column(), data.parent, ok);
      if (! ok) return State::SKIP;

      details_->addValue(var);

      if      (details_->type() == CQBaseModelType::INTEGER) {
        long i = var.toInt(&ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(int(i)))
          return State::SKIP;

        details_->addInt((int) i);

        addInt(i);
      }
      else if (details_->type() == CQBaseModelType::REAL) {
        double r = var.toDouble(&ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(r))
          return State::SKIP;

        details_->addReal(r);

        addReal(r);
      }
      else if (details_->type() == CQBaseModelType::STRING) {
        QString s = var.toString();

        if (! details_->checkRow(s))
          return State::SKIP;

        details_->addString(s);

        addString(s);
      }
      else {
        QString s = var.toString();

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

        long imin = min_.toInt(&ok1);

        imin = (! ok1 ? i : std::min(imin, i));

        min_ = QVariant(int(imin));
      }

      // if no type defined max, update max value
      if (visitMax_) {
        bool ok1;

        long imax = max_.toInt(&ok1);

        imax = (! ok1 ? i : std::max(imax, i));

        max_ = QVariant(int(imax));
      }

      if (lastValue1_.isValid() && lastValue2_.isValid()) {
        bool ok1, ok2;

        long i1 = lastValue1_.toInt(&ok1);
        long i2 = lastValue2_.toInt(&ok2);

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

        double rmin = min_.toDouble(&ok1);

        rmin = (! ok1 ? r : std::min(rmin, r));

        min_ = QVariant(rmin);
      }

      // if no type defined max, update max value
      if (visitMax_) {
        bool ok1;

        double rmax = max_.toDouble(&ok1);

        rmax = (! ok1 ? r : std::max(rmax, r));

        max_ = QVariant(rmax);
      }

      if (lastValue1_.isValid() && lastValue2_.isValid()) {
        bool ok1, ok2;

        double r1 = lastValue1_.toDouble(&ok1);
        double r2 = lastValue2_.toDouble(&ok2);

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
        bool ok1 = min_.isValid();

        QString smin = min_.toString();

        smin = (! ok1 ? s : std::min(smin, s));

        min_ = QVariant(smin);
      }

      // if no type defined max, update max value
      if (visitMax_) {
        bool ok1 = min_.isValid();

        QString smax = max_.toString();

        smax = (! ok1 ? s : std::max(smax, s));

        max_ = QVariant(smax);
      }

      if (lastValue1_.isValid() && lastValue2_.isValid()) {
        QString s1 = lastValue1_.toString();
        QString s2 = lastValue2_.toString();

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

    QVariant minValue() const { return min_; }
    QVariant maxValue() const { return max_; }

    bool isMonotonic () const { return monotonicSet_ && monotonic_; }
    bool isIncreasing() const { return increasing_; }

   private:
    CQModelColumnDetails* details_      { nullptr };
    QVariant              min_;
    QVariant              max_;
    bool                  visitMin_     { true };
    bool                  visitMax_     { true };
    QVariant              lastValue1_;
    QVariant              lastValue2_;
    bool                  monotonicSet_ { false };
    bool                  monotonic_    { true };
    bool                  increasing_   { true };
  };

  //---

  DetailVisitor detailVisitor(this);

  CQModelVisit::exec(model(), detailVisitor);

  //---

  minValue_   = detailVisitor.minValue();
  maxValue_   = detailVisitor.maxValue();
  numRows_    = detailVisitor.numRows();
  monotonic_  = detailVisitor.isMonotonic();
  increasing_ = detailVisitor.isIncreasing();

  //---

  initialized_ = true;

  return true;
}

void
CQModelColumnDetails::
initType() const
{
  if (! typeInitialized_) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (! typeInitialized_) {
      CQModelColumnDetails *th = const_cast<CQModelColumnDetails *>(this);

      (void) th->calcType();
    }
  }
}

bool
CQModelColumnDetails::
calcType()
{
  //CQPerfTrace trace("CQModelColumnDetails::calcType");

  assert(! typeInitialized_);

  //---

  if (column_ < 0)
    return false;

  int numColumns = details_->numColumns();

  if (column_ < 0 || column_ >= numColumns)
    return false;

  //---

  // get column type
  if (! CQModelUtil::columnValueType(model(), column_, type_))
    type_ = CQBaseModelType::NONE;

  //---

  if (type_ == CQBaseModelType::NONE) {
    CQBaseModel *baseModel = qobject_cast<CQBaseModel *>(CQModelUtil::getBaseModel(model()));

    int maxRows = (baseModel ? baseModel->maxTypeRows() : -1);

    type_ = CQModelUtil::calcColumnType(model(), column_, maxRows);
  }

  //---

  typeInitialized_ = true;

  return true;
}

void
CQModelColumnDetails::
addInt(int i)
{
  valueSet_->ivals().addValue(i);
}

void
CQModelColumnDetails::
addReal(double r)
{
  if (! CMathUtil::isNaN(r))
    valueSet_->rvals().addValue(r);
  else
    valueSet_->rvals().addValue(CQRValues::OptReal());
}

void
CQModelColumnDetails::
addString(const QString &s)
{
  valueSet_->svals().addValue(s);
}

void
CQModelColumnDetails::
addValue(const QVariant &value)
{
  auto p = valueInds_.find(value);

  if (p == valueInds_.end()) {
    int ind = valueInds_.size();

    (void) valueInds_.insert(p, VariantInds::value_type(value, ind));
  }
}
