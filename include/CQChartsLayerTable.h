#ifndef CQChartsLayerTable_H
#define CQChartsLayerTable_H

#include <CQChartsLayerTable.h>
#include <CQChartsLayer.h>
#include <CQChartsWidgetIFace.h>

#include <CQTableWidget.h>

class CQChartsView;
class CQChartsPlot;

class CQChartsViewLayerTable : public CQTableWidget, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  CQChartsViewLayerTable();

  CQChartsView *view() const { return view_; }
  void setView(CQChartsView *view) override;

  QImage *selectedImage(CQChartsView *view) const;

  void initLayers();

  void updateLayers(CQChartsView *view);

 private:
  CQChartsView *view_ { nullptr };
};

//---

class CQChartsPlotLayerTable : public CQTableWidget, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  CQChartsPlotLayerTable();

  CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *plot) override;

  QImage *selectedImage(CQChartsPlot *plot) const;

  void initLayers();

  void updateLayers(CQChartsPlot *plot);

  bool getLayerState(CQChartsPlot *plot, int row, CQChartsLayer::Type &type, bool &active);

 private:
  CQChartsPlot *plot_ { nullptr };
};

#endif
