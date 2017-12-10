#ifndef CQChartsExprData_H
#define CQChartsExprData_H

#include <CQChartsModelFilter.h>

class CQCharts;
class CQDataModel;
class CQExprModel;

class CQChartsExprData : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsExprData(CQCharts *charts, int nc=1, int nr=100);
 ~CQChartsExprData();

  CQDataModel *dataModel() const { return dataModel_; }
  CQExprModel *exprModel() const { return exprModel_; }

  //---

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

 private:
  CQCharts*    charts_    { nullptr };
  CQDataModel* dataModel_ { nullptr };
  CQExprModel* exprModel_ { nullptr };
};

#endif
