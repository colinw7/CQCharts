#ifndef CQChartsCsv_H
#define CQChartsCsv_H

#include <CQChartsModelFilter.h>

class CQCharts;
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

  bool load(const QString &filename);

  //---

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

 private:
  CQCharts*    charts_    { nullptr };
  CQCsvModel*  csvModel_  { nullptr };
  CQExprModel* exprModel_ { nullptr };
};

#endif
