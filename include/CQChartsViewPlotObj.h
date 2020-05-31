#ifndef CQChartsViewPlotObj_h
#define CQChartsViewPlotObj_h

#include <CQChartsObj.h>
#include <CQChartsRect.h>
#include <CQChartsPosition.h>
#include <CQChartsLength.h>
#include <CQChartsLineDash.h>
#include <CQChartsFillPattern.h>
#include <CQChartsTextOptions.h>
#include <CQChartsDrawUtil.h>

class CQChartsView;
class CQChartsPlot;
class CQChartsPaintDevice;

/*!
 * \brief Object which could be on a view or a plot
 * \ingroup Charts
 */
class CQChartsViewPlotObj : public CQChartsObj {
  Q_OBJECT

 public:
  using DrawType = CQChartsObjDrawType;

  using Point = CQChartsGeom::Point;
  using BBox  = CQChartsGeom::BBox;

 public:
  CQChartsViewPlotObj(CQChartsView *view);
  CQChartsViewPlotObj(CQChartsPlot *plot);

  virtual ~CQChartsViewPlotObj() { }

  //---

  CQCharts *charts() const;

  CQChartsView *view() const;
  CQChartsPlot *plot() const { return plot_; }

  //---

  // set pen/brush
  void setPenBrush(CQChartsPenBrush &penBrush, const CQChartsPenData &penData,
                   const CQChartsBrushData &brushData) const;

  void setPenBrush(CQChartsPenBrush &penBrush,
                   bool stroked, const QColor &strokeColor, const CQChartsAlpha &strokeAlpha,
                   const CQChartsLength &strokeWidth, const CQChartsLineDash &strokeDash,
                   bool filled, const QColor &fillColor, const CQChartsAlpha &fillAlpha,
                   const CQChartsFillPattern &pattern) const;

  void setPen(QPen &pen, bool stroked, const QColor &strokeColor,
              const CQChartsAlpha &strokeAlpha=CQChartsAlpha(),
              const CQChartsLength &strokeWidth=CQChartsLength("0px"),
              const CQChartsLineDash &strokeDash=CQChartsLineDash()) const;

  void setBrush(QBrush &brush, bool filled, const QColor &fillColor=QColor(),
                const CQChartsAlpha &fillAlpha=CQChartsAlpha(),
                const CQChartsFillPattern &pattern=CQChartsFillPattern()) const;

  void updatePenBrushState(CQChartsPenBrush &penBrush, DrawType drawType=DrawType::BOX) const;

  //---

  // text utilities
  QFont calcFont(const CQChartsFont &font) const;

  void setPainterFont(CQChartsPaintDevice *painter, const CQChartsFont &font) const;

  void adjustTextOptions(CQChartsTextOptions &textOptions) const;

  //---

  // conversion utilities
  Point positionToParent(const CQChartsPosition &pos) const;
  Point positionToPixel (const CQChartsPosition &pos) const;

  double lengthParentWidth (const CQChartsLength &len) const;
  double lengthParentHeight(const CQChartsLength &len) const;

  double lengthPixelWidth (const CQChartsLength &len) const;
  double lengthPixelHeight(const CQChartsLength &len) const;

  Point windowToPixel(const Point &w) const;
  BBox  windowToPixel(const BBox  &w) const;

  Point pixelToWindow(const Point &w) const;
  BBox  pixelToWindow(const BBox  &w) const;

  double pixelToWindowWidth (double pw) const;
  double pixelToWindowHeight(double ph) const;

  //---

  QColor backgroundColor() const;

  //---

  static CQChartsLength   makeLength(CQChartsView *view, CQChartsPlot *plot,
                                     double len);
  static CQChartsPosition makePosition(CQChartsView *view, CQChartsPlot *plot,
                                       double x, double y);
  static CQChartsRect     makeRect(CQChartsView *view, CQChartsPlot *plot,
                                   double x1, double y1, double x2, double y2);
  static CQChartsRect     makeRect(CQChartsView *view, CQChartsPlot *plot,
                                   const CQChartsPosition &start, const CQChartsPosition &end);

 protected:
  CQChartsView* view_ { nullptr }; //!< parent view
  CQChartsPlot* plot_ { nullptr }; //!< parent plot
};

#endif
