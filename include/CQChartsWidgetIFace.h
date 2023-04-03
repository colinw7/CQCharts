#ifndef CQChartsWidgetIFace_H
#define CQChartsWidgetIFace_H

class CQCharts;
class CQChartsView;
class CQChartsPlot;
class CQChartsModelData;

class CQChartsWidgetIFace {
 public:
  using Charts    = CQCharts;
  using View      = CQChartsView;
  using Plot      = CQChartsPlot;
  using ModelData = CQChartsModelData;

 public:
  CQChartsWidgetIFace() = default;

  virtual ~CQChartsWidgetIFace() = default;

  virtual void setCharts(Charts *) { }

  virtual void setView(View *) { }
  virtual void setPlot(Plot *) { }

  virtual void setModelData(ModelData *) { }
};

#endif
