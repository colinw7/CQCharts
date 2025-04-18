#ifndef CQChartsCompositePlot_H
#define CQChartsCompositePlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>

//---

/*!
 * \brief Composite plot type
 * \ingroup Charts
 */
class CQChartsCompositePlotType : public CQChartsPlotType {
 public:
  CQChartsCompositePlotType();

  QString name() const override { return "composite"; }
  QString desc() const override { return "Composite"; }

  void addParameters() override;

  bool canProbe() const override;

  bool hasObjs() const override;

  QString description() const override;

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsCompositePlot;

//---

/*!
 * \brief Composite Plot
 * \ingroup Charts
 */
class CQChartsCompositePlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(CompositeType  compositeType  READ compositeType  WRITE setCompositeType )
  Q_PROPERTY(bool           commonXRange   READ isCommonXRange WRITE setCommonXRange  )
  Q_PROPERTY(bool           commonYRange   READ isCommonYRange WRITE setCommonYRange  )
  Q_PROPERTY(int            currentPlotInd READ currentPlotInd WRITE setCurrentPlotInd)
  Q_PROPERTY(int            numColumns     READ numColumns     WRITE setNumColumns    )
  Q_PROPERTY(CQChartsLength headerHeight   READ headerHeight   WRITE setHeaderHeight  )
  Q_PROPERTY(CQChartsLength rowHeight      READ rowHeight      WRITE setRowHeight     )

  Q_ENUMS(CompositeType)

 public:
  enum class CompositeType {
    NONE,
    X1X2,
    Y1Y2,
    TABBED,
    TABLE
  };

 public:
  using Plot = CQChartsPlot;

 public:
  CQChartsCompositePlot(View *view, const ModelP &model);
 ~CQChartsCompositePlot();

  //---

  void init() override;
  void term() override;

  //---

  const CompositeType &compositeType() const { return compositeType_; }
  void setCompositeType(const CompositeType &t);

  bool isCommonXRange() const { return commonXRange_; }
  void setCommonXRange(bool b);

  bool isCommonYRange() const { return commonYRange_; }
  void setCommonYRange(bool b);

  //---

  int numColumns() const { return numColumns_; }
  void setNumColumns(int i);

  const Length &headerHeight() const { return headerHeight_; }
  void setHeaderHeight(const Length &v);

  const Length &rowHeight() const { return rowHeight_; }
  void setRowHeight(const Length &v);

  //---

  void addPlot(Plot *plot);

  //---

  void addProperties() override;

  //---

 public:
  void preResize() override;

  //---

  Range calcRange() const override;

  void postUpdateObjs() override;

  void updateAndAdjustRanges() override;

  //---

  const Range &dataRange() const override;
  void setDataRange(const Range &r, bool update=true) override;

  void resetDataRange(bool updateRange=true, bool updateObjs=true) override;

  //---

 public:
  bool hasPlotObjs() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void resetInsideObjs() override;

  //--

 public:
  void doPostObjTree() override;

  bool isPlotObjTreeSet() const override;
  void setPlotObjTreeSet(bool b) override;

  //----

  void clearPlotObjects() override;
  void clearInsideObjects() override;

  void invalidateObjTree() override;

  bool updateInsideObjects(const Point &w, Constraints constraints) override;

  bool setInsideObjects(const Point &w, Objs &objs) override;

  Obj *insideObject() const override;

  QString insideObjectText() const override;

  //---

  void initPlotObjs() override;

  void initObjTree() override;

  //---

  void updatePlotKeyPosition(Plot *plot, bool force=false) override;

  //---

  BBox calcExtraFitBBox() const override;

  void updateAxisRanges(const BBox &adjustedRange) override;

  void applyDataRange(bool propagate=true) override;

  //---

  std::vector<Plot *> getPlots() const;

  //---

#if 0
  // key
 public:
  bool isKeyVisible() const override;
  void setKeyVisible(bool b) override;

  bool isKeyVisibleAndNonEmpty() const override;

  bool isColorKey() const override;
  void setColorKey(bool b) override;
#endif

  //---

  // equal scale
  bool isEqualScale() const override;
  void setEqualScale(bool b) override;

  //---

  // fit
 public:
  void autoFitOne() override;

  void autoFit() override;

  bool isAutoFit() const override;
  void setAutoFit(bool b) override;

  const PlotMargin &fitMargin() const override;
  void setFitMargin(const PlotMargin &m) override;

  void resetExtraFitBBox() const override;

 protected:
  bool needsAutoFit() const override;
  void setNeedsAutoFit(bool b) override;

 public:
  BBox calcViewBBox() const override;

  BBox adjustedViewBBox(const Plot *plot) const;

  //---

 public:
  void waitRange() override;
  void waitDraw() override;
  void waitObjs() override;
  void waitTree() override;

  //---

  // invert x/y
  bool isInvertX() const override;
  void setInvertX(bool b) override;

  bool isInvertY() const override;
  void setInvertY(bool b) override;

  //---

  // log x/y
  bool isLogX() const override;
  void setLogX(bool b) override;

  bool isLogY() const override;
  void setLogY(bool b) override;

  //---

  bool hasBackground() const override;
  bool hasForeground() const override;

  bool hasBgAxes() const override;
  bool hasFgAxes() const override;

  bool hasGroupedBgKey() const override;
  bool hasGroupedFgKey() const override;

  bool hasObjs(const CQChartsLayer::Type &layerType) const override;

  //---

  bool isBufferLayers() const override;

  void drawPlotParts(QPainter *painter) const override;

  void drawBackgroundDeviceParts(PaintDevice *device,
                                 const BackgroundParts &bgParts) const override;

  void drawMiddleDeviceParts(PaintDevice *device, bool bg, bool mid, bool fg) const override;

  void drawForegroundDeviceParts(PaintDevice *device,
                                 const ForegroundParts &fgParts) const override;

  void drawOverlayDeviceParts(PaintDevice *device,
                              const OverlayParts &overlayParts) const override;

  void drawBgAxes(PaintDevice *device) const override;
  void drawFgAxes(PaintDevice *device) const override;

  void drawTitle(PaintDevice *device) const override;

  //---

  bool addMenuItems(QMenu *menu, const Point &p) override;

  void resetPlotKeyItems(Plot *plot, bool add=true) override;

  //---

  //! plot select interface
  bool handleSelectPress  (const Point &p, SelMod selMod) override;
  bool handleSelectMove   (const Point &p, Constraints constraints, bool first=false) override;
  bool handleSelectRelease(const Point &p) override;

  //---

  bool isLayerActive(const Layer::Type &type) const override;

  void invalidateOverlay() override;
  void invalidateLayers() override;
  void invalidateLayer(const CQChartsBuffer::Type &layerType) override;

  //---

  BBox adjustDataRangeBBox(const BBox &bbox) const override;

  //---

  bool isComposite() const override;

  Plot *currentPlot() const override;

  void setCurrentPlot(Plot *plot);

  //---

  bool hasTitle() const override;

  bool hasXAxis() const override;
  bool hasYAxis() const override;

  Title *title() const override;

  Axis *xAxis() const override;
  Axis *yAxis() const override;

#if 0
  PlotKey *key() const override;
#endif

  //---

  int childPlotIndex(const Plot *) const override;
  int numChildPlots() const override;
  Plot *childPlot(int i) const override;

  //! get/set current plot ind
  int currentPlotInd() const;
  void setCurrentPlotInd(int i);

  //---

  void groupedObjsAtPoint(const Point &p, Objs &objs,
                          const Constraints &constraints) const override;

  void groupedObjsIntersectRect(const BBox &r, Objs &objs, bool inside,
                                const Constraints &constraints) const override;

  //---

  bool keyPress(int key, int modifier) override;

  //---

  bool allowZoomX() const override;
  bool allowZoomY() const override;

  bool allowPanX() const override;
  bool allowPanY() const override;

  //---

  double dataScaleX() const override;
  void setDataScaleX(double r) override;

  double dataScaleY() const override;
  void setDataScaleY(double r) override;

  Point dataOffset() const override;
  void setDataOffset(const Point &p) override;

  //---

  const ZoomData &zoomData() const override;
  void setZoomData(const ZoomData &zoomData) override;

  //---

 protected:
  void placeTable();

  void pixelToWindowI(double px, double py, double &wx, double &wy) const override;
  void viewToWindowI (double vx, double vy, double &wx, double &wy) const override;

  void windowToPixelI(double wx, double wy, double &px, double &py) const override;
  void windowToViewI (double wx, double wy, double &vx, double &vy) const override;

  //---

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 public Q_SLOTS:
  void zoomFull(bool notify=true) override;

 private Q_SLOTS:
  void currentPlotSlot();

 private:
  void updatePlots();

  void setOverlayAxisLabels();

 private:
  using PlotP = QPointer<Plot>;
  using Plots = std::vector<PlotP>;

  Plots plots_;
  PlotP currentPlot_;

  CompositeType compositeType_ { CompositeType::NONE };

  bool commonXRange_  { true };
  bool commonYRange_  { true };

  // table data
  int    numColumns_    { 1 };
  Length headerHeight_;
  Length rowHeight_;

  mutable bool inSelectMove_ { false };
};

#endif
