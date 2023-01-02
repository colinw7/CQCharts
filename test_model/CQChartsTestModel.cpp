#include <CQChartsTestModel.h>
#include <CQLorenzModel.h>

#include <CQCharts.h>
#include <CQChartsPlotType.h>
#include <CQChartsWindow.h>
#include <CQChartsView.h>
#include <CQChartsScatterPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CHRTimer.h>

#include <QApplication>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  auto *test = new CQChartsTestPlot;

  test->show();

  return app.exec();
}

//---

CQChartsTestPlot::
CQChartsTestPlot(QWidget *parent) :
 QFrame(parent)
{
  charts_ = new CQCharts;

  charts_->init();

  view_   = charts_->addView();
  window_ = charts_->createWindow(view_);
  model_  = new CQChartsTestModel;

  CQCharts::ModelP modelp(model_->proxyModel());

  auto *modelData = charts_->initModelData(modelp);
  assert(modelData);

  auto *plotType = charts_->plotType("scatter");

  plot_ = plotType->createAndInit(view_, modelp);

  auto *scatterPlot = dynamic_cast<CQChartsScatterPlot *>(plot_);

  scatterPlot->setXColumn         (CQChartsColumn(0));
  scatterPlot->setYColumn         (CQChartsColumn(1));
  scatterPlot->setSymbolSizeColumn(CQChartsColumn(2));
  scatterPlot->setColorColumn     (CQChartsColumn(3));

  scatterPlot->setXMin(CQChartsPlot::OptReal(0.0));
  scatterPlot->setYMin(CQChartsPlot::OptReal(0.0));
  scatterPlot->setXMax(CQChartsPlot::OptReal(1000.0));
  scatterPlot->setYMax(CQChartsPlot::OptReal(1000.0));

  scatterPlot->xAxis()->setVisible(false);
  scatterPlot->yAxis()->setVisible(false);
  scatterPlot->key()->setVisible(false);

  scatterPlot->setSymbolSizeMapped(false);

//connect(scatterPlot, SIGNAL(updateAnimData()), this, SLOT(updateDataSlot()));
  connect(scatterPlot, SIGNAL(plotDrawn()), this, SLOT(updateDataSlot()));

  //---

  view_->addPlot(plot_);

  //---

  auto *layout = new QVBoxLayout(this);

  layout->addWidget(window_);

  //---

//scatterPlot->setAnimTick(750);
//scatterPlot->setAnimating(true);
}

void
CQChartsTestPlot::
updateDataSlot()
{
//if (lastTime_.getSecs() == 0) lastTime_ = CHRTime::getTime();
//auto currentTime = CHRTime::getTime();
//auto diffTime = lastTime_.diffTime(currentTime);
//std::cerr << "updateDataSlot " << diffTime.getMSecs() << "\n";

  model_->updateData();

  //plot_->invalidateModelData();

  //plot_->updateRangeAndObjs();

//lastTime_ = currentTime;
}

//---

CQChartsTestModel::
CQChartsTestModel() :
 CQDataModel(4, NUM_STARS)
{
  proxyModel_ = new QSortFilterProxyModel;

  proxyModel_->setObjectName("proxyModel");

  proxyModel_->setSortRole(static_cast<int>(Qt::EditRole));

  proxyModel_->setSourceModel(this);

  //---

  stars_.resize(NUM_STARS);

  updateData();
}

void
CQChartsTestModel::
updateData()
{
  beginResetModel();

  resetColumnTypes();

//CElapsedTimer elapsed("CQChartsTestModel::updateData");

  double w = 1000;
  double h = 1000;

  int i = 1;

  for (auto &star : stars_) {
    double r = std::min(99*i, 255);
    double g = std::min( 2*i, 255);
    double b = std::min(   i, 255);

    star.c = QColor(r, g, b, 255);

    double F = 260*(t_ + 9)/i + std::sin(i*i);

    star.x = w/2 +        i*std::sin(F);
    star.y = h/2 + 0.2*(2*i*std::cos(F) + 10.0*double(NUM_STARS)/double(i));
    star.r = std::sin(i)*8;

    star.y = h - star.y;

    int i1 = i - 1;

    setModelData(i1, 0, star.x);
    setModelData(i1, 1, star.y);
    setModelData(i1, 2, star.r);
    setModelData(i1, 3, star.c);

    ++i;
  }

  t_ += dt_;

  endResetModel();
}
