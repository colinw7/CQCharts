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
#include <QLabel>
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

  propertyTree_ = new CQPropertyViewTree(this, window_->view()->propertyModel());

  viewLayout->addWidget(propertyTree_);

  tab_->addTab(viewFrame, "Properties");

  //------

  QFrame *themeFrame = new QFrame;

  themeFrame->setObjectName("palette");

  tab_->addTab(themeFrame, "Theme");

  QVBoxLayout *paletteLayout = new QVBoxLayout(themeFrame);

  //---

#if 0
  QFrame *themeColorsFrame = new QFrame;

  themeColorsFrame->setObjectName("themeColorsFrame");

  QGridLayout *themeColorsLayout = new QGridLayout(themeColorsFrame);

  QLabel    *selColorLabel = new QLabel("Selection");
  QLineEdit *selColorEdit  = new QLineEdit;

  themeColorsLayout->addWidget(selColorLabel, 0, 0);
  themeColorsLayout->addWidget(selColorEdit , 0, 1);

  paletteLayout->addWidget(themeColorsFrame);
#endif

  QFrame *paletteFrame = new QFrame;

  paletteFrame->setObjectName("paletteFrame");

  QHBoxLayout *paletteFrameLayout = new QHBoxLayout(paletteFrame);

  QLabel    *gradientLabel = new QLabel("Gradient");
  QComboBox *gradientCombo = new QComboBox;

  gradientCombo->addItem("Palette");
  gradientCombo->addItem("Theme");

  connect(gradientCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(gradientComboSlot(int)));

  paletteFrameLayout->addWidget(gradientLabel);
  paletteFrameLayout->addWidget(gradientCombo);
  paletteFrameLayout->addStretch(1);

  paletteLayout->addWidget(paletteFrame);

  //---

  QSplitter *splitter = new QSplitter;

  splitter->setOrientation(Qt::Vertical);
  splitter->setObjectName("splitter");

  paletteLayout->addWidget(splitter);

  palettePlot_    = new CQGradientControlPlot(this, window_->view()->theme()->palette());
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
gradientComboSlot(int ind)
{
  if (ind == 0)
    palettePlot_->setGradientPalette(window_->view()->theme()->palette());
  else
    palettePlot_->setGradientPalette(window_->view()->theme()->theme());

  paletteControl_->updateState();
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
