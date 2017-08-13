#include <CQChartsPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsPlotObj.h>
#include <CQPropertyTree.h>
#include <CQToolTip.h>
#include <CQUtil.h>
#include <CGradientPalette.h>

#include <svg/select_svg.h>
#include <svg/zoom_svg.h>

#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QRubberBand>
#include <QPainter>
#include <QMouseEvent>

class CQChartsPlotToolTip : public CQToolTipIFace {
 public:
  CQChartsPlotToolTip(CQChartsPlot *plot) :
   plot_(plot) {
  }

 ~CQChartsPlotToolTip() {
    delete widget_;
  }

  QWidget *showWidget(const QPoint &gpos) override {
    if (! widget_)
      widget_ = new QLabel;

    if (! updateWidget(gpos))
      return nullptr;

    return widget_;
  }

  void hideWidget() override {
    delete widget_;

    widget_ = 0;
  }

  bool trackMouse() const override { return true; }

  bool updateWidget(const QPoint &gpos) override {
    if (! widget_) return false;

    QPoint p = plot_->mapFromGlobal(gpos);

    double wx, wy;

    plot_->pixelToWindow(p.x(), p.y(), wx, wy);

    QString tip;

    if (! plot_->tipText(CPoint2D(wx, wy), tip))
      return false;

    widget_->setText(tip);

    return true;
  }

 private:
  CQChartsPlot* plot_   { nullptr };
  QLabel*       widget_ { nullptr };
};

//------

CQChartsPlot::
CQChartsPlot(QWidget *parent, QAbstractItemModel *model) :
 parent_(parent), model_(model), displayTransform_(&displayRange_)
{
  setMouseTracking(true);

  setFocusPolicy(Qt::StrongFocus);

  //---

  displayRange_.setWindowRange(0, 0, 1, 1);

  //---

  expander_ = new CQChartsPlotExpander(this);

  propertyTree_ = new CQPropertyTree(this);

  addProperty("", this, "background");

  //---

  status_ = new CQChartsPlotStatus(this);

  toolbar_ = new CQChartsPlotToolBar(this);

  //---

  updateMargins();

  CQToolTip::setToolTip(this, new CQChartsPlotToolTip(this));
}

CQChartsPlot::
~CQChartsPlot()
{
}

void
CQChartsPlot::
updateMargins()
{
  QFontMetrics fm(font());

  statusHeight_  = fm.height() + 4;
  toolBarHeight_ = fm.height() + 4;

  setContentsMargins(/*l*/0, /*t*/toolBarHeight_, /*r*/expander_->width(), /*b*/statusHeight_);
}

void
CQChartsPlot::
addAxes()
{
  xAxis_ = new CQChartsAxis(this, CQChartsAxis::DIR_HORIZONTAL, 0, 1);
  yAxis_ = new CQChartsAxis(this, CQChartsAxis::DIR_VERTICAL  , 0, 1);

  xAxis_->addProperties(propertyTree_, "X Axis");
  yAxis_->addProperties(propertyTree_, "Y Axis");
}

void
CQChartsPlot::
addKey()
{
  key_ = new CQChartsKey(this);

  key_->addProperties(propertyTree_, "Key");
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
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  propertyTree_->addProperty(path, object, name, alias);
}

void
CQChartsPlot::
addPlotObject(CQChartsPlotObj *obj)
{
  plotObjTree_.add(obj);

  plotObjs_.push_back(obj);
}

void
CQChartsPlot::
mousePressEvent(QMouseEvent *me)
{
  if      (mode_ == Mode::SELECT) {
    CPoint2D p = CQUtil::fromQPoint(QPointF(me->pos()));

    CPoint2D w;

    pixelToWindow(p, w);

    PlotObjTree::DataList dataList;

    objsAtPoint(w, dataList);

    for (auto obj : dataList) {
      std::cerr << obj->id().toStdString() << std::endl;
    }
  }
  else if (mode_ == Mode::ZOOM) {
    mouseData_.pressPoint = me->pos();
    mouseData_.pressed    = true;

    if (! zoomBand_)
      zoomBand_ = new QRubberBand(QRubberBand::Rectangle, this);

    zoomBand_->setGeometry(QRect(mouseData_.pressPoint, QSize()));
    zoomBand_->show();
  }
}

void
CQChartsPlot::
mouseMoveEvent(QMouseEvent *me)
{
  if      (mode_ == Mode::SELECT) {
    CPoint2D p = CQUtil::fromQPoint(QPointF(me->pos()));

    CPoint2D w;

    pixelToWindow(p, w);

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
      status_->setText(objText + " : " + posText);
    else
      status_->setText(posText);

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
  else if (mode_ == Mode::ZOOM) {
    if (mouseData_.pressed) {
      mouseData_.movePoint = me->pos();

      zoomBand_->setGeometry(QRect(mouseData_.pressPoint, mouseData_.movePoint));
    }
  }
}

void
CQChartsPlot::
mouseReleaseEvent(QMouseEvent *me)
{
  if      (mode_ == Mode::SELECT) {
  }
  else if (mode_ == Mode::ZOOM) {
    if (mouseData_.pressed) {
      mouseData_.pressed   = false;
      mouseData_.movePoint = me->pos();

      zoomBand_->hide();

      CPoint2D w1, w2;

      pixelToWindow(CQUtil::fromQPointF(mouseData_.pressPoint), w1);
      pixelToWindow(CQUtil::fromQPointF(mouseData_.movePoint ), w2);

      displayTransform_.zoomTo(CBBox2D(w1, w2));

      update();
    }
  }
}

void
CQChartsPlot::
keyPressEvent(QKeyEvent *ke)
{
  if      (ke->key() == Qt::Key_Left || ke->key() == Qt::Key_Right) {
    if (ke->key() == Qt::Key_Right)
      displayTransform_.panLeft();
    else
      displayTransform_.panRight();
  }
  else if (ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down) {
    if (ke->key() == Qt::Key_Up)
      displayTransform_.panDown();
    else
      displayTransform_.panUp();
  }
  else if (ke->key() == Qt::Key_Plus) {
    displayTransform_.zoomIn();
  }
  else if (ke->key() == Qt::Key_Minus) {
    displayTransform_.zoomOut();
  }
  else if (ke->key() == Qt::Key_Home) {
    displayTransform_.reset();
  }
  else
    return;

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
resizeEvent(QResizeEvent *)
{
  updateGeometry();

  if (key_)
    key_->invalidateLayout();

  handleResize();
}

void
CQChartsPlot::
handleResize()
{
  updateKeyPosition();
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
updateGeometry()
{
  double m = std::min(width(), height())/10;

  displayRange_.setPixelRange(m, m, width() - m, height() - m);

  propertyTree_->setVisible(expander_->isExpanded());

  if (expander_->isExpanded()) {
    propertyTree_->move  (width() - propertyTree_->width(), 0);
    propertyTree_->resize(propertyTree_->width(), height());
  }

  //---

  expander_->setVisible(true);

  if (expander_->isExpanded())
    expander_->move(width() - propertyTree_->width() - expander_->width(), 0);
  else
    expander_->move(width() - expander_->width(), 0);

  expander_->resize(expander_->width(), height());

  //---

  toolbar_->move(0, 0);

  //---

  status_->move(0, height() - statusHeight_);
  status_->resize(width(), statusHeight_);
}

void
CQChartsPlot::
moveExpander(int dx)
{
  expander_->move(expander_->x() - dx, expander_->y());

  propertyTree_->resize(propertyTree_->width() + dx, propertyTree_->height());

  propertyTree_->move(propertyTree_->x() - dx, propertyTree_->y());
}

void
CQChartsPlot::
drawBackground(QPainter *painter)
{
  painter->fillRect(rect(), QBrush(Qt::white));

  //---

  double pxmin, pymin, pxmax, pymax;

  displayRange_.getPixelRange(&pxmin, &pymin, &pxmax, &pymax);

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

  displayRange_.windowToPixel(wx1, wy1, &px, &py);
}

void
CQChartsPlot::
pixelToWindow(double px, double py, double &wx, double &wy) const
{
  double wx1, wy1;

  displayRange_.pixelToWindow(px, py, &wx1, &wy1);

  displayTransform_.getIMatrix().multiplyPoint(wx1, wy1, &wx, &wy);
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

QSize
CQChartsPlot::
sizeHint() const
{
  return QSize(1000, 1000);
}

//---

CQChartsPlotExpander::
CQChartsPlotExpander(CQChartsPlot *plot) :
 QFrame(plot), plot_(plot)
{
  setObjectName("expander");

  setAutoFillBackground(true);

  setFixedWidth(8);
}

void
CQChartsPlotExpander::
mousePressEvent(QMouseEvent *e)
{
  int s  = width();
  int ym = height()/2;

  QRect handleRect(0, ym - s, width(), 2*s);

  if (handleRect.contains(e->pos())) {
    setExpanded(! isExpanded());

    plot_->updateGeometry();

    return;
  }

  if (! isExpanded())
    return;

  pressed_  = true;
  pressPos_ = e->pos();
}

void
CQChartsPlotExpander::
mouseMoveEvent(QMouseEvent *e)
{
  if (pressed_) {
    movePos_ = e->pos();

    int dx = pressPos_.x() - movePos_.x();

    plot_->moveExpander(dx);

    movePos_ = pressPos_;
  }
}

void
CQChartsPlotExpander::
mouseReleaseEvent(QMouseEvent *e)
{
  mouseMoveEvent(e);

  pressed_ = false;
}

void
CQChartsPlotExpander::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  int s  = width();
  int ym = height()/2;

  p.setBrush(QColor(0,0,0));

  QPolygonF poly;

  if (! expanded_) {
    poly << QPoint(0, ym    );
    poly << QPoint(s, ym - s);
    poly << QPoint(s, ym + s);
  }
  else {
    poly << QPoint(s, ym    );
    poly << QPoint(0, ym - s);
    poly << QPoint(0, ym + s);
  }

  p.drawPolygon(poly);
}

//---

CQChartsPlotStatus::
CQChartsPlotStatus(CQChartsPlot *plot) :
 QFrame(plot), plot_(plot)
{
  setObjectName("status");
}

void
CQChartsPlotStatus::
setText(const QString &s)
{
  text_ = s;

  update();
}

void
CQChartsPlotStatus::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  QFontMetrics fm(font());

  p.drawText(2, 2 + fm.ascent(), text());
}

//---

CQChartsPlotToolBar::
CQChartsPlotToolBar(CQChartsPlot *plot) :
 QFrame(plot), plot_(plot)
{
  setObjectName("toolbar");

  QHBoxLayout *layout = new QHBoxLayout(this);

  selectButton_ = new QToolButton(this);

  selectButton_->setIcon(CQPixmapCacheInst->getIcon("SELECT"));
  selectButton_->setCheckable(true);
  selectButton_->setChecked(true);

  connect(selectButton_, SIGNAL(clicked(bool)), this, SLOT(selectSlot(bool)));

  zoomButton_ = new QToolButton(this);

  zoomButton_->setIcon(CQPixmapCacheInst->getIcon("ZOOM"));
  zoomButton_->setCheckable(true);

  connect(zoomButton_, SIGNAL(clicked(bool)), this, SLOT(zoomSlot(bool)));

  layout->addWidget(selectButton_);
  layout->addWidget(zoomButton_);
  layout->addStretch(1);
}

void
CQChartsPlotToolBar::
selectSlot(bool)
{
  plot_->setMode(CQChartsPlot::Mode::SELECT);

  zoomButton_  ->setChecked(false);
  selectButton_->setChecked(true);
}

void
CQChartsPlotToolBar::
zoomSlot(bool)
{
  plot_->setMode(CQChartsPlot::Mode::ZOOM);

  selectButton_->setChecked(false);
  zoomButton_  ->setChecked(true);
}
