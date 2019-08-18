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
class CQChartsViewSettingsViewLayerTable;
class CQChartsViewSettingsPlotLayerTable;
class CQChartsPlotTip;

class CQChartsModelDetailsWidget;
class CQChartsWindow;
class CQChartsView;
class CQChartsPlot;
class CQChartsAnnotation;
class CQChartsFilterEdit;
class CQChartsLoadModelDlg;
class CQChartsCreateAnnotationDlg;
class CQChartsEditAnnotationDlg;
class CQChartsEditTitleDlg;
class CQChartsEditKeyDlg;
class CQChartsEditAxisDlg;
class CQChartsPropertyViewTree;

class CQColorsEditCanvas;
class CQColorsEditControl;
class CQColorsEditList;

class CQTabWidget;
class CQIntegerSpin;
class CQGroupBox;

class QComboBox;
class QSpinBox;
class QPushButton;
class QToolButton;
class QRadioButton;
class QCheckBox;
class QLabel;

/*!
 * \brief View settings widget
 * \ingroup Charts
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

  CQChartsPropertyViewTree *plotPropertyTree(CQChartsPlot *plot) const;

 signals:
  void propertyItemSelected(QObject *obj, const QString &path);

 private slots:
  void updateModels();

  void invalidateModelDetails();
//void updateModelDetails();

  void plotsTabChangedSlot();

  void updatePlots();
  void updateCurrentPlot();

  void updateAnnotations();

  void updateLayers();

  void viewLayerImageSlot();
  void plotLayerImageSlot();

  void updateSelection();

  void updateView();

  //---

  void modelsSelectionChangeSlot();

  void loadModelSlot();
  void editModelSlot();
  void plotModelSlot();

  //---

  void viewLayersSelectionChangeSlot();
  void viewLayersClickedSlot(int, int);

  void plotLayersSelectionChangeSlot();
  void plotLayersClickedSlot(int, int);

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

  void writePlotSlot();

  //---

  void viewAnnotationSelectionChangeSlot();
  void plotAnnotationSelectionChangeSlot();

  void createAnnotationSlot();
  void editAnnotationSlot();
  void removeAnnotationsSlot();
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
  void initThemeFrame      (QFrame *themeFrame);
  void initLayersFrame     (QFrame *layersFrame);

  void updatePaletteWidgets();

  CQChartsPlot *getPropertiesPlot() const;

  CQChartsPlot *getSelectedPlot() const;

  void getSelectedPlots(Plots &plots) const;

  void getSelectedAnnotations(Annotations &viewAnnotations, Annotations &plotAnnotations) const;

 private:
  using FilterEdit           = CQChartsViewSettingsFilterEdit;
  using ViewPropertiesWidget = CQChartsViewSettingsViewPropertiesWidget;
  using ModelTable           = CQChartsViewSettingsModelTable;
  using PlotTable            = CQChartsViewSettingsPlotTable;
  using ViewAnnotationsTable = CQChartsViewSettingsViewAnnotationsTable;
  using PlotAnnotationsTable = CQChartsViewSettingsPlotAnnotationsTable;
  using ViewLayerTable       = CQChartsViewSettingsViewLayerTable;
  using PlotLayerTable       = CQChartsViewSettingsPlotLayerTable;

  struct PropertiesWidgets {
    FilterEdit*           viewFilterEdit   { nullptr }; //!< view settings filter
    ViewPropertiesWidget* viewPropertyTree { nullptr }; //!< view settings tree
    CQTabWidget*          plotsTab         { nullptr }; //!< plots settings tab
//  CQChartsPlotTip*      plotTip          { nullptr }; //!< current plot tip
//  QToolButton*          plotTipButton    { nullptr }; //!< current plot tip button
  };

  struct ModelsWidgets {
    ModelTable*                 modelTable    { nullptr }; //!< model table
    QFrame*                     detailsFrame  { nullptr }; //!< model details frame
    CQChartsModelDetailsWidget* detailsWidget { nullptr }; //!< model details
    QPushButton*                editButton    { nullptr }; //!< edit model
    QPushButton*                plotButton    { nullptr }; //!< edit model
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
    QPushButton*   removeButton         { nullptr }; //!< remove plot button
  };

  struct AnnotationsWidgets {
    ViewAnnotationsTable* viewTable    { nullptr }; //!< view annotations table
    PlotAnnotationsTable* plotTable    { nullptr }; //!< plot annotations table
    QPushButton*          editButton   { nullptr }; //!< edit button
    QPushButton*          removeButton { nullptr }; //!< remove button
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

  CQChartsWindow*              window_              { nullptr }; //!< parent window
  CQTabWidget*                 tab_                 { nullptr }; //!< settings/palette tab
  PropertiesWidgets            propertiesWidgets_;               //!< properties widgets
  ModelsWidgets                modelsWidgets_;                   //!< models widgets
  PlotsWidgets                 plotsWidgets_;                    //!< plots widgets
  AnnotationsWidgets           annotationsWidgets_;              //!< annotations widgets
  ThemeWidgets                 themeWidgets_;                    //!< theme widgets
  LayersWidgets                layersWidgets_;                   //!< layers widgets
  CQChartsLoadModelDlg*        loadModelDlg_        { nullptr }; //!< load dialog
  CQChartsCreateAnnotationDlg* createAnnotationDlg_ { nullptr }; //!< create annotation dialog
  CQChartsEditAnnotationDlg*   editAnnotationDlg_   { nullptr }; //!< edit annotation dialog
  CQChartsEditTitleDlg*        editTitleDlg_        { nullptr }; //!< edit plot title dialog
  CQChartsEditKeyDlg*          editKeyDlg_          { nullptr }; //!< edit plot key dialog
  CQChartsEditAxisDlg*         editXAxisDlg_        { nullptr }; //!< edit plot x axis dialog
  CQChartsEditAxisDlg*         editYAxisDlg_        { nullptr }; //!< edit plot y axis dialog
  QString                      plotId_;                          //!< current plot id
//bool                         modelDetailsValid_   { false };   //!< model details valid
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
  CQChartsViewSettingsPlotPropertiesWidget(CQChartsViewSettings *settings, CQChartsPlot *plot);

  CQChartsPlot *plot() const { return plot_; }

  CQChartsPropertyViewTree *propertyTree() const { return propertyTree_; }

 signals:
  void propertyItemSelected(QObject *obj, const QString &path);

 private slots:
  void filterStateSlot(bool show, bool focus);

 private:
  CQChartsPlot*                   plot_         { nullptr };
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
