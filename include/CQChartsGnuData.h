#ifndef CQChartsGnuData_H
#define CQChartsGnuData_H

#include <CQChartsModelFilter.h>

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

 private:
  CQGnuDataModel* dataModel_ { nullptr };
  CQExprModel*    exprModel_ { nullptr };
};

#endif
