#ifndef CQChartsViewSettings_H
#define CQChartsViewSettings_H

#include <QFrame>

class CQChartsWindow;
class CQChartsPlot;
class CQChartsAnnotation;
class CQChartsFilterEdit;
class CQChartsGradientPaletteCanvas;
class CQChartsGradientPaletteControl;
class CQChartsLoadDlg;
class CQChartsPlotDlg;
class CQChartsAnnotationDlg;
class CQPropertyViewTree;
class CQIntegerSpin;

class QTabWidget;
class QTableWidget;
class QTextBrowser;
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
  using Plots       = std::vector<CQChartsPlot *>;
  using Annotations = std::vector<CQChartsAnnotation *>;

 public:
  CQChartsViewSettings(CQChartsWindow *window);
 ~CQChartsViewSettings();

  CQChartsWindow *window() const { return window_; }

 signals:
  void propertyItemSelected(QObject *obj, const QString &path);

 private slots:
  void updateModels();

  void invalidateModelDetails();
  void updateModelDetails();

  void updatePlots();
  void updateCurrentPlot();

  void updateAnnotations();

  void updateLayers();

  void paletteIndexSlot(int ind);

  void loadPaletteNameSlot();

  void replaceFilterSlot(const QString &text);
  void addFilterSlot(const QString &text);

  void replaceSearchSlot(const QString &text);
  void addSearchSlot(const QString &text);

  //---

  void modelsSelectionChangeSlot();

  void loadModelSlot();

  //---

  void layersSelectionChangeSlot();
  void layersClickedSlot(int, int);

  //---

  void plotsSelectionChangeSlot();

  void groupPlotsSlot();

  void placePlotsSlot();

  void raisePlotSlot();
  void lowerPlotSlot();

  void removePlotsSlot();

  void createPlotSlot();
  void writePlotSlot();

  //---

  void viewAnnotationSelectionChangeSlot();
  void plotAnnotationSelectionChangeSlot();

  void createAnnotationSlot();
  void removeAnnotationsSlot();
  void writeAnnotationSlot();

  //---

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

  void addWidgets();

  void initPropertiesFrame (QFrame *propertiesFrame);
  void initModelsFrame     (QFrame *modelsFrame);
  void initPlotsFrame      (QFrame *plotsFrame);
  void initAnnotationsFrame(QFrame *annotationsFrame);
  void initThemeFrame      (QFrame *themeFrame);
  void initLayersFrame     (QFrame *layersFrame);

  CQChartsPlot *getSelectedPlot() const;

  void getSelectedPlots(Plots &plots) const;

  void getSelectedAnnotations(Annotations &viewAnnotations, Annotations &plotAnnotations) const;

 private:
  struct PropertiesWidgets {
    CQChartsFilterEdit* filterEdit   { nullptr }; // settings filter
    CQPropertyViewTree* propertyTree { nullptr }; // settings tree
  };

  struct ModelsWidgets {
    QTableWidget* modelTable          { nullptr };
    QPushButton*  updateDetailsButton { nullptr };
    QTextBrowser* modelDetailsText    { nullptr };
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

  struct AnnotationsWidgets {
    QTableWidget* viewTable    { nullptr };
    QTableWidget* plotTable    { nullptr };
    QPushButton*  removeButton { nullptr };
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

  CQChartsWindow*        window_             { nullptr }; // parent window
  QTabWidget*            tab_                { nullptr }; // settings/palette tab
  PropertiesWidgets      propertiesWidgets_;              // properties widgets
  ModelsWidgets          modelsWidgets_;                  // models widgets
  PlotsWidgets           plotsWidgets_;                   // plots widgets
  AnnotationsWidgets     annotationsWidgets_;             // annotations widgets
  ThemeWidgets           themeWidgets_;                   // theme widgets
  LayersWidgets          layersWidgets_;                  // layers widgets
  CQChartsLoadDlg*       loadDlg_            { nullptr }; // load dalog
  CQChartsPlotDlg*       plotDlg_            { nullptr }; // plot dalog
  CQChartsAnnotationDlg* annotationDlg_      { nullptr }; // annotation dalog
  QString                plotId_;                         // current plot id
  bool                   modelDetailsValid_  { false };   // model details valid
};

#endif
