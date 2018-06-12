#ifndef CQChartsExprDataFilterModel_H
#define CQChartsExprDataFilterModel_H

#include <CQChartsModelFilter.h>

class CQDataModel;
class CQExprModel;

class CQChartsExprDataFilterModel : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsExprDataFilterModel(CQCharts *charts, int nc=1, int nr=100);
 ~CQChartsExprDataFilterModel();

  CQDataModel *dataModel() const { return dataModel_; }
  CQExprModel *exprModel() const { return exprModel_; }

 private:
  CQDataModel* dataModel_ { nullptr };
  CQExprModel* exprModel_ { nullptr };
};

#endif
