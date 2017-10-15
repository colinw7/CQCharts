#ifndef CQCharts_H
#define CQCharts_H

#include <QAbstractItemModel>

class CQChartsPlotTypeMgr;
class CQChartsPlotType;
class CQChartsColumnTypeMgr;
class CQChartsView;

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

  void getPlotTypeNames(QStringList &names, QStringList &descs) const;

  CQChartsColumnTypeMgr *columnTypeMgr() const { return columnTypeMgr_; }

  CQChartsView *addView(const QString &id="");

  CQChartsView *getView(const QString &id) const;

  void getViewIds(QStringList &names) const;

  void errorMsg(const QString &msg);

 private:
  typedef std::map<QString,CQChartsView *> Views;

  CQChartsPlotTypeMgr*   plotTypeMgr_   { nullptr };
  CQChartsColumnTypeMgr* columnTypeMgr_ { nullptr };
  Views                  views_;
};

#endif
