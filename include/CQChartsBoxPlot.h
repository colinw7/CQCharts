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

// box plot type
class CQChartsBoxPlotType : public CQChartsGroupPlotType {
 public:
  CQChartsBoxPlotType();

  QString name() const override { return "boxplot"; }
  QString desc() const override { return "BoxPlot"; }

  const char *yColumnName() const override { return "value"; }

  bool allowXAxisIntegral() const override { return false; }

  bool allowXLog() const override { return false; }

  Dimension dimension() const override { return Dimension::ONE_D; }

  void addParameters() override;

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsBoxPlot;

// box value
struct CQChartsBoxPlotValue {
  CQChartsBoxPlotValue() = default;

  CQChartsBoxPlotValue(double value, const QModelIndex &ind) :
   value(value), ind(ind) {
  }

  operator double() const { return value; }

  double      value { 0.0 };
  QModelIndex ind;
};

//---

using CQChartsBoxPlotWhisker = CQChartsBoxWhiskerT<CQChartsBoxPlotValue>;

struct CQChartsBoxWhiskerData {
  using Outliers = std::vector<double>;

  QModelIndex ind;
  QString     name;
  double      x       { 0.0 };
  double      min     { 0.0 };
  double      lower   { 0.0 };
  double      median  { 0.5 };
  double      upper   { 0.0 };
  double      max     { 1.0 };
  double      mean    { 0.0 };
  double      stddev  { 0.0 };
  double      notch   { 0.0 };
  Outliers    outliers;
  double      dataMin { 0.0 };
  double      dataMax { 1.0 };
};

//---

// box plot base object
class CQChartsBoxPlotObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxPlotObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect);

  void drawHText(QPainter *painter, double xl, double xr, double y,
                 const QString &text, bool onLeft);
  void drawVText(QPainter *painter, double yt, double yb, double x,
                 const QString &text, bool onBottom);

  void addHBBox(CQChartsGeom::BBox &pbbox, double xl, double xr, double y,
                const QString &text, bool onLeft) const;
  void addVBBox(CQChartsGeom::BBox &pbbox, double yb, double yt, double x,
                const QString &text, bool onBottom) const;

 protected:
  CQChartsBoxPlot* plot_ { nullptr }; // parent plot
};

//---

// box plot whisker object
class CQChartsBoxPlotWhiskerObj : public CQChartsBoxPlotObj {
  Q_OBJECT

  Q_PROPERTY(double pos    READ pos   )
  Q_PROPERTY(double min    READ min   )
  Q_PROPERTY(double lower  READ lower )
  Q_PROPERTY(double median READ median)
  Q_PROPERTY(double upper  READ upper )
  Q_PROPERTY(double max    READ max   )
  Q_PROPERTY(double mean   READ mean  )
  Q_PROPERTY(double stddev READ stddev)
  Q_PROPERTY(double notch  READ notch )

 public:
  CQChartsBoxPlotWhiskerObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect, int setId,
                            int groupInd, const CQChartsBoxPlotWhisker &whisker,
                            int ig, int ng, int is, int ns);

  QString typeName() const override { return "whisker"; }

  double pos   () const;
  double min   () const;
  double lower () const;
  double median() const;
  double upper () const;
  double max   () const;
  double mean  () const;
  double stddev() const;
  double notch () const;

  QString calcId   () const override;
  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

  CQChartsGeom::BBox annotationBBox() const;

 private:
  double remapPos(double pos) const;

 private:
  int                    setId_    { 0 };  // set id
  int                    groupInd_ { 0 };  // set id
  CQChartsBoxPlotWhisker whisker_;         // whisker data
  QPolygonF              ppoly_;           // draw polygon
  int                    ig_       { -1 }; // group index
  int                    ng_       { 0 };  // group count
  int                    is_       { -1 }; // value set index
  int                    ns_       { 0 };  // value set count
};

//---

// box plot whisker object
class CQChartsBoxPlotDataObj : public CQChartsBoxPlotObj {
  Q_OBJECT

  Q_PROPERTY(double pos READ pos)

 public:
  CQChartsBoxPlotDataObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                         const CQChartsBoxWhiskerData &data);

  QString typeName() const override { return "data"; }

  double pos() const;

  QString calcId   () const override;
  QString calcTipId() const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

  CQChartsGeom::BBox annotationBBox() const;

 private:
  double remapPos(double pos) const;

 private:
  CQChartsBoxWhiskerData data_;             // whisker data
  double                 ymargin_ { 0.05 };
};

//---

// box plot connected objects
class CQChartsBoxPlotConnectedObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxPlotConnectedObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                              int groupInd, int i, int n);

  QString typeName() const override { return "connected"; }

  QString calcId   () const override;
  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &) const override { }

  void addColumnSelectIndex(Indices &, const CQChartsColumn &) const override { }

  void draw(QPainter *painter) override;

 private:
  using SetWhiskerMap = std::map<int,CQChartsBoxPlotWhisker>;

  const SetWhiskerMap &setWhiskerMap() const;

  void initPolygon();

 private:
  CQChartsBoxPlot* plot_     { nullptr }; // parent plot
  int              groupInd_ { -1 };      // group ind
  int              i_        { -1 };      // group index
  int              n_        { 0 };       // group count
  QPolygonF        line_;
  QPolygonF        poly_;
};

//---

// box plot connected objects
class CQChartsBoxPlotPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxPlotPointObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                         int setId, int groupInd, const QPointF &p, const QModelIndex &ind,
                         int ig, int ng, int is, int ns, int iv, int nv);

  QString typeName() const override { return "point"; }

  QString calcId   () const override;
  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &) const override;

  void addColumnSelectIndex(Indices &, const CQChartsColumn &) const override;

  void draw(QPainter *painter) override;

 private:
  CQChartsBoxPlot* plot_     { nullptr }; // parent plot
  int              setId_    { -1 };      // set id
  int              groupInd_ { -1 };      // group id
  QPointF          p_;                    // point
  QModelIndex      ind_;                  // model index
  int              ig_       { -1 };      // group index
  int              ng_       { 0 };       // group count
  int              is_       { -1 };      // set index
  int              ns_       { 0 };       // set count
  int              iv_       { -1 };      // value index
  int              nv_       { 0 };       // value count
};

//---

#include <CQChartsKey.h>

class CQChartsBoxKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsBoxKeyColor(CQChartsBoxPlot *plot, int i, int n);

  bool selectPress(const CQChartsGeom::Point &p) override;

  QBrush fillBrush() const override;
};

class CQChartsBoxKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsBoxKeyText(CQChartsBoxPlot *plot, int i, const QString &text);

  QColor interpTextColor(int i, int n) const override;

 private:
  int i_ { 0 };
};

//---

CQCHARTS_NAMED_SHAPE_DATA(Box,box)
CQCHARTS_NAMED_LINE_DATA(Whisker,whisker)
CQCHARTS_NAMED_POINT_DATA(Outlier,outlier)
CQCHARTS_NAMED_POINT_DATA(Jitter,jitter)

// box plot
class CQChartsBoxPlot : public CQChartsGroupPlot,
 public CQChartsPlotBoxShapeData    <CQChartsBoxPlot>,
 public CQChartsPlotTextData        <CQChartsBoxPlot>,
 public CQChartsPlotWhiskerLineData <CQChartsBoxPlot>,
 public CQChartsPlotOutlierPointData<CQChartsBoxPlot>,
 public CQChartsPlotJitterPointData <CQChartsBoxPlot> {
  Q_OBJECT

  // calc value columns
  Q_PROPERTY(CQChartsColumn valueColumn  READ valueColumn     WRITE setValueColumn    )
  Q_PROPERTY(QString        valueColumns READ valueColumnsStr WRITE setValueColumnsStr)
  Q_PROPERTY(CQChartsColumn nameColumn   READ nameColumn      WRITE setNameColumn     )
  Q_PROPERTY(CQChartsColumn setColumn    READ setColumn       WRITE setSetColumn      )

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

  // jitter
  Q_PROPERTY(bool pointsJitter  READ isPointsJitter  WRITE setPointsJitter )
  Q_PROPERTY(bool pointsStacked READ isPointsStacked WRITE setPointsStacked)

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

  Q_PROPERTY(CQChartsLength boxWidth    READ boxWidth   WRITE setBoxWidth  )
  Q_PROPERTY(CQChartsLength cornerSize  READ cornerSize WRITE setCornerSize)

  // text
  CQCHARTS_TEXT_DATA_PROPERTIES

  // whisker line
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Whisker,whisker)

  Q_PROPERTY(double whiskerRange  READ whiskerRange  WRITE setWhiskerRange )
  Q_PROPERTY(double whiskerExtent READ whiskerExtent WRITE setWhiskerExtent)

  // labels
  Q_PROPERTY(double textMargin READ textMargin WRITE setTextMargin)

  // outliers
  CQCHARTS_NAMED_POINT_DATA_PROPERTIES(Outlier,outlier)

  Q_ENUMS(ErrorBarType)

 public:
  enum class ErrorBarType {
    CROSS_BAR,
    ERROR_BAR,
    LINE_RANGE,
    POINT_RANGE
  };

  using SetWhiskerMap      = std::map<int,CQChartsBoxPlotWhisker>;
  using GroupSetWhiskerMap = std::map<int,SetWhiskerMap>;
  using WhiskerDataList    = std::vector<CQChartsBoxWhiskerData>;

 public:
  CQChartsBoxPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsBoxPlot();

  //---

  const CQChartsColumn &valueColumn() const { return valueColumns_.column(); }
  void setValueColumn(const CQChartsColumn &c);

  const Columns &valueColumns() const { return valueColumns_.columns(); }
  void setValueColumns(const Columns &valueColumns);

  QString valueColumnsStr() const;
  bool setValueColumnsStr(const QString &s);

  const CQChartsColumn &valueColumnAt(int i) const;

  int numValueColumns() const;

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
  bool isPointsJitter() const { return pointsJitter_; }

  bool isPointsStacked() const { return pointsStacked_; }

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
  const CQChartsLength &cornerSize() const;
  void setCornerSize(const CQChartsLength &r);

  //---

  double whiskerRange() const { return whiskerRange_; }
  void setWhiskerRange(double r);

  double whiskerExtent() const { return whiskerExtent_; }
  void setWhiskerExtent(double r);

  //---

  // label
  double textMargin() const { return textMargin_; }
  void setTextMargin(double r);

  //---

  bool isGrouped() const { return grouped_; }

  int numGroups() const { return groupWhiskers_.size(); }

  const GroupSetWhiskerMap &groupWhiskers() const { return groupWhiskers_; }

  //---

  void addProperties() override;

  //---

  bool isPreCalc() const;

  //---

  void calcRange() override;

  void updateRawRange ();
  void updateCalcRange();

  void updateRawWhiskers();

  //---

  bool hasSets  () const;
  bool hasGroups() const;

  QString setIdName(int setId) const;

  //---

  CQChartsGeom::BBox annotationBBox() const override;

  bool initObjs() override;

  bool initRawObjs ();
  bool initCalcObjs();

  void addKeyItems(CQChartsPlotKey *key) override;

  //---

  bool probe(ProbeData &probeData) const override;

  //---

  bool addMenuItems(QMenu *menu) override;

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

  // set notched
  void setNotched(bool b);

  // set violin
  void setViolin(bool b);

  // set error bar
  void setErrorBar(bool b);

 private:
  void addCalcRow(const ModelVisitor::VisitData &vdata, WhiskerDataList &dataList);

  void addRawWhiskerRow(const ModelVisitor::VisitData &vdata);

 private:
  CQChartsColumns       valueColumns_   { 1 };                       // value columns
  CQChartsColumn        nameColumn_;                                 // name column
  CQChartsColumn        setColumn_;                                  // set column

  CQChartsColumn        xColumn_;                                    // x column
  CQChartsColumn        minColumn_;                                  // min column
  CQChartsColumn        lowerMedianColumn_;                          // lower median column
  CQChartsColumn        medianColumn_;                               // median column
  CQChartsColumn        upperMedianColumn_;                          // upper median column
  CQChartsColumn        maxColumn_;                                  // max column
  CQChartsColumn        outliersColumn_;                             // outliers column

  bool                  showOutliers_   { true };                    // show outliers
  bool                  connected_      { false };                   // connect boxes
  bool                  horizontal_     { false };                   // horizontal bars
  bool                  normalized_     { false };                   // normalized values
  bool                  pointsJitter_   { false };                   // show jitter points
  bool                  pointsStacked_  { false };                   // show stacked points
  bool                  notched_        { false };                   // show notch
  bool                  violin_         { false };                   // show violin
  CQChartsLength        violinWidth_    { 0.6 };                     // violin width
  bool                  violinBox_      { false };                   // show box width violin
  bool                  errorBar_       { false };                   // show error bar
  ErrorBarType          errorBarType_   { ErrorBarType::CROSS_BAR }; // error bar type
  bool                  colorBySet_     { false };                   // color by set
  CQChartsLength        boxWidth_       { 0.2 };                     // box width
  double                whiskerRange_   { 1.5 };                     // whisker range
  double                whiskerExtent_  { 0.2 };                     // whisker extent
  CQChartsGeom::RMinMax xrange_;                                     // x range
  CQChartsBoxData       boxData_;                                    // shape fill/border style
  double                textMargin_     { 2 };                       // text margin
  ColumnType            setType_        { ColumnType::NONE };        // set column data type
  GroupSetWhiskerMap    groupWhiskers_;                              // grouped whisker data
  WhiskerDataList       whiskerDataList_;                            // whisker data
  CQChartsValueInd      setValueInd_;                                // set value inds
  bool                  grouped_        { false };                   // is grouped values
};

#endif
