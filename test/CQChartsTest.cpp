#include <CQChartsTest.h>
#include <CQCharts.h>
#include <CQChartsTable.h>
#include <CQChartsTree.h>
#include <CQChartsCsv.h>
#include <CQChartsTsv.h>
#include <CQChartsJson.h>
#include <CQChartsGnuData.h>
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
#include <QStackedWidget>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
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
      else if (arg == "type") {
        ++i;

        if (i < argc)
          initData.typeName = argv[i];
      }
      else if (arg == "column" || arg == "columns") {
        ++i;

        if (i < argc) {
          QString columnsStr = argv[i];

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
              std::cerr << "Invalid " << argv[i] << " option '" << argv[i] << "'" << std::endl;
            }
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

  //---

  QFrame *plotFrame = CQUtil::makeWidget<QFrame>("plotFrame");

  QVBoxLayout *plotLayout = new QVBoxLayout(plotFrame);
  plotLayout->setMargin(0); plotLayout->setSpacing(2);

  layout->addWidget(plotFrame);

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

  QGroupBox *typeGroup = new QGroupBox;

  typeGroup->setObjectName("typeGroup");
  typeGroup->setTitle("Column Type");

  plotLayout->addWidget(typeGroup);

  QVBoxLayout *typeGroupLayout = new QVBoxLayout(typeGroup);

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

  model_.clear();
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

  setId(QString("%1").arg(i + 1));

  int r = i / initData.nc;
  int c = i % initData.nc;

  if (initData.overlay || initData.y1y2) {
    setBBox(CBBox2D(0, 0, 1000, 1000));
  }
  else {
    double x1 =  c     *initData.dx;
    double x2 = (c + 1)*initData.dx;
    double y1 =  r     *initData.dy;
    double y2 = (r + 1)*initData.dy;

    setBBox(CBBox2D(x1, 1000.0 - y2, x2, 1000.0 - y1));
  }

  //---

  model_.clear();

  if (initData.filenames.size() > 0) {
    if      (initData.csv) {
      QAbstractItemModel *csv = loadCsv(initData.filenames[0],
                                        initData.commentHeader,
                                        initData.firstLineHeader);

      model_ = ModelP(csv);

      setTableModel(model_);
    }
    else if (initData.tsv) {
      QAbstractItemModel *tsv = loadTsv(initData.filenames[0],
                                        initData.commentHeader,
                                        initData.firstLineHeader);

      model_ = ModelP(tsv);

      setTableModel(model_);
    }
    else if (initData.json) {
      bool hierarchical;

      QAbstractItemModel *json = loadJson(initData.filenames[0], hierarchical);

      model_ = ModelP(json);

      if (hierarchical)
        setTreeModel(model_);
      else
        setTableModel(model_);
    }
    else if (initData.data) {
      QAbstractItemModel *data = loadData(initData.filenames[0],
                                          initData.commentHeader,
                                          initData.firstLineHeader);

      model_ = ModelP(data);

      setTableModel(model_);
    }
    else {
      std::cerr << "No plot type specified" << std::endl;
    }
  }

  if (! model_)
    return false;

  //---

  CQChartsPlot *plot = init(model_, initData, i);

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

  model_.clear();

  if      (type == "CSV") {
    QAbstractItemModel *csv = loadCsv(filename, commentHeader, firstLineHeader);

    model_ = ModelP(csv);

    setTableModel(model_);
  }
  else if (type == "TSV") {
    QAbstractItemModel *tsv = loadTsv(filename, commentHeader, firstLineHeader);

    model_ = ModelP(tsv);

    setTableModel(model_);
  }
  else if (type == "Json") {
    bool hierarchical;

    QAbstractItemModel *json = loadJson(filename, hierarchical);

    model_ = ModelP(json);

    if (hierarchical)
      setTreeModel(model_);
    else
      setTableModel(model_);
  }
  else if (type == "Data") {
    QAbstractItemModel *data = loadData(filename, commentHeader, firstLineHeader);

    model_ = ModelP(data);

    setTableModel(model_);
  }
  else {
    std::cerr << "Bad type specified '" << type.toStdString() << "'" << std::endl;
    return;
  }
}

//------

void
CQChartsTest::
createSlot()
{
  if (! model_)
    return;

  //---

  CQChartsPlotDlg *dlg = new CQChartsPlotDlg(charts_, model_);

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

  plot->view()->show();
}

//------

void
CQChartsTest::
plotObjPressedSlot(CQChartsPlotObj *obj)
{
  QString id = obj->id();

  if (id.length())
    std::cerr << id.toStdString() << std::endl;
}

//------

void
CQChartsTest::
filterSlot()
{
  if (stack_->currentIndex() == 0)
    table_->setFilter(filterEdit_->text());
  else
    tree_->setFilter(filterEdit_->text());
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

CQChartsPlot *
CQChartsTest::
init(const ModelP &model, const InitData &initData, int i)
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

      if (stringToColumn(model, columnStr, column1))
        column = column1;
      else
        std::cerr << "Bad column name '" << columnStr.toStdString() << "'" << std::endl;

      type = type.mid(pos + 1).simplified();
    }

    if (! model->setHeaderData(column, Qt::Horizontal, type, CQCharts::Role::ColumnType)) {
      std::cerr << "Failed to set column type '" << type.toStdString() <<
                   "' for section '" << column << "'" << std::endl;
      continue;
    }
  }

  //---

  QString typeName = initData.typeName;

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

  // create plot from init (argument) data
  CQChartsPlot *plot = createPlot(model, type, initData, reuse);
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

CQChartsPlot *
CQChartsTest::
createPlot(const ModelP &model, CQChartsPlotType *type, const InitData &initData, bool reuse)
{
  CQChartsView *view = getView(reuse);

  //---

  CQChartsPlot *plot = type->create(view, model);

  connect(plot, SIGNAL(objPressed(CQChartsPlotObj *)),
          this, SLOT(plotObjPressedSlot(CQChartsPlotObj *)));

  //---

  // set plot property for widgets for plot parameters
  for (const auto &parameter : type->parameters()) {
    if      (parameter.type() == "column") {
      auto p = initData.nameValues.find(parameter.name());

      if (p == initData.nameValues.end())
        continue;

      int column;

      if (! stringToColumn(model, (*p).second, column)) {
        std::cerr << "Bad column name '" << (*p).second.toStdString() << "'" << std::endl;
        column = -1;
      }

      if (! CQUtil::setProperty(plot, parameter.propName(), QVariant(column)))
        std::cerr << "Failed to set parameter " << parameter.propName().toStdString() << std::endl;
    }
    else if (parameter.type() == "columns") {
      auto p = initData.nameValues.find(parameter.name());

      if (p == initData.nameValues.end())
        continue;

      QStringList strs = (*p).second.split(" ", QString::SkipEmptyParts);

      std::vector<int> columns;

      for (int j = 0; j < strs.size(); ++j) {
        int column;

        if (! stringToColumn(model, strs[j], column)) {
          std::cerr << "Bad column name '" << strs[j].toStdString() << "'" << std::endl;
          continue;
        }

        columns.push_back(column);
      }

      QString s = CQChartsUtil::toString(columns);

      if (! CQUtil::setProperty(plot, parameter.propName(), QVariant(s)))
        std::cerr << "Failed to set parameter " << parameter.propName().toStdString() << std::endl;
    }
    else if (parameter.type() == "bool") {
      auto p = initData.nameBools.find(parameter.name());

      if (p == initData.nameBools.end())
        continue;

      bool b = (*p).second;

      if (! CQUtil::setProperty(plot, parameter.propName(), QVariant(b)))
        std::cerr << "Failed to set parameter " << parameter.propName().toStdString() << std::endl;
    }
    else
      assert(false);
  }

  //---

  // init plot
  if (initData.typeName == "xy") {
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

QAbstractItemModel *
CQChartsTest::
loadCsv(const QString &filename, bool commentHeader, bool firstLineHeader)
{
  CQChartsCsv *csv = new CQChartsCsv(charts_);

  csv->setCommentHeader  (commentHeader);
  csv->setFirstLineHeader(firstLineHeader);

  csv->load(filename);

  return csv;
}

QAbstractItemModel *
CQChartsTest::
loadTsv(const QString &filename, bool commentHeader, bool firstLineHeader)
{
  CQChartsTsv *tsv = new CQChartsTsv(charts_);

  tsv->setCommentHeader  (commentHeader);
  tsv->setFirstLineHeader(firstLineHeader);

  tsv->load(filename);

  return tsv;
}

QAbstractItemModel *
CQChartsTest::
loadJson(const QString &filename, bool &hierarchical)
{
  CQChartsJson *json = new CQChartsJson(charts_);

  json->load(filename);

  hierarchical = json->isHierarchical();

  return json;
}

QAbstractItemModel *
CQChartsTest::
loadData(const QString &filename, bool commentHeader, bool firstLineHeader)
{
  CQChartsGnuData *data = new CQChartsGnuData(charts_);

  data->setCommentHeader  (commentHeader);
  data->setFirstLineHeader(firstLineHeader);

  data->load(filename);

  return data;
}

//------

void
CQChartsTest::
setTableModel(const ModelP &model)
{
  table_->setModel(model);

  stack_->setCurrentIndex(0);
}

void
CQChartsTest::
setTreeModel(const ModelP &model)
{
  tree_->setModel(model);

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
stringToColumn(const ModelP &model, const QString &str, int &column) const
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

  for (int column1 = 0; column1 < model->columnCount(); ++column1) {
    QVariant var = model->headerData(column1, Qt::Horizontal, Qt::DisplayRole);

    if (! var.isValid())
      continue;

    if (var.toString() == str) {
      column = column1;
      return true;
    }
  }

  return false;
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
      view_ = charts_->addView();
  }
  else {
    view_ = charts_->addView();
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
