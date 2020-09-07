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
        auto *th = const_cast<CQChartsBoxWhiskerT *>(this);

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
        auto *th = const_cast<CQChartsBoxWhiskerT *>(this);

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

#include <CQChartsObj.h>

/*!
 * \brief Axis Box Whisker Object
 * \ingroup Charts
 */
class CQChartsAxisBoxWhisker : public CQChartsObj {
  Q_OBJECT

  Q_PROPERTY(Side            side      READ side      WRITE setSide     )
  Q_PROPERTY(Qt::Orientation direction READ direction WRITE setDirection)
  Q_PROPERTY(CQChartsLength  width     READ width     WRITE setWidth    )
  Q_PROPERTY(CQChartsLength  margin    READ margin    WRITE setMargin   )
  Q_PROPERTY(CQChartsAlpha   alpha     READ alpha     WRITE setAlpha    )
  Q_PROPERTY(DrawType        drawType  READ drawType  WRITE setDrawType )

  Q_ENUMS(Side)
  Q_ENUMS(DrawType)

 public:
  enum class Side {
    BOTTOM_LEFT,
    TOP_RIGHT
  };

  enum DrawType {
    WHISKER,
    WHISKER_BAR
  };

  using Density = CQChartsDensity;

 public:
  CQChartsAxisBoxWhisker(CQChartsPlot *plot, const Qt::Orientation &direction=Qt::Horizontal);

  //! get plot
  CQChartsPlot *plot() const { return plot_; }

  //! get/set side
  const Side &side() const { return side_; }
  void setSide(const Side &s);

  //! get/set direction
  const Qt::Orientation &direction() const { return direction_; }
  void setDirection(const Qt::Orientation &o);

  //! get/set width
  const CQChartsLength &width() const { return width_; }
  void setWidth(const CQChartsLength &l);

  //! get/set margin
  const CQChartsLength &margin() const { return margin_; }
  void setMargin(const CQChartsLength &l);

  //! get/set alpha
  const CQChartsAlpha &alpha() const { return alpha_; }
  void setAlpha(const CQChartsAlpha &a);

  //! get/set draw type
  const DrawType &drawType() const { return drawType_; }
  void setDrawType(const DrawType &t);

  //! get/set whisker data
  const CQChartsBoxWhisker &whisker() const { return whisker_; }
  void setWhisker(const CQChartsBoxWhisker &w) { whisker_.setValues(w.values()); }

  //! add value
  void addValue(double v) { whisker_.addValue(v); }

  //! get number of values
  int numValues() const { return whisker_.numValues(); }

  //! get density
  const Density &density() const { return whisker_.density(); }

  //! get min/max
  double min() const { return whisker_.min(); }
  double max() const { return whisker_.max(); }

  //! calc bbox or n points
  CQChartsGeom::BBox calcNBBox(int n) const;
  CQChartsGeom::BBox calcNDeltaBBox(int n, double delta) const;

  //! calc bbox
  CQChartsGeom::BBox calcBBox() const;
  CQChartsGeom::BBox calcDeltaBBox(double delta) const;

  //! point inside
  bool contains(const Point &p) const override;

  //! handle data invalidation
  void dataInvalidate() override;

  //! add properties
  void addProperties(const QString &path, const QString &desc);

  //! draw
  void draw(CQChartsPaintDevice *device, const CQChartsPenBrush &penBrush,
            int ind=1, double delta=0.0);

 private:
  CQChartsPlot*      plot_      { nullptr };           //!< plot
  Side               side_      { Side::BOTTOM_LEFT }; //!< rug side
  Qt::Orientation    direction_ { Qt::Horizontal };    //!< rug direction
  CQChartsLength     width_     { "24px" };            //!< width
  CQChartsLength     margin_    { "8px" };             //!< margin
  CQChartsAlpha      alpha_     { 0.5 };               //!< alpha
  DrawType           drawType_  { DrawType::WHISKER }; //!< draw type
  CQChartsBoxWhisker whisker_;                         //!< whisker data
};

//---

/*!
 * \brief Axis Density Object
 * \ingroup Charts
 */
class CQChartsAxisDensity : public CQChartsObj {
  Q_OBJECT

  Q_PROPERTY(Side            side      READ side      WRITE setSide     )
  Q_PROPERTY(Qt::Orientation direction READ direction WRITE setDirection)
  Q_PROPERTY(CQChartsLength  width     READ width     WRITE setWidth    )
  Q_PROPERTY(CQChartsAlpha   alpha     READ alpha     WRITE setAlpha    )
  Q_PROPERTY(DrawType        drawType  READ drawType  WRITE setDrawType )

  Q_ENUMS(Side)
  Q_ENUMS(DrawType)

 public:
  enum class Side {
    BOTTOM_LEFT,
    TOP_RIGHT
  };

  enum DrawType {
    DISTRIBUTION,
    BUCKETS
  };

  using Density = CQChartsDensity;

 public:
  CQChartsAxisDensity(CQChartsPlot *plot, const Qt::Orientation &direction=Qt::Horizontal);

  //! get plot
  CQChartsPlot *plot() const { return plot_; }

  //! get/set side
  const Side &side() const { return side_; }
  void setSide(const Side &s);

  //! get/set direction
  const Qt::Orientation &direction() const { return direction_; }
  void setDirection(const Qt::Orientation &o);

  //! get/set width
  const CQChartsLength &width() const { return width_; }
  void setWidth(const CQChartsLength &l);

  //! get/set alpha
  const CQChartsAlpha &alpha() const { return alpha_; }
  void setAlpha(const CQChartsAlpha &a);

  //! get/set draw type
  const DrawType &drawType() const { return drawType_; }
  void setDrawType(const DrawType &t);

  //! get/set whisker
  const CQChartsBoxWhisker &whisker() const { return whisker_; }
  void setWhisker(const CQChartsBoxWhisker &w) { whisker_.setValues(w.values()); }

  //! add distribution value
  void addValue(double v) { whisker_.addValue(v); }

  //! number of distribution value
  int numValues() const { return whisker_.numValues(); }

  //! density data
  const Density &density() const { return whisker_.density(); }

  //! min/max value
  double min() const { return whisker_.min(); }
  double max() const { return whisker_.max(); }

  //! calc bbox
  CQChartsGeom::BBox calcBBox() const;
  CQChartsGeom::BBox calcDeltaBBox(double delta) const;

  //! is point inside
  bool contains(const Point &p) const override;

  //! handle data invalidation
  void dataInvalidate() override;

  //! add properties
  void addProperties(const QString &path, const QString &desc);

  //! draw density
  void draw(CQChartsPaintDevice *device, const CQChartsPenBrush &penBrush, double delta=0.0);

 private:
  CQChartsPlot*      plot_      { nullptr };                //!< plot
  Side               side_      { Side::BOTTOM_LEFT };      //!< rug side
  Qt::Orientation    direction_ { Qt::Horizontal };         //!< rug direction
  CQChartsLength     width_     { "48px" };                 //!< width
  CQChartsAlpha      alpha_     { 0.5 };                    //!< alpha
  DrawType           drawType_  { DrawType::DISTRIBUTION }; //!< draw type
  CQChartsBoxWhisker whisker_;                              //!< whisker data
};

//------

#include <CQChartsGeom.h>
#include <CQChartsLength.h>

class CQChartsPlot;

/*!
 * \brief box whisker utility functions
 * \ingroup Charts
 */
namespace CQChartsBoxWhiskerUtil {

using Point   = CQChartsGeom::Point;
using BBox    = CQChartsGeom::BBox;
using Polygon = CQChartsGeom::Polygon;

void drawWhisker(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                 const CQChartsBoxWhisker &whisker, const BBox &bbox,
                 const CQChartsLength &width, const Qt::Orientation &orientation,
                 const CQChartsLength &cornerSize=CQChartsLength());
void drawWhisker(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                 const CQStatData &data, const std::vector<double> &outliers, const BBox &bbox,
                 const CQChartsLength &width, const Qt::Orientation &orientation,
                 const CQChartsLength &cornerSize=CQChartsLength());

void drawWhiskerBar(const CQChartsPlot *plot, CQChartsPaintDevice *device, const CQStatData &data,
                    double pos, const Qt::Orientation &orientation, double ww, double bw,
                    const CQChartsLength &cornerSize, bool notched, bool median,
                    const std::vector<double> &outliers);

}

#endif
