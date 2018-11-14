#include <CQChartsModelVisitor.h>
#include <CQChartsColumnType.h>
#include <CQCharts.h>
#include <CQChartsUtil.h>

void
CQChartsModelVisitor::
init(QAbstractItemModel *model)
{
  model_        = model;
  numCols_      = model_->columnCount(QModelIndex());
  hierarchical_ = false;
  hierSet_      = false;
  row_          = 0;
  numRows_      = 0;
}

bool
CQChartsModelVisitor::
isHierarchical() const
{
  if (! hierSet_) {
    CQChartsModelVisitor *th = const_cast<CQChartsModelVisitor *>(this);

    th->hierarchical_ = CQChartsUtil::isHierarchical(model_);
    th->hierSet_      = true;
  }

  return hierarchical_;
}

//----

namespace CQChartsModelVisit {

bool exec(CQCharts *charts, QAbstractItemModel *model, CQChartsModelVisitor &visitor) {
  if (! model)
    return false;

  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  columnTypeMgr->initCache();

  visitor.init(model);

  QModelIndex parent;

  (void) execIndex(model, parent, visitor);

  visitor.term();

  columnTypeMgr->termCache();

  return true;
}

bool exec(QAbstractItemModel *model, const QModelIndex &parent, int row,
          CQChartsModelVisitor &visitor) {
  if (! model)
    return false;

  visitor.init(model);

  (void) execRow(model, parent, row, visitor);

  visitor.term();

  return true;
}

CQChartsModelVisitor::State
execIndex(QAbstractItemModel *model, const QModelIndex &parent, CQChartsModelVisitor &visitor) {
  int nr = model->rowCount(parent);

  visitor.setNumRows(nr);

  for (int row = 0; row < nr; ++row) {
    CQChartsModelVisitor::State state = execRow(model, parent, row, visitor);

    if (state == CQChartsModelVisitor::State::TERMINATE) return state;
    if (state == CQChartsModelVisitor::State::SKIP     ) continue;
  }

  return CQChartsModelVisitor::State::OK;
}

CQChartsModelVisitor::State
execRow(QAbstractItemModel *model, const QModelIndex &parent, int row,
        CQChartsModelVisitor &visitor) {
  QModelIndex ind1 = model->index(row, 0, parent);

  if (visitor.maxRows() > 0 && row > visitor.maxRows())
    return CQChartsModelVisitor::State::TERMINATE;

  CQChartsModelVisitor::VisitData data(parent, row);

  data.vrow = visitor.row();

  if (model->hasChildren(ind1)) {
    CQChartsModelVisitor::State state = visitor.hierVisit(model, data);
    if (state != CQChartsModelVisitor::State::OK) return state;

    CQChartsModelVisitor::State iterState = execIndex(model, ind1, visitor);
    if (iterState != CQChartsModelVisitor::State::OK) return iterState;

    CQChartsModelVisitor::State postState = visitor.hierPostVisit(model, data);
    if (postState != CQChartsModelVisitor::State::OK) return postState;
  }
  else {
    CQChartsModelVisitor::State preState = visitor.preVisit(model, data);
    if (preState != CQChartsModelVisitor::State::OK) return preState;

    CQChartsModelVisitor::State state = visitor.visit(model, data);
    if (state != CQChartsModelVisitor::State::OK) return state;

    visitor.step();

    // postVisit ?
  }

  return CQChartsModelVisitor::State::OK;
}

}
