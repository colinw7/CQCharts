#include <CQChartsTest.h>
#include <CQCharts.h>
#include <CQChartsTable.h>
#include <CQChartsTree.h>
#include <CQChartsCsv.h>
#include <CQChartsTsv.h>
#include <CQChartsJson.h>
#include <CQChartsData.h>
#include <CQChartsAxis.h>

#include <CQChartsView.h>
#include <CQChartsAdjacencyPlot.h>
#include <CQChartsBarChartPlot.h>
#include <CQChartsBoxPlot.h>
#include <CQChartsBubblePlot.h>
#include <CQChartsDelaunayPlot.h>
#include <CQChartsGeometryPlot.h>
#include <CQChartsHierBubblePlot.h>
#include <CQChartsParallelPlot.h>
#include <CQChartsPiePlot.h>
#include <CQChartsScatterPlot.h>
#include <CQChartsSunburstPlot.h>
#include <CQChartsTreeMapPlot.h>
#include <CQChartsXYPlot.h>

#include <CQChartsLoader.h>
#include <CQApp.h>

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
#include <QVBoxLayout>

int
main(int argc, char **argv)
{
  CQApp app(argc, argv);

  std::vector<CQChartsTest::InitData> initDatas;

  bool overlay = false;
  bool y1y2    = false;

  COptReal xmin1, xmax1, xmin2, xmax2;
  COptReal ymin1, ymax1, ymin2, ymax2;

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
      else if (arg == "x" || arg == "arg1") {
        ++i;

        if (i < argc)
          initData.setArg(0, argv[i]);
      }
      else if (arg == "y" || arg == "arg2") {
        ++i;

        if (i < argc)
          initData.setArg(1, argv[i]);
      }
      else if (arg == "z" || arg == "arg3") {
        ++i;

        if (i < argc)
          initData.setArg(2, argv[i]);
      }
      else if (arg == "arg4") {
        ++i;

        if (i < argc)
          initData.setArg(3, argv[i]);
      }
      else if (arg == "format") {
        ++i;

        if (i < argc)
          initData.format = argv[i];
      }
      else if (arg == "bivariate") {
        initData.bivariate = true;
      }
      else if (arg == "stacked") {
        initData.stacked = true;
      }
      else if (arg == "comment_header") {
        initData.commentHeader = true;
      }
      else if (arg == "first_line_header") {
        initData.firstLineHeader = true;
      }
      else if (arg == "cumulative") {
        initData.cumulative = true;
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
      else if (arg == "overlay") {
        overlay = true;
      }
      else if (arg == "y1y2") {
        y1y2 = true;
      }
      else if (arg == "and") {
        initDatas.push_back(initData);

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

  int i  = 0;
  int nd = initDatas.size();

  int nr = std::max(int(sqrt(nd)), 1);
  int nc = (nd + nr - 1)/nr;

  int dx = 1000/nc;
  int dy = 1000/nr;

  CQChartsPlot *rootPlot = nullptr;

  for (auto &initData : initDatas) {
    initData.overlay = overlay;
    initData.y1y2    = y1y2;

    initData.xmin = xmin1;
    initData.xmax = xmax1;

    if (initData.overlay || initData.y1y2) {
      if      (i == 0) {
        initData.ymin = ymin1;
        initData.ymax = ymax1;
      }
      else if (i == 1) {
        initData.ymin = ymin2;
        initData.ymax = ymax2;
      }
    }
    else {
      initData.ymin = ymin1;
      initData.ymax = ymax1;
    }

    //---

    int r = i / nc;
    int c = i % nc;

    test.setId(QString("%1").arg(i + 1));

    if (initData.overlay || initData.y1y2)
      test.setBBox(CBBox2D(0, 0, 1000, 1000));
    else
      test.setBBox(CBBox2D(c*dx, r*dy, (c + 1)*dx, (r + 1)*dy));

    if (initData.filenames.size() > 0) {
      if      (initData.csv)
        test.loadCsv(initData.filenames[0], initData.commentHeader, initData.firstLineHeader);
      else if (initData.tsv)
        test.loadTsv(initData.filenames[0], initData.commentHeader, initData.firstLineHeader);
      else if (initData.json)
        test.loadJson(initData.filenames[0]);
      else if (initData.data)
        test.loadData(initData.filenames[0]);
      else
        std::cerr << "No plot type specified" << std::endl;
    }

    CQChartsPlot *plot = test.init(initData, i);

    if (! plot)
      continue;

    if (i == 0)
      rootPlot = plot;

    if      (overlay) {
      if (i > 0) {
        plot->setRootPlot(rootPlot);

        rootPlot->addRefPlot(plot);

        plot->setDataRange(rootPlot->dataRange());

        rootPlot->applyDataRange();
      }
    }
    else if (y1y2) {
      if      (i == 0) {
      }
      else if (i == 1) {
        plot    ->setOtherPlot(rootPlot, 0);
        rootPlot->setOtherPlot(plot    , 1);

        plot->xAxis()->setVisible(false);
        plot->yAxis()->setSide(CQChartsAxis::Side::TOP_RIGHT);

        plot->key()->setVisible(false);
      }
      else {
        std::cerr << "Too many plots" << std::endl;
      }
    }

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

  addPieTab       (plotTab_);
  addXYTab        (plotTab_);
  addScatterTab   (plotTab_);
  addSunburstTab  (plotTab_);
  addBarChartTab  (plotTab_);
  addBoxTab       (plotTab_);
  addParallelTab  (plotTab_);
  addGeometryTab  (plotTab_);
  addDelaunayTab  (plotTab_);
  addAdjacencyTab (plotTab_);
  addBubbleTab    (plotTab_);
  addHierBubbleTab(plotTab_);
  addTreeMapTab   (plotTab_);

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

//------

void
CQChartsTest::
addMenus()
{
  QMenuBar *menuBar = addMenuBar();

  QMenu *fileMenu = menuBar->addMenu("&File");
  QMenu *helpMenu = menuBar->addMenu("&Help");

  QAction *loadAction = new QAction("Load", menuBar);
  QAction *helpAction = new QAction("Help", menuBar);

  connect(loadAction, SIGNAL(triggered()), this, SLOT(loadSlot()));

  fileMenu->addAction(loadAction);
  helpMenu->addAction(helpAction);
}

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
    loadData(filename);
  else
    std::cerr << "Base type specified '" << type.toStdString() << "'" << std::endl;
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

  CQChartsModel *model = qobject_cast<CQChartsModel *>(model_);

  if (model) {
    QString type = model->columnType(tableColumn_);

    columnTypeEdit_->setText(type);
  }
}

//------

void
CQChartsTest::
typeOKSlot()
{
  CQChartsModel *model = qobject_cast<CQChartsModel *>(model_);

  if (model) {
    QString type = columnTypeEdit_->text();

    model->setColumnType(tableColumn_, type);
  }
}

//------

void
CQChartsTest::
addPieTab(QTabWidget *plotTab)
{
  QFrame *pieFrame = new QFrame;

  pieFrame->setObjectName("pieFrame");

  QVBoxLayout *pieFrameLayout = new QVBoxLayout(pieFrame);

  //---

  QGridLayout *pieEditLayout = new QGridLayout;

  pieFrameLayout->addLayout(pieEditLayout);

  int row = 0;

  pieChartData_.labelEdit = addLineEdit(pieEditLayout, row, "Label", "label");
  pieChartData_.dataEdit  = addLineEdit(pieEditLayout, row, "Data" , "data" );

  //---

  QHBoxLayout *pieButtonLayout = new QHBoxLayout;

  pieChartData_.okButton = new QPushButton("OK");

  pieChartData_.okButton->setObjectName("ok");

  pieButtonLayout->addWidget (pieChartData_.okButton);
  pieButtonLayout->addStretch(1);

  connect(pieChartData_.okButton, SIGNAL(clicked()), this, SLOT(pieOKSlot()));

  pieFrameLayout->addLayout(pieButtonLayout);

  //---

  plotTab->addTab(pieFrame, "Pie");
}

CQChartsPlot *
CQChartsTest::
pieOKSlot()
{
  int col1, col2;

  (void) lineEditValue(pieChartData_.labelEdit, col1, 0);
  (void) lineEditValue(pieChartData_.dataEdit , col2, 1);

  //---

  CQChartsView *view = getView();

  CQChartsPiePlot *plot = new CQChartsPiePlot(view, model_);

  plot->setId(QString("%1%2").arg(plot->typeName()).arg(id_));

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  plot->addProperties();

  //---

  view->addPlot(plot, bbox_);

  view->show();

  //---

  return plot;
}

//------

void
CQChartsTest::
addXYTab(QTabWidget *plotTab)
{
  QFrame *xyFrame = new QFrame;

  xyFrame->setObjectName("xyFrame");

  QVBoxLayout *xyFrameLayout = new QVBoxLayout(xyFrame);

  //---

  QGridLayout *xyEditLayout = new QGridLayout;

  xyFrameLayout->addLayout(xyEditLayout);

  int row = 0;

  xyPlotData_.xEdit    = addLineEdit(xyEditLayout, row, "X"   , "x");
  xyPlotData_.yEdit    = addLineEdit(xyEditLayout, row, "Y"   , "y");
  xyPlotData_.nameEdit = addLineEdit(xyEditLayout, row, "Name", "name");

  xyPlotData_.bivariateCheck = new QCheckBox("Bivariate");
  xyPlotData_.bivariateCheck->setObjectName("bivariateCheck");

  xyEditLayout->addWidget(xyPlotData_.bivariateCheck, row, 0, 1, 2); ++row;

  xyPlotData_.stackedCheck = new QCheckBox("Stacked");
  xyPlotData_.stackedCheck->setObjectName("stackedCheck");

  xyEditLayout->addWidget(xyPlotData_.stackedCheck, row, 0, 1, 2); ++row;

  xyPlotData_.cumulativeCheck = new QCheckBox("Cumulative");
  xyPlotData_.cumulativeCheck->setObjectName("cumulativeCheck");

  xyEditLayout->addWidget(xyPlotData_.cumulativeCheck, row, 0, 1, 2);

  //---

  QHBoxLayout *xyButtonLayout = new QHBoxLayout;

  xyPlotData_.okButton = new QPushButton("OK");

  xyPlotData_.okButton->setObjectName("ok");

  xyButtonLayout->addWidget (xyPlotData_.okButton);
  xyButtonLayout->addStretch(1);

  connect(xyPlotData_.okButton, SIGNAL(clicked()), this, SLOT(xyOKSlot()));

  xyFrameLayout->addLayout(xyButtonLayout);

  //---

  plotTab->addTab(xyFrame, "XY");
}

CQChartsPlot *
CQChartsTest::
xyOKSlot(bool reuse)
{
  int col1, col2;

  (void)     lineEditValue(xyPlotData_.xEdit, col1, 0);
  bool ok2 = lineEditValue(xyPlotData_.yEdit, col2, 1);

  CQChartsPlot::Columns columns;

  if (! ok2) {
    QStringList strs = xyPlotData_.yEdit->text().split(" ", QString::SkipEmptyParts);

    for (int i = 0; i < strs.size(); ++i) {
      bool ok;

      int col = strs[i].toInt(&ok);

      if (ok)
        columns.push_back(col);
    }
  }

  int ncol;

  lineEditValue(xyPlotData_.nameEdit, ncol, -1);

  //---

  CQChartsView *view = getView(reuse);

  CQChartsXYPlot *plot = new CQChartsXYPlot(view, model_);

  plot->setId(QString("%1%2").arg(plot->typeName()).arg(id_));

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  plot->setYColumns(columns);

  plot->setNameColumn(ncol);

  if      (xyPlotData_.bivariateCheck->isChecked()) {
    plot->setBivariate(true);
    plot->setFillUnder(true);
    plot->setPoints   (false);
  }
  else if (xyPlotData_.stackedCheck->isChecked()) {
    plot->setStacked  (true);
    plot->setFillUnder(true);
    plot->setPoints   (false);
  }

  if (xyPlotData_.cumulativeCheck->isChecked())
    plot->setCumulative(true);

  plot->addProperties();

  //---

  view->addPlot(plot, bbox_);

  view->show();

  //---

  return plot;
}

//------

void
CQChartsTest::
addScatterTab(QTabWidget *plotTab)
{
  QFrame *scatterFrame = new QFrame;

  scatterFrame->setObjectName("scatterFrame");

  QVBoxLayout *scatterFrameLayout = new QVBoxLayout(scatterFrame);

  //---

  QGridLayout *scatterEditLayout = new QGridLayout;

  scatterFrameLayout->addLayout(scatterEditLayout);

  int row = 0;

  scatterPlotData_.nameEdit = addLineEdit(scatterEditLayout, row, "Name", "name");
  scatterPlotData_.xEdit    = addLineEdit(scatterEditLayout, row, "X"   , "x");
  scatterPlotData_.yEdit    = addLineEdit(scatterEditLayout, row, "Y"   , "y");

  //---

  QHBoxLayout *scatterButtonLayout = new QHBoxLayout;

  scatterPlotData_.okButton = new QPushButton("OK");

  scatterPlotData_.okButton->setObjectName("ok");

  scatterButtonLayout->addWidget (scatterPlotData_.okButton);
  scatterButtonLayout->addStretch(1);

  connect(scatterPlotData_.okButton, SIGNAL(clicked()), this, SLOT(scatterOKSlot()));

  scatterFrameLayout->addLayout(scatterButtonLayout);

  //---

  plotTab->addTab(scatterFrame, "Scatter");
}

CQChartsPlot *
CQChartsTest::
scatterOKSlot()
{
  int ncol, col1, col2;

  (void) lineEditValue(scatterPlotData_.nameEdit, ncol, -1);
  (void) lineEditValue(scatterPlotData_.xEdit   , col1, 0);
  (void) lineEditValue(scatterPlotData_.yEdit   , col2, 1);

  //---

  CQChartsView *view = getView();

  CQChartsScatterPlot *plot = new CQChartsScatterPlot(view, model_);

  plot->setId(QString("%1%2").arg(plot->typeName()).arg(id_));

  plot->setXColumn   (col1);
  plot->setYColumn   (col2);
  plot->setNameColumn(ncol);

  plot->addProperties();

  //---

  view->addPlot(plot, bbox_);

  view->show();

  //---

  return plot;
}

//------

void
CQChartsTest::
addSunburstTab(QTabWidget *plotTab)
{
  QFrame *sunburstFrame = new QFrame;

  sunburstFrame->setObjectName("sunburstFrame");

  QVBoxLayout *sunburstFrameLayout = new QVBoxLayout(sunburstFrame);

  //---

  QGridLayout *sunburstEditLayout = new QGridLayout;

  sunburstFrameLayout->addLayout(sunburstEditLayout);

  int row = 0;

  sunburstData_.nameEdit  = addLineEdit(sunburstEditLayout, row, "Name" , "name" );
  sunburstData_.valueEdit = addLineEdit(sunburstEditLayout, row, "Value", "value");

  //---

  QHBoxLayout *sunburstButtonLayout = new QHBoxLayout;

  sunburstData_.okButton = new QPushButton("OK");

  sunburstData_.okButton->setObjectName("ok");

  sunburstButtonLayout->addWidget (sunburstData_.okButton);
  sunburstButtonLayout->addStretch(1);

  connect(sunburstData_.okButton, SIGNAL(clicked()), this, SLOT(sunburstOKSlot()));

  sunburstFrameLayout->addLayout(sunburstButtonLayout);

  //---

  plotTab->addTab(sunburstFrame, "Sunburst");
}

CQChartsPlot *
CQChartsTest::
sunburstOKSlot()
{
  int col1, col2;

  (void) lineEditValue(sunburstData_.nameEdit , col1, 0);
  (void) lineEditValue(sunburstData_.valueEdit, col2, 1);

  //---

  CQChartsView *view = getView();

  CQChartsSunburstPlot *plot = new CQChartsSunburstPlot(view, model_);

  plot->setId(QString("%1%2").arg(plot->typeName()).arg(id_));

  plot->addProperties();

  //---

  view->addPlot(plot, bbox_);

  view->show();

  //---

  return plot;
}

//------

void
CQChartsTest::
addBarChartTab(QTabWidget *plotTab)
{
  QFrame *barChartFrame = new QFrame;

  barChartFrame->setObjectName("barChartFrame");

  QVBoxLayout *barChartFrameLayout = new QVBoxLayout(barChartFrame);

  //---

  QGridLayout *barChartEditLayout = new QGridLayout;

  barChartFrameLayout->addLayout(barChartEditLayout);

  int row = 0;

  barChartData_.nameEdit  = addLineEdit(barChartEditLayout, row, "Name" , "name" );
  barChartData_.valueEdit = addLineEdit(barChartEditLayout, row, "Value", "value");

  barChartData_.stackedCheck = new QCheckBox("Stacked");
  barChartData_.stackedCheck->setObjectName("stackedCheck");

  barChartEditLayout->addWidget(barChartData_.stackedCheck, row, 0, 1, 2);

  //---

  QHBoxLayout *barChartButtonLayout = new QHBoxLayout;

  barChartData_.okButton = new QPushButton("OK");

  barChartData_.okButton->setObjectName("ok");

  barChartButtonLayout->addWidget (barChartData_.okButton);
  barChartButtonLayout->addStretch(1);

  connect(barChartData_.okButton, SIGNAL(clicked()), this, SLOT(barChartOKSlot()));

  barChartFrameLayout->addLayout(barChartButtonLayout);

  //---

  plotTab->addTab(barChartFrame, "Bar Chart");
}

CQChartsPlot *
CQChartsTest::
barChartOKSlot()
{
  int col1, col2;

  (void)     lineEditValue(barChartData_.nameEdit , col1, 0);
  bool ok2 = lineEditValue(barChartData_.valueEdit, col2, 1);

  CQChartsPlot::Columns columns;

  if (! ok2) {
    QStringList strs = barChartData_.valueEdit->text().split(" ", QString::SkipEmptyParts);

    for (int i = 0; i < strs.size(); ++i) {
      bool ok;

      int col = strs[i].toInt(&ok);

      if (ok)
        columns.push_back(col);
    }
  }

  //---

  CQChartsView *view = getView();

  CQChartsBarChartPlot *plot = new CQChartsBarChartPlot(view, model_);

  plot->setId(QString("%1%2").arg(plot->typeName()).arg(id_));

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  plot->setYColumns(columns);

  plot->setStacked(barChartData_.stackedCheck->isChecked());

  plot->addProperties();

  //---

  view->addPlot(plot, bbox_);

  view->show();

  //---

  return plot;
}

//------

void
CQChartsTest::
addBoxTab(QTabWidget *plotTab)
{
  QFrame *boxFrame = new QFrame;

  boxFrame->setObjectName("boxFrame");

  QVBoxLayout *boxFrameLayout = new QVBoxLayout(boxFrame);

  //---

  QGridLayout *boxEditLayout = new QGridLayout;

  boxFrameLayout->addLayout(boxEditLayout);

  int row = 0;

  boxPlotData_.xEdit = addLineEdit(boxEditLayout, row, "X" , "x");
  boxPlotData_.yEdit = addLineEdit(boxEditLayout, row, "Y" , "y");

  //---

  QHBoxLayout *boxButtonLayout = new QHBoxLayout;

  boxPlotData_.okButton = new QPushButton("OK");

  boxPlotData_.okButton->setObjectName("ok");

  boxButtonLayout->addWidget (boxPlotData_.okButton);
  boxButtonLayout->addStretch(1);

  connect(boxPlotData_.okButton, SIGNAL(clicked()), this, SLOT(boxOKSlot()));

  boxFrameLayout->addLayout(boxButtonLayout);

  //---

  plotTab->addTab(boxFrame, "Box");
}

CQChartsPlot *
CQChartsTest::
boxOKSlot()
{
  int col1, col2;

  (void) lineEditValue(boxPlotData_.xEdit, col1, 0);
  (void) lineEditValue(boxPlotData_.yEdit, col2, 1);

  //---

  CQChartsView *view = getView();

  CQChartsBoxPlot *plot = new CQChartsBoxPlot(view, model_);

  plot->setId(QString("%1%2").arg(plot->typeName()).arg(id_));

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  plot->addProperties();

  plot->init();

  //---

  view->addPlot(plot, bbox_);

  view->show();

  //---

  return plot;
}

//------

void
CQChartsTest::
addParallelTab(QTabWidget *plotTab)
{
  QFrame *parallelFrame = new QFrame;

  parallelFrame->setObjectName("parallelFrame");

  QVBoxLayout *parallelFrameLayout = new QVBoxLayout(parallelFrame);

  //---

  QGridLayout *parallelEditLayout = new QGridLayout;

  parallelFrameLayout->addLayout(parallelEditLayout);

  int row = 0;

  parallelPlotData_.xEdit = addLineEdit(parallelEditLayout, row, "X" , "x");
  parallelPlotData_.yEdit = addLineEdit(parallelEditLayout, row, "Y" , "y");

  //---

  QHBoxLayout *parallelButtonLayout = new QHBoxLayout;

  parallelPlotData_.okButton = new QPushButton("OK");

  parallelPlotData_.okButton->setObjectName("ok");

  parallelButtonLayout->addWidget (parallelPlotData_.okButton);
  parallelButtonLayout->addStretch(1);

  connect(parallelPlotData_.okButton, SIGNAL(clicked()), this, SLOT(parallelOKSlot()));

  parallelFrameLayout->addLayout(parallelButtonLayout);

  //---

  plotTab->addTab(parallelFrame, "Parallel");
}

CQChartsPlot *
CQChartsTest::
parallelOKSlot()
{
  int col1, col2;

  (void)     lineEditValue(parallelPlotData_.xEdit, col1, 0);
  bool ok2 = lineEditValue(parallelPlotData_.yEdit, col2, 1);

  CQChartsPlot::Columns columns;

  if (! ok2) {
    QStringList strs = parallelPlotData_.yEdit->text().split(" ", QString::SkipEmptyParts);

    for (int i = 0; i < strs.size(); ++i) {
      bool ok;

      int col = strs[i].toInt(&ok);

      if (ok)
        columns.push_back(col);
    }
  }

  //---

  CQChartsView *view = getView();

  CQChartsParallelPlot *plot = new CQChartsParallelPlot(view, model_);

  plot->setId(QString("%1%2").arg(plot->typeName()).arg(id_));

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  plot->setYColumns(columns);

  plot->updateRange();

  plot->addProperties();

  //---

  view->addPlot(plot, bbox_);

  view->show();

  //---

  return plot;
}

//------

void
CQChartsTest::
addGeometryTab(QTabWidget *plotTab)
{
  QFrame *geometryFrame = new QFrame;

  geometryFrame->setObjectName("geometryFrame");

  QVBoxLayout *geometryFrameLayout = new QVBoxLayout(geometryFrame);

  //---

  QGridLayout *geometryEditLayout = new QGridLayout;

  geometryFrameLayout->addLayout(geometryEditLayout);

  int row = 0;

  geometryPlotData_.nameEdit     = addLineEdit(geometryEditLayout, row, "Name"    , "name");
  geometryPlotData_.geometryEdit = addLineEdit(geometryEditLayout, row, "Geometry", "geometry");
  geometryPlotData_.valueEdit    = addLineEdit(geometryEditLayout, row, "Value"   , "value");

  //---

  QHBoxLayout *geometryButtonLayout = new QHBoxLayout;

  geometryPlotData_.okButton = new QPushButton("OK");

  geometryPlotData_.okButton->setObjectName("ok");

  geometryButtonLayout->addWidget (geometryPlotData_.okButton);
  geometryButtonLayout->addStretch(1);

  connect(geometryPlotData_.okButton, SIGNAL(clicked()), this, SLOT(geometryOKSlot()));

  geometryFrameLayout->addLayout(geometryButtonLayout);

  //---

  plotTab->addTab(geometryFrame, "Geometry");
}

CQChartsPlot *
CQChartsTest::
geometryOKSlot()
{
  int col1, col2, col3;

  lineEditValue(geometryPlotData_.nameEdit    , col1, 0);
  lineEditValue(geometryPlotData_.geometryEdit, col2, 1);
  lineEditValue(geometryPlotData_.valueEdit   , col3, -1);

  //---

  CQChartsView *view = getView();

  CQChartsGeometryPlot *plot = new CQChartsGeometryPlot(view, model_);

  plot->setId(QString("%1%2").arg(plot->typeName()).arg(id_));

  plot->setNameColumn    (col1);
  plot->setGeometryColumn(col2);
  plot->setValueColumn   (col3);

  plot->updateRange();

  plot->addProperties();

  //---

  view->addPlot(plot, bbox_);

  view->show();

  //---

  return plot;
}

//------

void
CQChartsTest::
addDelaunayTab(QTabWidget *plotTab)
{
  QFrame *delaunayFrame = new QFrame;

  delaunayFrame->setObjectName("delaunayFrame");

  QVBoxLayout *delaunayFrameLayout = new QVBoxLayout(delaunayFrame);

  //---

  QGridLayout *delaunayEditLayout = new QGridLayout;

  delaunayFrameLayout->addLayout(delaunayEditLayout);

  int row = 0;

  delaunayPlotData_.xEdit    = addLineEdit(delaunayEditLayout, row, "X" , "x");
  delaunayPlotData_.yEdit    = addLineEdit(delaunayEditLayout, row, "Y" , "y");
  delaunayPlotData_.nameEdit = addLineEdit(delaunayEditLayout, row, "Name", "name");

  //---

  QHBoxLayout *delaunayButtonLayout = new QHBoxLayout;

  delaunayPlotData_.okButton = new QPushButton("OK");

  delaunayPlotData_.okButton->setObjectName("ok");

  delaunayButtonLayout->addWidget (delaunayPlotData_.okButton);
  delaunayButtonLayout->addStretch(1);

  connect(delaunayPlotData_.okButton, SIGNAL(clicked()), this, SLOT(delaunayOKSlot()));

  delaunayFrameLayout->addLayout(delaunayButtonLayout);

  //---

  plotTab->addTab(delaunayFrame, "Delaunay");
}

CQChartsPlot *
CQChartsTest::
delaunayOKSlot()
{
  int col1, col2;

  lineEditValue(delaunayPlotData_.xEdit, col1, 0);
  lineEditValue(delaunayPlotData_.yEdit, col2, 1);

  int ncol;

  lineEditValue(delaunayPlotData_.nameEdit, ncol, -1);

  //---

  CQChartsView *view = getView();

  CQChartsDelaunayPlot *plot = new CQChartsDelaunayPlot(view, model_);

  plot->setId(QString("%1%2").arg(plot->typeName()).arg(id_));

  plot->setXColumn   (col1);
  plot->setYColumn   (col2);
  plot->setNameColumn(ncol);

  plot->updateRange();

  plot->addProperties();

  //---

  view->addPlot(plot, bbox_);

  view->show();

  //---

  return plot;
}

//------

void
CQChartsTest::
addAdjacencyTab(QTabWidget *plotTab)
{
  QFrame *adjacencyFrame = new QFrame;

  adjacencyFrame->setObjectName("adjacencyFrame");

  QVBoxLayout *adjacencyFrameLayout = new QVBoxLayout(adjacencyFrame);

  //---

  QGridLayout *adjacencyEditLayout = new QGridLayout;

  adjacencyFrameLayout->addLayout(adjacencyEditLayout);

  int row = 0;

  adjacencyPlotData_.nodeEdit        =
    addLineEdit(adjacencyEditLayout, row, "Node"       , "node");
  adjacencyPlotData_.connectionsEdit =
    addLineEdit(adjacencyEditLayout, row, "Connections", "connections");
  adjacencyPlotData_.nameEdit        =
    addLineEdit(adjacencyEditLayout, row, "Name"       , "name");
  adjacencyPlotData_.groupEdit       =
    addLineEdit(adjacencyEditLayout, row, "Group"      , "group");

  //---

  QHBoxLayout *adjacencyButtonLayout = new QHBoxLayout;

  adjacencyPlotData_.okButton = new QPushButton("OK");

  adjacencyPlotData_.okButton->setObjectName("ok");

  adjacencyButtonLayout->addWidget (adjacencyPlotData_.okButton);
  adjacencyButtonLayout->addStretch(1);

  connect(adjacencyPlotData_.okButton, SIGNAL(clicked()), this, SLOT(adjacencyOKSlot()));

  adjacencyFrameLayout->addLayout(adjacencyButtonLayout);

  //---

  plotTab->addTab(adjacencyFrame, "Adjacency");
}

CQChartsPlot *
CQChartsTest::
adjacencyOKSlot()
{
  int col1, col2, col3;

  lineEditValue(adjacencyPlotData_.nodeEdit       , col1, 0);
  lineEditValue(adjacencyPlotData_.connectionsEdit, col2, 1);
  lineEditValue(adjacencyPlotData_.groupEdit      , col3, -1);

  int ncol;

  lineEditValue(adjacencyPlotData_.nameEdit, ncol, -1);

  //---

  CQChartsView *view = getView();

  CQChartsAdjacencyPlot *plot = new CQChartsAdjacencyPlot(view, model_);

  plot->setId(QString("%1%2").arg(plot->typeName()).arg(id_));

  plot->setNodeColumn       (col1);
  plot->setConnectionsColumn(col2);
  plot->setGroupColumn      (col3);
  plot->setNameColumn       (ncol);

  plot->updateRange();

  plot->addProperties();

  //---

  view->addPlot(plot, bbox_);

  view->show();

  //---

  return plot;
}

//------

void
CQChartsTest::
addBubbleTab(QTabWidget *plotTab)
{
  QFrame *bubbleFrame = new QFrame;

  bubbleFrame->setObjectName("bubbleFrame");

  QVBoxLayout *bubbleFrameLayout = new QVBoxLayout(bubbleFrame);

  //---

  QGridLayout *bubbleEditLayout = new QGridLayout;

  bubbleFrameLayout->addLayout(bubbleEditLayout);

  int row = 0;

  bubbleData_.nameEdit  = addLineEdit(bubbleEditLayout, row, "Name" , "name" );
  bubbleData_.valueEdit = addLineEdit(bubbleEditLayout, row, "Value", "value");

  //---

  QHBoxLayout *bubbleButtonLayout = new QHBoxLayout;

  bubbleData_.okButton = new QPushButton("OK");

  bubbleData_.okButton->setObjectName("ok");

  bubbleButtonLayout->addWidget (bubbleData_.okButton);
  bubbleButtonLayout->addStretch(1);

  connect(bubbleData_.okButton, SIGNAL(clicked()), this, SLOT(bubbleOKSlot()));

  bubbleFrameLayout->addLayout(bubbleButtonLayout);

  //---

  plotTab->addTab(bubbleFrame, "Bubble");
}

CQChartsPlot *
CQChartsTest::
bubbleOKSlot()
{
  int col1, col2;

  (void) lineEditValue(bubbleData_.nameEdit , col1, 0);
  (void) lineEditValue(bubbleData_.valueEdit, col2, 1);

  //---

  CQChartsView *view = getView();

  CQChartsBubblePlot *plot = new CQChartsBubblePlot(view, model_);

  plot->setId(QString("%1%2").arg(plot->typeName()).arg(id_));

  plot->addProperties();

  //---

  view->addPlot(plot, bbox_);

  view->show();

  //---

  return plot;
}

//------

void
CQChartsTest::
addHierBubbleTab(QTabWidget *plotTab)
{
  QFrame *hierBubbleFrame = new QFrame;

  hierBubbleFrame->setObjectName("hierBubbleFrame");

  QVBoxLayout *hierBubbleFrameLayout = new QVBoxLayout(hierBubbleFrame);

  //---

  QGridLayout *hierBubbleEditLayout = new QGridLayout;

  hierBubbleFrameLayout->addLayout(hierBubbleEditLayout);

  int row = 0;

  hierBubbleData_.nameEdit  = addLineEdit(hierBubbleEditLayout, row, "Name" , "name" );
  hierBubbleData_.valueEdit = addLineEdit(hierBubbleEditLayout, row, "Value", "value");

  //---

  QHBoxLayout *hierBubbleButtonLayout = new QHBoxLayout;

  hierBubbleData_.okButton = new QPushButton("OK");

  hierBubbleData_.okButton->setObjectName("ok");

  hierBubbleButtonLayout->addWidget (hierBubbleData_.okButton);
  hierBubbleButtonLayout->addStretch(1);

  connect(hierBubbleData_.okButton, SIGNAL(clicked()), this, SLOT(hierBubbleOKSlot()));

  hierBubbleFrameLayout->addLayout(hierBubbleButtonLayout);

  //---

  plotTab->addTab(hierBubbleFrame, "Hier Bubble");
}

CQChartsPlot *
CQChartsTest::
hierBubbleOKSlot()
{
  int col1, col2;

  (void) lineEditValue(hierBubbleData_.nameEdit , col1, 0);
  (void) lineEditValue(hierBubbleData_.valueEdit, col2, 1);

  //---

  CQChartsView *view = getView();

  CQChartsHierBubblePlot *plot = new CQChartsHierBubblePlot(view, model_);

  plot->setId(QString("%1%2").arg(plot->typeName()).arg(id_));

  plot->addProperties();

  //---

  view->addPlot(plot, bbox_);

  view->show();

  //---

  return plot;
}

//------

void
CQChartsTest::
addTreeMapTab(QTabWidget *plotTab)
{
  QFrame *treeMapFrame = new QFrame;

  treeMapFrame->setObjectName("treeMapFrame");

  QVBoxLayout *treeMapFrameLayout = new QVBoxLayout(treeMapFrame);

  //---

  QGridLayout *treeMapEditLayout = new QGridLayout;

  treeMapFrameLayout->addLayout(treeMapEditLayout);

  int row = 0;

  treeMapData_.nameEdit  = addLineEdit(treeMapEditLayout, row, "Name" , "name" );
  treeMapData_.valueEdit = addLineEdit(treeMapEditLayout, row, "Value", "value");

  //---

  QHBoxLayout *treeMapButtonLayout = new QHBoxLayout;

  treeMapData_.okButton = new QPushButton("OK");

  treeMapData_.okButton->setObjectName("ok");

  treeMapButtonLayout->addWidget (treeMapData_.okButton);
  treeMapButtonLayout->addStretch(1);

  connect(treeMapData_.okButton, SIGNAL(clicked()), this, SLOT(treeMapOKSlot()));

  treeMapFrameLayout->addLayout(treeMapButtonLayout);

  //---

  plotTab->addTab(treeMapFrame, "Tree Map");
}

CQChartsPlot *
CQChartsTest::
treeMapOKSlot()
{
  int col1, col2;

  (void) lineEditValue(treeMapData_.nameEdit , col1, 0);
  (void) lineEditValue(treeMapData_.valueEdit, col2, 1);

  //---

  CQChartsView *view = getView();

  CQChartsTreeMapPlot *plot = new CQChartsTreeMapPlot(view, model_);

  plot->setId(QString("%1%2").arg(plot->typeName()).arg(id_));

  plot->addProperties();

  //---

  view->addPlot(plot, bbox_);

  view->show();

  //---

  return plot;
}

//------

CQChartsPlot *
CQChartsTest::
init(const InitData &initData, int i)
{
  CQChartsModel *model = qobject_cast<CQChartsModel *>(model_);

  if (! model) {
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(model_);

    if (proxyModel)
      model = qobject_cast<CQChartsModel *>(proxyModel->sourceModel());
  }

  QStringList fstrs = initData.format.split(";", QString::SkipEmptyParts);

  for (int i = 0; i < fstrs.length(); ++i) {
    QString type = fstrs[i];

    if (model)
      model->setColumnType(i, type);
  }

  //---

  CQChartsPlot *plot = nullptr;

  if      (initData.plot == "pie" || initData.plot == "piechart") {
    plotTab_->setCurrentIndex(0);

    pieChartData_.labelEdit->setText(initData.arg(0));
    pieChartData_.dataEdit ->setText(initData.arg(1));

    plot = pieOKSlot();
  }
  else if (initData.plot == "xy" || initData.plot == "xyplot") {
    plotTab_->setCurrentIndex(1);

    xyPlotData_.xEdit   ->setText(initData.arg(0));
    xyPlotData_.yEdit   ->setText(initData.arg(1));
    xyPlotData_.nameEdit->setText(initData.arg(2));

    xyPlotData_.bivariateCheck ->setChecked(initData.bivariate);
    xyPlotData_.stackedCheck   ->setChecked(initData.stacked);
    xyPlotData_.cumulativeCheck->setChecked(initData.cumulative);

    plot = xyOKSlot(/*reuse*/ true);
  }
  else if (initData.plot == "scatter" || initData.plot == "scatterplot") {
    plotTab_->setCurrentIndex(2);

    scatterPlotData_.nameEdit->setText(initData.arg(2));
    scatterPlotData_.xEdit   ->setText(initData.arg(0));
    scatterPlotData_.yEdit   ->setText(initData.arg(1));

    plot = scatterOKSlot();
  }
  else if (initData.plot == "sunburst") {
    plotTab_->setCurrentIndex(3);

    sunburstData_.nameEdit ->setText(initData.arg(0));
    sunburstData_.valueEdit->setText(initData.arg(1));

    plot = sunburstOKSlot();
  }
  else if (initData.plot == "bar" || initData.plot == "barchart") {
    plotTab_->setCurrentIndex(4);

    barChartData_.nameEdit ->setText(initData.arg(0));
    barChartData_.valueEdit->setText(initData.arg(1));

    barChartData_.stackedCheck->setChecked(initData.stacked);

    plot = barChartOKSlot();
  }
  else if (initData.plot == "box" || initData.plot == "boxplot") {
    plotTab_->setCurrentIndex(5);

    boxPlotData_.xEdit->setText(initData.arg(0));
    boxPlotData_.yEdit->setText(initData.arg(1));

    plot = boxOKSlot();
  }
  else if (initData.plot == "parallel" || initData.plot == "parallelplot") {
    plotTab_->setCurrentIndex(6);

    parallelPlotData_.xEdit->setText(initData.arg(0));
    parallelPlotData_.yEdit->setText(initData.arg(1));

    plot = parallelOKSlot();
  }
  else if (initData.plot == "geometry" || initData.plot == "geometryplot") {
    plotTab_->setCurrentIndex(7);

    geometryPlotData_.nameEdit    ->setText(initData.arg(0));
    geometryPlotData_.geometryEdit->setText(initData.arg(1));
    geometryPlotData_.valueEdit   ->setText(initData.arg(2));

    plot = geometryOKSlot();
  }
  else if (initData.plot == "delaunay" || initData.plot == "delaunayplot") {
    plotTab_->setCurrentIndex(8);

    delaunayPlotData_.xEdit   ->setText(initData.arg(0));
    delaunayPlotData_.yEdit   ->setText(initData.arg(1));
    delaunayPlotData_.nameEdit->setText(initData.arg(2));

    plot = delaunayOKSlot();
  }
  else if (initData.plot == "adjacency" || initData.plot == "adjacencyplot") {
    plotTab_->setCurrentIndex(9);

    adjacencyPlotData_.nodeEdit       ->setText(initData.arg(0));
    adjacencyPlotData_.connectionsEdit->setText(initData.arg(1));
    adjacencyPlotData_.nameEdit       ->setText(initData.arg(2));
    adjacencyPlotData_.groupEdit      ->setText(initData.arg(3));

    plot = adjacencyOKSlot();
  }
  else if (initData.plot == "bubble") {
    plotTab_->setCurrentIndex(10);

    bubbleData_.nameEdit ->setText(initData.arg(0));
    bubbleData_.valueEdit->setText(initData.arg(1));

    plot = bubbleOKSlot();
  }
  else if (initData.plot == "hierbubble") {
    plotTab_->setCurrentIndex(11);

    hierBubbleData_.nameEdit ->setText(initData.arg(0));
    hierBubbleData_.valueEdit->setText(initData.arg(1));

    plot = hierBubbleOKSlot();
  }
  else if (initData.plot == "treemap") {
    plotTab_->setCurrentIndex(12);

    treeMapData_.nameEdit ->setText(initData.arg(0));
    treeMapData_.valueEdit->setText(initData.arg(1));

    plot = treeMapOKSlot();
  }
  else {
    return nullptr;
  }

  //---

  if (initData.overlay || initData.y1y2) {
    if (i > 0)
      plot->setBackground(QColor("#00000000"));
  }

  if (initData.title != "")
    plot->setTitle(initData.title);

  if (initData.xintegral)
    plot->xAxis()->setIntegral(true);

  if (initData.yintegral)
    plot->yAxis()->setIntegral(true);

  plot->setXMin(initData.xmin); plot->setYMin(initData.ymin);
  plot->setXMax(initData.xmax); plot->setYMax(initData.ymax);

  return plot;
}

//------

void
CQChartsTest::
loadCsv(const QString &filename, bool commentHeader, bool firstLineHeader)
{
  CQChartsCsv *csv = new CQChartsCsv;

  csv->setCommentHeader  (commentHeader);
  csv->setFirstLineHeader(firstLineHeader);

  csv->load(filename);

  setTableModel(csv);
}

void
CQChartsTest::
loadTsv(const QString &filename, bool commentHeader, bool firstLineHeader)
{
  CQChartsTsv *tsv = new CQChartsTsv;

  tsv->setCommentHeader  (commentHeader);
  tsv->setFirstLineHeader(firstLineHeader);

  tsv->load(filename);

  setTableModel(tsv);
}

void
CQChartsTest::
loadJson(const QString &filename)
{
  CQChartsJson *json = new CQChartsJson;

  json->load(filename);

  if (json->isHierarchical())
    setTreeModel(json);
  else
    setTableModel(json);
}

void
CQChartsTest::
loadData(const QString &filename)
{
  CQChartsData *data = new CQChartsData;

  data->load(filename);

  setTableModel(data);
}

//------

void
CQChartsTest::
setTableModel(QAbstractItemModel *model)
{
  //model_ = model;

  //table_->setModel(model_);

  QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel;

  proxyModel->setSourceModel(model);

  table_->setModel(proxyModel);

  model_ = proxyModel;

  stack_->setCurrentIndex(0);
}

void
CQChartsTest::
setTreeModel(QAbstractItemModel *model)
{
  model_ = model;

  tree_->setModel(model_);

  stack_->setCurrentIndex(1);
}

//------

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
lineEditValue(QLineEdit *le, int &i, int defi) const
{
  bool ok = false;

  i = le->text().toInt(&ok);

  if (! ok)
    i = defi;

  return ok;
}

//------

CQChartsView *
CQChartsTest::
getView(bool reuse)
{
  if (reuse) {
    if (! view_)
      view_ = new CQChartsView;
  }
  else {
    view_ = new CQChartsView;
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
