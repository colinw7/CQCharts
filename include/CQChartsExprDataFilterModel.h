#ifndef CQChartsExprDataFilterModel_H
#define CQChartsExprDataFilterModel_H

#include <CQChartsModelFilter.h>

class CQChartsExprModel;
class CQDataModel;

class CQChartsExprDataFilterModel : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsExprDataFilterModel(CQCharts *charts, int nc=1, int nr=100);
 ~CQChartsExprDataFilterModel();

  CQDataModel *dataModel() const { return dataModel_; }

  CQChartsExprModel  *exprModel() const override { return exprModel_; }
  QAbstractItemModel *baseModel() const override;

 private:
  CQDataModel*       dataModel_ { nullptr };
  CQChartsExprModel* exprModel_ { nullptr };
};

#endif
