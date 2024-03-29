#ifndef CQChartsPlotDrawUtil_H
#define CQChartsPlotDrawUtil_H

#include <CQChartsGeom.h>

class CQChartsPlot;
class CQChartsPaintDevice;
class CQChartsPenBrush;
class CQChartsRValues;
class CQChartsPaletteName;

namespace CQChartsPlotDrawUtil {

using Plot        = CQChartsPlot;
using PaintDevice = CQChartsPaintDevice;
using Values      = CQChartsRValues;
using PaletteName = CQChartsPaletteName;
using PenBrush    = CQChartsPenBrush;
using BBox        = CQChartsGeom::BBox;

//---

void drawPie(Plot *plot, PaintDevice *device, const Values &values, const BBox &bbox,
             const PaletteName &paletteName, const PenBrush &penBrush);

void drawTreeMap(Plot *plot, PaintDevice *device, const Values &values, const BBox &bbox,
                 const PaletteName &paletteName, const PenBrush &penBrush);

}

#endif
