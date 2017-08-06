#include <CQChartsTest.h>
#include <CQCharts.h>
#include <CQChartsTable.h>
#include <CQChartsTree.h>
#include <CQChartsCsv.h>
#include <CQChartsTsv.h>
#include <CQChartsJson.h>
#include <CQChartsData.h>

#include <CQChartsBarChartPlot.h>
#include <CQChartsBoxPlot.h>
#include <CQChartsParallelPlot.h>
#include <CQChartsPiePlot.h>
#include <CQChartsSunburstPlot.h>
#include <CQChartsXYPlot.h>

#include <CQChartsLoader.h>
#include <CQGradientPalette.h>
#include <CQGradientPaletteControl.h>
#include <CQApp.h>

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

  bool csv  = false;
  bool tsv  = false;
  bool json = false;
  bool data = false;

  QString plot;
  QString xarg;
  QString yarg;
  QString zarg;
  QString format;

  std::vector<QString> filenames;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      std::string arg = &argv[i][1];

      if      (arg == "csv")
        csv  = true;
      else if (arg == "tsv")
        tsv  = true;
      else if (arg == "json")
        json = true;
      else if (arg == "data")
        data = true;
      else if (arg == "plot") {
        ++i;

        if (i < argc)
          plot = argv[i];
      }
      else if (arg == "x") {
        ++i;

        if (i < argc)
          xarg = argv[i];
      }
      else if (arg == "y") {
        ++i;

        if (i < argc)
          yarg = argv[i];
      }
      else if (arg == "z") {
        ++i;

        if (i < argc)
          zarg = argv[i];
      }
      else if (arg == "format") {
        ++i;

        if (i < argc)
          format = argv[i];
      }
    }
    else
      filenames.push_back(argv[i]);
  }

  //---

  CQChartsTest test;

  if (filenames.size() > 0) {
    if      (csv)
      test.loadCsv(filenames[0]);
    else if (tsv)
      test.loadTsv(filenames[0]);
    else if (json)
      test.loadJson(filenames[0]);
    else if (data)
      test.loadData(filenames[0]);
    else
      std::cerr << "No plot type specified" << std::endl;
  }

  //---

  test.show();

  //---

  test.init(plot, xarg, yarg, zarg, format);

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

  QHBoxLayout *layout = new QHBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(0);

  QSplitter *splitter = new QSplitter;

  splitter->setObjectName("splitter");

  layout->addWidget(splitter);

  //---

  QFrame *plotFrame = new QFrame;

  plotFrame->setObjectName("plotFrame");

  QVBoxLayout *plotLayout = new QVBoxLayout(plotFrame);
  plotLayout->setMargin(0); plotLayout->setSpacing(2);

  splitter->addWidget(plotFrame);

  //---

  // plots (one per tab)
  plotTab_ = new QTabWidget;

  plotTab_->setObjectName("plotTab");

  plotLayout->addWidget(plotTab_);

  plotTab_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  addPieTab     (plotTab_);
  addXYTab      (plotTab_);
  addSunburstTab(plotTab_);
  addBarChartTab(plotTab_);
  addBoxTab     (plotTab_);
  addParallelTab(plotTab_);

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
  if      (type == "CSV")
    loadCsv(filename);
  else if (type == "TSV")
    loadTsv(filename);
  else if (type == "Json")
    loadJson(filename);
  else if (type == "Data")
    loadData(filename);
  else
    std::cerr << "Base type specified '" << type.toStdString() << "'" << std::endl;
}

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

  xyEditLayout->addWidget(xyPlotData_.bivariateCheck, row, 0, 1, 2);

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

//------

void
CQChartsTest::
init(const QString &plot, const QString &xarg, const QString &yarg,
     const QString &zarg, const QString &format)
{
  CQChartsModel *model = qobject_cast<CQChartsModel *>(model_);

  QStringList fstrs = format.split(";", QString::SkipEmptyParts);

  for (int i = 0; i < fstrs.length(); ++i) {
    QString type = fstrs[i];

    if (model)
      model->setColumnType(i, type);
  }

  //---

  if      (plot == "pie" || plot == "piechart") {
    plotTab_->setCurrentIndex(0);

    pieChartData_.labelEdit->setText(xarg);
    pieChartData_.dataEdit ->setText(yarg);

    pieOKSlot();
  }
  else if (plot == "xy" || plot == "xy") {
    plotTab_->setCurrentIndex(1);

    xyPlotData_.xEdit   ->setText(xarg);
    xyPlotData_.yEdit   ->setText(yarg);
    xyPlotData_.nameEdit->setText(zarg);

    xyOKSlot();
  }
  else if (plot == "sunburst") {
    plotTab_->setCurrentIndex(2);

    sunburstData_.nameEdit ->setText(xarg);
    sunburstData_.valueEdit->setText(yarg);

    sunburstOKSlot();
  }
  else if (plot == "bar" || plot == "barchart") {
    plotTab_->setCurrentIndex(3);

    barChartData_.nameEdit ->setText(xarg);
    barChartData_.valueEdit->setText(yarg);

    barChartOKSlot();
  }
  else if (plot == "box" || plot == "boxplot") {
    plotTab_->setCurrentIndex(4);

    boxPlotData_.xEdit->setText(xarg);
    boxPlotData_.yEdit->setText(yarg);

    boxOKSlot();
  }
  else if (plot == "parallel" || plot == "parallelplot") {
    plotTab_->setCurrentIndex(5);

    parallelPlotData_.xEdit->setText(xarg);
    parallelPlotData_.yEdit->setText(yarg);

    parallelOKSlot();
  }
}

//------

void
CQChartsTest::
pieOKSlot()
{
  int col1, col2;

  (void) lineEditValue(pieChartData_.labelEdit, col1, 0);
  (void) lineEditValue(pieChartData_.dataEdit , col2, 1);

  //---

  CQChartsPiePlot *plot = new CQChartsPiePlot(model_);

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  //---

  plot->setPalette(palettePlot_->gradientPalette());

  plot->show();
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

  CQChartsXYPlot *plot = new CQChartsXYPlot(model_);

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  plot->setYColumns(columns);

  plot->setNameColumn(ncol);

  plot->setBivariate(xyPlotData_.bivariateCheck->isChecked());

  plot->updateRange();

  //---

  plot->setPalette(palettePlot_->gradientPalette());

  plot->show();
}

void
CQChartsTest::
sunburstOKSlot()
{
  int col1, col2;

  (void) lineEditValue(sunburstData_.nameEdit , col1, 0);
  (void) lineEditValue(sunburstData_.valueEdit, col2, 1);

  //---

  CQChartsSunburstPlot *plot = new CQChartsSunburstPlot(model_);

  //---

  plot->setPalette(palettePlot_->gradientPalette());

  plot->show();
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

  CQChartsBarChartPlot *plot = new CQChartsBarChartPlot(model_);

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  plot->setYColumns(columns);

  plot->updateRange();

  //---

  plot->setPalette(palettePlot_->gradientPalette());

  plot->show();
}

void
CQChartsTest::
boxOKSlot()
{
  int col1, col2;

  (void) lineEditValue(boxPlotData_.xEdit, col1, 0);
  (void) lineEditValue(boxPlotData_.yEdit, col2, 1);

  //---

  CQChartsBoxPlot *plot = new CQChartsBoxPlot(model_);

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  plot->init();

  //---

  plot->setPalette(palettePlot_->gradientPalette());

  plot->show();
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

  CQChartsParallelPlot *plot = new CQChartsParallelPlot(model_);

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  plot->setYColumns(columns);

  plot->updateRange();

  //---

  plot->setPalette(palettePlot_->gradientPalette());

  plot->show();
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
loadTsv(const QString &filename)
{
  CQChartsTsv *tsv = new CQChartsTsv;

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
  model_ = model;

  table_->setModel(model_);

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

QSize
CQChartsTest::
sizeHint() const
{
  return QSize(1600, 1200);
}
