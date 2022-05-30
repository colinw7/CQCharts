#ifndef CQChartsPlot_H
#define CQChartsPlot_H

#include <CQChartsView.h>
#include <CQChartsObj.h>
#include <CQChartsColor.h>
#include <CQChartsSymbol.h>
#include <CQChartsObjData.h>
#include <CQChartsGroupData.h>
#include <CQChartsTextOptions.h>
#include <CQChartsLayer.h>
#include <CQChartsUtil.h>
#include <CQChartsTypes.h>
#include <CQChartsGeom.h>
#include <CQChartsPlotMargin.h>
#include <CQChartsOptReal.h>
#include <CQChartsColorStops.h>
#include <CQChartsSymbolTypeMap.h>
#include <CQChartsPaletteName.h>
#include <CQChartsModelTypes.h>
#include <CQChartsModelIndex.h>
#include <CQChartsPlotModelVisitor.h>
#include <CQChartsColorColumnData.h>
#include <CQChartsAlphaColumnData.h>
#include <CQChartsSymbolTypeData.h>
#include <CQChartsSymbolSizeData.h>
#include <CQChartsFontSizeData.h>

#include <CSafeIndex.h>

#include <QAbstractItemModel>
#include <QFrame>
#include <QTimer>
#include <QPointer>

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
class CQChartsColorBoxKeyItem;
class CQChartsMapKey;
class CQChartsColorMapKey;
class CQChartsTitle;
class CQChartsPlotObj;
class CQChartsPlotObjTree;
class CQChartsPlotCustomControls;
class CQChartsSymbolSet;

class CQChartsAnnotation;
class CQChartsAnnotationGroup;
class CQChartsArcAnnotation;
class CQChartsArcConnectorAnnotation;
class CQChartsArrowAnnotation;
class CQChartsAxisAnnotation;
class CQChartsButtonAnnotation;
class CQChartsEllipseAnnotation;
class CQChartsImageAnnotation;
class CQChartsPathAnnotation;
class CQChartsKeyAnnotation;
class CQChartsPieSliceAnnotation;
class CQChartsPointAnnotation;
class CQChartsPointSetAnnotation;
class CQChartsPoint3DSetAnnotation;
class CQChartsPolygonAnnotation;
class CQChartsPolylineAnnotation;
class CQChartsRectangleAnnotation;
class CQChartsShapeAnnotation;
class CQChartsTextAnnotation;
class CQChartsValueSetAnnotation;
class CQChartsWidgetAnnotation;
class CQChartsSymbolSizeMapKeyAnnotation;

class CQChartsPlotParameter;
class CQChartsDisplayRange;
class CQChartsValueSet;
class CQChartsModelDetails;
class CQChartsModelColumnDetails;
class CQChartsModelData;
class CQChartsEditHandles;
class CQChartsTableTip;
class CQChartsPolygon;
class CQChartsRect;
class CQChartsPosition;
class CQChartsPoints;
class CQChartsReals;
class CQChartsPlotControlIFace;

class CQChartsScriptPaintDevice;
class CQChartsHtmlPaintDevice;
class CQChartsSVGPaintDevice;
class CQChartsStatsPaintDevice;

class CQPropertyViewModel;
class CQPropertyViewItem;

class CQThreadObject;

class QAbstractProxyModel;
class QItemSelectionModel;
class QTextBrowser;
class QRubberBand;
class QMenu;
class QLabel;
class QPainter;
class QCheckBox;

//----

/*!
 * \brief Update plot timer
 * \ingroup Charts
 */
class CQChartsPlotUpdateTimer : public QTimer {
 public:
  using Plot = CQChartsPlot;

 public:
  CQChartsPlotUpdateTimer(Plot *plot) :
   plot_(plot) {
    setSingleShot(true);
  }

  Plot *plot() const { return plot_; }

 private:
  Plot* plot_ { nullptr };
};

//----

CQCHARTS_NAMED_SHAPE_DATA(Plot, plot) // plot area
CQCHARTS_NAMED_SHAPE_DATA(Data, data) // data area
CQCHARTS_NAMED_SHAPE_DATA(Fit , fit ) // fit area

/*!
 * \brief Base class for Plot
 * \ingroup Charts
 *
 * All plots are derived from this base class which contains the virtual functions
 * to handle the required customization.
 *
 * A plot performs the following required steps:
 *  + calcRange  : calculate display range
 *  + createObjs : create plot objects
 *
 * Other key operations are:
 *  + addProperties : add customization properties to property view tree
 *  + addMenuItems  : add custom menu items to context menu
 *
 */
class CQChartsPlot : public CQChartsObj, public CQChartsEditableIFace,
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
  // . idColumn used as unique id for row (tcl row lookup can use)
  // . tipHeaderColumn/tipColumns/noTipColumns used for object tooltips for data in extra rows
  // . visibleColumn used for individual row hide in model traversal
  // . colorColumn and alphaColumn used for custom color for data on row
  // . fontColumn used for custom font for data on row
  // . imageColumn used for custom image for data on row
  // . controlColumns used for controls for filters on data in specified column
  Q_PROPERTY(CQChartsColumn  idColumn        READ idColumn        WRITE setIdColumn       )
  Q_PROPERTY(CQChartsColumn  tipHeaderColumn READ tipHeaderColumn WRITE setTipHeaderColumn)
  Q_PROPERTY(CQChartsColumns tipColumns      READ tipColumns      WRITE setTipColumns     )
  Q_PROPERTY(CQChartsColumns noTipColumns    READ noTipColumns    WRITE setNoTipColumns   )
  Q_PROPERTY(CQChartsColumn  visibleColumn   READ visibleColumn   WRITE setVisibleColumn  )
  Q_PROPERTY(CQChartsColumn  colorColumn     READ colorColumn     WRITE setColorColumn    )
  Q_PROPERTY(CQChartsColumn  alphaColumn     READ alphaColumn     WRITE setAlphaColumn    )
  Q_PROPERTY(CQChartsColumn  fontColumn      READ fontColumn      WRITE setFontColumn     )
  Q_PROPERTY(CQChartsColumn  imageColumn     READ imageColumn     WRITE setImageColumn    )
  Q_PROPERTY(CQChartsColumns controlColumns  READ controlColumns  WRITE setControlColumns )

  // color map (for color column)
  Q_PROPERTY(ColorType           colorType       READ colorType       WRITE setColorType      )
  Q_PROPERTY(bool                colorMapped     READ isColorMapped   WRITE setColorMapped    )
  Q_PROPERTY(double              colorMapMin     READ colorMapMin     WRITE setColorMapMin    )
  Q_PROPERTY(double              colorMapMax     READ colorMapMax     WRITE setColorMapMax    )
  Q_PROPERTY(CQChartsPaletteName colorMapPalette READ colorMapPalette WRITE setColorMapPalette)
  Q_PROPERTY(CQChartsColorStops  colorXStops     READ colorXStops     WRITE setColorXStops    )
  Q_PROPERTY(CQChartsColorStops  colorYStops     READ colorYStops     WRITE setColorYStops    )
  Q_PROPERTY(CQChartsColorMap    colorMap        READ colorMap        WRITE setColorMap       )

  // alpha map (for alpha column)
  Q_PROPERTY(bool   alphaMapped READ isAlphaMapped WRITE setAlphaMapped)
  Q_PROPERTY(double alphaMapMin READ alphaMapMin   WRITE setAlphaMapMin)
  Q_PROPERTY(double alphaMapMax READ alphaMapMax   WRITE setAlphaMapMax)

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

  Q_PROPERTY(bool   skipBad   READ isSkipBad   WRITE setSkipBad)
  Q_PROPERTY(bool   badUseRow READ isBadUseRow WRITE setBadUseRow)
  Q_PROPERTY(double badValue  READ badValue    WRITE setBadValue)

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
  Q_PROPERTY(QString filename READ filename WRITE setFileName)
  Q_PROPERTY(QString xLabel   READ xLabel   WRITE setXLabel  )
  Q_PROPERTY(QString yLabel   READ yLabel   WRITE setYLabel  )

  // plot area
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Plot, plot)

  Q_PROPERTY(CQChartsSides plotBorderSides READ plotBorderSides WRITE setPlotBorderSides)
  Q_PROPERTY(bool          plotClip        READ isPlotClip      WRITE setPlotClip       )

  // data area
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Data, data)

  Q_PROPERTY(CQChartsSides dataBorderSides READ dataBorderSides WRITE setDataBorderSides)
  Q_PROPERTY(bool          dataRawRange    READ isDataRawRange  WRITE setDataRawRange   )
  Q_PROPERTY(bool          dataClip        READ isDataClip      WRITE setDataClip       )
  Q_PROPERTY(bool          dataRawClip     READ isDataRawClip   WRITE setDataRawClip    )

  // fit area
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Fit, fit)

  Q_PROPERTY(CQChartsSides fitBorderSides READ fitBorderSides WRITE setFitBorderSides)
  Q_PROPERTY(bool          fitClip        READ isFitClip      WRITE setFitClip       )

  // key
  Q_PROPERTY(bool keyVisible  READ isKeyVisible  WRITE setKeyVisible )
  Q_PROPERTY(bool controlsKey READ isControlsKey WRITE setControlsKey)
  Q_PROPERTY(bool colorKey    READ isColorKey    WRITE setColorKey   )

  // color map key
  Q_PROPERTY(bool colorMapKey READ isColorMapKey WRITE setColorMapKey)

  Q_PROPERTY(int maxMappedValues READ maxMappedValues WRITE setMaxMappedValues)

  // font
  Q_PROPERTY(CQChartsFont font       READ font       WRITE setFont)
  Q_PROPERTY(CQChartsFont tabbedFont READ tabbedFont WRITE setTabbedFont)
  Q_PROPERTY(bool         zoomText   READ isZoomText WRITE setZoomText)

  // default palette
  Q_PROPERTY(CQChartsPaletteName defaultPalette READ defaultPalette WRITE setDefaultPalette)

  // default symbol set name
  Q_PROPERTY(QString defaultSymbolSetName READ defaultSymbolSetName WRITE setDefaultSymbolSetName)

  // scale symbol size
  Q_PROPERTY(bool scaleSymbolSize READ isScaleSymbolSize WRITE setScaleSymbolSize)

  // scaled fonts
  Q_PROPERTY(double minScaleFontSize READ minScaleFontSize WRITE setMinScaleFontSize)
  Q_PROPERTY(double maxScaleFontSize READ maxScaleFontSize WRITE setMaxScaleFontSize)

  // connect
  Q_PROPERTY(bool overlay READ isOverlay WRITE setOverlay)
  Q_PROPERTY(bool x1x2    READ isX1X2    WRITE setX1X2   )
  Q_PROPERTY(bool y1y2    READ isY1Y2    WRITE setY1Y2   )
  Q_PROPERTY(bool tabbed  READ isTabbed  WRITE setTabbed )

  // show all overlay x/y axes
  Q_PROPERTY(bool showAllXOverlayAxes READ isShowAllXOverlayAxes WRITE setShowAllXOverlayAxes)
  Q_PROPERTY(bool showAllYOverlayAxes READ isShowAllYOverlayAxes WRITE setShowAllYOverlayAxes)

  // misc
  Q_PROPERTY(bool followMouse READ isFollowMouse WRITE setFollowMouse)
  Q_PROPERTY(bool invertX     READ isInvertX     WRITE setInvertX    )
  Q_PROPERTY(bool invertY     READ isInvertY     WRITE setInvertY    )
//Q_PROPERTY(bool logX        READ isLogX        WRITE setLogX       )
//Q_PROPERTY(bool logY        READ isLogY        WRITE setLogY       )

  // fit
  Q_PROPERTY(bool autoFit READ isAutoFit WRITE setAutoFit)

  // fit margin
  Q_PROPERTY(CQChartsLength fitMarginLeft   READ fitMarginLeft   WRITE setFitMarginLeft  )
  Q_PROPERTY(CQChartsLength fitMarginTop    READ fitMarginTop    WRITE setFitMarginTop   )
  Q_PROPERTY(CQChartsLength fitMarginRight  READ fitMarginRight  WRITE setFitMarginRight )
  Q_PROPERTY(CQChartsLength fitMarginBottom READ fitMarginBottom WRITE setFitMarginBottom)

  // preview
  Q_PROPERTY(bool preview        READ isPreview      WRITE setPreview       )
  Q_PROPERTY(int  previewMaxRows READ previewMaxRows WRITE setPreviewMaxRows)

  Q_PROPERTY(bool queueUpdate       READ isQueueUpdate     WRITE setQueueUpdate      )
  Q_PROPERTY(bool showBoxes         READ showBoxes         WRITE setShowBoxes        )
  Q_PROPERTY(bool showSelectedBoxes READ showSelectedBoxes WRITE setShowSelectedBoxes)

  Q_ENUMS(ColorType)

 public:
  enum class ColorType {
    AUTO    = int(CQChartsColorType::AUTO),
    SET     = int(CQChartsColorType::SET),
    GROUP   = int(CQChartsColorType::GROUP),
    INDEX   = int(CQChartsColorType::INDEX),
    X_VALUE = int(CQChartsColorType::X_VALUE),
    Y_VALUE = int(CQChartsColorType::Y_VALUE)
  };

  enum class DrawRegion {
    PLOT,
    FIT,
    DATA,
    CLIP
  };

  using Plot = CQChartsPlot;

  using Point    = CQChartsGeom::Point;
  using BBox     = CQChartsGeom::BBox;
  using Polygon  = CQChartsGeom::Polygon;
  using Range    = CQChartsGeom::Range;
  using Size     = CQChartsGeom::Size;
  using RMinMax  = CQChartsGeom::RMinMax;
  using IMinMax  = CQChartsGeom::IMinMax;
  using Polygons = CQChartsGeom::Polygons;

  using SelMod = CQChartsSelMod;

  //! \brief associated plot for overlay/y1y2
  struct ConnectData {
    Plot* parent  { nullptr }; //!< parent plot (for composite)
    Plot* root    { nullptr }; //!< root plot (for expand/collapse)
    bool  x1x2    { false };   //!< is double x axis plot
    bool  y1y2    { false };   //!< is double y axis plot
    bool  overlay { false };   //!< is overlay plot
    bool  tabbed  { false };   //!< is tabbed plot
    bool  current { false };   //!< is current
    BBox  tabRect;             //!< tab rect
    Plot* next    { nullptr }; //!< next plot
    Plot* prev    { nullptr }; //!< previous plot

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

  //---

  //! \brief probe value data
  struct ProbeValue {
    double  value;
    QString label;
    QString valueStr;

    ProbeValue() = default;

    explicit ProbeValue(double value, const QString &label, const QString &valueStr) :
     value(value), label(label), valueStr(valueStr) {
    }
  };

  //! \brief probe data
  struct ProbeData {
    using Values = std::vector<ProbeValue>;

    Point           p;
    Values          xvals;
    Values          yvals;
    Qt::Orientation direction { Qt::Vertical };
    bool            both      { false };
  };

  //---

  enum class Constraints {
    NONE       = 0,
    SELECTABLE = (1<<0),
    EDITABLE   = (1<<1)
  };

  //---

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

  using Plots    = std::vector<Plot *>;
  using PlotType = CQChartsPlotType;

  using Buffer  = CQChartsBuffer;
  using Buffers = std::map<Buffer::Type, Buffer *>;

  using Layer  = CQChartsLayer;
  using Layers = std::map<Layer::Type, Layer *>;

  using ColorInd = CQChartsUtil::ColorInd;

  using DrawType = CQChartsObjDrawType;

  using View          = CQChartsView;
  using Axis          = CQChartsAxis;
  using Title         = CQChartsTitle;
  using PlotKey       = CQChartsPlotKey;
  using PlotKeyItem   = CQChartsKeyItem;
  using EditHandles   = CQChartsEditHandles;
  using PlotParameter = CQChartsPlotParameter;
  using PropertyModel = CQPropertyViewModel;

  using Font        = CQChartsFont;
  using Color       = CQChartsColor;
  using Alpha       = CQChartsAlpha;
  using Length      = CQChartsLength;
  using Units       = CQChartsUnits::Type;
  using Angle       = CQChartsAngle;
  using FillPattern = CQChartsFillPattern;
  using LineDash    = CQChartsLineDash;
  using Position    = CQChartsPosition;
  using Rect        = CQChartsRect;
  using Image       = CQChartsImage;
  using Path        = CQChartsPath;
  using Widget      = CQChartsWidget;

  using PaintDevice       = CQChartsPaintDevice;
  using ScriptPaintDevice = CQChartsScriptPaintDevice;
  using SVGPaintDevice    = CQChartsSVGPaintDevice;
  using HtmlPaintDevice   = CQChartsHtmlPaintDevice;
  using StatsPaintDevice  = CQChartsStatsPaintDevice;
  using BrushData         = CQChartsBrushData;
  using PenData           = CQChartsPenData;

  using DisplayRange = CQChartsDisplayRange;
  using ValueSet     = CQChartsValueSet;
  using OptReal      = CQChartsOptReal;
  using OptBool      = boost::optional<bool>;
  using PenBrush     = CQChartsPenBrush;
  using Sides        = CQChartsSides;
  using PlotMargin   = CQChartsPlotMargin;
  using Symbol       = CQChartsSymbol;
  using ColorStops   = CQChartsColorStops;
  using PaletteName  = CQChartsPaletteName;

  using ModelData          = CQChartsModelData;
  using ModelDetails       = CQChartsModelDetails;
  using ModelColumnDetails = CQChartsModelColumnDetails;
  using ModelTypeData      = CQChartsModelTypeData;

  using ColorColumnData = CQChartsColorColumnData;
  using AlphaColumnData = CQChartsAlphaColumnData;
  using SymbolTypeData  = CQChartsSymbolTypeData;
  using SymbolSizeData  = CQChartsSymbolSizeData;
  using FontSizeData    = CQChartsFontSizeData;

 public:
  CQChartsPlot(View *view, PlotType *type, const ModelP &model);

  virtual ~CQChartsPlot();

  //---

  virtual void init();
  virtual void term();

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

  virtual bool isUpdatesEnabled() const;
  virtual void setUpdatesEnabled(bool b, bool update=true);

  //---

  virtual bool isUpdateRangeAndObjs() const;
  virtual bool isUpdateObjs() const;

  virtual void updateRange();
  virtual void updateRangeAndObjs();
  virtual void updateObjs();

  virtual void updateAndAdjustRanges();

  //---

  void applyVisibleFilter();

  //---

  void drawBackground();
  void drawForeground();

  void drawObjs();

  virtual void init3D() { }
  virtual void draw3D() { }

  //---

  void writeScript(ScriptPaintDevice *device) const;

  virtual void writeScriptParts(PaintDevice *) const { }

  void writeScriptRange(ScriptPaintDevice *device) const;

  void writeSVG(SVGPaintDevice *device) const;

  void writeHtml(HtmlPaintDevice *device) const;

  void writeStats(StatsPaintDevice *device) const;

  //---

  const DisplayRange &displayRange() const;
  void setDisplayRange(const DisplayRange &r);

  const DisplayRange &rawDisplayRange() const;

  virtual const Range &dataRange() const;
  virtual void setDataRange(const Range &r, bool update=true);

  virtual void resetDataRange(bool updateRange=true, bool updateObjs=true);

  void resetRange();

  virtual Range objTreeRange() const;

  //---

 public:
  //! \brief zoom data
  struct ZoomData {
    Point dataScale  { 1.0, 1.0 }; //!< data scale (zoom in x/y direction)
    Point dataOffset { 0.0, 0.0 }; //!< data offset (pan)
  };

  virtual double dataScaleX() const;
  virtual void setDataScaleX(double r);

  virtual double dataScaleY() const;
  virtual void setDataScaleY(double r);

  virtual double dataOffsetX() const;
  virtual void setDataOffsetX(double x);

  virtual double dataOffsetY() const;
  virtual void setDataOffsetY(double y);

  //---

  virtual const ZoomData &zoomData() const;
  virtual void setZoomData(const ZoomData &zoomData);

  //---

  void updateDataScale(double r);
  void updateDataScaleX(double r);
  void updateDataScaleY(double r);

  double dataScale() const;

  //---

 public:
  virtual const OptReal &xmin() const;
  virtual void setXMin(const OptReal &r);

  virtual const OptReal &ymin() const;
  virtual void setYMin(const OptReal &r);

  virtual const OptReal &xmax() const;
  virtual void setXMax(const OptReal &r);

  virtual const OptReal &ymax() const;
  virtual void setYMax(const OptReal &r);

  //---

  // filter every n rows from start to end
 public:
  int isEveryEnabled() const { return everyData().enabled; }
  void setEveryEnabled(bool b);

  int everyStart() const { return everyData().start; }
  void setEveryStart(int i);

  int everyEnd() const { return everyData().end; }
  void setEveryEnd(int i);

  int everyStep() const { return everyData().step; }
  void setEveryStep(int i);

  //---

  // filter by expression string
 public:
  virtual const QString &filterStr() const;
  virtual void setFilterStr(const QString &s);

  virtual const QString &visibleFilterStr() const;
  virtual void setVisibleFilterStr(const QString &s);

  //---

  // override for custom plot filter of row values
  virtual bool modelPreVisit(const QAbstractItemModel * /*model*/,
                             const CQChartsModelVisitor::VisitData & /*data*/) const {
    return true;
  }

  //---

  // bad data handling
 public:
  //! get/set skip bad
  bool isSkipBad() const { assert(! isComposite()); return badData_.skip; }
  void setSkipBad(bool b);

  //! get/set bad value uses row number
  bool isBadUseRow() const { return badData_.useRow; }
  void setBadUseRow(bool b);

  //! get/set bad value (when not using row number)
  double badValue() const { return badData_.value; }
  void setBadValue(double v);

  virtual double getRowBadValue(int row) const;

  //---

 public:
  const QString &titleStr() const;
  void setTitleStr(const QString &s);

  const QString &filename() const { return filename_; }
  void setFileName(const QString &s) { filename_ = s; }

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

  bool isDataRawRange() const { return dataRawRange_; }
  void setDataRawRange(bool b);

  bool isDataClip() const { return dataClip_; }
  void setDataClip(bool b);

  bool isDataRawClip() const { return dataRawClip_; }
  void setDataRawClip(bool b);

  //---

  // fit area
  const Sides &fitBorderSides() const { return fitBorderSides_; }
  void setFitBorderSides(const Sides &s);

  bool isFitClip() const { return fitClip_; }
  void setFitClip(bool b);

  //---

  //! get/set font
  const Font &font() const { return font_; }
  virtual void setFont(const Font &f);

  //! get Qt font
  QFont qfont() const;
  QFont qfont(const Font &font) const;

  //! get/set tabbed bar font
  void setTabbedFont(const Font &f);
  const Font &tabbedFont() const;

  //! get/set zoom text
  bool isZoomText() const { return zoomText_; }
  void setZoomText(bool b);

  //---

  //! get/set default palette
  const PaletteName &defaultPalette() const { return defaultPalette_; }
  void setDefaultPalette(const PaletteName &name);

  //---

  //! get/set default symbol set name
  const QString &defaultSymbolSetName() const { return defaultSymbolSetName_; }
  void setDefaultSymbolSetName(const QString &name);

  CQChartsSymbolSet *defaultSymbolSet() const;

  //! get/set scale symbol size
  bool isScaleSymbolSize() const { return scaleSymbolSize_; }
  void setScaleSymbolSize(bool b);

  //---

  // scaled font size
  double minScaleFontSize() const { return minScaleFontSize_; }
  void setMinScaleFontSize(double r) { minScaleFontSize_ = r; }

  double maxScaleFontSize() const { return maxScaleFontSize_; }
  void setMaxScaleFontSize(double r) { maxScaleFontSize_ = r; }

  //---

 public:
  // key
  virtual bool isKeyVisible() const;
  virtual void setKeyVisible(bool b);

  virtual bool isKeyVisibleAndNonEmpty() const;

  bool isControlsKey() const { return controlsKey_; }
  void setControlsKey(bool b);

  virtual bool isColorKey() const;
  virtual void setColorKey(bool b);

 public:
  // color map key
  void addColorMapKey();

  bool isColorMapKey() const;

  //---

 public:
  virtual bool isEqualScale() const;
  virtual void setEqualScale(bool b);

  virtual void applyEqualScale(Range &dataRange) const;

  //---

  virtual bool isFollowMouse() const { return followMouse_; }
  virtual void setFollowMouse(bool b) { followMouse_ = b; }

  //---

  // fit
 public:
  virtual bool isAutoFit() const;
  virtual void setAutoFit(bool b);

  virtual const PlotMargin &fitMargin() const;
  virtual void setFitMargin(const PlotMargin &m);

  const Length &fitMarginLeft() const { return fitMargin().left(); }
  void setFitMarginLeft(const Length &l);

  const Length &fitMarginTop() const { return fitMargin().top(); }
  void setFitMarginTop(const Length &t);

  const Length &fitMarginRight() const { return fitMargin().right(); }
  void setFitMarginRight(const Length &r);

  const Length &fitMarginBottom() const { return fitMargin().bottom(); }
  void setFitMarginBottom(const Length &b);

  virtual void resetExtraFitBBox() const;

  virtual bool needsAutoFit() const;
  virtual void setNeedsAutoFit(bool b);

  //---

 public:
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

  bool showSelectedBoxes() const { return showSelectedBoxes_; }
  void setShowSelectedBoxes(bool b);

  //---

  // get/set bbox in view range
  virtual const BBox &viewBBox() const;
  virtual void setViewBBox(const BBox &bbox);

  virtual BBox calcViewBBox() const;

  // get inner view bbox
  virtual BBox innerViewBBox() const;

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
  const PlotMargin &innerMargin() const;
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
  const PlotMargin &outerMargin() const;
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

  // pixel aspect ratio
  double pixelAspect() const;
  void pixelSize(double &pw, double &ph) const;

  //---

  virtual Axis *xAxis() const;
  virtual Axis *yAxis() const;

  virtual Axis *mappedXAxis() const;
  virtual Axis *mappedYAxis() const;

  virtual bool xAxisName(QString &, const QString& ="") const { return false; }
  virtual bool yAxisName(QString &, const QString& ="") const { return false; }

  void setOverlayPlotsAxisNames();
  void setPlotsAxisNames(const Plots &plots, Plot *axisPlot);

  void initAxisSizes();

  virtual void clearAxisSideDelta();

  virtual double xAxisSideDelta(const CQChartsAxisSide::Type &side) const;
  virtual void setXAxisSideDelta(const CQChartsAxisSide::Type &side, double d);
  virtual double yAxisSideDelta(const CQChartsAxisSide::Type &side) const;
  virtual void setYAxisSideDelta(const CQChartsAxisSide::Type &side, double d);

  //---

  virtual Title *title() const;

  virtual void emitTitleChanged();

  //---

  virtual PlotKey *key() const;

  //---

  // composite plot control data
  virtual bool isComposite() const { return false; }

  Plot *parentPlot() const { return connectData_.parent; }
  void setParentPlot(Plot *parent) { connectData_.parent = parent; }

  virtual int childPlotIndex(const Plot *) const { return -1; }
  virtual int numChildPlots() const { return 0; }
  virtual Plot *childPlot(int) const { return nullptr; }

  int plotDepth() const { return (parentPlot() ? parentPlot()->plotDepth() + 1 : 0); }

  virtual Plot *currentPlot() const { return const_cast<Plot *>(this); }

  //---

  // expand/collapse control data
  const Plot *rootPlot() const { return connectData_.root; }
  void setRootPlot(Plot *root) { connectData_.root = root; }

  virtual void notifyCollapse() { }

  //---

  // Connection (overlay, x1x2, y1y2, tabbed
  virtual bool isOverlay(bool checkVisible=true) const;
  void setOverlay(bool b, bool notify=true);

  void updateOverlay();

  virtual bool isX1X2(bool checkVisible=true) const;
  void setX1X2(bool b, bool notify=true);

  virtual bool isY1Y2(bool checkVisible=true) const;
  void setY1Y2(bool b, bool notify=true);

  virtual bool isTabbed(bool checkVisible=true) const;
  void setTabbed(bool b, bool notify=true);

  //---

  //! get/set show all overlay x axes
  bool isShowAllXOverlayAxes() const { return showAllXOverlayAxes_; }
  void setShowAllXOverlayAxes(bool b);

  //! get/set show all overlay y axes
  bool isShowAllYOverlayAxes() const { return showAllYOverlayAxes_; }
  void setShowAllYOverlayAxes(bool b);

  //---

  //! get/set tabbed bar rect
  const BBox &tabRect() const { return connectData_.tabRect; }
  void setTabRect(const BBox &rect) { connectData_.tabRect = rect; }

  //! get/set is current overlay
  bool isCurrent() const { return connectData_.current; }
  void setCurrent(bool b, bool notify=false);

  //! get connection state summary string
  QString connectionStateStr() const;

  //---

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

  // run const function on all overlay plots
  template<typename FUNCTION>
  void processOverlayPlots(FUNCTION f) const {
    const auto *plot = firstPlot();

    while (plot) {
      f(plot);

      plot = plot->nextPlot();
    }
  }

  // run non-const function on all overlay plots
  template<typename FUNCTION>
  void processOverlayPlots(FUNCTION f) {
    auto *plot = firstPlot();

    while (plot) {
      f(plot);

      plot = plot->nextPlot();
    }
  }

  // run const function on all overlay plots until result does not match boolean
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

  void x1x2Plots(Plots &plots);
  void y1y2Plots(Plots &plots);

  void resetConnectData(bool notify=true);

  //---

  Plot *selectionPlot() const;

  //---

  void cycleNextPlot();
  void cyclePrevPlot();

  //---

  // invert x/y
  virtual bool isInvertX() const { return invertX_; }
  virtual void setInvertX(bool b);

  virtual bool isInvertY() const { return invertY_; }
  virtual void setInvertY(bool b);

  //---

  // log x/y
  virtual bool isLogX() const;
  virtual void setLogX(bool b);

  virtual bool isLogY() const;
  virtual void setLogY(bool b);

  //---

  // animation
  void startAnimateTimer();
  void stopAnimateTimer ();

  virtual bool isAnimated() const { return false; }

  virtual void animateStep() { }

  //---

  // properties
  const PropertyModel *propertyModel() const;
  PropertyModel *propertyModel();

  // add plot properties to model
  virtual void addProperties();

  void addBaseProperties();

  void addSymbolProperties(const QString &path, const QString &prefix,
                           const QString &descPrefix, bool hidden=false);

  void addLineProperties(const QString &path, const QString &prefix, const QString &descPrefix,
                         uint types=uint(CQChartsStrokeDataTypes::STANDARD), bool hidden=false);

  void addFillProperties(const QString &path, const QString &prefix, const QString &descPrefix,
                         uint types=uint(CQChartsFillDataTypes::STANDARD), bool hidden=false);

  void addTextProperties(const QString &path, const QString &prefix, const QString &descPrefix,
                         uint valueTypes=CQChartsTextOptions::ValueType::SIMPLE,
                         bool hidden=false);

  void addColorMapProperties();

  virtual void updateProperties();

  CQPropertyViewItem *addStyleProp(const QString &path, const QString &name, const QString &alias,
                                   const QString &desc, bool hidden=false);

  CQPropertyViewItem *addProp(const QString &path, const QString &name, const QString &alias,
                              const QString &desc, bool hidden=false);

  void hideProp(QObject *obj, const QString &path);

  void enableProp(QObject *obj, const QString &path, bool enabled);

  //---

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

  //! get nams of all properties settable on plot
  void getObjectPropertyNames(PlotObj *plotObj, QStringList &names) const;

  void hideProperty(const QString &path, QObject *object);

  //---

  // margins
  void updateMargins(bool update=true);

  void updateMargins(const PlotMargin &outerMargin);

  //---

  bool isNormalizedIndex(const ModelIndex &ind) const;

  ModelIndex  normalizeIndex(const ModelIndex &ind) const;
  QModelIndex normalizeIndex(const QModelIndex &ind) const;

  ModelIndex  unnormalizeIndex(const ModelIndex &ind) const;
  QModelIndex unnormalizeIndex(const QModelIndex &ind) const;

  QAbstractItemModel *sourceModel() const;

  using ProxyModels = std::vector<QAbstractProxyModel *>;

  void proxyModels(ProxyModels &proxyModels, QAbstractItemModel* &sourceModel) const;

  //---

  void addColumnValues(const Column &column, CQChartsValueSet &valueSet) const;

  //---

  using ModelVisitor = CQChartsPlotModelVisitor;

  void visitModel(ModelVisitor &visitor) const;

  //---

 public:
  double modelNumericValue(const ModelIndex &ind, bool &ok) const;

 public:
  // get model real value with support for log value mapping
  bool modelMappedReal(const ModelIndex &ind, double &r, bool log, double def) const;

  //---

  int getRowForId(const QString &id) const;

  QString idColumnString(int row, const QModelIndex &parent, bool &ok) const;

  //---

  QModelIndex normalizedModelIndex(const ModelIndex &ind) const;

  QModelIndex modelIndex(const ModelIndex &ind) const;

 private:
  bool modelMappedReal(int row, const Column &col, const QModelIndex &ind,
                       double &r, bool log, double def) const;

  QModelIndex modelIndex(int row, const Column &column,
                         const QModelIndex &parent=QModelIndex(), bool normalized=false) const;

  //----

 public:
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

  QString modelVHeaderString(int section, int role, bool &ok) const;
  QString modelVHeaderString(int section, bool &ok) const;

  virtual QString modelVHeaderString(QAbstractItemModel *model, int section,
                                     int role, bool &ok) const;
  virtual QString modelVHeaderString(QAbstractItemModel *model, int section, bool &ok) const;

  //---

 public:
  QVariant modelValue(const ModelIndex &ind, int role, bool &ok) const;
  QVariant modelValue(const ModelIndex &ind, bool &ok) const;

 protected:
  virtual QVariant modelValue(QAbstractItemModel *model, int row, const Column &column,
                              const QModelIndex &parent, int role, bool &ok) const;
  virtual QVariant modelValue(QAbstractItemModel *model, int row, const Column &column,
                              const QModelIndex &parent, bool &ok) const;

 private:
  QVariant modelValue(int row, const Column &column,
                      const QModelIndex &parent, int role, bool &ok) const;
  QVariant modelValue(int row, const Column &column,
                      const QModelIndex &parent, bool &ok) const;

  //---

 public:
  QString modelString(const ModelIndex &ind, int role, bool &ok) const;
  QString modelString(const ModelIndex &ind, bool &ok) const;

  QString modelString(QAbstractItemModel *model, const ModelIndex &ind, int role, bool &ok) const;
  QString modelString(QAbstractItemModel *model, const ModelIndex &ind, bool &ok) const;

 protected:
  virtual QString modelString(QAbstractItemModel *model, int row, const Column &column,
                              const QModelIndex &parent, int role, bool &ok) const;
  virtual QString modelString(QAbstractItemModel *model, int row, const Column &column,
                              const QModelIndex &parent, bool &ok) const;

 private:
  QString modelString(int row, const Column &column,
                      const QModelIndex &parent, int role, bool &ok) const;
  QString modelString(int row, const Column &column,
                      const QModelIndex &parent, bool &ok) const;

  //---

 public:
  double modelReal(const ModelIndex &ind, int role, bool &ok) const;
  double modelReal(const ModelIndex &ind, bool &ok) const;

 protected:
  virtual double modelReal(QAbstractItemModel *model, int row, const Column &column,
                           const QModelIndex &parent, int role, bool &ok) const;
  virtual double modelReal(QAbstractItemModel *model, int row, const Column &column,
                           const QModelIndex &parent, bool &ok) const;

 private:
  double modelReal(int row, const Column &column,
                   const QModelIndex &parent, int role, bool &ok) const;
  double modelReal(int row, const Column &column,
                   const QModelIndex &parent, bool &ok) const;

  //---

 public:
  long modelInteger(const ModelIndex &ind, int role, bool &ok) const;
  long modelInteger(const ModelIndex &ind, bool &ok) const;

 protected:
  virtual long modelInteger(QAbstractItemModel *model, int row, const Column &column,
                            const QModelIndex &parent, int role, bool &ok) const;
  virtual long modelInteger(QAbstractItemModel *model, int row, const Column &column,
                            const QModelIndex &parent, bool &ok) const;

 private:
  long modelInteger(int row, const Column &column,
                    const QModelIndex &parent, int role, bool &ok) const;
  long modelInteger(int row, const Column &column,
                    const QModelIndex &parent, bool &ok) const;

  //---

 public:
  Column mapColumn(const Column &column) const;

 public:
  std::vector<double> modelReals(const ModelIndex &ind, bool &ok) const;

 private:
  std::vector<double> modelReals(int row, const Column &column,
                                 const QModelIndex &parent, bool &ok) const;

  //---

 public:
  QVariant modelRootValue(const ModelIndex &ind, bool &ok) const;
  QVariant modelRootValue(const ModelIndex &ind, int role, bool &ok) const;

 private:
  QVariant modelRootValue(int row, const Column &column,
                          const QModelIndex &parent, int role, bool &ok) const;
  QVariant modelRootValue(int row, const Column &column,
                          const QModelIndex &parent, bool &ok) const;

  //---

 public:
  QVariant modelHierValue(const ModelIndex &ind, bool &ok) const;

 private:
  QVariant modelHierValue(int row, const Column &column,
                          const QModelIndex &parent, int role, bool &ok) const;
  QVariant modelHierValue(int row, const Column &column,
                          const QModelIndex &parent, bool &ok) const;

  //---

 public:
  QString modelHierString(const ModelIndex &ind, bool &ok) const;

 private:
  QString modelHierString(int row, const Column &column,
                          const QModelIndex &parent, int role, bool &ok) const;
  QString modelHierString(int row, const Column &column,
                          const QModelIndex &parent, bool &ok) const;

  //---

 public:
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

  double lengthPlotSignedWidth (const Length &len) const;
  double lengthPlotSignedHeight(const Length &len) const;

  double lengthPixelSize(const Length &len, bool vertical) const;

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

  Size windowToPixelSize(const Size &s) const;

  double windowToPixelWidth (double ww) const;
  double windowToPixelHeight(double wh) const;

  Polygon windowToPixel(const Polygon &p) const;

  QPainterPath windowToPixel(const QPainterPath &p) const;

 protected:
  // windowToViewI : view::windowToPixel
  void windowToPixelI(const Point &w, Point &p) const;
//void windowToPixelI(const BBox &wrect, BBox &prect) const;

  // view:pixelToWindow : viewToWindowI
  void pixelToWindowI(const Point &p, Point &w) const;
//void pixelToWindowI(const BBox &prect, BBox &wrect) const;

  virtual void pixelToWindowI(double px, double py, double &wx, double &wy) const;
  virtual void viewToWindowI (double vx, double vy, double &wx, double &wy) const;

  virtual void windowToPixelI(double wx, double wy, double &px, double &py) const;
  virtual void windowToViewI (double wx, double wy, double &vx, double &vy) const;

  //---

 public:
  void plotSymbolSize(const Length &xs, const Length &ys,
                      double &sx, double &sy, bool scale) const;
  void plotSymbolSize(const Length &xs, const Length &ys, double &sx, double &sy) const;
  void plotSymbolSize(const Length &s, double &sx, double &sy,
                      bool scale, const Qt::Orientation &dir=Qt::Horizontal) const;
  void plotSymbolSize(const Length &s, double &sx, double &sy,
                      const Qt::Orientation &dir=Qt::Horizontal) const;

  void pixelSymbolSize(const Length &s, double &sx, double &sy,
                       bool scale, const Qt::Orientation &dir=Qt::Horizontal) const;
  void pixelSymbolSize(const Length &s, double &sx, double &sy,
                       const Qt::Orientation &dir=Qt::Horizontal) const;

  double limitSymbolSize(double s) const;
  double limitFontSize(double s) const;
  double limitLineWidth(double w) const;

 private:
  void plotSymbolSize1(const Length &xs, const Length &ys,
                       double &sx, double &sy, bool scale) const;
  void plotSymbolSize1(const Length &s, double &sx, double &sy,
                       bool scale, const Qt::Orientation &dir) const;

  void pixelSymbolSize1(const Length &s, double &sx, double &sy,
                        bool scale, const Qt::Orientation &dir) const;

  //---

 public:
  virtual void addAxes();

  virtual void addXAxis();
  virtual void addYAxis();

  void setAxisColumnLabels(Axis *axis);

  //---

  virtual void addKey();

  virtual void resetKeyItems(bool add=true);
  virtual void resetPlotKeyItems(Plot *plot, bool add=true);

  // add items to key
  virtual void doAddKeyItems(PlotKey *key);

  virtual void addKeyItems(PlotKey *) { }

  virtual bool addColorKeyItems(PlotKey *key);

  //---

  virtual void addTitle();

 protected:
  // update data range (calls calcRange)
  void execUpdateRange();

 public:
  // calculate and return range from data
  virtual Range calcRange() const = 0;

  virtual void postCalcRange() { }

  virtual void postUpdateRange() { }

  // update plot objects (clear objects, objects updated on next redraw)
  void updateGroupedObjs();

  // reset range and objects
  void clearRangeAndObjs();

  virtual void postUpdateObjs() { }

  virtual void postDraw() { }

  virtual void postObjTree() { }

 protected:
  // recalc range and clear objects (objects updated on next redraw)
  void execUpdateRangeAndObjs();

  // update plot objects (clear objects, objects updated on next redraw)
  void execUpdateObjs();

 protected:
  void startThreadTimer();
  void stopThreadTimer();

  //---

  void setUpdatesEnabled1(bool b, bool update);

  void updateRange1();
  void updateRangeAndObjs1();
  void updateObjs1();

  //---

  void updateAndApplyRange(bool apply, bool updateObjs);

  void updateAndApplyPlotRange(bool apply, bool updateObjs);

  void updateAndApplyPlotRange1(bool updateObjs);

  //---

  static void updateRangeASync(Plot *plot);

  void updateRangeThread();

  void interruptRange();

  //---

  void startUpdateRange();
  void startUpdateRangeAndObjs();
  void startUpdateObjs();
  void startUpdateDrawBackground();
  void startUpdateDrawForeground();
  void startUpdateDrawObjs();
  void startCalcRange(bool updateObjs);
  void startCalcObjs();
  void startDrawObjs();

  void drawObjs1();

 protected:
  bool isReady() const;

#if 0
 protected:
  virtual void waitDataRange();
  virtual void waitCalcObjs();
  virtual void waitDrawObjs();
#endif

 public:
  void syncAll();

  void syncRange();
  void syncObjs();
  void syncDraw();

 protected:
  void syncState();

 protected:
  friend class CQChartsCompositePlot;

  virtual void waitRange();
  virtual void waitDraw();
  virtual void waitObjs();
  virtual void waitTree();

 protected:
  void execWaitRange();

  //---

  static void updateObjsASync(Plot *plot);

  void updateObjsThread();

  void interruptObjs();

  void execWaitObjs();

  //---

  void updatePlotObjs();

  virtual void resetInsideObjs();
  void resetInsideObjs1();

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

  void initSymbolTypeData(SymbolTypeData &symbolTypeData) const;

  bool columnSymbolType(int row, const QModelIndex &parent, const SymbolTypeData &symbolTypeData,
                        Symbol &symbolType) const;

  bool varSymbolType(const QVariant &var, const SymbolTypeData &symbolTypeData,
                     Symbol &symbolType) const;

  //---

  void initSymbolSizeData(SymbolSizeData &symbolSizeData) const;

  bool columnSymbolSize(int row, const QModelIndex &parent, const SymbolSizeData &symbolSizeData,
                        Length &symbolSize, Qt::Orientation &sizeDir) const;

  bool varSymbolSize(const QVariant &var, const SymbolSizeData &symbolSizeData,
                     Length &symbolSize, Qt::Orientation &sizeDir) const;

  //---

  void initFontSizeData(FontSizeData &fontSizeData) const;

  bool columnFontSize(int row, const QModelIndex &parent, const FontSizeData &fontSizeData,
                      Length &fontSize, Qt::Orientation &sizeDir) const;

  //---

 protected:
  // (re)initialize grouped plot objects
  void initGroupedPlotObjs();

  // (re)initialize plot range
  bool initPlotRange();

  // (re)initialize plot objects
  virtual void initPlotObjs();

  bool addNoDataObj();

  void updateAutoFit();

  virtual void autoFitOne();

  virtual void autoFitUpdate();

 public:
  // (re)initialize plot objects (called by initPlotObjs)
  virtual bool initObjs();

  // create plot objects (called by initObjs)
  bool createObjs();

  virtual bool hasPlotObjs() const;

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

  virtual BBox findEmptyBBox(double w, double h) const;

  //---

  BBox calcDataRange(bool adjust=true) const;
  void calcDataRanges(BBox &rawRange, BBox &adjustedRange) const;

  BBox getDataRange() const;

  virtual void updateAxisRanges(const BBox &adjustedRange);

  void clearOverlayErrors();
  void updateOverlayRanges();

  void setPixelRange(const BBox &bbox);

  void resetWindowRange();
  void setWindowRange(const BBox &rawBBox, const BBox &bbox);

  virtual bool isApplyDataRange() const;
  virtual void applyDataRangeAndDraw();

  virtual void applyDataRange(bool propagate=true);

  Range adjustDataRange(const Range &range) const;

  struct RangeTypes {
    bool axes       { false };
    bool key        { false };
    bool title      { false };
    bool annotation { false };
    bool extra      { false };

    RangeTypes &setAxes      (bool b=true) { axes       = b; return *this; }
    RangeTypes &setKey       (bool b=true) { key        = b; return *this; }
    RangeTypes &setTitle     (bool b=true) { title      = b; return *this; }
    RangeTypes &setAnnotation(bool b=true) { annotation = b; return *this; }
    RangeTypes &setExtra     (bool b=true) { extra      = b; return *this; }

    RangeTypes() { }
  };

  BBox calcGroupedDataRange(const RangeTypes &rangeTypes=RangeTypes()) const;

  BBox calcGroupedXAxisRange(const CQChartsAxisSide::Type &side) const;
  BBox calcGroupedYAxisRange(const CQChartsAxisSide::Type &side) const;

  //---

  CQPropertyViewItem *addProperty(const QString &path, QObject *object,
                                  const QString &name, const QString &alias="");

  void addPlotObject(PlotObj *obj);

  virtual void clearPlotObjects();
  virtual void clearInsideObjects();

  virtual void invalidateObjTree();

  virtual bool updateInsideObjects(const Point &w, Constraints constraints);

  bool setInsideObjects(const Point &w, Objs &objs);

  Obj *groupedInsideObject() const;

  virtual Obj *insideObject() const;

  void setInsideObject();

  virtual QString insideObjectText() const;

  void nextInsideInd();
  void prevInsideInd();

  //---

  //! \brief general error
  struct Error {
    QString msg;
  };

  //! \brief error for bad column
  struct ColumnError {
    Column  column;
    QString msg;
  };

  //! \brief error accessing data in model
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

  int numPlotObjects() const { return int(plotObjs_.size()); }

  PlotObj *plotObject(int i) const { return CUtil::safeIndex(plotObjs_, i); }

  bool isNoData() const { return noData_; }
  void setNoData(bool b) { noData_ = b; }

  //---

  virtual void doPostObjTree();

  virtual bool isPlotObjTreeSet() const;
  virtual void setPlotObjTreeSet(bool b);

  //----

  // columns
  const Column &xValueColumn() const { return xValueColumn_; }
  void setXValueColumn(const Column &column);

  const Column &yValueColumn() const { return yValueColumn_; }
  void setYValueColumn(const Column &column);

  const Column &idColumn() const { return idColumn_; }
  void setIdColumn(const Column &column);

  const Column &tipHeaderColumn() const { return tipHeaderColumn_; }
  void setTipHeaderColumn(const Column &c);

  const Columns &tipColumns() const { return tipColumns_; }
  void setTipColumns(const Columns &columns);

  const Columns &noTipColumns() const { return noTipColumns_; }
  void setNoTipColumns(const Columns &columns);

  const Column &visibleColumn() const { return visibleColumn_; }
  void setVisibleColumn(const Column &column);

  const Column &imageColumn() const { return imageColumn_; }
  void setImageColumn(const Column &column);

  const Column &colorColumn() const { return colorColumnData_.column; };
  virtual void setColorColumn(const Column &c);

  const Column &alphaColumn() const { return alphaColumnData_.column; };
  void setAlphaColumn(const Column &c);

  const Column &fontColumn() const { return fontColumn_; };
  void setFontColumn(const Column &c);

  const Columns &controlColumns() const { return controlColumns_; }
  void setControlColumns(const Columns &columns);

  //--

  virtual Column getNamedColumn(const QString &name) const;
  virtual void setNamedColumn(const QString &name, const Column &c);

  virtual Columns getNamedColumns(const QString &name) const;
  virtual void setNamedColumns(const QString &name, const Columns &c);

 public:
  // coloring
  const ColorColumnData &colorColumnData() const { return colorColumnData_; }

  ColorType colorType() const { return static_cast<ColorType>(colorColumnData_.colorType); }
  void setColorType(const ColorType &t);

  bool isColorMapped() const { return colorColumnData_.mapped; }
  void setColorMapped(bool b);

  double colorMapMin() const { return colorColumnData_.map_min; }
  void setColorMapMin(double r);

  double colorMapMax() const { return colorColumnData_.map_max; }
  void setColorMapMax(double r);

  const ColumnType &colorMapType() const { return colorColumnData_.modelType; }

  const PaletteName &colorMapPalette() const { return colorColumnData_.palette; }
  void setColorMapPalette(const PaletteName &name);

  const ColorStops &colorXStops() const { return colorColumnData_.xStops; }
  void setColorXStops(const ColorStops &s);

  const ColorStops &colorYStops() const { return colorColumnData_.yStops; }
  void setColorYStops(const ColorStops &s);

  const CQChartsColorMap &colorMap() const { return colorColumnData_.colorMap; }
  void setColorMap(const CQChartsColorMap &s);

 protected:
  double colorMapDataMin() const { return colorColumnData_.data_min; }
  double colorMapDataMax() const { return colorColumnData_.data_max; }

  //---

 public:
  bool isAlphaMapped() const { return alphaColumnData_.mapped; }
  void setAlphaMapped(bool b);

  double alphaMapMin() const { return alphaColumnData_.map_min; }
  void setAlphaMapMin(double r);

  double alphaMapMax() const { return alphaColumnData_.map_max; }
  void setAlphaMapMax(double r);

 protected:
  double alphaMapDataMin() const { return alphaColumnData_.data_min; }
  double alphaMapDataMax() const { return alphaColumnData_.data_max; }

 public:
  // color column
  bool colorColumnColor(int row, const QModelIndex &parent, Color &color) const;

  bool modelIndexColor(const ModelIndex &ind, Color &color) const;

  bool columnValueColor(const QVariant &var, Color &color) const;

  Color colorMapRealColor(double r) const;
  Color normalizedColorMapRealColor(double r) const;

  Color colorFromColorMapPaletteValue(double r) const;

  //---

  // alpha column
  bool alphaColumnAlpha(int row, const QModelIndex &parent, Alpha &alpha) const;

  bool modelIndexAlpha(const ModelIndex &ind, Alpha &alpha) const;

  bool columnValueAlpha(const QVariant &var, Alpha &alpha) const;

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

  virtual QString columnValueToString(const Column &column, const QVariant &var) const;

  bool formatColumnValue(const Column &column, const QVariant &var, QString &str) const;

  //---

  // handle mouse press/move/release
  virtual bool selectMousePress  (const Point &p, SelMod selMod);
  virtual bool selectMouseMove   (const Point &p, bool first=false);
  virtual bool selectMouseRelease(const Point &p);

  virtual bool selectMouseDoubleClick(const Point &p, SelMod selMod);

  //---

  // select interface
  virtual bool handleSelectPress  (const Point &p, SelMod selMod);
  virtual bool handleSelectMove   (const Point &p, Constraints constraints=Constraints::EDITABLE,
                                   bool first=false);
  virtual bool handleSelectRelease(const Point &p);

  virtual bool handleSelectDoubleClick(const Point &p, SelMod selMod);

  //-

  bool tabbedSelectPress(const Point &w, SelMod selMod);

  Plot *tabbedPressPlot(const Point &w, Plots &plots) const;

  bool keySelectPress   (PlotKey *key, const Point &w, SelMod selMod);
  bool mapKeySelectPress(const Point &w, SelMod selMod);
  bool titleSelectPress (Title *title, const Point &w, SelMod selMod);

//bool annotationsSelectPress(const Point &w, SelMod selMod);

  Obj *objectsSelectPress(const Point &w, SelMod selMod);

  Obj *objectsSelectDoubleClick(const Point &w, SelMod selMod);

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
    MAP_KEY,     //!< map key
    TITLE,       //!< title
    ANNOTATION   //!< annotation
  };

  //! mouse state data
  // handle mouse drag press/move/release
  bool editMousePress  (const Point &p, bool inside=false);
  bool editMouseMove   (const Point &p, bool first=false);
  bool editMouseMotion (const Point &p);
  bool editMouseRelease(const Point &p);

  //---

  // handle edit
  virtual bool handleEditPress  (const Point &p, const Point &w, bool inside=false);
  virtual bool handleEditMove   (const Point &p, const Point &w, bool first=false);
  virtual bool handleEditMotion (const Point &p, const Point &w); // return true if inside
  virtual bool handleEditRelease(const Point &p, const Point &w);

  void handleEditMoveBy(const Point &d);

  //---

  void setDragObj(DragObjType objType, Obj *obj);

  void flipSelected(Qt::Orientation orient);

  //-

  bool keyEditPress(PlotKey *key, const Point &w);

  bool mapKeyEditPress(const Point &w);

  bool axisEditPress (Axis  *axis , const Point &w);
  bool titleEditPress(Title *title, const Point &w);

  bool annotationsEditPress(const Point &w);

  bool objectsEditPress(const Point &w, bool inside);

  bool editHandlePress(Obj *obj, const Point &w, const DragObjType &dragObjType);

  //--

  bool keyEditSelect(PlotKey *key  , const Point &w);

  bool mapKeyEditSelect(const Point &w);

  bool axisEditSelect (Axis  *axis , const Point &w);
  bool titleEditSelect(Title *title, const Point &w);

//bool annotationsEditSelect(const Point &w);

  bool objectsEditSelect(const Point &w, bool inside);

  //--

  void selectOneObj(Obj *obj, bool allObjs);

  void deselectAllObjs();

  void deselectAll();

  // handle key press
  virtual bool keyPress(int key, int modifier);

  // get tip text at point (checked)
  bool tipText(const Point &p, QString &tip) const;

  // get tip text at point (unchecked)
  virtual bool plotTipText(const Point &p, QString &tip, bool single) const;

  void addTipHeader(CQChartsTableTip &tableTip, const QModelIndex &ind) const;
  void addTipColumns(CQChartsTableTip &tableTip, const QModelIndex &ind) const;
  void addTipColumn(CQChartsTableTip &tableTip, const Column &c, const QModelIndex &ind) const;
  void addNoTipColumns(CQChartsTableTip &tableTip) const;

  void resetObjTips();

  // handle rect select
  bool rectSelect(const BBox &r, SelMod selMod);

  //---

  void selectObjs(const PlotObjs &objs, bool exportSel);

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

  //---

  bool executeSlot(const QString &name, const QStringList &args, QVariant &res);

  virtual bool executeSlotFn(const QString &name, const QVariantList &args, QVariant &res);

  void registerSlot(const QString &name, const QStringList &argTypes);

  QStringList getSlotNames() const;

 public slots:
  // color map key
  void setColorMapKey(bool b);

  //---

  void propertyItemChanged(QObject *, const QString &);

  void updateSlot();
  void updateObjsSlot();

  virtual void cycleNext();
  virtual void cyclePrev();

  //---

  virtual void wheelHScroll(int delta);
  virtual void wheelVScroll(int delta);
  virtual void wheelZoom(const Point &pp, int delta);

  //---

  virtual void panLeft (double f=0.125);
  virtual void panRight(double f=0.125);
  virtual void panUp   (double f=0.125);
  virtual void panDown (double f=0.125);

  //---

  virtual void zoomIn(double f=1.5);
  virtual void zoomOut(double f=1.5);

  virtual void zoomFull(bool notify=true);
  virtual void zoomToData();

  //---

  virtual bool allowZoomX() const { return true; }
  virtual bool allowZoomY() const { return true; }

  virtual bool allowPanX() const { return true; }
  virtual bool allowPanY() const { return true; }

 public:
  virtual void cycleNextPrev(bool prev);

  virtual void pan(double dx, double dy);

  virtual void adjustPan() { }

  virtual void zoomTo(const BBox &bbox);
  virtual void unzoomTo(const BBox &bbox);

  virtual void centerAt(const Point &c);

  //---

  void updateTransform();

  //---

  virtual bool addMenuItems(QMenu *) { return false; }

  void addRootMenuItems(QMenu *menu);

  QAction *addMenuAction(QMenu *menu, const QString &name, const char *slot);

  QAction *addMenuCheckedAction(QMenu *menu, const QString &name, bool isSet, const char *slot);

  //---

  // probe at x, y
  virtual bool probe(ProbeData & /*probeData*/) const { return false; }

  //---

  // called before resize
  virtual void preResize();

  // called after resize
  virtual void postResize();

  //---

  virtual void updateTitlePosition();

  //---

  virtual void updateKeyPosition(bool force=false);
  virtual void updatePlotKeyPosition(Plot *plot, bool force=false);

  //---

  BBox displayRangeBBox() const;
  BBox rawDisplayRangeBBox() const;

  BBox calcDataPixelRect() const;

  BBox calcPlotRect() const;
  BBox calcPlotPixelRect() const;
  BBox calcPlotViewRect() const;

  BBox calcFitPixelRect() const;

  Size calcPixelSize() const;

  void calcTabData(const Plots &plots) const;

  BBox calcTabPixelRect() const;

  //---

  // auto fit
  virtual void autoFit();

  virtual BBox fitBBox() const;

  void setFitBBox(const BBox &bbox);

  virtual BBox dataFitBBox       () const; // get bounding box to fit data objects
  virtual BBox axesFitBBox       () const; // get bounding box to fit axes
  virtual BBox keyFitBBox        () const; // get bounding box to fit key
  virtual BBox titleFitBBox      () const; // get bounding box to fit title
  virtual BBox annotationsFitBBox() const; // get bounding box to fit fittable annotations
  virtual BBox extraFitBBox      () const; // get bounding box of extra 'stuff' outside plot area

  // return cached extra fit box
  virtual BBox calcExtraFitBBox() const { return BBox(); }

  //---

  // --- annotations ---

  using Annotation             = CQChartsAnnotation;
  using Annotations            = std::vector<Annotation *>;
  using AnnotationGroup        = CQChartsAnnotationGroup;
  using ArcAnnotation          = CQChartsArcAnnotation;
  using ArcConnectorAnnotation = CQChartsArcConnectorAnnotation;
  using ArrowAnnotation        = CQChartsArrowAnnotation;
  using AxisAnnotation         = CQChartsAxisAnnotation;
  using ButtonAnnotation       = CQChartsButtonAnnotation;
  using EllipseAnnotation      = CQChartsEllipseAnnotation;
  using ImageAnnotation        = CQChartsImageAnnotation;
  using PathAnnotation         = CQChartsPathAnnotation;
  using KeyAnnotation          = CQChartsKeyAnnotation;
  using PieSliceAnnotation     = CQChartsPieSliceAnnotation;
  using PointAnnotation        = CQChartsPointAnnotation;
  using PointSetAnnotation     = CQChartsPointSetAnnotation;
  using Point3DSetAnnotation   = CQChartsPoint3DSetAnnotation;
  using PolygonAnnotation      = CQChartsPolygonAnnotation;
  using PolylineAnnotation     = CQChartsPolylineAnnotation;
  using RectangleAnnotation    = CQChartsRectangleAnnotation;
  using ShapeAnnotation        = CQChartsShapeAnnotation;
  using TextAnnotation         = CQChartsTextAnnotation;
  using ValueSetAnnotation     = CQChartsValueSetAnnotation;
  using WidgetAnnotation       = CQChartsWidgetAnnotation;
  using SymbolMapKeyAnnotation = CQChartsSymbolSizeMapKeyAnnotation;

  // get annotations
  const Annotations &annotations() const { return annotations_; }

  // --- add annotation ---

  using Point3DArray = std::vector<CQChartsGeom::Point3D>;

  Annotation *addAnnotation(CQChartsAnnotationType type);

  // TODO: remove custom add annotation routines
  AnnotationGroup        *addAnnotationGroup       ();
  ArcAnnotation          *addArcAnnotation         (const Position &start, const Position &end);
  ArcConnectorAnnotation *addArcConnectorAnnotation(const Position &center, const Length &radius,
                                                    const Angle &srcStartAngle,
                                                    const Angle &srcSpanAngle,
                                                    const Angle &destStartAngle,
                                                    const Angle &destSpanAngle, bool self);
  ArrowAnnotation        *addArrowAnnotation       (const Position &start, const Position &end);
  AxisAnnotation         *addAxisAnnotation        (Qt::Orientation direction, double start,
                                                    double end);
  ButtonAnnotation       *addButtonAnnotation      (const Position &pos, const QString &text);
  EllipseAnnotation      *addEllipseAnnotation     (const Position &center, const Length &xRadius,
                                                    const Length &yRadius);
  ImageAnnotation        *addImageAnnotation       (const Position &pos, const Image &image);
  ImageAnnotation        *addImageAnnotation       (const Rect &rect, const Image &image);
  PathAnnotation         *addPathAnnotation        (const Path &path);
  KeyAnnotation          *addKeyAnnotation         (const Column &column=Column());
  PieSliceAnnotation     *addPieSliceAnnotation    (const Position &pos, const Length &innerRadius,
                                                    const Length &outerRadius,
                                                    const Angle &startAngle,
                                                    const Angle &spanAngle);
  PointAnnotation        *addPointAnnotation       (const Position &pos, const Symbol &type);
  PointSetAnnotation     *addPointSetAnnotation    (const Rect &rect, const CQChartsPoints &values);
  Point3DSetAnnotation   *addPoint3DSetAnnotation  (const Point3DArray &points);
  PolygonAnnotation      *addPolygonAnnotation     (const CQChartsPolygon &polygon);
  PolylineAnnotation     *addPolylineAnnotation    (const CQChartsPolygon &polygon);
  RectangleAnnotation    *addRectangleAnnotation   (const Rect &rect);
  ShapeAnnotation        *addShapeAnnotation       (const Rect &rect);
  TextAnnotation         *addTextAnnotation        (const Position &pos, const QString &text);
  TextAnnotation         *addTextAnnotation        (const Rect &rect, const QString &text);
  ValueSetAnnotation     *addValueSetAnnotation    (const Rect &rect, const CQChartsReals &values);
  WidgetAnnotation       *addWidgetAnnotation      (const Position &pos, const Widget &widget);
  WidgetAnnotation       *addWidgetAnnotation      (const Rect &rect, const Widget &widget);
  SymbolMapKeyAnnotation *addSymbolMapKeyAnnotation();

  void initWidgetAnnotation(const Widget &widget);

  Annotation *addAnnotationI(Annotation *annotation);

  template<typename TYPE>
  TYPE *addAnnotationT(Plot *plot) {
    return dynamic_cast<TYPE *>(addAnnotationI(new TYPE(plot)));
  }

  template<typename TYPE>
  TYPE *addAnnotationT(TYPE *annotation) {
    return dynamic_cast<TYPE *>(addAnnotationI(annotation));
  }

  // --- get annotation ---

  Annotation *getAnnotationById(const QString &id) const;
  Annotation *getAnnotationByPathId(const QString &pathId) const;
  Annotation *getAnnotationByInd(int ind) const;

  //! raise/lower annotation
  void raiseAnnotation(Annotation *annotation);
  void lowerAnnotation(Annotation *annotation);

  //! get annotation position
  int annotationPos(Annotation *annotation) const;

  //! remove annotation
  void removeAnnotation(Annotation *annotation);

  //! remove all annotations
  void removeAllAnnotations();

  //---

  PlotObj *getPlotObject(const QString &objectId) const;

  Obj *getObject(const QString &objectId) const;

  QList<QModelIndex> getObjectInds(const QString &objectId) const;

  std::vector<Obj *> getObjectConnected(const QString &objectId) const;

  //---

  const Layers &layers() const { return layers_; }

  Layer *initLayer(const Layer::Type &type, const Buffer::Type &buffer, bool active);

  virtual void setLayerActive(const Layer::Type &type, bool b);
  virtual bool isLayerActive(const Layer::Type &type) const;

  //---

  virtual bool isInvalidateLayers() const;
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

 protected:
  virtual BBox adjustDataRangeBBox(const BBox &bbox) const;

  void setLayerActive1(const Layer::Type &type, bool b);
  bool isLayerActive1(const Layer::Type &type) const;

  void invalidateLayer1(const Buffer::Type &layerType);

  void setLayersChanged(bool update);

  void clearPlotObjects1();
  void clearInsideObjects1();

  void invalidateObjTree1();

  bool updateInsideObjects1(const Point &w, Constraints constraints);

  Obj *insideObject1() const;

  QString insideObjectText1() const;

  void deselectAll1(bool &changed);

  //---

  void zoomFull1(bool notify);

 public:
  virtual bool hasXAxis() const;
  virtual bool hasYAxis() const;

  //---

 public:
  struct BackgroundParts {
    bool rects       { false };
    bool axes        { false };
    bool key         { false };
    bool annotations { false };
    bool custom      { false };
  };

  struct ForegroundParts {
    bool axes        { false };
    bool key         { false };
    bool annotations { false };
    bool title       { false };
    bool custom      { false };
    bool tabbed      { false };
  };

  struct OverlayParts {
    bool selObjs         { false };
    bool selAnnotations  { false };
    bool boxes           { false };
    bool editHandles     { false };
    bool overObjs        { false };
    bool overAnnotations { false };
    bool custom          { false };
  };

  // draw plot parts (from draw)
  virtual void drawPlotParts(QPainter *painter) const;

  // draw plot parts (from draw and thread)
  virtual void drawParts(QPainter *painter) const;

  // draw background layer plot parts
  virtual void drawBackgroundParts(QPainter *painter) const;

  // draw background layer plot device parts
  virtual void drawBackgroundDeviceParts(PaintDevice *device, const BackgroundParts &bgParts) const;

  // draw middle layer plot parts
  virtual void drawMiddleParts(QPainter *painter) const;

  // draw middle layer plot device parts
  virtual void drawMiddleDeviceParts(PaintDevice *device, bool bg, bool mid, bool fg) const;

  // draw foreground layer plot parts
  virtual void drawForegroundParts(QPainter *painter) const;

  // draw foreground layer plot device parts
  virtual void drawForegroundDeviceParts(PaintDevice *device, const ForegroundParts &fgParts) const;

  virtual void drawTabs(PaintDevice *device) const;

  void drawTabs(PaintDevice *device, const Plots &plots) const;
  void drawTabs(PaintDevice *device, const Plots &plots, Plot *currentPlot) const;

  // draw overlay layer plot parts
  virtual void drawOverlayParts(QPainter *painter) const;

  // draw overlay layer plot device parts
  virtual void drawOverlayDeviceParts(PaintDevice *device, const OverlayParts &overlayParts) const;

  void drawOverlayFade(PaintDevice *device) const;

  //---

  // draw background rects
  virtual bool hasBackgroundRects() const;

  virtual void drawBackgroundRects(PaintDevice *device) const;

  virtual void drawBackgroundRect(PaintDevice *device, const DrawRegion &drawRegion,
                                  const BBox &rect, const BrushData &brushData,
                                  const PenData &penData, const Sides &sides) const;

  virtual void drawBackgroundSides(PaintDevice *device, const DrawRegion &drawRegion,
                                   const BBox &bbox, const Sides &sides) const;

  // draw custom background
  virtual bool hasBackground() const;

  virtual void drawCustomBackground(PaintDevice *device) const;

  virtual void execDrawBackground(PaintDevice *device) const;

  // draw axes on background
  virtual bool hasGroupedBgAxes() const;

  virtual bool hasBgAxes() const;

  virtual void drawGroupedBgAxes(PaintDevice *device) const;

  virtual void drawBgAxes (PaintDevice *device) const;
  virtual bool drawBgXAxis(PaintDevice *device) const;
  virtual bool drawBgYAxis(PaintDevice *device) const;

  // draw key on background
  virtual bool hasGroupedBgKey() const;

  virtual void drawBgKey(PaintDevice *device) const;

  //---

  // draw objects
  virtual bool hasGroupedObjs(const Layer::Type &layerType) const;

  virtual void drawGroupedObjs(PaintDevice *device, const Layer::Type &layerType) const;

  virtual bool hasObjs(const Layer::Type &layerType) const;

  virtual void preDrawFgObjs(PaintDevice *) const { }
  virtual void preDrawBgObjs(PaintDevice *) const { }
  virtual void preDrawObjs  (PaintDevice *) const { }

  virtual void execDrawObjs(PaintDevice *device, const Layer::Type &type) const;

  virtual void postDrawFgObjs(PaintDevice *) const { }
  virtual void postDrawBgObjs(PaintDevice *) const { }
  virtual void postDrawObjs  (PaintDevice *) const { }

  virtual bool objInsideBox(PlotObj *plotObj, const BBox &bbox) const;

  //---

  // draw axes on foreground
  virtual bool hasGroupedFgAxes() const;

  virtual bool hasFgAxes() const;

  virtual void drawGroupedFgAxes(PaintDevice *device) const;

  virtual void drawFgAxes (PaintDevice *device) const;
  virtual bool drawFgXAxis(PaintDevice *device) const;
  virtual bool drawFgYAxis(PaintDevice *device) const;

  virtual void drawXGrid(PaintDevice *device) const;
  virtual void drawYGrid(PaintDevice *device) const;

  virtual void drawXAxis(PaintDevice *device) const;
  virtual void drawYAxis(PaintDevice *device) const;

  virtual void drawXAxisAt(PaintDevice *device, Plot *plot, double pos) const;
  virtual void drawYAxisAt(PaintDevice *device, Plot *plot, double pos) const;

  void drawXAxis1(PaintDevice *device) const;
  void drawYAxis1(PaintDevice *device) const;

  virtual double xAxisHeight(const CQChartsAxisSide::Type &side) const;
  virtual double yAxisWidth (const CQChartsAxisSide::Type &side) const;

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

  virtual void drawCustomForeground(PaintDevice *device) const;

  virtual void execDrawForeground(PaintDevice *device) const;

  // draw debug boxes
  virtual bool hasGroupedBoxes() const;

  virtual void drawGroupedBoxes(PaintDevice *device) const;

  virtual bool hasBoxes() const;

  virtual void drawBoxes(PaintDevice *device) const;

  //---

  // draw edit handles
  EditHandles *editHandles() const override;

  virtual bool hasGroupedEditHandles() const;

  virtual void drawGroupedEditHandles(PaintDevice *device) const;

  virtual bool hasEditHandles() const;

  void drawEditHandles(PaintDevice *device) const override;

  //---

  // draw custom overlay
  virtual bool hasOverlay() const;

  virtual void drawCustomOverlay(PaintDevice *device) const;

  virtual void execDrawOverlay(PaintDevice *device) const;

  //---

  void addColorMapKeyProperties();

  bool canDrawColorMapKey() const;
  void drawColorMapKey(PaintDevice *device) const;

  void updateColorMapKey() const;

  CQChartsColorMapKey *colorMapKey() const { return colorMapKey_.get(); }

  void addColorMapKeyItems(QMenu *menu);
  void addColorMapKeySubItems(QMenu *keysMenu);

  int maxMappedValues() const { return maxMappedValues_; }
  void setMaxMappedValues(int i) { maxMappedValues_ = i; }

  //---

  virtual bool colorVisible(const QColor &color) const;

  QStringList colorFilterNames() const;
  void setColorFilterNames(const QStringList &names);

  virtual bool symbolTypeVisible(const Symbol &) const { return true; }
  virtual bool symbolSizeVisible(const Length &) const { return true; }

  //---

  virtual void updateMapKey(CQChartsMapKey *key) const;

  //---

  void addFilterColumn(const Column &column);
  void removeFilterColumn(const Column &column);

  bool isValueVisible(int row, const QModelIndex &parent) const;

  void setColumnValueVisible(const Column &column, const QVariant &value, bool visible);
  bool isColumnValueVisible(const Column &column, const QVariant &value) const;

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

  // draw symbol (symbol size is pixels)
  void drawSymbol(PaintDevice *device, const Point &p, const Symbol &symbol,
                  double xs, double ys, const PenBrush &penBrush, bool scaled=true) const;
  void drawSymbol(PaintDevice *device, const Point &p, const Symbol &symbol,
                  double xs, double ys, bool scaled=true) const;

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

  void setPainterFont(PaintDevice *device, const Font &font) const;

  //---

  void updateObjPenBrushState(const Obj *obj, PenBrush &penBrush,
                              DrawType drawType=DrawType::BOX) const;
  void updateObjPenBrushState(const Obj *obj, const ColorInd &ic,
                              PenBrush &penBrush, DrawType drawType) const;

  void updatePenBrushState(const ColorInd &ic, PenBrush &penBrush,
                           bool selected, bool inside);

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
  QColor        calcTextColor(const QColor &bg) const;
  CQChartsColor calcTextColor(const CQChartsColor &bg) const;

  //---

  ColorInd calcColorInd(const PlotObj *obj, const CQChartsColorBoxKeyItem *keyBox,
                        const ColorInd &is, const ColorInd &ig, const ColorInd &iv) const;

  //---

  bool checkNumericColumns(const Columns &columns, const QString &name,
                           bool required=false) const;
  bool checkNumericColumn(const Column &column, const QString &name,
                          bool required=false) const;
  bool checkNumericColumn(const Column &column, const QString &name,
                          ColumnType &type, bool required=false) const;

  bool checkColumns(const Columns &columns, const QString &name, bool required=false) const;

  bool checkColumn(const Column &column, const QString &name, bool required=false) const;

  bool checkColumn(const Column &column, const QString &name,
                   ColumnType &type, bool required=false) const;

  ColumnType columnValueType(const Column &column,
                             const ColumnType &defType=ColumnType::STRING) const;

  bool columnValueType(const Column &column, ModelTypeData &columnTypeData,
                       const ColumnType &defType=ColumnType::STRING) const;

  bool modelColumnValueType(const Column &column, ModelTypeData &columnTypeData) const;

#if 0
  bool columnTypeStr(const Column &column, QString &typeStr) const;

  bool setColumnTypeStr(const Column &column, const QString &typeStr);
#endif

  bool columnDetails(const Column &column, QString &typeName,
                     QVariant &minValue, QVariant &maxValue) const;

  ModelColumnDetails *columnDetails(const Column &column) const;

  //---

  ModelData *getModelData() const;

  ModelDetails *modelDetails() const;

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
  QString alphaHeaderName(bool tip=false) const { return columnHeaderName(alphaColumn(), tip); }
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

  bool isHideValue(const QVariant &value) const;

  //---

  void update();

  //---

  bool hasForegroundI() const;
  bool hasBackgroundI() const;
  bool hasOverlayI() const;

  //---

  // draw plot
  virtual void draw(QPainter *painter);

 // does plot buffer layers
  virtual bool isBufferLayers() const;

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

  //---

  void collapseRoot();

 public slots:
  void colorSelected(const QColor &c, bool visible);

 protected slots:
  void animateSlot();

  void threadTimerSlot();

  // model change slots
  void modelChangedSlot();

  void currentModelChangedSlot();

  //---

  void selectionSlot(QItemSelectionModel *sm);

  void updateAnnotationSlot();

  void plotControlUpdateSlot();

  void colorMapKeyPositionSlot(QAction *);
  void colorMapKeyInsideXSlot(bool);
  void colorMapKeyInsideYSlot(bool);

  void collapseRootSlot();

 signals:
  // model data changed
  void modelChanged();

  // data range changed
  void rangeChanged();

  // view box changed
  void viewBoxChanged();

  // plot objects added
  void plotObjsAdded();

  // plot drawn
  void plotDrawn();

  // connection (x1x2, y1y2, overlay) changed
  void connectDataChanged();

  // current connected plot changed
  void currentPlotChanged(CQChartsPlot *);
  void currentPlotIdChanged(const QString &);

  // layers changed (active, valid)
  void layersChanged();

  // control columns changed
  void controlColumnsChanged();

  // key signals (key, key item pressed)
  void keyItemPressed(CQChartsKeyItem *);
  void keyItemIdPressed(const QString &);

  void keyPressed(CQChartsPlotKey *);
  void keyIdPressed(const QString &);

  void mapKeyPressed(CQChartsMapKey *);

  // title signals (title changed)
  void titleChanged();

  // title signals (title pressed)
  void titlePressed(CQChartsTitle *);
  void titleIdPressed(const QString &);

  // annotation signals (annotation pressed, double clicked)
  void annotationPressed(CQChartsAnnotation *);
  void annotationIdPressed(const QString &);

  void annotationDoubleClicked(CQChartsAnnotation *);
  void annotationIdDoubleClicked(const QString &);

  // object signals (object pressed, double clicked)
  void objPressed(CQChartsPlotObj *);
  void objIdPressed(const QString &);

  void objDoubleClicked(CQChartsPlotObj *);
  void objIdDoubleClicked(const QString &);

  // pressed
  void selectPressSignal(const CQChartsGeom::Point &p);

  void selectDoubleClickSignal(const CQChartsGeom::Point &p);

  // zoom/pan changed
  void zoomPanChanged();

  // annotations changed
  void annotationsChanged();

  // emitted when annotations reordered
  void annotationsReordered();

  // emitted when selection changed
  void selectionChanged();

  // emitted when errors cleared or added
  void errorsCleared();
  void errorAdded();

  // emitted when color details changed (color column, color range, color palette)
  void colorDetailsChanged();

  // emitted when custom data changed
  void customDataChanged();

 protected:
  //! \brief RAII class to enable/disable no update state
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

  using ObjP        = QPointer<Obj>;
  using ObjPSet     = std::set<ObjP>;
  using SizeObjPSet = std::map<double, ObjPSet>;

 protected:
  void connectModel();
  void disconnectModel();

  void connectDisconnectModel(bool connectDisconnect);

  //---

 public:
  virtual void groupedObjsAtPoint(const Point &p, Objs &objs,
                                  const Constraints &constraints) const;

  void groupedAnnotationsAtPoint(const Point &p, Annotations &annotations,
                                 const Constraints &constraints) const;

  virtual void groupedObjsIntersectRect(const BBox &r, Objs &objs, bool inside,
                                        const Constraints &constraints) const;

  virtual bool objNearestPoint(const Point &p, PlotObj* &obj) const;

 protected:
  virtual void plotObjsAtPoint(const Point &p, PlotObjs &objs,
                               const Constraints &constraints) const;

  void groupedPlotObjsAtPoint(const Point &p, PlotObjs &objs,
                              const Constraints &constraints) const;

  void annotationsAtPoint(const Point &p, Annotations &annotations,
                          const Constraints &constraints) const;

  virtual void groupedPlotObjsIntersectRect(const BBox &r, PlotObjs &plotObjs, bool inside,
                                            const Constraints &constraints) const;
  virtual void plotObjsIntersectRect(const BBox &r, PlotObjs &plotObjs, bool inside,
                                     const Constraints &constraints) const;

  void annotationsIntersectRect(const BBox &r, Annotations &annotations, bool inside,
                                const Constraints &constraints) const;
  void annotationsIntersectRect1(const BBox &r, Annotations &annotations, bool inside,
                                 const Constraints &constraints) const;

  //---

 public:
  void getSelectIndices(QItemSelectionModel *sm, QModelIndexSet &indices);

  //---

#if 0
 public:
  void clearSkipColors() { skipColors_.clear(); }
  void addSkipColor(const QVariant &v, int row) { skipColors_[v].insert(row); }

  bool hasSkipColors() const { return ! skipColors_.empty(); }

  size_t numSkipColors(const QVariant &v) const {
    auto p = skipColors_.find(v);
    if (p == skipColors_.end()) return 0;

    return (*p).second.size();
  }
#endif

  //---

 public:
  virtual CQChartsPlotCustomControls *createCustomControls() = 0;

 protected:
  //*! \brief update state enum
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

  using ThreadObj = CQThreadObject;

  //*! \brief lock data
  struct LockData {
    mutable std::mutex lock;
    const char*        id { nullptr };
  };

  //*! \brief draw of busy indication data
  struct DrawBusyData {
    QColor      bgColor  { 255, 255, 255 };
    QColor      fgColor  { 100, 200, 100 };
    Font        font;
    int         count    { 10 };
    int         multiple { 10 };
    mutable int ind      { 0 };
  };

  //*! \brief update (threading) data
  using ThreadObjP = std::unique_ptr<ThreadObj>;
  using TimerP     = QPointer<QTimer>;

  struct UpdateData {
    std::atomic<int> state       { 0 };
    std::atomic<int> interrupt   { 0 };
    ThreadObjP       rangeThread;
    ThreadObjP       objsThread;
    ThreadObjP       drawThread;
    LockData         lockData;
    bool             updateObjs  { false };
    TimerP           timer;
    int              timeout     { 10 };
    DrawBusyData     drawBusy;
  };

  const QColor &updateBusyColor() const { return updateData_.drawBusy.fgColor; }
  void setUpdateBusyColor(const QColor &c) { updateData_.drawBusy.fgColor = c; }

  int updateBusyCount() const { return updateData_.drawBusy.count; }
  void setUpdateBusyCount(int i) { updateData_.drawBusy.count = i; }

  void setGroupedUpdateState(UpdateState state);

  UpdateState updateState() { return static_cast<UpdateState>(updateData_.state.load()); }
  void setUpdateState(UpdateState state);

  UpdateState calcNextState() const;

  void setInterrupt(bool b=true);

  //---

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

  //*! \brief RAII class to lock/unlock plot mutex
  struct LockMutex {
    LockMutex(Plot *plot, const char *id) : plot(plot) { plot->updateLock(id); }
   ~LockMutex() { plot->updateUnLock(); }

    Plot* plot { nullptr };
  };

  //*! \brief RAII class to try to lock/unlock plot mutex
  struct TryLockMutex {
    TryLockMutex(Plot *plot, const char *id) : plot(plot) {
      locked = plot->updateTryLock(id); }
   ~TryLockMutex() { if (locked) plot->updateUnLock(); }

    Plot* plot   { nullptr };
    bool  locked { false };
  };

  //---

  void drawBusy(QPainter *painter, const UpdateState &updateState) const;

 public:
  bool isInterrupt() const { return updateData_.interrupt.load() > 0; }

 protected:
  using IdHidden        = std::map<int, bool>;
  using Rows            = std::set<int>;
  using ColumnRows      = std::map<int, Rows>;
  using IndexColumnRows = std::map<QModelIndex, ColumnRows>;

  //---

  //! \brief every row selection data
  struct EveryData {
    bool enabled { false };
    int  start   { 0 };
    int  end     { std::numeric_limits<int>::max() };
    int  step    { 1 };

    friend bool operator==(const EveryData &lhs, const EveryData &rhs) {
      if (lhs.enabled != rhs.enabled) return false;
      if (lhs.start   != rhs.start  ) return false;
      if (lhs.end     != rhs.end    ) return false;
      if (lhs.step    != rhs.step   ) return false;
      return true;
    }

    friend bool operator!=(const EveryData &lhs, const EveryData &rhs) {
      return ! operator==(lhs, rhs);
    }
  };

  //---

  //! \brief mouse state (event) data
  struct MouseData {
    Point              pressPoint  { 0, 0 };
    Point              movePoint   { 0, 0 };
    bool               pressed     { false };
    DragObjType        dragObjType { DragObjType::NONE };
    Obj*               dragObj     { nullptr };
    CQChartsResizeSide dragSide    { CQChartsResizeSide::NONE };
    bool               dragged     { false };
  };

  //---

  //! \brief animation data
  struct AnimateData {
    TimerP timer;
    int    tickLen { 30 };
  };

  //---

  //! \brief update state data
  struct UpdatesData {
    using StateFlag = std::map<UpdateState, int>;

    std::atomic<int>  enabled            { 0 };     //!< updates enabled
    std::atomic<bool> updateRangeAndObjs { false }; //!< call execUpdateRangeAndObjs (on enable)
    std::atomic<bool> updateObjs         { false }; //!< call execUpdateObjs (on enable)
    std::atomic<bool> applyDataRange     { false }; //!< call applyDataRange (on enable)
    std::atomic<bool> invalidateLayers   { false }; //!< call needsInvalidate invalidate (on enable)
    StateFlag         stateFlag;                    //!< state flags

    void reset() {
      updateRangeAndObjs = false;
      updateObjs         = false;
      applyDataRange     = false;
      invalidateLayers   = false;

      //stateFlag.clear();
    }
  };

  //---

  using ColumnNames = std::map<Column, QString>;

  //---

 protected:
  virtual const EveryData &everyData() const;
  virtual void setEveryData(const EveryData &everyData);

 protected:
  using DisplayRangeP     = std::unique_ptr<DisplayRange>;
  using EditHandlesP      = std::unique_ptr<EditHandles>;
  using TitleP            = std::unique_ptr<Title>;
  using AxisP             = std::unique_ptr<Axis>;
  using PlotKeyP          = std::unique_ptr<PlotKey>;
  using PropertyModelP    = std::unique_ptr<PropertyModel>;
  using ColorFilter       = std::set<Color>;
  using ColumnSet         = std::set<Column>;
  using VariantSet        = std::set<QVariant>;
  using ColumnValueFilter = std::map<Column, VariantSet>;

  View*     view_ { nullptr }; //!< parent view
  PlotType* type_ { nullptr }; //!< plot type data
  ModelP    model_;            //!< abstract model

  PropertyModelP propertyModel_; //!< property model

  bool modelNameSet_ { false }; //!< model name set from plot

  // name
  QString name_; //!< custom name

  // ranges
  BBox          viewBBox_       { 0, 0, 1, 1 }; //!< view box
  BBox          innerViewBBox_  { 0, 0, 1, 1 }; //!< inner view box
  PlotMargin    innerMargin_;                   //!< inner margin
  PlotMargin    outerMargin_;                   //!< outer margin
  DisplayRangeP displayRange_;                  //!< value range mapping
  DisplayRangeP rawDisplayRange_;               //!< value range mapping (non zoom)
  mutable bool  useRawRange_    { false };      //!< use raw display range
  Range         calcDataRange_;                 //!< calc data range
  Range         unequalDataRange_;              //!< raw data range
  Range         dataRange_;                     //!< equal range adjusted data range
  Range         outerDataRange_;                //!< outer data range
  ZoomData      zoomData_;                      //!< zoom data

  // override range
  OptReal xmin_; //!< xmin override
  OptReal ymin_; //!< ymin override
  OptReal xmax_; //!< xmax override
  OptReal ymax_; //!< ymax override

  // filter
  EveryData everyData_;                 //!< every data
  QString   filterStr_;                 //!< data filter
  QString   visibleFilterStr_;          //!< visible filter

  // bad value data
  struct BadData {
    bool   skip   { true }; //!< skip bad values
    bool   useRow { true }; //!< bad values default to row number
    double value  { 0.0 };  //!< vad value (if not use row number)
  };

  BadData badData_; //!< bad value data

  // plot border
  Sides plotBorderSides_ { "tlbr" }; //!< plot border sides
  bool  plotClip_        { true };   //!< is clipped at plot limits

  // data border
  Sides dataBorderSides_ { "tlbr" }; //!< data border sides
  bool  dataRawRange_    { false };  //!< use raw range for data box draw
  bool  dataClip_        { false };  //!< is clipped at data limits
  bool  dataRawClip_     { false };  //!< clip to raw range for data box draw

  // fit border
  Sides fitBorderSides_ { "tlbr" }; //!< fit border sides
  bool  fitClip_        { false };  //!< is clipped at fit limits

  mutable BBox clipRect_;

  // title
  TitleP  titleObj_; //!< title object
  QString titleStr_; //!< title string
  QString filename_; //!< associated data filename

  // axes
  AxisP xAxis_; //!< x axis object
  AxisP yAxis_; //!< y axis object

  using AxisSideDelta = std::map<CQChartsAxisSide::Type, double>;

  AxisSideDelta xAxisSideDelta_;
  AxisSideDelta yAxisSideDelta_;

  // key
  PlotKeyP keyObj_;                //!< key object
  bool     controlsKey_ { false }; //!< show key in custom controls
  bool     colorKey_    { false }; //!< use color column for key

  // color map key
  using ColorMapKeyP = std::unique_ptr<CQChartsColorMapKey>;
  using MapKeys      = std::vector<CQChartsMapKey *>;

  ColorMapKeyP colorMapKey_; //!< color map key

  int maxMappedValues_ { 20 }; // max number of unique values to treats as distinct

  ColorFilter colorFilter_;  //!< color map filter

  MapKeys mapKeys_; //!< all map keys

  // column filters
  ColumnSet         filterColumns_;     //!< filter columns
  ColumnValueFilter columnValueFilter_; //!< per column value filter

  // columns
  Column  xValueColumn_;    //!< x axis value column
  Column  yValueColumn_;    //!< y axis value column
  Column  idColumn_;        //!< unique data id column (signalled)
  Column  tipHeaderColumn_; //!< tip header column
  Columns tipColumns_;      //!< tip columns
  Columns noTipColumns_;    //!< no tip columns
  Column  visibleColumn_;   //!< visible column
  Column  fontColumn_;      //!< font column
  Column  imageColumn_;     //!< image column
  Columns controlColumns_;  //!< control columns

  // color data
  ColorColumnData    colorColumnData_; //!< color column data
  mutable std::mutex colorMutex_;      //!< color mutex

  AlphaColumnData alphaColumnData_; //!< alpha column data

  // cached column names
  ColumnNames columnNames_; //!< column header names

  // font
  Font   font_;                      //!< font
  double minScaleFontSize_ { 6.0 };  //!< min scaled font size
  double maxScaleFontSize_ { 48.0 }; //!< max scaled font size

  Font tabbedFont_; //!< font for tab text

  bool zoomText_ { true }; //!< zoom text

  // palette
  PaletteName defaultPalette_; //!< default palette

  // symbol data
  QString defaultSymbolSetName_ { "filled" }; //!< default symbol set name
  bool    scaleSymbolSize_      { true };     //!< scale pixel/percent symbol size

  // scaling
  bool equalScale_ { false }; //!< equal scaled

  // follow mouse
  bool followMouse_ { true }; //!< track object under mouse

  // fit
  bool         autoFit_      { false }; //!< auto fit on data change
  mutable bool inAutoFit_    { false }; //!< in auto fit
  PlotMargin   fitMargin_;              //!< fit margin
  bool         needsAutoFit_ { false }; //!< needs auto fit on next draw
  mutable BBox extraFitBBox_;           //!< cached extra fit bbox

  // preview
  bool preview_        { false }; //!< is preview plot
  int  previewMaxRows_ { 1000 };  //!< preview max rows

  bool sequential_        { false }; //!< is sequential (non-threaded)
  bool queueUpdate_       { true };  //!< is queued update
  bool bufferSymbols_     { false }; //!< buffer symbols
  bool showBoxes_         { false }; //!< show debug boxes
  bool showSelectedBoxes_ { false }; //!< show selected debug boxes
  bool overview_          { false }; //!< is overview

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

  bool showAllXOverlayAxes_ { false }; //!< show all x overlay axes
  bool showAllYOverlayAxes_ { false }; //!< show all y overlay axes

  // objects
  PlotObjs plotObjs_; //!< plot objects

  //---

  // inside data
  struct InsideData {
    bool        set     { false }; //!< is set
    Point       p;                 //!< inside point
    int         ind     { 0 };     //!< current inside object ind
    ObjPSet     objs;              //!< inside plot objects
    SizeObjPSet sizeObjs;          //!< inside plot objects by size

    bool isSet() const { return set; }

    void clear() {
      set = false;

      p   = Point();
      ind = 0;

      objs    .clear();
      sizeObjs.clear();
    }

    void setPoint(const Point &point) {
      set = true;
      p   = point;
    }

    void nextInd() {
      ++ind;

      if (ind >= numSizeObjs())
        ind = 0;
    }

    void prevInd() {
      --ind;

      if (ind < 0)
        ind = numSizeObjs() - 1;
    }

    int numSizeObjs() const {
      int n = 0;

      for (const auto &po : sizeObjs)
        n += po.second.size();

      return n;
    }
  };

  InsideData insideData_;

  //---

  using PlotObjTree  = CQChartsPlotObjTree;
  using PlotObjTreeP = std::unique_ptr<PlotObjTree>;

  //! \brief object tree data
  struct ObjTreeData {
    bool         init   { false }; //!< needs init obj tree
    PlotObjTreeP tree;             //!< plot object quad tree
    bool         isSet  { false }; //!< is plot object quad tree set
    bool         notify { false }; //!< is plot object quad tree notify
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
  EditHandlesP editHandles_;           //!< edit controls
  bool         editing_     { false }; //!< is editing

  // annotations
  Annotations annotations_;      //!< extra annotations
  Annotations pressAnnotations_; //!< press annotations

  //---

  using ControlWidgets = std::vector<CQChartsPlotControlIFace *>;

  ControlWidgets controls_;

  //---

  UpdatesData updatesData_;              //!< updates data
  bool        fromInvalidate_ { false }; //!< call from invalidate

  //---

#if 0
  using SkipColors = std::map<QVariant, Rows>;

  SkipColors skipColors_;
#endif

  //---

  // named slots
  using NamedSlots = std::map<QString, QStringList>;

  NamedSlots namedSlots_;

  //---

  //! \brief tab data
  struct TabData {
    enum class DrawType {
      TITLE,
      CIRCLES
    };

    DrawType drawType { DrawType::TITLE };
    double   pxm      { 0.0 };
    double   pym      { 0.0 };
    double   ptw      { 0.0 };
    double   pth      { 0.0 };
  };

  TabData tabData_;

  //---

  //! \brief error data
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

  mutable std::mutex resizeMutex_;  //!< resize mutex
  mutable std::mutex updatesMutex_; //!< updates enabled mutex
};

//------

// common named data
CQCHARTS_NAMED_LINE_DATA (Grid, grid)
CQCHARTS_NAMED_SHAPE_DATA(Node, node)
CQCHARTS_NAMED_SHAPE_DATA(Edge, edge)
CQCHARTS_NAMED_LINE_DATA (Edge, edge)
CQCHARTS_NAMED_TEXT_DATA (Node, node)
CQCHARTS_NAMED_TEXT_DATA (Edge, edge)
CQCHARTS_NAMED_POINT_DATA(Dot , dot )
CQCHARTS_NAMED_POINT_DATA(Rug , rug )

#endif
