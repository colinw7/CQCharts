#ifndef CQChartsGnuDataFilterModel_H
#define CQChartsGnuDataFilterModel_H

#include <CQChartsModelFilter.h>

class CQChartsExprModel;
class CQGnuDataModel;

class CQChartsGnuDataFilterModel : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsGnuDataFilterModel(CQCharts *charts);
 ~CQChartsGnuDataFilterModel();

  CQGnuDataModel *dataModel() const { return dataModel_; }

  CQChartsExprModel  *exprModel() const override { return exprModel_; }
  QAbstractItemModel *baseModel() const override;

  void setCommentHeader(bool b);
  void setFirstLineHeader(bool b);

  bool load(const QString &filename);

 private:
  CQGnuDataModel*    dataModel_ { nullptr };
  CQChartsExprModel* exprModel_ { nullptr };
};

#endif
