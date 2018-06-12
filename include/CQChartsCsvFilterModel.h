#ifndef CQChartsCsvFilterModel_H
#define CQChartsCsvFilterModel_H

#include <CQChartsModelFilter.h>

class CQCsvModel;
class CQExprModel;

class CQChartsCsvFilterModel : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsCsvFilterModel(CQCharts *charts);
 ~CQChartsCsvFilterModel();

  CQCsvModel  *csvModel () const { return csvModel_ ; }
  CQExprModel *exprModel() const { return exprModel_; }

  void setCommentHeader(bool b);
  void setFirstLineHeader(bool b);
  void setFirstColumnHeader(bool b);
  void setSeparator(char c);

  bool load(const QString &filename);

 private:
  CQCsvModel*  csvModel_  { nullptr };
  CQExprModel* exprModel_ { nullptr };
};

#endif
