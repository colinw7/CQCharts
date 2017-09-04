#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsTitle.h>
#include <CQChartsPlotObj.h>
#include <CQChartsModel.h>
#include <CQPropertyTree.h>
#include <CQUtil.h>
#include <CGradientPalette.h>

#include <QSortFilterProxyModel>
#include <QPainter>

CQChartsPlot::
CQChartsPlot(CQChartsView *view, QAbstractItemModel *model) :
 view_(view), model_(model), displayTransform_(&displayRange_)
{
  displayRange_.setPixelRange(0, 1000, 1000, 0);

  displayRange_.setWindowRange(0, 0, 1, 1);
}

CQChartsPlot::
~CQChartsPlot()
{
}

CQChartsModel *
CQChartsPlot::
chartsModel() const
{
  CQChartsModel *model = qobject_cast<CQChartsModel *>(model_);

  if (! model) {
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(model_);

    if (proxyModel)
      model = qobject_cast<CQChartsModel *>(proxyModel->sourceModel());
  }

  return model;
}

void
CQChartsPlot::
setTitle(const QString &s)
{
  title_ = s;

  if (titleObj_)
    titleObj_->setText(title_);
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

  updateKeyPosition(/*force*/true);

  updateTitlePosition();

  update();
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

QRectF
CQChartsPlot::
range() const
{
  CBBox2D bbox;

  if (dataRange_.isSet())
    bbox = CBBox2D(dataRange_.xmin(), dataRange_.ymin(), dataRange_.xmax(), dataRange_.ymax());
  else
    bbox = CBBox2D(0, 0, 1, 1);

  return CQUtil::toQRect(bbox);
}

void
CQChartsPlot::
setRange(const QRectF &r)
{
  CBBox2D bbox = CQUtil::fromQRect(r);

  dataRange_.set(bbox.getXMin(), bbox.getYMin(), bbox.getXMax(), bbox.getYMax());

  applyDataRange();
}

void
CQChartsPlot::
addProperties()
{
  addProperty("", this, "background" );
  addProperty("", this, "rect"       );
  addProperty("", this, "range"      );
  addProperty("", this, "followMouse");
  addProperty("", this, "showBoxes"  );

  addProperty("margin", this, "marginLeft"  , "left"  );
  addProperty("margin", this, "marginTop"   , "top"   );
  addProperty("margin", this, "marginRight" , "right" );
  addProperty("margin", this, "marginBottom", "bottom");

  if (xAxis_)
    xAxis_->addProperties(view_->propertyTree(), id() + "/" + "X Axis");

  if (yAxis_)
    yAxis_->addProperties(view_->propertyTree(), id() + "/" + "Y Axis");

  if (keyObj_)
    keyObj_->addProperties(view_->propertyTree(), id() + "/" + "Key");

  if (titleObj_)
    titleObj_->addProperties(view_->propertyTree(), id() + "/" + "Title");
}

void
CQChartsPlot::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  view_->addProperty(id() + "/" + path, object, name, alias);
}

void
CQChartsPlot::
addAxes()
{
  addXAxis();
  addYAxis();
}

void
CQChartsPlot::
addXAxis()
{
  xAxis_ = new CQChartsAxis(this, CQChartsAxis::Direction::HORIZONTAL, 0, 1);
}

void
CQChartsPlot::
addYAxis()
{
  yAxis_ = new CQChartsAxis(this, CQChartsAxis::Direction::VERTICAL, 0, 1);
}

void
CQChartsPlot::
addKey()
{
  keyObj_ = new CQChartsKey(this);
}

void
CQChartsPlot::
addTitle()
{
  titleObj_ = new CQChartsTitle(this);

  titleObj_->setText(title_);
}

void
CQChartsPlot::
applyDataRange(bool propagate)
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

  if (propagate) {
    if (rootPlot_) {
      rootPlot_->setDataRange(dataRange_);

      applyDataRange(/*propagate*/false);
    }
    else {
      for (const auto &plot : refPlots_) {
        plot->setDataRange(dataRange_);

        applyDataRange(/*propagate*/false);
      }
    }
  }

  updateKeyPosition(/*force*/true);

  updateTitlePosition();
}

void
CQChartsPlot::
applyDisplayTransform(bool propagate)
{
  if (propagate) {
    if (rootPlot_) {
      rootPlot_->setDisplayTransform(displayTransform_);

      applyDisplayTransform(/*propagate*/false);
    }
    else {
      for (const auto &plot : refPlots_) {
        plot->setDisplayTransform(displayTransform_);

        applyDisplayTransform(/*propagate*/false);
      }
    }
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

bool
CQChartsPlot::
mousePress(const CPoint2D &w)
{
  if (keyObj_ && keyObj_->contains(w)) {
    CQChartsKeyItem *item = keyObj_->getItemAt(w);

    bool handled = false;

    if (item)
      handled = item->mousePress(w);

    if (! handled)
      handled = keyObj_->mousePress(w);

    if (handled)
      return true;
  }

  //---

  if (titleObj_ && titleObj_->contains(w)) {
    if (titleObj_->mousePress(w))
      return true;
  }

  //---

  typedef std::map<CQChartsPlotObj *, bool> PlotSelected;

  PlotSelected selectedPlots;

  for (auto &plotObj : plotObjs_)
    selectedPlots[plotObj] = false;

  PlotObjTree::DataList dataList;

  objsAtPoint(w, dataList);

  for (auto obj : dataList) {
    selectedPlots[obj] = ! obj->isSelected();

    emit objPressed(obj);

    QString id = obj->id();

    if (id.length())
      std::cerr << id.toStdString() << std::endl;
  }

  //---

  bool changed = false;

  for (const auto &plot : selectedPlots) {
    if (plot.first->isSelected() != plot.second) {
      plot.first->setSelected(plot.second);

      changed = true;
    }
  }

  if (changed)
    update();

  //---

  return ! dataList.empty();
}

void
CQChartsPlot::
mouseMove(const CPoint2D &w)
{
  QString posText = xStr(w.x) + " " + yStr(w.y);

  //---

  QString objText;

  PlotObjTree::DataList dataList;

  if (isFollowMouse()) {
    objsAtPoint(w, dataList);

    for (auto obj : dataList) {
      if (objText != "")
      objText += " ";

      objText += obj->id();
    }
  }

  //---

  if (objText != "")
    view_->setStatusText(objText + " : " + posText);
  else
    view_->setStatusText(posText);

  //--

  if (isFollowMouse()) {
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
}

void
CQChartsPlot::
mouseRelease(const CPoint2D &)
{
}

void
CQChartsPlot::
setXValueColumn(int column)
{
  xValueColumn_ = column;

  if (xAxis_)
    xAxis_->setColumn(xValueColumn_);
}

void
CQChartsPlot::
setYValueColumn(int column)
{
  yValueColumn_ = column;

  if (yAxis_)
    yAxis_->setColumn(yValueColumn_);
}

QString
CQChartsPlot::
xStr(double x) const
{
  if (xValueColumn_ < 0)
    return CQChartsUtil::toString(x);

  CQChartsModel *model = chartsModel();

  if (! model)
    return CQChartsUtil::toString(x);

  CQChartsNameValues nameValues;

  CQChartsColumnType *typeData = model->columnTypeData(xValueColumn_, nameValues);

  if (! typeData)
    return CQChartsUtil::toString(x);

  return typeData->dataName(x, nameValues);
}

QString
CQChartsPlot::
yStr(double y) const
{
  if (yValueColumn_ < 0)
    return CQChartsUtil::toString(y);

  CQChartsModel *model = chartsModel();

  if (! model)
    return CQChartsUtil::toString(y);

  CQChartsNameValues nameValues;

  CQChartsColumnType *typeData = model->columnTypeData(yValueColumn_, nameValues);

  if (! typeData)
    return CQChartsUtil::toString(y);

  return typeData->dataName(y, nameValues);
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

  applyDisplayTransform();

  handleResize();

  update();
}

void
CQChartsPlot::
zoomTo(const CBBox2D &bbox)
{
  if (bbox.getWidth() < 1E-50 || bbox.getHeight() < 1E-50)
    return;

  displayTransform_.zoomTo(bbox);

  applyDisplayTransform();

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

void
CQChartsPlot::
handleResize()
{
  updateKeyPosition(/*force*/true);

  updateTitlePosition();

  for (auto &obj : plotObjs_)
    obj->handleResize();
}

void
CQChartsPlot::
updateKeyPosition(bool force)
{
  if (! keyObj_)
    return;

  if (force)
    keyObj_->invalidateLayout();

  if (! dataRange_.isSet())
    return;

  QSizeF ks = keyObj_->calcSize();

  CQChartsKey::Location location = keyObj_->location();

  double xm = pixelToWindowWidth (8);
  double ym = pixelToWindowHeight(8);

  double kx { 0.0 }, ky { 0.0 };

  if      (location == CQChartsKey::Location::TOP_LEFT ||
           location == CQChartsKey::Location::CENTER_LEFT ||
           location == CQChartsKey::Location::BOTTOM_LEFT) {
    if (keyObj_->isInsideX())
      kx = dataRange_.xmin() + xm;
    else
      kx = dataRange_.xmin() - ks.width() - xm;
  }
  else if (location == CQChartsKey::Location::TOP_CENTER ||
           location == CQChartsKey::Location::CENTER_CENTER ||
           location == CQChartsKey::Location::BOTTOM_CENTER) {
    kx = dataRange_.xmid() - ks.width()/2;
  }
  else if (location == CQChartsKey::Location::TOP_RIGHT ||
           location == CQChartsKey::Location::CENTER_RIGHT ||
           location == CQChartsKey::Location::BOTTOM_RIGHT) {
    if (keyObj_->isInsideX())
      kx = dataRange_.xmax() - ks.width() - xm;
    else
      kx = dataRange_.xmax() + xm;
  }

  if      (location == CQChartsKey::Location::TOP_LEFT ||
           location == CQChartsKey::Location::TOP_CENTER ||
           location == CQChartsKey::Location::TOP_RIGHT) {
    if (keyObj_->isInsideY())
      ky = dataRange_.ymax() - ym;
    else
      ky = dataRange_.ymax() + ks.height() + ym;
  }
  else if (location == CQChartsKey::Location::CENTER_LEFT ||
           location == CQChartsKey::Location::CENTER_CENTER ||
           location == CQChartsKey::Location::CENTER_RIGHT) {
    ky = dataRange_.ymid() - ks.height()/2;
  }
  else if (location == CQChartsKey::Location::BOTTOM_LEFT ||
           location == CQChartsKey::Location::BOTTOM_CENTER ||
           location == CQChartsKey::Location::BOTTOM_RIGHT) {
    if (keyObj_->isInsideY())
      ky = dataRange_.ymin() + ks.height() + ym;
    else
      ky = dataRange_.ymin() - ym;
  }

  keyObj_->setPosition(QPointF(kx, ky));
}

void
CQChartsPlot::
updateTitlePosition()
{
  if (! titleObj_)
    return;

  if (! dataRange_.isSet())
    return;

  QSizeF ts = titleObj_->calcSize();

  CQChartsTitle::Location location = titleObj_->location();

//double xm = pixelToWindowWidth (8);
  double ym = pixelToWindowHeight(8);

  double kx = dataRange_.xmid() - ts.width()/2;

  double ky = 0.0;

  if      (location == CQChartsTitle::Location::TOP) {
    if (! titleObj_->isInside())
      ky = dataRange_.ymax() + ym;
    else
      ky = dataRange_.ymax() - ts.height() - ym;
  }
  else if (location == CQChartsTitle::Location::CENTER) {
    ky = dataRange_.ymid() - ts.height()/2;
  }
  else if (location == CQChartsTitle::Location::BOTTOM) {
    if (! titleObj_->isInside())
      ky = dataRange_.ymin() - ts.height() - ym;
    else
      ky = dataRange_.ymin() + ym;
  }
  else {
    ky = dataRange_.ymid() - ts.height()/2;
  }

  titleObj_->setPosition(QPointF(kx, ky));
}

void
CQChartsPlot::
drawBackground(QPainter *painter)
{
  //painter->fillRect(rect(), QBrush(Qt::white));

  //---

  //double pxmin, pymin, pxmax, pymax;

  //double xmin, ymin, xmax, ymax;

  //displayRange_.getWindowRange(&xmin, &ymin, &xmax, &ymax);

  //windowToPixel(xmin, ymin, pxmin, pymax);
  //windowToPixel(xmax, ymax, pxmax, pymin);

  painter->fillRect(CQUtil::toQRect(calcPixelRect()), QBrush(background()));
}

QRectF
CQChartsPlot::
calcRect() const
{
  double pxmin, pymin, pxmax, pymax;

  double xmin, ymin, xmax, ymax;

  displayRange_.getWindowRange(&xmin, &ymin, &xmax, &ymax);

  windowToPixel(xmin, ymin, pxmin, pymax);
  windowToPixel(xmax, ymax, pxmax, pymin);

  return QRectF(pxmin, pymin, pxmax - pxmin - 1, pymax - pymin - 1);
}

CBBox2D
CQChartsPlot::
calcPixelRect() const
{
  //double xmin, ymin, xmax, ymax;

  //displayRange_.getPixelRange(&xmin, &ymin, &xmax, &ymax);

  //double px1, py1, px2, py2;

  //view_->windowToPixel(xmin, ymin, px1, py1);
  //view_->windowToPixel(xmax, ymax, px2, py2);

  //return CBBox2D(px1, py1, px2, py2);

  return view_->windowToPixel(bbox_);
}

void
CQChartsPlot::
autoFit()
{
  double xmin, ymin, xmax, ymax;

  displayRange_.getWindowRange(&xmin, &ymin, &xmax, &ymax);

  CBBox2D pbbox(xmin, ymin, xmax, ymax);

  CBBox2D bbox = pbbox;

  if (xAxis_)
    bbox += xAxis_->bbox();

  if (yAxis_)
    bbox += yAxis_->bbox();

  if (keyObj_)
    bbox += keyObj_->bbox();

  if (titleObj_)
    bbox += titleObj_->bbox();

  margin_.left   = 100.0*(pbbox.getXMin() -  bbox.getXMin())/bbox.getWidth ();
  margin_.bottom = 100.0*(pbbox.getYMin() -  bbox.getYMin())/bbox.getHeight();
  margin_.right  = 100.0*( bbox.getXMax() - pbbox.getXMax())/bbox.getWidth ();
  margin_.top    = 100.0*( bbox.getYMax() - pbbox.getYMax())/bbox.getHeight();

  updateMargin();
}

void
CQChartsPlot::
drawObjs(QPainter *painter)
{
  for (const auto &plotObj : plotObjs_)
    plotObj->draw(painter);
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
  if (keyObj_)
    keyObj_->draw(painter);
}

void
CQChartsPlot::
drawTitle(QPainter *painter)
{
  if (titleObj_)
    titleObj_->draw(painter);
}

void
CQChartsPlot::
drawSymbol(QPainter *painter, const CPoint2D &p, CSymbol2D::Type type, double s,
           const QColor &c, bool filled)
{
  class CQChartsPlotSymbol2DRenderer : public CSymbol2DRenderer {
   public:
    CQChartsPlotSymbol2DRenderer(CQChartsPlot *plot, QPainter *painter,
                                 const CPoint2D &p, double s) :
     plot_(plot), painter_(painter), p_(p), s_(s) {
      pc_ = painter_->pen  ().color();
      fc_ = painter_->brush().color();

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
      painter_->strokePath(path_, QPen(pc_));
    }

    void fill() override {
      painter_->fillPath(path_, QBrush(fc_));
    }

    void strokeCircle(double x, double y, double r) override {
      QRectF rect(px_.x + (x - r)*s_, px_.y + (y - r)*s_, 2*r*s_, 2*r*s_);

      painter_->save();

      painter_->setBrush(Qt::NoBrush);
      painter_->setPen  (pc_);

      painter_->drawEllipse(rect);

      painter_->restore();
    }

    void fillCircle(double x, double y, double r) override {
      QRectF rect(px_.x + (x - r)*s_, px_.y + (y - r)*s_, 2*r*s_, 2*r*s_);

      painter_->save();

      painter_->setBrush(fc_);
      painter_->setPen  (Qt::NoPen);

      painter_->drawEllipse(rect);

      painter_->restore();
    }

    double lineWidth() const override {
      return w_;
    }

   private:
    CQChartsPlot *plot_    { nullptr };
    QPainter     *painter_ { nullptr };
    CPoint2D      p_       { 0, 0 };
    CPoint2D      px_      { 0, 0 };
    double        s_       { 2.0 };
    double        w_       { 0.0 };
    QPainterPath  path_;
    QColor        pc_;
    QColor        fc_;
  };

  //---

  painter->setPen  (c);
  painter->setBrush(c);

  CQChartsPlotSymbol2DRenderer renderer(this, painter, p, s);

  if (! filled)
    CSymbol2DMgr::drawSymbol(type, &renderer);
  else
    CSymbol2DMgr::fillSymbol(type, &renderer);
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
  view_->update();
}

QColor
CQChartsPlot::
paletteColor(int i, int n, const QColor &def) const
{
  return interpPaletteColor((1.0*i + 1.0)/(n + 1), def);
}

QColor
CQChartsPlot::
interpPaletteColor(double r, const QColor &def) const
{
  QColor c = def;

  if (palette()) {
    CRGBA rgba = palette()->getColor(r).rgba();

    c = CQUtil::toQColor(rgba);
  }

  return c;
}

QColor
CQChartsPlot::
textColor(const QColor &bg) const
{
  int g = qGray(bg.red(), bg.green(), bg.blue());

  return (g > 80 ? QColor(0,0,0) : QColor(255, 255, 255));
}

void
CQChartsPlot::
windowToPixel(double wx, double wy, double &px, double &py) const
{
  double wx1, wy1;

  displayTransform_.getMatrix().multiplyPoint(wx, wy, &wx1, &wy1);

  double wx2, wy2;

  displayRange_.windowToPixel(wx1, wy1, &wx2, &wy2);

  view_->windowToPixel(wx2, wy2, px, py);
}

void
CQChartsPlot::
pixelToWindow(double px, double py, double &wx, double &wy) const
{
  double wx1, wy1;

  view_->pixelToWindow(px, py, wx1, wy1);

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
