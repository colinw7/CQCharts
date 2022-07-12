#ifndef CQChartsPlotTable_H
#define CQChartsPlotTable_H

#include <CQChartsWidgetIFace.h>
#include <CQTableWidget.h>

class CQChartsPlotTable;
class CQChartsView;
class CQChartsPlot;

class CQIntegerSpin;

class QCheckBox;
class QRadioButton;
class QPushButton;

class CQChartsPlotTableControl : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  using View  = CQChartsView;
  using Plot  = CQChartsPlot;
  using Plots = std::vector<Plot *>;

 public:
  CQChartsPlotTableControl();

  View *view() const { return view_; }
  void setView(View *view) override;

 private Q_SLOTS:
  void updatePlots();

  void plotsSelectionChangeSlot();

  void updateCurrentPlot();

  void updatePlotOverlayState();

  void groupPlotsSlot();

  void placePlotsSlot();

  void raisePlotSlot();
  void lowerPlotSlot();

  void createPlotSlot();

  void removePlotsSlot();

 private:
  CQChartsPlot *getSelectedPlot() const;

  void getSelectedPlots(Plots &plots) const;

 private:
  CQChartsPlotTable *plotTable_ { nullptr }; //!< plot table

  View *view_ { nullptr };

  QCheckBox*     overlayCheck_         { nullptr }; //!< overlay check
  QCheckBox*     x1x2Check_            { nullptr }; //!< x1x2 check
  QCheckBox*     y1y2Check_            { nullptr }; //!< y1y2 check
  QRadioButton*  placeVerticalRadio_   { nullptr }; //!< place vertical radio
  QRadioButton*  placeHorizontalRadio_ { nullptr }; //!< place horizontal radio
  QRadioButton*  placeGridRadio_       { nullptr }; //!< place grid radio
  CQIntegerSpin* placeRowsEdit_        { nullptr }; //!< grid number of rows edit
  CQIntegerSpin* placeColumnsEdit_     { nullptr }; //!< grid number of columns edit
  QPushButton*   raiseButton_          { nullptr }; //!< raise plot button
  QPushButton*   lowerButton_          { nullptr }; //!< lower plot button
  QPushButton*   createButton_         { nullptr }; //!< create plot button
  QPushButton*   removeButton_         { nullptr }; //!< remove plot button
};

//------

class CQChartsPlotTable : public CQTableWidget, public CQChartsWidgetIFace {
  Q_OBJECT

 public:
  CQChartsPlotTable();

  void updatePlots(CQChartsView *view);

  void setCurrentInd(int ind);

  void getSelectedPlots(CQChartsView *view, std::vector<CQChartsPlot *> &plots);
};

#endif
