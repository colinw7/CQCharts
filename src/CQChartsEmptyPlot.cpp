#include <CQChartsEmptyPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>

#include <QMenu>

CQChartsEmptyPlotType::
CQChartsEmptyPlotType()
{
}

void
CQChartsEmptyPlotType::
addParameters()
{
  CQChartsPlotType::addParameters();
}

QString
CQChartsEmptyPlotType::
description() const
{
  return CQChartsHtml().
   h2("Empty Plot").
    h3("Summary").
     p("Empty plot which can used to display custom data using annotations.").
    h3("Limitations").
     p("None.");
}

CQChartsPlot *
CQChartsEmptyPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsEmptyPlot(view, model);
}

//------

CQChartsEmptyPlot::
CQChartsEmptyPlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("empty"), model)
{
}

CQChartsEmptyPlot::
~CQChartsEmptyPlot()
{
  CQChartsEmptyPlot::term();
}

//---

void
CQChartsEmptyPlot::
init()
{
  CQChartsPlot::init();

  //---

  addAxes();

  addTitle();

  xAxis()->setVisible(false);
  yAxis()->setVisible(false);

  //---

  setOuterMargin(PlotMargin(Length::plot(0), Length::plot(0), Length::plot(0), Length::plot(0)));
}

void
CQChartsEmptyPlot::
term()
{
}

//---

void
CQChartsEmptyPlot::
addProperties()
{
  addBaseProperties();
}

CQChartsGeom::Range
CQChartsEmptyPlot::
calcRange() const
{
  Range dataRange;

  dataRange.updateRange(0.0, 0.0);
  dataRange.updateRange(1.0, 1.0);

  return dataRange;
}

bool
CQChartsEmptyPlot::
createObjs(PlotObjs &) const
{
  return true;
}

//------

bool
CQChartsEmptyPlot::
addMenuItems(QMenu *, const Point &)
{
  return true;
}

//---

CQChartsPlotCustomControls *
CQChartsEmptyPlot::
createCustomControls()
{
  auto *controls = new CQChartsEmptyPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//---

CQChartsEmptyPlotCustomControls::
CQChartsEmptyPlotCustomControls(CQCharts *charts) :
 CQChartsPlotCustomControls(charts, "empty")
{
}

void
CQChartsEmptyPlotCustomControls::
init()
{
  connectSlots(true);
}

void
CQChartsEmptyPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_ && emptyPlot_)
    disconnect(emptyPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  emptyPlot_ = dynamic_cast<CQChartsEmptyPlot *>(plot);

  CQChartsPlotCustomControls::setPlot(plot);

#ifdef CQCHARTS_MODULE_SHLIB
  addModuleWidgets();
#endif

  if (emptyPlot_)
    connect(emptyPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  addLayoutStretch();
}
