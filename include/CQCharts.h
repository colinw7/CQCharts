#ifndef CQCharts_H
#define CQCharts_H

#include <CQBaseModelTypes.h>
#include <CQChartsTheme.h>
#include <CQChartsColor.h>

#include <QObject>
#include <QAbstractItemModel>
#include <QSharedPointer>
#include <vector>

class CQChartsWindow;
class CQChartsView;
class CQChartsPlotTypeMgr;
class CQChartsPlotType;
class CQChartsPlot;
class CQChartsModelData;
class CQChartsColumnTypeMgr;

class CQCharts : public QObject {
  Q_OBJECT

 public:
  struct ProcData {
    QString name;
    QString args;
    QString body;

    ProcData(const QString &name="", const QString &args="", const QString &body="") :
     name(name), args(args), body(body) {
    }
  };

  using PlotTypes  = std::vector<CQChartsPlotType *>;
  using ModelP     = QSharedPointer<QAbstractItemModel>;
  using ModelDatas = std::vector<CQChartsModelData *>;
  using Views      = std::vector<CQChartsView *>;
  using Procs      = std::map<QString,ProcData>;

 public:
  CQCharts();

  virtual ~CQCharts();

  void init();

  //---

  bool hasViewKey() const { return viewKey_; }
  void setViewKey(bool b) { viewKey_ = b; }

  //---

  bool isPlotType(const QString &name) const;

  CQChartsPlotType *plotType(const QString &name) const;

  void getPlotTypeNames(QStringList &names, QStringList &descs) const;

  void getPlotTypes(PlotTypes &types) const;

  //---

  CQChartsColumnTypeMgr *columnTypeMgr() const { return columnTypeMgr_; }

  //---

  QColor interpColor(const CQChartsColor &c, int i, int n) const;
  QColor interpColor(const CQChartsColor &c, double value) const;

  QColor interpColorValue(const CQChartsColor &c, int i, int n, double value) const;

  //---

  const CQChartsInterfaceTheme &interfaceTheme() const { return interfaceTheme_; }
  CQChartsInterfaceTheme &interfaceTheme() { return interfaceTheme_; }

  const CQChartsTheme &plotTheme() const { return plotTheme_; }
  CQChartsTheme &plotTheme() { return plotTheme_; }

  void setPlotTheme(const CQChartsTheme &theme);

  //---

  QColor interpPaletteColor(double r, bool scale=false) const;
  QColor interpIndPaletteColor(int ind, double r, bool scale=false) const;

  QColor interpPaletteColorValue(int i, int n, double r, bool scale=false) const;
  QColor interpIndPaletteColorValue(int ind, int i, int n, double r, bool scale=false) const;

  QColor interpThemeColor(double r) const;

  CQChartsGradientPalette *themeGroupPalette(int i, int n) const;

  CQChartsGradientPalette *themePalette(int ind) const;

  const CQChartsThemeObj *themeObj() const;
  CQChartsThemeObj *themeObj();

  //---

  CQChartsModelData *initModelData(ModelP &model);

  CQChartsModelData *getModelData(const QAbstractItemModel *model) const;
  CQChartsModelData *getModelData(int ind) const;

  int currentModelInd() const { return currentModelInd_; }
  void setCurrentModelInd(int ind);

  CQChartsModelData *currentModelData() const;
  void setCurrentModelData(CQChartsModelData *modelData);

  void getModelDatas(ModelDatas &modelDatas) const;

  void setModelName(CQChartsModelData *modelData, const QString &name);

  //---

  CQChartsView *addView(const QString &id="");

  virtual CQChartsView *createView();

  CQChartsView *getView(const QString &id) const;

  CQChartsView *currentView() const;

  void getViews(Views &views) const;

  void getViewIds(QStringList &names) const;

  void removeView(CQChartsView *view);

  //---

  CQChartsWindow *createWindow(CQChartsView *view);

  //---

  void addProc(const QString &name, const QString &args, const QString &body);

  const Procs &procs() const { return procs_; }

  //---

  void emitModelTypeChanged(int modelId);

  //---

  // model index management. Model stores unique index in "modelInd" property
  // and charts class keeps last free model index.

  // get current model index for model (fails if not set yet)
  bool getModelInd(const QAbstractItemModel *model, int &ind) const;

  // get next free model index and set it into the model
  // (only fails if can't set property in model)
  bool assignModelInd(QAbstractItemModel *model, int &ind);

  //---

  void errorMsg(const QString &msg) const;

 signals:
  void modelDataAdded(int);

  void currentModelChanged(int);

  void modelNameChanged(const QString &);

  void modelTypeChanged(int);

  void windowCreated(CQChartsWindow *window);

  void viewAdded(CQChartsView *view);

  void plotAdded(CQChartsPlot *plot);

  void themeChanged();

 private:
  // add new model data for model
  int addModelData(ModelP &model);

  // assign model index to model
  bool setModelInd(QAbstractItemModel *model, int ind);

 private:
  using NameViews = std::map<QString,CQChartsView*>;

  bool                   viewKey_         { true };    // has view key
  CQChartsPlotTypeMgr*   plotTypeMgr_     { nullptr }; // plot type manager
  CQChartsColumnTypeMgr* columnTypeMgr_   { nullptr }; // column type manager
  CQChartsInterfaceTheme interfaceTheme_;              // interface theme
  CQChartsTheme          plotTheme_;                   // plot theme
  int                    currentModelInd_ { -1 };      // current model index
  ModelDatas             modelDatas_;                  // model datas
  int                    lastModelInd_    { 0 };       // last model ind
  NameViews              views_;                       // views
  Procs                  procs_;                       // tcl procs
};

#endif
