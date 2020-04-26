#ifndef CQChartsChordPlot_H
#define CQChartsChordPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <QModelIndex>

class CQChartsRotatedTextBoxObj;

//---

/*!
 * \brief Chord Plot Type
 * \ingroup Charts
 */
class CQChartsChordPlotType : public CQChartsPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsChordPlotType();

  QString name() const override { return "chord"; }
  QString desc() const override { return "Chord"; }

  void addParameters() override;

  bool customXRange() const override { return false; }
  bool customYRange() const override { return false; }

  bool hasAxes() const override { return false; }
  bool hasKey () const override { return false; }

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool canProbe() const override { return false; }

  QString description() const override;

  //---

  bool isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                            CQChartsPlotParameter *parameter) const override;

  void analyzeModel(CQChartsModelData *modelData,
                    CQChartsAnalyzeModelData &analyzeModelData) override;

  //---

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsChordPlot;

//---

/*!
 * \brief Chord Data
 * \ingroup Charts
 */
class CQChartsChordData {
 public:
  struct Value {
    int    to    { -1 };
    double value { 0.0 };

    Value(int to=-1, double value=0.0) :
     to(to), value(value) {
    }
  };

  struct Group {
    QString str;
    int     i { 0 };
    int     n { 0 };

    Group(const QString &str="", int i=0, int n=1) :
     str(str), i(i), n(n) {
    }

    double value() const { return (n > 0 ? double(i)/n : 0.0); }
  };

  using Values = std::vector<Value>;

 public:
  CQChartsChordData() { }

  int from() const { return from_; }
  void setFrom(int i) { from_ = i; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  const Group &group() const { return group_; }
  void setGroup(const Group &group) { group_ = group; }

  const Values &values() const { return values_; }

  void addValue(int to, double value) {
    values_.emplace_back(to, value);

    totalValid_ = false;
  }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &i) { ind_ = i; }

  double total() const {
    if (totalValid_)
      return total_;

    auto *th = const_cast<CQChartsChordData *>(this);

    th->total_      = calcTotal();
    th->totalValid_ = true;

    return total_;
  }

  double calcTotal() const {
    double total = 0.0;

    for (auto &value : values_)
      total += value.value;

    return total;
  }

  void setAngles(const CQChartsAngle &a, const CQChartsAngle &da) {
    a_  = a;
    da_ = da;
  }

  const CQChartsAngle &angle() const { return a_; }

  const CQChartsAngle &dangle() const { return da_; }

  void sort() {
    std::sort(values_.begin(), values_.end(),
      [](const Value &lhs, const Value &rhs) {
        return lhs.value < rhs.value;
      });
  }

 private:
  int           from_       { -1 };    //!< from node
  QString       name_;                 //!< value name
  Group         group_;                //!< group
  Values        values_;               //!< connection values
  QModelIndex   ind_;                  //!< model index
  CQChartsAngle a_;                    //!< start angle
  CQChartsAngle da_;                   //!< delta angle
  double        total_      { 0.0 };   //!< value total
  double        totalValid_ { false }; //!< is total valid
};

//---

/*!
 * \brief Chord Plot object
 * \ingroup Charts
 */
class CQChartsChordObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsChordObj(const CQChartsChordPlot *plot, const CQChartsGeom::BBox &rect,
                   const CQChartsChordData &data, const ColorInd &ig, const ColorInd &iv);

  const CQChartsChordData &data() { return data_; }

  //---

  QString typeName() const override { return "chord"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isArc() const override { return true; }
  CQChartsArcData arcData() const override;

  //---

  double innerRadius() const;
  double outerRadius() const;

  //---

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  //---

  void draw(CQChartsPaintDevice *device) override;

  void drawFg(CQChartsPaintDevice *device) const override;

  //---

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const;

  void calcArcPenBrush(CQChartsChordObj *toObj, CQChartsPenBrush &penBrush) const;

  QColor calcFromColor() const;

  //---

  void writeScriptData(CQChartsScriptPainter *device) const override;

  //---

  CQChartsGeom::BBox textBBox() const;

 private:
  CQChartsChordObj *plotObject(int ind) const;

  void valueAngles(int ind, double &a, double &da) const;

 private:
  const CQChartsChordPlot* plot_ { nullptr }; //!< parent plot
  CQChartsChordData        data_;             //!< chord data
};

//---

/*!
 * \brief Chord Plot
 * \ingroup Charts
 *
 * columns:
 *   + link  : link
 *   + value : value
 *   + group : group
 *
 * Plot Type
 *   + \ref CQChartsChordPlotType
 *
 * Example
 *   + \image html chord_plot.png
 */
class CQChartsChordPlot : public CQChartsPlot,
 public CQChartsObjStrokeData<CQChartsChordPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn linkColumn  READ linkColumn  WRITE setLinkColumn )
  Q_PROPERTY(CQChartsColumn valueColumn READ valueColumn WRITE setValueColumn)
  Q_PROPERTY(CQChartsColumn groupColumn READ groupColumn WRITE setGroupColumn)

  // options
  Q_PROPERTY(bool   sorted      READ isSorted    WRITE setSorted     )
  Q_PROPERTY(double innerRadius READ innerRadius WRITE setInnerRadius)
  Q_PROPERTY(double labelRadius READ labelRadius WRITE setLabelRadius)

  // stroke
  CQCHARTS_STROKE_DATA_PROPERTIES

  // style
  Q_PROPERTY(CQChartsAlpha segmentAlpha READ segmentAlpha WRITE setSegmentAlpha)
  Q_PROPERTY(CQChartsAlpha arcAlpha     READ arcAlpha     WRITE setArcAlpha    )
  Q_PROPERTY(CQChartsAngle gapAngle     READ gapAngle     WRITE setGapAngle    )
  Q_PROPERTY(CQChartsAngle startAngle   READ startAngle   WRITE setStartAngle  )

 public:
  CQChartsChordPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsChordPlot();

  const CQChartsColumn &linkColumn() const { return linkColumn_; }
  void setLinkColumn(const CQChartsColumn &c);

  const CQChartsColumn &valueColumn() const { return valueColumn_; }
  void setValueColumn(const CQChartsColumn &c);

  const CQChartsColumn &groupColumn() const { return groupColumn_; }
  void setGroupColumn(const CQChartsColumn &c);

  //---

  bool isSorted() const { return sorted_; }
  void setSorted(bool b);

  double innerRadius() const { return innerRadius_; }
  void setInnerRadius(double r);

  double labelRadius() const { return labelRadius_; }
  void setLabelRadius(double r);

  //---

  const CQChartsAlpha &segmentAlpha() const { return segmentAlpha_; }
  void setSegmentAlpha(const CQChartsAlpha &a);

  const CQChartsAlpha &arcAlpha() const { return arcAlpha_; }
  void setArcAlpha(const CQChartsAlpha &a);

  const CQChartsAngle &gapAngle() const { return gapAngle_; }
  void setGapAngle(const CQChartsAngle &a);

  const CQChartsAngle &startAngle() const { return startAngle_; }
  void setStartAngle(const CQChartsAngle &a);

  //---

  CQChartsRotatedTextBoxObj *textBox() const { return textBox_; }

  //---

  double valueToDegrees(double v) const { return v*valueToDegrees_; }

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  CQChartsGeom::BBox calcAnnotationBBox() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void postResize() override;

  //---

  void write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
             const QString &viewVarName) const override;

 private:
  bool initTableObjs(PlotObjs &objs) const;
  bool initHierObjs(PlotObjs &objs) const;

 private:
  CQChartsColumn             linkColumn_;                 //!< link column
  CQChartsColumn             valueColumn_;                //!< value column
  CQChartsColumn             groupColumn_;                //!< group column
  bool                       sorted_         { false };   //!< is sorted
  double                     innerRadius_    { 0.9 };     //!< inner radius
  double                     labelRadius_    { 1.1 };     //!< label radius
  CQChartsAlpha              segmentAlpha_   { 0.7 };     //!< segment alpha
  CQChartsAlpha              arcAlpha_       { 0.3 };     //!< arc alpha
  CQChartsAngle              gapAngle_       { 2.0 };     //!< gap angle
  CQChartsAngle              startAngle_     { 90.0 };    //!< start angle
  CQChartsRotatedTextBoxObj* textBox_        { nullptr }; //!< text box
  double                     valueToDegrees_ { 1.0 };     //!< value to degrees scale
};

#endif
