#ifndef CQChartsCmds_H
#define CQChartsCmds_H

#include <CQChartsCmdBase.h>
#include <CQChartsCmdsSlot.h>

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

class CQChartsCmds;

class CQTcl;

class QAbstractItemModel;
class QItemSelectionModel;

//---

/*!
 * \brief Charts Tcl Commands
 * \ingroup Charts
 */
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

  //void setViewProperties(CQChartsView *view, const QString &properties);
  //void setPlotProperties(CQChartsPlot *plot, const QString &properties);

  bool setAnnotationProperties(CQChartsAnnotation *annotation, const QString &properties);

  //---

  bool loadFileModel(const QString &filename, CQChartsFileType type,
                     const CQChartsInputData &inputData);

  //---

  bool sortModel(ModelP &model, const QString &args);
  bool sortModel(ModelP &model, int column, Qt::SortOrder order);

  //---

  CQChartsPlot *createPlot(CQChartsView *view, const ModelP &model,
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
  bool loadChartsModelCmd   (CQChartsCmdArgs &args);
  bool processChartsModelCmd(CQChartsCmdArgs &args);
  bool defineChartsProcCmd  (CQChartsCmdArgs &args);
  bool sortChartsModelCmd   (CQChartsCmdArgs &args);
  bool foldChartsModelCmd   (CQChartsCmdArgs &args);
  bool filterChartsModelCmd (CQChartsCmdArgs &args);
  bool flattenChartsModelCmd(CQChartsCmdArgs &args);
  bool copyChartsModelCmd   (CQChartsCmdArgs &args);
  bool writeChartsModelCmd  (CQChartsCmdArgs &args);
  bool removeChartsModelCmd (CQChartsCmdArgs &args);

  bool createChartsCorrelationModelCmd(CQChartsCmdArgs &args);
  bool createChartsFoldedModelCmd     (CQChartsCmdArgs &args);
  bool createChartsBucketModelCmd     (CQChartsCmdArgs &args);
  bool createChartsSubsetModelCmd     (CQChartsCmdArgs &args);
  bool createChartsTransposeModelCmd  (CQChartsCmdArgs &args);
  bool createChartsSummaryModelCmd    (CQChartsCmdArgs &args);
  bool createChartsCollapseModelCmd   (CQChartsCmdArgs &args);
  bool createChartsPivotModelCmd      (CQChartsCmdArgs &args);
  bool createChartsStatsModelCmd      (CQChartsCmdArgs &args);

  bool exportChartsModelCmd(CQChartsCmdArgs &args);

  bool groupChartsPlotsCmd(CQChartsCmdArgs &args);
  bool placeChartsPlotsCmd(CQChartsCmdArgs &args);

  bool measureChartsTextCmd(CQChartsCmdArgs &args);
  bool encodeChartsTextCmd (CQChartsCmdArgs &args);

  bool createChartsViewCmd(CQChartsCmdArgs &args);
  bool removeChartsViewCmd(CQChartsCmdArgs &args);

  bool createChartsPlotCmd(CQChartsCmdArgs &args);
  bool removeChartsPlotCmd(CQChartsCmdArgs &args);

  bool getChartsPropertyCmd(CQChartsCmdArgs &args);
  bool setChartsPropertyCmd(CQChartsCmdArgs &args);

  bool getChartsDataCmd(CQChartsCmdArgs &args);
  bool setChartsDataCmd(CQChartsCmdArgs &args);

  bool createChartsPaletteCmd(CQChartsCmdArgs &args);
  bool getChartsPaletteCmd   (CQChartsCmdArgs &args);
  bool setChartsPaletteCmd   (CQChartsCmdArgs &args);

  bool createChartsArrowAnnotationCmd    (CQChartsCmdArgs &args);
  bool createChartsEllipseAnnotationCmd  (CQChartsCmdArgs &args);
  bool createChartsImageAnnotationCmd    (CQChartsCmdArgs &args);
  bool createChartsPointAnnotationCmd    (CQChartsCmdArgs &args);
  bool createChartsPolygonAnnotationCmd  (CQChartsCmdArgs &args);
  bool createChartsPolylineAnnotationCmd (CQChartsCmdArgs &args);
  bool createChartsRectangleAnnotationCmd(CQChartsCmdArgs &args);
  bool createChartsTextAnnotationCmd     (CQChartsCmdArgs &args);
  bool removeChartsAnnotationCmd         (CQChartsCmdArgs &args);

  bool connectChartsSignalCmd(CQChartsCmdArgs &args);

  bool printChartsImageCmd(CQChartsCmdArgs &args);
  bool writeChartsDataCmd (CQChartsCmdArgs &args);

  bool showChartsLoadModelDlgCmd   (CQChartsCmdArgs &args);
  bool showChartsManageModelsDlgCmd(CQChartsCmdArgs &args);
  bool showChartsCreatePlotDlgCmd  (CQChartsCmdArgs &args);
  bool showChartsHelpDlgCmd        (CQChartsCmdArgs &args);

  bool testEditCmd(CQChartsCmdArgs &args);

  //---

 private:
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

  QStringList stringToNamedColumns(const QString &str) const;

 private:
  CQCharts*        charts_  { nullptr };
  CQChartsCmdBase* cmdBase_ { nullptr };
};

//---

/*!
 * \brief Charts Tcl Command
 * \ingroup Charts
 */
#define CQCHARTS_DEF_CMD(NAME, PROC) \
class CQCharts##NAME##Cmd : public CQChartsCmdProc { \
 public: \
  CQCharts##NAME##Cmd(CQChartsCmds *cmds) : CQChartsCmdProc(cmds->cmdBase()), cmds_(cmds) { } \
 \
  bool exec(CQChartsCmdArgs &args) override { return cmds_->PROC(args); } \
\
 private: \
  CQChartsCmds* cmds_ { nullptr }; \
};

//---

CQCHARTS_DEF_CMD(LoadChartsModel   , loadChartsModelCmd)
CQCHARTS_DEF_CMD(ProcessChartsModel, processChartsModelCmd)
CQCHARTS_DEF_CMD(DefineChartsProc  , defineChartsProcCmd)
CQCHARTS_DEF_CMD(SortChartsModel   , sortChartsModelCmd)
CQCHARTS_DEF_CMD(FoldChartsModel   , foldChartsModelCmd)
CQCHARTS_DEF_CMD(FilterChartsModel , filterChartsModelCmd)
CQCHARTS_DEF_CMD(FlattenChartsModel, flattenChartsModelCmd)
CQCHARTS_DEF_CMD(CopyChartsModel   , copyChartsModelCmd)
CQCHARTS_DEF_CMD(WriteChartsModel  , writeChartsModelCmd)
CQCHARTS_DEF_CMD(RemoveChartsModel , removeChartsModelCmd)

//---

CQCHARTS_DEF_CMD(CreateChartsCorrelationModel, createChartsCorrelationModelCmd)
CQCHARTS_DEF_CMD(CreateChartsFoldedModel     , createChartsFoldedModelCmd)
CQCHARTS_DEF_CMD(CreateChartsBucketModel     , createChartsBucketModelCmd)
CQCHARTS_DEF_CMD(CreateChartsSubsetModel     , createChartsSubsetModelCmd)
CQCHARTS_DEF_CMD(CreateChartsTransposeModel  , createChartsTransposeModelCmd)
CQCHARTS_DEF_CMD(CreateChartsSummaryModel    , createChartsSummaryModelCmd)
CQCHARTS_DEF_CMD(CreateChartsCollapseModel   , createChartsCollapseModelCmd)
CQCHARTS_DEF_CMD(CreateChartsPivotModel      , createChartsPivotModelCmd)
CQCHARTS_DEF_CMD(CreateChartsStatsModel      , createChartsStatsModelCmd)

//---

CQCHARTS_DEF_CMD(ExportChartsModel, exportChartsModelCmd)

//---

CQCHARTS_DEF_CMD(GroupChartsPlots, groupChartsPlotsCmd)
CQCHARTS_DEF_CMD(PlaceChartsPlots, placeChartsPlotsCmd)

CQCHARTS_DEF_CMD(MeasureChartsText, measureChartsTextCmd)
CQCHARTS_DEF_CMD(EncodeChartsText , encodeChartsTextCmd )

CQCHARTS_DEF_CMD(CreateChartsView, createChartsViewCmd)
CQCHARTS_DEF_CMD(RemoveChartsView, removeChartsViewCmd)

CQCHARTS_DEF_CMD(CreateChartsPlot, createChartsPlotCmd)
CQCHARTS_DEF_CMD(RemoveChartsPlot, removeChartsPlotCmd)

CQCHARTS_DEF_CMD(GetChartsProperty, getChartsPropertyCmd)
CQCHARTS_DEF_CMD(SetChartsProperty, setChartsPropertyCmd)

CQCHARTS_DEF_CMD(GetChartsData, getChartsDataCmd)
CQCHARTS_DEF_CMD(SetChartsData, setChartsDataCmd)

CQCHARTS_DEF_CMD(CreateChartsPalette, createChartsPaletteCmd)
CQCHARTS_DEF_CMD(GetChartsPalette   , getChartsPaletteCmd   )
CQCHARTS_DEF_CMD(SetChartsPalette   , setChartsPaletteCmd   )

//---

CQCHARTS_DEF_CMD(CreateChartsArrowAnnotation    , createChartsArrowAnnotationCmd)
CQCHARTS_DEF_CMD(CreateChartsEllipseAnnotation  , createChartsEllipseAnnotationCmd)
CQCHARTS_DEF_CMD(CreateChartsImageAnnotation    , createChartsImageAnnotationCmd)
CQCHARTS_DEF_CMD(CreateChartsPointAnnotation    , createChartsPointAnnotationCmd)
CQCHARTS_DEF_CMD(CreateChartsPolygonAnnotation  , createChartsPolygonAnnotationCmd)
CQCHARTS_DEF_CMD(CreateChartsPolylineAnnotation , createChartsPolylineAnnotationCmd)
CQCHARTS_DEF_CMD(CreateChartsRectangleAnnotation, createChartsRectangleAnnotationCmd)
CQCHARTS_DEF_CMD(CreateChartsTextAnnotation     , createChartsTextAnnotationCmd)

CQCHARTS_DEF_CMD(RemoveChartsAnnotation, removeChartsAnnotationCmd)

//---

CQCHARTS_DEF_CMD(ConnectChartsSignal, connectChartsSignalCmd)

//---

CQCHARTS_DEF_CMD(PrintChartsImage, printChartsImageCmd)
CQCHARTS_DEF_CMD(WriteChartsData , writeChartsDataCmd)

//---

CQCHARTS_DEF_CMD(ShowChartsLoadModelDlg   , showChartsLoadModelDlgCmd)
CQCHARTS_DEF_CMD(ShowChartsManageModelsDlg, showChartsManageModelsDlgCmd)
CQCHARTS_DEF_CMD(ShowChartsCreatePlotDlg  , showChartsCreatePlotDlgCmd)
CQCHARTS_DEF_CMD(ShowChartsHelpDlg        , showChartsHelpDlgCmd)

//---

CQCHARTS_DEF_CMD(TestEdit, testEditCmd)

//---

#endif
