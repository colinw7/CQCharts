#ifndef CQChartsChordPlot_H
#define CQChartsChordPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsTextBoxObj.h>
#include <QModelIndex>

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
  void setName(const QString &v) { name_ = v; }

  const Group &group() const { return group_; }
  void setGroup(const Group &group) { group_ = group; }

  const Values &values() const { return values_; }

  void addValue(int to, double value) {
    values_.emplace_back(to, value);

    totalValid_ = false;
  }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &v) { ind_ = v; }

  double total() const {
    if (totalValid_)
      return total_;

    total_      = calcTotal();
    totalValid_ = true;

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
  int         from_;
  QString     name_;
  Group       group_;
  Values      values_;
  QModelIndex ind_;
  double      a_  { 0 };
  double      da_ { 0 };

  mutable double total_ { 0.0 };
  mutable double totalValid_ { false };
};

//---

class CQChartsChordObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsChordObj(CQChartsChordPlot *plot, const CQChartsGeom::BBox &rect,
                   const CQChartsChordData &data, int i, int n);

  const CQChartsChordData &data() { return data_; }

  int i() const { return i_; }
  int n() const { return n_; }

  QString calcId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void mousePress(const CQChartsGeom::Point &) override;

  bool isIndex(const QModelIndex &ind) const override;

  void draw(CQChartsRenderer *renderer, const CQChartsPlot::Layer &) override;

 private:
  CQChartsChordObj *plotObject(int ind) const;

  void valueAngles(int ind, double &a, double &da) const;

 private:
  CQChartsChordPlot* plot_ { nullptr };
  CQChartsChordData  data_;
  int                i_    { 0 };
  int                n_    { 1 };
};

//---

class CQChartsChordPlotType : public CQChartsPlotType {
 public:
  CQChartsChordPlotType();

  QString name() const override { return "chord"; }
  QString desc() const override { return "Chord"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsChordPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int     nameColumn  READ nameColumn     WRITE setNameColumn    )
  Q_PROPERTY(int     groupColumn READ groupColumn    WRITE setGroupColumn   )
  Q_PROPERTY(bool    sorted      READ isSorted       WRITE setSorted        )
  Q_PROPERTY(double  innerRadius READ innerRadius    WRITE setInnerRadius   )
  Q_PROPERTY(double  labelRadius READ labelRadius    WRITE setLabelRadius   )
  Q_PROPERTY(QString borderColor READ borderColorStr WRITE setBorderColorStr)
  Q_PROPERTY(double  arcAlpha    READ arcAlpha       WRITE setArcAlpha      )

 public:
  CQChartsChordPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsChordPlot();

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; updateRangeAndObjs(); }

  int groupColumn() const { return groupColumn_; }
  void setGroupColumn(int i) { groupColumn_ = i; updateRangeAndObjs(); }

  //---

  bool isSorted() const { return sorted_; }
  void setSorted(bool b) { sorted_ = b; updateObjs(); }

  double innerRadius() const { return innerRadius_; }
  void setInnerRadius(double r) { innerRadius_ = r; update(); }

  double labelRadius() const { return labelRadius_; }
  void setLabelRadius(double r) { labelRadius_ = r; update(); }

  QString borderColorStr() const { return borderColor_.colorStr(); }
  void setBorderColorStr(const QString &s) { borderColor_.setColorStr(s); }

  QColor interpBorderColor(int i, int n) const;

  double arcAlpha() const { return arcAlpha_; }
  void setArcAlpha(double r) { arcAlpha_ = r; update(); }

  CQChartsRotatedTextBoxObj *textBox() const { return textBox_; }

  //---

  double valueToDegrees(double v) const { return v*valueToDegrees_; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  bool initObjs() override;

  //---

  void handleResize() override;

  void draw(CQChartsRenderer *) override;

 private:
  int                        nameColumn_     { -1 };
  int                        groupColumn_    { -1 };
  bool                       sorted_         { false };
  double                     innerRadius_    { 0.9 };
  double                     labelRadius_    { 1.1 };
  CQChartsPaletteColor       borderColor_;
  double                     arcAlpha_       { 0.3 };
  CQChartsRotatedTextBoxObj* textBox_;
  double                     valueToDegrees_ { 1.0 };
};

#endif
