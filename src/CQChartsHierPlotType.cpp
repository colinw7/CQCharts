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

  addColumnsParameter("names", "Names", "nameColumns", "0").
   setRequired().setString().setTip("Hierarchical path for data");

  addColumnParameter ("value", "Value", "valueColumn").
   setNumeric().setTip("Data value column");

  addColumnParameter ("color", "Color", "colorColumn").
   setColor().setTip("Custom color");

  addStringParameter("separator", "Separator", "separator", "/").
   setTip("Separator for hierarchical path");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}
