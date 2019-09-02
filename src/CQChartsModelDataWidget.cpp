#include <CQChartsModelDataWidget.h>
#include <CQChartsModelDetailsWidget.h>

#ifdef CQCHARTS_MODEL_VIEW
#include <CQChartsModelView.h>
#else
#include <CQChartsTable.h>
#include <CQChartsTree.h>
#endif

#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQCharts.h>

#include <CQLineEdit.h>
#include <CQUtil.h>

#include <QVBoxLayout>
#include <QStackedWidget>
#include <QTabWidget>
#include <set>

CQChartsModelDataWidget::
CQChartsModelDataWidget(CQCharts *charts, CQChartsModelData *modelData) :
 QFrame(nullptr), charts_(charts), modelData_(modelData)
{
  setObjectName("modelData");

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //--

  // model/details tab
  QTabWidget *tableTab = CQUtil::makeWidget<QTabWidget>("tableTab");

  layout->addWidget(tableTab);

  //---

  // model frame
  QFrame *viewFrame = CQUtil::makeWidget<QFrame>("view");

  QVBoxLayout *viewLayout = CQUtil::makeLayout<QVBoxLayout>(viewFrame, 2, 2);

  tableTab->addTab(viewFrame, "Model");

  //---

  // details frame
  QFrame *detailsFrame = CQUtil::makeWidget<QFrame>("details");

  QVBoxLayout *detailsLayout = CQUtil::makeLayout<QVBoxLayout>(detailsFrame, 2, 2);

  tableTab->addTab(detailsFrame, "Details");

  //---

  // model filter
  CQLineEdit *filterEdit = CQUtil::makeWidget<CQLineEdit>("filter");

  filterEdit->setToolTip("Filter Model by Tcl Expression\n"
                         "Use column name variable or column(<col>)\n"
                         "Use row(<row>), cell(<row>,<col>), header(<col>)\n"
                         "Use \"selected\" or \"non-selected\" for selected items");

  viewLayout->addWidget(filterEdit);

  connect(filterEdit, SIGNAL(returnPressed()), this, SLOT(filterSlot()));

  //---

#ifdef CQCHARTS_MODEL_VIEW
  view_ = new CQChartsModelView(charts_);

  viewLayout->addWidget(view_);

  connect(view_, SIGNAL(columnClicked(int)), this, SLOT(columnClicked(int)));
  connect(view_, SIGNAL(selectionHasChanged()), this, SLOT(selectionChanged()));
#else
  // table/tree stack
  stack_ = CQUtil::makeWidget<QStackedWidget>("stack");

  viewLayout->addWidget(stack_);

  //---

  // tree (hier data)
  tree_ = new CQChartsTree(charts_);

  tree_->setSelectionBehavior(QAbstractItemView::SelectItems);

  stack_->addWidget(tree_);

  connect(tree_, SIGNAL(columnClicked(int)), this, SLOT(columnClicked(int)));
  connect(tree_, SIGNAL(selectionHasChanged()), this, SLOT(selectionChanged()));

  //---

  // table (flat data)
  table_ = new CQChartsTable(charts_);

  table_->setSelectionBehavior(QAbstractItemView::SelectItems);

  stack_->addWidget(table_);

  connect(table_, SIGNAL(columnClicked(int)), this, SLOT(columnClicked(int)));
  connect(table_, SIGNAL(selectionHasChanged()), this, SLOT(selectionChanged()));
#endif

  //------

  // details
  detailsWidget_ = new CQChartsModelDetailsWidget(charts_);

  detailsLayout->addWidget(detailsWidget_);

  //---

  reloadModel();
  setDetails();
}

CQChartsModelDataWidget::
~CQChartsModelDataWidget()
{
}

void
CQChartsModelDataWidget::
setModelData(CQChartsModelData *modelData)
{
  modelData_ = modelData;

  reloadModel();
  setDetails();
}

void
CQChartsModelDataWidget::
filterSlot()
{
  CQLineEdit *filterEdit = qobject_cast<CQLineEdit *>(sender());

#ifdef CQCHARTS_MODEL_VIEW
  view_->setFilter(filterEdit->text());
#else
  if (stack_->currentIndex() == 0) {
    if (tree_)
      tree_->setFilter(filterEdit->text());
  }
  else {
    if (table_)
      table_->setFilter(filterEdit->text());
  }
#endif
}

void
CQChartsModelDataWidget::
columnClicked(int column)
{
  if (modelData_)
    modelData_->setCurrentColumn(column);
}

void
CQChartsModelDataWidget::
selectionChanged()
{
#ifdef CQCHARTS_MODEL_VIEW
#else
  if      (tree_) {
  }
  else if (table_) {
    QItemSelectionModel *sm = table_->selectionModel();

    std::set<int> columns;

    if      (table_->selectionBehavior() == QAbstractItemView::SelectColumns) {
      QModelIndexList inds = sm->selectedColumns();

      if (inds.size() >= 1) {
        int column = inds[0].column();

        columns.insert(column);
      }
    }
    else if (table_->selectionBehavior() == QAbstractItemView::SelectItems) {
      QModelIndexList inds = sm->selectedIndexes();

      for (int i = 0; i < inds.length(); ++i)
        columns.insert(inds[i].column());
    }

    if (columns.size() >= 1) {
      int column = *columns.begin();

      modelData_->setCurrentColumn(column);
    }
  }
#endif
}

void
CQChartsModelDataWidget::
reloadModel()
{
  if (! modelData_)
    return;

#ifdef CQCHARTS_MODEL_VIEW
  view_->setModelP(modelData_->currentModel());
#else
  if (modelData_->details()->isHierarchical()) {
    if (tree_)
      tree_->setModelP(modelData_->currentModel());

    stack_->setCurrentIndex(0);
  }
  else {
    if (table_)
      table_->setModelP(modelData_->currentModel());

    stack_->setCurrentIndex(1);
  }
#endif
}

void
CQChartsModelDataWidget::
setDetails()
{
  if (! modelData_)
    return;

  //---

  const CQChartsModelData *modelData1 = nullptr;

#ifdef CQCHARTS_MODEL_VIEW
  modelData1 = charts_->getModelData(view_->modelP().data());
#else
  if (stack_->currentIndex() == 0) {
    if (tree_)
      modelData1 = charts_->getModelData(tree_->modelP().data());
  }
  else {
    if (table_)
      modelData1 = charts_->getModelData(table_->modelP().data());
  }
#endif

  if (! modelData1)
    modelData1 = modelData_;

  //---

  const CQChartsModelDetails *details = (modelData1 ? modelData1->details() : nullptr);

  detailsWidget_->setDetails(details);
}
