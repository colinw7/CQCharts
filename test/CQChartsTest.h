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
class CQChartsModelWidgets;
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

class CQChartsTest : public QObject {
  Q_OBJECT

 public:
  using ParserType = CQChartsCmds::ParserType;
  using Args       = std::vector<QString>;
  using Vars       = std::vector<QString>;
  using ModelP     = QSharedPointer<QAbstractItemModel>;

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

  bool isShowApp() const { return showApp_; }
  void setShowApp(bool b) { showApp_ = b; }

  CQCharts *charts() const { return charts_; }

  CQChartsCmds *cmds() const { return cmds_; }

  bool exec(const QString &filename);

  void print(const QString &filename);

  void loop();

  void timeout();

  void errorMsg(const QString &msg);

 private slots:
  void plotObjPressedSlot(CQChartsPlotObj *obj);

  void windowCreated(CQChartsWindow *window);

  void plotAdded(CQChartsPlot *plot);

 private:
  CQCharts*     charts_  { nullptr };
  CQChartsCmds* cmds_    { nullptr };
  bool          gui_     { true };
  bool          show_    { true };
  bool          showApp_ { false };
};

#endif
