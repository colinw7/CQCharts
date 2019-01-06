#ifndef CQChartsView_H
#define CQChartsView_H

#include <CQChartsObjData.h>
#include <CQChartsGeom.h>
#include <CQChartsTheme.h>
#include <CQChartsSymbol.h>
#include <CQChartsLineDash.h>
#include <CQChartsPosition.h>
#include <CQChartsLength.h>
#include <CQChartsRect.h>
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
class CQChartsTextAnnotation;
class CQChartsArrowAnnotation;
class CQChartsRectAnnotation;
class CQChartsEllipseAnnotation;
class CQChartsPolygonAnnotation;
class CQChartsPolylineAnnotation;
class CQChartsPointAnnotation;
class CQChartsProbeBand;
class CQChartsGradientPalette;
class CQChartsDisplayRange;
class CQPropertyViewModel;
class CQPropertyViewItem;

struct CQChartsTextOptions;

class QPainter;
class QToolButton;
class QRubberBand;
class QLabel;
class QMenu;

CQCHARTS_NAMED_SHAPE_DATA(Selected,selected)
CQCHARTS_NAMED_SHAPE_DATA(Inside,inside)

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

  // edit, select mode
  Q_PROPERTY(Mode       mode       READ mode       WRITE setMode      )
  Q_PROPERTY(SelectMode selectMode READ selectMode WRITE setSelectMode)

  // theme
  Q_PROPERTY(CQChartsTheme theme READ theme  WRITE setTheme)
  Q_PROPERTY(bool          dark  READ isDark WRITE setDark )

  // selection appearance
  Q_PROPERTY(HighlightDataMode selectedMode READ selectedMode WRITE setSelectedMode)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Selected,selected)

  // inside appearance
  Q_PROPERTY(HighlightDataMode insideMode READ insideMode WRITE setInsideMode)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Inside,inside)

  // zoom to data
  Q_PROPERTY(bool zoomData READ isZoomData WRITE setZoomData)

  // scroll (TODO remove)
  Q_PROPERTY(bool        scrolled       READ isScrolled     WRITE setScrolled      )
  Q_PROPERTY(double      scrollDelta    READ scrollDelta    WRITE setScrollDelta   )
  Q_PROPERTY(int         scrollNumPages READ scrollNumPages WRITE setScrollNumPages)
  Q_PROPERTY(int         scrollPage     READ scrollPage     WRITE setScrollPage    )

  // anti-alias, buffer, preview, pos text type
  Q_PROPERTY(bool        antiAlias      READ isAntiAlias    WRITE setAntiAlias   )
  Q_PROPERTY(bool        bufferLayers   READ isBufferLayers WRITE setBufferLayers)
  Q_PROPERTY(bool        preview        READ isPreview      WRITE setPreview     )
  Q_PROPERTY(bool        scaleFont      READ isScaleFont    WRITE setScaleFont   )
  Q_PROPERTY(double      fontFactor     READ fontFactor     WRITE setFontFactor  )
  Q_PROPERTY(PosTextType posTextType    READ posTextType    WRITE setPosTextType )

  Q_ENUMS(Mode)
  Q_ENUMS(SelectMode)
  Q_ENUMS(HighlightDataMode)
  Q_ENUMS(ThemeType)
  Q_ENUMS(PosTextType)

 public:
  using SelMod = CQChartsSelMod;

  enum class Mode {
    SELECT,
    ZOOM,
    PAN,
    PROBE,
    EDIT
  };

  enum class SelectMode {
    POINT,
    RECT
  };

  enum class HighlightDataMode {
    OUTLINE,
    FILL
  };

  enum class PosTextType {
    PLOT,
    VIEW,
    PIXEL
  };

 public:
  using Plots       = std::vector<CQChartsPlot*>;
  using Objs        = std::vector<CQChartsObj*>;
  using PlotSet     = std::set<CQChartsPlot*>;
  using Annotations = std::vector<CQChartsAnnotation *>;

 public:
  static double viewportRange() { return 100.0; }

  static const QSize &defSizeHint() { return defSizeHint_; }
  static void setDefSizeHint(const QSize &s) { defSizeHint_ = s; }

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

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &m);

  const SelectMode &selectMode() const { return selectMode_; }
  void setSelectMode(const SelectMode &m);

  //---

  void setPainterFont(QPainter *painter, const QFont &font) const;

  void setPlotPainterFont(const CQChartsPlot *plot, QPainter *painter, const QFont &font) const;

  QFont viewFont(const QFont &font) const;
  QFont plotFont(const CQChartsPlot *plot, const QFont &font) const;

  double calcFontScale(const QSizeF &size) const;

  QFont scaledFont(const QFont &font, const QSizeF &size) const;

  QFont scaledFont(const QFont &font, double s) const;

  //---

  void setCurrentPlot(CQChartsPlot *plot);

  //---

  const HighlightDataMode &selectedMode() const { return selectedHighlight_.mode; }
  void setSelectedMode(const HighlightDataMode &mode) { selectedHighlight_.mode = mode; }

  const HighlightDataMode &insideMode() const { return insideHighlight_.mode; }
  void setInsideMode(const HighlightDataMode &mode) { insideHighlight_.mode = mode; }

  //---

  bool isZoomData() const { return zoomData_; }
  void setZoomData(bool b) { zoomData_ = b; }

  //---

  bool isScrolled() const { return scrollData_.active; }
  void setScrolled(bool b) { scrollData_.active = b; }

  double scrollDelta() const { return scrollData_.delta; }
  void setScrollDelta(double r) { scrollData_.delta = r; }

  int scrollNumPages() const { return scrollData_.numPages; }
  void setScrollNumPages(int i) { scrollData_.numPages = i; }

  int scrollPage() const { return scrollData_.page; }
  void setScrollPage(int i) { scrollData_.page = i; }

  //---

  bool isAntiAlias() const { return antiAlias_; }
  void setAntiAlias(bool b);

  bool isBufferLayers() const { return bufferLayers_; }
  void setBufferLayers(bool b);

  bool isPreview() const { return preview_; }
  void setPreview(bool b);

  bool isScaleFont() const { return scaleFont_; }
  void setScaleFont(bool b);

  double fontFactor() const { return fontFactor_; }
  void setFontFactor(double r);

  const PosTextType &posTextType() const { return posTextType_; }
  void setPosTextType(const PosTextType &t);

  //---

  CQChartsThemeObj *themeObj();
  const CQChartsThemeObj *themeObj() const;

  const CQChartsTheme &theme() const;
  void setTheme(const CQChartsTheme &name);

  //---

  CQChartsGradientPalette *interfacePalette() const;

  CQChartsGradientPalette *themeGroupPalette(int i, int n) const;

  CQChartsGradientPalette *themePalette(int ind=0) const;

  bool isDark() const;
  void setDark(bool b);

  //---

  CQPropertyViewModel *propertyModel() const { return propertyModel_; }

  //---

  bool setProperties(const QString &properties);

  bool setProperty(const QString &name, const QVariant &value);
  bool getProperty(const QString &name, QVariant &value) const;

  CQPropertyViewItem *addProperty(const QString &path, QObject *object,
                                  const QString &name, const QString &alias="");

  void getPropertyNames(QStringList &names) const;

  //---

  CQChartsViewKey *key() const { return keyObj_; }

  //---

  void addPlot(CQChartsPlot *plot, const CQChartsGeom::BBox &bbox=CQChartsGeom::BBox());

  int numPlots() const { return plots_.size(); }

  CQChartsPlot *plot(int i) { assert(i >= 0 && i < int(plots_.size())); return plots_[i]; }

  CQChartsPlot *getPlot(const QString &id) const;

  void getPlots(Plots &plots) const;

  void raisePlot(CQChartsPlot *plot);
  void lowerPlot(CQChartsPlot *plot);

  void removePlot(CQChartsPlot *plot);

  void removeAllPlots();

  void deselectAll();

  void startSelection();
  void endSelection();

  //---

  // annotations

  const Annotations &annotations() const { return annotations_; }

  CQChartsTextAnnotation     *addTextAnnotation    (const CQChartsPosition &pos,
                                                    const QString &text);
  CQChartsTextAnnotation     *addTextAnnotation    (const CQChartsRect &rect,
                                                    const QString &text);
  CQChartsArrowAnnotation    *addArrowAnnotation   (const CQChartsPosition &start,
                                                    const CQChartsPosition &end);
  CQChartsRectAnnotation     *addRectAnnotation    (const CQChartsRect &rect);
  CQChartsEllipseAnnotation  *addEllipseAnnotation (const CQChartsPosition &center,
                                                    const CQChartsLength &xRadius,
                                                    const CQChartsLength &yRadius);
  CQChartsPolygonAnnotation  *addPolygonAnnotation (const QPolygonF &points);
  CQChartsPolylineAnnotation *addPolylineAnnotation(const QPolygonF &points);
  CQChartsPointAnnotation    *addPointAnnotation   (const CQChartsPosition &pos,
                                                    const CQChartsSymbol &type);

  void addAnnotation(CQChartsAnnotation *annotation);

  CQChartsAnnotation *getAnnotationByName(const QString &id) const;

  void removeAnnotation(CQChartsAnnotation *annotation);

  void removeAllAnnotations();

  //---

  void resetGrouping();

  void initOverlay(const Plots &plots, bool reset=false);

  void initX1X2(CQChartsPlot *plot1, CQChartsPlot *plot2, bool overlay, bool reset=false);
  void initY1Y2(CQChartsPlot *plot1, CQChartsPlot *plot2, bool overlay, bool reset=false);

  //---

  void placePlots(const Plots &plots, bool vertical, bool horizontal, int rows, int columns);

  //---

  QColor interpPaletteColor(double r, bool scale=false) const;
  QColor interpIndPaletteColor(int ind, double r, bool scale=false) const;
  QColor interpGroupPaletteColor(int ig, int ng, double r, bool scale) const;

  QColor interpThemeColor(double r) const;

  //---

  void mousePressEvent  (QMouseEvent *me) override;
  void mouseMoveEvent   (QMouseEvent *me) override;
  void mouseReleaseEvent(QMouseEvent *me) override;

  bool editMousePress (const QPointF &p);
  void editMouseMotion(const QPointF &p);

  void keyPressEvent(QKeyEvent *ke) override;

  //---

  void resizeEvent(QResizeEvent *) override;

  void doResize(int w, int h);

  //---

  void paintEvent(QPaintEvent *) override;

  void paint(QPainter *painter, CQChartsPlot *plot=nullptr);

  void lockPainter(bool lock);

  //---

  void printPNG(const QString &filename, CQChartsPlot *plot=nullptr);
  void printSVG(const QString &filename, CQChartsPlot *plot=nullptr);

  //---

  void setPen(QPen &pen, bool stroked, const QColor &strokeColor, double strokeAlpha=1.0,
              const CQChartsLength &strokeWidth=CQChartsLength("0px"),
              const CQChartsLineDash &strokeDash=CQChartsLineDash()) const;

  void setBrush(QBrush &brush, bool filled, const QColor &fillColor, double fillAlpha=1.0,
                const CQChartsFillPattern &pattern=CQChartsFillPattern()) const;

  //---

  static void drawTextInBox(QPainter *painter, const QRectF &rect, const QString &text,
                            const QPen &pen, const CQChartsTextOptions &options);
  static void drawContrastText(QPainter *painter, double x, double y, const QString &text,
                               const QPen &pen);

  //---

  void cycleEdit();

  void editObjs(Objs &objs);

  //---

  void showProbeLines(const QPointF &p);

  void updatePosText(const QPointF &pos);

  //---

  void startRegionBand(const QPoint &pos);
  void updateRegionBand(CQChartsPlot *plot, const QPoint &pressPoint, const QPoint &movePoint);
  void updateRegionBand(const QPoint &pressPoint, const QPoint &movePoint);
  void endRegionBand();

  //---

  void updateSelText();

  void selectedObjs(Objs &objs) const;

  void allSelectedObjs(Objs &objs) const;

  //---

  void showMenu(const QPoint &p);

  QPointF menuPos() const { return mouseData_.pressPoint; }

  //---

  bool plots(Plots &plots, bool clear=true) const;

  bool basePlots(PlotSet &plots, bool clear=true) const;

  CQChartsPlot *plotAt(const CQChartsGeom::Point &p) const;

  bool plotsAt(const CQChartsGeom::Point &p, Plots &plots, CQChartsPlot* &plot,
               bool clear=true) const;

  bool plotsAt(const CQChartsGeom::Point &p, Plots &plots, bool clear=true) const;

  bool basePlotsAt(const CQChartsGeom::Point &p, PlotSet &plots, bool clear=true) const;

  CQChartsGeom::BBox plotBBox(CQChartsPlot *plot) const;

  //---

  int plotInd(CQChartsPlot *plot) const;

  CQChartsPlot *currentPlot(bool remap=true) const;

  CQChartsPlot *getPlotForInd(int ind) const;

  //---

  virtual void setPosText(const QString &text);

  virtual void setStatusText(const QString &text);

  virtual void setSelText(const QString &text);

  //---

  void scrollLeft();
  void scrollRight();
  void updateScroll();

  //---

  void setPaletteColors1();
  void setPaletteColors2();

  void setLightThemeColors();
  void setDarkThemeColors();

  void updateTheme();

  //---

  QPointF positionToView (const CQChartsPosition &pos) const;
  QPointF positionToPixel(const CQChartsPosition &pos) const;

  QRectF rectToView (const CQChartsRect &rect) const;
  QRectF rectToPixel(const CQChartsRect &rect) const;

  double lengthViewWidth (const CQChartsLength &len) const;
  double lengthViewHeight(const CQChartsLength &len) const;

  double lengthPixelWidth (const CQChartsLength &len) const;
  double lengthPixelHeight(const CQChartsLength &len) const;

  //---

  void windowToPixel(double wx, double wy, double &px, double &py) const;
  void pixelToWindow(double px, double py, double &wx, double &wy) const;

  CQChartsGeom::Point windowToPixel(const CQChartsGeom::Point &w) const;
  CQChartsGeom::Point pixelToWindow(const CQChartsGeom::Point &p) const;

  CQChartsGeom::BBox windowToPixel(const CQChartsGeom::BBox &w) const;
  CQChartsGeom::BBox pixelToWindow(const CQChartsGeom::BBox &p) const;

  double pixelToSignedWindowWidth (double ww) const;
  double pixelToSignedWindowHeight(double wh) const;

  double pixelToWindowWidth (double pw) const;
  double pixelToWindowHeight(double ph) const;

  double windowToSignedPixelWidth (double ww) const;
  double windowToSignedPixelHeight(double wh) const;

  double windowToPixelWidth (double ww) const;
  double windowToPixelHeight(double wh) const;

  //---

  const CQChartsGeom::BBox prect() const { return prect_; }

  double aspect() const { return aspect_; }

  //---

  const QSize &viewSizeHint() const { return viewSizeHint_; }
  void setViewSizeHint(const QSize &s) { viewSizeHint_ = s; }

  QSize sizeHint() const override;

 signals:
  void modeChanged();

  void selectModeChanged();

  void interfacePaletteChanged();
  void themePalettesChanged();

  void statusTextChanged(const QString &text);

  void posTextChanged(const QString &text);

  void selTextChanged(const QString &text);

  void currentPlotChanged();

  void plotAdded(CQChartsPlot *);
  void plotAdded(const QString &id);
  void plotRemoved(const QString &id);
  void allPlotsRemoved();
  void plotsChanged();
  void plotsReordered();

  void plotConnectDataChanged(const QString &id);
  void connectDataChanged();

  void annotationPressed(CQChartsAnnotation *);
  void annotationIdPressed(const QString &);

  void annotationsChanged();

  void selectionChanged();

 public slots:
  void plotModelChanged();
  void plotConnectDataChangedSlot();

  void updatePlots();

  void updateSlot();

  void searchSlot();

  void keyVisibleSlot(bool b);
  void keyPositionSlot(QAction *action);
  void keyInsideXSlot(bool b);
  void keyInsideYSlot(bool b);

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

  void defaultThemeSlot();
  void palette1Slot();
  void palette2Slot();

  void themeSlot(const QString &name);

  //---

  void printFile(const QString &filename, CQChartsPlot *plot=nullptr);

  void printPNGSlot();
  void printPNGSlot(const QString &filename);

  void printSVGSlot();
  void printSVGSlot(const QString &filename);

  //---

  void showBoxesSlot(bool b);

  void bufferLayersSlot(bool b);

  //---

  void xRangeMapSlot(bool b);
  void yRangeMapSlot(bool b);

  void dataTableSlot(bool b);
  void viewSettingsSlot(bool b);

  //---

  void currentPlotSlot();

 private slots:
  void currentPlotZoomPanChanged();

  void updateAnnotationSlot();

 private:
  void resetConnections(bool notify);

  void initOverlayPlot(CQChartsPlot *firstPlot);

  int plotPos(CQChartsPlot *plot) const;

 private:
  struct MouseData {
    Plots         plots;
    CQChartsPlot* plot      { nullptr };
    QPoint        pressPoint;
    QPoint        movePoint;
    bool          pressed   { false };
    bool          escape    { false };
    int           button    { Qt::NoButton };

    void reset() {
      plots.clear();

      plot    = nullptr;
      pressed = false;
      escape  = false;
      button  = Qt::NoButton;
    }
  };

  struct HighlightData {
    HighlightDataMode mode { HighlightDataMode::OUTLINE };

    bool isOutline() const { return int(mode) & int(HighlightDataMode::OUTLINE); }
    bool isFill   () const { return int(mode) & int(HighlightDataMode::FILL   ); }
  };

  struct ScrollData {
    bool   active   { false }; // active
    double delta    { 100 };   // delta
    int    numPages { 1 };     // num pages
    int    page     { 0 };     // current page
  };

  using ProbeBands = std::vector<CQChartsProbeBand*>;

  static QSize defSizeHint_;

  CQCharts*             charts_           { nullptr };           // parent charts
  CQChartsWindow*       window_           { nullptr };           // parent window
  QImage*               image_            { nullptr };           // image buffer
  QPainter*             ipainter_         { nullptr };           // image painter
  CQChartsDisplayRange* displayRange_     { nullptr };           // display range
  CQPropertyViewModel*  propertyModel_    { nullptr };           // property model
  QString               id_;                                     // view id
  QString               title_;                                  // view title
  CQChartsViewKey*      keyObj_           { nullptr };           // key object
  Plots                 plots_;                                  // child plots
  int                   currentPlotInd_   { -1 };                // current plot index
  Annotations           annotations_;                            // annotations
  Mode                  mode_             { Mode::SELECT };      // mouse mode
  SelectMode            selectMode_       { SelectMode::POINT }; // selection sub mode
  int                   selecting_        { 0 };                 // selecting depth
  HighlightData         selectedHighlight_;                      // select highlight
  HighlightData         insideHighlight_;                        // inside highlight
  bool                  zoomData_         { true };              // zoom data
  ScrollData            scrollData_;                             // scroll data
  bool                  antiAlias_        { true };              // anit alias
  bool                  bufferLayers_     { true };              // buffer draw layers
  bool                  preview_          { false };             // preview
  bool                  scaleFont_        { true };              // auto scale font
  double                fontFactor_       { 1.0 };               // font scale factor
  PosTextType           posTextType_      { PosTextType::PLOT }; // position text type
  CQChartsGeom::BBox    prect_            { 0, 0, 100, 100 };    // plot rect
  double                aspect_           { 1.0 };               // current aspect
  MouseData             mouseData_;                              // mouse data
  QTimer                searchTimer_;                            // search timer
  QPointF               searchPos_;                              // search pos
  QRubberBand*          regionBand_       { nullptr };           // zoom region rubberband
  ProbeBands            probeBands_;                             // probe lines
  QMenu*                popupMenu_        { nullptr };           // context menu
  QSize                 viewSizeHint_;                           // view size hint
  mutable std::mutex    painterMutex_;
};

#endif
