#ifndef CQChartsJsonFilterModel_H
#define CQChartsJsonFilterModel_H

#include <CQChartsModelFilter.h>

class CQChartsExprModel;
class CQJsonModel;

class CQChartsJsonFilterModel : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsJsonFilterModel(CQCharts *charts);
 ~CQChartsJsonFilterModel();

  CQJsonModel *jsonModel() const { return jsonModel_; }

  CQChartsExprModel  *exprModel() const override { return exprModel_; }
  QAbstractItemModel *baseModel() const override;

  bool load(const QString &filename);

  bool isHierarchical() const;

  //---

 private:
  CQJsonModel*       jsonModel_ { nullptr };
  CQChartsExprModel* exprModel_ { nullptr };
};

#endif
