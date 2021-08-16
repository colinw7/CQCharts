#ifndef CQChartsRotatedTextBoxObj_H
#define CQChartsRotatedTextBoxObj_H

#include <CQChartsTextBoxObj.h>

/*!
 * \brief Charts Rotated Text Box Object
 * \ingroup Charts
 *
 * TODO: allow round to increments (90 degrees, H/V)
 */
class CQChartsRotatedTextBoxObj : public CQChartsTextBoxObj {
 public:
  using Plot        = CQChartsPlot;
  using PaintDevice = CQChartsPaintDevice;
  using TextOptions = CQChartsTextOptions;
  using Length      = CQChartsLength;
  using ColorInd    = CQChartsUtil::ColorInd;

 public:
  CQChartsRotatedTextBoxObj(Plot *plot);

  void draw(PaintDevice *device, const Point &c, const QString &text,
            double angle=0.0, Qt::Alignment align=Qt::AlignHCenter | Qt::AlignVCenter,
            bool isRotated=false) const;

  void draw(PaintDevice *device, const Point &c, const QString &text1, const QString &text2,
            double angle=0.0, Qt::Alignment align=Qt::AlignHCenter | Qt::AlignVCenter,
            bool isRotated=false) const;

  static void draw(PaintDevice *device, const Point &c, const QString &text1,
                   const QString &text2, bool isRotated, const PenBrush &penBrush,
                   const TextOptions &textOptions, const Margin &margin, const Length &cornerSize,
                   const Sides &borderSides, BBox &drawBBox);

  BBox bbox(const Point &pcenter, const QString &text, double angle=0.0,
            Qt::Alignment align=Qt::AlignHCenter | Qt::AlignVCenter,
            bool isRotated=false) const;

  static BBox bbox(Plot *plot, const Point &pcenter, const QString &text,
                   bool isRotated, const TextOptions &textOptions, const Margin &margin);
  static BBox bbox(View *view, const Point &pcenter, const QString &text,
                   bool isRotated, const TextOptions &textOptions, const Margin &margin);

  void drawConnectedRadialText(PaintDevice *device, const Point &center,
                               double ro, double lr, double ta, const QString &text,
                               const QPen &lpen, bool isRotated);

  static void drawConnectedRadialText(PaintDevice *device, const Point &center,
                                      double ro, double lr, const QString &text,
                                      const QPen &lpen, bool isRotated, const PenBrush &penBrush,
                                      const TextOptions &textOptions, const Margin &margin,
                                      const Length &cornerSize, const Sides &borderSides);

  void calcConnectedRadialTextBBox(const Point &center, double ro, double lr, double ta,
                                   const QString &text, bool isRotated, BBox &tbbox);

  static void calcConnectedRadialTextBBox(Plot *plot, const Point &center, double ro, double lr,
                                          const QString &text, bool isRotated,
                                          const TextOptions &textOptions,
                                          const Margin &margin, BBox &tbbox);

  static void calcConnectedRadialTextBBox(View *view, const Point &center, double ro, double lr,
                                          const QString &text, bool isRotated,
                                          const TextOptions &textOptions,
                                          const Margin &margin, BBox &tbbox);

 private:
  static BBox bbox1(View *view, Plot *plot, const Point &pcenter, const QString &text,
                    bool isRotated, const TextOptions &textOptions, const Margin &margin);

  static void drawCalcConnectedRadialText(View *view, Plot *plot, PaintDevice *device,
                                          const Point &center, double ro, double lr,
                                          const QString &text, const QPen &lpen, bool isRotated,
                                          const PenBrush &penBrush, const TextOptions &textOptions,
                                          const Margin &margin, const Length &cornerSize,
                                          const Sides &borderSides, BBox &tbbox);
};

#endif
