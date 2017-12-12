#ifndef CQChartsPiePlot_H
#define CQChartsPiePlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsValueSet.h>

#include <CQChartsGeom.h>

#include <boost/optional.hpp>
#include <string>

class CQChartsPiePlot;

//---

class CQChartsPieTextObj : public CQChartsRotatedTextBoxObj {
 public:
  CQChartsPieTextObj(CQChartsPiePlot *plot);

 private:
  CQChartsPiePlot* plot_ { nullptr };
};

//---

class CQChartsPieObj : public CQChartsPlotObj {
 public:
  using OptColor = boost::optional<CQChartsPaletteColor>;

 public:
  CQChartsPieObj(CQChartsPiePlot *plot, const CQChartsGeom::BBox &rect, const QModelIndex &ind,
                 int i, int n);

  QString calcId() const override;

  double angle1() const { return angle1_; }
  void setAngle1(double a) { angle1_ = a; }

  double angle2() const { return angle2_; }
  void setAngle2(double a) { angle2_ = a; }

  const QString &label() const { return label_; }
  void setLabel(const QString &s) { label_ = s; }

  double value() const { return value_; }
  void setValue(double r) { value_ = r; }

  void setColor(const CQChartsPaletteColor &c) { color_ = c; }

  bool isExploded() const { return exploded_; }
  void setExploded(bool b) { exploded_ = b; }

  //---

  bool inside(const CQChartsGeom::Point &p) const override;

  void addSelectIndex() override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 protected:
  CQChartsPiePlot* plot_     { nullptr }; // parent plot
  QModelIndex      ind_;
  int              i_        { -1 };      // index
  int              n_        { -1 };      // number of wedges
  double           angle1_   { 0 };       // wedge start angle
  double           angle2_   { 360 };     // wedge start angle
  QString          label_    { "" };      // label
  double           value_    { 0 };       // value
  OptColor         color_;                // color
  bool             exploded_ { false };   // exploded
};

//---

#include <CQChartsKey.h>

class CQChartsPieKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsPieKeyColor(CQChartsPiePlot *plot, int i, int n);

  bool mousePress(const CQChartsGeom::Point &p) override;

  QBrush fillBrush() const override;
};

class CQChartsPieKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsPieKeyText(CQChartsPiePlot *plot, int i, const QString &text);

  QColor interpTextColor(int i, int n) const override;

 private:
  int i_ { 0 };
};

//---

class CQChartsPiePlotType : public CQChartsPlotType {
 public:
  CQChartsPiePlotType();

  QString name() const override { return "pie"; }
  QString desc() const override { return "Pie"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsPiePlot : public CQChartsPlot {
  Q_OBJECT

  // propeties
  //   donut

  Q_PROPERTY(int    labelColumn     READ labelColumn       WRITE setLabelColumn    )
  Q_PROPERTY(int    dataColumn      READ dataColumn        WRITE setDataColumn     )
  Q_PROPERTY(int    keyLabelColumn  READ keyLabelColumn    WRITE setKeyLabelColumn )
  Q_PROPERTY(int    colorColumn     READ colorColumn       WRITE setColorColumn    )
  Q_PROPERTY(bool   donut           READ isDonut           WRITE setDonut          )
  Q_PROPERTY(double innerRadius     READ innerRadius       WRITE setInnerRadius    )
  Q_PROPERTY(double outerRadius     READ outerRadius       WRITE setOuterRadius    )
  Q_PROPERTY(double labelRadius     READ labelRadius       WRITE setLabelRadius    )
  Q_PROPERTY(bool   rotatedText     READ isRotatedText     WRITE setRotatedText    )
  Q_PROPERTY(bool   explodeSelected READ isExplodeSelected WRITE setExplodeSelected)
  Q_PROPERTY(double explodeRadius   READ explodeRadius     WRITE setExplodeRadius  )
  Q_PROPERTY(double startAngle      READ startAngle        WRITE setStartAngle     )
  Q_PROPERTY(bool   colorMapEnabled READ isColorMapEnabled WRITE setColorMapEnabled)
  Q_PROPERTY(double colorMapMin     READ colorMapMin       WRITE setColorMapMin    )
  Q_PROPERTY(double colorMapMax     READ colorMapMax       WRITE setColorMapMax    )

 public:
  using OptColor = boost::optional<CQChartsPaletteColor>;

 public:
  CQChartsPiePlot(CQChartsView *view, const ModelP &model);

 ~CQChartsPiePlot();

  int labelColumn() const { return labelColumn_; }
  void setLabelColumn(int i) { labelColumn_ = i; updateRangeAndObjs(); }

  int dataColumn() const { return dataColumn_; }
  void setDataColumn(int i) { dataColumn_ = i; updateRangeAndObjs(); }

  int keyLabelColumn() const { return keyLabelColumn_; }
  void setKeyLabelColumn(int i) { keyLabelColumn_ = i; updateRangeAndObjs(); }

  int colorColumn() const { return colorColumn_; }
  void setColorColumn(int i) { colorColumn_ = i; updateRangeAndObjs(); }

  //---

  bool isDonut() const { return donut_; }
  void setDonut(bool b) { donut_ = b; update(); }

  double innerRadius() const { return innerRadius_; }
  void setInnerRadius(double r) { innerRadius_ = r; updateObjs(); }

  double outerRadius() const { return outerRadius_; }
  void setOuterRadius(double r) { outerRadius_ = r; updateObjs(); }

  double labelRadius() const { return labelRadius_; }
  void setLabelRadius(double r) { labelRadius_ = r; updateRange(); update(); }

  bool isRotatedText() const { return rotatedText_; }
  void setRotatedText(bool b) { rotatedText_ = b; update(); }

  bool isExplodeSelected() const { return explodeSelected_; }
  void setExplodeSelected(bool b) { explodeSelected_ = b; update(); }

  double explodeRadius() const { return explodeRadius_; }
  void setExplodeRadius(double r) { explodeRadius_ = r; update(); }

  double startAngle() const { return startAngle_; }
  void setStartAngle(double r) { startAngle_ = r; updateObjs(); }

  //---

  double innerRadius1() const { return innerRadius1_; }
  void setInnerRadius1(double r) { innerRadius1_ = r; }

  //---

  CQChartsPieTextObj *textBox() const { return textBox_; }

  //---

  bool isColorMapEnabled() const { return colorSet_.isMapEnabled(); }
  void setColorMapEnabled(bool b) { colorSet_.setMapEnabled(b); updateObjs(); }

  double colorMapMin() const { return colorSet_.mapMin(); }
  void setColorMapMin(double r) { colorSet_.setMapMin(r); updateObjs(); }

  double colorMapMax() const { return colorSet_.mapMax(); }
  void setColorMapMax(double r) { colorSet_.setMapMax(r); updateObjs(); }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void initColorSet();

  bool colorSetColor(int i, OptColor &color);

  bool initObjs() override;

  void addKeyItems(CQChartsKey *key) override;

  //---

  void handleResize() override;

  void draw(QPainter *) override;

 private:
  int                 labelColumn_     { 0 };
  int                 dataColumn_      { 1 };
  int                 keyLabelColumn_  { -1 };
  int                 colorColumn_     { -1 };
  bool                donut_           { false };
  double              innerRadius_     { 0.6 };
  double              outerRadius_     { 0.9 };
  double              labelRadius_     { 0.5 };
  bool                rotatedText_     { false };
  bool                explodeSelected_ { true };
  double              explodeRadius_   { 0.05 };
  double              startAngle_      { 90 };
  double              innerRadius1_    { 0.6 };
  CQChartsValueSet    colorSet_;
  CQChartsPieTextObj* textBox_         { nullptr };
};

#endif
