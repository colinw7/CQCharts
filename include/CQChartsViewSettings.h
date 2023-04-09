#ifndef CQChartsViewSettings_H
#define CQChartsViewSettings_H

#include <QFrame>
#include <QPointer>

class CQChartsGlobalPropertiesWidget;
class CQChartsViewPropertiesControl;
class CQChartsPlotPropertiesControl;
class CQChartsObjectPropertiesWidget;
class CQChartsSelectedObjectWidget;

class CQChartsModelTableControl;
class CQChartsModelDetailsWidget;

class CQChartsPlotTableControl;
class CQChartsAnnotationsControl;
class CQChartsLayerTableControl;

class CQChartsSymbolSetsList;
class CQChartsSymbolsListControl;
class CQChartsSymbolEditor;

class CQChartsViewQuery;
class CQChartsViewError;

class CQChartsWindow;
class CQChartsView;
class CQChartsPlot;
class CQChartsAnnotation;
class CQChartsPropertyViewTree;
class CQChartsPlotCustomControls;
class CQChartsPlotControlFrame;
class CQCharts;

class CQChartsPaletteControl;
class CQChartsInterfaceControl;
class CQColorsEditList;
class CQPropertyViewModel;

//class CQTabWidget;
class CQTabBarWidget;
class CQTabSplit;

class QPushButton;
class QVBoxLayout;
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

  CQChartsPropertyViewTree *globalPropertyTree() const;
  CQChartsPropertyViewTree *viewPropertyTree() const;
  CQChartsPropertyViewTree *plotPropertyTree(Plot *plot) const;

  // show query tab on query
  void showQueryTab();

  // show error tab on error
  void showErrorsTab();

  Plot *currentPlot(bool remap=true) const;

  QWidget *widgetsFrame() const { return widgetsFrame_; }

 Q_SIGNALS:
  void objectsPropertyItemChanged(QObject *obj, const QString &path);

  void propertyItemSelected(QObject *obj, const QString &path);

 public Q_SLOTS:
  void updatePlotObjects();

 private Q_SLOTS:
  void updateModels();
  void updateModelsData();

  void invalidateModelDetails(bool changed=true);
//void updateModelDetails();

  void updatePlots();
  void updateCurrentPlot();

  void updatePlotControls();

  void symbolListSymbolChangeSlot();

  void updateLayers();

  void updatePlotSelection();

  void showQueryText(const QString &text);

  void updateErrorsSlot();
  void updateErrors();

  void updateSelection();
  void updateTabs();

  void updateView();

  //---

  void updatePalettes();
  void updateInterface();

 private:
  struct FrameLayout {
    QFrame*      frame  { nullptr };
    QVBoxLayout* layout { nullptr };

    FrameLayout(QFrame *frame, QVBoxLayout *layout) :
     frame(frame), layout(layout) {
    }
  };

  void addWidgets();

  void initControlsFrame   (QFrame *controlsFrame);
  void initWidgetsFrame    (QFrame *widgetsFrame);
  void initPropertiesFrame (QFrame *propertiesFrame);
  void initModelsFrame     (QFrame *modelsFrame);
  void initPlotsFrame      (QFrame *plotsFrame);
  void initAnnotationsFrame(QFrame *annotationsFrame);
  void initObjectsFrame    (QFrame *objectsFrame);
  void initColorsFrame     (QFrame *colorsFrame);
  void initSymbolsFrame    (QFrame *symbolsFrame);
  void initLayersFrame     (QFrame *layersFrame);
  void initQueryFrame      (QFrame *queryFrame);
  void initErrorsFrame     (QFrame *errorsFrame);

  Plot *getSelectedPlot() const;

  void getSelectedPlots(Plots &plots) const;

  CQTabSplit *createTabSplit(const QString &name, bool tabbed) const;

  FrameLayout addSplitFrame(CQTabSplit *split, const QString &label, const QString &name) const;

 private:
  using GlobalPropertiesWidget = CQChartsGlobalPropertiesWidget;
  using ViewPropertiesControl  = CQChartsViewPropertiesControl;
  using PlotPropertiesControl  = CQChartsPlotPropertiesControl;
  using ObjectPropertiesWidget = CQChartsObjectPropertiesWidget;
  using SelectedObjectWidget   = CQChartsSelectedObjectWidget;

  using ModelTable         = CQChartsModelTableControl;
  using ModelDetailsWidget = CQChartsModelDetailsWidget;

  using PlotTable          = CQChartsPlotTableControl;
  using AnnotationsControl = CQChartsAnnotationsControl;
  using LayerTableControl  = CQChartsLayerTableControl;
  using PlotControlFrame   = CQChartsPlotControlFrame;
  using PlotCustomControls = CQChartsPlotCustomControls;
  using SymbolSetsList     = CQChartsSymbolSetsList;
  using SymbolsList        = CQChartsSymbolsListControl;
  using ViewQuery          = CQChartsViewQuery;
  using ViewError          = CQChartsViewError;

  struct PropertiesWidgets {
    CQTabSplit*             propertiesSplit    { nullptr }; //!< properties split
    GlobalPropertiesWidget* globalPropertyTree { nullptr }; //!< global settings tree
    ViewPropertiesControl*  viewControl        { nullptr }; //!< view settings control
    PlotPropertiesControl*  plotsControl       { nullptr }; //!< plots settings control
  };

  struct ModelsWidgets {
    ModelTable*         modelTable    { nullptr }; //!< model table widget
    ModelDetailsWidget* detailsWidget { nullptr }; //!< model details widget
  };

  struct PlotsWidgets {
    PlotTable* plotTable { nullptr }; //!< plot table
  };

  struct AnnotationsWidgets {
    CQChartsAnnotationsControl* control { nullptr }; //!< annotations control
  };

  struct ObjectsWidgets {
    SelectedObjectWidget*   selectedObject { nullptr };
    ObjectPropertiesWidget* propertyTree   { nullptr };
    CQPropertyViewModel*    propertyModel  { nullptr };
  };

  struct ThemeWidgets {
    CQColorsEditList*         palettesList     { nullptr }; //!< palettes list
    CQChartsPaletteControl   *palettesControl  { nullptr }; //!< named palette control
    CQChartsInterfaceControl *interfaceControl { nullptr }; //!< interface palette control
  };

  struct LayersWidgets {
    LayerTableControl* layerTableControl { nullptr }; //!< layer table control
  };

 private:
  SymbolsList *symbolsList() const { return symbolsList_; }

 private:
  using PlotCustomControlsP = QPointer<PlotCustomControls>;
  using TabNum              = std::map<QString, int>;

  CQChartsWindow* window_ { nullptr }; //!< parent window

  // widgets
//CQTabWidget*          tab_                { nullptr }; //!< settings/palette tab
  CQTabBarWidget*       tab_                { nullptr }; //!< settings/palette tab
  PropertiesWidgets     propertiesWidgets_;              //!< properties widgets
  PlotControlFrame*     quickControlFrame_  { nullptr }; //!< quick control widgets
  QFrame*               customControlFrame_ { nullptr }; //!< custom control widgets
  QFrame*               widgetsFrame_       { nullptr }; //!< custom widgets frame
  QFrame*               propertiesFrame_    { nullptr }; //!< properties frame
  PlotCustomControlsP   plotCustomControls_;             //!< plot custom controls
  QFrame*               modelsFrame_        { nullptr }; //!< models frame
  ModelsWidgets         modelsWidgets_;                  //!< models widgets
  QFrame*               plotsFrame_         { nullptr }; //!< plots frame
  PlotsWidgets          plotsWidgets_;                   //!< plots widgets
  QFrame*               annotationsFrame_   { nullptr }; //!< annotations frame
  AnnotationsWidgets    annotationsWidgets_;             //!< annotations widgets
  QFrame*               objectsFrame_       { nullptr }; //!< objects frame
  ObjectsWidgets        objectsWidgets_;                 //!< objects widgets
  QFrame*               colorsFrame_        { nullptr }; //!< colors frame
  ThemeWidgets          themeWidgets_;                   //!< theme widgets
  QFrame*               symbolsFrame_       { nullptr }; //!< symbols frame
  SymbolSetsList*       symbolSetsList_     { nullptr }; //!< symbol sets list
  SymbolsList*          symbolsList_        { nullptr }; //!< symbols list
  CQChartsSymbolEditor* symbolEdit_         { nullptr }; //!< symbol editor
  QFrame*               layersFrame_        { nullptr }; //!< layer frame
  LayersWidgets         layersWidgets_;                  //!< layers widgets
  QFrame*               queryFrame_         { nullptr }; //!< query frame
  ViewQuery*            query_              { nullptr }; //!< query widget
  QFrame*               errorsFrame_        { nullptr }; //!< errors frame
  ViewError*            error_              { nullptr }; //!< error widget

  TabNum  tabNum_;                  //!< current tab number
  uint    settingsTabs_ { 0xFFFF }; //!< displayed tabs

  QString plotId_; //!< current plot id

  QTimer *updateErrorsTimer_ { nullptr }; //!< update error delay timer
};

//---

#include <QTextBrowser>

class CQChartsPlotObj;

class CQChartsSelectedObjectWidget : public QTextBrowser {
  Q_OBJECT

 public:
  using PlotObjs = std::vector<CQChartsPlotObj *>;

 public:
  CQChartsSelectedObjectWidget(QWidget *parent=nullptr);

  void setObjects(const PlotObjs &objs);
};

#endif
