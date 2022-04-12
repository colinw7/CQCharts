#include <CQChartsModelViewHolder.h>
#include <CQChartsModelData.h>
#include <CQCharts.h>

#ifdef CQCHARTS_MODEL_VIEW
#include <CQChartsModelView.h>
#else
#include <CQChartsTable.h>
#include <CQChartsTree.h>
#endif

#include <CQUtil.h>

#include <QStackedWidget>
#include <QVBoxLayout>

CQChartsModelViewHolder::
CQChartsModelViewHolder(CQCharts *charts, QWidget *parent) :
 QFrame(parent)
{
  setObjectName("modelViewHolder");

  setCharts(charts);
}

CQChartsModelViewHolder::
~CQChartsModelViewHolder()
{
#ifdef CQCHARTS_MODEL_VIEW
  delete view_;
#else
  delete table_;
  delete tree_;
#endif
}

void
CQChartsModelViewHolder::
setCharts(CQCharts *charts)
{
  charts_ = charts;

#ifdef CQCHARTS_MODEL_VIEW
  if (! view_)
    init();
#else
  if (! table_)
    init();
#endif

#ifdef CQCHARTS_MODEL_VIEW
  view_ ->setCharts(this->charts());
#else
  table_->setCharts(this->charts());
  tree_ ->setCharts(this->charts());
#endif
}

void
CQChartsModelViewHolder::
init()
{
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

#ifdef CQCHARTS_MODEL_VIEW
  assert(! view_);

  view_ = new CQChartsModelView(charts(), this);

  layout->addWidget(view_);

  connect(view_, SIGNAL(columnClicked(int)), this, SLOT(columnClickedSlot(int)));
#else
  assert(! stack_);

  // table/tree stack
  stack_ = CQUtil::makeWidget<QStackedWidget>("stack");

  layout->addWidget(stack_);

  //---

  // table (flat data)
  table_ = new CQChartsTable(charts(), this);

  connect(table_, SIGNAL(filterChanged()), this, SIGNAL(filterChanged()));
  connect(table_, SIGNAL(columnClicked(int)), this, SLOT(columnClickedSlot(int)));

  stack_->addWidget(table_);

  // tree (hier data)
  tree_ = new CQChartsTree(charts(), this);

  connect(tree_, SIGNAL(columnClicked(int)), this, SLOT(columnClickedSlot(int)));

  stack_->addWidget(tree_ );
#endif
}

void
CQChartsModelViewHolder::
columnClickedSlot(int column)
{
  auto *modelData = (charts_ ? charts_->getModelData(model()) : nullptr);

  if (modelData)
    modelData->setCurrentColumn(column);

  emit columnClicked(column);
}

void
CQChartsModelViewHolder::
setFilterAnd(bool b)
{
#ifdef CQCHARTS_MODEL_VIEW
  view_->setFilterAnd(b);
#else
  table_->setFilterAnd(b);
#endif
}

void
CQChartsModelViewHolder::
setFilter(const QString &text)
{
#ifdef CQCHARTS_MODEL_VIEW
  view_->setFilter(text);
#else
  table_->setFilter(text);
#endif
}

void
CQChartsModelViewHolder::
addFilter(const QString &text)
{
#ifdef CQCHARTS_MODEL_VIEW
  view_->addFilter(text);
#else
  table_->addFilter(text);
#endif
}

QString
CQChartsModelViewHolder::
filterDetails() const
{
#ifdef CQCHARTS_MODEL_VIEW
  return view_->filterDetails();
#else
  return table_->filterDetails();
#endif
}

void
CQChartsModelViewHolder::
setSearch(const QString &text)
{
#ifdef CQCHARTS_MODEL_VIEW
  view_->setSearch(text);
#else
  table_->setSearch(text);
#endif
}

void
CQChartsModelViewHolder::
addSearch(const QString &text)
{
#ifdef CQCHARTS_MODEL_VIEW
  view_->addSearch(text);
#else
  table_->addSearch(text);
#endif
}

CQChartsModelViewHolder::ModelP
CQChartsModelViewHolder::
model() const
{
#ifdef CQCHARTS_MODEL_VIEW
  return view_->modelP();
#else
  if (! hierarchical_)
    return table_->modelP();
  else
    return tree_->modelP();
#endif
}

int
CQChartsModelViewHolder::
modelInd() const
{
  auto *modelData = (charts_ ? charts_->getModelData(model()) : nullptr);
  if (! modelData) return -1;

  return modelData->ind();
}

void
CQChartsModelViewHolder::
setModelInd(int ind)
{
  auto *modelData = (charts_ ? charts_->getModelDataByInd(ind) : nullptr);
  if (! modelData) return;

  setModelData(modelData);
}

void
CQChartsModelViewHolder::
setModelData(ModelData *modelData)
{
  if (modelData)
    setModel(modelData->currentModel(), modelData->isHierarchical());
  else
    setModel(ModelP(), false);
}

void
CQChartsModelViewHolder::
setModel(ModelP model, bool hierarchical)
{
  hierarchical_ = hierarchical;

#ifdef CQCHARTS_MODEL_VIEW
  view_->setModelP(model);
#else
  if (! hierarchical_) {
    table_->setModelP(model);
    tree_ ->setModelP(CQChartsTree::ModelP());

    stack_->setCurrentIndex(0);
  }
  else {
    tree_ ->setModelP(model);
    table_->setModelP(CQChartsTable::ModelP());

    stack_->setCurrentIndex(1);
  }
#endif
}

QItemSelectionModel *
CQChartsModelViewHolder::
selectionModel()
{
#ifdef CQCHARTS_MODEL_VIEW
  return view_->selectionModel();
#else
  if (! hierarchical_)
    return table_->selectionModel();
  else
    return tree_->selectionModel();
#endif
}

void
CQChartsModelViewHolder::
showColumn(int column)
{
#ifdef CQCHARTS_MODEL_VIEW
  return view_->showColumn(column);
#else
  if (! hierarchical_)
    return table_->showColumn(column);
  else
    return tree_->showColumn(column);
#endif
}

void
CQChartsModelViewHolder::
hideColumn(int column)
{
#ifdef CQCHARTS_MODEL_VIEW
  return view_->hideColumn(column);
#else
  if (! hierarchical_)
    return table_->hideColumn(column);
  else
    return tree_->hideColumn(column);
#endif
}
