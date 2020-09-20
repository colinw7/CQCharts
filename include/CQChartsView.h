#ifndef CQChartsView_H
#define CQChartsView_H

#include <CQChartsObjData.h>
#include <CQChartsGeom.h>
#include <CQChartsThemeName.h>
#include <CQChartsSymbol.h>
#include <CQChartsLineDash.h>
#include <CQChartsPosition.h>
#include <CQChartsLength.h>
#include <CQChartsRect.h>
#include <CQChartsPolygon.h>
#include <CQChartsLayer.h>
#include <CQChartsImage.h>
#include <CQChartsWidget.h>
#include <CQChartsTypes.h>
#include <QFrame>
#include <QTimer>

#include <set>
#include <future>

class CQCharts;
class CQChartsWindow;
class CQChartsPlot;
class CQChartsObj;
class CQChartsViewKey;

class CQChartsAnnotation;
class CQChartsAnnotationGroup;
class CQChartsArrowAnnotation;
class CQChartsButtonAnnotation;
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

class CQChartsProbeBand;
class CQChartsDisplayRange;
class CQChartsEditAnnotationDlg;
class CQChartsEditAxisDlg;
class CQChartsEditKeyDlg;
class CQChartsEditTitleDlg;
class CQChartsPaintDevice;
class CQChartsPoints;
class CQChartsReals;
class CQChartsDocument;
class CQChartsSplitter;

struct CQChartsTextOptions;

class CQPropertyViewModel;
class CQPropertyViewItem;
class CQColorsPalette;

class QPainter;
class QToolButton;
class QRubberBand;
class QScrollBar;
class QLabel;
class QMenu;

CQCHARTS_NAMED_SHAPE_DATA(Selected, selected)
CQCHARTS_NAMED_SHAPE_DATA(Inside, inside)

/*!
 * \brief View widget in which plots are positioned and displayed
 * \ingroup Charts
 *
 * This view widget is a container for plots and is in charge of
 * positioning and drawing them.
 *
 * It has its own title and key. The key shows the list of plots
 * and can be used to show/hide them.
 */
class CQChartsView : public QFrame,
 public CQChartsObjBackgroundFillData<CQChartsView>,
 public CQChartsObjSelectedShapeData <CQChartsView>,
 public CQChartsObjInsideShapeData   <CQChartsView> {
  Q_OBJECT

  Q_PROPERTY(QString id             READ id             WRITE setId            )
  Q_PROPERTY(QString title          READ title          WRITE setTitle         )
  Q_PROPERTY(int     currentPlotInd READ currentPlotInd WRITE setCurrentPlotInd)
  Q_PROPERTY(QSize   viewSizeHint   READ viewSizeHint   WRITE setViewSizeHint  )

  CQCHARTS_NAMED_FILL_DATA_PROPERTIES(Background, background)

  // mode
  Q_PROPERTY(Mode mode READ mode WRITE setMode)

  // select mode
  Q_PROPERTY(SelectMode selectMode   READ selectMode     WRITE setSelectMode  )
  Q_PROPERTY(bool       selectInside READ isSelectInside WRITE setSelectInside)

  // region mode
  Q_PROPERTY(RegionMode regionMode READ regionMode WRITE setRegionMode)

  // theme
  Q_PROPERTY(CQChartsThemeName theme READ themeName WRITE setThemeName)
  Q_PROPERTY(bool              dark  READ isDark    WRITE setDark     )

  // default palette
  Q_PROPERTY(QString defaultPalette READ defaultPalette WRITE setDefaultPalette)

  //---

  // selected/inside appearance
  Q_PROPERTY(HighlightDataMode selectedMode  READ selectedMode    WRITE setSelectedMode )
  Q_PROPERTY(bool              selectedBlend READ isSelectedBlend WRITE setSelectedBlend)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Selected, selected)

  Q_PROPERTY(HighlightDataMode insideMode  READ insideMode    WRITE setInsideMode )
  Q_PROPERTY(bool              insideBlend READ isInsideBlend WRITE setInsideBlend)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Inside, inside)

  //---

  // scroll (TODO remove)
  Q_PROPERTY(bool        scrolled       READ isScrolled     WRITE setScrolled      )
  Q_PROPERTY(double      scrollDelta    READ scrollDelta    WRITE setScrollDelta   )
  Q_PROPERTY(int         scrollNumPages READ scrollNumPages WRITE setScrollNumPages)
  Q_PROPERTY(int         scrollPage     READ scrollPage     WRITE setScrollPage    )

  // anti-alias, buffer, preview, pos text type
  Q_PROPERTY(bool        antiAlias      READ isAntiAlias    WRITE setAntiAlias   )
  Q_PROPERTY(bool        showTable      READ isShowTable    WRITE setShowTable   )
  Q_PROPERTY(bool        showSettings   READ isShowSettings WRITE setShowSettings)
  Q_PROPERTY(bool        bufferLayers   READ isBufferLayers WRITE setBufferLayers)
  Q_PROPERTY(bool        preview        READ isPreview      WRITE setPreview     )
  Q_PROPERTY(PosTextType posTextType    READ posTextType    WRITE setPosTextType )

  // fixed size
  Q_PROPERTY(bool  autoSize  READ isAutoSize WRITE setAutoSize )
  Q_PROPERTY(QSize fixedSize READ fixedSize  WRITE setFixedSize)

  // font scaling/factor
  Q_PROPERTY(bool         scaleFont  READ isScaleFont WRITE setScaleFont )
  Q_PROPERTY(double       fontFactor READ fontFactor  WRITE setFontFactor)
  Q_PROPERTY(CQChartsFont font       READ font        WRITE setFont      )

  // separators
  Q_PROPERTY(bool plotSeparators READ isPlotSeparators WRITE setPlotSeparators)

  // hand drawn
  Q_PROPERTY(bool   handDrawn     READ isHandDrawn   WRITE setHandDrawn    )
  Q_PROPERTY(double handRoughness READ handRoughness WRITE setHandRoughness)
  Q_PROPERTY(double handFillDelta READ handFillDelta WRITE setHandFillDelta)

  //! search timeout
  Q_PROPERTY(int searchTimeout READ searchTimeout WRITE setSearchTimeout)

  Q_ENUMS(Mode)
  Q_ENUMS(SelectMode)
  Q_ENUMS(HighlightDataMode)
  Q_ENUMS(RegionMode)
  Q_ENUMS(ThemeType)
  Q_ENUMS(PosTextType)

 public:
  using SelMod = CQChartsSelMod;

  //! mouse mode
  enum class Mode {
    SELECT,
    ZOOM_IN,
    ZOOM_OUT,
    PAN,
    PROBE,
    QUERY,
    EDIT,
    REGION
  };

  //! select mode
  enum class SelectMode {
    POINT,
    RECT
  };

  //! highlight mode
  enum class HighlightDataMode {
    OUTLINE,
    FILL
  };

  //! region mode
  enum class RegionMode {
    POINT,
    RECT
  };

  //! units of position displayed in position text
  enum class PosTextType {
    PLOT,
    VIEW,
    PIXEL
  };

  using DrawType = CQChartsObjDrawType;

  using Point   = CQChartsGeom::Point;
  using BBox    = CQChartsGeom::BBox;
  using Size    = CQChartsGeom::Size;
  using Polygon = CQChartsGeom::Polygon;

 public:
  using Plot    = CQChartsPlot;
  using Plots   = std::vector<Plot *>;
  using Objs    = std::vector<CQChartsObj *>;
  using PlotSet = std::set<Plot *>;

  using Window      = CQChartsWindow;
  using Position    = CQChartsPosition;
  using Rect        = CQChartsRect;
  using Length      = CQChartsLength;
  using Angle       = CQChartsAngle;
  using Symbol      = CQChartsSymbol;
  using Image       = CQChartsImage;
  using Widget      = CQChartsWidget;
  using Font        = CQChartsFont;
  using Color       = CQChartsColor;
  using Alpha       = CQChartsAlpha;
  using PaintDevice = CQChartsPaintDevice;
  using PenBrush    = CQChartsPenBrush;
  using PenData     = CQChartsPenData;
  using BrushData   = CQChartsBrushData;
  using ColorInd    = CQChartsUtil::ColorInd;

 public:
  static double viewportRange() { return 100.0; }

  static const QSize &defSizeHint() { return defSizeHint_; }
  static void setDefSizeHint(const QSize &s) { defSizeHint_ = s; }

  static QString description();

 public:
  CQChartsView(CQCharts *charts, QWidget *parent=nullptr);

  virtual ~CQChartsView();

  CQCharts *charts() const { return charts_; }

  //---

  Window *window() const { return window_; }
  void setWindow(Window *window);

  //---

  QPainter *ipainter() const { return ipainter_; }

  //---

  const QString &id() const { return id_; }
  void setId(const QString &s);

  const QString &title() const { return title_; }
  void setTitle(const QString &s);

  //---

  int currentPlotInd() const { return currentPlotInd_; }
  void setCurrentPlotInd(int i);

  //---

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &m);

  //---

  const SelectMode &selectMode() const { return selectData_.mode; }
  void setSelectMode(const SelectMode &m);

  bool isSelectInside() const { return selectData_.inside; }
  void setSelectInside(bool b) { selectData_.inside = b; }

  //---

  const RegionMode &regionMode() const { return regionData_.mode; }
  void setRegionMode(const RegionMode &m);

  //---

  void setCurrentPlot(Plot *plot);

  //---

  // selected/inside appearance
  const HighlightDataMode &selectedMode() const { return selectedHighlight_.mode; }
  void setSelectedMode(const HighlightDataMode &mode) { selectedHighlight_.mode = mode; }

  bool isSelectedBlend() const { return selectedHighlight_.blend; }
  void setSelectedBlend(bool b) { selectedHighlight_.blend = b; }

  const HighlightDataMode &insideMode() const { return insideHighlight_.mode; }
  void setInsideMode(const HighlightDataMode &mode) { insideHighlight_.mode = mode; }

  bool isInsideBlend() const { return insideHighlight_.blend; }
  void setInsideBlend(bool b) { insideHighlight_.blend = b; }

  //---

  void maximizePlot(Plot *plot);

  //---

  void updateSeparators();

  //---

  bool isScrolled() const { return scrollData_.active; }
  void setScrolled(bool b, bool update=true);

  double scrollDelta() const { return scrollData_.delta; }
  void setScrollDelta(double r) { scrollData_.delta = r; }

  int scrollNumPages() const { return scrollData_.numPages; }
  void setScrollNumPages(int i) { scrollData_.numPages = i; }

  int scrollPage() const { return scrollData_.page; }
  void setScrollPage(int i) { scrollData_.page = i; }

  //---

  bool isAntiAlias() const { return antiAlias_; }
  void setAntiAlias(bool b);

  bool isShowTable() const;
  bool isShowSettings() const;

  bool isBufferLayers() const { return bufferLayers_; }
  void setBufferLayers(bool b);

  bool isPreview() const { return preview_; }
  void setPreview(bool b);

  //---

  // auto/fixed size
  bool isAutoSize() const { return sizeData_.autoSize; }

  QSize fixedSize() const { return QSize(sizeData_.width, sizeData_.height); }
  void setFixedSize(const QSize &size);

  //---

  // font scaling/factor
  bool isScaleFont() const { return scaleFont_; }
  void setScaleFont(bool b);

  double fontFactor() const { return fontFactor_; }
  void setFontFactor(double r);

  const Font &font() const { return font_; }
  void setFont(const Font &f);

  double fontEm() const;
  double fontEx() const;

  //---

  bool isPlotSeparators() const { return plotSeparators_; }
  void setPlotSeparators(bool b);

  //---

  bool isHandDrawn() const { return handDrawn_; }
  void setHandDrawn(bool b);

  double handRoughness() const { return handRoughness_; }
  void setHandRoughness(double r);

  double handFillDelta() const { return handFillDelta_; }
  void setHandFillDelta(double r);

  //---

  int searchTimeout() const { return searchTimeout_; }
  void setSearchTimeout(int i);

  //---

  void setPainterFont(PaintDevice *device, const Font &font) const;

  void setPlotPainterFont(const Plot *plot, PaintDevice *device, const Font &font) const;

  QFont viewFont(const Font &font) const;

  QFont plotFont(const Plot *plot, const Font &font, bool scaled=true) const;

  double calcFontScale(const Size &size) const;

  //---

  void invalidateObjects();
  void invalidateOverlay();

  //---

  void expandedModelIndices(QModelIndexList &inds);

 private:
//QFont viewFont(const QFont &font) const;

//QFont plotFont(const Plot *plot, const QFont &font, bool scaled=true) const;

  QFont scaledFont(const QFont &font, const Size &size) const;
  QFont scaledFont(const QFont &font, double s) const;

  //---

 public:
  // position text type
  const PosTextType &posTextType() const { return posTextType_; }
  void setPosTextType(const PosTextType &t);

  //---

  // theme
  CQColorsTheme *theme();
  const CQColorsTheme *theme() const;

  const CQChartsThemeName &themeName() const;
  void setThemeName(const CQChartsThemeName &name);

  //---

  // palettes
  CQColorsPalette *interfacePalette() const;

  CQColorsPalette *themeGroupPalette(int i, int n) const;

  CQColorsPalette *themePalette(int ind=0) const;

  // is dark
  bool isDark() const;
  void setDark(bool b);

  // default palette
  const QString &defaultPalette() const { return defaultPalette_; }
  void setDefaultPalette(const QString &s);

  //---

  // property model
  CQPropertyViewModel *propertyModel() const { return propertyModel_; }

  //---

  // get/set properties
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

  CQPropertyViewItem *addProperty(const QString &path, QObject *object,
                                  const QString &name, const QString &alias="");

  virtual void getPropertyNames(QStringList &names, bool hidden=false) const;

  void hideProperty(const QString &path, QObject *object);

  const CQPropertyViewItem *propertyItem(const QString &name, bool hidden) const;

  //---

  // view key
  CQChartsViewKey *key() const { return keyObj_; }

  //---

  // add/get/modify plots
  void addPlot(Plot *plot, const BBox &bbox=BBox());

  int numPlots() const { return plots_.size(); }

  const Plots &plots() const { return plots_; }
  Plot *plot(int i) { assert(i >= 0 && i < int(plots_.size())); return plots_[i]; }

  Plot *getPlotForId(const QString &id) const;

  void getDrawPlots(Plots &plots) const;
  void getPlots(Plots &plots) const;

  void raisePlot(Plot *plot);
  void lowerPlot(Plot *plot);

  void removePlot(Plot *plot);

  void removeAllPlots();

  //---

  // select
  void selectOneObj(CQChartsObj *obj);

  void deselectAll();

  void startSelection();
  void endSelection();

  //---

  // annotations
  using Annotation          = CQChartsAnnotation;
  using Annotations         = std::vector<Annotation *>;
  using AnnotationGroup     = CQChartsAnnotationGroup;
  using ArrowAnnotation     = CQChartsArrowAnnotation;
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
  using Buffer              = CQChartsBuffer;

  const Annotations &annotations() const { return annotations_; }

  AnnotationGroup     *addAnnotationGroup    ();
  ArrowAnnotation     *addArrowAnnotation    (const Position &start, const Position &end);
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

  Annotation *getAnnotationByName(const QString &id) const;
  Annotation *getAnnotationByInd(int ind) const;

  void removeAnnotation(Annotation *annotation);

  void removeAllAnnotations();

  //---

  // group plots (overlay, x1/x2, y1/y2)
  void resetGrouping();

  void resetPlotGrouping();
  void resetPlotGrouping(const Plots &plots);

  void initOverlay(const Plots &plots, bool reset=false);

  void initX1X2(const Plots &plots, bool overlay, bool reset=false);
  void initY1Y2(const Plots &plots, bool overlay, bool reset=false);

  void initTabbed(const Plots &plots, bool reset=false);

  //---

  // place plots
  void autoPlacePlots();

  void placePlots(const Plots &plots, bool vertical, bool horizontal,
                  int rows, int columns, bool reset=false);

  //---

 public:
  // interp palette/theme color
  QColor interpPaletteColor(const ColorInd &ind, bool scale=false) const;

  QColor interpGroupPaletteColor(const ColorInd &ig, const ColorInd &iv, bool scale) const;

  QColor interpThemeColor(const ColorInd &ind) const;

  QColor interpInterfaceColor(double r) const;

  QColor interpColor(const Color &c, const ColorInd &ind) const;

  //---

 public:
  // Qt event handling
  void mousePressEvent  (QMouseEvent *me) override;
  void mouseMoveEvent   (QMouseEvent *me) override;
  void mouseReleaseEvent(QMouseEvent *me) override;

  bool editMousePress  ();
  bool editMouseMove   ();
  void editMouseMotion ();
  void editMouseRelease();

  void keyPressEvent(QKeyEvent *ke) override;

  void wheelEvent(QWheelEvent *e) override;

  Point adjustMousePos(const Point &pos) const;

  //---

  // handle resize
  void resizeEvent(QResizeEvent *) override;

  void doResize(int w, int h);

  //---

  // handle paint
  void paintEvent(QPaintEvent *) override;

  void paint(QPainter *painter, Plot *plot=nullptr);

  void drawBackground(PaintDevice *device) const;

  void drawPlots(QPainter *painter);

  void showNoData(bool show);

  void drawNoData(PaintDevice *device);

  bool hasAnnotations() const;
  void drawAnnotations(PaintDevice *device, const CQChartsLayer::Type &layerType);

  void drawKey(PaintDevice *device, const CQChartsLayer::Type &layerType);

  void lockPainter(bool lock);

  //---

  // get buffers
  Buffer *objectsBuffer() const { return objectsBuffer_; }
  Buffer *overlayBuffer() const { return overlayBuffer_; }

  // get/set draw layer type
  const CQChartsLayer::Type &drawLayerType() const { return drawLayerType_; }
  void setDrawLayerType(const CQChartsLayer::Type &t) { drawLayerType_ = t; }

  //---

  // print to PNG/SVG
  bool printPNG(const QString &filename, Plot *plot=nullptr);
  bool printSVG(const QString &filename, Plot *plot=nullptr);
  bool writeSVG(const QString &filename, Plot *plot=nullptr);

  // write javascript
  bool writeScript(const QString &filename, Plot *plot=nullptr);

  const QString &scriptSelectProc() const { return scriptSelectProc_; }
  void setScriptSelectProc(const QString &s) { scriptSelectProc_ = s; }

  //---

  // set pen/brush
  void setPenBrush(PenBrush &penBrush, const PenData &penData, const BrushData &brushData) const;

  void setPen(PenBrush &penBrush, const PenData &penData) const;

  void setBrush(PenBrush &penBrush, const BrushData &brushData) const;

  //---

  void updateObjPenBrushState(const CQChartsObj *obj, PenBrush &penBrush,
                              DrawType drawType=DrawType::BOX) const;

  void updateObjPenBrushState(const CQChartsObj *obj, const ColorInd &ic,
                              PenBrush &penBrush, DrawType drawType) const;

  void updateInsideObjPenBrushState  (const ColorInd &ic, PenBrush &penBrush,
                                      bool outline, DrawType drawType) const;
  void updateSelectedObjPenBrushState(const ColorInd &ic, PenBrush &penBrush,
                                      DrawType drawType) const;

  QColor insideColor  (const QColor &c) const;
  QColor selectedColor(const QColor &c) const;

  //---

  // cycle and edit
  void cycleEdit();

  void editObjs(Objs &objs);

  //---

  // probe lines
  void showProbeLines(const Point &p);

  //---

  // update cursor position text
  void updatePosText(const Point &pos);

  //---

  // handle region rubberband
  void startRegionBand(const Point &pos);
  void updateRegionBand(Plot *plot, const Point &pressPoint, const Point &movePoint);
  void updateRegionBand(const Point &pressPoint, const Point &movePoint);
  void endRegionBand();

  //---

  // update selected text
  void updateSelText();

  //---

  // get selected plots
  void selectedPlots(Plots &plots) const;

  // get selected objects
  void selectedObjs(Objs &objs) const;

  // get all selected objects
  void allSelectedObjs(Objs &objs) const;

  //---

  // show context menu (override for custom menu)
  virtual void showMenu(const Point &p);

  // create new menu (lifetime handled by view)
  virtual QMenu *createPopupMenu();

  // get menu position
  Point menuPos() const { return mousePressPoint(); }

  //---

  // mouse data
  const Plots &mousePlots() const { return mouseData_.plots; }
  const Plot *mousePlot() const { return mouseData_.plot; }

  Point mousePressPoint() const { return mouseData_.pressPoint; }
  Point mouseMovePoint () const { return mouseData_.movePoint; }

  bool mousePressed() const { return mouseData_.pressed; }
  int  mouseButton () const { return mouseData_.button; }

  SelMod mouseSelMod  () const { return mouseData_.selMod; }
  SelMod mouseClickMod() const { return mouseData_.clickMod; }

  //---

  // add plots to array
  bool addPlots(Plots &plots, bool clear=true) const;

  // get base plot for plot
  Plot *basePlot(Plot *plot) const;

  // get all base plots
  bool addBasePlots(PlotSet &plots, bool clear=true) const;

  // get plots at point
  Plot *plotAt(const Point &p) const;

  bool plotsAt(const Point &p, Plots &plots, Plot* &plot,
               bool clear=true, bool first=false) const;

  bool plotsAt(const Point &p, Plots &plots, bool clear=true) const;

  bool basePlotsAt(const Point &p, PlotSet &plots, bool clear=true) const;

  // get plot bbox
  BBox plotBBox(Plot *plot) const;

  //---

  void initOverlayAxes();

  //---

  // convert plot to/from index
  Plot *getPlotForInd(int ind) const;
  int getIndForPlot(const Plot *plot) const;

  // get current plot
  Plot *currentPlot(bool remap=true) const;

  int calcCurrentPlotInd(bool remap=true) const;

  //---

  // virtual to handle status text display
  virtual void setPosText(const QString &text);

  virtual void setStatusText(const QString &text);

  virtual void setSelText(const QString &text);

  //---

  // scroll left/right
  void scrollLeft();
  void scrollRight();

  void updateScroll();

  //---

  // update palette/theme colors
  void setPaletteColors1();
  void setPaletteColors2();

  void setLightThemeColors();
  void setDarkThemeColors();

  void updateTheme();

  //---

  // write all details to output (model, view, plots and annotations)
  void writeAll(std::ostream &os) const;

  // write view details to output
  void write(std::ostream &os) const;

  //---

  // position, rect, length coordinate conversions
  Point positionToView (const Position &pos) const;
  Point positionToPixel(const Position &pos) const;

  // rect
  BBox rectToView (const Rect &rect) const;
  BBox rectToPixel(const Rect &rect) const;

  double lengthViewWidth (const Length &len) const;
  double lengthViewHeight(const Length &len) const;

  double lengthPixelWidth (const Length &len) const;
  double lengthPixelHeight(const Length &len) const;

  //---

  // coordinate conversions
  Point windowToPixel(const Point &w) const;
  Point pixelToWindow(const Point &p) const;

  BBox windowToPixel(const BBox &w) const;
  BBox pixelToWindow(const BBox &p) const;

  double pixelToSignedWindowWidth (double ww) const;
  double pixelToSignedWindowHeight(double wh) const;

  double pixelToWindowWidth (double pw) const;
  double pixelToWindowHeight(double ph) const;

  double windowToSignedPixelWidth (double ww) const;
  double windowToSignedPixelHeight(double wh) const;

  double windowToPixelWidth (double ww) const;
  double windowToPixelHeight(double wh) const;

  Size pixelToWindowSize(const Size &ps) const;

  Polygon windowToPixel(const Polygon &p) const;

  QPainterPath windowToPixel(const QPainterPath &p) const;

  //---

  // get pixel rect
  const BBox prect() const { return prect_; }

  // get aspect
  double aspect() const { return aspect_; }

  //---

  // get/set view size hint
  const QSize &viewSizeHint() const { return viewSizeHint_; }
  void setViewSizeHint(const QSize &s) { viewSizeHint_ = s; }

  QSize sizeHint() const override;

 signals:
  // emitted when user asks to show table
  //void showTable(bool b);

  // emitted when current mode changed
  void modeChanged();

  // emitted when selection mode changed
  void selectModeChanged();

  // emitted when selection mode changed
  void regionModeChanged();

  // emitted when interface or theme palettes changed
  void interfacePaletteChanged();
  void themePalettesChanged();

  // emitted when status text changes
  void statusTextChanged(const QString &);

  // emitted when position text changes
  void posTextChanged(const QString &);

  // emitted when selection text changed
  void selTextChanged(const QString &);

  // emitted when current plot changed
  void currentPlotChanged();

  // emitted when plot added
  void plotAdded(CQChartsPlot *);
  void plotAdded(const QString &);

  // emitted when plot removed
  void plotRemoved(const QString &);
  // emitted when all plots removed
  void allPlotsRemoved();

  // emitted when plots changed
  void plotsChanged();

  // emitted when plots reordered
  void plotsReordered();

  // emitted when plot connect data changed
  void plotConnectDataChanged(const QString &);
  void connectDataChanged();

  // emitted when key pressed
  void keyPressed(CQChartsViewKey *);
  void keyIdPressed(const QString &);

  // emitted when annotation pressed
  void annotationPressed(CQChartsAnnotation *);
  void annotationIdPressed(const QString &);

  // emitted when annotations changed
  void annotationsChanged();

  // emitted when selection changed
  void selectionChanged();

  // emitted when scrolled data changed
  void scrollDataChanged();

  // region point mouse release
  void regionPointRelease(const CQChartsGeom::Point &);
  void regionRectRelease (const CQChartsGeom::BBox &);

  // emitted when errors added
  void updateErrors();

 public slots:
  void updateNoData();

  void editObjectSlot();

  void expansionChangeSlot();

  void setAutoSize(bool b);

  void selectModeSlot();
  void zoomModeSlot();
  void panModeSlot();
  void probeModeSlot();
  void queryModeSlot();
  void editModeSlot();
  void regionModeSlot();

  void plotModelChanged();
  void plotConnectDataChangedSlot();

  void plotViewBoxChanged();

  void updateAll();
  void updatePlots();

  void updateSlot();

  void searchSlot();

  void themeChangedSlot(const QString &);
  void paletteChangedSlot(const QString &);

  void maximizePlotsSlot();
  void restorePlotsSlot();

  void viewKeyVisibleSlot(bool b);
  void viewKeyPositionSlot(QAction *action);

  void plotKeyVisibleSlot(bool b);
  void plotKeyPositionSlot(QAction *action);
  void plotKeyInsideXSlot(bool b);
  void plotKeyInsideYSlot(bool b);

  void xAxisVisibleSlot(bool b);
  void xAxisGridSlot(bool b);
  void xAxisSideSlot(QAction *action);

  void yAxisVisibleSlot(bool b);
  void yAxisGridSlot(bool b);
  void yAxisSideSlot(QAction *action);

  void titleVisibleSlot(bool b);
  void titleLocationSlot(QAction *action);

  void invertXSlot(bool);
  void invertYSlot(bool);

  //---

  void noDataTextClicked(const QString &);

  void manageModelsSlot();
  void addPlotSlot();
  void helpSlot();

  //---

  void nextSlot();
  void prevSlot();

  //---

  void fitSlot();
  void zoomFullSlot();

  //---

  void lightPaletteSlot();
  void darkPaletteSlot();

  void themeNameSlot();

  void themeSlot(const QString &name);

  //---

  bool printFile(const QString &filename, Plot *plot=nullptr);

  void printPNGSlot();
  void printPNGSlot(const QString &filename);

  void printSVGSlot();
  void printSVGSlot(const QString &filename);

  void writeSVGSlot();
  void writeSVGSlot(const QString &filename);

  void writeScriptSlot();
  void writeScriptSlot(const QString &filename);

  //---

  void showBoxesSlot(bool b);

  void bufferLayersSlot(bool b);

  //---

  void xRangeMapSlot(bool b);
  void yRangeMapSlot(bool b);

  void setShowTable(bool b);
  void setShowSettings(bool b);

  //---

  void resizeToView();

  //---

  void currentPlotSlot();

 private slots:
  void hbarScrollSlot(int pos);
  void vbarScrollSlot(int pos);

  void currentPlotZoomPanChanged();

  void updateAnnotationSlot();

 private:
  void addProperties();

  void selectMousePress();
  void selectMouseMotion();
  void selectMouseMove();
  void selectMouseRelease();

  void selectPointPress();

  bool isRectSelectMode() const;
  bool isPointSelectMode() const;

  void regionMousePress();
  void regionMouseMotion();
  void regionMouseMove();
  void regionMouseRelease();

  void zoomMousePress();
  void zoomMouseMove();
  void zoomMouseRelease();

  void panMouseMove();

  void resetConnections(bool notify);
  void resetConnections(const Plots &plots, bool notify);

  void initOverlayPlot(Plot *firstPlot);

  int plotPos(Plot *plot) const;

  void annotationsAtPoint(const Point &w, Annotations &annotations) const;

  void windowToPixelI(double wx, double wy, double &px, double &py) const;
  void pixelToWindowI(double px, double py, double &wx, double &wy) const;

 private:
  //! process all mouse point plots using lambda
  template<typename FUNCTION, typename DATA=int>
  bool processMouseDataPlots(FUNCTION f, const DATA &data=DATA()) const {
    if (mousePlot()) {
      if (f(mouseData_.plot, data))
        return true;
    }

    for (auto &plot : mousePlots()) {
      if (plot == mousePlot()) continue;

      if (f(plot, data))
        return true;
    }

    return false;
  }

 private:
  //! drag object
  enum class DragObj {
    NONE,
    KEY,
    TITLE,
    ANNOTATION
  };

  using ResizeSide = CQChartsResizeSide;

  //! structure for mouse interaction data
  struct MouseData {
    Plots      plots;                           //!< plots at mouse point
    Plot*      plot       { nullptr };          //!< plot at mouse point
    Point      pressPoint { 0, 0 };             //!< press point
    Point      oldMovePoint;                    //!< previous move point
    Point      movePoint  { 0, 0 };             //!< move point
    bool       pressed    { false };            //!< is pressed
    bool       escape     { false };            //!< escape pressed
    int        button     { Qt::NoButton };     //!< press button
    SelMod     selMod     { SelMod::REPLACE };  //!< selection modifier
    SelMod     clickMod   { SelMod::REPLACE };  //!< click modifier
    DragObj    dragObj    { DragObj::NONE };    //!< drag object
    ResizeSide dragSide   { ResizeSide::NONE }; //!< drag side
    bool       dragged    { false };            //!< is dragged

    void reset() {
      plots.clear();

      plot     = nullptr;
      pressed  = false;
      escape   = false;
      button   = Qt::NoButton;
      selMod   = SelMod::REPLACE;
      clickMod = SelMod::REPLACE;
      dragSide = ResizeSide::NONE;
      dragged  = false;
    }
  };

  //! structure containing the select mode
  struct SelectData {
    SelectMode mode        { SelectMode::RECT }; //!< selection sub mode
    bool       inside      { true };             //!< select inside/touching
    int        selectDepth { 0 };                //!< selecting depth
  };

  //! structure containing the highlight mode
  struct HighlightData {
    HighlightDataMode mode  { HighlightDataMode::OUTLINE }; //!< highlight mode
    bool              blend { false };                      //!< blend
  };

  //! structure containing the region mode
  struct RegionData {
    RegionMode mode { RegionMode::RECT }; //!< region sub mode
    Size       size { 1, 1 };             //!< point region size
  };

  //! structure containing the auto/fixed size data and associated scroll bars
  struct SizeData {
    bool        autoSize { true };    //!< is auto sized
    bool        sizeSet  { false };   //!< has specific size been set
    int         width    { 800 };     //!< specified width
    int         height   { 800 };     //!< specified height
    QScrollBar* hbar     { nullptr }; //!< horizontal scroll bar
    QScrollBar* vbar     { nullptr }; //!< vertical scroll bar
    int         xpos     { 0 };       //!< horizontal scroll bar position
    int         ypos     { 0 };       //!< vertical scroll bar position
  };

  //! structure containing the data for scrolled plots
  struct ScrollData {
    using PlotBBox = std::map<QString, BBox>;

    bool     active      { false }; //!< active
    double   delta       { 100 };   //!< delta percent
    int      numPages    { 1 };     //!< num pages
    int      page        { 0 };     //!< current page
    bool     autoInit    { true };  //!< auto init plot placement on activate
    PlotBBox plotBBoxMap;           //!< saved plot bbox
  };

  class RegionBand {
   public:
    RegionBand() = default;

    const BBox &bbox() const { return bbox_; }

    void init(QWidget *parent) {
      if (! rubberBand_)
        rubberBand_ = new QRubberBand(QRubberBand::Rectangle, parent);
    }

    void setGeometry(const BBox &bbox) {
      assert(rubberBand_);

      bbox_ = bbox;

      rubberBand_->setGeometry(bbox_.qrecti());
    }

    void show() {
      assert(rubberBand_);

      rubberBand_->show();
    }

    void hide() {
      if (rubberBand_)
        rubberBand_->hide();
    }

    double x() const { assert(rubberBand_); return rubberBand_->x(); } // TODO: use bbox
    double y() const { assert(rubberBand_); return rubberBand_->y(); } // TODO: use bbox

    double width () const { assert(rubberBand_); return rubberBand_->width (); } // TODO: use bbox
    double height() const { assert(rubberBand_); return rubberBand_->height(); } // TODO: use bbox

   private:
    BBox         bbox_;
    QRubberBand* rubberBand_ { nullptr };
  };

  using DisplayRange  = CQChartsDisplayRange;
  using PropertyModel = CQPropertyViewModel;

  using EditAnnotationDlg = CQChartsEditAnnotationDlg;
  using EditAxisDlg       = CQChartsEditAxisDlg;
  using EditKeyDlg        = CQChartsEditKeyDlg;
  using EditTitleDlg      = CQChartsEditTitleDlg;

  using ProbeBands = std::vector<CQChartsProbeBand*>;

  using LayerType = CQChartsLayer::Type;

  using Separators = std::vector<CQChartsSplitter *>;

  static QSize defSizeHint_;

  CQCharts*          charts_            { nullptr };           //!< parent charts
  Window*            window_            { nullptr };           //!< parent window
  QImage*            image_             { nullptr };           //!< image buffer
  QPainter*          ipainter_          { nullptr };           //!< image painter
  DisplayRange*      displayRange_      { nullptr };           //!< display range
  PropertyModel*     propertyModel_     { nullptr };           //!< property model
  QString            id_;                                      //!< view id
  QString            title_;                                   //!< view title
  CQChartsViewKey*   keyObj_            { nullptr };           //!< key object
  Plots              plots_;                                   //!< child plots
  int                currentPlotInd_    { -1 };                //!< current plot index
  Annotations        annotations_;                             //!< annotations
  Mode               mode_              { Mode::SELECT };      //!< mouse mode
  SelectData         selectData_;                              //!< select sub mode data
  HighlightData      selectedHighlight_;                       //!< select highlight
  HighlightData      insideHighlight_;                         //!< inside highlight
  RegionData         regionData_;                              //!< region sub mode
  QString            defaultPalette_;                          //!< default palette
  ScrollData         scrollData_;                              //!< scroll data
  bool               antiAlias_         { true };              //!< anti alias
//bool               showTable_         { false };             //!< show table with plot
  bool               bufferLayers_      { true };              //!< buffer draw layers
  bool               preview_           { false };             //!< preview
  bool               scaleFont_         { true };              //!< auto scale font
  double             fontFactor_        { 1.0 };               //!< font scale factor
  Font               font_;                                    //!< font
  Font               saveFont_;                                //!< saved font
  bool               plotSeparators_    { false };             //!< show plot separators
  bool               handDrawn_         { false };             //!< is handdrawn
  double             handRoughness_     { 1.0 };               //!< handdrawn roughness
  double             handFillDelta_     { 16 };                //!< handdrawn fill delta
  SizeData           sizeData_;                                //!< size control
  PosTextType        posTextType_       { PosTextType::PLOT }; //!< position text type
  BBox               prect_             { 0, 0, 100, 100 };    //!< plot rect
  double             aspect_            { 1.0 };               //!< current aspect
  MouseData          mouseData_;                               //!< mouse data
  int                searchTimeout_     { 10 };                //!< search timeout
  QTimer*            searchTimer_       { nullptr };           //!< search timer
  Point              searchPos_;                               //!< search pos
  RegionBand         regionBand_;                              //!< zoom region rubberband
  ProbeBands         probeBands_;                              //!< probe lines
  QMenu*             popupMenu_         { nullptr };           //!< context menu
  QSize              viewSizeHint_;                            //!< view size hint
  Buffer*            objectsBuffer_     { nullptr };           //!< buffer for view objects
  Buffer*            overlayBuffer_     { nullptr };           //!< buffer for view overlays
  LayerType          drawLayerType_     { LayerType::NONE };   //!< current draw layer type
  mutable std::mutex painterMutex_;                            //!< painter mutex
  EditAnnotationDlg* editAnnotationDlg_ { nullptr };           //!< edit annotation dialog
  EditAxisDlg*       editAxisDlg_       { nullptr };           //!< edit axis dialog
  EditKeyDlg*        editKeyDlg_        { nullptr };           //!< edit key dialog
  EditTitleDlg*      editTitleDlg_      { nullptr };           //!< edit title dialog
  QString            scriptSelectProc_;                        //!< script select proc
  Annotations        pressAnnotations_;                        //!< press annotations
  CQChartsDocument*  noDataText_        { nullptr };           //!< no data text
  bool               updateNoData_      { true };              //!< no data needs update
  Separators         separators_;
  bool               separatorsInvalid_ { true };
  bool               plotsHorizontal_   { false };
  bool               plotsVertical_     { false };
};

//------

/*!
 * \brief View Plot Splitter to allow interactive resize
 * \ingroup Charts
 */
class CQChartsSplitter : public QFrame {
  Q_OBJECT

 public:
  using View = CQChartsView;
  using Plot = CQChartsPlot;

 public:
  CQChartsSplitter(View *view, Qt::Orientation orientation);

  const Qt::Orientation &orientation() const { return orientation_; }
  void setOrientation(const Qt::Orientation &o);

  void setPlot1(Plot *plot) { plot1_ = plot; }
  void setPlot2(Plot *plot) { plot2_ = plot; }

  void paintEvent(QPaintEvent *) override;

  void mousePressEvent  (QMouseEvent *event) override;
  void mouseMoveEvent   (QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

  bool event(QEvent *event) override;

  QSize sizeHint() const override {
    return QSize(8, 8);
  }

 private:
  View*           view_        { nullptr };
  Qt::Orientation orientation_ { Qt::Vertical };
  Plot*           plot1_       { nullptr };
  Plot*           plot2_       { nullptr };
  bool            pressed_     { false };
  QPoint          pressPos_;
  QPoint          movePos_;
  QPoint          initPos_;
  bool            hover_       { false };
};

#endif
