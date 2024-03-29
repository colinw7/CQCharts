#ifndef CQChartsBarChartPlot_H
#define CQChartsBarChartPlot_H

#include <CQChartsBarPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsColor.h>
#include <CSafeIndex.h>

class CQChartsBarChartPlot;
class CQChartsDensity;

//---

/*!
 * \brief Bar Chart plot type
 * \ingroup Charts
 */
class CQChartsBarChartPlotType : public CQChartsGroupPlotType {
 public:
  using BarChartPlot = CQChartsBarChartPlot;

 public:
  CQChartsBarChartPlotType();

  QString name() const override { return "barchart"; }
  QString desc() const override { return "BarChart"; }

  Category category() const override { return Category::ONE_D; }

  void addParameters() override;

  QString yColumnName() const override { return "value"; }

  bool allowXAxisIntegral() const override { return false; }
  bool allowYAxisIntegral() const override { return false; }

  bool allowXLog() const override { return false; }

  bool canProbe() const override { return true; }

  QString description() const override;

  Plot *create(View *view, const ModelP &model) const override;
};

//---

/*!
 * brief values for bar
 *
 * values for bar (normal: 1 value : range bar: multiple values)
 * \todo handle range bar in distribution plot
 */
class CQChartsBarChartValue {
 public:
  using Column = CQChartsColumn;

  struct ColumnValue {
    ColumnValue() = default;

    ColumnValue(const Column &column, const QString &value) :
     column(column), value(value) {
    }

    Column  column;
    QString value;
  };

  using NameColumnValueMap = std::map<QString, ColumnValue>;

 public:
  //! value index
  struct ValueInd {
    double      value { 0.0 };
    QModelIndex ind;
    int         vrow  { -1 };
  };

  using ValueInds = std::vector<ValueInd>;

 public:
  CQChartsBarChartValue() = default;

  void addValueInd(const ValueInd &valueInd) {
    valueInds_.push_back(valueInd);
  }

  const ValueInds &valueInds() const { return valueInds_; }
  const ValueInd &valueInd(uint i) const { return valueInds_[i]; }

  const QString &valueName() const { return valueName_; }
  void setValueName(const QString &s) { valueName_ = s; }

  const QString &groupName() const { return groupName_; }
  void setGroupName(const QString &s) { groupName_ = s; }

  const Column &valueColumn() const { return valueColumn_; }
  void setValueColumn(const Column &v) { valueColumn_ = v; }

  int setId() const { return setId_; }
  void setSetId(int i) { setId_ = i; }

  //---

  const NameColumnValueMap &nameColumnValueMap() const { return nameColumnValueMap_; }
  void setNameColumnValueMap(const NameColumnValueMap &v) { nameColumnValueMap_ = v; }

  void setNameColumnValue(const QString &name, const Column &column, const QString &value) {
    nameColumnValueMap_[name] = ColumnValue(column, value);
  }

  QString getNameValue(const QString &name) const {
    auto p = nameColumnValueMap_.find(name);
    if (p == nameColumnValueMap_.end()) return "";

    return (*p).second.value;
  }

  //---

  void calcRange(ValueInd &minInd, ValueInd &maxInd, double &mean, double &sum) const {
    assert(! valueInds_.empty());

    auto n = valueInds_.size();

    minInd = valueInds_[0];
    maxInd = valueInds_[0];

    sum = valueInds_[0].value;

    for (size_t i = 1; i < n; ++i) {
      double value = valueInds_[i].value;

      if (value < minInd.value)
        minInd = valueInds_[i];

      if (value > maxInd.value)
        maxInd = valueInds_[i];

      sum += value;
    }

    mean = sum/double(n);
  }

 private:
  ValueInds          valueInds_;          //!< value indices
  QString            valueName_;          //!< value name
  QString            groupName_;          //!< group name
  Column             valueColumn_;        //!< value column
  int                setId_ { -1 };       //!< set id
  NameColumnValueMap nameColumnValueMap_; //!< name values
};

//------

/*!
 * \brief set of value bars for group
 * \ingroup Charts
 */
class CQChartsBarChartValueSet {
 public:
  using BarValue = CQChartsBarChartValue;
  using Column   = CQChartsColumn;
  using OptReal  = CQChartsOptReal;
  using Values   = std::vector<BarValue>;

 public:
  CQChartsBarChartValueSet() = default;

  CQChartsBarChartValueSet(const QString &name, int ind, const Column &column) :
   name_(name), ind_(ind), column_(column) {
  }

  const QString &name() const { return name_; }

  int ind() const { return ind_; }

  const Column &column() const { return column_; }

  int groupInd() const { return groupInd_; }
  void setGroupInd(int i) { groupInd_ = i; }

  int numValues() const { return int(values_.size()); }

  const Values &values() const { return values_; }

  void addValue(const BarValue &value) {
    values_.push_back(value);
  }

  const BarValue &value(int i) const { return CUtil::safeIndex(values_, i); }

  bool calcSums(double &posSum, double &negSum) const {
    posSum = 0.0;
    negSum = 0.0;

    if (sumValue_.isSet()) {
      posSum = sumValue_.real();
      return true;
    }

    if (values_.empty()) return false;

    std::vector<double> rvalues;
    getValues(rvalues);

    for (auto &value : rvalues) {
      if (value >= 0) posSum += value;
      else            negSum += value;
    }

    return true;
  }

  bool calcStats(double &min, double &max, double &mean, double &sum) const {
    if (values_.empty()) return false;

    std::vector<double> rvalues;
    getValues(rvalues);

    min  = 0.0;
    max  = 0.0;
    mean = 0.0;
    sum  = 0.0;

    int n = 0;

    for (auto &value : rvalues) {
      if (n == 0) {
        min = value;
        max = value;
      }
      else {
        min = std::min(min, value);
        max = std::max(max, value);
      }

      sum += value;

      ++n;
    }

    mean = (n > 0 ? sum/n : 0.0);

    return true;
  }

  void getValues(std::vector<double> &rvalues) const {
    for (auto &v : values_) {
      for (auto &vi : v.valueInds()) {
        double value = vi.value;

        rvalues.push_back(value);
      }
    }
  }

  const OptReal &minValue() const { return minValue_; }
  void setMinValue(const OptReal &r) { minValue_ = r; }

  const OptReal &maxValue() const { return maxValue_; }
  void setMaxValue(const OptReal &r) { maxValue_ = r; }

  const OptReal &sumValue() const { return sumValue_; }
  void setSumValue(const OptReal &r) { sumValue_ = r; }

 private:
  QString name_;            //!< group name
  int     ind_      { 0 };  //!< index
  Column  column_;          //!< column
  int     groupInd_ { -1 }; //!< group ind
  Values  values_;          //!< value bars
  OptReal minValue_;        //!< custom min value
  OptReal maxValue_;        //!< custom max value
  OptReal sumValue_;        //!< custom value sum
};

//------

/*!
 * \brief Bar Chart Bar object
 * \ingroup Charts
 */
class CQChartsBarChartObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString       group READ groupStr)
  Q_PROPERTY(QString       name  READ nameStr )
  Q_PROPERTY(QString       value READ valueStr)
  Q_PROPERTY(CQChartsColor color READ color    WRITE setColor)

 public:
  using BarChartPlot = CQChartsBarChartPlot;
  using ValueSet     = CQChartsBarChartValueSet;
  using BarValue     = CQChartsBarChartValue;
  using Columns      = CQChartsColumns;
  using Column       = CQChartsColumn;

 public:
  CQChartsBarChartObj(const BarChartPlot *plot, const BBox &rect, bool isValueSet, int valueSetInd,
                      const Column &column, const ColorInd &iset, const ColorInd &ival,
                      const ColorInd &isval, const QModelIndices &inds,
                      bool singleValue, int ivalueSetInd);

 ~CQChartsBarChartObj();

  QString typeName() const override { return "bar"; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  QString groupStr(bool tip=false) const;
  QString nameStr (bool tip=false) const;
  QString valueStr(bool tip=false) const;

  //---

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &color) { color_ = color; }

  BBox dataLabelRect() const;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  void getObjSelectIndices(Indices &inds) const override;

  void calcValueColumns(Column &column, Columns &columns) const;

  //---

  void draw(PaintDevice *device) const override;

  void drawFg(PaintDevice *device) const override;

  //---

  bool isValueSet() const { return valueSet_; }
  int  valueSetInd() const { return valueSetInd_; }

  const Column &column() const { return column_; }

  const BarValue *value() const;

  const ValueSet *valueSet() const;

 private:
  void drawShape(PaintDevice *device, const BBox &bbox) const;

  void drawRect   (PaintDevice *device, const BBox &bbox,
                   const CQChartsPenBrush &barPenBrush) const;
  void drawDotLine(PaintDevice *device, const BBox &bbox,
                   const CQChartsPenBrush &barPenBrush) const;
  void drawBox    (PaintDevice *device, const BBox &bbox) const;
  void drawScatter(PaintDevice *device, const BBox &bbox) const;
  void drawViolin (PaintDevice *device, const BBox &bbox) const;

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const override;

  QColor calcBarColor() const;

 private:
  using DensityP = std::unique_ptr<CQChartsDensity>;

  const BarChartPlot* barChartPlot_ { nullptr }; //!< parent plot

  // custom style
  CQChartsColor color_; //!< custom color

  // value set
  bool valueSet_    { false }; //!< is value set
  int  valueSetInd_ { -1 };    //!< value set ind in all value sets

  // indiviual value
  Column column_;                //!< value column
  bool   singleValue_ { false }; //!< is single value
  int    ivalueSetInd_ { -1 };   //!< value ind in value set

  // density data (for violin)
  mutable DensityP density_; //!< density data
};

//---

#include <CQChartsKey.h>

/*!
 * \brief Bar Chart Plot Key Color Box
 * \ingroup Charts
 */
class CQChartsBarColorKeyItem : public CQChartsColorBoxKeyItem {
  Q_OBJECT

  Q_PROPERTY(CQChartsColor color READ color WRITE setColor)

 public:
  using BarChartPlot = CQChartsBarChartPlot;

 public:
  CQChartsBarColorKeyItem(BarChartPlot *plot, const QString &name, const ColorInd &is,
                          const ColorInd &ig, const ColorInd &iv);

  QBrush fillBrush() const override;
  QPen   strokePen() const override;

  bool tipText(const Point &p, QString &tip) const override;

  ColorInd calcColorInd() const override;

 private:
  BarChartPlot* barChartPlot_ { nullptr }; //!< plot
  QString       name_;                     //!< item name
};

/*!
 * \brief Bar Chart Key Text
 * \ingroup Charts
 */
class CQChartsBarTextKeyItem : public CQChartsTextKeyItem {
  Q_OBJECT

 public:
  using BarChartPlot = CQChartsBarChartPlot;

 public:
  CQChartsBarTextKeyItem(BarChartPlot *plot, const QString &text, const ColorInd &ic);

  QColor interpTextColor(const ColorInd &ind) const override;

  //bool isSetHidden() const override;
  ColorInd setIndex() const override;

 private:
  BarChartPlot* barChartPlot_ { nullptr }; //!< plot
};

//---

/*!
 * \brief bar chart plot
 * \ingroup Charts
 *
 * columns:
 *   + x     : name
 *   + y     : value(s)
 *   + group : group(s)
 *   + bar   : custom color, stacked, percent, range, horizontal, margin, stroke, fill
 *
 * Plot Type
 *   + \ref CQChartsBarChartPlotType
 *
 * Example
 *   + \image html barchart.png
 */
class CQChartsBarChartPlot : public CQChartsBarPlot,
 public CQChartsObjDotPointData<CQChartsBarChartPlot> {
  Q_OBJECT

  // style
  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(CQChartsColumn labelColumn READ labelColumn WRITE setLabelColumn)

  // options
  Q_PROPERTY(PlotType  plotType  READ plotType  WRITE setPlotType )
  Q_PROPERTY(ValueType valueType READ valueType WRITE setValueType)
  Q_PROPERTY(ShapeType shapeType READ shapeType WRITE setShapeType)

  Q_PROPERTY(bool percent       READ isPercent       WRITE setPercent      )
  Q_PROPERTY(bool skipEmpty     READ isSkipEmpty     WRITE setSkipEmpty    )
  Q_PROPERTY(bool groupByColumn READ isGroupByColumn WRITE setGroupByColumn)
  Q_PROPERTY(bool colorBySet    READ isColorBySet    WRITE setColorBySet   )
  Q_PROPERTY(bool sortSets      READ isSortSets      WRITE setSortSets     )

  // dot line
  Q_PROPERTY(CQChartsLength dotLineWidth READ dotLineWidth WRITE setDotLineWidth)

  CQCHARTS_NAMED_POINT_DATA_PROPERTIES(Dot, dot)

  Q_ENUMS(PlotType)
  Q_ENUMS(ValueType)
  Q_ENUMS(ShapeType)

 public:
  enum class PlotType {
    NORMAL,
    STACKED,
  };

  enum class ValueType {
    VALUE,
    RANGE,
    MIN,
    MAX,
    MEAN,
    SUM
  };

  enum class ShapeType {
    RECT,
    DOT_LINE,
    BOX,
    SCATTER,
    VIOLIN
  };

  using BarValue = CQChartsBarChartValue;
  using Symbol   = CQChartsSymbol;
  using Length   = CQChartsLength;
  using Color    = CQChartsColor;
  using ColorInd = CQChartsUtil::ColorInd;

 public:
  CQChartsBarChartPlot(View *view, const ModelP &model);
 ~CQChartsBarChartPlot();

  //---

  void init() override;
  void term() override;

  //---

  const Column &nameColumn() const { return nameColumn_; }
  void setNameColumn(const Column &c);

  const Column &labelColumn() const { return labelColumn_; }
  void setLabelColumn(const Column &c);

  //---

  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  Columns getNamedColumns(const QString &name) const override;
  void setNamedColumns(const QString &name, const Columns &c) override;

  //---

  Columns calcValueColumns() const;

  bool calcGroupByColumn() const;
  bool calcColorBySet() const;

  //---

  bool isLabelsVisible() const;
  void setLabelsVisible(bool b);

  CQChartsLabelPosition labelPosition() const;

  //---

  PlotType plotType() const { return plotType_; }

  bool isNormal () const { return (plotType() == PlotType::NORMAL ); }
  bool isStacked() const { return (plotType() == PlotType::STACKED); }

  //---

  ValueType valueType() const { return valueType_; }

  bool isValueValue() const { return (valueType_ == ValueType::VALUE); }
  bool isValueRange() const { return (valueType_ == ValueType::RANGE); }
  bool isValueMin  () const { return (valueType_ == ValueType::MIN  ); }
  bool isValueMax  () const { return (valueType_ == ValueType::MAX  ); }
  bool isValueMean () const { return (valueType_ == ValueType::MEAN ); }
  bool isValueSum  () const { return (valueType_ == ValueType::SUM  ); }

  //---

  ShapeType shapeType() const { return shapeType_; }

  //---

  bool isPercent  () const { return percent_  ; }
  bool isSkipEmpty() const { return skipEmpty_; }

  //---

  bool isGroupByColumn() const { return groupByColumn_; }
  void setGroupByColumn(bool b);

  // when multiple columns and grouped then color by value in value set (group)
  bool isColorBySet() const { return colorBySet_; }
  void setColorBySet(bool b);

  bool isSortSets() const { return sortSets_; }
  void setSortSets(bool b);

  bool isGroupKey  () const { return isGroupKey_; }
  bool isColumnsKey() const { return isColumnsKey_; }

  //---

  const Length &dotLineWidth() const { return dotLineData_.width; }
  void setDotLineWidth(const Length &l);

  //---

  bool calcStacked() const;
  bool calcPercent() const;

  //---

  BBox calcExtraFitBBox() const override;

  //---

  void addProperties() override;

  //---

  Range calcRange() const override;

  void postCalcRange() override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  QString valueName() const;
  QString valueStr(double v) const;

  Axis *mappedXAxis() const override;
  Axis *mappedYAxis() const override;

  void addKeyItems(PlotKey *key) override;

  //---

  bool isSetHidden(int i) const;

  bool isValueHidden(const BarValue &value, int i) const;

  //---

  bool addMenuItems(QMenu *menu, const Point &p) override;

  //---

  double getPanX(bool is_shift) const override;
  double getPanY(bool is_shift) const override;

 public Q_SLOTS:
  // set plot type
  void setPlotType(PlotType plotType);

  // set normal or stacked
  void setNormal (bool b);
  void setStacked(bool b);

  //---

  // set value type
  void setValueType(ValueType valueType);

  void setValueValue(bool b);
  void setValueRange(bool b);
  void setValueMin  (bool b);
  void setValueMax  (bool b);
  void setValueMean (bool b);
  void setValueSum  (bool b);

  //---

  void setShapeType(ShapeType shapeType);

  //---

  // set percent
  void setPercent(bool b);

  // set skip empty
  void setSkipEmpty(bool b);

 protected:
  struct RangeData {
    OptReal minValue;
    OptReal maxValue;
    OptReal sumValue;
    Range   dataRange;
  };

  void addRow(const ModelVisitor::VisitData &data, RangeData &rangeData) const;

  void addRowColumn(const ModelVisitor::VisitData &data, const Columns &valueColumns,
                    const Column &valueColumn, RangeData &rangeData, int columnInd, int ic) const;

  void initRangeAxes() const;
  void initRangeAxesI();

  void initObjAxes() const;
  void initObjAxesI();

  void addKeyItems1(PlotKey *key);

 protected:
  using ValueSet      = CQChartsBarChartValueSet;
  using ValueSets     = std::vector<ValueSet>;
  using ValueInds     = std::map<QVariant, int>;
  using ValueNames    = std::vector<QString>;
  using ValueGroupInd = std::map<int, int>;
  using SortedInds    = std::vector<int>;

  struct ValueData {
    ValueSets     valueSets;     //!< value sets
    ValueInds     valueInds;     //!< value inds
    ValueGroupInd valueGroupInd; //!< group ind to value index map
    SortedInds    sortedInds;    //!< sorted value inds (by group value)

    void clear() {
      valueSets    .clear();
      valueInds    .clear();
      valueGroupInd.clear();
      sortedInds   .clear();
    }
  };

 public:
  int calcNumSets() const;

  int numValueSets() const;

  const ValueSet &valueSet(int i) const;

  int numSetValues() const;

 protected:
  void initGroupValueSet() const;

  const ValueSet *groupValueSet(int groupId, int columnInd=-1) const;

  ValueSet *groupValueSetI(int groupId, int columnInd=-1);

  using BarObj = CQChartsBarChartObj;

  //---

  bool hasForeground() const override;

  void execDrawForeground(PaintDevice *) const override;

  //---

  virtual BarObj *createBarObj(const BBox &rect, bool isValueSet, int valueSetInd,
                               const Column &column, const ColorInd &is, const ColorInd &ig,
                               const ColorInd &iv, const QModelIndices &inds,
                               bool singleValue, int ivalueSetInd) const;

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 protected:
  struct DotLineData {
    bool   enabled { false };            //!< shown
    Length width   { Length::pixel(3) }; //!< width
  };

 private:
  // columns
  Column nameColumn_;  //!< name column
  Column labelColumn_; //!< data label column

  // options
  PlotType  plotType_  { PlotType::NORMAL }; //!< plot type
  ValueType valueType_ { ValueType::VALUE }; //!< bar value type
  ShapeType shapeType_ { ShapeType::RECT };  //!< bar object shape type

  bool percent_       { false }; //!< percent values
  bool skipEmpty_     { false }; //!< skip empty groups
  bool groupByColumn_ { false }; //!< group by column when multiple columns
  bool colorBySet_    { false }; //!< color bars by set or value
  bool sortSets_      { true };  //!< sort sets by value

  DotLineData    dotLineData_;          //!< dot line data
  mutable double barWidth_     { 1.0 }; //!< minimum bar width
  ValueData      valueData_;            //!< value data

  bool isGroupKey_   { false }; //!< is group key (groupByColumn, colorBySet)
  bool isColumnsKey_ { false }; //!< is columns key (! groupByColumn, multiple Columns, colorBySet)
};

//---

#include <CQChartsGroupPlotCustomControls.h>

/*!
 * \brief Bar Chart plot custom controls
 * \ingroup Charts
 */
class CQChartsBarChartPlotCustomControls : public CQChartsGroupPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsBarChartPlotCustomControls(CQCharts *charts);

  void init() override;

  void setPlot(Plot *plot) override;

 public Q_SLOTS:
  void updateWidgets() override;

 protected:
  void addWidgets() override;

  void addColumnWidgets() override;

  virtual void addLabelGroup();

  void addOptionsWidgets() override;

  void connectSlots(bool b) override;

  //---

  CQChartsColor getColorValue() override;
  void setColorValue(const CQChartsColor &c) override;

 protected Q_SLOTS:
  void labelColumnSlot();
  void labelVisibleSlot(int);

  void orientationSlot();
  void plotTypeSlot();
  void valueTypeSlot();
  void shapeTypeSlot();

  void percentSlot();
  void skipEmptySlot();
  void groupByColumnSlot();
  void colorBySetSlot();

 protected:
  CQChartsBarChartPlot* barChartPlot_ { nullptr };

  FrameData optionsFrame_;

  FrameData                  labelFrame_;
  QCheckBox*                 labelCheck_         { nullptr };
  CQChartsColumnCombo*       labelColumnCombo_   { nullptr };
  CQChartsEnumParameterEdit* orientationCombo_   { nullptr };
  CQChartsEnumParameterEdit* plotTypeCombo_      { nullptr };
  CQChartsEnumParameterEdit* valueTypeCombo_     { nullptr };
  CQChartsEnumParameterEdit* shapeTypeCombo_     { nullptr };
  CQChartsBoolParameterEdit* percentCheck_       { nullptr };
  CQChartsBoolParameterEdit* skipEmptyCheck_     { nullptr };
  CQChartsBoolParameterEdit* groupByColumnCheck_ { nullptr };
  CQChartsBoolParameterEdit* colorBySetCheck_    { nullptr };
};

#endif
