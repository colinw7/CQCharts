#ifndef CQChartsPlot3D_H
#define CQChartsPlot3D_H

#include <CQChartsGroupPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsCamera.h>
#include <CInterval.h>

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

  void drawBackgroundRects(CQChartsPaintDevice *device) const override;

  void drawAxis(CQChartsPaintDevice *device) const;

  //---

  bool objInsideBox(CQChartsPlotObj *, const CQChartsGeom::BBox &) const override { return true; }

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
};

#endif
