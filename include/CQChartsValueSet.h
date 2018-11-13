#ifndef CQChartsValueSet_H
#define CQChartsValueSet_H

#include <CQChartsUtil.h>
#include <CQChartsTrie.h>
#include <vector>
#include <set>
#include <map>
#include <boost/optional.hpp>

class CQChartsPlot;

//------

class CQChartsRValues {
 public:
  using OptReal = boost::optional<double>;
  using Values  = std::vector<double>;
  using Counts  = std::vector<int>;
  using Indices = std::vector<int>;

 public:
  CQChartsRValues() { }

  void clear() {
    values_ .clear();
    svalues_.clear();
    valset_ .clear();
    setvals_.clear();

    numNull_    = 0;
    calculated_ = false;
  }

  bool isValid() const { return ! values_.empty(); }

  bool canMap() const { return ! valset_.empty(); }

  int size() const { return values_.size(); }

  // get nth value (non-unique)
  const OptReal &value(int i) const { return values_[i]; }

  int addValue(const OptReal &r) {
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

  int numNull() const { return numNull_; }

  // real to id
  int id(double r) const {
    // get real set index
    auto p = valset_.find(r);

    if (p == valset_.end())
      return -1;

    return (*p).second.first;
  }

  // id to real
  double ivalue(int i) const {
    // get real for index
    auto p = setvals_.find(i);

    if (p == setvals_.end())
      return 0.0;

    return (*p).second;
  }

  // map value into real in range
  double map(double r, double mapMin=0.0, double mapMax=1.0) const {
    // map value using real value range
    double rmin = this->min();
    double rmax = this->max();

    if (rmin == rmax)
      return mapMin;

    return CMathUtil::map(r, rmin, rmax, mapMin, mapMax);
  }

  // min/max value
  double min() const {
    if (valset_.empty()) return CMathUtil::getNaN();
    return valset_. begin()->first;
  }
  double max() const {
    if (valset_.empty()) return CMathUtil::getNaN();
    return valset_.rbegin()->first;
  }

  // min/max index
  int imin() const { if (setvals_.empty()) return -1; return setvals_. begin()->first; }
  int imax() const { if (setvals_.empty()) return -1; return setvals_.rbegin()->first; }

  // number of unique values
  int numUnique() const { return valset_.size(); }

  void uniqueValues(Values &values) {
    for (const auto &vi : valset_)
      values.push_back(vi.first);
  }

  void uniqueCounts(Counts &counts) {
    for (const auto &vi : valset_)
      counts.push_back(vi.second.second);
  }

  // calculated stats
  double sum () const { const_calc(); return sum_ ; }
  double mean() const { const_calc(); return mean_; }

  double stddev() const { const_calc(); return stddev_; }

  double median() const { const_calc(); return median_; }

  double lowerMedian() const { const_calc(); return lowerMedian_; }
  double upperMedian() const { const_calc(); return upperMedian_; }

  const Indices &outliers() const { const_calc(); return outliers_; }

  double svalue(int i) const { return svalues_[i]; }

 private:
  void const_calc() const {
    const_cast<CQChartsRValues *>(this)->calc();
  }

  void calc() {
    if (calculated_)
      return;

    calculated_ = true;

    //---

    // init statistics
    sum_         = 0.0;
    mean_        = 0.0;
    stddev_      = 0.0;
    median_      = 0.0;
    lowerMedian_ = 0.0;
    upperMedian_ = 0.0;

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

      sum_ += r;

      svalues_.push_back(r);
    }

    if (svalues_.empty())
      return;

    int n = svalues_.size();

    mean_ = sum_/n;

    //---

    double sum2 = 0.0;

    for (auto &v : values_) {
      if (! v) continue;

      double r = *v;

      double dr = (r - mean_);

      sum2 += dr*dr;
    }

    stddev_ = (n > 1 ? sqrt(sum2)/(n - 1) : 0.0);

    //---

    // sort values
    std::sort(svalues_.begin(), svalues_.end());

    int nv = svalues_.size();

    //---

    // calc median
    int nv1, nv2;

    medianInd(0, nv - 1, nv1, nv2);

    median_ = (svalues_[nv1] + svalues_[nv2])/2.0;

    // calc lower median
    if (nv1 > 0) {
      int nl1, nl2;

      medianInd(0, nv1 - 1, nl1, nl2);

      lowerMedian_ = (svalues_[nl1] + svalues_[nl2])/2.0;
    }
    else
      lowerMedian_ = svalues_[0];

    // calc upper median
    if (nv2 < nv - 1) {
      int nu1, nu2;

      medianInd(nv2 + 1, nv - 1, nu1, nu2);

      upperMedian_ = (svalues_[nu1] + svalues_[nu2])/2.0;
    }
    else
      upperMedian_ = svalues_[nv - 1];

    //---

    // calc outliers outside range()*(upper - lower)
    double routlier = upperMedian_ - lowerMedian_;
    double loutlier = lowerMedian_ - outlierRange_*routlier;
    double uoutlier = upperMedian_ + outlierRange_*routlier;

    int i = 0;

    for (auto v : svalues_) {
      if (v < loutlier || v > uoutlier)
        outliers_.push_back(i);

      ++i;
    }
  }

  void medianInd(int i1, int i2, int &n1, int &n2) {
    int n = i2 - i1 + 1;

    if (n & 1) {
      n1 = i1 + n/2;
      n2 = n1;
    }
    else {
      n2 = i1 + n/2;
      n1 = n2 - 1;
    }
  }

 private:
  using OptValues = std::vector<OptReal>;
  using KeyCount  = std::pair<int,int>;
  using ValueSet  = std::map<double,KeyCount,CQChartsUtil::RealCmp>;
  using SetValues = std::map<int,double>;

  OptValues values_;                 // all real values
  Values    svalues_;                // sorted real values
  ValueSet  valset_;                 // unique indexed real values
  SetValues setvals_;                // index to real map
  int       numNull_      { 0 };     // number of null values
  bool      calculated_   { false }; // are stats calculated
  double    sum_          { 0.0 };
  double    mean_         { 0.0 };
  double    stddev_       { 0.0 };
  double    median_       { 0.0 };
  double    lowerMedian_  { 0.0 };
  double    upperMedian_  { 0.0 };
  double    outlierRange_ { 1.5 };
  Indices   outliers_;
};

//---

class CQChartsIValues {
 public:
  using OptInt  = boost::optional<int>;
  using Values  = std::vector<double>;
  using Counts  = std::vector<int>;
  using Indices = std::vector<int>;

 public:
  CQChartsIValues() { }

  void clear() {
    values_ .clear();
    svalues_.clear();
    valset_ .clear();
    setvals_.clear();

    numNull_    = 0;
    calculated_ = false;
  }

  bool isValid() const { return ! values_.empty(); }

  bool canMap() const { return ! valset_.empty(); }

  int size() const { return values_.size(); }

  // get nth value (non-unique)
  const OptInt &value(int i) const { return values_[i]; }

  int addValue(const OptInt &i) {
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

  int numNull() const { return numNull_; }

  // integer to id
  int id(int i) const {
    // get integer set index
    auto p = valset_.find(i);

    if (p == valset_.end())
      return -1;

    return (*p).second.first;
  }

  // id to integer
  int ivalue(int i) const {
    // get integer for index
    auto p = setvals_.find(i);

    if (p == setvals_.end())
      return 0;

    return (*p).second;
  }

  // map value into real in range
  double map(int i, double mapMin=0.0, double mapMax=1.0) const {
    // map value using real value range
    double imin = this->min();
    double imax = this->max();

    if (imin == imax)
      return mapMin;

    return CMathUtil::map(i, imin, imax, mapMin, mapMax);
  }

  // min/max value
  int min() const { assert(! valset_.empty()); return valset_. begin()->first; }
  int max() const { assert(! valset_.empty()); return valset_.rbegin()->first; }

  // min/max index
  int imin() const { assert(! setvals_.empty()); return setvals_. begin()->first; }
  int imax() const { assert(! setvals_.empty()); return setvals_.rbegin()->first; }

  // number of unique values
  int numUnique() const { return valset_.size(); }

  void uniqueValues(Values &values) {
    for (const auto &vi : valset_)
      values.push_back(vi.first);
  }

  void uniqueCounts(Counts &counts) {
    for (const auto &vi : valset_)
      counts.push_back(vi.second.second);
  }

  // calculated stats
  double sum () const { const_calc(); return sum_ ; }
  double mean() const { const_calc(); return mean_; }

  double stddev() const { const_calc(); return stddev_; }

  double median() const { const_calc(); return median_; }

  double lowerMedian() const { const_calc(); return lowerMedian_; }
  double upperMedian() const { const_calc(); return upperMedian_; }

  const Indices &outliers() const { const_calc(); return outliers_; }

  double svalue(int i) const { return svalues_[i]; }

 private:
  void const_calc() const {
    const_cast<CQChartsIValues *>(this)->calc();
  }

  void calc() {
    if (calculated_)
      return;

    calculated_ = true;

    //---

    // init statistics
    sum_         = 0.0;
    mean_        = 0.0;
    median_      = 0.0;
    lowerMedian_ = 0.0;
    upperMedian_ = 0.0;

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

      sum_ += i;

      svalues_.push_back(i);
    }

    if (svalues_.empty())
      return;

    int n = svalues_.size();

    mean_ = sum_/n;

    //---

    double sum2 = 0.0;

    for (auto &v : values_) {
      if (! v) continue;

      double r = *v;

      double dr = (r - mean_);

      sum2 += dr*dr;
    }

    stddev_ = (n > 1 ? sqrt(sum2)/(n - 1) : 0.0);

    //---

    // sort values
    std::sort(svalues_.begin(), svalues_.end());

    int nv = svalues_.size();

    //---

    // calc median
    int nv1, nv2;

    medianInd(0, nv - 1, nv1, nv2);

    median_ = (svalues_[nv1] + svalues_[nv2])/2.0;

    // calc lower median
    if (nv1 > 0) {
      int nl1, nl2;

      medianInd(0, nv1 - 1, nl1, nl2);

      lowerMedian_ = (svalues_[nl1] + svalues_[nl2])/2.0;
    }
    else
      lowerMedian_ = svalues_[0];

    // calc upper median
    if (nv2 < nv - 1) {
      int nu1, nu2;

      medianInd(nv2 + 1, nv - 1, nu1, nu2);

      upperMedian_ = (svalues_[nu1] + svalues_[nu2])/2.0;
    }
    else
      upperMedian_ = svalues_[nv - 1];

    //---

    // calc outliers outside range()*(upper - lower)
    double routlier = upperMedian_ - lowerMedian_;
    double loutlier = lowerMedian_ - outlierRange_*routlier;
    double uoutlier = upperMedian_ + outlierRange_*routlier;

    int i = 0;

    for (auto v : svalues_) {
      if (v < loutlier || v > uoutlier)
        outliers_.push_back(i);

      ++i;
    }
  }

  void medianInd(int i1, int i2, int &n1, int &n2) {
    int n = i2 - i1 + 1;

    if (n & 1) {
      n1 = i1 + n/2;
      n2 = n1;
    }
    else {
      n2 = i1 + n/2;
      n1 = n2 - 1;
    }
  }

 private:
  using OptValues = std::vector<OptInt>;
  using KeyCount  = std::pair<int,int>;
  using ValueSet  = std::map<int,KeyCount>;
  using SetValues = std::map<int,int>;

  OptValues values_;                 // all integer values
  Values    svalues_;                // sorted integer values
  ValueSet  valset_;                 // unique indexed integer values
  SetValues setvals_;                // index to integr map
  int       numNull_      { 0 };     // number of null values
  bool      calculated_   { false }; // are stats calculated
  double    sum_          { 0.0 };
  double    mean_         { 0.0 };
  double    stddev_       { 0.0 };
  double    median_       { 0.0 };
  double    lowerMedian_  { 0.0 };
  double    upperMedian_  { 0.0 };
  double    outlierRange_ { 1.5 };
  Indices   outliers_;
};

//---

class CQChartsSValues {
 public:
  using OptString = boost::optional<QString>;
  using Values    = std::vector<QString>;
  using Counts    = std::vector<int>;

 public:
  CQChartsSValues() { }

  void clear() {
    values_ .clear();
    valset_ .clear();
    setvals_.clear();

    numNull_ = 0;

    trie_.clear();

    spatterns_.clear();

    spatternsSet_ = false;
  }

  bool isValid() const { return ! values_.empty(); }

  bool canMap() const { return ! valset_.empty(); }

  int size() const { return values_.size(); }

  // get nth value (non-unique)
  const OptString &value(int i) const { return values_[i]; }

  int addValue(const OptString &s) {
    // add to all values
    values_.push_back(s);

    // TODO: don't calc key unless needed

    // TODO: assert
    if (! s) {
      ++numNull_;

      return -1;
    }

    // add to trie
    trie_.addWord(*s);

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

  int numNull() const { return numNull_; }

  // string to id
  int id(const QString &s) const {
    // get string set index
    auto p = valset_.find(s);

    if (p == valset_.end())
      return -1;

    return (*p).second.first;
  }

  // id to string
  QString ivalue(int i) const {
    // get string for index
    auto p = setvals_.find(i);

    if (p == setvals_.end())
      return "";

    return (*p).second;
  }

  // min/max value
  QString min() const { assert(! valset_.empty()); return valset_. begin()->first; }
  QString max() const { assert(! valset_.empty()); return valset_.rbegin()->first; }

  // min/max index
  int imin() const { assert(! setvals_.empty()); return setvals_. begin()->first; }
  int imax() const { assert(! setvals_.empty()); return setvals_.rbegin()->first; }

  // number of unique values
  int numUnique() const { return valset_.size(); }

  void uniqueValues(Values &values) {
    for (const auto &vi : valset_)
      values.push_back(vi.first);
  }

  void uniqueCounts(Counts &counts) {
    for (const auto &vi : valset_)
      counts.push_back(vi.second.second);
  }

  // map value into real in range
  double map(const QString &s, double mapMin=0.0, double mapMax=1.0) const {
    // get string set index
    int i = id(s);

    // map string set index using 1 -> number of unique values
    int n = numUnique();

    if (! n)
      return mapMin;

    return CMathUtil::map(i, imin(), imax(), mapMin, mapMax);
  }

  int sbucket(const QString &s) const {
    initPatterns(initBuckets_);

    return trie_.patternIndex(s, spatterns_);
  }

  QString buckets(int i) const {
    initPatterns(initBuckets_);

    return trie_.indexPattern(i, spatterns_);
  }

 private:
  void initPatterns(int numIdeal) const {
    if (spatternsSet_)
      return;

    using DepthCountMap = std::map<int,CQChartsTrie::Patterns>;

    DepthCountMap depthCountMap;

    for (int depth = 1; depth <= 3; ++depth) {
      CQChartsTrie::Patterns patterns;

      trie_.patterns(depth, patterns);

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

    CQChartsSValues *th = const_cast<CQChartsSValues *>(this);

    th->spatterns_    = depthCountMap[minDepth];
    th->spatternsSet_ = true;

    //spatterns_.print(std::cerr);
  }

 private:
  using OptValues = std::vector<OptString>;
  using KeyCount  = std::pair<int,int>;
  using ValueSet  = std::map<QString,KeyCount>;
  using SetValues = std::map<int,QString>;

  OptValues values_;        // all string values
  ValueSet  valset_;        // unique indexed string values
  SetValues setvals_;       // index to string map
  int       numNull_ { 0 }; // number of null values

  int                    initBuckets_  { 10 };
  CQChartsTrie           trie_;                   // string trie
  CQChartsTrie::Patterns spatterns_;              // trie patterns
  bool                   spatternsSet_ { false }; // trie patterns set
};

//---

class CQChartsCValues {
 public:
  using Values = std::vector<CQChartsColor>;
  using Counts = std::vector<int>;

 public:
  CQChartsCValues() { }

  void clear() {
    values_ .clear();
    valset_ .clear();
    setvals_.clear();

    numNull_ = 0;
  }

  bool isValid() const { return ! values_.empty(); }

  bool canMap() const { return ! valset_.empty(); }

  int size() const { return values_.size(); }

  // get nth value (non-unique)
  const CQChartsColor &value(int i) const { return values_[i]; }

  int addValue(const CQChartsColor &c) {
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

  int numNull() const { return numNull_; }

  // color to id
  int id(const CQChartsColor &c) const {
    // get color set index
    auto p = valset_.find(c);

    if (p == valset_.end())
      return -1;

    return (*p).second.first;
  }

  // id to color
  CQChartsColor ivalue(int i) const {
    // get string for index
    auto p = setvals_.find(i);

    if (p == setvals_.end())
      return CQChartsColor();

    return (*p).second;
  }

  // min/max value
  CQChartsColor min() const { assert(! valset_.empty()); return valset_. begin()->first; }
  CQChartsColor max() const { assert(! valset_.empty()); return valset_.rbegin()->first; }

  // min/max index
  int imin() const { assert(! setvals_.empty()); return setvals_. begin()->first; }
  int imax() const { assert(! setvals_.empty()); return setvals_.rbegin()->first; }

  // number of unique values
  int numUnique() const { return valset_.size(); }

  void uniqueValues(Values &values) {
    for (const auto &vi : valset_)
      values.push_back(vi.first);
  }

  void uniqueCounts(Counts &counts) {
    for (const auto &vi : valset_)
      counts.push_back(vi.second.second);
  }

  // map value into real in range
  double map(const CQChartsColor &c, double mapMin=0.0, double mapMax=1.0) const {
    // get color set index
    int i = id(c);

    // map color set index using 1 -> number of unique values
    int n = numUnique();

    if (! n)
      return mapMin;

    return CMathUtil::map(i, imin(), imax(), mapMin, mapMax);
  }

 private:
  using KeyCount  = std::pair<int,int>;
  using ValueSet  = std::map<CQChartsColor,KeyCount,CQChartsUtil::ColorCmp>;
  using SetValues = std::map<int,CQChartsColor>;

  Values    values_;        // all color values
  ValueSet  valset_;        // unique indexed color values
  SetValues setvals_;       // index to color map
  int       numNull_ { 0 }; // number of null values
};

//------

// set of real, integer or string values which will be grouped by their unique values.
// Auto detects value type from input data
class CQChartsValueSet : public QObject {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn column   READ column      WRITE setColumn  )
  Q_PROPERTY(bool           mappped  READ isMapped    WRITE setMapped  )
  Q_PROPERTY(double         mapMin   READ mapMin      WRITE setMapMin  )
  Q_PROPERTY(double         mapMax   READ mapMax      WRITE setMapMax  )
  Q_PROPERTY(bool           allowNaN READ isAllowNaN  WRITE setAllowNaN)

 public:
  using Type      = CQBaseModel::Type;
  using OptInt    = boost::optional<int>;
  using OptReal   = boost::optional<double>;
  using OptString = boost::optional<QString>;

 public:
  CQChartsValueSet(CQChartsPlot *plot=nullptr);

  //---

  const CQChartsColumn &column() const { return column_; }
  void setColumn(const CQChartsColumn &c) { column_ = c; }

  // get/set mapping enabled
  bool isMapped() const { return column_.isMapped(); }
  void setMapped(bool b) { column_.setMapped(b); }

  // get/set map min value
  double mapMin() const { return column_.mapMin(); }
  void setMapMin(double r) { column_.setMapMin(r); }

  // get/set map max value
  double mapMax() const { return column_.mapMax(); }
  void setMapMax(double r) { column_.setMapMax(r); }

  //---

  void addProperties(const QString &path);

  //---

  int empty() const { return values_.empty(); }

  void addValue(const QVariant &value);

  int numValues() const { return values_.size(); }

  const QVariant &value(int i) const { return values_[i]; }

  void clear();

  //---

  // is initialized
  bool initialized() const { return initialized_; }

  // get type
  Type type() const { init(); return type_; }
  void setType(const Type &type) { type_ = type; }

  bool isReal   () const { return (type() == Type::REAL   ); }
  bool isInteger() const { return (type() == Type::INTEGER); }
  bool isString () const { return (type() == Type::STRING ); }
  bool isColor  () const { return (type() == Type::COLOR  ); }
  bool isTime   () const { return (type() == Type::TIME   ); }
  bool isNumeric() const { return (isReal() || isInteger()); }

  bool isValid() const;

  //---

  CQChartsIValues &ivals() { return ivals_; };
  const CQChartsIValues &ivals() const { return ivals_; };

  CQChartsRValues &rvals() { return rvals_; };
  const CQChartsRValues &rvals() const { return rvals_; };

  CQChartsSValues &svals() { return svals_; };
  const CQChartsSValues &svals() const { return svals_; };

  CQChartsCValues &cvals() { return cvals_; };
  const CQChartsCValues &cvals() const { return cvals_; };

  CQChartsRValues &tvals() { return tvals_; };
  const CQChartsRValues &tvals() const { return tvals_; };

  void clearVals();

  //---

  bool canMap() const;

  // check if has value for specified index
  bool hasInd(int i) const;

  // map nth value to real range (mapMin()->mapMax())
  double imap(int i) const;

  // map nth value to real range (min->max)
  double imap(int i, double min, double max) const;

  // get/set allow nam values
  bool isAllowNaN() const { return allowNaN_; }
  void setAllowNaN(bool b) { allowNaN_ = b; }

  // get string index
  int sind(const QString &s) const;
  // get index string
  QString inds(int ind) const;
  // get number of string indices
  int snum() const;

  // get string bucket
  int sbucket(const QString &s) const;
  // get bucket string
  QString buckets(int ind) const;

  // get minimum index value
  // (for integers this is the minimum value, for real/string this is zero)
  int imin() const;

  // get maximum index value
  // (for integers this is the maximum value, for real/string this is the one
  // less than the number of unique values)
  int imax() const;

  // get minimum real value
  // (for integers and reals this is the minimum unique value, otherwise this  is imin))
  double rmin() const;

  // get maximum real value
  // (for integers and reals this is the maximum unique value, otherwise this is imax))
  double rmax() const;

  // get sum real value
  // (for integers and reals this is the sum value, otherwise this is 0.0))
  double rsum() const;

  // get mean real value
  // (for integers and reals this is the mean value, otherwise this is imax/2))
  double rmean() const;

  int    rid(double r) const;
  double idr(int i) const;

  int iid(int i) const;
  int idi(int i) const;

 private:
  void init() const;
  void init();

 protected:
  using Values = std::vector<QVariant>;

  CQChartsPlot *plot_ { nullptr };

  CQChartsColumn column_; // associated model column

  Values values_;                // input values
  bool   initialized_ { false }; // are real, integer, string values initialized

  Type type_ { Type::NONE }; // calculated type

  CQChartsIValues ivals_; // integer values
  CQChartsRValues rvals_; // real values
  CQChartsSValues svals_; // string values
  CQChartsCValues cvals_; // color values
  CQChartsRValues tvals_; // time values

  bool allowNaN_ { false }; // allow NaN values
};

#endif
