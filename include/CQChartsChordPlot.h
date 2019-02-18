#ifndef CQChartsChordPlot_H
#define CQChartsChordPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <QModelIndex>

class CQChartsRotatedTextBoxObj;

//---

class CQChartsChordPlotType : public CQChartsPlotType {
 public:
  CQChartsChordPlotType();

  QString name() const override { return "chord"; }
  QString desc() const override { return "Chord"; }

  Dimension dimension() const override { return Dimension::NONE; }

  void addParameters() override;

  bool customXRange() const override { return false; }
  bool customYRange() const override { return false; }

  bool hasAxes() const override { return false; }
  bool hasKey () const override { return false; }

  QString description() const override;

  bool isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                            CQChartsPlotParameter *parameter) const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsChordPlot;

//---

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
    double  value { -1 };

    Group(const QString &str="", double value=-1) :
     str(str), value(value) {
    }
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

    CQChartsChordData *th = const_cast<CQChartsChordData *>(this);

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

  void setAngles(double a, double da) {
    a_  = a;
    da_ = da;
  }

  double angle() const { return a_; }

  double dangle() const { return da_; }

  void sort() {
    std::sort(values_.begin(), values_.end(),
      [](const Value &lhs, const Value &rhs) {
        return lhs.value < rhs.value;
      });
  }

 private:
  int         from_       { -1 };
  QString     name_;
  Group       group_;
  Values      values_;
  QModelIndex ind_;
  double      a_          { 0.0 };
  double      da_         { 0.0 };
  double      total_      { 0.0 };
  double      totalValid_ { false };
};

//---

class CQChartsChordObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsChordObj(const CQChartsChordPlot *plot, const CQChartsGeom::BBox &rect,
                   const CQChartsChordData &data, int i, int n);

  const CQChartsChordData &data() { return data_; }

  int i() const { return i_; }
  int n() const { return n_; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

  void drawFg(QPainter *painter) const override;

  CQChartsGeom::BBox textBBox() const;

 private:
  CQChartsChordObj *plotObject(int ind) const;

  void valueAngles(int ind, double &a, double &da) const;

 private:
  const CQChartsChordPlot* plot_ { nullptr };
  CQChartsChordData        data_;
  int                      i_    { 0 };
  int                      n_    { 1 };
};

//---

class CQChartsChordPlot : public CQChartsPlot,
 public CQChartsObjStrokeData<CQChartsChordPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn linkColumn  READ linkColumn  WRITE setLinkColumn )
  Q_PROPERTY(CQChartsColumn valueColumn READ valueColumn WRITE setValueColumn)
  Q_PROPERTY(CQChartsColumn groupColumn READ groupColumn WRITE setGroupColumn)

  Q_PROPERTY(bool   sorted      READ isSorted    WRITE setSorted     )
  Q_PROPERTY(double innerRadius READ innerRadius WRITE setInnerRadius)
  Q_PROPERTY(double labelRadius READ labelRadius WRITE setLabelRadius)

  CQCHARTS_STROKE_DATA_PROPERTIES

  Q_PROPERTY(double segmentAlpha READ segmentAlpha WRITE setSegmentAlpha)
  Q_PROPERTY(double arcAlpha     READ arcAlpha     WRITE setArcAlpha    )
  Q_PROPERTY(double gapAngle     READ gapAngle     WRITE setGapAngle    )
  Q_PROPERTY(double startAngle   READ startAngle   WRITE setStartAngle  )

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

  double segmentAlpha() const { return segmentAlpha_; }
  void setSegmentAlpha(double r);

  double arcAlpha() const { return arcAlpha_; }
  void setArcAlpha(double r);

  double gapAngle() const { return gapAngle_; }
  void setGapAngle(double r);

  double startAngle() const { return startAngle_; }
  void setStartAngle(double r);

  //---

  CQChartsRotatedTextBoxObj *textBox() const { return textBox_; }

  //---

  double valueToDegrees(double v) const { return v*valueToDegrees_; }

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  CQChartsGeom::BBox annotationBBox() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void postResize() override;

  //---

 private:
  bool initTableObjs(PlotObjs &objs) const;
  bool initHierObjs(PlotObjs &objs) const;

 private:
  CQChartsColumn             linkColumn_;                 //! link column
  CQChartsColumn             valueColumn_;                //! value column
  CQChartsColumn             groupColumn_;                //! group column
  bool                       sorted_         { false };   //! is sorted
  double                     innerRadius_    { 0.9 };     //! inner radius
  double                     labelRadius_    { 1.1 };     //! label radius
  double                     segmentAlpha_   { 0.7 };     //! segment alpha
  double                     arcAlpha_       { 0.3 };     //! arc alpha
  double                     gapAngle_       { 2.0 };     //! gap angle
  double                     startAngle_     { 90.0 };    //! start angle
  CQChartsRotatedTextBoxObj* textBox_        { nullptr }; //! text box
  double                     valueToDegrees_ { 1.0 };     //! value to degrees scale
};

#endif
