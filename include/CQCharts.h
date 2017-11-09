#ifndef CQCharts_H
#define CQCharts_H

#include <CQBaseModel.h>
#include <QAbstractItemModel>

class CQChartsPlotTypeMgr;
class CQChartsPlotType;
class CQChartsColumnTypeMgr;
class CQChartsView;

class CQCharts {
 public:
  CQCharts();

  virtual ~CQCharts();

  void init();

  bool isPlotType(const QString &name) const;

  CQChartsPlotType *plotType(const QString &name) const;

  void getPlotTypeNames(QStringList &names, QStringList &descs) const;

  CQChartsColumnTypeMgr *columnTypeMgr() const { return columnTypeMgr_; }

  CQChartsView *addView(const QString &id="");

  virtual CQChartsView *createView();

  CQChartsView *getView(const QString &id) const;

  void getViewIds(QStringList &names) const;

  void errorMsg(const QString &msg);

 private:
  using Views = std::map<QString,CQChartsView*>;

  CQChartsPlotTypeMgr*   plotTypeMgr_   { nullptr };
  CQChartsColumnTypeMgr* columnTypeMgr_ { nullptr };
  Views                  views_;
};

#endif
