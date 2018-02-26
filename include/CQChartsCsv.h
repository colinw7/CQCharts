#ifndef CQChartsCsv_H
#define CQChartsCsv_H

#include <CQChartsModelFilter.h>

class CQCsvModel;
class CQExprModel;

class CQChartsCsv : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsCsv(CQCharts *charts);
 ~CQChartsCsv();

  CQCsvModel  *csvModel () const { return csvModel_ ; }
  CQExprModel *exprModel() const { return exprModel_; }

  void setCommentHeader(bool b);
  void setFirstLineHeader(bool b);
  void setFirstColumnHeader(bool b);

  bool load(const QString &filename);

 private:
  CQCsvModel*  csvModel_  { nullptr };
  CQExprModel* exprModel_ { nullptr };
};

#endif
