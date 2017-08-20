#ifndef CQChartsWindow_H
#define CQChartsWindow_H

#include <QFrame>
#include <CDisplayRange2D.h>

class CQChartsPlot;
class CQChartsWindowExpander;
class CQChartsWindowStatus;
class CQChartsWindowToolBar;

class CQPropertyTree;
class QToolButton;
class QRubberBand;

class CQChartsWindow : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QColor background READ background WRITE setBackground)

 public:
  enum class Mode {
    SELECT,
    ZOOM
  };

 public:
  CQChartsWindow(QWidget *parent=nullptr);

  virtual ~CQChartsWindow();

  CQPropertyTree *propertyTree() const { return propertyTree_; }

  const QColor &background() const { return background_; }
  void setBackground(const QColor &v) { background_ = v; update(); }

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &v) { mode_ = v; }

  void updateMargins();

  void addProperty(const QString &path, QObject *object,
                   const QString &name, const QString &alias="");

  void addPlot(CQChartsPlot *plot, const CBBox2D &bbox=CBBox2D(0, 0, 1, 1));

  int numPlots() const { return plots_.size(); }

  void mousePressEvent  (QMouseEvent *me) override;
  void mouseMoveEvent   (QMouseEvent *me) override;
  void mouseReleaseEvent(QMouseEvent *me) override;

  void keyPressEvent(QKeyEvent *ke);

  void resizeEvent(QResizeEvent *) override;

  void updateGeometry();

  void moveExpander(int dx);

  void paintEvent(QPaintEvent *) override;

  CQChartsPlot *plotAt(const CPoint2D &p) const;

  void setStatusText(const QString &text);

  void windowToPixel(double wx, double wy, double &px, double &py) const;
  void pixelToWindow(double px, double py, double &wx, double &wy) const;

  CPoint2D windowToPixel(const CPoint2D &w) const;
  CPoint2D pixelToWindow(const CPoint2D &p) const;

  QSize sizeHint() const;

 private:
  struct PlotData {
    CQChartsPlot *plot { nullptr };
    CBBox2D       bbox;

    PlotData(CQChartsPlot *plot, const CBBox2D &bbox) :
     plot(plot), bbox(bbox) {
    }
  };

  typedef std::vector<PlotData> Plots;

  struct MouseData {
    CQChartsPlot *plot { nullptr };
    QPoint        pressPoint;
    QPoint        movePoint;
    bool          pressed { false };
  };

  QWidget*                parent_        { nullptr };
  CDisplayRange2D         displayRange_;
  CQChartsWindowExpander* expander_      { nullptr };
  CQPropertyTree*         propertyTree_  { nullptr };
  QColor                  background_    { 255, 255, 255 };
  Plots                   plots_;
  Mode                    mode_          { Mode::SELECT };
  CQChartsWindowStatus*   status_        { nullptr };
  CQChartsWindowToolBar*  toolbar_       { nullptr };
  int                     toolBarHeight_ { 8 };
  int                     statusHeight_  { 8 };
  MouseData               mouseData_;
  QRubberBand*            zoomBand_      { nullptr };
};

//---

class CQChartsWindowExpander : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool expanded READ isExpanded WRITE setExpanded)

 public:
  CQChartsWindowExpander(CQChartsWindow *window);

  bool isExpanded() const { return expanded_; }
  void setExpanded(bool b) { expanded_ = b; update(); }

  void mousePressEvent  (QMouseEvent *);
  void mouseMoveEvent   (QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);

  void paintEvent(QPaintEvent *);

 private:
  CQChartsWindow *window_   { nullptr };
  bool            expanded_ { false };
  bool            pressed_  { false };
  QPoint          pressPos_;
  QPoint          movePos_;
};

//---

class CQChartsWindowStatus : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString text READ text WRITE setText)

 public:
  CQChartsWindowStatus(CQChartsWindow *window);

  const QString &text() const { return text_; }
  void setText(const QString &s);

  void paintEvent(QPaintEvent *);

 private:
  CQChartsWindow *window_ { nullptr };
  QString         text_;
};

//---

class CQChartsWindowToolBar : public QFrame {
  Q_OBJECT

 public:
  CQChartsWindowToolBar(CQChartsWindow *window);

 private slots:
  void selectSlot(bool b);
  void zoomSlot(bool b);

 private:
  CQChartsWindow* window_       { nullptr };
  QToolButton*    selectButton_ { nullptr };
  QToolButton*    zoomButton_   { nullptr };
};

#endif
