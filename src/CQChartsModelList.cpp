#include <CQChartsModelList.h>
#include <CQChartsModelControl.h>
#include <CQChartsTable.h>
#include <CQChartsTree.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQCharts.h>
#include <CQTableWidget.h>
#include <CQUtil.h>

#include <QVBoxLayout>
#include <QStackedWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>

CQChartsModelList::
CQChartsModelList(CQCharts *charts) :
 charts_(charts)
{
  setObjectName("modelList");

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  //---

  viewTab_ = CQUtil::makeWidget<QTabWidget>("viewTab");

  layout->addWidget(viewTab_);

  connect(viewTab_, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

  //---

  connect(charts, SIGNAL(modelDataAdded(int)), this, SLOT(addModelData(int)));

  connect(charts, SIGNAL(modelTypeChanged(int)), this, SLOT(updateModelType(int)));

  connect(charts, SIGNAL(currentModelChanged(int)), this, SLOT(updateCurrentModel()));
}

CQChartsModelList::
~CQChartsModelList()
{
  for (auto &p : viewWidgetDatas_)
    delete p.second;
}

void
CQChartsModelList::
addModelData(int ind)
{
  CQChartsModelData *modelData = charts_->getModelData(ind);
  if (! modelData) return;

  addModelData(modelData);
}

void
CQChartsModelList::
addModelData(CQChartsModelData *modelData)
{
  addModelDataWidgets(modelData);

  charts_->setCurrentModelData(modelData);
}

void
CQChartsModelList::
updateCurrentModel()
{
  modelData_ = charts_->currentModelData();

  updateModel();
}

void
CQChartsModelList::
updateModel()
{
  reloadModel();

  setDetails();
}

void
CQChartsModelList::
updateModelType(int ind)
{
  if (! modelData_ || modelData_->ind() != ind)
    return;

  setDetails();
}

void
CQChartsModelList::
addModelDataWidgets(CQChartsModelData *modelData)
{
  CQChartsViewWidgetData *viewWidgetData = new CQChartsViewWidgetData;

  viewWidgetData->ind = modelData->ind();

  viewWidgetDatas_[viewWidgetData->ind] = viewWidgetData;

  //---

  QTabWidget *tableTab = CQUtil::makeWidget<QTabWidget>("tableTab");

  int tabInd = viewTab_->addTab(tableTab, QString("Model %1").arg(viewWidgetData->ind));

  viewTab_->setCurrentIndex(viewTab_->count() - 1);

  viewWidgetData->tabInd = tabInd;

  //---

  QFrame *viewFrame = CQUtil::makeWidget<QFrame>("view");

  QVBoxLayout *viewLayout = new QVBoxLayout(viewFrame);
  viewLayout->setMargin(2); viewLayout->setSpacing(2);

  tableTab->addTab(viewFrame, "Model");

  //---

  QFrame *detailsFrame = CQUtil::makeWidget<QFrame>("details");

  QVBoxLayout *detailsLayout = new QVBoxLayout(detailsFrame);
  detailsLayout->setMargin(2); detailsLayout->setSpacing(2);

  tableTab->addTab(detailsFrame, "Details");

  //---

  QLineEdit *filterEdit = CQUtil::makeWidget<QLineEdit>("filter");

  filterEdit->setToolTip("Filter Model by Tcl Expression\n"
                         "Use column name variable or column(<col>)\n"
                         "Use row(<row>), cell(<row>,<col>), header(<col>)\n"
                         "Use \"selected\" or \"non-selected\" for selected items");

  viewLayout->addWidget(filterEdit);

  connect(filterEdit, SIGNAL(returnPressed()), this, SLOT(filterSlot()));

  viewWidgetData->filterEdit = filterEdit;

  //---

  QStackedWidget *stack = CQUtil::makeWidget<QStackedWidget>("stack");

  viewLayout->addWidget(stack);

  viewWidgetData->stack = stack;

  //---

  CQChartsTree *tree = new CQChartsTree(charts_);

  stack->addWidget(tree);

  connect(tree, SIGNAL(columnClicked(int)), this, SLOT(treeColumnClicked(int)));
  connect(tree, SIGNAL(selectionChanged()), this, SLOT(treeSelectionChanged()));

  viewWidgetData->tree = tree;

  //---

  CQChartsTable *table = new CQChartsTable(charts_);

  stack->addWidget(table);

  connect(table, SIGNAL(columnClicked(int)), this, SLOT(tableColumnClicked(int)));
  connect(table, SIGNAL(selectionChanged()), this, SLOT(tableSelectionChanged()));

  viewWidgetData->table = table;

  //------

  QHBoxLayout *detailsControlLayout = new QHBoxLayout;
  detailsControlLayout->setMargin(2); detailsControlLayout->setSpacing(2);

  viewWidgetData->detailsUpdate = new QPushButton("Update");

  connect(viewWidgetData->detailsUpdate, SIGNAL(clicked()), this, SLOT(updateDetails()));

  detailsControlLayout->addWidget(viewWidgetData->detailsUpdate);
  detailsControlLayout->addStretch(1);

  detailsLayout->addLayout(detailsControlLayout);

  QTextEdit *detailsText = CQUtil::makeWidget<QTextEdit>("detailsText");

  detailsText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  detailsText->setReadOnly(true);

  detailsLayout->addWidget(detailsText);

  viewWidgetData->detailsText = detailsText;

  CQTableWidget *detailsTable = CQUtil::makeWidget<CQTableWidget>("detailsText");

  detailsLayout->addWidget(detailsTable);

  viewWidgetData->detailsTable = detailsTable;
}

void
CQChartsModelList::
currentTabChanged(int)
{
  CQChartsViewWidgetData *viewWidgetData = currentViewWidgetData();

  if (viewWidgetData)
    charts_->setCurrentModelInd(viewWidgetData->ind);
}

CQChartsModelData *
CQChartsModelList::
currentModelData() const
{
  CQChartsViewWidgetData *viewWidgetData = currentViewWidgetData();

  if (viewWidgetData)
    return charts_->getModelData(viewWidgetData->ind);

  return nullptr;
}

CQChartsViewWidgetData *
CQChartsModelList::
currentViewWidgetData() const
{
  int ind = viewTab_->currentIndex();

  for (auto &p : viewWidgetDatas_) {
    CQChartsViewWidgetData *viewWidgetData = p.second;

    if (viewWidgetData->tabInd == ind)
      return viewWidgetData;
  }

  return nullptr;
}

void
CQChartsModelList::
filterSlot()
{
  QLineEdit *filterEdit = qobject_cast<QLineEdit *>(sender());

  for (auto &p : viewWidgetDatas_) {
    CQChartsViewWidgetData *viewWidgetData = p.second;

    if (viewWidgetData->filterEdit == filterEdit) {
      if (viewWidgetData->stack->currentIndex() == 0) {
        if (viewWidgetData->tree)
          viewWidgetData->tree->setFilter(filterEdit->text());
      }
      else {
        if (viewWidgetData->table)
          viewWidgetData->table->setFilter(filterEdit->text());
      }
    }
  }
}

void
CQChartsModelList::
treeColumnClicked(int column)
{
  if (modelData_)
    modelData_->setCurrentColumn(column);
}

void
CQChartsModelList::
treeSelectionChanged()
{
}

void
CQChartsModelList::
tableColumnClicked(int column)
{
  if (modelData_)
    modelData_->setCurrentColumn(column);
}

void
CQChartsModelList::
tableSelectionChanged()
{
  CQChartsViewWidgetData *viewWidgetData = currentViewWidgetData();

  if (viewWidgetData) {
    QItemSelectionModel *sm = viewWidgetData->table->selectionModel();

    QModelIndexList inds = sm->selectedColumns();

    if (inds.size() == 1) {
      int column = inds[0].column();

      if (modelData_)
        modelData_->setCurrentColumn(column);
    }
  }
}

void
CQChartsModelList::
setTabTitle(int ind, const QString &title)
{
  CQChartsViewWidgetData *viewWidgetData = this->viewWidgetData(ind);
  assert(viewWidgetData);

  if (viewWidgetData->stack->currentIndex() == 0) {
    if (viewWidgetData->tree)
      viewWidgetData->tree->setWindowTitle(title);
  }
  else {
    if (viewWidgetData->table)
      viewWidgetData->table->setWindowTitle(title);
  }

  viewTab()->setTabText(viewWidgetData->tabInd, title);
}

void
CQChartsModelList::
redrawView()
{
  if (! modelData_)
    return;

  CQChartsViewWidgetData *viewWidgetData = this->viewWidgetData(modelData_->ind());
  assert(viewWidgetData);

  if (viewWidgetData->stack->currentIndex() == 0) {
    if (viewWidgetData->tree)
      viewWidgetData->tree->update();
  }
  else {
    if (viewWidgetData->table)
      viewWidgetData->table->update();
  }
}

void
CQChartsModelList::
reloadModel()
{
  if (! modelData_)
    return;

  CQChartsViewWidgetData *viewWidgetData = this->viewWidgetData(modelData_->ind());
  assert(viewWidgetData);

  if (modelData_->details()->isHierarchical()) {
    if (viewWidgetData->tree) {
      viewWidgetData->tree->setModelP(modelData_->currentModel());

      modelData_->setSelectionModel(viewWidgetData->tree->selectionModel());
    }
    else
      modelData_->setSelectionModel(nullptr);

    viewWidgetData->stack->setCurrentIndex(0);
  }
  else {
    if (viewWidgetData->table) {
      viewWidgetData->table->setModelP(modelData_->currentModel());

      modelData_->setSelectionModel(viewWidgetData->table->selectionModel());
    }
    else
      modelData_->setSelectionModel(nullptr);

    viewWidgetData->stack->setCurrentIndex(1);
  }
}

void
CQChartsModelList::
setDetails()
{
  if (! modelData_)
    return;

  currentViewWidgetData_ = this->viewWidgetData(modelData_->ind());
  assert(currentViewWidgetData_);

  //---

  const CQChartsModelData *modelData1 = nullptr;

  if (currentViewWidgetData_->stack->currentIndex() == 0) {
    if (currentViewWidgetData_->tree)
      modelData1 = charts_->getModelData(currentViewWidgetData_->tree->modelP().data());
  }
  else {
    if (currentViewWidgetData_->table)
      modelData1 = charts_->getModelData(currentViewWidgetData_->table->modelP().data());
  }

  if (! modelData1)
    modelData1 = modelData_;

  //---

  if (currentDetails_)
    disconnect(currentDetails_, SIGNAL(detailsReset()), this, SLOT(invalidateDetails()));

  currentDetails_ = nullptr;

  if (modelData1)
    currentDetails_ = modelData1->details();

  if (! currentDetails_)
    return;

  connect(currentDetails_, SIGNAL(detailsReset()), this, SLOT(invalidateDetails()));

  //---

  invalidateDetails();
}

void
CQChartsModelList::
invalidateDetails()
{
  currentViewWidgetData_->detailsUpdate->setEnabled(true);
}

void
CQChartsModelList::
updateDetails()
{
  assert(currentDetails_);

  //---

  currentViewWidgetData_->detailsUpdate->setEnabled(false);

  //---

  int nc = currentDetails_->numColumns();
  int nr = currentDetails_->numRows   ();

  //---

  currentViewWidgetData_->detailsTable->clear();

  QStringList columnNames = (QStringList() <<
    "Column" << "Type" << "Min" << "Max" << "Mean" << "StdDev" <<
    "Monotonic" << "Num Unique" << "Num Null");

  currentViewWidgetData_->detailsTable->setColumnCount(columnNames.length());

  currentViewWidgetData_->detailsTable->setHorizontalHeaderLabels(columnNames);

  currentViewWidgetData_->detailsTable->setRowCount(nc);

  auto columnDetails = [&](int c, QString &nameStr, QString &typeStr, QString &minStr,
                           QString &maxStr, QString &meanStr, QString &stdDevStr,
                           QString &monoStr, QString &uniqueStr, QString &nullStr) {
    const CQChartsModelColumnDetails *columnDetails = currentDetails_->columnDetails(c);

    nameStr   = columnDetails->headerName();
    typeStr   = columnDetails->typeName();
    minStr    = columnDetails->dataName(columnDetails->minValue   ()).toString();
    maxStr    = columnDetails->dataName(columnDetails->maxValue   ()).toString();
    meanStr   = columnDetails->dataName(columnDetails->meanValue  ()).toString();
    stdDevStr = columnDetails->dataName(columnDetails->stdDevValue()).toString();

    if (columnDetails->isMonotonic())
      monoStr = (columnDetails->isIncreasing() ? "Increasing" : "Decreasing");
    else
      monoStr = "";

    uniqueStr = QString("%1").arg(columnDetails->numUnique());
    nullStr   = QString("%1").arg(columnDetails->numNull());
  };

  auto setTableRow = [&](int c) {
    QString nameStr, typeStr, minStr, maxStr, meanStr, stdDevStr, monoStr, uniqueStr, nullStr;

    columnDetails(c, nameStr, typeStr, minStr, maxStr, meanStr, stdDevStr,
                  monoStr, uniqueStr, nullStr);

    QTableWidgetItem *item1 = new QTableWidgetItem(nameStr);
    QTableWidgetItem *item2 = new QTableWidgetItem(typeStr);
    QTableWidgetItem *item3 = new QTableWidgetItem(minStr);
    QTableWidgetItem *item4 = new QTableWidgetItem(maxStr);
    QTableWidgetItem *item5 = new QTableWidgetItem(meanStr);
    QTableWidgetItem *item6 = new QTableWidgetItem(stdDevStr);
    QTableWidgetItem *item7 = new QTableWidgetItem(monoStr);
    QTableWidgetItem *item8 = new QTableWidgetItem(uniqueStr);
    QTableWidgetItem *item9 = new QTableWidgetItem(nullStr);

    currentViewWidgetData_->detailsTable->setItem(c, 0, item1);
    currentViewWidgetData_->detailsTable->setItem(c, 1, item2);
    currentViewWidgetData_->detailsTable->setItem(c, 2, item3);
    currentViewWidgetData_->detailsTable->setItem(c, 3, item4);
    currentViewWidgetData_->detailsTable->setItem(c, 4, item5);
    currentViewWidgetData_->detailsTable->setItem(c, 5, item6);
    currentViewWidgetData_->detailsTable->setItem(c, 6, item7);
    currentViewWidgetData_->detailsTable->setItem(c, 7, item8);
    currentViewWidgetData_->detailsTable->setItem(c, 8, item9);
  };

  //---

  QString text = "<b></b>";

  text += "<table padding=\"4\">";
  text += QString("<tr><td>Columns</td><td>%1</td></tr>").arg(nc);
  text += QString("<tr><td>Rows</td><td>%1</td></tr>").arg(nr);
  text += "</table>";

  currentViewWidgetData_->detailsText->setHtml(text);

  //---

  for (int c = 0; c < nc; ++c)
    setTableRow(c);
}

CQChartsViewWidgetData *
CQChartsModelList::
viewWidgetData(int ind) const
{
  auto p = viewWidgetDatas_.find(ind);

  if (p == viewWidgetDatas_.end())
    return nullptr;

  return (*p).second;
}
