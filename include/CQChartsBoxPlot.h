#ifndef CQChartsBoxPlot_H
#define CQChartsBoxPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsUtil.h>

#include <CQChartsBoxWhisker.h>
#include <map>

class CQChartsBoxPlot;

struct CQChartsBoxPlotValue {
  CQChartsBoxPlotValue() = default;

  CQChartsBoxPlotValue(double value, const QModelIndex &ind) :
   value(value), ind(ind) {
  }

  operator double() const { return value; }

  double      value { 0.0 };
  QModelIndex ind;
};

using CQChartsBoxPlotWhisker = CQChartsBoxWhiskerT<CQChartsBoxPlotValue>;

// box plot object
class CQChartsBoxPlotObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxPlotObj(CQChartsBoxPlot *plot, const CBBox2D &rect, double pos,
                     const CQChartsBoxPlotWhisker &whisker, int i, int n);

  void mousePress(const CPoint2D &) override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsBoxPlot*       plot_ { nullptr };
  double                 pos_  { 0.0 };
  CQChartsBoxPlotWhisker whisker_;
  int                    i_    { -1 };
  int                    n_    { 0 };
};

//---

#include <CQChartsKey.h>

class CQChartsBoxKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsBoxKeyColor(CQChartsBoxPlot *plot, int i, int n);

  bool mousePress(const CPoint2D &p) override;

  QBrush fillBrush() const override;
};

class CQChartsBoxKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsBoxKeyText(CQChartsBoxPlot *plot, int i, const QString &text);

  QColor textColor() const override;

 private:
  int i_ { 0 };
};

//---

class CQChartsBoxPlotType : public CQChartsPlotType {
 public:
  CQChartsBoxPlotType();

  QString name() const override { return "box"; }
  QString desc() const override { return "BoxPlot"; }

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsBoxPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int    xColumn      READ xColumn      WRITE setXColumn     )
  Q_PROPERTY(int    yColumn      READ yColumn      WRITE setYColumn     )
  Q_PROPERTY(QColor boxColor     READ boxColor     WRITE setBoxColor    )
  Q_PROPERTY(double cornerRadius READ cornerRadius WRITE setCornerRadius)

 public:
  CQChartsBoxPlot(CQChartsView *view, const ModelP &model);

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; update(); }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; update(); }

  const QColor &boxColor() const { return boxColor_; }
  void setBoxColor(const QColor &c) { boxColor_ = c; update(); }

  double cornerRadius() const { return cornerRadius_; }
  void setCornerRadius(double r) { cornerRadius_ = r; update(); }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void initObjs() override;

  void addKeyItems(CQChartsKey *key) override;

  //---

  void draw(QPainter *) override;

 private:
  struct RealCmp {
    bool operator()(const double &lhs, const double &rhs) const {
      if (CQChartsUtil::realEq(lhs, rhs))
        return false;

      return lhs < rhs;
    }
  };

  using Whiskers = std::map<int,CQChartsBoxPlotWhisker>;
  using ValueSet = std::map<double,int,RealCmp>;
  using SetValue = std::map<int,double>;
  using NameSet  = std::map<QString,int>;
  using SetName  = std::map<int,QString>;

  int      xColumn_      { 0 };
  int      yColumn_      { 1 };
  QColor   boxColor_     { "#46A2B4" };
  double   cornerRadius_ { 0.0 };
  Whiskers whiskers_;
  ValueSet valueSet_;
  SetValue setValue_;
  NameSet  nameSet_;
  SetName  setName_;
};

#endif
