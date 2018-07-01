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

  addColumnParameter ("name" , "Name" , "nameColumn").setTip("Data name");
  addColumnsParameter("names", "Names", "nameColumns", "0").
    setRequired().setTip("Hierarchical path for data");
  addColumnParameter ("value", "Value", "valueColumn").setTip("Data value");
  addColumnParameter ("color", "Color", "colorColumn").setTip("Custom color");

  addStringParameter("separator", "Separator", "separator", "/").
   setTip("Separator for hierarchical path");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}
