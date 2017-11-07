#ifndef CQChartsDistributionPlot_H
#define CQChartsDistributionPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>

class CQChartsDistributionPlot;
class CQChartsBoxObj;
class CQChartsFillObj;

class CQChartsDistributionBarObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Values = std::vector<int>;

 public:
  CQChartsDistributionBarObj(CQChartsDistributionPlot *plot, const CBBox2D &rect,
                             int bucket, const Values &value, int i, int n);

  bool inside(const CPoint2D &p) const override;

  void mousePress(const CPoint2D &) override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsDistributionPlot *plot_   { nullptr };
  int                       bucket_ { 0 };
  Values                    values_;
  int                       i_      { -1 };
  int                       n_      { -1 };
};

//---

// distribution plot type
class CQChartsDistributionPlotType : public CQChartsPlotType {
 public:
  CQChartsDistributionPlotType();

  QString name() const override { return "distribution"; }
  QString desc() const override { return "Distribution"; }

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

// distribution plot
class CQChartsDistributionPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int     valueColumn      READ valueColumn       WRITE setValueColumn     )
  Q_PROPERTY(double  startValue       READ startValue        WRITE setStartValue      )
  Q_PROPERTY(double  deltaValue       READ deltaValue        WRITE setDeltaValue      )
  Q_PROPERTY(bool    horizontal       READ isHorizontal      WRITE setHorizontal      )
  Q_PROPERTY(double  margin           READ margin            WRITE setMargin          )
  Q_PROPERTY(bool    border           READ isBorder          WRITE setBorder          )
  Q_PROPERTY(QColor  borderColor      READ borderColor       WRITE setBorderColor     )
  Q_PROPERTY(double  borderWidth      READ borderWidth       WRITE setBorderWidth     )
  Q_PROPERTY(double  borderCornerSize READ borderCornerSize  WRITE setBorderCornerSize)
  Q_PROPERTY(bool    barFill          READ isBarFill         WRITE setBarFill         )
  Q_PROPERTY(QString barColor         READ barColorStr       WRITE setBarColorStr     )
  Q_PROPERTY(double  barAlpha         READ barAlpha          WRITE setBarAlpha        )
  Q_PROPERTY(Pattern barPattern       READ barPattern        WRITE setBarPattern      )

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

 public:
  CQChartsDistributionPlot(CQChartsView *view, const ModelP &model);
 ~CQChartsDistributionPlot();

  //---

  int valueColumn() const { return valueColumn_; }
  void setValueColumn(int i) { valueColumn_ = i; update(); }

  double startValue() const { return startValue_; }
  void setStartValue(double r) { startValue_ = r; updateRange(); updateObjs(); }

  double deltaValue() const { return deltaValue_; }
  void setDeltaValue(double r) { deltaValue_ = r; updateRange(); updateObjs(); }

  //---

  bool isHorizontal() const { return horizontal_; }
  void setHorizontal(bool b) { horizontal_ = b; updateRange(); updateObjs(); }

  //---

  // bar margin
  int margin() const { return margin_; }
  void setMargin(int i) { margin_ = i; update(); }

  //---

  // bar stroke
  bool isBorder() const;
  void setBorder(bool b);

  const QColor &borderColor() const;
  void setBorderColor(const QColor &c);

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

  double barAlpha() const;
  void setBarAlpha(double a);

  Pattern barPattern() const;
  void setBarPattern(Pattern pattern);

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void initObjs() override;

  //---

  QColor barColor(int i, int n) const;

  //---

  void draw(QPainter *) override;

 private:
  using Values  = std::vector<int>;
  using IValues = std::map<int,Values>;

  int              valueColumn_ { -1 };
  double           startValue_  { 0.0 };
  double           deltaValue_  { 1.0 };
  IValues          ivalues_;
  bool             horizontal_  { false };
  double           margin_      { 2 };
  CQChartsBoxObj*  borderObj_   { nullptr };
  CQChartsFillObj* fillObj_     { nullptr };
};

#endif
