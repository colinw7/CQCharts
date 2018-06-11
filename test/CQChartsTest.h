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
class CQChartsModelControl;
class CQChartsModelList;
class CQChartsLoadDlg;
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
class QLabel;
class QLineEdit;
class QTextEdit;
class QTabWidget;
class QComboBox;
class QGridLayout;
class QVBoxLayout;
class QStackedWidget;

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
  const ParserType &parserType() const;

  bool initPlot(const CQChartsInitData &initData);

  CQChartsPlot *initPlotView(const CQChartsModelData *modelData, const CQChartsInitData &initData,
                             int i, const CQChartsGeom::BBox &bbox);

  bool isGui() const { return gui_; }
  void setGui(bool b) { gui_ = b; }

  bool isShow() const { return show_; }
  void setShow(bool b) { show_ = b; }

  CQCharts *charts() const { return charts_; }

  CQChartsModelList *modelList() const { return modelList_; }

  CQChartsModelControl *modelControl() const { return modelControl_; }

  CQChartsCmds *cmds() const { return cmds_; }

  bool exec(const QString &filename);

  void print(const QString &filename);

  void loop();

  void timeout();

  void updateModel(CQChartsModelData *modelData);

  void errorMsg(const QString &msg);

 private:
  void addControlWidgets(QVBoxLayout *layout);

  void addMenus();

  void updateModelDetails(const CQChartsModelData *modelData);

  QSize sizeHint() const;

 public slots:
  void closeSlot();

 private slots:
  void loadModelSlot();
  void modelLoadedSlot(int);

  void createPlotSlot();
  void plotDialogCreatedSlot(CQChartsPlot *plot);

  void plotObjPressedSlot(CQChartsPlotObj *obj);

  void updateModel(int ind);
  void updateModelDetails(int ind);

  void windowCreated(CQChartsWindow *window);

  void plotAdded(CQChartsPlot *plot);

  void updateModelControl();

 private:
  using Plots   = std::vector<CQChartsPlot*>;
  using WindowP = QPointer<CQChartsWindow>;

 private:
  CQCharts*             charts_        { nullptr };
  CQChartsModelList*    modelList_     { nullptr };
  CQChartsModelControl* modelControl_  { nullptr };
  CQChartsLoadDlg*      loadDlg_       { nullptr };
  CQChartsCmds*         cmds_          { nullptr };
  bool                  gui_           { true };
  bool                  show_          { true };
};

#endif
