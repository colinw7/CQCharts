#include <CQChartsValueSet.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>

CQChartsValueSet::
CQChartsValueSet(CQChartsPlot *plot) :
 plot_(plot)
{
}

void
CQChartsValueSet::
addProperties(const QString &path)
{
  plot_->addProperty(path, this, "mapEnabled", "mapEnabled");
  plot_->addProperty(path, this, "mapMin"    , "mapMin"    );
  plot_->addProperty(path, this, "mapMax"    , "mapMax"    );
}

void
CQChartsValueSet::
addValue(const QVariant &value)
{
  values_.push_back(value);

  initialized_ = false;
}

void
CQChartsValueSet::
clear()
{
  type_ = Type::NONE;

  values_.clear();

  initialized_ = false;
}

bool
CQChartsValueSet::
hasInd(int i) const
{
  if      (type() == Type::INTEGER)
    return (i >= 0 && i < ivals_.size());
  else if (type() == Type::REAL)
    return (i >= 0 && i < rvals_.size());
  else if (type() == Type::STRING)
    return (i >= 0 && i < svals_.size());
  else if (type() == Type::COLOR)
    return (i >= 0 && i < cvals_.size());
  else
    return false;
}

double
CQChartsValueSet::
imap(int i) const
{
  double min = mapMin();
  double max = std::max(min, mapMax());

  return imap(i, min, max);
}

double
CQChartsValueSet::
imap(int i, double mapMin, double mapMax) const
{
  assert(hasInd(i));

  if      (type() == Type::INTEGER) {
    // get nth integer
    int ival = ivals_.value(i);

    // return actual value if mapping disabled
    if (! isMapEnabled())
      return ival;

#if 0
    // map value using integer value range
    int imin = ivals_.imin();
    int imax = ivals_.imax();

    if (imin != imax)
      return CQChartsUtil::map(ival, imin, imax, mapMin, mapMax);
    else
      return mapMin;
#endif

    // map value using real value range
    return ivals_.map(ival, mapMin, mapMax);
  }
  else if (type() == Type::REAL) {
    // get nth real
    double rval = rvals_.value(i);

    // return actual value if mapping disabled
    if (! isMapEnabled())
      return rval;

    // map value using real value range
    return rvals_.map(rval, mapMin, mapMax);
  }
  else if (type() == Type::STRING) {
    // get nth string
    QString sval = svals_.value(i);

    // return string set index if mapping disabled
    if (! isMapEnabled())
      return svals_.id(sval);

    // map string using number of sets
    return svals_.map(sval, mapMin, mapMax);
  }
  else if (type() == Type::COLOR) {
    // get nth color
    QColor cval = cvals_.value(i);

    // return color set index if mapping disabled
    if (! isMapEnabled())
      return cvals_.id(cval);

    // map string using number of sets
    return cvals_.map(cval, mapMin, mapMax);
  }
  else {
    return 0.0;
  }
}

int
CQChartsValueSet::
sbucket(const QString &s) const
{
  init();

  if (type() == Type::STRING)
    return svals_.sbucket(s);
  else
    return -1;
}

QString
CQChartsValueSet::
buckets(int i) const
{
  init();

  if (type() == Type::STRING)
    return svals_.buckets(i);
  else
    return "";
}

int
CQChartsValueSet::
sind(const QString &s) const
{
  init();

  if (type() == Type::STRING)
    return svals_.id(s);
  else
    return -1;
}

QString
CQChartsValueSet::
inds(int ind) const
{
  init();

  if (type() == Type::STRING)
    return svals_.ivalue(ind);
  else
    return "";
}

int
CQChartsValueSet::
snum() const
{
  init();

  if (type() == Type::STRING)
    return svals_.numUnique();
  else
    return -1;
}

int
CQChartsValueSet::
imin() const
{
  if      (type() == Type::INTEGER)
    return ivals_.imin();
  else if (type() == Type::REAL)
    return rvals_.imin();
  else if (type() == Type::STRING)
    return svals_.imin();
  else if (type() == Type::COLOR)
    return cvals_.imin();
  else
    return -1;
}

int
CQChartsValueSet::
imax() const
{
  if      (type() == Type::INTEGER)
    return ivals_.imax();
  else if (type() == Type::REAL)
    return rvals_.imax();
  else if (type() == Type::STRING)
    return svals_.imax();
  else if (type() == Type::COLOR)
    return cvals_.imax();
  else
    return -1;
}

double
CQChartsValueSet::
rmin() const
{
  if      (type() == Type::INTEGER)
    return ivals_.min();
  else if (type() == Type::REAL)
    return rvals_.min();
  else if (type() == Type::STRING)
    return svals_.imin();
  else if (type() == Type::COLOR)
    return cvals_.imin();
  else
    return 0.0;
}

double
CQChartsValueSet::
rmax() const
{
  if      (type() == Type::INTEGER)
    return ivals_.max();
  else if (type() == Type::REAL)
    return rvals_.max();
  else if (type() == Type::STRING)
    return svals_.imax();
  else if (type() == Type::COLOR)
    return cvals_.imax();
  else
    return 0.0;
}

double
CQChartsValueSet::
rmean() const
{
  if      (type() == Type::INTEGER)
    return ivals_.mean();
  else if (type() == Type::REAL)
    return rvals_.mean();
  else if (type() == Type::STRING)
    return (svals_.imin() + svals_.imax())/2.0;
  else if (type() == Type::COLOR)
    return (cvals_.imin() + cvals_.imax())/2.0;
  else
    return 0.0;
}

void
CQChartsValueSet::
init() const
{
  if (initialized_)
    return;

  CQChartsValueSet *th = const_cast<CQChartsValueSet *>(this);

  th->init();
}

void
CQChartsValueSet::
init()
{
  initialized_ = true;

  type_ = Type::NONE;

  // get type from column values
  if (column().isValid())
    type_ = plot_->columnValueType(column());

  // if no type then look at added value (TODO: always the same sas color values ?)
  if (type_ == Type::NONE) {
    int ni = 0, nr = 0;

    for (const auto &value : values_) {
      if      (value.type() == QVariant::Int)
        ++ni;
      else if (value.type() == QVariant::Double) {
        ++nr;

        bool ok;

        double r = CQChartsUtil::toReal(value, ok);

        if (CQChartsUtil::isInteger(r))
          ++ni;
        else
          ++nr;
      }
      else {
        bool ok;

        double r = CQChartsUtil::toReal(value, ok);

        if (ok) {
          if (CQChartsUtil::isInteger(r))
            ++ni;
          else
            ++nr;
        }
        else {
          type_ = Type::STRING;
          break;
        }
      }
    }

    if (type_ == Type::NONE) {
      if (nr == 0)
        type_ = Type::INTEGER;
      else
        type_ = Type::REAL;
    }
  }

  //---

  ivals_.clear();
  rvals_.clear();
  svals_.clear();

  bool ok;

  for (const auto &value : values_) {
    if      (type_ == Type::INTEGER) {
      int i = CQChartsUtil::toInt(value, ok);

      ivals_.addValue(i);
    }
    else if (type_ == Type::REAL) {
      double r = CQChartsUtil::toReal(value, ok);

      if (! isAllowNaN() && CQChartsUtil::isNaN(r))
        continue;

      rvals_.addValue(r);
    }
    else if (type_ == Type::STRING) {
      QString s;

      if (value.isValid())
        CQChartsUtil::variantToString(value, s);

      svals_.addValue(s);
    }
    else if (type_ == Type::COLOR) {
      QColor c = value.value<QColor>();

      cvals_.addValue(c);
    }
  }
}