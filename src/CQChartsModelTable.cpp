#include <CQChartsModelTable.h>
#include <CQChartsModelData.h>
#include <CQChartsVariant.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsCreatePlotDlg.h>
#include <CQChartsView.h>
#include <CQCharts.h>

#include <QHeaderView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>

CQChartsModelTableControl::
CQChartsModelTableControl(CQCharts *charts) :
 QFrame()
{
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  modelTable_ = new CQChartsModelTable(charts);

  layout->addWidget(modelTable_);

  connect(modelTable_, SIGNAL(itemSelectionChanged()), this, SLOT(modelsSelectionChangeSlot()));

  //---

  // Model Buttons
  auto *modelControlFrame  = CQUtil::makeWidget<QFrame>("modelControlFrame");
  auto *modelControlLayout = CQUtil::makeLayout<QHBoxLayout>(modelControlFrame, 2, 2);

  layout->addWidget(modelControlFrame);

  //--

  auto createButton = [&](const QString &label, const QString &objName, const QString &tip,
                          const char *slotName) {
    auto *button = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    button->setToolTip(tip);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  auto *loadButton = createButton("Load...", "load", "Load Model", SLOT(loadModelSlot()));

  saveButton_   = createButton("Save...", "save", "Save Model to CSV",
                               SLOT(saveModelSlot()));
  editButton_   = createButton("Edit...", "edit", "Edit Selected Model",
                               SLOT(editModelSlot()));
  removeButton_ = createButton("Remove", "remove", "Remove Selected Model",
                               SLOT(removeModelSlot()));

  modelControlLayout->addWidget(loadButton);
  modelControlLayout->addWidget(saveButton_);
  modelControlLayout->addWidget(editButton_);
  modelControlLayout->addWidget(removeButton_);
  modelControlLayout->addStretch(1);

  saveButton_  ->setEnabled(false);
  editButton_  ->setEnabled(false);
  removeButton_->setEnabled(false);

  //---

  // Extra Buttons
  auto *extraControlFrame  = CQUtil::makeWidget<QFrame>("extraControlFrame");
  auto *extraControlLayout = CQUtil::makeLayout<QHBoxLayout>(extraControlFrame, 2, 2);

  layout->addWidget(extraControlFrame);

  plotButton_ = createButton("Create Plot...", "plot", "Create Plot from Selected Model",
                             SLOT(createPlotModelSlot()));

  extraControlLayout->addWidget(plotButton_);
  extraControlLayout->addStretch(1);

  plotButton_->setEnabled(false);

  //---

  setCharts(charts);
}

void
CQChartsModelTableControl::
setCharts(CQCharts *charts)
{
  charts_ = charts;

  modelTable_->setCharts(charts);

  updateState();
}

void
CQChartsModelTableControl::
setView(CQChartsView *view)
{
  view_ = view;
}

void
CQChartsModelTableControl::
modelsSelectionChangeSlot()
{
  long ind = modelTable_->selectedModel();

  if (ind >= 0)
    charts_->setCurrentModelInd(int(ind));

  updateState();
}

void
CQChartsModelTableControl::
updateState()
{
  long ind = modelTable_->selectedModel();

  saveButton_  ->setEnabled(ind >= 0);
  editButton_  ->setEnabled(ind >= 0);
  removeButton_->setEnabled(ind >= 0);
  plotButton_  ->setEnabled(ind >= 0);
}

void
CQChartsModelTableControl::
loadModelSlot()
{
  if (! charts_)
    return;

  (void) charts_->loadModelDlg();
}

void
CQChartsModelTableControl::
saveModelSlot()
{
  auto *modelData = modelTable_->selectedModelData();
  if (! modelData) return;

  auto dir = QDir::current().dirName() + "/model.csv";

  auto filename = QFileDialog::getSaveFileName(this, "Write Model", dir, "Files (*.csv)");
  if (! filename.length()) return; // cancelled

  modelData->writeCSV(filename);
}

void
CQChartsModelTableControl::
editModelSlot()
{
  if (! charts_)
    return;

  auto *modelData = modelTable_->selectedModelData();
  if (! modelData) return;

  charts_->editModelDlg(modelData);
}

void
CQChartsModelTableControl::
removeModelSlot()
{
  if (! charts_)
    return;

  auto *modelData = modelTable_->selectedModelData();
  if (! modelData) return;

  charts_->removeModelData(modelData);
}

void
CQChartsModelTableControl::
createPlotModelSlot()
{
  if (! charts_)
    return;

  auto *modelData = modelTable_->selectedModelData();
  if (! modelData) return;

  auto *createPlotDlg = charts_->createPlotDlg(modelData);

  auto *view = (view_ ? view_ : charts_->currentView());

  if (view)
    createPlotDlg->setViewName(view->id());
}

//------

CQChartsModelTable::
CQChartsModelTable(CQCharts *charts) :
 CQTableWidget()
{
  setObjectName("modelTable");

  horizontalHeader()->setStretchLastSection(true);

//setSelectionMode(ExtendedSelection);

  setSelectionBehavior(QAbstractItemView::SelectRows);

  setCharts(charts);
}

void
CQChartsModelTable::
setCharts(CQCharts *charts)
{
  if (charts_) {
    disconnect(charts_, SIGNAL(modelDataChanged()), this, SLOT(updateModels()));
    disconnect(charts_, SIGNAL(modelDataDataChanged()), this, SLOT(updateModels()));
  }

  charts_ = charts;

  if (charts_) {
    updateModels();

    connect(charts_, SIGNAL(modelDataChanged()), this, SLOT(updateModels()));
    connect(charts_, SIGNAL(modelDataDataChanged()), this, SLOT(updateModels()));
  }
}

void
CQChartsModelTable::
updateModels()
{
  headerNames_ = QStringList() << "Name" << "Index" << "Filename" << "Object Name";

  CQCharts::ModelDatas modelDatas;

  charts_->getModelDatas(modelDatas);

  clear();

  setColumnCount(4);
  setRowCount(int(modelDatas.size()));

  for (int i = 0; i < headerNames_.length(); ++i)
    setHorizontalHeaderItem(i, new QTableWidgetItem(headerNames_[i]));

  auto createItem = [&](const QString &name, int r, int c) {
    auto *item = new QTableWidgetItem(name);

    item->setToolTip(name);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    setItem(r, c, item);

    return item;
  };

  int i = 0;

  for (const auto &modelData : modelDatas) {
    auto *nameItem = createItem(modelData->id(), i, 0);

    nameItem->setData(Qt::UserRole, modelData->ind());

    (void) createItem(QString::number(modelData->ind()), i, 1);
    (void) createItem(modelData->filename()            , i, 2);
    (void) createItem(modelData->model()->objectName() , i, 3);

    ++i;
  }
}

CQChartsModelData *
CQChartsModelTable::
selectedModelData() const
{
  long ind = selectedModel();
  if (ind < 0) return nullptr;

  auto *modelData = charts_->getModelDataByInd(int(ind));

  return modelData;
}

int
CQChartsModelTable::
selectedModel() const
{
  auto items = selectedItems();

  for (int i = 0; i < items.length(); ++i) {
    auto *item = items[i];
    if (item->column() != 0) continue;

    bool ok;

    long ind = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);
    if (! ok) return -1;

    return int(ind);
  }

  return -1;
}

QSize
CQChartsModelTable::
sizeHint() const
{
  //auto s = CQTableWidget::sizeHint();

  int nr = rowCount();

  QFontMetrics fm(font());

  int tw = 0;

  for (const auto &name : headerNames_)
    tw += fm.horizontalAdvance(name) + 4;

  int rh = fm.height() + 4;
  int th = (nr + 1)*rh;

  return QSize(tw, th);
}
