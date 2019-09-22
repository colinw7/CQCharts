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

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
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
  CQChartsBoxPlotObj(const CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                     const ColorInd &is=ColorInd(), const ColorInd &ig=ColorInd(),
                     const ColorInd &iv=ColorInd());

  void drawHText(CQChartsPaintDevice *device, double xl, double xr, double y,
                 const QString &text, bool onLeft);
  void drawVText(CQChartsPaintDevice *device, double yt, double yb, double x,
                 const QString &text, bool onBottom);

  void addHBBox(CQChartsGeom::BBox &pbbox, double xl, double xr, double y,
                const QString &text, bool onLeft) const;
  void addVBBox(CQChartsGeom::BBox &pbbox, double yb, double yt, double x,
                const QString &text, bool onBottom) const;

 protected:
  const CQChartsBoxPlot* plot_ { nullptr }; //!< parent plot
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
  CQChartsBoxPlotWhiskerObj(const CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                            int setId, int groupInd, const CQChartsBoxPlotWhisker *whisker,
                            const ColorInd &is, const ColorInd &ig);

  QString typeName() const override { return "whisker"; }

  double pos        () const;
  double min        () const;
  double lowerMedian() const;
  double median     () const;
  double upperMedian() const;
  double max        () const;
  double mean       () const;
  double stddev     () const;
  double notch      () const;

  QString calcId   () const override;
  QString calcTipId() const override;

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

  CQChartsGeom::BBox annotationBBox() const;

 private:
  double remapPos(double pos) const;

 private:
  int                           setId_    { 0 };       //!< set id
  int                           groupInd_ { 0 };       //!< group ind
  const CQChartsBoxPlotWhisker* whisker_  { nullptr }; //!< whisker data
  QPolygonF                     poly_;                 //!< draw polygon
};

//---

/*!
 * \brief box plot whisker outlier object
 * \ingroup Charts
 */
class CQChartsBoxPlotOutlierObj : public CQChartsBoxPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxPlotOutlierObj(const CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect, int setId,
                            int groupInd, const CQChartsBoxPlotWhisker *whisker,
                            const ColorInd &is, const ColorInd &ig, int io);

  QString typeName() const override { return "outlier"; }

  QString calcId   () const override;
  QString calcTipId() const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 private:
  double remapPos(double pos) const;

 private:
  int                           setId_    { 0 };       //!< set id
  int                           groupInd_ { 0 };       //!< group ind
  const CQChartsBoxPlotWhisker* whisker_  { nullptr }; //!< whisker data
  int                           io_       { 0 };       //!< outlier index
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
  CQChartsBoxPlotDataObj(const CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                         const CQChartsBoxWhiskerData &data, const ColorInd &is);

  QString typeName() const override { return "data"; }

  double pos() const;

  QString calcId   () const override;
  QString calcTipId() const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

  CQChartsGeom::BBox annotationBBox() const;

 private:
  double remapPos(double pos) const;

 private:
  CQChartsBoxWhiskerData data_; //!< whisker data
};

//---

/*!
 * \brief Box Plot Connected object
 * \ingroup Charts
 */
class CQChartsBoxPlotConnectedObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxPlotConnectedObj(const CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                              int groupInd, const ColorInd &ig);

  QString typeName() const override { return "connected"; }

  QString calcId   () const override;
  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &) const override { }

  void draw(CQChartsPaintDevice *device) override;

 private:
  using SetWhiskerMap = std::map<int,CQChartsBoxPlotWhisker *>;

  const SetWhiskerMap &setWhiskerMap() const;

  void initPolygon();

 private:
  const CQChartsBoxPlot* plot_     { nullptr }; //!< parent plot
  int                    groupInd_ { -1 };      //!< group ind
  QPolygonF              line_;                 //!< connected line
  QPolygonF              poly_;                 //!< connected polygon
};

//---

/*!
 * \brief Box Plot Point object
 * \ingroup Charts
 */
class CQChartsBoxPlotPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxPlotPointObj(const CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                          int setId, int groupInd, const QPointF &p, const QModelIndex &ind,
                          const ColorInd &is, const ColorInd &ig, const ColorInd &iv);

  QString typeName() const override { return "point"; }

  QString calcId   () const override;
  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &) const override;

  void draw(CQChartsPaintDevice *device) override;

 private:
  const CQChartsBoxPlot* plot_     { nullptr }; //!< parent plot
  int                    setId_    { -1 };      //!< set id
  int                    groupInd_ { -1 };      //!< group id
  QPointF                p_;                    //!< point
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
  CQChartsBoxKeyColor(CQChartsBoxPlot *plot, const ColorInd &is, const ColorInd &ig);

  bool selectPress(const CQChartsGeom::Point &p, CQChartsSelMod selMod) override;

  QBrush fillBrush() const override;

  double xColorValue(bool relative) const override;
  double yColorValue(bool relative) const override;

 private:
  CQChartsBoxPlotWhiskerObj *boxObj() const;
};

/*!
 * \brief Box Plot Key Text
 * \ingroup Charts
 */
class CQChartsBoxKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsBoxKeyText(CQChartsBoxPlot *plot, const QString &text,
                     const ColorInd &is, const ColorInd &ig);

  QColor interpTextColor(const ColorInd &ind) const override;
};

//---

CQCHARTS_NAMED_SHAPE_DATA(Box,box)
CQCHARTS_NAMED_LINE_DATA(Whisker,whisker)
CQCHARTS_NAMED_POINT_DATA(Outlier,outlier)
CQCHARTS_NAMED_POINT_DATA(Jitter,jitter)

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
  Q_PROPERTY(bool showOutliers READ isShowOutliers WRITE setShowOutliers)
  Q_PROPERTY(bool connected    READ isConnected    WRITE setConnected   )
  Q_PROPERTY(bool horizontal   READ isHorizontal   WRITE setHorizontal  )
  Q_PROPERTY(bool normalized   READ isNormalized   WRITE setNormalized  )
  Q_PROPERTY(bool notched      READ isNotched      WRITE setNotched     )
  Q_PROPERTY(bool colorBySet   READ isColorBySet   WRITE setColorBySet  )

  // jitter/stacked points
  Q_PROPERTY(PointsType pointsType  READ pointsType WRITE setPointsType)

  CQCHARTS_NAMED_POINT_DATA_PROPERTIES(Jitter,jitter)

  // violin (density)
  Q_PROPERTY(bool           violin      READ isViolin    WRITE setViolin     )
  Q_PROPERTY(CQChartsLength violinWidth READ violinWidth WRITE setViolinWidth)
  Q_PROPERTY(bool           violinBox   READ isViolinBox WRITE setViolinBox  )

  // error bar
  Q_PROPERTY(bool         errorBar     READ isErrorBar   WRITE setErrorBar    )
  Q_PROPERTY(ErrorBarType errorBarType READ errorBarType WRITE setErrorBarType)

  // box
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Box,box)

  Q_PROPERTY(CQChartsLength boxWidth READ boxWidth WRITE setBoxWidth)

  // text
  CQCHARTS_TEXT_DATA_PROPERTIES

  // whisker line
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Whisker,whisker)

  Q_PROPERTY(double whiskerRange  READ whiskerRange  WRITE setWhiskerRange )
  Q_PROPERTY(double whiskerExtent READ whiskerExtent WRITE setWhiskerExtent)

  // labels
  Q_PROPERTY(double textMargin READ textMargin WRITE setTextMargin)

  // margin
  Q_PROPERTY(double ymargin READ ymargin WRITE setYMargin)

  // outliers
  CQCHARTS_NAMED_POINT_DATA_PROPERTIES(Outlier,outlier)

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

  using SetWhiskerMap      = std::map<int,CQChartsBoxPlotWhisker *>;
  using GroupSetWhiskerMap = std::map<int,SetWhiskerMap>;
  using WhiskerDataList    = std::vector<CQChartsBoxWhiskerData>;

 public:
  CQChartsBoxPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsBoxPlot();

  //---

  const CQChartsColumns &valueColumns() const { return valueColumns_; }
  void setValueColumns(const CQChartsColumns &c);

  //---

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumn &setColumn() const { return setColumn_; }
  void setSetColumn(const CQChartsColumn &c);

  //---

  const CQChartsColumn &xColumn() const { return xColumn_; }
  void setXColumn(const CQChartsColumn &c);

  const CQChartsColumn &minColumn() const { return minColumn_; }
  void setMinColumn(const CQChartsColumn &c);

  const CQChartsColumn &lowerMedianColumn() const { return lowerMedianColumn_; }
  void setLowerMedianColumn(const CQChartsColumn &c);

  const CQChartsColumn &medianColumn() const { return medianColumn_; }
  void setMedianColumn(const CQChartsColumn &c);

  const CQChartsColumn &upperMedianColumn() const { return upperMedianColumn_; }
  void setUpperMedianColumn(const CQChartsColumn &c);

  const CQChartsColumn &maxColumn() const { return maxColumn_; }
  void setMaxColumn(const CQChartsColumn &c);

  const CQChartsColumn &outliersColumn() const { return outliersColumn_; }
  void setOutliersColumn(const CQChartsColumn &c);

  //---

  QString groupSetColumnName(const QString &def="") const;
  QString valueColumnName   (const QString &def="value") const;
  QString groupColumnName   (const QString &def="") const;

  //---

  bool isShowOutliers() const { return showOutliers_; }
  void setShowOutliers(bool b);

  bool isConnected() const { return connected_; }
  void setConnected(bool b);

  const CQChartsLength &boxWidth() const { return boxWidth_; }
  void setBoxWidth(const CQChartsLength &l);

  //---

  bool isHorizontal() const { return horizontal_; }

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

  const CQChartsLength &violinWidth() const { return violinWidth_; }
  void setViolinWidth(const CQChartsLength &l);

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

  CQChartsGeom::Range calcRange() const override;

  CQChartsGeom::Range updateRawRange () const;
  CQChartsGeom::Range updateCalcRange() const;

  void updateRawWhiskers() const;

  //---

  bool hasSets  () const;
  bool hasGroups() const;

  QString setIdName(int setId) const;

  //---

  CQChartsAxis *mappedXAxis() const override;
  CQChartsAxis *mappedYAxis() const override;

  //---

  CQChartsGeom::BBox annotationBBox() const override;

  bool createObjs(PlotObjs &objs) const override;

  void addKeyItems(CQChartsPlotKey *key) override;

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

 private:
  bool initRawObjs (PlotObjs &objs) const;
  bool initCalcObjs(PlotObjs &objs) const;

  void addJitterPoints(int groupInd, int setId, double pos, const CQChartsBoxPlotWhisker *whisker,
                       const ColorInd &is, const ColorInd &ig, PlotObjs &objs) const;
  void addStackedPoints(int groupInd, int setId, double pos, const CQChartsBoxPlotWhisker *whisker,
                        const ColorInd &is, const ColorInd &ig, PlotObjs &objs) const;

  void clearRawWhiskers();

  void addCalcRow(const ModelVisitor::VisitData &vdata, WhiskerDataList &dataList,
                  CQChartsGeom::Range &dataRange, CQChartsGeom::RMinMax &xrange) const;

  void addRawWhiskerRow(const ModelVisitor::VisitData &vdata) const;

 private:
  using RMinMax = CQChartsGeom::RMinMax;

  CQChartsColumns    valueColumns_;                                  //!< value columns
  CQChartsColumn     nameColumn_;                                    //!< name column
  CQChartsColumn     setColumn_;                                     //!< set column

  CQChartsColumn     xColumn_;                                       //!< x column
  CQChartsColumn     minColumn_;                                     //!< min column
  CQChartsColumn     lowerMedianColumn_;                             //!< lower median column
  CQChartsColumn     medianColumn_;                                  //!< median column
  CQChartsColumn     upperMedianColumn_;                             //!< upper median column
  CQChartsColumn     maxColumn_;                                     //!< max column
  CQChartsColumn     outliersColumn_;                                //!< outliers column

  bool               showOutliers_      { true };                    //!< show outliers
  bool               connected_         { false };                   //!< connect boxes
  bool               horizontal_        { false };                   //!< horizontal bars
  bool               normalized_        { false };                   //!< normalized values
  PointsType         pointsType_        { PointsType::NONE };        //!< show points type
  bool               notched_           { false };                   //!< show notch
  bool               violin_            { false };                   //!< show violin
  CQChartsLength     violinWidth_       { 0.6 };                     //!< violin width
  bool               violinBox_         { false };                   //!< show box width violin
  bool               errorBar_          { false };                   //!< show error bar
  ErrorBarType       errorBarType_      { ErrorBarType::CROSS_BAR }; //!< error bar type
  bool               colorBySet_        { false };                   //!< color by set
  CQChartsLength     boxWidth_          { 0.2 };                     //!< box width
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
