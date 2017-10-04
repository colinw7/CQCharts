#include <CQChartsTest.h>
#include <CQCharts.h>
#include <CQChartsTable.h>
#include <CQChartsTree.h>
#include <CQChartsCsv.h>
#include <CQChartsTsv.h>
#include <CQChartsJson.h>
#include <CQChartsData.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsXYPlot.h>

#include <CQChartsLoader.h>
#include <CQChartsPlotDlg.h>

#ifdef CQ_APP_H
#include <CQApp.h>
#else
#include <QApplication>
#endif

#include <CQUtil.h>

#include <QSortFilterProxyModel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QSplitter>
#include <QStackedWidget>
#include <QTabWidget>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

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

int
main(int argc, char **argv)
{
  qInstallMessageHandler(myMessageOutput);

#ifdef CQ_APP_H
  CQApp app(argc, argv);
#else
  QApplication app(argc, argv);
#endif

  CQUtil::initProperties();

  std::vector<CQChartsTest::InitData> initDatas;

  bool overlay = false;
  bool y1y2    = false;

  CQChartsTest::OptReal xmin1 = boost::make_optional(false, 0.0);
  CQChartsTest::OptReal xmax1 = boost::make_optional(false, 0.0);
  CQChartsTest::OptReal xmin2 = boost::make_optional(false, 0.0);
  CQChartsTest::OptReal xmax2 = boost::make_optional(false, 0.0);
  CQChartsTest::OptReal ymin1 = boost::make_optional(false, 0.0);
  CQChartsTest::OptReal ymax1 = boost::make_optional(false, 0.0);
  CQChartsTest::OptReal ymin2 = boost::make_optional(false, 0.0);
  CQChartsTest::OptReal ymax2 = boost::make_optional(false, 0.0);

  CQChartsTest::InitData initData;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      std::string arg = &argv[i][1];

      if      (arg == "csv")
        initData.csv  = true;
      else if (arg == "tsv")
        initData.tsv  = true;
      else if (arg == "json")
        initData.json = true;
      else if (arg == "data")
        initData.data = true;
      else if (arg == "plot") {
        ++i;

        if (i < argc)
          initData.plot = argv[i];
      }
      else if (arg == "column") {
        ++i;

        if (i < argc) {
          QString nameValue = argv[i];

          auto pos = nameValue.indexOf('=');

          if (pos >= 0) {
            auto name  = nameValue.mid(0, pos).simplified();
            auto value = nameValue.mid(pos + 1).simplified();

            initData.setNameValue(name, value);
          }
          else {
            std::cerr << "Invalid -column option '" << argv[i] << "'" << std::endl;
          }
        }
      }
      else if (arg == "x") {
        ++i;

        if (i < argc)
          initData.setNameValue("x", argv[i]);
      }
      else if (arg == "y") {
        ++i;

        if (i < argc)
          initData.setNameValue("y", argv[i]);
      }
      else if (arg == "z") {
        ++i;

        if (i < argc)
          initData.setNameValue("z", argv[i]);
      }
      else if (arg == "bool") {
        ++i;

        if (i < argc) {
          QString nameValue = argv[i];

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
            std::cerr << "Invalid -bool option '" << argv[i] << "'" << std::endl;
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
      else if (arg == "column_type") {
        ++i;

        if (i < argc)
          initData.columnType = argv[i];
      }
      else if (arg == "comment_header") {
        initData.commentHeader = true;
      }
      else if (arg == "first_line_header") {
        initData.firstLineHeader = true;
      }
      else if (arg == "xintegral") {
        initData.xintegral = true;
      }
      else if (arg == "yintegral") {
        initData.yintegral = true;
      }
      else if (arg == "plot_title") {
        ++i;

        if (i < argc)
          initData.title = argv[i];
      }
      else if (arg == "properties") {
        ++i;

        if (i < argc)
          initData.properties = argv[i];
      }
      else if (arg == "overlay") {
        overlay = true;
      }
      else if (arg == "y1y2") {
        y1y2 = true;
      }
      else if (arg == "and") {
        initDatas.push_back(initData);

        xmin1 = boost::make_optional(false, 0.0);
        xmax1 = boost::make_optional(false, 0.0);
        xmin2 = boost::make_optional(false, 0.0);
        xmax2 = boost::make_optional(false, 0.0);
        ymin1 = boost::make_optional(false, 0.0);
        ymax1 = boost::make_optional(false, 0.0);
        ymin2 = boost::make_optional(false, 0.0);
        ymax2 = boost::make_optional(false, 0.0);

        initData = CQChartsTest::InitData();
      }
      else if (arg == "xmin" || arg == "xmin1") {
        ++i;

        if (i < argc)
          xmin1 = std::stod(argv[i]);
      }
      else if (arg == "xmin2") {
        ++i;

        if (i < argc)
          xmin2 = std::stod(argv[i]);
      }
      else if (arg == "xmax" || arg == "xmax1") {
        ++i;

        if (i < argc)
          xmax1 = std::stod(argv[i]);
      }
      else if (arg == "xmax2") {
        ++i;

        if (i < argc)
          xmax2 = std::stod(argv[i]);
      }
      else if (arg == "ymin" || arg == "ymin1") {
        ++i;

        if (i < argc)
          ymin1 = std::stod(argv[i]);
      }
      else if (arg == "ymin2") {
        ++i;

        if (i < argc)
          ymin2 = std::stod(argv[i]);
      }
      else if (arg == "ymax" || arg == "ymax1") {
        ++i;

        if (i < argc)
          ymax1 = std::stod(argv[i]);
      }
      else if (arg == "ymax2") {
        ++i;

        if (i < argc)
          ymax2 = std::stod(argv[i]);
      }
      else {
        std::cerr << "Invalid option '" << argv[i] << "'" << std::endl;
      }
    }
    else {
      initData.filenames.push_back(argv[i]);
    }
  }

  initDatas.push_back(initData);

  //---

  CQChartsTest test;

  int nd = initDatas.size();

  int nr = std::max(int(sqrt(nd)), 1);
  int nc = (nd + nr - 1)/nr;

  double dx = 1000.0/nc;
  double dy = 1000.0/nr;

  int i = 0;

  for (auto &initData : initDatas) {
    initData.overlay = overlay;
    initData.y1y2    = y1y2;
    initData.nr      = nr;
    initData.nc      = nc;
    initData.dx      = dx;
    initData.dy      = dy;

    //---

    if (xmin1) initData.xmin = xmin1;
    if (xmax1) initData.xmax = xmax1;

    if (initData.overlay || initData.y1y2) {
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

  test.show();

  if (test.view())
    test.view()->raise();

  //---

  app.exec();

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

  QHBoxLayout *layout = CQUtil::makeLayout<QHBoxLayout>(centralWidget(), 0, 0);

  QSplitter *splitter = CQUtil::makeWidget<QSplitter>("splitter");

  layout->addWidget(splitter);

  //---

  QFrame *plotFrame = CQUtil::makeWidget<QFrame>("plotFrame");

  QVBoxLayout *plotLayout = new QVBoxLayout(plotFrame);
  plotLayout->setMargin(0); plotLayout->setSpacing(2);

  splitter->addWidget(plotFrame);

  //---

  // plots (one per tab)
  plotTab_ = new QTabWidget;

  plotTab_->setObjectName("plotTab");

  plotLayout->addWidget(plotTab_);

  plotTab_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  addPlotTab(plotTab_, "pie"       );
  addPlotTab(plotTab_, "xy"        );
  addPlotTab(plotTab_, "scatter"   );
  addPlotTab(plotTab_, "sunburst"  );
  addPlotTab(plotTab_, "barchart"  );
  addPlotTab(plotTab_, "box"       );
  addPlotTab(plotTab_, "parallel"  );
  addPlotTab(plotTab_, "geometry"  );
  addPlotTab(plotTab_, "delaunay"  );
  addPlotTab(plotTab_, "adjacency" );
  addPlotTab(plotTab_, "bubble"    );
  addPlotTab(plotTab_, "hierbubble");
  addPlotTab(plotTab_, "treemap"   );

  //---

  filterEdit_ = CQUtil::makeWidget<QLineEdit>("filter");

  plotLayout->addWidget(filterEdit_);

  connect(filterEdit_, SIGNAL(returnPressed()), this, SLOT(filterSlot()));

  //---

  // table/tree widgets
  stack_ = new QStackedWidget;

  stack_->setObjectName("stack");

  table_ = new CQChartsTable;
  tree_  = new CQChartsTree;

  stack_->addWidget(table_);
  stack_->addWidget(tree_);

  plotLayout->addWidget(stack_);

  connect(table_, SIGNAL(columnClicked(int)), this, SLOT(tableColumnClicked(int)));

  //---

  typeGroup_ = new QGroupBox;

  typeGroup_->setObjectName("typeGroup");
  typeGroup_->setTitle("Column Type");

  plotLayout->addWidget(typeGroup_);

  QVBoxLayout *typeGroupLayout = new QVBoxLayout(typeGroup_);

  QFrame *columnFrame = new QFrame;

  columnFrame->setObjectName("columnFrame");

  typeGroupLayout->addWidget(columnFrame);

  QGridLayout *columnLayout = new QGridLayout(columnFrame);

  int row = 0;

  columnTypeEdit_ = addLineEdit(columnLayout, row, "Type", "type");

  QPushButton *typeOKButton = new QPushButton("Set");

  typeOKButton->setObjectName("typeOK");

  connect(typeOKButton, SIGNAL(clicked()), this, SLOT(typeOKSlot()));

  columnLayout->addWidget(typeOKButton);
}

CQChartsTest::
~CQChartsTest()
{
  delete charts_;
  delete csv_;
  delete tsv_;
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

bool
CQChartsTest::
initPlot(const InitData &initData)
{
  int i = plots_.size();

  //---

  int r = i / initData.nc;
  int c = i % initData.nc;

  setId(QString("%1").arg(i + 1));

  if (initData.overlay || initData.y1y2)
    setBBox(CBBox2D(0, 0, 1000, 1000));
  else {
    double x1 =  c     *initData.dx;
    double x2 = (c + 1)*initData.dx;
    double y1 =  r     *initData.dy;
    double y2 = (r + 1)*initData.dy;

    setBBox(CBBox2D(x1, 1000.0 - y2, x2, 1000.0 - y1));
  }

  if (initData.filenames.size() > 0) {
    if      (initData.csv)
      loadCsv(initData.filenames[0], initData.commentHeader, initData.firstLineHeader);
    else if (initData.tsv)
      loadTsv(initData.filenames[0], initData.commentHeader, initData.firstLineHeader);
    else if (initData.json)
      loadJson(initData.filenames[0]);
    else if (initData.data)
      loadData(initData.filenames[0], initData.commentHeader, initData.firstLineHeader);
    else
      std::cerr << "No plot type specified" << std::endl;
  }

  CQChartsPlot *plot = init(initData, i);

  if (! plot)
    return false;

  //---

  if (plots_.empty())
    rootPlot_ = plot;

  if      (initData.overlay) {
    plot->setOverlay(true);

    if      (i == 0) {
    }
    else if (i >= 1) {
      CQChartsPlot *prevPlot = plots_.back();

      plot    ->setPrevPlot(prevPlot);
      prevPlot->setNextPlot(plot);

      plot->setDataRange(rootPlot_->dataRange());

      rootPlot_->applyDataRange();
    }
  }
  else if (initData.y1y2) {
    if      (i == 0) {
    }
    else if (i >= 1) {
      CQChartsPlot *prevPlot = plots_.back();

      plot    ->setPrevPlot(prevPlot);
      prevPlot->setNextPlot(plot);

      plot->xAxis()->setVisible(false);
      plot->yAxis()->setSide(CQChartsAxis::Side::TOP_RIGHT);

      plot->key()->setVisible(false);
    }
  }

  //---

  plots_.push_back(plot);

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

void
CQChartsTest::
loadFileSlot(const QString &type, const QString &filename)
{
  bool commentHeader   = loader_->isCommentHeader();
  bool firstLineHeader = loader_->isFirstLineHeader();

  if      (type == "CSV")
    loadCsv(filename, commentHeader, firstLineHeader);
  else if (type == "TSV")
    loadTsv(filename, commentHeader, firstLineHeader);
  else if (type == "Json")
    loadJson(filename);
  else if (type == "Data")
    loadData(filename, commentHeader, firstLineHeader);
  else
    std::cerr << "Base type specified '" << type.toStdString() << "'" << std::endl;
}

//------

void
CQChartsTest::
createSlot()
{
  if (! model_)
    return;

  //---

  CQChartsView *view = getView(/*reuse*/false);

  CQChartsPlotDlg *dlg = new CQChartsPlotDlg(view, model_);

  if (! dlg->exec())
    return;

  //---

  // add plot to view and show
  CQChartsPlot *plot = dlg->plot();

  plot->setId(QString("%1%2").arg(plot->typeName()).arg(id_));

  view->addPlot(plot, bbox_);

  view->show();

  //---

  delete dlg;
}

//------

void
CQChartsTest::
filterSlot()
{
  if (stack_->currentIndex() == 0)
    table_->setFilter(filterEdit_->text());
}

//------

void
CQChartsTest::
tableColumnClicked(int column)
{
  tableColumn_ = column;

  QVariant var = model_->headerData(tableColumn_, Qt::Horizontal, CQCharts::Role::ColumnType);

  QString type = var.toString();

  if (type.length())
    columnTypeEdit_->setText(type);
}

//------

void
CQChartsTest::
typeOKSlot()
{
  QString type = columnTypeEdit_->text();

  model_->setHeaderData(tableColumn_, Qt::Horizontal, type, CQCharts::Role::ColumnType);
}

//------

void
CQChartsTest::
addPlotTab(QTabWidget *plotTab, const QString &typeName)
{
  CQChartsPlotType *type = charts_->plotType(typeName);
  assert(type);

  //

  QFrame *frame = new QFrame;

  frame->setObjectName("frame");

  QVBoxLayout *frameLayout = new QVBoxLayout(frame);

  //---

  QGridLayout *editLayout = new QGridLayout;

  frameLayout->addLayout(editLayout);

  int row = 0;

  PlotData &plotData = typePlotData_[type->name()];

  addParameterEdits(type, plotData, editLayout, row);

  editLayout->setRowStretch(row, 1);

  //---

  QHBoxLayout *buttonLayout = new QHBoxLayout;

  plotData.okButton = new QPushButton("OK");

  plotData.okButton->setObjectName("ok");

  buttonLayout->addWidget (plotData.okButton);
  buttonLayout->addStretch(1);

  connect(plotData.okButton, SIGNAL(clicked()), this, SLOT(tabOKSlot()));

  frameLayout->addLayout(buttonLayout);

  //---

  plotData.tabInd = plotTab->addTab(frame, type->desc());

  tabTypeName_[plotData.tabInd] = type->name();
}

CQChartsPlot *
CQChartsTest::
tabOKSlot(bool reuse)
{
  CQChartsView *view = getView(reuse);

  //---

  // create plot for typename of current tab
  QString typeName = tabTypeName_[plotTab_->currentIndex()];

  if (! view->charts()->isPlotType(typeName))
    return nullptr;

  CQChartsPlotType *plotType = view->charts()->plotType(typeName);

  CQChartsPlot *plot = plotType->create(view, model_);

  //---

  // set plot property for widgets for plot parameters
  CQChartsPlotType *type = charts_->plotType(typeName);
  assert(type);

  PlotData &plotData = typePlotData_[typeName];

  for (const auto &parameter : type->parameters()) {
    if      (parameter.type() == "column") {
      bool ok;

      int column = parameter.defValue().toInt(&ok);

      if (! ok)
        column = -1;

      QString columnType;

      parseParameterColumnEdit(parameter, plotData, column, columnType);

      if (! CQUtil::setProperty(plot, parameter.propName(), QVariant(column)))
        std::cerr << "Failed to set parameter " << parameter.propName().toStdString() << std::endl;

      if (columnType.length())
        model_->setHeaderData(column, Qt::Horizontal, columnType, CQCharts::Role::ColumnType);
    }
    else if (parameter.type() == "columns") {
      QString columnsStr = parameter.defValue().toString();

      std::vector<int> columns;

      (void) CQChartsUtil::fromString(columnsStr, columns);

      QString columnType;

      parseParameterColumnsEdit(parameter, plotData, columns, columnType);

      QString s = CQChartsUtil::toString(columns);

      if (! CQUtil::setProperty(plot, parameter.propName(), QVariant(s)))
        std::cerr << "Failed to set parameter " << parameter.propName().toStdString() << std::endl;

      if (columnType.length() && ! columns.empty())
        model_->setHeaderData(columns[0], Qt::Horizontal, columnType, CQCharts::Role::ColumnType);
    }
    else if (parameter.type() == "bool") {
      bool b = parameter.defValue().toBool();

      parseParameterBoolEdit(parameter, plotData, b);

      if (! CQUtil::setProperty(plot, parameter.propName(), QVariant(b)))
        std::cerr << "Failed to set parameter " << parameter.propName().toStdString() << std::endl;
    }
    else
      assert(false);
  }

  //---

  // init plot
  if (typeName == "xy") {
    CQChartsXYPlot *xyPlot = dynamic_cast<CQChartsXYPlot *>(plot);
    assert(plot);

    if      (xyPlot->isBivariate()) {
      xyPlot->setFillUnder(true);
      xyPlot->setPoints   (false);
    }
    else if (xyPlot->isStacked()) {
      xyPlot->setFillUnder(true);
      xyPlot->setPoints   (false);
    }
  }

  //---

  // add plot to view and show
  plot->setId(QString("%1%2").arg(plot->typeName()).arg(id_));

  view->addPlot(plot, bbox_);

  view->show();

  return plot;
}

//------

CQChartsPlot *
CQChartsTest::
init(const InitData &initData, int i)
{
  QStringList fstrs = initData.columnType.split(";", QString::KeepEmptyParts);

  for (int i = 0; i < fstrs.length(); ++i) {
    QString type = fstrs[i].simplified();

    if (! type.length())
      continue;

    int column = i;

    int pos = type.indexOf("#");

    if (pos >= 0) {
      QString columnStr = type.mid(0, pos).simplified();

      int column1;

      if (stringToColumn(columnStr, column1))
        column = column1;

      type = type.mid(pos + 1).simplified();
    }

    if (! model_->setHeaderData(column, Qt::Horizontal, type, CQCharts::Role::ColumnType)) {
      std::cerr << "Failed to set column type '" << type.toStdString() <<
                   "' for section '" << column << "'" << std::endl;
      continue;
    }
  }

  //---

  QString typeName = initData.plot;

  if (typeName == "")
    return nullptr;

  // adjust typename for alias (TODO: add to typeData)
  if      (typeName == "piechart")
    typeName = "pie";
  else if (typeName == "xyplot")
    typeName = "xy";
  else if (typeName == "scatterplot")
    typeName = "scatter";
  else if (typeName == "bar")
    typeName = "barchart";
  else if (typeName == "boxplot")
    typeName = "box";
  else if (typeName == "parallelplot")
    typeName = "parallel";
  else if (typeName == "geometryplot")
    typeName = "geometry";
  else if (typeName == "delaunayplot")
    typeName = "delaunay";
  else if (typeName == "adjacencyplot")
    typeName = "adjacency";

  // ignore if bad type
  CQChartsPlotType *type = charts_->plotType(typeName);

  if (! type) {
    std::cerr << "Invalid type '" << typeName.toStdString() << "' for plot" << std::endl;
    return nullptr;
  }

  //---

  // result plot if needed
  bool reuse = false;

  if (initData.overlay || initData.y1y2) {
    if (typeName == "xy")
      reuse = true;
  }
  else {
    reuse = true;
  }

  //---

  // set edits from init (argument) data
  PlotData &plotData = typePlotData_[typeName];

  plotTab_->setCurrentIndex(plotData.tabInd);

  setEditsFromInitData(plotData, initData);

  //---

  // press ok button to create plot
  CQChartsPlot *plot = tabOKSlot(reuse);
  assert(plot);

  //---

  // init plot
  if (initData.overlay || initData.y1y2) {
    if (i > 0) {
      plot->setBackground    (false);
      plot->setDataBackground(false);
    }
  }

  if (initData.title != "")
    plot->setTitle(initData.title);

  if (initData.xintegral)
    plot->xAxis()->setIntegral(true);

  if (initData.yintegral)
    plot->yAxis()->setIntegral(true);

  if (initData.xmin) plot->setXMin(initData.xmin);
  if (initData.ymin) plot->setYMin(initData.ymin);
  if (initData.xmax) plot->setXMax(initData.xmax);
  if (initData.ymax) plot->setYMax(initData.ymax);

  //---

  if (initData.properties != "") {
    QStringList strs = initData.properties.split(",", QString::SkipEmptyParts);

    for (int i = 0; i < strs.size(); ++i) {
      QString str = strs[i].simplified();

      int pos = str.indexOf("=");

      QString name  = str.mid(0, pos).simplified();
      QString value = str.mid(pos + 1).simplified();

      if (! plot->setProperty(name, value))
        std::cerr << "Failed to set property " << name.toStdString() << std::endl;
    }
  }

  return plot;
}

void
CQChartsTest::
setEditsFromInitData(const PlotData &plotData, const InitData &initData)
{
  for (const auto &le : plotData.columnEdits)
    le.second->setText(initData.nameValue(le.first));

  for (const auto &le : plotData.columnsEdits)
    le.second->setText(initData.nameValue(le.first));

  for (const auto &cb : plotData.boolEdits)
    cb.second->setChecked(initData.nameBool(cb.first));
}

//------

CQChartsModel *
CQChartsTest::
loadCsv(const QString &filename, bool commentHeader, bool firstLineHeader)
{
  //assert(! csv_);

  csv_ = new CQChartsCsv(charts_);

  csv_->setCommentHeader  (commentHeader);
  csv_->setFirstLineHeader(firstLineHeader);

  csv_->load(filename);

  setTableModel(csv_->proxyModel());

  model_ = csv_;

  return csv_;
}

CQChartsModel *
CQChartsTest::
loadTsv(const QString &filename, bool commentHeader, bool firstLineHeader)
{
  //assert(! tsv_);

  tsv_ = new CQChartsTsv(charts_);

  tsv_->setCommentHeader  (commentHeader);
  tsv_->setFirstLineHeader(firstLineHeader);

  tsv_->load(filename);

  setTableModel(tsv_->proxyModel());

  model_ = tsv_;

  return tsv_;
}

CQChartsModel *
CQChartsTest::
loadJson(const QString &filename)
{
  CQChartsJson *json = new CQChartsJson(charts_);

  json->load(filename);

  if (json->isHierarchical())
    setTreeModel(json);
  else
    setTableModel(json);

  model_ = nullptr;

  //return json;
  return nullptr;
}

CQChartsModel *
CQChartsTest::
loadData(const QString &filename, bool commentHeader, bool firstLineHeader)
{
  CQChartsData *data = new CQChartsData(charts_);

  data->setCommentHeader  (commentHeader);
  data->setFirstLineHeader(firstLineHeader);

  data->load(filename);

  setTableModel(data);

  model_ = data;

  return data;
}

//------

void
CQChartsTest::
setTableModel(QAbstractItemModel *model)
{
  table_->setModel(model);

  stack_->setCurrentIndex(0);
}

void
CQChartsTest::
setTreeModel(QAbstractItemModel *model)
{
  tree_->setModel(model);

  stack_->setCurrentIndex(1);
}

//------

void
CQChartsTest::
addParameterEdits(CQChartsPlotType *type, PlotData &plotData, QGridLayout *layout, int &row)
{
  int nbool = 0;

  for (const auto &parameter : type->parameters()) {
    if      (parameter.type() == "column")
      addParameterColumnEdit(plotData, layout, row, parameter);
    else if (parameter.type() == "columns")
      addParameterColumnsEdit(plotData, layout, row, parameter);
    else if (parameter.type() == "bool")
      ++nbool;
    else
      assert(false);
  }

  if (nbool > 0) {
    QHBoxLayout *boolLayout = new QHBoxLayout;

    for (const auto &parameter : type->parameters()) {
      if (parameter.type() == "bool")
        addParameterBoolEdit(plotData, boolLayout, parameter);
    }

    boolLayout->addStretch(1);

    layout->addLayout(boolLayout, row, 0, 1, 2);
  }
}

void
CQChartsTest::
addParameterColumnEdit(PlotData &plotData, QGridLayout *layout, int &row,
                       const CQChartsPlotParameter &parameter)
{
  bool ok;

  int column = parameter.defValue().toInt(&ok);

  QLineEdit *le = addLineEdit(layout, row, parameter.desc(), parameter.name());

  if (ok)
    le->setText(QString("%1").arg(column));

  plotData.columnEdits[parameter.name()] = le;
}

void
CQChartsTest::
addParameterColumnsEdit(PlotData &plotData, QGridLayout *layout, int &row,
                        const CQChartsPlotParameter &parameter)
{
  QLineEdit *le = addLineEdit(layout, row, parameter.desc(), parameter.name());

  le->setText(parameter.defValue().toString());

  plotData.columnsEdits[parameter.name()] = le;
}

void
CQChartsTest::
addParameterBoolEdit(PlotData &plotData, QHBoxLayout *layout,
                     const CQChartsPlotParameter &parameter)
{
  bool b = parameter.defValue().toBool();

  QCheckBox *checkBox = new QCheckBox(parameter.desc());

  checkBox->setObjectName(parameter.name());

  checkBox->setChecked(b);

  layout->addWidget(checkBox);

  plotData.boolEdits[parameter.name()] = checkBox;
}

QLineEdit *
CQChartsTest::
addLineEdit(QGridLayout *grid, int &row, const QString &name, const QString &objName) const
{
  QLabel    *label = new QLabel(name);
  QLineEdit *edit  = new QLineEdit;

  label->setObjectName(objName + "Label");
  label->setObjectName(objName + "Edit" );

  grid->addWidget(label, row, 0);
  grid->addWidget(edit , row, 1);

  ++row;

  return edit;
}

bool
CQChartsTest::
parseParameterColumnEdit(const CQChartsPlotParameter &parameter, const PlotData &plotData,
                         int &column, QString &columnType)
{
  bool ok;

  int defColumn = parameter.defValue().toInt(&ok);

  if (! ok)
    defColumn = -1;

  auto p = plotData.columnEdits.find(parameter.name());
  assert(p != plotData.columnEdits.end());

  if (! lineEditValue((*p).second, column, columnType, defColumn))
    return false;

  return true;
}

bool
CQChartsTest::
parseParameterColumnsEdit(const CQChartsPlotParameter &parameter, const PlotData &plotData,
                          std::vector<int> &columns, QString &columnType)
{
  auto p = plotData.columnsEdits.find(parameter.name());
  assert(p != plotData.columnsEdits.end());

  int column;

  bool ok = lineEditValue((*p).second, column, columnType, -1);

  if (ok) {
    columns.clear();

    columns.push_back(column);

    return true;
  }

  columns.clear();

  return lineEditValues((*p).second, columns, columnType);
}

bool
CQChartsTest::
parseParameterBoolEdit(const CQChartsPlotParameter &parameter, const PlotData &plotData, bool &b)
{
  auto p = plotData.boolEdits.find(parameter.name());
  assert(p != plotData.boolEdits.end());

  b = (*p).second->isChecked();

  return true;
}

bool
CQChartsTest::
lineEditValue(QLineEdit *le, int &i, QString &columnType, int defi) const
{
  QString str = le->text();

  //--

  int pos = str.indexOf(":");

  if (pos >= 0) {
    str = str.mid(0, pos).simplified();

    columnType = str.mid(pos + 1).simplified();
  }
  else
    str = str.simplified();

  if (! stringToColumn(str, i)) {
    i = defi;

    return false;
  }

  return true;
}

bool
CQChartsTest::
stringToColumn(const QString &str, int &column) const
{
  bool ok = false;

  int column1 = str.toInt(&ok);

  if (ok) {
    column = column1;

    return true;
  }

  //---

  if (! str.length())
    return false;

  for (int column1 = 0; column1 < model_->columnCount(); ++column1) {
    QVariant var = model_->headerData(column1, Qt::Horizontal, Qt::DisplayRole);

    if (! var.isValid())
      continue;

    if (var.toString() == str) {
      column = column1;
      return true;
    }
  }

  return false;
}

bool
CQChartsTest::
lineEditValues(QLineEdit *le, std::vector<int> &ivals, QString &columnType) const
{
  bool ok = true;

  QStringList strs = le->text().split(" ", QString::SkipEmptyParts);

  for (int i = 0; i < strs.size(); ++i) {
    int pos = strs[i].indexOf(":");

    QString lhs, rhs;

    if (pos > 0) {
      lhs = strs[i].mid(0, pos).simplified();
      rhs = strs[i].mid(pos + 1).simplified();
    }
    else
      lhs = strs[i].simplified();

    //---

    bool ok1;

    int col = lhs.toInt(&ok1);

    if (ok1) {
      ivals.push_back(col);

      if (rhs.length())
        columnType = rhs;
    }
    else
      ok = false;
  }

  return ok;
}

//------

CQChartsView *
CQChartsTest::
view() const
{
  return view_;
}

CQChartsView *
CQChartsTest::
getView(bool reuse)
{
  if (reuse) {
    if (! view_)
      view_ = new CQChartsView(charts_);
  }
  else {
    view_ = new CQChartsView(charts_);
  }

  return view_;
}

//------

QSize
CQChartsTest::
sizeHint() const
{
  return QSize(1600, 1200);
}
