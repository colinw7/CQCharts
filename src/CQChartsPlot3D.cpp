#include <CQChartsPlot3D.h>
#include <CQChartsCamera.h>
#include <CQChartsPaintDevice.h>
#include <CQPropertyViewItem.h>

CQChartsPlot3DType::
CQChartsPlot3DType() :
 CQChartsGroupPlotType()
{
}

//---

CQChartsPlot3D::
CQChartsPlot3D(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model) :
 CQChartsGroupPlot(view, plotType, model)
{
  camera_ = new CQChartsCamera(this);
}

//---

void
CQChartsPlot3D::
setGridLines(bool b)
{
  CQChartsUtil::testAndSet(gridLines_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsPlot3D::
setCameraRotateX(double x)
{
  if (x != camera_->rotateX()) { camera_->setRotateX(x); drawObjs(); }
}

void
CQChartsPlot3D::
setCameraRotateY(double y)
{
  if (y != camera_->rotateY()) { camera_->setRotateY(y); drawObjs(); }
}

void
CQChartsPlot3D::
setCameraRotateZ(double z)
{
  if (z != camera_->rotateZ()) { camera_->setRotateZ(z); drawObjs(); }
}

void
CQChartsPlot3D::
setCameraScale(double s)
{
  if (s != camera_->scaleX() || s != camera_->scaleY() || s != camera_->scaleZ()) {
    camera_->setScaleX(s);
    camera_->setScaleY(s);
    camera_->setScaleZ(s);

    drawObjs();
  }
}

void
CQChartsPlot3D::
setCameraXMin(double x)
{
  if (x != camera_->xmin()) { camera_->setXMin(x); drawObjs(); }
}

void
CQChartsPlot3D::
setCameraXMax(double x)
{
  if (x != camera_->xmax()) { camera_->setXMax(x); drawObjs(); }
}

void
CQChartsPlot3D::
setCameraYMin(double y)
{
  if (y != camera_->ymin()) { camera_->setYMin(y); drawObjs(); }
}

void
CQChartsPlot3D::
setCameraYMax(double y)
{
  if (y != camera_->ymax()) { camera_->setYMax(y); drawObjs(); }
}

void
CQChartsPlot3D::
setCameraNear(double z)
{
  if (z != camera_->near()) { camera_->setNear(z); drawObjs(); }
}

void
CQChartsPlot3D::
setCameraFar(double z)
{
  if (z != camera_->far()) { camera_->setFar(z); drawObjs(); }
}

void
CQChartsPlot3D::
addCameraProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  addProp("axes", "gridLines", "gridLines", "Show Axis Grid Lines");

  addProp("camera", "cameraRotateX", "rotateX", "Rotate X");
  addProp("camera", "cameraRotateY", "rotateY", "Rotate Y");
  addProp("camera", "cameraRotateZ", "rotateZ", "Rotate Z");

  addProp("camera", "cameraScale", "scale", "Scale");

  addProp("camera", "cameraXMin", "xmin", "XMin");
  addProp("camera", "cameraXMax", "xmax", "XMax");
  addProp("camera", "cameraYMin", "ymin", "YMin");
  addProp("camera", "cameraYMax", "ymax", "YMax");
  addProp("camera", "cameraNear", "near", "Near");
  addProp("camera", "cameraFar" , "far" , "Far" );
}

void
CQChartsPlot3D::
drawBackgroundRects(CQChartsPaintDevice *) const
{
}

void
CQChartsPlot3D::
drawAxis(CQChartsPaintDevice *device) const
{
  if (! range3D_.isSet())
    return;

  //---

  auto *th = const_cast<CQChartsPlot3D *>(this);

  th->xAxis_.init(range3D_.xmin(), range3D_.xmax());
  th->yAxis_.init(range3D_.ymin(), range3D_.ymax());
  th->zAxis_.init(range3D_.zmin(), range3D_.zmax());

  //---

  CQChartsCamera *camera = this->camera();

  //---

  QColor bc1(210, 210, 210);
  QColor bc2(200, 200, 200);
  QColor bc3(190, 190, 190);

  using Colors = std::vector<QColor>;

  Colors colors = { bc1, bc2, bc3 };

  CQChartsPenBrush penBrush;

  CQChartsGeom::Point3D pxs0(range3D_.xmin(), range3D_.ymin(), range3D_.zmin());
  CQChartsGeom::Point3D pxs1(range3D_.xmin(), range3D_.ymax(), range3D_.zmin());
  CQChartsGeom::Point3D pxs2(range3D_.xmin(), range3D_.ymax(), range3D_.zmax());
  CQChartsGeom::Point3D pxs3(range3D_.xmin(), range3D_.ymin(), range3D_.zmax());

  CQChartsGeom::Point3D pxs4(range3D_.xmax(), range3D_.ymin(), range3D_.zmin());
  CQChartsGeom::Point3D pxs5(range3D_.xmax(), range3D_.ymax(), range3D_.zmin());
  CQChartsGeom::Point3D pxs6(range3D_.xmax(), range3D_.ymax(), range3D_.zmax());

  CQChartsGeom::Polygon poly1, poly2, poly3;

  using ZPoly = std::map<double, CQChartsGeom::Polygon>;

  ZPoly zpoly;

  auto addZPoly = [&](double z, const CQChartsGeom::Polygon &poly) {
    auto p = zpoly.find(-z);

    while (p != zpoly.end()) {
      z += 0.0001;

      p = zpoly.find(-z);
    }

    zpoly[-z] = poly;
  };

  poly1.addPoint(camera->transform(pxs0).point2D());
  poly1.addPoint(camera->transform(pxs1).point2D());
  poly1.addPoint(camera->transform(pxs2).point2D());
  poly1.addPoint(camera->transform(pxs3).point2D());

  CQChartsGeom::Point3D p1m(range3D_.xmin(), range3D_.ymid(), range3D_.zmid());

  addZPoly(camera->transform(p1m).z, poly1);

  poly2.addPoint(camera->transform(pxs0).point2D());
  poly2.addPoint(camera->transform(pxs4).point2D());
  poly2.addPoint(camera->transform(pxs5).point2D());
  poly2.addPoint(camera->transform(pxs1).point2D());

  CQChartsGeom::Point3D p2m(range3D_.xmid(), range3D_.ymid(), range3D_.zmin());

  addZPoly(camera->transform(p2m).z, poly2);

  poly3.addPoint(camera->transform(pxs1).point2D());
  poly3.addPoint(camera->transform(pxs5).point2D());
  poly3.addPoint(camera->transform(pxs6).point2D());
  poly3.addPoint(camera->transform(pxs2).point2D());

  CQChartsGeom::Point3D p3m(range3D_.xmid(), range3D_.ymax(), range3D_.zmid());

  addZPoly(camera->transform(p3m).z, poly3);

  int ic = 0;

  for (const auto &pp : zpoly) {
    setPenBrush(penBrush, CQChartsPenData(false), CQChartsBrushData(true, colors[ic]));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    device->drawPolygon(pp.second);

    ++ic;
  }

  //---

  // draw axis lines and ticks
  auto drawAxis = [&](const Axis &axis) {
    QColor pc(0, 0, 0);

    // draw axis lines
    setPenBrush(penBrush, CQChartsPenData(true, pc), CQChartsBrushData(false));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    if      (axis.dir == Axis::Dir::X) {
      CQChartsGeom::Point3D pa1(range3D_.xmin(), range3D_.ymin(), range3D_.zmin());
      CQChartsGeom::Point3D pa2(range3D_.xmax(), range3D_.ymin(), range3D_.zmin());

      device->drawLine(camera->transform(pa1).point2D(), camera->transform(pa2).point2D());
    }
    else if (axis.dir == Axis::Dir::Y) {
      CQChartsGeom::Point3D pa1(range3D_.xmax(), range3D_.ymin(), range3D_.zmin());
      CQChartsGeom::Point3D pa2(range3D_.xmax(), range3D_.ymax(), range3D_.zmin());

      device->drawLine(camera->transform(pa1).point2D(), camera->transform(pa2).point2D());
    }
    else if (axis.dir == Axis::Dir::Z) {
      CQChartsGeom::Point3D pa1(range3D_.xmax(), range3D_.ymax(), range3D_.zmin());
      CQChartsGeom::Point3D pa2(range3D_.xmax(), range3D_.ymax(), range3D_.zmax());

      device->drawLine(camera->transform(pxs5).point2D(), camera->transform(pa2).point2D());
    }

    //---

    CQChartsGeom::Point3D pam;

    if      (axis.dir == Axis::Dir::X)
      pam = CQChartsGeom::Point3D(range3D_.xmid(), range3D_.ymin(), range3D_.zmin());
    else if (axis.dir == Axis::Dir::Y)
      pam = CQChartsGeom::Point3D(range3D_.xmax(), range3D_.ymid(), range3D_.zmin());
    else if (axis.dir == Axis::Dir::Z)
      pam = CQChartsGeom::Point3D(range3D_.xmax(), range3D_.ymax(), range3D_.zmid());

    auto tpam = camera->transform(pam).point2D();

    CQChartsTextOptions textOptions;

    textOptions.align = 0;

    if (tpam.x < 0.0)
      textOptions.align |= Qt::AlignRight;
    else
      textOptions.align |= Qt::AlignLeft;

    if (tpam.y < 0.0)
      textOptions.align |= Qt::AlignTop;
    else
      textOptions.align |= Qt::AlignBottom;

    //---

    int n = axis.interval.calcNumMajor();

    for (int i = 0; i < n; ++i) {
      double min, max;

      axis.interval.intervalValues(i, min, max);

      if      (axis.dir == Axis::Dir::X) {
        if (min < range3D_.xmin() || min > range3D_.xmax()) continue;
      }
      else if (axis.dir == Axis::Dir::Y) {
        if (min < range3D_.ymin() || min > range3D_.ymax()) continue;
      }
      else if (axis.dir == Axis::Dir::Z) {
        if (min < range3D_.zmin() || min > range3D_.zmax()) continue;
      }

      //---

      double dt = 0.05;

      //---

      // draw grid line
     if (isGridLines()) {
        QColor gc(150, 150, 150);

        setPenBrush(penBrush, CQChartsPenData(true, gc), CQChartsBrushData(false));

        CQChartsDrawUtil::setPenBrush(device, penBrush);

        if      (axis.dir == Axis::Dir::X) {
          CQChartsGeom::Point3D pg1(min, range3D_.ymin(), range3D_.zmin());
          CQChartsGeom::Point3D pg2(min, range3D_.ymax(), range3D_.zmin());
          CQChartsGeom::Point3D pg3(min, range3D_.ymax(), range3D_.zmax());

          device->drawLine(camera->transform(pg1).point2D(), camera->transform(pg2).point2D());
          device->drawLine(camera->transform(pg2).point2D(), camera->transform(pg3).point2D());
        }
        else if (axis.dir == Axis::Dir::Y) {
          CQChartsGeom::Point3D pg1(range3D_.xmin(), min, range3D_.zmax());
          CQChartsGeom::Point3D pg2(range3D_.xmin(), min, range3D_.zmin());
          CQChartsGeom::Point3D pg3(range3D_.xmax(), min, range3D_.zmin());

          device->drawLine(camera->transform(pg1).point2D(), camera->transform(pg2).point2D());
          device->drawLine(camera->transform(pg2).point2D(), camera->transform(pg3).point2D());
        }
        else if (axis.dir == Axis::Dir::Z) {
          CQChartsGeom::Point3D pg1(range3D_.xmax(), range3D_.ymax(), min);
          CQChartsGeom::Point3D pg2(range3D_.xmin(), range3D_.ymax(), min);
          CQChartsGeom::Point3D pg3(range3D_.xmin(), range3D_.ymin(), min);

          device->drawLine(camera->transform(pg1).point2D(), camera->transform(pg2).point2D());
          device->drawLine(camera->transform(pg2).point2D(), camera->transform(pg3).point2D());
        }
      }

      //---

      // draw tick line
      CQChartsGeom::Point3D px1, px2;

      if      (axis.dir == Axis::Dir::X) {
        px1 = CQChartsGeom::Point3D(min, range3D_.ymin()     , range3D_.zmin());
        px2 = CQChartsGeom::Point3D(min, range3D_.ymin() - dt, range3D_.zmin());
      }
      else if (axis.dir == Axis::Dir::Y) {
        px1 = CQChartsGeom::Point3D(range3D_.xmax()     , min, range3D_.zmin());
        px2 = CQChartsGeom::Point3D(range3D_.xmax() + dt, min, range3D_.zmin());
      }
      else if (axis.dir == Axis::Dir::Z) {
        px1 = CQChartsGeom::Point3D(range3D_.xmax()     , range3D_.ymax(), min);
        px2 = CQChartsGeom::Point3D(range3D_.xmax() + dt, range3D_.ymax(), min);
      }

      setPenBrush(penBrush, CQChartsPenData(true, pc), CQChartsBrushData(false));

      CQChartsDrawUtil::setPenBrush(device, penBrush);

      device->drawLine(camera->transform(px1).point2D(), camera->transform(px2).point2D());

      //---

      // draw tick text
      QString label = QString("%1").arg(min);

      auto tp = camera->transform(px2).point2D();

      CQChartsDrawUtil::drawTextAtPoint(device, tp, label, textOptions);
    }
  };

  drawAxis(xAxis_);
  drawAxis(yAxis_);
  drawAxis(zAxis_);
}
