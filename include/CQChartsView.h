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
class CQChartsArrowAnnotation;
class CQChartsEllipseAnnotation;
class CQChartsImageAnnotation;
class CQChartsPointAnnotation;
class CQChartsPolygonAnnotation;
class CQChartsPolylineAnnotation;
class CQChartsRectangleAnnotation;
class CQChartsTextAnnotation;
class CQChartsProbeBand;
class CQChartsDisplayRange;
class CQChartsEditAnnotationDlg;
class CQChartsEditAxisDlg;
class CQChartsEditKeyDlg;
class CQChartsEditTitleDlg;
class CQChartsPaintDevice;
class CQPropertyViewModel;
class CQPropertyViewItem;
class CQColorsPalette;

struct CQChartsTextOptions;

class QPainter;
class QToolButton;
class QRubberBand;
class QScrollBar;
class QLabel;
class QMenu;

CQCHARTS_NAMED_SHAPE_DATA(Selected,selected)
CQCHARTS_NAMED_SHAPE_DATA(Inside,inside)

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

  CQCHARTS_NAMED_FILL_DATA_PROPERTIES(Background,background)

  // mode
  Q_PROPERTY(Mode mode READ mode WRITE setMode)

  // select mode
  Q_PROPERTY(SelectMode selectMode   READ selectMode     WRITE setSelectMode  )
  Q_PROPERTY(bool       selectInside READ isSelectInside WRITE setSelectInside)

  // theme
  Q_PROPERTY(CQChartsThemeName theme READ themeName WRITE setThemeName)
  Q_PROPERTY(bool              dark  READ isDark    WRITE setDark     )

  // default palette
  Q_PROPERTY(QString defaultPalette READ defaultPalette WRITE setDefaultPalette)

  // selection appearance
  Q_PROPERTY(HighlightDataMode selectedMode READ selectedMode WRITE setSelectedMode)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Selected,selected)

  // inside appearance
  Q_PROPERTY(HighlightDataMode insideMode READ insideMode WRITE setInsideMode)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Inside,inside)

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

  Q_ENUMS(Mode)
  Q_ENUMS(SelectMode)
  Q_ENUMS(HighlightDataMode)
  Q_ENUMS(ThemeType)
  Q_ENUMS(PosTextType)

 public:
  using SelMod = CQChartsSelMod;

  //! mouse mode
  enum class Mode {
    SELECT,
    ZOOM,
    PAN,
    PROBE,
    QUERY,
    EDIT
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

  //! units of position displayed in position text
  enum class PosTextType {
    PLOT,
    VIEW,
    PIXEL
  };

  using DrawType = CQChartsObjDrawType;

 public:
  using Plots       = std::vector<CQChartsPlot*>;
  using Objs        = std::vector<CQChartsObj*>;
  using PlotSet     = std::set<CQChartsPlot*>;
  using Annotations = std::vector<CQChartsAnnotation *>;

  using ColorInd = CQChartsUtil::ColorInd;

 public:
  static double viewportRange() { return 100.0; }

  static const QSize &defSizeHint() { return defSizeHint_; }
  static void setDefSizeHint(const QSize &s) { defSizeHint_ = s; }

  static QString description();

 public:
  CQChartsView(CQCharts *charts, QWidget *parent=nullptr);

  virtual ~CQChartsView();

  CQCharts *charts() const { return charts_; }

  //--

  CQChartsWindow *window() const { return window_; }
  void setWindow(CQChartsWindow *window) { window_ = window; }

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

  const SelectMode &selectMode() const { return selectMode_; }
  void setSelectMode(const SelectMode &m);

  bool isSelectInside() const { return selectInside_; }
  void setSelectInside(bool b) { selectInside_ = b; }

  //---

  void setCurrentPlot(CQChartsPlot *plot);

  //---

  const HighlightDataMode &selectedMode() const { return selectedHighlight_.mode; }
  void setSelectedMode(const HighlightDataMode &mode) { selectedHighlight_.mode = mode; }

  const HighlightDataMode &insideMode() const { return insideHighlight_.mode; }
  void setInsideMode(const HighlightDataMode &mode) { insideHighlight_.mode = mode; }

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

  const CQChartsFont &font() const { return font_; }
  void setFont(const CQChartsFont &f);

  //---

  void setPainterFont(CQChartsPaintDevice *device, const CQChartsFont &font) const;

  void setPlotPainterFont(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                          const CQChartsFont &font) const;

  QFont viewFont(const CQChartsFont &font) const;

  QFont plotFont(const CQChartsPlot *plot, const CQChartsFont &font) const;

  double calcFontScale(const QSizeF &size) const;

  //---

  void invalidateObjects();
  void invalidateOverlay();

 private:
  QFont viewFont(const QFont &font) const;

  QFont plotFont(const CQChartsPlot *plot, const QFont &font) const;

  QFont scaledFont(const QFont &font, const QSizeF &size) const;
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

  //---

  // view key
  CQChartsViewKey *key() const { return keyObj_; }

  //---

  // add/get/modify plots
  void addPlot(CQChartsPlot *plot, const CQChartsGeom::BBox &bbox=CQChartsGeom::BBox());

  int numPlots() const { return plots_.size(); }

  CQChartsPlot *plot(int i) { assert(i >= 0 && i < int(plots_.size())); return plots_[i]; }

  CQChartsPlot *getPlot(const QString &id) const;

  void getPlots(Plots &plots) const;

  void raisePlot(CQChartsPlot *plot);
  void lowerPlot(CQChartsPlot *plot);

  void removePlot(CQChartsPlot *plot);

  void removeAllPlots();

  //---

  // select
  void selectOneObj(CQChartsObj *obj);

  void deselectAll();

  void startSelection();
  void endSelection();

  //---

  // annotations
  const Annotations &annotations() const { return annotations_; }

  CQChartsArrowAnnotation     *addArrowAnnotation    (const CQChartsPosition &start,
                                                      const CQChartsPosition &end);
  CQChartsEllipseAnnotation   *addEllipseAnnotation  (const CQChartsPosition &center,
                                                      const CQChartsLength &xRadius,
                                                      const CQChartsLength &yRadius);
  CQChartsImageAnnotation     *addImageAnnotation    (const CQChartsPosition &pos,
                                                      const QImage &image);
  CQChartsImageAnnotation     *addImageAnnotation    (const CQChartsRect &rect,
                                                      const QImage &image);
  CQChartsPointAnnotation     *addPointAnnotation    (const CQChartsPosition &pos,
                                                      const CQChartsSymbol &type);
  CQChartsPolygonAnnotation   *addPolygonAnnotation  (const CQChartsPolygon &polygon);
  CQChartsPolylineAnnotation  *addPolylineAnnotation (const CQChartsPolygon &polygon);
  CQChartsRectangleAnnotation *addRectangleAnnotation(const CQChartsRect &rect);
  CQChartsTextAnnotation      *addTextAnnotation     (const CQChartsPosition &pos,
                                                      const QString &text);
  CQChartsTextAnnotation      *addTextAnnotation     (const CQChartsRect &rect,
                                                      const QString &text);

  void addAnnotation(CQChartsAnnotation *annotation);

  CQChartsAnnotation *getAnnotationByName(const QString &id) const;

  void removeAnnotation(CQChartsAnnotation *annotation);

  void removeAllAnnotations();

  //---

  // group plots (overlay, x1/x2, y1/y2)
  void resetGrouping();

  void initOverlay(const Plots &plots, bool reset=false);

  void initX1X2(CQChartsPlot *plot1, CQChartsPlot *plot2, bool overlay, bool reset=false);
  void initY1Y2(CQChartsPlot *plot1, CQChartsPlot *plot2, bool overlay, bool reset=false);

  //---

  // place plots
  void placePlots(const Plots &plots, bool vertical, bool horizontal,
                  int rows, int columns, bool reset=false);

  //---

  // interp palette/theme color
  QColor interpPaletteColor(const ColorInd &ind, bool scale=false) const;
  QColor interpPaletteColor(int i, int n, bool scale=false) const;
  QColor interpPaletteColor(double r, bool scale=false) const;

  QColor interpGroupPaletteColor(const ColorInd &ig, const ColorInd &iv, bool scale) const;
  QColor interpGroupPaletteColor(int ig, int ng, int i, int n, bool scale) const;

  QColor interpGroupPaletteColor(int ig, int ng, double r, bool scale) const;

  QColor interpThemeColor(const ColorInd &ind) const;
  QColor interpThemeColor(int i, int n) const;
  QColor interpThemeColor(double r) const;

  QColor interpColor(const CQChartsColor &c, const ColorInd &ind) const;
  QColor interpColor(const CQChartsColor &c, int i, int n) const;
  QColor interpColor(const CQChartsColor &c, double r) const;

  //---

  // Qt event handling
  void mousePressEvent  (QMouseEvent *me) override;
  void mouseMoveEvent   (QMouseEvent *me) override;
  void mouseReleaseEvent(QMouseEvent *me) override;

  bool editMousePress  ();
  bool editMouseMove   ();
  void editMouseMotion ();
  void editMouseRelease();

  void keyPressEvent(QKeyEvent *ke) override;

  QPoint adjustMousePos(const QPoint &pos) const;

  //---

  // handle resize
  void resizeEvent(QResizeEvent *) override;

  void doResize(int w, int h);

  //---

  // handle paint
  void paintEvent(QPaintEvent *) override;

  void paint(QPainter *painter, CQChartsPlot *plot=nullptr);

  bool hasAnnotations() const;
  void drawAnnotations(CQChartsPaintDevice *device, const CQChartsLayer::Type &layerType);

  void drawKey(CQChartsPaintDevice *device, const CQChartsLayer::Type &layerType);

  void lockPainter(bool lock);

  //---

  // get buffers
  CQChartsBuffer *objectsBuffer() const { return objectsBuffer_; }
  CQChartsBuffer *overlayBuffer() const { return overlayBuffer_; }

  // get/set draw layer type
  const CQChartsLayer::Type &drawLayerType() const { return drawLayerType_; }
  void setDrawLayerType(const CQChartsLayer::Type &t) { drawLayerType_ = t; }

  //---

  // print to PNG/SVG
  bool printPNG(const QString &filename, CQChartsPlot *plot=nullptr);
  bool printSVG(const QString &filename, CQChartsPlot *plot=nullptr);

  // write javascript
  bool writeScript(const QString &filename, CQChartsPlot *plot=nullptr);

  const QString &scriptSelectProc() const { return scriptSelectProc_; }
  void setScriptSelectProc(const QString &s) { scriptSelectProc_ = s; }

  //---

  // set pen/brush
  void setPen(QPen &pen, bool stroked, const QColor &strokeColor, double strokeAlpha=1.0,
              const CQChartsLength &strokeWidth=CQChartsLength("0px"),
              const CQChartsLineDash &strokeDash=CQChartsLineDash()) const;

  void setBrush(QBrush &brush, bool filled, const QColor &fillColor=QColor(), double fillAlpha=1.0,
                const CQChartsFillPattern &pattern=CQChartsFillPattern()) const;

  //---

  void updateObjPenBrushState(const CQChartsObj *obj, QPen &pen, QBrush &brush,
                              DrawType drawType=DrawType::BOX) const;

  void updateObjPenBrushState(const CQChartsObj *obj, const ColorInd &ic,
                              QPen &pen, QBrush &brush, DrawType drawType) const;

  void updateInsideObjPenBrushState  (const ColorInd &ic, QPen &pen, QBrush &brush,
                                      bool outline, DrawType drawType) const;
  void updateSelectedObjPenBrushState(const ColorInd &ic, QPen &pen, QBrush &brush,
                                      DrawType drawType) const;

  QColor insideColor(const QColor &c) const;
  QColor selectedColor(const QColor &c) const;

  //---

  // cycle and edit
  void cycleEdit();

  void editObjs(Objs &objs);

  //---

  // probe lines
  void showProbeLines(const QPointF &p);

  //---

  // update cursor position text
  void updatePosText(const QPointF &pos);

  //---

  // handle region rubberband
  void startRegionBand(const QPoint &pos);
  void updateRegionBand(CQChartsPlot *plot, const QPoint &pressPoint, const QPoint &movePoint);
  void updateRegionBand(const QPoint &pressPoint, const QPoint &movePoint);
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

  // show context menu
  void showMenu(const QPoint &p);

  QPointF menuPos() const { return mousePressPoint(); }

  //---

  // mouse data
  const Plots &mousePlots() const { return mouseData_.plots; }
  const CQChartsPlot *mousePlot() const { return mouseData_.plot; }

  QPoint mousePressPoint() const { return mouseData_.pressPoint; }
  QPoint mouseMovePoint () const { return mouseData_.movePoint; }

  bool mousePressed() const { return mouseData_.pressed; }
  int  mouseButton () const { return mouseData_.button; }

  CQChartsSelMod mouseSelMod  () const { return mouseData_.selMod; }
  CQChartsSelMod mouseClickMod() const { return mouseData_.clickMod; }

  //---

  // get plots
  bool plots(Plots &plots, bool clear=true) const;

  bool basePlots(PlotSet &plots, bool clear=true) const;

  // get plots at point
  CQChartsPlot *plotAt(const CQChartsGeom::Point &p) const;

  bool plotsAt(const CQChartsGeom::Point &p, Plots &plots, CQChartsPlot* &plot,
               bool clear=true, bool first=false) const;

  bool plotsAt(const CQChartsGeom::Point &p, Plots &plots, bool clear=true) const;

  bool basePlotsAt(const CQChartsGeom::Point &p, PlotSet &plots, bool clear=true) const;

  // get plot bbox
  CQChartsGeom::BBox plotBBox(CQChartsPlot *plot) const;

  //---

  // convert plot to/from index
  int plotInd(CQChartsPlot *plot) const;

  CQChartsPlot *getPlotForInd(int ind) const;

  // get current plot
  CQChartsPlot *currentPlot(bool remap=true) const;

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
  QPointF positionToView (const CQChartsPosition &pos) const;
  QPointF positionToPixel(const CQChartsPosition &pos) const;

  // rect
  QRectF rectToView (const CQChartsRect &rect) const;
  QRectF rectToPixel(const CQChartsRect &rect) const;

  double lengthViewWidth (const CQChartsLength &len) const;
  double lengthViewHeight(const CQChartsLength &len) const;

  double lengthPixelWidth (const CQChartsLength &len) const;
  double lengthPixelHeight(const CQChartsLength &len) const;

  //---

  // coordinate conversions
  CQChartsGeom::Point windowToPixel(const CQChartsGeom::Point &w) const;
  CQChartsGeom::Point pixelToWindow(const CQChartsGeom::Point &p) const;

  QPointF windowToPixel(const QPointF &w) const;
  QPointF pixelToWindow(const QPointF &p) const;

  CQChartsGeom::BBox windowToPixel(const CQChartsGeom::BBox &w) const;
  CQChartsGeom::BBox pixelToWindow(const CQChartsGeom::BBox &p) const;

  QRectF windowToPixel(const QRectF &w) const;
  QRectF pixelToWindow(const QRectF &p) const;

  double pixelToSignedWindowWidth (double ww) const;
  double pixelToSignedWindowHeight(double wh) const;

  double pixelToWindowWidth (double pw) const;
  double pixelToWindowHeight(double ph) const;

  double windowToSignedPixelWidth (double ww) const;
  double windowToSignedPixelHeight(double wh) const;

  double windowToPixelWidth (double ww) const;
  double windowToPixelHeight(double wh) const;

  QSizeF pixelToWindowSize(const QSizeF &ps) const;

  //---

  // get pixel rect
  const CQChartsGeom::BBox prect() const { return prect_; }

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

  // emitted when interface or theme palettes changed
  void interfacePaletteChanged();
  void themePalettesChanged();

  // emitted when status text changes
  void statusTextChanged(const QString &text);

  // emitted when position text changes
  void posTextChanged(const QString &text);

  // emitted when selection text changed
  void selTextChanged(const QString &text);

  // emitted when current plot changed
  void currentPlotChanged();

  // emitted when plot added
  void plotAdded(CQChartsPlot *);
  void plotAdded(const QString &id);

  // emitted when plot removed
  void plotRemoved(const QString &id);
  // emitted when all plots removed
  void allPlotsRemoved();

  // emitted when plots changed
  void plotsChanged();

  // emitted when plots reordered
  void plotsReordered();

  // emitted when plot connect data changed
  void plotConnectDataChanged(const QString &id);
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

 public slots:
  void editObjectSlot();

  void setAutoSize(bool b);

  void selectModeSlot();
  void zoomModeSlot();
  void panModeSlot();
  void probeModeSlot();
  void queryModeSlot();
  void editModeSlot();

  void plotModelChanged();
  void plotConnectDataChangedSlot();

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

  void fitSlot();

  //---

  void lightPaletteSlot();
  void darkPaletteSlot();

  void themeNameSlot();

  void themeSlot(const QString &name);

  //---

  bool printFile(const QString &filename, CQChartsPlot *plot=nullptr);

  void printPNGSlot();
  void printPNGSlot(const QString &filename);

  void printSVGSlot();
  void printSVGSlot(const QString &filename);

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

  void zoomMousePress();
  void zoomMouseMove();
  void zoomMouseRelease();

  void panMouseMove();

  void resetConnections(bool notify);

  void initOverlayPlot(CQChartsPlot *firstPlot);

  int plotPos(CQChartsPlot *plot) const;

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

  //! structure for mouse interaction data
  struct MouseData {
    Plots              plots;                                   //!< plots at mouse point
    CQChartsPlot*      plot       { nullptr };                  //!< plot at mouse point
    QPoint             pressPoint { 0, 0 };                     //!< press point
    QPoint             oldMovePoint;                            //!< previous move point
    QPoint             movePoint  { 0, 0 };                     //!< move point
    bool               pressed    { false };                    //!< is pressed
    bool               escape     { false };                    //!< escape pressed
    int                button     { Qt::NoButton };             //!< press button
    CQChartsSelMod     selMod     { CQChartsSelMod::REPLACE };  //!< selection modifier
    CQChartsSelMod     clickMod   { CQChartsSelMod::REPLACE };  //!< click modifier
    DragObj            dragObj    { DragObj::NONE };            //!< drag object
    CQChartsResizeSide dragSide   { CQChartsResizeSide::NONE }; //!< drag sided
    bool               dragged    { false };                    //!< is dragged

    void reset() {
      plots.clear();

      plot     = nullptr;
      pressed  = false;
      escape   = false;
      button   = Qt::NoButton;
      selMod   = CQChartsSelMod::REPLACE;
      dragSide = CQChartsResizeSide::NONE;
      dragged  = false;
    }
  };

  //! structure containing the highlight mode
  struct HighlightData {
    bool isOutline() const { return int(mode) & int(HighlightDataMode::OUTLINE); }
    bool isFill   () const { return int(mode) & int(HighlightDataMode::FILL   ); }

    HighlightDataMode mode { HighlightDataMode::OUTLINE }; //!< highlight mode
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
    using PlotBBox = std::map<QString,CQChartsGeom::BBox>;

    bool     active      { false }; //!< active
    double   delta       { 100 };   //!< delta percent
    int      numPages    { 1 };     //!< num pages
    int      page        { 0 };     //!< current page
    bool     autoInit    { true };  //!< auto init plot placement on activate
    PlotBBox plotBBoxMap;           //!< saved plot bbox
  };

  using EditAnnotationDlg = CQChartsEditAnnotationDlg;

  using ProbeBands = std::vector<CQChartsProbeBand*>;

  static QSize defSizeHint_;

  CQCharts*             charts_            { nullptr };           //!< parent charts
  CQChartsWindow*       window_            { nullptr };           //!< parent window
  QImage*               image_             { nullptr };           //!< image buffer
  QPainter*             ipainter_          { nullptr };           //!< image painter
  CQChartsDisplayRange* displayRange_      { nullptr };           //!< display range
  CQPropertyViewModel*  propertyModel_     { nullptr };           //!< property model
  QString               id_;                                      //!< view id
  QString               title_;                                   //!< view title
  CQChartsViewKey*      keyObj_            { nullptr };           //!< key object
  Plots                 plots_;                                   //!< child plots
  int                   currentPlotInd_    { -1 };                //!< current plot index
  Annotations           annotations_;                             //!< annotations
  Mode                  mode_              { Mode::SELECT };      //!< mouse mode
  SelectMode            selectMode_        { SelectMode::RECT };  //!< selection sub mode
  bool                  selectInside_      { true };              //!< select inside/touching
  int                   selecting_         { 0 };                 //!< selecting depth
  HighlightData         selectedHighlight_;                       //!< select highlight
  HighlightData         insideHighlight_;                         //!< inside highlight
  QString               defaultPalette_;                          //!< default palette
  ScrollData            scrollData_;                              //!< scroll data
  bool                  antiAlias_         { true };              //!< anti alias
//bool                  showTable_         { false };             //!< show table with plot
  bool                  bufferLayers_      { true };              //!< buffer draw layers
  bool                  preview_           { false };             //!< preview
  bool                  scaleFont_         { true };              //!< auto scale font
  double                fontFactor_        { 1.0 };               //!< font scale factor
  CQChartsFont          font_;                                    //!< font
  SizeData              sizeData_;                                //!< size control
  PosTextType           posTextType_       { PosTextType::PLOT }; //!< position text type
  CQChartsGeom::BBox    prect_             { 0, 0, 100, 100 };    //!< plot rect
  double                aspect_            { 1.0 };               //!< current aspect
  MouseData             mouseData_;                               //!< mouse data
  int                   searchTimeout_     { 10 };                //!< search timeout
  QTimer*               searchTimer_       { nullptr };           //!< search timer
  QPointF               searchPos_;                               //!< search pos
  QRubberBand*          regionBand_        { nullptr };           //!< zoom region rubberband
  ProbeBands            probeBands_;                              //!< probe lines
  QMenu*                popupMenu_         { nullptr };           //!< context menu
  QSize                 viewSizeHint_;                            //!< view size hint
  CQChartsBuffer*       objectsBuffer_     { nullptr };           //!< buffer for view objects
  CQChartsBuffer*       overlayBuffer_     { nullptr };           //!< buffer for view overlays
  CQChartsLayer::Type   drawLayerType_;                           //!< current draw layer type
  mutable std::mutex    painterMutex_;                            //!< painter mutex
  EditAnnotationDlg*    editAnnotationDlg_ { nullptr };           //!< edit annotation dialog
  CQChartsEditAxisDlg*  editAxisDlg_       { nullptr };           //!< edit axis dialog
  CQChartsEditKeyDlg*   editKeyDlg_        { nullptr };           //!< edit key dialog
  CQChartsEditTitleDlg* editTitleDlg_      { nullptr };           //!< edit title dialog
  QString               scriptSelectProc_;
};

#endif
