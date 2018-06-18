#ifndef CQChartsCsvFilterModel_H
#define CQChartsCsvFilterModel_H

#include <CQChartsModelFilter.h>

class CQChartsExprModel;
class CQCsvModel;

class CQChartsCsvFilterModel : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsCsvFilterModel(CQCharts *charts);
 ~CQChartsCsvFilterModel();

  CQCsvModel *csvModel() const { return csvModel_; }

  CQChartsExprModel  *exprModel() const override { return exprModel_; }
  QAbstractItemModel *baseModel() const override;

  void setCommentHeader(bool b);
  void setFirstLineHeader(bool b);
  void setFirstColumnHeader(bool b);
  void setSeparator(char c);

  bool load(const QString &filename);

 private:
  CQCsvModel*        csvModel_  { nullptr };
  CQChartsExprModel* exprModel_ { nullptr };
};

#endif
