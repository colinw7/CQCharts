#ifndef CQChartsRotatedTextBoxObj_H
#define CQChartsRotatedTextBoxObj_H

#include <CQChartsTextBoxObj.h>

/*!
 * \brief Charts Rotated Text Box Object
 * \ingroup Charts
 */
class CQChartsRotatedTextBoxObj : public CQChartsTextBoxObj {
 public:
  using Plot        = CQChartsPlot;
  using PaintDevice = CQChartsPaintDevice;

 public:
  CQChartsRotatedTextBoxObj(Plot *plot);

  void draw(PaintDevice *device, const Point &c, const QString &text,
            double angle=0.0, Qt::Alignment align=Qt::AlignHCenter|Qt::AlignVCenter,
            bool isRotated=false) const;

  BBox bbox(const Point &pcenter, const QString &text, double angle=0.0,
            Qt::Alignment align=Qt::AlignHCenter|Qt::AlignVCenter,
            bool isRotated=false) const;

  void drawConnectedRadialText(PaintDevice *device, const Point &center,
                               double ro, double lr, double ta, const QString &text,
                               const QPen &lpen, bool isRotated);

  void calcConnectedRadialTextBBox(const Point &center, double ro, double lr, double ta,
                                   const QString &text, bool isRotated, BBox &tbbox);

 private:
  void drawCalcConnectedRadialText(PaintDevice *device, const Point &center,
                                   double ro, double lr, double ta, const QString &text,
                                   const QPen &lpen, bool isRotated, BBox &tbbox);

 private:
//mutable BBox bbox_;
};

#endif
