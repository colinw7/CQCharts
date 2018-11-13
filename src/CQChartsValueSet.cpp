#include <CQChartsValueSet.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>

CQChartsValueSet::
CQChartsValueSet(CQChartsPlot *plot) :
 plot_(plot)
{
}

bool
CQChartsValueSet::
isValid() const
{
  if      (type() == Type::INTEGER) return ivals_.isValid();
  else if (type() == Type::REAL   ) return rvals_.isValid();
  else if (type() == Type::STRING ) return svals_.isValid();
  else if (type() == Type::COLOR  ) return cvals_.isValid();
  else if (type() == Type::TIME   ) return tvals_.isValid();
  else                              return false;
}

bool
CQChartsValueSet::
canMap() const
{
  if      (type() == Type::INTEGER) return ivals_.canMap();
  else if (type() == Type::REAL   ) return rvals_.canMap();
  else if (type() == Type::STRING ) return svals_.canMap();
  else if (type() == Type::COLOR  ) return cvals_.canMap();
  else if (type() == Type::TIME   ) return tvals_.canMap();
  else                              return false;
}

void
CQChartsValueSet::
addProperties(const QString &path)
{
  if (plot_) {
    plot_->addProperty(path, this, "mapped", "mapped");
    plot_->addProperty(path, this, "mapMin", "mapMin");
    plot_->addProperty(path, this, "mapMax", "mapMax");
  }
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
  else if (type() == Type::TIME)
    return (i >= 0 && i < tvals_.size());
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
    OptInt ival = ivals_.value(i);

    if (! ival)
      return mapMin;

    // return actual value if mapping disabled
    if (! isMapped())
      return *ival;

#if 0
    // map value using integer value range
    int imin = ivals_.imin();
    int imax = ivals_.imax();

    if (imin != imax)
      return CMathUtil::map(*ival, imin, imax, mapMin, mapMax);
    else
      return mapMin;
#endif

    // map value using real value range
    return ivals_.map(*ival, mapMin, mapMax);
  }
  else if (type() == Type::REAL) {
    // get nth real
    OptReal rval = rvals_.value(i);

    if (! rval)
      return mapMin;

    // return actual value if mapping disabled
    if (! isMapped())
      return *rval;

    // map value using real value range
    return rvals_.map(*rval, mapMin, mapMax);
  }
  else if (type() == Type::STRING) {
    // get nth string
    OptString sval = svals_.value(i);

    if (! sval)
      return mapMin;

    // return string set index if mapping disabled
    if (! isMapped())
      return svals_.id(*sval);

    // map string using number of sets
    return svals_.map(*sval, mapMin, mapMax);
  }
  else if (type() == Type::COLOR) {
    // get nth color
    CQChartsColor cval = cvals_.value(i);

    if (! cval.isValid())
      return mapMin;

    // return color set index if mapping disabled
    if (! isMapped())
      return cvals_.id(cval);

    // map string using number of sets
    return cvals_.map(cval, mapMin, mapMax);
  }
  else if (type() == Type::TIME) {
    // get nth ti,e
    OptReal tval = tvals_.value(i);

    if (! tval)
      return mapMin;

    // return actual value if mapping disabled
    if (! isMapped())
      return *tval;

    // map value using time value range
    return tvals_.map(*tval, mapMin, mapMax);
  }
  else {
    return mapMin;
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
  else if (type() == Type::TIME)
    return tvals_.imin();
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
  else if (type() == Type::TIME)
    return tvals_.imax();
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
  else if (type() == Type::TIME)
    return tvals_.min();
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
  else if (type() == Type::TIME)
    return tvals_.max();
  else
    return 0.0;
}

double
CQChartsValueSet::
rsum() const
{
  if      (type() == Type::INTEGER)
    return ivals_.sum();
  else if (type() == Type::REAL)
    return rvals_.sum();
  else if (type() == Type::STRING)
    return 0.0;
  else if (type() == Type::COLOR)
    return 0.0;
  else if (type() == Type::TIME)
    return 0.0;
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
  else if (type() == Type::TIME)
    return 0.0;
  else
    return 0.0;
}

int
CQChartsValueSet::
rid(double r) const
{
  if      (type() == Type::INTEGER)
    return -1;
  else if (type() == Type::REAL)
    return rvals_.id(r);
  else if (type() == Type::STRING)
    return -1;
  else if (type() == Type::COLOR)
    return -1;
  else if (type() == Type::TIME)
    return tvals_.id(r);
  else
    return -1;
}

double
CQChartsValueSet::
idr(int i) const
{
  if      (type() == Type::INTEGER)
    return 0.0;
  else if (type() == Type::REAL)
    return rvals_.ivalue(i);
  else if (type() == Type::STRING)
    return 0.0;
  else if (type() == Type::COLOR)
    return 0.0;
  else if (type() == Type::TIME)
    return tvals_.ivalue(i);
  else
    return 0.0;
}

int
CQChartsValueSet::
iid(int i) const
{
  if      (type() == Type::INTEGER)
    return ivals_.id(i);
  else if (type() == Type::REAL)
    return -1;
  else if (type() == Type::STRING)
    return -1;
  else if (type() == Type::COLOR)
    return -1;
  else if (type() == Type::TIME)
    return -1;
  else
    return -1;
}

int
CQChartsValueSet::
idi(int i) const
{
  if      (type() == Type::INTEGER)
    return ivals_.ivalue(i);
  else if (type() == Type::REAL)
    return 0;
  else if (type() == Type::STRING)
    return 0;
  else if (type() == Type::COLOR)
    return 0;
  else if (type() == Type::TIME)
    return 0;
  else
    return 0;
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

  if (type_ == Type::NONE) {
    // get type from column values
    if (column().isValid() && plot_)
      type_ = plot_->columnValueType(column(), Type::NONE);
  }

  // if no type then look at added value (TODO: always the same as color values ?)
  if (type_ == Type::NONE) {
    int ni = 0, nr = 0;

    for (const auto &value : values_) {
      if      (value.type() == QVariant::Int)
        ++ni;
      else if (value.type() == QVariant::Double) {
        ++nr;

        bool ok;

        double r = CQChartsVariant::toReal(value, ok);

        if (CMathUtil::isInteger(r))
          ++ni;
        else
          ++nr;
      }
      else {
        bool ok;

        double r = CQChartsVariant::toReal(value, ok);

        if (ok) {
          if (CMathUtil::isInteger(r))
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
      if      (nr == 0 && ni > 0)
        type_ = Type::INTEGER;
      else if (nr > 0)
        type_ = Type::REAL;
      else
        type_ = Type::STRING;
    }
  }

  //---

  clearVals();

  bool ok;

  for (const auto &value : values_) {
    if      (type() == Type::INTEGER) {
      int i = CQChartsVariant::toInt(value, ok);

      ivals_.addValue(ok ? OptInt(i) : OptInt());
    }
    else if (type() == Type::REAL) {
      double r = CQChartsVariant::toReal(value, ok);

      if (! isAllowNaN() && CMathUtil::isNaN(r))
        ok = false;

      rvals_.addValue(ok ? OptReal(r) : OptReal());
    }
    else if (type() == Type::STRING) {
      QString s;

      bool ok = false;

      if (value.isValid()) {
        CQChartsVariant::toString(value, s);

        ok = true;
      }

      svals_.addValue(ok ? OptString(s) : OptString());
    }
    else if (type() == Type::COLOR) {
      CQChartsColor c = CQChartsVariant::toColor(value, ok);

      cvals_.addValue(c);
    }
    else if (type() == Type::TIME) {
      double t = CQChartsVariant::toReal(value, ok);

      if (! isAllowNaN() && CMathUtil::isNaN(t))
        ok = false;

      tvals_.addValue(ok ? OptReal(t) : OptReal());
    }
  }
}

void
CQChartsValueSet::
clearVals()
{
  ivals_.clear();
  rvals_.clear();
  svals_.clear();
  cvals_.clear();
  tvals_.clear();
}
