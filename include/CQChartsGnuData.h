#ifndef CQChartsGnuData_H
#define CQChartsGnuData_H

#include <CQChartsModelFilter.h>

class CQCharts;
class CQGnuDataModel;
class CQExprModel;

class CQChartsGnuData : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsGnuData(CQCharts *charts);
 ~CQChartsGnuData();

  CQGnuDataModel *dataModel() const { return dataModel_; }
  CQExprModel    *exprModel() const { return exprModel_; }

  void setCommentHeader(bool b);
  void setFirstLineHeader(bool b);

  bool load(const QString &filename);

  //---

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

 private:
  CQCharts*       charts_    { nullptr };
  CQGnuDataModel* dataModel_ { nullptr };
  CQExprModel*    exprModel_ { nullptr };
};

#endif
