#include <CQChartsWindow.h>
#include <CQChartsPlot.h>
#include <CQPropertyTree.h>
#include <CQToolTip.h>
#include <CQUtil.h>

#include <svg/select_svg.h>
#include <svg/zoom_svg.h>
#include <svg/probe_svg.h>

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

    CQChartsWindow::Plots plots;

    window_->plotsAt(CPoint2D(wx, wy), plots);

    if (plots.empty())
      return false;

    QString tip;

    for (const auto &plot : plots) {
      CPoint2D w;

      plot->pixelToWindow(CQUtil::fromQPoint(QPointF(p)), w);

      QString tip1;

      if (plot->tipText(w, tip1)) {
        if (tip.length())
          tip += "\n";

        tip += tip1;
      }
    }

    if (! tip.length())
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

  //setContentsMargins(/*l*/0, /*t*/toolBarHeight_, /*r*/expander_->width(), /*b*/statusHeight_);
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

  plotDatas_.push_back(PlotData(plot, bbox));
}

void
CQChartsWindow::
mousePressEvent(QMouseEvent *me)
{
  mouseData_.pressPoint = me->pos();
  mouseData_.pressed    = true;

  if      (mode_ == Mode::SELECT) {
    CPoint2D w = pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())));

    mouseData_.plots.clear();

    plotsAt(w, mouseData_.plots);

    for (auto &plot : mouseData_.plots) {
      CPoint2D w;

      plot->pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())), w);

      plot->mousePress(w);
    }
  }
  else if (mode_ == Mode::ZOOM) {
    if (! zoomBand_)
      zoomBand_ = new QRubberBand(QRubberBand::Rectangle, this);

    zoomBand_->setGeometry(QRect(mouseData_.pressPoint, QSize()));
    zoomBand_->show();
  }
  else if (mode_ == Mode::PROBE) {
  }
}

void
CQChartsWindow::
mouseMoveEvent(QMouseEvent *me)
{
  if      (mode_ == Mode::SELECT) {
    CPoint2D w = pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())));

    if (! mouseData_.pressed) {
      mouseData_.plots.clear();

      plotsAt(w, mouseData_.plots);
    }

    for (auto &plot : mouseData_.plots) {
      CPoint2D w;

      plot->pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())), w);

      plot->mouseMove(w);
    }
  }
  else if (mode_ == Mode::ZOOM) {
    if (mouseData_.pressed) {
      mouseData_.movePoint = me->pos();

      zoomBand_->setGeometry(QRect(mouseData_.pressPoint, mouseData_.movePoint));
    }
  }
  else if (mode_ == Mode::PROBE) {
    auto addProbeBand = [&](int &ind, const QString &tip, double px,
                            double py1, double py2) -> void {
      while (ind >= int(probeBands_.size())) {
        ProbeBand *probeBand = new ProbeBand(this);

        probeBands_.push_back(probeBand);
      }

      probeBands_[ind]->show(tip, px, py1, py2);

      ++ind;
    };

    int px = me->pos().x();

    CPoint2D w = pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())));

    mouseData_.plots.clear();

    plotsAt(w, mouseData_.plots);

    int probeInd = 0;

    for (auto &plot : mouseData_.plots) {
      CPoint2D w;

      plot->pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())), w);

      std::vector<double> yvals1;

      if (! plot->interpY(w.x, yvals1))
        continue;

      CPoint2D p1;

      plot->windowToPixel(CPoint2D(w.x, plot->dataRange().ymin()), p1);

      for (const auto &y1 : yvals1) {
        CPoint2D p2;

        plot->windowToPixel(CPoint2D(w.x, y1), p2);

        QString tip = QString("%1").arg(y1);

        addProbeBand(probeInd, tip, px, p1.y, p2.y);
      }
    }

    for (int i = probeInd; i < int(probeBands_.size()); ++i)
      probeBands_[i]->hide();
  }
}

void
CQChartsWindow::
mouseReleaseEvent(QMouseEvent *me)
{
  if      (mode_ == Mode::SELECT) {
    if (mouseData_.pressed) {
      for (auto &plot : mouseData_.plots) {
        CPoint2D w;

        plot->pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())), w);

        plot->mouseRelease(w);
      }
    }
  }
  else if (mode_ == Mode::ZOOM) {
    if (mouseData_.pressed) {
      mouseData_.movePoint = me->pos();

      zoomBand_->hide();

      for (auto &plot : mouseData_.plots) {
        CPoint2D w1, w2;

        plot->pixelToWindow(CQUtil::fromQPointF(mouseData_.pressPoint), w1);
        plot->pixelToWindow(CQUtil::fromQPointF(mouseData_.movePoint ), w2);

        CBBox2D bbox(w1, w2);

        plot->zoomTo(bbox);
      }
    }
  }
  else if (mode_ == Mode::PROBE) {
  }

  mouseData_.pressed = false;
}

void
CQChartsWindow::
keyPressEvent(QKeyEvent *ke)
{
  QPoint gpos = QCursor::pos();

  QPointF pos = mapFromGlobal(gpos);

  CPoint2D w = pixelToWindow(CQUtil::fromQPoint(pos));

  Plots plots;

  if (plotsAt(w, plots)) {
    CQChartsPlot *plot = plots[0];

    plot->keyPress(ke->key());
  }
}

void
CQChartsWindow::
resizeEvent(QResizeEvent *)
{
  updateGeometry();

  for (const auto &plot : plotDatas_)
    plot.plot->handleResize();
}

void
CQChartsWindow::
updateGeometry()
{
  displayRange_.setPixelRange(0, toolBarHeight_,
    width() - expander_->width(), height() - statusHeight_ - toolBarHeight_);

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

  for (const auto &plot : plotDatas_)
    plot.plot->draw(&painter);
}

CQChartsPlot *
CQChartsWindow::
plotAt(const CPoint2D &p) const
{
  Plots plots;

  if (! plotsAt(p, plots))
    return nullptr;

  return plots[0];
}

bool
CQChartsWindow::
plotsAt(const CPoint2D &p, Plots &plots) const
{
  for (const auto &plot : plotDatas_)
    if (plot.bbox.inside(p))
      plots.push_back(plot.plot);

  return ! plots.empty();
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

  auto createButton = [&](const QString &name, const QString &iconName,
                          const char *receiver) -> QToolButton * {
    QToolButton *button = new QToolButton(this);

    button->setObjectName(name);
    button->setIcon(CQPixmapCacheInst->getIcon(iconName));
    button->setCheckable(true);

    connect(button, SIGNAL(clicked(bool)), this, receiver);

    return button;
  };

  selectButton_ = createButton("select", "SELECT", SLOT(selectSlot(bool)));
  zoomButton_   = createButton("zoom"  , "ZOOM"  , SLOT(zoomSlot(bool)));
  probeButton_  = createButton("probe" , "PROBE" , SLOT(probeSlot(bool)));

  selectButton_->setChecked(true);

  layout->addWidget(selectButton_);
  layout->addWidget(zoomButton_  );
  layout->addWidget(probeButton_ );

  layout->addStretch(1);
}

void
CQChartsWindowToolBar::
selectSlot(bool)
{
  window_->setMode(CQChartsWindow::Mode::SELECT);

  zoomButton_  ->setChecked(false);
  probeButton_ ->setChecked(false);
  selectButton_->setChecked(true);
}

void
CQChartsWindowToolBar::
zoomSlot(bool)
{
  window_->setMode(CQChartsWindow::Mode::ZOOM);

  selectButton_->setChecked(false);
  probeButton_ ->setChecked(false);
  zoomButton_  ->setChecked(true);
}

void
CQChartsWindowToolBar::
probeSlot(bool)
{
  window_->setMode(CQChartsWindow::Mode::PROBE);

  probeButton_ ->setChecked(false);
  zoomButton_  ->setChecked(false);
  probeButton_ ->setChecked(true);
}

//---

CQChartsWindow::ProbeBand::
ProbeBand(CQChartsWindow *window) :
 window_(window)
{
  vband_ = new QRubberBand(QRubberBand::Line, window);
  hband_ = new QRubberBand(QRubberBand::Line, window);
  tip_   = new QLabel;

  tip_->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
}

CQChartsWindow::ProbeBand::
~ProbeBand()
{
  delete vband_;
  delete hband_;
  delete tip_;
}

void
CQChartsWindow::ProbeBand::
show(const QString &text, double px, double py1, double py2)
{
  int tickLen = 8;

  CPoint2D p1(px          , py1);
  CPoint2D p2(px          , py2);
  CPoint2D p3(px + tickLen, py2);

  vband_->setGeometry(CQUtil::toQRectI(CBBox2D(p1, p2)));
  hband_->setGeometry(CQUtil::toQRectI(CBBox2D(p2, p3)));

  vband_->show();
  hband_->show();

  tip_->setText(text);

  CPoint2D p4(px + tickLen + 2, py2 - tip_->sizeHint().height()/2);

  QPoint pos = window_->mapToGlobal(CQUtil::toQPointI(p4));

  tip_->move(pos);
  tip_->show();
}

void
CQChartsWindow::ProbeBand::
hide()
{
  vband_->hide();
  hband_->hide();
  tip_  ->hide();
}
