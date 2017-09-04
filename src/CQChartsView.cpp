#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQPropertyTree.h>
#include <CQGradientPalette.h>
#include <CQGradientPaletteControl.h>
#include <CQToolTip.h>
#include <CQUtil.h>

#include <svg/select_svg.h>
#include <svg/zoom_svg.h>
#include <svg/probe_svg.h>
#include <svg/zoom_fit_svg.h>

#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QRubberBand>
#include <QPainter>
#include <QMouseEvent>
#include <QPainter>

class CQChartsViewToolTip : public CQToolTipIFace {
 public:
  CQChartsViewToolTip(CQChartsView *view) :
   view_(view) {
  }

 ~CQChartsViewToolTip() {
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

    QPoint p = view_->mapFromGlobal(gpos);

    double wx, wy;

    view_->pixelToWindow(p.x(), p.y(), wx, wy);

    CQChartsView::Plots plots;

    view_->plotsAt(CPoint2D(wx, wy), plots);

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

  QSize sizeHint() const override {
    return widget_->sizeHint();
  }

 private:
  CQChartsView* view_   { nullptr };
  QLabel*       widget_ { nullptr };
};

//------

CQChartsView::
CQChartsView(QWidget *parent) :
 parent_(parent)
{
  setMouseTracking(true);

  setFocusPolicy(Qt::StrongFocus);

  //---

  displayRange_.setWindowRange(0, 0, 1000, 1000);

  //---

  expander_ = new CQChartsViewExpander(this);
  settings_ = new CQChartsViewSettings(this);
  status_   = new CQChartsViewStatus(this);
  toolbar_  = new CQChartsViewToolBar(this);

  //---

  addProperty("", this, "background");

  //---

  updateMargins();

  CQToolTip::setToolTip(this, new CQChartsViewToolTip(this));
}

CQChartsView::
~CQChartsView()
{
}

CQPropertyTree *
CQChartsView::
propertyTree() const
{
  return settings_->propertyTree();
}

void
CQChartsView::
updateMargins()
{
  statusHeight_  = status_ ->sizeHint().height();
  toolBarHeight_ = toolbar_->sizeHint().height();
}

void
CQChartsView::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  propertyTree()->addProperty(path, object, name, alias);
}

void
CQChartsView::
addPlot(CQChartsPlot *plot, const CBBox2D &bbox)
{
  plot->setPalette(settings_->palettePlot()->gradientPalette());

  plot->setBBox(bbox);

  plotDatas_.push_back(PlotData(plot, bbox));
}

void
CQChartsView::
mousePressEvent(QMouseEvent *me)
{
  if (me->button() == Qt::LeftButton) {
    mouseData_.pressPoint = me->pos();
    mouseData_.pressed    = true;

    if      (mode_ == Mode::SELECT) {
      CPoint2D w = pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())));

      mouseData_.plots.clear();

      plotsAt(w, mouseData_.plots);

      for (auto &plot : mouseData_.plots) {
        CPoint2D w;

        plot->pixelToWindow(CQUtil::fromQPoint(QPointF(me->pos())), w);

        if (plot->mousePress(w))
          break;
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
}

void
CQChartsView::
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
CQChartsView::
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
CQChartsView::
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
CQChartsView::
resizeEvent(QResizeEvent *)
{
  updateGeometry();

  for (const auto &plot : plotDatas_)
    plot.plot->handleResize();
}

void
CQChartsView::
updateGeometry()
{
  setMinimumSize(16 + expander_->width(), 16 + statusHeight_ + toolBarHeight_);

  //---

  prect_ = CBBox2D(0, toolBarHeight_, width() - expander_->width(), height() - statusHeight_);

  //---

  displayRange_.setPixelRange(prect_.getXMin(), prect_.getYMin(),
                              prect_.getXMax(), prect_.getYMax());

  settings_->setVisible(expander_->isExpanded());

  if (expander_->isExpanded()) {
    settings_->move  (width() - settings_->width(), 0);
    settings_->resize(settings_->width(), height());
  }

  //---

  expander_->setVisible(true);

  if (expander_->isExpanded())
    expander_->move(width() - settings_->width() - expander_->width(), 0);
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
CQChartsView::
moveExpander(int dx)
{
  expander_->move(expander_->x() - dx, expander_->y());

  settings_->resize(settings_->width() + dx, settings_->height());

  settings_->move(settings_->x() - dx, settings_->y());
}

void
CQChartsView::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  //---

  painter.fillRect(CQUtil::toQRect(prect_), QBrush(background()));

  //---

  for (const auto &plot : plotDatas_)
    plot.plot->draw(&painter);
}

CQChartsPlot *
CQChartsView::
plotAt(const CPoint2D &p) const
{
  Plots plots;

  if (! plotsAt(p, plots))
    return nullptr;

  return plots[0];
}

bool
CQChartsView::
plotsAt(const CPoint2D &p, Plots &plots) const
{
  for (const auto &plot : plotDatas_)
    if (plot.bbox.inside(p))
      plots.push_back(plot.plot);

  return ! plots.empty();
}

void
CQChartsView::
setStatusText(const QString &text)
{
  status_->setText(text);
}

void
CQChartsView::
windowToPixel(double wx, double wy, double &px, double &py) const
{
  displayRange_.windowToPixel(wx, wy, &px, &py);
}

void
CQChartsView::
pixelToWindow(double px, double py, double &wx, double &wy) const
{
  displayRange_.pixelToWindow(px, py, &wx, &wy);
}

CPoint2D
CQChartsView::
windowToPixel(const CPoint2D &w) const
{
  CPoint2D p;

  windowToPixel(w.x, w.y, p.x, p.y);

  return p;
}

CPoint2D
CQChartsView::
pixelToWindow(const CPoint2D &p) const
{
  CPoint2D w;

  pixelToWindow(p.x, p.y, w.x, w.y);

  return w;
}

CBBox2D
CQChartsView::
windowToPixel(const CBBox2D &wrect) const
{
  double px1, py1, px2, py2;

  windowToPixel(wrect.getXMin(), wrect.getYMin(), px1, py2);
  windowToPixel(wrect.getXMax(), wrect.getYMax(), px2, py1);

  return CBBox2D(px1, py1, px2, py2);
}

CBBox2D
CQChartsView::
pixelToWindow(const CBBox2D &prect) const
{
  double wx1, wy1, wx2, wy2;

  pixelToWindow(prect.getXMin(), prect.getYMin(), wx1, wy2);
  pixelToWindow(prect.getXMax(), prect.getYMax(), wx2, wy1);

  return CBBox2D(wx1, wy1, wx2, wy2);
}

QSize
CQChartsView::
sizeHint() const
{
  return QSize(1280, 1024);
}

//---

CQChartsViewExpander::
CQChartsViewExpander(CQChartsView *view) :
 QFrame(view), view_(view)
{
  setObjectName("expander");

  setAutoFillBackground(true);

  setFixedWidth(8);
}

void
CQChartsViewExpander::
mousePressEvent(QMouseEvent *me)
{
  if (me->button() == Qt::LeftButton) {
    int s  = width();
    int ym = height()/2;

    QRect handleRect(0, ym - s, width(), 2*s);

    if (handleRect.contains(me->pos())) {
      setExpanded(! isExpanded());

      view_->updateGeometry();

      return;
    }

    if (! isExpanded())
      return;

    pressed_  = true;
    pressPos_ = me->pos();
  }
}

void
CQChartsViewExpander::
mouseMoveEvent(QMouseEvent *me)
{
  if (pressed_) {
    movePos_ = me->pos();

    int dx = pressPos_.x() - movePos_.x();

    view_->moveExpander(dx);

    movePos_ = pressPos_;
  }
}

void
CQChartsViewExpander::
mouseReleaseEvent(QMouseEvent *me)
{
  mouseMoveEvent(me);

  pressed_ = false;
}

void
CQChartsViewExpander::
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

CQChartsViewStatus::
CQChartsViewStatus(CQChartsView *view) :
 QFrame(view), view_(view)
{
  setObjectName("status");
}

void
CQChartsViewStatus::
setText(const QString &s)
{
  text_ = s;

  update();
}

void
CQChartsViewStatus::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  QFontMetrics fm(font());

  p.drawText(2, 2 + fm.ascent(), text());
}

QSize
CQChartsViewStatus::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("XX"), fm.height() + 4);
}

//---

CQChartsViewSettings::
CQChartsViewSettings(CQChartsView *view) :
 QFrame(view), view_(view)
{
  setObjectName("settings");

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  tab_ = new QTabWidget;

  tab_->setObjectName("tab");

  layout->addWidget(tab_);

  tab_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  //---

  propertyTree_ = new CQPropertyTree(this);

  tab_->addTab(propertyTree_, "Properties");

  //---

  QFrame *paletteFrame = new QFrame;

  paletteFrame->setObjectName("palette");

  tab_->addTab(paletteFrame, "Palette");

  QVBoxLayout *paletteLayout = new QVBoxLayout(paletteFrame);

  palettePlot_    = new CQGradientPalette(this, nullptr);
  paletteControl_ = new CQGradientPaletteControl(palettePlot_);

  paletteLayout->addWidget(palettePlot_);
  paletteLayout->addWidget(paletteControl_);
}

//---

CQChartsViewToolBar::
CQChartsViewToolBar(CQChartsView *view) :
 QFrame(view), view_(view)
{
  setObjectName("toolbar");

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  auto createButton = [&](const QString &name, const QString &iconName,
                          const char *receiver, bool checkable=true) -> QToolButton * {
    QToolButton *button = new QToolButton(this);

    button->setObjectName(name);
    button->setIcon(CQPixmapCacheInst->getIcon(iconName));
    button->setCheckable(checkable);

    connect(button, SIGNAL(clicked(bool)), this, receiver);

    return button;
  };

  selectButton_  = createButton("select", "SELECT"  , SLOT(selectSlot(bool)));
  zoomButton_    = createButton("zoom"  , "ZOOM"    , SLOT(zoomSlot(bool)));
  probeButton_   = createButton("probe" , "PROBE"   , SLOT(probeSlot(bool)));
  autoFitButton_ = createButton("fit"   , "ZOOM_FIT", SLOT(autoFitSlot()), false);

  selectButton_->setChecked(true);

  layout->addWidget(selectButton_ );
  layout->addWidget(zoomButton_   );
  layout->addWidget(probeButton_  );
  layout->addWidget(autoFitButton_);

  layout->addStretch(1);
}

void
CQChartsViewToolBar::
selectSlot(bool)
{
  view_->setMode(CQChartsView::Mode::SELECT);

  zoomButton_  ->setChecked(false);
  probeButton_ ->setChecked(false);
  selectButton_->setChecked(true);
}

void
CQChartsViewToolBar::
zoomSlot(bool)
{
  view_->setMode(CQChartsView::Mode::ZOOM);

  selectButton_->setChecked(false);
  probeButton_ ->setChecked(false);
  zoomButton_  ->setChecked(true);
}

void
CQChartsViewToolBar::
probeSlot(bool)
{
  view_->setMode(CQChartsView::Mode::PROBE);

  probeButton_ ->setChecked(false);
  zoomButton_  ->setChecked(false);
  probeButton_ ->setChecked(true);
}

void
CQChartsViewToolBar::
autoFitSlot()
{
  for (int i = 0; i < view_->numPlots(); ++i) {
    view_->plot(i)->autoFit();
  }
}

QSize
CQChartsViewToolBar::
sizeHint() const
{
  int w = probeButton_->sizeHint().width ();
  int h = probeButton_->sizeHint().height();

  return QSize(4*w + 6, h);
}

//---

CQChartsView::ProbeBand::
ProbeBand(CQChartsView *view) :
 view_(view)
{
  vband_ = new QRubberBand(QRubberBand::Line, view);
  hband_ = new QRubberBand(QRubberBand::Line, view);
  tip_   = new QLabel;

  tip_->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
}

CQChartsView::ProbeBand::
~ProbeBand()
{
  delete vband_;
  delete hband_;
  delete tip_;
}

void
CQChartsView::ProbeBand::
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

  QPoint pos = view_->mapToGlobal(CQUtil::toQPointI(p4));

  tip_->move(pos);
  tip_->show();
}

void
CQChartsView::ProbeBand::
hide()
{
  vband_->hide();
  hband_->hide();
  tip_  ->hide();
}
