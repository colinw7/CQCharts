#include <CQChartsViewToolBar.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQPixmapCache.h>
#include <QToolButton>
#include <QHBoxLayout>

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
