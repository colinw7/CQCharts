#ifndef CQChartsTsv_H
#define CQChartsTsv_H

#include <CQChartsModelFilter.h>

class CQTsvModel;
class CQExprModel;

class CQChartsTsv : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsTsv(CQCharts *charts);
 ~CQChartsTsv();

  CQTsvModel  *tsvModel () const { return tsvModel_ ; }
  CQExprModel *exprModel() const { return exprModel_; }

  void setCommentHeader(bool b);
  void setFirstLineHeader(bool b);
  void setFirstColumnHeader(bool b);

  bool load(const QString &filename);

 private:
  CQTsvModel*  tsvModel_  { nullptr };
  CQExprModel* exprModel_ { nullptr };
};

#endif
