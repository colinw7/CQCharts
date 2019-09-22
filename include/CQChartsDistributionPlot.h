#ifndef CQChartsDistributionPlot_H
#define CQChartsDistributionPlot_H

#include <CQChartsBarPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsColor.h>
#include <CQStatData.h>

class CQChartsDensity;

//---

struct CQChartsDistributionBarValue {
  using RangeValue = CQChartsGeom::RangeValue;

  double     n1 { 0 };
  double     n2 { 0 };
  RangeValue xr;
  RangeValue yr;
};

//---

/*!
 * \brief Distribution Plot Bucket
 * \ingroup Charts
 */
class CQChartsDistributionBucket {
 public:
  enum class Type {
    VALUE,
    UNDERFLOW,
    OVERFLOW
  };

  CQChartsDistributionBucket() { }

  explicit
  CQChartsDistributionBucket(int value) :
   type_(Type::VALUE), value_(value) {
  }

  explicit
  CQChartsDistributionBucket(Type type) :
   type_(type) {
  }

  bool hasValue() const { return (type_ == Type::VALUE); }

  int value() const { assert(type_ == Type::VALUE); return value_; }

  bool isUnderflow() const { return (type_ == Type::UNDERFLOW); }
  bool isOverflow () const { return (type_ == Type::OVERFLOW ); }

  int outsideValue(int min, int max) const {
    if (isUnderflow()) return min;
    if (isOverflow ()) return max;
    assert(false);
  }

  bool operator<(const CQChartsDistributionBucket &rhs) const {
    if      (type_ == Type::UNDERFLOW) {
      if (rhs.type_ == Type::UNDERFLOW)
        return false;

      return true;
    }
    else if (type_ == Type::OVERFLOW) {
      if (rhs.type_ == Type::OVERFLOW)
        return false;

      return false;
    }
    else {
      if      (rhs.type_ == Type::UNDERFLOW)
        return false;
      else if (rhs.type_ == Type::OVERFLOW)
        return true;
      else
        return value_ < rhs.value_;
    }
  }

 private:
  Type type_  { Type::VALUE }; //!< bucket type
  int  value_ { -1 };          //!< bucket value
};

//---

/*!
 * \brief Distribution plot type
 * \ingroup Charts
 */
class CQChartsDistributionPlotType : public CQChartsGroupPlotType {
 public:
  CQChartsDistributionPlotType();

  QString name() const override { return "distribution"; }
  QString desc() const override { return "Distribution"; }

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

class CQChartsDistributionPlot;

/*!
 * \brief Distribution Plot Bar object
 * \ingroup Charts
 */
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
  using ColorSize  = std::pair<CQChartsColor,double>;
  using ColorSizes = std::vector<ColorSize>;

  struct ColorData {
    ColorCount colorCount;
    ColorSet   colorSet;
    ColorRows  colorRows;
    int        nv { 0 };
    ColorSizes colorSizes;
  };

 public:
  using Bucket   = CQChartsDistributionBucket;
  using BarValue = CQChartsDistributionBarValue;

  CQChartsDistributionBarObj(const CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                             int groupInd, const Bucket &bucket, const BarValue &barValue,
                             const ColorInd &ig, const ColorInd &iv);

  int groupInd() const { return groupInd_; }

  const Bucket &bucket() const { return bucket_; }

  //---

  QString typeName() const override { return "bar"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  QString groupName() const;

  QString bucketStr() const;

  bool bucketXValue(double x, double &value) const;
  bool bucketYValue(double y, double &value) const;

  int count() const;

  double minValue() const;
  double maxValue() const;

  //---

  CQChartsGeom::BBox dataLabelRect() const;

  //---

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  //---

  void draw(CQChartsPaintDevice *device) override;

  void drawFg(CQChartsPaintDevice *device) const override;

  void drawRug(CQChartsPaintDevice *device) const;

  //---

  CQChartsGeom::BBox calcRect() const;

  //---

  double mapValue(double v) const;

 private:
  bool getBarColoredRects(ColorData &colorData) const;

  void drawRect(CQChartsPaintDevice *device, const QRectF &qrect, const CQChartsColor &color,
                bool useLine) const;

  QColor barColor() const;

  double xColorValue(bool relative=true) const override;
  double yColorValue(bool relative=true) const override;

 private:
  const CQChartsDistributionPlot* plot_     { nullptr };
  int                             groupInd_ { -1 };
  Bucket                          bucket_;
  BarValue                        barValue_;
  double                          value1_   { 0.0 };
  double                          value2_   { 1.0 };
  ColorData                       colorData_;
};

//---

/*!
 * \brief Distribution Plot Density object
 * \ingroup Charts
 */
class CQChartsDistributionDensityObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString groupName  READ groupName )
  Q_PROPERTY(int     numSamples READ numSamples)

 public:
  using Bucket = CQChartsDistributionBucket;

  using Points = std::vector<QPointF>;

  struct BucketCount {
    Bucket bucket;
    int    n;

    BucketCount(const Bucket &bucket, int n) :
     bucket(bucket), n(n) {
    }
  };

  using BucketCounts = std::vector<BucketCount>;

  struct Data {
    Points       points;
    double       xmin { 0.0 };
    double       xmax { 0.0 };
    double       ymin { 0.0 };
    double       ymax { 0.0 };
    CQStatData   statData;
    BucketCounts buckets;
  };

 public:
  CQChartsDistributionDensityObj(const CQChartsDistributionPlot *plot,
                                 const CQChartsGeom::BBox &rect, int groupInd,
                                 const Data &data, double doffset, const ColorInd &is);

  int groupInd() const { return groupInd_; }

  //---

  QString typeName() const override { return "density"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  QString groupName() const;

  int numSamples() const;

  //---

  bool inside(const CQChartsGeom::Point &p) const override;

  //---

  void getSelectIndices(Indices &inds) const override;

  //---

  void draw(CQChartsPaintDevice *device) override;

  void drawFg(CQChartsPaintDevice *device) const override;

  void drawStatsLines(CQChartsPaintDevice *device) const;

  void drawRug(CQChartsPaintDevice *device) const;

  //---

  CQChartsGeom::BBox calcRect() const;

 private:
  const CQChartsDistributionPlot* plot_        { nullptr };
  int                             groupInd_    { -1 };
  Data                            data_;
  double                          doffset_     { 0.0 };
  ColorInd                        is_;
  QPolygonF                       poly_;
  double                          bucketScale_ { 1.0 };
};

//---

/*!
 * \brief Distribution Plot Scatter object
 * \ingroup Charts
 */
class CQChartsDistributionScatterObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Bucket = CQChartsDistributionBucket;

 public:
  CQChartsDistributionScatterObj(const CQChartsDistributionPlot *plot,
                                 const CQChartsGeom::BBox &rect, int groupInd,
                                 const Bucket &bucket, int n,
                                 const ColorInd &is, const ColorInd &iv);

  int groupInd() const { return groupInd_; }

  const Bucket &bucket() const { return bucket_; }

  //---

  QString typeName() const override { return "scatter"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void draw(CQChartsPaintDevice *device) override;

  //---

  void getSelectIndices(Indices &inds) const override;

 private:
  using Points = std::vector<QPointF>;

  const CQChartsDistributionPlot* plot_     { nullptr };
  int                             groupInd_ { -1 };
  Bucket                          bucket_;
  int                             n_        { 0 };
  ColorInd                        is_;
  ColorInd                        iv_;
  Points                          points_;
};

//---

#include <CQChartsKey.h>

/*!
 * \brief Distribution Plot Key Color Box
 * \ingroup Charts
 */
class CQChartsDistKeyColorBox : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsDistKeyColorBox(CQChartsDistributionPlot *plot, const ColorInd &ig, const ColorInd &iv,
                          const RangeValue &xv, const RangeValue &yv);

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &v) { color_ = v; }

  bool selectPress(const CQChartsGeom::Point &p, CQChartsSelMod selMod) override;

  QBrush fillBrush() const override;

  // get/set hidden
  bool isSetHidden() const;
  void setSetHidden(bool b);

 private:
  CQChartsDistributionPlot* plot_ { nullptr }; //!< plot
  CQChartsColor             color_;            //!< custom color
};

/*!
 * \brief Distribution Plot Key Text
 * \ingroup Charts
 */
class CQChartsDistKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsDistKeyText(CQChartsDistributionPlot *plot, const QString &text, const ColorInd &iv);

  QColor interpTextColor(const ColorInd &ind) const override;

  bool isSetHidden() const;
};

//---

/*!
 * \brief Distribution plot
 * \ingroup Charts
 */
class CQChartsDistributionPlot : public CQChartsBarPlot,
 public CQChartsObjStatsLineData<CQChartsDistributionPlot>,
 public CQChartsObjDotPointData <CQChartsDistributionPlot>,
 public CQChartsObjRugPointData <CQChartsDistributionPlot> {
  Q_OBJECT

  // columns
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

  // stats data
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Stats,stats)

  Q_PROPERTY(bool includeOutlier READ isIncludeOutlier WRITE setIncludeOutlier)

  // underflow/overflow bucket
  Q_PROPERTY(CQChartsOptReal underflowBucket READ underflowBucket WRITE setUnderflowBucket)
  Q_PROPERTY(CQChartsOptReal overflowBucket  READ overflowBucket  WRITE setOverflowBucket )

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

  using Bucket = CQChartsDistributionBucket;

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

  bool isIncludeOutlier() const { return includeOutlier_; }
  void setIncludeOutlier(bool b);

  //---

  const CQChartsOptReal &underflowBucket() const { return underflowBucket_; }
  void setUnderflowBucket(const CQChartsOptReal &r);

  const CQChartsOptReal &overflowBucket() const { return overflowBucket_; }
  void setOverflowBucket(const CQChartsOptReal &r);

  //---

  double minBarSize() const { return minBarSize_; }
  void setMinBarSize(double s);

  double scatterMargin() const { return scatterMargin_; }
  void setScatterMargin(double m);

  //---

  bool checkFilter(int groupInd, const QVariant &value) const;

  Bucket calcBucket(int groupInd, double v) const;

  //---

  CQChartsGeom::BBox annotationBBox() const override;

  //---

  bool allowZoomX() const override;
  bool allowZoomY() const override;

  bool allowPanX() const override;
  bool allowPanY() const override;

  //---

  void addProperties() override;

  //---

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  CQChartsAxis *valueAxis() const;
  CQChartsAxis *countAxis() const;

  //---

  void addKeyItems(CQChartsPlotKey *key) override;

  //---

  QString posStr(const CQChartsGeom::Point &w) const override;

  //---

  bool addMenuItems(QMenu *) override;

  //---

  bool hasForeground() const override;

  void execDrawForeground(CQChartsPaintDevice *) const override;

  void drawStatsLines(CQChartsPaintDevice *) const;

  //---

  virtual QString bucketValuesStr(int groupInd, const Bucket &bucket,
                                  BucketValueType type=BucketValueType::ALL) const;

  QString bucketStr(int groupInd, const Bucket &bucket,
                    BucketValueType type=BucketValueType::ALL) const;

  virtual void bucketValues(int groupInd, const Bucket &bucket,
                            double &value1, double &value2) const;

  //---

  CQBucketer &groupBucketer(int groupInd);
  const CQBucketer &groupBucketer(int groupInd) const;

  bool isConsistentBucketer() const;

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
    VariantInds           inds;         //!< model indices
    double                min  { 0.0 }; //!< min value
    double                max  { 0.0 }; //!< max value
    CQStatData            statData;     //!< stats data
    CQChartsGeom::RMinMax valueRange;   //!< value range
  };

  using BarValue = CQChartsDistributionBarValue;

 public:
  void calcVarIndsData(VariantIndsData &varInds) const;

  BarValue varIndsValue(const VariantIndsData &varInds) const;

  void getInds(int groupInd, const Bucket &bucket, VariantInds &inds) const;

  void getXVals(int groupInd, const Bucket &bucket, std::vector<double> &xvals) const;

  bool getStatData(int groupInd, CQStatData &statData) const;

  bool getRealValues(int groupInd, std::vector<double> &xvals, CQStatData &statData) const;

  bool hasGroups() const;

 private:
  using Inds         = std::vector<CQChartsModelIndex>;
  using BucketValues = std::map<Bucket,VariantIndsData>;

  struct Values {
    Inds                  inds;                      //!< value indices
    CQChartsValueSet*     valueSet      { nullptr }; //!< value set
    BucketValues          bucketValues;              //!< bucketed values
    CQChartsDensity*      densityData   { nullptr }; //!< density data
    CQStatData            statData;                  //!< stat data
    CQChartsGeom::RMinMax xValueRange;               //!< x value range
    CQChartsGeom::RMinMax yValueRange;               //!< y value range

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

  QString bucketValuesStr(int groupInd, const Bucket &bucket, const Values *values,
                          BucketValueType type=BucketValueType::ALL) const;

  Values *getGroupIndValues(int groupInd, const CQChartsModelIndex &ind) const;

  const Values *getGroupValues(int groupInd) const;

  //---

  double getPanX(bool is_shift) const override;
  double getPanY(bool is_shift) const override;

  //---

 public slots:
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

  // set show stats lines
  void setStatsLinesSlot(bool b);

  // push to bar range
  void pushSlot();
  // pop out of bar range
  void popSlot();
  // pop out of all bar ranges
  void popTopSlot();

 private:
  struct DensityData {
    double offset     { 0.0 };   //!< offset
    int    numSamples { 100 };   //!< num samples
    bool   gradient   { false }; //!< gradient
    bool   bars       { false }; //!< show bars
  };

  struct ScatterData {
    double factor { 1.0 }; //!< factor
  };

  struct DotLineData {
    bool           enabled { false }; //!< shown
    CQChartsLength width   { "3px" }; //!< width
  };

  struct GroupData {
    GroupValues      groupValues;      //!< grouped value sets
    GroupBucketer    groupBucketer;    //!< group bucketer
    GroupBucketRange groupBucketRange; //!< bucketer per group
  };

  CQChartsColumn     nameColumn_;                          //!< name column
  CQChartsColumn     dataColumn_;                          //!< data column
  PlotType           plotType_       { PlotType::NORMAL }; //!< plot type
  ValueType          valueType_      { ValueType::COUNT }; //!< show value count
  bool               percent_        { false };            //!< percent values
  bool               skipEmpty_      { false };            /*!< skip empty buckets
                                                                (non contiguous range) */
  bool               sorted_         { false };            //!< sort by count
  DensityData        densityData_;                         //!< density data
  ScatterData        scatterData_;                         //!< scatter data
  DotLineData        dotLineData_;                         //!< dot line data
  bool               rug_            { false };            //!< show rug
  bool               includeOutlier_ { true };             //!< include outlier values
  CQChartsOptReal    underflowBucket_;                     //!< underflow bucket threshold
  CQChartsOptReal    overflowBucket_;                      //!< overflow bucket threshold
  double             minBarSize_     { 3.0 };              //!< min bar size (pixels)
  double             scatterMargin_  { 0.05 };             //!< scatter point margin
  CQBucketer         bucketer_;                            //!< shared bucketer
  bool               bucketed_       { true };             //!< is bucketed
  FilterStack        filterStack_;                         //!< filter stack
  GroupData          groupData_;                           //!< grouped value sets
  double             barWidth_       { 1.0 };              //!< bar width
  mutable std::mutex mutex_;                               //!< mutex
};

#endif
