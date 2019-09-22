#ifndef CQChartsViewPlotObj_h
#define CQChartsViewPlotObj_h

#include <CQChartsObj.h>
#include <CQChartsPosition.h>
#include <CQChartsLength.h>
#include <CQChartsLineDash.h>
#include <CQChartsFillPattern.h>
#include <CQChartsTextOptions.h>

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
  void setPen(QPen &pen, bool stroked, const QColor &strokeColor, double strokeAlpha=1.0,
              const CQChartsLength &strokeWidth=CQChartsLength("0px"),
              const CQChartsLineDash &strokeDash=CQChartsLineDash()) const;

  void setBrush(QBrush &brush, bool filled, const QColor &fillColor=QColor(), double fillAlpha=1.0,
                const CQChartsFillPattern &pattern=CQChartsFillPattern()) const;

  void updatePenBrushState(QPen &pen, QBrush &brush, DrawType drawType=DrawType::BOX) const;

  //---

  // text utilities
  QFont calcFont(const CQChartsFont &font) const;

  void setPainterFont(CQChartsPaintDevice *painter, const CQChartsFont &font) const;

  void adjustTextOptions(CQChartsTextOptions &textOptions) const;

  //---

  // conversion utilities
  QPointF positionToParent(const CQChartsPosition &pos) const;

  double lengthParentWidth (const CQChartsLength &len) const;
  double lengthParentHeight(const CQChartsLength &len) const;

  double lengthPixelWidth (const CQChartsLength &len) const;
  double lengthPixelHeight(const CQChartsLength &len) const;

  CQChartsGeom::Point windowToPixel(const CQChartsGeom::Point &w) const;
  CQChartsGeom::BBox  windowToPixel(const CQChartsGeom::BBox  &w) const;

  CQChartsGeom::Point pixelToWindow(const CQChartsGeom::Point &w) const;
  CQChartsGeom::BBox  pixelToWindow(const CQChartsGeom::BBox  &w) const;

  double pixelToWindowWidth (double pw) const;
  double pixelToWindowHeight(double ph) const;

  //---

  QColor backgroundColor() const;

  //---

 protected:
  CQChartsView* view_ { nullptr }; //!< parent view
  CQChartsPlot* plot_ { nullptr }; //!< parent plot
};

#endif
