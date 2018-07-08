#include <CQChartsBarPlot.h>

CQChartsBarPlot::
CQChartsBarPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model) :
 CQChartsGroupPlot(view, plotType, model)
{
  (void) addColorSet("color");

  setBarFill (true);
  setBarColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  //---

  setBorder(true);

  setLayerActive(Layer::FG, true);

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
  addProperty("options", this, "margin"     , "barMargin"  );
  addProperty("options", this, "groupMargin", "groupMargin");

  addProperty("stroke", this, "border"    , "visible"   );
  addProperty("stroke", this, "cornerSize", "cornerSize");

  addLineProperties("stroke", "border");

  addProperty("fill", this, "barFill", "visible");

  addFillProperties("fill", "bar");

  addProperty("color", this, "colorMapped", "mapped");
  addProperty("color", this, "colorMapMin", "mapMin");
  addProperty("color", this, "colorMapMax", "mapMax");
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
  CQChartsUtil::testAndSet(margin_, l, [&]() { update(); } );
}

void
CQChartsBarPlot::
setGroupMargin(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(groupMargin_, l, [&]() { update(); } );
}

//---

bool
CQChartsBarPlot::
isBorder() const
{
  return boxData_.shape.border.visible;
}

void
CQChartsBarPlot::
setBorder(bool b)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.visible, b, [&]() { update(); } );
}

const CQChartsColor &
CQChartsBarPlot::
borderColor() const
{
  return boxData_.shape.border.color;
}

void
CQChartsBarPlot::
setBorderColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.color, c, [&]() { update(); } );
}

QColor
CQChartsBarPlot::
interpBorderColor(int i, int n) const
{
  return borderColor().interpColor(this, i, n);
}

double
CQChartsBarPlot::
borderAlpha() const
{
  return boxData_.shape.border.alpha;
}

void
CQChartsBarPlot::
setBorderAlpha(double a)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.alpha, a, [&]() { update(); } );
}

const CQChartsLength &
CQChartsBarPlot::
borderWidth() const
{
  return boxData_.shape.border.width;
}

void
CQChartsBarPlot::
setBorderWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.width, l, [&]() { update(); } );
}

const CQChartsLineDash &
CQChartsBarPlot::
borderDash() const
{
  return boxData_.shape.border.dash;
}

void
CQChartsBarPlot::
setBorderDash(const CQChartsLineDash &d)
{
  CQChartsUtil::testAndSet(boxData_.shape.border.dash, d, [&]() { update(); } );
}

const CQChartsLength &
CQChartsBarPlot::
cornerSize() const
{
  return boxData_.cornerSize;
}

void
CQChartsBarPlot::
setCornerSize(const CQChartsLength &s)
{
  CQChartsUtil::testAndSet(boxData_.cornerSize, s, [&]() { update(); } );
}

//---

bool
CQChartsBarPlot::
isBarFill() const
{
  return boxData_.shape.background.visible;
}

void
CQChartsBarPlot::
setBarFill(bool b)
{
  CQChartsUtil::testAndSet(boxData_.shape.background.visible, b, [&]() { update(); } );
}

const CQChartsColor &
CQChartsBarPlot::
barColor() const
{
  return boxData_.shape.background.color;
}

void
CQChartsBarPlot::
setBarColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(boxData_.shape.background.color, c, [&]() { update(); } );
}

QColor
CQChartsBarPlot::
interpBarColor(int i, int n) const
{
  return barColor().interpColor(this, i, n);
}

double
CQChartsBarPlot::
barAlpha() const
{
  return boxData_.shape.background.alpha;
}

void
CQChartsBarPlot::
setBarAlpha(double a)
{
  CQChartsUtil::testAndSet(boxData_.shape.background.alpha, a, [&]() { update(); } );
}

CQChartsBarPlot::Pattern
CQChartsBarPlot::
barPattern() const
{
  return (Pattern) boxData_.shape.background.pattern;
}

void
CQChartsBarPlot::
setBarPattern(Pattern pattern)
{
  if (pattern != (Pattern) boxData_.shape.background.pattern) {
    boxData_.shape.background.pattern = (CQChartsFillPattern::Type) pattern;

    update();
  }
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
