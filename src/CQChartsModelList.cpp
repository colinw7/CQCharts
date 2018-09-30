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

  setDetails(modelData);
}

void
CQChartsModelList::
updateModelType(int ind)
{
  CQChartsModelData *modelData = charts_->getModelData(ind);

  if (! modelData)
    return;

  setDetails(modelData);
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
currentTabChanged(int ind)
{
  for (auto &p : viewWidgetDatas_) {
    CQChartsViewWidgetData *viewWidgetDatas = p.second;

    if (viewWidgetDatas->tabInd == ind)
      charts_->setCurrentModelInd(viewWidgetDatas->ind);
  }
}

CQChartsModelData *
CQChartsModelList::
currentModelData() const
{
  int ind = viewTab_->currentIndex();

  for (auto &p : viewWidgetDatas_) {
    CQChartsViewWidgetData *viewWidgetDatas = p.second;

    if (viewWidgetDatas->tabInd == ind) {
      return charts_->getModelData(viewWidgetDatas->ind);
    }
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
tableColumnClicked(int column)
{
  using ModelP = QSharedPointer<QAbstractItemModel>;

  CQChartsTable *table = qobject_cast<CQChartsTable *>(sender());

  ModelP model = table->modelP();

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

  if (modelData->details()->isHierarchical()) {
    if (viewWidgetData->tree) {
      viewWidgetData->tree->setModelP(modelData->currentModel());

      modelData->setSelectionModel(viewWidgetData->tree->selectionModel());
    }
    else
      modelData->setSelectionModel(nullptr);

    viewWidgetData->stack->setCurrentIndex(0);
  }
  else {
    if (viewWidgetData->table) {
      viewWidgetData->table->setModelP(modelData->currentModel());

      modelData->setSelectionModel(viewWidgetData->table->selectionModel());
    }
    else
      modelData->setSelectionModel(nullptr);

    viewWidgetData->stack->setCurrentIndex(1);
  }
}

void
CQChartsModelList::
setDetails(const CQChartsModelData *modelData)
{
  CQChartsViewWidgetData *viewWidgetData = this->viewWidgetData(modelData->ind());
  assert(viewWidgetData);

  CQChartsModelData *modelData1 = nullptr;

  CQChartsModelDetails *details = nullptr;

  if (viewWidgetData->stack->currentIndex() == 0) {
    if (viewWidgetData->tree)
      modelData1 = charts_->getModelData(viewWidgetData->tree->modelP().data());
  }
  else {
    if (viewWidgetData->table)
      modelData1 = charts_->getModelData(viewWidgetData->table->modelP().data());
  }

  if (! modelData1)
    modelData1 = const_cast<CQChartsModelData *>(modelData);

  if (modelData1)
    details = modelData1->details();

  if (! details)
    return;

  int nc = details->numColumns();
  int nr = details->numRows   ();

  //---

  viewWidgetData->detailsTable->clear();

  viewWidgetData->detailsTable->setColumnCount(7);

  viewWidgetData->detailsTable->setHorizontalHeaderLabels(QStringList() <<
    "Column" << "Type" << "Min" << "Max" << "Mean" << "StdDev" << "Monotonic");

  viewWidgetData->detailsTable->setRowCount(nc);

  auto columnDetails = [&](int c, QString &nameStr, QString &typeStr, QString &minStr,
                           QString &maxStr, QString &meanStr, QString &stdDevStr,
                           QString &monoStr) {
    const CQChartsModelColumnDetails *columnDetails = details->columnDetails(c);

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
  };

  auto setTableRow = [&](int c) {
    QString nameStr, typeStr, minStr, maxStr, meanStr, stdDevStr, monoStr;

    columnDetails(c, nameStr, typeStr, minStr, maxStr, meanStr, stdDevStr, monoStr);

    QTableWidgetItem *item1 = new QTableWidgetItem(nameStr);
    QTableWidgetItem *item2 = new QTableWidgetItem(typeStr);
    QTableWidgetItem *item3 = new QTableWidgetItem(minStr);
    QTableWidgetItem *item4 = new QTableWidgetItem(maxStr);
    QTableWidgetItem *item5 = new QTableWidgetItem(meanStr);
    QTableWidgetItem *item6 = new QTableWidgetItem(stdDevStr);
    QTableWidgetItem *item7 = new QTableWidgetItem(monoStr);

    viewWidgetData->detailsTable->setItem(c, 0, item1);
    viewWidgetData->detailsTable->setItem(c, 1, item2);
    viewWidgetData->detailsTable->setItem(c, 2, item3);
    viewWidgetData->detailsTable->setItem(c, 3, item4);
    viewWidgetData->detailsTable->setItem(c, 4, item5);
    viewWidgetData->detailsTable->setItem(c, 5, item6);
    viewWidgetData->detailsTable->setItem(c, 6, item7);
  };

  //---

  QString text = "<b></b>";

  text += "<table padding=\"4\">";
  text += QString("<tr><td>Columns</td><td>%1</td></tr>").arg(nc);
  text += QString("<tr><td>Rows</td><td>%1</td></tr>").arg(nr);
  text += "</table>";

#if 0
  text += "<br>";

  text += "<table padding=\"4\">";
  text += "<tr><th>Column</th><th>Type</th><th>Min</th><th>Max</th><th>Monotonic</th></tr>";

  for (int c = 0; c < nc; ++c) {
    text += "<tr>";

    QString nameStr, typeStr, minStr, maxStr, meanStr, stdDevStr, monoStr;

    columnDetails(c, nameStr, typeStr, minStr, maxStr, meanStr, stdDevStr, monoStr);

    text += QString("<td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td>").
             arg(nameStr).arg(typeStr).arg(minStr).arg(maxStr).arg(monoStr);

    text += "</tr>";
  }

  text += "</table>";
#endif

  viewWidgetData->detailsText->setHtml(text);

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
