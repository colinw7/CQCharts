#ifndef CQChartsView_H
#define CQChartsView_H

#include <CQChartsTheme.h>
#include <CQChartsGeom.h>
#include <CQChartsLineDash.h>
#include <QFrame>
#include <QTimer>
#include <set>

class CQCharts;
class CQChartsPlot;
class CQChartsViewKey;
class CQChartsProbeBand;
class QPainter;
class CQPropertyViewModel;
class CQChartsDisplayRange;

class CQChartsGradientPalette;
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
  using Plots   = std::vector<CQChartsPlot*>;
  using PlotSet = std::set<CQChartsPlot*>;

 public:
  static double viewportRange() { return 100.0; }

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
  void setAntiAlias(bool b) { antiAlias_ = b; }

  const PosTextType &posTextType() const { return posTextType_; }
  void setPosTextType(const PosTextType &v) { posTextType_ = v; }

  //---

  CQChartsTheme *theme() { return theme_; }
  const CQChartsTheme *theme() const { return theme_; }

  QString themeName() const { return (theme() ? theme()->name() : ""); }
  void setThemeName(const QString &name);

  CQChartsGradientPalette *themeGroupPalette(int i, int n) const;

  CQChartsGradientPalette *themePalette() const { return theme_->palette(); }
  CQChartsGradientPalette *themeTheme  () const { return theme_->theme  (); }

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

  //---

  void initOverlay();
  void initOverlay(const Plots &plots);
  void initOverlay(CQChartsPlot *firstPlot);

  void initY1Y2(CQChartsPlot *plot1, CQChartsPlot *plot2);

  //---

  void mousePressEvent  (QMouseEvent *me) override;
  void mouseMoveEvent   (QMouseEvent *me) override;
  void mouseReleaseEvent(QMouseEvent *me) override;

  void keyPressEvent(QKeyEvent *ke) override;

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

  void paint(QPainter *painter);

  //---

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

  void windowToPixel(double wx, double wy, double &px, double &py) const;
  void pixelToWindow(double px, double py, double &wx, double &wy) const;

  CQChartsGeom::Point windowToPixel(const CQChartsGeom::Point &w) const;
  CQChartsGeom::Point pixelToWindow(const CQChartsGeom::Point &p) const;

  CQChartsGeom::BBox windowToPixel(const CQChartsGeom::BBox &w) const;
  CQChartsGeom::BBox pixelToWindow(const CQChartsGeom::BBox &p) const;

  double pixelToWindowWidth (double pw) const;
  double pixelToWindowHeight(double ph) const;

  double windowToPixelWidth (double ww) const;
  double windowToPixelHeight(double wh) const;

  //---

  const CQChartsGeom::BBox prect() const { return prect_; }

  double aspect() const { return aspect_; }

  //---

  QSize sizeHint() const override;

 signals:
  void modeChanged();
  void selectModeChanged();

  void themeChanged();

  void statusTextChanged(const QString &text);

  void posTextChanged(const QString &text);

  void selTextChanged(const QString &text);

  void currentPlotChanged();

 public slots:
  void plotModelChanged();

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

  void defaultThemeSlot();
  void light1ThemeSlot();
  void light2ThemeSlot();
  void dark1ThemeSlot();
  void dark2ThemeSlot();

  void themeSlot(const QString &name);

  //---

  void printPNGSlot();
  void printSVGSlot();

  //---

  void showBoxesSlot(bool b);

  //---

  void currentPlotSlot();

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

  CQCharts*             charts_         { nullptr };
  CQChartsDisplayRange* displayRange_   { nullptr };
  CQChartsTheme*        theme_          { nullptr };
  CQPropertyViewModel*  propertyModel_  { nullptr };
  QString               id_;
  QString               title_;
  QColor                background_     { 255, 255, 255 };
  CQChartsViewKey*      keyObj_         { nullptr };
  PlotDatas             plotDatas_;
  int                   currentPlotInd_ { -1 };
  Mode                  mode_           { Mode::SELECT };
  SelectMode            selectMode_     { SelectMode::POINT };
  HighlightData         selectedHighlight_;
  HighlightData         insideHighlight_;
  bool                  zoomData_       { true };
  bool                  scrolled_       { false };
  double                scrollDelta_    { 100 };
  int                   scrollNumPages_ { 1 };
  int                   scrollPage_     { 0 };
  bool                  antiAlias_      { true };
  PosTextType           posTextType_    { PosTextType::PLOT };
  CQChartsGeom::BBox    prect_          { 0, 0, 100, 100 };
  double                aspect_         { 1.0 };
  MouseData             mouseData_;
  QTimer                searchTimer_;
  QPointF               searchPos_;
  QRubberBand*          regionBand_     { nullptr };
  ProbeBands            probeBands_;
  QMenu*                popupMenu_      { nullptr };
  ThemeType             themeType_      { ThemeType::NONE };
};

#endif
