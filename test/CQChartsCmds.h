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
class CQTcl;

class QAbstractItemModel;
class QItemSelectionModel;

//---

class CQChartsCmdsSlot : public QObject {
  Q_OBJECT

 public:
  CQChartsCmdsSlot(CQChartsCmds *cmds, CQChartsView *view, CQChartsPlot *plot,
                   const QString &procName);

 private:
  QString getCeilCmd(const QString &id) const;
  QString getTclCmd(const QString &id) const;

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
  enum class ParserType {
    SCRIPT,
    CEIL,
    TCL
  };

  using Args    = std::vector<QString>;
  using Vars    = std::vector<QVariant>;
  using OptReal = boost::optional<double>;
  using ModelP  = QSharedPointer<QAbstractItemModel>;
  using Strs    = std::vector<QString>;
  using ViewP   = QPointer<CQChartsView>;
  using Plots   = std::vector<CQChartsPlot *>;

 public:
  CQChartsCmds(CQCharts *charts);
 ~CQChartsCmds();

  CExpr* expr() const { return expr_; }

  const ParserType &parserType() const { return parserType_; }
  void setParserType(const ParserType &type);

  void addCeilCommand(const QString &name);
  void addTclCommand(const QString &name);
  void addCommand(const QString &name);
  void addCommands();

  bool processCmd(const QString &cmd, const Vars &vars);

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

  bool isCompleteLine(QString &line, bool &join) const;

  void parseLine(const QString &line);

  void parseScriptLine(const QString &line);

  CQTcl *qtcl() const { return qtcl_; }

 private:
  friend class CQChartsCeilCmd;

  bool loadModelCmd   (const Vars &vars);
  void processModelCmd(const Vars &vars);
  void sortModelCmd   (const Vars &vars);
  void exportModelCmd (const Vars &vars);

  void groupPlotsCmd(const Vars &vars);
  void placePlotsCmd(const Vars &vars);

  void setModelCmd(const Vars &vars);
  void getModelCmd(const Vars &vars);

  void measureTextCmd(const Vars &vars);

  void createPlotCmd(const Vars &vars);
  void removePlotCmd(const Vars &vars);

  void setPropertyCmd(const Vars &vars);
  void getPropertyCmd(const Vars &vars);

  void setDataCmd(const Vars &vars);
  void getDataCmd(const Vars &vars);

  void setThemeCmd  (const Vars &vars);
  void getThemeCmd  (const Vars &vars);
  void setPaletteCmd(const Vars &vars);
  void getPaletteCmd(const Vars &vars);

  void textShapeCmd    (const Vars &vars);
  void arrowShapeCmd   (const Vars &vars);
  void rectShapeCmd    (const Vars &vars);
  void ellipseShapeCmd (const Vars &vars);
  void polygonShapeCmd (const Vars &vars);
  void polylineShapeCmd(const Vars &vars);
  void pointShapeCmd   (const Vars &vars);

  void connectCmd(const Vars &vars);

  void letCmd     (const Vars &vars);
  void ifCmd      (const Vars &vars);
  void whileCmd   (const Vars &vars);
  void continueCmd(const Vars &vars);
  void printCmd   (const Vars &vars);
  void sourceCmd  (const Vars &vars);

  //---

  void setPaletteData(CQChartsGradientPalette *palette,
                      const CQChartsPaletteColorData &paletteData);

  QStringList stringToCmds(const QString &str) const;

#ifdef CQ_CHARTS_CEIL
  Vars parseCommandArgs(ClLanguageCommand *command, ClLanguageArgs *largs);
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

  bool getPlotsByName(CQChartsView *view, const Vars &plotNames, Plots &plot) const;

  CQChartsPlot *getOptPlotByName(CQChartsView *view, const QString &name) const;

  CQChartsPlot *getPlotByName(CQChartsView *view, const QString &name) const;

  void setCmdRc(int rc);
  void setCmdRc(double rc);
  void setCmdRc(const QString &rc);
  void setCmdRc(const QVariant &rc);

  void errorMsg(const QString &msg) const;

 signals:
  void titleChanged(int ind, const QString &title);

  void updateModelDetails(int ind);
  void updateModel(int ind);

  void windowCreated(CQChartsWindow *window);
  void viewCreated(CQChartsView *window);
  void plotCreated(CQChartsPlot *plot);

  void modelDataAdded(int ind);

 private:
  CQCharts*   charts_       { nullptr };
  CExpr*      expr_         { nullptr };
  ParserType  parserType_   { ParserType::SCRIPT };
  ViewP       view_;
  int         currentInd_   { -1 };
  bool        continueFlag_ { false };
  CQTcl*      qtcl_         { nullptr };
};

#endif
