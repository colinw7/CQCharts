#ifndef CQChartsDistributionPlot_H
#define CQChartsDistributionPlot_H

#include <CQChartsGroupPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDataLabel.h>
#include <CQChartsColor.h>

//---

// distribution plot type
class CQChartsDistributionPlotType : public CQChartsGroupPlotType {
 public:
  CQChartsDistributionPlotType();

  QString name() const override { return "distribution"; }
  QString desc() const override { return "Distribution"; }

  Dimension dimension() const override { return Dimension::ONE_D; }

  const char *yColumnName() const override { return "value"; }

  bool allowXAxisIntegral() const override { return false; }

  bool allowXLog() const override { return false; }

  void addParameters() override;

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;

  //---

  bool isGroupRequired() const override { return true; }

  bool allowRowGrouping() const override { return false; }

  bool allowUsePath() const override { return false; }

  bool allowUseRow() const override { return false; }
};

//---

class CQChartsDistributionPlot;
class CQChartsBoxObj;

// bar object
class CQChartsDistributionBarObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Values = std::vector<QModelIndex>;

  using OptColor = boost::optional<CQChartsColor>;

 public:
  CQChartsDistributionBarObj(CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                             int bucket, const Values &value, int i, int n);

  int bucket() const { return bucket_; }

  const Values &values() const { return values_; }

  QString calcId() const override;

  CQChartsGeom::BBox dataLabelRect() const;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

  CQChartsGeom::BBox calcRect() const;

 private:
  CQChartsDistributionPlot *plot_   { nullptr };
  int                       bucket_ { 0 };
  Values                    values_;
  int                       i_      { -1 };
  int                       n_      { -1 };
};

//---

#include <CQChartsKey.h>

// key color box
class CQChartsDistKeyColorBox : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsDistKeyColorBox(CQChartsDistributionPlot *plot, int i, int n);

  QBrush fillBrush() const override;
};

//---

// distribution plot
class CQChartsDistributionPlot : public CQChartsGroupPlot {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn   colorColumn  READ colorColumn     WRITE setColorColumn    )

  // options
  Q_PROPERTY(bool             horizontal   READ isHorizontal    WRITE setHorizontal     )
  Q_PROPERTY(CQChartsLength   margin       READ margin          WRITE setMargin         )

  // bar border
  Q_PROPERTY(bool             border       READ isBorder        WRITE setBorder         )
  Q_PROPERTY(CQChartsColor    borderColor  READ borderColor     WRITE setBorderColor    )
  Q_PROPERTY(double           borderAlpha  READ borderAlpha     WRITE setBorderAlpha    )
  Q_PROPERTY(CQChartsLength   borderWidth  READ borderWidth     WRITE setBorderWidth    )
  Q_PROPERTY(CQChartsLineDash borderDash   READ borderDash      WRITE setBorderDash     )
  Q_PROPERTY(CQChartsLength   cornerSize   READ cornerSize      WRITE setCornerSize     )

  // bar fill
  Q_PROPERTY(bool             barFill      READ isBarFill       WRITE setBarFill        )
  Q_PROPERTY(CQChartsColor    barColor     READ barColor        WRITE setBarColor       )
  Q_PROPERTY(double           barAlpha     READ barAlpha        WRITE setBarAlpha       )
  Q_PROPERTY(Pattern          barPattern   READ barPattern      WRITE setBarPattern     )

  // color map
  Q_PROPERTY(bool             colorMapped  READ isColorMapped   WRITE setColorMapped    )
  Q_PROPERTY(double           colorMapMin  READ colorMapMin     WRITE setColorMapMin    )
  Q_PROPERTY(double           colorMapMax  READ colorMapMax     WRITE setColorMapMax    )

  Q_ENUMS(Pattern)

 public:
  enum class Pattern {
    SOLID,
    HATCH,
    DENSE,
    HORIZ,
    VERT,
    FDIAG,
    BDIAG
  };

  struct Filter {
    Filter(double min, double max) :
     minValue(min), maxValue(max) {
    }

    double minValue { 1.0 };
    double maxValue { 1.0 };
  };

  using Columns = std::vector<CQChartsColumn>;
  using Values  = std::vector<QModelIndex>;

  enum class BucketValueType {
    START,
    END,
    ALL
  };

 public:
  CQChartsDistributionPlot(CQChartsView *view, const ModelP &model);

  virtual ~CQChartsDistributionPlot();

  //---

  void setGroupColumn(const CQChartsColumn &c) override;

  //---

  bool isHorizontal() const { return horizontal_; }

  //---

  bool checkFilter(double value) const;

  int calcBucket(double v) const;

  //---

  // bar margin
  const CQChartsLength &margin() const { return margin_; }
  void setMargin(const CQChartsLength &l);

  //---

  int numValues() const { return ivalues_.size(); }

  const Values &ivalues(int i) const {
    auto p = ivalues_.find(i);
    assert(p != ivalues_.end());

    return (*p).second;
  }

  //---

  // bar stroke
  bool isBorder() const;
  void setBorder(bool b);

  const CQChartsColor &borderColor() const;
  void setBorderColor(const CQChartsColor &c);

  QColor interpBorderColor(int i, int n) const;

  double borderAlpha() const;
  void setBorderAlpha(double r);

  const CQChartsLength &borderWidth() const;
  void setBorderWidth(const CQChartsLength &l);

  const CQChartsLineDash &borderDash() const;
  void setBorderDash(const CQChartsLineDash &v);

  const CQChartsLength &cornerSize() const;
  void setCornerSize(const CQChartsLength &r);

  //---

  // bar fill
  bool isBarFill() const;
  void setBarFill(bool b);

  const CQChartsColor &barColor() const;
  void setBarColor(const CQChartsColor &c);

  QColor interpBarColor(int i, int n) const;

  double barAlpha() const;
  void setBarAlpha(double a);

  Pattern barPattern() const;
  void setBarPattern(Pattern pattern);

  //---

  const CQChartsColumn &colorColumn() const { return valueSetColumn("color"); }
  void setColorColumn(const CQChartsColumn &c) {
    setValueSetColumn("color", c); updateRangeAndObjs(); }

  bool isColorMapped() const { return isValueSetMapped("color"); }
  void setColorMapped(bool b) { setValueSetMapped("color", b); updateObjs(); }

  double colorMapMin() const { return valueSetMapMin("color"); }
  void setColorMapMin(double r) { setValueSetMapMin("color", r); updateObjs(); }

  double colorMapMax() const { return valueSetMapMax("color"); }
  void setColorMapMax(double r) { setValueSetMapMax("color", r); updateObjs(); }

  //---

  const CQChartsDataLabel &dataLabel() const { return dataLabel_; }
  CQChartsDataLabel &dataLabel() { return dataLabel_; }

  //---

  CQChartsGeom::BBox annotationBBox() const override;

  //---

  bool allowZoomX() const override { return ! isHorizontal(); }
  bool allowZoomY() const override { return   isHorizontal(); }

  bool allowPanX() const override { return ! isHorizontal(); }
  bool allowPanY() const override { return   isHorizontal(); }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void updateObjs() override;

  bool initObjs() override;

  //---

  CQChartsAxis *valueAxis() const;
  CQChartsAxis *countAxis() const;

  //---

  void addKeyItems(CQChartsPlotKey *key) override;

  //---

  QString bucketValuesStr(int bucket, BucketValueType valueType=BucketValueType::ALL) const;

  void bucketValues(int bucket, double &value1, double &value2) const;

  //---

  bool probe(ProbeData &probeData) const;

  bool addMenuItems(QMenu *) override;

  void draw(QPainter *) override;

  //---

  double getPanX(bool is_shift) const override;
  double getPanY(bool is_shift) const override;

  //---

 private slots:
  // set horizontal
  void setHorizontal(bool b);

  // push to bar range
  void pushSlot();
  // pop out of bar range
  void popSlot();
  // pop out of all bar ranges
  void popTopSlot();

 private:
  using IValues     = std::map<int,Values>;
  using Filters     = std::vector<Filter>;
  using FilterStack = std::vector<Filters>;

 private:
  bool              horizontal_  { false }; // horizontal bars
  CQChartsLength    margin_      { "2px" }; // bar margin
  CQChartsBoxData   boxData_;               // border/fill data
  CQChartsDataLabel dataLabel_;             // data label data
  IValues           ivalues_;               // indexed values
  FilterStack       filterStack_;           // filter stack
};

#endif
