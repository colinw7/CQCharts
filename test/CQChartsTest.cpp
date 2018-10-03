#include <CQChartsTest.h>
#include <CQChartsAppWindow.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsAxis.h>
#include <CQChartsCmds.h>
#include <CQCharts.h>
#include <CQChartsModelDlg.h>

#include <CQSortModel.h>

#ifdef CQ_APP_H
#include <CQApp.h>
#else
#include <QApplication>
#endif

#include <CQMsgHandler.h>
#include <CQUtil.h>
#include <CReadLine.h>

#include <QFile>
#include <QTextStream>

#include <mcheck.h>

namespace {

void errorMsg(const QString &msg) {
  std::cerr << msg.toStdString() << "\n";
}

}

//----

class MainArgs {
 public:
  using OptReal   = boost::optional<double>;
  using OptInt    = boost::optional<int>;
  using OptString = boost::optional<QString>;

 public:
  MainArgs(int &argc, char **argv) :
   argc_(argc), argv_(argv) {
  }

  bool eof() const { return i_ >= argc_; };

  bool isOpt() const { return argv_[i_][0] == '-'; }

  QString arg() const { return argv_[i_]; }
  QString opt() const { return &argv_[i_][1]; }

  void next() { ++i_; }

  bool parseOpt(QString &s) {
    ++i_;

    if (i_ >= argc_)
      return false;

    s = argv_[i_];

    return true;
  }

  bool parseOpt(int &i) {
    ++i_;

    if (i_ >= argc_)
      return false;

    i = atoi(argv_[i_]);

    return true;
  }

  bool parseOpt(double &r) {
    ++i_;

    if (i_ >= argc_)
      return false;

    r = std::stod(argv_[i_]);

    return true;
  }

  bool parseOpt(OptString &s) {
    ++i_;

    if (i_ >= argc_)
      return false;

    s = argv_[i_];

    return true;
  }

  bool parseOpt(OptReal &r) {
    ++i_;

    if (i_ >= argc_)
      return false;

    r = std::stod(argv_[i_]);

    return true;
  }

 private:
  int    i_    { 1 };
  int    argc_ { 0 };
  char **argv_ { nullptr };
};

struct MainData {
  using InitDatas = std::vector<CQChartsInitData>;
  using OptString = boost::optional<QString>;
  using OptReal   = boost::optional<double>;

  bool             dark         { false };
  QString          execFile;
  QString          viewTitle;
  bool             overlay      { false };
  bool             x1x2         { false };
  bool             y1y2         { false };
  bool             horizontal   { false };
  bool             vertical     { false };
  bool             loop         { false };
  OptString        printFile;
  bool             gui          { true };
  bool             showApp      { false };
  bool             showAppSet   { false };
  bool             showModel    { false };
  bool             showModelSet { false };
  bool             exit         { false };
  int              viewWidth    { 100 };
  int              viewHeight   { 100 };
  OptReal          xmin1;
  OptReal          xmax1;
  OptReal          xmin2;
  OptReal          xmax2;
  OptReal          ymin1;
  OptReal          ymax1;
  OptReal          ymin2;
  OptReal          ymax2;
  CQChartsInitData initData;
  InitDatas        initDatas;
};

//----

void parseArgs(int argc, char **argv, MainData &mainData);

int
main(int argc, char **argv)
{
  //mtrace();

  //---

  CQMsgHandler::install();

#ifdef CQ_APP_H
  CQApp app(argc, argv);
#else
  QApplication app(argc, argv);
#endif

  CQUtil::initProperties();

  //---

  // parse arguments into main data
  MainData mainData;

  mainData.viewWidth  = CQChartsView::getSizeHint().width ();
  mainData.viewHeight = CQChartsView::getSizeHint().height();

  parseArgs(argc, argv, mainData);

  //---

  // set size hint
  CQChartsView::setSizeHint(QSize(mainData.viewWidth, mainData.viewHeight));

  //---

  // set dark theme
  if (mainData.dark) {
#ifdef CQ_APP_H
    app.setDarkTheme(true);
#endif
  }

  //---

  // create test object
  CQChartsTest test;

  if (! mainData.gui)
    test.setGui(false);

  //---

  // calculate default layout for plots
  int nd = mainData.initDatas.size();

  int nr = 1, nc = 1;

  if      (mainData.horizontal)
    nc = nd;
  else if (mainData.vertical)
    nr = nd;
  else {
    nr = std::max(int(sqrt(nd)), 1);
    nc = (nd + nr - 1)/nr;
  }

  //---

  // create plots
  double vr = CQChartsView::viewportRange();

  double dx = vr/nc;
  double dy = vr/nr;

  int numPlots = 0;

  for (auto &initData : mainData.initDatas) {
    // set placement
    initData.viewTitle = mainData.viewTitle;
    initData.overlay   = mainData.overlay;
    initData.x1x2      = mainData.x1x2;
    initData.y1y2      = mainData.y1y2;
    initData.nr        = nr;
    initData.nc        = nc;
    initData.dx        = dx;
    initData.dy        = dy;

    //---

    // set range
    if (mainData.xmin1) initData.xmin = mainData.xmin1;
    if (mainData.xmax1) initData.xmax = mainData.xmax1;

    if      (initData.x1x2) {
      if      (numPlots == 0) {
        if (mainData.xmin1) initData.xmin = mainData.xmin1;
        if (mainData.xmax1) initData.xmax = mainData.xmax1;
      }
      else if (numPlots >= 1) {
        if (mainData.xmin2) initData.xmin = mainData.xmin2;
        if (mainData.xmax2) initData.xmax = mainData.xmax2;
      }
    }
    else if (initData.y1y2) {
      if      (numPlots == 0) {
        if (mainData.ymin1) initData.ymin = mainData.ymin1;
        if (mainData.ymax1) initData.ymax = mainData.ymax1;
      }
      else if (numPlots >= 1) {
        if (mainData.ymin2) initData.ymin = mainData.ymin2;
        if (mainData.ymax2) initData.ymax = mainData.ymax2;
      }
    }
    else {
      if (mainData.ymin1) initData.ymin = mainData.ymin1;
      if (mainData.ymax1) initData.ymax = mainData.ymax1;
    }

    //---

    // create plot
    if (! test.initPlot(initData))
      continue;

    ++numPlots;
  }

  //---

  // exec init file
  if (mainData.execFile.length()) {
    test.exec(mainData.execFile);
  }

  //---

  // no plots and app state no specified so show if no plots
  if (! mainData.showAppSet) {
    if (! mainData.execFile.length() && mainData.initDatas.empty())
      mainData.showApp = true;
  }

  if (! mainData.showModelSet) {
    if (! mainData.execFile.length() && ! mainData.initDatas.empty() && numPlots == 0)
      mainData.showModel = true;
  }

  //---

  // show test widget
  CQChartsAppWindow *appWindow = nullptr;

  if      (mainData.showApp) {
    appWindow = new CQChartsAppWindow(test.charts());

    appWindow->show();

    if (appWindow->window())
      appWindow->window()->raise();
  }
  else if (mainData.showModel) {
    CQChartsModelDlg *dlg = new CQChartsModelDlg(test.charts());

    dlg->show();
  }

  // print plot
  if (mainData.printFile)
    test.print(*mainData.printFile);

  //---

  // loop
  if (! mainData.exit) {
    if (! mainData.loop)
      app.exec();
    else
      test.loop();
  }

  return 0;
}

void
parseArgs(int argc, char **argv, MainData &mainData)
{
  MainArgs args(argc, argv);

  for ( ; ! args.eof(); args.next()) {
    if (args.isOpt()) {
      QString arg = args.opt();

      if      (arg == "dark") {
        mainData.dark = true;
      }

      // input data type
      else if (arg == "csv")
        mainData.initData.fileType = CQChartsFileType::CSV;
      else if (arg == "tsv")
        mainData.initData.fileType = CQChartsFileType::TSV;
      else if (arg == "json")
        mainData.initData.fileType = CQChartsFileType::JSON;
      else if (arg == "data")
        mainData.initData.fileType = CQChartsFileType::DATA;
      else if (arg == "expr")
        mainData.initData.fileType = CQChartsFileType::EXPR;

      // input data control
      else if (arg == "comment_header")
        mainData.initData.inputData.commentHeader = true;
      else if (arg == "first_line_header")
        mainData.initData.inputData.firstLineHeader = true;
      else if (arg == "first_column_header")
        mainData.initData.inputData.firstColumnHeader = true;
      else if (arg == "num_rows") {
        if (args.parseOpt(mainData.initData.inputData.numRows))
          mainData.initData.inputData.numRows = std::max(mainData.initData.inputData.numRows, 1);
      }
      else if (arg == "filter") {
        args.parseOpt(mainData.initData.inputData.filter);
      }
#ifdef CQCHARTS_FOLDED_MODEL
      else if (arg == "fold") {
        args.parseOpt(mainData.initData.inputData.fold);
      }
#endif

      // process data
      else if (arg == "process") {
        QString str;

        if (args.parseOpt(str)) {
          if (mainData.initData.process.length())
            mainData.initData.process += ";";

          mainData.initData.process += str;
        }
      }
      else if (arg == "process-add") {
        QString str;

        if (args.parseOpt(str)) {
          if (mainData.initData.processAdd.length())
            mainData.initData.processAdd += ";";

          mainData.initData.processAdd += str;
        }
      }
      // sort data
      else if (arg == "sort") {
        args.parseOpt(mainData.initData.inputData.sort);
      }

      // plot type
      else if (arg == "type") {
        args.parseOpt(mainData.initData.typeName);
      }

      // plot filter
      else if (arg == "where") {
        args.parseOpt(mainData.initData.filterStr);
      }

      // plot columns
      else if (arg == "column" || arg == "columns") {
        QString columnsStr;

        if (args.parseOpt(columnsStr)) {
          QStringList strs = columnsStr.split(",", QString::SkipEmptyParts);

          for (int j = 0; j < strs.size(); ++j) {
            const QString &nameValue = strs[j];

            auto pos = nameValue.indexOf('=');

            if (pos >= 0) {
              auto name  = nameValue.mid(0, pos).simplified();
              auto value = nameValue.mid(pos + 1).simplified();

              mainData.initData.setNameValue(name, value);
            }
            else {
              errorMsg("Invalid " + arg + " option '" + QString(columnsStr));
            }
          }
        }
      }
      else if (arg == "x") {
        QString str;

        if (args.parseOpt(str))
          mainData.initData.setNameValue("x", str);
      }
      else if (arg == "y") {
        QString str;

        if (args.parseOpt(str))
          mainData.initData.setNameValue("y", str);
      }
      else if (arg == "z") {
        QString str;

        if (args.parseOpt(str))
          mainData.initData.setNameValue("z", str);
      }

      // plot bool parameters
      else if (arg == "bool") {
        QString boolStr;

        if (args.parseOpt(boolStr)) {
          QStringList strs = boolStr.split(",", QString::SkipEmptyParts);

          for (int j = 0; j < strs.size(); ++j) {
            const QString &nameValue = strs[j];

            auto pos = nameValue.indexOf('=');

            QString name, value;

            if (pos >= 0) {
              name  = nameValue.mid(0, pos).simplified();
              value = nameValue.mid(pos + 1).simplified();
            }
            else {
              name  = nameValue;
              value = "true";
            }

            bool ok;

            bool b = CQChartsUtil::stringToBool(value, &ok);

            if (ok)
              mainData.initData.setNameBool(name, b);
            else {
              errorMsg("Invalid -bool option '" + QString(boolStr));
            }
          }
        }
      }
      // plot string parameters
      else if (arg == "string") {
        QString stringStr;

        if (args.parseOpt(stringStr)) {
          QStringList strs = stringStr.split(",", QString::SkipEmptyParts);

          for (int j = 0; j < strs.size(); ++j) {
            const QString &nameValue = strs[j];

            auto pos = nameValue.indexOf('=');

            QString name, value;

            if (pos >= 0) {
              name  = nameValue.mid(0, pos).simplified();
              value = nameValue.mid(pos + 1).simplified();
            }
            else {
              name  = nameValue;
              value = "";
            }

            mainData.initData.setNameString(name, value);
          }
        }
      }
      // plot real parameters
      else if (arg == "real") {
        QString realStr;

        if (args.parseOpt(realStr)) {
          QStringList strs = realStr.split(",", QString::SkipEmptyParts);

          for (int j = 0; j < strs.size(); ++j) {
            const QString &nameValue = strs[j];

            auto pos = nameValue.indexOf('=');

            QString name, value;

            if (pos >= 0) {
              name  = nameValue.mid(0, pos).simplified();
              value = nameValue.mid(pos + 1).simplified();
            }
            else {
              name  = nameValue;
              value = "0.0";
            }

            bool ok;

            double r = value.toDouble(&ok);

            if (ok)
              mainData.initData.setNameReal(name, r);
            else {
              errorMsg("Invalid -bool option '" + QString(realStr));
            }
          }
        }
      }
      else if (arg == "bivariate") {
        mainData.initData.setNameBool("bivariate", true);
      }
      else if (arg == "stacked") {
        mainData.initData.setNameBool("stacked", true);
      }
      else if (arg == "cumulative") {
        mainData.initData.setNameBool("cumulative", true);
      }
      else if (arg == "fillunder") {
        mainData.initData.setNameBool("fillUnder", true);
      }
      else if (arg == "impulse") {
        mainData.initData.setNameBool("impulse", true);
      }

      // column types
      else if (arg == "column_type") {
        args.parseOpt(mainData.initData.columnType);
      }

      // axis type
      else if (arg == "xintegral") {
        mainData.initData.xintegral = true;
      }
      else if (arg == "yintegral") {
        mainData.initData.yintegral = true;
      }

      // log scale
      else if (arg == "xlog") {
        mainData.initData.xlog = true;
      }
      else if (arg == "ylog") {
        mainData.initData.ylog = true;
      }

      // title
      else if (arg == "view_title") {
        args.parseOpt(mainData.viewTitle);
      }

      else if (arg == "plot_title") {
        args.parseOpt(mainData.initData.title);
      }

      // view properties
      else if (arg == "view_properties") {
        QString str;

        if (args.parseOpt(str)) {
          if (mainData.initData.viewProperties.length())
            mainData.initData.viewProperties += ",";

          mainData.initData.viewProperties += str;
        }
      }

      // plot properties
      else if (arg == "properties" || arg == "plot_properties") {
        QString str;

        if (args.parseOpt(str)) {
          if (mainData.initData.plotProperties.length())
            mainData.initData.plotProperties += ",";

          mainData.initData.plotProperties += str;
        }
      }

      // plot chaining (overlay, y1y2, and)
      else if (arg == "overlay") {
        mainData.overlay = true;
      }
      else if (arg == "x1x2") {
        mainData.x1x2 = true;
      }
      else if (arg == "y1y2") {
        mainData.y1y2 = true;
      }
      else if (arg == "horizontal") {
        mainData.horizontal = true;
      }
      else if (arg == "vertical") {
        mainData.vertical = true;
      }
      else if (arg == "and") {
        mainData.initDatas.push_back(mainData.initData);

        if (! mainData.overlay) {
          mainData.xmin1 = boost::make_optional(false, 0.0);
          mainData.xmax1 = boost::make_optional(false, 0.0);
          mainData.xmin2 = boost::make_optional(false, 0.0);
          mainData.xmax2 = boost::make_optional(false, 0.0);
          mainData.ymin1 = boost::make_optional(false, 0.0);
          mainData.ymax1 = boost::make_optional(false, 0.0);
          mainData.ymin2 = boost::make_optional(false, 0.0);
          mainData.ymax2 = boost::make_optional(false, 0.0);
        }

        mainData.initData = CQChartsInitData();
      }

      // data range
      else if (arg == "xmin" || arg == "xmin1") {
        args.parseOpt(mainData.xmin1);
      }
      else if (arg == "xmin2") {
        args.parseOpt(mainData.xmin2);
      }
      else if (arg == "xmax" || arg == "xmax1") {
        args.parseOpt(mainData.xmax1);
      }
      else if (arg == "xmax2") {
        args.parseOpt(mainData.xmax2);
      }
      else if (arg == "ymin" || arg == "ymin1") {
        args.parseOpt(mainData.ymin1);
      }
      else if (arg == "ymin2") {
        args.parseOpt(mainData.ymin2);
      }
      else if (arg == "ymax" || arg == "ymax1") {
        args.parseOpt(mainData.ymax1);
      }
      else if (arg == "ymax2") {
        args.parseOpt(mainData.ymax2);
      }

      // exec file
      else if (arg == "exec") {
        args.parseOpt(mainData.execFile);
      }
      // prompt loop
      else if (arg == "loop") {
        mainData.loop = true;
      }

      // view_width
      else if (arg == "view_width") {
        args.parseOpt(mainData.viewWidth);
      }
      // view_height
      else if (arg == "view_height") {
        args.parseOpt(mainData.viewHeight);
      }

      // print
      else if (arg == "print") {
        args.parseOpt(mainData.printFile);
      }

      // no gui
      else if (arg == "nogui" || arg == "no_gui") {
        mainData.gui = false;
      }

      // show app
      else if (arg == "show_app") {
        mainData.showApp    = true;
        mainData.showAppSet = true;
      }
      // hide app
      else if (arg == "hide_app") {
        mainData.showApp    = false;
        mainData.showAppSet = true;
      }

      // show model dlg
      else if (arg == "show_app") {
        mainData.showModel    = true;
        mainData.showModelSet = true;
      }

      // exit
      else if (arg == "exit") {
        mainData.exit = true;
      }

      else {
        errorMsg("Invalid option '" + QString(args.arg()));
      }
    }
    else {
      mainData.initData.filenames.push_back(args.arg());
    }
  }

  if (mainData.initData.fileType != CQChartsFileType::NONE)
    mainData.initDatas.push_back(mainData.initData);
}

//-----

CQChartsTest::
CQChartsTest() :
 QObject()
{
  charts_ = new CQCharts;

  charts_->init();

  //---

  // create commands
  cmds_ = new CQChartsCmds(charts_);

  connect(charts_, SIGNAL(windowCreated(CQChartsWindow *)),
          this, SLOT(windowCreated(CQChartsWindow *)));
  connect(charts_, SIGNAL(plotAdded(CQChartsPlot *)),
          this, SLOT(plotAdded(CQChartsPlot *)));
}

CQChartsTest::
~CQChartsTest()
{
  delete cmds_;
  delete charts_;
}

//------

bool
CQChartsTest::
initPlot(const CQChartsInitData &initData)
{
  CQChartsView *view = charts_->currentView();

  //---

  int i = (view ? view->numPlots() : 0);

  int r = i / initData.nc;
  int c = i % initData.nc;

  double vr = CQChartsView::viewportRange();

  CQChartsGeom::BBox bbox(0, 0, vr, vr);

  if (! initData.overlay) {
    double x1 =  c     *initData.dx;
    double x2 = (c + 1)*initData.dx;
    double y1 =  r     *initData.dy;
    double y2 = (r + 1)*initData.dy;

    bbox = CQChartsGeom::BBox(x1, vr - y2, x2, vr - y1);
  }

  //---

  QString filename;

  if (initData.filenames.size() > 0) {
    filename = initData.filenames[0];

    if (initData.fileType != CQChartsFileType::NONE) {
      if (! cmds_->loadFileModel(filename, initData.fileType, initData.inputData))
        return false;
    }
    else {
      errorMsg("No file type specified");
    }
  }
  else {
    if (initData.fileType == CQChartsFileType::EXPR) {
      if (! cmds_->loadFileModel("", initData.fileType, initData.inputData))
        return false;
    }
  }

  //---

  CQChartsModelData *modelData = charts_->currentModelData();

  if (! modelData)
    return false;

  //---

  if (initData.process.length()) {
    ModelP model = modelData->currentModel();

    QStringList strs = initData.process.split(";", QString::SkipEmptyParts);

    for (int i = 0; i < strs.size(); ++i)
      CQChartsUtil::processExpression(model.data(), strs[i]);
  }

  if (initData.processAdd.length()) {
    ModelP model = modelData->currentModel();

    QStringList strs = initData.processAdd.split(";", QString::SkipEmptyParts);

    for (int i = 0; i < strs.size(); ++i)
      CQChartsUtil::processAddExpression(model.data(), strs[i]);
  }

  //---

  CQChartsPlot *plot = initPlotView(modelData, initData, i, bbox);

  if (! plot)
    return false;

  if (filename != "")
    plot->setFileName(filename);

  //---

  if      (initData.x1x2) {
    if      (i == 0) {
    }
    else if (i >= 1) {
      if (initData.viewTitle.length())
        view->setTitle(initData.viewTitle);

      CQChartsPlot *prevPlot = view->plot(0);

      view->initX1X2(prevPlot, plot, initData.overlay);
    }
  }
  else if (initData.y1y2) {
    if      (i == 0) {
    }
    else if (i >= 1) {
      if (initData.viewTitle.length())
        view->setTitle(initData.viewTitle);

      CQChartsPlot *prevPlot = view->plot(0);

      view->initY1Y2(prevPlot, plot, initData.overlay);
    }
  }
  else if (initData.overlay) {
    plot->setOverlay(true);

    if      (i == 0) {
    }
    else if (i >= 1) {
      if (initData.viewTitle.length())
        view->setTitle(initData.viewTitle);

      CQChartsPlot *prevPlot = view->plot(i - 1);

      CQChartsView::Plots plots;

      plots.push_back(prevPlot);
      plots.push_back(plot);

      view->initOverlay(plots);
    }
  }

  //---

  return true;
}

//------

CQChartsPlot *
CQChartsTest::
initPlotView(const CQChartsModelData *modelData, const CQChartsInitData &initData, int i,
             const CQChartsGeom::BBox &bbox)
{
  ModelP model = modelData->currentModel();

  CQChartsUtil::setColumnTypeStrs(charts_, model.data(), initData.columnType);

  //---

  QString typeName = CQChartsCmds::fixTypeName(initData.typeName);

  if (typeName == "")
    return nullptr;

  // ignore if bad type
  CQChartsPlotType *type = charts_->plotType(typeName);

  if (! type) {
    errorMsg("Invalid type '" + typeName + "' for plot");
    return nullptr;
  }

  //---

  // reuse plot if needed
  bool reuse = false;

  if (initData.overlay) {
    if (typeName == "xy"       || typeName == "barchart" ||
        typeName == "geometry" || typeName == "delaunay")
      reuse = true;
  }
  else {
    reuse = true;
  }

  //---

  CQChartsPlot *plot = nullptr;

  // create plot from init (argument) data
  if (initData.filterStr.length()) {
    CQSortModel *sortModel = new CQSortModel(model.data());

    ModelP sortModelP(sortModel);

    sortModel->setFilter(initData.filterStr);

    plot = cmds_->createPlot(nullptr, sortModelP, modelData->selectionModel(), type,
                             initData.nameValueData, reuse, bbox);
  }
  else {
    plot = cmds_->createPlot(nullptr, model, modelData->selectionModel(), type,
                             initData.nameValueData, reuse, bbox);
  }

  if (! plot) {
    errorMsg("Create plot failed");
    return nullptr;
  }

  if (initData.viewProperties != "") {
    plot->view()->setProperties(initData.viewProperties);
  }

  //---

  // init plot
  if (initData.overlay) {
    if (i > 0) {
      plot->setPlotFilled(false);
      plot->setDataFilled(false);
    }
  }

  if (initData.title != "")
    plot->setTitleStr(initData.title);

  if (initData.xlog)
    plot->setLogX(true);

  if (initData.ylog)
    plot->setLogY(true);

  if (initData.xintegral)
    plot->xAxis()->setIntegral(true);

  if (initData.yintegral)
    plot->yAxis()->setIntegral(true);

  if (initData.xmin) plot->setXMin(initData.xmin);
  if (initData.ymin) plot->setYMin(initData.ymin);
  if (initData.xmax) plot->setXMax(initData.xmax);
  if (initData.ymax) plot->setYMax(initData.ymax);

  //---

  if (initData.plotProperties != "") {
    if (! plot->setProperties(initData.plotProperties))
      errorMsg("Failed to set plot properties '" + initData.plotProperties + "'");
  }

  return plot;
}

//------

void
CQChartsTest::
windowCreated(CQChartsWindow *window)
{
  if (isShow())
    window->show();
}

void
CQChartsTest::
plotAdded(CQChartsPlot *plot)
{
  connect(plot, SIGNAL(objPressed(CQChartsPlotObj *)),
          this, SLOT(plotObjPressedSlot(CQChartsPlotObj *)));
}

void
CQChartsTest::
plotObjPressedSlot(CQChartsPlotObj *obj)
{
  QString id = obj->id();

  if (id.length())
    errorMsg(id);
}

//------

bool
CQChartsTest::
exec(const QString &filename)
{
  // open file
  QFile file(filename);

  if (! file.open(QIODevice::ReadOnly))
    return false;

  // read lines
  QTextStream in(&file);

  while (! in.atEnd()) {
    QString line = in.readLine();

    bool join;

    while (! CQChartsCmds::isCompleteLine(line, join)) {
      if (in.atEnd())
        break;

      QString line1 = in.readLine();

      if (! join)
        line += "\n" + line1;
      else
        line += line1;
    }

    cmds_->parseLine(line, /*log*/false);
  }

  file.close();

  return true;
}

//------

void
CQChartsTest::
print(const QString &filename)
{
  CQChartsView *view = charts_->currentView();

  if (! view)
    return;

  if (! isShow()) {
    view->resize(view->sizeHint());

    view->resizeEvent(0);
  }

  view->printFile(filename);
}

//------

class CQChartsReadLine : public CReadLine {
 public:
  CQChartsReadLine(CQChartsTest *test) :
   test_(test) {
  }

  void timeout() {
    test_->timeout();
  }

 private:
  CQChartsTest *test_;
};

void
CQChartsTest::
loop()
{
  int rlTimeout = 10;

  CQChartsReadLine *readLine = new CQChartsReadLine(this);

  readLine->enableTimeoutHook(rlTimeout);

  for (;;) {
    readLine->setPrompt("> ");

    QString line = readLine->readLine().c_str();

    bool join;

    while (! CQChartsCmds::isCompleteLine(line, join)) {
      readLine->setPrompt("+> ");

      QString line1 = readLine->readLine().c_str();

      if (! join)
        line += "\n" + line1;
      else
        line += line1;
    }

    cmds_->parseLine(line);

    readLine->addHistory(line.toStdString());
  }
}

void
CQChartsTest::
timeout()
{
  if (! qApp->activeModalWidget())
    qApp->processEvents();
}

void
CQChartsTest::
errorMsg(const QString &msg)
{
  std::cerr << msg.toStdString() << "\n";
}
