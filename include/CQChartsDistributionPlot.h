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

  QString calcId() const override;

  QString calcTipId() const override;

  CQChartsGeom::BBox dataLabelRect() const;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

  void drawFg(QPainter *painter) override;

  CQChartsGeom::BBox calcRect() const;

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
};

//---

// density object
class CQChartsDistributionDensityObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Points = std::vector<QPointF>;

 public:
  CQChartsDistributionDensityObj(CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                                 int groupInd, const Points &points, double mean, int is, int ns);

  int groupInd() const { return groupInd_; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

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
  Q_PROPERTY(bool showMean  READ isShowMean  WRITE setShowMean )

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

  bool isAutoBucket() const { return bucketer_.type() == CQBucketer::Type::REAL_AUTO; }
  void setAutoBucket(bool b) {
    bucketer_.setType(b ? CQBucketer::Type::REAL_AUTO : CQBucketer::Type::REAL_RANGE);
    updateRangeAndObjs(); }

  double startBucketValue() const { return bucketer_.rstart(); }
  void setStartBucketValue(double r) { bucketer_.setRStart(r); updateRangeAndObjs(); }

  double deltaBucketValue() const { return bucketer_.rdelta(); }
  void setDeltaBucketValue(double r) { bucketer_.setRDelta(r); updateRangeAndObjs(); }

  int numAutoBuckets() const { return bucketer_.numAuto(); }
  void setNumAutoBuckets(int i) { bucketer_.setNumAuto(i); updateRangeAndObjs(); }

  //---

  bool isOverlay() const { return overlay_; }

  bool isSkipEmpty() const { return skipEmpty_; }

  bool isRangeBar() const { return rangeBar_; }

  bool isDensity() const { return density_; }

  //---

  bool isShowMean() const { return showMean_; }
  void setShowMean(bool b) { showMean_ = b; updateRangeAndObjs(); }

  //---

  bool checkFilter(int groupInd, const QVariant &value) const;

  int calcBucket(double v) const;

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

  QString bucketValuesStr(int groupInd, int bucket, BucketValueType type) const;

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

  using GroupValues = std::map<int,Values *>;

  using Filters     = std::vector<Filter>;
  using FilterStack = std::vector<Filters>;

  using GroupBucketRange = std::map<int,CQChartsGeom::IMinMax>;

 private:
  void clearGroupValues();

  void addRow(const QModelIndex &parent, int row);
  void addRowColumn(const CQChartsModelIndex &ind);

  //---

  QString bucketValuesStr(int bucket, const Values *values, BucketValueType type) const;

  void bucketValues(int bucket, double &value1, double &value2) const;

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

  // push to bar range
  void pushSlot();
  // pop out of bar range
  void popSlot();
  // pop out of all bar ranges
  void popTopSlot();

 private:
  CQChartsColumn    nameColumn_;             // name column
  bool              overlay_      { false }; // overlay groups
  bool              skipEmpty_    { false }; // skip empty buckets
  bool              rangeBar_     { false }; // show range bar
  bool              density_      { false }; // show density
  bool              showMean_     { false }; // show mean
  CQChartsDataLabel dataLabel_;              // data label data
  GroupValues       groupValues_;            // grouped value sets
  bool              bucketed_     { true };  // is bucketed
  CQBucketer        bucketer_;               // bucketer
  FilterStack       filterStack_;            // filter stack
  GroupBucketRange  groupBucketRange_;
};

#endif
