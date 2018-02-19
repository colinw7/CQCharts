#ifndef CQChartsDataModel_H
#define CQChartsDataModel_H

#include <CQChartsModelFilter.h>

class CQDataModel;

class CQChartsDataModel : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsDataModel(CQCharts *charts, int nc=1, int nr=100, bool fill=false);
 ~CQChartsDataModel();

  CQDataModel *dataModel() const { return dataModel_; }

 private:
  CQDataModel* dataModel_ { nullptr };
};

#endif
