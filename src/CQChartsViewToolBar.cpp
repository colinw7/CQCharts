#include <CQChartsViewToolBar.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQPixmapCache.h>

#include <QToolButton>
#include <QStackedWidget>
#include <QHBoxLayout>

CQChartsViewToolBar::
CQChartsViewToolBar(CQChartsView *view) :
 QFrame(view), view_(view)
{
  setObjectName("toolbar");

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

  selectButton_->setChecked(true);

  buttonsLayout->addWidget(selectButton_ );
  buttonsLayout->addWidget(zoomButton_   );
  buttonsLayout->addWidget(probeButton_  );
  buttonsLayout->addWidget(autoFitButton_);

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
  view_->setMode(CQChartsView::Mode::SELECT);
}

void
CQChartsViewToolBar::
zoomSlot(bool)
{
  view_->setMode(CQChartsView::Mode::ZOOM);

  updateMode();
}

void
CQChartsViewToolBar::
probeSlot(bool)
{
  view_->setMode(CQChartsView::Mode::PROBE);

  updateMode();
}

void
CQChartsViewToolBar::
updateMode()
{
  if      (view_->mode() == CQChartsView::Mode::SELECT)
    controlsStack_->setCurrentIndex(0);
  else if (view_->mode() == CQChartsView::Mode::ZOOM)
    controlsStack_->setCurrentIndex(1);
  else if (view_->mode() == CQChartsView::Mode::PROBE)
    controlsStack_->setCurrentIndex(2);

  selectButton_->setChecked(view_->mode() == CQChartsView::Mode::SELECT);
  zoomButton_  ->setChecked(view_->mode() == CQChartsView::Mode::ZOOM);
  probeButton_ ->setChecked(view_->mode() == CQChartsView::Mode::PROBE);
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
