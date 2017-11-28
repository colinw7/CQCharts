#ifndef CQChartsValueSet_H
#define CQChartsValueSet_H

#include <CQChartsUtil.h>
#include <vector>
#include <set>
#include <map>

class CQChartsPlot;

// set of real, integer or string values which will be grouped by their unique values.
// Auto detects value type from input data
class CQChartsValueSet : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool   mapEnabled READ isMapEnabled WRITE setMapEnabled)
  Q_PROPERTY(double mapMin     READ mapMin       WRITE setMapMin    )
  Q_PROPERTY(double mapMax     READ mapMax       WRITE setMapMax    )

 public:
  enum class Type {
    NONE,
    INTEGER,
    REAL,
    STRING
  };

 public:
  CQChartsValueSet();

  void addProperties(CQChartsPlot *plot, const QString &path);

  //---

  int empty() const { return values_.empty(); }

  void addValue(const QVariant &value);

  int numValues() const { return values_.size(); }

  const QVariant &value(int i) const { return values_[i]; }

  //---

  // get type
  Type type() const { init(); return type_; }

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

  // get minimum index value
  // for integers this is the minimum value, for real/string this is one
  int imin() const;

  // get maximum index value
  // for integers this is the maximum value, for real/string this is the number of unique values
  int imax() const;

 private:
  void init() const;
  void init();

 private:
  // compare reals with tolerance
  struct RealCmp {
    bool operator()(const double &lhs, const double &rhs) const {
      if (CQChartsUtil::realEq(lhs, rhs))
        return false;

      return lhs < rhs;
    }
  };

  using IVals   = std::vector<int>;
  using RVals   = std::vector<double>;
  using SVals   = std::vector<QString>;
  using ISets   = std::set<int>;
  using RValSet = std::map<double,int,RealCmp>;
  using SetRVal = std::map<int,double>;
  using SValSet = std::map<QString,int>;
  using SetSVal = std::map<int,QString>;
  using Values  = std::vector<QVariant>;

  Values  values_;                // input values
  bool    initialized_ { false }; // are real, integer, string values initialized

  Type    type_ { Type::NONE };   // calculated type

  IVals   ivals_; // integer values
  ISets   iset_;  // unique integer values

  RVals   rvals_;   // real values
  RValSet rvalset_; // unique indexed real values
  SetRVal setrval_; // index to real map

  SVals   svals_;   // string values
  SValSet svalset_; // unique indexed string values
  SetSVal setsval_; // index to string map

  bool    mapEnabled_ { true };
  double  mapMin_     { 0.0 };
  double  mapMax_     { 1.0 };
};

#endif
