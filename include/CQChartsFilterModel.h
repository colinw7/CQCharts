#ifndef CQChartsFilterModel_H
#define CQChartsFilterModel_H

#include <CQChartsModelFilter.h>

class CQChartsExprModel;

/*!
 * \brief Filter model
 * \ingroup Charts
 */
class CQChartsFilterModel : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsFilterModel(CQCharts *charts, QAbstractItemModel *baseModel, bool exprModel=true);
 ~CQChartsFilterModel();

  QAbstractItemModel *baseModel() const override { return baseModel_; }
  CQChartsExprModel  *exprModel() const override { return exprModel_; }

 private:
  QAbstractItemModel* baseModel_ { nullptr };
  CQChartsExprModel*  exprModel_ { nullptr };
};

#endif
