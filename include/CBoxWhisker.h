#ifndef CBoxWhisker_H
#define CBoxWhisker_H

#include <vector>
#include <algorithm>
#include <cassert>

class CBoxWhisker {
 public:
  typedef std::vector<double> Values;
  typedef std::vector<int>    Outliers;

 public:
  CBoxWhisker() { }

  CBoxWhisker(const Values &values) :
   values_(values) {
    invalidate();
  }

  int numValues() const { return int(values_.size()); }

  double value(int i) const {
    assert(i >= 0 && i < int(values_.size()));

    return values_[i];
  }

  void addValue(double value) {
    values_.push_back(value);

    invalidate();
  }

  void setValues(const Values &values) {
    values_ = values;

    invalidate();
  }

  void addValues(std::initializer_list<double> values) {
    for (auto value : values)
      values_.push_back(value);

    invalidate();
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
    const_cast<CBoxWhisker *>(this)->calc();
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

      median_ = (value(nv1) + value(nv2))/2.0;

      // lower median
      if (nv1 > 0) {
        int nl1, nl2;

        medianInd(0, nv1 - 1, nl1, nl2);

        lower_ = (value(nl1) + value(nl2))/2.0;
      }
      else
        lower_ = value(0);

      // upper median
      if (nv2 < nv - 1) {
        int nu1, nu2;

        medianInd(nv2 + 1, nv - 1, nu1, nu2);

        upper_ = (value(nu1) + value(nu2))/2.0;
      }
      else
        upper_ = value(nv - 1);

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
        if (v < loutlier || v > uoutlier)
          outliers_.push_back(i);
        else {
          min_ = std::min(v, min_);
          max_ = std::max(v, max_);
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

#endif
