#include <CQChartsViewSettings.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsFilterEdit.h>
#include <CQPropertyViewTree.h>
#include <CQChartsGradientPaletteCanvas.h>
#include <CQChartsGradientPaletteControl.h>
#include <CQChartsLoadDlg.h>
#include <CQChartsPlotDlg.h>
#include <CQChartsAnnotationDlg.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsAnnotation.h>
#include <CQCharts.h>
#include <CQChartsVariant.h>
#include <CQChartsUtil.h>
#include <CQPropertyViewItem.h>
#include <CQIconCombo.h>
#include <CQIntegerSpin.h>
#include <CQUtil.h>

#include <QTabWidget>
#include <QTextBrowser>
#include <QHeaderView>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QSplitter>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

class CQChartsPropertyViewTree : public CQPropertyViewTree {
 public:
  CQChartsPropertyViewTree(CQChartsViewSettings *settings, CQPropertyViewModel *model) :
   CQPropertyViewTree(settings, model), settings_(settings) {
  }

  void printItem(CQPropertyViewItem *item) const {
    QObject *object = item->object();

    QString dataStr = item->dataStr();
    QString path    = item->path(".", /*alias*/true);

    CQChartsPlot       *plot       = qobject_cast<CQChartsPlot       *>(object);
    CQChartsAnnotation *annotation = qobject_cast<CQChartsAnnotation *>(object);

    if      (plot) {
      if (path.startsWith(plot->id()))
        path = path.mid(plot->id().length() + 1);

      std::cerr << "set_charts_property -plot " << plot->id().toStdString() <<
                   " -name " << path.toStdString() <<
                   " -value " << dataStr.toStdString() << "\n";
    }
    else if (annotation) {
      CQChartsPlot *plot = annotation->plot();

      if (plot) {
        if (path.startsWith(plot->id()))
          path = path.mid(plot->id().length() + 1);

        if (path.startsWith("annotations."))
          path = path.mid(12);

        if (path.startsWith(annotation->propertyId()))
          path = path.mid(annotation->propertyId().length() + 1);
      }
      else {
        if (path.startsWith("annotations."))
          path = path.mid(12);

        if (path.startsWith(annotation->propertyId()))
          path = path.mid(annotation->propertyId().length() + 1);
      }

      std::cerr << "set_charts_property -annotation " <<
                   annotation->pathId().toStdString() <<
                   " -name " << path.toStdString() <<
                   " -value " << dataStr.toStdString() << "\n";
    }
    else {
      CQChartsView *view = settings_->window()->view();

      std::cerr << "set_charts_property -view " << view->id().toStdString() <<
                   " -name " << path.toStdString() <<
                   " -value " << dataStr.toStdString() << "\n";
    }
  }

 private:
  CQChartsViewSettings *settings_ { nullptr };
};

//---

CQChartsViewSettings::
CQChartsViewSettings(CQChartsWindow *window) :
 QFrame(window), window_(window)
{
  CQChartsView *view = window_->view();

  CQCharts *charts = view->charts();

  connect(charts, SIGNAL(modelDataAdded(int)), this, SLOT(updateModels()));
  connect(charts, SIGNAL(currentModelChanged(int)), this, SLOT(invalidateModelDetails()));
  connect(charts, SIGNAL(modelNameChanged(const QString &)), this, SLOT(updateModels()));

  connect(view, SIGNAL(plotsChanged()), this, SLOT(updatePlots()));
  connect(view, SIGNAL(plotsReordered()), this, SLOT(updatePlots()));

  connect(view, SIGNAL(connectDataChanged()), this, SLOT(updatePlots()));

  connect(view, SIGNAL(currentPlotChanged()), this, SLOT(updateCurrentPlot()));

  connect(view, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));

  connect(view, SIGNAL(selectionChanged()), this, SLOT(updateSelection()));

  connect(window, SIGNAL(themePalettesChanged()), this, SLOT(updatePalettes()));
  connect(window, SIGNAL(interfacePaletteChanged()), this, SLOT(updateInterface()));

  //--

  setObjectName("settings");

  setAutoFillBackground(true);

  addWidgets();
}

CQChartsViewSettings::
~CQChartsViewSettings()
{
}

void
CQChartsViewSettings::
addWidgets()
{
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  tab_ = CQUtil::makeWidget<QTabWidget>("tab");

  layout->addWidget(tab_);

  tab_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  //----

  // Properties Tab
  QFrame *propertiesFrame = new QFrame;
  propertiesFrame->setObjectName("propertiesFrame");

  tab_->addTab(propertiesFrame, "Properties");

  initPropertiesFrame(propertiesFrame);

  //--

  // Models Tab
  QFrame *modelsFrame = new QFrame;
  modelsFrame->setObjectName("modelsFrame");

  tab_->addTab(modelsFrame, "Models");

  initModelsFrame(modelsFrame);

  //--

  // Plots Tab
  QFrame *plotsFrame = new QFrame;
  plotsFrame->setObjectName("plotsFrame");

  tab_->addTab(plotsFrame, "Plots");

  initPlotsFrame(plotsFrame);

  //--

  // Annotations Tab
  QFrame *annotationsFrame = new QFrame;
  annotationsFrame->setObjectName("annotationsFrame");

  tab_->addTab(annotationsFrame, "Annotations");

  initAnnotationsFrame(annotationsFrame);

  //--

  // Theme Tab
  QFrame *themeFrame = new QFrame;
  themeFrame->setObjectName("themeFrame");

  tab_->addTab(themeFrame, "Theme");

  initThemeFrame(themeFrame);

  //--

  // Layers Tab
  QFrame *layersFrame = new QFrame;
  layersFrame->setObjectName("layersFrame");

  tab_->addTab(layersFrame, "Layers");

  initLayersFrame(layersFrame);

  //----

  updateModels();

  updateAnnotations();
}

//------

void
CQChartsViewSettings::
initPropertiesFrame(QFrame *propertiesFrame)
{
  QVBoxLayout *propertiesLayout = new QVBoxLayout(propertiesFrame);
  propertiesLayout->setMargin(0); propertiesLayout->setSpacing(2);

  propertiesWidgets_.filterEdit = new CQChartsFilterEdit;

  connect(propertiesWidgets_.filterEdit, SIGNAL(replaceFilter(const QString &)),
          this, SLOT(replaceFilterSlot(const QString &)));
  connect(propertiesWidgets_.filterEdit, SIGNAL(addFilter(const QString &)),
          this, SLOT(addFilterSlot(const QString &)));

  connect(propertiesWidgets_.filterEdit, SIGNAL(replaceSearch(const QString &)),
          this, SLOT(replaceSearchSlot(const QString &)));
  connect(propertiesWidgets_.filterEdit, SIGNAL(addSearch(const QString &)),
          this, SLOT(addSearchSlot(const QString &)));

  propertiesLayout->addWidget(propertiesWidgets_.filterEdit);

  CQChartsView *view = window_->view();

  propertiesWidgets_.propertyTree = new CQChartsPropertyViewTree(this, view->propertyModel());

  connect(propertiesWidgets_.propertyTree, SIGNAL(itemSelected(QObject *, const QString &)),
          this, SIGNAL(propertyItemSelected(QObject *, const QString &)));

  propertiesLayout->addWidget(propertiesWidgets_.propertyTree);
}

void
CQChartsViewSettings::
initModelsFrame(QFrame *modelsFrame)
{
  QVBoxLayout *modelsFrameLayout = new QVBoxLayout(modelsFrame);

  modelsWidgets_.modelTable = new QTableWidget;
  modelsWidgets_.modelTable->setObjectName("modelTable");

  modelsWidgets_.modelTable->horizontalHeader()->setStretchLastSection(true);

  modelsWidgets_.modelTable->setSelectionBehavior(QAbstractItemView::SelectRows);

  modelsFrameLayout->addWidget(modelsWidgets_.modelTable);

  connect(modelsWidgets_.modelTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(modelsSelectionChangeSlot()));

  //--

  QFrame *detailsControlFrame = new QFrame;
  detailsControlFrame->setObjectName("detailsControlFrame");

  QHBoxLayout *detailsControlLayout = new QHBoxLayout(detailsControlFrame);

  modelsFrameLayout->addWidget(detailsControlFrame);

  modelsWidgets_.updateDetailsButton = new QPushButton("Update Details");
  modelsWidgets_.updateDetailsButton->setObjectName("updateDetails");

  detailsControlLayout->addWidget(modelsWidgets_.updateDetailsButton);
  detailsControlLayout->addStretch(1);

  connect(modelsWidgets_.updateDetailsButton, SIGNAL(clicked()),
          this, SLOT(updateModelDetails()));

  modelsWidgets_.modelDetailsText = new QTextBrowser;
  modelsWidgets_.modelDetailsText->setObjectName("modelDetailsText");

  modelsFrameLayout->addWidget(modelsWidgets_.modelDetailsText);

  //--

  QHBoxLayout *modelControlLayout = new QHBoxLayout;

  modelsFrameLayout->addLayout(modelControlLayout);

  QPushButton *loadModelButton = new QPushButton("Load");
  loadModelButton->setObjectName("load");

  connect(loadModelButton, SIGNAL(clicked()), this, SLOT(loadModelSlot()));

  modelControlLayout->addWidget(loadModelButton);
  modelControlLayout->addStretch(1);
}

void
CQChartsViewSettings::
initPlotsFrame(QFrame *plotsFrame)
{
  QVBoxLayout *plotsFrameLayout = new QVBoxLayout(plotsFrame);

  plotsWidgets_.plotTable = new QTableWidget;
  plotsWidgets_.plotTable->setObjectName("plotTable");

  plotsWidgets_.plotTable->horizontalHeader()->setStretchLastSection(true);

  plotsWidgets_.plotTable->setSelectionBehavior(QAbstractItemView::SelectRows);

  plotsFrameLayout->addWidget(plotsWidgets_.plotTable);

  connect(plotsWidgets_.plotTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(plotsSelectionChangeSlot()));

  //--

  QGroupBox *groupPlotsGroup = new QGroupBox("Group");
  groupPlotsGroup->setObjectName("groupPlotsGroup");

  QVBoxLayout *groupPlotsGroupLayout = new QVBoxLayout(groupPlotsGroup);

  plotsFrameLayout->addWidget(groupPlotsGroup);

  //--

  QHBoxLayout *groupPlotsCheckLayout = new QHBoxLayout;

  groupPlotsGroupLayout->addLayout(groupPlotsCheckLayout);

  plotsWidgets_.overlayCheck = new QCheckBox("Overlay");
  plotsWidgets_.overlayCheck->setObjectName("overlay");

  plotsWidgets_.x1x2Check = new QCheckBox("X1/X2");
  plotsWidgets_.overlayCheck->setObjectName("x1x2");

  plotsWidgets_.y1y2Check = new QCheckBox("Y1/Y2");
  plotsWidgets_.overlayCheck->setObjectName("y1y2");

  groupPlotsCheckLayout->addWidget(plotsWidgets_.overlayCheck);
  groupPlotsCheckLayout->addWidget(plotsWidgets_.x1x2Check);
  groupPlotsCheckLayout->addWidget(plotsWidgets_.y1y2Check);
  groupPlotsCheckLayout->addStretch(1);

  //--

  QHBoxLayout *groupPlotsButtonsLayout = new QHBoxLayout;

  groupPlotsGroupLayout->addLayout(groupPlotsButtonsLayout);

  QPushButton *groupApplyButton = new QPushButton("Apply");
  groupApplyButton->setObjectName("apply");

  groupPlotsButtonsLayout->addWidget(groupApplyButton);
  groupPlotsButtonsLayout->addStretch(1);

  connect(groupApplyButton, SIGNAL(clicked()), this, SLOT(groupPlotsSlot()));

  //----

  QGroupBox *placePlotsGroup = new QGroupBox("Place");
  placePlotsGroup->setObjectName("placePlotsGroup");

  QVBoxLayout *placePlotsGroupLayout = new QVBoxLayout(placePlotsGroup);

  plotsFrameLayout->addWidget(placePlotsGroup);

  //--

  QHBoxLayout *placePlotsCheckLayout = new QHBoxLayout;

  placePlotsGroupLayout->addLayout(placePlotsCheckLayout);

  plotsWidgets_.placeVerticalRadio = new QRadioButton("Vertical");
  plotsWidgets_.placeVerticalRadio->setObjectName("vertical");

  plotsWidgets_.placeHorizontalRadio = new QRadioButton("Horizontal");
  plotsWidgets_.placeHorizontalRadio->setObjectName("horizontal");

  plotsWidgets_.placeGridRadio = new QRadioButton("Grid");
  plotsWidgets_.placeGridRadio->setObjectName("grid");

  plotsWidgets_.placeVerticalRadio->setChecked(true);

  placePlotsCheckLayout->addWidget(plotsWidgets_.placeVerticalRadio);
  placePlotsCheckLayout->addWidget(plotsWidgets_.placeHorizontalRadio);
  placePlotsCheckLayout->addWidget(plotsWidgets_.placeGridRadio);
  placePlotsCheckLayout->addStretch(1);

  //--

  QHBoxLayout *placePlotsGridLayout = new QHBoxLayout;

  placePlotsGroupLayout->addLayout(placePlotsGridLayout);

  plotsWidgets_.placeRowsEdit = new CQIntegerSpin;
  plotsWidgets_.placeRowsEdit->setObjectName("rowsEdit");

  plotsWidgets_.placeColumnsEdit = new CQIntegerSpin;
  plotsWidgets_.placeColumnsEdit->setObjectName("columnsEdit");

  plotsWidgets_.placeRowsEdit   ->setValue(1);
  plotsWidgets_.placeColumnsEdit->setValue(1);

  plotsWidgets_.placeRowsEdit   ->setMinimum(1);
  plotsWidgets_.placeColumnsEdit->setMinimum(1);

  placePlotsGridLayout->addWidget(new QLabel("Rows"));
  placePlotsGridLayout->addWidget(plotsWidgets_.placeRowsEdit);
  placePlotsGridLayout->addWidget(new QLabel("Columns"));
  placePlotsGridLayout->addWidget(plotsWidgets_.placeColumnsEdit);
  placePlotsGridLayout->addStretch(1);

  //--

  QHBoxLayout *placePlotsButtonsLayout = new QHBoxLayout;

  placePlotsGroupLayout->addLayout(placePlotsButtonsLayout);

  QPushButton *placeApplyButton = new QPushButton("Apply");
  placeApplyButton->setObjectName("apply");

  placePlotsButtonsLayout->addWidget(placeApplyButton);
  placePlotsButtonsLayout->addStretch(1);

  connect(placeApplyButton, SIGNAL(clicked()), this, SLOT(placePlotsSlot()));

  //----

  QGroupBox *controlPlotsGroup = new QGroupBox("Control");
  controlPlotsGroup->setObjectName("controlPlotsGroup");

  plotsFrameLayout->addWidget(controlPlotsGroup);

  QHBoxLayout *controlPlotsGroupLayout = new QHBoxLayout(controlPlotsGroup);

  plotsWidgets_.raiseButton = new QPushButton("Raise");
  plotsWidgets_.raiseButton->setObjectName("raise");

  plotsWidgets_.lowerButton = new QPushButton("Lower");
  plotsWidgets_.lowerButton->setObjectName("lower");

  QPushButton *createPlotButton = new QPushButton("Create");
  createPlotButton->setObjectName("create");

  plotsWidgets_.removeButton = new QPushButton("Remove");
  plotsWidgets_.removeButton->setObjectName("remove");

  QPushButton *writePlotButton = new QPushButton("Write");
  writePlotButton->setObjectName("write");

  plotsWidgets_.raiseButton ->setEnabled(false);
  plotsWidgets_.lowerButton ->setEnabled(false);
  plotsWidgets_.removeButton->setEnabled(false);

  connect(plotsWidgets_.raiseButton , SIGNAL(clicked()), this, SLOT(raisePlotSlot()));
  connect(plotsWidgets_.lowerButton , SIGNAL(clicked()), this, SLOT(lowerPlotSlot()));
  connect(createPlotButton          , SIGNAL(clicked()), this, SLOT(createPlotSlot()));
  connect(plotsWidgets_.removeButton, SIGNAL(clicked()), this, SLOT(removePlotsSlot()));
  connect(writePlotButton           , SIGNAL(clicked()), this, SLOT(writePlotSlot()));

  controlPlotsGroupLayout->addWidget(plotsWidgets_.raiseButton);
  controlPlotsGroupLayout->addWidget(plotsWidgets_.lowerButton);
  controlPlotsGroupLayout->addWidget(createPlotButton);
  controlPlotsGroupLayout->addWidget(plotsWidgets_.removeButton);
  controlPlotsGroupLayout->addWidget(writePlotButton);
  controlPlotsGroupLayout->addStretch(1);
}

void
CQChartsViewSettings::
initAnnotationsFrame(QFrame *annotationsFrame)
{
  QVBoxLayout *annotationsFrameLayout = new QVBoxLayout(annotationsFrame);

  //---

  annotationsWidgets_.viewTable = new QTableWidget;
  annotationsWidgets_.viewTable->setObjectName("viewTable");

  annotationsWidgets_.viewTable->horizontalHeader()->setStretchLastSection(true);

  annotationsWidgets_.viewTable->setSelectionBehavior(QAbstractItemView::SelectRows);

  annotationsFrameLayout->addWidget(annotationsWidgets_.viewTable);

  connect(annotationsWidgets_.viewTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(viewAnnotationSelectionChangeSlot()));

  //---

  annotationsWidgets_.plotTable = new QTableWidget;
  annotationsWidgets_.plotTable->setObjectName("plotTable");

  annotationsWidgets_.plotTable->horizontalHeader()->setStretchLastSection(true);

  annotationsWidgets_.plotTable->setSelectionBehavior(QAbstractItemView::SelectRows);

  annotationsFrameLayout->addWidget(annotationsWidgets_.plotTable);

  connect(annotationsWidgets_.plotTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(plotAnnotationSelectionChangeSlot()));

  //----

  QGroupBox *controlGroup = new QGroupBox("Control");
  controlGroup->setObjectName("controlGroup");

  annotationsFrameLayout->addWidget(controlGroup);

  QHBoxLayout *controlGroupLayout = new QHBoxLayout(controlGroup);

  QPushButton *createButton = new QPushButton("Create");
  createButton->setObjectName("create");

  annotationsWidgets_.removeButton = new QPushButton("Remove");
  annotationsWidgets_.removeButton->setObjectName("remove");

  QPushButton *writeButton = new QPushButton("Write");
  writeButton->setObjectName("write");

  annotationsWidgets_.removeButton->setEnabled(false);

  connect(createButton                    , SIGNAL(clicked()), this, SLOT(createAnnotationSlot()));
  connect(annotationsWidgets_.removeButton, SIGNAL(clicked()), this, SLOT(removeAnnotationsSlot()));
  connect(writeButton                     , SIGNAL(clicked()), this, SLOT(writeAnnotationSlot()));

  controlGroupLayout->addWidget(createButton);
  controlGroupLayout->addWidget(annotationsWidgets_.removeButton);
  controlGroupLayout->addWidget(writeButton);
  controlGroupLayout->addStretch(1);
}

void
CQChartsViewSettings::
initThemeFrame(QFrame *themeFrame)
{
  QVBoxLayout *themeFrameLayout = new QVBoxLayout(themeFrame);

  //--

  QTabWidget *themeSubTab = new QTabWidget;
  themeSubTab->setObjectName("themeSubTab");

  themeFrameLayout->addWidget(themeSubTab);

  // tab for theme palettes
  QFrame *palettesFrame = new QFrame;
  palettesFrame->setObjectName("palettesFrame");

  QVBoxLayout *palettesFrameLayout = new QVBoxLayout(palettesFrame);

  themeSubTab->addTab(palettesFrame, "Palettes");

  // tab for interface palette
  QFrame *interfaceFrame = new QFrame;
  interfaceFrame->setObjectName("interfaceFrame");

  QHBoxLayout *interfaceFrameLayout = new QHBoxLayout(interfaceFrame);

  themeSubTab->addTab(interfaceFrame, "Interface");

  //--

#if 0
  QFrame *themeColorsFrame = new QFrame;
  themeColorsFrame->setObjectName("themeColorsFrame");

  QGridLayout *themeColorsLayout = new QGridLayout(themeColorsFrame);

  QLabel *selColorLabel = new QLabel("Selection");
  selColorLabel->setObjectName("selColorLabel");

  QLineEdit *selColorEdit = new QLineEdit;
  selColorEdit->setObjectName("selColorEdit");

  themeColorsLayout->addWidget(selColorLabel, 0, 0);
  themeColorsLayout->addWidget(selColorEdit , 0, 1);

  paletteLayout->addWidget(themeColorsFrame);
#endif

  //--

  QFrame *palettesControlFrame = new QFrame;
  palettesControlFrame->setObjectName("control");

  QHBoxLayout *palettesControlFrameLayout = new QHBoxLayout(palettesControlFrame);

  QLabel *spinLabel = new QLabel("Index");
  spinLabel->setObjectName("indexLabel");

  themeWidgets_.palettesSpin = new QSpinBox;
  themeWidgets_.palettesSpin->setObjectName("indexSpin");

  CQChartsView *view = window_->view();

  int np = view->themeObj()->numPalettes();

  themeWidgets_.palettesSpin->setRange(0, np);

  connect(themeWidgets_.palettesSpin, SIGNAL(valueChanged(int)), this, SLOT(paletteIndexSlot(int)));

  palettesControlFrameLayout->addWidget(spinLabel);
  palettesControlFrameLayout->addWidget(themeWidgets_.palettesSpin);

  QLabel *paletteNameLabel = new QLabel("Name");
  paletteNameLabel->setObjectName("paletteNameLabel");

  themeWidgets_.palettesCombo = new QComboBox;
  themeWidgets_.palettesCombo->setObjectName("palettesCombo");

  QStringList paletteNames;

  CQChartsThemeMgrInst->getPaletteNames(paletteNames);

  themeWidgets_.palettesCombo->addItems(paletteNames);

  palettesControlFrameLayout->addWidget(paletteNameLabel);
  palettesControlFrameLayout->addWidget(themeWidgets_.palettesCombo);

  themeWidgets_.palettesLoadButton = new QPushButton("Load");
  themeWidgets_.palettesLoadButton->setObjectName("load");

  connect(themeWidgets_.palettesLoadButton, SIGNAL(clicked()), this, SLOT(loadPaletteNameSlot()));

  palettesControlFrameLayout->addWidget(themeWidgets_.palettesLoadButton);

  palettesControlFrameLayout->addStretch(1);

  palettesFrameLayout->addWidget(palettesControlFrame);

  //--

  QSplitter *palettesSplitter = new QSplitter;
  palettesSplitter->setObjectName("splitter");

  palettesSplitter->setOrientation(Qt::Vertical);

  palettesFrameLayout->addWidget(palettesSplitter);

  themeWidgets_.palettesPlot    =
    new CQChartsGradientPaletteCanvas(this, view->themeObj()->palette());
  themeWidgets_.palettesControl =
    new CQChartsGradientPaletteControl(themeWidgets_.palettesPlot);

  palettesSplitter->addWidget(themeWidgets_.palettesPlot);
  palettesSplitter->addWidget(themeWidgets_.palettesControl);

  connect(themeWidgets_.palettesControl, SIGNAL(stateChanged()), view, SLOT(updatePlots()));

  //--

  QSplitter *interfaceSplitter = new QSplitter;
  interfaceSplitter->setObjectName("splitter");

  interfaceSplitter->setOrientation(Qt::Vertical);

  interfaceFrameLayout->addWidget(interfaceSplitter);

  themeWidgets_.interfacePlot    =
    new CQChartsGradientPaletteCanvas(this, view->interfacePalette());
  themeWidgets_.interfaceControl =
    new CQChartsGradientPaletteControl(themeWidgets_.interfacePlot);

  interfaceSplitter->addWidget(themeWidgets_.interfacePlot);
  interfaceSplitter->addWidget(themeWidgets_.interfaceControl);

  connect(themeWidgets_.interfaceControl, SIGNAL(stateChanged()), view, SLOT(updatePlots()));
}

void
CQChartsViewSettings::
initLayersFrame(QFrame *layersFrame)
{
  QVBoxLayout *layersFrameLayout = new QVBoxLayout(layersFrame);

  layersWidgets_.layerTable = new QTableWidget;
  layersWidgets_.layerTable->setObjectName("layerTable");

  layersWidgets_.layerTable->horizontalHeader()->setStretchLastSection(true);

  layersWidgets_.layerTable->setSelectionBehavior(QAbstractItemView::SelectRows);

  layersFrameLayout->addWidget(layersWidgets_.layerTable);

  connect(layersWidgets_.layerTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(layersSelectionChangeSlot()));
  connect(layersWidgets_.layerTable, SIGNAL(cellClicked(int, int)),
          this, SLOT(layersClickedSlot(int, int)));

  layersWidgets_.layerTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  //---

  QFrame *controlFrame = new QFrame;
  controlFrame->setObjectName("control");

  QHBoxLayout *controlLayout = new QHBoxLayout(controlFrame);
  controlLayout->setMargin(0); controlLayout->setSpacing(0);

  layersFrameLayout->addWidget(controlFrame);

  //--

  QPushButton *imageButton = new QPushButton("Image");

  controlLayout->addWidget(imageButton);
  controlLayout->addStretch(1);

  connect(imageButton, SIGNAL(clicked()), this, SLOT(layerImageSlot()));
}

class CQChartsViewSettingsLayerImage : public QDialog {
 public:
  CQChartsViewSettingsLayerImage() {
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

void
CQChartsViewSettings::
layerImageSlot()
{
  static CQChartsViewSettingsLayerImage *layerImage;

  //---

  CQChartsPlot *plot = window_->view()->currentPlot();
  if (! plot) return;

  QList<QTableWidgetItem *> items = layersWidgets_.layerTable->selectedItems();
  if (items.length() <= 0) return;

  QTableWidgetItem *item = items[0];

  bool ok;

  long l = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);
  if (! ok) return;

  CQChartsLayer *layer = plot->getLayer((CQChartsLayer::Type) l);
  if (! layer) return;

  CQChartsBuffer *buffer = plot->getBuffer(layer->buffer());
  if (! buffer) return;

  QImage *image = buffer->image();
  if (! image) return;

  //---

  if (! layerImage)
    layerImage = new CQChartsViewSettingsLayerImage;

  layerImage->setImage(*image);

  layerImage->show();
}

//------

void
CQChartsViewSettings::
updateModels()
{
  CQChartsView *view = window_->view();

  CQCharts *charts = view->charts();

  CQCharts::ModelDatas modelDatas;

  charts->getModelDatas(modelDatas);

  modelsWidgets_.modelTable->clear();

  modelsWidgets_.modelTable->setColumnCount(1);
  modelsWidgets_.modelTable->setRowCount(modelDatas.size());

  modelsWidgets_.modelTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Id"));

  int i = 0;

  for (const auto &modelData : modelDatas) {
    QTableWidgetItem *idItem = new QTableWidgetItem(modelData->id());

    modelsWidgets_.modelTable->setItem(i, 0, idItem);

    idItem->setData(Qt::UserRole, modelData->ind());

    ++i;
  }

  //---

  invalidateModelDetails();
}

void
CQChartsViewSettings::
invalidateModelDetails()
{
  modelDetailsValid_ = false;

  modelsWidgets_.updateDetailsButton->setEnabled(true);
}

void
CQChartsViewSettings::
updateModelDetails()
{
  modelDetailsValid_ = true;

  modelsWidgets_.updateDetailsButton->setEnabled(false);

  //---

  CQCharts *charts = window_->view()->charts();

  CQChartsModelData *modelData = charts->currentModelData();
  if (! modelData) return;

  CQChartsModelDetails *details = modelData->details();
  if (! details) return;

  //---

  QString text = "<b></b>";

  text += "<table padding=\"4\">";
  text += QString("<tr><td>Columns</td><td>%1</td></tr>").arg(details->numColumns());
  text += QString("<tr><td>Rows</td><td>%1</td></tr>").arg(details->numRows());
  text += "</table>";

  text += "<br>";

  text += "<table padding=\"4\">";
  text += "<tr><th>Column</th><th>Type</th><th>Min</th><th>Max</th><th>Monotonic</th></tr>";

  for (int c = 0; c < details->numColumns(); ++c) {
    const CQChartsModelColumnDetails *columnDetails = details->columnDetails(c);

    text += "<tr>";

    text += QString("<td>%1</td><td>%2</td><td>%3</td><td>%4</td>").
             arg(c + 1).
             arg(columnDetails->typeName()).
             arg(columnDetails->dataName(columnDetails->minValue()).toString()).
             arg(columnDetails->dataName(columnDetails->maxValue()).toString());

    if (columnDetails->isMonotonic())
      text += QString("<td>%1</td>").
        arg(columnDetails->isIncreasing() ? "Increasing" : "Decreasing");
    else
      text += QString("<td></td>");

    text += "</tr>";
  }

  text += "</table>";

  modelsWidgets_.modelDetailsText->setHtml(text);
}

void
CQChartsViewSettings::
modelsSelectionChangeSlot()
{
  CQCharts *charts = window_->view()->charts();

  QList<QTableWidgetItem *> items = modelsWidgets_.modelTable->selectedItems();

  for (int i = 0; i < items.length(); ++i) {
    QTableWidgetItem *item = items[i];

    bool ok;

    long ind = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);

    if (ok) {
      charts->setCurrentModelInd(ind);
      return;
    }
  }
}

void
CQChartsViewSettings::
loadModelSlot()
{
  CQCharts *charts = window_->view()->charts();

  if (loadDlg_)
    delete loadDlg_;

  loadDlg_ = new CQChartsLoadDlg(charts);

  loadDlg_->show();
}

//------

void
CQChartsViewSettings::
updatePlots()
{
  CQChartsView *view = window_->view();

  plotsWidgets_.plotTable->clear();

  int np = view->numPlots();

  plotsWidgets_.plotTable->setColumnCount(2);
  plotsWidgets_.plotTable->setRowCount(np);

  plotsWidgets_.plotTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Id"   ));
  plotsWidgets_.plotTable->setHorizontalHeaderItem(1, new QTableWidgetItem("State"));

  for (int i = 0; i < np; ++i) {
    CQChartsPlot *plot = view->plot(i);

    QTableWidgetItem *idItem = new QTableWidgetItem(plot->id());

    plotsWidgets_.plotTable->setItem(i, 0, idItem);

    int ind = view->plotInd(plot);

    idItem->setData(Qt::UserRole, ind);

    QStringList states;

    if (plot->isOverlay()) states += "overlay";
    if (plot->isX1X2   ()) states += "x1x2";
    if (plot->isY1Y2   ()) states += "y1y2";

    QTableWidgetItem *stateItem = new QTableWidgetItem(states.join("|"));

    plotsWidgets_.plotTable->setItem(i, 1, stateItem);
  }
}

void
CQChartsViewSettings::
updateCurrentPlot()
{
  CQChartsView *view = window_->view();
  assert(view);

  if (plotId_.length()) {
    CQChartsPlot *plot = view->getPlot(plotId_);

    if (plot) {
      disconnect(plot, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));
      disconnect(plot, SIGNAL(layersChanged()), this, SLOT(updateLayers()));
    }
  }

  //---

  int ind = view->currentPlotInd();

  int nr = plotsWidgets_.plotTable->rowCount();

  for (int i = 0; i < nr; ++i) {
    QTableWidgetItem *item = plotsWidgets_.plotTable->item(i, 0);

    bool ok;

    long ind1 = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);

    item->setSelected(ind1 == ind);
  }

  //---

  CQChartsPlot *plot = view->currentPlot();

  plotId_ = (plot ? plot->id() : "");

  if (plot) {
    connect(plot, SIGNAL(annotationsChanged()), this, SLOT(updateAnnotations()));
    connect(plot, SIGNAL(layersChanged()), this, SLOT(updateLayers()));
  }

  //---

  updateAnnotations();

  updateLayers();
}

CQChartsPlot *
CQChartsViewSettings::
getSelectedPlot() const
{
  Plots plots;

  getSelectedPlots(plots);

  if (plots.size() == 1)
    return plots[0];

  return nullptr;
}

void
CQChartsViewSettings::
getSelectedPlots(Plots &plots) const
{
  CQChartsView *view = window_->view();

  QList<QTableWidgetItem *> items = plotsWidgets_.plotTable->selectedItems();

  for (int i = 0; i < items.length(); ++i) {
    QTableWidgetItem *item = items[i];
    if (item->column() != 0) continue;

    QString id = item->text();

    CQChartsPlot *plot = view->getPlot(id);

    plots.push_back(plot);
  }
}

void
CQChartsViewSettings::
plotsSelectionChangeSlot()
{
  Plots plots;

  getSelectedPlots(plots);

  plotsWidgets_.raiseButton->setEnabled(plots.size() == 1);
  plotsWidgets_.lowerButton->setEnabled(plots.size() == 1);

  plotsWidgets_.removeButton->setEnabled(plots.size() > 0);

  //---

  CQChartsView *view = window_->view();

  view->startSelection();

  view->deselectAll();

  for (auto &plot : plots)
    plot->setSelected(true);

  view->endSelection();
}

void
CQChartsViewSettings::
groupPlotsSlot()
{
  CQChartsView *view = window_->view();

  CQCharts *charts = view->charts();

  // get selected plots ?
  Plots plots;

  view->getPlots(plots);

  bool overlay = plotsWidgets_.overlayCheck->isChecked();
  bool x1x2    = plotsWidgets_.x1x2Check   ->isChecked();
  bool y1y2    = plotsWidgets_.y1y2Check   ->isChecked();

  if      (x1x2) {
    if (plots.size() != 2) {
      charts->errorMsg("Need 2 plots for x1x2");
      return;
    }

    view->initX1X2(plots[0], plots[1], overlay, /*reset*/true);
  }
  else if (y1y2) {
    if (plots.size() != 2) {
      charts->errorMsg("Need 2 plots for y1y2");
      return;
    }

    view->initY1Y2(plots[0], plots[1], overlay, /*reset*/true);
  }
  else if (overlay) {
    if (plots.size() < 2) {
      charts->errorMsg("Need 2 or more plots for overlay");
      return;
    }

    view->initOverlay(plots, /*reset*/true);
  }
  else {
    view->resetGrouping();
  }
}

void
CQChartsViewSettings::
placePlotsSlot()
{
  CQChartsView *view = window_->view();

  // get selected plots ?
  Plots plots;

  view->getPlots(plots);

  bool vertical   = plotsWidgets_.placeVerticalRadio  ->isChecked();
  bool horizontal = plotsWidgets_.placeHorizontalRadio->isChecked();
  int  rows       = plotsWidgets_.placeRowsEdit       ->value();
  int  columns    = plotsWidgets_.placeColumnsEdit    ->value();

  view->placePlots(plots, vertical, horizontal, rows, columns);
}

void
CQChartsViewSettings::
raisePlotSlot()
{
  CQChartsView *view = window_->view();
  CQChartsPlot *plot = getSelectedPlot();

  if (plot)
    view->raisePlot(plot);
}

void
CQChartsViewSettings::
lowerPlotSlot()
{
  CQChartsView *view = window_->view();
  CQChartsPlot *plot = getSelectedPlot();

  if (plot)
    view->lowerPlot(plot);
}

void
CQChartsViewSettings::
removePlotsSlot()
{
  Plots plots;

  getSelectedPlots(plots);

  CQChartsView *view = window_->view();

  for (auto &plot : plots)
    view->removePlot(plot);

  view->updatePlots();
}

void
CQChartsViewSettings::
createPlotSlot()
{
  CQCharts *charts = window_->view()->charts();

  CQChartsModelData *modelData = charts->currentModelData();

  if (! modelData)
    return;

  if (plotDlg_)
    delete plotDlg_;

  plotDlg_ = new CQChartsPlotDlg(charts, modelData);

  plotDlg_->setViewName(window_->view()->id());

  plotDlg_->show();
}

void
CQChartsViewSettings::
writePlotSlot()
{
  CQChartsView *view = window_->view();
  assert(view);

  CQChartsPlot *plot = view->currentPlot();
  if (! plot) return;

  plot->write(std::cerr);
}

//------

void
CQChartsViewSettings::
updateAnnotations()
{
  CQChartsView *view = window_->view();

  annotationsWidgets_.viewTable->clear();

  const CQChartsView::Annotations &viewAnnotations = view->annotations();

  int nv = viewAnnotations.size();

  annotationsWidgets_.viewTable->setColumnCount(2);
  annotationsWidgets_.viewTable->setRowCount(nv);

  annotationsWidgets_.viewTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Id"  ));
  annotationsWidgets_.viewTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Type"));

  for (int i = 0; i < nv; ++i) {
    CQChartsAnnotation *annotation = viewAnnotations[i];

    QTableWidgetItem *idItem = new QTableWidgetItem(annotation->id());

    annotationsWidgets_.viewTable->setItem(i, 0, idItem);

    int ind = annotation->ind();

    idItem->setData(Qt::UserRole, ind);

    QTableWidgetItem *typeItem = new QTableWidgetItem(annotation->typeName());

    annotationsWidgets_.viewTable->setItem(i, 1, typeItem);
  }

  //---

  annotationsWidgets_.plotTable->clear();

  CQChartsPlot *plot = view->currentPlot();

  if (plot) {
    const CQChartsPlot::Annotations &plotAnnotations = plot->annotations();

    int np = plotAnnotations.size();

    annotationsWidgets_.plotTable->setColumnCount(2);
    annotationsWidgets_.plotTable->setRowCount(np);

    annotationsWidgets_.plotTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Id"  ));
    annotationsWidgets_.plotTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Type"));

    for (int i = 0; i < np; ++i) {
      CQChartsAnnotation *annotation = plotAnnotations[i];

      QTableWidgetItem *idItem = new QTableWidgetItem(annotation->id());

      annotationsWidgets_.plotTable->setItem(i, 0, idItem);

      int ind = annotation->ind();

      idItem->setData(Qt::UserRole, ind);

      QTableWidgetItem *typeItem = new QTableWidgetItem(annotation->typeName());

      annotationsWidgets_.plotTable->setItem(i, 1, typeItem);
    }
  }
}

void
CQChartsViewSettings::
viewAnnotationSelectionChangeSlot()
{
  Annotations viewAnnotations, plotAnnotations;

  getSelectedAnnotations(viewAnnotations, plotAnnotations);

  annotationsWidgets_.removeButton->setEnabled(viewAnnotations.size() > 0);

  if (viewAnnotations.size()) {
    disconnect(annotationsWidgets_.plotTable, SIGNAL(itemSelectionChanged()),
               this, SLOT(plotAnnotationSelectionChangeSlot()));

    annotationsWidgets_.plotTable->selectionModel()->clear();

    connect(annotationsWidgets_.plotTable, SIGNAL(itemSelectionChanged()),
            this, SLOT(plotAnnotationSelectionChangeSlot()));
  }

  //---

  CQChartsView *view = window_->view();

  view->startSelection();

  view->deselectAll();

  for (auto &annotation : viewAnnotations)
    annotation->setSelected(true);

  view->endSelection();
}

void
CQChartsViewSettings::
plotAnnotationSelectionChangeSlot()
{
  Annotations viewAnnotations, plotAnnotations;

  getSelectedAnnotations(viewAnnotations, plotAnnotations);

  annotationsWidgets_.removeButton->setEnabled(plotAnnotations.size() > 0);

  if (plotAnnotations.size()) {
    disconnect(annotationsWidgets_.viewTable, SIGNAL(itemSelectionChanged()),
               this, SLOT(viewAnnotationSelectionChangeSlot()));

    annotationsWidgets_.viewTable->selectionModel()->clear();

    connect(annotationsWidgets_.viewTable, SIGNAL(itemSelectionChanged()),
            this, SLOT(viewAnnotationSelectionChangeSlot()));
  }

  //---

  CQChartsView *view = window_->view();

  view->startSelection();

  view->deselectAll();

  for (auto &annotation : plotAnnotations)
    annotation->setSelected(true);

  view->endSelection();
}

void
CQChartsViewSettings::
getSelectedAnnotations(Annotations &viewAnnotations, Annotations &plotAnnotations) const
{
  CQChartsView *view = window_->view();

  QList<QTableWidgetItem *> items = annotationsWidgets_.viewTable->selectedItems();

  for (int i = 0; i < items.length(); ++i) {
    QTableWidgetItem *item = items[i];
    if (item->column() != 0) continue;

    QString id = item->text();

    CQChartsAnnotation *annotation = view->getAnnotationByName(id);

    if (annotation)
      viewAnnotations.push_back(annotation);
  }

  CQChartsPlot *plot = view->currentPlot();

  if (plot) {
    QList<QTableWidgetItem *> items = annotationsWidgets_.plotTable->selectedItems();

    for (int i = 0; i < items.length(); ++i) {
      QTableWidgetItem *item = items[i];
      if (item->column() != 0) continue;

      QString id = item->text();

      CQChartsAnnotation *annotation = plot->getAnnotationByName(id);

      if (annotation)
        plotAnnotations.push_back(annotation);
    }
  }
}

void
CQChartsViewSettings::
createAnnotationSlot()
{
  CQChartsView *view = window_->view();

  CQChartsPlot *plot = view->currentPlot();

  if (! plot)
    return;

  if (annotationDlg_)
    delete annotationDlg_;

  annotationDlg_ = new CQChartsAnnotationDlg(plot);

  annotationDlg_->show();
}

void
CQChartsViewSettings::
removeAnnotationsSlot()
{
  Annotations viewAnnotations, plotAnnotations;

  getSelectedAnnotations(viewAnnotations, plotAnnotations);

  CQChartsView *view = window_->view();

  for (const auto &annotation : viewAnnotations)
    view->removeAnnotation(annotation);

  CQChartsPlot *plot = view->currentPlot();

  if (plot) {
    for (const auto &annotation : plotAnnotations)
      plot->removeAnnotation(annotation);
  }

  view->updatePlots();
}

void
CQChartsViewSettings::
writeAnnotationSlot()
{
  CQChartsView *view = window_->view();

  const CQChartsView::Annotations &viewAnnotations = view->annotations();

  for (const auto &annotation : viewAnnotations)
    annotation->write(std::cerr);

  //---

  CQChartsView::Plots plots;

  view->getPlots(plots);

  for (const auto &plot : plots) {
    const CQChartsPlot::Annotations &plotAnnotations = plot->annotations();

    for (const auto &annotation : plotAnnotations)
      annotation->write(std::cerr);
  }
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
paletteIndexSlot(int)
{
  updatePalettes();
}

void
CQChartsViewSettings::
loadPaletteNameSlot()
{
  CQChartsView *view = window_->view();

  QString name = themeWidgets_.palettesCombo->currentText();

  CQChartsGradientPalette *palette = CQChartsThemeMgrInst->getNamedPalette(name);

  if (! palette)
    return;

  int i = themeWidgets_.palettesSpin->value();

  view->themeObj()->setPalette(i, palette->dup());

  updatePalettes();
}

void
CQChartsViewSettings::
updatePalettes()
{
  CQChartsView *view = window_->view();

  themeWidgets_.palettesSpin      ->setEnabled(true);
  themeWidgets_.palettesCombo     ->setEnabled(true);
  themeWidgets_.palettesLoadButton->setEnabled(true);

  int i = themeWidgets_.palettesSpin->value();

  CQChartsGradientPalette *palette = view->themeObj()->palette(i);

  themeWidgets_.palettesPlot->setGradientPalette(palette);

  int ind = themeWidgets_.palettesCombo->findText(palette->name());

  themeWidgets_.palettesCombo->setCurrentIndex(ind);

  themeWidgets_.palettesControl->updateState();

  view->updatePlots();
}

void
CQChartsViewSettings::
updateInterface()
{
  CQChartsView *view = window_->view();

  CQChartsGradientPalette *palette = view->interfacePalette();

  themeWidgets_.interfacePlot->setGradientPalette(palette);

  themeWidgets_.interfaceControl->updateState();
}

void
CQChartsViewSettings::
replaceFilterSlot(const QString &text)
{
  propertyTree()->setFilter(text);
}

void
CQChartsViewSettings::
addFilterSlot(const QString &text)
{
  //propertyTree()->addFilter(text);
  propertyTree()->setFilter(text);
}

void
CQChartsViewSettings::
replaceSearchSlot(const QString &text)
{
  propertyTree()->search(text);
}

void
CQChartsViewSettings::
addSearchSlot(const QString &text)
{
  //propertyTree()->addSearch(text);
  propertyTree()->search(text);
}

//------

void
CQChartsViewSettings::
updateLayers()
{
  int l1 = (int) CQChartsLayer::firstLayer();
  int l2 = (int) CQChartsLayer::lastLayer ();

  if (layersWidgets_.layerTable->rowCount() == 0) {
    layersWidgets_.layerTable->clear();

    layersWidgets_.layerTable->setColumnCount(3);
    layersWidgets_.layerTable->setRowCount(l2 - l1 + 1);

    layersWidgets_.layerTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Layer"));
    layersWidgets_.layerTable->setHorizontalHeaderItem(1, new QTableWidgetItem("State"));
    layersWidgets_.layerTable->setHorizontalHeaderItem(2, new QTableWidgetItem("Rect" ));

    for (int l = l1; l <= l2; ++l) {
      int i = l - l1;

      CQChartsLayer::Type type = (CQChartsLayer::Type) l;

      QString name = CQChartsLayer::typeName(type);

      QTableWidgetItem *idItem = new QTableWidgetItem(name);

      layersWidgets_.layerTable->setItem(i, 0, idItem);

      idItem->setData(Qt::UserRole, l);

      QTableWidgetItem *stateItem = new QTableWidgetItem("");

    //stateItem->setFlags(stateItem->flags() | Qt::ItemIsEnabled);
      stateItem->setFlags(stateItem->flags() | Qt::ItemIsUserCheckable);
    //stateItem->setFlags(stateItem->flags() & ! Qt::ItemIsEditable);

      layersWidgets_.layerTable->setItem(i, 1, stateItem);

      QTableWidgetItem *rectItem = new QTableWidgetItem("");

      layersWidgets_.layerTable->setItem(i, 2, rectItem);
    }
  }

  //---

  CQChartsView *view = window_->view();

  CQChartsPlot *plot = view->currentPlot();

  if (! plot)
    return;

  for (int l = l1; l <= l2; ++l) {
    int i = l - l1;

    CQChartsLayer::Type type = (CQChartsLayer::Type) l;

    CQChartsLayer *layer = plot->getLayer(type);

    const CQChartsBuffer *buffer = (layer ? plot->getBuffer(layer->buffer()) : nullptr);

//  QTableWidgetItem *idItem    = layersWidgets_.layerTable->item(i, 0);
    QTableWidgetItem *stateItem = layersWidgets_.layerTable->item(i, 1);
    QTableWidgetItem *rectItem  = layersWidgets_.layerTable->item(i, 2);

    QStringList states;

    if (layer  && layer ->isActive()) states += "active";
    if (buffer && buffer->isValid ()) states += "valid";

    stateItem->setText(states.join("|"));

    stateItem->setCheckState((layer && layer->isActive()) ? Qt::Checked : Qt::Unchecked);

    QRectF rect = (buffer ? buffer->rect() : QRectF());

    QString rectStr = QString("X:%1, Y:%2, W:%3, H:%4").
                        arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());

    rectItem->setText(rectStr);
  }
}

void
CQChartsViewSettings::
layersSelectionChangeSlot()
{
}

void
CQChartsViewSettings::
layersClickedSlot(int row, int column)
{
  if (column != 1)
    return;

  CQChartsView *view = window_->view();
  assert(view);

  CQChartsPlot *plot = view->currentPlot();
  if (! plot) return;

  QTableWidgetItem *nameItem = layersWidgets_.layerTable->item(row, 0);

  QString name = nameItem->text();

  CQChartsLayer::Type type = CQChartsLayer::nameType(name);

  CQChartsLayer *layer = plot->getLayer(type);
  if (! layer) return;

  QTableWidgetItem *stateItem = layersWidgets_.layerTable->item(row, 1);

  bool active = (stateItem->checkState() == Qt::Checked);

  plot->setLayerActive(type, active);

  const CQChartsBuffer *buffer = plot->getBuffer(layer->buffer());

  if (buffer->type() != CQChartsBuffer::Type::MIDDLE)
    plot->invalidateLayer(buffer->type());
  else
    plot->queueDrawObjs();
}
