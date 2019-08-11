#ifndef CQStatData_H
#define CQStatData_H

#include <vector>
#include <cmath>

/*!
 * \brief statistics for set of variant values of a single type
 */
struct CQStatData {
  bool   set          { false }; //!< is set
  double outlierRange { 1.5 };   //!< outlier range factor
  double median       { 0.0 };   //!< values median
  double lowerMedian  { 0.0 };   //!< values lower median
  double upperMedian  { 0.0 };   //!< values upper median
  double loutlier     { 0.0 };   //!< lower outlier value
  double uoutlier     { 0.0 };   //!< upper outlier value
  double min          { 0.0 };   //!< min (non-outlier) value
  double max          { 0.0 };   //!< max (non-outlier) value
  double notch        { 0.0 };   //!< center notch
  double lnotch       { 0.0 };   //!< lower notch
  double unotch       { 0.0 };   //!< upper notch
  double sum          { 0.0 };   //!< sum of values
  double mean         { 0.0 };   //!< mean of values
  double stddev       { 0.0 };   //!< standard deviation of values

  void reset() {
    set         = false;
    median      = 0.0;
    lowerMedian = 0.0;
    upperMedian = 0.0;
    loutlier    = 0.0;
    uoutlier    = 0.0;
    min         = 0.0;
    max         = 0.0;
    notch       = 0.0;
    lnotch      = 0.0;
    unotch      = 0.0;
    sum         = 0.0;
    mean        = 0.0;
    stddev      = 0.0;
  }

  template<class T>
  void calcStatValues(const std::vector<T> &values) {
    set = true;

    int nv = values.size();

    if (nv > 0) {
      // calc median
      int nv1, nv2;

      medianInd(0, nv - 1, nv1, nv2);

      median = (values[nv1] + values[nv2])/2.0;

      // calc lower median
      if (nv1 > 0) {
        int nl1, nl2;

        medianInd(0, nv1 - 1, nl1, nl2);

        lowerMedian = (values[nl1] + values[nl2])/2.0;
      }
      else
        lowerMedian = values[0];

      // calc upper median
      if (nv2 < nv - 1) {
        int nu1, nu2;

        medianInd(nv2 + 1, nv - 1, nu1, nu2);

        upperMedian = (values[nu1] + values[nu2])/2.0;
      }
      else
        upperMedian = values[nv - 1];

      //---

      // calc outlier range - outside range()*(upper - lower)
      double routlier = upperMedian - lowerMedian;

      loutlier = lowerMedian - outlierRange*routlier;
      uoutlier = upperMedian + outlierRange*routlier;

      //---

      // calc min, max, sum and mean
      min = lowerMedian;
      max = min;

      sum = 0.0;

      for (auto v : values) {
        min = std::min(double(v), min);
        max = std::max(double(v), max);

        sum += v;
      }

      mean = sum/nv;

      //---

      // calc standard deviation
      double sum2 = 0.0;

      for (auto v : values) {
        double dr = v - mean;

        sum2 += dr*dr;
      }

      // TODO: Brussel's correction divides by (n - 1)
      // TODO: Also sqrt(sum2/nc - mean*mean) ?

      stddev = sqrt(sum2/nv);

      //---

      // calc notch (confidence interval around the median +/- 1.57 x IQR/sqrt of n).
      notch = 1.57*(upperMedian - lowerMedian)/sqrt(nv);

      lnotch = median - notch;
      unotch = median + notch;
    }
    else {
      reset();
    }
  }

  bool isOutlier(double v) const {
    return (v < loutlier || v > uoutlier);
  }

 private:
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
};

#endif
