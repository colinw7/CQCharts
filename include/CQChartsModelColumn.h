#ifndef CQChartsModelColumn_H
#define CQChartsModelColumn_H

#include <CQChartsColumn.h>

class CQCharts;

class CQChartsModelColumn {
 public:
  CQChartsModelColumn(CQCharts *charts);
 ~CQChartsModelColumn();

  QVariant columnUserData(int column, const QVariant &var) const;
  QVariant columnDisplayData(int column, const QVariant &var) const;

  QString columnType(int col) const;
  bool setColumnType(int col, const QString &type);

 private:
  typedef std::vector<CQChartsColumn> Columns;

  CQCharts* charts_ { nullptr };
  Columns   columns_;
};

#endif
