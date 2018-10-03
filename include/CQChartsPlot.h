#ifndef CQChartsPlot_H
#define CQChartsPlot_H

#include <CQChartsColor.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsObjData.h>
#include <CQChartsGroupData.h>
#include <CQChartsPosition.h>
#include <CQChartsModelVisitor.h>
#include <CQChartsTextOptions.h>
#include <CQChartsLayer.h>
#include <CQChartsUtil.h>
#include <CQChartsTypes.h>
#include <CQChartsGeom.h>
#include <CQChartsPlotMargin.h>
#include <CQBaseModel.h>

#include <QAbstractItemModel>
#include <QItemSelection>
#include <QFrame>
#include <QTimer>
#include <QPointer>

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
class CQChartsAnnotation;
class CQChartsTextAnnotation;
class CQChartsArrowAnnotation;
class CQChartsRectAnnotation;
class CQChartsEllipseAnnotation;
class CQChartsPolygonAnnotation;
class CQChartsPolylineAnnotation;
class CQChartsPointAnnotation;
class CQChartsObj;
class CQPropertyViewModel;
class CQChartsDisplayRange;
class CQChartsDisplayTransform;
class CQChartsValueSet;
class CQChartsColorSet;
class CQChartsModelExprMatch;
class CQPropertyViewItem;
class QPainter;

class QSortFilterProxyModel;
class QItemSelectionModel;
class QRubberBand;
class QMenu;

//----

#include <CQChartsEditHandles.h>

//----

class CQChartsPlotUpdateTimer : public QTimer {
 public:
  CQChartsPlotUpdateTimer(CQChartsPlot *plot) :
   plot_(plot) {
    setSingleShot(true);
  }

  CQChartsPlot *plot() const { return plot_; }

 private:
  CQChartsPlot *plot_ { nullptr };
};

//----

CQCHARTS_NAMED_SHAPE_DATA(Plot,plot)
CQCHARTS_NAMED_SHAPE_DATA(Data,data)
CQCHARTS_NAMED_SHAPE_DATA(Fit,fit)

class CQChartsPlot : public QObject,
 public CQChartsObjPlotShapeData<CQChartsPlot>,
 public CQChartsObjDataShapeData<CQChartsPlot>,
 public CQChartsObjFitShapeData <CQChartsPlot> {
  Q_OBJECT

  // view
  Q_PROPERTY(QString viewId READ viewId)

  // type
  Q_PROPERTY(QString typeStr READ typeStr)

  // generic columns and control
  Q_PROPERTY(CQChartsColumn idColumn  READ idColumn  WRITE setIdColumn )
  Q_PROPERTY(CQChartsColumn tipColumn READ tipColumn WRITE setTipColumn)

  // visible, selected
  Q_PROPERTY(bool visible  READ isVisible  WRITE setVisible )
  Q_PROPERTY(bool selected READ isSelected WRITE setSelected)

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
  Q_PROPERTY(bool logX           READ isLogX         WRITE setLogX          )
  Q_PROPERTY(bool logY           READ isLogY         WRITE setLogY          )
  Q_PROPERTY(bool autoFit        READ isAutoFit      WRITE setAutoFit       )
  Q_PROPERTY(bool preview        READ isPreview      WRITE setPreview       )
  Q_PROPERTY(int  previewMaxRows READ previewMaxRows WRITE setPreviewMaxRows)
  Q_PROPERTY(bool showBoxes      READ showBoxes      WRITE setShowBoxes     )

  Q_PROPERTY(int updateTimeout READ updateTimeout WRITE setUpdateTimeout)

 public:
  using SelMod = CQChartsSelMod;

  // associated plot for overlay/y1y2
  struct ConnectData {
    bool          x1x2    { false };   // is double x axis plot
    bool          y1y2    { false };   // is double y axis plot
    bool          overlay { false };   // is overlay plot
    CQChartsPlot *next    { nullptr }; // next plot
    CQChartsPlot *prev    { nullptr }; // previos plot

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
    enum class Direction {
      VERTICAL,
      HORIZONTAL
    };

    using Values = std::vector<ProbeValue>;

    double    x         { 0.0 };
    double    y         { 0.0 };
    Values    xvals;
    Values    yvals;
    Direction direction { Direction::VERTICAL };
  };

  using OptReal = boost::optional<double>;

  using ModelP          = QSharedPointer<QAbstractItemModel>;
  using SelectionModelP = QPointer<QItemSelectionModel>;

  using PlotObjs    = std::vector<CQChartsPlotObj*>;
  using Annotations = std::vector<CQChartsAnnotation *>;
  using Objs        = std::vector<CQChartsObj*>;

  using ModelIndices = std::vector<QModelIndex>;

  using ColumnType = CQBaseModel::Type;

  using Plots = std::vector<CQChartsPlot*>;

  using Buffers = std::map<CQChartsBuffer::Type,CQChartsBuffer *>;
  using Layers  = std::map<CQChartsLayer::Type,CQChartsLayer *>;

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

  void setSelectionModel(QItemSelectionModel *sm);
  QItemSelectionModel *selectionModel() const;

  //---

  virtual bool isHierarchical() const;

  //---

  CQCharts *charts() const;

  QString typeName() const;

  const QString &id() const { return id_; }
  void setId(const QString &s) { id_ = s; }

  QString pathId() const;

  bool isVisible() const { return visible_; }
  void setVisible(bool b);

  bool isSelected() const { return selected_; }
  void setSelected(bool b);

  //---

  const CQChartsDisplayRange &displayRange() const;
  void setDisplayRange(const CQChartsDisplayRange &r);

  const CQChartsDisplayTransform &displayTransform() const;
  void setDisplayTransform(const CQChartsDisplayTransform &t);

  const CQChartsGeom::Range &dataRange() const { return dataRange_; }
  void setDataRange(const CQChartsGeom::Range &r, bool update=true);

  double dataScaleX() const;
  void setDataScaleX(double r);

  double dataScaleY() const;
  void setDataScaleY(double r);

  void updateDataScaleX(double r);
  void updateDataScaleY(double r);

  double dataOffsetX() const;
  void setDataOffsetX(double x);

  double dataOffsetY() const;
  void setDataOffsetY(double y);

  //---

  const OptReal &xmin() const { return xmin_; }
  void setXMin(const OptReal &r);

  const OptReal &xmax() const { return xmax_; }
  void setXMax(const OptReal &r);

  const OptReal &ymin() const { return ymin_; }
  void setYMin(const OptReal &r);

  const OptReal &ymax() const { return ymax_; }
  void setYMax(const OptReal &r);

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

  const QString &xLabel() const { return xLabel_; }
  void setXLabel(const QString &s) { xLabel_ = s; }

  const QString &yLabel() const { return yLabel_; }
  void setYLabel(const QString &s) { yLabel_ = s; }

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

  // scaled font size
  double minScaleFontSize() const { return minScaleFontSize_; }
  void setMinScaleFontSize(double r) { minScaleFontSize_ = r; }

  double maxScaleFontSize() const { return maxScaleFontSize_; }
  void setMaxScaleFontSize(double r) { maxScaleFontSize_ = r; }

  //---

  // key
  bool isKeyVisible() const;
  void setKeyVisible(bool b);

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

  //---

  bool showBoxes() const { return showBoxes_; }
  void setShowBoxes(bool b);

  //---

  int updateTimeout() const { return updateTimeout_; }
  void setUpdateTimeout(int i) { updateTimeout_ = i; }

  //---

  // bbox in view range
  const CQChartsGeom::BBox &viewBBox() const { return viewBBox_; }
  void setViewBBox(const CQChartsGeom::BBox &bbox);

  const CQChartsGeom::BBox &innerViewBBox() const { return innerViewBBox_; }

  // get set in view rect (as QRectF)
  QRectF viewRect() const;
  void setViewRect(const QRectF &r);

  QRectF innerViewRect() const;

  // data range
  QRectF range() const;
  void setRange(const QRectF &r);

  //---

  QRectF calcDataRect () const;
  QRectF outerDataRect() const;
  QRectF dataRect     () const;

  //---

  // inner margin
  const CQChartsLength &innerMarginLeft() const { return innerMargin_.left(); }
  void setInnerMarginLeft(const CQChartsLength &l);

  const CQChartsLength &innerMarginTop() const { return innerMargin_.top(); }
  void setInnerMarginTop(const CQChartsLength &t);

  const CQChartsLength &innerMarginRight() const { return innerMargin_.right(); }
  void setInnerMarginRight(const CQChartsLength &r);

  const CQChartsLength &innerMarginBottom() const { return innerMargin_.bottom(); }
  void setInnerMarginBottom(const CQChartsLength &b);

  void setInnerMargin(const CQChartsLength &l, const CQChartsLength &t,
                      const CQChartsLength &r, const CQChartsLength &b);

  // outer margin
  const CQChartsLength &outerMarginLeft() const { return outerMargin_.left(); }
  void setOuterMarginLeft(const CQChartsLength &l);

  const CQChartsLength &outerMarginTop() const { return outerMargin_.top(); }
  void setOuterMarginTop(const CQChartsLength &t);

  const CQChartsLength &outerMarginRight() const { return outerMargin_.right(); }
  void setOuterMarginRight(const CQChartsLength &r);

  const CQChartsLength &outerMarginBottom() const { return outerMargin_.bottom(); }
  void setOuterMarginBottom(const CQChartsLength &b);

  void setOuterMargin(const CQChartsLength &l, const CQChartsLength &t,
                      const CQChartsLength &r, const CQChartsLength &b);

  //---

  // aspect ratio
  double aspect() const;

  //---

  CQChartsAxis *xAxis() const { return xAxis_; }
  CQChartsAxis *yAxis() const { return yAxis_; }

  CQChartsPlotKey *key() const { return keyObj_; }

  CQChartsTitle *title() const { return titleObj_; }

  //---

  bool isOverlay() const { return connectData_.overlay; }
  void setOverlay(bool b, bool notify=true);

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

  bool isLogX() const { return logX_; }
  void setLogX(bool b);

  bool isLogY() const { return logY_; }
  void setLogY(bool b);

  //---

  CQChartsPlot *prevPlot() const { return connectData_.prev; }
  CQChartsPlot *nextPlot() const { return connectData_.next; }

  void setNextPlot(CQChartsPlot *plot, bool notify=true);
  void setPrevPlot(CQChartsPlot *plot, bool notify=true);

  CQChartsPlot *firstPlot() const;
  CQChartsPlot *lastPlot () const;

  bool isFirstPlot() const {
    return (const_cast<CQChartsPlot *>(this)->firstPlot() == this);
  }

  bool isOverlayFirstPlot() const {
    return (isOverlay() && isFirstPlot());
  }

  bool isOverlayOtherPlot() const {
    return (isOverlay() && ! isFirstPlot());
  }

  void overlayPlots(Plots &plots) const;

  void x1x2Plots(CQChartsPlot* &plot1, CQChartsPlot* &plot2);
  void y1y2Plots(CQChartsPlot* &plot1, CQChartsPlot* &plot2);

  void resetConnectData(bool notify=true);

  //---

  void startAnimateTimer();

  virtual void animateStep() { }

  //---

  CQPropertyViewModel *propertyModel() const;

  // add plot properties to model
  virtual void addProperties();

  void addSymbolProperties(const QString &path, const QString &prefix="");

  void addLineProperties(const QString &path, const QString &prefix);
  void addFillProperties(const QString &path, const QString &prefix);
  void addTextProperties(const QString &path, const QString &prefix);

  void addColorMapProperties();

  bool setProperties(const QString &properties);

  bool setProperty(const QString &name, const QVariant &value);
  bool getProperty(const QString &name, QVariant &value);

  void propertyItemSelected(QObject *obj, const QString &path);

  //---

  void updateMargins(bool update=true);

  //---

  QModelIndex normalizeIndex(const QModelIndex &ind) const;
  QModelIndex unnormalizeIndex(const QModelIndex &ind) const;

  void proxyModels(std::vector<QSortFilterProxyModel *> &proxyModels,
                   QAbstractItemModel* &sourceModel) const;

  //---

  CQChartsValueSet *addValueSet(const QString &name, double min, double max);
  CQChartsValueSet *addValueSet(const QString &name);
  CQChartsValueSet *addColorSet(const QString &name);

  CQChartsValueSet *getValueSet(const QString &name) const;
  CQChartsColorSet *getColorSet(const QString &name) const;

  void clearValueSets();
  void deleteValueSets();

  const CQChartsColumn &valueSetColumn(const QString &name) const;
  bool setValueSetColumn(const QString &name, const CQChartsColumn &column);

  bool isValueSetMapped(const QString &name) const;
  void setValueSetMapped(const QString &name, bool b);

  double valueSetMapMin(const QString &name) const;
  void setValueSetMapMin(const QString &name, double min);

  double valueSetMapMax(const QString &name) const;
  void setValueSetMapMax(const QString &name, double max);

  bool colorSetColor(const QString &name, int i, CQChartsColor &color);

  void initValueSets();

  void addColumnValues(const CQChartsColumn &column, CQChartsValueSet &valueSet);

  //---

  class ModelVisitor : public CQChartsModelVisitor {
   public:
    ModelVisitor();

    virtual ~ModelVisitor();

    CQChartsPlot *plot() const { return plot_; }
    void setPlot(CQChartsPlot *p) { plot_ = p; }

    void init();

    State preVisit(QAbstractItemModel *model, const VisitData &data) override;

   private:
    CQChartsPlot*           plot_ { nullptr };
    int                     vrow_ { 0 };
    CQChartsModelExprMatch* expr_ { nullptr };
  };

  void visitModel(ModelVisitor &visitor);

  void addValueSetRow(const ModelVisitor::VisitData &data);

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

  QString modelHeaderString(const CQChartsColumn &column, bool &ok) const;
  QString modelHeaderString(const CQChartsColumn &column, int role, bool &ok) const;

  QString modelHeaderString(int section, Qt::Orientation orient, bool &ok) const;
  QString modelHeaderString(int section, Qt::Orientation orient, int role, bool &ok) const;

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

  void beginSelect();

  void addSelectIndex(const CQChartsModelIndex &ind);
  void addSelectIndex(int row, int col, const QModelIndex &parent=QModelIndex());
  void addSelectIndex(const QModelIndex &ind);

  void endSelect();

  //---

  double logValue(double x, int base=10) const;
  double expValue(double x, int base=10) const;

  //---

  QPointF positionToPlot (const CQChartsPosition &pos) const;
  QPointF positionToPixel(const CQChartsPosition &pos) const;

  //---

  double lengthPlotSize(const CQChartsLength &len, bool horizontal) const;

  double lengthPlotWidth (const CQChartsLength &len) const;
  double lengthPlotHeight(const CQChartsLength &len) const;

  double lengthPixelSize(const CQChartsLength &len, bool horizontal) const;

  double lengthPixelWidth (const CQChartsLength &len) const;
  double lengthPixelHeight(const CQChartsLength &len) const;

  //---

  void windowToPixel(double wx, double wy, double &px, double &py) const;
  void windowToView (double wx, double wy, double &vx, double &vy) const;

  double windowToViewWidth (double wx) const;
  double windowToViewHeight(double wy) const;

  double viewToWindowWidth (double vx) const;
  double viewToWindowHeight(double vy) const;

  void pixelToWindow(double px, double py, double &wx, double &wy) const;
  void viewToWindow (double vx, double vy, double &wx, double &wy) const;

  void windowToPixel(const CQChartsGeom::Point &w, CQChartsGeom::Point &p) const;
  void pixelToWindow(const CQChartsGeom::Point &p, CQChartsGeom::Point &w) const;

  CQChartsGeom::Point windowToPixel(const CQChartsGeom::Point &w) const;
  CQChartsGeom::Point windowToView (const CQChartsGeom::Point &w) const;
  CQChartsGeom::Point pixelToWindow(const CQChartsGeom::Point &p) const;

  QPointF windowToPixel(const QPointF &w) const;
  QPointF pixelToWindow(const QPointF &w) const;

  void windowToPixel(const CQChartsGeom::BBox &wrect, CQChartsGeom::BBox &prect) const;
  void pixelToWindow(const CQChartsGeom::BBox &prect, CQChartsGeom::BBox &wrect) const;

  CQChartsGeom::BBox windowToPixel(const CQChartsGeom::BBox &wrect) const;
  CQChartsGeom::BBox pixelToWindow(const CQChartsGeom::BBox &prect) const;

  double pixelToSignedWindowWidth (double ww) const;
  double pixelToSignedWindowHeight(double wh) const;

  double pixelToWindowSize(double ps, bool horizontal) const;

  double pixelToWindowWidth (double pw) const;
  double pixelToWindowHeight(double ph) const;

  double windowToSignedPixelWidth (double ww) const;
  double windowToSignedPixelHeight(double wh) const;

  double windowToPixelWidth (double ww) const;
  double windowToPixelHeight(double wh) const;

  //---

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

  // update data range (calls calcRange)
  virtual void updateRange(bool apply=true);

  virtual CQChartsGeom::Range calcRange() = 0;

  // update plot objects (clear objects, objects updated on next redraw)
  virtual void updateObjs();

  // reset range and objects
  void clearRangeAndObjs();

  // recalc range and clear objects (objects updated on next redraw)
  void updateRangeAndObjs();

 private:
  void updateRangeAndObjsInternal();

  // (re)initialize grouped plot objects
  void initGroupedPlotObjs();

  // (re)initialize plot objects
  void initPlotObjs();

 public:
  // (re)initialize plot objects (called by initPlotObjs)
  virtual bool initObjs() = 0;

  // add plotObjects to quad tree (create no data object in no objects)
  void initObjTree();

  // routine to run after plot set up (usually fixes up some defaults)
  virtual void postInit();

  //---

  CQChartsGeom::BBox calcDataRange(bool adjust=true) const;

  CQChartsGeom::BBox getDataRange() const;

  void applyDataRange(bool propagate=true);

  void applyDisplayTransform(bool propagate=true);

  void adjustDataRange();

  //---

  CQPropertyViewItem *addProperty(const QString &path, QObject *object,
                                  const QString &name, const QString &alias="");

  void addPlotObject(CQChartsPlotObj *obj);

  void clearPlotObjects();

  bool updatePlotObjects(const CQChartsGeom::Point &w);

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

  //---

  const CQChartsColumn &xValueColumn() const { return xValueColumn_; }
  void setXValueColumn(const CQChartsColumn &column);

  const CQChartsColumn &yValueColumn() const { return yValueColumn_; }
  void setYValueColumn(const CQChartsColumn &column);

  const CQChartsColumn &idColumn() const { return idColumn_; }
  void setIdColumn(const CQChartsColumn &column);

  const CQChartsColumn &tipColumn() const { return tipColumn_; }
  void setTipColumn(const CQChartsColumn &column);

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
  bool editMousePress  (const QPointF &p);
  bool editMouseMove   (const QPointF &p, bool first=false);
  bool editMouseMotion (const QPointF &p);
  bool editMouseRelease(const QPointF &p);

  virtual bool editPress  (const CQChartsGeom::Point &p, const CQChartsGeom::Point &w);
  virtual bool editMove   (const CQChartsGeom::Point &p, const CQChartsGeom::Point &w,
                           bool first=false);
  virtual bool editMotion (const CQChartsGeom::Point &p, const CQChartsGeom::Point &w);
  virtual bool editRelease(const CQChartsGeom::Point &p, const CQChartsGeom::Point &w);

  virtual void editMoveBy(const QPointF &d);

  void deselectAllObjs();

  void deselectAll();

  // handle key press
  virtual void keyPress(int key, int modifier);

  // get tip text at point
  virtual bool tipText(const CQChartsGeom::Point &p, QString &tip) const;

  // handle rect select
  bool rectSelect(const CQChartsGeom::BBox &r, SelMod selMod);

  //---

  void selectedObjs(Objs &objs) const;

  void selectedPlotObjs(PlotObjs &objs) const;

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
  virtual void zoomFull();

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

  // called after resize
  virtual void handleResize();

  //---

  void updateKeyPosition(bool force=false);

  CQChartsGeom::BBox displayRangeBBox() const;

  CQChartsGeom::BBox calcDataPixelRect() const;
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

  CQChartsTextAnnotation     *addTextAnnotation    (const CQChartsPosition &pos,
                                                    const QString &text);
  CQChartsArrowAnnotation    *addArrowAnnotation   (const CQChartsPosition &start,
                                                    const CQChartsPosition &end);
  CQChartsRectAnnotation     *addRectAnnotation    (const CQChartsPosition &start,
                                                    const CQChartsPosition &end);
  CQChartsEllipseAnnotation  *addEllipseAnnotation (const CQChartsPosition &center,
                                                    const CQChartsLength &xRadius,
                                                    const CQChartsLength &yRadius);
  CQChartsPolygonAnnotation  *addPolygonAnnotation (const QPolygonF &points);
  CQChartsPolylineAnnotation *addPolylineAnnotation(const QPolygonF &points);
  CQChartsPointAnnotation    *addPointAnnotation   (const CQChartsPosition &pos,
                                                    const CQChartsSymbol &type);

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
  void setLayerActive1(const CQChartsLayer::Type &type, bool b);

  void invalidateLayer1(const CQChartsBuffer::Type &layerType);

  //---

 public:
  // draw plot parts
  virtual void drawParts(QPainter *painter);

  // draw background layer plot parts
  virtual void drawBackgroundParts(QPainter *painter);

  // draw middle layer plot parts
  virtual void drawMiddleParts(QPainter *painter);

  // draw foreground layer plot parts
  virtual void drawForegroundParts(QPainter *painter);

  // draw overlay layer plot parts
  virtual void drawOverlayParts(QPainter *painter);

  //---

  // draw background (layer and detail)
  virtual bool hasBackgroundLayer() const;

  virtual void drawBackgroundLayer(QPainter *painter);

  virtual bool hasBackground() const;

  virtual void drawBackground(QPainter *painter);

  void drawBackgroundSides(QPainter *painter, const QRectF &rect, const CQChartsSides &sides);

  // draw axes on background
  virtual bool hasBgAxes() const;

  virtual void drawBgAxes(QPainter *painter);

  // draw key on background
  virtual bool hasBgKey() const;

  virtual void drawBgKey(QPainter *painter);

  //---

  // draw objects
  bool hasGroupedObjs(const CQChartsLayer::Type &layerType) const;

  void drawGroupedObjs(QPainter *painter, const CQChartsLayer::Type &layerType);

  virtual bool hasObjs(const CQChartsLayer::Type &layerType) const;

  virtual void drawObjs(QPainter *painter, const CQChartsLayer::Type &type);

  //---

  // draw axes on foreground
  virtual bool hasFgAxes() const;

  virtual void drawFgAxes(QPainter *painter);

  // draw key on foreground
  virtual bool hasFgKey() const;

  virtual void drawFgKey(QPainter *painter);

  // draw title
  virtual bool hasTitle() const;

  virtual void drawTitle(QPainter *painter);

  // draw annotations
  virtual bool hasGroupedAnnotations(const CQChartsLayer::Type &layerType) const;

  void drawGroupedAnnotations(QPainter *painter, const CQChartsLayer::Type &layerType);

  virtual bool hasAnnotations(const CQChartsLayer::Type &layerType) const;

  virtual void drawAnnotations(QPainter *painter, const CQChartsLayer::Type &layerType);

  // draw foreground
  virtual bool hasForeground() const;

  virtual void drawForeground(QPainter *painter);

  // draw debug boxes
  virtual bool hasGroupedBoxes() const;

  void drawGroupedBoxes(QPainter *painter);

  virtual bool hasBoxes() const;

  virtual void drawBoxes(QPainter *painter);

  // draw edit handles
  bool hasGroupedEditHandles() const;

  void drawGroupedEditHandles(QPainter *painter);

  bool hasEditHandles() const;

  virtual void drawEditHandles(QPainter *painter);

  //---

  // set clip rect
  void setClipRect(QPainter *painter);

  //---

  virtual bool selectInvalidateObjs() const { return false; }

  //---

  QPainter *beginPaint(CQChartsBuffer *layer, QPainter *painter, const QRectF &rect=QRectF());
  void      endPaint  (CQChartsBuffer *layer);

  //---

  CQChartsPlotKey *getFirstPlotKey() const;

  //---

#if 0
  void drawLine(QPainter *painter, const QPointF &p1, const QPointF &p2,
                const CQChartsLineData &data);
#endif

  //---

#if 0
  void drawSymbol(QPainter *painter, const QPointF &p, const CQChartsSymbolData &data);
#endif

#if 0
  void drawSymbol(QPainter *painter, const QPointF &p, const CQChartsSymbol &symbol,
                  double size, bool stroked, const QColor &strokeColor,
                  double lineWidth, bool filled, const QColor &fillColor);
#endif

  void drawSymbol(QPainter *painter, const QPointF &p, const CQChartsSymbol &symbol, double size);

  void drawSymbol(QPainter *painter, const QPointF &p, const CQChartsSymbol &symbol,
                  double size, const QPen &pen, const QBrush &brush);

  //---

  void drawTextAtPoint(QPainter *painter, const QPointF &point,
                       const QString &text, const QPen &pen,
                       const CQChartsTextOptions &options=CQChartsTextOptions()) const;

  void drawTextInBox(QPainter *painter, const QRectF &rect,
                     const QString &text, const QPen &pen,
                     const CQChartsTextOptions &options=CQChartsTextOptions()) const;

  void drawContrastText(QPainter *painter, double x, double y,
                        const QString &text, const QPen &pen) const;

  //---

  // debug draw (default to red boxes)
  void drawWindowColorBox(QPainter *painter, const CQChartsGeom::BBox &bbox,
                          const QColor &c=Qt::red);

  void drawColorBox(QPainter *painter, const CQChartsGeom::BBox &bbox,
                    const QColor &c=Qt::red);

  //---

  void drawPieSlice(QPainter *painter, const CQChartsGeom::Point &c,
                    double ri, double ro, double a1, double a2) const;

  //---

  void setPenBrush(QPen &pen, QBrush &brush,
                   bool stroked, const QColor &strokeColor, double strokeAlpha,
                   const CQChartsLength &strokeWidth, const CQChartsLineDash &strokeDash,
                   bool filled, const QColor &fillColor, double fillAlpha,
                   const CQChartsFillPattern &pattern=CQChartsFillPattern::Type::SOLID) const;

  void setPen(QPen &pen, bool stroked,
              const QColor &strokeColor=QColor(), double strokeAlpha=1.0,
              const CQChartsLength &strokeWidth=CQChartsLength("0px"),
              const CQChartsLineDash &strokeDash=CQChartsLineDash()) const;

  void setBrush(QBrush &brush, bool filled,
                const QColor &fillColor=QColor(), double fillAlpha=1.0,
                const CQChartsFillPattern &pattern=CQChartsFillPattern::Type::SOLID) const;

  double limitLineWidth(double w) const;

  //---

  void updateObjPenBrushState(const CQChartsObj *obj, QPen &pen, QBrush &brush) const;

  void updateInsideObjPenBrushState  (QPen &pen, QBrush &brush, bool outline=true) const;
  void updateSelectedObjPenBrushState(QPen &pen, QBrush &brush) const;

  QColor insideColor(const QColor &c) const;
  QColor selectedColor(const QColor &c) const;

  //---

  // get palette color for ith value of n values
  virtual QColor interpPaletteColor(int i, int n, bool scale=false) const;

  QColor interpPaletteColor(double r, bool scale=false) const;
  QColor interpIndPaletteColor(int ind, double r, bool scale=false) const;

  virtual QColor interpGroupPaletteColor(int ig, int ng, int i, int n, bool scale=false) const;
  QColor interpGroupPaletteColor(double r1, double r2, double dr) const;

  QColor interpThemeColor(double r) const;

  //---

  QColor calcTextColor(const QColor &bg) const;

  //---

  ColumnType columnValueType(const CQChartsColumn &column,
                             const ColumnType &defType=ColumnType::STRING) const;

  bool columnValueType(const CQChartsColumn &column, ColumnType &columnType,
                       CQChartsNameValues &nameValues,
                       const ColumnType &defType=ColumnType::STRING) const;

  bool columnTypeStr(const CQChartsColumn &column, QString &typeStr) const;

  bool setColumnTypeStr(const CQChartsColumn &column, const QString &typeStr);

  bool columnDetails(const CQChartsColumn &column, QString &typeName,
                     QVariant &minValue, QVariant &maxValue) const;

  //---

  CQChartsModelData *getModelData() const;

  //---

  bool getHierColumnNames(const QModelIndex &parent, int row, const CQChartsColumns &nameColumns,
                          const QString &separator, QStringList &nameStrs, ModelIndices &nameInds);

  //---

  // get/set/reset id hidden
  bool isSetHidden(int id) const;

  void setSetHidden(int id, bool hidden);

  void resetSetHidden();

  virtual void hiddenChanged();

  //---

  void update();

  //---

  // draw plot
  void draw(QPainter *painter);

  // draw plot layer
  void drawLayer(QPainter *painter, CQChartsLayer::Type type);

  // draw plot layer type
  CQChartsLayer::Type drawLayerType() const { return drawLayer_; }

  //---

  bool printLayer(CQChartsLayer::Type type, const QString &filename);

 protected slots:
  void animateSlot();

  // model change slots
  void modelChangedSlot();

  void currentModelChangedSlot();

  //---

  void updateTimerSlot();

  void selectionSlot();

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

 protected:
  using ObjSet     = std::set<CQChartsObj*>;
  using SizeObjSet = std::map<double,ObjSet>;

 protected:
  void connectModel();
  void disconnectModel();

  void objsAtPoint(const CQChartsGeom::Point &p, Objs &objs) const;

  void plotObjsAtPoint(const CQChartsGeom::Point &p, PlotObjs &objs) const;

  void objsTouchingRect(const CQChartsGeom::BBox &r, Objs &objs) const;

 protected:
  using IdHidden        = std::map<int,bool>;
  using Rows            = std::set<int>;
  using ColumnRows      = std::map<int,Rows>;
  using IndexColumnRows = std::map<QModelIndex,ColumnRows>;
  using ValueSets       = std::map<QString,CQChartsValueSet *>;

  struct EveryData {
    bool enabled { false };
    int  start   { 0 };
    int  end     { std::numeric_limits<int>::max() };
    int  step    { 1 };
  };

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

  struct MouseData {
    QPointF                    pressPoint { 0, 0 };
    QPointF                    movePoint  { 0, 0 };
    bool                       pressed    { false };
    DragObj                    dragObj    { DragObj::NONE };
    CQChartsResizeHandle::Side dragSide   { CQChartsResizeHandle::Side::NONE };
  };

  struct AnimateData {
    QTimer* timer   { nullptr };
    int     tickLen { 30 };
  };

  //---

  CQChartsView*             view_             { nullptr };    // parent view
  CQChartsPlotType*         type_             { nullptr };    // plot type data
  ModelP                    model_;                           // abstract model
  bool                      modelNameSet_     { false };      // model name set from plot
  SelectionModelP           selectionModel_;                  // selection model
  QString                   id_;                              // plot id
  bool                      visible_          { true };       // is visible
  bool                      selected_         { false };      // is selected
  CQChartsGeom::BBox        viewBBox_         { 0, 0, 1, 1 }; // view box
  CQChartsGeom::BBox        innerViewBBox_    { 0, 0, 1, 1 }; // inner view box
  CQChartsPlotMargin        innerMargin_      { 0, 0, 0, 0 }; // inner margin
  CQChartsPlotMargin        outerMargin_      { 10, 10, 10, 10 }; // outer margin
  CQChartsDisplayRange*     displayRange_     { nullptr };    // value range mapping
  CQChartsDisplayTransform* displayTransform_ { nullptr };    // value range transform (zoom/pan)
  CQChartsGeom::Range       calcDataRange_;                   // calc data range
  CQChartsGeom::Range       dataRange_;                       // data range
  CQChartsGeom::Range       outerDataRange_;                  // outer data range
  double                    dataScaleX_       { 1.0 };        // data scale (zoom in x direction)
  double                    dataScaleY_       { 1.0 };        // data scale (zoom in y direction)
  CQChartsGeom::Point       dataOffset_       { 0.0, 0.0 };   // data offset (pan)
  OptReal                   xmin_;                            // xmin override
  OptReal                   ymin_;                            // ymin override
  OptReal                   xmax_;                            // xmax override
  OptReal                   ymax_;                            // ymax override
  EveryData                 everyData_;                       // every data
  QString                   filterStr_;                       // filter
  CQChartsSides             plotBorderSides_  { "tlbr" };     // plot border sides
  bool                      plotClip_         { true };       // is clipped at plot limits
  CQChartsSides             dataBorderSides_  { "tlbr" };     // data border sides
  bool                      dataClip_         { false };      // is clipped at data limits
  CQChartsSides             fitBorderSides_   { "tlbr" };     // fit border sides
  QString                   titleStr_;                        // title string
  QString                   fileName_;                        // associated data filename
  QString                   xLabel_;                          // x label override
  QString                   yLabel_;                          // y label override
  CQChartsAxis*             xAxis_            { nullptr };    // x axis object
  CQChartsAxis*             yAxis_            { nullptr };    // y axis object
  CQChartsPlotKey*          keyObj_           { nullptr };    // key object
  CQChartsTitle*            titleObj_         { nullptr };    // tilte object
  CQChartsColumn            xValueColumn_;                    // x axis value column
  CQChartsColumn            yValueColumn_;                    // y axis value column
  CQChartsColumn            idColumn_;                        // unique data id column (signalled)
  CQChartsColumn            tipColumn_;                       // tip column
  double                    minScaleFontSize_ { 6.0 };        // min scaled font size
  double                    maxScaleFontSize_ { 48.0 };       // max scaled font size
  bool                      equalScale_       { false };      // equal scaled
  bool                      followMouse_      { true };       // track object under mouse
  bool                      autoFit_          { false };      // auto fit on data change
  bool                      needsAutoFit_     { false };      // needs auto fit on next draw
  bool                      preview_          { false };      // is preview plot
  int                       previewMaxRows_   { 1000 };       // preview max rows
  bool                      showBoxes_        { false };      // show debug boxes
  bool                      invertX_          { false };      // x values inverted
  bool                      invertY_          { false };      // y values inverted
  bool                      logX_             { false };      // x values log scaled
  bool                      logY_             { false };      // y values log scaled
  bool                      noData_           { false };      // is no data
  ConnectData               connectData_;                     // associated plot data
  PlotObjs                  plotObjs_;                        // plot objects
  ValueSets                 valueSets_;                       // named value sets
  int                       insideInd_        { 0 };          // current inside object ind
  ObjSet                    insideObjs_;                      // inside plot objects
  SizeObjSet                sizeInsideObjs_;                  // inside plot objects (size sorted)
  CQChartsPlotObjTree*      plotObjTree_      { nullptr };    // plot object quad tree
  MouseData                 mouseData_;                       // mouse event data
  AnimateData               animateData_;                     // animation data
  Buffers                   buffers_;                         // draw layer bufferss
  Layers                    layers_;                          // draw layers
  CQChartsBuffer::Type      drawBuffer_;                      // objects draw buffer
  CQChartsLayer::Type       drawLayer_;                       // objects draw layer
  IdHidden                  idHidden_;                        // hidden object ids
  IndexColumnRows           selIndexColumnRows_;              // sel model indices (by col/row)
  QItemSelection            itemSelection_;                   // selected model indices
  CQChartsPlotUpdateTimer*  updateTimer_      { nullptr };    // update timer
  int                       updateTimeout_    { 100 };        // update timeout
  CQChartsEditHandles       editHandles_;                     // edit controls
  Annotations               annotations_;                     // extra annotations
  bool                      fromInvalidate_   { false };      // call from invalidate
};

//------

CQCHARTS_NAMED_LINE_DATA(Grid,grid)
CQCHARTS_NAMED_SHAPE_DATA(Node,node)
CQCHARTS_NAMED_LINE_DATA(Edge,edge)
CQCHARTS_NAMED_POINT_DATA(Dot,dot)
CQCHARTS_NAMED_POINT_DATA(Rug,rug)

#endif
