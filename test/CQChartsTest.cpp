#include <CQChartsTest.h>
#include <CQCharts.h>
#include <CQChartsTable.h>
#include <CQChartsTree.h>
#include <CQChartsCsv.h>
#include <CQChartsJson.h>
#include <CQChartsData.h>
#include <CQChartsPiePlot.h>
#include <CQChartsXYPlot.h>
#include <CQChartsSunburstPlot.h>
#include <CQChartsBarChartPlot.h>
#include <CQGradientPalette.h>
#include <CQGradientPaletteControl.h>
#include <CQApp.h>

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
  bool json = false;
  bool data = false;

  std::vector<QString> filenames;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      std::string arg = &argv[i][1];

      if      (arg == "csv")
        csv  = true;
      else if (arg == "json")
        json = true;
      else if (arg == "data")
        data = true;
    }
    else
      filenames.push_back(argv[i]);
  }

  //---

  CQCharts charts;

  charts.init();

  CQChartsTest test;

  if (filenames.size() > 0) {
    if      (csv)
      test.loadCsv(filenames[0]);
    else if (json)
      test.loadJson(filenames[0]);
    else if (data)
      test.loadData(filenames[0]);
    else
      std::cerr << "No plot type specified" << std::endl;
  }

  test.show();

  app.exec();

  return 0;
}

//-----

CQChartsTest::
CQChartsTest()
{
  QHBoxLayout *layout = new QHBoxLayout(this);

  QSplitter *splitter = new QSplitter;

  layout->addWidget(splitter);

  //---

  QFrame *plotFrame = new QFrame;

  QVBoxLayout *plotLayout = new QVBoxLayout(plotFrame);

  splitter->addWidget(plotFrame);

  //---

  // plots (one per tab)
  QTabWidget *plotTab = new QTabWidget;

  plotLayout->addWidget(plotTab);

  plotTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  addPieTab     (plotTab);
  addXYTab      (plotTab);
  addSunburstTab(plotTab);
  addBarChartTab(plotTab);

  //---

  // table/tree widgets
  stack_ = new QStackedWidget;

  table_ = new CQChartsTable;
  tree_  = new CQChartsTree;

  stack_->addWidget(table_);
  stack_->addWidget(tree_);

  plotLayout->addWidget(stack_);

  connect(table_, SIGNAL(columnClicked(int)), this, SLOT(tableColumnClicked(int)));

  //---

  typeGroup_ = new QGroupBox;

  typeGroup_->setTitle("Column Type");

  plotLayout->addWidget(typeGroup_);

  QVBoxLayout *typeGroupLayout = new QVBoxLayout(typeGroup_);

  QFrame *columnFrame = new QFrame;

  typeGroupLayout->addWidget(columnFrame);

  QGridLayout *columnLayout = new QGridLayout(columnFrame);

  int row = 0;

  columnTypeEdit_ = addLineEdit(columnLayout, row, "Type");

  QPushButton *typeOKButton = new QPushButton("Set");

  connect(typeOKButton, SIGNAL(clicked()), this, SLOT(typeOKSlot()));

  columnLayout->addWidget(typeOKButton);

  //---

  QFrame *gradientFrame = new QFrame;

  QVBoxLayout *gradientLayout = new QVBoxLayout(gradientFrame);

  palettePlot_    = new CQGradientPalette(this, nullptr);
  paletteControl_ = new CQGradientPaletteControl(palettePlot_);

  gradientLayout->addWidget(palettePlot_);
  gradientLayout->addWidget(paletteControl_);

  splitter->addWidget(gradientFrame);
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

void
CQChartsTest::
addPieTab(QTabWidget *plotTab)
{
  QFrame *pieFrame = new QFrame;

  QVBoxLayout *pieFrameLayout = new QVBoxLayout(pieFrame);

  //---

  QGridLayout *pieEditLayout = new QGridLayout;

  pieFrameLayout->addLayout(pieEditLayout);

  int row = 0;

  pieLabelEdit_ = addLineEdit(pieEditLayout, row, "Label");
  pieDataEdit_  = addLineEdit(pieEditLayout, row, "Data" );

  //---

  QHBoxLayout *pieButtonLayout = new QHBoxLayout;

  pieButtonLayout->addWidget (pieOKButton_ = new QPushButton("OK"));
  pieButtonLayout->addStretch(1);

  connect(pieOKButton_, SIGNAL(clicked()), this, SLOT(pieOKSlot()));

  pieFrameLayout->addLayout(pieButtonLayout);

  //---

  plotTab->addTab(pieFrame, "Pie");
}

void
CQChartsTest::
addXYTab(QTabWidget *plotTab)
{
  QFrame *xyFrame = new QFrame;

  QVBoxLayout *xyFrameLayout = new QVBoxLayout(xyFrame);

  //---

  QGridLayout *xyEditLayout = new QGridLayout;

  xyFrameLayout->addLayout(xyEditLayout);

  int row = 0;

  xyXEdit_ = addLineEdit(xyEditLayout, row, "X");
  xyYEdit_ = addLineEdit(xyEditLayout, row, "Y" );

  xyEditLayout->addWidget(xyBivariateCheck_ = new QCheckBox("Bivariate"), row, 0, 1, 2);

  //---

  QHBoxLayout *xyButtonLayout = new QHBoxLayout;

  xyButtonLayout->addWidget (xyOKButton_ = new QPushButton("OK"));
  xyButtonLayout->addStretch(1);

  connect(xyOKButton_, SIGNAL(clicked()), this, SLOT(xyOKSlot()));

  xyFrameLayout->addLayout(xyButtonLayout);

  //---

  plotTab->addTab(xyFrame, "XY");
}

void
CQChartsTest::
addSunburstTab(QTabWidget *plotTab)
{
  QFrame *sunburstFrame = new QFrame;

  QVBoxLayout *sunburstFrameLayout = new QVBoxLayout(sunburstFrame);

  //---

  QGridLayout *sunburstEditLayout = new QGridLayout;

  sunburstFrameLayout->addLayout(sunburstEditLayout);

  int row = 0;

  sunburstNameEdit_  = addLineEdit(sunburstEditLayout, row, "Name" );
  sunburstValueEdit_ = addLineEdit(sunburstEditLayout, row, "Value");

  //---

  QHBoxLayout *sunburstButtonLayout = new QHBoxLayout;

  sunburstButtonLayout->addWidget (sunburstOKButton_ = new QPushButton("OK"));
  sunburstButtonLayout->addStretch(1);

  connect(sunburstOKButton_, SIGNAL(clicked()), this, SLOT(sunburstOKSlot()));

  sunburstFrameLayout->addLayout(sunburstButtonLayout);

  //---

  plotTab->addTab(sunburstFrame, "Sunburst");
}

void
CQChartsTest::
addBarChartTab(QTabWidget *plotTab)
{
  QFrame *barChartFrame = new QFrame;

  QVBoxLayout *barChartFrameLayout = new QVBoxLayout(barChartFrame);

  //---

  QGridLayout *barChartEditLayout = new QGridLayout;

  barChartFrameLayout->addLayout(barChartEditLayout);

  int row = 0;

  barChartNameEdit_  = addLineEdit(barChartEditLayout, row, "Name" );
  barChartValueEdit_ = addLineEdit(barChartEditLayout, row, "Value");

  //---

  QHBoxLayout *barChartButtonLayout = new QHBoxLayout;

  barChartButtonLayout->addWidget (barChartOKButton_ = new QPushButton("OK"));
  barChartButtonLayout->addStretch(1);

  connect(barChartOKButton_, SIGNAL(clicked()), this, SLOT(barChartOKSlot()));

  barChartFrameLayout->addLayout(barChartButtonLayout);

  //---

  plotTab->addTab(barChartFrame, "Bar Chart");
}

void
CQChartsTest::
pieOKSlot()
{
  int col1, col2;

  (void) lineEditValue(pieLabelEdit_, col1, 0);
  (void) lineEditValue(pieDataEdit_ , col2, 1);

  //---

  CQChartsPiePlot *plot = new CQChartsPiePlot(model_);

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  plot->setPalette(palettePlot_->gradientPalette());

  plot->show();
}

void
CQChartsTest::
xyOKSlot()
{
  int col1, col2;

  (void)     lineEditValue(xyXEdit_, col1, 0);
  bool ok2 = lineEditValue(xyYEdit_, col2, 1);

  CQChartsXYPlot::Columns columns;

  if (! ok2) {
    QStringList strs = xyYEdit_->text().split(" ", QString::SkipEmptyParts);

    for (int i = 0; i < strs.size(); ++i) {
      bool ok;

      int col = strs[i].toInt(&ok);

      if (ok)
        columns.push_back(col);
    }
  }

  //---

  CQChartsXYPlot *plot = new CQChartsXYPlot(model_);

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  plot->setYColumns(columns);

  plot->setBivariate(xyBivariateCheck_->isChecked());

  plot->updateRange();

  plot->show();
}

void
CQChartsTest::
sunburstOKSlot()
{
  int col1, col2;

  (void) lineEditValue(sunburstNameEdit_ , col1, 0);
  (void) lineEditValue(sunburstValueEdit_, col2, 1);

  //---

  CQChartsSunburstPlot *plot = new CQChartsSunburstPlot(model_);

  plot->show();
}

void
CQChartsTest::
barChartOKSlot()
{
  int col1, col2;

  (void) lineEditValue(barChartNameEdit_ , col1, 0);
  (void) lineEditValue(barChartValueEdit_, col2, 1);

  //---

  CQChartsPlotBarChart *plot = new CQChartsPlotBarChart(model_);

  plot->setXColumn(col1);
  plot->setYColumn(col2);

  plot->updateRange();

  plot->show();
}

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

QLineEdit *
CQChartsTest::
addLineEdit(QGridLayout *grid, int &row, const QString &name) const
{
  QLabel    *label = new QLabel(name);
  QLineEdit *edit  = new QLineEdit;

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
