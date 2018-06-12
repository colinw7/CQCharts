#ifndef CQChartsJsonFilterModel_H
#define CQChartsJsonFilterModel_H

#include <CQChartsModelFilter.h>

class CQCharts;
class CQJsonModel;

class CQChartsJsonFilterModel : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsJsonFilterModel(CQCharts *charts);
 ~CQChartsJsonFilterModel();

  CQJsonModel *jsonModel() const { return jsonModel_; }

  bool load(const QString &filename);

  bool isHierarchical() const;

  //---

 private:
  CQJsonModel* jsonModel_ { nullptr };
};

#endif
