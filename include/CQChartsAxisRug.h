#ifndef CQChartsAxisRug_H
#define CQChartsAxisRug_H

#include <CQChartsObj.h>
#include <CQChartsObjData.h>

//! class for run on x/y axis
class CQChartsAxisRug : public CQChartsObj,
 public CQChartsObjPointData<CQChartsAxisRug> {
  Q_OBJECT

  Q_PROPERTY(Side side READ side WRITE setSide)

  CQCHARTS_POINT_DATA_PROPERTIES

  Q_ENUMS(Side)

 public:
  enum class Side {
    BOTTOM_LEFT,
    TOP_RIGHT
  };

  struct RugPoint {
    double pos { 0.0 };
    QColor c;

    RugPoint() = default;

    RugPoint(double pos, const QColor &c) :
     pos(pos), c(c) {
    }
  };

 public:
  CQChartsAxisRug(CQChartsPlot *plot, const Qt::Orientation &direction=Qt::Horizontal);

  CQChartsPlot *plot() const { return plot_; }

  const Side &side() const { return side_; }
  void setSide(const Side &s);

  const Qt::Orientation &direction() const { return direction_; }
  void setDirection(const Qt::Orientation &o);

  void clearPoints() { points_.clear(); }
  void addPoint(const RugPoint &p) { points_.push_back(p); }

  bool contains(const Point &p) const override;

  void addProperties(const QString &path, const QString &desc);

  CQChartsGeom::BBox calcBBox() const;

  void dataInvalidate() override;

  void draw(CQChartsPaintDevice *device);

 private:
  using Points = std::vector<RugPoint>;

  CQChartsPlot*   plot_      { nullptr };           //!< plot
  Side            side_      { Side::BOTTOM_LEFT }; //!< rug side
  Qt::Orientation direction_ { Qt::Horizontal };    //!< rug direction
  Points          points_;                          //!< rug points
};

#endif
