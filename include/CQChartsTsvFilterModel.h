#ifndef CQChartsTsvFilterModel_H
#define CQChartsTsvFilterModel_H

#include <CQChartsModelFilter.h>

class CQTsvModel;
class CQExprModel;

class CQChartsTsvFilterModel : public CQChartsModelFilter {
  Q_OBJECT

 public:
  CQChartsTsvFilterModel(CQCharts *charts);
 ~CQChartsTsvFilterModel();

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
