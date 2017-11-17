#include <CQChartsViewToolBar.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQPixmapCache.h>

#include <svg/select_svg.h>
#include <svg/zoom_svg.h>
#include <svg/probe_svg.h>
#include <svg/zoom_fit_svg.h>
#include <svg/left_svg.h>
#include <svg/right_svg.h>

#include <QToolButton>
#include <QStackedWidget>
#include <QHBoxLayout>

CQChartsViewToolBar::
CQChartsViewToolBar(CQChartsWindow *window) :
 QFrame(window), window_(window)
{
  setObjectName("toolbar");

  setAutoFillBackground(true);

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  //---

  auto createButton = [&](const QString &name, const QString &iconName,
                          const char *receiver, bool checkable=true) -> QToolButton * {
    QToolButton *button = new QToolButton(this);

    button->setObjectName(name);
    button->setIcon(CQPixmapCacheInst->getIcon(iconName));
    button->setCheckable(checkable);

    button->setFocusPolicy(Qt::NoFocus);

    connect(button, SIGNAL(clicked(bool)), this, receiver);

    return button;
  };

  //---

  QFrame *buttonsFrame = new QFrame;

  buttonsFrame ->setObjectName("buttons");

  layout->addWidget(buttonsFrame);

  QHBoxLayout *buttonsLayout = new QHBoxLayout(buttonsFrame);
  buttonsLayout->setMargin(0); buttonsLayout->setSpacing(2);

  selectButton_  = createButton("select", "SELECT"  , SLOT(selectSlot(bool)));
  zoomButton_    = createButton("zoom"  , "ZOOM"    , SLOT(zoomSlot(bool)));
  probeButton_   = createButton("probe" , "PROBE"   , SLOT(probeSlot(bool)));
  autoFitButton_ = createButton("fit"   , "ZOOM_FIT", SLOT(autoFitSlot()), false);
  leftButton_    = createButton("left"  , "LEFT"    , SLOT(leftSlot()));
  rightButton_   = createButton("right" , "RIGHT"   , SLOT(rightSlot()));

  selectButton_->setChecked(true);

  buttonsLayout->addWidget(selectButton_ );
  buttonsLayout->addWidget(zoomButton_   );
  buttonsLayout->addWidget(probeButton_  );
  buttonsLayout->addWidget(autoFitButton_);
  buttonsLayout->addWidget(leftButton_);
  buttonsLayout->addWidget(rightButton_);

  //---

  controlsStack_ = new QStackedWidget;

  controlsStack_->setObjectName("controls");

  controlsStack_->layout()->setMargin(0); controlsStack_->layout()->setSpacing(0);

  layout->addWidget(controlsStack_);

  QFrame *selectControls = new QFrame;
  QFrame *zoomControl    = new QFrame;
  QFrame *probeControl   = new QFrame;

  selectControls->setObjectName("select");
  zoomControl   ->setObjectName("zoom");
  probeControl  ->setObjectName("probe");

  controlsStack_->addWidget(selectControls);
  controlsStack_->addWidget(zoomControl);
  controlsStack_->addWidget(probeControl);

  //---

  layout->addStretch(1);
}

void
CQChartsViewToolBar::
selectSlot(bool)
{
  window_->view()->setMode(CQChartsView::Mode::SELECT);
}

void
CQChartsViewToolBar::
zoomSlot(bool)
{
  window_->view()->setMode(CQChartsView::Mode::ZOOM);

  updateMode();
}

void
CQChartsViewToolBar::
probeSlot(bool)
{
  window_->view()->setMode(CQChartsView::Mode::PROBE);

  updateMode();
}

void
CQChartsViewToolBar::
updateMode()
{
  if      (window_->view()->mode() == CQChartsView::Mode::SELECT)
    controlsStack_->setCurrentIndex(0);
  else if (window_->view()->mode() == CQChartsView::Mode::ZOOM)
    controlsStack_->setCurrentIndex(1);
  else if (window_->view()->mode() == CQChartsView::Mode::PROBE)
    controlsStack_->setCurrentIndex(2);

  selectButton_->setChecked(window_->view()->mode() == CQChartsView::Mode::SELECT);
  zoomButton_  ->setChecked(window_->view()->mode() == CQChartsView::Mode::ZOOM);
  probeButton_ ->setChecked(window_->view()->mode() == CQChartsView::Mode::PROBE);
}

void
CQChartsViewToolBar::
autoFitSlot()
{
  window_->view()->fitSlot();
}

void
CQChartsViewToolBar::
leftSlot()
{
  window_->view()->scrollLeft();
}

void
CQChartsViewToolBar::
rightSlot()
{
  window_->view()->scrollRight();
}

QSize
CQChartsViewToolBar::
sizeHint() const
{
  int w = probeButton_->sizeHint().width ();
  int h = probeButton_->sizeHint().height();

  return QSize(4*w + 6, h);
}
