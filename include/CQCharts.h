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

  bool isPlotType(const QString &name) const;

  CQChartsPlotType *plotType(const QString &name) const;

  void getPlotTypes(QStringList &names, QStringList &descs) const;

  CQChartsColumnTypeMgr *columnTypeMgr() const { return columnTypeMgr_; }

 private:
  CQChartsPlotTypeMgr*   plotTypeMgr_   { nullptr };
  CQChartsColumnTypeMgr* columnTypeMgr_ { nullptr };
};

#endif
