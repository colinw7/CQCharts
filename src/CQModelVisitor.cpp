#include <CQModelVisitor.h>
#include <CQModelUtil.h>

void
CQModelVisitor::
init(const QAbstractItemModel *model)
{
  model_            = model;
  numCols_          = model_->columnCount(QModelIndex());
  numRows_          = 0;
  hierarchical_     = false;
  hierSet_          = false;
  row_              = 0;
  numProcessedRows_ = 0;

  initVisit();
}

bool
CQModelVisitor::
isHierarchical() const
{
  if (! hierSet_) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (! hierSet_) {
      auto *th = const_cast<CQModelVisitor *>(this);

      th->hierarchical_ = CQModelUtil::isHierarchical(model_);
      th->hierSet_      = true;
    }
  }

  return hierarchical_;
}

void
CQModelVisitor::
term()
{
  numProcessedRows_ = row_;

  termVisit();
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
    auto state = execRow(model, parent, row, visitor);

    if (state == CQModelVisitor::State::TERMINATE) return state;
    if (state == CQModelVisitor::State::SKIP     ) continue;
  }

  return CQModelVisitor::State::OK;
}

CQModelVisitor::State
execRow(const QAbstractItemModel *model, const QModelIndex &parent,
        int row, CQModelVisitor &visitor)
{
  auto ind1 = model->index(row, 0, parent);

  if (visitor.maxRows() > 0 && row > visitor.maxRows())
    return CQModelVisitor::State::TERMINATE;

  CQModelVisitor::VisitData data(parent, row);

  data.vrow = visitor.row();

  if (model->hasChildren(ind1)) {
    // visit hier row
    auto state = visitor.hierVisit(model, data);
    if (state != CQModelVisitor::State::OK) { return state; }

    visitor.step();

    visitor.enter();

    // visit child rows
    auto iterState = execIndex(model, ind1, visitor);
    if (iterState != CQModelVisitor::State::OK) { visitor.leave(); return iterState; }

    // post visit hier row
    auto postState = visitor.hierPostVisit(model, data);
    if (postState != CQModelVisitor::State::OK) { visitor.leave(); return postState; }

    visitor.leave();
  }
  else {
    // pre visit leaf row
    auto preState = visitor.preVisit(model, data);
    if (preState != CQModelVisitor::State::OK) return preState;

    // visit leaf row
    auto state = visitor.visit(model, data);
    if (state != CQModelVisitor::State::OK) return state;

    visitor.step();

    // postVisit ?
  }

  return CQModelVisitor::State::OK;
}

}
