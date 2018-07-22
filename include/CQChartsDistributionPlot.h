#ifndef CQChartsDistributionPlot_H
#define CQChartsDistributionPlot_H

#include <CQChartsBarPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDataLabel.h>
#include <CQChartsColor.h>
#include <CQChartsDensity.h>

//---

// distribution plot type
class CQChartsDistributionPlotType : public CQChartsGroupPlotType {
 public:
  CQChartsDistributionPlotType();

  QString name() const override { return "distribution"; }
  QString desc() const override { return "Distribution"; }

  const char *yColumnName() const override { return "value"; }

  bool allowXAxisIntegral() const override { return false; }

  bool allowXLog() const override { return false; }

  Dimension dimension() const override { return Dimension::ONE_D; }

  void addParameters() override;

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsDistributionPlot;
class CQChartsBoxObj;

// bar object
class CQChartsDistributionBarObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString groupName READ groupName)
  Q_PROPERTY(QString bucketStr READ bucketStr)
  Q_PROPERTY(int     count     READ count    )
  Q_PROPERTY(int     minValue  READ minValue )
  Q_PROPERTY(int     maxValue  READ maxValue )

 public:
  using ColorCount = std::map<int,int>;
  using ColorSet   = std::map<CQChartsColor,int>;

  struct ColorData {
    ColorCount colorCount;
    ColorSet   colorSet;
    int        nv { 0 };
  };

 public:
  CQChartsDistributionBarObj(CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                             int groupInd, int bucket, double n1, double n2,
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

  int minValue() const;
  int maxValue() const;

  //---

  CQChartsGeom::BBox dataLabelRect() const;

  //---

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  //---

  void draw(QPainter *painter) override;

  void drawFg(QPainter *painter) override;

  void drawRug(QPainter *painter);

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
  CQChartsDistributionPlot *plot_     { nullptr };
  int                       groupInd_ { -1 };
  int                       bucket_   { -1 };
  double                    n1_       { 0.0 };
  double                    n2_       { 0.0 };
  int                       is_       { -1 };
  int                       ns_       { -1 };
  int                       iv_       { -1 };
  int                       nv_       { -1 };
  double                    value1_   { 0.0 };
  double                    value2_   { 1.0 };
};

//---

// density object
class CQChartsDistributionDensityObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString groupName  READ groupName )
  Q_PROPERTY(int     numSamples READ numSamples)

 public:
  using Points = std::vector<QPointF>;

 public:
  CQChartsDistributionDensityObj(CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                                 int groupInd, const Points &points, double mean, int is, int ns);

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

  void drawFg(QPainter *painter) override;

  void drawMeanLine(QPainter *painter);

  void drawRug(QPainter *painter);

  //---

  CQChartsGeom::BBox calcRect() const;

 private:
  CQChartsDistributionPlot *plot_     { nullptr };
  int                       groupInd_ { -1 };
  Points                    points_;
  double                    mean_     { 0.0 };
  int                       is_       { -1 };
  int                       ns_       { -1 };
  QPolygonF                 poly_;
};

//---

#include <CQChartsKey.h>

// key color box
class CQChartsDistKeyColorBox : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsDistKeyColorBox(CQChartsDistributionPlot *plot, int i, int n);

  bool selectPress(const CQChartsGeom::Point &p) override;

  QBrush fillBrush() const override;

  bool isSetHidden() const;

  void setSetHidden(bool b);

 private:
  CQChartsDistributionPlot *plot_;  // plot
};

//---

// distribution plot
class CQChartsDistributionPlot : public CQChartsBarPlot {
  Q_OBJECT

  // style
  Q_PROPERTY(CQChartsColumn nameColumn READ nameColumn WRITE setNameColumn)

  // bucketer
  Q_PROPERTY(bool   bucketed         READ isBucketed        WRITE setBucketed        )
  Q_PROPERTY(bool   autoBucket       READ isAutoBucket      WRITE setAutoBucket      )
  Q_PROPERTY(double startBucketValue READ startBucketValue  WRITE setStartBucketValue)
  Q_PROPERTY(double deltaBucketValue READ deltaBucketValue  WRITE setDeltaBucketValue)
  Q_PROPERTY(int    numAutoBuckets   READ numAutoBuckets    WRITE setNumAutoBuckets  )

  // options
  Q_PROPERTY(bool overlay   READ isOverlay   WRITE setOverlay  )
  Q_PROPERTY(bool skipEmpty READ isSkipEmpty WRITE setSkipEmpty)
  Q_PROPERTY(bool rangeBar  READ isRangeBar  WRITE setRangeBar )
  Q_PROPERTY(bool density   READ isDensity   WRITE setDensity  )

  // mean line
  Q_PROPERTY(bool             showMean  READ isShowMean WRITE setShowMean)
  Q_PROPERTY(CQChartsLineDash meanDash  READ meanDash   WRITE setMeanDash)

  // dot line
  Q_PROPERTY(bool           dotLines      READ isDotLines    WRITE setDotLines     )
  Q_PROPERTY(CQChartsLength dotLineWidth  READ dotLineWidth  WRITE setDotLineWidth )
  Q_PROPERTY(CQChartsSymbol dotSymbolType READ dotSymbolType WRITE setDotSymbolType)
  Q_PROPERTY(double         dotSymbolSize READ dotSymbolSize WRITE setDotSymbolSize)

  // rug
  Q_PROPERTY(bool           rug           READ isRug         WRITE setRug          )
  Q_PROPERTY(CQChartsSymbol rugSymbolType READ rugSymbolType WRITE setRugSymbolType)
  Q_PROPERTY(double         rugSymbolSize READ rugSymbolSize WRITE setRugSymbolSize)

 public:
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

  //---

  bool isBucketed() const { return bucketed_; }
  void setBucketed(bool b) { bucketed_ = b; }

  bool isAutoBucket() const;
  void setAutoBucket(bool b);

  double startBucketValue() const;
  void setStartBucketValue(double r);

  double deltaBucketValue() const;
  void setDeltaBucketValue(double r);

  int numAutoBuckets() const;
  void setNumAutoBuckets(int i);

  //---

  bool isOverlay() const { return overlay_; }

  bool isSkipEmpty() const { return skipEmpty_; }

  bool isRangeBar() const { return rangeBar_; }

  bool isDensity() const { return density_; }

  //---

  bool isDotLines() const { return dotLines_; }

  const CQChartsLength &dotLineWidth() const { return dotLineWidth_; }
  void setDotLineWidth(const CQChartsLength &l);

  const CQChartsSymbol &dotSymbolType() const { return dotSymbolType_; }
  void setDotSymbolType(const CQChartsSymbol &s);

  double dotSymbolSize() const { return dotSymbolSize_; }
  void setDotSymbolSize(double r);

  //---

  bool isRug() const { return rug_; }

  const CQChartsSymbol &rugSymbolType() const { return rugSymbolType_; }
  void setRugSymbolType(const CQChartsSymbol &s);

  double rugSymbolSize() const { return rugSymbolSize_; }
  void setRugSymbolSize(double r);

  //---

  bool isShowMean() const { return showMean_; }

  const CQChartsLineDash &meanDash() const { return meanDash_; }
  void setMeanDash(const CQChartsLineDash &d);

  //---

  bool checkFilter(int groupInd, const QVariant &value) const;

  int calcBucket(int groupInd, double v) const;

  //---

  const CQChartsDataLabel &dataLabel() const { return dataLabel_; }
  CQChartsDataLabel &dataLabel() { return dataLabel_; }

  CQChartsGeom::BBox annotationBBox() const override;

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  bool initObjs() override;

  //---

  CQChartsAxis *valueAxis() const;
  CQChartsAxis *countAxis() const;

  //---

  void addKeyItems(CQChartsPlotKey *key) override;

  //---

  bool addMenuItems(QMenu *) override;

  //---

  void drawForeground(QPainter *) override;

  void drawMeanLine(QPainter *);

  //---

  QString bucketValuesStr(int groupInd, int bucket, BucketValueType type) const;

  void bucketValues(int groupInd, int bucket, double &value1, double &value2) const;

  CQBucketer &groupBucketer(int groupInd) const;

  //---

 public:
  struct VariantInd {
    QVariant           var;
    CQChartsModelIndex ind;

    VariantInd(const QVariant &var, const CQChartsModelIndex &ind) :
     var(var), ind(ind) {
    }
  };

  using VariantInds = std::vector<VariantInd>;

  struct VariantIndsData {
    VariantInds inds;
    double      min { 0.0 };
    double      max { 0.0 };
  };

 public:
  void getInds(int groupInd, int bucket, VariantInds &inds) const;

  void getXVals(int groupInd, int bucket, std::vector<double> &xvals);

 private:
  using ValueSet     = CQChartsValueSet;
  using Inds         = std::vector<CQChartsModelIndex>;
  using BucketValues = std::map<int,VariantIndsData>;

  struct Values {
    Inds            inds;                      // value indices
    ValueSet*       valueSet      { nullptr }; // value set
    BucketValues    bucketValues;              // bucketed values
    CQChartsDensity densityData;               // density data
    double          mean          { 0.0 };     // mean

    Values(ValueSet *valueSet) :
     valueSet(valueSet) {
    }

   ~Values() {
      delete valueSet;
    }
  };

  using GroupValues   = std::map<int,Values *>;
  using GroupBucketer = std::map<int,CQBucketer>;

  using Filters     = std::vector<Filter>;
  using FilterStack = std::vector<Filters>;

  using GroupBucketRange = std::map<int,CQChartsGeom::IMinMax>;

 private:
  void clearGroupValues();

  void addRow(const QModelIndex &parent, int row);
  void addRowColumn(const CQChartsModelIndex &ind);

  //---

  QString bucketValuesStr(int groupInd, int bucket, const Values *values,
                          BucketValueType type) const;

 private slots:
  // set horizontal
  void setHorizontal(bool b) override;

  // set overlay
  void setOverlay(bool b);

  // set skip empty
  void setSkipEmpty(bool b);

  // set range bar
  void setRangeBar(bool b);

  // set density
  void setDensity(bool b);

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
  CQChartsColumn    nameColumn_;                                     // name column
  bool              overlay_       { false };                        // overlay groups
  bool              skipEmpty_     { false };                        // skip empty buckets
  bool              rangeBar_      { false };                        // show range bar
  bool              density_       { false };                        // show density
  bool              dotLines_      { false };                        // show dot lines
  CQChartsLength    dotLineWidth_  { "3px" };                        // dot line width
  CQChartsSymbol    dotSymbolType_ { CQChartsSymbol::Type::CIRCLE }; // dot symbol type
  double            dotSymbolSize_ { 7.0 };                          // dot symbol size
  bool              rug_           { false };                        // show rug
  CQChartsSymbol    rugSymbolType_ { CQChartsSymbol::Type::NONE };   // rug symbol type
  double            rugSymbolSize_ { 5.0 };                          // rug symbol size
  bool              showMean_      { false };                        // show mean
  CQChartsLineDash  meanDash_      { { 2, 2 }, 0 };                  // mean line dash
  CQChartsDataLabel dataLabel_;                                      // data label data
  GroupValues       groupValues_;                                    // grouped value sets
  GroupBucketer     groupBucketer_;                                  // group bucketer
  CQBucketer        bucketer_;                                       // shared bucketer
  bool              bucketed_      { true };                         // is bucketed
  FilterStack       filterStack_;                                    // filter stack
  GroupBucketRange  groupBucketRange_;                               // bucketer per group
};

#endif
