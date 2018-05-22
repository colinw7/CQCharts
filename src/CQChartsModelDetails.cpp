#include <CQChartsModelDetails.h>
#include <CQChartsColumnType.h>
#include <CQChartsValueSet.h>
#include <CQCharts.h>
#include <CQChartsUtil.h>
#include <QAbstractItemModel>

CQChartsModelDetails::
CQChartsModelDetails(CQCharts *charts, QAbstractItemModel *model) :
 charts_(charts), model_(model)
{
}

void
CQChartsModelDetails::
init() const
{
  if (! initialized_) {
    CQChartsModelDetails *th = const_cast<CQChartsModelDetails *>(this);

    th->initialized_ = true;

    th->update();
  }
}

void
CQChartsModelDetails::
update()
{
  numColumns_ = model_->columnCount();
  numRows_    = model_->rowCount   ();

  columnDetails_.clear();

  for (int c = 0; c < numColumns_; ++c) {
    CQChartsModelColumnDetails columnDetails(charts_, model_, c);

    columnDetails_.push_back(columnDetails);

    numRows_ = std::max(numRows_, columnDetails.numRows());
  }
}

//------

CQChartsModelColumnDetails::
CQChartsModelColumnDetails(CQCharts *charts, QAbstractItemModel *model,
                           const CQChartsColumn &column) :
 charts_(charts), model_(model), column_(column)
{
}

CQChartsModelColumnDetails::
~CQChartsModelColumnDetails()
{
  delete ivals_;
  delete rvals_;
  delete svals_;
}

QString
CQChartsModelColumnDetails::
typeName() const
{
  if (! initialized_)
    (void) const_cast<CQChartsModelColumnDetails *>(this)->init();

  return typeName_;
}

QVariant
CQChartsModelColumnDetails::
minValue() const
{
  if (! initialized_)
    (void) const_cast<CQChartsModelColumnDetails *>(this)->init();

  return minValue_;
}

QVariant
CQChartsModelColumnDetails::
maxValue() const
{
  if (! initialized_)
    (void) const_cast<CQChartsModelColumnDetails *>(this)->init();

  return maxValue_;
}

QVariant
CQChartsModelColumnDetails::
dataName(const QVariant &v) const
{
  if (! initialized_)
    (void) const_cast<CQChartsModelColumnDetails *>(this)->init();

  CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

  CQChartsColumnType *columnType = columnTypeMgr->getType(type_);

  if (! columnType)
    return v;

  return columnType->dataName(v, nameValues_);
}

int
CQChartsModelColumnDetails::
numRows() const
{
  if (! initialized_)
    (void) const_cast<CQChartsModelColumnDetails *>(this)->init();

  return numRows_;
}

int
CQChartsModelColumnDetails::
numUnique() const
{
  if      (type() == CQBaseModel::Type::INTEGER) {
    return (ivals_ ? ivals_->numUnique() : 0);
  }
  else if (type() == CQBaseModel::Type::REAL) {
    return (rvals_ ? rvals_->numUnique() : 0);
  }
  else if (type() == CQBaseModel::Type::STRING) {
    return (svals_ ? svals_->numUnique() : 0);
  }
  else if (type() == CQBaseModel::Type::TIME) {
    return (ivals_ ? ivals_->numUnique() : 0);
  }
  else if (type() == CQBaseModel::Type::COLOR) {
    return (svals_ ? svals_->numUnique() : 0);
  }
  else {
    return 0;
  }
}

double
CQChartsModelColumnDetails::
map(const QVariant &var) const
{
  bool ok;

  if      (type() == CQBaseModel::Type::INTEGER) {
    long l = CQChartsUtil::toInt(var, ok);

    return (ivals_ ? ivals_->map(l) : 0.0);
  }
  else if (type() == CQBaseModel::Type::REAL) {
    double r = CQChartsUtil::toReal(var, ok);

    return (rvals_ ? rvals_->map(r) : 0.0);
  }
  else if (type() == CQBaseModel::Type::STRING) {
    return 0;
  }
  else if (type() == CQBaseModel::Type::TIME) {
    long l = CQChartsUtil::toInt(var, ok);

    return (ivals_ ? ivals_->map(l) : 0.0);
  }
  else if (type() == CQBaseModel::Type::COLOR) {
    return 0;
  }
  else {
    return 0;
  }
}

bool
CQChartsModelColumnDetails::
init()
{
  initialized_ = true;

  //---

  if (! model_)
    return false;

  if (! column_.isValid())
    return false;

  if (column_.type() == CQChartsColumn::Type::DATA) {
    int icolumn = column_.column();

    int numColumns = model_->columnCount(QModelIndex());

    if (icolumn < 0 || icolumn >= numColumns)
      return false;
  }

  //---

  // get column type and name values
  // TODO: calls CQChartsModelVisitor, integrate into this visitor
  if (! CQChartsUtil::columnValueType(charts_, model_, column_, type_, nameValues_))
    type_ = CQBaseModel::Type::NONE;

  class DetailVisitor : public CQChartsModelVisitor {
   public:
    DetailVisitor(CQChartsModelColumnDetails *details) :
     details_(details) {
      CQChartsColumnTypeMgr *columnTypeMgr = details_->charts()->columnTypeMgr();

      CQChartsColumnType *columnType = columnTypeMgr->getType(details_->type());

      if (columnType) {
        typeName_ = columnType->name();

        min_ = columnType->minValue(details->nameValues()); // type custom min value
        max_ = columnType->maxValue(details->nameValues()); // type custom max value

        visitMin_ = ! min_.isValid();
        visitMax_ = ! max_.isValid();
      }
      else {
        details->setType(CQBaseModel::Type::STRING);

        typeName_ = "string";
      }

      monotonicSet_ = false;
      monotonic_    = true;
      increasing_   = true;
    }

    // visit row
    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      if      (details_->type() == CQBaseModel::Type::INTEGER) {
        bool ok;

        long i = CQChartsUtil::modelInteger(model, row, details_->column(), parent, ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(int(i)))
          return State::SKIP;

        addInt(i);
      }
      else if (details_->type() == CQBaseModel::Type::REAL) {
        bool ok;

        double r = CQChartsUtil::modelReal(model, row, details_->column(), parent, ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(r))
          return State::SKIP;

        addReal(r);
      }
      else if (details_->type() == CQBaseModel::Type::STRING) {
        bool ok;

        QString s = CQChartsUtil::modelString(model, row, details_->column(), parent, ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(s))
          return State::SKIP;

        addString(s);
      }
      else if (details_->type() == CQBaseModel::Type::TIME) {
        bool ok;

        long t = CQChartsUtil::modelInteger(model, row, details_->column(), parent, ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(int(t)))
          return State::SKIP;

        addInt((int) t);
      }
      else if (details_->type() == CQBaseModel::Type::COLOR) {
        bool ok;

        QString s = CQChartsUtil::modelString(model, row, details_->column(), parent, ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(s))
          return State::SKIP;

        addString(s);
      }

      return State::OK;
    }

    void addInt(long i) {
      details_->addInt((int) i);

      // if no type defined min, update min value
      if (visitMin_) {
        bool ok1;

        long imin = CQChartsUtil::toInt(min_, ok1);

        imin = (! ok1 ? i : std::min(imin, i));

        min_ = QVariant(int(imin));
      }

      // if no type defined max, update max value
      if (visitMax_) {
        bool ok1;

        long imax = CQChartsUtil::toInt(max_, ok1);

        imax = (! ok1 ? i : std::max(imax, i));

        max_ = QVariant(int(imax));
      }

      if (lastValue1_.isValid() && lastValue2_.isValid()) {
        bool ok1, ok2;

        long i1 = CQChartsUtil::toInt(lastValue1_, ok1);
        long i2 = CQChartsUtil::toInt(lastValue2_, ok2);

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
      details_->addReal(r);

      // if no type defined min, update min value
      if (visitMin_) {
        bool ok1;

        double rmin = CQChartsUtil::toReal(min_, ok1);

        rmin = (! ok1 ? r : std::min(rmin, r));

        min_ = QVariant(rmin);
      }

      // if no type defined max, update max value
      if (visitMax_) {
        bool ok1;

        double rmax = CQChartsUtil::toReal(max_, ok1);

        rmax = (! ok1 ? r : std::max(rmax, r));

        max_ = QVariant(rmax);
      }

      if (lastValue1_.isValid() && lastValue2_.isValid()) {
        bool ok1, ok2;

        double r1 = CQChartsUtil::toReal(lastValue1_, ok1);
        double r2 = CQChartsUtil::toReal(lastValue2_, ok2);

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
      details_->addString(s);

      // if no type defined min, update min value
      if (visitMin_) {
        bool ok1;

        QString smin = CQChartsUtil::toString(min_, ok1);

        smin = (! ok1 ? s : std::min(smin, s));

        min_ = QVariant(smin);
      }

      // if no type defined max, update max value
      if (visitMax_) {
        bool ok1;

        QString smax = CQChartsUtil::toString(max_, ok1);

        smax = (! ok1 ? s : std::max(smax, s));

        max_ = QVariant(smax);
      }

      if (lastValue1_.isValid() && lastValue2_.isValid()) {
        bool ok1, ok2;

        QString s1 = CQChartsUtil::toString(lastValue1_, ok1);
        QString s2 = CQChartsUtil::toString(lastValue2_, ok2);

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

    QString typeName() const { return typeName_; }

    QVariant minValue() const { return min_; }
    QVariant maxValue() const { return max_; }

    bool isMonotonic () const { return monotonic_; }
    bool isIncreasing() const { return increasing_; }

   private:
    CQChartsModelColumnDetails* details_      { nullptr };
    QString                     typeName_;
    QVariant                    min_;
    QVariant                    max_;
    bool                        visitMin_     { true };
    bool                        visitMax_     { true };
    QVariant                    lastValue1_;
    QVariant                    lastValue2_;
    bool                        monotonic_    { true };
    bool                        increasing_   { true };
    bool                        monotonicSet_ { false };
  };

  //---

  DetailVisitor detailVisitor(this);

  CQChartsUtil::visitModel(model_, detailVisitor);

  //---

  typeName_   = detailVisitor.typeName();
  minValue_   = detailVisitor.minValue();
  maxValue_   = detailVisitor.maxValue();
  numRows_    = detailVisitor.numRows();
  monotonic_  = detailVisitor.isMonotonic();
  increasing_ = detailVisitor.isIncreasing();

  return true;
}

void
CQChartsModelColumnDetails::
addInt(int i)
{
  if (! ivals_)
    ivals_ = new CQChartsIValues;

  ivals_->addValue(i);
}

void
CQChartsModelColumnDetails::
addReal(double r)
{
  if (! rvals_)
    rvals_ = new CQChartsRValues;

  rvals_->addValue(r);
}

void
CQChartsModelColumnDetails::
addString(const QString &s)
{
  if (! svals_)
    svals_ = new CQChartsSValues;

  svals_->addValue(s);
}