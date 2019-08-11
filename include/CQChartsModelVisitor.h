#ifndef CQChartsModelVisitor_H
#define CQChartsModelVisitor_H

#include <CQModelVisitor.h>

class CQCharts;

//---

/*!
 * \brief Visitor class for model data
 * \ingroup Charts
 */
class CQChartsModelVisitor : public CQModelVisitor {
 public:
  CQChartsModelVisitor() { }

  virtual ~CQChartsModelVisitor() { }
};

//---

namespace CQChartsModelVisit {

bool exec(CQCharts *charts, const QAbstractItemModel *model, CQChartsModelVisitor &visitor);

}

#endif
