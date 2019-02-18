#ifndef CQChartsDistributionPlot_H
#define CQChartsDistributionPlot_H

#include <CQChartsBarPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsColor.h>

class CQChartsDataLabel;
class CQChartsDensity;

//---

struct CQChartsDistributionBarValue {
  double n1 { 0 };
  double n2 { 0 };
};

//---

// distribution plot type
class CQChartsDistributionPlotType : public CQChartsGroupPlotType {
 public:
  CQChartsDistributionPlotType();

  QString name() const override { return "distribution"; }
  QString desc() const override { return "Distribution"; }

  QString yColumnName() const override { return "value"; }

  bool allowXAxisIntegral() const override { return false; }

  bool allowXLog() const override { return false; }

  Dimension dimension() const override { return Dimension::ONE_D; }

  void addParameters() override;

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsDistributionPlot;

// bar object
class CQChartsDistributionBarObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString groupName READ groupName)
  Q_PROPERTY(QString bucketStr READ bucketStr)
  Q_PROPERTY(int     count     READ count    )
  Q_PROPERTY(double  minValue  READ minValue )
  Q_PROPERTY(double  maxValue  READ maxValue )

 public:
  using ColorCount = std::map<int,int>;
  using ColorSet   = std::map<CQChartsColor,int>;
  using ColorRows  = std::set<int>;

  struct ColorData {
    ColorCount colorCount;
    ColorSet   colorSet;
    ColorRows  colorRows;
    int        nv { 0 };
  };

 public:
  using BarValue = CQChartsDistributionBarValue;

  CQChartsDistributionBarObj(const CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                             int groupInd, int bucket, const BarValue &barValue,
                             int is, int ns, int iv, int nv);

  int groupInd() const { return groupInd_; }

  int bucket() const { return bucket_; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  QString groupName() const;

  QString bucketStr() const;

  int count() const;

  double minValue() const;
  double maxValue() const;

  //---

  CQChartsGeom::BBox dataLabelRect() const;

  //---

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  //---

  void draw(QPainter *painter) override;

  void drawFg(QPainter *painter) const override;

  void drawRug(QPainter *painter) const;

  //---

  CQChartsGeom::BBox calcRect() const;

  //---

  double mapValue(double v) const;

 private:
  bool getBarColoredRects(ColorData &colorData) const;

  void drawRect(QPainter *painter, const QRectF &qrect, const CQChartsColor &color,
                bool useLine) const;

  QColor barColor() const;

 private:
  const CQChartsDistributionPlot* plot_     { nullptr };
  int                             groupInd_ { -1 };
  int                             bucket_   { -1 };
  BarValue                        barValue_;
  int                             is_       { -1 };
  int                             ns_       { -1 };
  int                             iv_       { -1 };
  int                             nv_       { -1 };
  double                          value1_   { 0.0 };
  double                          value2_   { 1.0 };
  ColorData                       colorData_;
};

//---

// density object
class CQChartsDistributionDensityObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString groupName  READ groupName )
  Q_PROPERTY(int     numSamples READ numSamples)

 public:
  using Points = std::vector<QPointF>;

  struct Bucket {
    int bucket;
    int n;

    Bucket(int bucket, int n) :
     bucket(bucket), n(n) {
    }
  };

  using Buckets = std::vector<Bucket>;

  struct Data {
    Points  points;
    double  xmin { 0.0 };
    double  xmax { 0.0 };
    double  ymin { 0.0 };
    double  ymax { 0.0 };
    double  mean { 0.0 };
    double  sum  { 0.0 };
    Buckets buckets;
  };

 public:
  CQChartsDistributionDensityObj(const CQChartsDistributionPlot *plot,
                                 const CQChartsGeom::BBox &rect, int groupInd,
                                 const Data &data, double doffset, int is, int ns);

  int groupInd() const { return groupInd_; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  QString groupName() const;

  int numSamples() const;

  //---

  bool inside(const CQChartsGeom::Point &p) const override;

  //---

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  //---

  void draw(QPainter *painter) override;

  void drawFg(QPainter *painter) const override;

  void drawMeanLine(QPainter *painter) const;

  void drawRug(QPainter *painter) const;

  //---

  CQChartsGeom::BBox calcRect() const;

 private:
  const CQChartsDistributionPlot* plot_        { nullptr };
  int                             groupInd_    { -1 };
  Data                            data_;
  double                          doffset_     { 0.0 };
  int                             is_          { -1 };
  int                             ns_          { -1 };
  QPolygonF                       poly_;
  double                          bucketScale_ { 1.0 };
};

//---

// scatter box object
class CQChartsDistributionScatterObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsDistributionScatterObj(const CQChartsDistributionPlot *plot,
                                 const CQChartsGeom::BBox &rect, int groupInd, int bucket,
                                 int n, int is, int ns, int iv, int nv);

  int groupInd() const { return groupInd_; }

  int bucket() const { return bucket_; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void draw(QPainter *painter) override;

  //---

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

 private:
  using Points = std::vector<QPointF>;

  const CQChartsDistributionPlot* plot_     { nullptr };
  int                             groupInd_ { -1 };
  int                             bucket_   { -1 };
  int                             n_        { 0 };
  int                             is_       { -1 };
  int                             ns_       { -1 };
  int                             iv_       { -1 };
  int                             nv_       { -1 };
  Points                          points_;
};

//---

#include <CQChartsKey.h>

// key color box
class CQChartsDistKeyColorBox : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsDistKeyColorBox(CQChartsDistributionPlot *plot, int i, int n);

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &v) { color_ = v; }

  bool selectPress(const CQChartsGeom::Point &p, CQChartsSelMod selMod) override;

  QBrush fillBrush() const override;

  bool isSetHidden() const;

  void setSetHidden(bool b);

 private:
  CQChartsDistributionPlot* plot_;  //! plot
  CQChartsColor             color_; //! custom color
};

// key text
class CQChartsDistKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsDistKeyText(CQChartsDistributionPlot *plot, const QString &text, int i, int n);

  QColor interpTextColor(int i, int n) const override;

  bool isSetHidden() const;
};

//---

CQCHARTS_NAMED_LINE_DATA(Mean,mean)

// distribution plot
class CQChartsDistributionPlot : public CQChartsBarPlot,
 public CQChartsObjMeanLineData<CQChartsDistributionPlot>,
 public CQChartsObjDotPointData<CQChartsDistributionPlot>,
 public CQChartsObjRugPointData<CQChartsDistributionPlot> {
  Q_OBJECT

  // style
  Q_PROPERTY(CQChartsColumn nameColumn READ nameColumn WRITE setNameColumn)
  Q_PROPERTY(CQChartsColumn dataColumn READ dataColumn WRITE setDataColumn)

  // bucketer
  Q_PROPERTY(bool   bucketed         READ isBucketed        WRITE setBucketed        )
  Q_PROPERTY(bool   autoBucket       READ isAutoBucket      WRITE setAutoBucket      )
  Q_PROPERTY(double startBucketValue READ startBucketValue  WRITE setStartBucketValue)
  Q_PROPERTY(double deltaBucketValue READ deltaBucketValue  WRITE setDeltaBucketValue)
  Q_PROPERTY(int    numAutoBuckets   READ numAutoBuckets    WRITE setNumAutoBuckets  )

  // options
  Q_PROPERTY(PlotType  plotType  READ plotType  WRITE setPlotType )
  Q_PROPERTY(ValueType valueType READ valueType WRITE setValueType)

  Q_PROPERTY(bool percent   READ isPercent   WRITE setPercent  )
  Q_PROPERTY(bool skipEmpty READ isSkipEmpty WRITE setSkipEmpty)
  Q_PROPERTY(bool sorted    READ isSorted    WRITE setSorted   )

  // density
  Q_PROPERTY(bool   density         READ isDensity         WRITE setDensity        )
  Q_PROPERTY(double densityOffset   READ densityOffset     WRITE setDensityOffset  )
  Q_PROPERTY(int    densitySamples  READ densitySamples    WRITE setDensitySamples )
  Q_PROPERTY(bool   densityGradient READ isDensityGradient WRITE setDensityGradient)
  Q_PROPERTY(bool   densityBars     READ isDensityBars     WRITE setDensityBars    )

  // scatter
  Q_PROPERTY(bool   scatter       READ isScatter     WRITE setScatter      )
  Q_PROPERTY(double scatterFactor READ scatterFactor WRITE setScatterFactor)

  // mean line
  Q_PROPERTY(bool showMean READ isShowMean WRITE setShowMean)

  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Mean,mean)

  // min bar size
  Q_PROPERTY(double minBarSize READ minBarSize WRITE setMinBarSize)

  // dot line (bar with dot)
  Q_PROPERTY(bool           dotLines     READ isDotLines   WRITE setDotLines    )
  Q_PROPERTY(CQChartsLength dotLineWidth READ dotLineWidth WRITE setDotLineWidth)

  CQCHARTS_NAMED_POINT_DATA_PROPERTIES(Dot,dot)

  // rug symbols
  Q_PROPERTY(bool rug READ isRug WRITE setRug)

  CQCHARTS_NAMED_POINT_DATA_PROPERTIES(Rug,rug)

  Q_ENUMS(PlotType)
  Q_ENUMS(ValueType)

 public:
  enum class PlotType {
    NORMAL,
    STACKED,
    SIDE_BY_SIDE,
    OVERLAY,
    SCATTER,
    DENSITY
  };

  enum class ValueType {
    COUNT,
    RANGE,
    MIN,
    MAX,
    MEAN,
    SUM
  };

  struct Filter {
    Filter(int groupInd, double min, double max) :
     groupInd(groupInd), minValue(min), maxValue(max) {
    }

    int    groupInd { -1 };
    double minValue { 1.0 };
    double maxValue { 1.0 };
  };

  enum class BucketValueType {
    START,
    END,
    ALL
  };

 public:
  CQChartsDistributionPlot(CQChartsView *view, const ModelP &model);

  virtual ~CQChartsDistributionPlot();

  //---

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumn &dataColumn() const { return dataColumn_; }
  void setDataColumn(const CQChartsColumn &c);

  //---

  QString valueColumnName(const QString &def="value") const;

  //---

  bool isBucketed() const { return bucketed_; }
  void setBucketed(bool b);

  bool isAutoBucket() const;
  void setAutoBucket(bool b);

  double startBucketValue() const;
  void setStartBucketValue(double r);

  double deltaBucketValue() const;
  void setDeltaBucketValue(double r);

  int numAutoBuckets() const;
  void setNumAutoBuckets(int i);

  //---

  PlotType plotType() const { return plotType_; }

  bool isNormal    () const { return (plotType() == PlotType::NORMAL      ); }
  bool isStacked   () const { return (plotType() == PlotType::STACKED     ); }
  bool isSideBySide() const { return (plotType() == PlotType::SIDE_BY_SIDE); }
  bool isOverlay   () const { return (plotType() == PlotType::OVERLAY     ); }
  bool isDensity   () const { return (plotType() == PlotType::DENSITY     ); }

  ValueType valueType() const { return valueType_; }

  bool isValueCount() const { return (valueType() == ValueType::COUNT); }
  bool isValueRange() const { return (valueType() == ValueType::RANGE); }
  bool isValueMin  () const { return (valueType() == ValueType::MIN  ); }
  bool isValueMax  () const { return (valueType() == ValueType::MAX  ); }
  bool isValueMean () const { return (valueType() == ValueType::MEAN ); }
  bool isValueSum  () const { return (valueType() == ValueType::SUM  ); }

  bool isPercent  () const { return percent_  ; }
  bool isSkipEmpty() const { return skipEmpty_; }
  bool isSorted   () const { return sorted_   ; }

  //---

  double densityOffset() const { return densityData_.offset; }
  void setDensityOffset(double o);

  int densitySamples() const { return densityData_.numSamples; }
  void setDensitySamples(int n);

  bool isDensityGradient() const { return densityData_.gradient; }
  void setDensityGradient(bool b);

  bool isDensityBars() const { return densityData_.bars; }
  void setDensityBars(bool b);

  //---

  bool isScatter() const { return (plotType_ == PlotType::SCATTER); }

  double scatterFactor() const { return scatterData_.factor; }
  void setScatterFactor(double r);

  //---

  bool isDotLines() const { return dotLineData_.enabled; }

  const CQChartsLength &dotLineWidth() const { return dotLineData_.width; }
  void setDotLineWidth(const CQChartsLength &l);

  //---

  bool isRug() const { return rug_; }

  //---

  bool isShowMean() const { return showMean_; }

  //---

  double minBarSize() const { return minBarSize_; }
  void setMinBarSize(double s) { minBarSize_ = s; }

  double scatterMargin() const { return scatterMargin_; }
  void setScatterMargin(double m) { scatterMargin_ = m; }

  //---

  bool checkFilter(int groupInd, const QVariant &value) const;

  int calcBucket(int groupInd, double v) const;

  //---

  const CQChartsDataLabel *dataLabel() const { return dataLabel_; }
  CQChartsDataLabel *dataLabel() { return dataLabel_; }

  CQChartsGeom::BBox annotationBBox() const override;

  //---

  bool allowZoomX() const override;
  bool allowZoomY() const override;

  bool allowPanX() const override;
  bool allowPanY() const override;

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  CQChartsAxis *valueAxis() const;
  CQChartsAxis *countAxis() const;

  //---

  void addKeyItems(CQChartsPlotKey *key) override;

  //---

  bool addMenuItems(QMenu *) override;

  //---

  bool hasForeground() const override;

  void drawForeground(QPainter *) const override;

  void drawMeanLine(QPainter *) const;

  //---

  virtual QString bucketValuesStr(int groupInd, int bucket,
                                  BucketValueType type=BucketValueType::ALL) const;

  QString bucketStr(int groupInd, int bucket, BucketValueType type=BucketValueType::ALL) const;

  virtual void bucketValues(int groupInd, int bucket, double &value1, double &value2) const;

  //---

  CQBucketer &groupBucketer(int groupInd);
  const CQBucketer &groupBucketer(int groupInd) const;

  //---

 public:
  struct VariantInd {
    QVariant           var;
    CQChartsModelIndex ind;
    QVariant           dvar;

    VariantInd(const QVariant &var, const CQChartsModelIndex &ind, const QVariant &dvar) :
     var(var), ind(ind), dvar(dvar) {
    }
  };

  using VariantInds = std::vector<VariantInd>;

  struct VariantIndsData {
    VariantInds inds;
    double      min  { 0.0 };
    double      max  { 0.0 };
    double      mean { 0.0 };
    double      sum  { 0.0 };
  };

  using BarValue = CQChartsDistributionBarValue;

 public:
  void calcVarIndsData(VariantIndsData &varInds) const;

  BarValue varIndsValue(const VariantIndsData &varInds) const;

  void getInds(int groupInd, int bucket, VariantInds &inds) const;

  void getXVals(int groupInd, int bucket, std::vector<double> &xvals) const;

  bool getMeanValue(int groupInd, double &mean) const;

  bool getRealValues(int groupInd, std::vector<double> &xvals, double &mean) const;

 private:
  using Inds         = std::vector<CQChartsModelIndex>;
  using BucketValues = std::map<int,VariantIndsData>;

  struct Values {
    Inds              inds;                      //! value indices
    CQChartsValueSet* valueSet      { nullptr }; //! value set
    BucketValues      bucketValues;              //! bucketed values
    CQChartsDensity*  densityData   { nullptr }; //! density data
    double            mean          { 0.0 };     //! mean

    Values(CQChartsValueSet *valueSet);
   ~Values();

    Values(const Values &) = delete;
    Values &operator=(const Values &) = delete;
  };

  using GroupValues   = std::map<int,Values *>;
  using GroupBucketer = std::map<int,CQBucketer>;

  using Filters     = std::vector<Filter>;
  using FilterStack = std::vector<Filters>;

  using GroupBucketRange = std::map<int,CQChartsGeom::IMinMax>;

 private:
  void bucketGroupValues() const;

  CQChartsGeom::Range calcBucketRanges() const;

  void clearGroupValues() const;

  void addRow(const ModelVisitor::VisitData &data) const;

  void addRowColumn(const CQChartsModelIndex &ind) const;

  //---

  QString bucketValuesStr(int groupInd, int bucket, const Values *values,
                          BucketValueType type=BucketValueType::ALL) const;

  Values *getGroupIndValues(int groupInd, const CQChartsModelIndex &ind) const;

  const Values *getGroupValues(int groupInd) const;

 public slots:
  // set horizontal
  void setHorizontal(bool b) override;

  // set plot type
  void setPlotType(PlotType plotType);

  // set normal, stacked, side by side, overlay
  void setNormal    (bool b);
  void setStacked   (bool b);
  void setSideBySide(bool b);
  void setOverlay   (bool b);

  // set value type
  void setValueType(ValueType valueType);

  void setValueCount(bool b);
  void setValueRange(bool b);
  void setValueMin  (bool b);
  void setValueMax  (bool b);
  void setValueMean (bool b);
  void setValueSum  (bool b);

  // set percent
  void setPercent(bool b);

  // set skip empty
  void setSkipEmpty(bool b);

  // set sorted
  void setSorted(bool b);

  // set density
  void setDensity(bool b);

  // set scatter
  void setScatter(bool b);

  // set dot lines
  void setDotLines(bool b);

  // set rug
  void setRug(bool b);

  // set show mean
  void setShowMean(bool b);

  // push to bar range
  void pushSlot();
  // pop out of bar range
  void popSlot();
  // pop out of all bar ranges
  void popTopSlot();

 private:
  struct DensityData {
    double offset     { 0.0 };   //! offset
    int    numSamples { 100 };   //! num samples
    bool   gradient   { false }; //! gradient
    bool   bars       { false }; //! show bars
  };

  struct ScatterData {
    double factor { 1.0 }; //! factor
  };

  struct DotLineData {
    bool           enabled { false }; //! shown
    CQChartsLength width   { "3px" }; //! width
  };

  struct GroupData {
    GroupValues      groupValues;      //! grouped value sets
    GroupBucketer    groupBucketer;    //! group bucketer
    GroupBucketRange groupBucketRange; //! bucketer per group
  };

  CQChartsColumn     nameColumn_;                          //! name column
  CQChartsColumn     dataColumn_;                          //! data column
  PlotType           plotType_       { PlotType::NORMAL }; //! plot type
  ValueType          valueType_      { ValueType::COUNT }; //! show value count
  bool               percent_        { false };            //! percent values
  bool               skipEmpty_      { false };            //! skip empty buckets (non cont range)
  bool               sorted_         { false };            //! sort by count
  DensityData        densityData_;                         //! density data
  ScatterData        scatterData_;                         //! scatter data
  DotLineData        dotLineData_;                         //! show dot lines
  bool               rug_            { false };            //! show rug
  bool               showMean_       { false };            //! show mean
  double             minBarSize_     { 3.0 };              //! min bar size
  double             scatterMargin_  { 0.05 };             //! scatter point margin
  CQChartsDataLabel* dataLabel_      { nullptr };          //! data label data
  CQBucketer         bucketer_;                            //! shared bucketer
  bool               bucketed_       { true };             //! is bucketed
  FilterStack        filterStack_;                         //! filter stack
  GroupData          groupData_;                           //! grouped value sets
  mutable std::mutex mutex_;                               //! mutex
};

#endif
