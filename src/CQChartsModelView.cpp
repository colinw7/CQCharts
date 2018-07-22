#include <CQChartsModelView.h>
#include <CQChartsTable.h>
#include <CQChartsTree.h>

#include <QStackedWidget>
#include <QVBoxLayout>

CQChartsModelView::
CQChartsModelView(CQCharts *charts, QWidget *parent) :
 QFrame(parent), charts_(charts)
{
  setObjectName("modelView");

  QVBoxLayout *layout = new QVBoxLayout(this);

  stack_ = new QStackedWidget;

  stack_->setObjectName("stack");

  table_ = new CQChartsTable(charts_, this);

  connect(table_, SIGNAL(filterChanged()), this, SIGNAL(filterChanged()));

  tree_ = new CQChartsTree(charts_, this);

  stack_->addWidget(table_);
  stack_->addWidget(tree_ );

  layout->addWidget(stack_);
}

CQChartsModelView::
~CQChartsModelView()
{
  delete table_;
  delete tree_;
}

void
CQChartsModelView::
setFilter(const QString &text)
{
  table_->setFilter(text);
}

void
CQChartsModelView::
addFilter(const QString &text)
{
  table_->addFilter(text);
}

void
CQChartsModelView::
setSearch(const QString &text)
{
  table_->setSearch(text);
}

void
CQChartsModelView::
addSearch(const QString &text)
{
  table_->addSearch(text);
}

void
CQChartsModelView::
setModel(ModelP model, bool hierarchical)
{
  hierarchical_ = hierarchical;

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
}

QItemSelectionModel *
CQChartsModelView::
selectionModel()
{
  if (! hierarchical_)
    return table_->selectionModel();
  else
    return tree_->selectionModel();
}
