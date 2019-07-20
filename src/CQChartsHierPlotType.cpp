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

  addColumnsParameter("name", "Name", "nameColumns").
   setRequired().setString().setTip("Hierarchical path columns for data");

  addColumnParameter ("value", "Value", "valueColumn").
   setNumeric().setTip("Data value column");

  addStringParameter("separator", "Separator", "separator", "/").
   setTip("Separator for hierarchical path in name column");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}
