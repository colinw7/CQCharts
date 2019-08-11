#include <CQValueSet.h>
#include <CQTrie.h>

CQValueSet::
CQValueSet()
{
}

bool
CQValueSet::
isValid() const
{
  if      (type() == Type::INTEGER) return ivals_.isValid();
  else if (type() == Type::REAL   ) return rvals_.isValid();
  else if (type() == Type::STRING ) return svals_.isValid();
  else                              return false;
}

bool
CQValueSet::
canMap() const
{
  if      (type() == Type::INTEGER) return ivals_.canMap();
  else if (type() == Type::REAL   ) return rvals_.canMap();
  else if (type() == Type::STRING ) return svals_.canMap();
  else                              return false;
}

void
CQValueSet::
addValue(const QVariant &value)
{
  values_.push_back(value);

  initialized_ = false;
}

void
CQValueSet::
clear()
{
  type_ = Type::NONE;

  values_.clear();

  initialized_ = false;
}

bool
CQValueSet::
hasInd(int i) const
{
  if      (type() == Type::INTEGER)
    return (i >= 0 && i < ivals_.size());
  else if (type() == Type::REAL)
    return (i >= 0 && i < rvals_.size());
  else if (type() == Type::STRING)
    return (i >= 0 && i < svals_.size());
  else
    return false;
}

int
CQValueSet::
iset(const QVariant &value) const
{
  bool ok;

  if      (type() == Type::INTEGER) {
    int i = value.toInt(&ok);

    if (ok)
      return ivals_.id(i);
  }
  else if (type() == Type::REAL) {
    double r = value.toDouble(&ok);

    if (! isAllowNaN() && CMathUtil::isNaN(r))
      ok = false;

    if (ok)
      return rvals_.id(r);
  }
  else if (type() == Type::STRING) {
    QString s = value.toString();

    return svals_.id(s);
  }

  return 0;
}

int
CQValueSet::
numUnique() const
{
  if      (type() == Type::INTEGER)
    return ivals_.numUnique();
  else if (type() == Type::REAL)
    return rvals_.numUnique();
  else if (type() == Type::STRING)
    return svals_.numUnique();

  return 0;
}

double
CQValueSet::
imap(const QVariant &value) const
{
  int i = iset(value);
  int n = numUnique();

  if (n == 0)
    return 0.0;

  return (1.0*i)/n;
}

double
CQValueSet::
imap(int i) const
{
  double min = mapMin();
  double max = std::max(min, mapMax());

  return imap(i, min, max);
}

double
CQValueSet::
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
  else {
    return mapMin;
  }
}

int
CQValueSet::
sbucket(const QString &s) const
{
  init();

  if (type() == Type::STRING)
    return svals_.sbucket(s);
  else
    return -1;
}

QString
CQValueSet::
buckets(int i) const
{
  init();

  if (type() == Type::STRING)
    return svals_.buckets(i);
  else
    return "";
}

int
CQValueSet::
sind(const QString &s) const
{
  init();

  if (type() == Type::STRING)
    return svals_.id(s);
  else
    return -1;
}

QString
CQValueSet::
inds(int ind) const
{
  init();

  if (type() == Type::STRING)
    return svals_.ivalue(ind);
  else
    return "";
}

int
CQValueSet::
snum() const
{
  init();

  if (type() == Type::STRING)
    return svals_.numUnique();
  else
    return -1;
}

int
CQValueSet::
imin(int def) const
{
  if      (type() == Type::INTEGER)
    return ivals_.imin(def);
  else if (type() == Type::REAL)
    return rvals_.imin(def);
  else if (type() == Type::STRING)
    return svals_.imin(def);
  else
    return def;
}

int
CQValueSet::
imax(int def) const
{
  if      (type() == Type::INTEGER)
    return ivals_.imax(def);
  else if (type() == Type::REAL)
    return rvals_.imax(def);
  else if (type() == Type::STRING)
    return svals_.imax(def);
  else
    return def;
}

double
CQValueSet::
rmin(double def) const
{
  if      (type() == Type::INTEGER)
    return ivals_.min(def);
  else if (type() == Type::REAL)
    return rvals_.min(def);
  else if (type() == Type::STRING)
    return svals_.imin(def);
  else
    return def;
}

double
CQValueSet::
rmax(double def) const
{
  if      (type() == Type::INTEGER)
    return ivals_.max(def);
  else if (type() == Type::REAL)
    return rvals_.max(def);
  else if (type() == Type::STRING)
    return svals_.imax(def);
  else
    return def;
}

double
CQValueSet::
rsum() const
{
  if      (type() == Type::INTEGER)
    return ivals_.sum();
  else if (type() == Type::REAL)
    return rvals_.sum();
  else if (type() == Type::STRING)
    return 0.0;
  else
    return 0.0;
}

double
CQValueSet::
rmean() const
{
  if      (type() == Type::INTEGER)
    return ivals_.mean();
  else if (type() == Type::REAL)
    return rvals_.mean();
  else if (type() == Type::STRING)
    return (svals_.imin() + svals_.imax())/2.0;
  else
    return 0.0;
}

int
CQValueSet::
rid(double r) const
{
  if      (type() == Type::INTEGER)
    return -1;
  else if (type() == Type::REAL)
    return rvals_.id(r);
  else if (type() == Type::STRING)
    return -1;
  else
    return -1;
}

double
CQValueSet::
idr(int i) const
{
  if      (type() == Type::INTEGER)
    return 0.0;
  else if (type() == Type::REAL)
    return rvals_.ivalue(i);
  else if (type() == Type::STRING)
    return 0.0;
  else
    return 0.0;
}

int
CQValueSet::
iid(int i) const
{
  if      (type() == Type::INTEGER)
    return ivals_.id(i);
  else if (type() == Type::REAL)
    return -1;
  else if (type() == Type::STRING)
    return -1;
  else
    return -1;
}

int
CQValueSet::
idi(int i) const
{
  if      (type() == Type::INTEGER)
    return ivals_.ivalue(i);
  else if (type() == Type::REAL)
    return 0;
  else if (type() == Type::STRING)
    return 0;
  else
    return 0;
}

void
CQValueSet::
init() const
{
  if (initialized_)
    return;

  std::unique_lock<std::mutex> lock(mutex_);

  if (! initialized_) {
    CQValueSet *th = const_cast<CQValueSet *>(this);

    th->init();
  }
}

void
CQValueSet::
init()
{
  initialized_ = true;

  // if no type then look at added values
  if (type_ == Type::NONE)
    type_ = calcType();

  //---

  clearVals();

  bool ok;

  for (const auto &value : values_) {
    if      (type() == Type::INTEGER) {
      int i = value.toInt(&ok);

      ivals_.addValue(ok ? OptInt(i) : OptInt());
    }
    else if (type() == Type::REAL) {
      double r = value.toDouble(&ok);

      if (! isAllowNaN() && CMathUtil::isNaN(r))
        ok = false;

      rvals_.addValue(ok ? OptReal(r) : OptReal());
    }
    else if (type() == Type::STRING) {
      QString s;

      bool ok = false;

      if (value.isValid()) {
        s = value.toString();

        ok = true;
      }

      svals_.addValue(ok ? OptString(s) : OptString());
    }
  }
}

CQValueSet::Type
CQValueSet::
calcType() const
{
  CQValueSet::Type type = Type::NONE;

  int ni = 0, nr = 0;

  for (const auto &value : values_) {
    if      (value.type() == QVariant::Int)
      ++ni;
    else if (value.type() == QVariant::Double) {
      ++nr;

      bool ok;

      double r = value.toDouble(&ok);

      if (CMathUtil::isInteger(r))
        ++ni;
      else
        ++nr;
    }
    else {
      bool ok;

      double r = value.toDouble(&ok);

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
CQValueSet::
clearVals()
{
  ivals_.clear();
  rvals_.clear();
  svals_.clear();
}

//------

int
CQRValues::
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
CQRValues::
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
CQRValues::
isOutlier(double v) const
{
  initCalc();

  return statData_.isOutlier(v);
}

//------

int
CQIValues::
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
CQIValues::
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

  int i = 0;

  for (auto v : svalues_) {
    if (statData_.isOutlier(v))
      outliers_.push_back(i);

    ++i;
  }
}

bool
CQIValues::
isOutlier(int v) const
{
  initCalc();

  return statData_.isOutlier(v);
}

//------

CQSValues::
CQSValues()
{
  trie_      = new CQTrie;
  spatterns_ = new CQTriePatterns;
}

CQSValues::
~CQSValues()
{
  delete trie_;
  delete spatterns_;
}

void
CQSValues::
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
CQSValues::
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
CQSValues::
sbucket(const QString &s) const
{
  initPatterns(initBuckets_);

  return trie_->patternIndex(s, *spatterns_);
}

QString
CQSValues::
buckets(int i) const
{
  initPatterns(initBuckets_);

  return trie_->indexPattern(i, *spatterns_);
}

void
CQSValues::
initPatterns(int numIdeal) const
{
  if (spatternsSet_)
    return;

  std::unique_lock<std::mutex> lock(mutex_);

  if (! spatternsSet_) {
    CQSValues *th = const_cast<CQSValues *>(this);

    using DepthCountMap = std::map<int,CQTriePatterns>;

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
