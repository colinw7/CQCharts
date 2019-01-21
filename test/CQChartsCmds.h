#ifndef CQChartsCmds_H
#define CQChartsCmds_H

#include <CQChartsCmdBase.h>

#include <CQCharts.h>
#include <CQChartsInitData.h>
#include <CQChartsGeom.h>
#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QPointer>

class CQChartsWindow;
class CQChartsView;
class CQChartsPlot;
class CQChartsGradientPalette;
class CQChartsColumn;
class CQChartsModelData;
class CQChartsPlotType;
class CQChartsAnnotation;

struct CQChartsPaletteColorData;

class CQChartsCmds;

#ifdef CQCharts_USE_TCL
class CQTcl;
#endif

class QAbstractItemModel;
class QItemSelectionModel;

//---

class CQChartsCmdsSlot : public QObject {
  Q_OBJECT

 public:
  CQChartsCmdsSlot(CQChartsCmds *cmds, CQChartsView *view, CQChartsPlot *plot,
                   const QString &procName);

 private:
  QString getTclCmd() const;
  QString getTclIdCmd(const QString &id) const;

 public slots:
  void objIdPressed       (const QString &);
  void annotationIdPressed(const QString &);
  void plotObjsAdded      ();

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
  using Vars   = std::vector<QVariant>;
  using ModelP = QSharedPointer<QAbstractItemModel>;
  using ViewP  = QPointer<CQChartsView>;
  using Plots  = std::vector<CQChartsPlot *>;

 private:
  using OptReal = boost::optional<double>;

 public:
  CQChartsCmds(CQCharts *charts);
 ~CQChartsCmds();

  CQCharts *charts() const { return charts_; }

  CQChartsCmdBase *cmdBase() { return cmdBase_; }

  void addCommands();

  void addCommand(const QString &name, CQChartsCmdProc *proc);

  static QString fixTypeName(const QString &typeName);

  void setViewProperties(CQChartsView *view, const QString &properties);
  void setPlotProperties(CQChartsPlot *plot, const QString &properties);

  //---

  bool loadFileModel(const QString &filename, CQChartsFileType type,
                     const CQChartsInputData &inputData);

  //---

  bool sortModel(ModelP &model, const QString &args);
  bool sortModel(ModelP &model, int column, Qt::SortOrder order);

  //---

  CQChartsPlot *createPlot(CQChartsView *view, const ModelP &model, QItemSelectionModel *sm,
                           CQChartsPlotType *type, bool reuse);

  bool initPlot(CQChartsPlot *plot, const CQChartsNameValueData &nameValueData,
                const CQChartsGeom::BBox &bbox);

  //---

  CQChartsModelData *getModelDataOrCurrent(int ind);

  CQChartsModelData *getModelData(int ind);

  //---

  CQChartsView *getView(bool reuse=true);

  CQChartsView *addView();

  //---

  static bool stringToColumn(QAbstractItemModel *model, const QString &str,
                             CQChartsColumn &column);

  //---

 public:
  bool loadModelCmd          (CQChartsCmdArgs &args);
  void processModelCmd       (CQChartsCmdArgs &args);
  void addProcessModelProcCmd(CQChartsCmdArgs &args);
  void sortModelCmd          (CQChartsCmdArgs &args);
  void foldModelCmd          (CQChartsCmdArgs &args);
  void filterModelCmd        (CQChartsCmdArgs &args);
  void flattenModelCmd       (CQChartsCmdArgs &args);
  void copyModelCmd          (CQChartsCmdArgs &args);

  void correlationModelCmd(CQChartsCmdArgs &args);
  void subsetModelCmd     (CQChartsCmdArgs &args);
  void transposeModelCmd  (CQChartsCmdArgs &args);

  void exportModelCmd(CQChartsCmdArgs &args);

  void groupPlotsCmd(CQChartsCmdArgs &args);
  void placePlotsCmd(CQChartsCmdArgs &args);

  void measureChartsTextCmd(CQChartsCmdArgs &args);

  void createViewCmd(CQChartsCmdArgs &args);

  void createPlotCmd(CQChartsCmdArgs &args);
  void removePlotCmd(CQChartsCmdArgs &args);

  void getChartsPropertyCmd(CQChartsCmdArgs &args);
  void setChartsPropertyCmd(CQChartsCmdArgs &args);

  void getChartsDataCmd(CQChartsCmdArgs &args);
  void setChartsDataCmd(CQChartsCmdArgs &args);

  void getPaletteCmd(CQChartsCmdArgs &args);
  void setPaletteCmd(CQChartsCmdArgs &args);

  void createTextAnnotationCmd    (CQChartsCmdArgs &args);
  void createArrowAnnotationCmd   (CQChartsCmdArgs &args);
  void createRectAnnotationCmd    (CQChartsCmdArgs &args);
  void createEllipseAnnotationCmd (CQChartsCmdArgs &args);
  void createPolygonAnnotationCmd (CQChartsCmdArgs &args);
  void createPolylineAnnotationCmd(CQChartsCmdArgs &args);
  void createPointAnnotationCmd   (CQChartsCmdArgs &args);
  void removeAnnotationCmd        (CQChartsCmdArgs &args);

  void connectChartsCmd(CQChartsCmdArgs &args);

  void printChartsCmd    (CQChartsCmdArgs &args);
  void writeChartsDataCmd(CQChartsCmdArgs &args);

  void loadModelDlgCmd  (CQChartsCmdArgs &args);
  void manageModelDlgCmd(CQChartsCmdArgs &args);
  void createPlotDlgCmd (CQChartsCmdArgs &args);

  //---

 private:
  void setPaletteData(CQChartsGradientPalette *palette,
                      const CQChartsPaletteColorData &paletteData);

  //---

  QAbstractItemModel *loadFile(const QString &filename, CQChartsFileType type,
                               const CQChartsInputData &inputData, bool &hierarchical);

  QAbstractItemModel *loadCsv (const QString &filename, const CQChartsInputData &inputData);
  QAbstractItemModel *loadTsv (const QString &filename, const CQChartsInputData &inputData);
  QAbstractItemModel *loadJson(const QString &filename, bool &hierarchical);
  QAbstractItemModel *loadData(const QString &filename, const CQChartsInputData &inputData);

  //---

  CQChartsView *getViewByName(const QString &viewName) const;

  bool getPlotsByName(CQChartsView *view, const Vars &plotNames, Plots &plot) const;

  CQChartsPlot *getOptPlotByName(CQChartsView *view, const QString &name) const;

  CQChartsPlot *getPlotByName(CQChartsView *view, const QString &name) const;

  CQChartsAnnotation *getAnnotationByName(const QString &name) const;

  //---

  QStringList stringToColumns(const QString &str) const;

 private:
  CQCharts*        charts_  { nullptr };
  CQChartsCmdBase* cmdBase_ { nullptr };
};

//---

#define CQCHARTS_DEF_CMD(NAME, PROC) \
class CQCharts##NAME##Cmd : public CQChartsCmdProc { \
 public: \
  CQCharts##NAME##Cmd(CQChartsCmds *cmds) : CQChartsCmdProc(cmds->cmdBase()), cmds_(cmds) { } \
 \
  void exec(CQChartsCmdArgs &args) override { cmds_->PROC(args); } \
\
 private: \
  CQChartsCmds* cmds_ { nullptr }; \
};

//---

CQCHARTS_DEF_CMD(LoadModel          , loadModelCmd)
CQCHARTS_DEF_CMD(ProcessModel       , processModelCmd)
CQCHARTS_DEF_CMD(AddProcessModelProc, addProcessModelProcCmd)
CQCHARTS_DEF_CMD(SortModel          , sortModelCmd)
CQCHARTS_DEF_CMD(FoldModel          , foldModelCmd)
CQCHARTS_DEF_CMD(FilterModel        , filterModelCmd)
CQCHARTS_DEF_CMD(FlattenModel       , flattenModelCmd)
CQCHARTS_DEF_CMD(CopyModel          , copyModelCmd)

//---

CQCHARTS_DEF_CMD(CorrelationModel, correlationModelCmd)
CQCHARTS_DEF_CMD(SubsetModel     , subsetModelCmd)
CQCHARTS_DEF_CMD(TransposeModel  , transposeModelCmd)

//---

CQCHARTS_DEF_CMD(ExportModel, exportModelCmd)

//---

CQCHARTS_DEF_CMD(GroupPlots, groupPlotsCmd)
CQCHARTS_DEF_CMD(PlacePlots, placePlotsCmd)

CQCHARTS_DEF_CMD(MeasureChartsText, measureChartsTextCmd)

CQCHARTS_DEF_CMD(CreateView, createViewCmd)

CQCHARTS_DEF_CMD(CreatePlot, createPlotCmd)
CQCHARTS_DEF_CMD(RemovePlot, removePlotCmd)

CQCHARTS_DEF_CMD(GetChartsProperty, getChartsPropertyCmd)
CQCHARTS_DEF_CMD(SetChartsProperty, setChartsPropertyCmd)

CQCHARTS_DEF_CMD(GetChartsData, getChartsDataCmd)
CQCHARTS_DEF_CMD(SetChartsData, setChartsDataCmd)

CQCHARTS_DEF_CMD(GetPalette, getPaletteCmd)
CQCHARTS_DEF_CMD(SetPalette, setPaletteCmd)

//---

CQCHARTS_DEF_CMD(CreateTextAnnotation    , createTextAnnotationCmd)
CQCHARTS_DEF_CMD(CreateArrowAnnotation   , createArrowAnnotationCmd)
CQCHARTS_DEF_CMD(CreateRectAnnotation    , createRectAnnotationCmd)
CQCHARTS_DEF_CMD(CreateEllipseAnnotation , createEllipseAnnotationCmd)
CQCHARTS_DEF_CMD(CreatePolygonAnnotation , createPolygonAnnotationCmd)
CQCHARTS_DEF_CMD(CreatePolylineAnnotation, createPolylineAnnotationCmd)
CQCHARTS_DEF_CMD(CreatePointAnnotation   , createPointAnnotationCmd)

CQCHARTS_DEF_CMD(RemoveAnnotation, removeAnnotationCmd)

//---

CQCHARTS_DEF_CMD(ConnectCharts, connectChartsCmd)

//---

CQCHARTS_DEF_CMD(PrintCharts    , printChartsCmd)
CQCHARTS_DEF_CMD(WriteChartsData, writeChartsDataCmd)

//---

CQCHARTS_DEF_CMD(LoadModelDlg  , loadModelDlgCmd)
CQCHARTS_DEF_CMD(ManageModelDlg, manageModelDlgCmd)
CQCHARTS_DEF_CMD(CreatePlotDlg , createPlotDlgCmd)

//---

#endif
