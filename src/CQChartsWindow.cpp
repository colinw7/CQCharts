#include <CQChartsWindow.h>
#include <CQChartsPlot.h>
#include <CQPropertyTree.h>
#include <CQToolTip.h>
#include <CQUtil.h>

#include <svg/select_svg.h>
#include <svg/zoom_svg.h>

#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QRubberBand>
#include <QPainter>
#include <QMouseEvent>
#include <QPainter>

class CQChartsWindowToolTip : public CQToolTipIFace {
 public:
  CQChartsWindowToolTip(CQChartsWindow *window) :
   window_(window) {
  }

 ~CQChartsWindowToolTip() {
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

    QPoint p = window_->mapFromGlobal(gpos);

    double wx, wy;

    window_->pixelToWindow(p.x(), p.y(), wx, wy);

    CQChartsPlot *plot = window_->plotAt(CPoint2D(wx, wy));

    if (! plot)
      return false;

    CPoint2D w;

    plot->pixelToWindow(CQUtil::fromQPoint(QPointF(p)), w);

    QString tip;

    if (! plot->tipText(w, tip))
      return false;

    widget_->setText(tip);

    return true;
  }

 private:
  CQChartsWindow* window_ { nullptr };
  QLabel*         widget_ { nullptr };
};

//------

CQChartsWindow::
CQChartsWindow(QWidget *parent) :
 parent_(parent)
{
  setMouseTracking(true);

  setFocusPolicy(Qt::StrongFocus);

  //---

  displayRange_.setWindowRange(0, 0, 1000, 1000);

  //---

  expander_     = new CQChartsWindowExpander(this);
  propertyTree_ = new CQPropertyTree(this);
  status_       = new CQChartsWindowStatus(this);
  toolbar_      = new CQChartsWindowToolBar(this);

  //---

  addProperty("", this, "background");

  //---

  updateMargins();

  CQToolTip::setToolTip(this, new CQChartsWindowToolTip(this));
}

CQChartsWindow::
~CQChartsWindow()
{
}

void
CQChartsWindow::
updateMargins()
{
  QFontMetrics fm(font());

  statusHeight_  = fm.height() + 4;
  toolBarHeight_ = fm.height() + 4;

  setContentsMargins(/*l*/0, /*t*/toolBarHeight_, /*r*/expander_->width(), /*b*/statusHeight_);
}

void
CQChartsWindow::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  propertyTree_->addProperty(path, object, name, alias);
}

void
CQChartsWindow::
addPlot(CQChartsPlot *plot, const CBBox2D &bbox)
{
  plot->setBBox(bbox);

  plots_.push_back(PlotData(plot, bbox));
}

void
CQChartsWindow::
mousePressEvent(QMouseEvent *me)
{
  mouseData_.pressPoint = me->pos();
  mouseData_.pressed    = true;

  if      (mode_ == Mode::SELECT) {
    CPoint2D w = pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())));

    mouseData_.plot = plotAt(w);

    if (mouseData_.plot) {
      CPoint2D w;

      mouseData_.plot->pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())), w);

      mouseData_.plot->mousePress(w);
    }
  }
  else if (mode_ == Mode::ZOOM) {
    if (! zoomBand_)
      zoomBand_ = new QRubberBand(QRubberBand::Rectangle, this);

    zoomBand_->setGeometry(QRect(mouseData_.pressPoint, QSize()));
    zoomBand_->show();
  }
}

void
CQChartsWindow::
mouseMoveEvent(QMouseEvent *me)
{
  if      (mode_ == Mode::SELECT) {
    CPoint2D w = pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())));

    if (! mouseData_.pressed)
      mouseData_.plot = plotAt(w);

    if (mouseData_.plot) {
      CPoint2D w;

      mouseData_.plot->pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())), w);

      mouseData_.plot->mouseMove(w);
    }
  }
  else if (mode_ == Mode::ZOOM) {
    if (mouseData_.pressed) {
      mouseData_.movePoint = me->pos();

      zoomBand_->setGeometry(QRect(mouseData_.pressPoint, mouseData_.movePoint));
    }
  }
}

void
CQChartsWindow::
mouseReleaseEvent(QMouseEvent *me)
{
  if      (mode_ == Mode::SELECT) {
    if (mouseData_.pressed && mouseData_.plot) {
      CPoint2D w;

      mouseData_.plot->pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())), w);

      mouseData_.plot->mouseRelease(w);
    }
  }
  else if (mode_ == Mode::ZOOM) {
    if (mouseData_.pressed) {
      mouseData_.pressed   = false;
      mouseData_.movePoint = me->pos();

      zoomBand_->hide();

      CPoint2D w1, w2;

      mouseData_.plot->pixelToWindow(CQUtil::fromQPointF(mouseData_.pressPoint), w1);
      mouseData_.plot->pixelToWindow(CQUtil::fromQPointF(mouseData_.movePoint ), w2);

      CBBox2D bbox(w1, w2);

      mouseData_.plot->zoomTo(bbox);
    }
  }
}

void
CQChartsWindow::
keyPressEvent(QKeyEvent *ke)
{
  QPoint gpos = QCursor::pos();

  QPointF pos = mapFromGlobal(gpos);

  CPoint2D w = pixelToWindow(CQUtil::fromQPoint(pos));

  CQChartsPlot *plot = plotAt(w);

  if (plot) {
    plot->keyPress(ke->key());
  }
}

void
CQChartsWindow::
resizeEvent(QResizeEvent *)
{
  updateGeometry();

  for (const auto &plot : plots_)
    plot.plot->handleResize();
}

void
CQChartsWindow::
updateGeometry()
{
  displayRange_.setPixelRange(0, toolBarHeight_,
    width(), height() - statusHeight_ - toolBarHeight_);

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
CQChartsWindow::
moveExpander(int dx)
{
  expander_->move(expander_->x() - dx, expander_->y());

  propertyTree_->resize(propertyTree_->width() + dx, propertyTree_->height());

  propertyTree_->move(propertyTree_->x() - dx, propertyTree_->y());
}

void
CQChartsWindow::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  painter.fillRect(rect(), QBrush(background()));

  for (const auto &plot : plots_)
    plot.plot->draw(&painter);
}

CQChartsPlot *
CQChartsWindow::
plotAt(const CPoint2D &p) const
{
  for (const auto &plot : plots_)
    if (plot.bbox.inside(p))
      return plot.plot;

  return nullptr;
}

void
CQChartsWindow::
setStatusText(const QString &text)
{
  status_->setText(text);
}

void
CQChartsWindow::
windowToPixel(double wx, double wy, double &px, double &py) const
{
  displayRange_.windowToPixel(wx, wy, &px, &py);
}

void
CQChartsWindow::
pixelToWindow(double px, double py, double &wx, double &wy) const
{
  displayRange_.pixelToWindow(px, py, &wx, &wy);
}

CPoint2D
CQChartsWindow::
windowToPixel(const CPoint2D &w) const
{
  CPoint2D p;

  windowToPixel(w.x, w.y, p.x, p.y);

  return p;
}

CPoint2D
CQChartsWindow::
pixelToWindow(const CPoint2D &p) const
{
  CPoint2D w;

  pixelToWindow(p.x, p.y, w.x, w.y);

  return w;
}

QSize
CQChartsWindow::
sizeHint() const
{
  return QSize(1000, 1000);
}

//---

CQChartsWindowExpander::
CQChartsWindowExpander(CQChartsWindow *window) :
 QFrame(window), window_(window)
{
  setObjectName("expander");

  setAutoFillBackground(true);

  setFixedWidth(8);
}

void
CQChartsWindowExpander::
mousePressEvent(QMouseEvent *e)
{
  int s  = width();
  int ym = height()/2;

  QRect handleRect(0, ym - s, width(), 2*s);

  if (handleRect.contains(e->pos())) {
    setExpanded(! isExpanded());

    window_->updateGeometry();

    return;
  }

  if (! isExpanded())
    return;

  pressed_  = true;
  pressPos_ = e->pos();
}

void
CQChartsWindowExpander::
mouseMoveEvent(QMouseEvent *e)
{
  if (pressed_) {
    movePos_ = e->pos();

    int dx = pressPos_.x() - movePos_.x();

    window_->moveExpander(dx);

    movePos_ = pressPos_;
  }
}

void
CQChartsWindowExpander::
mouseReleaseEvent(QMouseEvent *e)
{
  mouseMoveEvent(e);

  pressed_ = false;
}

void
CQChartsWindowExpander::
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

CQChartsWindowStatus::
CQChartsWindowStatus(CQChartsWindow *window) :
 QFrame(window), window_(window)
{
  setObjectName("status");
}

void
CQChartsWindowStatus::
setText(const QString &s)
{
  text_ = s;

  update();
}

void
CQChartsWindowStatus::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  QFontMetrics fm(font());

  p.drawText(2, 2 + fm.ascent(), text());
}

//---

CQChartsWindowToolBar::
CQChartsWindowToolBar(CQChartsWindow *window) :
 QFrame(window), window_(window)
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
CQChartsWindowToolBar::
selectSlot(bool)
{
  window_->setMode(CQChartsWindow::Mode::SELECT);

  zoomButton_  ->setChecked(false);
  selectButton_->setChecked(true);
}

void
CQChartsWindowToolBar::
zoomSlot(bool)
{
  window_->setMode(CQChartsWindow::Mode::ZOOM);

  selectButton_->setChecked(false);
  zoomButton_  ->setChecked(true);
}
