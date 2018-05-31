#include <CQChartsHierPlot.h>

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
  addColumnParameter ("color", "Color", "colorColumn");

  addStringParameter("separator", "Separator", "separator", "/");

  CQChartsPlotType::addParameters();
}

//------

CQChartsHierPlot::
CQChartsHierPlot(CQChartsView *view, CQChartsPlotType *type, const ModelP &model) :
 CQChartsPlot(view, type, model)
{
  (void) addColorSet("color");

  nameColumns_.push_back(nameColumn_);
}

CQChartsHierPlot::
~CQChartsHierPlot()
{
}

//----

void
CQChartsHierPlot::
setNameColumn(const CQChartsColumn &c)
{
  if (c != nameColumn_) {
    nameColumn_ = c;

    nameColumns_.clear();

    if (nameColumn_.isValid())
      nameColumns_.push_back(nameColumn_);

    updateRangeAndObjs();
  }
}

void
CQChartsHierPlot::
setNameColumns(const Columns &nameColumns)
{
  nameColumns_ = nameColumns;

  if (! nameColumns_.empty())
    nameColumn_ = nameColumns_[0];
  else
    nameColumn_ = -1;

  updateRangeAndObjs();
}

QString
CQChartsHierPlot::
nameColumnsStr() const
{
  return CQChartsColumn::columnsToString(nameColumns_);
}

bool
CQChartsHierPlot::
setNameColumnsStr(const QString &s)
{
  if (s != nameColumnsStr()) {
    Columns nameColumns;

    if (! CQChartsColumn::stringToColumns(s, nameColumns))
      return false;

    setNameColumns(nameColumns);
  }

  return true;
}

void
CQChartsHierPlot::
setValueColumn(const CQChartsColumn &c)
{
  if (c != valueColumn_) {
    valueColumn_ = c;

    updateRangeAndObjs();
  }
}

void
CQChartsHierPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nameColumn" , "name" );
  addProperty("columns", this, "nameColumns", "names");
  addProperty("columns", this, "valueColumn", "value");
  addProperty("columns", this, "colorColumn", "color");

  addProperty("", this, "separator");

  addProperty("color", this, "colorMapped", "mapped");
  addProperty("color", this, "colorMapMin", "mapMin");
  addProperty("color", this, "colorMapMax", "mapMax");
}
