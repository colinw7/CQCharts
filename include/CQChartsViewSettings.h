#ifndef CQChartsViewSettings_H
#define CQChartsViewSettings_H

#include <QFrame>

class CQChartsViewSettingsViewPropertiesWidget;
class CQChartsViewSettingsPlotPropertiesWidget;
class CQChartsViewSettingsObjectPropertiesWidget;

class CQChartsViewSettingsFilterEdit;
class CQChartsViewSettingsModelTable;
class CQChartsViewSettingsPlotTable;
class CQChartsViewSettingsViewAnnotationsTable;
class CQChartsViewSettingsPlotAnnotationsTable;
class CQChartsViewSettingsViewLayerTable;
class CQChartsViewSettingsPlotLayerTable;
class CQChartsPlotTip;

class CQChartsModelDetailsWidget;
class CQChartsWindow;
class CQChartsView;
class CQChartsPlot;
class CQChartsAnnotation;
class CQChartsFilterEdit;
class CQChartsCreateAnnotationDlg;
class CQChartsEditAnnotationDlg;
class CQChartsEditTitleDlg;
class CQChartsEditKeyDlg;
class CQChartsEditAxisDlg;
class CQChartsPropertyViewTree;
class CQChartsViewError;
class CQChartsPlotControlFrame;

class CQColorsEditCanvas;
class CQColorsEditControl;
class CQColorsEditList;
class CQPropertyViewModel;

class CQTabWidget;
class CQIntegerSpin;
class CQGroupBox;
class CQTabSplit;

class QComboBox;
class QSpinBox;
class QPushButton;
class QToolButton;
class QRadioButton;
class QCheckBox;
class QTextEdit;
class QLabel;
class QGridLayout;
class QButtonGroup;
class QTimer;

/*!
 * \brief View settings widget
 * \ingroup Charts
 */
class CQChartsViewSettings : public QFrame {
  Q_OBJECT

 public:
  using Plot        = CQChartsPlot;
  using Plots       = std::vector<Plot *>;
  using Annotation  = CQChartsAnnotation;
  using Annotations = std::vector<Annotation *>;

 public:
  CQChartsViewSettings(CQChartsWindow *window);
 ~CQChartsViewSettings();

  CQChartsWindow *window() const { return window_; }

  CQChartsPropertyViewTree *viewPropertyTree() const;

  CQChartsPropertyViewTree *plotPropertyTree(Plot *plot) const;

  void showQueryTab();

  void showErrorsTab();

  CQChartsPlot *currentPlot(bool remap=true) const;

 signals:
  void propertyItemSelected(QObject *obj, const QString &path);

 private slots:
  void updateModels();
  void updateModelsData();

  void invalidateModelDetails(bool changed=true);
//void updateModelDetails();

  void plotsTabChangedSlot();

  void updatePlots();
  void updateCurrentPlot();

  void updatePlotControls();

  void updateAnnotations();
  void updateViewAnnotations();
  void updatePlotAnnotations();

  void updatePlotObjects();

  void updateLayers();

  void showQueryText(const QString &text);

  void updateErrorsSlot();
  void updateErrors();

  void viewLayerImageSlot();
  void plotLayerImageSlot();

  void updateSelection();

  void updateView();

  //---

  void modelsSelectionChangeSlot();

  void loadModelSlot();
  void editModelSlot();
  void removeModelSlot();
  void createPlotModelSlot();

  //---

  void createPlotSlot();

  //---

  void viewLayersSelectionChangeSlot();
  void viewLayersClickedSlot(int, int);

  void plotLayersSelectionChangeSlot();
  void plotLayersClickedSlot(int, int);

  //---

  void writeViewSlot();

  //---

  void plotsSelectionChangeSlot();

  void updatePlotOverlayState();

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

  void writePlotSlot();

  //---

  void viewAnnotationSelectionChangeSlot();
  void plotAnnotationSelectionChangeSlot();

  void raiseViewAnnotationSlot();
  void lowerViewAnnotationSlot();
  void createViewAnnotationSlot();
  void editViewAnnotationSlot();
  void removeViewAnnotationsSlot();

  void raisePlotAnnotationSlot();
  void lowerPlotAnnotationSlot();
  void createPlotAnnotationSlot();
  void editPlotAnnotationSlot();
  void removePlotAnnotationsSlot();

  void writeAnnotationSlot();

  //---

  void palettesComboSlot(int);
  void palettesResetSlot();

  void paletteColorsChangedSlot();

  void updatePalettes();
  void updateInterface();

 private:
  CQColorsEditCanvas  *interfacePlot   () const { return themeWidgets_.interfacePlot; }
  CQColorsEditControl *interfaceControl() const { return themeWidgets_.interfaceControl; }

  CQColorsEditCanvas  *palettesPlot   () const { return themeWidgets_.palettesPlot; }
  CQColorsEditControl *palettesControl() const { return themeWidgets_.palettesControl; }

  void addWidgets();

  void initPropertiesFrame (QFrame *propertiesFrame);
  void initModelsFrame     (QFrame *modelsFrame);
  void initPlotsFrame      (QFrame *plotsFrame);
  void initAnnotationsFrame(QFrame *annotationsFrame);
  void initObjectsFrame    (QFrame *objectsFrame);
  void initThemeFrame      (QFrame *themeFrame);
  void initLayersFrame     (QFrame *layersFrame);
  void initQueryFrame      (QFrame *queryFrame);
  void initErrorsFrame     (QFrame *errorsFrame);

  void updatePaletteWidgets();

  Plot *getPropertiesPlot() const;

  Plot *getSelectedPlot() const;

  void getSelectedPlots(Plots &plots) const;

  void getSelectedAnnotations(Annotations &viewAnnotations, Annotations &plotAnnotations) const;

  CQChartsAnnotation *getSelectedViewAnnotation() const;
  CQChartsAnnotation *getSelectedPlotAnnotation() const;

 private:
  using ViewPropertiesWidget   = CQChartsViewSettingsViewPropertiesWidget;
  using PlotPropertiesWidget   = CQChartsViewSettingsPlotPropertiesWidget;
  using ObjectPropertiesWidget = CQChartsViewSettingsObjectPropertiesWidget;

  using FilterEdit           = CQChartsViewSettingsFilterEdit;
  using ModelTable           = CQChartsViewSettingsModelTable;
  using PlotTable            = CQChartsViewSettingsPlotTable;
  using ViewAnnotationsTable = CQChartsViewSettingsViewAnnotationsTable;
  using PlotAnnotationsTable = CQChartsViewSettingsPlotAnnotationsTable;
  using ViewLayerTable       = CQChartsViewSettingsViewLayerTable;
  using PlotLayerTable       = CQChartsViewSettingsPlotLayerTable;

  struct PropertiesWidgets {
    CQTabSplit*           propertiesSplit  { nullptr }; //!< properties split
//  FilterEdit*           viewFilterEdit   { nullptr }; //!< view settings filter
    ViewPropertiesWidget* viewPropertyTree { nullptr }; //!< view settings tree
    CQTabWidget*          plotsTab         { nullptr }; //!< plots settings tab
//  CQChartsPlotTip*      plotTip          { nullptr }; //!< current plot tip
//  QToolButton*          plotTipButton    { nullptr }; //!< current plot tip button
  };

  struct ModelsWidgets {
    ModelTable*                 modelTable    { nullptr }; //!< model table widget
    QFrame*                     detailsFrame  { nullptr }; //!< model details frame widget
    CQChartsModelDetailsWidget* detailsWidget { nullptr }; //!< model details widget
    QPushButton*                loadButton    { nullptr }; //!< load model button
    QPushButton*                editButton    { nullptr }; //!< edit model button
    QPushButton*                removeButton  { nullptr }; //!< remove model button
    QPushButton*                plotButton    { nullptr }; //!< plot model button
  };

  struct PlotsWidgets {
    PlotTable*     plotTable            { nullptr }; //!< plot table
    QCheckBox*     overlayCheck         { nullptr }; //!< overlay check
    QCheckBox*     x1x2Check            { nullptr }; //!< x1x2 check
    QCheckBox*     y1y2Check            { nullptr }; //!< y1y2 check
    QRadioButton*  placeVerticalRadio   { nullptr }; //!< place vertical radio
    QRadioButton*  placeHorizontalRadio { nullptr }; //!< place horizontal radio
    QRadioButton*  placeGridRadio       { nullptr }; //!< place grid radio
    CQIntegerSpin* placeRowsEdit        { nullptr }; //!< grid number of rows edit
    CQIntegerSpin* placeColumnsEdit     { nullptr }; //!< grid number of columns edit
    QPushButton*   raiseButton          { nullptr }; //!< raise plot button
    QPushButton*   lowerButton          { nullptr }; //!< lower plot button
    QPushButton*   createButton         { nullptr }; //!< create plot button
    QPushButton*   removeButton         { nullptr }; //!< remove plot button
    QPushButton*   writeButton          { nullptr }; //!< write plot button
  };

  struct AnnotationsWidgets {
    CQTabSplit*           split            { nullptr }; //!< tab split
    QFrame*               viewFrame        { nullptr }; //!< view frame
    QFrame*               plotFrame        { nullptr }; //!< plot frame
    ViewAnnotationsTable* viewTable        { nullptr }; //!< view annotations table
    PlotAnnotationsTable* plotTable        { nullptr }; //!< plot annotations table
    QPushButton*          viewRaiseButton  { nullptr }; //!< view annotation raise button
    QPushButton*          viewLowerButton  { nullptr }; //!< view annotation lower button
    QPushButton*          viewCreateButton { nullptr }; //!< view annotation create button
    QPushButton*          viewEditButton   { nullptr }; //!< view annotation edit button
    QPushButton*          viewRemoveButton { nullptr }; //!< view annotation remove button
    QPushButton*          plotRaiseButton  { nullptr }; //!< plot annotation raise button
    QPushButton*          plotLowerButton  { nullptr }; //!< plot annotation lower button
    QPushButton*          plotCreateButton { nullptr }; //!< plot annotation create button
    QPushButton*          plotEditButton   { nullptr }; //!< plot annotation edit button
    QPushButton*          plotRemoveButton { nullptr }; //!< plot annotation remove button
    QPushButton*          writeButton      { nullptr }; //!< view and plot annotations write button
  };

  struct ObjectsWidgets {
    ObjectPropertiesWidget* propertyTree  { nullptr };
    CQPropertyViewModel*    propertyModel { nullptr };
  };

  struct ThemeWidgets {
    CQColorsEditList*    palettesList     { nullptr }; //!< palettes list
    QComboBox*           palettesCombo    { nullptr }; //!< palettes name combo
    CQColorsEditCanvas*  palettesPlot     { nullptr }; //!< current palette plot
    CQColorsEditControl* palettesControl  { nullptr }; //!< current palette control
    CQColorsEditCanvas*  interfacePlot    { nullptr }; //!< interface palette plot
    CQColorsEditControl* interfaceControl { nullptr }; //!< interface palette control
  };

  struct LayersWidgets {
    ViewLayerTable* viewLayerTable { nullptr }; //!< view layer table widget
    PlotLayerTable* plotLayerTable { nullptr }; //!< plot layer table widget
  };

  CQChartsWindow* window_ { nullptr }; //!< parent window

  // widgets
  CQTabWidget*              tab_                { nullptr }; //!< settings/palette tab
  PropertiesWidgets         propertiesWidgets_;              //!< properties widgets
  CQChartsPlotControlFrame* controlFrame_       { nullptr }; //!< control widgets
  ModelsWidgets             modelsWidgets_;                  //!< models widgets
  PlotsWidgets              plotsWidgets_;                   //!< plots widgets
  AnnotationsWidgets        annotationsWidgets_;             //!< annotations widgets
  ObjectsWidgets            objectsWidgets_;                 //!< objects widgets
  ThemeWidgets              themeWidgets_;                   //!< theme widgets
  LayersWidgets             layersWidgets_;                  //!< layers widgets
  QTextEdit*                queryText_          { nullptr }; //!< query text
  CQChartsViewError*        error_              { nullptr }; //!< error widget

  // dialogs
  CQChartsCreateAnnotationDlg* createAnnotationDlg_ { nullptr }; //!< create annotation dialog
  CQChartsEditAnnotationDlg*   editAnnotationDlg_   { nullptr }; //!< edit annotation dialog
  CQChartsEditTitleDlg*        editTitleDlg_        { nullptr }; //!< edit plot title dialog
  CQChartsEditKeyDlg*          editKeyDlg_          { nullptr }; //!< edit plot key dialog
  CQChartsEditAxisDlg*         editXAxisDlg_        { nullptr }; //!< edit plot x axis dialog
  CQChartsEditAxisDlg*         editYAxisDlg_        { nullptr }; //!< edit plot y axis dialog

  QString plotId_; //!< current plot id

  QTimer *updateErrorsTimer_ { nullptr }; //!< update error delay timer

  int maxPlotObjs_ { 100 };
};

//---

/*!
 * \brief View settings view properties widget
 * \ingroup Charts
 */
class CQChartsViewSettingsViewPropertiesWidget : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewSettingsViewPropertiesWidget(CQChartsViewSettings *settings, CQChartsView *view);

  CQChartsView *view() const { return view_; }

  CQChartsPropertyViewTree *propertyTree() const { return propertyTree_; }

 signals:
  void propertyItemSelected(QObject *obj, const QString &path);

 private slots:
  void filterStateSlot(bool show, bool focus);

 private:
  CQChartsView*                   view_         { nullptr };
  CQChartsPropertyViewTree*       propertyTree_ { nullptr };
  CQChartsViewSettingsFilterEdit* filterEdit_   { nullptr };
};

//---

/*!
 * \brief View settings plot properties widget
 * \ingroup Charts
 */
class CQChartsViewSettingsPlotPropertiesWidget : public QFrame {
  Q_OBJECT

 public:
  using Plot = CQChartsPlot;

 public:
  CQChartsViewSettingsPlotPropertiesWidget(CQChartsViewSettings *settings, Plot *plot);

  Plot *plot() const { return plot_; }

  CQChartsPropertyViewTree *propertyTree() const { return propertyTree_; }

 signals:
  void propertyItemSelected(QObject *obj, const QString &path);

 private slots:
  void filterStateSlot(bool show, bool focus);

 private:
  Plot*                           plot_         { nullptr };
  CQChartsPropertyViewTree*       propertyTree_ { nullptr };
  CQChartsViewSettingsFilterEdit* filterEdit_   { nullptr };
};

//---

/*!
 * \brief View settings plot object properties widget
 * \ingroup Charts
 */
class CQChartsViewSettingsObjectPropertiesWidget : public QFrame {
  Q_OBJECT

 public:
  using Plot = CQChartsPlot;

 public:
  CQChartsViewSettingsObjectPropertiesWidget(CQChartsViewSettings *settings);

  Plot *plot() const { return plot_; }

  CQChartsPropertyViewTree *propertyTree() const { return propertyTree_; }

 signals:
  void propertyItemSelected(QObject *obj, const QString &path);

 private slots:
  void filterStateSlot(bool show, bool focus);

 private:
  Plot*                           plot_         { nullptr };
  CQChartsPropertyViewTree*       propertyTree_ { nullptr };
  CQChartsViewSettingsFilterEdit* filterEdit_   { nullptr };
};

//---

/*!
 * \brief View settings filter edit widget
 * \ingroup Charts
 */
class CQChartsViewSettingsFilterEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewSettingsFilterEdit(CQChartsPropertyViewTree *tree);

 private slots:
  void replaceFilterSlot(const QString &text);
  void addFilterSlot(const QString &text);

  void replaceSearchSlot(const QString &text);
  void addSearchSlot(const QString &text);

  void hideFilterSlot();

 private:
  CQChartsPropertyViewTree* tree_       { nullptr };
  CQChartsFilterEdit*       filterEdit_ { nullptr };
};

#endif
