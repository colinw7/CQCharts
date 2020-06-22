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
class CQChartsEditHandles;
class CQChartsPaintDevice;

/*!
 * \brief Object which could be on a view or a plot
 * \ingroup Charts
 */
class CQChartsViewPlotObj : public CQChartsObj {
  Q_OBJECT

 public:
  using View        = CQChartsView;
  using Plot        = CQChartsPlot;
  using DrawType    = CQChartsObjDrawType;
  using EditHandles = CQChartsEditHandles;
  using Position    = CQChartsPosition;
  using Length      = CQChartsLength;
  using Rect        = CQChartsRect;
  using PenBrush    = CQChartsPenBrush;
  using PenData     = CQChartsPenData;
  using BrushData   = CQChartsBrushData;
  using Alpha       = CQChartsAlpha;
  using FillPattern = CQChartsFillPattern;
  using LineDash    = CQChartsLineDash;
  using Font        = CQChartsFont;
  using PaintDevice = CQChartsPaintDevice;
  using TextOptions = CQChartsTextOptions;

  using Polygon = CQChartsGeom::Polygon;

 public:
  CQChartsViewPlotObj(View *view);
  CQChartsViewPlotObj(Plot *plot);

  virtual ~CQChartsViewPlotObj();

  //---

  CQCharts *charts() const;

  View *view() const;
  Plot *plot() const { return plot_; }

  //---

  //! get edit handles
  EditHandles *editHandles() const;

  virtual void drawEditHandles(QPainter *painter) const;

  //---

  // set pen/brush
  void setPenBrush(PenBrush &penBrush, const PenData &penData, const BrushData &brushData) const;

  void setPenBrush(PenBrush &penBrush,
                   bool stroked, const QColor &strokeColor, const Alpha &strokeAlpha,
                   const Length &strokeWidth, const LineDash &strokeDash,
                   bool filled, const QColor &fillColor, const Alpha &fillAlpha,
                   const FillPattern &pattern) const;

  void setPen(QPen &pen, bool stroked, const QColor &strokeColor,
              const Alpha &strokeAlpha=Alpha(), const Length &strokeWidth=Length("0px"),
              const LineDash &strokeDash=LineDash()) const;

  void setBrush(QBrush &brush, bool filled, const QColor &fillColor=QColor(),
                const Alpha &fillAlpha=Alpha(), const FillPattern &pattern=FillPattern()) const;

  void updatePenBrushState(PenBrush &penBrush, DrawType drawType=DrawType::BOX) const;

  //---

  // text utilities
  QFont calcFont(const Font &font) const;

  void setPainterFont(PaintDevice *painter, const Font &font) const;

  void adjustTextOptions(TextOptions &textOptions) const;

  //---

  // conversion utilities
  Point positionToParent(const Position &pos) const;
  Point positionToPixel (const Position &pos) const;

  double lengthParentWidth (const Length &len) const;
  double lengthParentHeight(const Length &len) const;

  double lengthPixelWidth (const Length &len) const;
  double lengthPixelHeight(const Length &len) const;

  Point windowToPixel(const Point &w) const;
  BBox  windowToPixel(const BBox  &w) const;

  Point pixelToWindow(const Point &w) const;
  BBox  pixelToWindow(const BBox  &w) const;

  double pixelToWindowWidth (double pw) const;
  double pixelToWindowHeight(double ph) const;

  //---

  QColor backgroundColor() const;

  //---

  static Length   makeLength(View *view, Plot *plot, double len);
  static Position makePosition(View *view, Plot *plot, double x, double y);
  static Rect     makeRect(View *view, Plot *plot, double x1, double y1, double x2, double y2);
  static Rect     makeRect(View *view, Plot *plot, const Position &start, const Position &end);

 protected:
  View*        view_        { nullptr }; //!< parent view
  Plot*        plot_        { nullptr }; //!< parent plot
  EditHandles* editHandles_ { nullptr }; //!< edit handles
};

#endif
