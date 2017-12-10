#include <CQChartsViewSettings.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsFilterEdit.h>
#include <CQPropertyViewTree.h>
#include <CQGradientControlPlot.h>
#include <CQGradientControlIFace.h>
#include <CQIconCombo.h>

#include <QTabWidget>
#include <QLineEdit>
#include <QSplitter>
#include <QVBoxLayout>

CQChartsViewSettings::
CQChartsViewSettings(CQChartsWindow *window) :
 QFrame(window), window_(window)
{
  setObjectName("settings");

  setAutoFillBackground(true);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  tab_ = new QTabWidget;

  tab_->setObjectName("tab");

  layout->addWidget(tab_);

  tab_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  //---

  QFrame *viewFrame = new QFrame;

  viewFrame->setObjectName("viewFrame");

  QVBoxLayout *viewLayout = new QVBoxLayout(viewFrame);
  viewLayout->setMargin(0); viewLayout->setSpacing(2);

  filterEdit_ = new CQChartsFilterEdit;

  connect(filterEdit_, SIGNAL(replaceFilter(const QString &)),
          this, SLOT(replaceFilterSlot(const QString &)));
  connect(filterEdit_, SIGNAL(addFilter(const QString &)),
          this, SLOT(addFilterSlot(const QString &)));

  connect(filterEdit_, SIGNAL(replaceSearch(const QString &)),
          this, SLOT(replaceSearchSlot(const QString &)));
  connect(filterEdit_, SIGNAL(addSearch(const QString &)),
          this, SLOT(addSearchSlot(const QString &)));

  viewLayout->addWidget(filterEdit_);

  propertyTree_ = new CQPropertyViewTree(this, window->view()->propertyModel());

  viewLayout->addWidget(propertyTree_);

  tab_->addTab(viewFrame, "Properties");

  //---

  QFrame *paletteFrame = new QFrame;

  paletteFrame->setObjectName("palette");

  tab_->addTab(paletteFrame, "Palette");

  QVBoxLayout *paletteLayout = new QVBoxLayout(paletteFrame);

  QSplitter *splitter = new QSplitter;

  splitter->setOrientation(Qt::Vertical);
  splitter->setObjectName("splitter");

  paletteLayout->addWidget(splitter);

  palettePlot_    = new CQGradientControlPlot(this, window->view()->gradientPalette());
  paletteControl_ = new CQGradientControlIFace(palettePlot_);

  splitter->addWidget(palettePlot_);
  splitter->addWidget(paletteControl_);
}

CQChartsViewSettings::
~CQChartsViewSettings()
{
}

void
CQChartsViewSettings::
replaceFilterSlot(const QString &text)
{
  propertyTree()->setFilter(text);
}

void
CQChartsViewSettings::
addFilterSlot(const QString &text)
{
  //propertyTree()->addFilter(text);
  propertyTree()->setFilter(text);
}

void
CQChartsViewSettings::
replaceSearchSlot(const QString &text)
{
  propertyTree()->search(text);
}

void
CQChartsViewSettings::
addSearchSlot(const QString &text)
{
  //propertyTree()->addSearch(text);
  propertyTree()->search(text);
}
