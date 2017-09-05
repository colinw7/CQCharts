#include <CQChartsViewSettings.h>
#include <CQChartsView.h>
#include <CQPropertyTree.h>
#include <CQGradientPalette.h>
#include <CQGradientPaletteControl.h>
#include <QTabWidget>
#include <QVBoxLayout>

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
