#ifndef CQChartsImagePlot_H
#define CQChartsImagePlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDataLabel.h>

//---

class CQChartsImagePlotType : public CQChartsPlotType {
 public:
  CQChartsImagePlotType();

  QString name() const override { return "image"; }
  QString desc() const override { return "Image"; }

  void addParameters() override;

  bool hasAxes() const override { return false; }

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsImagePlot;

class CQChartsImageObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsImageObj(CQChartsImagePlot *plot, const CQChartsGeom::BBox &rect,
                   double value, const QModelIndex &ind);

  QString calcId() const override;

  QString calcTipId() const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsImagePlot *plot_  { nullptr };
  double             value_ { 0.0 };
  QModelIndex        ind_;
};

//---

class CQChartsImagePlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(double        minValue   READ minValue     WRITE setMinValue  )
  Q_PROPERTY(double        maxValue   READ maxValue     WRITE setMaxValue  )
  Q_PROPERTY(bool          xLabels    READ isXLabels    WRITE setXLabels   )
  Q_PROPERTY(bool          yLabels    READ isYLabels    WRITE setYLabels   )
  Q_PROPERTY(bool          cellLabels READ isCellLabels WRITE setCellLabels)
  Q_PROPERTY(CQChartsColor textColor  READ textColor    WRITE setTextColor )
  Q_PROPERTY(double        textAlpha  READ textAlpha    WRITE setTextAlpha )
  Q_PROPERTY(QFont         textFont   READ textFont     WRITE setTextFont  )

 public:
  CQChartsImagePlot(CQChartsView *view, const ModelP &model);

  //---

  double minValue() const { return minValue_; }
  void setMinValue(double r) { minValue_ = r; update(); }

  double maxValue() const { return maxValue_; }
  void setMaxValue(double r) { maxValue_ = r; update(); }

  //---

  bool isXLabels() const { return xLabels_; }
  void setXLabels(bool b) { xLabels_ = b; update(); }

  bool isYLabels() const { return yLabels_; }
  void setYLabels(bool b) { yLabels_ = b; update(); }

  bool isCellLabels() const { return cellLabels_; }
  void setCellLabels(bool b) { cellLabels_ = b; update(); }

  //---

  const CQChartsColor &textColor() const;
  void setTextColor(const CQChartsColor &c);

  double textAlpha() const;
  void setTextAlpha(double a);

  const QFont &textFont() const;
  void setTextFont(const QFont &f);

  QColor interpTextColor(int i, int n) const;

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  bool initObjs() override;

  //---

  void draw(QPainter *) override;

  CQChartsGeom::BBox annotationBBox() const override;

 private:
  void addImageObj(double x, double y, double dx, double dy, double value, const QModelIndex &ind);

  void drawXLabels(QPainter *);
  void drawYLabels(QPainter *);

 private:
  double           minValue_   { 0.0 };   // min value
  double           maxValue_   { 0.0 };   // max value
  bool             xLabels_    { false }; // x labels
  bool             yLabels_    { false }; // y labels
  bool             cellLabels_ { false }; // cell labels
  CQChartsTextData textData_;             // text style
  int              nc_         { 0 };     // number of grid columns
  int              nr_         { 0 };     // number of grid rows
};

#endif
