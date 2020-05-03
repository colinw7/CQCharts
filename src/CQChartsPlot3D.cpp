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

//---

void
CQChartsPlot3D::
postUpdateRange()
{
  deletePointObjs();
}

void
CQChartsPlot3D::
deletePointObjs()
{
  for (auto &po : pointObjs_) {
    for (const auto &obj : po.second)
      delete obj;
  }

  pointObjs_.clear();
}

//---

void
CQChartsPlot3D::
drawBackgroundRects(CQChartsPaintDevice *) const
{
}

void
CQChartsPlot3D::
addAxis(const CQChartsColumn &xColumn, const CQChartsColumn &yColumn,
        const CQChartsColumn &zColumn) const
{
  if (! range3D_.isSet())
    return;

  //---

  auto *th = const_cast<CQChartsPlot3D *>(this);

  th->xAxis_.init(range3D_.xmin(), range3D_.xmax());
  th->yAxis_.init(range3D_.ymin(), range3D_.ymax());
  th->zAxis_.init(range3D_.zmin(), range3D_.zmax());

  //---

  // add axis lines and ticks
  auto addAxis = [&](const Axis &axis) {
    double dt = 0.01;
    double dx = dt*(range3D_.xmax() - range3D_.xmin());
    double dy = dt*(range3D_.ymax() - range3D_.ymin());
    double dz = dt*(range3D_.zmax() - range3D_.zmin());

    QColor pc(0, 0, 0);

    // add axis lines
    CQChartsGeom::Point3D pa1, pa2;

    if      (axis.dir == Axis::Dir::X) {
      pa1 = CQChartsGeom::Point3D(range3D_.xmin(), range3D_.ymin(), range3D_.zmin());
      pa2 = CQChartsGeom::Point3D(range3D_.xmax(), range3D_.ymin(), range3D_.zmin());
    }
    else if (axis.dir == Axis::Dir::Y) {
      pa1 = CQChartsGeom::Point3D(range3D_.xmax(), range3D_.ymin(), range3D_.zmin());
      pa2 = CQChartsGeom::Point3D(range3D_.xmax(), range3D_.ymax(), range3D_.zmin());
    }
    else if (axis.dir == Axis::Dir::Z) {
      pa1 = CQChartsGeom::Point3D(range3D_.xmax(), range3D_.ymax(), range3D_.zmin());
      pa2 = CQChartsGeom::Point3D(range3D_.xmax(), range3D_.ymax(), range3D_.zmax());
    }

    auto *lineObj = new CQChartsLine3DObj(this, pa1, pa2, pc);

    th->addPointObj((pa1 + pa2)/2.0, lineObj);

    //---

    int nx = xAxis_.interval.calcNumMajor();
    int ny = yAxis_.interval.calcNumMajor();
    int nz = zAxis_.interval.calcNumMajor();

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

      // draw grid line
     if (isGridLines()) {
        QColor gc(150, 150, 150);

        CQChartsGeom::Point3D pg1, pg2, pg3;

        if      (axis.dir == Axis::Dir::X) {
          for (int iy = 0; iy < ny; ++iy) {
            double y1 = CMathUtil::map(iy    , 0, ny, range3D_.ymin(), range3D_.ymax());
            double y2 = CMathUtil::map(iy + 1, 0, ny, range3D_.ymin(), range3D_.ymax());

            CQChartsGeom::Point3D pg1(min, y1, range3D_.zmin());
            CQChartsGeom::Point3D pg2(min, y2, range3D_.zmin());

            auto *lineObj = new CQChartsLine3DObj(this, pg1, pg2, gc);

            th->addPointObj((pg1 + pg2)/2.0, lineObj);
          }

          for (int iz = 0; iz < nz; ++iz) {
            double z1 = CMathUtil::map(iz    , 0, nz, range3D_.zmin(), range3D_.zmax());
            double z2 = CMathUtil::map(iz + 1, 0, nz, range3D_.zmin(), range3D_.zmax());

            CQChartsGeom::Point3D pg1(min, range3D_.ymax(), z1);
            CQChartsGeom::Point3D pg2(min, range3D_.ymax(), z2);

            auto *lineObj = new CQChartsLine3DObj(this, pg1, pg2, gc);

            th->addPointObj((pg1 + pg2)/2.0, lineObj);
          }
        }
        else if (axis.dir == Axis::Dir::Y) {
          for (int iz = 0; iz < nz; ++iz) {
            double z1 = CMathUtil::map(iz    , 0, nz, range3D_.zmin(), range3D_.zmax());
            double z2 = CMathUtil::map(iz + 1, 0, nz, range3D_.zmin(), range3D_.zmax());

            CQChartsGeom::Point3D pg1(range3D_.xmin(), min, z1);
            CQChartsGeom::Point3D pg2(range3D_.xmin(), min, z2);

            auto *lineObj = new CQChartsLine3DObj(this, pg1, pg2, gc);

            th->addPointObj((pg1 + pg2)/2.0, lineObj);
          }

          for (int ix = 0; ix < nx; ++ix) {
            double x1 = CMathUtil::map(ix    , 0, nx, range3D_.xmin(), range3D_.xmax());
            double x2 = CMathUtil::map(ix + 1, 0, nx, range3D_.xmin(), range3D_.xmax());

            CQChartsGeom::Point3D pg1(x1, min, range3D_.zmin());
            CQChartsGeom::Point3D pg2(x2, min, range3D_.zmin());

            auto *lineObj = new CQChartsLine3DObj(this, pg1, pg2, gc);

            th->addPointObj((pg1 + pg2)/2.0, lineObj);
          }
        }
        else if (axis.dir == Axis::Dir::Z) {
          for (int ix = 0; ix < nx; ++ix) {
            double x1 = CMathUtil::map(ix    , 0, nx, range3D_.xmin(), range3D_.xmax());
            double x2 = CMathUtil::map(ix + 1, 0, nx, range3D_.xmin(), range3D_.xmax());

            CQChartsGeom::Point3D pg1(x1, range3D_.ymax(), min);
            CQChartsGeom::Point3D pg2(x2, range3D_.ymax(), min);

            auto *lineObj = new CQChartsLine3DObj(this, pg1, pg2, gc);

            th->addPointObj((pg1 + pg2)/2.0, lineObj);
          }

          for (int iy = 0; iy < ny; ++iy) {
            double y1 = CMathUtil::map(iy    , 0, ny, range3D_.ymin(), range3D_.ymax());
            double y2 = CMathUtil::map(iy + 1, 0, ny, range3D_.ymin(), range3D_.ymax());

            CQChartsGeom::Point3D pg1(range3D_.xmin(), y1, min);
            CQChartsGeom::Point3D pg2(range3D_.xmin(), y2, min);

            auto *lineObj = new CQChartsLine3DObj(this, pg1, pg2, gc);

            th->addPointObj((pg1 + pg2)/2.0, lineObj);
          }
        }
      }

      //---

      // add tick line
      CQChartsGeom::Point3D pt1, pt2, pt3;

      if      (axis.dir == Axis::Dir::X) {
        pt1 = CQChartsGeom::Point3D(min, range3D_.ymin()       , range3D_.zmin());
        pt2 = CQChartsGeom::Point3D(min, range3D_.ymin() -   dy, range3D_.zmin());
        pt3 = CQChartsGeom::Point3D(min, range3D_.ymin() - 2*dy, range3D_.zmin());
      }
      else if (axis.dir == Axis::Dir::Y) {
        pt1 = CQChartsGeom::Point3D(range3D_.xmax()       , min, range3D_.zmin());
        pt2 = CQChartsGeom::Point3D(range3D_.xmax() +   dx, min, range3D_.zmin());
        pt3 = CQChartsGeom::Point3D(range3D_.xmax() + 2*dx, min, range3D_.zmin());
      }
      else if (axis.dir == Axis::Dir::Z) {
        pt1 = CQChartsGeom::Point3D(range3D_.xmax()       , range3D_.ymax(), min);
        pt2 = CQChartsGeom::Point3D(range3D_.xmax() +   dx, range3D_.ymax(), min);
        pt3 = CQChartsGeom::Point3D(range3D_.xmax() + 2*dx, range3D_.ymax(), min);
      }

      auto *tickLineObj = new CQChartsLine3DObj(this, pt1, pt2, pc);

      th->addPointObj((pt1 + pt2)/2.0, tickLineObj);

      //---

      // draw tick text
      QString label;

      if      (axis.dir == Axis::Dir::X && xColumn.isValid())
        label = columnStr(xColumn, min);
      else if (axis.dir == Axis::Dir::Y && yColumn.isValid())
        label = columnStr(yColumn, min);
      else if (axis.dir == Axis::Dir::Z && zColumn.isValid())
        label = columnStr(zColumn, min);
      else
        label = QString("%1").arg(min);

      auto *textObj = new CQChartsText3DObj(this, pt2, pt3, label);

      th->addPointObj(pt2, textObj);
    }

    //---

    // draw tick text
    CQChartsGeom::Point3D pl1, pl2;
    QString               label;

    if      (axis.dir == Axis::Dir::X) {
      pl1 = CQChartsGeom::Point3D(range3D_.xmid()     , range3D_.ymin() - 5*dy, range3D_.zmin());
      pl2 = CQChartsGeom::Point3D(range3D_.xmid() + dx, range3D_.ymin() - 5*dy, range3D_.zmin());

      bool ok;
      label = modelHHeaderString(xColumn, ok);
      if (! ok) label = "X";
    }
    else if (axis.dir == Axis::Dir::Y) {
      pl1 = CQChartsGeom::Point3D(range3D_.xmax() + 5*dx, range3D_.ymid()     , range3D_.zmin());
      pl2 = CQChartsGeom::Point3D(range3D_.xmax() + 5*dx, range3D_.ymid() + dy, range3D_.zmin());

      bool ok;
      label = modelHHeaderString(yColumn, ok);
      if (! ok) label = "Y";
    }
    else if (axis.dir == Axis::Dir::Z) {
      pl1 = CQChartsGeom::Point3D(range3D_.xmax() + 5*dx, range3D_.ymax(), range3D_.zmid()     );
      pl2 = CQChartsGeom::Point3D(range3D_.xmax() + 5*dx, range3D_.ymax(), range3D_.zmid() + dz);

      bool ok;
      label = modelHHeaderString(zColumn, ok);
      if (! ok) label = "Z";
    }

    auto *textObj = new CQChartsText3DObj(this, pl1, pl2, label);

    textObj->setVertical(true);

    th->addPointObj(pl1, textObj);
  };

  addAxis(xAxis_);
  addAxis(yAxis_);
  addAxis(zAxis_);
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
}

void
CQChartsPlot3D::
addPointObj(const CQChartsGeom::Point3D &p, CQChartsPlot3DObj *obj)
{
  obj->setRefPoint(p);

  pointObjs_[p].push_back(obj);
}

void
CQChartsPlot3D::
drawPointObjs(CQChartsPaintDevice *device) const
{
  CQChartsCamera *camera = this->camera();

  const auto &range3D = this->range3D();

  double z[8];

  z[0] = camera->transform(
    CQChartsGeom::Point3D(range3D.xmin(), range3D.ymin(), range3D.zmin())).z;
  z[1] = camera->transform(
    CQChartsGeom::Point3D(range3D.xmax(), range3D.ymin(), range3D.zmin())).z;
  z[2] = camera->transform(
    CQChartsGeom::Point3D(range3D.xmax(), range3D.ymax(), range3D.zmin())).z;
  z[3] = camera->transform(
    CQChartsGeom::Point3D(range3D.xmin(), range3D.ymax(), range3D.zmin())).z;
  z[4] = camera->transform(
    CQChartsGeom::Point3D(range3D.xmin(), range3D.ymin(), range3D.zmax())).z;
  z[5] = camera->transform(
    CQChartsGeom::Point3D(range3D.xmax(), range3D.ymin(), range3D.zmax())).z;
  z[6] = camera->transform(
    CQChartsGeom::Point3D(range3D.xmax(), range3D.ymax(), range3D.zmax())).z;
  z[7] = camera->transform(
    CQChartsGeom::Point3D(range3D.xmin(), range3D.ymax(), range3D.zmax())).z;

  auto *th = const_cast<CQChartsPlot3D *>(this);

  th->boxZMin_ = z[0];
  th->boxZMax_ = z[0];

  for (int i = 1; i < 7; ++i) {
    th->boxZMin_ = std::min(th->boxZMin_, z[i]);
    th->boxZMax_ = std::max(th->boxZMax_, z[i]);
  }

  //---

  using ZObjs = std::map<double,Objs>;

  ZObjs zObjs;

  for (auto &po : pointObjs_) {
    double z = camera->transform(po.first).z;

    for (const auto &obj : po.second)
      zObjs[-z].push_back(obj);
  }

  for (auto &po : zObjs) {
    for (const auto &obj : po.second)
      obj->postDraw(device);
  }
}

//---

bool
CQChartsPlot3D::
selectMousePress(const CQChartsGeom::Point &p, SelMod)
{
  if (! isReady()) return false;

  CQChartsCamera *camera = this->camera();

  auto w = pixelToWindow(p);

  auto w1 = camera->untransform(CQChartsGeom::Point3D(w.x, w.y, 0.0));

  std::cerr << w1.x << " " << w1.y << " " << w1.z << "\n";

  return false;
}

//---

CQChartsPlot3DObj::
CQChartsPlot3DObj(const CQChartsPlot3D *plot3D) :
 CQChartsPlotObj(const_cast<CQChartsPlot3D *>(plot3D), CQChartsGeom::BBox(0, 0, 1, 1),
                 ColorInd(), ColorInd(), ColorInd()),
 plot3D_(plot3D)
{
}

//---

CQChartsLine3DObj::
CQChartsLine3DObj(const CQChartsPlot3D *plot, const CQChartsGeom::Point3D &p1,
                  const CQChartsGeom::Point3D &p2, const QColor &color) :
 CQChartsPlot3DObj(plot), p1_(p1), p2_(p2), color_(color)
{
}

void
CQChartsLine3DObj::
postDraw(CQChartsPaintDevice *device)
{
  CQChartsCamera *camera = plot3D()->camera();

  CQChartsPenBrush penBrush;

  plot_->setPenBrush(penBrush, CQChartsPenData(true, color()), CQChartsBrushData(false));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawLine(camera->transform(p1()).point2D(), camera->transform(p2()).point2D());
}

//---

CQChartsText3DObj::
CQChartsText3DObj(const CQChartsPlot3D *plot, const CQChartsGeom::Point3D &p1,
                  const CQChartsGeom::Point3D &p2, const QString &text) :
 CQChartsPlot3DObj(plot), p1_(p1), p2_(p2), text_(text)
{
}

void
CQChartsText3DObj::
postDraw(CQChartsPaintDevice *device)
{
  CQChartsCamera *camera = plot3D()->camera();

  CQChartsPenBrush penBrush;

  QColor pc(0, 0, 0);

  plot_->setPenBrush(penBrush, CQChartsPenData(true, pc), CQChartsBrushData(false));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  auto pt1 = camera->transform(p1()).point2D();
  auto pt2 = camera->transform(p2()).point2D();

  CQChartsTextOptions textOptions;

  double angle = CMathUtil::Rad2Deg(CQChartsGeom::pointAngle(pt1, pt2));

  textOptions.align = 0;

  if (! isVertical()) {
    if      (angle < -90) {
      angle += 180;

      textOptions.align |= Qt::AlignRight;
    }
    else if (angle >  90) {
      angle -= 180;

      textOptions.align |= Qt::AlignRight;
    }
    else
      textOptions.align |= Qt::AlignLeft;

    textOptions.align |= Qt::AlignVCenter;

    angle = 0;
  }

  textOptions.angle = CQChartsAngle(angle);

  CQChartsDrawUtil::drawTextAtPoint(device, pt1, text(), textOptions);
}

//------

CQChartsPolygon3DObj::
CQChartsPolygon3DObj(const CQChartsPlot3D *plot, const CQChartsGeom::Polygon3D &poly) :
 CQChartsPlot3DObj(plot), poly_(poly)
{
}

void
CQChartsPolygon3DObj::
postDraw(CQChartsPaintDevice *device)
{
  CQChartsCamera *camera = plot3D()->camera();

  CQChartsPenBrush penBrush;

  QColor fc = plot_->charts()->interpPaletteColor(ig());
  QColor pc = plot_->charts()->interpInterfaceColor(0.0);
  QColor bg = plot_->charts()->interpInterfaceColor(1.0);

  double z = camera->transform(refPoint()).z;
  double s = CMathUtil::map(z, plot3D()->boxZMin(), plot3D()->boxZMax(), 1.0, 0.3);

  QColor fc1 = CQChartsUtil::blendColors(fc, bg, s);

  plot_->setPenBrush(penBrush, CQChartsPenData(true, pc, CQChartsAlpha(0.2)),
                     CQChartsBrushData(true, fc1));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  CQChartsGeom::Polygon poly;

  for (const auto &p : poly_.points())
    poly.addPoint(camera->transform(p).point2D());

  device->drawPolygon(poly);
}

//------

CQChartsPolyline3DObj::
CQChartsPolyline3DObj(const CQChartsPlot3D *plot, const CQChartsGeom::Polygon3D &poly) :
 CQChartsPlot3DObj(plot), poly_(poly)
{
}

void
CQChartsPolyline3DObj::
postDraw(CQChartsPaintDevice *device)
{
  CQChartsPenBrush penBrush;

  QColor lc = plot_->charts()->interpPaletteColor(ig());

  plot_->setPenBrush(penBrush, CQChartsPenData(true, lc), CQChartsBrushData(false));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  CQChartsCamera *camera = plot3D()->camera();

  CQChartsGeom::Polygon poly;

  for (const auto &p : poly_.points())
    poly.addPoint(camera->transform(p).point2D());

  device->drawPolygon(poly);
}
