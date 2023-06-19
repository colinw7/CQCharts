#include <CQChartsSankeyView.h>

#include <CQCharts.h>
#include <CQChartsSankeyPlot.h>
#include <CQCsvModel.h>
#include <CQChartsPlotType.h>
#include <CQChartsModelData.h>
#include <CQChartsLoader.h>
#include <CQChartsInputData.h>
#include <CQChartsFile.h>

MainWindow::MainWindow() :
 QMainWindow(nullptr)
{
  // create charts
  charts_ = new CQCharts();

  charts_->init();

  // add view
  auto *view = charts_->addView();

  setCentralWidget(view);

  //---

  // load model
  CQChartsInputData inputData;

  inputData.firstLineHeader = true;

  CQChartsLoader loader(charts_);

  bool hierarchical = false;
  CQChartsFile file(charts_, QString("sankey_energy.csv") );
  auto *model = loader.loadFile(file, CQChartsFileType::CSV, inputData, hierarchical);

  CQCharts::ModelP modelp;

  if (model) {
    modelp = CQCharts::ModelP(model);

    // register model with charts
    auto *modelData = charts_->initModelData(modelp);

    charts_->setModelFileName(modelData, file.resolve());
  }

  //---

  // create plot
  auto *plotType = charts_->plotType("sankey");

  auto *plot = plotType->createAndInit(view, modelp);

  view->addPlot(plot);

  sankeyPlot_ = dynamic_cast<CQChartsSankeyPlot *>(plot);
  assert(sankeyPlot_);

  sankeyPlot_->setLinkColumn (CQChartsColumn(0));
  sankeyPlot_->setValueColumn(CQChartsColumn(1));
}

MainWindow::
~MainWindow()
{
  delete charts_;
}
