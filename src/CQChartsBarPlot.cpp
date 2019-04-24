#include <CQChartsBarPlot.h>
#include <CQPropertyViewItem.h>

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
  // columns
  addProperty("columns", this, "valueColumns", "values")->setDesc("Value columns");

  // options
  addProperty("options", this, "horizontal")->setDesc("Draw bars horizontally");

  // margins
  addProperty("margins", this, "margin"     , "bar"  )->setDesc("Bar margin");
  addProperty("margins", this, "groupMargin", "group")->setDesc("Grouped bar margin");

  // fill
  addProperty("fill", this, "barFilled", "visible")->setDesc("Bar fill visible");

  addFillProperties("fill", "barFill", "Bar");

  // stroke
  addProperty("stroke", this, "barBorder", "visible")->setDesc("Bar stroke visible");

  addLineProperties("stroke", "barBorder", "Bar");

  addProperty("stroke", this, "barCornerSize", "cornerSize")->setDesc("Bar corner size");

  // color map
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
  CQChartsUtil::testAndSet(margin_, l, [&]() { drawObjs(); } );
}

void
CQChartsBarPlot::
setGroupMargin(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(groupMargin_, l, [&]() { drawObjs(); } );
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
