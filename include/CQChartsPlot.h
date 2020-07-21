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
#include <CQChartsWidget.h>
#include <CQChartsUtil.h>
#include <CQChartsTypes.h>
#include <CQChartsGeom.h>
#include <CQChartsPlotMargin.h>
#include <CQChartsOptReal.h>
#include <CQChartsColorStops.h>
#include <CQChartsPaletteName.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsModelTypes.h>
#include <CQChartsModelIndex.h>
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
class CQChartsButtonAnnotation;
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
class CQChartsWidgetAnnotation;

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
  Q_PROPERTY(CQChartsColumns noTipColumns   READ noTipColumns   WRITE setNoTipColumns  )
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

  // fit
  Q_PROPERTY(bool autoFit READ isAutoFit WRITE setAutoFit)

  Q_PROPERTY(CQChartsLength fitMarginLeft   READ fitMarginLeft   WRITE setFitMarginLeft  )
  Q_PROPERTY(CQChartsLength fitMarginTop    READ fitMarginTop    WRITE setFitMarginTop   )
  Q_PROPERTY(CQChartsLength fitMarginRight  READ fitMarginRight  WRITE setFitMarginRight )
  Q_PROPERTY(CQChartsLength fitMarginBottom READ fitMarginBottom WRITE setFitMarginBottom)

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
  using IMinMax  = CQChartsGeom::IMinMax;
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

  using PlotObj  = CQChartsPlotObj;
  using PlotObjs = std::vector<PlotObj *>;

  using Obj  = CQChartsObj;
  using Objs = std::vector<Obj *>;

  using QModelIndices  = std::vector<QModelIndex>;
  using QModelIndexSet = std::set<QModelIndex>;

  using Column     = CQChartsColumn;
  using Columns    = CQChartsColumns;
  using ColumnType = CQBaseModelType;
  using ModelIndex = CQChartsModelIndex;

  using Plot     = CQChartsPlot;
  using Plots    = std::vector<Plot *>;
  using PlotType = CQChartsPlotType;

  using Buffer  = CQChartsBuffer;
  using Buffers = std::map<Buffer::Type,Buffer *>;

  using Layer  = CQChartsLayer;
  using Layers = std::map<Layer::Type,Layer *>;

  using ColorInd = CQChartsUtil::ColorInd;

  using DrawType = CQChartsView::DrawType;

  using View          = CQChartsView;
  using Axis          = CQChartsAxis;
  using Title         = CQChartsTitle;
  using PlotKey       = CQChartsPlotKey;
  using EditHandles   = CQChartsEditHandles;
  using PlotParameter = CQChartsPlotParameter;

  using Font        = CQChartsFont;
  using Color       = CQChartsColor;
  using Alpha       = CQChartsAlpha;
  using Length      = CQChartsLength;
  using Angle       = CQChartsAngle;
  using FillPattern = CQChartsFillPattern;
  using LineDash    = CQChartsLineDash;
  using Position    = CQChartsPosition;
  using Rect        = CQChartsRect;
  using Image       = CQChartsImage;
  using Widget      = CQChartsWidget;

  using PaintDevice       = CQChartsPaintDevice;
  using ScriptPaintDevice = CQChartsScriptPaintDevice;
  using SVGPaintDevice    = CQChartsSVGPaintDevice;
  using HtmlPaintDevice   = CQChartsHtmlPaintDevice;
  using BrushData         = CQChartsBrushData;
  using PenData           = CQChartsPenData;

  using DisplayRange = CQChartsDisplayRange;
  using ValueSet     = CQChartsValueSet;
  using OptReal      = CQChartsOptReal;
  using PenBrush     = CQChartsPenBrush;
  using Sides        = CQChartsSides;
  using PlotMargin   = CQChartsPlotMargin;
  using Symbol       = CQChartsSymbol;
  using ColorStops   = CQChartsColorStops;
  using PaletteName  = CQChartsPaletteName;

 public:
  CQChartsPlot(View *view, PlotType *type, const ModelP &model);

  virtual ~CQChartsPlot();

  //---

  View *view() const { return view_; }

  PlotType *type() const { return type_; }

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

  void writeScript(ScriptPaintDevice *device) const;

  void writeScriptRange(ScriptPaintDevice *device) const;

  void writeSVG(SVGPaintDevice *device) const;

  void writeHtml(HtmlPaintDevice *device) const;

  //---

  const DisplayRange &displayRange() const;
  void setDisplayRange(const DisplayRange &r);

  const Range &dataRange() const { return dataRange_; }
  void setDataRange(const Range &r, bool update=true);

  void resetDataRange(bool updateRange=true, bool updateObjs=true);

  virtual Range objTreeRange() const { return dataRange_; }

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

  const OptReal &xmin() const { return xmin_; }
  void setXMin(const OptReal &r);

  const OptReal &xmax() const { return xmax_; }
  void setXMax(const OptReal &r);

  const OptReal &ymin() const { return ymin_; }
  void setYMin(const OptReal &r);

  const OptReal &ymax() const { return ymax_; }
  void setYMax(const OptReal &r);

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
  const Sides &plotBorderSides() const { return plotBorderSides_; }
  void setPlotBorderSides(const Sides &s);

  bool isPlotClip() const { return plotClip_; }
  void setPlotClip(bool b);

  //---

  // data area
  const Sides &dataBorderSides() const { return dataBorderSides_; }
  void setDataBorderSides(const Sides &s);

  bool isDataClip() const { return dataClip_; }
  void setDataClip(bool b);

  //---

  // fit area
  const Sides &fitBorderSides() const { return fitBorderSides_; }
  void setFitBorderSides(const Sides &s);

  //---

  // font
  const Font &font() const { return font_; }
  virtual void setFont(const Font &f);

  QFont qfont() const;
  QFont qfont(const Font &font) const;

  //---

  // default palette
  const PaletteName &defaultPalette() const { return defaultPalette_; }
  void setDefaultPalette(const PaletteName &name);

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

  //---

  // fit
  bool isAutoFit() const { return autoFit_; }
  void setAutoFit(bool b);

  const PlotMargin &fitMargin() const { return fitMargin_; }
  void setFitMargin(const PlotMargin &m);

  const Length &fitMarginLeft() const { return fitMargin().left(); }
  void setFitMarginLeft(const Length &l);

  const Length &fitMarginTop() const { return fitMargin().top(); }
  void setFitMarginTop(const Length &t);

  const Length &fitMarginRight() const { return fitMargin().right(); }
  void setFitMarginRight(const Length &r);

  const Length &fitMarginBottom() const { return fitMargin().bottom(); }
  void setFitMarginBottom(const Length &b);

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
  virtual BBox adjustedViewBBox(const Plot *plot) const;

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
  const PlotMargin &innerMargin() const { return innerMargin_; }
  void setInnerMargin(const PlotMargin &m);

  const Length &innerMarginLeft() const { return innerMargin().left(); }
  void setInnerMarginLeft(const Length &l);

  const Length &innerMarginTop() const { return innerMargin().top(); }
  void setInnerMarginTop(const Length &t);

  const Length &innerMarginRight() const { return innerMargin().right(); }
  void setInnerMarginRight(const Length &r);

  const Length &innerMarginBottom() const { return innerMargin().bottom(); }
  void setInnerMarginBottom(const Length &b);

  // outer margin
  const PlotMargin &outerMargin() const { return outerMargin_; }
  void setOuterMargin(const PlotMargin &m);

  const Length &outerMarginLeft() const { return outerMargin().left(); }
  void setOuterMarginLeft(const Length &l);

  const Length &outerMarginTop() const { return outerMargin().top(); }
  void setOuterMarginTop(const Length &t);

  const Length &outerMarginRight() const { return outerMargin().right(); }
  void setOuterMarginRight(const Length &r);

  const Length &outerMarginBottom() const { return outerMargin().bottom(); }
  void setOuterMarginBottom(const Length &b);

  //---

  // aspect ratio
  double aspect() const;

  //---

  Axis *xAxis() const { return xAxis_; }
  Axis *yAxis() const { return yAxis_; }

  virtual Axis *mappedXAxis() const { return xAxis(); }
  virtual Axis *mappedYAxis() const { return yAxis(); }

  virtual bool xAxisName(QString &, const QString& ="") const { return false; }
  virtual bool yAxisName(QString &, const QString& ="") const { return false; }

  void setOverlayPlotsAxisNames();
  void setPlotsAxisNames(const Plots &plots, Plot *axisPlot);

  //---

  PlotKey *key() const { return keyObj_; }

  Title *title() const { return titleObj_; }

  //---

  Plot *parentPlot() const { return connectData_.parent; }
  void setParentPlot(Plot *parent) { connectData_.parent = parent; }

  virtual int childPlotIndex(const Plot *) const { return -1; }
  virtual int numChildPlots() const { return 0; }
  virtual Plot *childPlot(int) const { return nullptr; }

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

  void setTabbedFont(const Font &f);
  const Font &tabbedFont() const;

  //-

  Plot *prevPlot() const { return connectData_.prev; }
  Plot *nextPlot() const { return connectData_.next; }

  void setNextPlot(Plot *plot, bool notify=true);
  void setPrevPlot(Plot *plot, bool notify=true);

  const Plot *firstPlot() const;
  Plot *firstPlot();

  const Plot *lastPlot() const;
  Plot *lastPlot();

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

  void x1x2Plots(Plot* &plot1, Plot* &plot2);
  void y1y2Plots(Plot* &plot1, Plot* &plot2);

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

  void addSymbolProperties(const QString &path, const QString &prefix,
                           const QString &descPrefix, bool hidden=false);

  void addLineProperties(const QString &path, const QString &prefix,
                         const QString &descPrefix, bool hidden=false);
  void addFillProperties(const QString &path, const QString &prefix,
                         const QString &descPrefix, bool hidden=false);

  void addTextProperties(const QString &path, const QString &prefix, const QString &descPrefix,
                         uint valueTypes=CQChartsTextOptions::ValueType::SIMPLE,
                         bool hidden=false);

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

  void getObjectPropertyNames(PlotObj *plotObj, QStringList &names) const;

  void hideProperty(const QString &path, QObject *object);

  //---

  // margins
  void updateMargins(bool update=true);

  void updateMargins(const PlotMargin &outerMargin);

  //---

  ModelIndex  normalizeIndex(const ModelIndex &ind) const;
  QModelIndex normalizeIndex(const QModelIndex &ind) const;

  ModelIndex  unnormalizeIndex(const ModelIndex &ind) const;
  QModelIndex unnormalizeIndex(const QModelIndex &ind) const;

  void proxyModels(std::vector<QSortFilterProxyModel *> &proxyModels,
                   QAbstractItemModel* &sourceModel) const;

  //---

  void addColumnValues(const Column &column, CQChartsValueSet &valueSet) const;

  //---

  using ModelVisitor = CQChartsPlotModelVisitor;

  void visitModel(ModelVisitor &visitor) const;

  //---

  bool modelMappedReal(const ModelIndex &ind, double &r, bool log, double def) const;
  bool modelMappedReal(int row, const Column &col, const QModelIndex &ind,
                       double &r, bool log, double def) const;

  //---

  int getRowForId(const QString &id) const;

  QString idColumnString(int row, const QModelIndex &parent, bool &ok) const;

  //----

  QModelIndex modelIndex(const ModelIndex &ind) const;

  QModelIndex modelIndex(int row, const Column &column,
                         const QModelIndex &parent=QModelIndex(), bool normalized=false) const;

  //----

#if 0
  QVariant modelHHeaderValue(const Column &column, int role, bool &ok) const;
  QVariant modelHHeaderValue(const Column &column, bool &ok) const;

  virtual QVariant modelHHeaderValue(QAbstractItemModel *model, const Column &column,
                                     int role, bool &ok) const;
  virtual QVariant modelHHeaderValue(QAbstractItemModel *model, const Column &column,
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

  QString modelHHeaderString(const Column &column, int role, bool &ok) const;
  QString modelHHeaderString(const Column &column, bool &ok) const;

  virtual QString modelHHeaderString(QAbstractItemModel *model, const Column &column,
                                     int role, bool &ok) const;
  virtual QString modelHHeaderString(QAbstractItemModel *model, const Column &column,
                                     bool &ok) const;

  //---

  QString modelHHeaderTip(const Column &column, bool &ok) const;

  //---

  QString modelVHeaderString(int section, Qt::Orientation orient, int role, bool &ok) const;
  QString modelVHeaderString(int section, Qt::Orientation orient, bool &ok) const;

  virtual QString modelVHeaderString(QAbstractItemModel *model, int section,
                                     Qt::Orientation orientation, int role, bool &ok) const;
  virtual QString modelVHeaderString(QAbstractItemModel *model,  int section,
                                     Qt::Orientation orientation, bool &ok) const;

  //---

  QVariant modelValue(const ModelIndex &ind, int role, bool &ok) const;
  QVariant modelValue(const ModelIndex &ind, bool &ok) const;

  QVariant modelValue(int row, const Column &column,
                      const QModelIndex &parent, int role, bool &ok) const;
  QVariant modelValue(int row, const Column &column,
                      const QModelIndex &parent, bool &ok) const;

  virtual QVariant modelValue(QAbstractItemModel *model, int row, const Column &column,
                              const QModelIndex &parent, int role, bool &ok) const;
  virtual QVariant modelValue(QAbstractItemModel *model, int row, const Column &column,
                              const QModelIndex &parent, bool &ok) const;

  //---

  QString modelString(const ModelIndex &ind, int role, bool &ok) const;
  QString modelString(const ModelIndex &ind, bool &ok) const;

  QString modelString(int row, const Column &column,
                      const QModelIndex &parent, int role, bool &ok) const;
  QString modelString(int row, const Column &column,
                      const QModelIndex &parent, bool &ok) const;

  QString modelString(QAbstractItemModel *model, const ModelIndex &ind, bool &ok) const;

  virtual QString modelString(QAbstractItemModel *model, int row, const Column &column,
                              const QModelIndex &parent, int role, bool &ok) const;
  virtual QString modelString(QAbstractItemModel *model, int row, const Column &column,
                              const QModelIndex &parent, bool &ok) const;

  //---

  double modelReal(int row, const Column &column,
                   const QModelIndex &parent, int role, bool &ok) const;
  double modelReal(int row, const Column &column,
                   const QModelIndex &parent, bool &ok) const;

  double modelReal(const ModelIndex &ind, int role, bool &ok) const;
  double modelReal(const ModelIndex &ind, bool &ok) const;

  virtual double modelReal(QAbstractItemModel *model, int row, const Column &column,
                           const QModelIndex &parent, int role, bool &ok) const;
  virtual double modelReal(QAbstractItemModel *model, int row, const Column &column,
                           const QModelIndex &parent, bool &ok) const;

  //---

  long modelInteger(int row, const Column &column,
                    const QModelIndex &parent, int role, bool &ok) const;
  long modelInteger(int row, const Column &column,
                    const QModelIndex &parent, bool &ok) const;

  long modelInteger(const ModelIndex &ind, int role, bool &ok) const;
  long modelInteger(const ModelIndex &ind, bool &ok) const;

  virtual long modelInteger(QAbstractItemModel *model, int row, const Column &column,
                            const QModelIndex &parent, int role, bool &ok) const;
  virtual long modelInteger(QAbstractItemModel *model, int row, const Column &column,
                            const QModelIndex &parent, bool &ok) const;

  //---

#if 0
  Color modelColor(int row, const Column &column,
                   const QModelIndex &parent, bool &ok) const;
  Color modelColor(int row, const Column &column,
                   const QModelIndex &parent, int role, bool &ok) const;

  virtual Color modelColor(QAbstractItemModel *model, int row, const Column &column,
                           const QModelIndex &parent, int role, bool &ok) const;
  virtual Color modelColor(QAbstractItemModel *model, int row, const Column &column,
                           const QModelIndex &parent, bool &ok) const;
#endif

  //---

  std::vector<double> modelReals(const ModelIndex &ind, bool &ok) const;
  std::vector<double> modelReals(int row, const Column &column,
                                 const QModelIndex &parent, bool &ok) const;

  //---

  QVariant modelRootValue(const ModelIndex &ind, int role, bool &ok) const;

  QVariant modelRootValue(int row, const Column &column,
                          const QModelIndex &parent, int role, bool &ok) const;
  QVariant modelRootValue(int row, const Column &column,
                          const QModelIndex &parent, bool &ok) const;

  //---

  QVariant modelHierValue(const ModelIndex &ind, bool &ok) const;
  QVariant modelHierValue(int row, const Column &column,
                          const QModelIndex &parent, int role, bool &ok) const;
  QVariant modelHierValue(int row, const Column &column,
                          const QModelIndex &parent, bool &ok) const;

  //---

  QString modelHierString(const ModelIndex &ind, bool &ok) const;
  QString modelHierString(int row, const Column &column,
                          const QModelIndex &parent, int role, bool &ok) const;
  QString modelHierString(int row, const Column &column,
                          const QModelIndex &parent, bool &ok) const;

  //---

#if 0
  double modelHierReal(int row, const Column &column,
                       const QModelIndex &parent, int role, bool &ok) const;
  double modelHierReal(int row, const Column &column,
                       const QModelIndex &parent, bool &ok) const;
#endif

  //---

#if 0
  long modelHierInteger(int row, const Column &column,
                        const QModelIndex &parent, int role, bool &ok) const;
  long modelHierInteger(int row, const Column &column,
                        const QModelIndex &parent, bool &ok) const;
#endif

  //---

  bool isSelectIndex(const QModelIndex &ind, int row, const Column &col,
                     const QModelIndex &parent=QModelIndex()) const;

  QModelIndex selectIndex(int row, const Column &col,
                          const QModelIndex &parent=QModelIndex()) const;

  void beginSelectIndex();

  void addSelectIndex(const ModelIndex &ind);
  void addSelectIndex(int row, int col, const QModelIndex &parent=QModelIndex());
  void addSelectIndex(const QModelIndex &ind);

  void endSelectIndex();

  //---

  double logValue(double x, int base=10) const;
  double expValue(double x, int base=10) const;

  //---

  Point positionToPlot (const Position &pos) const;
  Point positionToPixel(const Position &pos) const;

  BBox rectToPlot (const Rect &rect) const;
  BBox rectToPixel(const Rect &rect) const;

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
  void doAddKeyItems(PlotKey *key);

  virtual void addKeyItems(PlotKey *) { }

  bool addColorKeyItems(PlotKey *key);

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

  static void updateRangeASync(Plot *plot);

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

  static void updateObjsASync(Plot *plot);

  void updateObjsThread();

  void interruptObjs();

  void execWaitObjs();

  //---

  void updatePlotObjs();

  void resetInsideObjs();

  void updateGroupedDraw();
  void updateDraw();

  //---

  static void drawASync(Plot *plot);

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
    Column column;             //!< symbol type column
    bool   valid    { false }; //!< symbol type valid
    bool   mapped   { false }; //!< symbol type values mapped
    int    data_min { 0 };     //!< model data min
    int    data_max { 1 };     //!< model data max
    int    map_min  { 0 };     //!< mapped size min
    int    map_max  { 1 };     //!< mapped size max
  };

  void initSymbolTypeData(SymbolTypeData &symbolTypeData) const;

  bool columnSymbolType(int row, const QModelIndex &parent, const SymbolTypeData &symbolTypeData,
                        Symbol &symbolType) const;

  //---

  struct SymbolSizeData {
    Column  column;              //!< symbol size column
    bool    valid     { false }; //!< symbol size valid
    bool    mapped    { false }; //!< symbol size values mapped
    double  data_min  { 0.0 };   //!< model data min
    double  data_max  { 1.0 };   //!< model data max
    double  data_mean { 0.0 };   //!< model data mean
    double  map_min   { 0.0 };   //!< mapped size min
    double  map_max   { 1.0 };   //!< mapped size max
    QString units     { "px" };  //!< mapped size units
  };

  void initSymbolSizeData(SymbolSizeData &symbolSizeData) const;

  bool columnSymbolSize(int row, const QModelIndex &parent, const SymbolSizeData &symbolSizeData,
                        Length &symbolSize) const;

  //---

  struct FontSizeData {
    Column  column;             //!< font size column
    bool    valid    { false }; //!< font size valid
    bool    mapped   { false }; //!< font size values mapped
    double  data_min { 0.0 };   //!< model data min
    double  data_max { 1.0 };   //!< model data max
    double  map_min  { 0.0 };   //!< mapped size min
    double  map_max  { 1.0 };   //!< mapped size max
    QString units    { "px" };  //!< mapped size units
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

  void crearOverlayErrors();
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

  void addPlotObject(PlotObj *obj);

  virtual void clearPlotObjects();

  void invalidateObjTree();

  bool updateInsideObjects(const Point &w);

  Obj *insideObject() const;

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
    Column  column;
    QString msg;
  };

  // error accessing data in model
  struct DataError {
    ModelIndex ind;
    QString    msg;
  };

  using Errors       = std::vector<Error>;
  using ColumnErrors = std::vector<ColumnError>;
  using DataErrors   = std::vector<DataError>;

  void clearErrors();

  bool hasErrors() const;

  bool addError(const QString &msg);
  bool addColumnError(const Column &c, const QString &msg);
  bool addDataError(const ModelIndex &ind, const QString &msg);

  void getErrors(QStringList &errors);

  void addErrorsToWidget(QTextBrowser *text);

  //---

  const PlotObjs &plotObjects() const { return plotObjs_; }

  int numPlotObjects() const { return plotObjs_.size(); }

  PlotObj *plotObject(int i) const { return plotObjs_[i]; }

  bool isNoData() const { return noData_; }
  void setNoData(bool b) { noData_ = b; }

  bool isPlotObjTreeSet() const { return objTreeData_.isSet; }
  void setPlotObjTreeSet(bool b);

  //----

  // columns
  const Column &xValueColumn() const { return xValueColumn_; }
  void setXValueColumn(const Column &column);

  const Column &yValueColumn() const { return yValueColumn_; }
  void setYValueColumn(const Column &column);

  const Column &idColumn() const { return idColumn_; }
  void setIdColumn(const Column &column);

  const Columns &tipColumns() const { return tipColumns_; }
  void setTipColumns(const Columns &columns);

  const Columns &noTipColumns() const { return noTipColumns_; }
  void setNoTipColumns(const Columns &columns);

  const Column &visibleColumn() const { return visibleColumn_; }
  void setVisibleColumn(const Column &column);

  const Column &imageColumn() const { return imageColumn_; }
  void setImageColumn(const Column &column);

  const Column &colorColumn() const { return colorColumnData_.column; };
  void setColorColumn(const Column &c);

  const Column &fontColumn() const { return fontColumn_; };
  void setFontColumn(const Column &c);

  const Columns &controlColumns() const { return controlColumns_; }
  void setControlColumns(const Columns &columns);

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

  const ColorStops &colorXStops() const { return colorColumnData_.xStops; }
  void setColorXStops(const ColorStops &s);

  const ColorStops &colorYStops() const { return colorColumnData_.yStops; }
  void setColorYStops(const ColorStops &s);

  //---

  // color column
  bool colorColumnColor(int row, const QModelIndex &parent, Color &color) const;

  bool modelIndexColor(const ModelIndex &ind, Color &color) const;

  bool columnValueColor(const QVariant &var, Color &color) const;

  //---

  // font column
  bool fontColumnFont(int row, const QModelIndex &parent, Font &font) const;

  bool modelIndexFont(const ModelIndex &ind, Font &font) const;

  bool columnValueFont(const QVariant &var, Font &font) const;

  //---

  virtual QString keyText() const;

  //---

  virtual QString posStr(const Point &w) const;

  virtual QString xStr(double x) const;
  virtual QString yStr(double y) const;

  virtual QString columnStr(const Column &column, double x) const;

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

  Plot *tabbedPressPlot(const Point &w, Plots &plots) const;

  bool keySelectPress  (PlotKey *key  , const Point &w, SelMod selMod);
  bool titleSelectPress(Title   *title, const Point &w, SelMod selMod);

  bool annotationsSelectPress(const Point &w, SelMod selMod);

  Obj *objectsSelectPress(const Point &w, SelMod selMod);

  //-

  //! drag object
  enum class DragObjType {
    NONE,        //!< none
    PLOT,        //!< plot (move)
    PLOT_HANDLE, //!< plot handles (resize)
    OBJECT,      //!< plot object
    XAXIS,       //!< xaxis
    YAXIS,       //!< yaxis
    KEY,         //!< key
    TITLE,       //!< title
    ANNOTATION   //!< annotation
  };

  //! mouse state data
  // handle mouse drag press/move/release
  bool editMousePress  (const Point &p, bool inside=false);
  bool editMouseMove   (const Point &p, bool first=false);
  bool editMouseMotion (const Point &p);
  bool editMouseRelease(const Point &p);

  virtual bool editPress  (const Point &p, const Point &w, bool inside=false);
  virtual bool editMove   (const Point &p, const Point &w, bool first=false);
  virtual bool editMotion (const Point &p, const Point &w);
  virtual bool editRelease(const Point &p, const Point &w);

  virtual void editMoveBy(const Point &d);

  void setDragObj(DragObjType objType, CQChartsObj *obj);

  void flipSelected(Qt::Orientation orient);

  //-

  bool keyEditPress  (PlotKey *key  , const Point &w);
  bool axisEditPress (Axis    *axis , const Point &w);
  bool titleEditPress(Title   *title, const Point &w);

  bool annotationsEditPress(const Point &w);

  bool objectsEditPress(const Point &w, bool inside);

  //-

  bool keyEditSelect  (PlotKey *key  , const Point &w);
  bool axisEditSelect (Axis    *axis , const Point &w);
  bool titleEditSelect(Title   *title, const Point &w);

  bool annotationsEditSelect(const Point &w);

  bool objectsEditSelect(const Point &w, bool inside);

  //-

  void selectOneObj(Obj *obj, bool allObjs);

  void deselectAllObjs();

  void deselectAll();

  // handle key press
  virtual void keyPress(int key, int modifier);

  // get tip text at point
  virtual bool tipText(const Point &p, QString &tip) const;

  void addNoTipColumns(CQChartsTableTip &tableTip) const;
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

  bool isZoomFull() const;

  virtual double getZoomFactor(bool is_shift) const;

 public slots:
  void updateSlot();
  void updateObjsSlot();

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
  using Annotation          = CQChartsAnnotation;
  using Annotations         = std::vector<Annotation *>;
  using ArrowAnnotation     = CQChartsArrowAnnotation;
  using AxisAnnotation      = CQChartsAxisAnnotation;
  using ButtonAnnotation    = CQChartsButtonAnnotation;
  using EllipseAnnotation   = CQChartsEllipseAnnotation;
  using ImageAnnotation     = CQChartsImageAnnotation;
  using KeyAnnotation       = CQChartsKeyAnnotation;
  using PieSliceAnnotation  = CQChartsPieSliceAnnotation;
  using PointAnnotation     = CQChartsPointAnnotation;
  using PointSetAnnotation  = CQChartsPointSetAnnotation;
  using PolygonAnnotation   = CQChartsPolygonAnnotation;
  using PolylineAnnotation  = CQChartsPolylineAnnotation;
  using RectangleAnnotation = CQChartsRectangleAnnotation;
  using TextAnnotation      = CQChartsTextAnnotation;
  using ValueSetAnnotation  = CQChartsValueSetAnnotation;
  using WidgetAnnotation    = CQChartsWidgetAnnotation;

  const Annotations &annotations() const { return annotations_; }

  ArrowAnnotation     *addArrowAnnotation    (const Position &start, const Position &end);
  AxisAnnotation      *addAxisAnnotation     (Qt::Orientation direction, double start, double end);
  ButtonAnnotation    *addButtonAnnotation   (const Position &pos, const QString &text);
  EllipseAnnotation   *addEllipseAnnotation  (const Position &center, const Length &xRadius,
                                              const Length &yRadius);
  ImageAnnotation     *addImageAnnotation    (const Position &pos, const Image &image);
  ImageAnnotation     *addImageAnnotation    (const Rect &rect, const Image &image);
  KeyAnnotation       *addKeyAnnotation      ();
  PieSliceAnnotation  *addPieSliceAnnotation (const Position &pos, const Length &innerRadius,
                                              const Length &outerRadius, const Angle &startAngle,
                                              const Angle &spanAngle);
  PointAnnotation     *addPointAnnotation    (const Position &pos, const Symbol &type);
  PointSetAnnotation  *addPointSetAnnotation (const CQChartsPoints &values);
  PolygonAnnotation   *addPolygonAnnotation  (const CQChartsPolygon &polygon);
  PolylineAnnotation  *addPolylineAnnotation (const CQChartsPolygon &polygon);
  RectangleAnnotation *addRectangleAnnotation(const Rect &rect);
  TextAnnotation      *addTextAnnotation     (const Position &pos, const QString &text);
  TextAnnotation      *addTextAnnotation     (const Rect &rect, const QString &text);
  ValueSetAnnotation  *addValueSetAnnotation (const Rect &rectangle, const CQChartsReals &values);
  WidgetAnnotation    *addWidgetAnnotation   (const Position &pos, const Widget &widget);
  WidgetAnnotation    *addWidgetAnnotation   (const Rect &rect, const Widget &widget);

  void addAnnotation(Annotation *annotation);

  template<typename TYPE>
  TYPE *addAnnotationT(TYPE *annotation) {
    addAnnotation(annotation);
    return annotation;
  }

  Annotation *getAnnotationById(const QString &id) const;
  Annotation *getAnnotationByPathId(const QString &pathId) const;
  Annotation *getAnnotationByInd(int ind) const;

  void removeAnnotation(Annotation *annotation);

  void removeAllAnnotations();

  //---

  PlotObj *getPlotObject(const QString &objectId) const;

  Obj *getObject(const QString &objectId) const;

  QList<QModelIndex> getObjectInds(const QString &objectId) const;

  std::vector<Obj *> getObjectConnected(const QString &objectId) const;

  //---

  const Layers &layers() const { return layers_; }

  Layer *initLayer(const Layer::Type &type, const Buffer::Type &buffer, bool active);

  void setLayerActive(const Layer::Type &type, bool b);

  bool isLayerActive(const Layer::Type &type) const;

  //---

  bool isInvalidateLayers() const { return updatesData_.invalidateLayers; }

  virtual void invalidateLayers();

 protected:
  void execInvalidateLayers();

 public:
  virtual void invalidateLayer(const Buffer::Type &layerType);

 protected:
  void execInvalidateLayer(const Buffer::Type &layerType);

 public:
  virtual void invalidateOverlay();

 protected:
  void execInvalidateOverlay();

  //---

 public:
  Buffer *getBuffer(const Buffer::Type &type) const;

  Layer *getLayer(const Layer::Type &type) const;

 private:
  BBox adjustDataRangeBBox(const BBox &bbox) const;

  void setLayerActive1(const Layer::Type &type, bool b);

  void invalidateLayer1(const Buffer::Type &layerType);

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
  virtual void drawDeviceParts(PaintDevice *) const { }

  // draw background layer plot parts
  virtual void drawBackgroundParts(QPainter *painter) const;

  // draw background layer plot device parts
  virtual void drawBackgroundDeviceParts(PaintDevice *device, bool bgLayer, bool bgAxes,
                                         bool bgKey) const;

  // draw middle layer plot parts
  virtual void drawMiddleParts(QPainter *painter) const;

  // draw middle layer plot device parts
  virtual void drawMiddleDeviceParts(PaintDevice *device, bool bg, bool mid, bool fg,
                                     bool annotations) const;

  // draw foreground layer plot parts
  virtual void drawForegroundParts(QPainter *painter) const;

  // draw foreground layer plot device parts
  virtual void drawForegroundDeviceParts(PaintDevice *device, bool fgAxes, bool fgKey,
                                         bool title, bool foreground, bool tabbed) const;

  virtual void drawTabs(PaintDevice *device) const;

  void drawTabs(PaintDevice *device, const Plots &plots) const;
  void drawTabs(PaintDevice *device, const Plots &plots, Plot *currentPlot) const;

  // draw overlay layer plot parts
  virtual void drawOverlayParts(QPainter *painter) const;

  // draw overlay layer plot device parts
  virtual void drawOverlayDeviceParts(PaintDevice *device, bool sel_objs,
                                      bool sel_annotations, bool boxes, bool edit_handles,
                                      bool over_objs, bool over_annotations) const;

  //---

  // draw background (layer and detail)
  virtual bool hasBackgroundLayer() const;

  virtual void drawBackgroundLayer(PaintDevice *device) const;

  virtual void drawBackgroundRects(PaintDevice *device) const;

  virtual bool hasBackground() const;

  virtual void execDrawBackground(PaintDevice *device) const;

  virtual void drawBackgroundSides(PaintDevice *device, const BBox &bbox,
                                   const Sides &sides) const;

  // draw axes on background
  virtual bool hasGroupedBgAxes() const;

  virtual bool hasBgAxes() const;

  virtual void drawGroupedBgAxes(PaintDevice *device) const;

  virtual void drawBgAxes(PaintDevice *device) const;

  // draw key on background
  virtual bool hasGroupedBgKey() const;

  virtual void drawBgKey(PaintDevice *device) const;

  //---

  // draw objects
  virtual bool hasGroupedObjs(const Layer::Type &layerType) const;

  virtual void drawGroupedObjs(PaintDevice *device, const Layer::Type &layerType) const;

  virtual bool hasObjs(const Layer::Type &layerType) const;

  virtual void preDrawObjs(PaintDevice *) const { }

  virtual void execDrawObjs(PaintDevice *device, const Layer::Type &type) const;

  virtual void postDrawObjs(PaintDevice *) const { }

  virtual bool objInsideBox(PlotObj *plotObj, const BBox &bbox) const;

  //---

  // draw axes on foreground
  virtual bool hasGroupedFgAxes() const;

  virtual bool hasFgAxes() const;

  virtual void drawGroupedFgAxes(PaintDevice *device) const;

  virtual void drawFgAxes(PaintDevice *device) const;

  // draw key on foreground
  virtual bool hasGroupedFgKey() const;

  virtual void drawFgKey(PaintDevice *painter) const;

  // draw title
  virtual bool hasTitle() const;

  virtual void drawTitle(PaintDevice *device) const;

  // draw annotations
  virtual bool hasGroupedAnnotations(const Layer::Type &layerType) const;

  virtual void drawGroupedAnnotations(PaintDevice *device, const Layer::Type &layerType) const;

  virtual bool hasAnnotations(const Layer::Type &layerType) const;

  virtual void drawAnnotations(PaintDevice *device, const Layer::Type &layerType) const;

  // draw foreground
  virtual bool hasForeground() const;

  virtual void execDrawForeground(PaintDevice *device) const;

  // draw debug boxes
  virtual bool hasGroupedBoxes() const;

  virtual void drawGroupedBoxes(PaintDevice *device) const;

  virtual bool hasBoxes() const;

  virtual void drawBoxes(PaintDevice *device) const;

  // draw edit handles
  virtual bool hasGroupedEditHandles() const;

  virtual void drawGroupedEditHandles(QPainter *painter) const;

  virtual bool hasEditHandles() const;

  virtual void drawEditHandles(QPainter *painter) const;

  //---

  // set clip rect
  void setClipRect(PaintDevice *device) const;

  //---

  virtual bool selectInvalidateObjs() const { return false; }

  //---

  QPainter *beginPaint(Buffer *layer, QPainter *painter, const QRectF &rect=QRectF()) const;
  void      endPaint  (Buffer *layer) const;

  //---

  PlotKey *getFirstPlotKey() const;

  //---

  void drawSymbol(PaintDevice *device, const Point &p, const Symbol &symbol,
                  const Length &size, const PenBrush &penBrush) const;
  void drawSymbol(PaintDevice *device, const Point &p,
                  const Symbol &symbol, const Length &size) const;

  void drawBufferedSymbol(QPainter *painter, const Point &p,
                          const Symbol &symbol, double size) const;

  //---

  CQChartsTextOptions adjustTextOptions(
    const CQChartsTextOptions &options=CQChartsTextOptions()) const;

  //---

  // debug draw (default to red boxes)
  void drawWindowColorBox(PaintDevice *device, const BBox &bbox,
                          const QColor &c=Qt::red) const;

  void drawColorBox(PaintDevice *device, const BBox &bbox,
                    const QColor &c=Qt::red) const;

  //---

 public:
  // set pen/brush
  void setPenBrush(PenBrush &penBrush, const CQChartsPenData &penData,
                   const CQChartsBrushData &brushData) const;

  void setPen(PenBrush &penBrush, const CQChartsPenData &penData) const;

  void setBrush(PenBrush &penBrush, const CQChartsBrushData &brushData) const;

  void setPenBrush(PaintDevice *device, const CQChartsPenData &penData,
                   const CQChartsBrushData &brushData) const;

  void setPen(PaintDevice *device, const CQChartsPenData &penData) const;

  void setBrush(PaintDevice *device, const CQChartsBrushData &brushData) const;

  //---

  void updateObjPenBrushState(const Obj *obj, PenBrush &penBrush,
                              DrawType drawType=DrawType::BOX) const;

  void updateObjPenBrushState(const Obj *obj, const ColorInd &ic,
                              PenBrush &penBrush, DrawType drawType) const;

 private:
  void updateInsideObjPenBrushState  (const ColorInd &ic, PenBrush &penBrush,
                                      bool outline, DrawType drawType) const;
  void updateSelectedObjPenBrushState(const ColorInd &ic, PenBrush &penBrush,
                                      DrawType drawType) const;

 public:
  QColor insideColor  (const QColor &c) const;
  QColor selectedColor(const QColor &c) const;

  //---

 public:
  // get palette color for index
  QColor interpPaletteColor(const ColorInd &ind, bool scale=false) const;

 public:
  virtual QColor interpGroupPaletteColor(const ColorInd &ig, const ColorInd &iv,
                                         bool scale=false) const;

  QColor blendGroupPaletteColor(double r1, double r2, double dr) const;

 public:
  QColor interpThemeColor(const ColorInd &ind) const;

  QColor interpInterfaceColor(double r) const;

 public:
  virtual QColor interpColor(const Color &c, const ColorInd &ind) const;

  //---

 public:
  QColor calcTextColor(const QColor &bg) const;

  //---

  ColorInd calcColorInd(const PlotObj *obj, const CQChartsKeyColorBox *keyBox,
                        const ColorInd &is, const ColorInd &ig, const ColorInd &iv) const;

  //---

  bool checkColumns(const Columns &columns, const QString &name, bool required=false) const;

  bool checkColumn(const Column &column, const QString &name,
                   bool required=false) const;

  bool checkColumn(const Column &column, const QString &name,
                   ColumnType &type, bool required=false) const;

  ColumnType columnValueType(const Column &column,
                             const ColumnType &defType=ColumnType::STRING) const;

  bool columnValueType(const Column &column, CQChartsModelTypeData &columnTypeData,
                       const ColumnType &defType=ColumnType::STRING) const;

#if 0
  bool columnTypeStr(const Column &column, QString &typeStr) const;

  bool setColumnTypeStr(const Column &column, const QString &typeStr);
#endif

  bool columnDetails(const Column &column, QString &typeName,
                     QVariant &minValue, QVariant &maxValue) const;

  CQChartsModelColumnDetails *columnDetails(const Column &column) const;

  //---

  CQChartsModelData *getModelData() const;

  //---

  bool getHierColumnNames(const QModelIndex &parent, int row, const Columns &nameColumns,
                          const QString &separator, QStringList &nameStrs,
                          QModelIndices &nameInds) const;

  //---

  // cached column names
  QString columnsHeaderName(const Columns &columns, bool tip=false) const;
  QString columnHeaderName(const Column &column, bool tip=false) const;

  QString idHeaderName   (bool tip=false) const { return columnHeaderName(idColumn   (), tip); }
  QString colorHeaderName(bool tip=false) const { return columnHeaderName(colorColumn(), tip); }
  QString fontHeaderName (bool tip=false) const { return columnHeaderName(fontColumn (), tip); }
  QString imageHeaderName(bool tip=false) const { return columnHeaderName(imageColumn(), tip); }

  virtual void updateColumnNames();

  void setColumnHeaderName(const Column &column, const QString &def);

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
  void drawLayer(QPainter *painter, Layer::Type type) const;

  // draw all layers
  void drawLayers(QPainter *painter) const;

  const Layer::Type &drawLayerType() const;

  //---

  // print layer data
  bool printLayer(Layer::Type type, const QString &filename) const;

  //---

  // get/set parameter
  bool getParameter(PlotParameter *param, QVariant &value) const;
  bool setParameter(PlotParameter *param, const QVariant &value);

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

  // pressed
  void selectPressSignal(const CQChartsGeom::Point &p);

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
    NoUpdate(const Plot *plot, bool update=false) :
     plot_(const_cast<Plot *>(plot)), update_(update) {
      plot_->setUpdatesEnabled(false);
    }

    NoUpdate(Plot *plot, bool update=false) :
     plot_(plot), update_(update) {
      plot_->setUpdatesEnabled(false);
    }

   ~NoUpdate() { plot_->setUpdatesEnabled(true, update_); }

    Plot* plot_   { nullptr };
    bool  update_ { false };
  };

  using ObjSet     = std::set<Obj *>;
  using SizeObjSet = std::map<double,ObjSet>;

 protected:
  void connectModel();
  void disconnectModel();

  void connectDisconnectModel(bool connectDisconnect);

  //---

  enum class Constraints {
    NONE       = 0,
    SELECTABLE = (1<<0),
    EDITABLE   = (1<<1)
  };

  void objsAtPoint(const Point &p, Objs &objs, const Constraints &constraints) const;

  void plotObjsAtPoint1(const Point &p, PlotObjs &objs) const;

  virtual void plotObjsAtPoint(const Point &p, PlotObjs &objs) const;

  void annotationsAtPoint(const Point &p, Annotations &annotations) const;

  void objsIntersectRect(const BBox &r, Objs &objs, bool inside, bool select=false) const;

  virtual bool objNearestPoint(const Point &p, PlotObj* &obj) const;

  //---

  void getSelectIndices(QItemSelectionModel *sm, QModelIndexSet &indices);

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

    void start(const Plot *plot, const char *id) {
      if (id) {
        std::cerr << "Start: " << plot->id().toStdString() << " : " << id << "\n";

        startTime = CHRTime::getTime();
      }

      busy.store(true);
    }

    void end(const Plot *plot, const char *id) {
      busy.store(false);

      if (id) {
        CHRTime dt = startTime.diffTime();

        std::cerr << "Elapsed: " << plot->id().toStdString() << " : " << id << " " <<
                     dt.getMSecs() << "\n";
      }
    }

    void finish(const Plot *plot, const char *id) {
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
    QColor      bgColor  { 255, 255, 255 };
    QColor      fgColor  { 100, 200, 100 };
    Font        font;
    int         count    { 10 };
    int         multiple { 10 };
    mutable int ind      { 0 };
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
    LockMutex(Plot *plot, const char *id) : plot(plot) { plot->updateLock(id); }
   ~LockMutex() { plot->updateUnLock(); }

    Plot* plot { nullptr };
  };

  struct TryLockMutex {
    TryLockMutex(Plot *plot, const char *id) : plot(plot) {
      locked = plot->updateTryLock(id); }
   ~TryLockMutex() { if (locked) plot->updateUnLock(); }

    Plot* plot   { nullptr };
    bool  locked { false };
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
    Column     column;
    ColorType  colorType { ColorType::AUTO };
    bool       valid     { false };
    bool       mapped    { true };
    double     map_min   { 0.0 };
    double     map_max   { 1.0 };
    double     data_min  { 0.0 };
    double     data_max  { 1.0 };
    ColumnType modelType;
    QString    palette;
    ColorStops xStops;
    ColorStops yStops;
  };

  //! every row selection data
  struct EveryData {
    bool enabled { false };
    int  start   { 0 };
    int  end     { std::numeric_limits<int>::max() };
    int  step    { 1 };
  };

  struct MouseData {
    Point              pressPoint  { 0, 0 };
    Point              movePoint   { 0, 0 };
    bool               pressed     { false };
    DragObjType        dragObjType { DragObjType::NONE };
    CQChartsObj*       dragObj     { nullptr };
    CQChartsResizeSide dragSide    { CQChartsResizeSide::NONE };
    bool               dragged     { false };
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

  using ColumnNames = std::map<Column,QString>;

  //---

 protected:
  View*                view_          { nullptr }; //!< parent view
  PlotType*            type_          { nullptr }; //!< plot type data
  ModelP               model_;                     //!< abstract model
  bool                 modelNameSet_  { false };   //!< model name set from plot
  CQPropertyViewModel* propertyModel_ { nullptr }; //!< property model

  // name
  QString name_; //!< custom name

  // ranges
  BBox          viewBBox_        { 0, 0, 1, 1 };     //!< view box
  BBox          innerViewBBox_   { 0, 0, 1, 1 };     //!< inner view box
  PlotMargin    innerMargin_     { 0, 0, 0, 0 };     //!< inner margin
  PlotMargin    outerMargin_     { 10, 10, 10, 10 }; //!< outer margin
  DisplayRange* displayRange_    { nullptr };        //!< value range mapping
  Range         calcDataRange_;                      //!< calc data range
  Range         dataRange_;                          //!< data range
  Range         outerDataRange_;                     //!< outer data range
  ZoomData      zoomData_;                           //!< zoom data

  // override range
  OptReal xmin_; //!< xmin override
  OptReal ymin_; //!< ymin override
  OptReal xmax_; //!< xmax override
  OptReal ymax_; //!< ymax override

  // filter
  EveryData everyData_;        //!< every data
  QString   filterStr_;        //!< data filter
  QString   visibleFilterStr_; //!< visible filter

  bool skipBad_ { true }; //!< skip bad values

  // borders
  Sides plotBorderSides_  { "tlbr" }; //!< plot border sides
  bool  plotClip_         { true };   //!< is clipped at plot limits
  Sides dataBorderSides_  { "tlbr" }; //!< data border sides
  bool  dataClip_         { false };  //!< is clipped at data limits
  Sides fitBorderSides_   { "tlbr" }; //!< fit border sides

  // title
  Title*  titleObj_  { nullptr }; //!< title object
  QString titleStr_;              //!< title string
  QString fileName_;              //!< associated data filename

  // axes
  Axis* xAxis_ { nullptr }; //!< x axis object
  Axis* yAxis_ { nullptr }; //!< y axis object

  // key
  PlotKey* keyObj_   { nullptr }; //!< key object
  bool     colorKey_ { false };   //!< use color column for key

  // columns
  Column  xValueColumn_;   //!< x axis value column
  Column  yValueColumn_;   //!< y axis value column
  Column  idColumn_;       //!< unique data id column (signalled)
  Columns tipColumns_;     //!< tip columns
  Columns noTipColumns_;   //!< no tip columns
  Column  visibleColumn_;  //!< visible column
  Column  fontColumn_;     //!< font column
  Column  imageColumn_;    //!< image column
  Columns controlColumns_; //!< control columns

  // color data
  ColorColumnData    colorColumnData_; //!< color color data
  mutable std::mutex colorMutex_;      //!< color mutex

  // cached column names
  ColumnNames columnNames_; //!< column header names

  // font
  Font   font_;                      //!< font
  double minScaleFontSize_ { 6.0 };  //!< min scaled font size
  double maxScaleFontSize_ { 48.0 }; //!< max scaled font size

  Font tabbedFont_; //!< font for tab text

  // palette
  PaletteName defaultPalette_; //!< default palette

  // scaling
  bool equalScale_ { false }; //!< equal scaled

  // follow mouse
  bool followMouse_ { true }; //!< track object under mouse

  // fit
  bool       autoFit_      { false };      //!< auto fit on data change
  PlotMargin fitMargin_    { 1, 1, 1, 1 }; //!< fit margin
  bool       needsAutoFit_ { false };      //!< needs auto fit on next draw

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
  using PlotObjTree = CQChartsPlotObjTree;

  struct ObjTreeData {
    bool         init   { false };   //!< needs init obj tree
    PlotObjTree* tree   { nullptr }; //!< plot object quad tree
    bool         isSet  { false };   //!< is plot object quad tree set
    bool         notify { false };   //!< is plot object quad tree notify
  };

  ObjTreeData objTreeData_; //!< object tree data

  //---

  UpdateData  updateData_;  //!< update data
  MouseData   mouseData_;   //!< mouse event data
  AnimateData animateData_; //!< animation data

  // draw layers, buffers
  Buffers              buffers_;    //!< draw layer buffers
  Layers               layers_;     //!< draw layers
  mutable Buffer::Type drawBuffer_; //!< objects draw buffer

  IdHidden idHidden_;  //!< hidden object ids
  QVariant hideValue_; //!< hide value

  IndexColumnRows selIndexColumnRows_; //!< sel model indices (by col/row)

  // edit handles
  EditHandles* editHandles_ { nullptr }; //!< edit controls
  bool         editing_     { false };   //!< is editing

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
