#ifndef CQChartsGnuDataFilterModel_H
#define CQChartsGnuDataFilterModel_H

#include <CQChartsModelFilter.h>

class CQGnuDataModel;
class CQExprModel;

class CQChartsGnuDataFilterModel : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsGnuDataFilterModel(CQCharts *charts);
 ~CQChartsGnuDataFilterModel();

  CQGnuDataModel *dataModel() const { return dataModel_; }
  CQExprModel    *exprModel() const { return exprModel_; }

  void setCommentHeader(bool b);
  void setFirstLineHeader(bool b);

  bool load(const QString &filename);

 private:
  CQGnuDataModel* dataModel_ { nullptr };
  CQExprModel*    exprModel_ { nullptr };
};

#endif
