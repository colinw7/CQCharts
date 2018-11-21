#include <CQChartsBarPlot.h>

CQChartsBarPlot::
CQChartsBarPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model) :
 CQChartsGroupPlot(view, plotType, model),
 CQChartsObjBarShapeData<CQChartsBarPlot>(this)
{
  NoUpdate noUpdate(this);

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
setValueColumns(const CQChartsColumns &c)
{
  CQChartsUtil::testAndSet(valueColumns_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBarPlot::
addProperties()
{
  addProperty("columns", this, "valueColumns", "values");

  addProperty("options", this, "horizontal");

  addProperty("margins", this, "margin"     , "bar"  );
  addProperty("margins", this, "groupMargin", "group");

  addProperty("stroke", this, "barBorder", "visible");

  addLineProperties("stroke", "barBorder");

  addProperty("stroke", this, "barCornerSize", "cornerSize");

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

bool
CQChartsBarPlot::
probe(ProbeData &probeData) const
{
  const CQChartsGeom::Range &dataRange = this->dataRange();

  if (! dataRange.isSet())
    return false;

  if (! isHorizontal()) {
    probeData.direction = ProbeData::Direction::VERTICAL;

    if (probeData.x < dataRange.xmin() + 0.5)
      probeData.x = dataRange.xmin() + 0.5;

    if (probeData.x > dataRange.xmax() - 0.5)
      probeData.x = dataRange.xmax() - 0.5;

    probeData.x = std::round(probeData.x);
  }
  else {
    probeData.direction = ProbeData::Direction::HORIZONTAL;

    if (probeData.y < dataRange.ymin() + 0.5)
      probeData.y = dataRange.ymin() + 0.5;

    if (probeData.y > dataRange.ymax() - 0.5)
      probeData.y = dataRange.ymax() - 0.5;

    probeData.y = std::round(probeData.y);
  }

  return true;
}
