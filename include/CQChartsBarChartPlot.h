#ifndef CQChartsBarChartPlot_H
#define CQChartsBarChartPlot_H

#include <CQChartsBarPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsColor.h>

//---

/*!
 * \brief Bar Chart plot type
 * \ingroup Charts
 */
class CQChartsBarChartPlotType : public CQChartsGroupPlotType {
 public:
  CQChartsBarChartPlotType();

  QString name() const override { return "barchart"; }
  QString desc() const override { return "BarChart"; }

  Dimension dimension() const override { return Dimension::ONE_D; }

  void addParameters() override;

  QString yColumnName() const override { return "value"; }

  bool allowXAxisIntegral() const override { return false; }
  bool allowYAxisIntegral() const override { return false; }

  bool allowXLog() const override { return false; }

  bool canProbe() const override { return true; }

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsBarChartPlot;

/*!
 * brief values for bar
 *
 * values for bar (normal: 1 value : range bar: multiple values)
 * \todo handle range bar in distribution plot
 */
class CQChartsBarChartValue {
 public:
  using NameValues = std::map<QString,QString>;

 public:
  struct ValueInd {
    double      value { 0.0 };
    QModelIndex ind;
    int         vrow  { -1 };
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
  ValueInds  valueInds_;  //!< value indices
  QString    valueName_;  //!< value name
  QString    groupName_;  //!< group name
  NameValues nameValues_; //!< name values
};

//------

/*!
 * \brief set of value bars for group
 * \ingroup Charts
 */
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
  QString name_;            //!< group name
  int     ind_      { 0 };  //!< index
  int     groupInd_ { -1 }; //!< group ind
  Values  values_;          //!< value bars
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
  CQChartsBarChartObj(const CQChartsBarChartPlot *plot, const CQChartsGeom::BBox &rect,
                      const ColorInd &iset, const ColorInd &ival, const ColorInd &isval,
                      const QModelIndex &ind);

  QString typeName() const override { return "bar"; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  QString groupStr() const;
  QString nameStr () const;
  QString valueStr() const;

  //---

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &color) { color_ = color; }

  CQChartsGeom::BBox dataLabelRect() const;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  void getSelectIndices(Indices &inds) const override;

  //---

  void draw(CQChartsPaintDevice *device) override;

  void drawFg(CQChartsPaintDevice *device) const override;

  QColor calcBarColor() const;

  //---

  const CQChartsBarChartValue *value() const;

 private:
  const CQChartsBarChartPlot* plot_  { nullptr }; //!< parent plot
  CQChartsColor               color_;             //!< custom color
};

//---

#include <CQChartsKey.h>

/*!
 * \brief Bar Chart Plot Key Color Box
 * \ingroup Charts
 */
class CQChartsBarKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

  Q_PROPERTY(CQChartsColor color READ color WRITE setColor)

 public:
  CQChartsBarKeyColor(CQChartsBarChartPlot *plot, const ColorInd &is, const ColorInd &ig,
                      const ColorInd &iv);

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &color) { color_ = color; }

  bool selectPress(const CQChartsGeom::Point &p, CQChartsSelMod selMod) override;

  QBrush fillBrush() const override;

  bool tipText(const CQChartsGeom::Point &p, QString &tip) const override;

  // get/set hidden
  bool isSetHidden() const;
  void setSetHidden(bool b);

 private:
  CQChartsBarChartPlot* plot_  { nullptr }; //!< plot
  CQChartsColor         color_;             //!< custom color
};

/*!
 * \brief Bar Chart Key Text
 * \ingroup Charts
 */
class CQChartsBarKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsBarKeyText(CQChartsBarChartPlot *plot, const QString &text, const ColorInd &ic);

  QColor interpTextColor(const ColorInd &ind) const override;

  bool isSetHidden() const;
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

  Q_PROPERTY(bool percent    READ isPercent    WRITE setPercent   )
  Q_PROPERTY(bool colorBySet READ isColorBySet WRITE setColorBySet)

  // dot line
  Q_PROPERTY(bool           dotLines     READ isDotLines   WRITE setDotLines    )
  Q_PROPERTY(CQChartsLength dotLineWidth READ dotLineWidth WRITE setDotLineWidth)

  CQCHARTS_NAMED_POINT_DATA_PROPERTIES(Dot,dot)

  Q_ENUMS(PlotType)
  Q_ENUMS(ValueType)

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
    MEAN
  };

 public:
  CQChartsBarChartPlot(CQChartsView *view, const ModelP &model);
 ~CQChartsBarChartPlot();

  //---

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumn &labelColumn() const { return labelColumn_; }
  void setLabelColumn(const CQChartsColumn &c);

  //---

  PlotType plotType() const { return plotType_; }

  bool isNormal () const { return (plotType() == PlotType::NORMAL ); }
  bool isStacked() const { return (plotType() == PlotType::STACKED); }

  ValueType valueType() const { return valueType_; }

  bool isValueValue() const { return (valueType_ == ValueType::VALUE); }
  bool isValueRange() const { return (valueType_ == ValueType::RANGE); }
  bool isValueMin  () const { return (valueType_ == ValueType::MIN  ); }
  bool isValueMax  () const { return (valueType_ == ValueType::MAX  ); }
  bool isValueMean () const { return (valueType_ == ValueType::MEAN ); }

  bool isPercent() const { return percent_; }

  //---

  // when multiple columns and grouped then color by value in value set (group)
  bool isColorBySet() const { return colorBySet_; }
  void setColorBySet(bool b);

  //---

  bool isDotLines() const { return dotLineData_.enabled; }

  const CQChartsLength &dotLineWidth() const { return dotLineData_.width; }
  void setDotLineWidth(const CQChartsLength &l);

  //---

  CQChartsGeom::BBox annotationBBox() const override;

  //---

  void addProperties() override;

  //---

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  QString valueName() const;
  QString valueStr(double v) const;

  CQChartsAxis *mappedXAxis() const override;
  CQChartsAxis *mappedYAxis() const override;

  void addKeyItems(CQChartsPlotKey *key) override;

   //---

  bool isSetHidden  (int i) const;
  bool isValueHidden(int i) const;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  double getPanX(bool is_shift) const override;
  double getPanY(bool is_shift) const override;

 public slots:
  // set plot type
  void setPlotType(PlotType plotType);

  // set normal or stacked
  void setNormal (bool b);
  void setStacked(bool b);

  // set value type
  void setValueType(ValueType valueType);

  void setValueValue(bool b);
  void setValueRange(bool b);
  void setValueMin  (bool b);
  void setValueMax  (bool b);
  void setValueMean (bool b);

  // set percent
  void setPercent(bool b);

  // set dot lines
  void setDotLines(bool b);

 private:
  void addRow(const ModelVisitor::VisitData &data, CQChartsGeom::Range &dataRange) const;

  void addRowColumn(const ModelVisitor::VisitData &data, const CQChartsColumns &valueColumns,
                    CQChartsGeom::Range &dataRange) const;

  void initRangeAxes() const;
  void initRangeAxesI();

  void initObjAxes() const;
  void initObjAxesI();

 private:
  using ValueSets     = std::vector<CQChartsBarChartValueSet>;
  using ValueNames    = std::vector<QString>;
  using ValueGroupInd = std::map<int,int>;

  struct ValueData {
    ValueSets     valueSets;     //!< value sets
    ValueGroupInd valueGroupInd; //!< group ind to value index map

    void clear() {
      valueSets    .clear();
      valueGroupInd.clear();
    }
  };

 public:
  int numValueSets() const { return valueData_.valueSets.size(); }

  const CQChartsBarChartValueSet &valueSet(int i) const {
    assert(i >= 0 && i < int(valueData_.valueSets.size()));
    return valueData_.valueSets[i];
  }

  int numSetValues() const {
    return (! valueData_.valueSets.empty() ? valueData_.valueSets[0].numValues() : 0);
  }

 private:
  void initGroupValueSet() const;

  const CQChartsBarChartValueSet *groupValueSet(int groupId) const;

  CQChartsBarChartValueSet *groupValueSetI(int groupId);

 private:
  struct DotLineData {
    bool           enabled { false }; //!< shown
    CQChartsLength width   { "3px" }; //!< width
  };

  CQChartsColumn nameColumn_;                        //!< name column
  CQChartsColumn labelColumn_;                       //!< data label column
  PlotType       plotType_     { PlotType::NORMAL }; //!< plot type
  ValueType      valueType_    { ValueType::VALUE }; //!< bar value type
  bool           percent_      { false };            //!< percent values
  bool           colorBySet_   { false };            //!< color bars by set or value
  DotLineData    dotLineData_;                       //!< dot line data
  mutable double barWidth_     { 1.0 };              //!< minimum bar width
  ValueData      valueData_;                         //!< value data
};

#endif
