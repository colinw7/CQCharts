#ifndef CQChartsJson_H
#define CQChartsJson_H

#include <CQChartsModelFilter.h>

class CQCharts;
class CQJsonModel;

class CQChartsJson : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsJson(CQCharts *charts);
 ~CQChartsJson();

  CQJsonModel *jsonModel() const { return jsonModel_; }

  bool load(const QString &filename);

  bool isHierarchical() const;

  //---

 private:
  CQJsonModel* jsonModel_ { nullptr };
};

#endif
