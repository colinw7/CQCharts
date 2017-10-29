#include <CQChartsValueSet.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>

#if 0
namespace {

bool isInt(const QVariant &value) {
  if (value.type() == QVariant::Int)
    return true;

  bool ok;

  (void) CQChartsUtil::toInt(value, ok);

  return ok;
}

bool isReal(const QVariant &value) {
  if (value.type() == QVariant::Double)
    return true;

  bool ok;

  (void) CQChartsUtil::toReal(value, ok);

  return ok;
}

}
#endif

//---

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
  if       (type() == Type::INTEGER) {
    // get nth integer
    int ival = ivals_[i];

    // return actual value if mapping disabled
    if (! isMapEnabled())
      return ival;

    // map value using integer value range
    int imin = *iset_.begin ();
    int imax = *iset_.rbegin();

    if (imin != imax)
      return CQChartsUtil::map(ival, imin, imax, mapMin, mapMax);
    else
      return mapMin;
  }
  else if (type() == Type::REAL) {
    // get nth real
    double rval = rvals_[i];

    // return actual value if mapping disabled
    if (! isMapEnabled())
      return rval;

    // map value using real value range
    double rmin = rvalset_.begin ()->first;
    double rmax = rvalset_.rbegin()->first;

    if (rmin != rmax)
      return CQChartsUtil::map(rval, rmin, rmax, mapMin, mapMax);
    else
      return mapMin;
  }
  else {
    // get nth string
    QString sval = svals_[i];

    // get string set index
    int ival = svalset_.find(sval)->second;

    // return string set index if mapping disabled
    if (! isMapEnabled())
      return ival;

    // map string set index using 1 -> number of sets
    int slen = svalset_.size();

    if (slen)
      return CQChartsUtil::map(ival, 1, slen, mapMin, mapMax);
    else
      return mapMin;
  }
}

int
CQChartsValueSet::
imin() const
{
  if      (type() == Type::INTEGER)
    return *iset_.begin();
  else if (type() == Type::REAL)
    return setrval_.begin()->first;
  else
    return setsval_.begin()->first;
}

int
CQChartsValueSet::
imax() const
{
  if      (type() == Type::INTEGER)
    return *iset_.rbegin();
  else if (type() == Type::REAL)
    return setrval_.rbegin()->first;
  else
    return setsval_.rbegin()->first;
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

  ivals_  .clear();
  iset_   .clear();

  rvals_  .clear();
  rvalset_.clear();
  setrval_.clear();

  svals_  .clear();
  svalset_.clear();
  setsval_.clear();

  bool ok;

  for (const auto &value : values_) {
    if      (type_ == Type::INTEGER) {
      int i = CQChartsUtil::toInt(value, ok);

      ivals_.push_back(i);
      iset_ .insert(i);
    }
    else if (type_ == Type::REAL) {
      double r = CQChartsUtil::toReal(value, ok);

      rvals_.push_back(r);

      auto p = rvalset_.find(r);

      if (p == rvalset_.end()) {
        int id = rvalset_.size() + 1;

        p = rvalset_.insert(p, RValSet::value_type(r, id));

        setrval_[id] = r;
      }
    }
    else {
      QString s = value.toString();

      svals_.push_back(s);

      auto p = svalset_.find(s);

      if (p == svalset_.end()) {
        int id = svalset_.size() + 1;

        p = svalset_.insert(p, SValSet::value_type(s, id));

        setsval_[id] = s;
      }
    }
  }
}
