#include <CQChartsViewSettings.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQPropertyViewTree.h>
#include <CQGradientControlPlot.h>
#include <CQGradientControlIFace.h>
#include <CQIconCombo.h>

#include <QTabWidget>
#include <QLineEdit>
#include <QSplitter>
#include <QVBoxLayout>

#include <svg/filter_svg.h>
#include <svg/search_svg.h>

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

  QFrame *filterFrame = new QFrame;

  filterFrame->setObjectName("filterFrame");

  QHBoxLayout *filterLayout = new QHBoxLayout(filterFrame);
  filterLayout->setMargin(0); filterLayout->setSpacing(2);

  filterEdit_ = new QLineEdit;

  connect(filterEdit_, SIGNAL(returnPressed()), this, SLOT(filterSlot()));

  filterLayout->addWidget(filterEdit_);

  filterCombo_ = new CQIconCombo;

  filterCombo_->setObjectName("filterCombo");

  filterCombo_->addItem(CQPixmapCacheInst->getIcon("FILTER"), "Filter");
  filterCombo_->addItem(CQPixmapCacheInst->getIcon("SEARCH"), "Search");

  filterLayout->addWidget(filterCombo_);

  viewLayout->addWidget(filterFrame);

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
filterSlot()
{
  QLineEdit *edit = qobject_cast<QLineEdit *>(sender());
  if (! edit) return;

  if (filterCombo_->currentIndex() == 0)
    propertyTree()->setFilter(edit->text());
  else
    propertyTree()->search(edit->text());
}
