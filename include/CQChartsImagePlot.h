#ifndef CQChartsImagePlot_H
#define CQChartsImagePlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDataLabel.h>

class CQChartsImagePlot;

class CQChartsImageObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsImageObj(CQChartsImagePlot *plot, const CQChartsGeom::BBox &rect,
                   double value, const QModelIndex &ind);

  QString calcId() const;

  void mousePress(const CQChartsGeom::Point &) override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsImagePlot *plot_  { nullptr };
  double             value_ { 0.0 };
  QModelIndex        ind_;
};

//---

class CQChartsImagePlotType : public CQChartsPlotType {
 public:
  CQChartsImagePlotType();

  QString name() const override { return "image"; }
  QString desc() const override { return "Image"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsImagePlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(double minValue READ minValue WRITE setMinValue)
  Q_PROPERTY(double maxValue READ maxValue WRITE setMaxValue)

 public:
  CQChartsImagePlot(CQChartsView *view, const ModelP &model);

  //---

  double minValue() const { return minValue_; }
  void setMinValue(double r) { minValue_ = r; update(); }

  double maxValue() const { return maxValue_; }
  void setMaxValue(double r) { maxValue_ = r; update(); }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void initObjs() override;

  //---

  void draw(QPainter *) override;

 private:
  double minValue_ { 0.0 };
  double maxValue_ { 0.0 };
};

#endif
