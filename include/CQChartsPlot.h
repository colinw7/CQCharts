#ifndef CQChartsPlot_H
#define CQChartsPlot_H

#include <CQChartsQuadTree.h>
#include <QFrame>
#include <CDisplayRange2D.h>
#include <CDisplayTransform2D.h>
#include <CSymbol2D.h>
#include <CRange2D.h>

class CQChartsWindow;
class CQChartsAxis;
class CQChartsKey;
class CQChartsPlotObj;

class CGradientPalette;
class QAbstractItemModel;
class QRubberBand;

class CQChartsPlot : public QObject {
  Q_OBJECT

  Q_PROPERTY(QColor background   READ background   WRITE setBackground  )
  Q_PROPERTY(QRectF rect         READ rect         WRITE setRect        )
  Q_PROPERTY(double marginLeft   READ marginLeft   WRITE setMarginLeft  )
  Q_PROPERTY(double marginTop    READ marginTop    WRITE setMarginTop   )
  Q_PROPERTY(double marginRight  READ marginRight  WRITE setMarginRight )
  Q_PROPERTY(double marginBottom READ marginBottom WRITE setMarginBottom)

 public:
  enum class Mode {
    SELECT,
    ZOOM
  };

  typedef std::vector<int> Columns;

  // margin (percent)
  struct Margin {
    double left   { 10 };
    double top    { 10 };
    double right  { 10 };
    double bottom { 10 };
  };

 public:
  CQChartsPlot(CQChartsWindow *window, QAbstractItemModel *model);

  virtual ~CQChartsPlot();

  CQChartsWindow *window() const { return window_; }

  QAbstractItemModel *model() const { return model_; }

  const QString &id() const { return id_; }
  void setId(const QString &v) { id_ = v; }

  const CRange2D &dataRange() const { return dataRange_; }
  void setDataRange(const CRange2D &r) { dataRange_ = r; }

  CGradientPalette *palette() const { return palette_; }
  void setPalette(CGradientPalette *palette) { palette_ = palette; update(); }

  const QColor &background() const { return background_; }
  void setBackground(const QColor &v) { background_ = v; update(); }

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &v) { mode_ = v; }

  const CBBox2D &bbox() const { return bbox_; }
  void setBBox(const CBBox2D &bbox);

  QRectF rect() const;
  void setRect(const QRectF &r);

  double marginLeft() const { return margin_.left; }
  void setMarginLeft(double r) { margin_.left = r; updateMargin(); }

  double marginTop() const { return margin_.top; }
  void setMarginTop(double r) { margin_.top = r; updateMargin(); }

  double marginRight() const { return margin_.right; }
  void setMarginRight(double r) { margin_.right = r; updateMargin(); }

  double marginBottom() const { return margin_.bottom; }
  void setMarginBottom(double r) { margin_.bottom = r; updateMargin(); }

  void setMargins(double l, double t, double r, double b) {
    margin_.left   = l;
    margin_.top    = t;
    margin_.right  = r;
    margin_.bottom = b;

    updateMargin();
  }

  CQChartsKey *key() const { return key_; }

  virtual void addProperties();

  void updateMargin();

  void windowToPixel(double wx, double wy, double &px, double &py) const;
  void pixelToWindow(double px, double py, double &wx, double &wy) const;

  void windowToPixel(const CPoint2D &w, CPoint2D &p) const;
  void pixelToWindow(const CPoint2D &p, CPoint2D &w) const;

  void windowToPixel(const CBBox2D &wrect, CBBox2D &prect) const;
  void pixelToWindow(const CBBox2D &prect, CBBox2D &wrect) const;

  double pixelToWindowWidth (double pw) const;
  double pixelToWindowHeight(double ph) const;

  double windowToPixelWidth (double ww) const;
  double windowToPixelHeight(double wh) const;

  void addAxes();

  void addKey();

  void applyDataRange();

  void addProperty(const QString &path, QObject *object,
                   const QString &name, const QString &alias="");

  void addPlotObject(CQChartsPlotObj *obj);

  void clearPlotObjects();

  int numPlotObjects() const { return plotObjs_.size(); }

  void mousePress  (const CPoint2D &p);
  void mouseMove   (const CPoint2D &p);
  void mouseRelease(const CPoint2D &p);

  void keyPress(int key);

  void zoomTo(const CBBox2D &bbox);

  virtual void handleResize();

  void updateKeyPosition();

  bool tipText(const CPoint2D &p, QString &tip) const;

  void drawBackground(QPainter *painter);

  void drawAxes(QPainter *painter);

  void drawKey(QPainter *painter);

  void drawSymbol(QPainter *painter, const CPoint2D &p, CSymbol2D::Type type, double s);

  QColor objectColor(CQChartsPlotObj *obj, int i, int n, const QColor &def=QColor(0,0,0)) const;

  QColor objectStateColor(CQChartsPlotObj *obj, const QColor &c) const;

  QColor paletteColor(int i, int n, const QColor &def=QColor(0,0,0)) const;

  void update();

  virtual void draw(QPainter *p) = 0;

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

  CQChartsWindow*       window_        { nullptr };
  QAbstractItemModel*   model_         { nullptr };
  QString               id_;
  CBBox2D               bbox_          { 0, 0, 1000, 1000 };
  Margin                margin_;
  CDisplayRange2D       displayRange_;
  CDisplayTransform2D   displayTransform_;
  CRange2D              dataRange_;
  CGradientPalette*     palette_       { nullptr };
  QColor                background_    { 255, 255, 255 };
  CQChartsAxis*         xAxis_         { nullptr };
  CQChartsAxis*         yAxis_         { nullptr };
  CQChartsKey*          key_           { nullptr };
  PlotObjs              plotObjs_;
  PlotObjTree           plotObjTree_;
  Mode                  mode_          { Mode::SELECT };
  MouseData             mouseData_;
  QRubberBand*          zoomBand_      { nullptr };
};

#endif
