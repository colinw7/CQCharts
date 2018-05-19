#ifndef CQChartsExprDataModel_H
#define CQChartsExprDataModel_H

#include <CQChartsModelFilter.h>

class CQDataModel;
class CQExprModel;

class CQChartsExprDataModel : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsExprDataModel(CQCharts *charts, int nc=1, int nr=100);
 ~CQChartsExprDataModel();

  CQDataModel *dataModel() const { return dataModel_; }
  CQExprModel *exprModel() const { return exprModel_; }

 private:
  CQDataModel* dataModel_ { nullptr };
  CQExprModel* exprModel_ { nullptr };
};

#endif
