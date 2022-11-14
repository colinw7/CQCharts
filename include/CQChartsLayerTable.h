#ifndef CQChartsLayerTable_H
#define CQChartsLayerTable_H

#include <CQChartsLayerTable.h>
#include <CQChartsLayer.h>
#include <CQChartsWidgetIFace.h>

#include <CQTableWidget.h>

#include <QPainter>

class CQChartsViewLayerTable;
class CQChartsPlotLayerTable;
class CQChartsView;
class CQChartsPlot;

class CQChartsLayerTableControl : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  using View = CQChartsView;
  using Plot = CQChartsPlot;

 public:
  CQChartsLayerTableControl(QWidget *parent=nullptr);

  View *view() const;
  void setView(View *view);

  Plot *plot() const;
  void setPlot(Plot *plot);

 private Q_SLOTS:
  void viewLayerImageSlot();
  void plotLayerImageSlot();

 private:
  using ViewP = QPointer<View>;
  using PlotP = QPointer<Plot>;

  ViewP                   view_;
  PlotP                   plot_;
  CQChartsViewLayerTable* viewLayerTable_ { nullptr };
  CQChartsPlotLayerTable* plotLayerTable_ { nullptr };
};

//---

class CQChartsViewLayerTable : public CQTableWidget, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  using View = CQChartsView;

 public:
  CQChartsViewLayerTable();

  View *view() const;
  void setView(View *view) override;

  QImage *selectedImage(View *view) const;

  void initLayers();

  void updateLayers(View *view);

 private Q_SLOTS:
  void selectionChangeSlot();
  void clickedSlot(int row, int column);

 private:
  using ViewP = QPointer<View>;

  ViewP view_;
};

//---

class CQChartsPlotLayerTable : public CQTableWidget, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  using Plot = CQChartsPlot;

 public:
  CQChartsPlotLayerTable();

  Plot *plot() const;
  void setPlot(Plot *plot) override;

  QImage *selectedImage(Plot *plot) const;

  void initLayers();

  void updateLayers(Plot *plot);

  bool getLayerState(Plot *plot, int row, CQChartsLayer::Type &type, bool &active);

 private Q_SLOTS:
  void selectionChangeSlot();
  void clickedSlot(int row, int column);

 private:
  using PlotP = QPointer<Plot>;

  PlotP plot_;
};

#endif
