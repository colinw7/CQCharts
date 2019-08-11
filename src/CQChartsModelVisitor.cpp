#include <CQChartsModelVisitor.h>
#include <CQChartsColumnType.h>
#include <CQChartsModelUtil.h>
#include <CQCharts.h>

//----

namespace CQChartsModelVisit {

bool exec(CQCharts *charts, const QAbstractItemModel *model, CQChartsModelVisitor &visitor) {
  if (! model)
    return false;

  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  columnTypeMgr->startCache(model);

  visitor.init(model);

  QModelIndex parent;

  (void) CQModelVisit::execIndex(model, parent, visitor);

  visitor.term();

  columnTypeMgr->endCache(model);

  return true;
}

}
