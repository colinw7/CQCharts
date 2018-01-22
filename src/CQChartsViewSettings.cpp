#include <CQChartsViewSettings.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsFilterEdit.h>
#include <CQPropertyViewTree.h>
#include <CQChartsGradientPaletteCanvas.h>
#include <CQChartsGradientPaletteControl.h>
#include <CQIconCombo.h>
#include <CQUtil.h>

#include <QTabWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QSplitter>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

CQChartsViewSettings::
CQChartsViewSettings(CQChartsWindow *window) :
 QFrame(window), window_(window)
{
  setObjectName("settings");

  setAutoFillBackground(true);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  tab_ = CQUtil::makeWidget<QTabWidget>("tab");

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

  QLabel *gradientLabel = new QLabel("Gradient");

  gradientCombo_ = new QComboBox;

  gradientCombo_->addItem("Palette");
  gradientCombo_->addItem("Theme");

  connect(gradientCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(gradientComboSlot(int)));

  paletteFrameLayout->addWidget(gradientLabel);
  paletteFrameLayout->addWidget(gradientCombo_);

  QLabel *spinLabel = new QLabel("Index");

  paletteSpin_ = new QSpinBox;

  int np = window_->view()->theme()->numPalettes();

  paletteSpin_->setRange(0, np);

  connect(paletteSpin_, SIGNAL(valueChanged(int)), this, SLOT(paletteIndexSlot(int)));

  paletteFrameLayout->addWidget(spinLabel);
  paletteFrameLayout->addWidget(paletteSpin_);

  QLabel *paletteNameLabel = new QLabel("Name");

  paletteCombo_ = new QComboBox;

  QStringList paletteNames;

  CQChartsThemeMgrInst->getPaletteNames(paletteNames);

  paletteCombo_->addItems(paletteNames);

  paletteFrameLayout->addWidget(paletteNameLabel);
  paletteFrameLayout->addWidget(paletteCombo_);

  QPushButton *paletteLoadButton = new QPushButton("Load");

  connect(paletteLoadButton, SIGNAL(clicked()), this, SLOT(loadPaletteNameSlot()));

  paletteFrameLayout->addWidget(paletteLoadButton);

  paletteFrameLayout->addStretch(1);

  paletteLayout->addWidget(paletteFrame);

  //---

  QSplitter *splitter = new QSplitter;

  splitter->setOrientation(Qt::Vertical);
  splitter->setObjectName("splitter");

  paletteLayout->addWidget(splitter);

  palettePlot_    = new CQChartsGradientPaletteCanvas(this, window_->view()->theme()->palette());
  paletteControl_ = new CQChartsGradientPaletteControl(palettePlot_);

  splitter->addWidget(palettePlot_);
  splitter->addWidget(paletteControl_);
}

CQChartsViewSettings::
~CQChartsViewSettings()
{
}

void
CQChartsViewSettings::
gradientComboSlot(int)
{
  updateGradientPalette();
}

void
CQChartsViewSettings::
paletteIndexSlot(int)
{
  updateGradientPalette();
}

void
CQChartsViewSettings::
loadPaletteNameSlot()
{
  QString name = paletteCombo_->currentText();

  CQChartsGradientPalette *palette = CQChartsThemeMgrInst->getNamedPalette(name);

  if (! palette)
    return;

  int i = paletteSpin_->value();

  window_->view()->theme()->setPalette(i, palette);

  updateGradientPalette();
}

void
CQChartsViewSettings::
updateGradientPalette()
{
  int ind = gradientCombo_->currentIndex();

  if (ind == 0) {
    paletteSpin_->setEnabled(true);

    int i = paletteSpin_->value();

    CQChartsGradientPalette *palette = window_->view()->theme()->palette(i);

    palettePlot_->setGradientPalette(palette);

    paletteCombo_->setCurrentText(palette->name());
  }
  else {
    palettePlot_->setGradientPalette(window_->view()->theme()->theme());

    paletteSpin_->setEnabled(false);

    paletteCombo_->setCurrentIndex(-1);
  }

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
