#include <CQChartsModelList.h>
#include <CQChartsModelControl.h>
#include <CQChartsTable.h>
#include <CQChartsTree.h>
#include <CQCharts.h>
#include <CQUtil.h>

#include <QVBoxLayout>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QLineEdit>

CQChartsModelList::
CQChartsModelList(CQCharts *charts) :
 charts_(charts)
{
  QVBoxLayout *layout = new QVBoxLayout(this);

  //---

  viewTab_ = CQUtil::makeWidget<QTabWidget>("viewTab");

  layout->addWidget(viewTab_);

  connect(viewTab_, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

  //---

  connect(charts, SIGNAL(modelDataAdded(int)), this, SLOT(addModelData(int)));

  connect(charts, SIGNAL(modelTypeChanged(int)), this, SLOT(updateModelType(int)));
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

  if (! modelData)
    return;

  addModelData(modelData);
}

void
CQChartsModelList::
addModelData(CQChartsModelData *modelData)
{
  addModelDataWidgets(modelData);

  updateModel(modelData);

  if (modelControl_)
    modelControl_->setEnabled(numModels() > 0);
}

void
CQChartsModelList::
updateModel(CQChartsModelData *modelData)
{
  reloadModel(modelData);

  setDetailsText(modelData);
}

void
CQChartsModelList::
updateModelType(int ind)
{
  CQChartsModelData *modelData = charts_->getModelData(ind);

  if (! modelData)
    return;

  setDetailsText(modelData);
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

  tableTab->addTab(viewFrame, "Model");

  //---

  QFrame *detailsFrame = CQUtil::makeWidget<QFrame>("details");

  QVBoxLayout *detailsLayout = new QVBoxLayout(detailsFrame);

  tableTab->addTab(detailsFrame, "Details");

  //---

  QLineEdit *filterEdit = CQUtil::makeWidget<QLineEdit>("filter");

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

  viewWidgetData->tree = tree;

  //---

  CQChartsTable *table = new CQChartsTable(charts_);

  stack->addWidget(table);

  connect(table, SIGNAL(columnClicked(int)), this, SLOT(tableColumnClicked(int)));

  viewWidgetData->table = table;

  //------

  QTextEdit *detailsText = CQUtil::makeWidget<QTextEdit>("detailsText");

  detailsText->setReadOnly(true);

  detailsLayout->addWidget(detailsText);

  viewWidgetData->detailsText = detailsText;
}

void
CQChartsModelList::
currentTabChanged(int ind)
{
  for (auto &p : viewWidgetDatas_) {
    CQChartsViewWidgetData *viewWidgetDatas = p.second;

    if (viewWidgetDatas->tabInd == ind)
      charts_->setCurrentModelInd(viewWidgetDatas->ind);
  }
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
tableColumnClicked(int column)
{
  using ModelP = QSharedPointer<QAbstractItemModel>;

  CQChartsTable *table = qobject_cast<CQChartsTable *>(sender());

  ModelP model = table->model();

  QString headerStr = model->headerData(column, Qt::Horizontal).toString();

  QString typeStr;

  if (! CQChartsUtil::columnTypeStr(charts_, model.data(), column, typeStr))
    typeStr = "";

  //---

  if (modelControl_)
    modelControl_->setColumnData(column, headerStr, typeStr);
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
redrawView(const CQChartsModelData *modelData)
{
  CQChartsViewWidgetData *viewWidgetData = this->viewWidgetData(modelData->ind());
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
reloadModel(CQChartsModelData *modelData)
{
  CQChartsViewWidgetData *viewWidgetData = this->viewWidgetData(modelData->ind());
  assert(viewWidgetData);

#ifdef CQCHARTS_FOLDED_MODEL
  using ModelP = QSharedPointer<QAbstractItemModel>;

  if (! modelData->foldedModels().empty()) {
    if (viewWidgetData->table)
      viewWidgetData->table->setModel(ModelP());

    if (viewWidgetData->tree)
      viewWidgetData->tree->setModel(modelData->foldProxyModel());

    viewWidgetData->stack->setCurrentIndex(0);
  }
  else
#endif
  {
    if (modelData->details()->isHierarchical()) {
      if (viewWidgetData->tree) {
        viewWidgetData->tree->setModel(modelData->model());

        modelData->setSelectionModel(viewWidgetData->tree->selectionModel());
      }
      else
        modelData->setSelectionModel(nullptr);

      viewWidgetData->stack->setCurrentIndex(0);
    }
    else {
      if (viewWidgetData->table) {
        viewWidgetData->table->setModel(modelData->model());

        modelData->setSelectionModel(viewWidgetData->table->selectionModel());
      }
      else
        modelData->setSelectionModel(nullptr);

      viewWidgetData->stack->setCurrentIndex(1);
    }
  }
}

void
CQChartsModelList::
setDetailsText(const CQChartsModelData *modelData)
{
  CQChartsViewWidgetData *viewWidgetData = this->viewWidgetData(modelData->ind());
  assert(viewWidgetData);

  CQChartsModelData *modelData1 = nullptr;

  CQChartsModelDetails *details = nullptr;

  if (viewWidgetData->stack->currentIndex() == 0) {
    if (viewWidgetData->tree) {
      modelData1 = charts_->getModelData(viewWidgetData->tree->model().data());

      if (modelData1)
        details = modelData1->details();
    }
  }
  else {
    if (viewWidgetData->table) {
      modelData1 = charts_->getModelData(viewWidgetData->table->model().data());

      if (modelData1)
        details = modelData1->details();
    }
  }

  if (! details)
    return;

  //---

  QString text = "<b></b>";

  text += "<table padding=\"4\">";
  text += QString("<tr><td>Columns</td><td>%1</td></tr>").arg(details->numColumns());
  text += QString("<tr><td>Rows</td><td>%1</td></tr>").arg(details->numRows());
  text += "</table>";

  text += "<br>";

  text += "<table padding=\"4\">";
  text += "<tr><th>Column</th><th>Type</th><th>Min</th><th>Max</th><th>Monotonic</th></tr>";

  for (int i = 0; i < details->numColumns(); ++i) {
    const CQChartsModelColumnDetails *columnDetails = details->columnDetails(i);

    text += "<tr>";

    text += QString("<td>%1</td><td>%2</td><td>%3</td><td>%4</td>").
             arg(i + 1).
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

  viewWidgetData->detailsText->setHtml(text);
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
