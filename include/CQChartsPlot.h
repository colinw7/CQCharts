#ifndef CQChartsPlot_H
#define CQChartsPlot_H

#include <CQChartsView.h>
#include <CQChartsObj.h>
#include <CQChartsColor.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsObjData.h>
#include <CQChartsGroupData.h>
#include <CQChartsPosition.h>
#include <CQChartsRect.h>
#include <CQChartsPolygon.h>
#include <CQChartsModelVisitor.h>
#include <CQChartsTextOptions.h>
#include <CQChartsLayer.h>
#include <CQChartsUtil.h>
#include <CQChartsTypes.h>
#include <CQChartsGeom.h>
#include <CQChartsPlotMargin.h>
#include <CQChartsOptReal.h>
#include <CQChartsColorStops.h>
#include <CQChartsPaletteName.h>
#include <CQBaseModelTypes.h>
#include <CHRTime.h>

#include <QAbstractItemModel>
#include <QFrame>
#include <QTimer>
#include <QPointer>

#include <future>
#include <memory>
#include <set>

#include <boost/optional.hpp>

class CQCharts;
class CQChartsView;
class CQChartsPlotType;
class CQChartsPlot;
class CQChartsAxis;
class CQChartsPlotKey;
class CQChartsKeyItem;
class CQChartsTitle;
class CQChartsPlotObj;
class CQChartsPlotObjTree;
class CQChartsKeyColorBox;
class CQChartsAnnotation;
class CQChartsArrowAnnotation;
class CQChartsEllipseAnnotation;
class CQChartsImageAnnotation;
class CQChartsPointAnnotation;
class CQChartsPolygonAnnotation;
class CQChartsPolylineAnnotation;
class CQChartsRectangleAnnotation;
class CQChartsTextAnnotation;
class CQChartsPlotParameter;
class CQChartsObj;
class CQChartsDisplayRange;
class CQChartsValueSet;
class CQChartsModelColumnDetails;
class CQChartsModelExprMatch;
class CQChartsModelData;
class CQChartsEditHandles;
class CQChartsTableTip;
class CQPropertyViewModel;
class CQPropertyViewItem;
class QPainter;

class QSortFilterProxyModel;
class QItemSelectionModel;
class QRubberBand;
class QMenu;

//----

/*!
 * \brief Update plot timer
 * \ingroup Charts
 */
class CQChartsPlotUpdateTimer : public QTimer {
 public:
  CQChartsPlotUpdateTimer(CQChartsPlot *plot) :
   plot_(plot) {
    setSingleShot(true);
  }

  CQChartsPlot *plot() const { return plot_; }

 private:
  CQChartsPlot* plot_ { nullptr };
};

//----

CQCHARTS_NAMED_SHAPE_DATA(Plot,plot)
CQCHARTS_NAMED_SHAPE_DATA(Data,data)
CQCHARTS_NAMED_SHAPE_DATA(Fit,fit)

/*!
 * \brief Base class for Plot
 * \ingroup Charts
 */
class CQChartsPlot : public CQChartsObj,
 public CQChartsObjPlotShapeData<CQChartsPlot>,
 public CQChartsObjDataShapeData<CQChartsPlot>,
 public CQChartsObjFitShapeData <CQChartsPlot> {
  Q_OBJECT

  // view
  Q_PROPERTY(QString viewId READ viewId)

  // type
  Q_PROPERTY(QString typeStr READ typeStr)

  // generic columns and control
  Q_PROPERTY(CQChartsColumn  idColumn      READ idColumn      WRITE setIdColumn     )
  Q_PROPERTY(CQChartsColumns tipColumns    READ tipColumns    WRITE setTipColumns   )
  Q_PROPERTY(CQChartsColumn  visibleColumn READ visibleColumn WRITE setVisibleColumn)
  Q_PROPERTY(CQChartsColumn  colorColumn   READ colorColumn   WRITE setColorColumn  )
  Q_PROPERTY(CQChartsColumn  imageColumn   READ imageColumn   WRITE setImageColumn  )

  // color map
  Q_PROPERTY(ColorType          colorType       READ colorType       WRITE setColorType      )
  Q_PROPERTY(bool               colorMapped     READ isColorMapped   WRITE setColorMapped    )
  Q_PROPERTY(double             colorMapMin     READ colorMapMin     WRITE setColorMapMin    )
  Q_PROPERTY(double             colorMapMax     READ colorMapMax     WRITE setColorMapMax    )
  Q_PROPERTY(QString            colorMapPalette READ colorMapPalette WRITE setColorMapPalette)
  Q_PROPERTY(CQChartsColorStops colorXStops     READ colorXStops     WRITE setColorXStops    )
  Q_PROPERTY(CQChartsColorStops colorYStops     READ colorYStops     WRITE setColorYStops    )

  // visible
  Q_PROPERTY(bool visible READ isVisible WRITE setVisible)

  // rectangle and data range
  Q_PROPERTY(QRectF viewRect READ viewRect WRITE setViewRect)
  Q_PROPERTY(QRectF range    READ range    WRITE setRange   )

  Q_PROPERTY(QRectF innerViewRect READ innerViewRect)
  Q_PROPERTY(QRectF calcDataRect  READ calcDataRect )
  Q_PROPERTY(QRectF outerDataRect READ outerDataRect)
  Q_PROPERTY(QRectF dataRect      READ dataRect     )

  // scaling
  Q_PROPERTY(bool   equalScale  READ isEqualScale WRITE setEqualScale   )
  Q_PROPERTY(double dataScaleX  READ dataScaleX   WRITE updateDataScaleX)
  Q_PROPERTY(double dataScaleY  READ dataScaleY   WRITE updateDataScaleY)
  Q_PROPERTY(double dataOffsetX READ dataOffsetX  WRITE setDataOffsetX  )
  Q_PROPERTY(double dataOffsetY READ dataOffsetY  WRITE setDataOffsetY  )

  // range
  Q_PROPERTY(CQChartsOptReal xmin READ xmin WRITE setXMin)
  Q_PROPERTY(CQChartsOptReal ymin READ ymin WRITE setYMin)
  Q_PROPERTY(CQChartsOptReal xmax READ xmax WRITE setXMax)
  Q_PROPERTY(CQChartsOptReal ymax READ ymax WRITE setYMax)

  // every
  Q_PROPERTY(bool everyEnabled READ isEveryEnabled WRITE setEveryEnabled)
  Q_PROPERTY(int  everyStart   READ everyStart     WRITE setEveryStart  )
  Q_PROPERTY(int  everyEnd     READ everyEnd       WRITE setEveryEnd    )
  Q_PROPERTY(int  everyStep    READ everyStep      WRITE setEveryStep   )

  // filter
  Q_PROPERTY(QString filterStr READ filterStr WRITE setFilterStr)

  // inner margin
  Q_PROPERTY(CQChartsLength innerMarginLeft   READ innerMarginLeft   WRITE setInnerMarginLeft  )
  Q_PROPERTY(CQChartsLength innerMarginTop    READ innerMarginTop    WRITE setInnerMarginTop   )
  Q_PROPERTY(CQChartsLength innerMarginRight  READ innerMarginRight  WRITE setInnerMarginRight )
  Q_PROPERTY(CQChartsLength innerMarginBottom READ innerMarginBottom WRITE setInnerMarginBottom)

  // outer margin
  Q_PROPERTY(CQChartsLength outerMarginLeft   READ outerMarginLeft   WRITE setOuterMarginLeft  )
  Q_PROPERTY(CQChartsLength outerMarginTop    READ outerMarginTop    WRITE setOuterMarginTop   )
  Q_PROPERTY(CQChartsLength outerMarginRight  READ outerMarginRight  WRITE setOuterMarginRight )
  Q_PROPERTY(CQChartsLength outerMarginBottom READ outerMarginBottom WRITE setOuterMarginBottom)

  // title and associated filename (if any)
  Q_PROPERTY(QString title    READ titleStr WRITE setTitleStr)
  Q_PROPERTY(QString fileName READ fileName WRITE setFileName)
  Q_PROPERTY(QString xLabel   READ xLabel   WRITE setXLabel  )
  Q_PROPERTY(QString yLabel   READ yLabel   WRITE setYLabel  )

  // plot area
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Plot,plot)

  Q_PROPERTY(CQChartsSides plotBorderSides READ plotBorderSides WRITE setPlotBorderSides)
  Q_PROPERTY(bool          plotClip        READ isPlotClip      WRITE setPlotClip       )

  // data area
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Data,data)

  Q_PROPERTY(CQChartsSides dataBorderSides READ dataBorderSides WRITE setDataBorderSides)
  Q_PROPERTY(bool          dataClip        READ isDataClip      WRITE setDataClip       )

  // fit area
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Fit,fit)

  Q_PROPERTY(CQChartsSides fitBorderSides READ fitBorderSides WRITE setFitBorderSides)

  // key
  Q_PROPERTY(bool keyVisible READ isKeyVisible WRITE setKeyVisible)

  // font
  Q_PROPERTY(CQChartsFont font READ font WRITE setFont)

  // default palette
  Q_PROPERTY(CQChartsPaletteName defaultPalette READ defaultPalette WRITE setDefaultPalette)

  // scaled fonts
  Q_PROPERTY(double minScaleFontSize READ minScaleFontSize WRITE setMinScaleFontSize)
  Q_PROPERTY(double maxScaleFontSize READ maxScaleFontSize WRITE setMaxScaleFontSize)

  // misc
  Q_PROPERTY(bool followMouse    READ isFollowMouse  WRITE setFollowMouse   )
  Q_PROPERTY(bool overlay        READ isOverlay      WRITE setOverlay       )
  Q_PROPERTY(bool x1x2           READ isX1X2         WRITE setX1X2          )
  Q_PROPERTY(bool y1y2           READ isY1Y2         WRITE setY1Y2          )
  Q_PROPERTY(bool invertX        READ isInvertX      WRITE setInvertX       )
  Q_PROPERTY(bool invertY        READ isInvertY      WRITE setInvertY       )
//Q_PROPERTY(bool logX           READ isLogX         WRITE setLogX          )
//Q_PROPERTY(bool logY           READ isLogY         WRITE setLogY          )
  Q_PROPERTY(bool autoFit        READ isAutoFit      WRITE setAutoFit       )
  Q_PROPERTY(bool preview        READ isPreview      WRITE setPreview       )
  Q_PROPERTY(int  previewMaxRows READ previewMaxRows WRITE setPreviewMaxRows)
  Q_PROPERTY(bool queueUpdate    READ isQueueUpdate  WRITE setQueueUpdate   )
  Q_PROPERTY(bool showBoxes      READ showBoxes      WRITE setShowBoxes     )

  Q_ENUMS(ColorType)

 public:
  enum ColorType {
    AUTO    = int(CQChartsColorType::AUTO),
    SET     = int(CQChartsColorType::SET),
    GROUP   = int(CQChartsColorType::GROUP),
    INDEX   = int(CQChartsColorType::INDEX),
    X_VALUE = int(CQChartsColorType::X_VALUE),
    Y_VALUE = int(CQChartsColorType::Y_VALUE)
  };

  using SelMod = CQChartsSelMod;

  // associated plot for overlay/y1y2
  struct ConnectData {
    bool          x1x2    { false };   //!< is double x axis plot
    bool          y1y2    { false };   //!< is double y axis plot
    bool          overlay { false };   //!< is overlay plot
    CQChartsPlot* next    { nullptr }; //!< next plot
    CQChartsPlot* prev    { nullptr }; //!< previous plot

    ConnectData() { }

    void reset() {
      x1x2    = false;
      y1y2    = false;
      overlay = false;
      next    = nullptr;
      prev    = nullptr;
    }
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

    CQChartsGeom::Point p;
    Values              xvals;
    Values              yvals;
    Qt::Orientation     direction { Qt::Vertical };
    bool                both      { false };
  };

  using ModelP          = QSharedPointer<QAbstractItemModel>;
  using SelectionModelP = QPointer<QItemSelectionModel>;

  using PlotObjs    = std::vector<CQChartsPlotObj*>;
  using Annotations = std::vector<CQChartsAnnotation *>;
  using Objs        = std::vector<CQChartsObj*>;

  using ModelIndices = std::vector<QModelIndex>;

  using ColumnType = CQBaseModelType;

  using Plots = std::vector<CQChartsPlot*>;

  using Buffers = std::map<CQChartsBuffer::Type,CQChartsBuffer *>;
  using Layers  = std::map<CQChartsLayer::Type,CQChartsLayer *>;

  using ColorInd = CQChartsUtil::ColorInd;

  using DrawType = CQChartsView::DrawType;

 public:
  CQChartsPlot(CQChartsView *view, CQChartsPlotType *type, const ModelP &model);

  virtual ~CQChartsPlot();

  //---

  CQChartsView *view() const { return view_; }

  CQChartsPlotType *type() const { return type_; }

  QString viewId() const;

  QString typeStr() const;

  //---

  ModelP model() const { return model_; }
  void setModel(const ModelP &model);

  //---

  virtual bool isHierarchical() const;

  virtual int numGroups() const { return 0; }

  //---

  CQCharts *charts() const;

  QString typeName() const;

  QString pathId() const;

  //---

  void setSelected(bool b) override;

  bool isVisible() const { return visible_; }
  void setVisible(bool b);

  //---

  bool isUpdatesEnabled() const { return updatesData_.enabled == 0; }
  void setUpdatesEnabled(bool b, bool update=true);

  //---

  void updateRange();
  void updateRangeAndObjs();
  void updateObjs();

  void drawBackground();
  void drawForeground();
  void drawObjs();

  void writeScript(std::ostream &os) const;

  void writeScriptRange(std::ostream &os) const;

  void invalidateOverlay();

  //---

  const CQChartsDisplayRange &displayRange() const;
  void setDisplayRange(const CQChartsDisplayRange &r);

  const CQChartsGeom::Range &dataRange() const { return dataRange_; }
  void setDataRange(const CQChartsGeom::Range &r, bool update=true);

  void resetDataRange(bool updateRange=true, bool updateObjs=true);

  //---

  struct ZoomData {
    CQChartsGeom::Point dataScale  { 1.0, 1.0 }; //!< data scale (zoom in x/y direction)
    CQChartsGeom::Point dataOffset { 0.0, 0.0 }; //!< data offset (pan)
  };

  double dataScaleX() const { return zoomData_.dataScale.x; }
  void setDataScaleX(double r);

  double dataScaleY() const { return zoomData_.dataScale.y; }
  void setDataScaleY(double r);

  double dataOffsetX() const { return zoomData_.dataOffset.x; }
  void setDataOffsetX(double x);

  double dataOffsetY() const { return zoomData_.dataOffset.y; }
  void setDataOffsetY(double y);

  const ZoomData &zoomData() const { return zoomData_; }
  void setZoomData(const ZoomData &zoomData);

  void updateDataScaleX(double r);
  void updateDataScaleY(double r);

  //---

  const CQChartsOptReal &xmin() const { return xmin_; }
  void setXMin(const CQChartsOptReal &r);

  const CQChartsOptReal &xmax() const { return xmax_; }
  void setXMax(const CQChartsOptReal &r);

  const CQChartsOptReal &ymin() const { return ymin_; }
  void setYMin(const CQChartsOptReal &r);

  const CQChartsOptReal &ymax() const { return ymax_; }
  void setYMax(const CQChartsOptReal &r);

  //---

  int isEveryEnabled() const { return everyData_.enabled; }
  void setEveryEnabled(bool b);

  int everyStart() const { return everyData_.start; }
  void setEveryStart(int i);

  int everyEnd() const { return everyData_.end; }
  void setEveryEnd(int i);

  int everyStep() const { return everyData_.step; }
  void setEveryStep(int i);

  //---

  const QString &filterStr() const { return filterStr_; }
  void setFilterStr(const QString &s);

  //---

  const QString &titleStr() const { return titleStr_; }
  void setTitleStr(const QString &s);

  const QString &fileName() const { return fileName_; }
  void setFileName(const QString &s) { fileName_ = s; }

  //---

  QString xLabel() const;
  void setXLabel(const QString &s);

  QString yLabel() const;
  void setYLabel(const QString &s);

  //---

  // plot area
  const CQChartsSides &plotBorderSides() const { return plotBorderSides_; }
  void setPlotBorderSides(const CQChartsSides &s);

  bool isPlotClip() const { return plotClip_; }
  void setPlotClip(bool b);

  //---

  // data area
  const CQChartsSides &dataBorderSides() const { return dataBorderSides_; }
  void setDataBorderSides(const CQChartsSides &s);

  bool isDataClip() const { return dataClip_; }
  void setDataClip(bool b);

  //---

  // fit area
  const CQChartsSides &fitBorderSides() const { return fitBorderSides_; }
  void setFitBorderSides(const CQChartsSides &s);

  //---

  // font
  const CQChartsFont &font() const { return font_; }
  void setFont(const CQChartsFont &f);

  //---

  // default palette
  const CQChartsPaletteName &defaultPalette() const { return defaultPalette_; }
  void setDefaultPalette(const CQChartsPaletteName &name);

  //---

  // scaled font size
  double minScaleFontSize() const { return minScaleFontSize_; }
  void setMinScaleFontSize(double r) { minScaleFontSize_ = r; }

  double maxScaleFontSize() const { return maxScaleFontSize_; }
  void setMaxScaleFontSize(double r) { maxScaleFontSize_ = r; }

  //---

  // key
  bool isKeyVisible() const;
  void setKeyVisible(bool b);

  bool isKeyVisibleAndNonEmpty() const;

  //---

  bool isEqualScale() const { return equalScale_; }
  void setEqualScale(bool b);

  bool isFollowMouse() const { return followMouse_; }
  void setFollowMouse(bool b) { followMouse_ = b; }

  bool isAutoFit() const { return autoFit_; }
  void setAutoFit(bool b) { autoFit_ = b; }

  bool isPreview() const { return preview_; }
  void setPreview(bool b) { preview_ = b; }

  int previewMaxRows() const { return previewMaxRows_; }
  void setPreviewMaxRows(int i) { previewMaxRows_ = i; }

  bool isSequential() const { return sequential_; }

  bool isQueueUpdate() const { return queueUpdate_; }
  void setQueueUpdate(bool b) { queueUpdate_ = b; }

  //---

  bool isOverview() const { return overview_; }
  void setOverview(bool b) { overview_ = b; }

  //---

  bool showBoxes() const { return showBoxes_; }
  void setShowBoxes(bool b);

  //---

  // get/set bbox in view range
  const CQChartsGeom::BBox &viewBBox() const { return viewBBox_; }
  void setViewBBox(const CQChartsGeom::BBox &bbox);

  // get inner view bbox
  const CQChartsGeom::BBox &innerViewBBox() const { return innerViewBBox_; }

  // get/set view rect (as QRectF)
  QRectF viewRect() const;
  void setViewRect(const QRectF &r);

  // get inner view rect (as QRectF)
  QRectF innerViewRect() const;

  // get/set data range
  QRectF range() const;
  void setRange(const QRectF &r);

  //---

  QRectF calcDataRect () const;
  QRectF outerDataRect() const;
  QRectF dataRect     () const;

  //---

  // inner margin
  const CQChartsPlotMargin &innerMargin() const { return innerMargin_; }

  const CQChartsLength &innerMarginLeft() const { return innerMargin().left(); }
  void setInnerMarginLeft(const CQChartsLength &l);

  const CQChartsLength &innerMarginTop() const { return innerMargin().top(); }
  void setInnerMarginTop(const CQChartsLength &t);

  const CQChartsLength &innerMarginRight() const { return innerMargin().right(); }
  void setInnerMarginRight(const CQChartsLength &r);

  const CQChartsLength &innerMarginBottom() const { return innerMargin().bottom(); }
  void setInnerMarginBottom(const CQChartsLength &b);

  void setInnerMargin(const CQChartsLength &l, const CQChartsLength &t,
                      const CQChartsLength &r, const CQChartsLength &b);
  void setInnerMargin(const CQChartsPlotMargin &m);

  // outer margin
  const CQChartsPlotMargin &outerMargin() const { return outerMargin_; }

  const CQChartsLength &outerMarginLeft() const { return outerMargin().left(); }
  void setOuterMarginLeft(const CQChartsLength &l);

  const CQChartsLength &outerMarginTop() const { return outerMargin().top(); }
  void setOuterMarginTop(const CQChartsLength &t);

  const CQChartsLength &outerMarginRight() const { return outerMargin().right(); }
  void setOuterMarginRight(const CQChartsLength &r);

  const CQChartsLength &outerMarginBottom() const { return outerMargin().bottom(); }
  void setOuterMarginBottom(const CQChartsLength &b);

  void setOuterMargin(const CQChartsLength &l, const CQChartsLength &t,
                      const CQChartsLength &r, const CQChartsLength &b);
  void setOuterMargin(const CQChartsPlotMargin &m);

  //---

  // aspect ratio
  double aspect() const;

  //---

  CQChartsAxis *xAxis() const { return xAxis_; }
  CQChartsAxis *yAxis() const { return yAxis_; }

  virtual CQChartsAxis *mappedXAxis() const { return xAxis(); }
  virtual CQChartsAxis *mappedYAxis() const { return yAxis(); }

  //---

  CQChartsPlotKey *key() const { return keyObj_; }

  CQChartsTitle *title() const { return titleObj_; }

  //---

  bool isOverlay() const { return connectData_.overlay; }
  void setOverlay(bool b, bool notify=true);

  void updateOverlay();

  bool isX1X2() const { return connectData_.x1x2; }
  void setX1X2(bool b, bool notify=true);

  bool isY1Y2() const { return connectData_.y1y2; }
  void setY1Y2(bool b, bool notify=true);

  //---

  bool isInvertX() const { return invertX_; }
  void setInvertX(bool b);

  bool isInvertY() const { return invertY_; }
  void setInvertY(bool b);

  //---

  bool isLogX() const;
  void setLogX(bool b);

  bool isLogY() const;
  void setLogY(bool b);

  //---

  CQChartsPlot *prevPlot() const { return connectData_.prev; }
  CQChartsPlot *nextPlot() const { return connectData_.next; }

  void setNextPlot(CQChartsPlot *plot, bool notify=true);
  void setPrevPlot(CQChartsPlot *plot, bool notify=true);

  const CQChartsPlot *firstPlot() const;
  CQChartsPlot *firstPlot();

  const CQChartsPlot *lastPlot() const;
  CQChartsPlot *lastPlot();

  bool isFirstPlot() const { return (firstPlot() == this); }

  bool isOverlayFirstPlot() const {
    return (isOverlay() && isFirstPlot());
  }

  bool isOverlayOtherPlot() const {
    return (isOverlay() && ! isFirstPlot());
  }

  void overlayPlots(Plots &plots) const;

  template<typename FUNCTION>
  void processOverlayPlots(FUNCTION f) const {
    const CQChartsPlot *plot = firstPlot();

    while (plot) {
      f(plot);

      plot = plot->nextPlot();
    }
  }

  template<typename FUNCTION>
  void processOverlayPlots(FUNCTION f) {
    CQChartsPlot *plot = firstPlot();

    while (plot) {
      f(plot);

      plot = plot->nextPlot();
    }
  }

  template<typename FUNCTION>
  bool processOverlayPlots(FUNCTION f, bool b) const {
    const CQChartsPlot *plot = firstPlot();

    while (plot) {
      bool b1 = f(plot);

      if (b1 != b)
        return b1;

      plot = plot->nextPlot();
    }

    return b;
  }

  void x1x2Plots(CQChartsPlot* &plot1, CQChartsPlot* &plot2);
  void y1y2Plots(CQChartsPlot* &plot1, CQChartsPlot* &plot2);

  void resetConnectData(bool notify=true);

  //---

  void startAnimateTimer();
  void stopAnimateTimer ();

  virtual bool isAnimated() const { return false; }

  virtual void animateStep() { }

  //---

  const CQPropertyViewModel *propertyModel() const;
  CQPropertyViewModel *propertyModel();

  // add plot properties to model
  virtual void addProperties();

  void addSymbolProperties(const QString &path, const QString &prefix, const QString &descPrefix);

  void addLineProperties(const QString &path, const QString &prefix,
                         const QString &descPrefix, bool hidden=false);
  void addFillProperties(const QString &path, const QString &prefix,
                         const QString &descPrefix, bool hidden=false);

  void addTextProperties   (const QString &path, const QString &prefix, const QString &descPrefix);
  void addAllTextProperties(const QString &path, const QString &prefix, const QString &descPrefix);

  void addColorMapProperties();

  bool setProperties(const QString &properties);

  bool setProperty(const QString &name, const QVariant &value);
  bool getProperty(const QString &name, QVariant &value) const;

  bool getTclProperty(const QString &name, QVariant &value) const;

  bool getPropertyDesc    (const QString &name, QString  &desc, bool hidden=false) const;
  bool getPropertyType    (const QString &name, QString  &type, bool hidden=false) const;
  bool getPropertyUserType(const QString &name, QString  &type, bool hidden=false) const;
  bool getPropertyObject  (const QString &name, QObject* &obj , bool hidden=false) const;
  bool getPropertyIsHidden(const QString &name, bool &is_hidden) const;
  bool getPropertyIsStyle (const QString &name, bool &is_style) const;

  void propertyItemSelected(QObject *obj, const QString &path);

  virtual void getPropertyNames(QStringList &names, bool hidden=false) const;

  void getObjectPropertyNames(CQChartsPlotObj *plotObj, QStringList &names) const;

  void hideProperty(const QString &path, QObject *object);

  //---

  void updateMargins(bool update=true);

  void updateMargins(const CQChartsPlotMargin &outerMargin);

  //---

  QModelIndex normalizeIndex(const QModelIndex &ind) const;
  QModelIndex unnormalizeIndex(const QModelIndex &ind) const;

  void proxyModels(std::vector<QSortFilterProxyModel *> &proxyModels,
                   QAbstractItemModel* &sourceModel) const;

  //---

  void addColumnValues(const CQChartsColumn &column, CQChartsValueSet &valueSet) const;

  //---

  //! plot model visitor
  class ModelVisitor : public CQChartsModelVisitor {
   public:
    ModelVisitor();

    virtual ~ModelVisitor();

    const CQChartsPlot *plot() const { return plot_; }
    void setPlot(const CQChartsPlot *p) { plot_ = p; }

    void init();

    State preVisit(const QAbstractItemModel *model, const VisitData &data) override;

   private:
    const CQChartsPlot*     plot_ { nullptr };
    int                     vrow_ { 0 };
    CQChartsModelExprMatch* expr_ { nullptr };
  };

  void visitModel(ModelVisitor &visitor) const;

  //---

  bool modelMappedReal(int row, const CQChartsColumn &col, const QModelIndex &ind,
                       double &r, bool log, double def) const;

  //---

  int getRowForId(const QString &id) const;

  QString idColumnString(int row, const QModelIndex &parent, bool &ok) const;

  //---

  QModelIndex modelIndex(const CQChartsModelIndex &ind) const;

  QModelIndex modelIndex(int row, const CQChartsColumn &column,
                         const QModelIndex &parent=QModelIndex()) const;
  QModelIndex modelIndex(int row, int column, const QModelIndex &parent=QModelIndex()) const;

  //---

  QVariant modelHeaderValue(const CQChartsColumn &column, bool &ok) const;
  QVariant modelHeaderValue(const CQChartsColumn &column, int role, bool &ok) const;

  QVariant modelHeaderValue(int section, Qt::Orientation orient, bool &ok) const;
  QVariant modelHeaderValue(int section, Qt::Orientation orient, int role, bool &ok) const;

  QString modelHeaderString(const CQChartsColumn &column, bool &ok) const;
  QString modelHeaderString(const CQChartsColumn &column, int role, bool &ok) const;

  QString modelHeaderString(int section, Qt::Orientation orient, bool &ok) const;
  QString modelHeaderString(int section, Qt::Orientation orient, int role, bool &ok) const;

  //---

  QVariant modelValue(const CQChartsModelIndex &ind, bool &ok) const;

  QVariant modelValue(int row, const CQChartsColumn &column,
                      const QModelIndex &parent, bool &ok) const;
  QVariant modelValue(int row, const CQChartsColumn &column,
                      const QModelIndex &parent, int role, bool &ok) const;

  QString modelString(const CQChartsModelIndex &ind, bool &ok) const;

  QString modelString(int row, const CQChartsColumn &column,
                      const QModelIndex &parent, bool &ok) const;
  QString modelString(int row, const CQChartsColumn &column,
                      const QModelIndex &parent, int role, bool &ok) const;

  double modelReal(const CQChartsModelIndex &ind, bool &ok) const;

  double modelReal(int row, const CQChartsColumn &column,
                   const QModelIndex &parent, bool &ok) const;
  double modelReal(int row, const CQChartsColumn &column,
                   const QModelIndex &parent, int role, bool &ok) const;

  long modelInteger(const CQChartsModelIndex &ind, bool &ok) const;

  long modelInteger(int row, const CQChartsColumn &column,
                    const QModelIndex &parent, bool &ok) const;
  long modelInteger(int row, const CQChartsColumn &column,
                    const QModelIndex &parent, int role, bool &ok) const;

  CQChartsColor modelColor(int row, const CQChartsColumn &column,
                           const QModelIndex &parent, bool &ok) const;
  CQChartsColor modelColor(int row, const CQChartsColumn &column,
                           const QModelIndex &parent, int role, bool &ok) const;

  std::vector<double> modelReals(int row, const CQChartsColumn &column,
                                 const QModelIndex &parent, bool &ok) const;

  //---

  virtual QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                                    bool &ok) const;
  virtual QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                                    int role, bool &ok) const;
  virtual QVariant modelHeaderValue(QAbstractItemModel *model, int section,
                                    Qt::Orientation orient, int role, bool &ok) const;
  virtual QVariant modelHeaderValue(QAbstractItemModel *model, int section,
                                    Qt::Orientation orient, bool &ok) const;

  virtual QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                                    bool &ok) const;
  virtual QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                                    int role, bool &ok) const;
  virtual QString modelHeaderString(QAbstractItemModel *model, int section,
                                    Qt::Orientation orient, int role, bool &ok) const;
  virtual QString modelHeaderString(QAbstractItemModel *model, int section,
                                    Qt::Orientation orient, bool &ok) const;

  //---

  virtual QVariant modelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                              const QModelIndex &parent, int role, bool &ok) const;
  virtual QVariant modelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                              const QModelIndex &parent, bool &ok) const;

  virtual QString modelString(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                              const QModelIndex &parent, int role, bool &ok) const;
  virtual QString modelString(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                              const QModelIndex &parent, bool &ok) const;

  virtual double modelReal(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                           const QModelIndex &parent, int role, bool &ok) const;
  virtual double modelReal(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                           const QModelIndex &parent, bool &ok) const;

  virtual long modelInteger(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                            const QModelIndex &parent, int role, bool &ok) const;
  virtual long modelInteger(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                            const QModelIndex &parent, bool &ok) const;

  virtual CQChartsColor modelColor(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                                   const QModelIndex &parent, int role, bool &ok) const;
  virtual CQChartsColor modelColor(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                                   const QModelIndex &parent, bool &ok) const;

  //---

  QVariant modelRootValue(int row, const CQChartsColumn &column,
                          const QModelIndex &parent, int role, bool &ok) const;
  QVariant modelRootValue(int row, const CQChartsColumn &column,
                          const QModelIndex &parent, bool &ok) const;

  //---

  QVariant modelHierValue(int row, const CQChartsColumn &column,
                          const QModelIndex &parent, int role, bool &ok) const;
  QVariant modelHierValue(int row, const CQChartsColumn &column,
                          const QModelIndex &parent, bool &ok) const;

  QString  modelHierString(int row, const CQChartsColumn &column,
                           const QModelIndex &parent, int role, bool &ok) const;
  QString  modelHierString(int row, const CQChartsColumn &column,
                           const QModelIndex &parent, bool &ok) const;

  double modelHierReal(int row, const CQChartsColumn &column,
                       const QModelIndex &parent, int role, bool &ok) const;
  double modelHierReal(int row, const CQChartsColumn &column,
                       const QModelIndex &parent, bool &ok) const;

  long modelHierInteger(int row, const CQChartsColumn &column,
                        const QModelIndex &parent, int role, bool &ok) const;
  long modelHierInteger(int row, const CQChartsColumn &column,
                        const QModelIndex &parent, bool &ok) const;

  //---

  bool isSelectIndex(const QModelIndex &ind, int row, const CQChartsColumn &col,
                     const QModelIndex &parent=QModelIndex()) const;

  QModelIndex selectIndex(int row, const CQChartsColumn &col,
                          const QModelIndex &parent=QModelIndex()) const;

  void beginSelectIndex();

  void addSelectIndex(const CQChartsModelIndex &ind);
  void addSelectIndex(int row, int col, const QModelIndex &parent=QModelIndex());
  void addSelectIndex(const QModelIndex &ind);

  void endSelectIndex();

  //---

  double logValue(double x, int base=10) const;
  double expValue(double x, int base=10) const;

  //---

  QPointF positionToPlot (const CQChartsPosition &pos) const;
  QPointF positionToPixel(const CQChartsPosition &pos) const;

  QRectF rectToPlot (const CQChartsRect &rect) const;
  QRectF rectToPixel(const CQChartsRect &rect) const;

  //---

  double lengthPlotSize(const CQChartsLength &len, bool horizontal) const;

  double lengthPlotWidth (const CQChartsLength &len) const;
  double lengthPlotHeight(const CQChartsLength &len) const;

  double lengthPixelSize(const CQChartsLength &len, bool horizontal) const;

  double lengthPixelWidth (const CQChartsLength &len) const;
  double lengthPixelHeight(const CQChartsLength &len) const;

  //---

  double windowToViewWidth (double wx) const;
  double windowToViewHeight(double wy) const;

  double viewToWindowWidth (double vx) const;
  double viewToWindowHeight(double vy) const;

  CQChartsGeom::Point windowToPixel(const CQChartsGeom::Point &w) const;
  CQChartsGeom::Point windowToView (const CQChartsGeom::Point &w) const;
  CQChartsGeom::Point pixelToWindow(const CQChartsGeom::Point &p) const;
  CQChartsGeom::Point viewToWindow (const CQChartsGeom::Point &v) const;

  QPointF windowToPixel(const QPointF &w) const;
  QPointF windowToView (const QPointF &w) const;
  QPointF pixelToWindow(const QPointF &p) const;
  QPointF viewToWindow (const QPointF &v) const;

  CQChartsGeom::BBox windowToPixel(const CQChartsGeom::BBox &wrect) const;
  CQChartsGeom::BBox pixelToWindow(const CQChartsGeom::BBox &prect) const;
  CQChartsGeom::BBox viewToWindow (const CQChartsGeom::BBox &vrect) const;

  QRectF windowToPixel(const QRectF &w) const;
  QRectF pixelToWindow(const QRectF &w) const;
  QRectF windowToView (const QRectF &w) const;
  QRectF viewToWindow (const QRectF &v) const;

  double pixelToSignedWindowWidth (double ww) const;
  double pixelToSignedWindowHeight(double wh) const;

  double pixelToWindowSize(double ps, bool horizontal) const;
  QSizeF pixelToWindowSize(const QSizeF &ps) const;

  double pixelToWindowWidth (double pw) const;
  double pixelToWindowHeight(double ph) const;

  double windowToSignedPixelWidth (double ww) const;
  double windowToSignedPixelHeight(double wh) const;

  double windowToPixelWidth (double ww) const;
  double windowToPixelHeight(double wh) const;

 private:
  void windowToPixelI(const CQChartsGeom::Point &w, CQChartsGeom::Point &p) const;
  void pixelToWindowI(const CQChartsGeom::Point &p, CQChartsGeom::Point &w) const;

  void windowToPixelI(const CQChartsGeom::BBox &wrect, CQChartsGeom::BBox &prect) const;
  void pixelToWindowI(const CQChartsGeom::BBox &prect, CQChartsGeom::BBox &wrect) const;

  void pixelToWindowI(double px, double py, double &wx, double &wy) const;
  void viewToWindowI (double vx, double vy, double &wx, double &wy) const;

  void windowToPixelI(double wx, double wy, double &px, double &py) const;
  void windowToViewI (double wx, double wy, double &vx, double &vy) const;

  //---

 public:
  void plotSymbolSize (const CQChartsLength &s, double &sx, double &sy) const;
  void pixelSymbolSize(const CQChartsLength &s, double &sx, double &sy) const;

  double limitSymbolSize(double s) const;
  double limitFontSize(double s) const;

  //---

  void addAxes();

  void addXAxis();
  void addYAxis();

  //---

  void addKey();

  void resetKeyItems();

  // add items to key
  virtual void addKeyItems(CQChartsPlotKey *) { }

  //---

  void addTitle();

  //---

  void resetRange();

 private:
  // update data range (calls calcRange)
  void execUpdateRange();

 public:
  // calculate and return range from data
  virtual CQChartsGeom::Range calcRange() const = 0;

  virtual void postUpdateRange() { }

  // update plot objects (clear objects, objects updated on next redraw)
  void updateGroupedObjs();

  // reset range and objects
  void clearRangeAndObjs();

  virtual void postUpdateObjs() { }

  virtual void postDraw() { }

  virtual void postObjTree() { }

 private:
  // recalc range and clear objects (objects updated on next redraw)
  void execUpdateRangeAndObjs();

  // update plot objects (clear objects, objects updated on next redraw)
  void execUpdateObjs();

 private:
  void startThreadTimer();
  void stopThreadTimer();

  //---

  void updateAndApplyRange(bool apply, bool updateObjs);

  void updateAndApplyPlotRange(bool apply, bool updateObjs);

  void updateAndApplyPlotRange1(bool updateObjs);

  //---

  static void updateRangeASync(CQChartsPlot *plot);

  void updateRangeThread();

  void interruptRange();

  bool isReady() const;

 public:
  void syncRange();

 private:
  void waitRange();
  void waitRange1();

  //---

  static void updateObjsASync(CQChartsPlot *plot);

  void updateObjsThread();

  void interruptObjs();

  void waitObjs();
  void waitObjs1();

  //---

  void updatePlotObjs();

  void resetInsideObjs();

  void updateGroupedDraw();
  void updateDraw();

  //---

  static void drawASync(CQChartsPlot *plot);

  void drawThread();

  void interruptDraw();

  void waitDraw();
  void waitDraw1();

 private:
  void initColorColumnData();

  // (re)initialize grouped plot objects
  void initGroupedPlotObjs();

  // (re)initialize plot range
  bool initPlotRange();

  // (re)initialize plot objects
  void initPlotObjs();

  bool addNoDataObj();

  void updateAutoFit();

  void autoFitOne();

 public:
  // (re)initialize plot objects (called by initPlotObjs)
  virtual bool initObjs();

  // create plot objects (called by initObjs)
  bool createObjs();

  // create objects to be added to plot
  // TODO: need axis update as well
  virtual bool createObjs(PlotObjs &) const = 0;

  // add plotObjects to quad tree (create no data object in no objects)
  void initObjTree();

  // routine to run after plot set up (usually fixes up some defaults)
  virtual void postInit();

  //---

  CQChartsGeom::BBox findEmptyBBox(double w, double h) const;

  //---

  CQChartsGeom::BBox calcDataRange(bool adjust=true) const;

  CQChartsGeom::BBox getDataRange() const;

  void setPixelRange (const CQChartsGeom::BBox &bbox);
  void setWindowRange(const CQChartsGeom::BBox &bbox);

  void applyDataRangeAndDraw();

  void applyDataRange(bool propagate=true);

  CQChartsGeom::Range adjustDataRange(const CQChartsGeom::Range &range) const;

  //---

  CQPropertyViewItem *addProperty(const QString &path, QObject *object,
                                  const QString &name, const QString &alias="");

  void addPlotObject(CQChartsPlotObj *obj);

  virtual void clearPlotObjects();

  bool updateInsideObjects(const CQChartsGeom::Point &w);

  CQChartsObj *insideObject() const;

  void setInsideObject();

  QString insideObjectText() const;

  void nextInsideInd();
  void prevInsideInd();

  //---

  const PlotObjs &plotObjects() const { return plotObjs_; }

  int numPlotObjects() const { return plotObjs_.size(); }

  CQChartsPlotObj *plotObject(int i) const { return plotObjs_[i]; }

  bool isNoData() const { return noData_; }
  void setNoData(bool b) { noData_ = b; }

  bool isPlotObjTreeSet() const { return plotObjTreeSet_; }
  void setPlotObjTreeSet(bool b);

  //---

  const CQChartsColumn &xValueColumn() const { return xValueColumn_; }
  void setXValueColumn(const CQChartsColumn &column);

  const CQChartsColumn &yValueColumn() const { return yValueColumn_; }
  void setYValueColumn(const CQChartsColumn &column);

  const CQChartsColumn &idColumn() const { return idColumn_; }
  void setIdColumn(const CQChartsColumn &column);

  const CQChartsColumns &tipColumns() const { return tipColumns_; }
  void setTipColumns(const CQChartsColumns &columns);

  const CQChartsColumn &visibleColumn() const { return visibleColumn_; }
  void setVisibleColumn(const CQChartsColumn &column);

  const CQChartsColumn &imageColumn() const { return imageColumn_; }
  void setImageColumn(const CQChartsColumn &column);

  //---

  const CQChartsColumn &colorColumn() const;
  void setColorColumn(const CQChartsColumn &c);

  const ColorType &colorType() const;
  void setColorType(const ColorType &t);

  bool isColorMapped() const;
  void setColorMapped(bool b);

  double colorMapMin() const;
  void setColorMapMin(double r);

  double colorMapMax() const;
  void setColorMapMax(double r);

  const QString &colorMapPalette() const;
  void setColorMapPalette(const QString &s);

  const CQChartsColorStops &colorXStops() const { return colorColumnData_.xStops; }
  void setColorXStops(const CQChartsColorStops &s);

  const CQChartsColorStops &colorYStops() const { return colorColumnData_.yStops; }
  void setColorYStops(const CQChartsColorStops &s);

  //---

  bool columnColor(int row, const QModelIndex &parent, CQChartsColor &color) const;

  //---

  virtual QString keyText() const;

  //---

  virtual QString posStr(const CQChartsGeom::Point &w) const;

  virtual QString xStr(double x) const;
  virtual QString yStr(double y) const;

  virtual QString columnStr(const CQChartsColumn &column, double x) const;

  //---

  // handle mouse press/move/release
  bool selectMousePress  (const QPointF &p, SelMod selMod);
  bool selectMouseMove   (const QPointF &p, bool first=false);
  bool selectMouseRelease(const QPointF &p);

  virtual bool selectPress  (const CQChartsGeom::Point &p, SelMod selMod);
  virtual bool selectMove   (const CQChartsGeom::Point &p, bool first=false);
  virtual bool selectRelease(const CQChartsGeom::Point &p);

  // handle mouse drag press/move/release
  bool editMousePress  (const QPointF &p, bool inside=false);
  bool editMouseMove   (const QPointF &p, bool first=false);
  bool editMouseMotion (const QPointF &p);
  bool editMouseRelease(const QPointF &p);

  virtual bool editPress  (const CQChartsGeom::Point &p, const CQChartsGeom::Point &w,
                           bool inside=false);
  virtual bool editMove   (const CQChartsGeom::Point &p, const CQChartsGeom::Point &w,
                           bool first=false);
  virtual bool editMotion (const CQChartsGeom::Point &p, const CQChartsGeom::Point &w);
  virtual bool editRelease(const CQChartsGeom::Point &p, const CQChartsGeom::Point &w);

  virtual void editMoveBy(const QPointF &d);

  void selectOneObj(CQChartsObj *obj, bool allObjs);

  void deselectAllObjs();

  void deselectAll();

  // handle key press
  virtual void keyPress(int key, int modifier);

  // get tip text at point
  virtual bool tipText(const CQChartsGeom::Point &p, QString &tip) const;

  void addTipColumns(CQChartsTableTip &tableTip, const QModelIndex &ind) const;

  void resetObjTips();

  // handle rect select
  bool rectSelect(const CQChartsGeom::BBox &r, SelMod selMod);

  //---

  void startSelection();
  void endSelection();

  //---

  void selectedObjs(Objs &objs) const;

  void selectedPlotObjs(PlotObjs &objs) const;

  void editObjs(Objs &objs);

  //---

  virtual double getPanX(bool is_shift) const;
  virtual double getPanY(bool is_shift) const;

  virtual double getMoveX(bool is_shift) const;
  virtual double getMoveY(bool is_shift) const;

  virtual double getZoomFactor(bool is_shift) const;

 public slots:
  void updateSlot();

  virtual void cycleNext();
  virtual void cyclePrev();

  virtual void panLeft (double f=0.125);
  virtual void panRight(double f=0.125);
  virtual void panUp   (double f=0.125);
  virtual void panDown (double f=0.125);

  virtual void zoomIn(double f=1.5);
  virtual void zoomOut(double f=1.5);

  virtual void zoomFull(bool notify=true);

  virtual bool allowZoomX() const { return true; }
  virtual bool allowZoomY() const { return true; }

  virtual bool allowPanX() const { return true; }
  virtual bool allowPanY() const { return true; }

 public:
  virtual void cycleNextPrev(bool prev);

  virtual void pan(double dx, double dy);

  virtual void zoomTo(const CQChartsGeom::BBox &bbox);

  //---

  void updateTransform();

  //---

  virtual bool addMenuItems(QMenu *) { return false; }

  //---

  // probe at x, y
  virtual bool probe(ProbeData & /*probeData*/) const { return false; }

  //---

  // called before resize
  virtual void preResize();

  // called after resize
  virtual void postResize();

  //---

  void updateKeyPosition(bool force=false);

  CQChartsGeom::BBox displayRangeBBox() const;

  CQChartsGeom::BBox calcDataPixelRect() const;

  CQChartsGeom::BBox calcPlotRect() const;
  CQChartsGeom::BBox calcPlotPixelRect() const;

  CQChartsGeom::BBox calcFitPixelRect() const;

  QSizeF calcPixelSize() const;

  //---

  // auto fit
  virtual void autoFit();

  void setFitBBox(const CQChartsGeom::BBox &bbox);

  CQChartsGeom::BBox fitBBox() const;

  virtual CQChartsGeom::BBox dataFitBBox () const;
  virtual CQChartsGeom::BBox axesFitBBox () const;
  virtual CQChartsGeom::BBox keyFitBBox  () const;
  virtual CQChartsGeom::BBox titleFitBBox() const;

  // get bounding box of annotations outside plot area
  virtual CQChartsGeom::BBox annotationBBox() const { return CQChartsGeom::BBox(); }

  //---

  // annotations

  const Annotations &annotations() const { return annotations_; }

  CQChartsArrowAnnotation     *addArrowAnnotation    (const CQChartsPosition &start,
                                                      const CQChartsPosition &end);
  CQChartsEllipseAnnotation   *addEllipseAnnotation  (const CQChartsPosition &center,
                                                      const CQChartsLength &xRadius,
                                                      const CQChartsLength &yRadius);
  CQChartsImageAnnotation     *addImageAnnotation    (const CQChartsPosition &pos,
                                                      const QImage &image);
  CQChartsImageAnnotation     *addImageAnnotation    (const CQChartsRect &rect,
                                                      const QImage &image);
  CQChartsPointAnnotation     *addPointAnnotation    (const CQChartsPosition &pos,
                                                      const CQChartsSymbol &type);
  CQChartsPolygonAnnotation   *addPolygonAnnotation  (const CQChartsPolygon &polygon);
  CQChartsPolylineAnnotation  *addPolylineAnnotation (const CQChartsPolygon &polygon);
  CQChartsRectangleAnnotation *addRectangleAnnotation(const CQChartsRect &rect);
  CQChartsTextAnnotation      *addTextAnnotation     (const CQChartsPosition &pos,
                                                      const QString &text);
  CQChartsTextAnnotation      *addTextAnnotation     (const CQChartsRect &rect,
                                                      const QString &text);

  void addAnnotation(CQChartsAnnotation *annotation);

  CQChartsAnnotation *getAnnotationByName(const QString &id) const;

  void removeAnnotation(CQChartsAnnotation *annotation);

  void removeAllAnnotations();

  //---

  CQChartsPlotObj *getObject(const QString &objectId) const;

  QList<QModelIndex> getObjectInds(const QString &objectId) const;

  //---

  const Layers &layers() const { return layers_; }

  CQChartsLayer *initLayer(const CQChartsLayer::Type &type,
                           const CQChartsBuffer::Type &buffer, bool active);

  void setLayerActive(const CQChartsLayer::Type &type, bool b);

  bool isLayerActive(const CQChartsLayer::Type &type) const;

  void invalidateLayers();

  void invalidateLayer(const CQChartsBuffer::Type &layerType);

  CQChartsBuffer *getBuffer(const CQChartsBuffer::Type &type) const;

  CQChartsLayer *getLayer(const CQChartsLayer::Type &type) const;

 private:
  CQChartsGeom::BBox adjustDataRangeBBox(const CQChartsGeom::BBox &bbox) const;

  void setLayerActive1(const CQChartsLayer::Type &type, bool b);

  void invalidateLayer1(const CQChartsBuffer::Type &layerType);

  void setLayersChanged(bool update);

  void deselectAll1(bool &changed);

  //---

 public:
  virtual bool hasXAxis() const;
  virtual bool hasYAxis() const;

  //---

 public:
  // draw plot parts
  virtual void drawParts(QPainter *painter) const;

  // draw plot device parts
  virtual void drawDeviceParts(CQChartsPaintDevice *) const { }

  // draw background layer plot parts
  virtual void drawBackgroundParts(QPainter *painter) const;

  void drawBackgroundDeviceParts(CQChartsPaintDevice *device) const;

  // draw middle layer plot parts
  virtual void drawMiddleParts(QPainter *painter) const;

  void drawMiddleDeviceParts(CQChartsPaintDevice *device) const;

  // draw foreground layer plot parts
  virtual void drawForegroundParts(QPainter *painter) const;

  void drawForegroundDeviceParts(CQChartsPaintDevice *device) const;

  // draw overlay layer plot parts
  virtual void drawOverlayParts(QPainter *painter) const;

  void drawOverlayDeviceParts(CQChartsPaintDevice *device) const;

  //---

  // draw background (layer and detail)
  virtual bool hasBackgroundLayer() const;

  virtual void drawBackgroundLayer(CQChartsPaintDevice *device) const;

  virtual bool hasBackground() const;

  virtual void execDrawBackground(CQChartsPaintDevice *device) const;

  void drawBackgroundSides(CQChartsPaintDevice *device, const QRectF &rect,
                           const CQChartsSides &sides) const;

  // draw axes on background
  bool hasGroupedBgAxes() const;

  virtual bool hasBgAxes() const;

  void drawGroupedBgAxes(CQChartsPaintDevice *device) const;

  virtual void drawBgAxes(CQChartsPaintDevice *device) const;

  // draw key on background
  virtual bool hasGroupedBgKey() const;

  virtual void drawBgKey(CQChartsPaintDevice *device) const;

  //---

  // draw objects
  bool hasGroupedObjs(const CQChartsLayer::Type &layerType) const;

  void drawGroupedObjs(CQChartsPaintDevice *device, const CQChartsLayer::Type &layerType) const;

  virtual bool hasObjs(const CQChartsLayer::Type &layerType) const;

  void execDrawObjs(CQChartsPaintDevice *device, const CQChartsLayer::Type &type) const;

  //---

  // draw axes on foreground
  bool hasGroupedFgAxes() const;

  virtual bool hasFgAxes() const;

  void drawGroupedFgAxes(CQChartsPaintDevice *device) const;

  virtual void drawFgAxes(CQChartsPaintDevice *device) const;

  // draw key on foreground
  virtual bool hasGroupedFgKey() const;

  virtual void drawFgKey(CQChartsPaintDevice *painter) const;

  // draw title
  virtual bool hasTitle() const;

  virtual void drawTitle(CQChartsPaintDevice *device) const;

  // draw annotations
  virtual bool hasGroupedAnnotations(const CQChartsLayer::Type &layerType) const;

  void drawGroupedAnnotations(CQChartsPaintDevice *device,
                              const CQChartsLayer::Type &layerType) const;

  virtual bool hasAnnotations(const CQChartsLayer::Type &layerType) const;

  virtual void drawAnnotations(CQChartsPaintDevice *device,
                               const CQChartsLayer::Type &layerType) const;

  // draw foreground
  virtual bool hasForeground() const;

  virtual void execDrawForeground(CQChartsPaintDevice *device) const;

  // draw debug boxes
  virtual bool hasGroupedBoxes() const;

  void drawGroupedBoxes(CQChartsPaintDevice *device) const;

  virtual bool hasBoxes() const;

  virtual void drawBoxes(CQChartsPaintDevice *device) const;

  // draw edit handles
  bool hasGroupedEditHandles() const;

  void drawGroupedEditHandles(QPainter *painter) const;

  bool hasEditHandles() const;

  virtual void drawEditHandles(QPainter *painter) const;

  //---

  // set clip rect
  void setClipRect(CQChartsPaintDevice *device) const;

  //---

  virtual bool selectInvalidateObjs() const { return false; }

  //---

  QPainter *beginPaint(CQChartsBuffer *layer, QPainter *painter, const QRectF &rect=QRectF()) const;
  void      endPaint  (CQChartsBuffer *layer) const;

  //---

  CQChartsPlotKey *getFirstPlotKey() const;

  //---

  void drawSymbol(CQChartsPaintDevice *painter, const QPointF &p, const CQChartsSymbol &symbol,
                  const CQChartsLength &size) const;
  void drawSymbol(CQChartsPaintDevice *device, const QPointF &p, const CQChartsSymbol &symbol,
                  const CQChartsLength &size, const QPen &pen, const QBrush &brush) const;

  void drawBufferedSymbol(QPainter *painter, const QPointF &p,
                          const CQChartsSymbol &symbol, double size) const;

  //---

  CQChartsTextOptions adjustTextOptions(
    const CQChartsTextOptions &options=CQChartsTextOptions()) const;

  //---

  // debug draw (default to red boxes)
  void drawWindowColorBox(CQChartsPaintDevice *device, const CQChartsGeom::BBox &bbox,
                          const QColor &c=Qt::red) const;

  void drawColorBox(CQChartsPaintDevice *device, const CQChartsGeom::BBox &bbox,
                    const QColor &c=Qt::red) const;

  //---

  void setPenBrush(QPen &pen, QBrush &brush,
                   bool stroked, const QColor &strokeColor, double strokeAlpha,
                   const CQChartsLength &strokeWidth, const CQChartsLineDash &strokeDash,
                   bool filled, const QColor &fillColor, double fillAlpha,
                   const CQChartsFillPattern &pattern=CQChartsFillPattern::Type::SOLID) const;

  void setPen(QPen &pen, bool stroked, const QColor &strokeColor=QColor(), double strokeAlpha=1.0,
              const CQChartsLength &strokeWidth=CQChartsLength("0px"),
              const CQChartsLineDash &strokeDash=CQChartsLineDash()) const;

  void setBrush(QBrush &brush, bool filled, const QColor &fillColor=QColor(), double fillAlpha=1.0,
                const CQChartsFillPattern &pattern=CQChartsFillPattern::Type::SOLID) const;

  //---

  void updateObjPenBrushState(const CQChartsObj *obj, QPen &pen, QBrush &brush,
                              DrawType drawType=DrawType::BOX) const;

  void updateObjPenBrushState(const CQChartsObj *obj, const ColorInd &ic,
                              QPen &pen, QBrush &brush, DrawType drawType) const;

  void updateInsideObjPenBrushState  (const ColorInd &ic, QPen &pen, QBrush &brush,
                                      bool outline, DrawType drawType) const;
  void updateSelectedObjPenBrushState(const ColorInd &ic, QPen &pen, QBrush &brush,
                                      DrawType drawType) const;

  QColor insideColor(const QColor &c) const;
  QColor selectedColor(const QColor &c) const;

  //---

 public:
  // get palette color for index
  virtual QColor interpPaletteColor(const ColorInd &ind, bool scale=false) const;

 private:
  QColor interpPaletteColorI(int i, int n, bool scale=false) const;
  QColor interpPaletteColorI(double r, bool scale=false) const;

 public:
  virtual QColor interpGroupPaletteColor(const ColorInd &ig, const ColorInd &iv,
                                         bool scale=false) const;

  QColor blendGroupPaletteColor(double r1, double r2, double dr) const;

 private:
  QColor interpGroupPaletteColorI(int ig, int ng, int i, int n, bool scale=false) const;

 public:
  QColor interpThemeColor(const ColorInd &ind) const;

 public:
  QColor interpColor(const CQChartsColor &c, const ColorInd &ind) const;

  //---

 public:
  QColor calcTextColor(const QColor &bg) const;

  //---

  ColorInd calcColorInd(const CQChartsPlotObj *obj, const CQChartsKeyColorBox *keyBox,
                        const ColorInd &is, const ColorInd &ig, const ColorInd &iv) const;

  //---

  ColumnType columnValueType(const CQChartsColumn &column,
                             const ColumnType &defType=ColumnType::STRING) const;

  bool columnValueType(const CQChartsColumn &column, ColumnType &columnType,
                       ColumnType &columnBaseType, CQChartsNameValues &nameValues,
                       const ColumnType &defType=ColumnType::STRING) const;

#if 0
  bool columnTypeStr(const CQChartsColumn &column, QString &typeStr) const;

  bool setColumnTypeStr(const CQChartsColumn &column, const QString &typeStr);
#endif

  bool columnDetails(const CQChartsColumn &column, QString &typeName,
                     QVariant &minValue, QVariant &maxValue) const;

  CQChartsModelColumnDetails *columnDetails(const CQChartsColumn &column) const;

  //---

  CQChartsModelData *getModelData() const;

  //---

  bool getHierColumnNames(const QModelIndex &parent, int row, const CQChartsColumns &nameColumns,
                          const QString &separator, QStringList &nameStrs,
                          ModelIndices &nameInds) const;

  //---

  // cached column names
  QString columnsHeaderName(const CQChartsColumns &columns) const;
  QString columnHeaderName(const CQChartsColumn &column) const;

  QString idHeaderName   () const { return columnHeaderName(idColumn()); }
  QString colorHeaderName() const { return columnHeaderName(colorColumn()); }
  QString imageHeaderName() const { return columnHeaderName(imageColumn()); }

  virtual void updateColumnNames();

  void setColumnHeaderName(const CQChartsColumn &column, const QString &def);

  //---

  // get/set/reset id hidden
  bool isSetHidden(int id) const;

  void setSetHidden(int id, bool hidden);

  void resetSetHidden();

  //---

  void update();

  //---

  // draw plot
  virtual void draw(QPainter *painter);

  // draw plot layer
  void drawLayer(QPainter *painter, CQChartsLayer::Type type) const;

  // draw all layers
  void drawLayers(QPainter *painter) const;

  const CQChartsLayer::Type &drawLayerType() const;

  //---

  // print layer data
  bool printLayer(CQChartsLayer::Type type, const QString &filename) const;

  //---

  // get/set parameter
  bool getParameter(CQChartsPlotParameter *param, QVariant &value) const;
  bool setParameter(CQChartsPlotParameter *param, const QVariant &value);

  //---

  // write details to output
  virtual void write(std::ostream &os, const QString &varName="",
                     const QString &modelName="") const;

  //---

 protected slots:
  void animateSlot();

  void threadTimerSlot();

  // model change slots
  void modelChangedSlot();

  void currentModelChangedSlot();

  //---

  void selectionSlot(QItemSelectionModel *sm);

  void updateAnnotationSlot();

 signals:
  // model data changed
  void modelChanged();

  // data range changed
  void rangeChanged();

  // plot objects added
  void plotObjsAdded();

  // connection (x1x2, y1y2, overlay) changed
  void connectDataChanged();

  // layers changed (active, valid)
  void layersChanged();

  // key signals (key, key item pressed)
  void keyItemPressed(CQChartsKeyItem *);
  void keyItemIdPressed(const QString &);

  void keyPressed(CQChartsPlotKey *);
  void keyIdPressed(const QString &);

  // title signals (title pressed)
  void titlePressed(CQChartsTitle *);
  void titleIdPressed(const QString &);

  // annotation signals (annotation pressed)
  void annotationPressed(CQChartsAnnotation *);
  void annotationIdPressed(const QString &);

  // object signals (object pressed)
  void objPressed(CQChartsPlotObj *);
  void objIdPressed(const QString &);

  // zoom/pan changed
  void zoomPanChanged();

  // annotations changed
  void annotationsChanged();

  // selection changed
  void selectionChanged();

 protected:
  struct NoUpdate {
    NoUpdate(const CQChartsPlot *plot, bool update=false) :
     plot_(const_cast<CQChartsPlot *>(plot)), update_(update) {
      plot_->setUpdatesEnabled(false);
    }

    NoUpdate(CQChartsPlot *plot, bool update=false) :
     plot_(plot), update_(update) {
      plot_->setUpdatesEnabled(false);
    }

   ~NoUpdate() { plot_->setUpdatesEnabled(true, update_); }

    CQChartsPlot* plot_   { nullptr };
    bool          update_ { false };
  };

  using ObjSet     = std::set<CQChartsObj*>;
  using SizeObjSet = std::map<double,ObjSet>;

 protected:
  void connectModel();
  void disconnectModel();

  void connectDisconnectModel(bool connectDisconnect);

  //---

  void objsAtPoint(const CQChartsGeom::Point &p, Objs &objs) const;

  void plotObjsAtPoint(const CQChartsGeom::Point &p, PlotObjs &objs) const;

  void objsIntersectRect(const CQChartsGeom::BBox &r, Objs &objs, bool inside) const;

  bool objNearestPoint(const CQChartsGeom::Point &p, CQChartsPlotObj* &obj) const;

 protected:
  //*! update state
  enum class UpdateState {
    INVALID,                //!< invalid state
    CALC_RANGE,             //!< calculating range
    CALC_OBJS,              //!< calculating objects
    DRAW_OBJS,              //!< drawing objects
    READY,                  //!< ready to draw
    UPDATE_RANGE,           //!< needs range update
    UPDATE_OBJS,            //!< needs objs update
    UPDATE_DRAW_OBJS,       //!< needs draw objects
    UPDATE_DRAW_BACKGROUND, //!< needs draw background
    UPDATE_DRAW_FOREGROUND, //!< needs draw foreground
    UPDATE_VIEW,            //!< update view
    DRAWN                   //!< drawn
  };

  struct ThreadData {
    CHRTime           startTime;
    std::future<void> future;
    std::atomic<bool> busy { false };

    void start(const CQChartsPlot *plot, const char *id) {
      if (id) {
        std::cerr << "Start: " << plot->id().toStdString() << " : " << id << "\n";

        startTime = CHRTime::getTime();
      }

      busy.store(true);
    }

    void end(const CQChartsPlot *plot, const char *id) {
      busy.store(false);

      if (id) {
        CHRTime dt = startTime.diffTime();

        std::cerr << "Elapsed: " << plot->id().toStdString() << " : " << id << " " <<
                     dt.getMSecs() << "\n";
      }
    }

    void finish(const CQChartsPlot *plot, const char *id) {
      if (id) {
        CHRTime dt = startTime.diffTime();

        std::cerr << "Finish: " << plot->id().toStdString() << " : " << id << " " <<
                     dt.getMSecs() << "\n";
      }
    }
  };

  struct LockData {
    mutable std::mutex lock;
    const char*        id { nullptr };
  };

  struct DrawBusyData {
    QColor       bgColor  { 255, 255, 255 };
    QColor       fgColor  { 100, 200, 100 };
    CQChartsFont font;
    int          count    { 10 };
    int          multiple { 10 };
    mutable int  ind      { 0 };
  };

  struct UpdateData {
    std::atomic<int> state       { 0 };
    std::atomic<int> interrupt   { 0 };
    ThreadData       rangeThread;
    ThreadData       objsThread;
    ThreadData       drawThread;
    LockData         lockData;
    bool             updateObjs  { false };
    QTimer*          timer       { nullptr };
    DrawBusyData     drawBusy;
  };

  const QColor &updateBusyColor() const { return updateData_.drawBusy.fgColor; }
  void setUpdateBusyColor(const QColor &c) { updateData_.drawBusy.fgColor = c; }

  int updateBusyCount() const { return updateData_.drawBusy.count; }
  void setUpdateBusyCount(int i) { updateData_.drawBusy.count = i; }

  void setGroupedUpdateState(UpdateState state);

  UpdateState updateState() { return (UpdateState) updateData_.state.load(); }
  void setUpdateState(UpdateState state);

  UpdateState calcNextState() const;

  void setInterrupt(bool b=true);

  void updateLock(const char *id) {
    //std::cerr << "> " << id << "\n";
    assert(! updateData_.lockData.id);
    updateData_.lockData.lock.lock();
    updateData_.lockData.id = id;
  }

  bool updateTryLock(const char *id) {
    //std::cerr << "> " << id << "\n";
    assert(! updateData_.lockData.id);
    bool locked = updateData_.lockData.lock.try_lock();
    updateData_.lockData.id = id;
    return locked;
  }

  void updateUnLock() {
    //std::cerr << "< " << updateData_.lockData.id << "\n";
    updateData_.lockData.lock.unlock();
    updateData_.lockData.id = nullptr;
   }

  struct LockMutex {
    LockMutex(CQChartsPlot *plot, const char *id) : plot(plot) { plot->updateLock(id); }
   ~LockMutex() { plot->updateUnLock(); }

    CQChartsPlot* plot { nullptr };
  };

  struct TryLockMutex {
    TryLockMutex(CQChartsPlot *plot, const char *id) : plot(plot) {
      locked = plot->updateTryLock(id); }
   ~TryLockMutex() { if (locked) plot->updateUnLock(); }

    CQChartsPlot *plot   { nullptr };
    bool          locked { false };
  };

  void drawBusy(QPainter *painter, const UpdateState &updateState) const;

 public:
  bool isInterrupt() const { return updateData_.interrupt.load() > 0; }

 protected:
  using IdHidden        = std::map<int,bool>;
  using Rows            = std::set<int>;
  using ColumnRows      = std::map<int,Rows>;
  using IndexColumnRows = std::map<QModelIndex,ColumnRows>;

  //! color column data
  struct ColorColumnData {
    CQChartsColumn     column;
    ColorType          colorType { ColorType::AUTO };
    bool               valid     { false };
    bool               mapped    { false };
    double             map_min   { 0.0 };
    double             map_max   { 1.0 };
    double             data_min  { 0.0 };
    double             data_max  { 1.0 };
    CQBaseModelType    modelType;
    QString            palette;
    CQChartsColorStops xStops;
    CQChartsColorStops yStops;
  };

  //! every row selection data
  struct EveryData {
    bool enabled { false };
    int  start   { 0 };
    int  end     { std::numeric_limits<int>::max() };
    int  step    { 1 };
  };

  //! drag object
  enum class DragObj {
    NONE,
    KEY,
    XAXIS,
    YAXIS,
    TITLE,
    ANNOTATION,
    PLOT,
    PLOT_HANDLE
  };

  //! mouse state data
  struct MouseData {
    QPointF            pressPoint { 0, 0 };
    QPointF            movePoint  { 0, 0 };
    bool               pressed    { false };
    DragObj            dragObj    { DragObj::NONE };
    CQChartsResizeSide dragSide   { CQChartsResizeSide::NONE };
    bool               dragged    { false };
  };

  //! animation data
  struct AnimateData {
    QTimer* timer   { nullptr };
    int     tickLen { 30 };
  };

  //! update state data
  struct UpdatesData {
    using StateFlag = std::map<UpdateState,int>;

    int       enabled            { 0 };     //!< updates enabled
    bool      updateRangeAndObjs { false }; //!< call execUpdateRangeAndObjs (on enable)
    bool      updateObjs         { false }; //!< call execUpdateObjs (on enable)
    bool      applyDataRange     { false }; //!< call applyDataRange (on enable)
    bool      invalidateLayers   { false }; //!< call needsInvalidate invalidate (on enable)
    StateFlag stateFlag;                    //!< state flags

    void reset() {
      updateRangeAndObjs = false;
      updateObjs         = false;
      applyDataRange     = false;
      invalidateLayers   = false;

      //stateFlag.clear();
    }
  };

  using ColumnNames = std::map<CQChartsColumn,QString>;

  //---

 protected:
  CQChartsView*                view_             { nullptr };    //!< parent view
  CQChartsPlotType*            type_             { nullptr };    //!< plot type data
  ModelP                       model_;                           //!< abstract model
  bool                         modelNameSet_     { false };      //!< model name set from plot
  CQPropertyViewModel*         propertyModel_    { nullptr };    //!< property model
  bool                         visible_          { true };       //!< is visible
  CQChartsGeom::BBox           viewBBox_         { 0, 0, 1, 1 }; //!< view box
  CQChartsGeom::BBox           innerViewBBox_    { 0, 0, 1, 1 }; //!< inner view box
  CQChartsPlotMargin           innerMargin_      { 0, 0, 0, 0 }; //!< inner margin
  CQChartsPlotMargin           outerMargin_      { 10, 10, 10, 10 }; //!< outer margin
  CQChartsDisplayRange*        displayRange_     { nullptr };    //!< value range mapping
  CQChartsGeom::Range          calcDataRange_;                   //!< calc data range
  CQChartsGeom::Range          dataRange_;                       //!< data range
  CQChartsGeom::Range          outerDataRange_;                  //!< outer data range
  ZoomData                     zoomData_;                        //!< zoom data
  CQChartsOptReal              xmin_;                            //!< xmin override
  CQChartsOptReal              ymin_;                            //!< ymin override
  CQChartsOptReal              xmax_;                            //!< xmax override
  CQChartsOptReal              ymax_;                            //!< ymax override
  EveryData                    everyData_;                       //!< every data
  QString                      filterStr_;                       //!< filter
  CQChartsSides                plotBorderSides_  { "tlbr" };     //!< plot border sides
  bool                         plotClip_         { true };       //!< is clipped at plot limits
  CQChartsSides                dataBorderSides_  { "tlbr" };     //!< data border sides
  bool                         dataClip_         { false };      //!< is clipped at data limits
  CQChartsSides                fitBorderSides_   { "tlbr" };     //!< fit border sides
  QString                      titleStr_;                        //!< title string
  QString                      fileName_;                        //!< associated data filename
  CQChartsAxis*                xAxis_            { nullptr };    //!< x axis object
  CQChartsAxis*                yAxis_            { nullptr };    //!< y axis object
  CQChartsPlotKey*             keyObj_           { nullptr };    //!< key object
  CQChartsTitle*               titleObj_         { nullptr };    //!< title object
  CQChartsColumn               xValueColumn_;                    //!< x axis value column
  CQChartsColumn               yValueColumn_;                    //!< y axis value column
  CQChartsColumn               idColumn_;                        //!< unique data id column
                                                                 //!< (signalled)
  CQChartsColumns              tipColumns_;                      //!< tip columns
  CQChartsColumn               visibleColumn_;                   //!< visible column
  ColorColumnData              colorColumnData_;                 //!< color color data
  mutable std::mutex           colorMutex_;                      //!< color mutex
  CQChartsColumn               imageColumn_;                     //!< image column
  ColumnNames                  columnNames_;                     //!< column header names
  CQChartsFont                 font_;                            //!< font
  CQChartsPaletteName          defaultPalette_;                  //!< default palette
  double                       minScaleFontSize_ { 6.0 };        //!< min scaled font size
  double                       maxScaleFontSize_ { 48.0 };       //!< max scaled font size
  bool                         equalScale_       { false };      //!< equal scaled
  bool                         followMouse_      { true };       //!< track object under mouse
  bool                         autoFit_          { false };      //!< auto fit on data change
  bool                         needsAutoFit_     { false };      //!< needs auto fit on next draw
  bool                         initObjTree_      { false };      //!< needs init obj tree
  bool                         preview_          { false };      //!< is preview plot
  int                          previewMaxRows_   { 1000 };       //!< preview max rows
  bool                         sequential_       { false };      //!< is sequential plot
  bool                         queueUpdate_      { true };       //!< is queued update
  bool                         bufferSymbols_    { false };      //!< buffer symbols
  bool                         showBoxes_        { false };      //!< show debug boxes
  bool                         overview_         { false };      //!< is overview
  bool                         invertX_          { false };      //!< x values inverted
  bool                         invertY_          { false };      //!< y values inverted
  bool                         noData_           { false };      //!< is no data
  bool                         debugUpdate_      { false };      //!< debug update
  bool                         debugQuadTree_    { false };      //!< debug quad tree
  ConnectData                  connectData_;                     //!< associated plot data
  PlotObjs                     plotObjs_;                        //!< plot objects
  int                          insideInd_        { 0 };          //!< current inside object ind
  ObjSet                       insideObjs_;                      //!< inside plot objects
  SizeObjSet                   sizeInsideObjs_;                  //!< inside plot objects
                                                                 //!< (size sorted)
  CQChartsPlotObjTree*         plotObjTree_       { nullptr };   //!< plot object quad tree
  bool                         plotObjTreeSet_    { false };     //!< is plot object quad tree set
  bool                         plotObjTreeNotify_ { false };     //!< is plot object quad tree set
  UpdateData                   updateData_;                      //!< update data
  MouseData                    mouseData_;                       //!< mouse event data
  AnimateData                  animateData_;                     //!< animation data
  Buffers                      buffers_;                         //!< draw layer buffers
  Layers                       layers_;                          //!< draw layers
  mutable CQChartsBuffer::Type drawBuffer_;                      //!< objects draw buffer
  IdHidden                     idHidden_;                        //!< hidden object ids
  IndexColumnRows              selIndexColumnRows_;              //!< sel model indices (by col/row)
  CQChartsEditHandles*         editHandles_      { nullptr };    //!< edit controls
  bool                         editing_          { false };      //!< is editing
  Annotations                  annotations_;                     //!< extra annotations
  UpdatesData                  updatesData_;                     //!< updates data
  bool                         fromInvalidate_   { false };      //!< call from invalidate
};

//------

CQCHARTS_NAMED_LINE_DATA(Grid,grid)
CQCHARTS_NAMED_SHAPE_DATA(Node,node)
CQCHARTS_NAMED_LINE_DATA(Edge,edge)
CQCHARTS_NAMED_POINT_DATA(Dot,dot)
CQCHARTS_NAMED_POINT_DATA(Rug,rug)

#endif
