#include <CQChartsBarPlot.h>

CQChartsBarPlot::
CQChartsBarPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model) :
 CQChartsGroupPlot(view, plotType, model),
 CQChartsPlotBarShapeData<CQChartsBarPlot>(this)
{
  (void) addColorSet("color");

  setBarFilled   (true);
  setBarFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setBarBorder(true);

  //---

  setLayerActive(CQChartsLayer::Type::FG_PLOT, true);

  addAxes();

  addKey();

  addTitle();
}

CQChartsBarPlot::
~CQChartsBarPlot()
{
}

//---

void
CQChartsBarPlot::
setValueColumn(const CQChartsColumn &c)
{
  if (c != valueColumns_.column()) {
    valueColumns_.setColumn(c);

    updateRangeAndObjs();
  }
}

void
CQChartsBarPlot::
setValueColumns(const Columns &cols)
{
  if (cols != valueColumns_.columns()) {
    valueColumns_.setColumns(cols);

    updateRangeAndObjs();
  }
}

QString
CQChartsBarPlot::
valueColumnsStr() const
{
  return valueColumns_.columnsStr();
}

bool
CQChartsBarPlot::
setValueColumnsStr(const QString &s)
{
  bool rc = true;

  if (s != valueColumnsStr()) {
    rc = valueColumns_.setColumnsStr(s);

    updateRangeAndObjs();
  }

  return rc;
}

const CQChartsColumn &
CQChartsBarPlot::
valueColumnAt(int i)
{
  return valueColumns_.getColumn(i);
}

int
CQChartsBarPlot::
numValueColumns() const
{
  return valueColumns_.count();
}

//---

void
CQChartsBarPlot::
addProperties()
{
  addProperty("columns", this, "valueColumns", "values");
  addProperty("columns", this, "colorColumn" , "color" );

  addProperty("options", this, "horizontal");

  addProperty("margins", this, "margin"     , "bar"  );
  addProperty("margins", this, "groupMargin", "group");

  addProperty("stroke", this, "barBorder", "visible");

  addLineProperties("stroke", "barBorder");

  addProperty("stroke", this, "cornerSize", "cornerSize");

  addProperty("fill", this, "barFilled", "visible");

  addFillProperties("fill", "barFill");

  addColorMapProperties();
}

//---

void
CQChartsBarPlot::
setHorizontal(bool b)
{
  CQChartsUtil::testAndSet(horizontal_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBarPlot::
setMargin(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(margin_, l, [&]() { invalidateLayers(); } );
}

void
CQChartsBarPlot::
setGroupMargin(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(groupMargin_, l, [&]() { invalidateLayers(); } );
}

//---

const CQChartsLength &
CQChartsBarPlot::
cornerSize() const
{
  return cornerSize_;
}

void
CQChartsBarPlot::
setCornerSize(const CQChartsLength &s)
{
  CQChartsUtil::testAndSet(cornerSize_, s, [&]() { invalidateLayers(); } );
}

//---

bool
CQChartsBarPlot::
probe(ProbeData &probeData) const
{
  if (! dataRange_.isSet())
    return false;

  if (! isHorizontal()) {
    probeData.direction = ProbeData::Direction::VERTICAL;

    if (probeData.x < dataRange_.xmin() + 0.5)
      probeData.x = dataRange_.xmin() + 0.5;

    if (probeData.x > dataRange_.xmax() - 0.5)
      probeData.x = dataRange_.xmax() - 0.5;

    probeData.x = std::round(probeData.x);
  }
  else {
    probeData.direction = ProbeData::Direction::HORIZONTAL;

    if (probeData.y < dataRange_.ymin() + 0.5)
      probeData.y = dataRange_.ymin() + 0.5;

    if (probeData.y > dataRange_.ymax() - 0.5)
      probeData.y = dataRange_.ymax() - 0.5;

    probeData.y = std::round(probeData.y);
  }

  return true;
}
