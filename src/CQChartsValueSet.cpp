#include <CQChartsValueSet.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQTrie.h>

CQChartsValueSet::
CQChartsValueSet(const CQChartsPlot *plot) :
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

#if 0
void
CQChartsValueSet::
addProperties(const QString &path)
{
  if (plot_) {
    plot_->addProperty(path, this, "mapped", "mapped")->setDesc("Is mapped");
    plot_->addProperty(path, this, "mapMin", "mapMin")->setDesc("Map min");
    plot_->addProperty(path, this, "mapMax", "mapMax")->setDesc("Map max");
  }
}
#endif

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

int
CQChartsValueSet::
iset(const QVariant &value) const
{
  bool ok;

  if      (type() == Type::INTEGER) {
    int i = (int) CQChartsVariant::toInt(value, ok);

    if (ok)
      return ivals_.id(i);
  }
  else if (type() == Type::REAL) {
    double r = CQChartsVariant::toReal(value, ok);

    if (! isAllowNaN() && CMathUtil::isNaN(r))
      ok = false;

    if (ok)
      return rvals_.id(r);
  }
  else if (type() == Type::STRING) {
    QString s;

    CQChartsVariant::toString(value, s);

    return svals_.id(s);
  }
  else if (type() == Type::COLOR) {
    auto c = CQChartsVariant::toColor(value, ok);

    if (ok)
      return cvals_.id(c);
  }
  else if (type() == Type::TIME) {
    double t = CQChartsVariant::toReal(value, ok);

    if (ok)
      return rvals_.id(t);
  }

  return 0;
}

int
CQChartsValueSet::
numUnique() const
{
  if      (type() == Type::INTEGER)
    return ivals_.numUnique();
  else if (type() == Type::REAL)
    return rvals_.numUnique();
  else if (type() == Type::STRING)
    return svals_.numUnique();
  else if (type() == Type::COLOR)
    return cvals_.numUnique();
  else if (type() == Type::TIME)
    return rvals_.numUnique();

  return 0;
}

QVariant
CQChartsValueSet::
uniqueValue() const
{
  if      (type() == Type::INTEGER)
    return ivals_.uniqueValue();
  else if (type() == Type::REAL)
    return rvals_.uniqueValue();
  else if (type() == Type::STRING)
    return svals_.uniqueValue();
  else if (type() == Type::COLOR)
    return cvals_.uniqueValue();
  else if (type() == Type::TIME)
    return rvals_.uniqueValue();

  return QVariant();
}

double
CQChartsValueSet::
imap(const QVariant &value) const
{
  int i = iset(value);
  int n = numUnique();

  if (n == 0)
    return 0.0;

  return (1.0*i)/n;
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
    auto cval = cvals_.value(i);

    if (! cval.isValid())
      return mapMin;

    // return color set index if mapping disabled
    if (! isMapped())
      return cvals_.id(cval);

    // map string using number of sets
    return cvals_.map(cval, mapMin, mapMax);
  }
  else if (type() == Type::TIME) {
    // get nth tile
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
imin(int def) const
{
  if      (type() == Type::INTEGER)
    return ivals_.imin(def);
  else if (type() == Type::REAL)
    return rvals_.imin(def);
  else if (type() == Type::STRING)
    return svals_.imin(def);
  else if (type() == Type::COLOR)
    return cvals_.imin(def);
  else if (type() == Type::TIME)
    return tvals_.imin(def);
  else
    return def;
}

int
CQChartsValueSet::
imax(int def) const
{
  if      (type() == Type::INTEGER)
    return ivals_.imax(def);
  else if (type() == Type::REAL)
    return rvals_.imax(def);
  else if (type() == Type::STRING)
    return svals_.imax(def);
  else if (type() == Type::COLOR)
    return cvals_.imax(def);
  else if (type() == Type::TIME)
    return tvals_.imax(def);
  else
    return def;
}

double
CQChartsValueSet::
rmin(double def) const
{
  if      (type() == Type::INTEGER)
    return (double) ivals_.min(int(def));
  else if (type() == Type::REAL)
    return rvals_.min(def);
  else if (type() == Type::STRING)
    return svals_.imin(int(def));
  else if (type() == Type::COLOR)
    return cvals_.imin(int(def));
  else if (type() == Type::TIME)
    return tvals_.min(def);
  else
    return def;
}

double
CQChartsValueSet::
rmax(double def) const
{
  if      (type() == Type::INTEGER)
    return (double) ivals_.max(int(def));
  else if (type() == Type::REAL)
    return rvals_.max(def);
  else if (type() == Type::STRING)
    return svals_.imax(int(def));
  else if (type() == Type::COLOR)
    return cvals_.imax(int(def));
  else if (type() == Type::TIME)
    return tvals_.max(def);
  else
    return def;
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
reals(std::vector<double> &reals) const
{
  if      (ivals_.isValid()) {
    int n = ivals_.size();

    for (int i = 0; i < n; ++i) {
      OptInt ival = ivals_.value(i);
      if (! ival) continue;

      reals.push_back(*ival);
    }
  }
  else if (rvals_.isValid()) {
    int n = rvals_.size();

    for (int i = 0; i < n; ++i) {
      OptReal rval = rvals_.value(i);
      if (! rval) continue;

      reals.push_back(*rval);
    }
  }
  else if (svals_.isValid()) {
    int n = svals_.size();

    for (int i = 0; i < n; ++i) {
      OptString sval = svals_.value(i);
      if (! sval) continue;

      bool ok;
      double r = (*sval).toDouble(&ok);
      if (! ok) continue;

      reals.push_back(r);
    }
  }
}

void
CQChartsValueSet::
init() const
{
  if (initialized_)
    return;

  std::unique_lock<std::mutex> lock(mutex_);

  if (! initialized_) {
    auto *th = const_cast<CQChartsValueSet *>(this);

    th->init();
  }
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

  // if no type then look at added value (TODO: always the same as column values ?)
  if (type_ == Type::NONE)
    type_ = calcType();

  //---

  clearVals();

  bool ok;

  for (const auto &value : values_) {
    if      (type() == Type::INTEGER) {
      int i = (int) CQChartsVariant::toInt(value, ok);

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
      auto c = CQChartsVariant::toColor(value, ok);

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

CQChartsValueSet::Type
CQChartsValueSet::
calcType() const
{
  auto type = Type::NONE;

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
        type = Type::STRING;
        break;
      }
    }
  }

  if (type == Type::NONE) {
    if      (nr == 0 && ni > 0)
      type = Type::INTEGER;
    else if (nr > 0)
      type = Type::REAL;
    else
      type = Type::STRING;
  }

  return type;
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

//------

int
CQChartsRValues::
addValue(const OptReal &r)
{
  // add to all values
  values_.push_back(r);

  calculated_ = false;

  // TODO: don't calc key unless needed

  // TODO: assert
  if (! r) {
    ++numNull_;

    return -1;
  }

  // add to unique values if new
  auto p = valset_.find(*r);

  if (p == valset_.end()) {
    int id = valset_.size();

    p = valset_.insert(p, ValueSet::value_type(*r, KeyCount(id, 1))); // id for value

    setvals_[id] = *r; // value for id
  }
  else {
    ++(*p).second.second; // increment count
  }

  return (*p).second.first; // return key
}

void
CQChartsRValues::
calc()
{
  if (calculated_)
    return;

  calculated_ = true;

  //---

  // init statistics
  statData_.reset();

  outliers_.clear();

  svalues_.clear();

  //---

  // no values then nothing to do
  if (values_.empty())
    return;

  //---

  // get value to sort (skip null values)
  for (auto &v : values_) {
    if (! v) continue;

    double r = *v;

    svalues_.push_back(r);
  }

  if (svalues_.empty())
    return;

  //---

  // sort values
  std::sort(svalues_.begin(), svalues_.end());

  //---

  statData_.calcStatValues(svalues_);

  //---

  int i = 0;

  for (auto v : svalues_) {
    if (statData_.isOutlier(v))
      outliers_.push_back(i);

    ++i;
  }
}

bool
CQChartsRValues::
isOutlier(double v) const
{
  initCalc();

  return statData_.isOutlier(v);
}

//------

int
CQChartsIValues::
addValue(const OptInt &i)
{
  // add to all values
  values_.push_back(i);

  calculated_ = false;

  // TODO: don't calc key unless needed

  // TODO: assert
  if (! i) {
    ++numNull_;

    return -1;
  }

  // add to unique values if new
  auto p = valset_.find(*i);

  if (p == valset_.end()) {
    int id = valset_.size();

    p = valset_.insert(p, ValueSet::value_type(*i, KeyCount(id, 1))); // id for value

    setvals_[id] = *i; // value for id
  }
  else {
    ++(*p).second.second; // increment count
  }

  return (*p).second.first; // return key
}

void
CQChartsIValues::
calc()
{
  if (calculated_)
    return;

  calculated_ = true;

  //---

  // init statistics
  statData_.reset();

  outliers_.clear();

  svalues_.clear();

  //---

  // no values then nothing to do
  if (values_.empty())
    return;

  //---

  // get value to sort (skip null values)
  for (auto &v : values_) {
    if (! v) continue;

    int i = *v;

    svalues_.push_back(i);
  }

  if (svalues_.empty())
    return;

  //---

  // sort values
  std::sort(svalues_.begin(), svalues_.end());

  //---

  statData_.calcStatValues(svalues_);

  //---

  int i = 0;

  for (auto v : svalues_) {
    if (statData_.isOutlier(v))
      outliers_.push_back(i);

    ++i;
  }
}

bool
CQChartsIValues::
isOutlier(int v) const
{
  initCalc();

  return statData_.isOutlier(v);
}

//------

CQChartsSValues::
CQChartsSValues()
{
  trie_      = new CQTrie;
  spatterns_ = new CQTriePatterns;
}

CQChartsSValues::
~CQChartsSValues()
{
  delete trie_;
  delete spatterns_;
}

void
CQChartsSValues::
clear()
{
  values_ .clear();
  valset_ .clear();
  setvals_.clear();

  numNull_ = 0;

  trie_->clear();

  spatterns_->clear();

  spatternsSet_ = false;
}

int
CQChartsSValues::
addValue(const OptString &s)
{
  // add to all values
  values_.push_back(s);

  // TODO: don't calc key unless needed

  // TODO: assert
  if (! s) {
    ++numNull_;

    return -1;
  }

  // add to trie
  trie_->addWord(*s);

  // add to unique values if new
  auto p = valset_.find(*s);

  if (p == valset_.end()) {
    int id = valset_.size();

    p = valset_.insert(p, ValueSet::value_type(*s, KeyCount(id, 1))); // id for value

    setvals_[id] = *s; // value for id
  }
  else {
    ++(*p).second.second; // increment count
  }

  return (*p).second.first; // return key
}

int
CQChartsSValues::
sbucket(const QString &s) const
{
  initPatterns(initBuckets_);

  return trie_->patternIndex(s, *spatterns_);
}

QString
CQChartsSValues::
buckets(int i) const
{
  initPatterns(initBuckets_);

  return trie_->indexPattern(i, *spatterns_);
}

void
CQChartsSValues::
initPatterns(int numIdeal) const
{
  if (spatternsSet_)
    return;

  std::unique_lock<std::mutex> lock(mutex_);

  if (! spatternsSet_) {
    auto *th = const_cast<CQChartsSValues *>(this);

    using DepthCountMap = std::map<int, CQTriePatterns>;

    DepthCountMap depthCountMap;

    for (int depth = 1; depth <= 3; ++depth) {
      CQTriePatterns patterns;

      trie_->patterns(depth, patterns);

      depthCountMap[depth] = patterns;

      //patterns.print(std::cerr);
    }

    int minD     = -1;
    int minDepth = -1;

    for (const auto &depthCount : depthCountMap) {
      int d = std::abs(depthCount.second.numPatterns() - numIdeal);

      if (minDepth < 0 || d < minD) {
        minD     = d;
        minDepth = depthCount.first;
      }
    }

    *th->spatterns_ = depthCountMap[minDepth];

    th->spatternsSet_ = true;

    //spatterns_->print(std::cerr);
  }
}

//------

int
CQChartsCValues::
addValue(const CQChartsColor &c)
{
  // add to all values
  values_.push_back(c);

  // TODO: don't calc key unless needed

  // TODO: assert
  if (! c.isValid()) {
    ++numNull_;

    return -1;
  }

  // add to unique values if new
  auto p = valset_.find(c);

  if (p == valset_.end()) {
    int id = valset_.size();

    p = valset_.insert(p, ValueSet::value_type(c, KeyCount(id, 1))); // id for value

    setvals_[id] = c; // value for id
  }
  else {
    ++(*p).second.second; // increment count
  }

  return (*p).second.first; // return key
}
