#include <CQChartsHierPlotType.h>

CQChartsHierPlotType::
CQChartsHierPlotType()
{
}

void
CQChartsHierPlotType::
addParameters()
{
  startParameterGroup("Hierarchy");

  addColumnsParameter("names", "Names", "nameColumns").
   setRequired().setString().setTip("Hierarchical path for data");

  addColumnParameter ("value", "Value", "valueColumn").
   setNumeric().setTip("Data value column");

  addStringParameter("separator", "Separator", "separator", "/").
   setTip("Separator for hierarchical path");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}
