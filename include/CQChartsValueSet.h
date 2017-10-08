#ifndef CQChartsValueSet_H
#define CQChartsValueSet_H

#include <CQChartsUtil.h>
#include <vector>
#include <set>
#include <map>

class CQChartsValueSet {
 public:
  enum class Type {
    NONE,
    INTEGER,
    REAL,
    STRING
  };

 public:
  CQChartsValueSet();

  void addValue(const QVariant &value);

  Type type() const { init(); return type_; }

  double imap(int i) const;

  int imin() const;
  int imax() const;

 private:
  void init() const;
  void init();

 private:
  struct RealCmp {
    bool operator()(const double &lhs, const double &rhs) const {
      if (CQChartsUtil::realEq(lhs, rhs))
        return false;

      return lhs < rhs;
    }
  };

  typedef std::vector<int>             IVals;
  typedef std::vector<double>          RVals;
  typedef std::vector<QString>         SVals;
  typedef std::set<int>                ISets;
  typedef std::map<double,int,RealCmp> RValSet;
  typedef std::map<int,double>         SetRVal;
  typedef std::map<QString,int>        SValSet;
  typedef std::map<int,QString>        SetSVal;
  typedef std::vector<QVariant>        Values;

  Values  values_;
  bool    initialized_ { false };

  Type    type_ { Type::NONE };
  IVals   ivals_;
  RVals   rvals_;
  SVals   svals_;
  ISets   iset_;
  RValSet rvalset_;
  SetRVal setrval_;
  SValSet svalset_;
  SetSVal setsval_;
};

#endif
