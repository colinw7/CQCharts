#ifndef CQChartsPlot_H
#define CQChartsPlot_H

#include <CQChartsPlotParameter.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsQuadTree.h>

#include <CDisplayRange2D.h>
#include <CDisplayTransform2D.h>
#include <CRange2D.h>

#include <QPointer>
#include <QAbstractItemModel>
#include <QItemSelection>
#include <QFrame>

#include <boost/optional.hpp>

class CQCharts;
class CQChartsView;
class CQChartsAxis;
class CQChartsKey;
class CQChartsTitle;
class CQChartsPlotObj;
class CQChartsBoxObj;
class CQPropertyViewModel;

class CGradientPalette;
class QSortFilterProxyModel;
class QItemSelectionModel;
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
  using Types = std::map<QString, CQChartsPlotType *>;

  Types types_;
};

//----

class CQChartsPlotType {
 public:
  using Parameters = std::vector<CQChartsPlotParameter>;

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsPlotType() { }

  virtual ~CQChartsPlotType() { }

  // type name and description
  virtual QString name() const = 0;
  virtual QString desc() const = 0;

  // plot parameters
  // (required/key options to initialize plot)
  virtual void addParameters();

  const Parameters &parameters() const { return parameters_; }

  CQChartsPlotParameter &
  addColumnParameter(const QString &name, const QString &desc, const QString &propName,
                     const QString &attributes="", int defValue=-1) {
    return addParameter(CQChartsColumnParameter(name, desc, propName, attributes, defValue));
  }

  CQChartsPlotParameter &
  addColumnsParameter(const QString &name, const QString &desc, const QString &propName,
                      const QString &attributes="", const QString &defValue="") {
    return addParameter(CQChartsColumnsParameter(name, desc, propName, attributes, defValue));
  }

  CQChartsPlotParameter &
  addStringParameter(const QString &name, const QString &desc, const QString &propName,
                     const QString &attributes="", const QString &defValue="") {
    return addParameter(CQChartsStringParameter(name, desc, propName, attributes, defValue));
  }

  CQChartsPlotParameter &
  addRealParameter(const QString &name, const QString &desc, const QString &propName,
                   const QString &attributes="", double defValue=0.0) {
    return addParameter(CQChartsRealParameter(name, desc, propName, attributes, defValue));
  }

  CQChartsPlotParameter &
  addBoolParameter(const QString &name, const QString &desc, const QString &propName,
                   const QString &attributes="", bool defValue=false) {
    return addParameter(CQChartsBoolParameter(name, desc, propName, attributes, defValue));
  }

  CQChartsPlotParameter &addParameter(const CQChartsPlotParameter &parameter) {
    parameters_.push_back(parameter);

    return parameters_.back();
  }

  // create plot
  virtual CQChartsPlot *create(CQChartsView *view, const ModelP &model) const = 0;

 protected:
  Parameters parameters_;
};

//----

class CQChartsPlot : public QObject {
  Q_OBJECT

  // visible, rectangle and data range
  Q_PROPERTY(bool    visible             READ isVisible           WRITE setVisible            )
  Q_PROPERTY(QRectF  rect                READ rect                WRITE setRect               )
  Q_PROPERTY(QRectF  range               READ range               WRITE setRange              )

  // margin
  Q_PROPERTY(double  marginLeft          READ marginLeft          WRITE setMarginLeft         )
  Q_PROPERTY(double  marginTop           READ marginTop           WRITE setMarginTop          )
  Q_PROPERTY(double  marginRight         READ marginRight         WRITE setMarginRight        )
  Q_PROPERTY(double  marginBottom        READ marginBottom        WRITE setMarginBottom       )

  // title and associated filesname (if any)
  Q_PROPERTY(QString title               READ title               WRITE setTitle              )
  Q_PROPERTY(QString fileName            READ fileName            WRITE setFileName           )

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

  // key
  Q_PROPERTY(bool    keyVisible          READ isKeyVisible        WRITE setKeyVisible         )

  // misc
  Q_PROPERTY(bool    equalScale          READ isEqualScale        WRITE setEqualScale         )
  Q_PROPERTY(bool    followMouse         READ isFollowMouse       WRITE setFollowMouse        )
  Q_PROPERTY(bool    overlay             READ isOverlay           WRITE setOverlay            )
  Q_PROPERTY(bool    showBoxes           READ showBoxes           WRITE setShowBoxes          )

 public:
  // per display layer (optional)
  enum Layer {
    BG,
    MID,
    FG
  };

  // margin (percent)
  struct Margin {
    double left   { 10 };
    double top    { 10 };
    double right  { 10 };
    double bottom { 10 };
  };

  // column array
  using Columns = std::vector<int>;

  // associated plot for overlay/y1y2
  struct OtherPlot {
    CQChartsPlot *next { nullptr };
    CQChartsPlot *prev { nullptr };

    OtherPlot() { }
  };

  using OptReal = boost::optional<double>;

  using ModelP          = QSharedPointer<QAbstractItemModel>;
  using SelectionModelP = QPointer<QItemSelectionModel>;

 public:
  CQChartsPlot(CQChartsView *view, CQChartsPlotType *type, const ModelP &model);

  virtual ~CQChartsPlot();

  CQChartsView *view() const { return view_; }

  QAbstractItemModel *model() const { return model_.data(); }

  void setSelectionModel(QItemSelectionModel *sm);
  QItemSelectionModel *selectionModel() const;

  QAbstractItemModel *sourceModel() const;

  //---

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
  void setDataRange(const CRange2D &r);

  double dataScale() const { return dataScale_; }
  void setDataScale(double r) { dataScale_ = r; }

  const CPoint2D &dataOffset() const { return dataOffset_; }
  void setDataOffset(const CPoint2D &o) { dataOffset_ = o; }

  //---

  CGradientPalette *palette() const { return palette_; }
  void setPalette(CGradientPalette *palette) { palette_ = palette; update(); }

  const OptReal &xmin() const { return xmin_; }
  void setXMin(const OptReal &r) { xmin_ = r; updateObjs(); }

  const OptReal &xmax() const { return xmax_; }
  void setXMax(const OptReal &r) { xmax_ = r; updateObjs(); }

  const OptReal &ymin() const { return ymin_; }
  void setYMin(const OptReal &r) { ymin_ = r; updateObjs(); }

  const OptReal &ymax() const { return ymax_; }
  void setYMax(const OptReal &r) { ymax_ = r; updateObjs(); }

  const QString &title() const { return title_; }
  void setTitle(const QString &s);

  const QString &fileName() const { return fileName_; }
  void setFileName(const QString &s) { fileName_ = s; }

  //---

  // plot area
  bool isBackground() const;
  void setBackground(bool b);

  const QColor &backgroundColor() const;
  void setBackgroundColor(const QColor &c);

  bool isBorder() const;
  void setBorder(bool b);

  const QColor &borderColor() const;
  void setBorderColor(const QColor &c);

  double borderWidth() const;
  void setBorderWidth(double r);

  const QString &borderSides() const;
  void setBorderSides(const QString &s);

  bool isClip() const { return clip_; }
  void setClip(bool b) { clip_ = b; update(); }

  //---

  // data area
  bool isDataBackground() const;
  void setDataBackground(bool b);

  const QColor &dataBackgroundColor() const;
  void setDataBackgroundColor(const QColor &c);

  bool isDataBorder() const;
  void setDataBorder(bool b);

  const QColor &dataBorderColor() const;
  void setDataBorderColor(const QColor &c);

  double dataBorderWidth() const;
  void setDataBorderWidth(double r);

  const QString &dataBorderSides() const;
  void setDataBorderSides(const QString &s);

  bool isDataClip() const { return dataClip_; }
  void setDataClip(bool b) { dataClip_ = b; update(); }

  //---

  // key
  bool isKeyVisible() const;
  void setKeyVisible(bool b);

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

  bool isFirstPlot() const {
    return (const_cast<CQChartsPlot *>(this)->firstPlot() == this);
  }

  bool isOverlayFirstPlot() const {
    return (isOverlay() && isFirstPlot());
  }

  bool isOverlayOtherPlot() const {
    return (isOverlay() && ! isFirstPlot());
  }

  //---

  CQPropertyViewModel *propertyModel() const;

  // add plot properties to model
  virtual void addProperties();

  bool setProperty(const QString &name, const QVariant &value);
  bool getProperty(const QString &name, QVariant &value);

  //---

  void updateMargin();

  //---

  QModelIndex normalizeIndex(const QModelIndex &ind) const;
  QModelIndex unnormalizeIndex(const QModelIndex &ind) const;

  void proxyModels(std::vector<QSortFilterProxyModel *> &proxyModels) const;

  //---

  void beginSelect();
  void addSelectIndex(int row, int col, const QModelIndex &parent=QModelIndex());
  void addSelectIndex(const QModelIndex &ind);
  void endSelect();

  //---

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

  //---

  void addKey();

  void resetKeyItems();

  // add items to key
  virtual void addKeyItems(CQChartsKey *) { }

  //---

  void addTitle();

  //---

  // update data range
  // (TODO: remove apply)
  virtual void updateRange(bool apply=true) = 0;

  void updateObjs();

  // (re)initialize plot objects
  virtual void initObjs() = 0;

  // routine to run after plot set up (usually fixes up some defaults)
  virtual void postInit();

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

  CQChartsPlotObj *plotObject(int i) const { return plotObjs_[i]; }

  //---

  int xValueColumn() const { return xValueColumn_; }
  void setXValueColumn(int column);

  int yValueColumn() const { return yValueColumn_; }
  void setYValueColumn(int column);

  QString xStr(double x) const;
  QString yStr(double x) const;

  QString columnStr(int column, double x) const;

  //---

  // handle mouse press/move/release
  virtual bool mousePress  (const CPoint2D &p);
  virtual bool mouseMove   (const CPoint2D &p, bool first=false);
  virtual void mouseRelease(const CPoint2D &p);

  // handle key press
  virtual void keyPress(int key);

  // get tip text at point
  virtual bool tipText(const CPoint2D &p, QString &tip) const;

  // handle mouse press in click zoom mode
  virtual void clickZoom(const CPoint2D &p);

  //---

  virtual void panLeft ();
  virtual void panRight();
  virtual void panUp   ();
  virtual void panDown ();

  //---

  virtual bool isClickZoom() const { return false; }

  virtual void zoomTo(const CBBox2D &bbox);
  virtual void zoomIn(double f=1.5);
  virtual void zoomOut(double f=1.5);
  virtual void zoomFull();

  //---

  void updateTransform();

  // interpolate y value from x for probe
  virtual bool interpY(double /*x*/, std::vector<double> & /*y*/) const { return false; }

  // called after resize
  virtual void handleResize();

  void updateKeyPosition(bool force=false);

  void updateTitlePosition();

  void drawSides(QPainter *painter, const QRectF &rect, const QString &sides,
                 double width, const QColor &color);

  QRectF calcRect() const;

  CBBox2D calcPixelRect() const;

  //---

  // auto fit
  virtual void autoFit();

  void setFitBBox(const CBBox2D &bbox);

  CBBox2D fitBBox() const;

  //---

  void setLayerActive(const Layer &layer, bool b);
  bool isLayerActive(const Layer &layer) const;

  //---

  void drawParts(QPainter *painter);

  // draw background
  virtual void drawBackground(QPainter *painter);

  // draw objects
  virtual void drawObjs(QPainter *painter, const Layer &layer);

  // draw axes on background/foreground
  virtual void drawBgAxes(QPainter *painter);
  virtual void drawFgAxes(QPainter *painter);

  // draw key on background/foreground
  virtual void drawBgKey(QPainter *painter);
  virtual void drawFgKey(QPainter *painter);

  // draw key
  virtual void drawKey(QPainter *painter);

  // draw title
  virtual void drawTitle(QPainter *painter);

  // draw foreground
  virtual void drawForeground(QPainter *) { }

  //---

  // debug draw (red boxes)
  void drawWindowRedBox(QPainter *painter, const CBBox2D &bbox);

  void drawRedBox(QPainter *painter, const CBBox2D &bbox);

  //---

  QColor objectColor(CQChartsPlotObj *obj, int i, int n, const QColor &def=QColor(0,0,0)) const;

  QColor objectStateColor(CQChartsPlotObj *obj, const QColor &c) const;

  QColor insideColor(const QColor &c) const;

  // get palette color for ith value of n values
  virtual QColor paletteColor(int i, int n, const QColor &def=QColor(0,0,0)) const;

  QColor interpPaletteColor(double r, const QColor &def=QColor(0,0,0)) const;

  QColor groupPaletteColor(double r1, double r2, double dr, const QColor &def=QColor(0,0,0)) const;

  QColor textColor(const QColor &bg) const;

  //---

  // get/set/reset id hidden
  bool isSetHidden(int id) const {
    auto p = idHidden_.find(id);

    if (p == idHidden_.end())
      return false;

    return (*p).second;
  }

  void setSetHidden(int id, bool hidden) { idHidden_[id] = hidden; }

  void resetSetHidden() { idHidden_.clear(); }

  //---

  void update();

  // draw plot
  virtual void draw(QPainter *p) = 0;

 private slots:
  void selectionSlot();

 signals:
  void objPressed(CQChartsPlotObj *);

 protected:
  using PlotObjs    = std::vector<CQChartsPlotObj *>;
  using PlotObjTree = CQChartsQuadTree<CQChartsPlotObj,CBBox2D>;

 protected:
  void objsAtPoint(const CPoint2D &p, PlotObjTree::DataList &dataList1) const;

 protected:
  using RefPlots    = std::vector<CQChartsPlot *>;
  using LayerActive = std::map<Layer,bool>;
  using IdHidden    = std::map<int,bool>;

  struct MouseData {
    QPoint pressPoint;
    QPoint movePoint;
    bool   pressed { false };
  };

  //---

  CQChartsView*       view_                { nullptr };
  CQChartsPlotType*   type_                { nullptr };
  ModelP              model_;
  SelectionModelP     selectionModel_;
  QString             id_;
  bool                visible_             { true };
  CBBox2D             bbox_                { 0, 0, 1, 1 };
  Margin              margin_;
  CDisplayRange2D     displayRange_;
  CDisplayTransform2D displayTransform_;
  CRange2D            dataRange_;
  double              dataScale_           { 1.0 };
  CPoint2D            dataOffset_          { 0.0, 0.0 };
  OptReal             xmin_;
  OptReal             ymin_;
  OptReal             xmax_;
  OptReal             ymax_;
  CGradientPalette*   palette_             { nullptr };
  CQChartsBoxObj*     borderObj_           { nullptr };
  bool                clip_                { true };
  CQChartsBoxObj*     dataBorderObj_       { nullptr };
  bool                dataClip_            { false };
  QString             title_;
  QString             fileName_;
  CQChartsAxis*       xAxis_               { nullptr };
  CQChartsAxis*       yAxis_               { nullptr };
  CQChartsKey*        keyObj_              { nullptr };
  CQChartsTitle*      titleObj_            { nullptr };
  int                 xValueColumn_        { -1 };
  int                 yValueColumn_        { -1 };
  bool                equalScale_          { false };
  bool                followMouse_         { true };
  bool                showBoxes_           { false };
  bool                overlay_             { false };
  OtherPlot           otherPlot_;
  PlotObjs            plotObjs_;
  PlotObjTree         plotObjTree_;
  MouseData           mouseData_;
  LayerActive         layerActive_;
  IdHidden            idHidden_;
  QItemSelection      itemSelection_;
};

#endif
