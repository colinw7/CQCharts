#include <CQChartsModelList.h>
#include <CQChartsModelControl.h>
#include <CQChartsModelDetailsWidget.h>
#include <CQChartsTable.h>
#include <CQChartsTree.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQCharts.h>

#include <CQTableWidget.h>
#include <CQLineEdit.h>
#include <CQUtil.h>

#include <QVBoxLayout>
#include <QStackedWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QTextEdit>

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

  CQLineEdit *filterEdit = CQUtil::makeWidget<CQLineEdit>("filter");

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

  viewWidgetData->detailsWidget = new CQChartsModelDetailsWidget(charts_);

  detailsLayout->addWidget(viewWidgetData->detailsWidget);
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
  CQLineEdit *filterEdit = qobject_cast<CQLineEdit *>(sender());

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

  const CQChartsModelDetails *details = (modelData1 ? modelData1->details() : nullptr);

  currentViewWidgetData_->detailsWidget->setDetails(details);
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
