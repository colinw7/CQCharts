#include <CQChartsEmptyPlot.h>
#include <CQChartsView.h>
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
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Empty Plot").
    h3("Summary").
     p("Empty plot.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/empty_plot.png"));
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
  term();
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
addMenuItems(QMenu *)
{
  return true;
}
