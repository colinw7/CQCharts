#ifndef CQChartsExprModel_H
#define CQChartsExprModel_H

#include <CQExprModel.h>

class CQCharts;

class CQChartsExprModel : public CQExprModel {
 public:
  CQChartsExprModel(CQCharts *charts, QAbstractItemModel *model);

  QVariant processCmd(const QString &name, const Values &values);

  //---

  QVariant remapCmd  (const Values &values);
  QVariant timevalCmd(const Values &values);

  //---

 private:
  bool getColumnRange(const QModelIndex &ind, double &rmin, double &rmax);

 private:
  CQCharts *charts_ { nullptr };
};

#endif
