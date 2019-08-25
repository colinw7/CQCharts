#ifndef CQCharts_H
#define CQCharts_H

#include <CQChartsThemeName.h>
#include <CQChartsUtil.h>

#include <CQBaseModelTypes.h>

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
class CQChartsInterfaceTheme;
class CQColorsPalette;
class CQChartsColor;

class CQChartsEditModelDlg;
class CQChartsCreatePlotDlg;

class CQPropertyViewItem;

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
 *   + \ref CQChartsDelaunayPlot
 *   + \ref CQChartsDendrogramPlot
 *   + \ref CQChartsDistributionPlot
 *   + \ref CQChartsForceDirectedPlot
 *   + \ref CQChartsGeometryPlot
 *   + \ref CQChartsHierBubblePlot
 *   + \ref CQChartsHierScatterPlot
 *   + \ref CQChartsImagePlot
 *   + \ref CQChartsParallelPlot
 *   + \ref CQChartsPiePlot
 *   + \ref CQChartsRadarPlot
 *   + \ref CQChartsSankeyPlot
 *   + \ref CQChartsScatterPlot
 *   + \ref CQChartsSunburstPlot
 *   + \ref CQChartsTreeMapPlot
 *   + \ref CQChartsXYPlot
 */

/*!
 * \brief Charts base class
 * \ingroup Charts
 */
class CQCharts : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool viewKey READ hasViewKey WRITE setViewKey)

 public:
  //! charts tcl proc data
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

  using ColorInd = CQChartsUtil::ColorInd;

 public:
  static QString description();

 public:
  CQCharts();

  virtual ~CQCharts();

  void init();

  //---

  bool hasViewKey() const { return viewKey_; }
  void setViewKey(bool b) { viewKey_ = b; }

  //---

  void getModelTypeNames(QStringList &names) const;

  //---

  bool isPlotType(const QString &name) const;

  CQChartsPlotType *plotType(const QString &name) const;

  void getPlotTypeNames(QStringList &names, QStringList &descs) const;

  void getPlotTypes(PlotTypes &types) const;

  //---

  CQChartsColumnTypeMgr *columnTypeMgr() const { return columnTypeMgr_; }

  //---

  QColor interpColor(const CQChartsColor &c, const ColorInd &ind) const;
  QColor interpColor(const CQChartsColor &c, int i, int n) const;
  QColor interpColor(const CQChartsColor &c, double value) const;

  QColor interpColorValue(const CQChartsColor &c, int ig, int ng, int i, int n) const;
  QColor interpColorValue(const CQChartsColor &c, int ig, int ng, double value) const;

  //---

  const CQChartsInterfaceTheme *interfaceTheme() const { return interfaceTheme_; }
  CQChartsInterfaceTheme *interfaceTheme() { return interfaceTheme_; }

  const CQChartsThemeName &plotTheme() const { return plotTheme_; }
  CQChartsThemeName &plotTheme() { return plotTheme_; }

  void setPlotTheme(const CQChartsThemeName &themeName);

  //---

 public:
  QColor interpPaletteColor(const ColorInd &ind, bool scale=false) const;
  QColor interpPaletteColor(int i, int n, bool scale=false) const;
  QColor interpPaletteColor(double r, bool scale=false) const;

  QColor interpIndPaletteColor(int ind, int i, int n, bool scale=false) const;
  QColor interpIndPaletteColor(int ind, double r, bool scale=false) const;

  QColor interpGroupPaletteColor(const ColorInd &ig, const ColorInd &iv, bool scale=false) const;
  QColor interpGroupPaletteColor(int ig, int ng, int i, int n, bool scale=false) const;
  QColor interpGroupPaletteColor(int ig, int ng, double r, bool scale=false) const;

  QColor interpPaletteColorValue(int ig, int ng, int i, int n, bool scale=false) const;
  QColor interpPaletteColorValue(int ig, int ng, double r, bool scale=false) const;

  QColor indexPaletteColor(int i, int n) const;
  QColor indexIndPaletteColor(int ind, int i, int n) const;

  //---

  QColor interpThemeColor(const ColorInd &ind) const;
  QColor interpThemeColor(int i, int n) const;
  QColor interpThemeColor(double r) const;

  CQColorsPalette *themeGroupPalette(int i, int n) const;

  CQColorsPalette *themePalette(int ind) const;

  const CQColorsTheme *theme() const;
  CQColorsTheme *theme();

  //---

  QColor interpModelColor(const CQChartsColor &c, double value) const;

  //---

  CQChartsColor adjustDefaultPalette(const CQChartsColor &c, const QString &defaultPalette) const;

 private:
  QColor interpIndPaletteColorValue(int ind, int ig, int ng,
                                    double r, bool scale) const;

  QColor interpNamePaletteColorValue(const QString &name, int ig, int ng,
                                     double r, bool scale) const;

  QColor interpNamePaletteColor(const QString &name, double r, bool scale) const;

  QColor indexNamePaletteColor(const QString &name, int ig, int ng) const;

  //---

 public:
  CQChartsModelData *initModelData(ModelP &model);

  CQChartsModelData *getModelData(const QAbstractItemModel *model) const;
  CQChartsModelData *getModelData(int ind) const;

  int currentModelInd() const { return currentModelInd_; }
  void setCurrentModelInd(int ind);

  CQChartsModelData *currentModelData() const;
  void setCurrentModelData(CQChartsModelData *modelData);

  void getModelDatas(ModelDatas &modelDatas) const;

  void setModelName(CQChartsModelData *modelData, const QString &name);

  void setModelFileName(CQChartsModelData *modelData, const QString &fileName);

  bool removeModelData(CQChartsModelData *modelData);

  //---

  CQChartsView *addView(const QString &id="");

  void addView(CQChartsView *view);

  virtual CQChartsView *createView();
  virtual void deleteView(CQChartsView *view);

  CQChartsView *getView(const QString &id) const;

  CQChartsView *currentView() const;

  void getViews(Views &views) const;

  void getViewIds(QStringList &names) const;

  void removeView(CQChartsView *view);

  //---

  virtual CQChartsWindow *createWindow(CQChartsView *view);
  virtual void deleteWindow(CQChartsWindow *window);

  //---

  void addProc(const QString &name, const QString &args, const QString &body);

  void removeProc(const QString &name);

  void getProcs(QStringList &names);

  bool getProcData(const QString &name, QString &args, QString &body) const;

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

  static void setItemIsStyle(CQPropertyViewItem *item);
  static bool getItemIsStyle(const CQPropertyViewItem *item);

  static void setItemIsHidden(CQPropertyViewItem *item);
  static bool getItemIsHidden(const CQPropertyViewItem *item);

  //---

  CQChartsEditModelDlg  *editModelDlg (CQChartsModelData *modelData);
  CQChartsCreatePlotDlg *createPlotDlg(CQChartsModelData *modelData);

  //---

  void errorMsg(const QString &msg) const;

 signals:
  void modelDataAdded(int);
  void modelDataRemoved(int);

  void currentModelChanged(int);

  void modelNameChanged(const QString &);

  void modelTypeChanged(int);

  void windowCreated(CQChartsWindow *window);
  void windowRemoved(CQChartsWindow *window);

  void viewAdded(CQChartsView *view);

  void viewRemoved(CQChartsView *view);

  void plotAdded(CQChartsPlot *plot);

  void themeChanged();

 private:
  // add new model data for model
  int addModelData(ModelP &model);

  // remove model data for model
  bool removeModelData(ModelP &model);

  // assign model index to model
  bool setModelInd(QAbstractItemModel *model, int ind);

 private:
  using NameViews = std::map<QString,CQChartsView*>;

  bool                    viewKey_         { true };    //!< has view key
  CQChartsPlotTypeMgr*    plotTypeMgr_     { nullptr }; //!< plot type manager
  CQChartsColumnTypeMgr*  columnTypeMgr_   { nullptr }; //!< column type manager
  CQChartsInterfaceTheme* interfaceTheme_  { nullptr }; //!< interface theme
  CQChartsThemeName       plotTheme_;                   //!< plot theme name
  int                     currentModelInd_ { -1 };      //!< current model index
  ModelDatas              modelDatas_;                  //!< model datas
  int                     lastModelInd_    { 0 };       //!< last model ind
  NameViews               views_;                       //!< views
  Procs                   procs_;                       //!< tcl procs
  CQChartsEditModelDlg*   editModelDlg_    { nullptr }; //!< edit model dialog
  CQChartsCreatePlotDlg*  createPlotDlg_   { nullptr }; //!< create plot dialog
};

#endif
