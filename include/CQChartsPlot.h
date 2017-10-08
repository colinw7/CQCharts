#ifndef CQChartsPlot_H
#define CQChartsPlot_H

#include <CQChartsPlotParameter.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsQuadTree.h>
#include <CQChartsBoxObj.h>

#include <CDisplayRange2D.h>
#include <CDisplayTransform2D.h>
#include <CRange2D.h>

#include <QPointer>
#include <QAbstractItemModel>
#include <QFrame>

#include <boost/optional.hpp>

class CQCharts;
class CQChartsView;
class CQChartsAxis;
class CQChartsKey;
class CQChartsTitle;
class CQChartsPlotObj;
class CQPropertyViewTree;

class CGradientPalette;
class QAbstractItemModel;
class QRubberBand;

//----

class CQChartsPlotType;

class CQChartsPlotTypeMgr {
 public:
  CQChartsPlotTypeMgr();
 ~CQChartsPlotTypeMgr();

  void addType(const QString &name, CQChartsPlotType *type);

  bool isType(const QString &name) const;

  CQChartsPlotType *type(const QString &name) const;

  void getTypeNames(QStringList &names, QStringList &descs) const;

 private:
  typedef std::map<QString, CQChartsPlotType *> Types;

  Types types_;
};

//----

class CQChartsPlotType {
 public:
  typedef std::vector<CQChartsPlotParameter> Parameters;

 public:
  typedef QSharedPointer<QAbstractItemModel> ModelP;

 public:
  CQChartsPlotType() { }

  virtual ~CQChartsPlotType() { }

  virtual QString name() const = 0;
  virtual QString desc() const = 0;

  const Parameters &parameters() const { return parameters_; }

  void addColumnParameter(const QString &name, const QString &desc, const QString &propName,
                          const QString &attributes="", int defValue=-1) {
    addParameter(CQChartsColumnParameter(name, desc, propName, attributes, defValue));
  }

  void addColumnsParameter(const QString &name, const QString &desc, const QString &propName,
                           const QString &attributes="", const QString &defValue="") {
    addParameter(CQChartsColumnsParameter(name, desc, propName, attributes, defValue));
  }

  void addBoolParameter(const QString &name, const QString &desc, const QString &propName,
                        const QString &attributes="", bool defValue=false) {
    addParameter(CQChartsBoolParameter(name, desc, propName, attributes, defValue));
  }

  void addParameter(const CQChartsPlotParameter &parameter) {
    parameters_.push_back(parameter);
  }

  virtual CQChartsPlot *create(CQChartsView *view, const ModelP &model) const = 0;

 protected:
  Parameters parameters_;
};

//----

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
  enum Layer {
    BG,
    MID,
    FG
  };

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

  typedef boost::optional<double> OptReal;

  typedef QSharedPointer<QAbstractItemModel> ModelP;

 public:
  CQChartsPlot(CQChartsView *view, CQChartsPlotType *type, const ModelP &model);

  virtual ~CQChartsPlot();

  CQChartsView *view() const { return view_; }

  QAbstractItemModel *model() const { return model_.data(); }

  CQCharts *charts() const;

  QString typeName() const { return type_->name(); }

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

  const OptReal &xmin() const { return xmin_; }
  void setXMin(const OptReal &r) { xmin_ = r; initObjs(/*force*/ true); }

  const OptReal &xmax() const { return xmax_; }
  void setXMax(const OptReal &r) { xmax_ = r; initObjs(/*force*/ true); }

  const OptReal &ymin() const { return ymin_; }
  void setYMin(const OptReal &r) { ymin_ = r; initObjs(/*force*/ true); }

  const OptReal &ymax() const { return ymax_; }
  void setYMax(const OptReal &r) { ymax_ = r; initObjs(/*force*/ true); }

  const QString &title() const { return title_; }
  void setTitle(const QString &s);

  //---

  // plot area
  bool isBackground() const { return borderObj_.isBackground(); }
  void setBackground(bool b) { borderObj_.setBackground(b); update(); }

  const QColor &backgroundColor() const { return borderObj_.backgroundColor(); }
  void setBackgroundColor(const QColor &c) { borderObj_.setBackgroundColor(c); update(); }

  bool isBorder() const { return borderObj_.isBorder(); }
  void setBorder(bool b) { borderObj_.setBorder(b); update(); }

  const QColor &borderColor() const { return borderObj_.borderColor(); }
  void setBorderColor(const QColor &c) { borderObj_.setBorderColor(c); update(); }

  double borderWidth() const { return borderObj_.borderWidth(); }
  void setBorderWidth(double r) { borderObj_.setBorderWidth(r); update(); }

  const QString &borderSides() const { return borderObj_.borderSides(); }
  void setBorderSides(const QString &s) { borderObj_.setBorderSides(s); update(); }

  bool isClip() const { return clip_; }
  void setClip(bool b) { clip_ = b; update(); }

  //---

  // data area
  bool isDataBackground() const { return dataBorderObj_.isBackground(); }
  void setDataBackground(bool b) { dataBorderObj_.setBackground(b); update(); }

  const QColor &dataBackgroundColor() const { return dataBorderObj_.backgroundColor(); }
  void setDataBackgroundColor(const QColor &c) { dataBorderObj_.setBackgroundColor(c); update(); }

  bool isDataBorder() const { return dataBorderObj_.isBorder(); }
  void setDataBorder(bool b) { dataBorderObj_.setBorder(b); update(); }

  const QColor &dataBorderColor() const { return dataBorderObj_.borderColor(); }
  void setDataBorderColor(const QColor &c) { dataBorderObj_.setBorderColor(c); update(); }

  double dataBorderWidth() const { return dataBorderObj_.borderWidth(); }
  void setDataBorderWidth(double r) { dataBorderObj_.setBorderWidth(r); update(); }

  const QString &dataBorderSides() const { return dataBorderObj_.borderSides(); }
  void setDataBorderSides(const QString &s) { dataBorderObj_.setBorderSides(s); update(); }

  bool isDataClip() const { return dataClip_; }
  void setDataClip(bool b) { dataClip_ = b; update(); }

  //---

  bool isEqualScale() const { return equalScale_; }
  void setEqualScale(bool b);

  bool isFollowMouse() const { return followMouse_; }
  void setFollowMouse(bool b) { followMouse_ = b; }

  bool showBoxes() const { return showBoxes_; }
  void setShowBoxes(bool b) { showBoxes_ = b; update(); }

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

  CQPropertyViewTree *propertyView() const;

  virtual void addProperties();

  bool setProperty(const QString &name, const QString &value);

  //---

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

  //---

  void addAxes();

  void addXAxis();
  void addYAxis();

  void addKey();

  virtual void addKeyItems(CQChartsKey *) { }

  void addTitle();

  //---

  virtual void updateRange() = 0;

  virtual void initObjs(bool force=false) = 0;

  //---

  CBBox2D calcDataRange() const;

  void applyDataRange(bool propagate=true);

  void applyDisplayTransform(bool propagate=true);

  //---

  void addProperty(const QString &path, QObject *object,
                   const QString &name, const QString &alias="");

  void addPlotObject(CQChartsPlotObj *obj);

  void clearPlotObjects();

  int numPlotObjects() const { return plotObjs_.size(); }

  //---

  int xValueColumn() const { return xValueColumn_; }
  void setXValueColumn(int column);

  int yValueColumn() const { return yValueColumn_; }
  void setYValueColumn(int column);

  QString xStr(double x) const;
  QString yStr(double x) const;

  QString columnStr(int column, double x) const;

  //---

  bool mousePress  (const CPoint2D &p);
  bool mouseMove   (const CPoint2D &p, bool first=false);
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

  void drawSides(QPainter *painter, const QRectF &rect, const QString &sides,
                 double width, const QColor &color);

  QRectF calcRect() const;

  CBBox2D calcPixelRect() const;

  //---

  virtual void autoFit();

  void setFitBBox(const CBBox2D &bbox);

  CBBox2D fitBBox() const;

  //---

  void setLayerActive(const Layer &layer, bool b);
  bool isLayerActive(const Layer &layer) const;

  //---

  void drawParts(QPainter *painter);

  virtual void drawBackground(QPainter *painter);

  virtual void drawObjs(QPainter *painter, const Layer &layer);

  virtual void drawBgAxes(QPainter *painter);
  virtual void drawFgAxes(QPainter *painter);

  virtual void drawBgKey(QPainter *painter);
  virtual void drawFgKey(QPainter *painter);

  virtual void drawKey(QPainter *painter);

  virtual void drawTitle(QPainter *painter);

  virtual void drawForeground(QPainter *) { }

  //---

  void drawWindowRedBox(QPainter *painter, const CBBox2D &bbox);

  void drawRedBox(QPainter *painter, const CBBox2D &bbox);

  //---

  QColor objectColor(CQChartsPlotObj *obj, int i, int n, const QColor &def=QColor(0,0,0)) const;

  QColor objectStateColor(CQChartsPlotObj *obj, const QColor &c) const;

  QColor insideColor(const QColor &c) const;

  virtual QColor paletteColor(int i, int n, const QColor &def=QColor(0,0,0)) const;

  QColor interpPaletteColor(double r, const QColor &def=QColor(0,0,0)) const;

  QColor textColor(const QColor &bg) const;

  QColor bwColor(const QColor &bg) const;

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
  typedef std::map<Layer,bool>        LayerActive;

  struct MouseData {
    QPoint pressPoint;
    QPoint movePoint;
    bool   pressed { false };
  };

  CQChartsView*         view_                { nullptr };
  CQChartsPlotType*     type_                { nullptr };
  ModelP                model_               { nullptr };
  QString               id_;
  bool                  visible_             { true };
  CBBox2D               bbox_                { 0, 0, 1000, 1000 };
  Margin                margin_;
  CDisplayRange2D       displayRange_;
  CDisplayTransform2D   displayTransform_;
  CRange2D              dataRange_;
  double                dataScale_           { 1.0 };
  CPoint2D              dataOffset_          { 0.0, 0.0 };
  OptReal               xmin_;
  OptReal               ymin_;
  OptReal               xmax_;
  OptReal               ymax_;
  CGradientPalette*     palette_             { nullptr };
  CQChartsBoxObj        borderObj_;
  bool                  clip_                { true };
  CQChartsBoxObj        dataBorderObj_;
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
  LayerActive           layerActive_;
};

#endif
