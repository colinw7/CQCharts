#include <CQChartsHierPlot.h>

CQChartsHierPlot::
CQChartsHierPlot(CQChartsView *view, CQChartsPlotType *type, const ModelP &model) :
 CQChartsPlot(view, type, model)
{
  (void) addColorSet("color");
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
  CQChartsUtil::testAndSet(nameColumns_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsHierPlot::
setValueColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
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
