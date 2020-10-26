#include <CQChartsPreviewPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQCharts.h>

#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>

CQChartsPreviewPlot::
CQChartsPreviewPlot(CQCharts *charts) :
 charts_(charts)
{
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //--

  auto *controlFrame  = CQUtil::makeWidget<QFrame>("control");
  auto *controlLayout = CQUtil::makeLayout<QHBoxLayout>(controlFrame, 2, 2);

  layout->addWidget(controlFrame);

  //--

  enabledCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Enabled", "previewEnabled");

  enabledCheck_->setToolTip("Enable plot preview");

  connect(enabledCheck_, SIGNAL(stateChanged(int)), this, SLOT(enabledSlot()));

  controlLayout->addWidget(enabledCheck_);

  //--

  auto *fitButton = CQUtil::makeLabelWidget<QPushButton>("Fit", "fit");

  fitButton->setToolTip("Fit preview plot");

  connect(fitButton, SIGNAL(clicked()), this, SLOT(fitSlot()));

  controlLayout->addWidget(fitButton);

  //---

  controlLayout->addStretch(1);

  //--

  view_ = charts_->createView();

  view_->setPreview(true);

  layout->addWidget(view_);
}

bool
CQChartsPreviewPlot::
isEnabled() const
{
  return enabledCheck_->isChecked();
}

void
CQChartsPreviewPlot::
updatePlot(const ModelP &model, const PlotType *type, const QString &filter)
{
  if (plot_ && plot_->type() == type && plot_->model() == model)
    return;

  view_->removeAllPlots();

  plot_ = type->createAndInit(view_, model);

  plot_->setPreview(true);

  plot_->setId("Preview");

  if (filter != "")
    plot_->setFilterStr(filter);

  view_->addPlot(plot_);

  plot_->setAutoFit(true);
}

void
CQChartsPreviewPlot::
resetPlot()
{
  if (! plot_)
    return;

  view_->removeAllPlots();

  plot_ = nullptr;

  view_->update();
}

void
CQChartsPreviewPlot::
enabledSlot()
{
  emit enableStateChanged();
}

void
CQChartsPreviewPlot::
fitSlot()
{
  if (plot_)
    plot_->autoFit();
}
