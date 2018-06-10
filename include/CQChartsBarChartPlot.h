#ifndef CQChartsBarChartPlot_H
#define CQChartsBarChartPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDataLabel.h>
#include <CQChartsColor.h>

class CQChartsBarChartPlot;
class CQChartsBoxObj;

// bar value
class CQChartsBarChartValue {
 public:
  using NameValues = std::map<QString,QString>;

 public:
  struct ValueInd {
    double      value { 0.0 };
    QModelIndex ind;
  };

  using ValueInds = std::vector<ValueInd>;

 public:
  CQChartsBarChartValue() { }

  void addValueInd(const ValueInd &valueInd) {
    valueInds_.push_back(valueInd);
  }

  const ValueInds &valueInds() const { return valueInds_; }

  const QString &groupName() const { return groupName_; }
  void setGroupName(const QString &v) { groupName_ = v; }

  const QString &valueName() const { return valueName_; }
  void setValueName(const QString &v) { valueName_ = v; }

  const NameValues &nameValues() const { return nameValues_; }
  void setNameValues(const NameValues &v) { nameValues_ = v; }

  void setNameValue(const QString &name, QString &value) {
    nameValues_[name] = value;
  }

  QString getNameValue(const QString &name) const {
    auto p = nameValues_.find(name);
    if (p == nameValues_.end()) return "";

    return (*p).second;
  }

  void calcRange(ValueInd &minInd, ValueInd &maxInd) const {
    assert(! valueInds_.empty());

    minInd = valueInds_[0];
    maxInd = valueInds_[0];

    for (std::size_t i = 1; i < valueInds_.size(); ++i) {
      double value = valueInds_[i].value;

      if (value < minInd.value)
        minInd = valueInds_[i];

      if (value > minInd.value)
        maxInd = valueInds_[i];
    }
  }

 private:
  ValueInds  valueInds_;
  QString    groupName_;
  QString    valueName_;
  NameValues nameValues_;
};

//------

class CQChartsBarChartValueSet {
 public:
  using Values = std::vector<CQChartsBarChartValue>;

 public:
  CQChartsBarChartValueSet() = default;

  CQChartsBarChartValueSet(const QString &name, int ind) :
   name_(name), ind_(ind) {
  }

  const QString &name() const { return name_; }

  int ind() const { return ind_; }

  int numValues() const { return values_.size(); }

  const Values &values() const { return values_; }

  void addValue(const CQChartsBarChartValue &value) {
    values_.push_back(value);
  }

  const CQChartsBarChartValue &value(int i) const {
    assert(i >= 0 && i < int(values_.size()));

    return values_[i];
  }

  void calcSums(double &posSum, double &negSum) const {
    posSum = 0.0;
    negSum = 0.0;

    for (auto &v : values_) {
      for (auto &vi : v.valueInds()) {
        double value = vi.value;

        if (value >= 0) posSum += value;
        else            negSum += value;
      }
    }
  }

 private:
  QString name_;
  int     ind_ { 0 };
  Values  values_;
};

//------

// bar object
class CQChartsBarChartObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using OptColor = boost::optional<CQChartsColor>;

 public:
  CQChartsBarChartObj(CQChartsBarChartPlot *plot, const CQChartsGeom::BBox &rect,
                      int iset, int nset, int ival, int nval, int isval, int nsval,
                      const CQChartsBarChartValue *value, const QModelIndex &ind);

  QString calcId() const override;

  QString calcTipId() const override;

  void setColor(const CQChartsColor &color) { color_ = color; }

  CQChartsGeom::BBox dataLabelRect() const;

  void getSelectIndices(Indices &inds) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsBarChartPlot*        plot_  { nullptr }; // parent plot
  int                          iset_  { -1 };      // set number
  int                          nset_  { -1 };      // number of sets
  int                          ival_  { -1 };      // value number
  int                          nval_  { -1 };      // number of values
  int                          isval_ { -1 };      // sub set number
  int                          nsval_ { -1 };      // number of sub sets
  const CQChartsBarChartValue* value_ { nullptr }; // value data
  QModelIndex                  ind_;               // model index
  OptColor                     color_;             // custom color
};

//---

#include <CQChartsKey.h>

// key color box
class CQChartsBarKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  using OptColor = boost::optional<CQChartsColor>;

 public:
  CQChartsBarKeyColor(CQChartsBarChartPlot *plot, int i, int n);

  void setColor(const CQChartsColor &color) { color_ = color; }

  bool selectPress(const CQChartsGeom::Point &p) override;

  QBrush fillBrush() const override;

  bool tipText(const CQChartsGeom::Point &p, QString &tip) const override;

  bool isSetHidden() const;

  void setSetHidden(bool b);

 private:
  CQChartsBarChartPlot *plot_;  // plot
  OptColor              color_; // custom color
};

// key text
class CQChartsBarKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsBarKeyText(CQChartsBarChartPlot *plot, int i, const QString &text);

  QColor interpTextColor(int i, int n) const override;

 private:
  int i_ { 0 }; // set id
};

//---

// bar chart plot type
class CQChartsBarChartPlotType : public CQChartsPlotType {
 public:
  CQChartsBarChartPlotType();

  QString name() const override { return "barchart"; }
  QString desc() const override { return "BarChart"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

// bar chart plot
//  x   : category, name
//  y   : value, values
//  bar : custom color, stacked, percent, range, horizontal, margin, border, fill
class CQChartsBarChartPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn   categoryColumn READ categoryColumn  WRITE setCategoryColumn )
  Q_PROPERTY(CQChartsColumn   valueColumn    READ valueColumn     WRITE setValueColumn    )
  Q_PROPERTY(QString          valueColumns   READ valueColumnsStr WRITE setValueColumnsStr)
  Q_PROPERTY(CQChartsColumn   nameColumn     READ nameColumn      WRITE setNameColumn     )
  Q_PROPERTY(CQChartsColumn   labelColumn    READ labelColumn     WRITE setLabelColumn    )
  Q_PROPERTY(bool             rowGrouping    READ isRowGrouping   WRITE setRowGrouping    )
  Q_PROPERTY(bool             colorBySet     READ isColorBySet    WRITE setColorBySet     )
  Q_PROPERTY(CQChartsColumn   colorColumn    READ colorColumn     WRITE setColorColumn    )
  Q_PROPERTY(bool             stacked        READ isStacked       WRITE setStacked        )
  Q_PROPERTY(bool             percent        READ isPercent       WRITE setPercent        )
  Q_PROPERTY(bool             rangeBar       READ isRangeBar      WRITE setRangeBar       )
  Q_PROPERTY(bool             horizontal     READ isHorizontal    WRITE setHorizontal     )
  Q_PROPERTY(CQChartsLength   margin         READ margin          WRITE setMargin         )
  Q_PROPERTY(bool             border         READ isBorder        WRITE setBorder         )
  Q_PROPERTY(CQChartsColor    borderColor    READ borderColor     WRITE setBorderColor    )
  Q_PROPERTY(double           borderAlpha    READ borderAlpha     WRITE setBorderAlpha    )
  Q_PROPERTY(CQChartsLength   borderWidth    READ borderWidth     WRITE setBorderWidth    )
  Q_PROPERTY(CQChartsLineDash borderDash     READ borderDash      WRITE setBorderDash     )
  Q_PROPERTY(CQChartsLength   cornerSize     READ cornerSize      WRITE setCornerSize     )
  Q_PROPERTY(bool             barFill        READ isBarFill       WRITE setBarFill        )
  Q_PROPERTY(CQChartsColor    barColor       READ barColor        WRITE setBarColor       )
  Q_PROPERTY(double           barAlpha       READ barAlpha        WRITE setBarAlpha       )
  Q_PROPERTY(Pattern          barPattern     READ barPattern      WRITE setBarPattern     )
  Q_PROPERTY(bool             colorMapped    READ isColorMapped   WRITE setColorMapped    )
  Q_PROPERTY(double           colorMapMin    READ colorMapMin     WRITE setColorMapMin    )
  Q_PROPERTY(double           colorMapMax    READ colorMapMax     WRITE setColorMapMax    )

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

  using Columns = std::vector<CQChartsColumn>;

 public:
  CQChartsBarChartPlot(CQChartsView *view, const ModelP &model);
 ~CQChartsBarChartPlot();

  //---

  const CQChartsColumn &categoryColumn() const { return categoryColumn_; }
  void setCategoryColumn(const CQChartsColumn &c);

  const CQChartsColumn &valueColumn() const { return valueColumn_; }
  void setValueColumn(const CQChartsColumn &c);

  const Columns &valueColumns() const { return valueColumns_; }
  void setValueColumns(const Columns &valueColumns);

  QString valueColumnsStr() const;
  bool setValueColumnsStr(const QString &s);

  const CQChartsColumn &valueColumnAt(int i) {
    assert(i >= 0 && i < int(valueColumns_.size()));

    return valueColumns_[i];
  }

  int numValueColumns() const { return std::max(int(valueColumns_.size()), 1); }

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumn &labelColumn() const { return labelColumn_; }
  void setLabelColumn(const CQChartsColumn &c);

  //---

  bool isColorBySet() const { return colorBySet_; }
  void setColorBySet(bool b) { colorBySet_ = b; resetSetHidden(); updateRangeAndObjs(); }

  //---

  bool isStacked() const { return stacked_; }

  bool isRangeBar() const { return rangeBar_; }

  bool isPercent() const { return percent_; }

  bool isHorizontal() const { return horizontal_; }

  //---

  // bar margin
  const CQChartsLength &margin() const { return margin_; }
  void setMargin(const CQChartsLength &l);

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
  void setCornerSize(const CQChartsLength &s);

  //---

  // bar fill
  bool isBarFill() const;
  void setBarFill(bool b);

  const CQChartsColor &barColor() const;
  void setBarColor(const CQChartsColor &c);

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

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void updateObjs() override;

  bool initObjs() override;

  //---

  QColor interpBarColor(int i, int n) const;

  QString valueStr(double v) const;

  void addKeyItems(CQChartsPlotKey *key) override;

   //---

  bool isSetHidden  (int i) const;
  bool isValueHidden(int i) const;

  //---

  bool probe(ProbeData &probeData) const override;

  bool addMenuItems(QMenu *menu) override;

  void draw(QPainter *) override;

  //---

 public slots:
  // set stacked
  void setStacked(bool b);

  // set range bar
  void setRangeBar(bool b);

  // set percent
  void setPercent(bool b);

  // set horizontal
  void setHorizontal(bool b);

 private:
  void addRow(QAbstractItemModel *model, const QModelIndex &parent, int r);

  void addRowColumn(QAbstractItemModel *model, const QModelIndex &parent, int row,
                    const Columns &valueColumns);

 private:
  using ValueSets     = std::vector<CQChartsBarChartValueSet>;
  using ValueNames    = std::vector<QString>;
  using ValueGroupInd = std::map<int,int>;

 public:
  int numValueSets() const { return valueSets_.size(); }

  const CQChartsBarChartValueSet &valueSet(int i) const {
    assert(i >= 0 && i < int(valueSets_.size()));
    return valueSets_[i];
  }

  int numSetValues() const { return (! valueSets_.empty() ? valueSets_[0].numValues() : 0); }

 private:
  const QString &valueName(int i) const { return valueNames_[i]; }

  CQChartsBarChartValueSet *groupValueSet(int groupId);

 private:
  CQChartsColumn     categoryColumn_;           // category column
  CQChartsColumn     valueColumn_    { 1 };     // value column
  Columns            valueColumns_;             // value columns
  CQChartsColumn     nameColumn_;               // name column
  CQChartsColumn     labelColumn_;              // data label column
  bool               colorBySet_     { false }; // color bars by set or value
  bool               stacked_        { false }; // stacked bars
  bool               percent_        { false }; // percent values
  bool               rangeBar_       { false }; // bar of value range
  bool               horizontal_     { false }; // horizontal bars
  CQChartsLength     margin_         { "2px" }; // bar margin
  CQChartsBoxData    boxData_;                  // box style data
  CQChartsDataLabel  dataLabel_;                // data label data
  ValueSets          valueSets_;                // value sets
  ValueNames         valueNames_;               // value names
  ValueGroupInd      valueGroupInd_;            // group ind to value index map
  int                numVisible_     { 0 };     // number of visible bars
};

#endif
