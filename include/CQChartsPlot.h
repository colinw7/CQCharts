#ifndef CQChartsPlot_H
#define CQChartsPlot_H

#include <CQChartsPlotModelVisitor.h>
#include <CQChartsView.h>
#include <CQChartsObj.h>
#include <CQChartsColor.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsObjData.h>
#include <CQChartsGroupData.h>
#include <CQChartsPosition.h>
#include <CQChartsRect.h>
#include <CQChartsPolygon.h>
#include <CQChartsTextOptions.h>
#include <CQChartsLayer.h>
#include <CQChartsImage.h>
#include <CQChartsUtil.h>
#include <CQChartsTypes.h>
#include <CQChartsGeom.h>
#include <CQChartsPlotMargin.h>
#include <CQChartsOptReal.h>
#include <CQChartsColorStops.h>
#include <CQChartsPaletteName.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsModelTypes.h>
#include <CHRTime.h>

#include <QAbstractItemModel>
#include <QFrame>
#include <QTimer>
#include <QPointer>

#include <future>
#include <memory>
#include <set>

#include <boost/optional.hpp>

class CQCharts;
class CQChartsView;
class CQChartsPlotType;
class CQChartsPlot;
class CQChartsCompositePlot;
class CQChartsAxis;
class CQChartsPlotKey;
class CQChartsKeyItem;
class CQChartsKeyColorBox;
class CQChartsTitle;
class CQChartsPlotObj;
class CQChartsPlotObjTree;

class CQChartsAnnotation;
class CQChartsArrowAnnotation;
class CQChartsAxisAnnotation;
class CQChartsEllipseAnnotation;
class CQChartsImageAnnotation;
class CQChartsKeyAnnotation;
class CQChartsPieSliceAnnotation;
class CQChartsPointAnnotation;
class CQChartsPointSetAnnotation;
class CQChartsPolygonAnnotation;
class CQChartsPolylineAnnotation;
class CQChartsRectangleAnnotation;
class CQChartsTextAnnotation;
class CQChartsValueSetAnnotation;
class CQChartsButtonAnnotation;

class CQChartsPlotParameter;
class CQChartsDisplayRange;
class CQChartsValueSet;
class CQChartsModelColumnDetails;
class CQChartsModelData;
class CQChartsEditHandles;
class CQChartsTableTip;
class CQChartsPoints;
class CQChartsReals;

class CQChartsScriptPaintDevice;
class CQChartsHtmlPaintDevice;
class CQChartsSVGPaintDevice;

class CQPropertyViewModel;
class CQPropertyViewItem;

class QSortFilterProxyModel;
class QItemSelectionModel;
class QTextBrowser;
class QRubberBand;
class QMenu;
class QPainter;

//----

/*!
 * \brief Update plot timer
 * \ingroup Charts
 */
class CQChartsPlotUpdateTimer : public QTimer {
 public:
  CQChartsPlotUpdateTimer(CQChartsPlot *plot) :
   plot_(plot) {
    setSingleShot(true);
  }

  CQChartsPlot *plot() const { return plot_; }

 private:
  CQChartsPlot* plot_ { nullptr };
};

//----

CQCHARTS_NAMED_SHAPE_DATA(Plot,plot)
CQCHARTS_NAMED_SHAPE_DATA(Data,data)
CQCHARTS_NAMED_SHAPE_DATA(Fit,fit)

/*!
 * \brief Base class for Plot
 * \ingroup Charts
 */
class CQChartsPlot : public CQChartsObj,
 public CQChartsObjPlotShapeData<CQChartsPlot>,
 public CQChartsObjDataShapeData<CQChartsPlot>,
 public CQChartsObjFitShapeData <CQChartsPlot> {
  Q_OBJECT

  // view
  Q_PROPERTY(QString viewId READ viewId)

  // type
  Q_PROPERTY(QString typeStr READ typeStr)

  // name
  Q_PROPERTY(QString name READ name WRITE setName)

  // generic columns and control
  Q_PROPERTY(CQChartsColumn  idColumn       READ idColumn       WRITE setIdColumn      )
  Q_PROPERTY(CQChartsColumns tipColumns     READ tipColumns     WRITE setTipColumns    )
  Q_PROPERTY(CQChartsColumn  visibleColumn  READ visibleColumn  WRITE setVisibleColumn )
  Q_PROPERTY(CQChartsColumn  colorColumn    READ colorColumn    WRITE setColorColumn   )
  Q_PROPERTY(CQChartsColumn  fontColumn     READ fontColumn     WRITE setFontColumn    )
  Q_PROPERTY(CQChartsColumn  imageColumn    READ imageColumn    WRITE setImageColumn   )
  Q_PROPERTY(CQChartsColumns controlColumns READ controlColumns WRITE setControlColumns)

  // color map
  Q_PROPERTY(ColorType          colorType       READ colorType       WRITE setColorType      )
  Q_PROPERTY(bool               colorMapped     READ isColorMapped   WRITE setColorMapped    )
  Q_PROPERTY(double             colorMapMin     READ colorMapMin     WRITE setColorMapMin    )
  Q_PROPERTY(double             colorMapMax     READ colorMapMax     WRITE setColorMapMax    )
  Q_PROPERTY(QString            colorMapPalette READ colorMapPalette WRITE setColorMapPalette)
  Q_PROPERTY(CQChartsColorStops colorXStops     READ colorXStops     WRITE setColorXStops    )
  Q_PROPERTY(CQChartsColorStops colorYStops     READ colorYStops     WRITE setColorYStops    )

  // rectangle and data range
  Q_PROPERTY(CQChartsGeom::BBox viewRect READ viewBBox WRITE setViewBBox)
  Q_PROPERTY(CQChartsGeom::BBox range    READ range    WRITE setRange   )

  Q_PROPERTY(CQChartsGeom::BBox innerViewRect READ innerViewBBox)
  Q_PROPERTY(CQChartsGeom::BBox calcDataRect  READ calcDataRect )
  Q_PROPERTY(CQChartsGeom::BBox outerDataRect READ outerDataRect)
  Q_PROPERTY(CQChartsGeom::BBox dataRect      READ dataRect     )

  // scaling
  Q_PROPERTY(bool   equalScale  READ isEqualScale WRITE setEqualScale   )
  Q_PROPERTY(double dataScaleX  READ dataScaleX   WRITE updateDataScaleX)
  Q_PROPERTY(double dataScaleY  READ dataScaleY   WRITE updateDataScaleY)
  Q_PROPERTY(double dataOffsetX READ dataOffsetX  WRITE setDataOffsetX  )
  Q_PROPERTY(double dataOffsetY READ dataOffsetY  WRITE setDataOffsetY  )

  // range
  Q_PROPERTY(CQChartsOptReal xmin READ xmin WRITE setXMin)
  Q_PROPERTY(CQChartsOptReal ymin READ ymin WRITE setYMin)
  Q_PROPERTY(CQChartsOptReal xmax READ xmax WRITE setXMax)
  Q_PROPERTY(CQChartsOptReal ymax READ ymax WRITE setYMax)

  // every
  Q_PROPERTY(bool everyEnabled READ isEveryEnabled WRITE setEveryEnabled)
  Q_PROPERTY(int  everyStart   READ everyStart     WRITE setEveryStart  )
  Q_PROPERTY(int  everyEnd     READ everyEnd       WRITE setEveryEnd    )
  Q_PROPERTY(int  everyStep    READ everyStep      WRITE setEveryStep   )

  // filter
  Q_PROPERTY(QString filterStr        READ filterStr        WRITE setFilterStr       )
  Q_PROPERTY(QString visibleFilterStr READ visibleFilterStr WRITE setVisibleFilterStr)

  Q_PROPERTY(bool skipBad READ isSkipBad WRITE setSkipBad)

  // inner margin
  Q_PROPERTY(CQChartsLength innerMarginLeft   READ innerMarginLeft   WRITE setInnerMarginLeft  )
  Q_PROPERTY(CQChartsLength innerMarginTop    READ innerMarginTop    WRITE setInnerMarginTop   )
  Q_PROPERTY(CQChartsLength innerMarginRight  READ innerMarginRight  WRITE setInnerMarginRight )
  Q_PROPERTY(CQChartsLength innerMarginBottom READ innerMarginBottom WRITE setInnerMarginBottom)

  // outer margin
  Q_PROPERTY(CQChartsLength outerMarginLeft   READ outerMarginLeft   WRITE setOuterMarginLeft  )
  Q_PROPERTY(CQChartsLength outerMarginTop    READ outerMarginTop    WRITE setOuterMarginTop   )
  Q_PROPERTY(CQChartsLength outerMarginRight  READ outerMarginRight  WRITE setOuterMarginRight )
  Q_PROPERTY(CQChartsLength outerMarginBottom READ outerMarginBottom WRITE setOuterMarginBottom)

  // title and associated filename (if any)
  Q_PROPERTY(QString title    READ titleStr WRITE setTitleStr)
  Q_PROPERTY(QString fileName READ fileName WRITE setFileName)
  Q_PROPERTY(QString xLabel   READ xLabel   WRITE setXLabel  )
  Q_PROPERTY(QString yLabel   READ yLabel   WRITE setYLabel  )

  // plot area
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Plot,plot)

  Q_PROPERTY(CQChartsSides plotBorderSides READ plotBorderSides WRITE setPlotBorderSides)
  Q_PROPERTY(bool          plotClip        READ isPlotClip      WRITE setPlotClip       )

  // data area
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Data,data)

  Q_PROPERTY(CQChartsSides dataBorderSides READ dataBorderSides WRITE setDataBorderSides)
  Q_PROPERTY(bool          dataClip        READ isDataClip      WRITE setDataClip       )

  // fit area
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Fit,fit)

  Q_PROPERTY(CQChartsSides fitBorderSides READ fitBorderSides WRITE setFitBorderSides)

  // key
  Q_PROPERTY(bool keyVisible READ isKeyVisible WRITE setKeyVisible)
  Q_PROPERTY(bool colorKey   READ isColorKey   WRITE setColorKey  )

  // font
  Q_PROPERTY(CQChartsFont font       READ font       WRITE setFont)
  Q_PROPERTY(CQChartsFont tabbedFont READ tabbedFont WRITE setTabbedFont)

  // default palette
  Q_PROPERTY(CQChartsPaletteName defaultPalette READ defaultPalette WRITE setDefaultPalette)

  // scaled fonts
  Q_PROPERTY(double minScaleFontSize READ minScaleFontSize WRITE setMinScaleFontSize)
  Q_PROPERTY(double maxScaleFontSize READ maxScaleFontSize WRITE setMaxScaleFontSize)

  // connect
  Q_PROPERTY(bool overlay READ isOverlay WRITE setOverlay)
  Q_PROPERTY(bool x1x2    READ isX1X2    WRITE setX1X2   )
  Q_PROPERTY(bool y1y2    READ isY1Y2    WRITE setY1Y2   )
  Q_PROPERTY(bool tabbed  READ isTabbed  WRITE setTabbed )

  // misc
  Q_PROPERTY(bool followMouse READ isFollowMouse WRITE setFollowMouse)
  Q_PROPERTY(bool invertX     READ isInvertX     WRITE setInvertX    )
  Q_PROPERTY(bool invertY     READ isInvertY     WRITE setInvertY    )
//Q_PROPERTY(bool logX        READ isLogX        WRITE setLogX       )
//Q_PROPERTY(bool logY        READ isLogY        WRITE setLogY       )
  Q_PROPERTY(bool autoFit     READ isAutoFit     WRITE setAutoFit    )

  // preview
  Q_PROPERTY(bool preview        READ isPreview      WRITE setPreview       )
  Q_PROPERTY(int  previewMaxRows READ previewMaxRows WRITE setPreviewMaxRows)

  Q_PROPERTY(bool queueUpdate    READ isQueueUpdate  WRITE setQueueUpdate   )
  Q_PROPERTY(bool showBoxes      READ showBoxes      WRITE setShowBoxes     )

  Q_ENUMS(ColorType)

 public:
  enum ColorType {
    AUTO    = int(CQChartsColorType::AUTO),
    SET     = int(CQChartsColorType::SET),
    GROUP   = int(CQChartsColorType::GROUP),
    INDEX   = int(CQChartsColorType::INDEX),
    X_VALUE = int(CQChartsColorType::X_VALUE),
    Y_VALUE = int(CQChartsColorType::Y_VALUE)
  };

  using Point    = CQChartsGeom::Point;
  using BBox     = CQChartsGeom::BBox;
  using Polygon  = CQChartsGeom::Polygon;
  using Range    = CQChartsGeom::Range;
  using Size     = CQChartsGeom::Size;
  using RMinMax  = CQChartsGeom::RMinMax;
  using Polygons = CQChartsGeom::Polygons;

  using SelMod = CQChartsSelMod;

  // associated plot for overlay/y1y2
  struct ConnectData {
    CQChartsPlot* parent  { nullptr }; //!< parent plot
    bool          x1x2    { false };   //!< is double x axis plot
    bool          y1y2    { false };   //!< is double y axis plot
    bool          overlay { false };   //!< is overlay plot
    bool          tabbed  { false };   //!< is tabbed plot
    bool          current { false };   //!< is current
    BBox          tabRect;             //!< tab rect
    CQChartsPlot* next    { nullptr }; //!< next plot
    CQChartsPlot* prev    { nullptr }; //!< previous plot

    ConnectData() { }

    void reset() {
      x1x2    = false;
      y1y2    = false;
      overlay = false;
      tabbed  = false;
      current = false;
      next    = nullptr;
      prev    = nullptr;
    }
  };

  struct ProbeValue {
    double  value;
    QString label;

    ProbeValue() = default;

    ProbeValue(double value, const QString &label="") :
     value(value), label(label) {
    }
  };

  struct ProbeData {
    using Values = std::vector<ProbeValue>;

    Point           p;
    Values          xvals;
    Values          yvals;
    Qt::Orientation direction { Qt::Vertical };
    bool            both      { false };
  };

  using ModelP          = QSharedPointer<QAbstractItemModel>;
  using SelectionModelP = QPointer<QItemSelectionModel>;

  using PlotObjs    = std::vector<CQChartsPlotObj*>;
  using Annotations = std::vector<CQChartsAnnotation *>;
  using Objs        = std::vector<CQChartsObj*>;

  using ModelIndices = std::vector<QModelIndex>;

  using ColumnType = CQBaseModelType;

  using Plots = std::vector<CQChartsPlot*>;

  using Buffers = std::map<CQChartsBuffer::Type,CQChartsBuffer *>;
  using Layers  = std::map<CQChartsLayer::Type,CQChartsLayer *>;

  using ColorInd = CQChartsUtil::ColorInd;

  using DrawType = CQChartsView::DrawType;

  using Color       = CQChartsColor;
  using Alpha       = CQChartsAlpha;
  using Length      = CQChartsLength;
  using Angle       = CQChartsAngle;
  using FillPattern = CQChartsFillPattern;
  using LineDash    = CQChartsLineDash;

 public:
  CQChartsPlot(CQChartsView *view, CQChartsPlotType *type, const ModelP &model);

  virtual ~CQChartsPlot();

  //---

  CQChartsView *view() const { return view_; }

  CQChartsPlotType *type() const { return type_; }

  QString viewId() const;

  QString typeStr() const;

  QString calcName() const;

  //---

  ModelP model() const { return model_; }
  virtual void setModel(const ModelP &model);

  //---

  virtual bool isHierarchical() const;

  virtual int numGroups() const { return 0; }

  //---

  CQCharts *charts() const;

  QString typeName() const;

  QString pathId() const;

  //---

  bool calcVisible() const;
  void setVisible(bool b) override;

  void setSelected(bool b) override;

  //---

  const QString &name() const { return name_; }
  void setName(const QString &name) { name_ = name; }

  //---

  bool isUpdatesEnabled() const { return updatesData_.enabled == 0; }
  void setUpdatesEnabled(bool b, bool update=true);

  //---

  bool isUpdateRangeAndObjs() const { return updatesData_.updateRangeAndObjs; }
  bool isUpdateObjs() const { return updatesData_.updateObjs; }

  void updateRange();
  void updateRangeAndObjs();
  void updateObjs();

  //---

  void applyVisibleFilter();

  //---

  void drawBackground();
  void drawForeground();
  void drawObjs();

  //---

  void writeScript(CQChartsScriptPaintDevice *device) const;

  void writeScriptRange(CQChartsScriptPaintDevice *device) const;

  void writeSVG(CQChartsSVGPaintDevice *device) const;

  void writeHtml(CQChartsHtmlPaintDevice *device) const;

  //---

  const CQChartsDisplayRange &displayRange() const;
  void setDisplayRange(const CQChartsDisplayRange &r);

  const Range &dataRange() const { return dataRange_; }
  void setDataRange(const Range &r, bool update=true);

  void resetDataRange(bool updateRange=true, bool updateObjs=true);

  //---

  struct ZoomData {
    Point dataScale  { 1.0, 1.0 }; //!< data scale (zoom in x/y direction)
    Point dataOffset { 0.0, 0.0 }; //!< data offset (pan)
  };

  double dataScaleX() const;
  void setDataScaleX(double r);

  double dataScaleY() const;
  void setDataScaleY(double r);

  double dataOffsetX() const;
  void setDataOffsetX(double x);

  double dataOffsetY() const;
  void setDataOffsetY(double y);

  const ZoomData &zoomData() const { return zoomData_; }
  void setZoomData(const ZoomData &zoomData);

  void updateDataScaleX(double r);
  void updateDataScaleY(double r);

  //---

  const CQChartsOptReal &xmin() const { return xmin_; }
  void setXMin(const CQChartsOptReal &r);

  const CQChartsOptReal &xmax() const { return xmax_; }
  void setXMax(const CQChartsOptReal &r);

  const CQChartsOptReal &ymin() const { return ymin_; }
  void setYMin(const CQChartsOptReal &r);

  const CQChartsOptReal &ymax() const { return ymax_; }
  void setYMax(const CQChartsOptReal &r);

  //---

  int isEveryEnabled() const { return everyData_.enabled; }
  void setEveryEnabled(bool b);

  int everyStart() const { return everyData_.start; }
  void setEveryStart(int i);

  int everyEnd() const { return everyData_.end; }
  void setEveryEnd(int i);

  int everyStep() const { return everyData_.step; }
  void setEveryStep(int i);

  //---

  const QString &filterStr() const { return filterStr_; }
  void setFilterStr(const QString &s);

  const QString &visibleFilterStr() const { return visibleFilterStr_; }
  void setVisibleFilterStr(const QString &s);

  //---

  bool isSkipBad() const { return skipBad_; }
  void setSkipBad(bool b);

  //---

  const QString &titleStr() const { return titleStr_; }
  void setTitleStr(const QString &s);

  const QString &fileName() const { return fileName_; }
  void setFileName(const QString &s) { fileName_ = s; }

  //---

  QString xLabel() const;
  void setXLabel(const QString &s);

  QString yLabel() const;
  void setYLabel(const QString &s);

  //---

  // plot area
  const CQChartsSides &plotBorderSides() const { return plotBorderSides_; }
  void setPlotBorderSides(const CQChartsSides &s);

  bool isPlotClip() const { return plotClip_; }
  void setPlotClip(bool b);

  //---

  // data area
  const CQChartsSides &dataBorderSides() const { return dataBorderSides_; }
  void setDataBorderSides(const CQChartsSides &s);

  bool isDataClip() const { return dataClip_; }
  void setDataClip(bool b);

  //---

  // fit area
  const CQChartsSides &fitBorderSides() const { return fitBorderSides_; }
  void setFitBorderSides(const CQChartsSides &s);

  //---

  // font
  const CQChartsFont &font() const { return font_; }
  virtual void setFont(const CQChartsFont &f);

  //---

  // default palette
  const CQChartsPaletteName &defaultPalette() const { return defaultPalette_; }
  void setDefaultPalette(const CQChartsPaletteName &name);

  //---

  // scaled font size
  double minScaleFontSize() const { return minScaleFontSize_; }
  void setMinScaleFontSize(double r) { minScaleFontSize_ = r; }

  double maxScaleFontSize() const { return maxScaleFontSize_; }
  void setMaxScaleFontSize(double r) { maxScaleFontSize_ = r; }

  //---

  // key
  bool isKeyVisible() const;
  void setKeyVisible(bool b);

  bool isKeyVisibleAndNonEmpty() const;

  bool isColorKey() const { return colorKey_; }
  void setColorKey(bool b);

  //---

  bool isEqualScale() const { return equalScale_; }
  void setEqualScale(bool b);

  bool isFollowMouse() const { return followMouse_; }
  void setFollowMouse(bool b) { followMouse_ = b; }

  bool isAutoFit() const { return autoFit_; }
  void setAutoFit(bool b) { autoFit_ = b; }

  //---

  bool isPreview() const { return preview_; }
  void setPreview(bool b) { preview_ = b; }

  int previewMaxRows() const { return previewMaxRows_; }
  void setPreviewMaxRows(int i) { previewMaxRows_ = i; }

  //---

  bool isSequential() const { return sequential_; }

  bool isQueueUpdate() const { return queueUpdate_; }
  void setQueueUpdate(bool b) { queueUpdate_ = b; }

  //---

  bool isOverview() const { return overview_; }
  void setOverview(bool b) { overview_ = b; }

  //---

  bool showBoxes() const { return showBoxes_; }
  void setShowBoxes(bool b);

  //---

  // get/set bbox in view range
  const BBox &viewBBox() const { return viewBBox_; }
  void setViewBBox(const BBox &bbox);

  BBox calcViewBBox() const;
  virtual BBox adjustedViewBBox(const CQChartsPlot *plot) const;

  // get inner view bbox
  BBox innerViewBBox() const;

  // get/set data range
  BBox range() const;
  void setRange(const BBox &bbox);

  //---

  BBox calcDataRect () const;
  BBox outerDataRect() const;
  BBox dataRect     () const;

  virtual Range getCalcDataRange() const;

  //---

  // inner margin
  const CQChartsPlotMargin &innerMargin() const { return innerMargin_; }

  const Length &innerMarginLeft() const { return innerMargin().left(); }
  void setInnerMarginLeft(const Length &l);

  const Length &innerMarginTop() const { return innerMargin().top(); }
  void setInnerMarginTop(const Length &t);

  const Length &innerMarginRight() const { return innerMargin().right(); }
  void setInnerMarginRight(const Length &r);

  const Length &innerMarginBottom() const { return innerMargin().bottom(); }
  void setInnerMarginBottom(const Length &b);

  void setInnerMargin(const Length &l, const Length &t, const Length &r, const Length &b);
  void setInnerMargin(const CQChartsPlotMargin &m);

  // outer margin
  const CQChartsPlotMargin &outerMargin() const { return outerMargin_; }

  const Length &outerMarginLeft() const { return outerMargin().left(); }
  void setOuterMarginLeft(const Length &l);

  const Length &outerMarginTop() const { return outerMargin().top(); }
  void setOuterMarginTop(const Length &t);

  const Length &outerMarginRight() const { return outerMargin().right(); }
  void setOuterMarginRight(const Length &r);

  const Length &outerMarginBottom() const { return outerMargin().bottom(); }
  void setOuterMarginBottom(const Length &b);

  void setOuterMargin(const Length &l, const Length &t, const Length &r, const Length &b);
  void setOuterMargin(const CQChartsPlotMargin &m);

  //---

  // aspect ratio
  double aspect() const;

  //---

  CQChartsAxis *xAxis() const { return xAxis_; }
  CQChartsAxis *yAxis() const { return yAxis_; }

  virtual CQChartsAxis *mappedXAxis() const { return xAxis(); }
  virtual CQChartsAxis *mappedYAxis() const { return yAxis(); }

  virtual bool xAxisName(QString &, const QString& ="") const { return false; }
  virtual bool yAxisName(QString &, const QString& ="") const { return false; }

  void setOverlayPlotsAxisNames();
  void setPlotsAxisNames(const Plots &plots, CQChartsPlot *axisPlot);

  //---

  CQChartsPlotKey *key() const { return keyObj_; }

  CQChartsTitle *title() const { return titleObj_; }

  //---

  CQChartsPlot *parentPlot() const { return connectData_.parent; }
  void setParentPlot(CQChartsPlot *parent) { connectData_.parent = parent; }

  virtual int childPlotIndex(const CQChartsPlot *) const { return -1; }
  virtual int numChildPlots() const { return 0; }
  virtual CQChartsPlot *childPlot(int) const { return nullptr; }

  int plotDepth() const { return (parentPlot() ? parentPlot()->plotDepth() + 1 : 0); }

  //---

  // Connection
  bool isOverlay(bool checkVisible=true) const;
  void setOverlay(bool b, bool notify=true);

  void updateOverlay();

  bool isX1X2(bool checkVisible=true) const;
  void setX1X2(bool b, bool notify=true);

  bool isY1Y2(bool checkVisible=true) const;
  void setY1Y2(bool b, bool notify=true);

  bool isTabbed(bool checkVisible=true) const;
  void setTabbed(bool b, bool notify=true);

  const BBox &tabRect() const { return connectData_.tabRect; }
  void setTabRect(const BBox &rect) { connectData_.tabRect = rect; }

  bool isCurrent() const { return connectData_.current; }
  void setCurrent(bool b, bool notify=false);

  QString connectionStateStr() const;

  //---

  void setTabbedFont(const CQChartsFont &f);
  const CQChartsFont &tabbedFont() const;

  //-

  CQChartsPlot *prevPlot() const { return connectData_.prev; }
  CQChartsPlot *nextPlot() const { return connectData_.next; }

  void setNextPlot(CQChartsPlot *plot, bool notify=true);
  void setPrevPlot(CQChartsPlot *plot, bool notify=true);

  const CQChartsPlot *firstPlot() const;
  CQChartsPlot *firstPlot();

  const CQChartsPlot *lastPlot() const;
  CQChartsPlot *lastPlot();

  bool isFirstPlot() const { return (firstPlot() == this); }
  bool isLastPlot () const { return (lastPlot () == this); }

  bool isOverlayFirstPlot() const {
    return (isOverlay() && isFirstPlot());
  }

  bool isOverlayOtherPlot() const {
    return (isOverlay() && ! isFirstPlot());
  }

  void overlayPlots(Plots &plots, bool visibleOnly=false) const;

  bool tabbedPlots(Plots &plots, bool visibleOnly=false) const;

  template<typename FUNCTION>
  void processOverlayPlots(FUNCTION f) const {
    const auto *plot = firstPlot();

    while (plot) {
      f(plot);

      plot = plot->nextPlot();
    }
  }

  template<typename FUNCTION>
  void processOverlayPlots(FUNCTION f) {
    auto *plot = firstPlot();

    while (plot) {
      f(plot);

      plot = plot->nextPlot();
    }
  }

  template<typename FUNCTION>
  bool processOverlayPlots(FUNCTION f, bool b) const {
    const auto *plot = firstPlot();

    while (plot) {
      bool b1 = f(plot);

      if (b1 != b)
        return b1;

      plot = plot->nextPlot();
    }

    return b;
  }

  void x1x2Plots(CQChartsPlot* &plot1, CQChartsPlot* &plot2);
  void y1y2Plots(CQChartsPlot* &plot1, CQChartsPlot* &plot2);

  void resetConnectData(bool notify=true);

  void cycleNextPlot();
  void cyclePrevPlot();

  //---

  // invert x/y
  bool isInvertX() const { return invertX_; }
  void setInvertX(bool b);

  bool isInvertY() const { return invertY_; }
  void setInvertY(bool b);

  //---

  // log x/y
  bool isLogX() const;
  void setLogX(bool b);

  bool isLogY() const;
  void setLogY(bool b);

  //---

  // animation
  void startAnimateTimer();
  void stopAnimateTimer ();

  virtual bool isAnimated() const { return false; }

  virtual void animateStep() { }

  //---

  // properties
  const CQPropertyViewModel *propertyModel() const;
  CQPropertyViewModel *propertyModel();

  // add plot properties to model
  virtual void addProperties();

  void addBaseProperties();

  void addSymbolProperties(const QString &path, const QString &prefix, const QString &descPrefix);

  void addLineProperties(const QString &path, const QString &prefix,
                         const QString &descPrefix, bool hidden=false);
  void addFillProperties(const QString &path, const QString &prefix,
                         const QString &descPrefix, bool hidden=false);

  void addTextProperties(const QString &path, const QString &prefix, const QString &descPrefix,
                         uint valueTypes=CQChartsTextOptions::ValueType::SIMPLE);

  void addColorMapProperties();

  bool setProperties(const QString &properties);

  bool setProperty(const QString &name, const QVariant &value);
  bool getProperty(const QString &name, QVariant &value) const;

  bool getTclProperty(const QString &name, QVariant &value) const;

  bool getPropertyDesc    (const QString &name, QString  &desc, bool hidden=false) const;
  bool getPropertyType    (const QString &name, QString  &type, bool hidden=false) const;
  bool getPropertyUserType(const QString &name, QString  &type, bool hidden=false) const;
  bool getPropertyObject  (const QString &name, QObject* &obj , bool hidden=false) const;
  bool getPropertyIsHidden(const QString &name, bool &is_hidden) const;
  bool getPropertyIsStyle (const QString &name, bool &is_style) const;

  void propertyItemSelected(QObject *obj, const QString &path);

  virtual void getPropertyNames(QStringList &names, bool hidden=false) const;

  void getObjectPropertyNames(CQChartsPlotObj *plotObj, QStringList &names) const;

  void hideProperty(const QString &path, QObject *object);

  //---

  // margins
  void updateMargins(bool update=true);

  void updateMargins(const CQChartsPlotMargin &outerMargin);

  //---

  CQChartsModelIndex normalizeIndex(const CQChartsModelIndex &ind) const;
  QModelIndex normalizeIndex(const QModelIndex &ind) const;

  QModelIndex unnormalizeIndex(const QModelIndex &ind) const;

  void proxyModels(std::vector<QSortFilterProxyModel *> &proxyModels,
                   QAbstractItemModel* &sourceModel) const;

  //---

  void addColumnValues(const CQChartsColumn &column, CQChartsValueSet &valueSet) const;

  //---

  using ModelVisitor = CQChartsPlotModelVisitor;

  void visitModel(ModelVisitor &visitor) const;

  //---

  bool modelMappedReal(const CQChartsModelIndex &ind, double &r, bool log, double def) const;
  bool modelMappedReal(int row, const CQChartsColumn &col, const QModelIndex &ind,
                       double &r, bool log, double def) const;

  //---

  int getRowForId(const QString &id) const;

  QString idColumnString(int row, const QModelIndex &parent, bool &ok) const;

  //----

  QModelIndex modelIndex(const CQChartsModelIndex &ind) const;

  QModelIndex modelIndex(int row, const CQChartsColumn &column,
                         const QModelIndex &parent=QModelIndex()) const;

  //----

#if 0
  QVariant modelHHeaderValue(const CQChartsColumn &column, int role, bool &ok) const;
  QVariant modelHHeaderValue(const CQChartsColumn &column, bool &ok) const;

  virtual QVariant modelHHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                                     int role, bool &ok) const;
  virtual QVariant modelHHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                                     bool &ok) const;
#endif

  //---

#if 0
  QVariant modelVHeaderValue(int section, Qt::Orientation orient, int role, bool &ok) const;
  QVariant modelVHeaderValue(int section, Qt::Orientation orient, bool &ok) const;

  virtual QVariant modelVHeaderValue(QAbstractItemModel *model, int section,
                                     Qt::Orientation orient, int role, bool &ok) const;
  virtual QVariant modelVHeaderValue(QAbstractItemModel *model, int section,
                                     Qt::Orientation orient, bool &ok) const;
#endif

  //---

  QString modelHHeaderString(const CQChartsColumn &column, int role, bool &ok) const;
  QString modelHHeaderString(const CQChartsColumn &column, bool &ok) const;

  virtual QString modelHHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                                     int role, bool &ok) const;
  virtual QString modelHHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                                     bool &ok) const;

  //---

  QString modelHHeaderTip(const CQChartsColumn &column, bool &ok) const;

  //---

  QString modelVHeaderString(int section, Qt::Orientation orient, int role, bool &ok) const;
  QString modelVHeaderString(int section, Qt::Orientation orient, bool &ok) const;

  virtual QString modelVHeaderString(QAbstractItemModel *model, int section,
                                     Qt::Orientation orientation, int role, bool &ok) const;
  virtual QString modelVHeaderString(QAbstractItemModel *model,  int section,
                                     Qt::Orientation orientation, bool &ok) const;

  //---

  QVariant modelValue(const CQChartsModelIndex &ind, int role, bool &ok) const;
  QVariant modelValue(const CQChartsModelIndex &ind, bool &ok) const;

  QVariant modelValue(int row, const CQChartsColumn &column,
                      const QModelIndex &parent, int role, bool &ok) const;
  QVariant modelValue(int row, const CQChartsColumn &column,
                      const QModelIndex &parent, bool &ok) const;

  virtual QVariant modelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                              const QModelIndex &parent, int role, bool &ok) const;
  virtual QVariant modelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                              const QModelIndex &parent, bool &ok) const;

  //---

  QString modelString(const CQChartsModelIndex &ind, int role, bool &ok) const;
  QString modelString(const CQChartsModelIndex &ind, bool &ok) const;

  QString modelString(int row, const CQChartsColumn &column,
                      const QModelIndex &parent, int role, bool &ok) const;
  QString modelString(int row, const CQChartsColumn &column,
                      const QModelIndex &parent, bool &ok) const;

  virtual QString modelString(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                              const QModelIndex &parent, int role, bool &ok) const;
  virtual QString modelString(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                              const QModelIndex &parent, bool &ok) const;

  //---

  double modelReal(int row, const CQChartsColumn &column,
                   const QModelIndex &parent, int role, bool &ok) const;
  double modelReal(int row, const CQChartsColumn &column,
                   const QModelIndex &parent, bool &ok) const;

  double modelReal(const CQChartsModelIndex &ind, int role, bool &ok) const;
  double modelReal(const CQChartsModelIndex &ind, bool &ok) const;

  virtual double modelReal(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                           const QModelIndex &parent, int role, bool &ok) const;
  virtual double modelReal(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                           const QModelIndex &parent, bool &ok) const;

  //---

  long modelInteger(int row, const CQChartsColumn &column,
                    const QModelIndex &parent, int role, bool &ok) const;
  long modelInteger(int row, const CQChartsColumn &column,
                    const QModelIndex &parent, bool &ok) const;

  long modelInteger(const CQChartsModelIndex &ind, int role, bool &ok) const;
  long modelInteger(const CQChartsModelIndex &ind, bool &ok) const;

  virtual long modelInteger(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                            const QModelIndex &parent, int role, bool &ok) const;
  virtual long modelInteger(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                            const QModelIndex &parent, bool &ok) const;

  //---

#if 0
  Color modelColor(int row, const CQChartsColumn &column,
                   const QModelIndex &parent, bool &ok) const;
  Color modelColor(int row, const CQChartsColumn &column,
                   const QModelIndex &parent, int role, bool &ok) const;

  virtual Color modelColor(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                           const QModelIndex &parent, int role, bool &ok) const;
  virtual Color modelColor(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                           const QModelIndex &parent, bool &ok) const;
#endif

  //---

  std::vector<double> modelReals(const CQChartsModelIndex &ind, bool &ok) const;
  std::vector<double> modelReals(int row, const CQChartsColumn &column,
                                 const QModelIndex &parent, bool &ok) const;

  //---

  QVariant modelRootValue(int row, const CQChartsColumn &column,
                          const QModelIndex &parent, int role, bool &ok) const;
  QVariant modelRootValue(int row, const CQChartsColumn &column,
                          const QModelIndex &parent, bool &ok) const;

  //---

  QVariant modelHierValue(const CQChartsModelIndex &ind, bool &ok) const;
  QVariant modelHierValue(int row, const CQChartsColumn &column,
                          const QModelIndex &parent, int role, bool &ok) const;
  QVariant modelHierValue(int row, const CQChartsColumn &column,
                          const QModelIndex &parent, bool &ok) const;

  //---

  QString modelHierString(const CQChartsModelIndex &ind, bool &ok) const;
  QString modelHierString(int row, const CQChartsColumn &column,
                          const QModelIndex &parent, int role, bool &ok) const;
  QString modelHierString(int row, const CQChartsColumn &column,
                          const QModelIndex &parent, bool &ok) const;

  //---

#if 0
  double modelHierReal(int row, const CQChartsColumn &column,
                       const QModelIndex &parent, int role, bool &ok) const;
  double modelHierReal(int row, const CQChartsColumn &column,
                       const QModelIndex &parent, bool &ok) const;
#endif

  //---

#if 0
  long modelHierInteger(int row, const CQChartsColumn &column,
                        const QModelIndex &parent, int role, bool &ok) const;
  long modelHierInteger(int row, const CQChartsColumn &column,
                        const QModelIndex &parent, bool &ok) const;
#endif

  //---

  bool isSelectIndex(const QModelIndex &ind, int row, const CQChartsColumn &col,
                     const QModelIndex &parent=QModelIndex()) const;

  QModelIndex selectIndex(int row, const CQChartsColumn &col,
                          const QModelIndex &parent=QModelIndex()) const;

  void beginSelectIndex();

  void addSelectIndex(const CQChartsModelIndex &ind);
  void addSelectIndex(int row, int col, const QModelIndex &parent=QModelIndex());
  void addSelectIndex(const QModelIndex &ind);

  void endSelectIndex();

  //---

  double logValue(double x, int base=10) const;
  double expValue(double x, int base=10) const;

  //---

  Point positionToPlot (const CQChartsPosition &pos) const;
  Point positionToPixel(const CQChartsPosition &pos) const;

  BBox rectToPlot (const CQChartsRect &rect) const;
  BBox rectToPixel(const CQChartsRect &rect) const;

  //---

  double lengthPlotSize(const Length &len, bool horizontal) const;

  double lengthPlotWidth (const Length &len) const;
  double lengthPlotHeight(const Length &len) const;

  double lengthPixelSize(const Length &len, bool horizontal) const;

  double lengthPixelWidth (const Length &len) const;
  double lengthPixelHeight(const Length &len) const;

  //---

  double windowToViewWidth (double wx) const;
  double windowToViewHeight(double wy) const;

  double viewToWindowWidth (double vx) const;
  double viewToWindowHeight(double vy) const;

  Point windowToPixel(const Point &w) const;
  Point windowToView (const Point &w) const;
  Point pixelToWindow(const Point &p) const;
  Point viewToWindow (const Point &v) const;

  BBox windowToPixel(const BBox &wrect) const;
  BBox windowToView (const BBox &wrect) const;
  BBox pixelToWindow(const BBox &prect) const;
  BBox viewToWindow (const BBox &vrect) const;

  double pixelToSignedWindowWidth (double ww) const;
  double pixelToSignedWindowHeight(double wh) const;

  double pixelToWindowSize(double ps, bool horizontal) const;
  Size pixelToWindowSize(const Size &ps) const;

  double pixelToWindowWidth (double pw) const;
  double pixelToWindowHeight(double ph) const;

  double windowToSignedPixelWidth (double ww) const;
  double windowToSignedPixelHeight(double wh) const;

  double windowToPixelWidth (double ww) const;
  double windowToPixelHeight(double wh) const;

  Polygon windowToPixel(const Polygon &p) const;

  QPainterPath windowToPixel(const QPainterPath &p) const;

 private:
  void windowToPixelI(const Point &w, Point &p) const;
  void pixelToWindowI(const Point &p, Point &w) const;

  void windowToPixelI(const BBox &wrect, BBox &prect) const;
  void pixelToWindowI(const BBox &prect, BBox &wrect) const;

  void pixelToWindowI(double px, double py, double &wx, double &wy) const;
  void viewToWindowI (double vx, double vy, double &wx, double &wy) const;

  void windowToPixelI(double wx, double wy, double &px, double &py) const;
  void windowToViewI (double wx, double wy, double &vx, double &vy) const;

  //---

 public:
  void plotSymbolSize (const Length &s, double &sx, double &sy) const;
  void pixelSymbolSize(const Length &s, double &sx, double &sy) const;

  double limitSymbolSize(double s) const;
  double limitFontSize(double s) const;

  //---

  void addAxes();

  void addXAxis();
  void addYAxis();

  //---

  void addKey();

  virtual void resetKeyItems();

  // add items to key
  void doAddKeyItems(CQChartsPlotKey *key);

  virtual void addKeyItems(CQChartsPlotKey *) { }

  bool addColorKeyItems(CQChartsPlotKey *key);

  //---

  void addTitle();

  //---

  void resetRange();

 private:
  // update data range (calls calcRange)
  void execUpdateRange();

 public:
  // calculate and return range from data
  virtual Range calcRange() const = 0;

  virtual void postUpdateRange() { }

  // update plot objects (clear objects, objects updated on next redraw)
  void updateGroupedObjs();

  // reset range and objects
  void clearRangeAndObjs();

  virtual void postUpdateObjs() { }

  virtual void postDraw() { }

  virtual void postObjTree() { }

 private:
  // recalc range and clear objects (objects updated on next redraw)
  void execUpdateRangeAndObjs();

  // update plot objects (clear objects, objects updated on next redraw)
  void execUpdateObjs();

 private:
  void startThreadTimer();
  void stopThreadTimer();

  //---

  void updateAndApplyRange(bool apply, bool updateObjs);

  void updateAndApplyPlotRange(bool apply, bool updateObjs);

  void updateAndApplyPlotRange1(bool updateObjs);

  //---

  static void updateRangeASync(CQChartsPlot *plot);

  void updateRangeThread();

  void interruptRange();

 protected:
  bool isReady() const;

 public:
  void syncAll();

  void syncRange();
  void syncObjs();
  void syncDraw();

 private:
  void syncState();

 protected:
  friend class CQChartsCompositePlot;

  virtual void waitRange();
  virtual void waitDraw();
  virtual void waitObjs();
  virtual void waitTree();

 private:
  void execWaitRange();

  //---

  static void updateObjsASync(CQChartsPlot *plot);

  void updateObjsThread();

  void interruptObjs();

  void execWaitObjs();

  //---

  void updatePlotObjs();

  void resetInsideObjs();

  void updateGroupedDraw();
  void updateDraw();

  //---

  static void drawASync(CQChartsPlot *plot);

  void drawThread();

  void interruptDraw();

  void execWaitDraw();

  //---

  void execWaitTree();

  //---

 protected:
  void initColorColumnData();

  //---

  struct SymbolTypeData {
    CQChartsColumn column;             //!< symbol type column
    bool           valid    { false }; //!< symbol type valid
    bool           mapped   { false }; //!< symbol type values mapped
    int            data_min { 0 };     //!< model data min
    int            data_max { 1 };     //!< model data max
    int            map_min  { 0 };     //!< mapped size min
    int            map_max  { 1 };     //!< mapped size max
  };

  void initSymbolTypeData(SymbolTypeData &symbolTypeData) const;

  bool columnSymbolType(int row, const QModelIndex &parent, const SymbolTypeData &symbolTypeData,
                        CQChartsSymbol &symbolType) const;

  //---

  struct SymbolSizeData {
    CQChartsColumn column;              //!< symbol size column
    bool           valid     { false }; //!< symbol size valid
    bool           mapped    { false }; //!< symbol size values mapped
    double         data_min  { 0.0 };   //!< model data min
    double         data_max  { 1.0 };   //!< model data max
    double         data_mean { 0.0 };   //!< model data mean
    double         map_min   { 0.0 };   //!< mapped size min
    double         map_max   { 1.0 };   //!< mapped size max
    QString        units     { "px" };  //!< mapped size units
  };

  void initSymbolSizeData(SymbolSizeData &symbolSizeData) const;

  bool columnSymbolSize(int row, const QModelIndex &parent, const SymbolSizeData &symbolSizeData,
                        Length &symbolSize) const;

  //---

  struct FontSizeData {
    CQChartsColumn column;             //!< font size column
    bool           valid    { false }; //!< font size valid
    bool           mapped   { false }; //!< font size values mapped
    double         data_min { 0.0 };   //!< model data min
    double         data_max { 1.0 };   //!< model data max
    double         map_min  { 0.0 };   //!< mapped size min
    double         map_max  { 1.0 };   //!< mapped size max
    QString        units    { "px" };  //!< mapped size units
  };

  void initFontSizeData(FontSizeData &fontSizeData) const;

  bool columnFontSize(int row, const QModelIndex &parent, const FontSizeData &fontSizeData,
                      Length &fontSize) const;

  //---

 private:
  // (re)initialize grouped plot objects
  void initGroupedPlotObjs();

  // (re)initialize plot range
  bool initPlotRange();

  // (re)initialize plot objects
  virtual void initPlotObjs();

  bool addNoDataObj();

  void updateAutoFit();

  void autoFitOne();

 public:
  // (re)initialize plot objects (called by initPlotObjs)
  virtual bool initObjs();

  // create plot objects (called by initObjs)
  bool createObjs();

  // create objects to be added to plot
  // TODO: need axis update as well
  virtual bool createObjs(PlotObjs &) const = 0;

 public:
  // add plotObjects to quad tree (create no data object in no objects)
  virtual void initObjTree();

 protected:
  void execInitObjTree();

 public:
  // routine to run after plot set up (usually fixes up some defaults)
  virtual void postInit();

  //---

  BBox findEmptyBBox(double w, double h) const;

  //---

  BBox calcDataRange(bool adjust=true) const;
  void calcDataRanges(BBox &rawRange, BBox &adjustedRange) const;

  BBox getDataRange() const;

  virtual void updateAxisRanges(const BBox &adjustedRange);

  void updateOverlayRanges();

  void setPixelRange(const BBox &bbox);

  void resetWindowRange();
  void setWindowRange(const BBox &bbox);

  bool isApplyDataRange() const { return updatesData_.applyDataRange; }

  void applyDataRangeAndDraw();

  void applyDataRange(bool propagate=true);

  Range adjustDataRange(const Range &range) const;

  BBox calcGroupedDataRange(bool includeAnnotation=true) const;

  BBox calcGroupedXAxisRange(const CQChartsAxisSide::Type &side) const;
  BBox calcGroupedYAxisRange(const CQChartsAxisSide::Type &side) const;

  //---

  CQPropertyViewItem *addProperty(const QString &path, QObject *object,
                                  const QString &name, const QString &alias="");

  void addPlotObject(CQChartsPlotObj *obj);

  virtual void clearPlotObjects();

  bool updateInsideObjects(const Point &w);

  CQChartsObj *insideObject() const;

  void setInsideObject();

  QString insideObjectText() const;

  void nextInsideInd();
  void prevInsideInd();

  //---

  // general error
  struct Error {
    QString msg;
  };

  // error for bad column
  struct ColumnError {
    CQChartsColumn column;
    QString        msg;
  };

  // error accessing data in model
  struct DataError {
    CQChartsModelIndex ind;
    QString            msg;
  };

  using Errors       = std::vector<Error>;
  using ColumnErrors = std::vector<ColumnError>;
  using DataErrors   = std::vector<DataError>;

  void clearErrors();

  bool hasErrors() const;

  bool addError(const QString &msg);
  bool addColumnError(const CQChartsColumn &c, const QString &msg);
  bool addDataError(const CQChartsModelIndex &ind, const QString &msg);

  void getErrors(QStringList &errors);

  void addErrorsToWidget(QTextBrowser *text);

  //---

  const PlotObjs &plotObjects() const { return plotObjs_; }

  int numPlotObjects() const { return plotObjs_.size(); }

  CQChartsPlotObj *plotObject(int i) const { return plotObjs_[i]; }

  bool isNoData() const { return noData_; }
  void setNoData(bool b) { noData_ = b; }

  bool isPlotObjTreeSet() const { return objTreeData_.isSet; }
  void setPlotObjTreeSet(bool b);

  //----

  // columns
  const CQChartsColumn &xValueColumn() const { return xValueColumn_; }
  void setXValueColumn(const CQChartsColumn &column);

  const CQChartsColumn &yValueColumn() const { return yValueColumn_; }
  void setYValueColumn(const CQChartsColumn &column);

  const CQChartsColumn &idColumn() const { return idColumn_; }
  void setIdColumn(const CQChartsColumn &column);

  const CQChartsColumns &tipColumns() const { return tipColumns_; }
  void setTipColumns(const CQChartsColumns &columns);

  const CQChartsColumn &visibleColumn() const { return visibleColumn_; }
  void setVisibleColumn(const CQChartsColumn &column);

  const CQChartsColumn &imageColumn() const { return imageColumn_; }
  void setImageColumn(const CQChartsColumn &column);

  const CQChartsColumn &colorColumn() const { return colorColumnData_.column; };
  void setColorColumn(const CQChartsColumn &c);

  const CQChartsColumn &fontColumn() const { return fontColumn_; };
  void setFontColumn(const CQChartsColumn &c);

  const CQChartsColumns &controlColumns() const { return controlColumns_; }
  void setControlColumns(const CQChartsColumns &columns);

  //--

  // coloring
  const ColorType &colorType() const { return colorColumnData_.colorType; }
  void setColorType(const ColorType &t);

  bool isColorMapped() const { return colorColumnData_.mapped; }
  void setColorMapped(bool b);

  double colorMapMin() const { return colorColumnData_.map_min; }
  void setColorMapMin(double r);

  double colorMapMax() const { return colorColumnData_.map_max; }
  void setColorMapMax(double r);

  const QString &colorMapPalette() const { return colorColumnData_.palette; }
  void setColorMapPalette(const QString &s);

  const CQChartsColorStops &colorXStops() const { return colorColumnData_.xStops; }
  void setColorXStops(const CQChartsColorStops &s);

  const CQChartsColorStops &colorYStops() const { return colorColumnData_.yStops; }
  void setColorYStops(const CQChartsColorStops &s);

  //---

  // color column
  bool colorColumnColor(int row, const QModelIndex &parent, Color &color) const;

  bool modelIndexColor(const CQChartsModelIndex &ind, Color &color) const;

  bool columnValueColor(const QVariant &var, Color &color) const;

  //---

  // font column
  bool fontColumnFont(int row, const QModelIndex &parent, CQChartsFont &font) const;

  bool modelIndexFont(const CQChartsModelIndex &ind, CQChartsFont &font) const;

  bool columnValueFont(const QVariant &var, CQChartsFont &font) const;

  //---

  virtual QString keyText() const;

  //---

  virtual QString posStr(const Point &w) const;

  virtual QString xStr(double x) const;
  virtual QString yStr(double y) const;

  virtual QString columnStr(const CQChartsColumn &column, double x) const;

  //---

  // handle mouse press/move/release
  virtual bool selectMousePress  (const Point &p, SelMod selMod);
  virtual bool selectMouseMove   (const Point &p, bool first=false);
  virtual bool selectMouseRelease(const Point &p);

  virtual bool selectPress  (const Point &p, SelMod selMod);
  virtual bool selectMove   (const Point &p, bool first=false);
  virtual bool selectRelease(const Point &p);

  //-

  bool tabbedSelectPress(const Point &w, SelMod selMod);

  CQChartsPlot *tabbedPressPlot(const Point &w, Plots &plots) const;

  bool keySelectPress  (CQChartsPlotKey *key  , const Point &w, SelMod selMod);
  bool titleSelectPress(CQChartsTitle   *title, const Point &w, SelMod selMod);

  bool annotationsSelectPress(const Point &w, SelMod selMod);

  CQChartsObj *objectsSelectPress(const Point &w, SelMod selMod);

  //-

  // handle mouse drag press/move/release
  bool editMousePress  (const Point &p, bool inside=false);
  bool editMouseMove   (const Point &p, bool first=false);
  bool editMouseMotion (const Point &p);
  bool editMouseRelease(const Point &p);

  virtual bool editPress  (const Point &p, const Point &w, bool inside=false);
  virtual bool editMove   (const Point &p, const Point &w, bool first=false);
  virtual bool editMotion (const Point &p, const Point &w);
  virtual bool editRelease(const Point &p, const Point &w);

  //-

  bool keyEditPress  (CQChartsPlotKey *key  , const Point &w);
  bool axisEditPress (CQChartsAxis    *axis , const Point &w);
  bool titleEditPress(CQChartsTitle   *title, const Point &w);

  bool annotationsEditPress(const Point &w);

  //-

  bool keyEditSelect  (CQChartsPlotKey *key  , const Point &w);
  bool axisEditSelect (CQChartsAxis    *axis , const Point &w);
  bool titleEditSelect(CQChartsTitle   *title, const Point &w);

  bool annotationsEditSelect(const Point &w);

  bool objectsEditSelect(const Point &w, bool inside);

  //-

  virtual void editMoveBy(const Point &d);

  void selectOneObj(CQChartsObj *obj, bool allObjs);

  void deselectAllObjs();

  void deselectAll();

  // handle key press
  virtual void keyPress(int key, int modifier);

  // get tip text at point
  virtual bool tipText(const Point &p, QString &tip) const;

  void addTipColumns(CQChartsTableTip &tableTip, const QModelIndex &ind) const;

  void resetObjTips();

  // handle rect select
  bool rectSelect(const BBox &r, SelMod selMod);

  //---

  void startSelection();
  void endSelection();

  //---

  void selectedObjs(Objs &objs) const;

  void selectedPlotObjs(PlotObjs &objs) const;

  void editObjs(Objs &objs);

  //---

  virtual double getPanX(bool is_shift) const;
  virtual double getPanY(bool is_shift) const;

  virtual double getMoveX(bool is_shift) const;
  virtual double getMoveY(bool is_shift) const;

  virtual double getZoomFactor(bool is_shift) const;

 public slots:
  void updateSlot();

  virtual void cycleNext();
  virtual void cyclePrev();

  virtual void panLeft (double f=0.125);
  virtual void panRight(double f=0.125);
  virtual void panUp   (double f=0.125);
  virtual void panDown (double f=0.125);

  virtual void zoomIn(double f=1.5);
  virtual void zoomOut(double f=1.5);

  virtual void zoomFull(bool notify=true);

  virtual bool allowZoomX() const { return true; }
  virtual bool allowZoomY() const { return true; }

  virtual bool allowPanX() const { return true; }
  virtual bool allowPanY() const { return true; }

 public:
  virtual void cycleNextPrev(bool prev);

  virtual void pan(double dx, double dy);

  virtual void adjustPan() { }

  virtual void zoomTo(const BBox &bbox);

  //---

  void updateTransform();

  //---

  virtual bool addMenuItems(QMenu *) { return false; }

  //---

  // probe at x, y
  virtual bool probe(ProbeData & /*probeData*/) const { return false; }

  //---

  // called before resize
  virtual void preResize();

  // called after resize
  virtual void postResize();

  //---

  void updateKeyPosition(bool force=false);

  BBox displayRangeBBox() const;

  BBox calcDataPixelRect() const;

  BBox calcPlotRect() const;
  BBox calcPlotPixelRect() const;

  BBox calcFitPixelRect() const;

  Size calcPixelSize() const;

  void calcTabData(const Plots &plots) const;

  BBox calcTabPixelRect() const;

  //---

  // auto fit
  virtual void autoFit();

  void setFitBBox(const BBox &bbox);

  BBox fitBBox() const;

  virtual BBox dataFitBBox () const;
  virtual BBox axesFitBBox () const;
  virtual BBox keyFitBBox  () const;
  virtual BBox titleFitBBox() const;

  // get bounding box of annotations outside plot area
  BBox annotationBBox() const;

  virtual BBox calcAnnotationBBox() const { return BBox(); }

  //---

  // annotations

  const Annotations &annotations() const { return annotations_; }

  CQChartsArrowAnnotation     *addArrowAnnotation    (const CQChartsPosition &start,
                                                      const CQChartsPosition &end);
  CQChartsAxisAnnotation      *addAxisAnnotation     (Qt::Orientation direction,
                                                      double start, double end);
  CQChartsEllipseAnnotation   *addEllipseAnnotation  (const CQChartsPosition &center,
                                                      const Length &xRadius,
                                                      const Length &yRadius);
  CQChartsImageAnnotation     *addImageAnnotation    (const CQChartsPosition &pos,
                                                      const CQChartsImage &image);
  CQChartsImageAnnotation     *addImageAnnotation    (const CQChartsRect &rect,
                                                      const CQChartsImage &image);
  CQChartsKeyAnnotation       *addKeyAnnotation      ();
  CQChartsPieSliceAnnotation  *addPieSliceAnnotation (const CQChartsPosition &pos,
                                                      const Length &innerRadius,
                                                      const Length &outerRadius,
                                                      const Angle &startAngle,
                                                      const Angle &spanAngle);
  CQChartsPointAnnotation     *addPointAnnotation    (const CQChartsPosition &pos,
                                                      const CQChartsSymbol &type);
  CQChartsPointSetAnnotation  *addPointSetAnnotation (const CQChartsPoints &values);
  CQChartsPolygonAnnotation   *addPolygonAnnotation  (const CQChartsPolygon &polygon);
  CQChartsPolylineAnnotation  *addPolylineAnnotation (const CQChartsPolygon &polygon);
  CQChartsRectangleAnnotation *addRectangleAnnotation(const CQChartsRect &rect);
  CQChartsTextAnnotation      *addTextAnnotation     (const CQChartsPosition &pos,
                                                      const QString &text);
  CQChartsTextAnnotation      *addTextAnnotation     (const CQChartsRect &rect,
                                                      const QString &text);
  CQChartsValueSetAnnotation  *addValueSetAnnotation (const CQChartsRect &rectangle,
                                                      const CQChartsReals &values);
  CQChartsButtonAnnotation    *addButtonAnnotation   (const CQChartsPosition &pos,
                                                      const QString &text);

  void addAnnotation(CQChartsAnnotation *annotation);

  CQChartsAnnotation *getAnnotationByName(const QString &id) const;
  CQChartsAnnotation *getAnnotationByInd(int ind) const;

  void removeAnnotation(CQChartsAnnotation *annotation);

  void removeAllAnnotations();

  //---

  CQChartsPlotObj *getPlotObject(const QString &objectId) const;

  CQChartsObj *getObject(const QString &objectId) const;

  QList<QModelIndex> getObjectInds(const QString &objectId) const;

  std::vector<CQChartsObj *> getObjectConnected(const QString &objectId) const;

  //---

  const Layers &layers() const { return layers_; }

  CQChartsLayer *initLayer(const CQChartsLayer::Type &type,
                           const CQChartsBuffer::Type &buffer, bool active);

  void setLayerActive(const CQChartsLayer::Type &type, bool b);

  bool isLayerActive(const CQChartsLayer::Type &type) const;

  //---

  bool isInvalidateLayers() const { return updatesData_.invalidateLayers; }

  virtual void invalidateLayers();

 protected:
  void execInvalidateLayers();

 public:
  virtual void invalidateLayer(const CQChartsBuffer::Type &layerType);

 protected:
  void execInvalidateLayer(const CQChartsBuffer::Type &layerType);

 public:
  virtual void invalidateOverlay();

 protected:
  void execInvalidateOverlay();

  //---

 public:
  CQChartsBuffer *getBuffer(const CQChartsBuffer::Type &type) const;

  CQChartsLayer *getLayer(const CQChartsLayer::Type &type) const;

 private:
  BBox adjustDataRangeBBox(const BBox &bbox) const;

  void setLayerActive1(const CQChartsLayer::Type &type, bool b);

  void invalidateLayer1(const CQChartsBuffer::Type &layerType);

  void setLayersChanged(bool update);

  void deselectAll1(bool &changed);

  //---

 public:
  virtual bool hasXAxis() const;
  virtual bool hasYAxis() const;

  //---

 public:
  // draw plot parts
  virtual void drawParts(QPainter *painter) const;

  // draw plot device parts
  virtual void drawDeviceParts(CQChartsPaintDevice *) const { }

  // draw background layer plot parts
  virtual void drawBackgroundParts(QPainter *painter) const;

  // draw background layer plot device parts
  virtual void drawBackgroundDeviceParts(CQChartsPaintDevice *device, bool bgLayer, bool bgAxes,
                                         bool bgKey) const;

  // draw middle layer plot parts
  virtual void drawMiddleParts(QPainter *painter) const;

  // draw middle layer plot device parts
  virtual void drawMiddleDeviceParts(CQChartsPaintDevice *device, bool bg, bool mid, bool fg,
                                     bool annotations) const;

  // draw foreground layer plot parts
  virtual void drawForegroundParts(QPainter *painter) const;

  // draw foreground layer plot device parts
  virtual void drawForegroundDeviceParts(CQChartsPaintDevice *device, bool fgAxes, bool fgKey,
                                         bool title, bool foreground, bool tabbed) const;

  virtual void drawTabs(CQChartsPaintDevice *device) const;

  void drawTabs(CQChartsPaintDevice *device, const Plots &plots) const;
  void drawTabs(CQChartsPaintDevice *device, const Plots &plots, CQChartsPlot *currentPlot) const;

  // draw overlay layer plot parts
  virtual void drawOverlayParts(QPainter *painter) const;

  // draw overlay layer plot device parts
  virtual void drawOverlayDeviceParts(CQChartsPaintDevice *device, bool sel_objs,
                                      bool sel_annotations, bool boxes, bool edit_handles,
                                      bool over_objs, bool over_annotations) const;

  //---

  // draw background (layer and detail)
  virtual bool hasBackgroundLayer() const;

  virtual void drawBackgroundLayer(CQChartsPaintDevice *device) const;

  virtual void drawBackgroundRects(CQChartsPaintDevice *device) const;

  virtual bool hasBackground() const;

  virtual void execDrawBackground(CQChartsPaintDevice *device) const;

  virtual void drawBackgroundSides(CQChartsPaintDevice *device, const BBox &bbox,
                                   const CQChartsSides &sides) const;

  // draw axes on background
  virtual bool hasGroupedBgAxes() const;

  virtual bool hasBgAxes() const;

  virtual void drawGroupedBgAxes(CQChartsPaintDevice *device) const;

  virtual void drawBgAxes(CQChartsPaintDevice *device) const;

  // draw key on background
  virtual bool hasGroupedBgKey() const;

  virtual void drawBgKey(CQChartsPaintDevice *device) const;

  //---

  // draw objects
  virtual bool hasGroupedObjs(const CQChartsLayer::Type &layerType) const;

  virtual void drawGroupedObjs(CQChartsPaintDevice *device,
                               const CQChartsLayer::Type &layerType) const;

  virtual bool hasObjs(const CQChartsLayer::Type &layerType) const;

  virtual void preDrawObjs(CQChartsPaintDevice *) const { }

  virtual void execDrawObjs(CQChartsPaintDevice *device,
                            const CQChartsLayer::Type &type) const;

  virtual void postDrawObjs(CQChartsPaintDevice *) const { }

  virtual bool objInsideBox(CQChartsPlotObj *plotObj, const BBox &bbox) const;

  //---

  // draw axes on foreground
  virtual bool hasGroupedFgAxes() const;

  virtual bool hasFgAxes() const;

  virtual void drawGroupedFgAxes(CQChartsPaintDevice *device) const;

  virtual void drawFgAxes(CQChartsPaintDevice *device) const;

  // draw key on foreground
  virtual bool hasGroupedFgKey() const;

  virtual void drawFgKey(CQChartsPaintDevice *painter) const;

  // draw title
  virtual bool hasTitle() const;

  virtual void drawTitle(CQChartsPaintDevice *device) const;

  // draw annotations
  virtual bool hasGroupedAnnotations(const CQChartsLayer::Type &layerType) const;

  virtual void drawGroupedAnnotations(CQChartsPaintDevice *device,
                                      const CQChartsLayer::Type &layerType) const;

  virtual bool hasAnnotations(const CQChartsLayer::Type &layerType) const;

  virtual void drawAnnotations(CQChartsPaintDevice *device,
                               const CQChartsLayer::Type &layerType) const;

  // draw foreground
  virtual bool hasForeground() const;

  virtual void execDrawForeground(CQChartsPaintDevice *device) const;

  // draw debug boxes
  virtual bool hasGroupedBoxes() const;

  virtual void drawGroupedBoxes(CQChartsPaintDevice *device) const;

  virtual bool hasBoxes() const;

  virtual void drawBoxes(CQChartsPaintDevice *device) const;

  // draw edit handles
  virtual bool hasGroupedEditHandles() const;

  virtual void drawGroupedEditHandles(QPainter *painter) const;

  virtual bool hasEditHandles() const;

  virtual void drawEditHandles(QPainter *painter) const;

  //---

  // set clip rect
  void setClipRect(CQChartsPaintDevice *device) const;

  //---

  virtual bool selectInvalidateObjs() const { return false; }

  //---

  QPainter *beginPaint(CQChartsBuffer *layer, QPainter *painter,
                       const QRectF &rect=QRectF()) const;
  void      endPaint  (CQChartsBuffer *layer) const;

  //---

  CQChartsPlotKey *getFirstPlotKey() const;

  //---

  void drawSymbol(CQChartsPaintDevice *device, const Point &p,
                  const CQChartsSymbol &symbol, const Length &size,
                  const CQChartsPenBrush &penBrush) const;
  void drawSymbol(CQChartsPaintDevice *device, const Point &p,
                  const CQChartsSymbol &symbol, const Length &size) const;

  void drawBufferedSymbol(QPainter *painter, const Point &p,
                          const CQChartsSymbol &symbol, double size) const;

  //---

  CQChartsTextOptions adjustTextOptions(
    const CQChartsTextOptions &options=CQChartsTextOptions()) const;

  //---

  // debug draw (default to red boxes)
  void drawWindowColorBox(CQChartsPaintDevice *device, const BBox &bbox,
                          const QColor &c=Qt::red) const;

  void drawColorBox(CQChartsPaintDevice *device, const BBox &bbox,
                    const QColor &c=Qt::red) const;

  //---

  // set pen/brush
  void setPenBrush(CQChartsPenBrush &penBrush, const CQChartsPenData &penData,
                   const CQChartsBrushData &brushData) const;

  void setPenBrush(CQChartsPenBrush &penBrush,
                   bool stroked, const QColor &strokeColor, const Alpha &strokeAlpha,
                   const Length &strokeWidth, const LineDash &strokeDash,
                   bool filled, const QColor &fillColor, const QColor &altFillColor,
                   const Alpha &fillAlpha,
                   const FillPattern &pattern=FillPattern::Type::SOLID) const;

  void setPen(CQChartsPenBrush &penBrush, const CQChartsPenData &penData) const;

  void setPen(QPen &pen, bool stroked, const QColor &strokeColor=QColor(),
              const Alpha &strokeAlpha=Alpha(), const Length &strokeWidth=Length("0px"),
              const LineDash &strokeDash=LineDash()) const;

  void setBrush(CQChartsPenBrush &penBrush, const CQChartsBrushData &brushData) const;

  void setBrush(QBrush &brush, bool filled, const QColor &fillColor=QColor(),
                const Alpha &fillAlpha=Alpha(),
                const FillPattern &pattern=FillPattern::Type::SOLID) const;

  //---

  void setPenBrush(CQChartsPaintDevice *device, const CQChartsPenData &penData,
                   const CQChartsBrushData &brushData) const;

  void setPen(CQChartsPaintDevice *device, const CQChartsPenData &penData) const;

  void setBrush(CQChartsPaintDevice *device, const CQChartsBrushData &brushData) const;

  //---

  void updateObjPenBrushState(const CQChartsObj *obj, CQChartsPenBrush &penBrush,
                              DrawType drawType=DrawType::BOX) const;

  void updateObjPenBrushState(const CQChartsObj *obj, const ColorInd &ic,
                              CQChartsPenBrush &penBrush, DrawType drawType) const;

 private:
  void updateInsideObjPenBrushState  (const ColorInd &ic, CQChartsPenBrush &penBrush,
                                      bool outline, DrawType drawType) const;
  void updateSelectedObjPenBrushState(const ColorInd &ic, CQChartsPenBrush &penBrush,
                                      DrawType drawType) const;

 public:
  QColor insideColor  (const QColor &c) const;
  QColor selectedColor(const QColor &c) const;

  //---

 public:
  // get palette color for index
  virtual QColor interpPaletteColor(const ColorInd &ind, bool scale=false) const;

 public:
  virtual QColor interpGroupPaletteColor(const ColorInd &ig, const ColorInd &iv,
                                         bool scale=false) const;

  QColor blendGroupPaletteColor(double r1, double r2, double dr) const;

 public:
  QColor interpThemeColor(const ColorInd &ind) const;

  QColor interpInterfaceColor(double r) const;

 public:
  QColor interpColor(const Color &c, const ColorInd &ind) const;

  //---

 public:
  QColor calcTextColor(const QColor &bg) const;

  //---

  ColorInd calcColorInd(const CQChartsPlotObj *obj, const CQChartsKeyColorBox *keyBox,
                        const ColorInd &is, const ColorInd &ig, const ColorInd &iv) const;

  //---

  bool checkColumns(const CQChartsColumns &columns, const QString &name,
                    bool required=false) const;

  bool checkColumn(const CQChartsColumn &column, const QString &name,
                   bool required=false) const;

  bool checkColumn(const CQChartsColumn &column, const QString &name,
                   ColumnType &type, bool required=false) const;

  ColumnType columnValueType(const CQChartsColumn &column,
                             const ColumnType &defType=ColumnType::STRING) const;

  bool columnValueType(const CQChartsColumn &column, CQChartsModelTypeData &columnTypeData,
                       const ColumnType &defType=ColumnType::STRING) const;

#if 0
  bool columnTypeStr(const CQChartsColumn &column, QString &typeStr) const;

  bool setColumnTypeStr(const CQChartsColumn &column, const QString &typeStr);
#endif

  bool columnDetails(const CQChartsColumn &column, QString &typeName,
                     QVariant &minValue, QVariant &maxValue) const;

  CQChartsModelColumnDetails *columnDetails(const CQChartsColumn &column) const;

  //---

  CQChartsModelData *getModelData() const;

  //---

  bool getHierColumnNames(const QModelIndex &parent, int row, const CQChartsColumns &nameColumns,
                          const QString &separator, QStringList &nameStrs,
                          ModelIndices &nameInds) const;

  //---

  // cached column names
  QString columnsHeaderName(const CQChartsColumns &columns, bool tip=false) const;
  QString columnHeaderName(const CQChartsColumn &column, bool tip=false) const;

  QString idHeaderName   (bool tip=false) const { return columnHeaderName(idColumn   (), tip); }
  QString colorHeaderName(bool tip=false) const { return columnHeaderName(colorColumn(), tip); }
  QString fontHeaderName (bool tip=false) const { return columnHeaderName(fontColumn (), tip); }
  QString imageHeaderName(bool tip=false) const { return columnHeaderName(imageColumn(), tip); }

  virtual void updateColumnNames();

  void setColumnHeaderName(const CQChartsColumn &column, const QString &def);

  //---

  // get/set/reset id hidden
  bool isSetHidden(int id) const;

  void setSetHidden(int id, bool hidden);

  void resetSetHidden();

  const QVariant &hideValue() const { return hideValue_; }
  void setHideValue(const QVariant &value) { hideValue_ = value; }

  //---

  void update();

  //---

  // draw plot
  virtual void draw(QPainter *painter);

  // draw plot layer
  void drawLayer(QPainter *painter, CQChartsLayer::Type type) const;

  // draw all layers
  void drawLayers(QPainter *painter) const;

  const CQChartsLayer::Type &drawLayerType() const;

  //---

  // print layer data
  bool printLayer(CQChartsLayer::Type type, const QString &filename) const;

  //---

  // get/set parameter
  bool getParameter(CQChartsPlotParameter *param, QVariant &value) const;
  bool setParameter(CQChartsPlotParameter *param, const QVariant &value);

  //---

  virtual void modelViewExpansionChanged() { }

  //---

  // write details to output
  virtual void write(std::ostream &os, const QString &plotVarName="",
                     const QString &modelVarName="", const QString &viewVarName="") const;

  //---

  bool contains(const Point &p) const override;

 protected slots:
  void animateSlot();

  void threadTimerSlot();

  // model change slots
  void modelChangedSlot();

  void currentModelChangedSlot();

  //---

  void selectionSlot(QItemSelectionModel *sm);

  void updateAnnotationSlot();

 signals:
  // model data changed
  void modelChanged();

  // data range changed
  void rangeChanged();

  // plot objects added
  void plotObjsAdded();

  // connection (x1x2, y1y2, overlay) changed
  void connectDataChanged();

  // current connected plot changed
  void currentPlotChanged(CQChartsPlot *plot);
  void currentPlotIdChanged(const QString &id);

  // layers changed (active, valid)
  void layersChanged();

  // control columns changed
  void controlColumnsChanged();

  // key signals (key, key item pressed)
  void keyItemPressed(CQChartsKeyItem *);
  void keyItemIdPressed(const QString &);

  void keyPressed(CQChartsPlotKey *);
  void keyIdPressed(const QString &);

  // title signals (title pressed)
  void titlePressed(CQChartsTitle *);
  void titleIdPressed(const QString &);

  // annotation signals (annotation pressed)
  void annotationPressed(CQChartsAnnotation *);
  void annotationIdPressed(const QString &);

  // object signals (object pressed)
  void objPressed(CQChartsPlotObj *);
  void objIdPressed(const QString &);

  // zoom/pan changed
  void zoomPanChanged();

  // annotations changed
  void annotationsChanged();

  // selection changed
  void selectionChanged();

  // errors cleared or added
  void errorsCleared();
  void errorAdded();

 protected:
  struct NoUpdate {
    NoUpdate(const CQChartsPlot *plot, bool update=false) :
     plot_(const_cast<CQChartsPlot *>(plot)), update_(update) {
      plot_->setUpdatesEnabled(false);
    }

    NoUpdate(CQChartsPlot *plot, bool update=false) :
     plot_(plot), update_(update) {
      plot_->setUpdatesEnabled(false);
    }

   ~NoUpdate() { plot_->setUpdatesEnabled(true, update_); }

    CQChartsPlot* plot_   { nullptr };
    bool          update_ { false };
  };

  using ObjSet     = std::set<CQChartsObj*>;
  using SizeObjSet = std::map<double,ObjSet>;

 protected:
  void connectModel();
  void disconnectModel();

  void connectDisconnectModel(bool connectDisconnect);

  //---

  void objsAtPoint(const Point &p, Objs &objs) const;

  void plotObjsAtPoint1(const Point &p, PlotObjs &objs) const;

  virtual void plotObjsAtPoint(const Point &p, PlotObjs &objs) const;

  void annotationsAtPoint(const Point &p, Annotations &annotations) const;

  void objsIntersectRect(const BBox &r, Objs &objs, bool inside, bool select=false) const;

  virtual bool objNearestPoint(const Point &p, CQChartsPlotObj* &obj) const;

 protected:
  //*! update state
  enum class UpdateState {
    INVALID,                //!< invalid state
    CALC_RANGE,             //!< calculating range
    CALC_OBJS,              //!< calculating objects
    DRAW_OBJS,              //!< drawing objects
    READY,                  //!< ready to draw
    UPDATE_RANGE,           //!< needs range update
    UPDATE_OBJS,            //!< needs objs update
    UPDATE_DRAW_OBJS,       //!< needs draw objects
    UPDATE_DRAW_BACKGROUND, //!< needs draw background
    UPDATE_DRAW_FOREGROUND, //!< needs draw foreground
    UPDATE_VIEW,            //!< update view
    DRAWN                   //!< drawn
  };

  struct ThreadData {
    CHRTime           startTime;
    std::future<void> future;
    std::atomic<bool> busy { false };

    void start(const CQChartsPlot *plot, const char *id) {
      if (id) {
        std::cerr << "Start: " << plot->id().toStdString() << " : " << id << "\n";

        startTime = CHRTime::getTime();
      }

      busy.store(true);
    }

    void end(const CQChartsPlot *plot, const char *id) {
      busy.store(false);

      if (id) {
        CHRTime dt = startTime.diffTime();

        std::cerr << "Elapsed: " << plot->id().toStdString() << " : " << id << " " <<
                     dt.getMSecs() << "\n";
      }
    }

    void finish(const CQChartsPlot *plot, const char *id) {
      if (id) {
        CHRTime dt = startTime.diffTime();

        std::cerr << "Finish: " << plot->id().toStdString() << " : " << id << " " <<
                     dt.getMSecs() << "\n";
      }
    }
  };

  struct LockData {
    mutable std::mutex lock;
    const char*        id { nullptr };
  };

  struct DrawBusyData {
    QColor       bgColor  { 255, 255, 255 };
    QColor       fgColor  { 100, 200, 100 };
    CQChartsFont font;
    int          count    { 10 };
    int          multiple { 10 };
    mutable int  ind      { 0 };
  };

  struct UpdateData {
    std::atomic<int> state       { 0 };
    std::atomic<int> interrupt   { 0 };
    ThreadData       rangeThread;
    ThreadData       objsThread;
    ThreadData       drawThread;
    LockData         lockData;
    bool             updateObjs  { false };
    QTimer*          timer       { nullptr };
    DrawBusyData     drawBusy;
  };

  const QColor &updateBusyColor() const { return updateData_.drawBusy.fgColor; }
  void setUpdateBusyColor(const QColor &c) { updateData_.drawBusy.fgColor = c; }

  int updateBusyCount() const { return updateData_.drawBusy.count; }
  void setUpdateBusyCount(int i) { updateData_.drawBusy.count = i; }

  void setGroupedUpdateState(UpdateState state);

  UpdateState updateState() { return (UpdateState) updateData_.state.load(); }
  void setUpdateState(UpdateState state);

  UpdateState calcNextState() const;

  void setInterrupt(bool b=true);

  bool hasLockId() const { return updateData_.lockData.id; }
  void setLockId(const char *id) { updateData_.lockData.id = id; }
  void resetLockId() { updateData_.lockData.id = nullptr; }

  void updateLock(const char *id) {
    //std::cerr << "> " << id << "\n";
    assert(! hasLockId());
    updateData_.lockData.lock.lock();
    setLockId(id);
  }

  bool updateTryLock(const char *id) {
    //std::cerr << "> " << id << "\n";
    assert(! hasLockId());
    bool locked = updateData_.lockData.lock.try_lock();
    setLockId(id);
    return locked;
  }

  void updateUnLock() {
    //std::cerr << "< " << updateData_.lockData.id << "\n";
    updateData_.lockData.lock.unlock();
    resetLockId();
   }

  struct LockMutex {
    LockMutex(CQChartsPlot *plot, const char *id) : plot(plot) { plot->updateLock(id); }
   ~LockMutex() { plot->updateUnLock(); }

    CQChartsPlot* plot { nullptr };
  };

  struct TryLockMutex {
    TryLockMutex(CQChartsPlot *plot, const char *id) : plot(plot) {
      locked = plot->updateTryLock(id); }
   ~TryLockMutex() { if (locked) plot->updateUnLock(); }

    CQChartsPlot *plot   { nullptr };
    bool          locked { false };
  };

  void drawBusy(QPainter *painter, const UpdateState &updateState) const;

 public:
  bool isInterrupt() const { return updateData_.interrupt.load() > 0; }

 protected:
  using IdHidden        = std::map<int,bool>;
  using Rows            = std::set<int>;
  using ColumnRows      = std::map<int,Rows>;
  using IndexColumnRows = std::map<QModelIndex,ColumnRows>;

  //! color column data
  struct ColorColumnData {
    CQChartsColumn     column;
    ColorType          colorType { ColorType::AUTO };
    bool               valid     { false };
    bool               mapped    { true };
    double             map_min   { 0.0 };
    double             map_max   { 1.0 };
    double             data_min  { 0.0 };
    double             data_max  { 1.0 };
    ColumnType         modelType;
    QString            palette;
    CQChartsColorStops xStops;
    CQChartsColorStops yStops;
  };

  //! every row selection data
  struct EveryData {
    bool enabled { false };
    int  start   { 0 };
    int  end     { std::numeric_limits<int>::max() };
    int  step    { 1 };
  };

  //! drag object
  enum class DragObj {
    NONE,
    KEY,
    XAXIS,
    YAXIS,
    TITLE,
    ANNOTATION,
    PLOT,
    PLOT_HANDLE
  };

  //! mouse state data
  struct MouseData {
    Point              pressPoint { 0, 0 };
    Point              movePoint  { 0, 0 };
    bool               pressed    { false };
    DragObj            dragObj    { DragObj::NONE };
    CQChartsResizeSide dragSide   { CQChartsResizeSide::NONE };
    bool               dragged    { false };
  };

  //! animation data
  struct AnimateData {
    QTimer* timer   { nullptr };
    int     tickLen { 30 };
  };

  //! update state data
  struct UpdatesData {
    using StateFlag = std::map<UpdateState,int>;

    int       enabled            { 0 };     //!< updates enabled
    bool      updateRangeAndObjs { false }; //!< call execUpdateRangeAndObjs (on enable)
    bool      updateObjs         { false }; //!< call execUpdateObjs (on enable)
    bool      applyDataRange     { false }; //!< call applyDataRange (on enable)
    bool      invalidateLayers   { false }; //!< call needsInvalidate invalidate (on enable)
    StateFlag stateFlag;                    //!< state flags

    void reset() {
      updateRangeAndObjs = false;
      updateObjs         = false;
      applyDataRange     = false;
      invalidateLayers   = false;

      //stateFlag.clear();
    }
  };

  using ColumnNames = std::map<CQChartsColumn,QString>;

  //---

 protected:
  CQChartsView*        view_          { nullptr }; //!< parent view
  CQChartsPlotType*    type_          { nullptr }; //!< plot type data
  ModelP               model_;                     //!< abstract model
  bool                 modelNameSet_  { false };   //!< model name set from plot
  CQPropertyViewModel* propertyModel_ { nullptr }; //!< property model

  // name
  QString name_; //!< custom name

  // ranges
  BBox                  viewBBox_        { 0, 0, 1, 1 };     //!< view box
  BBox                  innerViewBBox_   { 0, 0, 1, 1 };     //!< inner view box
  CQChartsPlotMargin    innerMargin_     { 0, 0, 0, 0 };     //!< inner margin
  CQChartsPlotMargin    outerMargin_     { 10, 10, 10, 10 }; //!< outer margin
  CQChartsDisplayRange* displayRange_    { nullptr };        //!< value range mapping
  Range                 calcDataRange_;                      //!< calc data range
  Range                 dataRange_;                          //!< data range
  Range                 outerDataRange_;                     //!< outer data range
  ZoomData              zoomData_;                           //!< zoom data

  // override range
  CQChartsOptReal xmin_; //!< xmin override
  CQChartsOptReal ymin_; //!< ymin override
  CQChartsOptReal xmax_; //!< xmax override
  CQChartsOptReal ymax_; //!< ymax override

  // filter
  EveryData everyData_;        //!< every data
  QString   filterStr_;        //!< data filter
  QString   visibleFilterStr_; //!< visible filter

  bool skipBad_ { true }; //!< skip bad values

  // borders
  CQChartsSides plotBorderSides_  { "tlbr" }; //!< plot border sides
  bool          plotClip_         { true };   //!< is clipped at plot limits
  CQChartsSides dataBorderSides_  { "tlbr" }; //!< data border sides
  bool          dataClip_         { false };  //!< is clipped at data limits
  CQChartsSides fitBorderSides_   { "tlbr" }; //!< fit border sides

  // title
  CQChartsTitle* titleObj_  { nullptr }; //!< title object
  QString        titleStr_;              //!< title string
  QString        fileName_;              //!< associated data filename

  // axes
  CQChartsAxis* xAxis_ { nullptr }; //!< x axis object
  CQChartsAxis* yAxis_ { nullptr }; //!< y axis object

  // key
  CQChartsPlotKey* keyObj_   { nullptr }; //!< key object
  bool             colorKey_ { false };   //!< use color column for key

  // columns
  CQChartsColumn  xValueColumn_;   //!< x axis value column
  CQChartsColumn  yValueColumn_;   //!< y axis value column
  CQChartsColumn  idColumn_;       //!< unique data id column (signalled)
  CQChartsColumns tipColumns_;     //!< tip columns
  CQChartsColumn  visibleColumn_;  //!< visible column
  CQChartsColumn  fontColumn_;     //!< font column
  CQChartsColumn  imageColumn_;    //!< image column
  CQChartsColumns controlColumns_; //!< control columns

  // color data
  ColorColumnData    colorColumnData_; //!< color color data
  mutable std::mutex colorMutex_;      //!< color mutex

  // cached column names
  ColumnNames columnNames_; //!< column header names

  // font
  CQChartsFont font_;                      //!< font
  double       minScaleFontSize_ { 6.0 };  //!< min scaled font size
  double       maxScaleFontSize_ { 48.0 }; //!< max scaled font size

  CQChartsFont tabbedFont_; //!< font for tab text

  // palette
  CQChartsPaletteName defaultPalette_; //!< default palette

  // scaling
  bool equalScale_ { false }; //!< equal scaled

  // follow mouse
  bool followMouse_ { true }; //!< track object under mouse

  // fit
  bool autoFit_      { false }; //!< auto fit on data change
  bool needsAutoFit_ { false }; //!< needs auto fit on next draw

  // preview
  bool preview_        { false }; //!< is preview plot
  int  previewMaxRows_ { 1000 };  //!< preview max rows

  bool sequential_    { false }; //!< is sequential (non-threaded)
  bool queueUpdate_   { true };  //!< is queued update
  bool bufferSymbols_ { false }; //!< buffer symbols
  bool showBoxes_     { false }; //!< show debug boxes
  bool overview_      { false }; //!< is overview

  // invert
  bool invertX_ { false }; //!< x values inverted
  bool invertY_ { false }; //!< y values inverted

  // no data
  bool noData_ { false }; //!< is no data

  // debug
  bool debugUpdate_   { false }; //!< debug update
  bool debugQuadTree_ { false }; //!< debug quad tree

  // connect data (overlay, x1/x2, y1/y2)
  ConnectData connectData_; //!< associated plot data

  // objects
  PlotObjs plotObjs_; //!< plot objects

  // inside
  int        insideInd_     { 0 }; //!< current inside object ind
  ObjSet     insideObjs_;          //!< inside plot objects
  SizeObjSet sizeInsideObjs_;      //!< inside plot objects
                                                                 //!< (size sorted)

  //---

  // object tree data
  struct ObjTreeData {
    bool                 init   { false };   //!< needs init obj tree
    CQChartsPlotObjTree* tree   { nullptr }; //!< plot object quad tree
    bool                 isSet  { false };   //!< is plot object quad tree set
    bool                 notify { false };   //!< is plot object quad tree notify
  };

  ObjTreeData objTreeData_; //!< object tree data

  //---

  UpdateData  updateData_;  //!< update data
  MouseData   mouseData_;   //!< mouse event data
  AnimateData animateData_; //!< animation data

  // draw layers, buffers
  Buffers                      buffers_;    //!< draw layer buffers
  Layers                       layers_;     //!< draw layers
  mutable CQChartsBuffer::Type drawBuffer_; //!< objects draw buffer

  IdHidden idHidden_;  //!< hidden object ids
  QVariant hideValue_; //!< hide value

  IndexColumnRows selIndexColumnRows_; //!< sel model indices (by col/row)

  // edit handles
  CQChartsEditHandles* editHandles_ { nullptr }; //!< edit controls
  bool                 editing_     { false };   //!< is editing

  // annotations
  Annotations  annotations_;      //!< extra annotations
  Annotations  pressAnnotations_; //!< press annotations
  mutable BBox annotationBBox_;   //!< cached annotation bbox

  //---

  UpdatesData updatesData_;              //!< updates data
  bool        fromInvalidate_ { false }; //!< call from invalidate

  //---

  // tab data
  struct TabData {
    double pxm { 0.0 };
    double pym { 0.0 };
    double ptw { 0.0 };
    double pth { 0.0 };
  };

  TabData tabData_;

  //---

  // error data
  struct ErrorData {
    Errors       globalErrors; //!< global errors
    ColumnErrors columnErrors; //!< column errors
    DataErrors   dataErrors;   //!< data access errors

    void clear() {
      globalErrors.clear();
      columnErrors.clear();
      dataErrors  .clear();
    }

    bool hasErrors() const {
      return (! globalErrors.empty() || ! columnErrors.empty() || ! dataErrors.empty());
    }
  };

  ErrorData errorData_;

  //---

  mutable std::mutex resizeMutex_; //!< resize mutex
};

//------

CQCHARTS_NAMED_LINE_DATA(Grid,grid)
CQCHARTS_NAMED_SHAPE_DATA(Node,node)
CQCHARTS_NAMED_LINE_DATA(Edge,edge)
CQCHARTS_NAMED_POINT_DATA(Dot,dot)
CQCHARTS_NAMED_POINT_DATA(Rug,rug)

#endif
