#ifndef CQChartsLayerTable_H
#define CQChartsLayerTable_H

#include <CQChartsLayerTable.h>
#include <CQChartsLayer.h>
#include <CQChartsWidgetIFace.h>

#include <CQTableWidget.h>

class CQChartsViewLayerTable;
class CQChartsPlotLayerTable;
class CQChartsView;
class CQChartsPlot;

class CQChartsLayerTableControl : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  CQChartsLayerTableControl(QWidget *parent=nullptr);

  CQChartsView *view() const { return view_; }
  void setView(CQChartsView *view);

  CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *plot);

 private Q_SLOTS:
  void viewLayerImageSlot();
  void plotLayerImageSlot();

 private:
  CQChartsView*           view_           { nullptr };
  CQChartsPlot*           plot_           { nullptr };
  CQChartsViewLayerTable* viewLayerTable_ { nullptr };
  CQChartsPlotLayerTable* plotLayerTable_ { nullptr };
};

//---

class CQChartsViewLayerTable : public CQTableWidget, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  CQChartsViewLayerTable();

  CQChartsView *view() const { return view_; }
  void setView(CQChartsView *view) override;

  QImage *selectedImage(CQChartsView *view) const;

  void initLayers();

  void updateLayers(CQChartsView *view);

 private Q_SLOTS:
  void selectionChangeSlot();
  void clickedSlot(int row, int column);

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

 private Q_SLOTS:
  void selectionChangeSlot();
  void clickedSlot(int row, int column);

 private:
  CQChartsPlot *plot_ { nullptr };
};

#endif
