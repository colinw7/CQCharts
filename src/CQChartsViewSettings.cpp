#include <CQChartsViewSettings.h>
#include <CQChartsView.h>
#include <CQPropertyView.h>
#include <CQGradientPalette.h>
#include <CQGradientPaletteControl.h>
#include <QTabWidget>
#include <QLineEdit>
#include <QVBoxLayout>

#include <svg/filter_svg.h>
#include <svg/search_svg.h>

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

  filterCombo_ = new QComboBox;

  filterCombo_->setObjectName("filterCombo");

  filterCombo_->addItem(CQPixmapCacheInst->getIcon("FILTER"), "Filter");
  filterCombo_->addItem(CQPixmapCacheInst->getIcon("SEARCH"), "Search");

  filterLayout->addWidget(filterCombo_);

  viewLayout->addWidget(filterFrame);

  propertyView_ = new CQPropertyView(this);

  viewLayout->addWidget(propertyView_);

  tab_->addTab(viewFrame, "Properties");

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

void
CQChartsViewSettings::
filterSlot()
{
  QLineEdit *edit = qobject_cast<QLineEdit *>(sender());
  if (! edit) return;

  if (filterCombo_->currentIndex() == 0)
    view_->propertyView()->setFilter(edit->text());
  else
    view_->propertyView()->search(edit->text());
}
