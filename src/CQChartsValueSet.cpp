#include <CQChartsValueSet.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>

CQChartsValueSet::
CQChartsValueSet()
{
}

void
CQChartsValueSet::
addProperties(CQChartsPlot *plot, const QString &path)
{
  plot->addProperty(path, this, "mapEnabled", "mapEnabled");
  plot->addProperty(path, this, "mapMin"    , "mapMin"    );
  plot->addProperty(path, this, "mapMax"    , "mapMin"    );
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
  else
    assert(false);
}

double
CQChartsValueSet::
imap(int i) const
{
  return imap(i, mapMin(), mapMax());
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

    // map value using integer value range
    int imin = ivals_.imin();
    int imax = ivals_.imax();

    if (imin != imax)
      return CQChartsUtil::map(ival, imin, imax, mapMin, mapMax);
    else
      return mapMin;
  }
  else if (type() == Type::REAL) {
    // get nth real
    double r = rvals_.value(i);

    // return actual value if mapping disabled
    if (! isMapEnabled())
      return r;

    // map value using real value range
    return rvals_.map(r, mapMin, mapMax);
  }
  else if (type() == Type::STRING) {
    // get nth string
    QString s = svals_.value(i);

    // return string set index if mapping disabled
    if (! isMapEnabled())
      return svals_.id(s);

    // map string using number of sets
    return svals_.map(s, mapMin, mapMax);
  }
  else
    assert(false);
}

int
CQChartsValueSet::
sbucket(const QString &s) const
{
  init();

  if (type() == Type::STRING)
    return svals_.sbucket(s);
  else
    assert(false);
}

QString
CQChartsValueSet::
buckets(int i) const
{
  init();

  if (type() == Type::STRING)
    return svals_.buckets(i);
  else
    assert(false);
}

int
CQChartsValueSet::
sind(const QString &s) const
{
  init();

  if (type() == Type::STRING)
    return svals_.id(s);
  else
    assert(false);
}

QString
CQChartsValueSet::
inds(int ind) const
{
  init();

  if (type() == Type::STRING)
    return svals_.ivalue(ind);
  else
    assert(false);
}

int
CQChartsValueSet::
snum() const
{
  init();

  if (type() == Type::STRING)
    return svals_.numUnique();
  else
    assert(false);
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
  else
    assert(false);
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
  else
    assert(false);
}

double
CQChartsValueSet::
rmin() const
{
  if      (type() == Type::INTEGER)
    return ivals_.min();
  else if (type() == Type::REAL)
    return rvals_.min();

  assert(false);

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

  assert(false);

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

  //---

  ivals_.clear();
  rvals_.clear();
  svals_.clear();

  bool ok;

  for (const auto &value : values_) {
    if      (type() == Type::INTEGER) {
      int i = CQChartsUtil::toInt(value, ok);

      ivals_.addValue(i);
    }
    else if (type() == Type::REAL) {
      double r = CQChartsUtil::toReal(value, ok);

      if (! isAllowNaN() && CQChartsUtil::isNaN(r))
        continue;

      rvals_.addValue(r);
    }
    else if (type() == Type::STRING) {
      QString s = value.toString();

      svals_.addValue(s);
    }
    else
      assert(false);
  }
}
