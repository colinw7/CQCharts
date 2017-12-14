#ifndef CQChartsDistributionPlot_H
#define CQChartsDistributionPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsValueSet.h>
#include <CQChartsDataLabel.h>
#include <CQChartsPaletteColor.h>

class CQChartsDistributionPlot;
class CQChartsBoxObj;
class CQChartsFillObj;

// bar object
class CQChartsDistributionBarObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Values = std::vector<QModelIndex>;

  using OptColor = boost::optional<CQChartsPaletteColor>;

 public:
  CQChartsDistributionBarObj(CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                             int bucket, const Values &value, int i, int n);

  int bucket() const { return bucket_; }

  const Values &values() const { return values_; }

  QString calcId() const override;

  void addSelectIndex() override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

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

// distribution plot type
class CQChartsDistributionPlotType : public CQChartsPlotType {
 public:
  CQChartsDistributionPlotType();

  QString name() const override { return "distribution"; }
  QString desc() const override { return "Distribution"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

// distribution plot
class CQChartsDistributionPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int     valueColumn      READ valueColumn       WRITE setValueColumn     )
  Q_PROPERTY(int     colorColumn      READ colorColumn       WRITE setColorColumn     )
  Q_PROPERTY(bool    autoRange        READ isAutoRange       WRITE setAutoRange       )
  Q_PROPERTY(double  startValue       READ startValue        WRITE setStartValue      )
  Q_PROPERTY(double  deltaValue       READ deltaValue        WRITE setDeltaValue      )
  Q_PROPERTY(int     numAuto          READ numAuto           WRITE setNumAuto         )
  Q_PROPERTY(bool    horizontal       READ isHorizontal      WRITE setHorizontal      )
  Q_PROPERTY(double  margin           READ margin            WRITE setMargin          )
  Q_PROPERTY(bool    border           READ isBorder          WRITE setBorder          )
  Q_PROPERTY(QString borderColor      READ borderColorStr    WRITE setBorderColorStr  )
  Q_PROPERTY(double  borderWidth      READ borderWidth       WRITE setBorderWidth     )
  Q_PROPERTY(double  borderCornerSize READ borderCornerSize  WRITE setBorderCornerSize)
  Q_PROPERTY(bool    barFill          READ isBarFill         WRITE setBarFill         )
  Q_PROPERTY(QString barColor         READ barColorStr       WRITE setBarColorStr     )
  Q_PROPERTY(double  barAlpha         READ barAlpha          WRITE setBarAlpha        )
  Q_PROPERTY(Pattern barPattern       READ barPattern        WRITE setBarPattern      )
  Q_PROPERTY(bool    colorMapEnabled  READ isColorMapEnabled WRITE setColorMapEnabled )
  Q_PROPERTY(double  colorMapMin      READ colorMapMin       WRITE setColorMapMin     )
  Q_PROPERTY(double  colorMapMax      READ colorMapMax       WRITE setColorMapMax     )

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

  using OptColor = boost::optional<CQChartsPaletteColor>;

  struct CategoryRange {
    enum class Type {
      DEFINED,
      AUTO
    };

    Type   type         { Type::AUTO };
    double start        { 0.0 };
    double delta        { 1.0 };
    int    numValues    { 0 };
    double minValue     { 0.0 };
    double maxValue     { 0.0 };
    int    numAuto      { 20 };
    double increment    { 1 };
    double calcMinValue { 1 };
  };

  struct Filter {
    Filter(double min, double max) :
     minValue(min), maxValue(max) {
    }

    double minValue { 1.0 };
    double maxValue { 1.0 };
  };

  using Values = std::vector<QModelIndex>;

  enum class BucketValueType {
    START,
    END,
    ALL
  };

 public:
  CQChartsDistributionPlot(CQChartsView *view, const ModelP &model);

  virtual ~CQChartsDistributionPlot();

  //---

  virtual int rowCount(QAbstractItemModel *model) const;

  virtual QVariant rowValue(QAbstractItemModel *model, int r, int c, bool &ok) const;

  virtual QVariant headerValue(QAbstractItemModel *model, int c, bool &ok) const;

  //---

  int valueColumn() const { return valueColumn_; }
  void setValueColumn(int i) { valueColumn_ = i; resetValues(); updateRangeAndObjs(); }

  int colorColumn() const { return colorColumn_; }
  void setColorColumn(int i) { colorColumn_ = i; updateRangeAndObjs(); }

  //---

  bool isAutoRange() const { return categoryRange_.type == CategoryRange::Type::AUTO; }

  void setAutoRange(bool b) {
    categoryRange_.type = (b ? CategoryRange::Type::AUTO : CategoryRange::Type::DEFINED);

    updateRangeAndObjs();
  }

  double startValue() const { return categoryRange_.start; }
  void setStartValue(double r) { categoryRange_.start = r; updateRangeAndObjs(); }

  double deltaValue() const { return categoryRange_.delta; }
  void setDeltaValue(double r) { categoryRange_.delta = r; updateRangeAndObjs(); }

  int numAuto() const { return categoryRange_.numAuto; }

  void setNumAuto(int i) {
    categoryRange_.numAuto = i; calcCategoryRange(); updateRangeAndObjs();
  }

  //---

  bool checkFilter(double value) const;

  int calcBucket(double v) const;

  double calcStartValue() const;

  void calcCategoryRange();

  void resetValues() { valueSet_.clear(); }

  //---

  bool isHorizontal() const { return horizontal_; }
  void setHorizontal(bool b) { horizontal_ = b; updateRangeAndObjs(); }

  //---

  // bar margin
  int margin() const { return margin_; }
  void setMargin(int i) { margin_ = i; update(); }

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

  QString borderColorStr() const;
  void setBorderColorStr(const QString &s);

  QColor interpBorderColor(int i, int n) const;

  double borderWidth() const;
  void setBorderWidth(double r);

  double borderCornerSize() const;
  void setBorderCornerSize(double r);

  //---

  // bar fill
  bool isBarFill() const;
  void setBarFill(bool b);

  QString barColorStr() const;
  void setBarColorStr(const QString &str);

  QColor interpBarColor(int i, int n) const;

  double barAlpha() const;
  void setBarAlpha(double a);

  Pattern barPattern() const;
  void setBarPattern(Pattern pattern);

  //---

  void initColorSet();

  bool colorSetColor(int i, OptColor &color);

  bool isColorMapEnabled() const { return colorSet_.isMapEnabled(); }
  void setColorMapEnabled(bool b) { colorSet_.setMapEnabled(b); updateObjs(); }

  double colorMapMin() const { return colorSet_.mapMin(); }
  void setColorMapMin(double r) { colorSet_.setMapMin(r); updateObjs(); }

  double colorMapMax() const { return colorSet_.mapMax(); }
  void setColorMapMax(double r) { colorSet_.setMapMax(r); updateObjs(); }

  //---

  const CQChartsDataLabel &dataLabel() const { return dataLabel_; }
  CQChartsDataLabel &dataLabel() { return dataLabel_; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void updateObjs() override;

  bool initObjs() override;

  //---

  CQChartsAxis *valueAxis() const;
  CQChartsAxis *countAxis() const;

  void addKeyItems(CQChartsKey *key) override;

  //---

  QString bucketValuesStr(int bucket, BucketValueType valueType=BucketValueType::ALL) const;

  void bucketValues(int bucket, double &value1, double &value2) const;

  //---

  bool addMenuItems(QMenu *) override;

  //---

  void draw(QPainter *) override;

  void drawDataLabel(QPainter *painter, const QRectF &qrect, const QString &ystr);

 private slots:
  void pushSlot();
  void popSlot();

 private:
  using IValues = std::map<int,Values>;
  using Filters = std::vector<Filter>;

 private:
  int                  valueColumn_ { -1 };      // value column
  int                  colorColumn_ { -1 };      // color column
  CategoryRange        categoryRange_;           // category range
  CQChartsValueSet     valueSet_;
  bool                 autoDelta_   { false };   // auto delta
  IValues              ivalues_;                 // indexed values
  bool                 horizontal_  { false };   // horizontal bars
  double               margin_      { 2 };       // bar margin
  CQChartsBoxObj*      borderObj_   { nullptr }; // border object
  CQChartsFillObj*     fillObj_     { nullptr }; // fill object
  CQChartsValueSet     colorSet_;                // color column value set
  CQChartsDataLabel    dataLabel_;               // data label data
  Filters              filters_;                 // filter stack
};

#endif
