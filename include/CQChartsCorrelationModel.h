#ifndef CQChartsCorrelationModel_H
#define CQChartsCorrelationModel_H

#include <CQChartsGeom.h>
#include <CQChartsFitData.h>
#include <CQChartsDensity.h>
#include <CQDataModel.h>

/*!
 * \brief Wrapper class for CQDataModel to remember correlation input data
 * \ingroup Charts
 */
class CQChartsCorrelationModel : public CQDataModel {
 public:
  using Point   = CQChartsGeom::Point;
  using Points  = std::vector<Point>;
  using RMinMax = CQChartsGeom::RMinMax;

 public:
  explicit CQChartsCorrelationModel(int numCols);

  const Points &points(int i, int j) const {
    auto pi = ijPoints_.find(i);
    assert(pi != ijPoints_.end());

    auto pj = (*pi).second.find(j);
    assert(pj != (*pi).second.end());

    return (*pj).second;
  }

  void setPoints(int i, int j, const Points &points) {
    ijPoints_[i][j] = points;

    ijBestFit_[i][j].calc(points);
  }

  const RMinMax &minMax(int i) {
    auto p = iMinMax_.find(i);
    assert(p != iMinMax_.end());

    return (*p).second;
  }

  void setMinMax(int i, const RMinMax &minMax) {
    iMinMax_[i] = minMax;
  }

  void devData(int i, int j, double &x, double &y) {
    auto pi = ijDevData_.find(i);
    assert(pi != ijDevData_.end());

    auto pj = (*pi).second.find(j);
    assert(pj != (*pi).second.end());

    x = (*pj).second.x;
    y = (*pj).second.y;
  }

  void setDevData(int i, int j, double x, double y) {
    ijDevData_[i][j] = DevData(x, y);
  }

  CQChartsFitData &bestFit(int i, int j) {
    auto pi = ijBestFit_.find(i);
    assert(pi != ijBestFit_.end());

    auto pj = (*pi).second.find(j);
    assert(pj != (*pi).second.end());

    return (*pj).second;
  }

  CQChartsDensity *density(int i) {
    auto p = iDensity_.find(i);
    assert(p != iDensity_.end());

    return (*p).second;
  }

  void setDensity(int i, CQChartsDensity *densiy) {
    iDensity_[i] = densiy;
  }

 private:
  struct DevData {
    double x { 0.0 };
    double y { 0.0 };

    DevData(double x1=0.0, double y1=0.0) :
     x(x1), y(y1) {
    }
  };

  using JPoints   = std::map<int, Points>;
  using IJPoints  = std::map<int, JPoints>;
  using IMinMax   = std::map<int, RMinMax>;
  using JDevData  = std::map<int, DevData>;
  using IJDevData = std::map<int, JDevData>;
  using JBestFit  = std::map<int, CQChartsFitData>;
  using IJBestFit = std::map<int, JBestFit>;
  using IDensity  = std::map<int, CQChartsDensity *>;

  IJPoints  ijPoints_;
  IMinMax   iMinMax_;
  IJDevData ijDevData_;
  IJBestFit ijBestFit_;
  IDensity  iDensity_;
};

#endif
