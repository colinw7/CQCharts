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

  Q_PROPERTY(CompositeType compositeType  READ compositeType  WRITE setCompositeType )
  Q_PROPERTY(bool          commonXRange   READ isCommonXRange WRITE setCommonXRange  )
  Q_PROPERTY(bool          commonYRange   READ isCommonYRange WRITE setCommonYRange  )
  Q_PROPERTY(bool          currentPlotInd READ currentPlotInd WRITE setCurrentPlotInd)

  Q_ENUMS(CompositeType);

 public:
  enum class CompositeType {
    NONE,
    X1X2,
    Y1Y2,
    TABBED
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

  void addPlot(Plot *plot);

  //---

  void addProperties() override;

  //---

 public:
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

  Obj *insideObject() const override;

  QString insideObjectText() const override;

  //---

  void initPlotObjs() override;

  void initObjTree() override;

  BBox calcExtraFitBBox() const override;

  void updateAxisRanges(const BBox &adjustedRange) override;

  void applyDataRange(bool propagate=true) override;

  //---

  // key
 public:
  bool isKeyVisible() const override;
  void setKeyVisible(bool b) override;

  bool isKeyVisibleAndNonEmpty() const override;

  bool isColorKey() const override;
  void setColorKey(bool b) override;

  //---

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

  void resetExtraBBox() const override;

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

  bool addMenuItems(QMenu *menu) override;

  void resetKeyItems() override;

  //---

  bool selectPress  (const Point &p, SelMod selMod) override;
  bool selectMove   (const Point &p, Constraints constraints, bool first=false) override;
  bool selectRelease(const Point &p) override;

  //---

  bool isLayerActive(const Layer::Type &type) const override;

  void invalidateOverlay() override;
  void invalidateLayers() override;
  void invalidateLayer(const CQChartsBuffer::Type &layerType) override;

  //---

  BBox adjustDataRangeBBox(const BBox &bbox) const override;

  //---

  bool isComposite() const override;

  Plot *currentPlot() const;
  void setCurrentPlot(Plot *plot);

  //---

  bool hasTitle() const override;

  bool hasXAxis() const override;
  bool hasYAxis() const override;

  Title *title() const override;

  Axis *xAxis() const override;
  Axis *yAxis() const override;

  PlotKey *key() const override;

  //---

  int childPlotIndex(const Plot *) const override;
  int numChildPlots() const override;
  Plot *childPlot(int i) const override;

  int currentPlotInd() const;
  void setCurrentPlotInd(int i);

  //---

  bool isX1X2(bool checkVisible=true) const override;
  bool isY1Y2(bool checkVisible=true) const override;

  //---

  void groupedObjsAtPoint(const Point &p, Objs &objs,
                          const Constraints &constraints) const override;

  void groupedObjsIntersectRect(const BBox &r, Objs &objs,
                                bool inside, bool select=false) const override;

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

  double dataOffsetX() const override;
  void setDataOffsetX(double x) override;

  double dataOffsetY() const override;
  void setDataOffsetY(double y) override;

  //---

  const CQChartsPlot::ZoomData &zoomData() const override;
  void setZoomData(const ZoomData &zoomData) override;

  //---

 protected:
  void pixelToWindowI(double px, double py, double &wx, double &wy) const override;
  void viewToWindowI (double vx, double vy, double &wx, double &wy) const override;

  void windowToPixelI(double wx, double wy, double &px, double &py) const override;
  void windowToViewI (double wx, double wy, double &vx, double &vy) const override;

  //---

 protected:
  CQChartsPlotCustomControls *createCustomControls(CQCharts *charts) override;

 public slots:
  void zoomFull(bool notify=true) override;

 private slots:
  void currentPlotSlot();

 private:
  void updatePlots();

  void setOverlayAxisLabels();

 private:
  using Plots = std::vector<Plot*>;

  Plots         plots_;
  Plot*         currentPlot_   { nullptr };
  CompositeType compositeType_ { CompositeType::NONE };
  bool          commonXRange_  { true };
  bool          commonYRange_  { true };
};

#endif
