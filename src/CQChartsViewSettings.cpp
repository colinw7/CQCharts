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
#include <CQCharts.h>
#include <CQIconCombo.h>
#include <CQIntegerSpin.h>
#include <CQUtil.h>

#include <QTabWidget>
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

CQChartsViewSettings::
CQChartsViewSettings(CQChartsWindow *window) :
 QFrame(window), window_(window)
{
  CQChartsView *view = window_->view();

  CQCharts *charts = view->charts();

  connect(charts, SIGNAL(modelDataAdded(int)), this, SLOT(updateModels()));
  connect(charts, SIGNAL(modelNameChanged(const QString &)), this, SLOT(updateModels()));

  connect(view, SIGNAL(plotAdded(const QString &)), this, SLOT(updatePlots()));
  connect(view, SIGNAL(plotsReordered()), this, SLOT(updatePlots()));
  connect(view, SIGNAL(plotRemoved(const QString &)), this, SLOT(updatePlots()));
  connect(view, SIGNAL(allPlotsRemoved()), this, SLOT(updatePlots()));

  connect(view, SIGNAL(connectDataChanged()), this, SLOT(updatePlots()));

  connect(view, SIGNAL(currentPlotChanged()), this, SLOT(updateCurrentPlot()));

  connect(window, SIGNAL(themePalettesChanged()), this, SLOT(updatePalettes()));
  connect(window, SIGNAL(interfacePaletteChanged()), this, SLOT(updateInterface()));

  //--

  setObjectName("settings");

  setAutoFillBackground(true);

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

  QVBoxLayout *propertiesLayout = new QVBoxLayout(propertiesFrame);
  propertiesLayout->setMargin(0); propertiesLayout->setSpacing(2);

  //--

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

  propertiesWidgets_.propertyTree = new CQPropertyViewTree(this, view->propertyModel());

  connect(propertiesWidgets_.propertyTree, SIGNAL(itemSelected(QObject *, const QString &)),
          this, SIGNAL(propertyItemSelected(QObject *, const QString &)));

  propertiesLayout->addWidget(propertiesWidgets_.propertyTree);

  //----

  // Models Tab
  QFrame *modelsFrame = new QFrame;
  modelsFrame->setObjectName("modelsFrame");

  tab_->addTab(modelsFrame, "Models");

  QVBoxLayout *modelsFrameLayout = new QVBoxLayout(modelsFrame);

  modelsWidgets_.modelTable = new QTableWidget;
  modelsWidgets_.modelTable->setObjectName("modelTable");

  modelsWidgets_.modelTable->horizontalHeader()->setStretchLastSection(true);

  modelsWidgets_.modelTable->setSelectionBehavior(QAbstractItemView::SelectRows);

  modelsFrameLayout->addWidget(modelsWidgets_.modelTable);

  connect(modelsWidgets_.modelTable, SIGNAL(itemSelectionChanged()),
          this, SLOT(modelsSelectionChangeSlot()));

  QHBoxLayout *modelControlLayout = new QHBoxLayout;

  modelsFrameLayout->addLayout(modelControlLayout);

  QPushButton *loadModelButton = new QPushButton("Load");
  loadModelButton->setObjectName("load");

  connect(loadModelButton, SIGNAL(clicked()), this, SLOT(loadModelSlot()));

  modelControlLayout->addWidget(loadModelButton);
  modelControlLayout->addStretch(1);

  //----

  // Plots Tab
  QFrame *plotsFrame = new QFrame;
  plotsFrame->setObjectName("plotsFrame");

  tab_->addTab(plotsFrame, "Charts");

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

  QGroupBox *stackPlotsGroup = new QGroupBox("Modify");
  stackPlotsGroup->setObjectName("stackPlotsGroup");

  plotsFrameLayout->addWidget(stackPlotsGroup);

  QHBoxLayout *stackPlotsGroupLayout = new QHBoxLayout(stackPlotsGroup);

  plotsWidgets_.raiseButton = new QPushButton("Raise");
  plotsWidgets_.raiseButton->setObjectName("raise");

  plotsWidgets_.lowerButton = new QPushButton("Lower");
  plotsWidgets_.lowerButton->setObjectName("lower");

  plotsWidgets_.removeButton = new QPushButton("Remove");
  plotsWidgets_.removeButton->setObjectName("remove");

  plotsWidgets_.raiseButton ->setEnabled(false);
  plotsWidgets_.lowerButton ->setEnabled(false);
  plotsWidgets_.removeButton->setEnabled(false);

  connect(plotsWidgets_.raiseButton , SIGNAL(clicked()), this, SLOT(raisePlotSlot()));
  connect(plotsWidgets_.lowerButton , SIGNAL(clicked()), this, SLOT(lowerPlotSlot()));
  connect(plotsWidgets_.removeButton, SIGNAL(clicked()), this, SLOT(removePlotsSlot()));

  stackPlotsGroupLayout->addWidget(plotsWidgets_.raiseButton);
  stackPlotsGroupLayout->addWidget(plotsWidgets_.lowerButton);
  stackPlotsGroupLayout->addWidget(plotsWidgets_.removeButton);
  stackPlotsGroupLayout->addStretch(1);

  //----

  QGroupBox *controlPlotsGroup = new QGroupBox("Control");
  controlPlotsGroup->setObjectName("controlPlotsGroup");

  plotsFrameLayout->addWidget(controlPlotsGroup);

  QHBoxLayout *controlPlotsGroupLayout = new QHBoxLayout(controlPlotsGroup);

  QPushButton *createPlotButton = new QPushButton("Create");
  createPlotButton->setObjectName("create");

  connect(createPlotButton, SIGNAL(clicked()), this, SLOT(createPlotSlot()));

  controlPlotsGroupLayout->addWidget(createPlotButton);
  controlPlotsGroupLayout->addStretch(1);

  //----

  // Theme Tab
  QFrame *themeFrame = new QFrame;
  themeFrame->setObjectName("themeFrame");

  tab_->addTab(themeFrame, "Theme");

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

  int np = view->theme()->numPalettes();

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
    new CQChartsGradientPaletteCanvas(this, view->theme()->palette());
  themeWidgets_.palettesControl =
    new CQChartsGradientPaletteControl(themeWidgets_.palettesPlot);

  palettesSplitter->addWidget(themeWidgets_.palettesPlot);
  palettesSplitter->addWidget(themeWidgets_.palettesControl);

  connect(themeWidgets_.palettesControl, SIGNAL(stateChanged()), view, SLOT(update()));

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

  connect(themeWidgets_.interfaceControl, SIGNAL(stateChanged()), view, SLOT(update()));

  //----

  updateModels();
}

CQChartsViewSettings::
~CQChartsViewSettings()
{
}

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
}

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

  int ind = view->currentPlotInd();

  int nr = plotsWidgets_.plotTable->rowCount();

  for (int i = 0; i < nr; ++i) {
    QTableWidgetItem *item = plotsWidgets_.plotTable->item(i, 0);

    bool ok;

    int ind1 = item->data(Qt::UserRole).toInt(&ok);

    item->setSelected(ind1 == ind);
  }
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

  view->theme()->setPalette(i, palette->dup());

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

  CQChartsGradientPalette *palette = view->theme()->palette(i);

  themeWidgets_.palettesPlot->setGradientPalette(palette);

  int ind = themeWidgets_.palettesCombo->findText(palette->name());

  themeWidgets_.palettesCombo->setCurrentIndex(ind);

  themeWidgets_.palettesControl->updateState();
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

void
CQChartsViewSettings::
modelsSelectionChangeSlot()
{
  CQCharts *charts = window_->view()->charts();

  QList<QTableWidgetItem *> items = modelsWidgets_.modelTable->selectedItems();

  for (int i = 0; i < items.length(); ++i) {
    QTableWidgetItem *item = items[i];

    bool ok;

    int ind = item->data(Qt::UserRole).toInt(&ok);

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

void
CQChartsViewSettings::
plotsSelectionChangeSlot()
{
  Plots plots;

  getSelectedPlots(plots);

  plotsWidgets_.raiseButton->setEnabled(plots.size() == 1);
  plotsWidgets_.lowerButton->setEnabled(plots.size() == 1);

  plotsWidgets_.removeButton->setEnabled(plots.size() > 0);
}

void
CQChartsViewSettings::
groupPlotsSlot()
{
  CQChartsView *view = window_->view();

  // get selected plots ?
  Plots plots;

  view->getPlots(plots);

  bool overlay = plotsWidgets_.overlayCheck->isChecked();
  bool x1x2    = plotsWidgets_.x1x2Check   ->isChecked();
  bool y1y2    = plotsWidgets_.y1y2Check   ->isChecked();

  if      (x1x2) {
    if (plots.size() != 2) {
      std::cerr << "Need 2 plots for x1x2\n";
      return;
    }

    view->initX1X2(plots[0], plots[1], overlay, /*reset*/true);
  }
  else if (y1y2) {
    if (plots.size() != 2) {
      std::cerr << "Need 2 plots for y1y2\n";
      return;
    }

    view->initY1Y2(plots[0], plots[1], overlay, /*reset*/true);
  }
  else if (overlay) {
    if (plots.size() < 2) {
      std::cerr << "Need 2 or more plots for overlay\n";
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
  CQChartsView *view = window_->view();

  Plots plots;

  getSelectedPlots(plots);

  for (auto &plot : plots)
    view->removePlot(plot);
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

  plotDlg_ = new CQChartsPlotDlg(charts, modelData->model());

  plotDlg_->setViewName(window_->view()->id());

  plotDlg_->show();
}
