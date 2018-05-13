#ifndef CQCharts_H
#define CQCharts_H

#include <CQChartsModelData.h>
#include <CQBaseModel.h>

#include <QObject>
#include <QAbstractItemModel>

class CQChartsPlotTypeMgr;
class CQChartsPlotType;
class CQChartsColumnTypeMgr;
class CQChartsView;

class CQCharts : public QObject {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQCharts();

  virtual ~CQCharts();

  void init();

  bool isPlotType(const QString &name) const;

  CQChartsPlotType *plotType(const QString &name) const;

  void getPlotTypeNames(QStringList &names, QStringList &descs) const;

  CQChartsColumnTypeMgr *columnTypeMgr() const { return columnTypeMgr_; }

  //---

  int addModel(ModelP &model, bool hierarchical=false);

  CQChartsModelData *getModelData(QAbstractItemModel *model) const;
  CQChartsModelData *getModelData(int ind) const;

  //---

  CQChartsView *addView(const QString &id="");

  virtual CQChartsView *createView();

  CQChartsView *getView(const QString &id) const;

  void getViewIds(QStringList &names) const;

  //---

  void errorMsg(const QString &msg);

 private:
  using Views      = std::map<QString,CQChartsView*>;
  using ModelDatas = std::vector<CQChartsModelData*>;

  CQChartsPlotTypeMgr*   plotTypeMgr_   { nullptr };
  CQChartsColumnTypeMgr* columnTypeMgr_ { nullptr };
  ModelDatas             modelDatas_;
  Views                  views_;
};

#endif
