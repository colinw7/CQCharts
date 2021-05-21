#ifndef CQChartsContourPlot_H
#define CQChartsContourPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>

//---

/*!
 * \brief Contour plot type
 * \ingroup Charts
 */
class CQChartsContourPlotType : public CQChartsPlotType {
 public:
  CQChartsContourPlotType();

  QString name() const override { return "contour"; }
  QString desc() const override { return "Contour"; }

  Category category() const override { return Category::TWO_D; }

  void addParameters() override;

  bool canProbe() const override { return false; }

  QString description() const override;

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsContourPlot;
class CQChartsContour;

//---

CQCHARTS_NAMED_SHAPE_DATA(Contour, contour)

/*!
 * \brief Contour Plot
 * \ingroup Charts
 */
class CQChartsContourPlot : public CQChartsPlot,
 public CQChartsObjContourShapeData<CQChartsContourPlot> {
  Q_OBJECT

  // value range
  Q_PROPERTY(bool solid            READ isSolid          WRITE setSolid)
  Q_PROPERTY(int  numContourLevels READ numContourLevels WRITE setNumContourLevels)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Contour, contour)

 public:
  using Color = CQChartsColor;

 public:
  CQChartsContourPlot(View *view, const ModelP &model);
 ~CQChartsContourPlot();

  //---

  void init() override;
  void term() override;

  //---

  bool isSolid() const { return solid_; }
  void setSolid(bool b);

  int numContourLevels() const { return numContourLevels_; }
  void setNumContourLevels(int n);

  //---

  void addProperties() override;

  Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  bool hasBackground() const override;
  bool hasForeground() const override;

  void execDrawBackground(PaintDevice *device) const override;
  void execDrawForeground(PaintDevice *device) const override;

 private:
  void drawContour(PaintDevice *) const;

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  CQChartsContour *contour_          { nullptr };
  int              nx_               { 0 };
  int              ny_               { 0 };
  bool             solid_            { false };
  int              numContourLevels_ { 10 };
};

//---

#include <CQChartsPlotCustomControls.h>

class CQChartsContourPlotCustomControls : public CQChartsPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsContourPlotCustomControls(CQCharts *charts);

  void setPlot(CQChartsPlot *plot) override;

 private:
  void connectSlots(bool b);

 public slots:
  void updateWidgets() override;

 private slots:
  void solidSlot();

 private:
  CQChartsContourPlot*       plot_       { nullptr };
  CQChartsBoolParameterEdit* solidCheck_ { nullptr };
};

#endif
