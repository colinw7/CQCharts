#ifndef CQChartsExprModel_H
#define CQChartsExprModel_H

#include <CQChartsModelFilter.h>

class CQDataModel;
class CQExprModel;

class CQChartsExprModel : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsExprModel(CQCharts *charts, int nc=1, int nr=100);
 ~CQChartsExprModel();

  CQDataModel *dataModel() const { return dataModel_; }
  CQExprModel *exprModel() const { return exprModel_; }

 private:
  CQDataModel* dataModel_ { nullptr };
  CQExprModel* exprModel_ { nullptr };
};

#endif
