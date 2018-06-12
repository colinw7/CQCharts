#ifndef CQChartsDataFilterModel_H
#define CQChartsDataFilterModel_H

#include <CQChartsModelFilter.h>

class CQDataModel;

class CQChartsDataFilterModel : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsDataFilterModel(CQCharts *charts, int nc=1, int nr=100, bool fill=false);
 ~CQChartsDataFilterModel();

  CQDataModel *dataModel() const { return dataModel_; }

 private:
  CQDataModel* dataModel_ { nullptr };
};

#endif
