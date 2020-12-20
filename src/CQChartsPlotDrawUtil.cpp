#include <CQChartsPlotDrawUtil.h>
#include <CQChartsPlot.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsValueSet.h>
#include <CQChartsColor.h>

namespace CQChartsPlotDrawUtil {

using PenBrush  = CQChartsPenBrush;
using BrushData = CQChartsBrushData;
using PenData   = CQChartsPenData;
using Color     = CQChartsColor;
using ColorInd  = CQChartsUtil::ColorInd;
using Angle     = CQChartsAngle;

void
drawPie(Plot *plot, PaintDevice *device, const Values &values, const BBox &bbox,
        const PaletteName &paletteName)
{
  int nv = values.size();
  if (! nv) return;

  double sum = values.sum();
  if (sum <= 0.0) return;

  Color color(Color::Type::PALETTE);

  color.setPaletteName(paletteName.name());

  double a  = 90.0;
  double da = 360.0/sum;

  auto c = bbox.getCenter();
  auto r = std::min(bbox.getWidth(), bbox.getHeight())/2.0;

  for (int i = 0; i < nv; ++i) {
    double da1 = da*values.value(i).value();

    PenBrush penBrush;

    plot->setPenBrush(penBrush,
      PenData(false), BrushData(true, plot->interpColor(color, ColorInd(i, nv))));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    CQChartsDrawUtil::drawPieSlice(device, c, 0.0, r, Angle(a), Angle(a + da1));

    a += da1;
  }
}

void
drawTreeMap(Plot *plot, PaintDevice *device, const Values &values, const BBox &bbox,
            const PaletteName &paletteName)
{
  int nv = values.size();
  if (! nv) return;

  double sum = values.sum();
  if (sum <= 0.0) return;

  Color color(Color::Type::PALETTE);

  color.setPaletteName(paletteName.name());

  double a  = bbox.area();
  double da = a/sum;

  struct IArea {
    int    i    { 0 };
    double area { 0.0 };
    BBox   bbox;

    IArea(int i, double area) :
     i(i), area(area) {
    }
  };

  using IAreas = std::vector<IArea>;

  IAreas iareas;

  for (int i = 0; i < nv; ++i) {
    double a1 = da*values.value(i).value();

    iareas.emplace_back(i, a1);
  }

  // reverse sort (largest to smallest)
  std::sort(iareas.begin(), iareas.end(),
    [](const IArea &lhs, const IArea &rhs) { return (lhs.area > rhs.area); });

  //---

  struct BBoxIAreas {
    BBox        bbox;
    IAreas      iareas;
    BBoxIAreas *bboxIAreas1 { nullptr };
    BBoxIAreas *bboxIAreas2 { nullptr };

    BBoxIAreas(const BBox &bbox) :
     bbox(bbox) {
    }

   ~BBoxIAreas() {
     delete bboxIAreas1;
     delete bboxIAreas2;
    }

    void placeAreas() {
      if (iareas.size() < 2)
        return;

      // divide areas in two
      double a = bbox.area();

      double midArea = a/2.0;

      IAreas iareas1, iareas2;

      int splitPos = 0;

      double a1 = 0.0;

      for (auto &iarea : iareas) {
        if (a1 >= midArea)
          break;

        ++splitPos;

        a1 += iarea.area;
      }

      if      (splitPos == 0)
        splitPos = 1;
      else if (splitPos >= int(iareas.size()))
        splitPos = iareas.size() - 1;

      for (auto &iarea : iareas) {
        if (splitPos > 0)
          iareas1.push_back(iarea);
        else
          iareas2.push_back(iarea);

        --splitPos;
      }

      assert(! iareas1.empty() && ! iareas2.empty());

      //---

      // determine bbox for each area
      a1 = 0.0;

      for (auto &iarea : iareas1)
        a1 += iarea.area;

      //double a2 = 0.0;

      //for (auto &iarea : iareas2)
      //  a2 += iarea.area;

      double f = a1/a;

      double w = bbox.getWidth ();
      double h = bbox.getHeight();

      BBox bbox1, bbox2;

      if (w >= h) {
        // divide vertically
        double w1 = w*f;

        bbox1 = BBox(bbox.getXMin(), bbox.getYMin(), bbox.getXMin() + w1, bbox.getYMax());
        bbox2 = BBox(bbox.getXMin() + w1, bbox.getYMin(), bbox.getXMax(), bbox.getYMax());
      }
      else {
        // divide horizontally
        double h1 = h*f;

        bbox1 = BBox(bbox.getXMin(), bbox.getYMin(), bbox.getXMax(), bbox.getYMin() + h1);
        bbox2 = BBox(bbox.getXMin(), bbox.getYMin() + h1, bbox.getXMax(), bbox.getYMax());
      }

      //---

      // create new placement set
      bboxIAreas1 = new BBoxIAreas(bbox1);
      bboxIAreas2 = new BBoxIAreas(bbox2);

      for (auto &iarea : iareas1)
        bboxIAreas1->iareas.push_back(iarea);

      for (auto &iarea : iareas2)
        bboxIAreas2->iareas.push_back(iarea);

      iareas.clear();

      //---

      // iterate
      bboxIAreas1->placeAreas();
      bboxIAreas2->placeAreas();
    }

    void draw(Plot *plot, PaintDevice *device, const Color &color, int nv) {
      for (const auto &iarea : iareas) {
        PenBrush penBrush;

        plot->setPenBrush(penBrush,
          PenData(false), BrushData(true, plot->interpColor(color, ColorInd(iarea.i, nv))));

        CQChartsDrawUtil::setPenBrush(device, penBrush);

        device->drawRect(bbox);
      }

      if (bboxIAreas1)
        bboxIAreas1->draw(plot, device, color, nv);

      if (bboxIAreas2)
        bboxIAreas2->draw(plot, device, color, nv);
    }
  };

  BBoxIAreas bboxIAreas(bbox);

  bboxIAreas.iareas = iareas;

  bboxIAreas.placeAreas();

  bboxIAreas.draw(plot, device, color, nv);
}

}
