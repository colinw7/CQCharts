#ifndef CQChartsView_H
#define CQChartsView_H

#include <QFrame>
#include <CQChartsGeom.h>

class CQCharts;
class CQChartsPlot;
class CQChartsProbeBand;

class CQPropertyViewModel;
class CGradientPalette;
class CQChartsDisplayRange;
class QToolButton;
class QRubberBand;
class QLabel;
class QMenu;

class CQChartsView : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString      id             READ id             WRITE setId            )
  Q_PROPERTY(QString      title          READ title          WRITE setTitle         )
  Q_PROPERTY(QColor       background     READ background     WRITE setBackground    )
  Q_PROPERTY(int          currentPlotInd READ currentPlotInd WRITE setCurrentPlotInd)
  Q_PROPERTY(Mode         mode           READ mode           WRITE setMode          )
  Q_PROPERTY(SelectedMode selectedMode   READ selectedMode   WRITE setSelectedMode  )
  Q_PROPERTY(InsideMode   insideMode     READ insideMode     WRITE setInsideMode    )
  Q_PROPERTY(bool         zoomData       READ isZoomData     WRITE setZoomData      )
  Q_PROPERTY(bool         scrolled       READ isScrolled     WRITE setScrolled      )
  Q_PROPERTY(double       scrollDelta    READ scrollDelta    WRITE setScrollDelta   )
  Q_PROPERTY(int          scrollNumPages READ scrollNumPages WRITE setScrollNumPages)
  Q_PROPERTY(int          scrollPage     READ scrollPage     WRITE setScrollPage    )

  Q_ENUMS(Mode)
  Q_ENUMS(SelectedMode)
  Q_ENUMS(InsideMode)

 public:
  enum class Mode {
    SELECT,
    ZOOM,
    PROBE
  };

  enum class SelectedMode {
    OUTLINE,
    FILL
  };

  enum class InsideMode {
    OUTLINE,
    FILL
  };

 public:
  using Plots = std::vector<CQChartsPlot*>;

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
  void setCurrentPlotInd(int i) { currentPlotInd_ = i; }

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &m);

  const SelectedMode &selectedMode() const { return selectedMode_; }
  void setSelectedMode(const SelectedMode &mode) { selectedMode_ = mode; }

  const InsideMode &insideMode() const { return insideMode_; }
  void setInsideMode(const InsideMode &mode) { insideMode_ = mode; }

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

  //---

  CGradientPalette *gradientPalette() const { return palette_; }
  CGradientPalette *themePalette   () const { return theme_  ; }

  CQPropertyViewModel *propertyModel() const { return propertyModel_; }

  //---

  bool setProperty(const QString &name, const QVariant &value);
  bool getProperty(const QString &name, QVariant &value);

  void addProperty(const QString &path, QObject *object,
                   const QString &name, const QString &alias="");

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

  //---

  void showMenu(const QPoint &p);

  //---

  void updatePlots();

  //---

  CQChartsPlot *plotAt(const CQChartsGeom::Point &p) const;

  bool plotsAt(const CQChartsGeom::Point &p, Plots &plots) const;

  CQChartsGeom::BBox plotBBox(CQChartsPlot *plot) const;

  CQChartsPlot *currentPlot(bool remap=true) const;

  //---

  virtual void setStatusText(const QString &text);

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

  const CQChartsGeom::BBox prect() const { return prect_; }

  double aspect() const { return aspect_; }

  //---

  QSize sizeHint() const override;

 signals:
  void modeChanged();

  void statusTextChanged(const QString &text);

 public slots:
  void keySlot(bool b);

  void fitSlot();

  void lightTheme1Slot();
  void lightTheme2Slot();

  void darkTheme1Slot();
  void darkTheme2Slot();

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
    Plots  plots;
    QPoint pressPoint;
    QPoint movePoint;
    bool   pressed   { false };
    bool   escape    { false };
    bool   clickZoom { false };

    void reset() {
      plots.clear();

      pressed   = false;
      escape    = false;
      clickZoom = false;
    }
  };

  using ProbeBands = std::vector<CQChartsProbeBand*>;

  CQCharts*             charts_         { nullptr };
  CQChartsDisplayRange* displayRange_   { nullptr };
  CGradientPalette*     palette_        { nullptr };
  CGradientPalette*     theme_          { nullptr };
  CQPropertyViewModel*  propertyModel_  { nullptr };
  QString               id_;
  QString               title_;
  QColor                background_     { 255, 255, 255 };
  PlotDatas             plotDatas_;
  int                   currentPlotInd_ { 0 };
  Mode                  mode_           { Mode::SELECT };
  SelectedMode          selectedMode_   { SelectedMode::OUTLINE };
  InsideMode            insideMode_     { InsideMode::OUTLINE };
  bool                  zoomData_       { true };
  bool                  scrolled_       { false };
  double                scrollDelta_    { 100 };
  int                   scrollNumPages_ { 1 };
  int                   scrollPage_     { 0 };
  CQChartsGeom::BBox    prect_          { 0, 0, 100, 100 };
  double                aspect_         { 1.0 };
  MouseData             mouseData_;
  QRubberBand*          zoomBand_       { nullptr };
  ProbeBands            probeBands_;
  QMenu*                popupMenu_      { nullptr };
};

#endif
