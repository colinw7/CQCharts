#include <CQChartsHierPlotType.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>

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
   setRequired().setStringColumn().setPropPath("columns.name").
   setTip("Hierarchical path columns for data");

  addColumnParameter("value", "Value", "valueColumn").setBasic().
   setNumericColumn().setPropPath("columns.value").setTip("Data value column");

  addColumnParameter("group", "Group", "groupColumn").setBasic().
   setStringColumn().setPropPath("columns.group").setTip("Group column");

  addStringParameter("separator", "Separator", "separator", "/").setBasic().
   setTip("Separator for hierarchical path in name column");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

void
CQChartsHierPlotType::
analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData)
{
  bool hasValue = (analyzeModelData.parameterNameColumn.find("value") !=
                   analyzeModelData.parameterNameColumn.end());

  if (hasValue)
    return;

  auto *details = modelData->details();
  if (! details) return;

  CQChartsColumn valueColumn;

  int nc = details->numColumns();

  for (int c = 0; c < nc; ++c) {
    auto *columnDetails = details->columnDetails(CQChartsColumn(c));
    if (! columnDetails) continue;

    if (columnDetails->isNumeric()) {
      if (! valueColumn.isValid())
        valueColumn = columnDetails->column();
    }
  }

  if (valueColumn.isValid())
    analyzeModelData.parameterNameColumn["value"] = valueColumn;
}
