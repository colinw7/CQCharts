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

  Range calcRange() const override;

  void postUpdateObjs() override;

  //---

  bool hasPlotObjs() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void resetInsideObjs() override;

  //--

  void clearPlotObjects() override;
  void clearInsideObjects() override;

  void invalidateObjTree() override;

  bool updateInsideObjects(const Point &w, Constraints constraints) override;

  Obj *insideObject() const override;

  QString insideObjectText() const override;

  void initPlotObjs() override;

  void initObjTree() override;

  BBox calcExtraFitBBox() const override;

  void updateAxisRanges(const BBox &adjustedRange) override;

  //---

  BBox adjustedViewBBox(const Plot *plot) const override;

  //---

  void waitRange() override;
  void waitDraw() override;
  void waitObjs() override;
  void waitTree() override;

  //---

  bool hasBackground() const override;
  bool hasForeground() const override;

  bool hasTitle() const override;

  bool hasXAxis() const override;
  bool hasYAxis() const override;

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

  Plot *currentPlot() const;
  void setCurrentPlot(Plot *plot);

  int childPlotIndex(const Plot *) const override;
  int numChildPlots() const override;
  Plot *childPlot(int i) const override;

  int currentPlotInd() const;
  void setCurrentPlotInd(int i);

  //---

  void groupedObjsAtPoint(const Point &p, Objs &objs,
                          const Constraints &constraints) const override;

  //---

  bool keyPress(int key, int modifier) override;

 private:
  template<typename FUNCTION>
  bool checkCurrentPlots(FUNCTION f) const {
    for (auto &plot : plots_) {
      if (! plot->isVisible())
        continue;

      if (compositeType_ == CompositeType::TABBED && plot != currentPlot())
        continue;

      if (f(plot))
        return true;
    }

    return false;
  }

 protected:
  CQChartsPlotCustomControls *createCustomControls(CQCharts *charts) override;

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
