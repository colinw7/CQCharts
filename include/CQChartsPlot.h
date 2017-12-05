#ifndef CQChartsPlot_H
#define CQChartsPlot_H

#include <CQChartsPlotParameter.h>
#include <CQChartsModelP.h>

#include <CQChartsGeom.h>

#include <QAbstractItemModel>
#include <QItemSelection>
#include <QFrame>

#include <memory>
#include <set>

#include <boost/optional.hpp>

class CQCharts;
class CQChartsView;
class CQChartsPlot;
class CQChartsAxis;
class CQChartsKey;
class CQChartsTitle;
class CQChartsPlotObj;
class CQChartsPlotObjTree;
class CQChartsBoxObj;
class CQPropertyViewModel;
class CQChartsDisplayRange;
class CQChartsDisplayTransform;
class CQChartsRenderer;

class CGradientPalette;
class QSortFilterProxyModel;
class QItemSelectionModel;
class QRubberBand;
class QMenu;

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
  using Types = std::map<QString,CQChartsPlotType*>;

  Types types_;
};

//----

class CQChartsPlotType {
 public:
  using Parameters = std::vector<CQChartsPlotParameter>;
  using ModelP     = CQChartsModelP;

 public:
  CQChartsPlotType() { }

  virtual ~CQChartsPlotType() { }

  // type name and description
  virtual QString name() const = 0;
  virtual QString desc() const = 0;

  // plot parameters
  // (required/key options to initialize plot)
  virtual void addParameters() = 0;

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
  Q_PROPERTY(QString backgroundColor     READ backgroundColorStr  WRITE setBackgroundColorStr )
  Q_PROPERTY(bool    border              READ isBorder            WRITE setBorder             )
  Q_PROPERTY(QString borderColor         READ borderColorStr      WRITE setBorderColorStr     )
  Q_PROPERTY(double  borderWidth         READ borderWidth         WRITE setBorderWidth        )
  Q_PROPERTY(QString borderSides         READ borderSides         WRITE setBorderSides        )
  Q_PROPERTY(bool    clip                READ isClip              WRITE setClip               )

  // data area
  Q_PROPERTY(bool    dataBackground      READ isDataBackground    WRITE setDataBackground     )
  Q_PROPERTY(QString dataBackgroundColor READ dataBackgroundColorStr
                                         WRITE setDataBackgroundColorStr)
  Q_PROPERTY(bool    dataBorder          READ isDataBorder        WRITE setDataBorder         )
  Q_PROPERTY(QString dataBorderColor     READ dataBorderColorStr  WRITE setDataBorderColorStr )
  Q_PROPERTY(double  dataBorderWidth     READ dataBorderWidth     WRITE setDataBorderWidth    )
  Q_PROPERTY(QString dataBorderSides     READ dataBorderSides     WRITE setDataBorderSides    )
  Q_PROPERTY(bool    dataClip            READ isDataClip          WRITE setDataClip           )

  // key
  Q_PROPERTY(bool    keyVisible          READ isKeyVisible        WRITE setKeyVisible         )

  // misc
  Q_PROPERTY(bool    equalScale          READ isEqualScale        WRITE setEqualScale         )
  Q_PROPERTY(bool    followMouse         READ isFollowMouse       WRITE setFollowMouse        )
  Q_PROPERTY(bool    overlay             READ isOverlay           WRITE setOverlay            )
  Q_PROPERTY(bool    y1y2                READ isY1Y2              WRITE setY1Y2               )
  Q_PROPERTY(bool    invertX             READ isInvertX           WRITE setInvertX            )
  Q_PROPERTY(bool    invertY             READ isInvertY           WRITE setInvertY            )
  Q_PROPERTY(bool    logX                READ isLogX              WRITE setLogX               )
  Q_PROPERTY(bool    logY                READ isLogY              WRITE setLogY               )
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

  struct ProbeValue {
    double  value;
    QString label;

    ProbeValue() = default;

    ProbeValue(double value, const QString &label="") :
     value(value), label(label) {
    }
  };

  struct ProbeData {
    using Values = std::vector<ProbeValue>;

    double x;
    double y;
    Values yvals;
  };

  using OptReal = boost::optional<double>;

  using ModelP          = CQChartsModelP;
  using SelectionModelP = QPointer<QItemSelectionModel>;

 public:
  CQChartsPlot(CQChartsView *view, CQChartsPlotType *type, const ModelP &model);

  virtual ~CQChartsPlot();

  CQChartsView *view() const { return view_; }

  QAbstractItemModel *model() const { return model_.data(); }

  void setSelectionModel(QItemSelectionModel *sm);
  QItemSelectionModel *selectionModel() const;

  //---

  CQCharts *charts() const;

  QString typeName() const { return type_->name(); }

  const QString &id() const { return id_; }
  void setId(const QString &s) { id_ = s; }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; update(); }

  //---

  const CQChartsDisplayRange &displayRange() const;
  void setDisplayRange(const CQChartsDisplayRange &r);

  const CQChartsDisplayTransform &displayTransform() const;
  void setDisplayTransform(const CQChartsDisplayTransform &t);

  const CQChartsGeom::Range &dataRange() const { return dataRange_; }
  void setDataRange(const CQChartsGeom::Range &r);

  double dataScale() const { return dataScale_; }
  void setDataScale(double r) { dataScale_ = r; }

  const CQChartsGeom::Point &dataOffset() const { return dataOffset_; }
  void setDataOffset(const CQChartsGeom::Point &o) { dataOffset_ = o; }

  //---

  const CGradientPalette *palette() const { return palette_; }
  void setPalette(CGradientPalette *palette) { palette_ = palette; update(); }

  const CGradientPalette *theme() const { return theme_; }
  void setTheme(CGradientPalette *theme) { theme_ = theme; update(); }

  //---

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

  QString backgroundColorStr() const;
  void setBackgroundColorStr(const QString &s);

  QColor interpBackgroundColor(int i, int n) const;

  bool isBorder() const;
  void setBorder(bool b);

  QString borderColorStr() const;
  void setBorderColorStr(const QString &s);

  QColor interpBorderColor(int i, int n) const;

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

  QString dataBackgroundColorStr() const;
  void setDataBackgroundColorStr(const QString &s);

  QColor interpDataBackgroundColor(int i, int n) const;

  bool isDataBorder() const;
  void setDataBorder(bool b);

  QString dataBorderColorStr() const;
  void setDataBorderColorStr(const QString &s);

  QColor interpDataBorderColor(int i, int n) const;

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

  //---

  const CQChartsGeom::BBox &bbox() const { return bbox_; }
  void setBBox(const CQChartsGeom::BBox &bbox);

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

  double aspect() const;

  //---

  CQChartsAxis *xAxis() const { return xAxis_; }
  CQChartsAxis *yAxis() const { return yAxis_; }

  CQChartsKey *key() const { return keyObj_; }

  CQChartsTitle *titleObj() const { return titleObj_; }

  //---

  bool isOverlay() const { return overlay_; }
  void setOverlay(bool b) { overlay_ = b; }

  bool isY1Y2() const { return y1y2_; }
  void setY1Y2(bool b) { y1y2_ = b; }

  //---

  bool isInvertX() const { return invertX_; }
  void setInvertX(bool b) { invertX_ = b; update(); }

  bool isInvertY() const { return invertY_; }
  void setInvertY(bool b) { invertY_ = b; update(); }

  //---

  bool isLogX() const { return logX_; }
  void setLogX(bool b);

  bool isLogY() const { return logY_; }
  void setLogY(bool b);

  //---

  CQChartsPlot *prevPlot() const { return otherPlot_.prev; }
  CQChartsPlot *nextPlot() const { return otherPlot_.next; }

  void setNextPlot(CQChartsPlot *plot) {
    assert(plot != this && ! otherPlot_.next);

    otherPlot_.next = plot;
  }

  void setPrevPlot(CQChartsPlot *plot) {
    assert(plot != this && ! otherPlot_.prev);

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

  void proxyModels(std::vector<QSortFilterProxyModel *> &proxyModels,
                   QAbstractItemModel* &sourceModel) const;

  //---

  QModelIndex selectIndex(int row, int col, const QModelIndex &parent=QModelIndex()) const;

  void beginSelect();
  void addSelectIndex(int row, int col, const QModelIndex &parent=QModelIndex());
  void addSelectIndex(const QModelIndex &ind);
  void endSelect();

  //---

  double logValue(double x, int base=10) const;
  double expValue(double x, int base=10) const;

  //---

  void windowToPixel(double wx, double wy, double &px, double &py) const;
  void pixelToWindow(double px, double py, double &wx, double &wy) const;

  void windowToPixel(const CQChartsGeom::Point &w, CQChartsGeom::Point &p) const;
  void pixelToWindow(const CQChartsGeom::Point &p, CQChartsGeom::Point &w) const;

  void windowToPixel(const CQChartsGeom::BBox &wrect, CQChartsGeom::BBox &prect) const;
  void pixelToWindow(const CQChartsGeom::BBox &prect, CQChartsGeom::BBox &wrect) const;

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

  virtual void updateObjs();

  void updateRangeAndObjs() { updateRange(); updateObjs(); }

  // (re)initialize plot objects
  void initPlotObjs();

  // (re)initialize plot objects (called by initPlotObjs)
  virtual bool initObjs() = 0;

  // add plotObjects to quad tree (create no data object in no objects)
  void initObjTree();

  // routine to run after plot set up (usually fixes up some defaults)
  virtual void postInit();

  //---

  CQChartsGeom::BBox calcDataRange() const;

  void applyDataRange(bool propagate=true);

  void applyDisplayTransform(bool propagate=true);

  void adjustDataRange();

  //---

  void addProperty(const QString &path, QObject *object,
                   const QString &name, const QString &alias="");

  void addPlotObject(CQChartsPlotObj *obj);

  void clearPlotObjects();

  bool updatePlotObjects(const CQChartsGeom::Point &w);

  CQChartsPlotObj *insidePlotObject() const;

  void setInsidePlotObject();

  QString insidePlotObjectText() const;

  void nextInsidePlotInd();

  //---

  int numPlotObjects() const { return plotObjs_.size(); }

  CQChartsPlotObj *plotObject(int i) const { return plotObjs_[i]; }

  //---

  int xValueColumn() const { return xValueColumn_; }
  void setXValueColumn(int column);

  int yValueColumn() const { return yValueColumn_; }
  void setYValueColumn(int column);

  //---

  virtual QString posStr(const CQChartsGeom::Point &w) const;

  virtual QString xStr(double x) const;
  virtual QString yStr(double x) const;

  virtual QString columnStr(int column, double x) const;

  //---

  // handle mouse press/move/release
  virtual bool mousePress  (const CQChartsGeom::Point &p);
  virtual bool mouseMove   (const CQChartsGeom::Point &p, bool first=false);
  virtual void mouseRelease(const CQChartsGeom::Point &p);

  // handle mouse drag press/move/release
  virtual bool mouseDragPress  (const CQChartsGeom::Point &p);
  virtual bool mouseDragMove   (const CQChartsGeom::Point &p, bool first=false);
  virtual void mouseDragRelease(const CQChartsGeom::Point &p);

  // handle key press
  virtual void keyPress(int key);

  // get tip text at point
  virtual bool tipText(const CQChartsGeom::Point &p, QString &tip) const;

  // handle mouse press in click zoom mode
  virtual void clickZoom(const CQChartsGeom::Point &p);

  //---

  virtual void panLeft ();
  virtual void panRight();
  virtual void panUp   ();
  virtual void panDown ();

  virtual void pan(double dx, double dy);

  //---

  virtual bool isClickZoom() const { return false; }

  virtual void zoomTo(const CQChartsGeom::BBox &bbox);
  virtual void zoomIn(double f=1.5);
  virtual void zoomOut(double f=1.5);
  virtual void zoomFull();

  //---

  void updateTransform();

  //---

  virtual bool addMenuItems(QMenu *) { return false; }

  //---

  // probe at x, y
  virtual bool probe(ProbeData & /*probeData*/) const { return false; }

  //---

  // called after resize
  virtual void handleResize();

  void updateKeyPosition(bool force=false);

  void updateTitlePosition();

  void drawSides(CQChartsRenderer *renderer, const QRectF &rect, const QString &sides,
                 double width, const QColor &color);

  QRectF calcRect() const;

  CQChartsGeom::BBox calcPixelRect() const;

  //---

  // auto fit
  virtual void autoFit();

  void setFitBBox(const CQChartsGeom::BBox &bbox);

  CQChartsGeom::BBox fitBBox() const;

  //---

  void setLayerActive(const Layer &layer, bool b);
  bool isLayerActive(const Layer &layer) const;

  //---

  void drawParts(CQChartsRenderer *renderer);

  // draw background
  virtual void drawBackground(CQChartsRenderer *renderer);

  // draw objects
  virtual void drawObjs(CQChartsRenderer *renderer, const Layer &layer);

  // draw axes on background/foreground
  virtual void drawBgAxes(CQChartsRenderer *renderer);
  virtual void drawFgAxes(CQChartsRenderer *renderer);

  // draw key on background/foreground
  virtual void drawBgKey(CQChartsRenderer *renderer);
  virtual void drawFgKey(CQChartsRenderer *renderer);

  // draw key
  virtual void drawKey(CQChartsRenderer *renderer);

  // draw title
  virtual void drawTitle(CQChartsRenderer *renderer);

  // draw foreground
  virtual void drawForeground(CQChartsRenderer *) { }

  //---

  void drawContrastText(CQChartsRenderer *renderer, double x, double y,
                        const QString &text, const QPen &pen);

  //---

  // debug draw (red boxes)
  void drawWindowRedBox(CQChartsRenderer *renderer, const CQChartsGeom::BBox &bbox);

  void drawRedBox(CQChartsRenderer *renderer, const CQChartsGeom::BBox &bbox);

  //---

  void updateObjPenBrushState(CQChartsPlotObj *obj, QPen &pen, QBrush &brush) const;

  QColor insideColor(const QColor &c) const;

  // get palette color for ith value of n values
  virtual QColor interpPaletteColor(int i, int n, bool scale=false) const;

  QColor interpPaletteColor(double r, bool scale=false) const;

  QColor groupPaletteColor(double r1, double r2, double dr) const;

  QColor textColor(const QColor &bg) const;

  QColor interpThemeColor(double r) const;

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

  virtual void hiddenChanged();

  //---

  void update();

  // draw plot
  virtual void draw(CQChartsRenderer *renderer) = 0;

 private slots:
  void modelDataChangedSlot(const QModelIndex &, const QModelIndex &);
  void modelLayoutChangedSlot();

  void selectionSlot();

 signals:
  void objPressed(CQChartsPlotObj *);

 protected:
  using PlotObjs   = std::vector<CQChartsPlotObj*>;
  using PlotObjSet = std::set<CQChartsPlotObj*>;

 protected:
  void objsAtPoint(const CQChartsGeom::Point &p, PlotObjs &objs) const;

 protected:
  using RefPlots    = std::vector<CQChartsPlot*>;
  using LayerActive = std::map<Layer,bool>;
  using IdHidden    = std::map<int,bool>;

  enum class DragObj {
    NONE,
    KEY,
    XAXIS,
    YAXIS
  };

  struct MouseData {
    QPoint  pressPoint;
    QPoint  movePoint;
    bool    pressed { false };
    DragObj dragObj { DragObj::NONE };
  };

  //---

  CQChartsView*             view_             { nullptr };
  CQChartsPlotType*         type_             { nullptr };
  ModelP                    model_;
  SelectionModelP           selectionModel_;
  QString                   id_;
  bool                      visible_          { true };
  CQChartsGeom::BBox        bbox_             { 0, 0, 1, 1 };
  Margin                    margin_;
  CQChartsDisplayRange*     displayRange_     { nullptr };
  CQChartsDisplayTransform* displayTransform_ { nullptr };
  CQChartsGeom::Range       dataRange_;
  double                    dataScale_        { 1.0 };
  CQChartsGeom::Point       dataOffset_       { 0.0, 0.0 };
  OptReal                   xmin_;
  OptReal                   ymin_;
  OptReal                   xmax_;
  OptReal                   ymax_;
  CGradientPalette*         palette_          { nullptr };
  CGradientPalette*         theme_            { nullptr };
  CQChartsBoxObj*           borderObj_        { nullptr };
  bool                      clip_             { true };
  CQChartsBoxObj*           dataBorderObj_    { nullptr };
  bool                      dataClip_         { false };
  QString                   title_;
  QString                   fileName_;
  CQChartsAxis*             xAxis_            { nullptr };
  CQChartsAxis*             yAxis_            { nullptr };
  CQChartsKey*              keyObj_           { nullptr };
  CQChartsTitle*            titleObj_         { nullptr };
  int                       xValueColumn_     { -1 };
  int                       yValueColumn_     { -1 };
  bool                      equalScale_       { false };
  bool                      followMouse_      { true };
  bool                      showBoxes_        { false };
  bool                      overlay_          { false };
  bool                      y1y2_             { false };
  bool                      invertX_          { false };
  bool                      invertY_          { false };
  bool                      logX_             { false };
  bool                      logY_             { false };
  OtherPlot                 otherPlot_;
  PlotObjs                  plotObjs_;
  int                       insidePlotInd_    { 0 };
  PlotObjSet                insidePlotObjs_;
  CQChartsPlotObjTree*      plotObjTree_      { nullptr };
  MouseData                 mouseData_;
  LayerActive               layerActive_;
  IdHidden                  idHidden_;
  QItemSelection            itemSelection_;
};

#endif
