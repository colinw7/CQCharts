#include <CQChartsHierPlotType.h>

CQChartsHierPlotType::
CQChartsHierPlotType()
{
}

void
CQChartsHierPlotType::
addParameters()
{
  addColumnParameter ("name" , "Name" , "nameColumn" , 0).setRequired();
  addColumnsParameter("names", "Names", "nameColumns");
  addColumnParameter ("value", "Value", "valueColumn");
  addColumnParameter ("color", "Color", "colorColumn").setTip("Custom color");

  addStringParameter("separator", "Separator", "separator", "/");

  CQChartsPlotType::addParameters();
}
