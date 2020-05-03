#ifndef CQChartsPlot3D_H
#define CQChartsPlot3D_H

#include <CQChartsGroupPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsPlotType.h>
#include <CQChartsCamera.h>
#include <CQCharts.h>
#include <CInterval.h>

class CQChartsPlot3DObj;

class CQChartsPlot3DType : public CQChartsGroupPlotType {
 public:
  CQChartsPlot3DType();
};

class CQChartsPlot3D : public CQChartsGroupPlot {
  Q_OBJECT

  Q_PROPERTY(bool gridLines READ isGridLines WRITE setGridLines)

  Q_PROPERTY(double cameraRotateX READ cameraRotateX WRITE setCameraRotateX)
  Q_PROPERTY(double cameraRotateY READ cameraRotateY WRITE setCameraRotateY)
  Q_PROPERTY(double cameraRotateZ READ cameraRotateZ WRITE setCameraRotateZ)
  Q_PROPERTY(double cameraScale   READ cameraScale   WRITE setCameraScale  )
  Q_PROPERTY(double cameraXMin    READ cameraXMin    WRITE setCameraXMin   )
  Q_PROPERTY(double cameraXMax    READ cameraXMax    WRITE setCameraXMax   )
  Q_PROPERTY(double cameraYMin    READ cameraYMin    WRITE setCameraYMin   )
  Q_PROPERTY(double cameraYMax    READ cameraYMax    WRITE setCameraYMax   )
  Q_PROPERTY(double cameraNear    READ cameraNear    WRITE setCameraNear   )
  Q_PROPERTY(double cameraFar     READ cameraFar     WRITE setCameraFar    )

 public:
  using Range3D = CQChartsGeom::Range3D;
  using Camera  = CQChartsCamera;

 public:
  CQChartsPlot3D(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model);

  //---

  const Range3D &range3D() const { return range3D_; }

  Camera *camera() const { return camera_; }

  //---

  bool isGridLines() const { return gridLines_; }
  void setGridLines(bool b);

  //---

  double cameraRotateX() const { return camera_->rotateX(); }
  void setCameraRotateX(double x);

  double cameraRotateY() const { return camera_->rotateY(); }
  void setCameraRotateY(double y);

  double cameraRotateZ() const { return camera_->rotateZ(); }
  void setCameraRotateZ(double z);

  double cameraScale() const { return camera_->scaleX(); }
  void setCameraScale(double s);

  double cameraXMin() const { return camera_->xmin(); }
  void setCameraXMin(double x);

  double cameraXMax() const { return camera_->xmax(); }
  void setCameraXMax(double x);

  double cameraYMin() const { return camera_->ymin(); }
  void setCameraYMin(double y);

  double cameraYMax() const { return camera_->ymax(); }
  void setCameraYMax(double y);

  double cameraNear() const { return camera_->near(); }
  void setCameraNear(double z);

  double cameraFar() const { return camera_->far(); }
  void setCameraFar(double z);

  //---

  void addCameraProperties();

  //---

  void postUpdateRange() override;

  void deletePointObjs();

  //---

  void drawBackgroundRects(CQChartsPaintDevice *device) const override;

  void addAxis(const CQChartsColumn &xColumn, const CQChartsColumn &yColumn,
               const CQChartsColumn &zColumn) const;

  void drawAxis(CQChartsPaintDevice *device) const;

  //---

  bool objInsideBox(CQChartsPlotObj *, const CQChartsGeom::BBox &) const override { return true; }

  //---

  double boxZMin() const { return boxZMin_; }
  double boxZMax() const { return boxZMax_; }

  //---

  void addPointObj(const CQChartsGeom::Point3D &p, CQChartsPlot3DObj *obj);

  void drawPointObjs(CQChartsPaintDevice *device) const;

  //---

  bool selectMousePress(const CQChartsGeom::Point &p, SelMod selMod) override;

 protected:
  struct Axis {
    enum class Dir {
      NONE,
      X,
      Y,
      Z
    };

    Axis(const Dir &dir) : dir(dir) { }

    void init(double start, double end) {
      interval.setStart(start);
      interval.setEnd  (end  );
    }

    Dir       dir      { Dir::NONE }; //!< direction
    CInterval interval;               //!< interval data
  };

  Range3D range3D_;                    //! 3D range
  Camera* camera_    { nullptr };      //! camera
  bool    gridLines_ { true };         //! show axis grid lines
  Axis    xAxis_     { Axis::Dir::X }; //! x axis
  Axis    yAxis_     { Axis::Dir::Y }; //! y axis
  Axis    zAxis_     { Axis::Dir::Z }; //! z axis

  double boxZMin_ { 0.0 };
  double boxZMax_ { 0.0 };

  using Objs      = std::vector<CQChartsPlot3DObj *>;
  using PointObjs = std::map<CQChartsGeom::Point3D,Objs>;

  mutable PointObjs pointObjs_;
};

//---

class CQChartsPlot3DObj : public CQChartsPlotObj {
 public:
  CQChartsPlot3DObj(const CQChartsPlot3D *plot);

  const CQChartsPlot3D *plot3D() const { return plot3D_; }

  const CQChartsGeom::Point3D &refPoint() const { return refPoint_; }
  void setRefPoint(const CQChartsGeom::Point3D &p) { refPoint_ = p; }

 private:
  const CQChartsPlot3D* plot3D_ { nullptr }; //! parent plot
  CQChartsGeom::Point3D refPoint_;           //! reference point
};

//---

class CQChartsLine3DObj : public CQChartsPlot3DObj {
 public:
  CQChartsLine3DObj(const CQChartsPlot3D *plot, const CQChartsGeom::Point3D &p1,
                    const CQChartsGeom::Point3D &p2, const QColor &color);

  const CQChartsGeom::Point3D &p1() const { return p1_; }
  void setP1(const CQChartsGeom::Point3D &p1) { p1_ = p1; }

  const CQChartsGeom::Point3D &p2() const { return p2_; }
  void setP2(const CQChartsGeom::Point3D &p2) { p2_ = p2; }

  const QColor &color() const { return color_; }
  void setColor(const QColor &v) { color_ = v; }

  double z() const { return z_; }
  void setZ(double r) { z_ = r; }

  QString typeName() const override { return "line"; }

  void postDraw(CQChartsPaintDevice *device) override;

 private:
  CQChartsGeom::Point3D p1_;                //!< start point
  CQChartsGeom::Point3D p2_;                //!< end point
  QColor                color_ { 0, 0, 0 }; //!< color
  double                z_    { 0.0 };      //!< z
};

//---

class CQChartsText3DObj : public CQChartsPlot3DObj {
 public:
  CQChartsText3DObj(const CQChartsPlot3D *plot, const CQChartsGeom::Point3D &p1,
                    const CQChartsGeom::Point3D &p2, const QString &text);

  const CQChartsGeom::Point3D &p1() const { return p1_; }
  void setP1(const CQChartsGeom::Point3D &p) { p1_ = p; }

  const CQChartsGeom::Point3D &p2() const { return p2_; }
  void setP2(const CQChartsGeom::Point3D &p) { p2_ = p; }

  bool isVertical() const { return vertical_; }
  void setVertical(bool b) { vertical_ = b; }

  const QString &text() const { return text_; }
  void setText(const QString &s) { text_ = s; }

  double z() const { return z_; }
  void setZ(double r) { z_ = r; }

  QString typeName() const override { return "line"; }

  void postDraw(CQChartsPaintDevice *device) override;

 private:
  CQChartsGeom::Point3D p1_;                 //!< point
  CQChartsGeom::Point3D p2_;                 //!< offset point
  bool                  vertical_ { false }; //!< is vertical
  QString               text_;               //!< text
  double                z_        { 0.0 };   //!< z
};

//---

class CQChartsPolyline3DObj : public CQChartsPlot3DObj {
 public:
  CQChartsPolyline3DObj(const CQChartsPlot3D *plot,
                        const CQChartsGeom::Polygon3D &poly=CQChartsGeom::Polygon3D());

  const CQChartsGeom::Polygon3D &poly() const { return poly_; }

  QString typeName() const override { return "poly"; }

  void addPoint(const CQChartsGeom::Point3D &p) { poly_.addPoint(p); }

  void postDraw(CQChartsPaintDevice *device) override;

 private:
  CQChartsGeom::Polygon3D poly_; //!< polygon
};

//---

class CQChartsPolygon3DObj : public CQChartsPlot3DObj {
 public:
  CQChartsPolygon3DObj(const CQChartsPlot3D *plot,
                       const CQChartsGeom::Polygon3D &poly=CQChartsGeom::Polygon3D());

  const CQChartsGeom::Polygon3D &poly() const { return poly_; }

  QString typeName() const override { return "poly"; }

  void addPoint(const CQChartsGeom::Point3D &p) { poly_.addPoint(p); }

  void postDraw(CQChartsPaintDevice *device) override;

 private:
  CQChartsGeom::Polygon3D poly_; //!< polygon
};

#endif
