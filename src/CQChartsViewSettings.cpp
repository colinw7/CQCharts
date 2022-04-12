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

#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsGroupPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsPropertyViewTree.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsAnnotation.h>
#include <CQChartsKey.h>
#include <CQChartsViewError.h>
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

#include <CQTabWidget.h>
#include <CQTabSplit.h>
#include <CQUtil.h>
#include <CQGroupBox.h>
#include <CQToolTip.h>

#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPainter>
#include <QFileDialog>
#include <QDir>
#include <QTimer>

#include <fstream>
#include <iostream>

CQChartsViewSettings::
CQChartsViewSettings(CQChartsWindow *window) :
 QFrame(window), window_(window)
{
  auto *view   = window_->view();
  auto *charts = view->charts();

  connect(charts, SIGNAL(modelDataChanged()), this, SLOT(updateModels()));
  connect(charts, SIGNAL(currentModelChanged(int)), this, SLOT(invalidateModelDetails()));
  connect(charts, SIGNAL(modelDataDataChanged()), this, SLOT(updateModelsData()));

  connect(view, SIGNAL(plotsChanged()), this, SLOT(updatePlots()));
  connect(view, SIGNAL(plotsReordered()), this, SLOT(updatePlots()));

  connect(view, SIGNAL(connectDataChanged()), this, SLOT(updatePlots()));

  connect(view, SIGNAL(currentPlotChanged()), this, SLOT(updateCurrentPlot()));

  connect(view, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));

  connect(view, SIGNAL(selectionChanged()), this, SLOT(updateSelection()));

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
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  tab_ = CQUtil::makeWidget<CQTabWidget>("tab");

  layout->addWidget(tab_);

  tab_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  //----

  int tabNum = 0;

  auto addTab = [&](const QString &name) {
    auto objectName = name + "Frame";

    objectName[0] = objectName[0].toLower();

    auto *area = CQUtil::makeWidget<QScrollArea>(objectName);

    auto *frame = CQUtil::makeWidget<QFrame>(area, "frame");

    area->setWidget(frame);
    area->setWidgetResizable(true);

    tab_->addTab(area, name);

    tabNum_[name] = tabNum++;

    return frame;
  };

  //--

  // Controls
  initControlsFrame(addTab("Controls"));

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

  updatePlotControls();

  updateAnnotations();

  updatePlotObjects();

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

void
CQChartsViewSettings::
initPropertiesFrame(QFrame *propertiesFrame)
{
  auto *view   = window_->view();
  auto *charts = view->charts();

  auto *propertiesLayout = CQUtil::makeLayout<QVBoxLayout>(propertiesFrame, 2, 2);

  //--

  propertiesWidgets_.propertiesSplit = createTabSplit("propertiesSplit", /*tabbed*/true);

  propertiesLayout->addWidget(propertiesWidgets_.propertiesSplit);

  //----

  // Create Global Properties Frame
  auto *globalFrame       = CQUtil::makeWidget<QFrame>("globalFrame");
  auto *globalFrameLayout = CQUtil::makeLayout<QVBoxLayout>(globalFrame, 2, 2);

  propertiesWidgets_.propertiesSplit->addWidget(globalFrame, "Global");

  //--

  propertiesWidgets_.globalPropertyTree = new GlobalPropertiesWidget(charts);

  globalFrameLayout->addWidget(propertiesWidgets_.globalPropertyTree);

  //----

  // Create View Properties Frame
  auto *viewFrame       = CQUtil::makeWidget<QFrame>("viewFrame");
  auto *viewFrameLayout = CQUtil::makeLayout<QVBoxLayout>(viewFrame, 2, 2);

  propertiesWidgets_.propertiesSplit->addWidget(viewFrame, "View");

  //--

  propertiesWidgets_.viewControl = new ViewPropertiesControl(view);

  viewFrameLayout->addWidget(propertiesWidgets_.viewControl);

  //----

  // Create Plots Properties Frame
  auto *plotsFrame       = CQUtil::makeWidget<QFrame>("plotsFrame");
  auto *plotsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(plotsFrame, 2, 2);

  propertiesWidgets_.propertiesSplit->addWidget(plotsFrame, "Plots");

  //---

  propertiesWidgets_.plotsControl = new PlotPropertiesControl(view);

  plotsFrameLayout->addWidget(propertiesWidgets_.plotsControl);

  connect(propertiesWidgets_.plotsControl,
          SIGNAL(propertyItemSelected(QObject *, const QString &)),
          this, SIGNAL(propertyItemSelected(QObject *, const QString &)));

  //--

  // Add Quick control frame
  quickControlFrame_ = CQUtil::makeWidget<CQChartsPlotControlFrame>("quickControlFrame");

  propertiesWidgets_.propertiesSplit->addWidget(quickControlFrame_, "Quick Controls");

  //----

  propertiesWidgets_.propertiesSplit->setCurrentIndex(tabNum_["Properties"]);

  //--

  int globalSize = int(INT_MAX*0.1);
  int viewSize   = int(INT_MAX*0.2);
  int quickSize  = int(INT_MAX*0.1); // quick controls
  int plotSize   = INT_MAX - viewSize - quickSize;

  propertiesWidgets_.propertiesSplit->
    setSizes(QList<int>({globalSize, viewSize, plotSize, quickSize}));
}

void
CQChartsViewSettings::
initModelsFrame(QFrame *modelsFrame)
{
  auto *view   = window_->view();
  auto *charts = view->charts();

  //---

  auto *modelsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(modelsFrame, 2, 2);

  //----

  auto *modelsSplit = createTabSplit("modelsSplit", /*tabbed*/false);

  modelsFrameLayout->addWidget(modelsSplit);

  //----

  // Models Frame
  auto *modelsModelsFrame       = CQUtil::makeWidget<QFrame>("modelsModelsFrame");
  auto *modelsModelsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(modelsModelsFrame, 2, 2);

  modelsSplit->addWidget(modelsModelsFrame, "Models");

  //--

  modelsWidgets_.modelTable = new CQChartsModelTableControl(charts);

  modelsModelsFrameLayout->addWidget(modelsWidgets_.modelTable);

  //----

  // Model Details
  modelsWidgets_.detailsFrame = CQUtil::makeWidget<QFrame>("detailsFrame");

  auto *detailsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(modelsWidgets_.detailsFrame, 2, 2);

  modelsSplit->addWidget(modelsWidgets_.detailsFrame, "Details");

  //--

  modelsWidgets_.detailsWidget = new CQChartsModelDetailsWidget(charts);

  detailsFrameLayout->addWidget(modelsWidgets_.detailsWidget);

  //----

  modelsSplit->setSizes(QList<int>({INT_MAX, INT_MAX}));
}

void
CQChartsViewSettings::
initPlotsFrame(QFrame *plotsFrame)
{
  auto *view = window_->view();

  //---

  auto *plotsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(plotsFrame, 2, 2);

  //----

  auto *plotsGroup       = CQUtil::makeLabelWidget<CQGroupBox>("Plots", "plotsGroup");
  auto *plotsGroupLayout = CQUtil::makeLayout<QVBoxLayout>(plotsGroup, 2, 2);

  plotsFrameLayout->addWidget(plotsGroup);

  //--

  plotsWidgets_.plotTable = new CQChartsPlotTableControl;

  plotsWidgets_.plotTable->setView(view);

  plotsGroupLayout->addWidget(plotsWidgets_.plotTable);
}

void
CQChartsViewSettings::
initAnnotationsFrame(QFrame *annotationsFrame)
{
  auto *annotationsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(annotationsFrame, 2, 2);

  //---

  annotationsWidgets_.split = createTabSplit("annotationsSplit", /*tabbed*/false);

  annotationsFrameLayout->addWidget(annotationsWidgets_.split);

  //----

  auto createPushButton = [&](const QString &label, const QString &objName,
                              const char *slotName, const QString &tip) {
    auto *button = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    connect(button, SIGNAL(clicked()), this, slotName);

    button->setToolTip(tip);

    return button;
  };

  //---

  // view annotations control
  annotationsWidgets_.viewTable = new CQChartsViewAnnotationsControl;

  annotationsWidgets_.split->addWidget(annotationsWidgets_.viewTable, "View");

  //---

  // plot annotations control
  annotationsWidgets_.plotTable = new CQChartsPlotAnnotationsControl;

  annotationsWidgets_.split->addWidget(annotationsWidgets_.plotTable, "Plot");

  //--

  annotationsWidgets_.split->setSizes(QList<int>({INT_MAX, INT_MAX}));

  //--

  // create view/plot annotation buttons
  auto *controlGroup = CQUtil::makeLabelWidget<CQGroupBox>("View/Plot Control", "controlGroup");

  annotationsFrameLayout->addWidget(controlGroup);

  auto *controlGroupLayout = CQUtil::makeLayout<QHBoxLayout>(controlGroup, 2, 2);

  annotationsWidgets_.writeButton =
    createPushButton("Write", "write", SLOT(writeAnnotationSlot()),
                     "Write View and Plot Annotations");

  controlGroupLayout->addWidget(annotationsWidgets_.writeButton);
  controlGroupLayout->addStretch(1);
}

void
CQChartsViewSettings::
initObjectsFrame(QFrame *objectsFrame)
{
  auto *objectsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(objectsFrame, 2, 2);

  objectsWidgets_.propertyTree = new ObjectPropertiesWidget;

  objectsFrameLayout->addWidget(objectsWidgets_.propertyTree);
}

void
CQChartsViewSettings::
initColorsFrame(QFrame *colorsFrame)
{
  auto *view = window_->view();

  //--

  auto *colorsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(colorsFrame, 2, 2);

  //--

  auto *colorsSubTab = CQUtil::makeWidget<CQTabWidget>("colorsSubTab");

  colorsFrameLayout->addWidget(colorsSubTab);

  //---

  // tab for theme (current theme and palettes list)
  auto *themePalettesFrame       = CQUtil::makeWidget<QFrame>("themePalettesFrame");
  auto *themePalettesFrameLayout = CQUtil::makeLayout<QVBoxLayout>(themePalettesFrame, 2, 2);

  colorsSubTab->addTab(themePalettesFrame, "Theme");

  // create palettes list
  themeWidgets_.palettesList = new CQColorsEditList(this);

  themePalettesFrameLayout->addWidget(themeWidgets_.palettesList);

  //connect(themeWidgets_.palettesList, SIGNAL(palettesChanged()), this, SLOT(updateView()));

  //--

  // tab for palettes (palette viewer/editor)
  themeWidgets_.palettesControl = new CQChartsPaletteControl(this);

  themeWidgets_.palettesControl->setView(view);

  colorsSubTab->addTab(themeWidgets_.palettesControl, "Palettes");

  //----

  // tab for interface (palette viewer/editor)
  themeWidgets_.interfaceControl = new CQChartsInterfaceControl(this);

  themeWidgets_.interfaceControl->setView(view);

  colorsSubTab->addTab(themeWidgets_.interfaceControl, "Interface");
}

void
CQChartsViewSettings::
initSymbolsFrame(QFrame *symbolSetsFrame)
{
  auto *charts = window_->view()->charts();

  auto *symbolSetsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(symbolSetsFrame, 2, 2);

  //---

  auto *symbolsSplit = createTabSplit("symbolsSplit", /*tabbed*/false);

  symbolSetsFrameLayout->addWidget(symbolsSplit);

  //---

  // List of all symbol sets
  // TODO: update on add/remove symbol set
  symbolSetsList_ = new CQChartsSymbolSetsList(charts);

  symbolsSplit->addWidget(symbolSetsList_, "Symbol Sets");

  //---

  auto *symbolsFrame  = CQUtil::makeWidget<QFrame>("symbolsFrame");
  auto *symbolsLayout = CQUtil::makeLayout<QHBoxLayout>(symbolsFrame, 2, 2);

  symbolsSplit->addWidget(symbolsFrame, "Symbols");

  //--

  // List of all symbols in set
  symbolsList_ = new CQChartsSymbolsListControl(charts);

  connect(symbolsList_, SIGNAL(symbolChanged()), this, SLOT(symbolListSymbolChangeSlot()));

  symbolsLayout->addWidget(symbolsList_);

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

void
CQChartsViewSettings::
initLayersFrame(QFrame *layersFrame)
{
  auto *layersFrameLayout = CQUtil::makeLayout<QVBoxLayout>(layersFrame, 2, 2);

  //---

  layersWidgets_.viewLayerTable = new CQChartsViewLayerTable;

  layersWidgets_.viewLayerTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layersFrameLayout->addWidget(layersWidgets_.viewLayerTable);

  connect(layersWidgets_.viewLayerTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(viewLayersSelectionChangeSlot()));
  connect(layersWidgets_.viewLayerTable, SIGNAL(cellClicked(int, int)),
          this, SLOT(viewLayersClickedSlot(int, int)));

  //---

  layersWidgets_.plotLayerTable = new CQChartsPlotLayerTable;

  layersWidgets_.plotLayerTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layersFrameLayout->addWidget(layersWidgets_.plotLayerTable);

  connect(layersWidgets_.plotLayerTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(plotLayersSelectionChangeSlot()));
  connect(layersWidgets_.plotLayerTable, SIGNAL(cellClicked(int, int)),
          this, SLOT(plotLayersClickedSlot(int, int)));

  //---

  auto *controlFrame  = CQUtil::makeWidget<QFrame>("control");
  auto *controlLayout = CQUtil::makeLayout<QHBoxLayout>(controlFrame, 2, 2);

  layersFrameLayout->addWidget(controlFrame);

  //--

  auto *viewImageButton = CQUtil::makeLabelWidget<QPushButton>("View Image", "viewImage");

  viewImageButton->setToolTip("Show View Layer Image");

  controlLayout->addWidget(viewImageButton);

  connect(viewImageButton, SIGNAL(clicked()), this, SLOT(viewLayerImageSlot()));

  //--

  auto *plotImageButton = CQUtil::makeLabelWidget<QPushButton>("Plot Image", "plotImage");

  plotImageButton->setToolTip("Show Plot Layer Image");

  controlLayout->addWidget(plotImageButton);

  connect(plotImageButton, SIGNAL(clicked()), this, SLOT(plotLayerImageSlot()));

  //--

  controlLayout->addStretch(1);
}

//------

void
CQChartsViewSettings::
initQueryFrame(QFrame *queryFrame)
{
  auto *queryFrameLayout = CQUtil::makeLayout<QVBoxLayout>(queryFrame, 2, 2);

  //---

  queryText_ = CQUtil::makeWidget<QTextEdit>("queryText");

  queryFrameLayout->addWidget(queryText_);

  //---

  auto *view = window_->view();

  connect(view, SIGNAL(showQueryText(const QString &)),
          this, SLOT(showQueryText(const QString &)));
}

void
CQChartsViewSettings::
showQueryText(const QString &text)
{
  queryText_->setText(text);

  showQueryTab();
}

void
CQChartsViewSettings::
showQueryTab()
{
  tab_->setCurrentIndex(tabNum_["Query"]);
}

//------

void
CQChartsViewSettings::
initErrorsFrame(QFrame *errorsFrame)
{
  auto *errorsFrameLayout = CQUtil::makeLayout<QVBoxLayout>(errorsFrame, 2, 2);

  //---

  auto *view = window_->view();

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

class CQChartsViewSettingsLayerImage : public QDialog {
 public:
  CQChartsViewSettingsLayerImage() {
    setWindowTitle("Layer Image");
  }

  void setImage(const QImage &image) {
    image_ = image;

    setFixedSize(image_.size());
  }

  void paintEvent(QPaintEvent *) {
    QPainter p(this);

    p.drawImage(0, 0, image_);
  }

 private:
  QImage image_;
};

//------

void
CQChartsViewSettings::
viewLayerImageSlot()
{
  static CQChartsViewSettingsLayerImage *layerImage;

  //---

  auto *view = window_->view();
  if (! view) return;

  auto *image = layersWidgets_.viewLayerTable->selectedImage(view);
  if (! image) return;

  //---

  if (! layerImage)
    layerImage = new CQChartsViewSettingsLayerImage;

  layerImage->setImage(*image);

  layerImage->show();
}

void
CQChartsViewSettings::
plotLayerImageSlot()
{
  static CQChartsViewSettingsLayerImage *layerImage;

  //---

  auto *plot = currentPlot();
  if (! plot) return;

  auto *image = layersWidgets_.plotLayerTable->selectedImage(plot);
  if (! image) return;

  //---

  if (! layerImage)
    layerImage = new CQChartsViewSettingsLayerImage;

  layerImage->setImage(*image);

  layerImage->show();
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
  auto *charts = window_->view()->charts();

  auto *modelData = charts->currentModelData();

  modelsWidgets_.detailsWidget->invalidateModelData(modelData, /*invalidate*/changed);
}

//------

void
CQChartsViewSettings::
updatePlots()
{
  auto *view = window_->view();

  //---

  plotsWidgets_.plotTable->setView(view);

  //---

  updatePlotControls();

  //---

  propertiesWidgets_.plotsControl->setView(view);
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
      disconnect(plot, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));
      disconnect(plot, SIGNAL(layersChanged()), this, SLOT(updateLayers()));
      disconnect(plot, SIGNAL(controlColumnsChanged()), this, SLOT(updatePlotControls()));
      disconnect(plot, SIGNAL(plotObjsAdded()), this, SLOT(updatePlotObjects()));
    }
  }

  //---

  auto *plot = view->currentPlot();

  plotId_ = (plot ? plot->id() : "");

  if (plot) {
    connect(plot, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));
    connect(plot, SIGNAL(layersChanged()), this, SLOT(updateLayers()));
    connect(plot, SIGNAL(controlColumnsChanged()), this, SLOT(updatePlotControls()));
    connect(plot, SIGNAL(plotObjsAdded()), this, SLOT(updatePlotObjects()));
  }

  //---

  updatePlotControls();

  //---

  updateAnnotations();

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
}

//------

void
CQChartsViewSettings::
updateAnnotations()
{
  auto *view = window_->view();

  annotationsWidgets_.viewTable->setView(view);

  //---

  auto *plot = currentPlot(/*remap*/false);

  auto plotName = (plot ? QString("Plot %1").arg(plot->id()) : "Plot");

  annotationsWidgets_.split->setWidgetName(annotationsWidgets_.plotTable, plotName);

  annotationsWidgets_.plotTable->setPlot(plot);
}

//---

void
CQChartsViewSettings::
writeAnnotationSlot()
{
  auto *view = window_->view();
  if (! view) return;

  auto dir = QDir::current().dirName() + "/annotation.tcl";

  auto filename = QFileDialog::getSaveFileName(this, "Write Annotations", dir, "Files (*.tcl)");
  if (! filename.length()) return; // cancelled

  auto fs = std::ofstream(filename.toStdString(), std::ofstream::out);

  //---

  const auto &viewAnnotations = view->annotations();

  for (const auto &annotation : viewAnnotations)
    annotation->write(fs);

  //---

  CQChartsView::Plots plots;

  view->getPlots(plots);

  for (const auto &plot : plots) {
    const auto &plotAnnotations = plot->annotations();

    for (const auto &annotation : plotAnnotations)
      annotation->write(fs);
  }
}

//------

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
updateView()
{
  auto *view = window_->view();

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

void
CQChartsViewSettings::
updateLayers()
{
  auto *view = window_->view();

  if (view)
    layersWidgets_.viewLayerTable->setView(view);

  auto *plot = currentPlot();

  if (plot)
    layersWidgets_.plotLayerTable->setPlot(plot);
}

void
CQChartsViewSettings::
viewLayersSelectionChangeSlot()
{
}

void
CQChartsViewSettings::
viewLayersClickedSlot(int row, int column)
{
  if (column != 1)
    return;

  auto *view = window_->view();

  if (row == 0)
    view->invalidateObjects();
  else
    view->invalidateOverlay();
}

void
CQChartsViewSettings::
plotLayersSelectionChangeSlot()
{
}

void
CQChartsViewSettings::
plotLayersClickedSlot(int row, int column)
{
  if (column != 1)
    return;

  auto *plot = currentPlot();
  if (! plot) return;

  CQChartsLayer::Type type;
  bool                active;

  if (! layersWidgets_.plotLayerTable->getLayerState(plot, row, type, active))
    return;

  auto *layer = plot->getLayer(type);
  if (! layer) return;

  plot->setLayerActive(type, active);

  const auto *buffer = plot->getBuffer(layer->buffer());

  if (buffer->type() != CQChartsBuffer::Type::MIDDLE)
    plot->invalidateLayer(buffer->type());
  else
    plot->drawObjs();
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
