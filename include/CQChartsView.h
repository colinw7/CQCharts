#ifndef CQChartsView_H
#define CQChartsView_H

#include <QFrame>
#include <CDisplayRange2D.h>

class CQChartsPlot;
class CQChartsViewExpander;
class CQChartsViewStatus;
class CQChartsViewSettings;
class CQChartsViewToolBar;
class CQChartsProbeBand;

class CQPropertyView;
class CQGradientPalette;
class CQGradientPaletteControl;
class QToolButton;
class QRubberBand;
class QTabWidget;
class QLabel;

class CQChartsView : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QColor background READ background WRITE setBackground)
  Q_PROPERTY(Mode   mode       READ mode       WRITE setMode      )
  Q_PROPERTY(bool   zoomData   READ isZoomData WRITE setZoomData  )

  Q_ENUMS(Mode)

 public:
  enum class Mode {
    SELECT,
    ZOOM,
    PROBE
  };

 public:
  typedef std::vector<CQChartsPlot *> Plots;

 public:
  CQChartsView(QWidget *parent=nullptr);

  virtual ~CQChartsView();

  CQPropertyView *propertyView() const;

  const QColor &background() const { return background_; }
  void setBackground(const QColor &c) { background_ = c; update(); }

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &m);

  bool isZoomData() const { return zoomData_; }
  void setZoomData(bool b) { zoomData_ = b; }

  void updateMargins();

  void addProperty(const QString &path, QObject *object,
                   const QString &name, const QString &alias="");

  void addPlot(CQChartsPlot *plot, const CBBox2D &bbox=CBBox2D(0, 0, 1, 1));

  int numPlots() const { return plotDatas_.size(); }
  CQChartsPlot *plot(int i) { return plotDatas_[i].plot; }

  void mousePressEvent  (QMouseEvent *me) override;
  void mouseMoveEvent   (QMouseEvent *me) override;
  void mouseReleaseEvent(QMouseEvent *me) override;

  void keyPressEvent(QKeyEvent *ke);

  void resizeEvent(QResizeEvent *) override;

  void updateGeometry();

  void moveExpander(int dx);

  void paintEvent(QPaintEvent *) override;

  CQChartsPlot *plotAt(const CPoint2D &p) const;

  bool plotsAt(const CPoint2D &p, Plots &plots) const;

  CBBox2D plotBBox(CQChartsPlot *plot) const;

  CQChartsPlot *currentPlot() const;

  void setStatusText(const QString &text);

  void windowToPixel(double wx, double wy, double &px, double &py) const;
  void pixelToWindow(double px, double py, double &wx, double &wy) const;

  CPoint2D windowToPixel(const CPoint2D &w) const;
  CPoint2D pixelToWindow(const CPoint2D &p) const;

  CBBox2D windowToPixel(const CBBox2D &w) const;
  CBBox2D pixelToWindow(const CBBox2D &p) const;

  const CBBox2D prect() const { return prect_; }

  QSize sizeHint() const;

 private:
  struct PlotData {
    CQChartsPlot *plot { nullptr };
    CBBox2D       bbox;

    PlotData(CQChartsPlot *plot, const CBBox2D &bbox) :
     plot(plot), bbox(bbox) {
    }
  };

  typedef std::vector<PlotData> PlotDatas;

  struct MouseData {
    Plots  plots;
    QPoint pressPoint;
    QPoint movePoint;
    bool   pressed { false };
    bool   escape  { false };
  };

  typedef std::vector<CQChartsProbeBand *> ProbeBands;

  QWidget*              parent_        { nullptr };
  CDisplayRange2D       displayRange_;
  CQChartsViewExpander* expander_      { nullptr };
  CQChartsViewSettings* settings_      { nullptr };
  QColor                background_    { 255, 255, 255 };
  PlotDatas             plotDatas_;
  Mode                  mode_          { Mode::SELECT };
  bool                  zoomData_      { true };
  CQChartsViewStatus*   status_        { nullptr };
  CQChartsViewToolBar*  toolbar_       { nullptr };
  CBBox2D               prect_         { 0, 0, 100, 100 };
  int                   toolBarHeight_ { 8 };
  int                   statusHeight_  { 8 };
  MouseData             mouseData_;
  QRubberBand*          zoomBand_      { nullptr };
  ProbeBands            probeBands_;
};

#endif
