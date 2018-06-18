#ifndef CQChartsTsvFilterModel_H
#define CQChartsTsvFilterModel_H

#include <CQChartsModelFilter.h>

class CQChartsExprModel;
class CQTsvModel;

class CQChartsTsvFilterModel : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsTsvFilterModel(CQCharts *charts);
 ~CQChartsTsvFilterModel();

  CQTsvModel *tsvModel() const { return tsvModel_; }

  CQChartsExprModel  *exprModel() const override { return exprModel_; }
  QAbstractItemModel *baseModel() const override;

  void setCommentHeader(bool b);
  void setFirstLineHeader(bool b);
  void setFirstColumnHeader(bool b);

  bool load(const QString &filename);

 private:
  CQTsvModel*        tsvModel_  { nullptr };
  CQChartsExprModel* exprModel_ { nullptr };
};

#endif
