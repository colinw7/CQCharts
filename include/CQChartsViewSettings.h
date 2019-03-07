#ifndef CQChartsViewSettings_H
#define CQChartsViewSettings_H

#include <QFrame>

class CQChartsViewSettingsViewPropertiesWidget;
class CQChartsViewSettingsPlotPropertiesWidget;
class CQChartsViewSettingsFilterEdit;
class CQChartsViewSettingsModelTable;
class CQChartsViewSettingsPlotTable;
class CQChartsViewSettingsViewAnnotationsTable;
class CQChartsViewSettingsPlotAnnotationsTable;
class CQChartsViewSettingsLayerTable;

class CQChartsModelDetailsWidget;
class CQChartsWindow;
class CQChartsView;
class CQChartsPlot;
class CQChartsAnnotation;
class CQChartsFilterEdit;
class CQChartsGradientPaletteCanvas;
class CQChartsGradientPaletteControl;
class CQChartsLoadModelDlg;
class CQChartsCreatePlotDlg;
class CQChartsCreateAnnotationDlg;
class CQChartsEditAnnotationDlg;
class CQChartsEditTitleDlg;
class CQChartsEditKeyDlg;
class CQChartsEditAxisDlg;
class CQChartsPropertyViewTree;

class CQIntegerSpin;
class CQGroupBox;

class QTabWidget;
class QComboBox;
class QSpinBox;
class QPushButton;
class QRadioButton;
class QCheckBox;
class QLineEdit;
class QLabel;

/*!
 * \brief View settings widget
 */
class CQChartsViewSettings : public QFrame {
  Q_OBJECT

 public:
  using Plots       = std::vector<CQChartsPlot *>;
  using Annotations = std::vector<CQChartsAnnotation *>;

 public:
  CQChartsViewSettings(CQChartsWindow *window);
 ~CQChartsViewSettings();

  CQChartsWindow *window() const { return window_; }

  CQChartsPropertyViewTree *viewPropertyTree() const;

 signals:
  void propertyItemSelected(QObject *obj, const QString &path);

 private slots:
  void updateModels();

  void invalidateModelDetails();
//void updateModelDetails();

  void updatePlots();
  void updateCurrentPlot();

  void updateAnnotations();

  void updateLayers();

  void layerImageSlot();

  void paletteIndexSlot(int ind);

  void loadPaletteNameSlot();

  void updateSelection();

  //---

  void modelsSelectionChangeSlot();

  void loadModelSlot();

  //---

  void layersSelectionChangeSlot();
  void layersClickedSlot(int, int);

  //---

  void writeViewSlot();

  //---

  void plotsSelectionChangeSlot();

  void editViewKeySlot();

  void editPlotTitleSlot();
  void editPlotKeySlot();
  void editPlotXAxisSlot();
  void editPlotYAxisSlot();

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
  void editAnnotationSlot();
  void removeAnnotationsSlot();
  void writeAnnotationSlot();

  //---

  void updatePalettes();
  void updateInterface();

 private:
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

  CQChartsPlot *getPropertiesPlot() const;

  CQChartsPlot *getSelectedPlot() const;

  void getSelectedPlots(Plots &plots) const;

  void getSelectedAnnotations(Annotations &viewAnnotations, Annotations &plotAnnotations) const;

 private:
  struct PropertiesWidgets {
    CQChartsViewSettingsFilterEdit*           viewFilterEdit   { nullptr }; //! view settings filter
    CQChartsViewSettingsViewPropertiesWidget* viewPropertyTree { nullptr }; //! view settings tree
    QTabWidget*                               plotsTab         { nullptr }; //! plots settings tab
  };

  struct ModelsWidgets {
    CQChartsViewSettingsModelTable* modelTable    { nullptr }; //! model table
    CQGroupBox*                     detailsGroup  { nullptr }; //! model details group box
    CQChartsModelDetailsWidget*     detailsWidget { nullptr }; //! model details
  };

  struct PlotsWidgets {
    CQChartsViewSettingsPlotTable* plotTable            { nullptr }; //! plot table
    QCheckBox*                     overlayCheck         { nullptr }; //! overlay check
    QCheckBox*                     x1x2Check            { nullptr }; //! x1x2 check
    QCheckBox*                     y1y2Check            { nullptr }; //! y1y2 check
    QRadioButton*                  placeVerticalRadio   { nullptr }; //! place vertical radio
    QRadioButton*                  placeHorizontalRadio { nullptr }; //! place horizontal radio
    QRadioButton*                  placeGridRadio       { nullptr }; //! place grid radio
    CQIntegerSpin*                 placeRowsEdit        { nullptr }; //! grid number of rows edit
    CQIntegerSpin*                 placeColumnsEdit     { nullptr }; //! grid number of columns edit
    QPushButton*                   raiseButton          { nullptr }; //! raise plot button
    QPushButton*                   lowerButton          { nullptr }; //! lower plot button
    QPushButton*                   removeButton         { nullptr }; //! remove plot button
  };

  struct AnnotationsWidgets {
    CQChartsViewSettingsViewAnnotationsTable* viewTable    { nullptr }; //! view annotations table
    CQChartsViewSettingsPlotAnnotationsTable* plotTable    { nullptr }; //! plot annotations table
    QPushButton*                              editButton   { nullptr }; //! edit button
    QPushButton*                              removeButton { nullptr }; //! remove button
  };

  struct ThemeWidgets {
    QSpinBox*                       palettesSpin       { nullptr }; //! palettes index spin
    QComboBox*                      palettesCombo      { nullptr }; //! palettes name combo
    QPushButton*                    palettesLoadButton { nullptr }; //! load palette name button
    CQChartsGradientPaletteCanvas*  palettesPlot       { nullptr }; //! current palette plot
    CQChartsGradientPaletteControl* palettesControl    { nullptr }; //! current palette control
    CQChartsGradientPaletteCanvas*  interfacePlot      { nullptr }; //! interface palette plot
    CQChartsGradientPaletteControl* interfaceControl   { nullptr }; //! interface palette control
  };

  struct LayersWidgets {
    CQChartsViewSettingsLayerTable* layerTable { nullptr }; //! layer table widget
  };

  CQChartsWindow*              window_              { nullptr }; //! parent window
  QTabWidget*                  tab_                 { nullptr }; //! settings/palette tab
  PropertiesWidgets            propertiesWidgets_;               //! properties widgets
  ModelsWidgets                modelsWidgets_;                   //! models widgets
  PlotsWidgets                 plotsWidgets_;                    //! plots widgets
  AnnotationsWidgets           annotationsWidgets_;              //! annotations widgets
  ThemeWidgets                 themeWidgets_;                    //! theme widgets
  LayersWidgets                layersWidgets_;                   //! layers widgets
  CQChartsLoadModelDlg*        loadModelDlg_        { nullptr }; //! load dialog
  CQChartsCreatePlotDlg*       createPlotDlg_       { nullptr }; //! plot dialog
  CQChartsCreateAnnotationDlg* createAnnotationDlg_ { nullptr }; //! create annotation dialog
  CQChartsEditAnnotationDlg*   editAnnotationDlg_   { nullptr }; //! edit annotation dialog
  CQChartsEditTitleDlg*        editTitleDlg_        { nullptr }; //! edit plot title dialog
  CQChartsEditKeyDlg*          editKeyDlg_          { nullptr }; //! edit plot key dialog
  CQChartsEditAxisDlg*         editXAxisDlg_        { nullptr }; //! edit plot x axis dialog
  CQChartsEditAxisDlg*         editYAxisDlg_        { nullptr }; //! edit plot y axis dialog
  QString                      plotId_;                          //! current plot id
//bool                         modelDetailsValid_   { false };   //! model details valid
};

//---

class CQChartsViewSettingsViewPropertiesWidget : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewSettingsViewPropertiesWidget(CQChartsViewSettings *settings, CQChartsView *view);

  CQChartsView *view() const { return view_; }

  CQChartsPropertyViewTree *propertyTree() const { return propertyTree_; }

 signals:
  void propertyItemSelected(QObject *obj, const QString &path);

 private slots:
  void filterStateSlot(bool b);

 private:
  CQChartsView*                   view_         { nullptr };
  CQChartsPropertyViewTree*       propertyTree_ { nullptr };
  CQChartsViewSettingsFilterEdit* filterEdit_   { nullptr };
};

//---

class CQChartsViewSettingsPlotPropertiesWidget : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewSettingsPlotPropertiesWidget(CQChartsViewSettings *settings, CQChartsPlot *plot);

  CQChartsPlot *plot() const { return plot_; }

  CQChartsPropertyViewTree *propertyTree() const { return propertyTree_; }

 signals:
  void propertyItemSelected(QObject *obj, const QString &path);

 private slots:
  void filterStateSlot(bool b);

 private:
  CQChartsPlot*                   plot_         { nullptr };
  CQChartsPropertyViewTree*       propertyTree_ { nullptr };
  CQChartsViewSettingsFilterEdit* filterEdit_   { nullptr };
};

//---

class CQChartsViewSettingsFilterEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewSettingsFilterEdit(CQChartsPropertyViewTree *tree);

 private slots:
  void replaceFilterSlot(const QString &text);
  void addFilterSlot(const QString &text);

  void replaceSearchSlot(const QString &text);
  void addSearchSlot(const QString &text);

 private:
  CQChartsPropertyViewTree* tree_       { nullptr };
  CQChartsFilterEdit*       filterEdit_ { nullptr };
};

#endif
