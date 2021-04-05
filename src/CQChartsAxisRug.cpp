#include <CQChartsAxisRug.h>
#include <CQChartsPlot.h>
#include <CQPropertyViewItem.h>

CQChartsAxisRug::
CQChartsAxisRug(CQChartsPlot *plot, const Qt::Orientation &direction) :
 CQChartsObj(plot->charts()), CQChartsObjPointData<CQChartsAxisRug>(this),
 plot_(plot), direction_(direction)
{
  if (direction == Qt::Horizontal)
    setSymbol(CQChartsSymbol(CQChartsSymbolType::Type::VLINE));
  else
    setSymbol(CQChartsSymbol(CQChartsSymbolType::Type::HLINE));

  setSymbolSize(CQChartsLength("5px"));
}

void
CQChartsAxisRug::
setSide(const Side &s)
{
  CQChartsUtil::testAndSet(side_, s, [&]() { dataInvalidate(); } );
}

void
CQChartsAxisRug::
setDirection(const Qt::Orientation &o)
{
  CQChartsUtil::testAndSet(direction_, o, [&]() { dataInvalidate(); } );
}

void
CQChartsAxisRug::
addProperties(const QString &path, const QString &desc)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(plot()->addProperty(path, this, name, alias)->setDesc(desc));
  };

  addProp(path, "visible", "", desc + " visible");
  addProp(path, "side"   , "", desc + " side");

  addProp(path, "symbol"           , "symbol", desc + " symbol");
  addProp(path, "symbolSize"       , "size"  , desc + " symbol size");
  addProp(path, "symbolStrokeAlpha", "alpha" , desc + " symbol alpha");
}

CQChartsGeom::BBox
CQChartsAxisRug::
calcBBox() const
{
  BBox bbox;

  if (! isVisible())
    return bbox;

  auto dataRange = CQChartsGeom::Range(plot()->calcDataRange());

  //---

  // rug axis
  double sx, sy;

  plot()->plotSymbolSize(symbolSize(), sx, sy);

  Point p1, p2;

  if (direction() == Qt::Horizontal) {
    double y1 = (side().type() == Side::Type::BOTTOM_LEFT ?
                   dataRange.ymin()        : dataRange.ymax()       );
    double y2 = (side().type() == Side::Type::BOTTOM_LEFT ?
                   dataRange.ymin() - 2*sy : dataRange.ymax() + 2*sy);

    p1 = Point(dataRange.xmin(), y1);
    p2 = Point(dataRange.xmax(), y2);
  }
  else {
    double x1 = (side().type() == Side::Type::BOTTOM_LEFT ?
                   dataRange.xmin()        : dataRange.xmax()       );
    double x2 = (side().type() == Side::Type::BOTTOM_LEFT ?
                   dataRange.xmin() - 2*sx : dataRange.xmax() + 2*sx);

    p1 = Point(x1, dataRange.ymin());
    p2 = Point(x2, dataRange.ymax());
  }

  bbox += p1;
  bbox += p2;

  return bbox;
}

bool
CQChartsAxisRug::
contains(const Point &p) const
{
  return calcBBox().inside(p);
}

void
CQChartsAxisRug::
dataInvalidate()
{
  plot()->resetExtraBBox();

  plot()->drawObjs();
}

void
CQChartsAxisRug::
draw(CQChartsPaintDevice *device, double delta)
{
  auto symbol      = this->symbol();
  auto symbolSize  = this->symbolSize();
  auto symbolAlpha = this->symbolStrokeAlpha();

  if (! symbol.isValid())
    symbol = CQChartsSymbol(direction() == Qt::Horizontal ?
      CQChartsSymbolType::Type::VLINE : CQChartsSymbolType::Type::HLINE);

  double sx, sy;

  plot()->plotSymbolSize(symbolSize, sx, sy);

  auto dataRange = CQChartsGeom::Range(plot()->calcDataRange());

  for (const auto &p : points_) {
    double x, y;

    if (direction() == Qt::Horizontal) {
      x = p.pos;
      y = (side().type() == Side::Type::BOTTOM_LEFT ? dataRange.ymin() - delta - sy/2 :
                                                      dataRange.ymax() + delta + sy/2);
    }
    else {
      x = (side().type() == Side::Type::BOTTOM_LEFT ? dataRange.xmin() - delta - sx/2 :
                                                      dataRange.xmax() + delta + sx/2);
      y = p.pos;
    }

    Point p1(x, y);

    CQChartsPenBrush penBrush;

    plot()->setPenBrush(penBrush,
      CQChartsPenData(true, p.c, symbolAlpha), CQChartsBrushData(false));

    CQChartsDrawUtil::drawSymbol(device, penBrush, symbol, p1, symbolSize);
  }

  if (plot()->showBoxes())
    plot()->drawWindowColorBox(device, calcBBox(), Qt::red);
}
