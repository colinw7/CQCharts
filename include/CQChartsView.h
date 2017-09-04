#ifndef CQChartsView_H
#define CQChartsView_H

#include <QFrame>
#include <CDisplayRange2D.h>

class CQChartsPlot;
class CQChartsViewExpander;
class CQChartsViewStatus;
class CQChartsViewSettings;
class CQChartsViewToolBar;

class CQPropertyTree;
class CQGradientPalette;
class CQGradientPaletteControl;
class QToolButton;
class QRubberBand;
class QTabWidget;
class QLabel;

class CQChartsView : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QColor background READ background WRITE setBackground)

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

  CQPropertyTree *propertyTree() const;

  const QColor &background() const { return background_; }
  void setBackground(const QColor &v) { background_ = v; update(); }

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &v) { mode_ = v; }

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
  };

  class ProbeBand {
   public:
    ProbeBand(CQChartsView *view);
   ~ProbeBand();

    void show(const QString &text, double px, double py1, double py2);
    void hide();

   private:
    CQChartsView* view_  { nullptr };
    QRubberBand*  vband_ { nullptr };
    QRubberBand*  hband_ { nullptr };
    QLabel*       tip_   { nullptr };
  };

  typedef std::vector<ProbeBand *> ProbeBands;

  QWidget*              parent_        { nullptr };
  CDisplayRange2D       displayRange_;
  CQChartsViewExpander* expander_      { nullptr };
  CQChartsViewSettings* settings_      { nullptr };
  QColor                background_    { 255, 255, 255 };
  PlotDatas             plotDatas_;
  Mode                  mode_          { Mode::SELECT };
  CQChartsViewStatus*   status_        { nullptr };
  CQChartsViewToolBar*  toolbar_       { nullptr };
  CBBox2D               prect_         { 0, 0, 100, 100 };
  int                   toolBarHeight_ { 8 };
  int                   statusHeight_  { 8 };
  MouseData             mouseData_;
  QRubberBand*          zoomBand_      { nullptr };
  ProbeBands            probeBands_;
};

//---

class CQChartsViewExpander : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool expanded READ isExpanded WRITE setExpanded)

 public:
  CQChartsViewExpander(CQChartsView *view);

  bool isExpanded() const { return expanded_; }
  void setExpanded(bool b) { expanded_ = b; update(); }

  void mousePressEvent  (QMouseEvent *);
  void mouseMoveEvent   (QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);

  void paintEvent(QPaintEvent *);

 private:
  CQChartsView *view_     { nullptr };
  bool          expanded_ { false };
  bool          pressed_  { false };
  QPoint        pressPos_;
  QPoint        movePos_;
};

class CQChartsViewSettings : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewSettings(CQChartsView *view);

  CQPropertyTree *propertyTree() const { return propertyTree_; }

  CQGradientPalette *palettePlot() const { return palettePlot_; }

  CQGradientPaletteControl *paletteControl() const { return paletteControl_; }

 private:
  CQChartsView             *view_           { nullptr };
  QTabWidget               *tab_            { nullptr };
  CQPropertyTree           *propertyTree_   { nullptr };
  CQGradientPalette        *palettePlot_    { nullptr };
  CQGradientPaletteControl *paletteControl_ { nullptr };
};

//---

class CQChartsViewStatus : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString text READ text WRITE setText)

 public:
  CQChartsViewStatus(CQChartsView *view);

  const QString &text() const { return text_; }
  void setText(const QString &s);

  void paintEvent(QPaintEvent *);

  QSize sizeHint() const;

 private:
  CQChartsView *view_ { nullptr };
  QString       text_;
};

//---

class CQChartsViewToolBar : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewToolBar(CQChartsView *view);

  QSize sizeHint() const;

 private slots:
  void selectSlot(bool b);
  void zoomSlot(bool b);
  void probeSlot(bool b);
  void autoFitSlot();

 private:
  CQChartsView* view_          { nullptr };
  QToolButton*  selectButton_  { nullptr };
  QToolButton*  zoomButton_    { nullptr };
  QToolButton*  probeButton_   { nullptr };
  QToolButton*  autoFitButton_ { nullptr };
};

#endif
