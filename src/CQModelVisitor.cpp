#include <CQModelVisitor.h>
#include <CQModelUtil.h>

void
CQModelVisitor::
init(const QAbstractItemModel *model)
{
  model_        = model;
  numCols_      = model_->columnCount(QModelIndex());
  hierarchical_ = false;
  hierSet_      = false;
  row_          = 0;
  numRows_      = 0;
}

bool
CQModelVisitor::
isHierarchical() const
{
  if (! hierSet_) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (! hierSet_) {
      CQModelVisitor *th = const_cast<CQModelVisitor *>(this);

      th->hierarchical_ = CQModelUtil::isHierarchical(model_);
      th->hierSet_      = true;
    }
  }

  return hierarchical_;
}

//----

namespace CQModelVisit {

bool exec(const QAbstractItemModel *model, CQModelVisitor &visitor)
{
  if (! model)
    return false;

  visitor.init(model);

  QModelIndex parent;

  (void) execIndex(model, parent, visitor);

  visitor.term();

  return true;
}

bool exec(const QAbstractItemModel *model, const QModelIndex &parent,
          int row, CQModelVisitor &visitor)
{
  if (! model)
    return false;

  visitor.init(model);

  (void) execRow(model, parent, row, visitor);

  visitor.term();

  return true;
}

CQModelVisitor::State
execIndex(const QAbstractItemModel *model, const QModelIndex &parent, CQModelVisitor &visitor)
{
  int nr = model->rowCount(parent);

  visitor.setNumRows(nr);

  for (int row = 0; row < nr; ++row) {
    CQModelVisitor::State state = execRow(model, parent, row, visitor);

    if (state == CQModelVisitor::State::TERMINATE) return state;
    if (state == CQModelVisitor::State::SKIP     ) continue;
  }

  return CQModelVisitor::State::OK;
}

CQModelVisitor::State
execRow(const QAbstractItemModel *model, const QModelIndex &parent,
        int row, CQModelVisitor &visitor)
{
  QModelIndex ind1 = model->index(row, 0, parent);

  if (visitor.maxRows() > 0 && row > visitor.maxRows())
    return CQModelVisitor::State::TERMINATE;

  CQModelVisitor::VisitData data(parent, row);

  data.vrow = visitor.row();

  if (model->hasChildren(ind1)) {
    CQModelVisitor::State state = visitor.hierVisit(model, data);
    if (state != CQModelVisitor::State::OK) return state;

    CQModelVisitor::State iterState = execIndex(model, ind1, visitor);
    if (iterState != CQModelVisitor::State::OK) return iterState;

    CQModelVisitor::State postState = visitor.hierPostVisit(model, data);
    if (postState != CQModelVisitor::State::OK) return postState;
  }
  else {
    CQModelVisitor::State preState = visitor.preVisit(model, data);
    if (preState != CQModelVisitor::State::OK) return preState;

    CQModelVisitor::State state = visitor.visit(model, data);
    if (state != CQModelVisitor::State::OK) return state;

    visitor.step();

    // postVisit ?
  }

  return CQModelVisitor::State::OK;
}

}
