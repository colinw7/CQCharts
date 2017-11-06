#ifndef CQChartsView_H
#define CQChartsView_H

#include <QFrame>
#include <CBBox2D.h>

class CQCharts;
class CQChartsPlot;
class CQChartsProbeBand;

class CQPropertyViewModel;
class CGradientPalette;
class CDisplayRange2D;
class QToolButton;
class QRubberBand;
class QTabWidget;
class QLabel;

class CQChartsView : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString id             READ id             WRITE setId            )
  Q_PROPERTY(QString title          READ title          WRITE setTitle         )
  Q_PROPERTY(QColor  background     READ background     WRITE setBackground    )
  Q_PROPERTY(int     currentPlotInd READ currentPlotInd WRITE setCurrentPlotInd)
  Q_PROPERTY(Mode    mode           READ mode           WRITE setMode          )
  Q_PROPERTY(bool    zoomData       READ isZoomData     WRITE setZoomData      )

  Q_ENUMS(Mode)

 public:
  enum class Mode {
    SELECT,
    ZOOM,
    PROBE
  };

 public:
  using Plots = std::vector<CQChartsPlot *>;

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

  bool isZoomData() const { return zoomData_; }
  void setZoomData(bool b) { zoomData_ = b; }

  void updateMargins();

  //---

  CGradientPalette* gradientPalette() const { return palette_; }

  CQPropertyViewModel *propertyModel() const { return propertyModel_; }

  //---

  bool setProperty(const QString &name, const QVariant &value);
  bool getProperty(const QString &name, QVariant &value);

  void addProperty(const QString &path, QObject *object,
                   const QString &name, const QString &alias="");

  //---

  void addPlot(CQChartsPlot *plot, const CBBox2D &bbox=CBBox2D(0, 0, 1, 1));

  int numPlots() const { return plotDatas_.size(); }
  CQChartsPlot *plot(int i) { return plotDatas_[i].plot; }

  CQChartsPlot *getPlot(const QString &id) const;

  //---

  void initOverlay();

  //---

  void mousePressEvent  (QMouseEvent *me) override;
  void mouseMoveEvent   (QMouseEvent *me) override;
  void mouseReleaseEvent(QMouseEvent *me) override;

  void keyPressEvent(QKeyEvent *ke) override;

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

  //---

  CQChartsPlot *plotAt(const CPoint2D &p) const;

  bool plotsAt(const CPoint2D &p, Plots &plots) const;

  CBBox2D plotBBox(CQChartsPlot *plot) const;

  CQChartsPlot *currentPlot(bool remap=true) const;

  //---

  void setStatusText(const QString &text);

  //---

  void windowToPixel(double wx, double wy, double &px, double &py) const;
  void pixelToWindow(double px, double py, double &wx, double &wy) const;

  CPoint2D windowToPixel(const CPoint2D &w) const;
  CPoint2D pixelToWindow(const CPoint2D &p) const;

  CBBox2D windowToPixel(const CBBox2D &w) const;
  CBBox2D pixelToWindow(const CBBox2D &p) const;

  const CBBox2D prect() const { return prect_; }

  double aspect() const { return aspect_; }

  //---

  QSize sizeHint() const override;

 signals:
  void modeChanged();

  void statusTextChanged(const QString &text);

 private:
  struct PlotData {
    CQChartsPlot *plot { nullptr };
    CBBox2D       bbox;

    PlotData(CQChartsPlot *plot, const CBBox2D &bbox) :
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

  using ProbeBands = std::vector<CQChartsProbeBand *>;

  CQCharts*            charts_         { nullptr };
  CDisplayRange2D*     displayRange_   { nullptr };
  CGradientPalette*    palette_        { nullptr };
  CQPropertyViewModel* propertyModel_  { nullptr };
  QString              id_;
  QString              title_;
  QColor               background_     { 255, 255, 255 };
  PlotDatas            plotDatas_;
  int                  currentPlotInd_ { 0 };
  Mode                 mode_           { Mode::SELECT };
  bool                 zoomData_       { true };
  CBBox2D              prect_          { 0, 0, 100, 100 };
  double               aspect_         { 1.0 };
  MouseData            mouseData_;
  QRubberBand*         zoomBand_       { nullptr };
  ProbeBands           probeBands_;
};

#endif
