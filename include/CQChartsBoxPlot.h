#ifndef CQChartsBoxPlot_H
#define CQChartsBoxPlot_H

#include <CQChartsGroupPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsUtil.h>
#include <CQChartsValueInd.h>
#include <CQChartsBoxWhisker.h>
#include <CQChartsGeom.h>

#include <map>

//---

/*!
 * \brief Box plot type
 * \ingroup Charts
 */
class CQChartsBoxPlotType : public CQChartsGroupPlotType {
 public:
  CQChartsBoxPlotType();

  QString name() const override { return "boxplot"; }
  QString desc() const override { return "BoxPlot"; }

  Dimension dimension() const override { return Dimension::ONE_D; }

  void addParameters() override;

  QString yColumnName() const override { return "value"; }

  bool allowXAxisIntegral() const override { return false; }

  bool allowXLog() const override { return false; }

  bool canProbe() const override { return true; }

  QString description() const override;

  //---

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsBoxPlot;

/*!
 * \brief box value
 * \ingroup Charts
 */
struct CQChartsBoxPlotValue {
  CQChartsBoxPlotValue() = default;

  CQChartsBoxPlotValue(double value, const QModelIndex &ind) :
   value(value), ind(ind) {
  }

  operator double() const { return value; }

  double      value { 0.0 }; //!< value
  QModelIndex ind;           //!< model index
};

//---

using CQChartsBoxPlotWhisker = CQChartsBoxWhiskerT<CQChartsBoxPlotValue>;

/*!
 * \brief whisker data
 * \ingroup Charts
 */
struct CQChartsBoxWhiskerData {
  using Outliers = std::vector<double>;

  QModelIndex ind;             //!< model index
  QString     name;            //!< x value name
  double      x       { 0.0 }; //!< x value
  CQStatData  statData;        //!< stats data
  Outliers    outliers;        //!< outlier y values
  double      dataMin { 0.0 }; //!< min data value
  double      dataMax { 1.0 }; //!< max data value
};

//---

/*!
 * \brief Box Plot Base object
 * \ingroup Charts
 */
class CQChartsBoxPlotObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  //! horizontal text data
  struct HText {
    double       xl     { 0.0 };
    double       xr     { 0.0 };
    double       y      { 0.0 };
    QString      text;
    bool         onLeft { false };
    mutable BBox bbox;

    HText(double xl, double xr, double y, const QString &text, bool onLeft) :
     xl(xl), xr(xr), y(y), text(text), onLeft(onLeft) {
    }
  };

  //! vertical text data
  struct VText {
    double       yb       { 0.0 };
    double       yt       { 0.0 };
    double       x        { 0.0 };
    QString      text;
    bool         onBottom { false };
    mutable BBox bbox;

    VText(double yb, double yt, double x, const QString &text, bool onBottom) :
     yb(yb), yt(yt), x(x), text(text), onBottom(onBottom) {
    }
  };

  using HTexts = std::vector<HText>;
  using VTexts = std::vector<VText>;

 public:
  using Plot  = CQChartsBoxPlot;
  using Angle = CQChartsAngle;

 public:
  CQChartsBoxPlotObj(const Plot *plot, const BBox &rect, const ColorInd &is=ColorInd(),
                     const ColorInd &ig=ColorInd(), const ColorInd &iv=ColorInd());

  void clearDrawBBoxes();

  void addDrawBBox(const BBox &bbox);

  bool checkDrawBBox(const BBox &bbox) const;

  bool drawHText(PaintDevice *device, double xl, double xr, double y,
                 const QString &text, bool onLeft, BBox &bbox) const;
  bool drawVText(PaintDevice *device, double yt, double yb, double x,
                 const QString &text, bool onBottom, BBox &bbox) const;

  void addHBBox(BBox &pbbox, double xl, double xr, double y, const QString &text,
                bool onLeft) const;
  void addVBBox(BBox &pbbox, double yb, double yt, double x, const QString &text,
                bool onBottom) const;

 protected:
  using BBoxes = std::vector<BBox>;

  const Plot* plot_ { nullptr }; //!< parent plot
  BBoxes      drawBBoxes_;
};

//---

/*!
 *  \brief box plot whisker object
 */
class CQChartsBoxPlotWhiskerObj : public CQChartsBoxPlotObj {
  Q_OBJECT

  Q_PROPERTY(double pos         READ pos        )
  Q_PROPERTY(double min         READ min        )
  Q_PROPERTY(double lowerMedian READ lowerMedian)
  Q_PROPERTY(double median      READ median     )
  Q_PROPERTY(double upperMedian READ upperMedian)
  Q_PROPERTY(double max         READ max        )
  Q_PROPERTY(double mean        READ mean       )
  Q_PROPERTY(double stddev      READ stddev     )
  Q_PROPERTY(double notch       READ notch      )

 public:
  using Plot    = CQChartsBoxPlot;
  using Symbol  = CQChartsSymbol;
  using Whisker = CQChartsBoxPlotWhisker;

 public:
  CQChartsBoxPlotWhiskerObj(const Plot *plot, const BBox &rect, int setId, int groupInd,
                            const Whisker *whisker, const ColorInd &is, const ColorInd &ig);

  //---

  QString typeName() const override { return "whisker"; }

  //---

  double pos        () const;
  double min        () const;
  double lowerMedian() const;
  double median     () const;
  double upperMedian() const;
  double max        () const;
  double mean       () const;
  double stddev     () const;
  double notch      () const;

  //---

  QString calcId   () const override;
  QString calcTipId() const override;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  void writeScriptData(ScriptPaintDevice *device) const override;

  //---

  BBox annotationBBox() const;

 private:
  double remapPos(double pos) const;

 private:
  int             setId_    { 0 };       //!< set id
  int             groupInd_ { 0 };       //!< group ind
  const Whisker*  whisker_  { nullptr }; //!< whisker data
  mutable Polygon poly_;                 //!< draw polygon
};

//---

/*!
 * \brief box plot whisker outlier object
 * \ingroup Charts
 */
class CQChartsBoxPlotOutlierObj : public CQChartsBoxPlotObj {
  Q_OBJECT

 public:
  using Plot    = CQChartsBoxPlot;
  using Whisker = CQChartsBoxPlotWhisker;

 public:
  CQChartsBoxPlotOutlierObj(const Plot *plot, const BBox &rect, int setId, int groupInd,
                            const Whisker *whisker, const ColorInd &is, const ColorInd &ig, int io);

  //---

  QString typeName() const override { return "outlier"; }

  //---

  bool isPoint() const override { return true; }

  //---

  QString calcId   () const override;
  QString calcTipId() const override;

  void getObjSelectIndices(Indices &inds) const override;

  void draw(PaintDevice *device) const override;

 private:
  double remapPos(double pos) const;

 private:
  int            setId_    { 0 };       //!< set id
  int            groupInd_ { 0 };       //!< group ind
  const Whisker* whisker_  { nullptr }; //!< whisker data
  int            io_       { 0 };       //!< outlier index
};

//---

/*!
 * \brief box plot whisker object
 * \ingroup Charts
 */
class CQChartsBoxPlotDataObj : public CQChartsBoxPlotObj {
  Q_OBJECT

  Q_PROPERTY(double pos READ pos)

 public:
  using Plot        = CQChartsBoxPlot;
  using WhiskerData = CQChartsBoxWhiskerData;

 public:
  CQChartsBoxPlotDataObj(const Plot *plot, const BBox &rect, const WhiskerData &data,
                         const ColorInd &is);

  QString typeName() const override { return "data"; }

  double pos() const;

  QString calcId   () const override;
  QString calcTipId() const override;

  void getObjSelectIndices(Indices &inds) const override;

  void draw(PaintDevice *device) const override;

  BBox annotationBBox() const;

 private:
  double remapPos(double pos) const;

 private:
  WhiskerData data_; //!< whisker data
};

//---

/*!
 * \brief Box Plot Connected object
 * \ingroup Charts
 */
class CQChartsBoxPlotConnectedObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot = CQChartsBoxPlot;

 public:
  CQChartsBoxPlotConnectedObj(const Plot *plot, const BBox &rect, int groupInd,
                              const ColorInd &ig);

  QString typeName() const override { return "connected"; }

  QString calcId   () const override;
  QString calcTipId() const override;

  bool inside(const Point &p) const override;

  void draw(PaintDevice *device) const override;

 private:
  using Whisker       = CQChartsBoxPlotWhisker;
  using SetWhiskerMap = std::map<int, Whisker *>;

  const SetWhiskerMap &setWhiskerMap() const;

  void initPolygon();

 private:
  const Plot* plot_     { nullptr }; //!< parent plot
  int         groupInd_ { -1 };      //!< group ind
  Polygon     line_;                 //!< connected line
  Polygon     poly_;                 //!< connected polygon
};

//---

/*!
 * \brief Box Plot Point object
 * \ingroup Charts
 */
class CQChartsBoxPlotPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot   = CQChartsBoxPlot;
  using Symbol = CQChartsSymbol;
  using Length = CQChartsLength;

 public:
  CQChartsBoxPlotPointObj(const Plot *plot, const BBox &rect, int setId,
                          int groupInd, const Point &p, const QModelIndex &ind,
                          const ColorInd &is, const ColorInd &ig, const ColorInd &iv);

  QString typeName() const override { return "point"; }

  //---

  bool isPoint() const override { return true; }

  //---

  QString calcId   () const override;
  QString calcTipId() const override;

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &) const override;

  //---

  void draw(PaintDevice *device) const override;

 private:
  const Plot* plot_     { nullptr }; //!< parent plot
  int         setId_    { -1 };      //!< set id
  int         groupInd_ { -1 };      //!< group id
  Point       p_;                    //!< point
};

//---

#include <CQChartsKey.h>

/*!
 * \brief Box Plot Key Color Box
 * \ingroup Charts
 */
class CQChartsBoxKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  using Plot = CQChartsBoxPlot;

 public:
  CQChartsBoxKeyColor(Plot *plot, const ColorInd &is, const ColorInd &ig);

  bool selectPress(const Point &p, SelMod selMod) override;

  QBrush fillBrush() const override;

  double xColorValue(bool relative) const override;
  double yColorValue(bool relative) const override;

 private:
  using WhiskerObj = CQChartsBoxPlotWhiskerObj;

  WhiskerObj *boxObj() const;
};

/*!
 * \brief Box Plot Key Text
 * \ingroup Charts
 */
class CQChartsBoxKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  using Plot = CQChartsBoxPlot;

 public:
  CQChartsBoxKeyText(Plot *plot, const QString &text, const ColorInd &is, const ColorInd &ig);

  QColor interpTextColor(const ColorInd &ind) const override;
};

//---

CQCHARTS_NAMED_SHAPE_DATA(Box, box)
CQCHARTS_NAMED_LINE_DATA(Whisker, whisker)
CQCHARTS_NAMED_POINT_DATA(Outlier, outlier)
CQCHARTS_NAMED_POINT_DATA(Jitter, jitter)

/*!
 * \brief Box Plot
 * \ingroup Charts
 *
 * data columns:
 *   + value : value columns
 *   + name  : name column
 *   + set   : set column
 *
 * precalculated data columns:
 *   + x           : x column
 *   + min         : min column
 *   + lowerMedian : lower median column
 *   + median      : median column
 *   + upperMedian : upper median column
 *   + max         : max column
 *   + outliers    : outliers column
 *
 * Plot Type
 *   + \ref CQChartsBoxPlotType
 *
 * Example
 *   + \image html boxplot.png
 */
class CQChartsBoxPlot : public CQChartsGroupPlot,
 public CQChartsObjBoxShapeData    <CQChartsBoxPlot>,
 public CQChartsObjTextData        <CQChartsBoxPlot>,
 public CQChartsObjWhiskerLineData <CQChartsBoxPlot>,
 public CQChartsObjOutlierPointData<CQChartsBoxPlot>,
 public CQChartsObjJitterPointData <CQChartsBoxPlot> {
  Q_OBJECT

  // calc value columns
  Q_PROPERTY(CQChartsColumns valueColumns READ valueColumns WRITE setValueColumns)
  Q_PROPERTY(CQChartsColumn  nameColumn   READ nameColumn   WRITE setNameColumn  )
  Q_PROPERTY(CQChartsColumn  setColumn    READ setColumn    WRITE setSetColumn   )

  // pre-calc columns
  Q_PROPERTY(CQChartsColumn xColumn           READ xColumn           WRITE setXColumn          )
  Q_PROPERTY(CQChartsColumn minColumn         READ minColumn         WRITE setMinColumn        )
  Q_PROPERTY(CQChartsColumn lowerMedianColumn READ lowerMedianColumn WRITE setLowerMedianColumn)
  Q_PROPERTY(CQChartsColumn medianColumn      READ medianColumn      WRITE setMedianColumn     )
  Q_PROPERTY(CQChartsColumn upperMedianColumn READ upperMedianColumn WRITE setUpperMedianColumn)
  Q_PROPERTY(CQChartsColumn maxColumn         READ maxColumn         WRITE setMaxColumn        )
  Q_PROPERTY(CQChartsColumn outliersColumn    READ outliersColumn    WRITE setOutliersColumn   )

  // options
  Q_PROPERTY(bool            showOutliers READ isShowOutliers WRITE setShowOutliers)
  Q_PROPERTY(bool            connected    READ isConnected    WRITE setConnected   )
  Q_PROPERTY(Qt::Orientation orientation  READ orientation    WRITE setOrientation )
  Q_PROPERTY(bool            normalized   READ isNormalized   WRITE setNormalized  )
  Q_PROPERTY(bool            notched      READ isNotched      WRITE setNotched     )
  Q_PROPERTY(bool            colorBySet   READ isColorBySet   WRITE setColorBySet  )

  // jitter/stacked points
  Q_PROPERTY(PointsType pointsType  READ pointsType WRITE setPointsType)

  CQCHARTS_NAMED_POINT_DATA_PROPERTIES(Jitter, jitter)

  // violin (density)
  Q_PROPERTY(bool           violin      READ isViolin    WRITE setViolin     )
  Q_PROPERTY(CQChartsLength violinWidth READ violinWidth WRITE setViolinWidth)
  Q_PROPERTY(bool           violinBox   READ isViolinBox WRITE setViolinBox  )

  // error bar
  Q_PROPERTY(bool         errorBar     READ isErrorBar   WRITE setErrorBar    )
  Q_PROPERTY(ErrorBarType errorBarType READ errorBarType WRITE setErrorBarType)

  // box
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Box, box)

  Q_PROPERTY(CQChartsLength boxWidth READ boxWidth WRITE setBoxWidth)

  // text
  CQCHARTS_TEXT_DATA_PROPERTIES

  // whisker line
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Whisker, whisker)

  Q_PROPERTY(double whiskerRange  READ whiskerRange  WRITE setWhiskerRange )
  Q_PROPERTY(double whiskerExtent READ whiskerExtent WRITE setWhiskerExtent)

  // labels
  Q_PROPERTY(double textMargin READ textMargin WRITE setTextMargin)

  // margin
  Q_PROPERTY(double ymargin READ ymargin WRITE setYMargin)

  // outliers
  CQCHARTS_NAMED_POINT_DATA_PROPERTIES(Outlier, outlier)

  Q_ENUMS(PointsType)
  Q_ENUMS(ErrorBarType)

 public:
  enum class PointsType {
    NONE,
    JITTER,
    STACKED
  };

  enum class ErrorBarType {
    CROSS_BAR,
    ERROR_BAR,
    LINE_RANGE,
    POINT_RANGE
  };

  using Whisker            = CQChartsBoxPlotWhisker;
  using WhiskerData        = CQChartsBoxWhiskerData;
  using SetWhiskerMap      = std::map<int, Whisker *>;
  using GroupSetWhiskerMap = std::map<int, SetWhiskerMap>;
  using WhiskerDataList    = std::vector<WhiskerData>;
  using Symbol             = CQChartsSymbol;
  using Length             = CQChartsLength;
  using Color              = CQChartsColor;
  using ColorInd           = CQChartsUtil::ColorInd;

 public:
  CQChartsBoxPlot(View *view, const ModelP &model);
 ~CQChartsBoxPlot();

  //---

  void init() override;
  void term() override;

  //---

  const Columns &valueColumns() const { return valueColumns_; }
  void setValueColumns(const Columns &c);

  //---

  const Column &nameColumn() const { return nameColumn_; }
  void setNameColumn(const Column &c);

  const Column &setColumn() const { return setColumn_; }
  void setSetColumn(const Column &c);

  //---

  const Column &xColumn() const { return xColumn_; }
  void setXColumn(const Column &c);

  const Column &minColumn() const { return minColumn_; }
  void setMinColumn(const Column &c);

  const Column &lowerMedianColumn() const { return lowerMedianColumn_; }
  void setLowerMedianColumn(const Column &c);

  const Column &medianColumn() const { return medianColumn_; }
  void setMedianColumn(const Column &c);

  const Column &upperMedianColumn() const { return upperMedianColumn_; }
  void setUpperMedianColumn(const Column &c);

  const Column &maxColumn() const { return maxColumn_; }
  void setMaxColumn(const Column &c);

  const Column &outliersColumn() const { return outliersColumn_; }
  void setOutliersColumn(const Column &c);

  //---

  QString groupSetColumnName(const QString &def="") const;
  QString valueColumnName   (const QString &def="value") const;
  QString groupColumnName   (const QString &def="") const;

  //---

  //! get/set show outliers
  bool isShowOutliers() const { return showOutliers_; }
  void setShowOutliers(bool b);

  //! get/set show connection line
  bool isConnected() const { return connected_; }
  void setConnected(bool b);

  //! get/set box width
  const Length &boxWidth() const { return boxWidth_; }
  void setBoxWidth(const Length &l);

  //---

  //! get/set orientation
  const Qt::Orientation &orientation() const { return orientation_; }
  void setOrientation(const Qt::Orientation &o);

  //---

  bool isNormalized() const { return normalized_; }

  bool isNotched() const { return notched_; }

  //---

  bool isColorBySet() const { return colorBySet_; }
  void setColorBySet(bool b);

  //---

  // points
  PointsType pointsType() const { return pointsType_; }

  bool isPointsJitter () const { return pointsType_ == PointsType::JITTER ; }
  bool isPointsStacked() const { return pointsType_ == PointsType::STACKED; }

  //---

  // violin (density)
  bool isViolin() const { return violin_; }

  const Length &violinWidth() const { return violinWidth_; }
  void setViolinWidth(const Length &l);

  bool isViolinBox() const { return violinBox_; }
  void setViolinBox(bool b);

  //---

  // error bar
  bool isErrorBar() const { return errorBar_; }

  const ErrorBarType &errorBarType() const { return errorBarType_; }
  void setErrorBarType(const ErrorBarType &t);

  //---

  // whisker box
  double whiskerRange() const { return whiskerRange_; }
  void setWhiskerRange(double r);

  double whiskerExtent() const { return whiskerExtent_; }
  void setWhiskerExtent(double r);

  //---

  // label
  double textMargin() const { return textMargin_; }
  void setTextMargin(double r);

  //---

  // y margin
  double ymargin() const { return ymargin_; }
  void setYMargin(double r);

  //---

  bool isWhiskersGrouped() const { return isWhiskersGrouped_; }

  int numGroupWhiskers() const { return groupWhiskers_.size(); }

  const GroupSetWhiskerMap &groupWhiskers() const { return groupWhiskers_; }

  //---

  void addProperties() override;

  //---

  bool isPreCalc() const;

  //---

  Range calcRange() const override;

  void postCalcRange() override;

  Range updateRawRange () const;
  Range updateCalcRange() const;

  void updateRawWhiskers() const;

  //---

  bool hasSets  () const;
  bool hasGroups() const;

  QString setIdName(int setId) const;

  //---

  Axis *mappedXAxis() const override;
  Axis *mappedYAxis() const override;

  //---

  BBox calcAnnotationBBox() const override;

  bool createObjs(PlotObjs &objs) const override;

  void addKeyItems(PlotKey *key) override;

  //---

  bool probe(ProbeData &probeData) const override;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  bool hasXAxis() const override;
  bool hasYAxis() const override;

  //---

 public slots:
  // set horizontal
  void setHorizontal(bool b);

  // set normalized
  void setNormalized(bool b);

  // set jitter points
  void setPointsJitter(bool b);

  // set jitter stacked
  void setPointsStacked(bool b);

  // set points type
  void setPointsType(const PointsType &pointsType);

  // set notched
  void setNotched(bool b);

  // set violin
  void setViolin(bool b);

  // set error bar
  void setErrorBar(bool b);

 protected:
  using WhiskerObj   = CQChartsBoxPlotWhiskerObj;
  using OutlierObj   = CQChartsBoxPlotOutlierObj;
  using DataObj      = CQChartsBoxPlotDataObj;
  using ConnectedObj = CQChartsBoxPlotConnectedObj;
  using PointObj     = CQChartsBoxPlotPointObj;

 protected:
  bool initRawObjs (PlotObjs &objs) const;
  bool initCalcObjs(PlotObjs &objs) const;

  void addJitterPoints(int groupInd, int setId, double pos, const Whisker *whisker,
                       const ColorInd &is, const ColorInd &ig, PlotObjs &objs) const;
  void addStackedPoints(int groupInd, int setId, double pos, const Whisker *whisker,
                        const ColorInd &is, const ColorInd &ig, PlotObjs &objs) const;

  void clearRawWhiskers();

  void addCalcRow(const ModelVisitor::VisitData &vdata, WhiskerDataList &dataList,
                  Range &dataRange, RMinMax &xrange) const;

  void addRawWhiskerRow(const ModelVisitor::VisitData &vdata) const;

  //---

  virtual WhiskerObj *createWhiskerObj(const BBox &rect, int setId, int groupInd,
                                       const Whisker *whisker, const ColorInd &is,
                                       const ColorInd &ig) const;

  virtual OutlierObj *createOutlierObj(const BBox &rect, int setId, int groupInd,
                                       const Whisker *whisker, const ColorInd &is,
                                       const ColorInd &ig, int io) const;

  virtual DataObj *createDataObj(const BBox &rect, const WhiskerData &data,
                                 const ColorInd &is) const;

  virtual ConnectedObj *createConnectedObj(const BBox &rect, int groupInd,
                                           const ColorInd &ig) const;

  virtual PointObj *createPointObj(const BBox &rect, int setId, int groupInd, const Point &p,
                                   const QModelIndex &ind, const ColorInd &is,
                                   const ColorInd &ig, const ColorInd &iv) const;

 private:
  Columns valueColumns_;      //!< value columns
  Column  nameColumn_;        //!< name column
  Column  setColumn_;         //!< set column

  Column  xColumn_;           //!< x column
  Column  minColumn_;         //!< min column
  Column  lowerMedianColumn_; //!< lower median column
  Column  medianColumn_;      //!< median column
  Column  upperMedianColumn_; //!< upper median column
  Column  maxColumn_;         //!< max column
  Column  outliersColumn_;    //!< outliers column

  bool               showOutliers_      { true };                    //!< show outliers
  bool               connected_         { false };                   //!< connect boxes
  Qt::Orientation    orientation_       { Qt::Vertical };            //!< bar draw direction
  bool               normalized_        { false };                   //!< normalized values
  PointsType         pointsType_        { PointsType::NONE };        //!< show points type
  bool               notched_           { false };                   //!< show notch
  bool               violin_            { false };                   //!< show violin
  Length             violinWidth_       { 0.6, Units::PLOT };        //!< violin width
  bool               violinBox_         { false };                   //!< show box width violin
  bool               errorBar_          { false };                   //!< show error bar
  ErrorBarType       errorBarType_      { ErrorBarType::CROSS_BAR }; //!< error bar type
  bool               colorBySet_        { false };                   //!< color by set
  Length             boxWidth_          { 0.2, Units::PLOT };        //!< box width
  double             whiskerRange_      { 1.5 };                     //!< whisker range
  double             whiskerExtent_     { 0.2 };                     //!< whisker extent
  double             textMargin_        { 2 };                       //!< text margin
  double             ymargin_           { 0.05 };                    //!< y margin
  ColumnType         setType_           { ColumnType::NONE };        //!< set column data type
  CQChartsValueInd   setValueInd_;                                   //!< set value inds
  ColumnType         xType_             { ColumnType::NONE };        //!< x column data type
  CQChartsValueInd   xValueInd_;                                     //!< x value inds
  GroupSetWhiskerMap groupWhiskers_;                                 //!< grouped whisker data
  WhiskerDataList    whiskerDataList_;                               //!< whisker data
  bool               isWhiskersGrouped_ { false };                   //!< is grouped whiskers
  bool               forceNoYAxis_      { false };                   //!< force no y axis
};

#endif
