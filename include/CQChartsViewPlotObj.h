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
class CQChartsObjRef;

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
  using ResizeSide  = CQChartsResizeSide;
  using ObjRef      = CQChartsObjRef;

  using Polygon = CQChartsGeom::Polygon;
  using RMinMax = CQChartsGeom::RMinMax;

 public:
  CQChartsViewPlotObj(View *view);
  CQChartsViewPlotObj(Plot *plot);

  virtual ~CQChartsViewPlotObj();

  //---

  CQCharts *charts() const;

  View *view() const;
  Plot *plot() const { return plot_; }

  //---

  // Select Interface

  //! handle select press, move, release
  virtual bool selectPress  (const Point &, CQChartsSelMod) { return false; }
  virtual bool selectMove   (const Point &) { return false; }
  virtual bool selectRelease(const Point &) { return false; }

  //---

  // Edit Interface

  // handle edit press, move, motion, release
  virtual bool editPress  (const Point &) { return false; }
  virtual bool editMove   (const Point &) { return false; }
  virtual bool editMotion (const Point &) { return false; } // return true if inside
  virtual bool editRelease(const Point &) { return true; }

  //! handle edit move by (move)
  virtual void editMoveBy(const Point &) { }

  //! set new bounding box (resize)
  virtual void setEditBBox(const BBox &, const ResizeSide &) { }

  //---

  //! get edit handles
  EditHandles *editHandles() const;

  virtual void drawEditHandles(QPainter *painter) const;

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
