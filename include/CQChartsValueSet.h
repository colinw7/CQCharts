#ifndef CQChartsValueSet_H
#define CQChartsValueSet_H

#include <CQChartsUtil.h>
#include <CQStatData.h>
#include <CQChartsModelTypes.h>
#include <vector>
#include <set>
#include <map>
#include <future>
#include <boost/optional.hpp>

class CQChartsPlot;
class CQTrie;
class CQTriePatterns;

//------

/*!
 * \brief class to store set of real values and returned cached data
 * \ingroup Charts
 */
class CQChartsRValues {
 public:
  using OptReal     = boost::optional<double>;
  using Values      = std::vector<double>;
  using Counts      = std::vector<int>;
  using ValueCount  = std::pair<double, int>;
  using ValueCounts = std::vector<ValueCount>;
  using Indices     = std::vector<int>;

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

  int addValue(const OptReal &r);

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
  double min(double def=CMathUtil::getNaN()) const {
    return (valset_.empty() ? def : valset_. begin()->first);
  }
  double max(double def=CMathUtil::getNaN()) const {
    return (valset_.empty() ? def : valset_.rbegin()->first);
  }

  // min/max index
  int imin(int def=0) const { return (setvals_.empty() ? def : setvals_. begin()->first); }
  int imax(int def=0) const { return (setvals_.empty() ? def : setvals_.rbegin()->first); }

  // number of unique values
  int numUnique() const { return valset_.size(); }

  QVariant uniqueValue() const {
    if (valset_.size() != 1) return QVariant();

    return QVariant(valset_.begin()->first);
  }

  void uniqueValueCounts(ValueCounts &valueCounts) {
    for (const auto &vi : valset_)
      valueCounts.push_back(ValueCount(vi.first, vi.second.second));
  }

  void uniqueCounts(Counts &counts) {
    for (const auto &vi : valset_)
      counts.push_back(vi.second.second);
  }

  void uniqueValues(Values &values) {
    for (const auto &vi : valset_)
      values.push_back(vi.first);
  }

  // calculated stats
  const CQStatData &statData() const { initCalc(); return statData_; }

  double sum   () const { return statData().sum   ; }
  double mean  () const { return statData().mean  ; }
  double stddev() const { return statData().stddev; }

  double lowerMedian() const { return statData().lowerMedian; }
  double median     () const { return statData().median     ; }
  double upperMedian() const { return statData().upperMedian; }

  const Indices &outliers() const { initCalc(); return outliers_; }

  bool isOutlier(double i) const;

  double svalue(int i) const { return svalues_[i]; }

 private:
  void initCalc() const {
    if (! calcValid_.load()) {
      std::unique_lock<std::mutex> lock(calcMutex_);

      if (! calcValid_.load()) {
        auto *th = const_cast<CQChartsRValues *>(this);

        th->calc();

        calcValid_.store(true);
      }
    }
  }

  void calc();

 private:
  using OptValues = std::vector<OptReal>;
  using KeyCount  = std::pair<int, int>;
  using ValueSet  = std::map<double, KeyCount, CQChartsUtil::RealCmp>;
  using SetValues = std::map<int, double>;

  OptValues                 values_;               //!< all real values
  Values                    svalues_;              //!< sorted real values
  ValueSet                  valset_;               //!< unique indexed real values
  SetValues                 setvals_;              //!< index to real map
  int                       numNull_    { 0 };     //!< number of null values
  bool                      calculated_ { false }; //!< are stats calculated
  CQStatData                statData_;             //!< stat data
  Indices                   outliers_;             //!< outlier values
  mutable std::atomic<bool> calcValid_  { false }; //!< is calculated
  mutable std::mutex        calcMutex_;            //!< calc mutex
};

//---

/*!
 * \brief class to store set of integer values and returned cached data
 * \ingroup Charts
 */
class CQChartsIValues {
 public:
  using OptInt      = boost::optional<int>;
  using Values      = std::vector<int>;
  using Counts      = std::vector<int>;
  using ValueCount  = std::pair<int, int>;
  using ValueCounts = std::vector<ValueCount>;
  using Indices     = std::vector<int>;

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

  int addValue(const OptInt &i);

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
  int min(int def=0) const { return (valset_.empty() ? def : valset_. begin()->first); }
  int max(int def=0) const { return (valset_.empty() ? def : valset_.rbegin()->first); }

  // min/max index
  int imin(int def=0) const { return (setvals_.empty() ? def : setvals_. begin()->first); }
  int imax(int def=0) const { return (setvals_.empty() ? def : setvals_.rbegin()->first); }

  // number of unique values
  int numUnique() const { return valset_.size(); }

  QVariant uniqueValue() const {
    if (valset_.size() != 1) return QVariant();

    return QVariant(valset_.begin()->first);
  }

  void uniqueValueCounts(ValueCounts &valueCounts) {
    for (const auto &vi : valset_)
      valueCounts.push_back(ValueCount(vi.first, vi.second.second));
  }

  void uniqueValues(Values &values) {
    for (const auto &vi : valset_)
      values.push_back(vi.first);
  }

  void uniqueCounts(Counts &counts) {
    for (const auto &vi : valset_)
      counts.push_back(vi.second.second);
  }

  // calculated stats
  const CQStatData &statData() const { initCalc(); return statData_; }

  double sum   () const { return statData().sum   ; }
  double mean  () const { return statData().mean  ; }
  double stddev() const { return statData().stddev; }

  double lowerMedian() const { return statData().lowerMedian; }
  double median     () const { return statData().median     ; }
  double upperMedian() const { return statData().upperMedian; }

  const Indices &outliers() const { initCalc(); return outliers_; }

  bool isOutlier(int v) const;

  double svalue(int i) const { return svalues_[i]; }

 private:
  void initCalc() const {
    if (! calcValid_.load()) {
      std::unique_lock<std::mutex> lock(calcMutex_);

      if (! calcValid_.load()) {
        auto *th = const_cast<CQChartsIValues *>(this);

        th->calc();

        calcValid_.store(true);
      }
    }
  }

  void calc();

 private:
  using OptValues = std::vector<OptInt>;
  using KeyCount  = std::pair<int, int>;
  using ValueSet  = std::map<int, KeyCount>;
  using SetValues = std::map<int, int>;

  OptValues                 values_;               //!< all integer values
  Values                    svalues_;              //!< sorted integer values
  ValueSet                  valset_;               //!< unique indexed integer values
  SetValues                 setvals_;              //!< index to integer map
  int                       numNull_    { 0 };     //!< number of null values
  bool                      calculated_ { false }; //!< are stats calculated
  CQStatData                statData_;             //!< stat data
  Indices                   outliers_;             //!< outlier values
  mutable std::atomic<bool> calcValid_  { false }; //!< is calculated
  mutable std::mutex        calcMutex_;            //!< calc mutex
};

//---

/*!
 * \brief class to store set of string values and returned cached data
 * \ingroup Charts
 */
class CQChartsSValues {
 public:
  using OptString   = boost::optional<QString>;
  using Values      = std::vector<QString>;
  using Counts      = std::vector<int>;
  using ValueCount  = std::pair<QString, int>;
  using ValueCounts = std::vector<ValueCount>;

 public:
  CQChartsSValues();
 ~CQChartsSValues();

  void clear();

  bool isValid() const { return ! values_.empty(); }

  bool canMap() const { return ! valset_.empty(); }

  int size() const { return values_.size(); }

  // get nth value (non-unique)
  const OptString &value(int i) const { return values_[i]; }

  int addValue(const OptString &s);

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
  QString min(const QString &def="") const {
    return (valset_.empty() ? def : valset_. begin()->first);
  }
  QString max(const QString &def="") const {
    return (valset_.empty() ? def : valset_.rbegin()->first);
  }

  // min/max index
  int imin(int def=0) const { return (setvals_.empty() ? def : setvals_. begin()->first); }
  int imax(int def=0) const { return (setvals_.empty() ? def : setvals_.rbegin()->first); }

  // number of unique values
  int numUnique() const { return valset_.size(); }

  QVariant uniqueValue() const {
    if (valset_.size() != 1) return QVariant();

    return QVariant(valset_.begin()->first);
  }

  void uniqueValueCounts(ValueCounts &valueCounts) {
    for (const auto &vi : valset_)
      valueCounts.push_back(ValueCount(vi.first, vi.second.second));
  }

  void uniqueValues(Values &values) {
    for (const auto &sv : setvals_)
      values.push_back(sv.second);
  }

  void uniqueCounts(Counts &counts) {
    for (const auto &sv : setvals_) {
      auto p = valset_.find(sv.second);

      counts.push_back((*p).second.second);
    }
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

  int sbucket(const QString &s) const;

  QString buckets(int i) const;

 private:
  void initPatterns(int numIdeal) const;

 private:
  using OptValues = std::vector<OptString>;
  using KeyCount  = std::pair<int, int>;
  using ValueSet  = std::map<QString, KeyCount>;
  using SetValues = std::map<int, QString>;

  OptValues values_;        //!< all string values
  ValueSet  valset_;        //!< unique indexed string values
  SetValues setvals_;       //!< index to string map
  int       numNull_ { 0 }; //!< number of null values

  int                initBuckets_  { 10 };      //!< initial buckets
  CQTrie*            trie_         { nullptr }; //!< string trie
  CQTriePatterns*    spatterns_    { nullptr }; //!< trie patterns
  bool               spatternsSet_ { false };   //!< trie patterns set
  mutable std::mutex mutex_;                    //!< mutex
};

//---

/*!
 * \brief class to store set of color values and returned cached data
 * \ingroup Charts
 */
class CQChartsCValues {
 public:
  using Values      = std::vector<CQChartsColor>;
  using Counts      = std::vector<int>;
  using ValueCount  = std::pair<CQChartsColor, int>;
  using ValueCounts = std::vector<ValueCount>;

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

  int addValue(const CQChartsColor &c);

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
  CQChartsColor min(const CQChartsColor &def=CQChartsColor()) const {
    return (valset_.empty() ? def : valset_. begin()->first);
  }
  CQChartsColor max(const CQChartsColor &def=CQChartsColor()) const {
    return (valset_.empty() ? def : valset_.rbegin()->first);
  }

  // min/max index
  int imin(int def=0) const { return (setvals_.empty() ? def : setvals_. begin()->first); }
  int imax(int def=0) const { return (setvals_.empty() ? def : setvals_.rbegin()->first); }

  // number of unique values
  int numUnique() const { return valset_.size(); }

  QVariant uniqueValue() const {
    if (valset_.size() != 1) return QVariant();

    return QVariant::fromValue<CQChartsColor>(valset_.begin()->first);
  }

  void uniqueValueCounts(ValueCounts &valueCounts) {
    for (const auto &vi : valset_)
      valueCounts.push_back(ValueCount(vi.first, vi.second.second));
  }

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
  using KeyCount  = std::pair<int, int>;
  using ValueSet  = std::map<CQChartsColor, KeyCount, CQChartsUtil::ColorCmp>;
  using SetValues = std::map<int, CQChartsColor>;

  Values    values_;        //!< all color values
  ValueSet  valset_;        //!< unique indexed color values
  SetValues setvals_;       //!< index to color map
  int       numNull_ { 0 }; //!< number of null values
};

//------

/*!
 * \brief set of real, integer or string values which will be grouped by their unique values.
 * \ingroup Charts
 *
 * Auto detects value type from input data
 */
class CQChartsValueSet : public QObject {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn column   READ column      WRITE setColumn  )
  Q_PROPERTY(bool           mappped  READ isMapped    WRITE setMapped  )
  Q_PROPERTY(double         mapMin   READ mapMin      WRITE setMapMin  )
  Q_PROPERTY(double         mapMax   READ mapMax      WRITE setMapMax  )
  Q_PROPERTY(bool           allowNaN READ isAllowNaN  WRITE setAllowNaN)

 public:
  using Plot      = CQChartsPlot;
  using Type      = CQBaseModelType;
  using OptInt    = boost::optional<int>;
  using OptReal   = boost::optional<double>;
  using OptString = boost::optional<QString>;

 public:
  CQChartsValueSet(const Plot *plot=nullptr);

  //---

  const Plot *plot() const { return plot_; }
  void setPlot(const Plot *p) { plot_ = p; }

  //---

  const CQChartsColumn &column() const { return column_; }
  void setColumn(const CQChartsColumn &c) { column_ = c; }

  // get/set mapping enabled
  bool isMapped() const { return mapped_; }
  void setMapped(bool b) { mapped_ = b; }

  // get/set map min value
  double mapMin() const { return map_min_; }
  void setMapMin(double r) { map_min_ = r; }

  // get/set map max value
  double mapMax() const { return map_max_; }
  void setMapMax(double r) { map_max_ = r; }

  //---

  //void addProperties(const QString &path);

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

  // map value to set index
  int iset(const QVariant &value) const;

  // number of unique values
  int numUnique() const;

  // single unique value
  QVariant uniqueValue() const;

  // map value to index
  double imap(const QVariant &value) const;

  // map nth value to real range (mapMin()->mapMax())
  double imap(int i) const;

  // map nth value to real range (min->max)
  double imap(int i, double min, double max) const;

  // get/set allow NaN values
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
  int imin(int def=0) const;

  // get maximum index value
  // (for integers this is the maximum value, for real/string this is the one
  // less than the number of unique values)
  int imax(int def=0) const;

  // get minimum real value
  // (for integers and reals this is the minimum unique value, otherwise this is imin)
  double rmin(double def=CMathUtil::getNaN()) const;

  // get maximum real value
  // (for integers and reals this is the maximum unique value, otherwise this is imax)
  double rmax(double def=CMathUtil::getNaN()) const;

  // get sum real value
  // (for integers and reals this is the sum value, otherwise this is 0.0)
  double rsum() const;

  // get mean real value
  // (for integers and reals this is the mean value, otherwise this is imax/2)
  double rmean() const;

  // get real index
  int rid(double r) const;

  // get indexed real
  double idr(int i) const;

  // get integer index
  int iid(int i) const;
  // get indexed integer
  int idi(int i) const;

  void reals(std::vector<double> &reals) const;

 private:
  void init() const;
  void init();

  Type calcType() const;

 protected:
  using Values = std::vector<QVariant>;

  const Plot* plot_ { nullptr }; //!< plot

  CQChartsColumn column_; //!< associated model column

  bool   mapped_  { false }; //!< is mapped
  double map_min_ { 0.0 };   //!< map min
  double map_max_ { 1.0 };   //!< map max

  Values values_;                //!< input values
  bool   initialized_ { false }; //!< are real, integer, string values initialized

  Type type_ { Type::NONE }; //!< calculated type

  CQChartsIValues ivals_; //!< integer values
  CQChartsRValues rvals_; //!< real values
  CQChartsSValues svals_; //!< string values
  CQChartsCValues cvals_; //!< color values
  CQChartsRValues tvals_; //!< time values

  bool allowNaN_ { false }; //!< allow NaN values

  mutable std::mutex mutex_; //!< mutex
};

#endif
