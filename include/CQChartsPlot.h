#ifndef CQChartsPlot_H
#define CQChartsPlot_H

#include <CQChartsQuadTree.h>
#include <QFrame>
#include <CDisplayRange2D.h>
#include <CDisplayTransform2D.h>
#include <CSymbol2D.h>
#include <CRange2D.h>

class CQChartsPlotExpander;
class CQChartsAxis;
class CQChartsKey;
class CQChartsPlotObj;
class CQChartsPlotStatus;
class CQChartsPlotToolBar;

class CQPropertyTree;
class CGradientPalette;
class QAbstractItemModel;
class QToolButton;
class QRubberBand;

class CQChartsPlot : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QColor background READ background WRITE setBackground)

 public:
  enum class Mode {
    SELECT,
    ZOOM
  };

  typedef std::vector<int> Columns;

 public:
  CQChartsPlot(QWidget *parent, QAbstractItemModel *model);

  virtual ~CQChartsPlot();

  QAbstractItemModel *model() const { return model_; }

  const CRange2D &dataRange() const { return dataRange_; }
  void setDataRange(const CRange2D &r) { dataRange_ = r; }

  CGradientPalette *palette() const { return palette_; }
  void setPalette(CGradientPalette *palette) { palette_ = palette; update(); }

  const QColor &background() const { return background_; }
  void setBackground(const QColor &v) { background_ = v; update(); }

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &v) { mode_ = v; }

  void windowToPixel(double wx, double wy, double &px, double &py) const;
  void pixelToWindow(double px, double py, double &wx, double &wy) const;

  void windowToPixel(const CPoint2D &w, CPoint2D &p) const;
  void pixelToWindow(const CPoint2D &p, CPoint2D &w) const;

  void windowToPixel(const CBBox2D &wrect, CBBox2D &prect) const;
  void pixelToWindow(const CBBox2D &prect, CBBox2D &wrect) const;

  double pixelToWindowWidth (double pw) const;
  double pixelToWindowHeight(double ph) const;

  void addAxes();

  CQChartsKey *key() const { return key_; }

  void addKey();

  void applyDataRange();

  void addProperty(const QString &path, QObject *object,
                   const QString &name, const QString &alias="");

  void updateMargins();

  void addPlotObject(CQChartsPlotObj *obj);

  int numPlotObjects() const { return plotObjs_.size(); }

  void mousePressEvent  (QMouseEvent *me) override;
  void mouseMoveEvent   (QMouseEvent *me) override;
  void mouseReleaseEvent(QMouseEvent *me) override;

  void keyPressEvent(QKeyEvent *ke);

  void resizeEvent(QResizeEvent *) override;

  virtual void handleResize();

  void updateKeyPosition();

  bool tipText(const CPoint2D &p, QString &tip) const;

  void updateGeometry();

  void moveExpander(int dx);

  void drawBackground(QPainter *painter);

  void drawAxes(QPainter *painter);

  void drawKey(QPainter *painter);

  void drawSymbol(QPainter *painter, const CPoint2D &p, CSymbol2D::Type type, double s);

  QColor objectColor(CQChartsPlotObj *obj, int i, int n, const QColor &def=QColor(0,0,0)) const;

  QColor objectStateColor(CQChartsPlotObj *obj, const QColor &c) const;

  QColor paletteColor(int i, int n, const QColor &def=QColor(0,0,0)) const;

  QSize sizeHint() const;

 protected:
  typedef std::vector<CQChartsPlotObj *>            PlotObjs;
  typedef CQChartsQuadTree<CQChartsPlotObj,CBBox2D> PlotObjTree;

 protected:
  void objsAtPoint(const CPoint2D &p, PlotObjTree::DataList &dataList1) const;

 protected:
  struct MouseData {
    QPoint pressPoint;
    QPoint movePoint;
    bool   pressed { false };
  };

  QWidget*              parent_        { nullptr };
  QAbstractItemModel*   model_         { nullptr };
  CDisplayRange2D       displayRange_;
  CDisplayTransform2D   displayTransform_;
  CRange2D              dataRange_;
  CGradientPalette*     palette_       { nullptr };
  CQChartsPlotExpander* expander_      { nullptr };
  CQPropertyTree*       propertyTree_  { nullptr };
  QColor                background_    { 255, 255, 255 };
  CQChartsAxis*         xAxis_         { nullptr };
  CQChartsAxis*         yAxis_         { nullptr };
  CQChartsKey*          key_           { nullptr };
  PlotObjs              plotObjs_;
  PlotObjTree           plotObjTree_;
  Mode                  mode_          { Mode::SELECT };
  CQChartsPlotStatus*   status_        { nullptr };
  CQChartsPlotToolBar*  toolbar_       { nullptr };
  int                   toolBarHeight_ { 8 };
  int                   statusHeight_  { 8 };
  MouseData             mouseData_;
  QRubberBand*          zoomBand_      { nullptr };
};

//---

class CQChartsPlotExpander : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool expanded READ isExpanded WRITE setExpanded)

 public:
  CQChartsPlotExpander(CQChartsPlot *plot);

  bool isExpanded() const { return expanded_; }
  void setExpanded(bool b) { expanded_ = b; update(); }

  void mousePressEvent  (QMouseEvent *);
  void mouseMoveEvent   (QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);

  void paintEvent(QPaintEvent *);

 private:
  CQChartsPlot *plot_     { nullptr };
  bool          expanded_ { false };
  bool          pressed_  { false };
  QPoint        pressPos_;
  QPoint        movePos_;
};

//---

class CQChartsPlotStatus : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString text READ text WRITE setText)

 public:
  CQChartsPlotStatus(CQChartsPlot *plot);

  const QString &text() const { return text_; }
  void setText(const QString &s);

  void paintEvent(QPaintEvent *);

 private:
  CQChartsPlot *plot_ { nullptr };
  QString       text_;
};

//---

class CQChartsPlotToolBar : public QFrame {
  Q_OBJECT

 public:
  CQChartsPlotToolBar(CQChartsPlot *plot);

 private slots:
  void selectSlot(bool b);
  void zoomSlot(bool b);

 private:
  CQChartsPlot* plot_         { nullptr };
  QToolButton*  selectButton_ { nullptr };
  QToolButton*  zoomButton_   { nullptr };
};

#endif
