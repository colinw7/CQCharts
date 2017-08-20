#include <CQChartsPlot.h>
#include <CQChartsWindow.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsPlotObj.h>
#include <CQPropertyTree.h>
#include <CQUtil.h>
#include <CGradientPalette.h>

#include <QPainter>

CQChartsPlot::
CQChartsPlot(CQChartsWindow *window, QAbstractItemModel *model) :
 window_(window), model_(model), displayTransform_(&displayRange_)
{
  displayRange_.setPixelRange(200, 800, 800, 200);

  displayRange_.setWindowRange(0, 0, 1, 1);
}

CQChartsPlot::
~CQChartsPlot()
{
}

void
CQChartsPlot::
setBBox(const CBBox2D &bbox)
{
  bbox_ = bbox;

  updateMargin();
}

void
CQChartsPlot::
updateMargin()
{
  double xml = bbox_.getWidth ()*marginLeft  ()/100.0;
  double ymt = bbox_.getHeight()*marginTop   ()/100.0;
  double xmr = bbox_.getWidth ()*marginRight ()/100.0;
  double ymb = bbox_.getHeight()*marginBottom()/100.0;

  displayRange_.setPixelRange(bbox_.getXMin() + xml, bbox_.getYMax() - ymt,
                              bbox_.getXMax() - xmr, bbox_.getYMin() + ymb);
}

QRectF
CQChartsPlot::
rect() const
{
  return CQUtil::toQRect(bbox());
}

void
CQChartsPlot::
setRect(const QRectF &r)
{
  setBBox(CQUtil::fromQRect(r));
}

void
CQChartsPlot::
addProperties()
{
  addProperty("", this, "background");
  addProperty("", this, "rect"      );

  addProperty("margin", this, "marginLeft"  , "left"  );
  addProperty("margin", this, "marginTop"   , "top"   );
  addProperty("margin", this, "marginRight" , "right" );
  addProperty("margin", this, "marginBottom", "bottom");

  if (xAxis_)
    xAxis_->addProperties(window_->propertyTree(), id() + "/" + "X Axis");

  if (yAxis_)
    yAxis_->addProperties(window_->propertyTree(), id() + "/" + "Y Axis");

  if (key_)
    key_->addProperties(window_->propertyTree(), id() + "/" + "Key");
}

void
CQChartsPlot::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  window_->addProperty(id() + "/" + path, object, name, alias);
}

void
CQChartsPlot::
addAxes()
{
  xAxis_ = new CQChartsAxis(this, CQChartsAxis::DIR_HORIZONTAL, 0, 1);
  yAxis_ = new CQChartsAxis(this, CQChartsAxis::DIR_VERTICAL  , 0, 1);
}

void
CQChartsPlot::
addKey()
{
  key_ = new CQChartsKey(this);
}

void
CQChartsPlot::
applyDataRange()
{
  if (! dataRange_.isSet()) {
    dataRange_.updateRange(0, 0);
    dataRange_.updateRange(1, 1);
  }

  displayRange_.setWindowRange(dataRange_.xmin(), dataRange_.ymin(),
                               dataRange_.xmax(), dataRange_.ymax());

  if (xAxis_) {
    xAxis_->setRange(dataRange_.xmin(), dataRange_.xmax());
    yAxis_->setRange(dataRange_.ymin(), dataRange_.ymax());
  }
}

void
CQChartsPlot::
addPlotObject(CQChartsPlotObj *obj)
{
  if (! obj->rect().isSet())
    return;

  plotObjTree_.add(obj);

  plotObjs_.push_back(obj);
}

void
CQChartsPlot::
clearPlotObjects()
{
  for (auto &plotObj : plotObjs_)
    delete plotObj;

  plotObjs_.clear();

  plotObjTree_.reset();
}

void
CQChartsPlot::
mousePress(const CPoint2D &w)
{
  if (key_ && key_->contains(w)) {
    CQChartsKeyItem *item = key_->getItemAt(w);

    if (item)
      item->mousePress(w);
    else
      key_->mousePress(w);
  }

  PlotObjTree::DataList dataList;

  objsAtPoint(w, dataList);

  for (auto obj : dataList) {
    std::cerr << obj->id().toStdString() << std::endl;
  }
}

void
CQChartsPlot::
mouseMove(const CPoint2D &w)
{
  QString posText = QString("%1 %2").arg(w.x).arg(w.y);

  PlotObjTree::DataList dataList;

  objsAtPoint(w, dataList);

  QString objText;

  for (auto obj : dataList) {
    if (objText != "")
      objText += " ";

    objText += obj->id();
  }

  if (objText != "")
    window_->setStatusText(objText + " : " + posText);
  else
    window_->setStatusText(posText);

  //--

  bool changed = false;

  auto resetInside = [&] (CQChartsPlotObj *obj) -> void {
    for (const auto &obj1 : dataList) {
      if (obj1 == obj) {
        if (! obj->isInside()) {
          obj->setInside(true);

          changed = true;
        }

        return;
      }
    }

    if (obj->isInside()) {
      obj->setInside(false);

      changed = true;
    }
  };

  plotObjTree_.process(resetInside);

  if (changed)
    update();
}

void
CQChartsPlot::
mouseRelease(const CPoint2D &)
{
}

void
CQChartsPlot::
keyPress(int key)
{
  if      (key == Qt::Key_Left || key == Qt::Key_Right) {
    if (key == Qt::Key_Right)
      displayTransform_.panLeft();
    else
      displayTransform_.panRight();
  }
  else if (key == Qt::Key_Up || key == Qt::Key_Down) {
    if (key == Qt::Key_Up)
      displayTransform_.panDown();
    else
      displayTransform_.panUp();
  }
  else if (key == Qt::Key_Plus) {
    displayTransform_.zoomIn();
  }
  else if (key == Qt::Key_Minus) {
    displayTransform_.zoomOut();
  }
  else if (key == Qt::Key_Home) {
    displayTransform_.reset();
  }
  else
    return;

  handleResize();

  update();
}

void
CQChartsPlot::
zoomTo(const CBBox2D &bbox)
{
  displayTransform_.zoomTo(bbox);

  handleResize();

  update();
}

bool
CQChartsPlot::
tipText(const CPoint2D &p, QString &tip) const
{
  PlotObjTree::DataList dataList;

  objsAtPoint(p, dataList);

  if (dataList.empty())
    return false;

  for (auto obj : dataList) {
    if (! obj->visible())
      continue;

    if (tip != "")
      tip += " ";

    tip += obj->id();
  }

  return tip.length();
}

void
CQChartsPlot::
objsAtPoint(const CPoint2D &p, PlotObjTree::DataList &dataList1) const
{
  PlotObjTree::DataList dataList;

  plotObjTree_.dataAtPoint(p.x, p.y, dataList);

  for (const auto &obj : dataList) {
    if (obj->inside(p))
      dataList1.push_back(obj);
  }
}

#if 0
void
CQChartsPlot::
resizeEvent(QResizeEvent *)
{
  if (key_)
    key_->invalidateLayout();

  handleResize();
}
#endif

void
CQChartsPlot::
handleResize()
{
  updateKeyPosition();

  for (auto &obj : plotObjs_)
    obj->handleResize();
}

void
CQChartsPlot::
updateKeyPosition()
{
  if (! key_)
    return;

  QSizeF ks = key_->calcSize();

  CQChartsKey::Location location = key_->location();

  double kx, ky;

  if      (location == CQChartsKey::Location::TOP_LEFT) {
    kx = dataRange_.xmin();
    ky = dataRange_.ymax();
  }
  else if (location == CQChartsKey::Location::TOP_CENTER) {
    kx = dataRange_.xmid() - ks.width()/2;
    ky = dataRange_.ymax();
  }
  else if (location == CQChartsKey::Location::TOP_RIGHT) {
    kx = dataRange_.xmax() - ks.width();
    ky = dataRange_.ymax();
  }
  else if (location == CQChartsKey::Location::CENTER_LEFT) {
    kx = dataRange_.xmin();
    ky = dataRange_.ymid() - ks.height()/2;
  }
  else if (location == CQChartsKey::Location::CENTER_CENTER) {
    kx = dataRange_.xmid() - ks.width ()/2;
    ky = dataRange_.ymid() - ks.height()/2;
  }
  else if (location == CQChartsKey::Location::CENTER_RIGHT) {
    kx = dataRange_.xmax() - ks.width();
    ky = dataRange_.ymid() - ks.height()/2;
  }
  else if (location == CQChartsKey::Location::BOTTOM_LEFT) {
    kx = dataRange_.xmin();
    ky = dataRange_.ymin() + ks.height();
  }
  else if (location == CQChartsKey::Location::BOTTOM_CENTER) {
    kx = dataRange_.xmid() - ks.width()/2;
    ky = dataRange_.ymin() + ks.height();
  }
  else if (location == CQChartsKey::Location::BOTTOM_RIGHT) {
    kx = dataRange_.xmax() - ks.width();
    ky = dataRange_.ymin() + ks.height();
  }
  else {
    kx = dataRange_.xmid() - ks.width ()/2;
    ky = dataRange_.ymid() - ks.height()/2;
  }

  key_->setPosition(QPointF(kx, ky));
}

void
CQChartsPlot::
drawBackground(QPainter *painter)
{
  //painter->fillRect(rect(), QBrush(Qt::white));

  //---

  double pxmin, pymin, pxmax, pymax;

  double xmin, ymin, xmax, ymax;

  displayRange_.getWindowRange(&xmin, &ymin, &xmax, &ymax);

  windowToPixel(xmin, ymin, pxmin, pymax);
  windowToPixel(xmax, ymax, pxmax, pymin);

  painter->fillRect(QRect(pxmin, pymin, pxmax - pxmin - 1, pymax - pymin - 1),
                    QBrush(background()));
}

void
CQChartsPlot::
drawAxes(QPainter *painter)
{
  if (xAxis_ && xAxis_->getVisible())
    xAxis_->draw(this, painter);

  if (yAxis_ && yAxis_->getVisible())
    yAxis_->draw(this, painter);
}

void
CQChartsPlot::
drawKey(QPainter *painter)
{
  if (key_)
    key_->draw(painter);
}

void
CQChartsPlot::
drawSymbol(QPainter *painter, const CPoint2D &p, CSymbol2D::Type type, double s)
{
  class CQChartsPlotSymbol2DRenderer : public CSymbol2DRenderer {
   public:
    CQChartsPlotSymbol2DRenderer(CQChartsPlot *plot, QPainter *painter,
                                 const CPoint2D &p, double s) :
     plot_(plot), painter_(painter), p_(p), s_(s) {
      plot_->windowToPixel(p_, px_);
    }

    void moveTo(double x, double y) override {
      path_.moveTo(px_.x + x*s_, px_.y - y*s_);
    }

    void lineTo(double x, double y) override {
      path_.lineTo(px_.x + x*s_, px_.y - y*s_);
    }

    void closePath() override {
      path_.closeSubpath();
    }

    void stroke() override {
      painter_->strokePath(path_, painter_->pen());
    }

   private:
    CQChartsPlot *plot_    { nullptr };
    QPainter     *painter_ { nullptr };
    CPoint2D      p_       { 0, 0 };
    CPoint2D      px_      { 0, 0 };
    double        s_       { 2 };
    QPainterPath  path_;
  };

  //---

  CQChartsPlotSymbol2DRenderer renderer(this, painter, p, s);

  CSymbol2DMgr::drawSymbol(type, &renderer);
}

QColor
CQChartsPlot::
objectColor(CQChartsPlotObj *obj, int i, int n, const QColor &def) const
{
  QColor c = paletteColor(i, n, def);

  return objectStateColor(obj, c);
}

QColor
CQChartsPlot::
objectStateColor(CQChartsPlotObj *obj, const QColor &c) const
{
  QColor c1 = c;

  if (obj->isInside())
    c1 = CQUtil::blendColors(c1, Qt::white, 0.8);

  return c1;
}

void
CQChartsPlot::
update()
{
  window_->update();
}

QColor
CQChartsPlot::
paletteColor(int i, int n, const QColor &def) const
{
  QColor c = def;

  if (palette()) {
    CRGBA rgba = palette()->getColor((1.0*i + 1.0)/(n + 1)).rgba();

    c = CQUtil::toQColor(rgba);
  }

  return c;
}

void
CQChartsPlot::
windowToPixel(double wx, double wy, double &px, double &py) const
{
  double wx1, wy1;

  displayTransform_.getMatrix().multiplyPoint(wx, wy, &wx1, &wy1);

  double wx2, wy2;

  displayRange_.windowToPixel(wx1, wy1, &wx2, &wy2);

  window_->windowToPixel(wx2, wy2, px, py);
}

void
CQChartsPlot::
pixelToWindow(double px, double py, double &wx, double &wy) const
{
  double wx1, wy1;

  window_->pixelToWindow(px, py, wx1, wy1);

  double wx2, wy2;

  displayRange_.pixelToWindow(wx1, wy1, &wx2, &wy2);

  displayTransform_.getIMatrix().multiplyPoint(wx2, wy2, &wx, &wy);
}

void
CQChartsPlot::
windowToPixel(const CPoint2D &w, CPoint2D &p) const
{
  windowToPixel(w.x, w.y, p.x, p.y);
}

void
CQChartsPlot::
pixelToWindow(const CPoint2D &p, CPoint2D &w) const
{
  pixelToWindow(p.x, p.y, w.x, w.y);
}

void
CQChartsPlot::
windowToPixel(const CBBox2D &wrect, CBBox2D &prect) const
{
  double px1, py1, px2, py2;

  windowToPixel(wrect.getXMin(), wrect.getYMin(), px1, py2);
  windowToPixel(wrect.getXMax(), wrect.getYMax(), px2, py1);

  prect = CBBox2D(px1, py1, px2, py2);
}

void
CQChartsPlot::
pixelToWindow(const CBBox2D &prect, CBBox2D &wrect) const
{
  double wx1, wy1, wx2, wy2;

  pixelToWindow(prect.getXMin(), prect.getYMin(), wx1, wy2);
  pixelToWindow(prect.getXMax(), prect.getYMax(), wx2, wy1);

  wrect = CBBox2D(wx1, wy1, wx2, wy2);
}

double
CQChartsPlot::
pixelToWindowWidth(double pw) const
{
  double wx1, wy1, wx2, wy2;

  pixelToWindow( 0, 0, wx1, wy1);
  pixelToWindow(pw, 0, wx2, wy2);

  return std::abs(wx2 - wx1);
}

double
CQChartsPlot::
pixelToWindowHeight(double ph) const
{
  double wx1, wy1, wx2, wy2;

  pixelToWindow(0, 0 , wx1, wy1);
  pixelToWindow(0, ph, wx2, wy2);

  return std::abs(wy2 - wy1);
}

double
CQChartsPlot::
windowToPixelWidth(double ww) const
{
  double px1, py1, px2, py2;

  windowToPixel( 0, 0, px1, py1);
  windowToPixel(ww, 0, px2, py2);

  return std::abs(px2 - px1);
}

double
CQChartsPlot::
windowToPixelHeight(double wh) const
{
  double px1, py1, px2, py2;

  windowToPixel(0, 0 , px1, py1);
  windowToPixel(0, wh, px2, py2);

  return std::abs(py2 - py1);
}
