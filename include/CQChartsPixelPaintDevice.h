#ifndef CQChartsPixelPaintDevice_H
#define CQChartsPixelPaintDevice_H

#include <CQChartsViewPlotPaintDevice.h>

/*!
 * \brief Paint Device for drawing on Widget (pixel only)
 * \ingroup Charts
 */
class CQChartsPixelPaintDevice : public CQChartsViewPlotPaintDevice {
 public:
  explicit CQChartsPixelPaintDevice(QPainter *painter);

  Type type() const override { return Type::PIXEL; }

  bool invertY() const override { return true; }
};

#endif
