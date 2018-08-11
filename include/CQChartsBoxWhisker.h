#ifndef CQChartsBoxWhisker_H
#define CQChartsBoxWhisker_H

#include <CQChartsDensity.h>
#include <CQChartsUtil.h>
#include <QString>
#include <cassert>
#include <vector>
#include <algorithm>

struct CQChartsWhiskerData {
  double min    { 0.0 };
  double lower  { 0.0 };
  double median { 0.0 };
  double upper  { 0.0 };
  double max    { 0.0 };
  double notch  { 0.0 };
  double lnotch { 0.0 };
  double unotch { 0.0 };
};

template<typename VALUE>
class CQChartsBoxWhiskerT {
 public:
  using Values   = std::vector<VALUE>;
  using Outliers = std::vector<int>;
  using Density  = CQChartsDensity;

 public:
  CQChartsBoxWhiskerT() { }

  CQChartsBoxWhiskerT(const Values &values) :
   values_(values) {
    invalidate();
  }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

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
    // use required operator double() of VALUE to get real value
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

  //---

  double min   () const { return data().min   ; }
  double lower () const { return data().lower ; }
  double median() const { return data().median; }
  double max   () const { return data().max   ; }
  double upper () const { return data().upper ; }

  const CQChartsWhiskerData &data() const { const_calc(); return data_; }

  //---

  double sum() const { const_calc(); return sum_; }

  double mean() const { const_calc(); return mean_; }

  double stddev() const { const_calc(); return stddev_; }

  double notch() const { return data().notch; }

  double lnotch() const { return data().lnotch; }
  double unotch() const { return data().unotch; }

  const Outliers &outliers() const { const_calc(); return outliers_; }

  void init() { calc(); }

  const Density &density() const {
    if (! densityValid_) {
      CQChartsBoxWhiskerT *th = const_cast<CQChartsBoxWhiskerT *>(this);

      int nv = numValues();

      std::vector<double> vals;

      for (int iv = 0; iv < nv; ++iv) {
        double v = rvalue(iv);

        vals.push_back(v);
      }

      th->density_.setXVals(vals);

      th->density_.calc();

      th->densityValid_ = true;
    }

    return density_;
  }

  double normalize(double x) const {
    return CQChartsUtil::map(x, min(), max(), 0.0, 1.0);
  }

 private:
  void invalidate() {
    valid_        = false;
    densityValid_ = false;
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

      data_.median = (rvalue(nv1) + rvalue(nv2))/2.0;

      // lower median
      if (nv1 > 0) {
        int nl1, nl2;

        medianInd(0, nv1 - 1, nl1, nl2);

        data_.lower = (rvalue(nl1) + rvalue(nl2))/2.0;
      }
      else
        data_.lower = rvalue(0);

      // upper median
      if (nv2 < nv - 1) {
        int nu1, nu2;

        medianInd(nv2 + 1, nv - 1, nu1, nu2);

        data_.upper = (rvalue(nu1) + rvalue(nu2))/2.0;
      }
      else
        data_.upper = rvalue(nv - 1);

      // outliers outside range()*(upper - lower)
      double routlier = data_.upper - data_.lower;
      double loutlier = data_.lower - range()*routlier;
      double uoutlier = data_.upper + range()*routlier;

      //---

      sum_ = 0.0;

      data_.min = data_.lower;
      data_.max = data_.min;

      outliers_.clear();

      int n = 0;

      for (auto v : values_) {
        double vr = (double) v;

        if (vr < loutlier || vr > uoutlier)
          outliers_.push_back(n);
        else {
          data_.min = std::min(vr, data_.min);
          data_.max = std::max(vr, data_.max);
        }

        sum_ += vr;

        ++n;
      }

      mean_ = (n > 0 ? sum_/n : 0.0);

      double sum2 = 0.0;

      for (auto v : values_) {
        double vr = (double) v;

        double dr = (vr - mean_);

        sum2 += dr*dr;
      }

      stddev_ = (n > 1 ? sqrt(sum2)/(n - 1) : 0.0);
    }
    else {
      data_.median = 0.0;
      data_.min    = 0.0;
      data_.max    = 0.0;
      data_.lower  = 0.0;
      data_.upper  = 0.0;

      sum_    = 0.0;
      stddev_ = 0.0;
    }

    //---

    data_.notch = 1.58*(data_.upper - data_.lower)/sqrt(nv);

    data_.lnotch = data_.median - data_.notch;
    data_.unotch = data_.median + data_.notch;
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
  QString             name_;
  Values              values_;
  bool                valid_    { false };
  double              range_    { 1.5 };
  double              fraction_ { 0.95 }; // TODO
  CQChartsWhiskerData data_;
  double              sum_      { 0.0 };
  double              mean_     { 0.0 };
  double              stddev_   { 0.0 };
  Outliers            outliers_;
  Density             density_;
  bool                densityValid_ { false };
};

using CQChartsBoxWhisker = CQChartsBoxWhiskerT<double>;

//------

#include <CQChartsGeom.h>
#include <CQChartsLength.h>

class CQChartsPlot;
class QPainter;

namespace CQChartsBoxWhiskerUtil {

void drawWhisker(CQChartsPlot *plot, QPainter *painter, const CQChartsBoxWhisker &whisker,
                 const CQChartsGeom::BBox &bbox, const CQChartsLength &width,
                 const Qt::Orientation &orientation);

void drawWhisker(CQChartsPlot *plot, QPainter *painter, const CQChartsWhiskerData &whisker,
                 const CQChartsGeom::BBox &bbox, const CQChartsLength &width,
                 const Qt::Orientation &orientation);

void drawWhiskerBar(CQChartsPlot *plot, QPainter *painter, const CQChartsWhiskerData &data,
                    double pos, const Qt::Orientation &orientation,
                    double ww, double bw, const CQChartsLength &cornerSize, bool notched);

#if 0
void drawOutliers(CQChartsPlot *plot, QPainter *painter, const CQChartsBoxWhisker &whisker,
                  double pos, const CQChartsSymbolData &symbol,
                  const Qt::Orientation &orientation);
void drawOutliers(CQChartsPlot *plot, QPainter *painter, const std::vector<double> &ovalues,
                  double pos, const CQChartsSymbolData &symbol,
                  const Qt::Orientation &orientation);
#endif
void drawOutliers(CQChartsPlot *plot, QPainter *painter, const std::vector<double> &ovalues,
                  double pos, const CQChartsSymbolData &symbol, const QPen &pen,
                  const QBrush &brush, const Qt::Orientation &orientation);
}

#endif
