#include <CQChartsBarPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsDataLabel.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

CQChartsBarPlot::
CQChartsBarPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model) :
 CQChartsGroupPlot(view, plotType, model),
 CQChartsObjBarShapeData<CQChartsBarPlot>(this)
{
}

CQChartsBarPlot::
~CQChartsBarPlot()
{
  term();
}

//---

void
CQChartsBarPlot::
init()
{
  CQChartsGroupPlot::init();

  //---

  NoUpdate noUpdate(this);

  setBarFilled   (true);
  setBarFillColor(Color(Color::Type::PALETTE));

  setBarStroked(true);

  //---

  dataLabel_ = std::make_shared<CQChartsDataLabel>(this);

  dataLabel_->setDrawClipped(false);

  setLayerActive(CQChartsLayer::Type::FG_PLOT, true);

  //---

  addAxes();

  addKey();

  addTitle();
}

void
CQChartsBarPlot::
term()
{
}

//---

void
CQChartsBarPlot::
setValueColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(valueColumns_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBarPlot::
setOrientation(const Qt::Orientation &orientation)
{
  CQChartsUtil::testAndSet(orientation_, orientation, [&]() {
    dataLabel()->setDirection(orientation);

    CQChartsAxis::swap(xAxis(), yAxis());

    updateRangeAndObjs();
  } );
}

void
CQChartsBarPlot::
setHorizontal(bool b)
{
  setOrientation(b ? Qt::Horizontal : Qt::Vertical);
}

//---

void
CQChartsBarPlot::
addProperties()
{
  addBoxProperties();
}

void
CQChartsBarPlot::
addBoxProperties()
{
  // columns
  addProp("columns", "valueColumns", "values", "Value columns");

  // options
  addProp("options", "orientation", "", "Bar orientation");

  // margins
  addProp("margins", "margin"     , "bar"  , "Bar margin");
  addProp("margins", "groupMargin", "group", "Grouped bar margin");

  // fill
  addProp("fill", "barFilled", "visible", "Bar fill visible");

  addFillProperties("fill", "barFill", "Bar");

  // stroke
  addProp("stroke", "barStroked", "visible", "Bar stroke visible");

  addLineProperties("stroke", "barStroke", "Bar");

  addStyleProp("stroke", "barCornerSize", "cornerSize", "Bar corner size");

  //---

  dataLabel()->addPathProperties("labels", "Labels");

  //---

  // color map
  addColorMapProperties();
}

void
CQChartsBarPlot::
getPropertyNames(QStringList &names, bool hidden) const
{
  CQChartsPlot::getPropertyNames(names, hidden);

  propertyModel()->objectNames(dataLabel(), names, hidden);
}

//---

void
CQChartsBarPlot::
setMargin(const Length &l)
{
  CQChartsUtil::testAndSet(margin_, l, [&]() { drawObjs(); } );
}

void
CQChartsBarPlot::
setGroupMargin(const Length &l)
{
  CQChartsUtil::testAndSet(groupMargin_, l, [&]() { drawObjs(); } );
}

//---

bool
CQChartsBarPlot::
probe(ProbeData &probeData) const
{
  const auto &dataRange = this->dataRange();

  if (! dataRange.isSet())
    return false;

  probeData.direction = orientation();

  if (probeData.direction == Qt::Vertical) {
    if (probeData.p.x < dataRange.xmin() + 0.5)
      probeData.p.x = dataRange.xmin() + 0.5;

    if (probeData.p.x > dataRange.xmax() - 0.5)
      probeData.p.x = dataRange.xmax() - 0.5;

    probeData.p.x = std::round(probeData.p.x);
  }
  else {
    if (probeData.p.y < dataRange.ymin() + 0.5)
      probeData.p.y = dataRange.ymin() + 0.5;

    if (probeData.p.y > dataRange.ymax() - 0.5)
      probeData.p.y = dataRange.ymax() - 0.5;

    probeData.p.y = std::round(probeData.p.y);
  }

  return true;
}

//---

void
CQChartsBarPlot::
write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
      const QString &viewVarName) const
{
  CQChartsPlot::write(os, plotVarName, modelVarName, viewVarName);

  dataLabel()->write(os, plotVarName);
}
