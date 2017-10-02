#ifndef CQCharts_H
#define CQCharts_H

#include <QAbstractItemModel>

class CQChartsPlotTypeMgr;
class CQChartsPlotType;
class CQChartsColumnTypeMgr;

class CQCharts {
 public:
  enum Role {
    ColumnType = Qt::UserRole + 101
  };

 public:
  CQCharts();

 ~CQCharts();

  void init();

  CQChartsPlotType *plotType(const QString &name);

  CQChartsColumnTypeMgr *columnTypeMgr() const { return columnTypeMgr_; }

 private:
  CQChartsPlotTypeMgr*   plotTypeMgr_   { nullptr };
  CQChartsColumnTypeMgr* columnTypeMgr_ { nullptr };
};

#endif
