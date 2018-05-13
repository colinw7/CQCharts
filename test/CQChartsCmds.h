#ifndef CQChartsCmds_H
#define CQChartsCmds_H

#include <CQChartsInitData.h>
#include <CQChartsGeom.h>
#include <CQExprModel.h>
#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QPointer>

#include <boost/optional.hpp>

class CQCharts;
class CQChartsWindow;
class CQChartsView;
class CQChartsPlot;
class CQChartsGradientPalette;
class CQChartsPaletteColorData;
class CQChartsColumn;
class CQChartsModelData;
class CQChartsPlotType;

#ifdef CQ_CHARTS_CEIL
class ClLanguageCommand;
class ClLanguageArgs;
#endif

class CQChartsCmds;

class CExpr;

class QAbstractItemModel;
class QItemSelectionModel;

//---

class CQChartsCmdsSlot : public QObject {
  Q_OBJECT

 public:
  CQChartsCmdsSlot(CQChartsCmds *cmds, CQChartsView *view, CQChartsPlot *plot,
                   const QString &procName);

 private:
  std::string getCmd(const QString &id) const;

 public slots:
  void objIdPressed       (const QString &);
  void annotationIdPressed(const QString &);

 private:
  CQChartsCmds* cmds_ { nullptr };
  CQChartsView* view_ { nullptr };
  CQChartsPlot* plot_ { nullptr };
  QString       procName_;
};

//---

class CQChartsCmds : public QObject {
  Q_OBJECT

 public:
  using Args    = std::vector<QString>;
  using OptReal = boost::optional<double>;
  using ModelP  = QSharedPointer<QAbstractItemModel>;
  using Vars    = std::vector<QString>;
  using ViewP   = QPointer<CQChartsView>;
  using Plots   = std::vector<CQChartsPlot *>;

 public:
  CQChartsCmds(CQCharts *charts);
 ~CQChartsCmds();

  CExpr* expr() const { return expr_; }

#ifdef CQ_CHARTS_CEIL
  void setCeil(bool b);
  bool isCeil() const { return ceil_; }
#endif

  bool processCmd(const QString &cmd, const Args &args);

  QString fixTypeName(const QString &typeName) const;

  void setViewProperties(CQChartsView *view, const QString &properties);
  void setPlotProperties(CQChartsPlot *plot, const QString &properties);

  //---

  void processAddExpression(ModelP &model, const QString &expr);

  void processExpression(ModelP &model, const QString &expr);
  void processExpression(ModelP &model, CQExprModel::Function function,
                         int column, const QString &expr);

  //---

  void setColumnFormats(const ModelP &model, const QString &columnType);

  bool loadFileModel(const QString &filename, CQChartsFileType type,
                     const CQChartsInputData &inputData);

  //---

  void foldModel(CQChartsModelData *modelData, const QString &str);

  void sortModel(ModelP &model, const QString &args);

  //---

  CQChartsPlot *createPlot(const ModelP &model, QItemSelectionModel *sm, CQChartsPlotType *type,
                           const CQChartsNameValueData &nameValueData, bool reuse,
                           const CQChartsGeom::BBox &bbox);

  //---

  int addModelData(ModelP &model, bool hierarchical);

  CQChartsModelData *getModelDataOrCurrent(int ind);

  CQChartsModelData *getModelData(int ind);

  CQChartsModelData *currentModelData();

  int currentInd() const { return currentInd_; }
  void setCurrentInd(int i) { currentInd_ = i; }

  //---

  CQChartsView *view() const;

  CQChartsView *getView(bool reuse=true);

  CQChartsView *addView();

  CQChartsView *currentView() const;

  //---

  bool isCompleteLine(QString &line) const;

  void parseLine(const QString &line);

 private:
#ifdef CQ_CHARTS_CEIL
  static void loadModelLCmd   (ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void processModelLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void sortModelLCmd   (ClLanguageCommand *, ClLanguageArgs *args, void *data);

  static void groupPlotsLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void placePlotsLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);

  static void setModelLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void getModelLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);

  static void setViewLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void getViewLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);

  static void createPlotLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void removePlotLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);

  static void setPropertyLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void getPropertyLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);

  static void setDataLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void getDataLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);

  static void setThemeLCmd  (ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void setPaletteLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);

  static void textShapeLCmd    (ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void arrowShapeLCmd   (ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void rectShapeLCmd    (ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void ellipseShapeLCmd (ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void polygonShapeLCmd (ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void polylineShapeLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);
  static void pointShapeLCmd   (ClLanguageCommand *, ClLanguageArgs *args, void *data);

  static void connectLCmd(ClLanguageCommand *, ClLanguageArgs *args, void *data);
#endif

  bool loadModelCmd   (const Args &args);
  void processModelCmd(const Args &args);
  void sortModelCmd   (const Args &args);

  void groupPlotsCmd(const Args &args);
  void placePlotsCmd(const Args &args);

  void setModelCmd(const Args &args);
  void getModelCmd(const Args &args);

  void setViewCmd(const Args &args);
  void getViewCmd(const Args &args);

  void createPlotCmd(const Args &args);
  void removePlotCmd(const Args &args);

  void setPropertyCmd(const Args &args);
  void getPropertyCmd(const Args &args);

  void setDataCmd(const Args &args);
  void getDataCmd(const Args &args);

  void setThemeCmd  (const Args &args);
  void setPaletteCmd(const Args &args);

  void textShapeCmd    (const Args &args);
  void arrowShapeCmd   (const Args &args);
  void rectShapeCmd    (const Args &args);
  void ellipseShapeCmd (const Args &args);
  void polygonShapeCmd (const Args &args);
  void polylineShapeCmd(const Args &args);
  void pointShapeCmd   (const Args &args);

  void connectCmd(const Args &args);

  void letCmd     (const Args &args);
  void ifCmd      (const Args &args);
  void whileCmd   (const Args &args);
  void continueCmd(const Args &args);
  void printCmd   (const Args &args);
  void sourceCmd  (const Args &args);

  //---

  void setPaleteData(CQChartsGradientPalette *palette,
                     const CQChartsPaletteColorData &paletteData);

  QStringList stringToCmds(const QString &str) const;

#ifdef CQ_CHARTS_CEIL
  Args parseCommandArgs(ClLanguageCommand *command, ClLanguageArgs *largs);
#endif

  //---

  QAbstractItemModel *loadFile(const QString &filename, CQChartsFileType type,
                               const CQChartsInputData &inputData, bool &hierarchical);

  QAbstractItemModel *loadCsv (const QString &filename, const CQChartsInputData &inputData);
  QAbstractItemModel *loadTsv (const QString &filename, const CQChartsInputData &inputData);
  QAbstractItemModel *loadJson(const QString &filename, bool &hierarchical);
  QAbstractItemModel *loadData(const QString &filename, const CQChartsInputData &inputData);

  QAbstractItemModel *createExprModel(int n=100);

  QAbstractItemModel *createVarsModel(const Vars &vars);

  //---

  CQExprModel *getExprModel(ModelP &model) const;

  void foldClear(CQChartsModelData *modelData);

  //---

  bool stringToColumn(const ModelP &model, const QString &str, CQChartsColumn &column) const;

  //---

  CQChartsView *getViewByName(const QString &viewName) const;

  bool getPlotsByName(CQChartsView *view, QStringList &plotNames, Plots &plot) const;

  CQChartsPlot *getOptPlotByName(CQChartsView *view, const QString &name) const;

  CQChartsPlot *getPlotByName(CQChartsView *view, const QString &name) const;

  void setCmdRc(int rc);
  void setCmdRc(double rc);
  void setCmdRc(const QString &rc);
  void setCmdRc(const QVariant &rc);

 signals:
  void titleChanged(int ind, const QString &title);

  void updateModelDetails(int ind);
  void updateModel(int ind);

  void windowCreated(CQChartsWindow *window);
  void viewCreated(CQChartsView *window);
  void plotCreated(CQChartsPlot *plot);

  void modelDataAdded(int ind);

 private:
  CQCharts* charts_       { nullptr };
  CExpr*    expr_         { nullptr };
#ifdef CQ_CHARTS_CEIL
  bool      ceil_         { false };
#endif
  ViewP     view_;
  int       currentInd_   { -1 };
  bool      continueFlag_ { false };
};

#endif
