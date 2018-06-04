#ifndef CQChartsView_H
#define CQChartsView_H

#include <CQChartsTheme.h>
#include <CQChartsGeom.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsLineDash.h>
#include <CQChartsPosition.h>
#include <CQChartsLength.h>
#include <QFrame>
#include <QTimer>
#include <set>

class CQCharts;
class CQChartsPlot;
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
class CQChartsTextOptions;
class CQPropertyViewModel;

class QPainter;
class QToolButton;
class QRubberBand;
class QLabel;
class QMenu;

class CQChartsView : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString    id             READ id             WRITE setId            )
  Q_PROPERTY(QString    title          READ title          WRITE setTitle         )
  Q_PROPERTY(QColor     background     READ background     WRITE setBackground    )
  Q_PROPERTY(int        currentPlotInd READ currentPlotInd WRITE setCurrentPlotInd)
  Q_PROPERTY(Mode       mode           READ mode           WRITE setMode          )
  Q_PROPERTY(SelectMode selectMode     READ selectMode     WRITE setSelectMode    )
  Q_PROPERTY(QString    themeName      READ themeName      WRITE setThemeName     )

  // selection appearance
  Q_PROPERTY(HighlightDataMode selectedMode               READ selectedMode
                                                          WRITE setSelectedMode)
  Q_PROPERTY(bool              selectedStrokeColorEnabled READ isSelectedStrokeColorEnabled
                                                          WRITE setSelectedStrokeColorEnabled)
  Q_PROPERTY(QColor            selectedStrokeColor        READ selectedStrokeColor
                                                          WRITE setSelectedStrokeColor)
  Q_PROPERTY(double            selectedStrokeWidth        READ selectedStrokeWidth
                                                          WRITE setSelectedStrokeWidth)
  Q_PROPERTY(CQChartsLineDash  selectedStrokeDash         READ selectedStrokeDash
                                                          WRITE setSelectedStrokeDash)
  Q_PROPERTY(bool              selectedFillColorEnabled   READ isSelectedFillColorEnabled
                                                          WRITE setSelectedFillColorEnabled)
  Q_PROPERTY(QColor            selectedFillColor          READ selectedFillColor
                                                          WRITE setSelectedFillColor)

  // inside appearance
  Q_PROPERTY(HighlightDataMode insideMode               READ insideMode
                                                        WRITE setInsideMode)
  Q_PROPERTY(bool              insideStrokeColorEnabled READ isInsideStrokeColorEnabled
                                                        WRITE setInsideStrokeColorEnabled)
  Q_PROPERTY(QColor            insideStrokeColor        READ insideStrokeColor
                                                        WRITE setInsideStrokeColor)
  Q_PROPERTY(double            insideStrokeWidth        READ insideStrokeWidth
                                                        WRITE setInsideStrokeWidth)
  Q_PROPERTY(CQChartsLineDash  insideStrokeDash         READ insideStrokeDash
                                                        WRITE setInsideStrokeDash)
  Q_PROPERTY(bool              insideFillColorEnabled   READ isInsideFillColorEnabled
                                                        WRITE setInsideFillColorEnabled)
  Q_PROPERTY(QColor            insideFillColor          READ insideFillColor
                                                        WRITE setInsideFillColor)

  Q_PROPERTY(bool        zoomData       READ isZoomData     WRITE setZoomData      )
  Q_PROPERTY(bool        scrolled       READ isScrolled     WRITE setScrolled      )
  Q_PROPERTY(double      scrollDelta    READ scrollDelta    WRITE setScrollDelta   )
  Q_PROPERTY(int         scrollNumPages READ scrollNumPages WRITE setScrollNumPages)
  Q_PROPERTY(int         scrollPage     READ scrollPage     WRITE setScrollPage    )
  Q_PROPERTY(bool        antiAlias      READ isAntiAlias    WRITE setAntiAlias     )
  Q_PROPERTY(bool        preview        READ isPreview      WRITE setPreview       )
  Q_PROPERTY(PosTextType posTextType    READ posTextType    WRITE setPosTextType   )

  Q_ENUMS(Mode)
  Q_ENUMS(SelectMode)
  Q_ENUMS(HighlightDataMode)
  Q_ENUMS(ThemeType)
  Q_ENUMS(PosTextType)

 public:
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

  enum class ThemeType {
    NONE,
    LIGHT1,
    LIGHT2,
    DARK1,
    DARK2
  };

  enum class PosTextType {
    PLOT,
    VIEW,
    PIXEL
  };

 public:
  using Plots       = std::vector<CQChartsPlot*>;
  using PlotSet     = std::set<CQChartsPlot*>;
  using Annotations = std::vector<CQChartsAnnotation *>;

 public:
  static double viewportRange() { return 100.0; }

  static QSize &getSizeHint() { return sizeHint_; }
  static void setSizeHint(const QSize &s) { sizeHint_ = s; }

 public:
  CQChartsView(CQCharts *charts, QWidget *parent=nullptr);

  virtual ~CQChartsView();

  CQCharts *charts() const { return charts_; }

  //---

  const QString &id() const { return id_; }
  void setId(const QString &s);

  const QString &title() const { return title_; }
  void setTitle(const QString &v);

  //---

  const QColor &background() const { return background_; }
  void setBackground(const QColor &c) { background_ = c; update(); }

  int currentPlotInd() const { return currentPlotInd_; }
  void setCurrentPlotInd(int i);

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &m);

  const SelectMode &selectMode() const { return selectMode_; }
  void setSelectMode(const SelectMode &m);

  //---

  const HighlightDataMode &selectedMode() const { return selectedHighlight_.mode; }
  void setSelectedMode(const HighlightDataMode &mode) { selectedHighlight_.mode = mode; }

  bool isSelectedStrokeColorEnabled() const { return selectedHighlight_.strokeColorEnabled; }
  void setSelectedStrokeColorEnabled(bool b) { selectedHighlight_.strokeColorEnabled = b; }

  const QColor &selectedStrokeColor() const { return selectedHighlight_.strokeColor; }
  void setSelectedStrokeColor(const QColor &c) { selectedHighlight_.strokeColor = c; }

  double selectedStrokeWidth() const { return selectedHighlight_.strokeWidth; }
  void setSelectedStrokeWidth(double w) { selectedHighlight_.strokeWidth = w; }

  const CQChartsLineDash &selectedStrokeDash() const { return selectedHighlight_.strokeDash; }
  void setSelectedStrokeDash(const CQChartsLineDash &d) { selectedHighlight_.strokeDash = d; }

  bool isSelectedFillColorEnabled() const { return selectedHighlight_.fillColorEnabled; }
  void setSelectedFillColorEnabled(bool b) { selectedHighlight_.fillColorEnabled = b; }

  const QColor &selectedFillColor() const { return selectedHighlight_.fillColor; }
  void setSelectedFillColor(const QColor &c) { selectedHighlight_.fillColor = c; }

  const HighlightDataMode &insideMode() const { return insideHighlight_.mode; }
  void setInsideMode(const HighlightDataMode &mode) { insideHighlight_.mode = mode; }

  bool isInsideStrokeColorEnabled() const { return insideHighlight_.strokeColorEnabled; }
  void setInsideStrokeColorEnabled(bool b) { insideHighlight_.strokeColorEnabled = b; }

  const QColor &insideStrokeColor() const { return insideHighlight_.strokeColor; }
  void setInsideStrokeColor(const QColor &c) { insideHighlight_.strokeColor = c; }

  double insideStrokeWidth() const { return insideHighlight_.strokeWidth; }
  void setInsideStrokeWidth(double w) { insideHighlight_.strokeWidth = w; }

  const CQChartsLineDash &insideStrokeDash() const { return insideHighlight_.strokeDash; }
  void setInsideStrokeDash(const CQChartsLineDash &d) { insideHighlight_.strokeDash = d; }

  bool isInsideFillColorEnabled() const { return insideHighlight_.fillColorEnabled; }
  void setInsideFillColorEnabled(bool b) { insideHighlight_.fillColorEnabled = b; }

  const QColor &insideFillColor() const { return insideHighlight_.fillColor; }
  void setInsideFillColor(const QColor &c) { insideHighlight_.fillColor = c; }

  //---

  bool isZoomData() const { return zoomData_; }
  void setZoomData(bool b) { zoomData_ = b; }

  bool isScrolled() const { return scrolled_; }
  void setScrolled(bool b) { scrolled_ = b; }

  double scrollDelta() const { return scrollDelta_; }
  void setScrollDelta(double r) { scrollDelta_ = r; }

  int scrollNumPages() const { return scrollNumPages_; }
  void setScrollNumPages(int i) { scrollNumPages_ = i; }

  int scrollPage() const { return scrollPage_; }
  void setScrollPage(int i) { scrollPage_ = i; }

  bool isAntiAlias() const { return antiAlias_; }
  void setAntiAlias(bool b) { antiAlias_ = b; updatePlots(); }

  bool isPreview() const { return preview_; }
  void setPreview(bool b) { preview_ = b; updatePlots(); }

  const PosTextType &posTextType() const { return posTextType_; }
  void setPosTextType(const PosTextType &v) { posTextType_ = v; }

  //---

  CQChartsGradientPalette *interfacePalette() { return interfacePalette_; }
  const CQChartsGradientPalette *interfacePalette() const { return interfacePalette_; }

  CQChartsTheme *theme() { return theme_; }
  const CQChartsTheme *theme() const { return theme_; }

  QString themeName() const { return (theme() ? theme()->name() : ""); }
  void setThemeName(const QString &name);

  CQChartsGradientPalette *themeGroupPalette(int i, int n) const;

  CQChartsGradientPalette *themePalette(int ind=0) const { return theme_->palette(ind); }

  bool isDark() const { return isDark_; }

  //---

  CQPropertyViewModel *propertyModel() const { return propertyModel_; }

  //---

  bool setProperties(const QString &properties);

  bool setProperty(const QString &name, const QVariant &value);
  bool getProperty(const QString &name, QVariant &value);

  void addProperty(const QString &path, QObject *object,
                   const QString &name, const QString &alias="");

  //---

  CQChartsViewKey *key() const { return keyObj_; }

  //---

  void addPlot(CQChartsPlot *plot, const CQChartsGeom::BBox &bbox=CQChartsGeom::BBox(0, 0, 1, 1));

  int numPlots() const { return plotDatas_.size(); }
  CQChartsPlot *plot(int i) { return plotDatas_[i].plot; }

  CQChartsPlot *getPlot(const QString &id) const;

  void getPlots(Plots &plots) const;

  void raisePlot(CQChartsPlot *plot);
  void lowerPlot(CQChartsPlot *plot);

  void removePlot(CQChartsPlot *plot);

  void removeAllPlots();

  void deselectAll();

  //---

  // annotations

  const Annotations &annotations() const { return annotations_; }

  CQChartsTextAnnotation     *addTextAnnotation    (const QPointF &pos, const QString &text);
  CQChartsArrowAnnotation    *addArrowAnnotation   (const QPointF &start, const QPointF &end);
  CQChartsRectAnnotation     *addRectAnnotation    (const QPointF &start, const QPointF &end);
  CQChartsEllipseAnnotation  *addEllipseAnnotation (const QPointF &center, double xRadius,
                                                    double yRadius);
  CQChartsPolygonAnnotation  *addPolygonAnnotation (const QPolygonF &points);
  CQChartsPolylineAnnotation *addPolylineAnnotation(const QPolygonF &points);
  CQChartsPointAnnotation    *addPointAnnotation   (const QPointF &pos,
                                                    const CQChartsPlotSymbol::Type &type);

  void addAnnotation(CQChartsAnnotation *annotation);

  CQChartsAnnotation *getAnnotationByName(const QString &id) const;

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

  QColor interpThemeColor(double r) const;

  //---

  void mousePressEvent  (QMouseEvent *me) override;
  void mouseMoveEvent   (QMouseEvent *me) override;
  void mouseReleaseEvent(QMouseEvent *me) override;

  bool editMousePress(const QPointF &p);

  void keyPressEvent(QKeyEvent *ke) override;

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

  void paint(QPainter *painter);

  //---

  static void drawTextInBox(QPainter *painter, const QRectF &rect, const QString &text,
                            const QPen &pen, const CQChartsTextOptions &options);
  static void drawContrastText(QPainter *painter, double x, double y, const QString &text,
                               const QPen &pen);

  //---

  void showProbeLines(const QPointF &p);

  void editMouseMotion(const QPointF &p);

  void updatePosText(const QPointF &pos);

  //---

  void startRegionBand(const QPoint &pos);
  void updateRegionBand(CQChartsPlot *plot, const QPoint &pressPoint, const QPoint &movePoint);
  void updateRegionBand(const QPoint &pressPoint, const QPoint &movePoint);
  void endRegionBand();

  //---

  void updateSelText();

  //---

  void showMenu(const QPoint &p);

  QPointF menuPos() const { return mouseData_.pressPoint; }

  //---

  void updatePlots();

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

  //---

  QPointF positionToView(const CQChartsPosition &pos) const;
  QPointF positionToPixel(const CQChartsPosition &pos) const;

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

  QSize sizeHint() const override { return CQChartsView::getSizeHint(); }

 signals:
  void modeChanged();

  void selectModeChanged();

  void themeChanged();

  void statusTextChanged(const QString &text);

  void posTextChanged(const QString &text);

  void selTextChanged(const QString &text);

  void currentPlotChanged();

  void plotAdded(CQChartsPlot *);
  void plotAdded(const QString &id);

  void plotsReordered();
  void plotRemoved(const QString &id);
  void allPlotsRemoved();

  void plotConnectDataChanged(const QString &id);
  void connectDataChanged();

  void annotationPressed(CQChartsAnnotation *);
  void annotationIdPressed(const QString &);

 public slots:
  void plotModelChanged();
  void plotConnectDataChangedSlot();

  void updateSlot();

  void searchSlot();

  void keyVisibleSlot(bool b);
  void keyPositionSlot(QAction *action);

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

  void printFile(const QString &filename);

  void printPNGSlot(const QString &filename="charts.png");
  void printSVGSlot(const QString &filename="charts.svg");

  //---

  void showBoxesSlot(bool b);

  //---

  void currentPlotSlot();

 private:
  void resetConnections(bool notify);

  void initOverlayPlot(CQChartsPlot *firstPlot);

  int plotPos(CQChartsPlot *plot) const;

 private:
  struct PlotData {
    CQChartsPlot*      plot { nullptr };
    CQChartsGeom::BBox bbox;

    PlotData(CQChartsPlot *plot, const CQChartsGeom::BBox &bbox) :
     plot(plot), bbox(bbox) {
    }
  };

  using PlotDatas = std::vector<PlotData>;

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
    HighlightDataMode mode               { HighlightDataMode::OUTLINE };
    bool              strokeColorEnabled { false };
    QColor            strokeColor        { Qt::black };
    double            strokeWidth        { 2 };
    CQChartsLineDash  strokeDash         { {2, 2}, 0 };
    bool              fillColorEnabled   { false };
    QColor            fillColor          { Qt::yellow };

    bool isOutline() const { return int(mode) & int(HighlightDataMode::OUTLINE); }
    bool isFill   () const { return int(mode) & int(HighlightDataMode::FILL   ); }
  };

  using ProbeBands = std::vector<CQChartsProbeBand*>;

  // TODO: view title, view key, view annotations

  static QSize sizeHint_;

  CQCharts*                charts_           { nullptr };
  CQChartsDisplayRange*    displayRange_     { nullptr };
  CQChartsGradientPalette* interfacePalette_ { nullptr };
  CQChartsTheme*           theme_            { nullptr };
  bool                     isDark_           { false };
  CQPropertyViewModel*     propertyModel_    { nullptr };
  QString                  id_;
  QString                  title_;
  QColor                   background_       { 255, 255, 255 };
  CQChartsViewKey*         keyObj_           { nullptr };
  PlotDatas                plotDatas_;
  int                      currentPlotInd_   { -1 };
  Annotations              annotations_;
  Mode                     mode_             { Mode::SELECT };
  SelectMode               selectMode_       { SelectMode::POINT };
  HighlightData            selectedHighlight_;
  HighlightData            insideHighlight_;
  bool                     zoomData_         { true };
  bool                     scrolled_         { false };
  double                   scrollDelta_      { 100 };
  int                      scrollNumPages_   { 1 };
  int                      scrollPage_       { 0 };
  bool                     antiAlias_        { true };
  bool                     preview_          { false };
  PosTextType              posTextType_      { PosTextType::PLOT };
  CQChartsGeom::BBox       prect_            { 0, 0, 100, 100 };
  double                   aspect_           { 1.0 };
  MouseData                mouseData_;
  QTimer                   searchTimer_;
  QPointF                  searchPos_;
  QRubberBand*             regionBand_       { nullptr };
  ProbeBands               probeBands_;
  QMenu*                   popupMenu_        { nullptr };
  ThemeType                themeType_        { ThemeType::NONE };
};

#endif
