#include <CQChartsTest.h>
#include <CQCharts.h>
#include <CQChartsTable.h>
#include <CQChartsTree.h>
#include <CQChartsCsv.h>
#include <CQChartsTsv.h>
#include <CQChartsJson.h>
#include <CQChartsData.h>

#include <CQChartsWindow.h>
#include <CQChartsAdjacencyPlot.h>
#include <CQChartsBarChartPlot.h>
#include <CQChartsBoxPlot.h>
#include <CQChartsDelaunayPlot.h>
#include <CQChartsGeometryPlot.h>
#include <CQChartsParallelPlot.h>
#include <CQChartsPiePlot.h>
#include <CQChartsScatterPlot.h>
#include <CQChartsSunburstPlot.h>
#include <CQChartsXYPlot.h>

#include <CQChartsLoader.h>
#include <CQGradientPalette.h>
#include <CQGradientPaletteControl.h>
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
      else if (arg == "and") {
        initDatas.push_back(initData);

        initData = CQChartsTest::InitData();
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

  int x  = 0;
  int y  = 0;
  int dx = 1000;
  int dy = 1000/nd;

  for (const auto &initData : initDatas) {
    test.setId(QString("Plot%1").arg(i + 1));

    test.setBBox(CBBox2D(x, y, x + dx, y + dy));

    if (initData.filenames.size() > 0) {
      if      (initData.csv)
        test.loadCsv(initData.filenames[0]);
      else if (initData.tsv)
        test.loadTsv(initData.filenames[0], initData.commentHeader, initData.firstLineHeader);
      else if (initData.json)
        test.loadJson(initData.filenames[0]);
      else if (initData.data)
        test.loadData(initData.filenames[0]);
      else
        std::cerr << "No plot type specified" << std::endl;
    }

    test.init(initData);

    y += dy;

    ++i;
  }

  //---

  test.show();

  if (test.window())
    test.window()->raise();

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

  addPieTab      (plotTab_);
  addXYTab       (plotTab_);
  addScatterTab  (plotTab_);
  addSunburstTab (plotTab_);
  addBarChartTab (plotTab_);
  addBoxTab      (plotTab_);
  addParallelTab (plotTab_);
  addGeometryTab (plotTab_);
  addDelaunayTab (plotTab_);
  addAdjacencyTab(plotTab_);

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

  //---

  QFrame *gradientFrame = new QFrame;

  gradientFrame->setObjectName("gradientFrame");

  QVBoxLayout *gradientLayout = new QVBoxLayout(gradientFrame);
  gradientLayout->setMargin(0); gradientLayout->setSpacing(0);

  palettePlot_    = new CQGradientPalette(this, nullptr);
  paletteControl_ = new CQGradientPaletteControl(palettePlot_);

  gradientLayout->addWidget(palettePlot_);
  gradientLayout->addWidget(paletteControl_);

  splitter->addWidget(gradientFrame);
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
    loadCsv(filename);
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

void
CQChartsTest::
pieOKSlot()
{
  int col1, col2;

  (void) lineEditValue(pieChartData_.labelEdit, col1, 0);
  (void) lineEditValue(pieChartData_.dataEdit , col2, 1);

  //---

  CQChartsWindow *window = getWindow();

  CQChartsPiePlot *plot = new CQChartsPiePlot(window, model_);

  plot->setId(id_);

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  plot->addProperties();

  //---

  plot->setPalette(palettePlot_->gradientPalette());

  window->addPlot(plot, bbox_);

  window->show();
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

  xyEditLayout->addWidget(xyPlotData_.stackedCheck, row, 0, 1, 2);

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

void
CQChartsTest::
xyOKSlot()
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

  CQChartsWindow *window = getWindow();

  CQChartsXYPlot *plot = new CQChartsXYPlot(window, model_);

  plot->setId(id_);

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  plot->setYColumns(columns);

  plot->setNameColumn(ncol);

  plot->setBivariate(xyPlotData_.bivariateCheck->isChecked());
  plot->setStacked  (xyPlotData_.stackedCheck  ->isChecked());

  plot->addProperties();

  //---

  plot->setPalette(palettePlot_->gradientPalette());

  window->addPlot(plot, bbox_);

  window->show();
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

void
CQChartsTest::
scatterOKSlot()
{
  int ncol, col1, col2;

  (void) lineEditValue(scatterPlotData_.nameEdit, ncol, -1);
  (void) lineEditValue(scatterPlotData_.xEdit   , col1, 0);
  (void) lineEditValue(scatterPlotData_.yEdit   , col2, 1);


  //---

  CQChartsWindow *window = getWindow();

  CQChartsScatterPlot *plot = new CQChartsScatterPlot(window, model_);

  plot->setId(id_);

  plot->setXColumn   (col1);
  plot->setYColumn   (col2);
  plot->setNameColumn(ncol);

  plot->updateRange();

  plot->addProperties();

  //---

  plot->setPalette(palettePlot_->gradientPalette());

  window->addPlot(plot, bbox_);

  window->show();
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

void
CQChartsTest::
sunburstOKSlot()
{
  int col1, col2;

  (void) lineEditValue(sunburstData_.nameEdit , col1, 0);
  (void) lineEditValue(sunburstData_.valueEdit, col2, 1);

  //---

  CQChartsWindow *window = getWindow();

  CQChartsSunburstPlot *plot = new CQChartsSunburstPlot(window, model_);

  plot->setId(id_);

  plot->addProperties();

  //---

  plot->setPalette(palettePlot_->gradientPalette());

  window->addPlot(plot, bbox_);

  window->show();
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

void
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

  CQChartsWindow *window = getWindow();

  CQChartsBarChartPlot *plot = new CQChartsBarChartPlot(window, model_);

  plot->setId(id_);

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  plot->setYColumns(columns);

  plot->setStacked(barChartData_.stackedCheck->isChecked());

  plot->addProperties();

  //---

  plot->setPalette(palettePlot_->gradientPalette());

  window->addPlot(plot, bbox_);

  window->show();
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

void
CQChartsTest::
boxOKSlot()
{
  int col1, col2;

  (void) lineEditValue(boxPlotData_.xEdit, col1, 0);
  (void) lineEditValue(boxPlotData_.yEdit, col2, 1);

  //---

  CQChartsWindow *window = getWindow();

  CQChartsBoxPlot *plot = new CQChartsBoxPlot(window, model_);

  plot->setId(id_);

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  plot->addProperties();

  plot->init();

  //---

  plot->setPalette(palettePlot_->gradientPalette());

  window->addPlot(plot, bbox_);

  window->show();
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

void
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

  CQChartsWindow *window = getWindow();

  CQChartsParallelPlot *plot = new CQChartsParallelPlot(window, model_);

  plot->setId(id_);

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  plot->setYColumns(columns);

  plot->updateRange();

  plot->addProperties();

  //---

  plot->setPalette(palettePlot_->gradientPalette());

  window->addPlot(plot, bbox_);

  window->show();
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

void
CQChartsTest::
geometryOKSlot()
{
  int col1, col2, col3;

  lineEditValue(geometryPlotData_.nameEdit    , col1, 0);
  lineEditValue(geometryPlotData_.geometryEdit, col2, 1);
  lineEditValue(geometryPlotData_.valueEdit   , col3, -1);

  //---

  CQChartsWindow *window = getWindow();

  CQChartsGeometryPlot *plot = new CQChartsGeometryPlot(window, model_);

  plot->setId(id_);

  plot->setNameColumn    (col1);
  plot->setGeometryColumn(col2);
  plot->setValueColumn   (col3);

  plot->updateRange();

  plot->addProperties();

  //---

  plot->setPalette(palettePlot_->gradientPalette());

  window->addPlot(plot, bbox_);

  window->show();
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

void
CQChartsTest::
delaunayOKSlot()
{
  int col1, col2;

  lineEditValue(delaunayPlotData_.xEdit, col1, 0);
  lineEditValue(delaunayPlotData_.yEdit, col2, 1);

  int ncol;

  lineEditValue(delaunayPlotData_.nameEdit, ncol, -1);

  //---

  CQChartsWindow *window = getWindow();

  CQChartsDelaunayPlot *plot = new CQChartsDelaunayPlot(window, model_);

  plot->setId(id_);

  plot->setXColumn   (col1);
  plot->setYColumn   (col2);
  plot->setNameColumn(ncol);

  plot->updateRange();

  plot->addProperties();

  //---

  plot->setPalette(palettePlot_->gradientPalette());

  window->addPlot(plot, bbox_);

  window->show();
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

void
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

  CQChartsWindow *window = getWindow();

  CQChartsAdjacencyPlot *plot = new CQChartsAdjacencyPlot(window, model_);

  plot->setId(id_);

  plot->setNodeColumn       (col1);
  plot->setConnectionsColumn(col2);
  plot->setGroupColumn      (col3);
  plot->setNameColumn       (ncol);

  plot->updateRange();

  plot->addProperties();

  //---

  plot->setPalette(palettePlot_->gradientPalette());

  window->addPlot(plot, bbox_);

  window->show();
}

//------

void
CQChartsTest::
init(const InitData &initData)
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

  if      (initData.plot == "pie" || initData.plot == "piechart") {
    plotTab_->setCurrentIndex(0);

    pieChartData_.labelEdit->setText(initData.arg(0));
    pieChartData_.dataEdit ->setText(initData.arg(1));

    pieOKSlot();
  }
  else if (initData.plot == "xy" || initData.plot == "xyplot") {
    plotTab_->setCurrentIndex(1);

    xyPlotData_.xEdit   ->setText(initData.arg(0));
    xyPlotData_.yEdit   ->setText(initData.arg(1));
    xyPlotData_.nameEdit->setText(initData.arg(2));

    xyPlotData_.bivariateCheck->setChecked(initData.bivariate);
    xyPlotData_.stackedCheck  ->setChecked(initData.stacked);

    xyOKSlot();
  }
  else if (initData.plot == "scatter" || initData.plot == "scatterplot") {
    plotTab_->setCurrentIndex(2);

    scatterPlotData_.nameEdit->setText(initData.arg(2));
    scatterPlotData_.xEdit   ->setText(initData.arg(0));
    scatterPlotData_.yEdit   ->setText(initData.arg(1));

    scatterOKSlot();
  }
  else if (initData.plot == "sunburst") {
    plotTab_->setCurrentIndex(3);

    sunburstData_.nameEdit ->setText(initData.arg(0));
    sunburstData_.valueEdit->setText(initData.arg(1));

    sunburstOKSlot();
  }
  else if (initData.plot == "bar" || initData.plot == "barchart") {
    plotTab_->setCurrentIndex(4);

    barChartData_.nameEdit ->setText(initData.arg(0));
    barChartData_.valueEdit->setText(initData.arg(1));

    barChartData_.stackedCheck->setChecked(initData.stacked);

    barChartOKSlot();
  }
  else if (initData.plot == "box" || initData.plot == "boxplot") {
    plotTab_->setCurrentIndex(5);

    boxPlotData_.xEdit->setText(initData.arg(0));
    boxPlotData_.yEdit->setText(initData.arg(1));

    boxOKSlot();
  }
  else if (initData.plot == "parallel" || initData.plot == "parallelplot") {
    plotTab_->setCurrentIndex(6);

    parallelPlotData_.xEdit->setText(initData.arg(0));
    parallelPlotData_.yEdit->setText(initData.arg(1));

    parallelOKSlot();
  }
  else if (initData.plot == "geometry" || initData.plot == "geometryplot") {
    plotTab_->setCurrentIndex(7);

    geometryPlotData_.nameEdit    ->setText(initData.arg(0));
    geometryPlotData_.geometryEdit->setText(initData.arg(1));
    geometryPlotData_.valueEdit   ->setText(initData.arg(2));

    geometryOKSlot();
  }
  else if (initData.plot == "delaunay" || initData.plot == "delaunayplot") {
    plotTab_->setCurrentIndex(8);

    delaunayPlotData_.xEdit   ->setText(initData.arg(0));
    delaunayPlotData_.yEdit   ->setText(initData.arg(1));
    delaunayPlotData_.nameEdit->setText(initData.arg(2));

    delaunayOKSlot();
  }
  else if (initData.plot == "adjacency" || initData.plot == "adjacencyplot") {
    plotTab_->setCurrentIndex(9);

    adjacencyPlotData_.nodeEdit       ->setText(initData.arg(0));
    adjacencyPlotData_.connectionsEdit->setText(initData.arg(1));
    adjacencyPlotData_.nameEdit       ->setText(initData.arg(2));
    adjacencyPlotData_.groupEdit      ->setText(initData.arg(3));

    adjacencyOKSlot();
  }
}

//------

void
CQChartsTest::
loadCsv(const QString &filename)
{
  CQChartsCsv *csv = new CQChartsCsv;

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

CQChartsWindow *
CQChartsTest::
getWindow()
{
  if (! window_)
    window_ = new CQChartsWindow;

  return window_;
}

//------

QSize
CQChartsTest::
sizeHint() const
{
  return QSize(1600, 1200);
}
