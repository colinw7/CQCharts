#ifndef CQChartsAxisRug_H
#define CQChartsAxisRug_H

#include <CQChartsObj.h>
#include <CQChartsObjData.h>

//! class for run on x/y axis
class CQChartsAxisRug : public CQChartsObj,
 public CQChartsObjPointData<CQChartsAxisRug> {
  Q_OBJECT

  Q_PROPERTY(CQChartsAxisSide side READ side WRITE setSide)

  CQCHARTS_POINT_DATA_PROPERTIES

 public:
  //! rug point data
  struct RugPoint {
    double pos { 0.0 };
    QColor c;

    RugPoint() = default;

    RugPoint(double pos, const QColor &c) :
     pos(pos), c(c) {
    }
  };

 public:
  using Plot        = CQChartsPlot;
  using PaintDevice = CQChartsPaintDevice;
  using Side        = CQChartsAxisSide;
  using BBox        = CQChartsGeom::BBox;

 public:
  CQChartsAxisRug(Plot *plot, const Qt::Orientation &direction=Qt::Horizontal);

  Plot *plot() const { return plot_; }

  const Side &side() const { return side_; }
  void setSide(const Side &s);

  const Qt::Orientation &direction() const { return direction_; }
  void setDirection(const Qt::Orientation &o);

  void clearPoints() { points_.clear(); }
  void addPoint(const RugPoint &p) { points_.push_back(p); }

  bool contains(const Point &p) const override;

  void addProperties(const QString &path, const QString &desc);

  BBox calcBBox() const;

  void dataInvalidate() override;

  void draw(PaintDevice *device, double delta=0.0);

 private:
  using Points = std::vector<RugPoint>;

  Plot*            plot_      { nullptr };                 //!< plot
  CQChartsAxisSide side_      { Side::Type::BOTTOM_LEFT }; //!< rug side
  Qt::Orientation  direction_ { Qt::Horizontal };          //!< rug direction
  Points           points_;                                //!< rug points
};

#endif
