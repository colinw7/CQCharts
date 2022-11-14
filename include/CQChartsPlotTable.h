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

  View *view() const;
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
  Plot *getSelectedPlot() const;

  void getSelectedPlots(Plots &plots) const;

 private:
  using ViewP = QPointer<View>;

  CQChartsPlotTable *plotTable_ { nullptr }; //!< plot table

  ViewP view_;

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
  using View = CQChartsView;
  using Plot = CQChartsPlot;

 public:
  CQChartsPlotTable();

  void updatePlots(View *view);

  void setCurrentInd(int ind);

  void getSelectedPlots(View *view, std::vector<Plot *> &plots);
};

#endif
