#include <CQChartsPlotTable.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsModelData.h>
#include <CQChartsVariant.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsCreatePlotDlg.h>
#include <CQCharts.h>

#include <CQGroupBox.h>
#include <CQIntegerSpin.h>
#include <CQUtil.h>

#include <QHeaderView>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>

CQChartsPlotTableControl::
CQChartsPlotTableControl() :
 QFrame()
{
  setObjectName("plotTableControl");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //----

  plotTable_ = new CQChartsPlotTable;

  connect(plotTable_, SIGNAL(itemSelectionChanged()), this, SLOT(plotsSelectionChangeSlot()));

  layout->addWidget(plotTable_);

  //----

  auto *groupPlotsGroup       = CQUtil::makeLabelWidget<CQGroupBox>("Connect", "groupPlotsGroup");
  auto *groupPlotsGroupLayout = CQUtil::makeLayout<QVBoxLayout>(groupPlotsGroup, 2, 2);

  layout->addWidget(groupPlotsGroup);

  //----

  auto *groupPlotsCheckLayout = CQUtil::makeLayout<QHBoxLayout>(2, 2);

  groupPlotsGroupLayout->addLayout(groupPlotsCheckLayout);

  //--

  overlayCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Overlay", "overlay");
  x1x2Check_    = CQUtil::makeLabelWidget<QCheckBox>("X1/X2"  , "x1x2");
  y1y2Check_    = CQUtil::makeLabelWidget<QCheckBox>("Y1/Y2"  , "y1y2");

  overlayCheck_->setToolTip("Overlay plots so they shared the same range");
  x1x2Check_   ->setToolTip("Plot shares Y axis with another plot");
  y1y2Check_   ->setToolTip("Plot shares X axis with another plot");

  connect(x1x2Check_, SIGNAL(stateChanged(int)), this, SLOT(updatePlotOverlayState()));
  connect(y1y2Check_, SIGNAL(stateChanged(int)), this, SLOT(updatePlotOverlayState()));

  groupPlotsCheckLayout->addWidget(overlayCheck_);
  groupPlotsCheckLayout->addWidget(x1x2Check_);
  groupPlotsCheckLayout->addWidget(y1y2Check_);
  groupPlotsCheckLayout->addStretch(1);

  //----

  auto *groupPlotsButtonsLayout = CQUtil::makeLayout<QHBoxLayout>(2, 2);

  groupPlotsGroupLayout->addLayout(groupPlotsButtonsLayout);

  //--

  auto *groupApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "apply");

  groupApplyButton->setToolTip("Apply connection options to all plots");

  connect(groupApplyButton, SIGNAL(clicked()), this, SLOT(groupPlotsSlot()));

  groupPlotsButtonsLayout->addWidget(groupApplyButton);

  //--

  groupPlotsButtonsLayout->addStretch(1);

  //----

  auto *placePlotsGroup       = CQUtil::makeLabelWidget<CQGroupBox>("Place", "placePlotsGroup");
  auto *placePlotsGroupLayout = CQUtil::makeLayout<QVBoxLayout>(placePlotsGroup, 2, 2);

  layout->addWidget(placePlotsGroup);

  //--

  auto *placePlotsCheckLayout = CQUtil::makeLayout<QHBoxLayout>(2, 2);

  placePlotsGroupLayout->addLayout(placePlotsCheckLayout);

  placeVerticalRadio_   = CQUtil::makeLabelWidget<QRadioButton>("Vertical", "vertical");
  placeHorizontalRadio_ = CQUtil::makeLabelWidget<QRadioButton>("Horizontal", "horizontal");
  placeGridRadio_       = CQUtil::makeLabelWidget<QRadioButton>("Grid", "grid");

  placeVerticalRadio_  ->setToolTip("Place places vertically");
  placeHorizontalRadio_->setToolTip("Place places horizontally");
  placeGridRadio_      ->setToolTip("Place places in grid");

  placeVerticalRadio_->setChecked(true);

  placePlotsCheckLayout->addWidget(placeVerticalRadio_);
  placePlotsCheckLayout->addWidget(placeHorizontalRadio_);
  placePlotsCheckLayout->addWidget(placeGridRadio_);

  placePlotsCheckLayout->addStretch(1);

  //--

  //--

  auto *placePlotsGridLayout = CQUtil::makeLayout<QHBoxLayout>(2, 2);

  placePlotsGroupLayout->addLayout(placePlotsGridLayout);

  placeRowsEdit_    = CQUtil::makeWidget<CQIntegerSpin>("rowsEdit");
  placeColumnsEdit_ = CQUtil::makeWidget<CQIntegerSpin>("columnsEdit");

  placeRowsEdit_->setValue(1);
  placeRowsEdit_->setMinimum(1);
  placeRowsEdit_->setToolTip("Number of Rows");

  placeColumnsEdit_->setValue(1);
  placeColumnsEdit_->setMinimum(1);
  placeColumnsEdit_->setToolTip("Number of Columns");

  placePlotsGridLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Rows", "row"));
  placePlotsGridLayout->addWidget(placeRowsEdit_);
  placePlotsGridLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Columns", "columns"));
  placePlotsGridLayout->addWidget(placeColumnsEdit_);
  placePlotsGridLayout->addStretch(1);

  //--

  auto *placePlotsButtonsLayout = CQUtil::makeLayout<QHBoxLayout>(2, 2);

  placePlotsGroupLayout->addLayout(placePlotsButtonsLayout);

  auto *placeApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "apply");

  placeApplyButton->setToolTip("Apply placement options to all plots");

  placePlotsButtonsLayout->addWidget(placeApplyButton);
  placePlotsButtonsLayout->addStretch(1);

  connect(placeApplyButton, SIGNAL(clicked()), this, SLOT(placePlotsSlot()));

  //----

  auto createPushButton = [&](const QString &label, const QString &objName, const char *slotName) {
    auto *button = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  auto *controlPlotsGroup = CQUtil::makeLabelWidget<CQGroupBox>("Control", "controlPlotsGroup");

  layout->addWidget(controlPlotsGroup);

  auto *controlPlotsGroupLayout = CQUtil::makeLayout<QHBoxLayout>(controlPlotsGroup, 2, 2);

  raiseButton_  = createPushButton("Raise"    , "raise" , SLOT(raisePlotSlot()));
  lowerButton_  = createPushButton("Lower"    , "lower" , SLOT(lowerPlotSlot()));
  createButton_ = createPushButton("Create...", "create", SLOT(createPlotSlot()));
  removeButton_ = createPushButton("Remove"   , "remove", SLOT(removePlotsSlot()));

  raiseButton_ ->setToolTip("Raise selected plot");
  lowerButton_ ->setToolTip("Lower selected plot");
  createButton_->setToolTip("Create new plot");
  removeButton_->setToolTip("Remove selected plot");

  raiseButton_ ->setEnabled(false);
  lowerButton_ ->setEnabled(false);
  removeButton_->setEnabled(false);

  controlPlotsGroupLayout->addWidget(raiseButton_);
  controlPlotsGroupLayout->addWidget(lowerButton_);
  controlPlotsGroupLayout->addWidget(CQChartsWidgetUtil::createHSpacer(1));
  controlPlotsGroupLayout->addWidget(createButton_);
  controlPlotsGroupLayout->addWidget(removeButton_);

  //---

  controlPlotsGroupLayout->addStretch(1);

  //---

  updatePlotOverlayState();
}

void
CQChartsPlotTableControl::
updateCurrentPlot()
{
  if (! view_)
    return;

  disconnect(plotTable_, SIGNAL(itemSelectionChanged()), this, SLOT(plotsSelectionChangeSlot()));

  int ind = view_->currentPlotInd();

  plotTable_->setCurrentInd(ind);

  connect(plotTable_, SIGNAL(itemSelectionChanged()), this, SLOT(plotsSelectionChangeSlot()));
}

void
CQChartsPlotTableControl::
updatePlotOverlayState()
{
  auto *obj = sender();

  if      (obj == x1x2Check_) {
    if (x1x2Check_->isChecked())
      y1y2Check_->setChecked(false);
  }
  else if (obj == y1y2Check_) {
    if (y1y2Check_->isChecked())
      x1x2Check_->setChecked(false);
  }
  else {
    if (x1x2Check_->isChecked() && y1y2Check_->isChecked())
      y1y2Check_->setChecked(false);
  }
}

void
CQChartsPlotTableControl::
setView(CQChartsView *view)
{
  if (view_) {
    disconnect(view_, SIGNAL(plotsChanged()), this, SLOT(updatePlots()));
    disconnect(view_, SIGNAL(plotsReordered()), this, SLOT(updatePlots()));

    disconnect(view_, SIGNAL(connectDataChanged()), this, SLOT(updatePlots()));

    disconnect(view_, SIGNAL(currentPlotChanged()), this, SLOT(updateCurrentPlot()));
  }

  view_ = view;

  if (view_) {
    connect(view_, SIGNAL(plotsChanged()), this, SLOT(updatePlots()));
    connect(view_, SIGNAL(plotsReordered()), this, SLOT(updatePlots()));

    connect(view_, SIGNAL(connectDataChanged()), this, SLOT(updatePlots()));

    connect(view_, SIGNAL(currentPlotChanged()), this, SLOT(updateCurrentPlot()));
  }

  updatePlots();
}

void
CQChartsPlotTableControl::
updatePlots()
{
  if (! view_)
    return;

  disconnect(plotTable_, SIGNAL(itemSelectionChanged()), this, SLOT(plotsSelectionChangeSlot()));

  plotTable_->updatePlots(view_);

  updatePlotOverlayState();

  plotsSelectionChangeSlot();

  connect(plotTable_, SIGNAL(itemSelectionChanged()), this, SLOT(plotsSelectionChangeSlot()));
}

void
CQChartsPlotTableControl::
groupPlotsSlot()
{
  if (! view_)
    return;

  auto *charts = view_->charts();

  // get selected plots ?
  Plots plots;

  view_->getPlots(plots);

  bool overlay = overlayCheck_->isChecked();
  bool x1x2    = x1x2Check_   ->isChecked();
  bool y1y2    = y1y2Check_   ->isChecked();

  if      (x1x2) {
    if (plots.size() < 2) {
      charts->errorMsg("Need 2 (or more) plots for x1x2");
      return;
    }

    view_->initX1X2(plots, overlay, /*reset*/true);
  }
  else if (y1y2) {
    if (plots.size() < 2) {
      charts->errorMsg("Need 2 (or more) plots for y1y2");
      return;
    }

    view_->initY1Y2(plots, overlay, /*reset*/true);
  }
  else if (overlay) {
    if (plots.size() < 2) {
      charts->errorMsg("Need 2 or more plots for overlay");
      return;
    }

    view_->initOverlay(plots, /*reset*/true);
  }
  else {
    view_->resetGrouping();
  }
}

void
CQChartsPlotTableControl::
placePlotsSlot()
{
  if (! view_)
    return;

  // get selected plots ?
  Plots plots;

  view_->getPlots(plots);

  bool vertical   = placeVerticalRadio_  ->isChecked();
  bool horizontal = placeHorizontalRadio_->isChecked();
  int  rows       = placeRowsEdit_       ->value();
  int  columns    = placeColumnsEdit_    ->value();

  view_->placePlots(plots, vertical, horizontal, rows, columns, /*reset*/false);
}

void
CQChartsPlotTableControl::
raisePlotSlot()
{
  if (! view_)
    return;

  auto *plot = getSelectedPlot();

  if (plot)
    view_->raisePlot(plot);
}

void
CQChartsPlotTableControl::
lowerPlotSlot()
{
  if (! view_)
    return;

  auto *plot = getSelectedPlot();

  if (plot)
    view_->lowerPlot(plot);
}

void
CQChartsPlotTableControl::
createPlotSlot()
{
  if (! view_)
    return;

  auto *charts = view_->charts();

  auto *modelData = charts->currentModelData();
  if (! modelData) return;

  auto *createPlotDlg = charts->createPlotDlg(modelData);

  createPlotDlg->setViewName(view_->id());
}

void
CQChartsPlotTableControl::
removePlotsSlot()
{
  if (! view_)
    return;

  Plots plots;

  getSelectedPlots(plots);

  for (auto &plot : plots)
    view_->removePlot(plot);

  //updateView();
}

void
CQChartsPlotTableControl::
plotsSelectionChangeSlot()
{
  if (! view_)
    return;

  Plots plots;

  getSelectedPlots(plots);

  raiseButton_ ->setEnabled(plots.size() == 1);
  lowerButton_ ->setEnabled(plots.size() == 1);
//createButton_->setEnabled(true);
  removeButton_->setEnabled(plots.size() > 0);

  //---

  view_->startSelection();

  view_->deselectAll();

  for (auto &plot : plots) {
    if (! plot->isSelectable())
      continue;

    plot->setSelected(true);
  }

  view_->endSelection();
}

CQChartsPlot *
CQChartsPlotTableControl::
getSelectedPlot() const
{
  Plots plots;

  getSelectedPlots(plots);

  if (plots.size() == 1)
    return plots[0];

  return nullptr;
}

void
CQChartsPlotTableControl::
getSelectedPlots(Plots &plots) const
{
  if (! view_)
    return;

  plotTable_->getSelectedPlots(view_, plots);
}

//------

CQChartsPlotTable::
CQChartsPlotTable()
{
  setObjectName("plotTable");

  horizontalHeader()->setStretchLastSection(true);

  setSelectionMode(ExtendedSelection);

  setSelectionBehavior(QAbstractItemView::SelectRows);
}

void
CQChartsPlotTable::
updatePlots(CQChartsView *view)
{
  if (! view)
    return;

  // add plots to plot table (id, type and state)
  clear();

  int np = view->numPlots();

  setColumnCount(4);
  setRowCount(np);

  setHorizontalHeaderItem(0, new QTableWidgetItem("Id"     ));
  setHorizontalHeaderItem(1, new QTableWidgetItem("Type"   ));
  setHorizontalHeaderItem(2, new QTableWidgetItem("Connect"));
  setHorizontalHeaderItem(3, new QTableWidgetItem("Model"  ));

  auto createItem = [&](const QString &name, int r, int c) {
    auto *item = new QTableWidgetItem(name);

    item->setToolTip(name);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    setItem(r, c, item);

    return item;
  };

  for (int i = 0; i < np; ++i) {
    auto *plot = view->plot(i);

    //--

    // set id item store plot index in user data
    auto *idItem = createItem(plot->id(), i, 0);

    idItem->setData(Qt::UserRole, view->getIndForPlot(plot));

    // set type item
    (void) createItem(plot->type()->name(), i, 1);

    // set state item
    (void) createItem(plot->connectionStateStr(), i, 2);

    // set model item
    auto *modelData = plot->getModelData();

    if (modelData)
      (void) createItem(modelData->id(), i, 3);
  }
}

void
CQChartsPlotTable::
setCurrentInd(int ind)
{
  int nr = rowCount();

  for (int i = 0; i < nr; ++i) {
    auto *item = this->item(i, 0);

    bool ok;

    long ind1 = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);

    item->setSelected(ind1 == ind);
  }
}

void
CQChartsPlotTable::
getSelectedPlots(CQChartsView *view, std::vector<CQChartsPlot *> &plots)
{
  auto items = selectedItems();

  for (int i = 0; i < items.length(); ++i) {
    auto *item = items[i];
    if (item->column() != 0) continue;

    auto id = item->text();

    auto *plot = view->getPlotForId(id);

    plots.push_back(plot);
  }
}
