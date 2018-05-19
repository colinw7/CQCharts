#ifndef CQChartsTest_H
#define CQChartsTest_H

#include <CQChartsCmds.h>
#include <CQChartsModelData.h>
#include <CQChartsInitData.h>
#include <CQAppWindow.h>
#include <CQChartsGeom.h>
#include <CQExprModel.h>

#include <QAbstractItemModel>
#include <QPointer>
#include <QSharedPointer>
#include <map>

#include <boost/optional.hpp>

class CQCharts;
class CQChartsLoader;
class CQChartsWindow;
class CQChartsView;
class CQChartsPlot;
class CQChartsPlotType;
class CQChartsPlotObj;
class CQChartsTable;
class CQChartsTree;
class CQExprModel;

class CQHistoryLineEdit;

class QItemSelectionModel;
class QLineEdit;
class QTextEdit;
class QTabWidget;
class QComboBox;
class QGridLayout;
class QStackedWidget;

//------

struct CQChartsViewWidgetData {
  int             ind          { -1 };
  int             tabInd       { -1 };
  QStackedWidget* stack        { nullptr };
  CQChartsTable*  table        { nullptr };
  CQChartsTree*   tree         { nullptr };
  QLineEdit*      filterEdit   { nullptr };
  QTextEdit*      detailsText  { nullptr };
};

//------

class CQChartsTest : public CQAppWindow {
  Q_OBJECT

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  using ParserType = CQChartsCmds::ParserType;
  using Args       = std::vector<QString>;
  using Vars       = std::vector<QString>;

 public:
  CQChartsTest();

 ~CQChartsTest();

  void setParserType(const ParserType &type);

  bool initPlot(const CQChartsInitData &initData);

  CQChartsPlot *initPlotView(const CQChartsModelData *modelData, const CQChartsInitData &initData,
                             int i, const CQChartsGeom::BBox &bbox);

  bool isGui() const { return gui_; }
  void setGui(bool b) { gui_ = b; }

  bool isShow() const { return show_; }
  void setShow(bool b) { show_ = b; }

  bool exec(const QString &filename);

  void print(const QString &filename);

  void loop();

  void timeout();

 private:
  //friend class CQChartsCmds;

 private:
  CQCharts *charts() const { return charts_; }

  void addMenus();

  void addViewWidgets(CQChartsModelData *modelData);

  QLineEdit *addLineEdit(QGridLayout *grid, int &row, const QString &name,
                         const QString &objName) const;

  void updateModel(CQChartsModelData *modelData);

  void updateModelDetails(const CQChartsModelData *modelData);

  QSize sizeHint() const;

  QTabWidget *viewTab() const { return viewTab_; }

  CQChartsViewWidgetData *viewWidgetData(int ind) const;

 private slots:
  void loadSlot();

  bool loadFileSlot(const QString &type, const QString &filename);

  void createSlot();

  void filterSlot();

  void exprSlot();

  void foldSlot();

  void tableColumnClicked(int column);

  void typeSetSlot();

  void currentTabChanged(int);

  void plotDialogCreatedSlot(CQChartsPlot *plot);

  void plotObjPressedSlot(CQChartsPlotObj *obj);

  void titleChanged(int ind, const QString &title);

  void updateModel(int ind);
  void updateModelDetails(int ind);

  void windowCreated(CQChartsWindow *window);
  void plotCreated(CQChartsPlot *plot);

  void modelDataAdded(int ind);

 private:
  using Plots   = std::vector<CQChartsPlot*>;
  using WindowP = QPointer<CQChartsWindow>;

  using ViewWidgetDatas = std::map<int,CQChartsViewWidgetData*>;

 private:
//Plots              plots_;
//CQChartsPlot*      rootPlot_       { nullptr };
  CQCharts*          charts_         { nullptr };
  QLineEdit*         foldEdit_       { nullptr };
  QLineEdit*         columnNumEdit_  { nullptr };
  QLineEdit*         columnNameEdit_ { nullptr };
  QLineEdit*         columnTypeEdit_ { nullptr };
  QTabWidget*        viewTab_        { nullptr };
  ViewWidgetDatas    viewWidgetDatas_;
  QComboBox*         exprCombo_      { nullptr };
  CQHistoryLineEdit* exprEdit_       { nullptr };
  QLineEdit*         exprColumn_     { nullptr };
  CQChartsLoader*    loader_         { nullptr };
  CQChartsCmds*      cmds_           { nullptr };
//QString            id_;
  bool               gui_            { true };
  bool               show_           { true };
};

#endif
