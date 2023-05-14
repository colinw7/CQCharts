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
class CQChartsKeyItem;
class CQChartsFile;

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
  using Vars    = std::vector<QVariant>;
  using ModelP  = QSharedPointer<QAbstractItemModel>;
  using ViewP   = QPointer<CQChartsView>;
  using Plots   = std::vector<CQChartsPlot *>;
  using Columns = std::vector<CQChartsColumn>;

 private:
  using OptReal = std::optional<double>;

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

  bool loadFileModel(const CQChartsFile &file, CQChartsFileType type,
                     const CQChartsInputData &inputData);

  //---

  static bool sortModel(ModelP &model, const QString &args);
  static bool sortModel(ModelP &model, int column, Qt::SortOrder order);

  //---

  CQChartsPlot *createPlot(CQChartsView *view, const ModelP &model,
                           CQChartsPlotType *type, bool reuse);

  bool initPlot(CQChartsPlot *plot, const CQChartsNameValueData &nameValueData,
                const CQChartsGeom::BBox &bbox);

  //---

  CQChartsModelData *getModelDataOrCurrent(const QString &id);

  CQChartsModelData *getModelData(const QString &id);

  //---

  CQChartsView *getView(bool reuse=true);

  CQChartsView *addView();

  //---

  QStringList modelArgValues() const;
  QStringList viewArgValues() const;
  QStringList plotArgValues(CQChartsView *view) const;
  QStringList plotTypeArgValues() const;
  QStringList annotationArgValues(CQChartsView *view, CQChartsPlot *plot) const;
  QStringList roleArgValues(QAbstractItemModel *model) const;

  //---

  bool stringToModelColumns(const ModelP &model, const QString &columnsStr, Columns &columns);

 public:
  QAbstractItemModel *loadFile(const CQChartsFile &file, CQChartsFileType type,
                               const CQChartsInputData &inputData, bool &hierarchical);

  QAbstractItemModel *loadCsv (const CQChartsFile &file, const CQChartsInputData &inputData);
  QAbstractItemModel *loadTsv (const CQChartsFile &file, const CQChartsInputData &inputData);
  QAbstractItemModel *loadJson(const CQChartsFile &file, bool &hierarchical);
  QAbstractItemModel *loadData(const CQChartsFile &file, const CQChartsInputData &inputData);

  //---

  void setNameValue(const QString &name, const QString &value);

  //---

  bool getViewPlotArg(CQChartsCmdArgs &argv, CQChartsView* &view, CQChartsPlot* &plot);

  bool getViewArg(CQChartsCmdArgs &argv, CQChartsView* &view);
  bool getPlotArg(CQChartsCmdArgs &argv, CQChartsPlot* &plot);

  CQChartsView *getViewByName(const QString &viewName) const;

  bool getPlotsByName(CQChartsView *view, const Vars &plotNames, Plots &plot,
                      Plots &extraPlots, bool checkView=true) const;

  CQChartsPlot *getOptPlotByName(CQChartsView *view, const QString &name) const;

  CQChartsPlot *getPlotByName(CQChartsView *view, const QString &name) const;

  CQChartsAnnotation *getAnnotationByName(const QString &name) const;

  CQChartsKeyItem *getKeyItemById(const QString &id) const;

  //---

  bool setAnnotationArgProperties(CQChartsCmdArgs &argv, CQChartsAnnotation *annotation);

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
#define CQCHARTS_DEF_CMD(NAME) \
class CQCharts##NAME##Cmd : public CQChartsCmdProc { \
 public: \
  using CmdArg   = CQTclCmd::CmdArg; \
  using CmdGroup = CQTclCmd::CmdGroup; \
\
  enum class ArgType { \
    None      = int(CmdArg::Type::None), \
    Boolean   = int(CmdArg::Type::Boolean), \
    Integer   = int(CmdArg::Type::Integer), \
    Real      = int(CmdArg::Type::Real), \
    String    = int(CmdArg::Type::String), \
    SBool     = int(CmdArg::Type::SBool), \
    Enum      = int(CmdArg::Type::Enum), \
    Color     = int(CmdArg::Type::Extra) + 1, \
    Font      = int(CmdArg::Type::Extra) + 2, \
    LineDash  = int(CmdArg::Type::Extra) + 3, \
    Length    = int(CmdArg::Type::Extra) + 4, \
    Position  = int(CmdArg::Type::Extra) + 5, \
    Rect      = int(CmdArg::Type::Extra) + 6, \
    Polygon   = int(CmdArg::Type::Extra) + 7, \
    Align     = int(CmdArg::Type::Extra) + 8, \
    Sides     = int(CmdArg::Type::Extra) + 9, \
    Column    = int(CmdArg::Type::Extra) + 10, \
    Row       = int(CmdArg::Type::Extra) + 11, \
    Reals     = int(CmdArg::Type::Extra) + 12, \
    ObjRefPos = int(CmdArg::Type::Extra) + 13, \
  }; \
\
 public: \
  CQCharts##NAME##Cmd(CQChartsCmds *cmds) : \
   CQChartsCmdProc(cmds->cmdBase()), cmds_(cmds) { } \
\
  CQCharts *charts() const { return cmds_->charts(); } \
\
  bool execCmd(CQChartsCmdArgs &args) override; \
\
  void addCmdArgs(CQChartsCmdArgs &args) override; \
\
  QStringList getArgValues(const QString &arg, \
                           const NameValueMap &nameValueMap=NameValueMap()) override; \
\
  CmdArg &addArg(CQChartsCmdArgs &args, const QString &name, ArgType type, \
                 const QString &argDesc="", const QString &desc="") { \
    return args.addCmdArg(name, int(type), argDesc, desc); \
  } \
\
  CQChartsCmds *cmds() const { return cmds_; } \
\
 private: \
  CQChartsCmds* cmds_ { nullptr }; \
};

//---

// model
CQCHARTS_DEF_CMD(LoadChartsModel)
CQCHARTS_DEF_CMD(ProcessChartsModel)

CQCHARTS_DEF_CMD(SortChartsModel)
CQCHARTS_DEF_CMD(FoldChartsModel)
CQCHARTS_DEF_CMD(ConnectionChartsModel)
CQCHARTS_DEF_CMD(FilterChartsModel)
CQCHARTS_DEF_CMD(FlattenChartsModel)
CQCHARTS_DEF_CMD(CopyChartsModel)
CQCHARTS_DEF_CMD(JoinChartsModel)
CQCHARTS_DEF_CMD(GroupChartsModel)
CQCHARTS_DEF_CMD(ExportChartsModel)
CQCHARTS_DEF_CMD(WriteChartsModel)

CQCHARTS_DEF_CMD(RemoveChartsModel)

// model proc
CQCHARTS_DEF_CMD(DefineChartsProc)

//--

// derived model
CQCHARTS_DEF_CMD(CreateChartsCorrelationModel)
CQCHARTS_DEF_CMD(CreateChartsFoldedModel)
CQCHARTS_DEF_CMD(CreateChartsBucketModel)
CQCHARTS_DEF_CMD(CreateChartsSubsetModel)
CQCHARTS_DEF_CMD(CreateChartsTransposeModel)
CQCHARTS_DEF_CMD(CreateChartsSummaryModel)
CQCHARTS_DEF_CMD(CreateChartsCollapseModel)
CQCHARTS_DEF_CMD(CreateChartsPivotModel)
CQCHARTS_DEF_CMD(CreateChartsStatsModel)
CQCHARTS_DEF_CMD(CreateChartsDataModel)
CQCHARTS_DEF_CMD(CreateChartsFractalModel)

//---

// view
CQCHARTS_DEF_CMD(CreateChartsView)
CQCHARTS_DEF_CMD(RemoveChartsView)

//---

// plot
CQCHARTS_DEF_CMD(CreateChartsPlot)
CQCHARTS_DEF_CMD(RemoveChartsPlot)

CQCHARTS_DEF_CMD(GroupChartsPlots)
CQCHARTS_DEF_CMD(PlaceChartsPlots)

//---

// properties and data
CQCHARTS_DEF_CMD(GetChartsProperty)
CQCHARTS_DEF_CMD(SetChartsProperty)

CQCHARTS_DEF_CMD(GetChartsData)
CQCHARTS_DEF_CMD(SetChartsData)

CQCHARTS_DEF_CMD(PrintChartsVariant)

//---

CQCHARTS_DEF_CMD(ExecuteChartsSlot)

//---

// annotation
CQCHARTS_DEF_CMD(CreateChartsAnnotationGroup)
CQCHARTS_DEF_CMD(CreateChartsAnnotation)
CQCHARTS_DEF_CMD(CreateChartsArcAnnotation)
CQCHARTS_DEF_CMD(CreateChartsArcConnectorAnnotation)
CQCHARTS_DEF_CMD(CreateChartsArrowAnnotation)
CQCHARTS_DEF_CMD(CreateChartsAxisAnnotation)
CQCHARTS_DEF_CMD(CreateChartsButtonAnnotation)
CQCHARTS_DEF_CMD(CreateChartsEllipseAnnotation)
CQCHARTS_DEF_CMD(CreateChartsImageAnnotation)
CQCHARTS_DEF_CMD(CreateChartsPathAnnotation)
CQCHARTS_DEF_CMD(CreateChartsKeyAnnotation)
CQCHARTS_DEF_CMD(CreateChartsPieSliceAnnotation)
CQCHARTS_DEF_CMD(CreateChartsPointAnnotation)
CQCHARTS_DEF_CMD(CreateChartsPointSetAnnotation)
CQCHARTS_DEF_CMD(CreateChartsPoint3DSetAnnotation)
CQCHARTS_DEF_CMD(CreateChartsPolygonAnnotation)
CQCHARTS_DEF_CMD(CreateChartsPolylineAnnotation)
CQCHARTS_DEF_CMD(CreateChartsRectangleAnnotation)
CQCHARTS_DEF_CMD(CreateChartsShapeAnnotation)
CQCHARTS_DEF_CMD(CreateChartsTextAnnotation)
CQCHARTS_DEF_CMD(CreateChartsValueSetAnnotation)
CQCHARTS_DEF_CMD(CreateChartsWidgetAnnotation)
CQCHARTS_DEF_CMD(CreateChartsSymbolMapKeyAnnotation)
CQCHARTS_DEF_CMD(RemoveChartsAnnotation)

//---

// key
CQCHARTS_DEF_CMD(AddChartsKeyItem)

//---

// palette
CQCHARTS_DEF_CMD(CreateChartsPalette)
CQCHARTS_DEF_CMD(GetChartsPalette)
CQCHARTS_DEF_CMD(SetChartsPalette)

//---

// symbol
CQCHARTS_DEF_CMD(CreateChartsSymbolSet)
CQCHARTS_DEF_CMD(AddChartsSymbol)

//---

// connections
CQCHARTS_DEF_CMD(ConnectChartsSignal)

//---

// custom widget
CQCHARTS_DEF_CMD(AddCustomWidget)

//---

// export
CQCHARTS_DEF_CMD(PrintChartsImage)
CQCHARTS_DEF_CMD(WriteChartsData)
CQCHARTS_DEF_CMD(WriteChartsStats)

//---

// misc
CQCHARTS_DEF_CMD(MeasureChartsText)
CQCHARTS_DEF_CMD(EncodeChartsText)
CQCHARTS_DEF_CMD(BucketChartsValues)

//---

// dialogs
CQCHARTS_DEF_CMD(ShowChartsLoadModelDlg)
CQCHARTS_DEF_CMD(ShowChartsManageModelsDlg)
CQCHARTS_DEF_CMD(ShowChartsCreatePlotDlg)
CQCHARTS_DEF_CMD(ShowChartsTextDlg)
CQCHARTS_DEF_CMD(ShowChartsHelpDlg)

//---

// test
CQCHARTS_DEF_CMD(TestEdit)
CQCHARTS_DEF_CMD(Test)

//---

// data frame
#ifdef CQCHARTS_DATA_FRAME
CQCHARTS_DEF_CMD(DataFrame)
#endif

//---

#endif
