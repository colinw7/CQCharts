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
  using Angle       = CQChartsAngle;
  using PenBrush    = CQChartsPenBrush;
  using ColorInd    = CQChartsUtil::ColorInd;

 public:
  CQChartsRotatedTextBoxObj(Plot *plot);

  // draw rotated text
  void draw(PaintDevice *device, const Point &c, const QString &text,
            double angle=0.0, Qt::Alignment align=Qt::AlignHCenter | Qt::AlignVCenter,
            bool isRadial=false) const;

  // draw rotated texts
  void draw(PaintDevice *device, const Point &c, const QString &text1, const QString &text2,
            double angle=0.0, Qt::Alignment align=Qt::AlignHCenter | Qt::AlignVCenter,
            bool isRadial=false) const;

  // draw rotated texts (static)
  static void draw(PaintDevice *device, const Point &c, const QString &text1, const QString &text2,
                   bool isRadial, const PenBrush &penBrush, const TextOptions &textOptions,
                   const Margin &margin, const Length &cornerSize, const Sides &borderSides,
                   BBox &drawBBox);

  // bbox of rotated text
  BBox bbox(const Point &pcenter, const QString &text, double angle=0.0,
            Qt::Alignment align=Qt::AlignHCenter | Qt::AlignVCenter,
            bool isRadial=false) const;

  // bbox of rotated view text (static)
  static BBox bbox(Plot *plot, const Point &pcenter, const QString &text,
                   bool isRadial, const TextOptions &textOptions, const Margin &margin);
  // bbox of rotated plot text (static)
  static BBox bbox(View *view, const Point &pcenter, const QString &text,
                   bool isRadial, const TextOptions &textOptions, const Margin &margin);

  // draw rotated text connected to radial point
  void drawConnectedRadialText(PaintDevice *device, const Point &center, double ro,
                               double lr, const Angle &ta, const QString &text,
                               const QPen &lpen, bool isRotated);

  // draw rotated text connected to radial point (static)
  static void drawConnectedRadialText(PaintDevice *device, const Point &center, double ro,
                                      double lr, const Angle &la, const QString &text,
                                      const QPen &lpen, const PenBrush &penBrush,
                                      const TextOptions &textOptions, const Margin &margin,
                                      const Length &cornerSize, const Sides &borderSides);

  // get bbox of rotated text connected to radial point
  void calcConnectedRadialTextBBox(const Point &center, double ro, double lr, const Angle &ta,
                                   const QString &text, bool isRotated, BBox &tbbox);

  // get bbox of rotated plot text connected to radial point (static)
  static void calcConnectedRadialTextBBox(Plot *plot, const Point &center, double ro,
                                          double lr, const Angle &la, const QString &text,
                                          const TextOptions &textOptions,
                                          const Margin &margin, BBox &tbbox);
  // get bbox of rotated view text connected to radial point (static)
  static void calcConnectedRadialTextBBox(View *view, const Point &center, double ro,
                                          double lr, const Angle &la, const QString &text,
                                          const TextOptions &textOptions,
                                          const Margin &margin, BBox &tbbox);

 private:
  static BBox bbox1(View *view, Plot *plot, const Point &pcenter, const QString &text,
                    bool isRadial, const TextOptions &textOptions, const Margin &margin);

  static void drawCalcConnectedRadialText(View *view, Plot *plot, PaintDevice *device,
                                          const Point &center, double ro, double lr,
                                          const Angle &la, const QString &text, const QPen &lpen,
                                          const PenBrush &penBrush, const TextOptions &textOptions,
                                          const Margin &margin, const Length &cornerSize,
                                          const Sides &borderSides, BBox &tbbox);
};

#endif
