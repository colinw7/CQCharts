#include <CQChartsGroupPlotCustomControls.h>
#include <CQChartsGroupPlot.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsWidgetUtil.h>
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
  groupFrame_ = createGroupFrame("Group", "groupFrame");

  //---

  groupColumnCombo_ = CQUtil::makeWidget<CQChartsColumnCombo>("groupColumnCombo");

  addFrameWidget(groupFrame_, "Column", groupColumnCombo_);

  //addFrameRowStretch(groupFrame_);
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
  connectSlots(false);

  if (groupColumnCombo_)
    groupColumnCombo_->setModelColumn(plot_->getModelData(), plot_->groupColumn());

  //---

  CQChartsPlotCustomControls::updateWidgets();

  connectSlots(true);
}

void
CQChartsGroupPlotCustomControls::
connectSlots(bool b)
{
  if (groupColumnCombo_)
    CQChartsWidgetUtil::connectDisconnect(b,
      groupColumnCombo_, SIGNAL(columnChanged()), this, SLOT(groupColumnSlot()));

  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsGroupPlotCustomControls::
groupColumnSlot()
{
  plot_->setGroupColumn(groupColumnCombo_->getColumn());
}
