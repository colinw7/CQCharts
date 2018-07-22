#ifndef CQChartsViewSettings_H
#define CQChartsViewSettings_H

#include <QFrame>

class CQChartsWindow;
class CQChartsPlot;
class CQChartsFilterEdit;
class CQChartsGradientPaletteCanvas;
class CQChartsGradientPaletteControl;
class CQChartsLoadDlg;
class CQChartsPlotDlg;
class CQPropertyViewTree;
class CQIntegerSpin;

class QTabWidget;
class QTableWidget;
class QComboBox;
class QSpinBox;
class QPushButton;
class QRadioButton;
class QCheckBox;
class QLineEdit;
class QLabel;

class CQChartsViewSettings : public QFrame {
  Q_OBJECT

 public:
  using Plots = std::vector<CQChartsPlot *>;

 public:
  CQChartsViewSettings(CQChartsWindow *window);
 ~CQChartsViewSettings();

 signals:
  void propertyItemSelected(QObject *obj, const QString &path);

 private slots:
  void updateModels();
  void updatePlots ();
  void updateLayers();

  void updateCurrentPlot();

  void paletteIndexSlot(int ind);

  void loadPaletteNameSlot();

  void replaceFilterSlot(const QString &text);
  void addFilterSlot(const QString &text);

  void replaceSearchSlot(const QString &text);
  void addSearchSlot(const QString &text);

  void modelsSelectionChangeSlot();

  void loadModelSlot();

  void plotsSelectionChangeSlot();

  void layersSelectionChangeSlot();
  void layersClickedSlot(int, int);

  void groupPlotsSlot();

  void placePlotsSlot();

  void raisePlotSlot();
  void lowerPlotSlot();

  void removePlotsSlot();

  void createPlotSlot();

  void updatePalettes();
  void updateInterface();

 private:
  CQPropertyViewTree *propertyTree() const { return propertiesWidgets_.propertyTree; }

  CQChartsGradientPaletteCanvas  *interfacePlot   () const {
    return themeWidgets_.interfacePlot; }
  CQChartsGradientPaletteControl *interfaceControl() const {
    return themeWidgets_.interfaceControl; }

  CQChartsGradientPaletteCanvas  *palettesPlot   () const {
    return themeWidgets_.palettesPlot; }
  CQChartsGradientPaletteControl *palettesControl() const {
    return themeWidgets_.palettesControl; }

  CQChartsPlot *getSelectedPlot() const;

  void getSelectedPlots(Plots &plots) const;

 private:
  struct PropertiesWidgets {
    CQChartsFilterEdit* filterEdit   { nullptr }; // settings filter
    CQPropertyViewTree* propertyTree { nullptr }; // settings tree
  };

  struct ModelsWidgets {
    QTableWidget* modelTable { nullptr };
  };

  struct PlotsWidgets {
    QTableWidget*  plotTable            { nullptr };
    QCheckBox*     overlayCheck         { nullptr };
    QCheckBox*     x1x2Check            { nullptr };
    QCheckBox*     y1y2Check            { nullptr };
    QRadioButton*  placeVerticalRadio   { nullptr };
    QRadioButton*  placeHorizontalRadio { nullptr };
    QRadioButton*  placeGridRadio       { nullptr };
    CQIntegerSpin* placeRowsEdit        { nullptr };
    CQIntegerSpin* placeColumnsEdit     { nullptr };
    QPushButton*   raiseButton          { nullptr };
    QPushButton*   lowerButton          { nullptr };
    QPushButton*   removeButton         { nullptr };
  };

  struct ThemeWidgets {
    QSpinBox*                       palettesSpin       { nullptr }; // palettes index spin
    QComboBox*                      palettesCombo      { nullptr }; // palettes name combo
    QPushButton*                    palettesLoadButton { nullptr }; // load palette name button
    CQChartsGradientPaletteCanvas*  palettesPlot       { nullptr }; // current palette plot
    CQChartsGradientPaletteControl* palettesControl    { nullptr }; // current palette control
    CQChartsGradientPaletteCanvas*  interfacePlot      { nullptr }; // interface palette plot
    CQChartsGradientPaletteControl* interfaceControl   { nullptr }; // interface palette control
  };

  struct LayersWidgets {
    QTableWidget* layerTable { nullptr };
  };

  CQChartsWindow*   window_       { nullptr }; // parent window
  QTabWidget*       tab_          { nullptr }; // settings/palette tab
  PropertiesWidgets propertiesWidgets_;
  ModelsWidgets     modelsWidgets_;
  PlotsWidgets      plotsWidgets_;
  ThemeWidgets      themeWidgets_;
  LayersWidgets     layersWidgets_;
  CQChartsLoadDlg*  loadDlg_      { nullptr };
  CQChartsPlotDlg*  plotDlg_      { nullptr };
  QString           plotId_;
};

#endif
