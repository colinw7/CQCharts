#ifndef CQChartsExprData_H
#define CQChartsExprData_H

#include <CQChartsModelFilter.h>

class CQDataModel;
class CQExprModel;

class CQChartsExprData : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsExprData(CQCharts *charts, int nc=1, int nr=100);
 ~CQChartsExprData();

  CQDataModel *dataModel() const { return dataModel_; }
  CQExprModel *exprModel() const { return exprModel_; }

 private:
  CQDataModel* dataModel_ { nullptr };
  CQExprModel* exprModel_ { nullptr };
};

#endif
