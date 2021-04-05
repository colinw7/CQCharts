#include <CQChartsGroupPlotCustomControls.h>
#include <CQChartsGroupPlot.h>
#include <CQChartsColumnCombo.h>
#include <CQUtil.h>

CQChartsGroupPlotCustomControls::
CQChartsGroupPlotCustomControls(CQCharts *charts, const QString &plotType) :
 CQChartsPlotCustomControls(charts, plotType)
{
}

void
CQChartsGroupPlotCustomControls::
addGroupColumnWidgets()
{
  // group group
  auto groupFrame = createGroupFrame("Group");

  //---

  groupColumnCombo_ = CQUtil::makeWidget<CQChartsColumnCombo>("groupColumnCombo");

  addFrameWidget(groupFrame, "Column", groupColumnCombo_);

  connect(groupColumnCombo_, SIGNAL(columnChanged()), this, SLOT(groupColumnSlot()));

  addFrameRowStretch(groupFrame);
}

void
CQChartsGroupPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  plot_ = dynamic_cast<CQChartsGroupPlot *>(plot);

  CQChartsPlotCustomControls::setPlot(plot);
}

void
CQChartsGroupPlotCustomControls::
updateWidgets()
{
  if (groupColumnCombo_)
    groupColumnCombo_->setModelColumn(plot_->getModelData(), plot_->groupColumn());

  //---

  CQChartsPlotCustomControls::updateWidgets();
}

void
CQChartsGroupPlotCustomControls::
groupColumnSlot()
{
  plot_->setGroupColumn(groupColumnCombo_->getColumn());
}
