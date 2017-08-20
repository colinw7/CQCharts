#ifndef CQChartsPiePlot_H
#define CQChartsPiePlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>

#include <COptVal.h>
#include <CPoint2D.h>
#include <CRGBA.h>
#include <string>

class CQChartsPiePlot;

class CQChartsPieObj : public CQChartsPlotObj {
 public:
  typedef std::pair<CHAlignType,double> HAlignPos;
  typedef std::pair<CVAlignType,double> VAlignPos;

 public:
  CQChartsPieObj(CQChartsPiePlot *plot, const CBBox2D &rect, int i, int n);

  const CPoint2D &center() const { return center_; }
  void setCenter(const CPoint2D &c) { center_ = c; }

  double radius() const { return r_; }
  void setRadius(double r) { r_ = r; }

  double angle1() const { return angle1_; }
  void setAngle1(double a) { angle1_ = a; }

  double angle2() const { return angle2_; }
  void setAngle2(double a) { angle2_ = a; }

  double innerRadius() const { return innerRadius_; }
  void setInnerRadius(double r) { innerRadius_ = r; }

  double labelRadius() const { return labelRadius_; }
  void setLabelRadius(double r) { labelRadius_ = r; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  double value() const { return value_; }
  void setValue(double r) { value_ = r; }

  bool isExploded() const { return exploded_; }
  void setExploded(bool b) { exploded_ = b; }

  bool isExplodeSelected() const { return explodeSelected_; }
  void setExplodeSelected(bool b) { explodeSelected_ = b; }

  bool isRotatedText() const { return rotatedText_; }
  void setRotatedText(bool b) { rotatedText_ = b; }

  bool isWedge() const { return wedge_; }
  void setWedge(bool b) { wedge_ = b; }

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p) override;

 protected:
  CQChartsPiePlot* plot_            { nullptr };
  int              i_               { -1 };
  int              n_               { -1 };
  CPoint2D         center_          { 0, 0 };
  double           r_               { 1 };
  double           angle1_          { 0 };
  double           angle2_          { 360 };
  double           innerRadius_     { 0.0 };
  double           labelRadius_     { 0.5 };
  QString          name_            { "" };
  double           value_           { 0 };
  bool             rotatedText_     { false };
  bool             wedge_           { true };
  bool             exploded_        { false };
  bool             explodeSelected_ { true };
};

//---

#include <CQChartsKey.h>

class CQChartsPieKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsPieKeyColor(CQChartsPiePlot *plot, int i, int n);

  void mousePress(const CPoint2D &p) override;
};

//---

class CQChartsPiePlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(bool donut   READ isDonut WRITE setDonut  )
  Q_PROPERTY(int  xColumn READ xColumn WRITE setXColumn)
  Q_PROPERTY(int  yColumn READ yColumn WRITE setYColumn)

  // propeties
  //   donut

 public:
  CQChartsPiePlot(CQChartsWindow *window, QAbstractItemModel *model);

  bool isDonut() const { return donut_; }
  void setDonut(bool b) { donut_ = b; update(); }

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; }

  void addProperties();

  void initObjs(bool force=false);

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

  QColor segmentColor(int i, int n) const;

  QColor textColor(const QColor &bg) const;

 private:
  typedef std::map<int,bool> IdHidden;

  bool     donut_   { false };
  int      xColumn_ { 0 };
  int      yColumn_ { 1 };
  IdHidden idHidden_;
};

#endif
