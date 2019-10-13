#ifndef CQChartsEmptyPlot_H
#define CQChartsEmptyPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>

//---

/*!
 * \brief Empty plot type
 * \ingroup Charts
 */
class CQChartsEmptyPlotType : public CQChartsPlotType {
 public:
  CQChartsEmptyPlotType();

  QString name() const override { return "empty"; }
  QString desc() const override { return "Empty"; }

  void addParameters() override;

  bool canProbe() const override { return false; }

  bool hasObjs() const override { return false; }

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsEmptyPlot;

//---

/*!
 * \brief Empty Plot
 * \ingroup Charts
 */
class CQChartsEmptyPlot : public CQChartsPlot {
  Q_OBJECT

 public:
  CQChartsEmptyPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsEmptyPlot();

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  bool addMenuItems(QMenu *menu) override;
};

#endif
