#ifndef CQChartsRotatedTextBoxObj_H
#define CQChartsRotatedTextBoxObj_H

#include <CQChartsTextBoxObj.h>

/*!
 * \brief Charts Rotated Text Box Object
 * \ingroup Charts
 */
class CQChartsRotatedTextBoxObj : public CQChartsTextBoxObj {
 public:
  CQChartsRotatedTextBoxObj(CQChartsPlot *plot);

  const QRectF &rect() const { return rect_; }

  void draw(CQChartsPaintDevice *device, const QPointF &c, const QString &text,
            double angle=0.0, Qt::Alignment align=Qt::AlignHCenter | Qt::AlignVCenter) const;

  CQChartsGeom::BBox bbox(const QPointF &center, const QString &text, double angle=0.0,
                          Qt::Alignment align=Qt::AlignHCenter | Qt::AlignVCenter) const;

  void drawConnectedRadialText(CQChartsPaintDevice *device, const QPointF &center,
                               double ro, double lr, double ta, const QString &text,
                               const QPen &lpen, bool isRotated);

  void calcConnectedRadialTextBBox(const QPointF &center, double ro, double lr, double ta,
                                   const QString &text, bool isRotated, CQChartsGeom::BBox &tbbox);

 private:
  void drawCalcConnectedRadialText(CQChartsPaintDevice *device, const QPointF &center,
                                   double ro, double lr, double ta, const QString &text,
                                   const QPen &lpen, bool isRotated, CQChartsGeom::BBox &tbbox);

 private:
  mutable QRectF rect_;
};

#endif
