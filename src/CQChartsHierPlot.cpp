#include <CQChartsHierPlot.h>

CQChartsHierPlot::
CQChartsHierPlot(CQChartsView *view, CQChartsPlotType *type, const ModelP &model) :
 CQChartsPlot(view, type, model)
{
}

CQChartsHierPlot::
~CQChartsHierPlot()
{
}

//----

void
CQChartsHierPlot::
setNameColumns(const CQChartsColumns &c)
{
  CQChartsUtil::testAndSet(nameColumns_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsHierPlot::
setValueColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsHierPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nameColumns", "names");
  addProperty("columns", this, "valueColumn", "value");

  addProperty("options", this, "separator");

  addColorMapProperties();
}
