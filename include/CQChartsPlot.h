#ifndef CQChartsPlot_H
#define CQChartsPlot_H

#include <CQChartsQuadTree.h>
#include <QFrame>
#include <CDisplayRange2D.h>
#include <CDisplayTransform2D.h>
#include <CSymbol2D.h>
#include <CRange2D.h>
#include <COptVal.h>

class CQChartsView;
class CQChartsAxis;
class CQChartsKey;
class CQChartsTitle;
class CQChartsPlotObj;
class CQChartsModel;

class CGradientPalette;
class QAbstractItemModel;
class QRubberBand;

class CQChartsPlot : public QObject {
  Q_OBJECT

  Q_PROPERTY(QColor  background   READ background    WRITE setBackground  )
  Q_PROPERTY(QRectF  rect         READ rect          WRITE setRect        )
  Q_PROPERTY(QRectF  range        READ range         WRITE setRange       )
  Q_PROPERTY(double  marginLeft   READ marginLeft    WRITE setMarginLeft  )
  Q_PROPERTY(double  marginTop    READ marginTop     WRITE setMarginTop   )
  Q_PROPERTY(double  marginRight  READ marginRight   WRITE setMarginRight )
  Q_PROPERTY(double  marginBottom READ marginBottom  WRITE setMarginBottom)
  Q_PROPERTY(QString title        READ title         WRITE setTitle       )
  Q_PROPERTY(bool    followMouse  READ isFollowMouse WRITE setFollowMouse )
  Q_PROPERTY(bool    showBoxes    READ showBoxes     WRITE setShowBoxes   )

 public:
  typedef std::vector<int> Columns;

  // margin (percent)
  struct Margin {
    double left   { 10 };
    double top    { 10 };
    double right  { 10 };
    double bottom { 10 };
  };

  struct OtherPlot {
    CQChartsPlot *plot { nullptr };
    int           ind  { -1 };

    OtherPlot(CQChartsPlot *plot=nullptr, int ind=-1) :
     plot(plot), ind(ind) {
    }
  };

 public:
  CQChartsPlot(CQChartsView *view, QAbstractItemModel *model);

  virtual ~CQChartsPlot();

  CQChartsView *view() const { return view_; }

  QAbstractItemModel *model() const { return model_; }

  CQChartsModel *chartsModel() const;

  virtual const char *typeName() const = 0;

  const QString &id() const { return id_; }
  void setId(const QString &v) { id_ = v; }

  const CDisplayRange2D &displayRange() const { return displayRange_; }
  void setDisplayRange(const CDisplayRange2D &v) { displayRange_ = v; }

  const CDisplayTransform2D &displayTransform() const { return displayTransform_; }
  void setDisplayTransform(const CDisplayTransform2D &v) { displayTransform_ = v; }

  const CRange2D &dataRange() const { return dataRange_; }
  void setDataRange(const CRange2D &r) { dataRange_ = r; }

  CGradientPalette *palette() const { return palette_; }
  void setPalette(CGradientPalette *palette) { palette_ = palette; update(); }

  const COptReal &xmin() const { return xmin_; }
  void setXMin(const COptReal &r) { xmin_ = r; }

  const COptReal &xmax() const { return xmax_; }
  void setXMax(const COptReal &r) { xmax_ = r; }

  const COptReal &ymin() const { return ymin_; }
  void setYMin(const COptReal &r) { ymin_ = r; }

  const COptReal &ymax() const { return ymax_; }
  void setYMax(const COptReal &r) { ymax_ = r; }

  const QColor &background() const { return background_; }
  void setBackground(const QColor &v) { background_ = v; update(); }

  const QString &title() const { return title_; }
  void setTitle(const QString &v);

  bool isFollowMouse() const { return followMouse_; }
  void setFollowMouse(bool b) { followMouse_ = b; }

  bool showBoxes() const { return showBoxes_; }
  void setShowBoxes(bool b) { showBoxes_ = b; }

  const CBBox2D &bbox() const { return bbox_; }
  void setBBox(const CBBox2D &bbox);

  QRectF rect() const;
  void setRect(const QRectF &r);

  QRectF range() const;
  void setRange(const QRectF &r);

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

  CQChartsAxis *xAxis() const { return xAxis_; }
  CQChartsAxis *yAxis() const { return yAxis_; }

  CQChartsKey *key() const { return keyObj_; }

  CQChartsTitle *titleObj() const { return titleObj_; }

  CQChartsPlot *rootPlot() const { return rootPlot_; }

  void setRootPlot(CQChartsPlot *plot) {
    assert(! rootPlot_);

    rootPlot_ = plot;
  }

  void addRefPlot(CQChartsPlot *plot) {
    assert(plot != this);

    refPlots_.push_back(plot);
  }

  CQChartsPlot *otherPlot() const { return otherPlot_.plot; }

  int otherPlotInd() const { return otherPlot_.ind; }

  void setOtherPlot(CQChartsPlot *plot, int ind) {
    assert(! otherPlot_.plot);

    otherPlot_ = OtherPlot(plot, ind);
  }

  virtual void addProperties();

  virtual void addKeyItems(CQChartsKey *) { }

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

  void addXAxis();
  void addYAxis();

  void addKey();

  void addTitle();

  void applyDataRange(bool propagate=true);

  void applyDisplayTransform(bool propagate=true);

  void addProperty(const QString &path, QObject *object,
                   const QString &name, const QString &alias="");

  void addPlotObject(CQChartsPlotObj *obj);

  void clearPlotObjects();

  int numPlotObjects() const { return plotObjs_.size(); }

  void setXValueColumn(int column);
  void setYValueColumn(int column);

  QString xStr(double x) const;
  QString yStr(double x) const;

  bool mousePress  (const CPoint2D &p);
  void mouseMove   (const CPoint2D &p);
  void mouseRelease(const CPoint2D &p);

  void keyPress(int key);

  void panLeft ();
  void panRight();
  void panUp   ();
  void panDown ();

  void zoomTo(const CBBox2D &bbox);
  void zoomIn();
  void zoomOut();
  void zoomFull();

  void updateTransform();

  virtual bool interpY(double /*x*/, std::vector<double> & /*y*/) const { return false; }

  virtual void handleResize();

  void updateKeyPosition(bool force=false);

  void updateTitlePosition();

  bool tipText(const CPoint2D &p, QString &tip) const;

  void drawBackground(QPainter *painter);

  QRectF calcRect() const;

  CBBox2D calcPixelRect() const;

  void autoFit();

  void drawObjs(QPainter *painter);

  void drawBgAxes(QPainter *painter);
  void drawFgAxes(QPainter *painter);

  void drawKey(QPainter *painter);

  void drawTitle(QPainter *painter);

  void drawSymbol(QPainter *painter, const CPoint2D &p, CSymbol2D::Type type,
                  double s, const QColor &c, bool filled=false);

  QColor objectColor(CQChartsPlotObj *obj, int i, int n, const QColor &def=QColor(0,0,0)) const;

  QColor objectStateColor(CQChartsPlotObj *obj, const QColor &c) const;

  virtual QColor paletteColor(int i, int n, const QColor &def=QColor(0,0,0)) const;

  QColor interpPaletteColor(double r, const QColor &def=QColor(0,0,0)) const;

  QColor textColor(const QColor &bg) const;

  void update();

  virtual void draw(QPainter *p) = 0;

 signals:
  void objPressed(CQChartsPlotObj *);

 protected:
  typedef std::vector<CQChartsPlotObj *>            PlotObjs;
  typedef CQChartsQuadTree<CQChartsPlotObj,CBBox2D> PlotObjTree;

 protected:
  void objsAtPoint(const CPoint2D &p, PlotObjTree::DataList &dataList1) const;

 protected:
  typedef std::vector<CQChartsPlot *> RefPlots;

  struct MouseData {
    QPoint pressPoint;
    QPoint movePoint;
    bool   pressed { false };
  };

  CQChartsView*         view_          { nullptr };
  QAbstractItemModel*   model_         { nullptr };
  QString               id_;
  CBBox2D               bbox_          { 0, 0, 1000, 1000 };
  Margin                margin_;
  CDisplayRange2D       displayRange_;
  CDisplayTransform2D   displayTransform_;
  CRange2D              dataRange_;
  COptReal              xmin_, ymin_, xmax_, ymax_;
  CGradientPalette*     palette_       { nullptr };
  QColor                background_    { 255, 255, 255 };
  QString               title_;
  CQChartsAxis*         xAxis_         { nullptr };
  CQChartsAxis*         yAxis_         { nullptr };
  CQChartsKey*          keyObj_        { nullptr };
  CQChartsTitle*        titleObj_      { nullptr };
  int                   xValueColumn_  { -1 };
  int                   yValueColumn_  { -1 };
  bool                  followMouse_   { true };
  bool                  showBoxes_     { false };
  CQChartsPlot*         rootPlot_      { nullptr };
  OtherPlot             otherPlot_;
  RefPlots              refPlots_;
  PlotObjs              plotObjs_;
  PlotObjTree           plotObjTree_;
  MouseData             mouseData_;
};

#endif
