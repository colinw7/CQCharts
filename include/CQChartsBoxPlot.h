#ifndef CQChartsBoxPlot_H
#define CQChartsBoxPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsUtil.h>

#include <CBoxWhisker.h>
#include <map>

class CQChartsBoxPlot;

class CQChartsBoxPlotObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxPlotObj(CQChartsBoxPlot *plot, const CBBox2D &rect, double pos,
                     const CBoxWhisker &whisker, int i, int n);

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsBoxPlot *plot_ { nullptr };
  double           pos_  { 0.0 };
  CBoxWhisker      whisker_;
  int              i_    { -1 };
  int              n_    { 0 };
};

//---

#include <CQChartsKey.h>

class CQChartsBoxKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsBoxKeyColor(CQChartsBoxPlot *plot, int i, int n);

  bool mousePress(const CPoint2D &p) override;

  QColor fillColor() const override;
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

  void updateRange() override;

  void initObjs(bool force=false) override;

  void addKeyItems(CQChartsKey *key) override;

  //---

  bool isSetHidden(int i) const {
    auto p = idHidden_.find(i);

    if (p == idHidden_.end())
      return false;

    return (*p).second;
  }

  void setSetHidden(int i, bool hidden) { idHidden_[i] = hidden; }

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

  typedef std::map<int,CBoxWhisker>    Whiskers;
  typedef std::map<int,bool>           IdHidden;
  typedef std::map<double,int,RealCmp> ValueSet;
  typedef std::map<int,double>         SetValue;
  typedef std::map<QString,int>        NameSet;
  typedef std::map<int,QString>        SetName;

  int      xColumn_      { 0 };
  int      yColumn_      { 1 };
  QColor   boxColor_     { "#46A2B4" };
  double   cornerRadius_ { 0.0 };
  Whiskers whiskers_;
  ValueSet valueSet_;
  SetValue setValue_;
  NameSet  nameSet_;
  SetName  setName_;
  IdHidden idHidden_;
};

#endif
