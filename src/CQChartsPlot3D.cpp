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
}

CQChartsPlot3D::
~CQChartsPlot3D()
{
}

//---

void
CQChartsPlot3D::
init()
{
  CQChartsGroupPlot::init();

  //---

  camera_ = new CQChartsCamera(this);
}

void
CQChartsPlot3D::
term()
{
}

//---

void
CQChartsPlot3D::
setZMin(const CQChartsOptReal &r)
{
  CQChartsUtil::testAndSet(zmin_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPlot3D::
setZMax(const CQChartsOptReal &r)
{
  CQChartsUtil::testAndSet(zmax_, r, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsPlot3D::
setGridLines(bool b)
{
  CQChartsUtil::testAndSet(gridLines_, b, [&]() { updateRangeAndObjs(); } );
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
  addProp("range", "zmin", "zmin", "Explicit minimum z value");
  addProp("range", "zmax", "zmax", "Explicit minimum z value");

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
  for (auto &po : bgPointObjs_) {
    for (const auto &obj : po.second)
      delete obj;
  }

  for (auto &po : pointObjs_) {
    for (const auto &obj : po.second)
      delete obj;
  }

  for (auto &po : fgPointObjs_) {
    for (const auto &obj : po.second)
      delete obj;
  }

  bgPointObjs_.clear();
  pointObjs_  .clear();
  fgPointObjs_.clear();
}

//---

bool
CQChartsPlot3D::
objNearestPoint(const Point &p, CQChartsPlotObj* &nearestObj) const
{
  for (auto &po : pointObjs_) {
    for (const auto &obj : po.second) {
      if (! obj->drawBBox().inside(p))
        continue;

      nearestObj = obj;

      return true;
    }
  }

  return false;
}

void
CQChartsPlot3D::
plotObjsAtPoint(const Point &p, PlotObjs &objs) const
{
  for (auto &po : pointObjs_) {
    for (const auto &obj : po.second) {
      if (! obj->drawBBox().inside(p))
        continue;

      objs.push_back(obj);
    }
  }
}

//---

void
CQChartsPlot3D::
drawBackgroundRects(PaintDevice *) const
{
}

void
CQChartsPlot3D::
addAxis(const Column &xColumn, const Column &yColumn, const Column &zColumn) const
{
  if (! range3D_.isSet())
    return;

  //---

  auto *th = const_cast<CQChartsPlot3D *>(this);

  th->xAxis_.init(range3D_.xmin(), range3D_.xmax());
  th->yAxis_.init(range3D_.ymin(), range3D_.ymax());
  th->zAxis_.init(range3D_.zmin(), range3D_.zmax());

  //---

#if 0
  // add axis lines and ticks
  auto addAxis = [&](const Axis &axis) {
    double dt = 0.01;
    double dx = dt*(range3D_.xmax() - range3D_.xmin());
    double dy = dt*(range3D_.ymax() - range3D_.ymin());
//  double dz = dt*(range3D_.zmax() - range3D_.zmin());

    QColor pc(0, 0, 0);

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

      // add tick line
      Point3D pt1, pt2, pt3;

      if      (axis.dir == Axis::Dir::X) {
        pt1 = Point3D(min, range3D_.ymin()       , range3D_.zmin());
        pt2 = Point3D(min, range3D_.ymin() -   dy, range3D_.zmin());
        pt3 = Point3D(min, range3D_.ymin() - 2*dy, range3D_.zmin());
      }
      else if (axis.dir == Axis::Dir::Y) {
        pt1 = Point3D(range3D_.xmax()       , min, range3D_.zmin());
        pt2 = Point3D(range3D_.xmax() +   dx, min, range3D_.zmin());
        pt3 = Point3D(range3D_.xmax() + 2*dx, min, range3D_.zmin());
      }
      else if (axis.dir == Axis::Dir::Z) {
        pt1 = Point3D(range3D_.xmax()       , range3D_.ymax(), min);
        pt2 = Point3D(range3D_.xmax() +   dx, range3D_.ymax(), min);
        pt3 = Point3D(range3D_.xmax() + 2*dx, range3D_.ymax(), min);
      }

      auto *tickLineObj = createLineObj(pt1, pt2, pc);

      th->addBgPointObj((pt1 + pt2)/2.0, tickLineObj);
    }
  };
#endif

  //---

  Point3D pxs0(range3D_.xmin(), range3D_.ymin(), range3D_.zmin());
  Point3D pxs1(range3D_.xmin(), range3D_.ymax(), range3D_.zmin());
  Point3D pxs2(range3D_.xmin(), range3D_.ymax(), range3D_.zmax());
  Point3D pxs3(range3D_.xmin(), range3D_.ymin(), range3D_.zmax());

  Point3D pxs4(range3D_.xmax(), range3D_.ymin(), range3D_.zmin());
  Point3D pxs5(range3D_.xmax(), range3D_.ymax(), range3D_.zmin());
  Point3D pxs6(range3D_.xmax(), range3D_.ymax(), range3D_.zmax());
  Point3D pxs7(range3D_.xmax(), range3D_.ymin(), range3D_.zmax());

  //---

  auto addZPoly = [&](const Point3D &p1, const Point3D &p2, const Point3D &p3, const Point3D &p4,
                      const QColor &c, const Point3D &n) {
    Polygon3D poly;

    poly.addPoint(p1);
    poly.addPoint(p2);
    poly.addPoint(p3);
    poly.addPoint(p4);

    auto pm = (p1 + p2 + p3 + p4)/4.0;

    auto *polyObj = new CQChartsAxisPolygon3DObj(this, poly);

    polyObj->setColor(c);
    polyObj->setNormal(n);

    th->addBgPointObj(pm, polyObj);

    return polyObj;
  };

  auto addAxisLines =[&](CQChartsAxisPolygon3DObj *polyObj,
                         CQChartsAxisPolygon3DObj *polyObj1, CQChartsAxisPolygon3DObj *polyObj2,
                         const Axis &axis1, const Axis &axis2, double pos) {
    auto isLabelPoint = [&](const Point3D &p) {
      return (p.z == range3D_.zmin() || (p.z != range3D_.zmax() && p.x != range3D_.xmin()));
    };

    // add tick text
    auto addText = [&](const Point3D &p, double value, CQChartsAxisPolygon3DObj *poly) {
      //bool ok1 = (p.z == range3D_.zmin();
      //bool ok2 = (p.z != range3D_.zmax() && p.x != range3D_.xmin());

      if (! isLabelPoint(p))
        return;

      QString label;

      if      (axis1.dir == Axis::Dir::X && xColumn.isValid())
        label = columnStr(xColumn, value);
      else if (axis1.dir == Axis::Dir::Y && yColumn.isValid())
        label = columnStr(yColumn, value);
      else if (axis1.dir == Axis::Dir::Z && zColumn.isValid())
        label = columnStr(zColumn, value);

      polyObj->addTickText(p, label, poly);
    };

    //---

    // add grid lines
    int n = axis1.interval.calcNumMajor();

    for (int i = 0; i < n; ++i) {
      double min, max;

      axis1.interval.intervalValues(i, min, max);

      if      (axis1.dir == Axis::Dir::X) {
        if (min < range3D_.xmin() || min > range3D_.xmax()) continue;
      }
      else if (axis1.dir == Axis::Dir::Y) {
        if (min < range3D_.ymin() || min > range3D_.ymax()) continue;
      }
      else if (axis1.dir == Axis::Dir::Z) {
        if (min < range3D_.zmin() || min > range3D_.zmax()) continue;
      }

      //---

      if      (axis1.dir == Axis::Dir::X) {
        if      (axis2.dir == Axis::Dir::Y) {
          Point3D pg1(min, range3D_.ymin(), pos);
          Point3D pg2(min, range3D_.ymax(), pos);

          polyObj->addGridLine(pg1, pg2);

          if (pos != range3D_.zmax()) {
            addText(pg1, min, polyObj1);
            addText(pg2, min, polyObj2);
          }
        }
        else if (axis2.dir == Axis::Dir::Z) {
          Point3D pg1(min, pos, range3D_.zmin());
          Point3D pg2(min, pos, range3D_.zmax());

          polyObj->addGridLine(pg1, pg2);

          addText(pg1, min, polyObj1);
          addText(pg2, min, polyObj2);
        }
      }
      else if (axis1.dir == Axis::Dir::Y) {
        if      (axis2.dir == Axis::Dir::X) {
          Point3D pg1(range3D_.xmin(), min, pos);
          Point3D pg2(range3D_.xmax(), min, pos);

          polyObj->addGridLine(pg1, pg2);

          if (pos != range3D_.zmax()) {
            addText(pg1, min, polyObj1);
            addText(pg2, min, polyObj2);
          }
        }
        else if (axis2.dir == Axis::Dir::Z) {
          Point3D pg1(pos, min, range3D_.zmin());
          Point3D pg2(pos, min, range3D_.zmax());

          polyObj->addGridLine(pg1, pg2);

          addText(pg1, min, polyObj1);
          addText(pg2, min, polyObj2);
        }
      }
      else if (axis1.dir == Axis::Dir::Z) {
        if      (axis2.dir == Axis::Dir::X) {
          Point3D pg1(range3D_.xmin(), pos, min);
          Point3D pg2(range3D_.xmax(), pos, min);

          polyObj->addGridLine(pg1, pg2);

          addText(pg1, min, polyObj1);
          addText(pg2, min, polyObj2);
        }
        else if (axis2.dir == Axis::Dir::Y) {
          Point3D pg1(pos, range3D_.ymin(), min);
          Point3D pg2(pos, range3D_.ymax(), min);

          polyObj->addGridLine(pg1, pg2);

          addText(pg1, min, polyObj1);
          addText(pg2, min, polyObj2);
        }
      }
    }

    //---

    // draw axis text
    Point3D pl1, pl2;
    QString label;

    if      (axis1.dir == Axis::Dir::X) {
      if      (axis2.dir == Axis::Dir::Y) {
        pl1 = Point3D(range3D_.xmid(), range3D_.ymin(), pos);
        pl2 = Point3D(range3D_.xmid(), range3D_.ymax(), pos);
      }
      else if (axis2.dir == Axis::Dir::Z) {
        pl1 = Point3D(range3D_.xmid(), pos, range3D_.zmin());
        pl2 = Point3D(range3D_.xmid(), pos, range3D_.zmax());
      }

      bool ok;
      label = modelHHeaderString(xColumn, ok);
      if (! ok) label = "X";
    }
    else if (axis1.dir == Axis::Dir::Y) {
      if      (axis2.dir == Axis::Dir::X) {
        pl1 = Point3D(range3D_.xmin(), range3D_.ymid(), pos);
        pl2 = Point3D(range3D_.xmax(), range3D_.ymid(), pos);
      }
      else if (axis2.dir == Axis::Dir::Z) {
        pl1 = Point3D(pos, range3D_.ymid(), range3D_.zmin());
        pl2 = Point3D(pos, range3D_.ymid(), range3D_.zmax());
      }

      bool ok;
      label = modelHHeaderString(yColumn, ok);
      if (! ok) label = "Y";
    }
    else if (axis1.dir == Axis::Dir::Z) {
      if      (axis2.dir == Axis::Dir::X) {
        pl1 = Point3D(range3D_.xmin(), pos, range3D_.zmid());
        pl2 = Point3D(range3D_.xmax(), pos, range3D_.zmid());
      }
      else if (axis2.dir == Axis::Dir::Y) {
        pl1 = Point3D(pos, range3D_.ymin(), range3D_.zmid());
        pl2 = Point3D(pos, range3D_.ymax(), range3D_.zmid());
      }

      bool ok;
      label = modelHHeaderString(zColumn, ok);
      if (! ok) label = "Z";
    }

    if (isLabelPoint(pl1))
      polyObj->addLabelText(pl1, label, polyObj1);

    if (isLabelPoint(pl2))
      polyObj->addLabelText(pl2, label, polyObj2);
  };

  QColor bc1(210, 210, 210);
  QColor bc2(200, 200, 200);
  QColor bc3(190, 190, 190);

  auto *xPolyObj1 = addZPoly(pxs0, pxs1, pxs2, pxs3, bc1, Point3D( 1, 0, 0));
  auto *xPolyObj2 = addZPoly(pxs4, pxs5, pxs6, pxs7, bc1, Point3D(-1, 0, 0));
  auto *yPolyObj1 = addZPoly(pxs0, pxs4, pxs7, pxs3, bc2, Point3D(0,  1, 0));
  auto *yPolyObj2 = addZPoly(pxs1, pxs5, pxs6, pxs2, bc2, Point3D(0, -1, 0));
  auto *zPolyObj1 = addZPoly(pxs0, pxs4, pxs5, pxs1, bc3, Point3D(0, 0,  1));
  auto *zPolyObj2 = addZPoly(pxs3, pxs7, pxs6, pxs2, bc3, Point3D(0, 0, -1));

  zPolyObj1->addSidePolygon(xPolyObj1, pxs0, pxs1);
  zPolyObj1->addSidePolygon(yPolyObj1, pxs0, pxs4);
  zPolyObj1->addSidePolygon(xPolyObj2, pxs5, pxs4);
  zPolyObj1->addSidePolygon(yPolyObj2, pxs5, pxs1);

  // add grid lines
  if (isGridLines()) {
    addAxisLines(xPolyObj1, zPolyObj1, zPolyObj2, yAxis_, zAxis_, range3D_.xmin());
    addAxisLines(xPolyObj1, yPolyObj1, yPolyObj2, zAxis_, yAxis_, range3D_.xmin());

    addAxisLines(xPolyObj2, zPolyObj1, zPolyObj2, yAxis_, zAxis_, range3D_.xmax());
    addAxisLines(xPolyObj2, yPolyObj1, yPolyObj2, zAxis_, yAxis_, range3D_.xmax());

    addAxisLines(yPolyObj1, zPolyObj1, zPolyObj2, xAxis_, zAxis_, range3D_.ymin());
    addAxisLines(yPolyObj1, xPolyObj1, xPolyObj2, zAxis_, xAxis_, range3D_.ymin());

    addAxisLines(yPolyObj2, zPolyObj1, zPolyObj2, xAxis_, zAxis_, range3D_.ymax());
    addAxisLines(yPolyObj2, xPolyObj1, xPolyObj2, zAxis_, xAxis_, range3D_.ymax());

    addAxisLines(zPolyObj1, yPolyObj1, yPolyObj2, xAxis_, yAxis_, range3D_.zmin());
    addAxisLines(zPolyObj1, xPolyObj1, xPolyObj2, yAxis_, xAxis_, range3D_.zmin());

    addAxisLines(zPolyObj2, yPolyObj1, yPolyObj2, xAxis_, yAxis_, range3D_.zmax());
    addAxisLines(zPolyObj2, xPolyObj1, xPolyObj2, yAxis_, xAxis_, range3D_.zmax());
  }

  //---

#if 0
  addAxis(xAxis_);
  addAxis(yAxis_);
  addAxis(zAxis_);
#endif
}

void
CQChartsPlot3D::
addBgPointObj(const Point3D &p, CQChartsPlot3DObj *obj)
{
  obj->setRefPoint(p);

  bgPointObjs_[p].push_back(obj);
}

void
CQChartsPlot3D::
addPointObj(const Point3D &p, CQChartsPlot3DObj *obj)
{
  obj->setRefPoint(p);

  pointObjs_[p].push_back(obj);
}

void
CQChartsPlot3D::
addFgPointObj(const Point3D &p, CQChartsPlot3DObj *obj)
{
  obj->setRefPoint(p);

  fgPointObjs_[p].push_back(obj);
}

void
CQChartsPlot3D::
drawPointObjs(PaintDevice *device) const
{
  auto *camera = this->camera();

  const auto &range3D = this->range3D();

  if (range3D.isSet()) {
    double z[8];

    z[0] = camera->transform(Point3D(range3D.xmin(), range3D.ymin(), range3D.zmin())).z;
    z[1] = camera->transform(Point3D(range3D.xmax(), range3D.ymin(), range3D.zmin())).z;
    z[2] = camera->transform(Point3D(range3D.xmax(), range3D.ymax(), range3D.zmin())).z;
    z[3] = camera->transform(Point3D(range3D.xmin(), range3D.ymax(), range3D.zmin())).z;
    z[4] = camera->transform(Point3D(range3D.xmin(), range3D.ymin(), range3D.zmax())).z;
    z[5] = camera->transform(Point3D(range3D.xmax(), range3D.ymin(), range3D.zmax())).z;
    z[6] = camera->transform(Point3D(range3D.xmax(), range3D.ymax(), range3D.zmax())).z;
    z[7] = camera->transform(Point3D(range3D.xmin(), range3D.ymax(), range3D.zmax())).z;

    auto *th = const_cast<CQChartsPlot3D *>(this);

    th->boxZMin_ = z[0];
    th->boxZMax_ = z[0];

    for (int i = 1; i < 7; ++i) {
      th->boxZMin_ = std::min(th->boxZMin_, z[i]);
      th->boxZMax_ = std::max(th->boxZMax_, z[i]);
    }
  }

  //---

  using ZObjs = std::map<double, Objs>;

  //---

  ZObjs bgZObjs;

  for (auto &po : bgPointObjs_) {
    double z = camera->transform(po.first).z;

    for (const auto &bgObj : po.second)
      bgZObjs[-z].push_back(bgObj);
  }

  for (auto &po : bgZObjs) {
    for (const auto &bgObj : po.second)
      bgObj->postDraw(device);
  }

  //---

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

  //---

  ZObjs fgZObjs;

  for (auto &po : fgPointObjs_) {
    double z = camera->transform(po.first).z;

    for (const auto &fgObj : po.second)
      fgZObjs[-z].push_back(fgObj);
  }

  for (auto &po : fgZObjs) {
    for (const auto &fgObj : po.second)
      fgObj->postDraw(device);
  }
}

//---

bool
CQChartsPlot3D::
selectMousePress(const Point &p, SelMod)
{
  if (! isReady()) return false;

  auto w = pixelToWindow(p);

  CQChartsPlotObj *obj;

  if (objNearestPoint(w, obj)) {
    std::cerr << obj->id().toStdString() << "\n";
  }

  auto *camera = this->camera();

  auto w1 = camera->untransform(Point3D(w.x, w.y, 0.0));

  std::cerr << w1.x << " " << w1.y << " " << w1.z << "\n";

  return false;
}

//---

CQChartsLine3DObj *
CQChartsPlot3D::
createLineObj(const Point3D &p1, const Point3D &p2, const QColor &color) const
{
  return new CQChartsLine3DObj(this, p1, p2, color);
}

CQChartsText3DObj *
CQChartsPlot3D::
createTextObj(const Point3D &p1, const Point3D &p2, const QString &text) const
{
  return new CQChartsText3DObj(this, p1, p2, text);
}

CQChartsPolyline3DObj *
CQChartsPlot3D::
createPolylineObj(const Polygon3D &poly) const
{
  return new CQChartsPolyline3DObj(this, poly);
}

CQChartsPolygon3DObj *
CQChartsPlot3D::
createPolygonObj(const Polygon3D &poly) const
{
  return new CQChartsPolygon3DObj(this, poly);
}

//---

CQChartsPlot3DObj::
CQChartsPlot3DObj(const CQChartsPlot3D *plot3D) :
 CQChartsPlotObj(const_cast<CQChartsPlot3D *>(plot3D), BBox(0, 0, 1, 1),
                 ColorInd(), ColorInd(), ColorInd()),
 plot3D_(plot3D)
{
}

//---

CQChartsLine3DObj::
CQChartsLine3DObj(const CQChartsPlot3D *plot, const Point3D &p1, const Point3D &p2,
                  const QColor &color) :
 CQChartsPlot3DObj(plot), p1_(p1), p2_(p2), color_(color)
{
}

void
CQChartsLine3DObj::
postDraw(PaintDevice *device)
{
  auto *camera = plot3D()->camera();

  CQChartsPenBrush penBrush;

  plot_->setPenBrush(penBrush, PenData(true, color()), BrushData(false));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawLine(camera->transform(p1()).point2D(), camera->transform(p2()).point2D());
}

//---

CQChartsText3DObj::
CQChartsText3DObj(const CQChartsPlot3D *plot, const Point3D &p1, const Point3D &p2,
                  const QString &text) :
 CQChartsPlot3DObj(plot), p1_(p1), p2_(p2), text_(text)
{
}

void
CQChartsText3DObj::
postDraw(PaintDevice *device)
{
  auto *camera = plot3D()->camera();

  auto pt1 = camera->transform(p1()).point2D();
  auto pt2 = camera->transform(p2()).point2D();

  //---

  auto textOptions = this->textOptions();

  //---

  if (isAutoAlign()) {
    double angle = CMathUtil::Rad2Deg(pointAngle(pt1, pt2));

    textOptions.align = 0;

    if (! isVertical()) {
      if      (angle < -90.0) {
        angle += 180.0;

        textOptions.align |= Qt::AlignRight;
      }
      else if (angle > 90.0) {
        angle -= 180.0;

        textOptions.align |= Qt::AlignRight;
      }
      else
        textOptions.align |= Qt::AlignLeft;

      textOptions.align |= Qt::AlignVCenter;

      angle = 0.0;
    }

    textOptions.angle = Angle(angle);
  }

  //---

  CQChartsDrawUtil::setPenBrush(device, penBrush());

  auto font = plot3D()->font().calcFont();

  device->setFont(font);

  CQChartsDrawUtil::drawTextAtPoint(device, pt1, text(), textOptions);
}

//------

CQChartsPolygon3DObj::
CQChartsPolygon3DObj(const CQChartsPlot3D *plot, const Polygon3D &poly) :
 CQChartsPlot3DObj(plot), poly_(poly)
{
}

void
CQChartsPolygon3DObj::
postDraw(PaintDevice *device)
{
  if (! checkVisible())
    return;

  //---

  auto *camera = plot3D()->camera();

  //---

  CQChartsPenBrush penBrush;

  auto pc = plot_->charts()->interpInterfaceColor(0.0);

  if (color_.isValid()) {
    plot_->setPenBrush(penBrush, PenData(true, pc, Alpha(0.2)), BrushData(true, color()));
  }
  else {
    auto fc = plot_->charts()->interpPaletteColor(ig());
    auto bg = plot_->charts()->interpInterfaceColor(1.0);

    auto trp = camera->transform(refPoint());

    double z = trp.z;
    double s = CMathUtil::map(z, plot3D()->boxZMin(), plot3D()->boxZMax(), 1.0, 0.3);

    auto fc1 = CQChartsUtil::blendColors(fc, bg, s);

    plot_->setPenBrush(penBrush, PenData(true, pc, Alpha(0.2)), BrushData(true, fc1));
  }

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  Polygon poly;

  for (const auto &p : poly_.points())
    poly.addPoint(camera->transform(p).point2D());

  device->drawPolygon(poly);

  //---

#if 0
  plot_->setPenBrush(penBrush, PenData(true, Qt::red), BrushData(false));

  CQChartsDrawUtil::drawSymbol(device, CQChartsSymbol::Type::CIRCLE, trp.point2D(),
                               Length(4, CQChartsUnits::PIXEL), penBrush);
#endif
}

bool
CQChartsPolygon3DObj::
checkVisible() const
{
  auto *camera = plot3D()->camera();

  //---

  auto p1 = camera->transform(Point3D(0, 0, 0));
  auto p2 = camera->transform(normal());

  double dz = p2.z - p1.z;

  return (dz <= 0);
}

//------

CQChartsPolyline3DObj::
CQChartsPolyline3DObj(const CQChartsPlot3D *plot, const Polygon3D &poly) :
 CQChartsPlot3DObj(plot), poly_(poly)
{
}

void
CQChartsPolyline3DObj::
postDraw(PaintDevice *device)
{
  CQChartsPenBrush penBrush;

  auto lc = plot_->charts()->interpPaletteColor(ig());

  plot_->setPenBrush(penBrush, PenData(true, lc), BrushData(false));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  auto *camera = plot3D()->camera();

  Polygon poly;

  for (const auto &p : poly_.points())
    poly.addPoint(camera->transform(p).point2D());

  device->drawPolygon(poly);
}

//------

CQChartsAxisPolygon3DObj::
CQChartsAxisPolygon3DObj(const CQChartsPlot3D *plot, const Polygon3D &poly) :
 CQChartsPolygon3DObj(plot, poly)
{
}

void
CQChartsAxisPolygon3DObj::
postDraw(PaintDevice *device)
{
  if (! checkVisible())
    return;

  //---

  CQChartsPolygon3DObj::postDraw(device);

  //---

  auto *camera = plot3D()->camera();

  const auto &range3D = plot3D()->range3D();

  double dx = range3D.xmax() - range3D.xmin();
  double dy = range3D.ymax() - range3D.ymin();
  double dz = range3D.zmax() - range3D.zmin();

  //---

  // draw grid lines
  CQChartsPenBrush penBrush;

  QColor gc(150, 150, 150);

  plot_->setPenBrush(penBrush, PenData(true, gc), BrushData(false));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  for (const auto &line : gridLines_) {
    device->drawLine(camera->transform(line.p1).point2D(),
                     camera->transform(line.p2).point2D());
  }

  //---

  QColor lc(0, 0, 0);

  plot_->setPenBrush(penBrush, PenData(true, lc), BrushData(false));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  for (auto &sidePolygon : sidePolygons_) {
    if (sidePolygon.poly->checkVisible())
      continue;

    auto p1 = camera->transform(sidePolygon.p1).point2D();
    auto p2 = camera->transform(sidePolygon.p2).point2D();

    device->drawLine(p1, p2);
  }

  //---

//QColor tlc(100, 200, 100);
  QColor tlc(0, 0, 0);

  plot_->setPenBrush(penBrush, PenData(true, tlc), BrushData(false));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  for (const auto &text : tickTexts_) {
    if (text.poly->checkVisible())
      continue;

    //---

    auto pr1 = camera->transform(text.poly->refPoint());
    auto pr2 = camera->transform(text.poly->refPoint() + text.poly->normal());

    double dxr = pr2.x - pr1.x;

    CQChartsTextOptions textOptions;

    if (dxr > 0)
      textOptions.align = Qt::AlignRight | Qt::AlignVCenter;
    else
      textOptions.align = Qt::AlignLeft | Qt::AlignVCenter;

    //---

    auto p1 = text.p;
    auto p2 = p1 + Point3D(-0.01*text.poly->normal().x*dx,
                           -0.01*text.poly->normal().y*dy,
                           -0.01*text.poly->normal().z*dz);

    //---

    device->drawLine(camera->transform(p1).point2D(), camera->transform(p2).point2D());

    //---

    CQChartsDrawUtil::drawTextAtPoint(device, camera->transform(p2).point2D(),
                                      text.text, textOptions);
  }

  //---

  QColor llc(100, 100, 200);

  plot_->setPenBrush(penBrush, PenData(true, llc), BrushData(false));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  for (const auto &text : labelTexts_) {
    if (text.poly->checkVisible())
      continue;

    //---

    auto pr1 = camera->transform(text.poly->refPoint());
    auto pr2 = camera->transform(text.poly->refPoint() + text.poly->normal());

    double dxr = pr2.x - pr1.x;

    CQChartsTextOptions textOptions;

    if (dxr > 0)
      textOptions.align = Qt::AlignRight | Qt::AlignVCenter;
    else
      textOptions.align = Qt::AlignLeft | Qt::AlignVCenter;

    //---

    auto p = text.p;

    p += Point3D(-0.1*text.poly->normal().x*dx,
                 -0.1*text.poly->normal().y*dy,
                 -0.1*text.poly->normal().z*dz);

    //---

    auto pt = camera->transform(p).point2D();

    CQChartsDrawUtil::drawTextAtPoint(device, pt, text.text, textOptions);
  }
}
