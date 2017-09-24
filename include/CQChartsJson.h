#ifndef CQChartsJson_H
#define CQChartsJson_H

#include <CQJsonModel.h>

class CQCharts;

class CQChartsJson : public CQJsonModel {
  Q_OBJECT

 public:
  CQChartsJson(CQCharts *charts);
};

#endif
