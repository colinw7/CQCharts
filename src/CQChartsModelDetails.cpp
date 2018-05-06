#include <CQChartsModelDetails.h>
#include <CQChartsColumnType.h>
#include <CQCharts.h>
#include <CQChartsUtil.h>
#include <QAbstractItemModel>

CQChartsModelDetails::
CQChartsModelDetails()
{
}

void
CQChartsModelDetails::
update(CQCharts *charts, QAbstractItemModel *model)
{
  charts_ = charts;
  model_  = model;

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

  class DetailVisitor : public CQChartsModelVisitor {
   public:
    DetailVisitor(CQChartsModelColumnDetails *details) :
     details_(details) {
      // get column type and name values
      // TODO: calls CQChartsModelVisitor, integrate into this visitor
      (void) CQChartsUtil::columnValueType(details_->charts(), details_->model(),
                                           details_->column(), type_, nameValues_);

      CQChartsColumnTypeMgr *columnTypeMgr = details_->charts()->columnTypeMgr();

      CQChartsColumnType *columnType = columnTypeMgr->getType(type_);

      if (columnType) {
        typeName_ = columnType->name();
        min_      = columnType->minValue(nameValues_); // type custom min value
        max_      = columnType->maxValue(nameValues_); // type custom max value
        visitMin_ = ! min_.isValid();
        visitMax_ = ! max_.isValid();
      }
      else {
        typeName_ = "string";
        type_     = CQBaseModel::Type::STRING;
      }

      monotonicSet_ = false;
      monotonic_    = true;
      increasing_   = true;
    }

    // visit row
    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      if      (type_ == CQBaseModel::Type::INTEGER) {
        bool ok;

        long i = CQChartsUtil::modelInteger(model, row, details_->column(), parent, ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(int(i)))
          return State::SKIP;

        if (visitMin_) {
          bool ok1;

          long imin = CQChartsUtil::toInt(min_, ok1);

          imin = (! ok1 ? i : std::min(imin, i));

          min_ = QVariant(int(imin));
        }

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
                if (i2 < i2)
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
      else if (type_ == CQBaseModel::Type::REAL) {
        bool ok;

        double r = CQChartsUtil::modelReal(model, row, details_->column(), parent, ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(r))
          return State::SKIP;

        if (visitMin_) {
          bool ok1;

          double rmin = CQChartsUtil::toReal(min_, ok1);

          rmin = (! ok1 ? r : std::min(rmin, r));

          min_ = QVariant(rmin);
        }

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
      else if (type_ == CQBaseModel::Type::TIME) {
        bool ok;

        long i = CQChartsUtil::modelInteger(model, row, details_->column(), parent, ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(int(i)))
          return State::SKIP;

        if (visitMin_) {
          bool ok1;

          long imin = CQChartsUtil::toInt(min_, ok1);

          imin = (! ok1 ? i : std::min(imin, i));

          min_ = QVariant(int(imin));
        }

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
      else {
        bool ok;

        QString s = CQChartsUtil::modelString(model, row, details_->column(), parent, ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(s))
          return State::SKIP;

        if (visitMin_) {
          bool ok1;

          QString smin = CQChartsUtil::toString(min_, ok1);

          smin = (! ok1 ? s : std::min(smin, s));

          min_ = QVariant(smin);
        }

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

      return State::OK;
    }

    CQBaseModel::Type type() const { return type_; }

    const CQChartsNameValues &nameValues() const { return nameValues_; }

    QString typeName() const { return typeName_; }

    QVariant minValue() const { return min_; }
    QVariant maxValue() const { return max_; }

    bool isMonotonic () const { return monotonic_; }
    bool isIncreasing() const { return increasing_; }

   private:
    CQChartsModelColumnDetails* details_      { nullptr };
    CQBaseModel::Type           type_         { CQBaseModel::Type::NONE };
    CQChartsNameValues          nameValues_;
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
  type_       = detailVisitor.type();
  nameValues_ = detailVisitor.nameValues();
  minValue_   = detailVisitor.minValue();
  maxValue_   = detailVisitor.maxValue();
  numRows_    = detailVisitor.numRows();
  monotonic_  = detailVisitor.isMonotonic();
  increasing_ = detailVisitor.isIncreasing();

  return true;
}
