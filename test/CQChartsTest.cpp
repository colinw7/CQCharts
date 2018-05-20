#include <CQChartsTest.h>
#include <CQCharts.h>
#include <CQChartsTable.h>
#include <CQChartsTree.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsLoader.h>
#include <CQChartsPlotDlg.h>
#include <CQChartsCmds.h>

#include <CQExprModel.h>
#include <CQSortModel.h>

#include <CQHistoryLineEdit.h>

#ifdef CQ_APP_H
#include <CQApp.h>
#else
#include <QApplication>
#endif

#include <CQUtil.h>
#include <CReadLine.h>

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStackedWidget>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  QByteArray localMsg = msg.toLocal8Bit();

  switch (type) {
    case QtDebugMsg:
      fprintf(stderr, "Debug: %s (%s:%u, %s)\n",
              localMsg.constData(), context.file, context.line, context.function);
      break;
    case QtInfoMsg:
      fprintf(stderr, "Info: %s (%s:%u, %s)\n",
              localMsg.constData(), context.file, context.line, context.function);
      break;
    case QtWarningMsg:
      fprintf(stderr, "Warning: %s (%s:%u, %s)\n",
              localMsg.constData(), context.file, context.line, context.function);
      break;
    case QtCriticalMsg:
      fprintf(stderr, "Critical: %s (%s:%u, %s)\n",
              localMsg.constData(), context.file, context.line, context.function);
      break;
    case QtFatalMsg:
      fprintf(stderr, "Fatal: %s (%s:%u, %s)\n",
              localMsg.constData(), context.file, context.line, context.function);
      abort();
  }
}

namespace {

bool stringToBool(const QString &str, bool *ok) {
  QString lstr = str.toLower();

  if (lstr == "0" || lstr == "false" || lstr == "no") {
    *ok = true;
    return false;
  }

  if (lstr == "1" || lstr == "true" || lstr == "yes") {
    *ok = true;
    return true;
  }

  *ok = false;

  return false;
}

}

//----

namespace {

void
errorMsg(const QString &msg)
{
  std::cerr << msg.toStdString() << "\n";
}

}

//----

int
main(int argc, char **argv)
{
  using ParserType = CQChartsCmds::ParserType;
  using OptString  = boost::optional<QString>;
  using OptReal    = boost::optional<double>;

  qInstallMessageHandler(myMessageOutput);

#ifdef CQ_APP_H
  CQApp app(argc, argv);
#else
  QApplication app(argc, argv);
#endif

  CQUtil::initProperties();

  std::vector<CQChartsInitData> initDatas;

  QString execFile;
  QString viewTitle;

  bool       overlay    = false;
  bool       x1x2       = false;
  bool       y1y2       = false;
  bool       horizontal = false;
  bool       vertical   = false;
  bool       loop       = false;
#ifdef CQ_CHARTS_CEIL
  ParserType parserType = ParserType::CEIL;
#else
  ParserType parserType = ParserType::SCRIPT;
#endif
  OptString  printFile;
  bool       gui        = true;
  bool       closeApp   = false;
  bool       exit       = false;
  int        viewWidth  = CQChartsView::getSizeHint().width ();
  int        viewHeight = CQChartsView::getSizeHint().height();

  OptReal xmin1 = boost::make_optional(false, 0.0);
  OptReal xmax1 = boost::make_optional(false, 0.0);
  OptReal xmin2 = boost::make_optional(false, 0.0);
  OptReal xmax2 = boost::make_optional(false, 0.0);
  OptReal ymin1 = boost::make_optional(false, 0.0);
  OptReal ymax1 = boost::make_optional(false, 0.0);
  OptReal ymin2 = boost::make_optional(false, 0.0);
  OptReal ymax2 = boost::make_optional(false, 0.0);

  class Args {
   public:
    Args(int &argc, char **argv) :
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

  Args args(argc, argv);

  CQChartsInitData initData;

  for ( ; ! args.eof(); args.next()) {
    if (args.isOpt()) {
      QString arg = args.opt();

      if      (arg == "dark") {
#ifdef CQ_APP_H
        app.setDarkTheme(true);
#endif
      }

      // input data type
      else if (arg == "csv")
        initData.fileType = CQChartsFileType::CSV;
      else if (arg == "tsv")
        initData.fileType = CQChartsFileType::TSV;
      else if (arg == "json")
        initData.fileType = CQChartsFileType::JSON;
      else if (arg == "data")
        initData.fileType = CQChartsFileType::DATA;
      else if (arg == "expr")
        initData.fileType = CQChartsFileType::EXPR;

      // input data control
      else if (arg == "comment_header")
        initData.inputData.commentHeader = true;
      else if (arg == "first_line_header")
        initData.inputData.firstLineHeader = true;
      else if (arg == "first_column_header")
        initData.inputData.firstColumnHeader = true;
      else if (arg == "num_rows") {
        if (args.parseOpt(initData.inputData.numRows))
          initData.inputData.numRows = std::max(initData.inputData.numRows, 1);
      }
      else if (arg == "filter") {
        args.parseOpt(initData.inputData.filter);
      }
      else if (arg == "fold") {
        args.parseOpt(initData.inputData.fold);
      }

      // process data
      else if (arg == "process") {
        QString str;

        if (args.parseOpt(str)) {
          if (initData.process.length())
            initData.process += ";";

          initData.process += str;
        }
      }
      else if (arg == "process-add") {
        QString str;

        if (args.parseOpt(str)) {
          if (initData.processAdd.length())
            initData.processAdd += ";";

          initData.processAdd += str;
        }
      }
      // sort data
      else if (arg == "sort") {
        args.parseOpt(initData.inputData.sort);
      }

      // plot type
      else if (arg == "type") {
        args.parseOpt(initData.typeName);
      }

      // plot filter
      else if (arg == "where") {
        args.parseOpt(initData.filterStr);
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

              initData.setNameValue(name, value);
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
          initData.setNameValue("x", str);
      }
      else if (arg == "y") {
        QString str;

        if (args.parseOpt(str))
          initData.setNameValue("y", str);
      }
      else if (arg == "z") {
        QString str;

        if (args.parseOpt(str))
          initData.setNameValue("z", str);
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

            bool b = stringToBool(value, &ok);

            if (ok)
              initData.setNameBool(name, b);
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

            initData.setNameString(name, value);
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
              initData.setNameReal(name, r);
            else {
              errorMsg("Invalid -bool option '" + QString(realStr));
            }
          }
        }
      }
      else if (arg == "bivariate") {
        initData.setNameBool("bivariate", true);
      }
      else if (arg == "stacked") {
        initData.setNameBool("stacked", true);
      }
      else if (arg == "cumulative") {
        initData.setNameBool("cumulative", true);
      }
      else if (arg == "fillunder") {
        initData.setNameBool("fillUnder", true);
      }
      else if (arg == "impulse") {
        initData.setNameBool("impulse", true);
      }

      // column types
      else if (arg == "column_type") {
        args.parseOpt(initData.columnType);
      }

      // axis type
      else if (arg == "xintegral") {
        initData.xintegral = true;
      }
      else if (arg == "yintegral") {
        initData.yintegral = true;
      }

      // log scale
      else if (arg == "xlog") {
        initData.xlog = true;
      }
      else if (arg == "ylog") {
        initData.ylog = true;
      }

      // title
      else if (arg == "view_title") {
        args.parseOpt(viewTitle);
      }

      else if (arg == "plot_title") {
        args.parseOpt(initData.title);
      }

      // view properties
      else if (arg == "view_properties") {
        QString str;

        if (args.parseOpt(str)) {
          if (initData.viewProperties.length())
            initData.viewProperties += ",";

          initData.viewProperties += str;
        }
      }

      // plot properties
      else if (arg == "properties" || arg == "plot_properties") {
        QString str;

        if (args.parseOpt(str)) {
          if (initData.plotProperties.length())
            initData.plotProperties += ",";

          initData.plotProperties += str;
        }
      }

      // plot chaining (overlay, y1y2, and)
      else if (arg == "overlay") {
        overlay = true;
      }
      else if (arg == "x1x2") {
        x1x2 = true;
      }
      else if (arg == "y1y2") {
        y1y2 = true;
      }
      else if (arg == "horizontal") {
        horizontal = true;
      }
      else if (arg == "vertical") {
        vertical = true;
      }
      else if (arg == "and") {
        initDatas.push_back(initData);

        if (! overlay) {
          xmin1 = boost::make_optional(false, 0.0);
          xmax1 = boost::make_optional(false, 0.0);
          xmin2 = boost::make_optional(false, 0.0);
          xmax2 = boost::make_optional(false, 0.0);
          ymin1 = boost::make_optional(false, 0.0);
          ymax1 = boost::make_optional(false, 0.0);
          ymin2 = boost::make_optional(false, 0.0);
          ymax2 = boost::make_optional(false, 0.0);
        }

        initData = CQChartsInitData();
      }

      // data range
      else if (arg == "xmin" || arg == "xmin1") {
        args.parseOpt(xmin1);
      }
      else if (arg == "xmin2") {
        args.parseOpt(xmin2);
      }
      else if (arg == "xmax" || arg == "xmax1") {
        args.parseOpt(xmax1);
      }
      else if (arg == "xmax2") {
        args.parseOpt(xmax2);
      }
      else if (arg == "ymin" || arg == "ymin1") {
        args.parseOpt(ymin1);
      }
      else if (arg == "ymin2") {
        args.parseOpt(ymin2);
      }
      else if (arg == "ymax" || arg == "ymax1") {
        args.parseOpt(ymax1);
      }
      else if (arg == "ymax2") {
        args.parseOpt(ymax2);
      }

      // exec file
      else if (arg == "exec") {
        args.parseOpt(execFile);
      }
      // prompt loop
      else if (arg == "loop") {
        loop = true;
      }
      // ceil
      else if (arg == "ceil") {
        parserType = ParserType::CEIL;
      }
      // tcl
      else if (arg == "tcl") {
        parserType = ParserType::TCL;
      }
      // script
      else if (arg == "script") {
        parserType = ParserType::SCRIPT;
      }

      // view_width
      else if (arg == "view_width") {
        args.parseOpt(viewWidth);
      }
      // view_height
      else if (arg == "view_height") {
        args.parseOpt(viewHeight);
      }

      // print
      else if (arg == "print") {
        args.parseOpt(printFile);
      }

      // no gui
      else if (arg == "nogui" || arg == "no_gui") {
        gui = false;
      }

      // close app
      else if (arg == "close_app") {
        closeApp = true;
      }

      // exit
      else if (arg == "exit") {
        exit = true;
      }

      else {
        errorMsg("Invalid option '" + QString(args.arg()));
      }
    }
    else {
      initData.filenames.push_back(args.arg());
    }
  }

  initDatas.push_back(initData);

  //---

  CQChartsView::setSizeHint(QSize(viewWidth, viewHeight));

  //---

  CQChartsTest test;

  if (! gui)
    test.setGui(false);

  if (! gui || (printFile && exit))
    test.setShow(false);

  test.setParserType(parserType);

  int nd = initDatas.size();

  int nr = 1, nc = 1;

  if     (horizontal)
    nc = nd;
  else if (vertical)
    nr = nd;
  else {
    nr = std::max(int(sqrt(nd)), 1);
    nc = (nd + nr - 1)/nr;
  }

  double vr = CQChartsView::viewportRange();

  double dx = vr/nc;
  double dy = vr/nr;

  int i = 0;

  for (auto &initData : initDatas) {
    initData.viewTitle = viewTitle;
    initData.overlay   = overlay;
    initData.x1x2      = x1x2;
    initData.y1y2      = y1y2;
    initData.nr        = nr;
    initData.nc        = nc;
    initData.dx        = dx;
    initData.dy        = dy;

    //---

    if (xmin1) initData.xmin = xmin1;
    if (xmax1) initData.xmax = xmax1;

    if      (initData.x1x2) {
      if      (i == 0) {
        if (xmin1) initData.xmin = xmin1;
        if (xmax1) initData.xmax = xmax1;
      }
      else if (i >= 1) {
        if (xmin2) initData.xmin = xmin2;
        if (xmax2) initData.xmax = xmax2;
      }
    }
    else if (initData.y1y2) {
      if      (i == 0) {
        if (ymin1) initData.ymin = ymin1;
        if (ymax1) initData.ymax = ymax1;
      }
      else if (i >= 1) {
        if (ymin2) initData.ymin = ymin2;
        if (ymax2) initData.ymax = ymax2;
      }
    }
    else {
      if (ymin1) initData.ymin = ymin1;
      if (ymax1) initData.ymax = ymax1;
    }

    //---

    if (! test.initPlot(initData))
      continue;

    ++i;
  }

  //---

  if (execFile.length()) {
    test.exec(execFile);
  }

  //---

  if (test.isShow())
    test.show();

  if (test.window()) {
    test.window()->raise();
  }

  if (printFile)
    test.print(*printFile);

  if (closeApp)
    test.close();

  //---

  if (! exit) {
    if (! loop)
      app.exec();
    else
      test.loop();
  }

  return 0;
}

//-----

CQChartsTest::
CQChartsTest() :
 CQAppWindow()
{
  charts_ = new CQCharts;

  charts_->init();

  //---

  addMenus();

  //---

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(centralWidget(), 0, 0);

  //---

  viewTab_ = CQUtil::makeWidget<QTabWidget>("viewTab");

  layout->addWidget(viewTab_);

  connect(viewTab_, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

  //---

  QTabWidget *controlTab = CQUtil::makeWidget<QTabWidget>("controlTab");

  layout->addWidget(controlTab);

  //------

  QFrame *expressionFrame = CQUtil::makeWidget<QFrame>("expressionFrame");

  controlTab->addTab(expressionFrame, "Expression");

  QVBoxLayout *expressionFrameLayout = new QVBoxLayout(expressionFrame);

  QHBoxLayout *exprFrameLayout = new QHBoxLayout;

  exprEdit_ = CQUtil::makeWidget<CQHistoryLineEdit>("exprEdit");

  exprEdit_->setToolTip("+<expr> OR -<column> OR =<column>:<expr>\n"
                        "Use: @<number> as shorthand for column(<number>)\n"
                        "Functions: column, row, cell, setColumn, setRow, setCell\n"
                        " header, setHeader, type, setType, map, bucket, norm, key, rand");

  connect(exprEdit_, SIGNAL(exec(const QString &)), this, SLOT(exprSlot()));

  exprCombo_ = CQUtil::makeWidget<QComboBox>("exprCombo");

  exprCombo_->addItems(QStringList() << "Add" << "Remove" << "Modify");

  exprColumn_ = CQUtil::makeWidget<QLineEdit>("exprColumn");

  exprFrameLayout->addWidget(exprCombo_ );
  exprFrameLayout->addWidget(exprEdit_  );
  exprFrameLayout->addWidget(new QLabel("Column"));
  exprFrameLayout->addWidget(exprColumn_);

  expressionFrameLayout->addLayout(exprFrameLayout);
  expressionFrameLayout->addStretch(1);

  //------

  QFrame *foldFrame = CQUtil::makeWidget<QFrame>("foldFrame");

  controlTab->addTab(foldFrame, "Fold");

  QGridLayout *foldFrameLayout = new QGridLayout(foldFrame);

  int foldRow = 0;

  foldEdit_ = addLineEdit(foldFrameLayout, foldRow, "Fold", "fold");

  foldFrameLayout->setRowStretch(foldRow, 1); ++foldRow;

  connect(foldEdit_, SIGNAL(returnPressed()), this, SLOT(foldSlot()));

  //------

  QFrame *columnDataFrame = CQUtil::makeWidget<QFrame>("columnDataFrame");

  controlTab->addTab(columnDataFrame, "Column Data");

  QGridLayout *columnDataFrameLayout = new QGridLayout(columnDataFrame);

  int columnDataRow = 0;

  columnNumEdit_  = addLineEdit(columnDataFrameLayout, columnDataRow, "Number", "number");
  columnNameEdit_ = addLineEdit(columnDataFrameLayout, columnDataRow, "Name"  , "name"  );
  columnTypeEdit_ = addLineEdit(columnDataFrameLayout, columnDataRow, "Type"  , "type"  );

  columnDataFrameLayout->setRowStretch(columnDataRow, 1); ++columnDataRow;

  QPushButton *typeSetButton = CQUtil::makeWidget<QPushButton>("typeSet");

  typeSetButton->setText("Set");

  connect(typeSetButton, SIGNAL(clicked()), this, SLOT(typeSetSlot()));

  columnDataFrameLayout->addWidget(typeSetButton, columnDataRow, 0);

  //---

  cmds_ = new CQChartsCmds(charts_);

  connect(cmds_, SIGNAL(titleChanged(int, const QString &)),
          this, SLOT(titleChanged(int, const QString &)));

  connect(cmds_, SIGNAL(updateModelDetails(int)), this, SLOT(updateModelDetails(int)));
  connect(cmds_, SIGNAL(updateModel(int)), this, SLOT(updateModel(int)));

  connect(cmds_, SIGNAL(windowCreated(CQChartsWindow *)),
          this, SLOT(windowCreated(CQChartsWindow *)));
  connect(cmds_, SIGNAL(plotCreated(CQChartsPlot *)),
          this, SLOT(plotCreated(CQChartsPlot *)));

  connect(cmds_, SIGNAL(modelDataAdded(int)), this, SLOT(modelDataAdded(int)));
}

CQChartsTest::
~CQChartsTest()
{
  for (auto &p : viewWidgetDatas_)
    delete p.second;

  delete charts_;
  delete loader_;
  delete cmds_;
}

//------

void
CQChartsTest::
addMenus()
{
  QMenuBar *menuBar = addMenuBar();

  QMenu *fileMenu = menuBar->addMenu("&File");
  QMenu *plotMenu = menuBar->addMenu("&Plot");
  QMenu *helpMenu = menuBar->addMenu("&Help");

  QAction *loadAction   = new QAction("Load"  , menuBar);
  QAction *createAction = new QAction("Create", menuBar);
  QAction *helpAction   = new QAction("Help"  , menuBar);

  connect(loadAction  , SIGNAL(triggered()), this, SLOT(loadSlot()));
  connect(createAction, SIGNAL(triggered()), this, SLOT(createSlot()));

  fileMenu->addAction(loadAction);
  plotMenu->addAction(createAction);
  helpMenu->addAction(helpAction);
}

//------

void
CQChartsTest::
addViewWidgets(CQChartsModelData *modelData)
{
  if (! isGui())
    return;

  CQChartsViewWidgetData *viewWidgetData = new CQChartsViewWidgetData;

  viewWidgetData->ind = modelData->ind();

  viewWidgetDatas_[viewWidgetData->ind] = viewWidgetData;

  //---

  QTabWidget *tableTab = CQUtil::makeWidget<QTabWidget>("tableTab");

  int tabInd = viewTab_->addTab(tableTab, QString("Model %1").arg(viewWidgetData->ind));

  viewTab_->setCurrentIndex(viewTab_->count() - 1);

  viewWidgetData->tabInd = tabInd;

  //---

  QFrame *viewFrame = CQUtil::makeWidget<QFrame>("view");

  QVBoxLayout *viewLayout = new QVBoxLayout(viewFrame);

  tableTab->addTab(viewFrame, "Model");

  //---

  QFrame *detailsFrame = CQUtil::makeWidget<QFrame>("details");

  QVBoxLayout *detailsLayout = new QVBoxLayout(detailsFrame);

  tableTab->addTab(detailsFrame, "Details");

  //---

  QLineEdit *filterEdit = CQUtil::makeWidget<QLineEdit>("filter");

  viewLayout->addWidget(filterEdit);

  connect(filterEdit, SIGNAL(returnPressed()), this, SLOT(filterSlot()));

  viewWidgetData->filterEdit = filterEdit;

  //---

  QStackedWidget *stack = CQUtil::makeWidget<QStackedWidget>("stack");

  viewLayout->addWidget(stack);

  viewWidgetData->stack = stack;

  //---

  CQChartsTree *tree = new CQChartsTree(charts_);

  stack->addWidget(tree);

  //---

  CQChartsTable *table = new CQChartsTable(charts_);

  stack->addWidget(table);

  connect(table, SIGNAL(columnClicked(int)), this, SLOT(tableColumnClicked(int)));

  viewWidgetData->table = table;

  //------

  QTextEdit *detailsText = CQUtil::makeWidget<QTextEdit>("detailsText");

  detailsText->setReadOnly(true);

  detailsLayout->addWidget(detailsText);

  viewWidgetData->detailsText = detailsText;
}

void
CQChartsTest::
currentTabChanged(int ind)
{
  for (auto &p : viewWidgetDatas_) {
    CQChartsViewWidgetData *viewWidgetDatas = p.second;

    if (viewWidgetDatas->tabInd == ind)
      cmds_->setCurrentInd(viewWidgetDatas->ind);
  }
}

//------

bool
CQChartsTest::
initPlot(const CQChartsInitData &initData)
{
  CQChartsView *view = cmds_->currentView();

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

  CQChartsModelData *modelData = cmds_->currentModelData();

  if (! modelData)
    return false;

  //---

  if (initData.process.length()) {
    ModelP model = modelData->model();

    QStringList strs = initData.process.split(";", QString::SkipEmptyParts);

    for (int i = 0; i < strs.size(); ++i)
      cmds_->processExpression(model, strs[i]);
  }

  if (initData.processAdd.length()) {
    ModelP model = modelData->model();

    QStringList strs = initData.processAdd.split(";", QString::SkipEmptyParts);

    for (int i = 0; i < strs.size(); ++i)
      cmds_->processAddExpression(model, strs[i]);
  }

  //---

  CQChartsPlot *plot = initPlotView(modelData, initData, i, bbox);

  if (! plot)
    return false;

  if (filename != "")
    plot->setFileName(filename);

  //---

  //if (! view->numPlots())
  //  rootPlot_ = plot;

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

  //plots_.push_back(plot);

  return true;
}

//------

void
CQChartsTest::
loadSlot()
{
  if (! loader_) {
    loader_ = new CQChartsLoader(charts_);

    connect(loader_, SIGNAL(loadFile(const QString &, const QString &)),
            this, SLOT(loadFileSlot(const QString &, const QString &)));
  }

  loader_->show();
}

bool
CQChartsTest::
loadFileSlot(const QString &type, const QString &filename)
{
  CQChartsFileType fileType = stringToFileType(type);

  if (fileType == CQChartsFileType::NONE) {
    errorMsg("Bad type specified '" + type + "'");
    return false;
  }

  //---

  CQChartsInputData inputData;

  inputData.commentHeader     = loader_->isCommentHeader();
  inputData.firstLineHeader   = loader_->isFirstLineHeader();
  inputData.firstColumnHeader = loader_->isFirstColumnHeader();
  inputData.numRows           = loader_->numRows();

  return cmds_->loadFileModel(filename, fileType, inputData);
}

//------

void
CQChartsTest::
createSlot()
{
  CQChartsModelData *modelData = cmds_->currentModelData();

  if (! modelData)
    return;

  ModelP model = modelData->model();

  //---

  CQChartsPlotDlg *dlg = new CQChartsPlotDlg(charts_, model);

  if (modelData->selectionModel())
    dlg->setSelectionModel(modelData->selectionModel());

  connect(dlg, SIGNAL(plotCreated(CQChartsPlot *)),
          this, SLOT(plotDialogCreatedSlot(CQChartsPlot *)));

  if (! dlg->exec())
    return;

  //---

  delete dlg;
}

void
CQChartsTest::
plotDialogCreatedSlot(CQChartsPlot *plot)
{
  connect(plot, SIGNAL(objPressed(CQChartsPlotObj *)),
          this, SLOT(plotObjPressedSlot(CQChartsPlotObj *)));
}

//------

void
CQChartsTest::
plotObjPressedSlot(CQChartsPlotObj *obj)
{
  QString id = obj->id();

  if (id.length())
    errorMsg(id);
}

//------

void
CQChartsTest::
filterSlot()
{
  QLineEdit *filterEdit = qobject_cast<QLineEdit *>(sender());

  for (auto &p : viewWidgetDatas_) {
    CQChartsViewWidgetData *viewWidgetData = p.second;

    if (viewWidgetData->filterEdit == filterEdit) {
      if (viewWidgetData->stack->currentIndex() == 0) {
        if (viewWidgetData->tree)
          viewWidgetData->tree ->setFilter(filterEdit->text());
      }
      else {
        if (viewWidgetData->table)
          viewWidgetData->table->setFilter(filterEdit->text());
      }
    }
  }
}

//------

void
CQChartsTest::
exprSlot()
{
  QString expr = exprEdit_->text().simplified();

  if (! expr.length())
    return;

  //---

  CQChartsModelData *modelData = cmds_->currentModelData();

  if (! modelData)
    return;

  CQExprModel::Function function { CQExprModel::Function::EVAL };

  switch (exprCombo_->currentIndex()) {
    case 0: function = CQExprModel::Function::ADD   ; break;
    case 1: function = CQExprModel::Function::DELETE; break;
    case 2: function = CQExprModel::Function::ASSIGN; break;
    default:                                          break;
  }

  ModelP model = modelData->model();

  bool ok;

  int column = exprColumn_->text().toInt(&ok);

  cmds_->processExpression(model, function, column, expr);

  exprEdit_->setText("");
}

//------

void
CQChartsTest::
foldSlot()
{
  CQChartsModelData *modelData = cmds_->currentModelData();

  if (! modelData)
    return;

  QString text = foldEdit_->text();

  cmds_->foldModel(modelData, text);

  updateModel(modelData);
}

//------

void
CQChartsTest::
tableColumnClicked(int column)
{
  CQChartsTable *table = qobject_cast<CQChartsTable *>(sender());

  ModelP model = table->model();

  columnNumEdit_->setText(QString("%1").arg(column));

  //---

  columnNameEdit_->setText(model->headerData(column, Qt::Horizontal).toString());

  //---

  QString typeStr;

  if (CQChartsUtil::columnTypeStr(charts_, model.data(), column, typeStr))
    columnTypeEdit_->setText(typeStr);
}

//------

void
CQChartsTest::
typeSetSlot()
{
  CQChartsModelData *modelData = cmds_->currentModelData();

  if (! modelData)
    return;

  ModelP model = modelData->model();

  //---

  QString numStr = columnNumEdit_->text();

  bool ok;

  int column = numStr.toInt(&ok);

  if (! ok) {
    errorMsg("Invalid column number '" + numStr + "'");
    return;
  }

  //--

  QString nameStr = columnNameEdit_->text();

  if (nameStr.length())
    model->setHeaderData(column, Qt::Horizontal, nameStr, Qt::DisplayRole);

  //---

  QString typeStr = columnTypeEdit_->text();

  if (! CQChartsUtil::setColumnTypeStr(charts_, model.data(), column, typeStr)) {
    errorMsg("Invalid type '" + typeStr + "'");
    return;
  }

  //---

  if (isGui()) {
    CQChartsViewWidgetData *viewWidgetData = this->viewWidgetData(modelData->ind());
    assert(viewWidgetData);

    if (viewWidgetData->stack->currentIndex() == 0) {
      if (viewWidgetData->tree)
        viewWidgetData->tree->update();
    }
    else {
      if (viewWidgetData->table)
        viewWidgetData->table->update();
    }
  }
}

//------

CQChartsViewWidgetData *
CQChartsTest::
viewWidgetData(int ind) const
{
  auto p = viewWidgetDatas_.find(ind);

  if (p == viewWidgetDatas_.end())
    return nullptr;

  return (*p).second;
}

//------

CQChartsPlot *
CQChartsTest::
initPlotView(const CQChartsModelData *modelData, const CQChartsInitData &initData, int i,
             const CQChartsGeom::BBox &bbox)
{
  cmds_->setColumnFormats(modelData->model(), initData.columnType);

  updateModelDetails(modelData);

  //---

  QString typeName = cmds_->fixTypeName(initData.typeName);

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

  ModelP model = modelData->currentModel();

  // create plot from init (argument) data
  if (initData.filterStr.length()) {
    CQSortModel *sortModel = new CQSortModel(model.data());

    ModelP sortModelP(sortModel);

    sortModel->setFilter(initData.filterStr);

    plot = cmds_->createPlot(sortModelP, modelData->selectionModel(), type,
                             initData.nameValueData, reuse, bbox);
  }
  else {
    plot = cmds_->createPlot(model, modelData->selectionModel(), type,
                             initData.nameValueData, reuse, bbox);
  }

  assert(plot);

  if (initData.viewProperties != "")
    plot->view()->setProperties(initData.viewProperties);

  //---

  // init plot
  if (initData.overlay) {
    if (i > 0) {
      plot->setBackground    (false);
      plot->setDataBackground(false);
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

  if (initData.plotProperties != "")
    cmds_->setPlotProperties(plot, initData.plotProperties);

  return plot;
}

//------

void
CQChartsTest::
setParserType(const ParserType &type)
{
  cmds_->setParserType(type);
}

const CQChartsTest::ParserType &
CQChartsTest::
parserType() const
{
  return cmds_->parserType();
}

void
CQChartsTest::
titleChanged(int ind, const QString &title)
{
  if (isGui()) {
    CQChartsViewWidgetData *viewWidgetData = this->viewWidgetData(ind);
    assert(viewWidgetData);

    if (viewWidgetData->stack->currentIndex() == 0) {
      if (viewWidgetData->tree)
        viewWidgetData->tree->setWindowTitle(title);
    }
    else {
      if (viewWidgetData->table)
        viewWidgetData->table->setWindowTitle(title);
    }

    viewTab()->setTabText(viewWidgetData->tabInd, title);
  }
}

void
CQChartsTest::
modelDataAdded(int ind)
{
  CQChartsModelData *modelData = cmds_->getModelData(ind);
  assert(modelData);

  addViewWidgets(modelData);
}

void
CQChartsTest::
updateModel(int ind)
{
  CQChartsModelData *modelData = cmds_->getModelData(ind);
  assert(modelData);

  updateModel(modelData);
}

void
CQChartsTest::
updateModel(CQChartsModelData *modelData)
{
  if (isGui()) {
    CQChartsViewWidgetData *viewWidgetData = this->viewWidgetData(modelData->ind());
    assert(viewWidgetData);

    if (! modelData->foldedModels().empty()) {
      if (viewWidgetData->table)
        viewWidgetData->table->setModel(ModelP());

      if (viewWidgetData->tree)
        viewWidgetData->tree->setModel(modelData->foldProxyModel());

      viewWidgetData->stack->setCurrentIndex(0);
    }
    else {
      if (modelData->isHierarchical()) {
        if (viewWidgetData->tree) {
          viewWidgetData->tree->setModel(modelData->model());

          modelData->setSelectionModel(viewWidgetData->tree->selectionModel());
        }
        else
          modelData->setSelectionModel(nullptr);

        viewWidgetData->stack->setCurrentIndex(0);
      }
      else {
        if (viewWidgetData->table) {
          viewWidgetData->table->setModel(modelData->model());

          modelData->setSelectionModel(viewWidgetData->table->selectionModel());
        }
        else
          modelData->setSelectionModel(nullptr);

        viewWidgetData->stack->setCurrentIndex(1);
      }
    }

    updateModelDetails(modelData);
  }
}

void
CQChartsTest::
updateModelDetails(int ind)
{
  CQChartsModelData *modelData = cmds_->getModelData(ind);
  assert(modelData);

  updateModelDetails(modelData);
}

void
CQChartsTest::
updateModelDetails(const CQChartsModelData *modelData)
{
  if (isGui()) {
    CQChartsViewWidgetData *viewWidgetData = this->viewWidgetData(modelData->ind());
    assert(viewWidgetData);

    CQChartsModelDetails details;

    if (viewWidgetData->stack->currentIndex() == 0) {
      if (viewWidgetData->tree)
        viewWidgetData->tree->calcDetails(details);
    }
    else {
      if (viewWidgetData->table)
        viewWidgetData->table->calcDetails(details);
    }

    //---

    QString text = "<b></b>";

    text += "<table padding=\"4\">";
    text += QString("<tr><td>Columns</td><td>%1</td></tr>").arg(details.numColumns());
    text += QString("<tr><td>Rows</td><td>%1</td></tr>").arg(details.numRows());
    text += "</table>";

    text += "<br>";

    text += "<table padding=\"4\">";
    text += "<tr><th>Column</th><th>Type</th><th>Min</th><th>Max</th><th>Monotonic</th></tr>";

    for (int i = 0; i < details.numColumns(); ++i) {
      const CQChartsModelColumnDetails &columnDetails = details.columnDetails(i);

      text += "<tr>";

      text += QString("<td>%1</td><td>%2</td><td>%3</td><td>%4</td>").
               arg(i + 1).
               arg(columnDetails.typeName()).
               arg(columnDetails.dataName(columnDetails.minValue()).toString()).
               arg(columnDetails.dataName(columnDetails.maxValue()).toString());

      if (columnDetails.isMonotonic())
        text += QString("<td>%1</td>").
          arg(columnDetails.isIncreasing() ? "Increasing" : "Decreasing");
      else
        text += QString("<td></td>");

      text += "</tr>";
    }

    text += "</table>";

    viewWidgetData->detailsText->setHtml(text);
  }
}

void
CQChartsTest::
windowCreated(CQChartsWindow *window)
{
  if (isShow())
    window->show();
}

void
CQChartsTest::
plotCreated(CQChartsPlot *plot)
{
  connect(plot, SIGNAL(objPressed(CQChartsPlotObj *)),
          this, SLOT(plotObjPressedSlot(CQChartsPlotObj *)));
}

//------

QLineEdit *
CQChartsTest::
addLineEdit(QGridLayout *grid, int &row, const QString &name, const QString &objName) const
{
  QLabel    *label = CQUtil::makeWidget<QLabel   >(objName + "Label");
  QLineEdit *edit  = CQUtil::makeWidget<QLineEdit>(objName + "Edit" );

  label->setText(name);

  grid->addWidget(label, row, 0);
  grid->addWidget(edit , row, 1);

  ++row;

  return edit;
}

//------

QSize
CQChartsTest::
sizeHint() const
{
  return QSize(1024, 1024);
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

    while (! cmds_->isCompleteLine(line)) {
      if (in.atEnd())
        break;

      QString line1 = in.readLine();

      if (parserType() == ParserType::TCL)
        line += line1;
      else
        line += "\n" + line1;
    }

    cmds_->parseLine(line);
  }

  file.close();

  return true;
}

//------

void
CQChartsTest::
print(const QString &filename)
{
  if (! cmds_->view())
    return;

  if (! isShow()) {
    cmds_->view()->resize(cmds_->view()->sizeHint());

    cmds_->view()->resizeEvent(0);
  }

  cmds_->view()->printFile(filename);
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
  CQChartsReadLine *readLine = new CQChartsReadLine(this);

  readLine->enableTimeoutHook(1);

  for (;;) {
    readLine->setPrompt("> ");

    QString line = readLine->readLine().c_str();

    while (! cmds_->isCompleteLine(line)) {
      readLine->setPrompt("+> ");

      QString line1 = readLine->readLine().c_str();

      line += "\n" + line1;
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
