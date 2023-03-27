#include <CQChartsViewSettings.h>

#include <CQChartsPlotCustomControls.h>
#include <CQChartsPlotControlWidgets.h>

#include <CQChartsGlobalPropertiesWidget.h>
#include <CQChartsViewPropertiesWidget.h>
#include <CQChartsPlotPropertiesWidget.h>
#include <CQChartsObjectPropertiesWidget.h>

#include <CQChartsModelDetailsWidget.h>

#include <CQChartsPlotTable.h>
#include <CQChartsAnnotationsTable.h>
#include <CQChartsLayerTable.h>

#include <CQChartsSymbolSetsList.h>
#include <CQChartsSymbolsList.h>
#include <CQChartsSymbolEditor.h>
#include <CQChartsSymbolSet.h>

#include <CQChartsViewQuery.h>
#include <CQChartsViewError.h>

#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsGroupPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsPropertyViewTree.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsAnnotation.h>
#include <CQChartsKey.h>
#include <CQChartsModelTable.h>
#include <CQChartsVariant.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>

#include <CQChartsPaletteControl.h>
#include <CQChartsInterfaceControl.h>
#include <CQChartsPaletteCanvas.h>
#include <CQColorsEditList.h>
#include <CQColors.h>
#include <CQColorsTheme.h>

#include <CQPropertyViewItem.h>
#include <CQPropertyViewModel.h>

//#include <CQTabWidget.h>
#include <CQTabBarWidget.h>
#include <CQTabSplit.h>
#include <CQUtil.h>
#include <CQGroupBox.h>
#include <CQToolTip.h>

#include <QVBoxLayout>
#include <QTimer>

#include <iostream>

CQChartsViewSettings::
CQChartsViewSettings(CQChartsWindow *window) :
 QFrame(window), window_(window)
{
  auto *view   = window_->view();
  auto *charts = view->charts();

  //---

  connect(charts, SIGNAL(modelDataChanged()), this, SLOT(updateModels()));
  connect(charts, SIGNAL(currentModelChanged(int)), this, SLOT(invalidateModelDetails()));
  connect(charts, SIGNAL(modelDataDataChanged()), this, SLOT(updateModelsData()));

  connect(view, SIGNAL(plotsChanged()), this, SLOT(updatePlots()));
  connect(view, SIGNAL(plotsReordered()), this, SLOT(updatePlots()));

  connect(view, SIGNAL(connectDataChanged()), this, SLOT(updatePlots()));

  connect(view, SIGNAL(currentPlotChanged()), this, SLOT(updateCurrentPlot()));

  connect(view, SIGNAL(selectionChanged()), this, SLOT(updateSelection()));

  connect(view, SIGNAL(settingsTabsChanged()), this, SLOT(updateTabs()));

  connect(window, SIGNAL(themePalettesChanged()), this, SLOT(updatePalettes()));
  connect(charts, SIGNAL(interfaceThemeChanged()), this, SLOT(updateInterface()));

  //--

  setObjectName("settings");

  setAutoFillBackground(true);

  addWidgets();

  //--

  updateErrorsTimer_ = new QTimer;

  updateErrorsTimer_->setSingleShot(true);

  connect(updateErrorsTimer_, SIGNAL(timeout()), this, SLOT(updateErrors()));

  //---

  updateTabs();
}

CQChartsViewSettings::
~CQChartsViewSettings()
{
//delete propertiesWidgets_.plotTip;

  delete updateErrorsTimer_;

  delete objectsWidgets_.propertyModel;
}

void
CQChartsViewSettings::
addWidgets()
{
  // add main tab widget
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //tab_ = CQUtil::makeWidget<CQTabWidget>("tab");
  tab_ = CQUtil::makeWidget<CQTabBarWidget>("tab");

  tab_->setButtonStyle(Qt::ToolButtonTextOnly);
  tab_->setFlowTabs(true);

  layout->addWidget(tab_);

  tab_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  //----

  // add frame tab
  int tabNum = 0;

  auto addTab = [&](const QString &name) {
    auto areaName  = name + "Area" ; areaName [0] = areaName [0].toLower();
    auto frameName = name + "Frame"; frameName[0] = frameName[0].toLower();

    auto *area  = CQUtil::makeWidget<QScrollArea>(areaName);
    auto *frame = CQUtil::makeWidget<QFrame>(area, frameName);

    area->setWidget(frame);
    area->setWidgetResizable(true);

    tab_->addTab(area, name);

    tabNum_[name] = tabNum++;

    return frame;
  };

  //--

  // Controls
  initControlsFrame(addTab("Controls"));

  // Widgets
  initWidgetsFrame(addTab("Widgets"));

  // Properties Tab
  initPropertiesFrame(addTab("Properties"));

  // Models Tab
  initModelsFrame(addTab("Models"));

  // Plots Tab
  initPlotsFrame(addTab("Plots"));

  // Annotations Tab
  initAnnotationsFrame(addTab("Annotations"));

  // Objects Tab
  initObjectsFrame(addTab("Objects"));

  // Themes/Colors Tab
  initColorsFrame(addTab("Colors"));

  // Symbols Tab
  initSymbolsFrame(addTab("Symbols"));

  // Layers Tab
  initLayersFrame(addTab("Layers"));

  // Query Tab
  initQueryFrame(addTab("Query"));

  // Errors Tab
  initErrorsFrame(addTab("Errors"));

  //----

  updateModelsData();

  //---

  updatePlotControls();

  updatePlotObjects();

  updateLayers();

  //---

  updateErrors();
}

//------

void
CQChartsViewSettings::
initControlsFrame(QFrame *controlsFrame)
{
  customControlFrame_ = controlsFrame;

  (void) CQUtil::makeLayout<QVBoxLayout>(customControlFrame_);
}

//------

void
CQChartsViewSettings::
initWidgetsFrame(QFrame *widgetsFrame)
{
  widgetsFrame_ = widgetsFrame;

  (void) CQUtil::makeLayout<QVBoxLayout>(widgetsFrame_, 0, 0);
}

//---

void
CQChartsViewSettings::
initPropertiesFrame(QFrame *propertiesFrame)
{
  propertiesFrame_ = propertiesFrame;

  //---

  auto *view   = window_->view();
  auto *charts = view->charts();

  //---

  auto *propertiesLayout = CQUtil::makeLayout<QVBoxLayout>(propertiesFrame, 2, 2);

  //--

  propertiesWidgets_.propertiesSplit = createTabSplit("propertiesSplit", /*tabbed*/true);

  propertiesLayout->addWidget(propertiesWidgets_.propertiesSplit);

  //----

  // Create Global Properties Split Frame
  auto globalFrame = addSplitFrame(propertiesWidgets_.propertiesSplit, "Global", "globalFrame");

  propertiesWidgets_.globalPropertyTree = new GlobalPropertiesWidget(charts);

  globalFrame.layout->addWidget(propertiesWidgets_.globalPropertyTree);

  //----

  // Create View Properties Split Frame
  auto viewFrame = addSplitFrame(propertiesWidgets_.propertiesSplit, "View", "viewFrame");

  propertiesWidgets_.viewControl = new ViewPropertiesControl(view);

  viewFrame.layout->addWidget(propertiesWidgets_.viewControl);

  //----

  // Create Plots Properties Split Frame
  auto plotsFrame = addSplitFrame(propertiesWidgets_.propertiesSplit, "Plots", "plotsFrame");

  propertiesWidgets_.plotsControl = new PlotPropertiesControl(view);

  plotsFrame.layout->addWidget(propertiesWidgets_.plotsControl);

  propertiesWidgets_.plotsControl->setView(view);

  connect(propertiesWidgets_.plotsControl,
          SIGNAL(propertyItemSelected(QObject *, const QString &)),
          this, SIGNAL(propertyItemSelected(QObject *, const QString &)));

  //--

  // Add Quick control Split Frame
  quickControlFrame_ = CQUtil::makeWidget<CQChartsPlotControlFrame>(this, "quickControlFrame");

  propertiesWidgets_.propertiesSplit->addWidget(quickControlFrame_, "Quick Controls");

  propertiesWidgets_.propertiesSplit->setCurrentIndex(tabNum_["Properties"]);

  //--

  int globalSize = int(INT_MAX*0.1);
  int viewSize   = int(INT_MAX*0.2);
  int quickSize  = int(INT_MAX*0.1); // quick controls
  int plotSize   = INT_MAX - viewSize - quickSize;

  propertiesWidgets_.propertiesSplit->
    setSizes(QList<int>({globalSize, viewSize, plotSize, quickSize}));

  propertiesWidgets_.propertiesSplit->setCurrentIndex(2);
}

//---

void
CQChartsViewSettings::
initModelsFrame(QFrame *modelsFrame)
{
  modelsFrame_ = modelsFrame;

  //---

  auto *view   = window_->view();
  auto *charts = view->charts();

  //---

  auto *modelsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(modelsFrame, 2, 2);

  //----

  // Create Models Split Frame
  auto *modelsSplit = createTabSplit("modelsSplit", /*tabbed*/false);

  modelsFrameLayout->addWidget(modelsSplit);

  //---

  // Create Model Control Split Frame
  auto modelsModelsFrame = addSplitFrame(modelsSplit, "Models", "modelsModelsFrame");

  modelsWidgets_.modelTable = new CQChartsModelTableControl(charts);

  modelsModelsFrame.layout->addWidget(modelsWidgets_.modelTable);

  //---

  // Create Model Details Split Frame
  auto detailsFrame = addSplitFrame(modelsSplit, "Details", "detailsFrame");

  modelsWidgets_.detailsWidget = new CQChartsModelDetailsWidget(charts);

  detailsFrame.layout->addWidget(modelsWidgets_.detailsWidget);

  //---

  modelsSplit->setSizes(QList<int>({int(INT_MAX*0.6), int(INT_MAX*0.4)}));
}

//---

void
CQChartsViewSettings::
initPlotsFrame(QFrame *plotsFrame)
{
  plotsFrame_ = plotsFrame;

  //---

  auto *view = window_->view();

  //---

  auto *plotsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(plotsFrame, 2, 2);

  //----

  auto *plotsGroup       = CQUtil::makeLabelWidget<CQGroupBox>("Plots", "plotsGroup");
  auto *plotsGroupLayout = CQUtil::makeLayout<QVBoxLayout>(plotsGroup, 2, 2);

  plotsFrameLayout->addWidget(plotsGroup);

  //--

  plotsWidgets_.plotTable = new CQChartsPlotTableControl;

  plotsGroupLayout->addWidget(plotsWidgets_.plotTable);

  plotsWidgets_.plotTable->setView(view);
}

//---

void
CQChartsViewSettings::
initAnnotationsFrame(QFrame *annotationsFrame)
{
  annotationsFrame_ = annotationsFrame;

  //---

  auto *view = window_->view();

  //---

  auto *annotationsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(annotationsFrame, 2, 2);

  //---

  // annotations control
  annotationsWidgets_.control = new CQChartsAnnotationsControl;

  annotationsFrameLayout->addWidget(annotationsWidgets_.control);

  annotationsWidgets_.control->setView(view);
}

//---

void
CQChartsViewSettings::
initObjectsFrame(QFrame *objectsFrame)
{
  objectsFrame_ = objectsFrame;

  //---

  // TODO: add Objects group box

  auto *objectsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(objectsFrame, 2, 2);

  //---

  // add object properties control
  objectsWidgets_.propertyTree = new ObjectPropertiesWidget;

  objectsFrameLayout->addWidget(objectsWidgets_.propertyTree);
}

//---

void
CQChartsViewSettings::
initColorsFrame(QFrame *colorsFrame)
{
  colorsFrame_ = colorsFrame;

  //---

  auto *view = window_->view();

  //--

  auto *colorsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(colorsFrame, 2, 2);

  //--

  auto *colorsSplit = createTabSplit("colorsSplit", /*tabbed*/true);

  colorsFrameLayout->addWidget(colorsSplit);

  //---

  // create palettes list
  themeWidgets_.palettesList = new CQColorsEditList(this);

  colorsSplit->addWidget(themeWidgets_.palettesList, "Theme");

  //--

  // tab for palettes (palette viewer/editor)
  themeWidgets_.palettesControl = new CQChartsPaletteControl(this);

  colorsSplit->addWidget(themeWidgets_.palettesControl, "Palettes");

  themeWidgets_.palettesControl->setView(view);

  //----

  // tab for interface (palette viewer/editor)
  themeWidgets_.interfaceControl = new CQChartsInterfaceControl(this);

  colorsSplit->addWidget(themeWidgets_.interfaceControl, "Interface");

  themeWidgets_.interfaceControl->setView(view);
}

//---

void
CQChartsViewSettings::
initSymbolsFrame(QFrame *symbolsFrame)
{
  symbolsFrame_ = symbolsFrame;

  //---

  auto *view   = window_->view();
  auto *charts = view->charts();

  //---

  auto *symbolsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(symbolsFrame, 2, 2);

  //---

  auto *symbolsSplit = createTabSplit("symbolsSplit", /*tabbed*/false);

  symbolsFrameLayout->addWidget(symbolsSplit);

  //---

  // List of all symbol sets
  // TODO: update on add/remove symbol set
  symbolSetsList_ = new CQChartsSymbolSetsList(charts);

  symbolsSplit->addWidget(symbolSetsList_, "Symbol Sets");

  //---

  auto symbolsSubFrame = addSplitFrame(symbolsSplit, "Symbols", "symbolsFrame");

  //--

  // List of all symbols in set
  symbolsList_ = new CQChartsSymbolsListControl(charts);

  symbolsSubFrame.layout->addWidget(symbolsList_);

  connect(symbolsList_, SIGNAL(symbolChanged()), this, SLOT(symbolListSymbolChangeSlot()));

  //---

  // Symbol editor
  symbolEdit_ = new CQChartsSymbolEditor(charts);

  symbolsSplit->addWidget(symbolEdit_, "Edit Symbol");

  //----

  connect(symbolSetsList_, SIGNAL(nameChanged(const QString &)),
          symbolsList_, SLOT(setSetNameSlot(const QString &)));

  symbolSetsList_->setCurrentRow(0);

  //---

  int size1 = int(INT_MAX*0.20);
  int size2 = int(INT_MAX*0.45);
  int size3 = INT_MAX - size1 - size2;

  symbolsSplit->setSizes(QList<int>({size1, size2, size3}));
}

//---

void
CQChartsViewSettings::
initLayersFrame(QFrame *layersFrame)
{
  layersFrame_ = layersFrame;

  //---

  auto *view = window_->view();

  //---

  auto *layersFrameLayout = CQUtil::makeLayout<QVBoxLayout>(layersFrame, 2, 2);

  //---

  layersWidgets_.layerTableControl = new CQChartsLayerTableControl;

  layersFrameLayout->addWidget(layersWidgets_.layerTableControl);

  layersWidgets_.layerTableControl->setView(view);
}

//---

void
CQChartsViewSettings::
initQueryFrame(QFrame *queryFrame)
{
  queryFrame_ = queryFrame;

  //---

  auto *view = window_->view();

  //---

  auto *queryFrameLayout = CQUtil::makeLayout<QVBoxLayout>(queryFrame, 2, 2);

  //---

  query_ = new CQChartsViewQuery(view);

  queryFrameLayout->addWidget(query_);

  //---

  connect(view, SIGNAL(showQueryText(const QString &)),
          this, SLOT(showQueryText(const QString &)));
}

void
CQChartsViewSettings::
showQueryText(const QString &text)
{
  query_->setText(text);

  showQueryTab();
}

void
CQChartsViewSettings::
showQueryTab()
{
  tab_->setCurrentIndex(tabNum_["Query"]);
}

//---

void
CQChartsViewSettings::
initErrorsFrame(QFrame *errorsFrame)
{
  errorsFrame_ = errorsFrame;

  //---

  auto *view = window_->view();

  //---

  auto *errorsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(errorsFrame, 2, 2);

  //---

  error_ = new CQChartsViewError(view);

  errorsFrameLayout->addWidget(error_);

  //---

  connect(view, SIGNAL(updateErrors()), this, SLOT(updateErrorsSlot()));
}

void
CQChartsViewSettings::
updateErrorsSlot()
{
  updateErrorsTimer_->start(250);
}

void
CQChartsViewSettings::
updateErrors()
{
  auto *view = window_->view();

  //---

  bool hasErrors = false;

  for (auto &plot : view->plots()) {
    if (plot->hasErrors()) {
      hasErrors = true;
      break;
    }
  }

  if (window_)
    window_->setHasErrors(hasErrors);

  error_->updatePlots();
}

void
CQChartsViewSettings::
showErrorsTab()
{
  tab_->setCurrentIndex(tabNum_["Errors"]);
}

//------

CQChartsPropertyViewTree *
CQChartsViewSettings::
viewPropertyTree() const
{
  return propertiesWidgets_.viewControl->propertiesWidget()->propertyTree();
}

CQChartsPropertyViewTree *
CQChartsViewSettings::
plotPropertyTree(CQChartsPlot *plot) const
{
  int ind;

  return propertiesWidgets_.plotsControl->getPlotPropertyViewTree(plot, ind);
}

//------

// models changed
void
CQChartsViewSettings::
updateModels()
{
  invalidateModelDetails(false);
}

// model data changed
void
CQChartsViewSettings::
updateModelsData()
{
  invalidateModelDetails(true);
}

void
CQChartsViewSettings::
invalidateModelDetails(bool changed)
{
  auto *view   = window_->view();
  auto *charts = view->charts();

  //---

  auto *modelData = charts->currentModelData();

  modelsWidgets_.detailsWidget->invalidateModelData(modelData, /*invalidate*/changed);
}

//------

void
CQChartsViewSettings::
updatePlots()
{
  // update plots to plot table (id, type and state)
  propertiesWidgets_.plotsControl->updatePlots();

  //---

  updatePlotControls();

  updatePlotObjects();

  updateLayers();
}

void
CQChartsViewSettings::
updateCurrentPlot()
{
  auto *view = window_->view();
  assert(view);

  if (plotId_.length()) {
    auto *plot = view->getPlotForId(plotId_);

    if (plot) {
      disconnect(plot, SIGNAL(controlColumnsChanged()), this, SLOT(updatePlotControls()));
      disconnect(plot, SIGNAL(plotObjsAdded()), this, SLOT(updatePlotObjects()));
      disconnect(plot, SIGNAL(layersChanged()), this, SLOT(updateLayers()));
    }
  }

  //---

  auto *plot = view->currentPlot();

  plotId_ = (plot ? plot->id() : "");

  if (plot) {
    connect(plot, SIGNAL(controlColumnsChanged()), this, SLOT(updatePlotControls()));
    connect(plot, SIGNAL(plotObjsAdded()), this, SLOT(updatePlotObjects()));
    connect(plot, SIGNAL(layersChanged()), this, SLOT(updateLayers()));
    connect(plot, SIGNAL(customWidgetChanged()), this, SLOT(updatePlotControls()));
  }

  //---

  updatePlotControls();

  updatePlotObjects();

  updateLayers();
}

CQChartsPlot *
CQChartsViewSettings::
currentPlot(bool remap) const
{
  auto *view = window_->view();
  if (! view) return nullptr;

  return view->currentPlot(remap);
}

//------

void
CQChartsViewSettings::
updatePlotControls()
{
  // add controls for plot and child plots
  auto *plot = currentPlot();

  auto *rootPlot = (plot ? const_cast<Plot *>(plot->rootPlot()) : nullptr);

  auto *controlPlot = (rootPlot ? rootPlot : plot);

  //---

  annotationsWidgets_.control->setPlot(plot);

  quickControlFrame_->setPlot(controlPlot);
  quickControlFrame_->setPlotControls();

  //---

  if (! plotCustomControls_ || plotCustomControls_->plot() != controlPlot) {
    delete plotCustomControls_;

    plotCustomControls_ = (controlPlot ? controlPlot->createCustomControls() : nullptr);

    if (plotCustomControls_)
      customControlFrame_->layout()->addWidget(plotCustomControls_);
  }

  //---

  int viewSize  = int(INT_MAX*0.3);
  int quickSize = 0;

  if (quickControlFrame_->numIFaces() > 0) {
    if (! propertiesWidgets_.propertiesSplit->hasWidget(quickControlFrame_)) {
      propertiesWidgets_.propertiesSplit->addWidget(quickControlFrame_, "Quick Controls");

      quickSize = int(INT_MAX*0.1); // quick controls

      quickControlFrame_->setVisible(true);
    }
  }
  else {
    if (propertiesWidgets_.propertiesSplit->hasWidget(quickControlFrame_)) {
      propertiesWidgets_.propertiesSplit->removeWidget(quickControlFrame_, /*delete*/false);

      quickControlFrame_->setVisible(false);
    }
  }

  int plotSize = INT_MAX - viewSize - quickSize;

  QList<int> sizes;

  sizes << viewSize << plotSize;

  if (quickSize) sizes << quickSize;

  propertiesWidgets_.propertiesSplit->setSizes(sizes);

  //---

  // update custom widget
  QWidget *customWidget = nullptr;

  if (plot)
    customWidget = plot->customWidget().widget();

  if (customWidget)
    widgetsFrame_->layout()->addWidget(customWidget);
  else {
    QLayoutItem *child;

    while ((child = widgetsFrame_->layout()->takeAt(0)) != nullptr)
      delete child;
  }
}

//------

// called when plot objects added
void
CQChartsViewSettings::
updatePlotObjects()
{
  if (objectsWidgets_.propertyModel)
    disconnect(objectsWidgets_.propertyModel, SIGNAL(valueChanged(QObject *, const QString &)),
               this, SIGNAL(objectsPropertyItemChanged(QObject *, const QString &)));

  objectsWidgets_.propertyTree->propertyTree()->setPropertyModel(nullptr);

  delete objectsWidgets_.propertyModel;

  objectsWidgets_.propertyModel = nullptr;

  //---

  int maxPlotObjects = window_->viewSettingsMaxObjects();

  auto *plot = currentPlot(/*remap*/false);

  if (plot) {
    CQChartsPlot::PlotObjs addPlotObjs;

    for (auto &obj : plot->plotObjects()) {
      if (obj->detailHint() == CQChartsPlotObj::DetailHint::MAJOR) {
        if (window_->isViewSettingsMajorObjects())
          addPlotObjs.push_back(obj);
      }
      else {
        if (window_->isViewSettingsMinorObjects())
          addPlotObjs.push_back(obj);
      }
    }

    if (int(addPlotObjs.size()) < maxPlotObjects) {
      objectsWidgets_.propertyModel = new CQPropertyViewModel;

      for (auto &obj : addPlotObjs)
        obj->addProperties(objectsWidgets_.propertyModel, obj->id());
    }
  }

  objectsWidgets_.propertyTree->propertyTree()->setPropertyModel(objectsWidgets_.propertyModel);

  if (objectsWidgets_.propertyModel)
    connect(objectsWidgets_.propertyModel, SIGNAL(valueChanged(QObject *, const QString &)),
            this, SIGNAL(objectsPropertyItemChanged(QObject *, const QString &)));
}

//------

void
CQChartsViewSettings::
updateSelection()
{
  window_->selectPropertyObjects();
}

//------

void
CQChartsViewSettings::
updateTabs()
{
  auto *view = window_->view();
  if (! view) return;

  auto settingsTabs = view->settingsTabs();

  if (uint(settingsTabs) == settingsTabs_)
    return;

  settingsTabs_ = uint(settingsTabs);

  //---

  int ind = tab_->currentIndex();

  auto tabName = tab_->tabText(ind);

  //---

  tab_->clear();

  tabNum_.clear();

  int tabNum = 0;

  ind = -1;

  auto addTab = [&](QFrame *frame, const QString &name) {
    auto *stack = frame->parentWidget();
    auto *area  = (stack ? stack->parentWidget() : nullptr);

    assert(area && qobject_cast<QScrollArea *>(area));

    tab_->addTab(area, name);

    if (name == tabName)
      ind = tabNum;

    tabNum_[name] = tabNum++;
  };

  if (settingsTabs & CQChartsView::SettingsTab::CONTROLS)
    addTab(customControlFrame_, "Controls");

  if (settingsTabs & CQChartsView::SettingsTab::WIDGETS)
    addTab(widgetsFrame_, "Widgets");

  if (settingsTabs & CQChartsView::SettingsTab::PROPERTIES)
    addTab(propertiesFrame_, "Properties");

  if (settingsTabs & CQChartsView::SettingsTab::MODELS)
    addTab(modelsFrame_, "Models");

  if (settingsTabs & CQChartsView::SettingsTab::PLOTS)
    addTab(plotsFrame_, "Plots");

  if (settingsTabs & CQChartsView::SettingsTab::ANNOTATIONS)
    addTab(annotationsFrame_, "Annotations");

  if (settingsTabs & CQChartsView::SettingsTab::OBJECTS)
    addTab(objectsFrame_, "Objects");

  if (settingsTabs & CQChartsView::SettingsTab::COLORS)
    addTab(colorsFrame_, "Colors");

  if (settingsTabs & CQChartsView::SettingsTab::SYMBOLS)
    addTab(symbolsFrame_, "Symbols");

  if (settingsTabs & CQChartsView::SettingsTab::LAYERS)
    addTab(layersFrame_, "Layers");

  if (settingsTabs & CQChartsView::SettingsTab::QUERY)
    addTab(queryFrame_, "Query");

  if (settingsTabs & CQChartsView::SettingsTab::ERRORS)
    addTab(errorsFrame_, "Errors");

  if (ind >= 0)
    tab_->setCurrentIndex(ind);
}

//------

void
CQChartsViewSettings::
updateView()
{
  auto *view = window_->view();
  if (! view) return;

  view->updatePlots();
}

void
CQChartsViewSettings::
updatePalettes()
{
  themeWidgets_.palettesControl->updatePalettes();
}

void
CQChartsViewSettings::
updateInterface()
{
  themeWidgets_.interfaceControl->updateState();
}

//------

void
CQChartsViewSettings::
symbolListSymbolChangeSlot()
{
  auto symbol = symbolsList_->symbol();

  symbolEdit_->setSymbol(symbol);
}

//------

// called when layers ot current plot changed
void
CQChartsViewSettings::
updateLayers()
{
  auto *plot = currentPlot();

  layersWidgets_.layerTableControl->setPlot(plot);
}

//---

CQTabSplit *
CQChartsViewSettings::
createTabSplit(const QString &name, bool tabbed) const
{
  auto *split = CQUtil::makeWidget<CQTabSplit>(name);

  split->setOrientation(Qt::Vertical);
  split->setGrouped(true);

  if (tabbed)
    split->setState(CQTabSplit::State::TAB);

  return split;
}

CQChartsViewSettings::FrameLayout
CQChartsViewSettings::
addSplitFrame(CQTabSplit *split, const QString &label, const QString &name) const
{
  auto *frame  = CQUtil::makeWidget<QFrame>(name);
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(frame, 2, 2);

  split->addWidget(frame, label);

  return FrameLayout(frame, layout);
}
