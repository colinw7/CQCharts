#ifndef CQChartsValueSet_H
#define CQChartsValueSet_H

#include <CQChartsUtil.h>
#include <CQChartsTrie.h>
#include <vector>
#include <set>
#include <map>

class CQChartsPlot;

//------

class CQChartsRValues {
 public:
  CQChartsRValues() { }

  void clear() {
    rvals_  .clear();
    rvalset_.clear();
    setrval_.clear();
  }

  int size() const { return rvals_.size(); }

  // get nth value (non-unique)
  double value(int i) const { return rvals_[i]; }

  int addValue(double r) {
    // add to all values
    rvals_.push_back(r);

    // add to unique values if new
    auto p = rvalset_.find(r);

    if (p == rvalset_.end()) {
      int id = rvalset_.size();

      p = rvalset_.insert(p, RValSet::value_type(r, id)); // id for value

      setrval_[id] = r; // value for id
    }

    return (*p).second;
  }

  // real to id
  int id(double r) const {
    auto p = rvalset_.find(r);

    if (p == rvalset_.end())
      return -1;

    return (*p).second;
  }

  // id to real
  double ivalue(int i) const {
    // get real for index
    auto p = setrval_.find(i);

    if (p == setrval_.end())
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

    return CQChartsUtil::map(r, rmin, rmax, mapMin, mapMax);
  }

  // min/max value
  double min() const { assert(! rvalset_.empty()); return rvalset_. begin()->first; }
  double max() const { assert(! rvalset_.empty()); return rvalset_.rbegin()->first; }

  double mean() const { assert(! rvalset_.empty()); return (min() + max())/2.0; }

  // min/max index
  int imin() const { assert(! setrval_.empty()); return setrval_. begin()->first; }
  int imax() const { assert(! setrval_.empty()); return setrval_.rbegin()->first; }

  // number of unique values
  int numUnique() const { return rvalset_.size(); }

 private:
  using RVals   = std::vector<double>;
  using RValSet = std::map<double,int,CQChartsUtil::RealCmp>;
  using SetRVal = std::map<int,double>;

  RVals   rvals_;   // all real values
  RValSet rvalset_; // unique indexed real values
  SetRVal setrval_; // index to real map
};

//---

class CQChartsIValues {
 public:
  CQChartsIValues() { }

  void clear() {
    ivals_  .clear();
    ivalset_.clear();
    setival_.clear();
  }

  int size() const { return ivals_.size(); }

  // get nth value (non-unique)
  int value(int i) const { return ivals_[i]; }

  int addValue(int i) {
    // add to all values
    ivals_.push_back(i);

    // add to unique values if new
    auto p = ivalset_.find(i);

    if (p == ivalset_.end()) {
      int id = ivalset_.size();

      p = ivalset_.insert(p, IValSet::value_type(i, id)); // id for value

      setival_[id] = i; // value for id
    }

    return (*p).second;
  }

  // integer to id
  int id(int i) const {
    auto p = ivalset_.find(i);

    if (p == ivalset_.end())
      return -1;

    return (*p).second;
  }

  // id to integer
  int ivalue(int i) const {
    // get integer for index
    auto p = setival_.find(i);

    if (p == setival_.end())
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

    return CQChartsUtil::map(i, imin, imax, mapMin, mapMax);
  }

  // min/max value
  int min() const { assert(! ivalset_.empty()); return ivalset_. begin()->first; }
  int max() const { assert(! ivalset_.empty()); return ivalset_.rbegin()->first; }

  double mean() const { assert(! ivalset_.empty()); return (min() + max())/2.0; }

  // min/max index
  int imin() const { assert(! setival_.empty()); return setival_. begin()->first; }
  int imax() const { assert(! setival_.empty()); return setival_.rbegin()->first; }

  // number of unique values
  int numUnique() const { return ivalset_.size(); }

 private:
  using IVals   = std::vector<int>;
  using IValSet = std::map<int,int>;
  using SetIVal = std::map<int,int>;

  IVals   ivals_;   // all integer values
  IValSet ivalset_; // unique indexed integer values
  SetIVal setival_; // index to integer map
};

//---

class CQChartsSValues {
 public:
  CQChartsSValues() { }

  void clear() {
    svals_  .clear();
    svalset_.clear();
    setsval_.clear();

    trie_.clear();

    spatterns_.clear();

    spatternsSet_ = false;
  }

  int size() const { return svals_.size(); }

  // get nth value (non-unique)
  const QString &value(int i) const { return svals_[i]; }

  int addValue(const QString &s) {
    // add to trie
    trie_.addWord(s);

    // add to all values
    svals_.push_back(s);

    // add to unique values if new
    auto p = svalset_.find(s);

    if (p == svalset_.end()) {
      int id = svalset_.size();

      p = svalset_.insert(p, SValSet::value_type(s, id));

      setsval_[id] = s;
    }

    return (*p).second;
  }

  // string to id
  int id(const QString &s) const {
    // get string set index
    auto p = svalset_.find(s);

    if (p == svalset_.end())
      return -1;

    return (*p).second;
  }

  // id to string
  QString ivalue(int i) const {
    // get string for index
    auto p = setsval_.find(i);

    if (p == setsval_.end())
      return "";

    return (*p).second;
  }

  // min/max value
  QString min() const { assert(! svalset_.empty()); return svalset_. begin()->first; }
  QString max() const { assert(! svalset_.empty()); return svalset_.rbegin()->first; }

  // min/max index
  int imin() const { assert(! setsval_.empty()); return setsval_. begin()->first; }
  int imax() const { assert(! setsval_.empty()); return setsval_.rbegin()->first; }

  // number of unique values
  int numUnique() const { return svalset_.size(); }

  // map value into real in range
  double map(const QString &s, double mapMin=0.0, double mapMax=1.0) const {
    // get string set index
    int i = id(s);

    // map string set index using 1 -> number of unique values
    int n = numUnique();

    if (! n)
      return mapMin;

    return CQChartsUtil::map(i, imin(), imax(), mapMin, mapMax);
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
  using SVals   = std::vector<QString>;
  using SValSet = std::map<QString,int>;
  using SetSVal = std::map<int,QString>;

  SVals   svals_;   // all string values
  SValSet svalset_; // unique indexed string values
  SetSVal setsval_; // index to string map

  int                    initBuckets_  { 10 };
  CQChartsTrie           trie_;                   // string trie
  CQChartsTrie::Patterns spatterns_;              // trie patterns
  bool                   spatternsSet_ { false }; // trie patterns set
};

//---

class CQChartsCValues {
 public:
  CQChartsCValues() { }

  void clear() {
    cvals_  .clear();
    cvalset_.clear();
    setcval_.clear();
  }

  int size() const { return cvals_.size(); }

  // get nth value (non-unique)
  const QColor &value(int i) const { return cvals_[i]; }

  int addValue(const QColor &c) {
    // add to all values
    cvals_.push_back(c);

    // add to unique values if new
    auto p = cvalset_.find(c);

    if (p == cvalset_.end()) {
      int id = cvalset_.size();

      p = cvalset_.insert(p, CValSet::value_type(c, id));

      setcval_[id] = c;
    }

    return (*p).second;
  }

  // color to id
  int id(const QColor &c) const {
    // get string set index
    auto p = cvalset_.find(c);

    if (p == cvalset_.end())
      return -1;

    return (*p).second;
  }

  // id to color
  QColor ivalue(int i) const {
    // get string for index
    auto p = setcval_.find(i);

    if (p == setcval_.end())
      return "";

    return (*p).second;
  }

  // min/max value
  QColor min() const { assert(! cvalset_.empty()); return cvalset_. begin()->first; }
  QColor max() const { assert(! cvalset_.empty()); return cvalset_.rbegin()->first; }

  // min/max index
  int imin() const { assert(! setcval_.empty()); return setcval_. begin()->first; }
  int imax() const { assert(! setcval_.empty()); return setcval_.rbegin()->first; }

  // number of unique values
  int numUnique() const { return cvalset_.size(); }

  // map value into real in range
  double map(const QColor &c, double mapMin=0.0, double mapMax=1.0) const {
    // get color set index
    int i = id(c);

    // map color set index using 1 -> number of unique values
    int n = numUnique();

    if (! n)
      return mapMin;

    return CQChartsUtil::map(i, imin(), imax(), mapMin, mapMax);
  }

 private:
  using CVals   = std::vector<QColor>;
  using CValSet = std::map<QColor,int,CQChartsUtil::ColorCmp>;
  using SetCVal = std::map<int,QColor>;

  CVals   cvals_;   // all color values
  CValSet cvalset_; // unique indexed color values
  SetCVal setcval_; // index to color map
};

//------

// set of real, integer or string values which will be grouped by their unique values.
// Auto detects value type from input data
class CQChartsValueSet : public QObject {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn column     READ column       WRITE setColumn    )
  Q_PROPERTY(bool           mapEnabled READ isMapEnabled WRITE setMapEnabled)
  Q_PROPERTY(double         mapMin     READ mapMin       WRITE setMapMin    )
  Q_PROPERTY(double         mapMax     READ mapMax       WRITE setMapMax    )
  Q_PROPERTY(bool           allowNaN   READ isAllowNaN   WRITE setAllowNaN  )

 public:
  using Type = CQBaseModel::Type;

 public:
  CQChartsValueSet(CQChartsPlot *plot);

  //---

  const CQChartsColumn &column() const { return column_; }
  void setColumn(const CQChartsColumn &c) { column_ = c; }

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

  bool isNumeric() const { return (type() == Type::REAL || type() == Type::INTEGER); }

  // check if has value for specified index
  bool hasInd(int i) const;

  // map nth value to real range (mapMin()->mapMax())
  double imap(int i) const;

  // map nth value to real range (min->max)
  double imap(int i, double min, double max) const;

  // get/set mapping enabled
  bool isMapEnabled() const { return mapEnabled_; }
  void setMapEnabled(bool b) { mapEnabled_ = b; }

  // get/set map min value
  double mapMin() const { return mapMin_; }
  void setMapMin(double r) { mapMin_ = r; }

  // get/set map max value
  double mapMax() const { return mapMax_; }
  void setMapMax(double r) { mapMax_ = r; }

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

  // get mean real value
  // (for integers and reals this is the mean value, otherwise this is imax/2))
  double rmean() const;

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

  bool   mapEnabled_ { true };
  double mapMin_     { 0.0 };
  double mapMax_     { 1.0 };

  bool allowNaN_ { false }; // allow NaN values
};

#endif
