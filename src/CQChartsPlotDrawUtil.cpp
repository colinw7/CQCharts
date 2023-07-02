#include <CQChartsPlotDrawUtil.h>
#include <CQChartsPlot.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsValueSet.h>
#include <CQChartsColor.h>
#include <CQChartsTreeMapPlace.h>

namespace CQChartsPlotDrawUtil {

using PenBrush  = CQChartsPenBrush;
using BrushData = CQChartsBrushData;
using PenData   = CQChartsPenData;
using Color     = CQChartsColor;
using ColorInd  = CQChartsUtil::ColorInd;
using Angle     = CQChartsAngle;

void
drawPie(Plot *plot, PaintDevice *device, const Values &values, const BBox &bbox,
        const PaletteName &paletteName, const PenBrush &penBrush)
{
  uint nv = values.size();
  if (! nv) return;

  double sum = values.sum();
  if (sum <= 0.0) return;

  auto color = Color::makePalette();

  color.setPaletteName(paletteName.name());

  double a  = 90.0;
  double da = 360.0/sum;

  auto c = bbox.getCenter();
  auto r = std::min(bbox.getWidth(), bbox.getHeight())/2.0;

  for (uint i = 0; i < nv; ++i) {
    double da1 = da*values.value(int(i)).value();

    PenBrush penBrush1;

    plot->setPenBrush(penBrush1,
      PenData(false), BrushData(true, plot->interpColor(color, ColorInd(int(i), int(nv)))));

    CQChartsDrawUtil::setPenBrush(device, penBrush1);

    device->setPen(penBrush.pen);

    CQChartsDrawUtil::drawPieSlice(device, c, 0.0, r, Angle(a), Angle(a + da1));

    a += da1;
  }
}

void
drawTreeMap(Plot *plot, PaintDevice *device, const Values &values, const BBox &bbox,
            const PaletteName &paletteName, const PenBrush &penBrush)
{
  uint nv = values.size();
  if (! nv) return;

  double sum = values.sum();
  if (sum <= 0.0) return;

  CQChartsTreeMapPlace place(bbox);

  for (uint i = 0; i < nv; ++i)
    place.addValue(values.value(int(i)).value());

  place.placeValues();

  //---

  auto color = Color::makePalette();

  color.setPaletteName(paletteName.name());

  place.processAreas([&](const BBox &bbox, const CQChartsTreeMapPlace::IArea &iarea) {
    PenBrush penBrush1;

    plot->setPenBrush(penBrush1,
      PenData(false), BrushData(true, plot->interpColor(color, ColorInd(iarea.i, int(nv)))));

    CQChartsDrawUtil::setPenBrush(device, penBrush1);

    device->setPen(penBrush.pen);

    device->drawRect(bbox);
  });
}

}
