#ifndef CQChartsPiePlot_H
#define CQChartsPiePlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsTextBoxObj.h>

#include <CPoint2D.h>

#include <boost/optional.hpp>
#include <string>

class CQChartsPiePlot;

//---

class CQChartsPieTextObj : public CQChartsTextBoxObj {
 public:
  CQChartsPieTextObj(CQChartsPiePlot *plot);

  void redrawBoxObj() override;

  const QRectF &rect() const { return rect_; }
  void setRect(const QRectF &r) { rect_ = r; }

  void draw(QPainter *p, const QPointF &c, const QString &text, double angle=0.0,
            Qt::Alignment align=Qt::AlignHCenter | Qt::AlignVCenter) const;

 private:
  CQChartsPiePlot *plot_ { nullptr };
  mutable QRectF   rect_;
};

//---

class CQChartsPieObj : public CQChartsPlotObj {
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

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  double value() const { return value_; }
  void setValue(double r) { value_ = r; }

  bool isWedge() const { return wedge_; }
  void setWedge(bool b) { wedge_ = b; }

  bool isExploded() const { return exploded_; }
  void setExploded(bool b) { exploded_ = b; }

  //---

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 protected:
  CQChartsPiePlot* plot_        { nullptr };
  int              i_           { -1 };
  int              n_           { -1 };
  CPoint2D         center_      { 0, 0 };
  double           r_           { 1 };
  double           angle1_      { 0 };
  double           angle2_      { 360 };
  QString          name_        { "" };
  double           value_       { 0 };
  bool             wedge_       { true };
  bool             exploded_    { false };
};

//---

#include <CQChartsKey.h>

class CQChartsPieKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsPieKeyColor(CQChartsPiePlot *plot, int i, int n);

  bool mousePress(const CPoint2D &p) override;

  QColor fillColor() const override;
};

class CQChartsPieKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsPieKeyText(CQChartsPiePlot *plot, int i, const QString &text);

  QColor textColor() const override;

 private:
  int i_ { 0 };
};

//---

class CQChartsPiePlotType : public CQChartsPlotType {
 public:
  CQChartsPiePlotType();

  QString name() const override { return "pie"; }
  QString desc() const override { return "Pie"; }
};

//---

class CQChartsPiePlot : public CQChartsPlot {
  Q_OBJECT

  // propeties
  //   donut

  Q_PROPERTY(int    labelColumn     READ labelColumn       WRITE setLabelColumn    )
  Q_PROPERTY(int    dataColumn      READ dataColumn        WRITE setDataColumn     )
  Q_PROPERTY(bool   donut           READ isDonut           WRITE setDonut          )
  Q_PROPERTY(double innerRadius     READ innerRadius       WRITE setInnerRadius    )
  Q_PROPERTY(double labelRadius     READ labelRadius       WRITE setLabelRadius    )
  Q_PROPERTY(bool   rotatedText     READ isRotatedText     WRITE setRotatedText    )
  Q_PROPERTY(bool   explodeSelected READ isExplodeSelected WRITE setExplodeSelected)

 public:
  CQChartsPiePlot(CQChartsView *view, QAbstractItemModel *model);

  int labelColumn() const { return labelColumn_; }
  void setLabelColumn(int i) { labelColumn_ = i; }

  int dataColumn() const { return dataColumn_; }
  void setDataColumn(int i) { dataColumn_ = i; }

  bool isDonut() const { return donut_; }
  void setDonut(bool b) { donut_ = b; update(); }

  double innerRadius() const { return innerRadius_; }
  void setInnerRadius(double r) { innerRadius_ = r; update(); }

  double labelRadius() const { return labelRadius_; }
  void setLabelRadius(double r) { labelRadius_ = r; updateRange(); }

  bool isRotatedText() const { return rotatedText_; }
  void setRotatedText(bool b) { rotatedText_ = b; update(); }

  bool isExplodeSelected() const { return explodeSelected_; }
  void setExplodeSelected(bool b) { explodeSelected_ = b; update(); }

  //---

  const CQChartsPieTextObj &textBox() const { return textBox_; }

  const CBBox2D &contentsBBox() const { return contentsBBox_; }
  void setContentsBBox(const CBBox2D &b) { contentsBBox_ = b; }

  //---

  void addProperties();

  void updateRange();

  void initObjs(bool force=false);

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
  typedef std::map<int,bool> IdHidden;

  int                labelColumn_     { 0 };
  int                dataColumn_      { 1 };
  bool               donut_           { false };
  double             innerRadius_     { 0.0 };
  double             labelRadius_     { 0.5 };
  bool               rotatedText_     { false };
  bool               explodeSelected_ { true };
  IdHidden           idHidden_;
  CQChartsPieTextObj textBox_;
  CBBox2D            contentsBBox_;
};

#endif
