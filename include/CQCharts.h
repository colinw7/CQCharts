#ifndef CQCharts_H
#define CQCharts_H

#include <CQChartsThemeName.h>
#include <CQChartsUtil.h>
#include <CQChartsModelTypes.h>

#include <QObject>
#include <QAbstractItemModel>
#include <QSharedPointer>

#include <vector>
#include <set>
#include <memory>

class CQChartsWindow;
class CQChartsView;
class CQChartsPlotTypeMgr;
class CQChartsPlotType;
class CQChartsPlot;
class CQChartsModelData;
class CQChartsColumnTypeMgr;
class CQChartsSymbolSetMgr;
class CQChartsSymbolSet;
class CQChartsInterfaceTheme;
class CQChartsExprTcl;
class CQColorsPalette;
class CQChartsColor;

class CQChartsLoadModelDlg;
class CQChartsManageModelsDlg;
class CQChartsEditModelDlg;
class CQChartsCreatePlotDlg;

class CQChartsPropertyMeta;

class CQPropertyViewModel;
class CQPropertyViewItem;

class CQTcl;
#ifdef CQCHARTS_TK_WIDGET
class CTkApp;
#endif

class QTimer;

/*!
 * \mainpage Charts Package
 *
 * \section Introduction
 *
 *  The charts package allows the display of multiple views of \ref CQChartsView
 *  in a window \ref CQChartsWindow.
 *
 *  The views contains one or more plots of \ref CQChartsPlot and any number of
 *  view annotations of \ref CQChartsAnnotation and can have a title of \ref CQChartsTitle
 *  and key of \ref CQChartsViewKey.
 *
 *  Each plot has a type of \ref CQChartsPlotType which can be configured using
 *  parameters of \ref CQChartsPlotParameter and properties using standard QVariant values.
 *  The plot also can have any number of plot annotations of \ref CQChartsAnnotation.
 *
 *  Plots can optionally have axes of \ref CQChartsAxis, a title of \ref CQChartsTitle
 *  and key of \ref CQChartsPlotKey.
 *
 *  Plot Types are:
 *   + \ref CQChartsAdjacencyPlot
 *   + \ref CQChartsBarChartPlot
 *   + \ref CQChartsBoxPlot
 *   + \ref CQChartsBubblePlot
 *   + \ref CQChartsChordPlot
 *   + \ref CQChartsContourPlot
 *   + \ref CQChartsDelaunayPlot
 *   + \ref CQChartsDendrogramPlot
 *   + \ref CQChartsDistributionPlot
 *   + \ref CQChartsForceDirectedPlot
 *   + \ref CQChartsGeometryPlot
 *   + \ref CQChartsGraphPlot
 *   + \ref CQChartsGraphVizPlot
 *   + \ref CQChartsGridPlot
 *   + \ref CQChartsHierBubblePlot
 *   + \ref CQChartsHierScatterPlot
 *   + \ref CQChartsImagePlot
 *   + \ref CQChartsParallelPlot
 *   + \ref CQChartsPiePlot
 *   + \ref CQChartsPivotPlot
 *   + \ref CQChartsRadarPlot
 *   + \ref CQChartsSankeyPlot
 *   + \ref CQChartsScatterPlot
 *   + \ref CQChartsStripPlot
 *   + \ref CQChartsSunburstPlot
 *   + \ref CQChartsTablePlot
 *   + \ref CQChartsTreeMapPlot
 *   + \ref CQChartsWheelPlot
 *   + \ref CQChartsWordCloudPlot
 *   + \ref CQChartsXYPlot
 */

/*!
 * \brief Charts base class
 * \ingroup Charts
 */
class CQCharts : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool   viewKey       READ hasViewKey    WRITE setViewKey      )
  Q_PROPERTY(double maxSymbolSize READ maxSymbolSize WRITE setMaxSymbolSize)
  Q_PROPERTY(double maxFontSize   READ maxFontSize   WRITE setMaxFontSize  )
  Q_PROPERTY(double maxLineWidth  READ maxLineWidth  WRITE setMaxLineWidth )

 public:
  enum class ProcType {
    TCL,
    SCRIPT,
    SVG
  };

  //! charts tcl proc data
  struct ProcData {
    QString name;
    QString args;
    QString body;

    ProcData(const QString &name="", const QString &args="", const QString &body="") :
     name(name), args(args), body(body) {
    }
  };

  using Window     = CQChartsWindow;
  using PlotTypes  = std::vector<CQChartsPlotType *>;
  using ModelP     = QSharedPointer<QAbstractItemModel>;
  using ModelDatas = std::vector<CQChartsModelData *>;
  using View       = CQChartsView;
  using Views      = std::vector<View *>;
  using Plot       = CQChartsPlot;
  using Procs      = std::map<QString, ProcData>;
  using TypeProcs  = std::map<ProcType, Procs>;

  using Color    = CQChartsColor;
  using ColorInd = CQChartsUtil::ColorInd;

 public:
  static QString description();

 public:
  CQCharts();

  virtual ~CQCharts();

  void init();

  //---

  void addExitTimer(double secs);

  //---

  //! get/set has view key
  bool hasViewKey() const { return viewKey_; }
  void setViewKey(bool b) { viewKey_ = b; }

  //! get/set max symbol size
  double maxSymbolSize() const { return maxSymbolSize_; }
  void setMaxSymbolSize(double r) { maxSymbolSize_ = r; }

  //! get/set max font size
  double maxFontSize() const { return maxFontSize_; }
  void setMaxFontSize(double r) { maxFontSize_ = r; }

  //! get/set max line width
  double maxLineWidth() const { return maxLineWidth_; }
  void setMaxLineWidth(double r) { maxLineWidth_ = r; }

  //---

  void getModelTypeNames(QStringList &names) const;

  void getPropertyTypeNames(QStringList &names) const;

  //---

  bool isPlotType(const QString &name) const;

  CQChartsPlotType *plotType(const QString &name) const;

  void getPlotTypeNames(QStringList &names, QStringList &descs) const;

  void getPlotTypes(PlotTypes &types) const;

  //---

  CQChartsPlotTypeMgr *plotTypeMgr() const {
    assert(plotTypeMgr_); return plotTypeMgr_.get(); }

  //---

  CQChartsColumnTypeMgr *columnTypeMgr() const {
    assert(columnTypeMgr_); return columnTypeMgr_.get(); }

  //---

  CQChartsSymbolSetMgr *symbolSetMgr() const {
    assert(symbolSetMgr_); return symbolSetMgr_.get(); }

  //---

 public:
  QColor interpColor(const CQChartsColor &c, const ColorInd &ind) const;
  QColor interpColor(const CQChartsColor &c, int ig, int ng, const ColorInd &ind) const;

 private:
  QColor interpColorValueI(const CQChartsColor &c, int ig, int ng,
                           double value, const QColor &ic) const;

  //---

 public:
  const CQChartsInterfaceTheme *interfaceTheme() const { return interfaceTheme_.get(); }
  CQChartsInterfaceTheme *interfaceTheme() { return interfaceTheme_.get(); }

  const CQChartsThemeName &plotTheme() const { return plotTheme_; }
  CQChartsThemeName &plotTheme() { return plotTheme_; }

  void setPlotTheme(const CQChartsThemeName &themeName);

  bool isDark() const;
  void setDark(bool b);

  //---

 public:
  QColor interpPaletteColor(const ColorInd &ind, bool scale=false, bool invert=false) const;

  QColor interpIndPaletteColor(int ind, int i, int n, bool scale=false, bool invert=false) const;
  QColor interpIndPaletteColor(int ind, double r, bool scale=false, bool invert=false) const;

  QColor interpGroupPaletteColor(const ColorInd &ig, const ColorInd &iv,
                                 bool scale=false, bool invert=false) const;

  QColor interpPaletteColorValue(int ig, int ng, int i, int n,
                                 bool scale=false, bool invert=false) const;
  QColor interpPaletteColorValue(int ig, int ng, double r,
                                 bool scale=false, bool invert=false) const;

  QColor indexPaletteColor(int i, int n) const;
  QColor indexIndPaletteColor(int ind, int i, int n) const;

  //---

  QColor interpThemeColor(const ColorInd &ind) const;

  QColor interpInterfaceColor(double r) const;

 public:
  CQColorsPalette *themeGroupPalette(int i, int n) const;

  CQColorsPalette *themePalette(int ind) const;

  const CQColorsTheme *theme() const;
  CQColorsTheme *theme();

  const QColor &contrastColor() const { return contrastColor_; }
  void setContrastColor(const QColor &c) { contrastColor_ = c; }
  void resetContrastColor() { contrastColor_ = QColor(); }

  //---

  QColor interpModelColor(const CQChartsColor &c, double value) const;

  //---

  CQChartsColor adjustDefaultPalette(const CQChartsColor &c, const QString &defaultPalette) const;

 private:
  QColor interpIndPaletteColorValue(int ind, int ig, int ng, double r,
                                    bool scale, bool invert) const;

  QColor interpNamePaletteColorValue(const QString &name, int ig, int ng, double r,
                                     bool scale, bool invert) const;

  QColor interpNamePaletteColor(const QString &name, double r, bool scale, bool invert) const;

  QColor indexNamePaletteColor(const QString &name, int ig, int ng) const;

  //---

 public:
  CQChartsModelData *initModelData(ModelP &model);

  CQChartsModelData *getModelData(const ModelP &model) const;
  CQChartsModelData *getModelData(const QAbstractItemModel *model) const;

  CQChartsModelData *getModelDataById(const QString &id) const;
  CQChartsModelData *getModelDataByInd(int ind) const;

  int currentModelInd() const { return currentModelInd_; }
  void setCurrentModelInd(int ind);

  CQChartsModelData *currentModelData() const;
  void setCurrentModelData(CQChartsModelData *modelData);

  void getModelDatas(ModelDatas &modelDatas) const;

  void setModelName(CQChartsModelData *modelData, const QString &name);

  void setModelFileName(CQChartsModelData *modelData, const QString &filename);

  //---

  // remove model data for model
  bool removeModel(ModelP &model);

  // remove model data
  bool removeModelData(CQChartsModelData *modelData);

  //---

  View *addView(const QString &id="");

  void addView(View *view);

  virtual View *createView();
  virtual void deleteView(View *view);

  View *getView(const QString &id) const;

  View *currentView() const;

  void getViews(Views &views) const;

  void getViewIds(QStringList &names) const;

  void removeView(View *view);

  //---

  virtual Window *createWindow(View *view);
  virtual void deleteWindow(Window *window);

  //---

  const TypeProcs &typeProcs() const { return typeProcs_; }

  void addProc(ProcType type, const QString &name, const QString &args, const QString &body);

  void removeProc(ProcType type, const QString &name);

  void getProcNames(ProcType type, QStringList &names) const;

  bool getProcData(ProcType type, const QString &name, QString &args, QString &body) const;

  const Procs &procs(ProcType type) const;

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

  // property model
  CQPropertyViewModel *propertyModel() const { return propertyModel_.get(); }

  //---

  static void setItemIsStyle(CQPropertyViewItem *item);
  static bool getItemIsStyle(const CQPropertyViewItem *item);

  static void setItemIsHidden(CQPropertyViewItem *item);
  static bool getItemIsHidden(const CQPropertyViewItem *item);

  //---

  bool hasSymbolSet(const QString &name) const;

  CQChartsSymbolSet *createSymbolSet(const QString &name);

  //---

  CQChartsLoadModelDlg    *loadModelDlg();
  CQChartsManageModelsDlg *manageModelsDlg();

  CQChartsEditModelDlg  *editModelDlg (CQChartsModelData *modelData);
  CQChartsCreatePlotDlg *createPlotDlg(CQChartsModelData *modelData);

  //---

#ifdef CQCHARTS_TK_WIDGET
  CTkApp *tkApp() const { return tkApp_; }
  void setTkApp(CTkApp *app) { tkApp_ = app; }
#endif

  //---

  CQTcl *cmdTcl() const { return cmdTcl_; }

  //---

  const CQChartsExprTcl *currentExpr() const { return currentExpr_; }
  void setCurrentExpr(CQChartsExprTcl *p) { currentExpr_ = p; }

  //---

  void addProperties();

  CQPropertyViewItem *addProperty(const QString &path, QObject *object,
                                  const QString &name, const QString &alias);

  //---

  const QStringList &pathList() const { return pathList_; }
  void setPathList(const QStringList &s) { pathList_ = s; }

  void addPath(const QString &path);

  QString lookupFile(const QString &fileName, bool exists=true) const;

  //---

  void errorMsg(const QString &msg) const;

  //---

  QStringList getMetaTypeNames() const;

  QString getPropertyTypeData(const QString &name) const;
  void setPropertyTypeData(const QString &name, const QString &value);

 Q_SIGNALS:
  void modelDataAdded(int);
  void modelDataRemoved(int);
  void modelDataChanged();

  void modelDataDataChanged();

  void currentModelChanged(int);

  void modelTypeChanged(int);

  void windowCreated(CQChartsWindow *window);
  void windowRemoved(CQChartsWindow *window);

  void viewAdded(CQChartsView *view);

  void viewRemoved(CQChartsView *view);

  void plotAdded(CQChartsPlot *plot);

  void themeChanged();
  void interfaceThemeChanged();

 private Q_SLOTS:
  void propertyItemChanged(QObject *, const QString &);

  void exitSlot();

 private:
  // add new model data for model
  int addModelData(ModelP &model);

  // assign model index to model
  bool setModelInd(QAbstractItemModel *model, int ind);

  void updateCurrentModelId();

 private:
  using NameViews       = std::map<QString, View*>;
  using InterfaceThemeP = std::unique_ptr<CQChartsInterfaceTheme>;
  using PlotTypeMgrP    = std::unique_ptr<CQChartsPlotTypeMgr>;
  using ColumnTypeMgrP  = std::unique_ptr<CQChartsColumnTypeMgr>;
  using SymbolSetMgrP   = std::unique_ptr<CQChartsSymbolSetMgr>;
  using PropertyModel   = CQPropertyViewModel;
  using PropertyModelP  = std::unique_ptr<PropertyModel>;

  QTimer* exitTimer_ { nullptr }; //!< auto exit timer

  // options
  bool   viewKey_       { true };  //!< has view key
  double maxSymbolSize_ { 100.0 }; //!< max symbol size (pixels)
  double maxFontSize_   { 100.0 }; //!< max font size (height in pixels)
  double maxLineWidth_  { 64.0 };  //!< max line width (pixels)

  // plot types
  PlotTypeMgrP plotTypeMgr_; //!< plot type manager

  // column types
  ColumnTypeMgrP columnTypeMgr_; //!< column type manager

  // theme
  InterfaceThemeP   interfaceTheme_; //!< interface theme
  CQChartsThemeName plotTheme_;      //!< plot theme name
  QColor            contrastColor_;  //!< color for contrast color calc

  SymbolSetMgrP symbolSetMgr_; //!< symbol set manager

  // model data
  int        currentModelInd_ { -1 }; //!< current model index
  ModelDatas modelDatas_;             //!< model datas
  int        lastModelInd_    { 0 };  //!< last model ind

  // views
  NameViews views_; //!< views

  // type procs
  TypeProcs typeProcs_; //!< tcl procs

  // dialogs
  CQChartsLoadModelDlg*    loadModelDlg_    { nullptr }; //!< load model dialog
  CQChartsManageModelsDlg* manageModelsDlg_ { nullptr }; //!< manage models dialog
  CQChartsEditModelDlg*    editModelDlg_    { nullptr }; //!< edit model dialog
  CQChartsCreatePlotDlg*   createPlotDlg_   { nullptr }; //!< create plot dialog

#ifdef CQCHARTS_TK_WIDGET
  // tk
  CTkApp *tkApp_ { nullptr };
#endif

  // tcl
  CQTcl*           cmdTcl_      { nullptr }; //!< command line tcl
  CQChartsExprTcl* currentExpr_ { nullptr }; //!< current expression evaluator

  std::set<QString> propertyNames_; //!< property names
  PropertyModelP    propertyModel_; //!< property model

  // paths
  QStringList pathList_; //!< list of paths for external files

  CQChartsPropertyMeta *metaObj_ { nullptr };
};

#endif
