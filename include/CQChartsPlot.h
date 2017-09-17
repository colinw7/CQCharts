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
class CQPropertyView;

class CGradientPalette;
class QAbstractItemModel;
class QRubberBand;

class CQChartsPlot : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool    visible             READ isVisible           WRITE setVisible            )
  Q_PROPERTY(QRectF  rect                READ rect                WRITE setRect               )
  Q_PROPERTY(QRectF  range               READ range               WRITE setRange              )
  Q_PROPERTY(double  marginLeft          READ marginLeft          WRITE setMarginLeft         )
  Q_PROPERTY(double  marginTop           READ marginTop           WRITE setMarginTop          )
  Q_PROPERTY(double  marginRight         READ marginRight         WRITE setMarginRight        )
  Q_PROPERTY(double  marginBottom        READ marginBottom        WRITE setMarginBottom       )
  Q_PROPERTY(QString title               READ title               WRITE setTitle              )

  // plot area
  Q_PROPERTY(bool    background          READ isBackground        WRITE setBackground         )
  Q_PROPERTY(QColor  backgroundColor     READ backgroundColor     WRITE setBackgroundColor    )
  Q_PROPERTY(bool    border              READ isBorder            WRITE setBorder             )
  Q_PROPERTY(QColor  borderColor         READ borderColor         WRITE setBorderColor        )
  Q_PROPERTY(double  borderWidth         READ borderWidth         WRITE setBorderWidth        )
  Q_PROPERTY(QString borderSides         READ borderSides         WRITE setBorderSides        )
  Q_PROPERTY(bool    clip                READ isClip              WRITE setClip               )

  // data area
  Q_PROPERTY(bool    dataBackground      READ isDataBackground    WRITE setDataBackground     )
  Q_PROPERTY(QColor  dataBackgroundColor READ dataBackgroundColor WRITE setDataBackgroundColor)
  Q_PROPERTY(bool    dataBorder          READ isDataBorder        WRITE setDataBorder         )
  Q_PROPERTY(QColor  dataBorderColor     READ dataBorderColor     WRITE setDataBorderColor    )
  Q_PROPERTY(double  dataBorderWidth     READ dataBorderWidth     WRITE setDataBorderWidth    )
  Q_PROPERTY(QString dataBorderSides     READ dataBorderSides     WRITE setDataBorderSides    )
  Q_PROPERTY(bool    dataClip            READ isDataClip          WRITE setDataClip           )

  Q_PROPERTY(bool    equalScale          READ isEqualScale        WRITE setEqualScale         )
  Q_PROPERTY(bool    followMouse         READ isFollowMouse       WRITE setFollowMouse        )
  Q_PROPERTY(bool    overlay             READ isOverlay           WRITE setOverlay            )
  Q_PROPERTY(bool    showBoxes           READ showBoxes           WRITE setShowBoxes          )

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
    CQChartsPlot *next { nullptr };
    CQChartsPlot *prev { nullptr };

    OtherPlot() { }
  };

 public:
  CQChartsPlot(CQChartsView *view, QAbstractItemModel *model);

  virtual ~CQChartsPlot();

  CQChartsView *view() const { return view_; }

  QAbstractItemModel *model() const { return model_; }

  CQChartsModel *chartsModel() const;

  virtual const char *typeName() const = 0;

  const QString &id() const { return id_; }
  void setId(const QString &s) { id_ = s; }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; update(); }

  //---

  const CDisplayRange2D &displayRange() const { return displayRange_; }
  void setDisplayRange(const CDisplayRange2D &r) { displayRange_ = r; }

  const CDisplayTransform2D &displayTransform() const { return displayTransform_; }
  void setDisplayTransform(const CDisplayTransform2D &t) { displayTransform_ = t; }

  const CRange2D &dataRange() const { return dataRange_; }
  void setDataRange(const CRange2D &r) { dataRange_ = r; }

  double dataScale() const { return dataScale_; }
  void setDataScale(double r) { dataScale_ = r; }

  const CPoint2D &dataOffset() const { return dataOffset_; }
  void setDataOffset(const CPoint2D &o) { dataOffset_ = o; }

  //---

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

  const QString &title() const { return title_; }
  void setTitle(const QString &s);

  //---

  bool isBackground() const { return background_; }
  void setBackground(bool b) { background_ = b; update(); }

  const QColor &backgroundColor() const { return backgroundColor_; }
  void setBackgroundColor(const QColor &c) { backgroundColor_ = c; update(); }

  bool isBorder() const { return border_; }
  void setBorder(bool b) { border_ = b; update(); }

  const QColor &borderColor() const { return borderColor_; }
  void setBorderColor(const QColor &c) { borderColor_ = c; update(); }

  double borderWidth() const { return borderWidth_; }
  void setBorderWidth(double r) { borderWidth_ = r; update(); }

  const QString &borderSides() const { return borderSides_; }
  void setBorderSides(const QString &s) { borderSides_ = s; update(); }

  bool isClip() const { return clip_; }
  void setClip(bool b) { clip_ = b; update(); }

  //---

  bool isDataBackground() const { return dataBackground_; }
  void setDataBackground(bool b) { dataBackground_ = b; update(); }

  const QColor &dataBackgroundColor() const { return dataBackgroundColor_; }
  void setDataBackgroundColor(const QColor &c) { dataBackgroundColor_ = c; update(); }

  bool isDataBorder() const { return dataBorder_; }
  void setDataBorder(bool b) { dataBorder_ = b; update(); }

  const QColor &dataBorderColor() const { return dataBorderColor_; }
  void setDataBorderColor(const QColor &c) { dataBorderColor_ = c; update(); }

  double dataBorderWidth() const { return dataBorderWidth_; }
  void setDataBorderWidth(double r) { dataBorderWidth_ = r; update(); }

  const QString &dataBorderSides() const { return dataBorderSides_; }
  void setDataBorderSides(const QString &s) { dataBorderSides_ = s; update(); }

  bool isDataClip() const { return dataClip_; }
  void setDataClip(bool b) { dataClip_ = b; update(); }

  //---

  bool isEqualScale() const { return equalScale_; }
  void setEqualScale(bool b);

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

  //---

  bool isOverlay() const { return overlay_; }
  void setOverlay(bool b) { overlay_ = b; }

  CQChartsPlot *prevPlot() const { return otherPlot_.prev; }
  CQChartsPlot *nextPlot() const { return otherPlot_.next; }

  void setNextPlot(CQChartsPlot *plot) {
    assert(! otherPlot_.next);

    otherPlot_.next = plot;
  }

  void setPrevPlot(CQChartsPlot *plot) {
    assert(! otherPlot_.prev);

    otherPlot_.prev = plot;
  }

  CQChartsPlot *firstPlot() {
    if (otherPlot_.prev)
      return otherPlot_.prev->firstPlot();

    return this;
  }

  CQChartsPlot *lastPlot() {
    if (otherPlot_.next)
      return otherPlot_.next->lastPlot();

    return this;
  }

  //---

  CQPropertyView *propertyView() const;

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

  CBBox2D calcDataRange() const;

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
  bool mouseMove   (const CPoint2D &p);
  void mouseRelease(const CPoint2D &p);

  void keyPress(int key);

  void panLeft ();
  void panRight();
  void panUp   ();
  void panDown ();

  void zoomTo(const CBBox2D &bbox);
  void zoomIn(double f=1.5);
  void zoomOut(double f=1.5);
  void zoomFull();

  void updateTransform();

  virtual bool interpY(double /*x*/, std::vector<double> & /*y*/) const { return false; }

  virtual void handleResize();

  void updateKeyPosition(bool force=false);

  void updateTitlePosition();

  bool tipText(const CPoint2D &p, QString &tip) const;

  void drawBackground(QPainter *painter);

  void drawSides(QPainter *painter, const QRectF &rect, const QString &sides,
                 double width, const QColor &color);

  QRectF calcRect() const;

  CBBox2D calcPixelRect() const;

  void autoFit();

  void setFixBBox(const CBBox2D &bbox);

  CBBox2D fitBBox() const;

  void drawParts(QPainter *painter);

  virtual void drawObjs(QPainter *painter);

  void drawBgAxes(QPainter *painter);
  void drawFgAxes(QPainter *painter);

  void drawBgKey(QPainter *painter);
  void drawFgKey(QPainter *painter);

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

  CQChartsView*         view_                { nullptr };
  QAbstractItemModel*   model_               { nullptr };
  QString               id_;
  bool                  visible_             { true };
  CBBox2D               bbox_                { 0, 0, 1000, 1000 };
  Margin                margin_;
  CDisplayRange2D       displayRange_;
  CDisplayTransform2D   displayTransform_;
  CRange2D              dataRange_;
  double                dataScale_           { 1.0 };
  CPoint2D              dataOffset_          { 0.0, 0.0 };
  COptReal              xmin_;
  COptReal              ymin_;
  COptReal              xmax_;
  COptReal              ymax_;
  CGradientPalette*     palette_             { nullptr };
  bool                  background_          { true };
  QColor                backgroundColor_     { 255, 255, 255 };
  bool                  border_              { false };
  QColor                borderColor_         { 0, 0, 0 };
  double                borderWidth_         { 0.0 };
  QString               borderSides_         { "tlbr" };
  bool                  clip_                { true };
  bool                  dataBackground_      { true };
  QColor                dataBackgroundColor_ { 255, 255, 255 };
  bool                  dataBorder_          { false };
  QColor                dataBorderColor_     { 0, 0, 0 };
  double                dataBorderWidth_     { 0.0 };
  QString               dataBorderSides_     { "tlbr" };
  bool                  dataClip_            { false };
  QString               title_;
  CQChartsAxis*         xAxis_               { nullptr };
  CQChartsAxis*         yAxis_               { nullptr };
  CQChartsKey*          keyObj_              { nullptr };
  CQChartsTitle*        titleObj_            { nullptr };
  int                   xValueColumn_        { -1 };
  int                   yValueColumn_        { -1 };
  bool                  equalScale_          { false };
  bool                  followMouse_         { true };
  bool                  showBoxes_           { false };
  bool                  overlay_             { false };
  OtherPlot             otherPlot_;
  PlotObjs              plotObjs_;
  PlotObjTree           plotObjTree_;
  MouseData             mouseData_;
};

#endif
