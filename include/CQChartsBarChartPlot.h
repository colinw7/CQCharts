#ifndef CQChartsBarChartPlot_H
#define CQChartsBarChartPlot_H

#include <CQChartsBarPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDataLabel.h>
#include <CQChartsColor.h>

//---

// bar chart plot type
class CQChartsBarChartPlotType : public CQChartsGroupPlotType {
 public:
  CQChartsBarChartPlotType();

  QString name() const override { return "barchart"; }
  QString desc() const override { return "BarChart"; }

  const char *yColumnName() const override { return "value"; }

  bool allowXAxisIntegral() const override { return false; }
  bool allowYAxisIntegral() const override { return false; }

  bool allowXLog() const override { return false; }

  Dimension dimension() const override { return Dimension::ONE_D; }

  void addParameters() override;

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsBarChartPlot;

// values for bar (normal: 1 value : range bar: multiple values)
// ToDo: handle range bar in distribution plot
class CQChartsBarChartValue {
 public:
  using NameValues = std::map<QString,QString>;

 public:
  struct ValueInd {
    double      value { 0.0 };
    QModelIndex ind;
    int         vrow { -1 };
  };

  using ValueInds = std::vector<ValueInd>;

 public:
  CQChartsBarChartValue() { }

  void addValueInd(const ValueInd &valueInd) {
    valueInds_.push_back(valueInd);
  }

  const ValueInds &valueInds() const { return valueInds_; }

  const QString &valueName() const { return valueName_; }
  void setValueName(const QString &s) { valueName_ = s; }

  const QString &groupName() const { return groupName_; }
  void setGroupName(const QString &s) { groupName_ = s; }

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
  QString    valueName_;
  QString    groupName_;
  NameValues nameValues_;
};

//------

// set of value bars for group
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

  int groupInd() const { return groupInd_; }
  void setGroupInd(int i) { groupInd_ = i; }

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
  QString name_;            // group name
  int     ind_      { 0 };  // index
  int     groupInd_ { -1 }; // group ind
  Values  values_;          // value bars
};

//------

// bar object
class CQChartsBarChartObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString group READ groupStr)
  Q_PROPERTY(QString name  READ nameStr )
  Q_PROPERTY(QString value READ valueStr)

 public:
  CQChartsBarChartObj(CQChartsBarChartPlot *plot, const CQChartsGeom::BBox &rect,
                      int iset, int nset, int ival, int nval, int isval, int nsval,
                      const QModelIndex &ind);

  QString calcId() const override;

  QString calcTipId() const override;

  QString groupStr() const;
  QString nameStr () const;
  QString valueStr() const;

  void setColor(const CQChartsColor &color) { color_ = color; }

  CQChartsGeom::BBox dataLabelRect() const;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

  void drawFg(QPainter *painter) override;

  const CQChartsBarChartValue *value() const;

 private:
  CQChartsBarChartPlot* plot_  { nullptr }; // parent plot
  int                   iset_  { -1 };      // set number
  int                   nset_  { -1 };      // number of sets
  int                   ival_  { -1 };      // value number
  int                   nval_  { -1 };      // number of values
  int                   isval_ { -1 };      // sub set number
  int                   nsval_ { -1 };      // number of sub sets
  QModelIndex           ind_;               // model index
  CQChartsColor         color_;             // custom color
};

//---

#include <CQChartsKey.h>

// key color box
class CQChartsBarKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsBarKeyColor(CQChartsBarChartPlot *plot, int i, int n);

  void setColor(const CQChartsColor &color) { color_ = color; }

  bool selectPress(const CQChartsGeom::Point &p) override;

  QBrush fillBrush() const override;

  bool tipText(const CQChartsGeom::Point &p, QString &tip) const override;

  bool isSetHidden() const;

  void setSetHidden(bool b);

 private:
  CQChartsBarChartPlot *plot_  { nullptr }; // plot
  CQChartsColor         color_;             // custom color
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

// bar chart plot
//  x     : name
//  y     : value(s)
//  group : group(s)
//  bar   : custom color, stacked, percent, range, horizontal, margin, border, fill
class CQChartsBarChartPlot : public CQChartsBarPlot,
 public CQChartsPlotDotPointData<CQChartsBarChartPlot> {
  Q_OBJECT

  // style
  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(CQChartsColumn labelColumn READ labelColumn WRITE setLabelColumn)

  // options
  Q_PROPERTY(bool stacked    READ isStacked    WRITE setStacked   )
  Q_PROPERTY(bool percent    READ isPercent    WRITE setPercent   )
  Q_PROPERTY(bool rangeBar   READ isRangeBar   WRITE setRangeBar  )
  Q_PROPERTY(bool colorBySet READ isColorBySet WRITE setColorBySet)

  // dot line
  Q_PROPERTY(bool           dotLines     READ isDotLines   WRITE setDotLines    )
  Q_PROPERTY(CQChartsLength dotLineWidth READ dotLineWidth WRITE setDotLineWidth)

  CQCHARTS_NAMED_POINT_DATA_PROPERTIES(Dot,dot)

 public:
  CQChartsBarChartPlot(CQChartsView *view, const ModelP &model);
 ~CQChartsBarChartPlot();

  //---

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumn &labelColumn() const { return labelColumn_; }
  void setLabelColumn(const CQChartsColumn &c);

  //---

  bool isStacked() const { return stacked_; }

  bool isRangeBar() const { return rangeBar_; }

  bool isPercent() const { return percent_; }

  //---

  bool isColorBySet() const { return colorBySet_; }
  void setColorBySet(bool b);

  //---

  bool isDotLines() const { return dotLines_; }

  const CQChartsLength &dotLineWidth() const { return dotLineWidth_; }
  void setDotLineWidth(const CQChartsLength &l);

  //---

  const CQChartsDataLabel &dataLabel() const { return dataLabel_; }
  CQChartsDataLabel &dataLabel() { return dataLabel_; }

  //---

  CQChartsGeom::BBox annotationBBox() const override;

  //---

  void addProperties() override;

  void calcRange() override;

  void updateObjs() override;

  bool initObjs() override;

  //---

  QString valueStr(double v) const;

  void addKeyItems(CQChartsPlotKey *key) override;

   //---

  bool isSetHidden  (int i) const;
  bool isValueHidden(int i) const;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  double getPanX(bool is_shift) const override;
  double getPanY(bool is_shift) const override;

  //---

 public slots:
  // set horizontal
  void setHorizontal(bool b) override;

  // set stacked
  void setStacked(bool b);

  // set range bar
  void setRangeBar(bool b);

  // set percent
  void setPercent(bool b);

  // set dot lines
  void setDotLines(bool b);

 private:
  void addRow(const ModelVisitor::VisitData &data);

  void addRowColumn(const ModelVisitor::VisitData &data, const Columns &valueColumns);

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
  CQChartsBarChartValueSet *groupValueSet(int groupId);

 private:
  CQChartsColumn    nameColumn_;               // name column
  CQChartsColumn    labelColumn_;              // data label column
  bool              stacked_        { false }; // stacked bars
  bool              percent_        { false }; // percent values
  bool              rangeBar_       { false }; // bar of value range
  bool              colorBySet_     { false }; // color bars by set or value
  bool              dotLines_       { false }; // show dot lines
  CQChartsLength    dotLineWidth_   { "3px" }; // dot line width
  CQChartsDataLabel dataLabel_;                // data label data
  ValueSets         valueSets_;                // value sets
  ValueGroupInd     valueGroupInd_;            // group ind to value index map
  int               numVisible_     { 0 };     // number of visible bars
  mutable double    barWidth_       { 1.0 };   // bar width
};

#endif
