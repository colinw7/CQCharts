#ifndef CQChartsBoxWhisker_H
#define CQChartsBoxWhisker_H

#include <vector>
#include <algorithm>
#include <cassert>

template<typename VALUE>
class CQChartsBoxWhiskerT {
 public:
  using Values   = std::vector<VALUE>;
  using Outliers = std::vector<int>;

 public:
  CQChartsBoxWhiskerT() { }

  CQChartsBoxWhiskerT(const Values &values) :
   values_(values) {
    invalidate();
  }

  int numValues() const { return int(values_.size()); }

  const Values &values() const { return values_; }

  const VALUE &value(int i) const {
    assert(i >= 0 && i < int(values_.size()));

    return values_[i];
  }

  void addValue(const VALUE &value) {
    values_.push_back(value);

    invalidate();
  }

  void setValues(const Values &values) {
    values_ = values;

    invalidate();
  }

  void addValues(std::initializer_list<VALUE> values) {
    for (auto value : values)
      values_.push_back(value);

    invalidate();
  }

  double rvalue(int i) const {
    return double(value(i));
  }

  double range() const { return range_; }

  void setRange(double r) {
    range_ = r;

    invalidate();
  }

  double fraction() const { return fraction_; }

  void setFraction(double r) {
    fraction_ = r;

    invalidate();
  }

  double median() const { const_calc(); return median_; }

  double min() const { const_calc(); return min_; }
  double max() const { const_calc(); return max_; }

  double lower() const { const_calc(); return lower_; }
  double upper() const { const_calc(); return upper_; }

  const Outliers &outliers() const { const_calc(); return outliers_; }

  void init() { calc(); }

 private:
  void invalidate() {
    valid_ = false;
  }

  void const_calc() const {
    const_cast<CQChartsBoxWhiskerT *>(this)->calc();
  }

  void calc() {
    if (valid_)
      return;

    valid_ = true;

    if (values_.empty())
      return;

    std::sort(values_.begin(), values_.end());

    int nv = values_.size();

    if (nv > 0) {
      // median
      int nv1, nv2;

      medianInd(0, nv - 1, nv1, nv2);

      median_ = (rvalue(nv1) + rvalue(nv2))/2.0;

      // lower median
      if (nv1 > 0) {
        int nl1, nl2;

        medianInd(0, nv1 - 1, nl1, nl2);

        lower_ = (rvalue(nl1) + rvalue(nl2))/2.0;
      }
      else
        lower_ = rvalue(0);

      // upper median
      if (nv2 < nv - 1) {
        int nu1, nu2;

        medianInd(nv2 + 1, nv - 1, nu1, nu2);

        upper_ = (rvalue(nu1) + rvalue(nu2))/2.0;
      }
      else
        upper_ = rvalue(nv - 1);

      // outliers outside range()*(upper - lower)
      double routlier = upper_ - lower_;
      double loutlier = lower_ - range()*routlier;
      double uoutlier = upper_ + range()*routlier;

      //---

      min_ = lower_;
      max_ = min_;

      outliers_.clear();

      int i = 0;

      for (auto v : values_) {
        double vr = (double) v;

        if (vr < loutlier || vr > uoutlier)
          outliers_.push_back(i);
        else {
          min_ = std::min(vr, min_);
          max_ = std::max(vr, max_);
        }

        ++i;
      }
    }
    else {
      median_ = 0.0;
      min_    = 0.0;
      max_    = 0.0;
      lower_  = 0.0;
      upper_  = 0.0;
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
  Values   values_;
  bool     valid_    { false };
  double   range_    { 1.5 };
  double   fraction_ { 0.95 }; // TODO
  double   median_   { 0.0 };
  double   min_      { 0.0 };
  double   max_      { 0.0 };
  double   lower_    { 0.0 };
  double   upper_    { 0.0 };
  Outliers outliers_;
};

//---

struct CQChartsBoxWhiskerData {
  double  x      { 0.0 };
  double  min    { 0.0 };
  double  lower  { 0.0 };
  double  median { 0.0 };
  double  upper  { 0.0 };
  double  max    { 0.0 };
};

#endif
