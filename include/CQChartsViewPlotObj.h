#ifndef CQChartsViewPlotObj_h
#define CQChartsViewPlotObj_h

#include <CQChartsObj.h>
#include <CQChartsRect.h>
#include <CQChartsPosition.h>
#include <CQChartsLength.h>
#include <CQChartsLineDash.h>
#include <CQChartsFillPattern.h>
#include <CQChartsTextOptions.h>

#include <QPointer>

class  CQChartsView;
class  CQChartsPlot;
class  CQChartsEditHandles;
class  CQChartsPaintDevice;
class  CQChartsObjRef;
struct CQChartsPenBrush;
class  CQChartsPenData;
class  CQChartsBrushData;

/*!
 * \brief Object which could be on a view or a plot
 * \ingroup Charts
 */
class CQChartsViewPlotObj : public CQChartsObj,
 public CQChartsSelectableIFace, public CQChartsEditableIFace {
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
  using ResizeSide  = CQChartsResizeSide;
  using ObjRef      = CQChartsObjRef;

  using Polygon = CQChartsGeom::Polygon;
  using RMinMax = CQChartsGeom::RMinMax;
  using BBox    = CQChartsGeom::BBox;
  using Point   = CQChartsGeom::Point;

 public:
  CQChartsViewPlotObj(View *view);
  CQChartsViewPlotObj(Plot *plot);

  virtual ~CQChartsViewPlotObj();

  //---

  CQCharts *charts() const;

  View *view() const;
  Plot *plot() const;

  //---

  //! get edit handles
  EditHandles *editHandles() const override;

  void drawEditHandles(PaintDevice *device) const override;

  virtual void setEditHandlesBBox() const;

  //---

  // set pen/brush
  void setPenBrush(PenBrush &penBrush, const PenData &penData, const BrushData &brushData) const;

  void setPen(PenBrush &penBrush, const PenData &penData) const;

  void updatePenBrushState(PenBrush &penBrush, DrawType drawType=DrawType::BOX) const;

  //---

  // text utilities
  QFont calcFont(const Font &font) const;

  void setPainterFont(PaintDevice *painter, const Font &font) const;

  void adjustTextOptions(TextOptions &textOptions) const;

  //---

  // conversion utilities
  Point positionToParent(const ObjRef &objRef, const Position &pos) const;
  Point positionToPixel (const ObjRef &objRef, const Position &pos) const;

  Position positionFromParent(const ObjRef &objRef, const Position &pos) const;

  Point positionToParent(const Position &pos) const;
  Point positionToPixel (const Position &pos) const;

  Point intersectObjRef(const ObjRef &objRef, const Point &p1, const Point &p2) const;

  bool objectRect(const ObjRef &objRef, CQChartsObj* &obj, BBox &bbox) const;

  //---

  double lengthParentWidth (const Length &len) const;
  double lengthParentHeight(const Length &len) const;

  double lengthParentSignedWidth (const Length &len) const;
  double lengthParentSignedHeight(const Length &len) const;

  double lengthWindowWidth(const Length &len) const;

  double lengthPixelWidth (const Length &len) const;
  double lengthPixelHeight(const Length &len) const;

  Point windowToPixel(const Point &w) const;
  BBox  windowToPixel(const BBox  &w) const;

  Point pixelToWindow(const Point &w) const;
  BBox  pixelToWindow(const BBox  &w) const;

  double pixelToWindowWidth (double pw) const;
  double pixelToWindowHeight(double ph) const;

  double windowToPixelWidth(double w) const;

  //---

  QColor backgroundColor() const;

  //---

  static Length   makeLength(View *view, Plot *plot, double len);
  static Position makePosition(View *view, Plot *plot, double x, double y);
  static Rect     makeRect(View *view, Plot *plot, double x1, double y1, double x2, double y2);
  static Rect     makeRect(View *view, Plot *plot, const Position &start, const Position &end);

 protected:
  using ViewP        = QPointer<View>;
  using PlotP        = QPointer<Plot>;
  using EditHandlesP = std::unique_ptr<EditHandles>;

  ViewP        view_;        //!< parent view
  PlotP        plot_;        //!< parent plot
  EditHandlesP editHandles_; //!< edit handles
};

#endif
