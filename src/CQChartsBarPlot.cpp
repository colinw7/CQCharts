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
  CQChartsUtil::testAndSet(valueColumns_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsBarPlot::
addProperties()
{
  // columns
  addProperty("columns", this, "valueColumns", "values");

  // options
  addProperty("options", this, "horizontal");

  // margins
  addProperty("margins", this, "margin"     , "bar"  );
  addProperty("margins", this, "groupMargin", "group");

  // fill
  addProperty("fill", this, "barFilled", "visible");

  addFillProperties("fill", "barFill");

  // stroke
  addProperty("stroke", this, "barBorder", "visible");

  addLineProperties("stroke", "barBorder");

  addProperty("stroke", this, "barCornerSize", "cornerSize");

  // color map
  addColorMapProperties();
}

//---

void
CQChartsBarPlot::
setHorizontal(bool b)
{
  CQChartsUtil::testAndSet(horizontal_, b, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsBarPlot::
setMargin(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(margin_, l, [&]() { queueDrawObjs(); } );
}

void
CQChartsBarPlot::
setGroupMargin(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(groupMargin_, l, [&]() { queueDrawObjs(); } );
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
    probeData.direction = Qt::Vertical;

    if (probeData.x < dataRange.xmin() + 0.5)
      probeData.x = dataRange.xmin() + 0.5;

    if (probeData.x > dataRange.xmax() - 0.5)
      probeData.x = dataRange.xmax() - 0.5;

    probeData.x = std::round(probeData.x);
  }
  else {
    probeData.direction = Qt::Horizontal;

    if (probeData.y < dataRange.ymin() + 0.5)
      probeData.y = dataRange.ymin() + 0.5;

    if (probeData.y > dataRange.ymax() - 0.5)
      probeData.y = dataRange.ymax() - 0.5;

    probeData.y = std::round(probeData.y);
  }

  return true;
}
