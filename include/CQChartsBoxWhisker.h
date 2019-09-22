#ifndef CQChartsBoxWhisker_H
#define CQChartsBoxWhisker_H

#include <CQChartsDensity.h>
#include <CQChartsUtil.h>
#include <CQStatData.h>
#include <QString>
#include <cassert>
#include <vector>
#include <algorithm>
#include <future>

/*!
 * \brief box whisker
 * \ingroup Charts
 */
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
  void setRange(double r) { range_ = r; invalidate(); }

  double fraction() const { return fraction_; }
  void setFraction(double r) { fraction_ = r; invalidate(); }

  //---

  double min() const { return statData().min; }
  double max() const { return statData().max; }

  double median() const { return statData().median; }

  double lowerMedian() const { return statData().lowerMedian; }
  double upperMedian() const { return statData().upperMedian; }

  double vmin() const { return (! values_.empty() ? double(values_.front()) : 0.0); }
  double vmax() const { return (! values_.empty() ? double(values_.back ()) : 0.0); }

  const CQStatData &statData() const { initCalc(); return statData_; }

  //---

  double sum() const { initCalc(); return statData().sum; }

  double mean() const { initCalc(); return statData().mean; }

  double stddev() const { initCalc(); return statData().stddev; }

  double notch() const { return statData().notch; }

  double lnotch() const { return statData().lnotch; }
  double unotch() const { return statData().unotch; }

  const Outliers &outliers() const { initCalc(); return outliers_; }

  void init() { initCalc(); }

  const Density &density() const { initDensity(); return density_; }

  double normalize(double x, bool includeOutliers) const {
    if (includeOutliers)
      return CMathUtil::map(x, vmin(), vmax(), 0.0, 1.0);
    else
      return CMathUtil::map(x, min(), max(), 0.0, 1.0);
  }

 private:
  void invalidate() {
    calcValid_   .store(false);
    densityValid_.store(false);
  }

  void initCalc() const {
    if (! calcValid_.load()) {
      std::unique_lock<std::mutex> lock(calcMutex_);

      if (! calcValid_.load()) {
        CQChartsBoxWhiskerT *th = const_cast<CQChartsBoxWhiskerT *>(this);

        th->calc();

        calcValid_.store(true);
      }
    }
  }

  void calc() {
    if (values_.empty())
      return;

    std::sort(values_.begin(), values_.end());

    //---

    statData_.calcStatValues(values_);

    //---

    outliers_.clear();

    int n = 0;

    for (auto v : values_) {
      if (statData_.isOutlier(v))
        outliers_.push_back(n);

      ++n;
    }
  }

  void initDensity() const {
    if (! densityValid_.load()) {
      std::unique_lock<std::mutex> lock(densityMutex_);

      if (! densityValid_.load()) {
        CQChartsBoxWhiskerT *th = const_cast<CQChartsBoxWhiskerT *>(this);

        th->calcDensity();

        densityValid_.store(true);
      }
    }
  }

  void calcDensity() {
    int nv = numValues();

    std::vector<double> vals;

    for (int iv = 0; iv < nv; ++iv) {
      double v = rvalue(iv);

      vals.push_back(v);
    }

    density_.setXVals(vals);

    density_.calc();
  }

 private:
  QString                   name_;                  //!< name
  Values                    values_;                //!< values
  double                    range_        { 1.5 };  //!< outlier range scale
  double                    fraction_     { 0.95 }; //!< fraction ? TODO

  // calculated data
  mutable std::atomic<bool> calcValid_    { false }; //!< calc valid
  mutable std::mutex        calcMutex_;              //!< calc mutex
  CQStatData                statData_;               //!< calc stats data
  Outliers                  outliers_;               //!< calc outliers

  // calculated density
  Density                   density_;                //!< density data
  mutable std::atomic<bool> densityValid_ { false }; //!< density data valid
  mutable std::mutex        densityMutex_;           //!< density data mutex
};

using CQChartsBoxWhisker = CQChartsBoxWhiskerT<double>;

//------

#include <CQChartsGeom.h>
#include <CQChartsLength.h>

class CQChartsPlot;

/*!
 * \brief box whisker utility functions
 * \ingroup Charts
 */
namespace CQChartsBoxWhiskerUtil {

void drawWhisker(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                 const CQChartsBoxWhisker &whisker, const CQChartsGeom::BBox &bbox,
                 const CQChartsLength &width, const Qt::Orientation &orientation);
void drawWhisker(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                 const CQStatData &data, const CQChartsGeom::BBox &bbox,
                 const CQChartsLength &width, const Qt::Orientation &orientation);

void drawWhiskerBar(const CQChartsPlot *plot, CQChartsPaintDevice *device, const CQStatData &data,
                    double pos, const Qt::Orientation &orientation,
                    double ww, double bw, const CQChartsLength &cornerSize, bool notched);

void drawOutliers(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                  const std::vector<double> &ovalues,
                  double pos, const CQChartsSymbolData &symbol, const QPen &pen,
                  const QBrush &brush, const Qt::Orientation &orientation);
}

#endif
