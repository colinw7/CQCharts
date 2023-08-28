#include <CQChartsPlotPropertiesWidget.h>
#include <CQChartsPropertyViewTree.h>
#include <CQChartsPropertyViewTreeFilterEdit.h>
#include <CQChartsTitleEdit.h>
#include <CQChartsKeyEdit.h>
#include <CQChartsAxisEdit.h>
#include <CQChartsKey.h>
#include <CQChartsPlot.h>
#include <CQChartsModelData.h>
#include <CQChartsWidgetUtil.h>

#include <CQTabWidget.h>
#include <CQUtil.h>

#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDir>

#include <fstream>

CQChartsPlotPropertiesControl::
CQChartsPlotPropertiesControl(View *view) :
 QFrame()
{
  setObjectName("plotPropertiesControl");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //--

  // create tab for plots (TODO: use CQTabSplit)
  plotsTab_ = CQUtil::makeWidget<CQTabWidget>("tab");

  layout->addWidget(plotsTab_);

  connect(plotsTab_, SIGNAL(currentChanged(int)), this, SLOT(tabChangedSlot()));

  //--

  auto createPushButton = [&](const QString &label, const QString &objName,
                              const QString &tipStr, const char *slotName) {
    auto *button = CQUtil::makeLabelWidget<QPushButton>(label, objName);

    button->setToolTip(tipStr);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  //---

  auto *editFrame       = CQUtil::makeWidget<QFrame>("editFrame");
  auto *editFrameLayout = CQUtil::makeLayout<QHBoxLayout>(editFrame, 2, 2);

  auto *titleButton =
    createPushButton("Edit Title..." , "title", "Edit Plot Title"  , SLOT(editTitleSlot()));
  auto *keyButton   =
    createPushButton("Edit Key..."   , "key"  , "Edit Plot Key"    , SLOT(editKeySlot()));
  auto *xAxisButton =
    createPushButton("Edit X Axis...", "xaxis", "Edit Plot X Axis" , SLOT(editXAxisSlot()));
  auto *yAxisButton =
    createPushButton("Edit Y Axis...", "yaxis", "Edit Plot Y Axis" , SLOT(editYAxisSlot()));
  auto *writeButton =
    createPushButton("Write"         , "write", "Write Plot Script", SLOT(writeSlot()));

  editFrameLayout->addWidget(titleButton);
  editFrameLayout->addWidget(keyButton);
  editFrameLayout->addWidget(xAxisButton);
  editFrameLayout->addWidget(yAxisButton);
  editFrameLayout->addWidget(CQChartsWidgetUtil::createHSpacer(1));
  editFrameLayout->addWidget(writeButton);
  editFrameLayout->addStretch(1);

  layout->addWidget(editFrame);

  //---

  setView(view);
}

CQChartsView *
CQChartsPlotPropertiesControl::
view() const
{
  return view_;
}

void
CQChartsPlotPropertiesControl::
setView(View *view)
{
  view_ = view;

  //---

  updatePlots();
}

void
CQChartsPlotPropertiesControl::
updatePlots()
{
  if (! view_) return;

  //--

  using PlotSet = std::set<CQChartsPlot *>;

  int np = view_->numPlots();

  PlotSet plotSet;

  for (int i = 0; i < np; ++i)
    plotSet.insert(view_->plot(i));

  //---

  // add new property view for new plots
  for (int i = 0; i < np; ++i) {
    auto *plot = view_->plot(i);

    int ind;
    (void) getPlotPropertyViewTree(plot, ind);

    if (ind < 0)
      (void) addPlotPropertyViewTree(plot);
  }

  //---

  // update tab order
  struct PlotData {
    CQChartsPlot*         plot             { nullptr };
    PlotPropertiesWidget* propertiesWidget { nullptr };
  };

  using PlotWidgets = std::map<CQChartsPlot *, PlotPropertiesWidget *>;

  PlotWidgets plotWidgets;

  for (int i = 0; i < plotsTab_->count(); ++i) {
    auto *plotWidget = qobject_cast<PlotPropertiesWidget *>(plotsTab_->widget(i));
    assert(plotWidget);

    plotWidgets[plotWidget->plot()] = plotWidget;
  }

  using PlotList = std::vector<CQChartsPlot *>;

  PlotList plotList;

  for (int i = 0; i < np; ++i) {
    auto *plot = view_->plot(i);

    if (! plot->subPlot())
      plotList.push_back(plot);
  }

  for (int i = 0; i < np; ++i) {
    auto *plot = view_->plot(i);

    if (plot->subPlot())
      plotList.push_back(plot);
  }

  plotsTab_->clear();

  for (auto *plot : plotList)
    (void) plotsTab_->addTab(plotWidgets[plot], plot->id());

  //---

  // remove existing property views for deleted plots
  updatePlotPropertyViewTrees(plotSet);
}

CQChartsPropertyViewTree *
CQChartsPlotPropertiesControl::
getPlotPropertyViewTree(Plot *plot, int &ind) const
{
  ind = -1;

  for (int i = 0; i < plotsTab_->count(); ++i) {
    auto *plotWidget = qobject_cast<PlotPropertiesWidget *>(plotsTab_->widget(i));
    assert(plotWidget);

    if (plotWidget->plot() == plot) {
      ind = i;
      return plotWidget->propertyTree();
    }
  }

  return nullptr;
}

int
CQChartsPlotPropertiesControl::
addPlotPropertyViewTree(Plot *plot)
{
  auto *plotWidget = new PlotPropertiesWidget(plot);

  plotWidget->setObjectName(QString("plotTabWidget_%1").  arg(plotsTab_->count() + 1));

  connect(plotWidget, SIGNAL(propertyItemSelected(QObject *, const QString &)),
          this, SIGNAL(propertyItemSelected(QObject *, const QString &)));

  int ind = plotsTab_->addTab(plotWidget, plot->id());

  //plotsTab_->setTabToolTip(ind, plot->type()->description());

  return ind;
}

void
CQChartsPlotPropertiesControl::
updatePlotPropertyViewTrees(const PlotSet &plotSet)
{
  using PlotWidgets = std::vector<PlotPropertiesWidget *>;

  PlotWidgets plotWidgets;

  for (int i = 0; i < plotsTab_->count(); ++i) {
    auto *plotWidget = qobject_cast<PlotPropertiesWidget *>(plotsTab_->widget(i));
    assert(plotWidget);

    if (plotSet.find(plotWidget->plot()) == plotSet.end())
      plotWidgets.push_back(plotWidget);
  }

  for (auto &plotWidget : plotWidgets) {
    int ind = plotsTab_->indexOf(plotWidget);
    assert(ind >= 0);

    plotsTab_->removeTab(ind);

    delete plotWidget;
  }
}

CQChartsPlot *
CQChartsPlotPropertiesControl::
getCurrentPlot() const
{
  auto *plotWidget = qobject_cast<PlotPropertiesWidget *>(plotsTab_->currentWidget());
  if (! plotWidget) return nullptr;

  return plotWidget->plot();
}

void
CQChartsPlotPropertiesControl::
tabChangedSlot()
{
  int ind = plotsTab_->currentIndex();
  if (ind < 0) return; // no plot

  auto *plotWidget = qobject_cast<PlotPropertiesWidget *>(plotsTab_->widget(ind));
  assert(plotWidget);

//propertiesWidgets_.plotTip->setPlot(plotWidget->plot());
}

void
CQChartsPlotPropertiesControl::
editTitleSlot()
{
  auto *plot = getCurrentPlot();

  if (! plot || ! plot->title())
    return;

  if (editTitleDlg_)
    delete editTitleDlg_;

  editTitleDlg_ = new CQChartsEditTitleDlg(this, plot->title());

  editTitleDlg_->show();
  editTitleDlg_->raise();
}

void
CQChartsPlotPropertiesControl::
editKeySlot()
{
  auto *plot = getCurrentPlot();

  if (! plot || ! plot->key())
    return;

  if (editKeyDlg_)
    delete editKeyDlg_;

  editKeyDlg_ = new CQChartsEditKeyDlg(this, plot->key());

  editKeyDlg_->show();
  editKeyDlg_->raise();
}

void
CQChartsPlotPropertiesControl::
editXAxisSlot()
{
  auto *plot = getCurrentPlot();

  if (! plot || ! plot->xAxis())
    return;

  if (editXAxisDlg_)
    delete editXAxisDlg_;

  editXAxisDlg_ = new CQChartsEditAxisDlg(this, plot->xAxis());

  editXAxisDlg_->show();
  editXAxisDlg_->raise();
}

void
CQChartsPlotPropertiesControl::
editYAxisSlot()
{
  auto *plot = getCurrentPlot();

  if (! plot || ! plot->yAxis())
    return;

  if (editYAxisDlg_)
    delete editYAxisDlg_;

  editYAxisDlg_ = new CQChartsEditAxisDlg(this, plot->yAxis());

  editYAxisDlg_->show();
  editYAxisDlg_->raise();
}

void
CQChartsPlotPropertiesControl::
writeSlot()
{
  auto *plot = getCurrentPlot();
  if (! plot) return;

  auto dir = QDir::current().dirName() + "/plot.tcl";

  auto filename = QFileDialog::getSaveFileName(this, "Write View", dir, "Files (*.tcl)");
  if (! filename.length()) return; // cancelled

  auto fs = std::ofstream(filename.toStdString(), std::ofstream::out);

  //---

  auto *view = plot->view();

  view->write(fs);

  auto *modelData = plot->currentModelData();

  if (modelData)
    modelData->write(fs);

  plot->write(fs);
}

//------

CQChartsPlotPropertiesWidget::
CQChartsPlotPropertiesWidget(CQChartsPlot *plot)
{
  setObjectName("plotPropertiesWidget");

  CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //--

  setPlot(plot);
}

CQChartsPlot *
CQChartsPlotPropertiesWidget::
plot() const
{
  return plot_.data();
}

void
CQChartsPlotPropertiesWidget::
setPlot(CQChartsPlot *plot)
{
  if (plot_ == plot)
    return;

  plot_ = plot;
  if (! plot_) return;

  //---

  auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
  assert(layout);

  propertyTree_ = new CQChartsPropertyViewTree(this, plot_->propertyModel());

  propertyTree_->setObjectName("propertyTree");

  propertyTree_->setPlot(plot);

  connect(propertyTree_, SIGNAL(itemSelected(QObject *, const QString &)),
          this, SIGNAL(propertyItemSelected(QObject *, const QString &)));

  connect(propertyTree_, SIGNAL(filterStateChanged(bool, bool)),
          this, SLOT(filterStateSlot(bool, bool)));

  //--

  filterEdit_ = new CQChartsPropertyViewTreeFilterEdit(propertyTree_);

  filterEdit_->setVisible(propertyTree_->isFilterDisplayed());

  //--

  layout->addWidget(filterEdit_);
  layout->addWidget(propertyTree_);
}

void
CQChartsPlotPropertiesWidget::
filterStateSlot(bool visible, bool focus)
{
  if (! filterEdit_) return;

  filterEdit_->setVisible(visible);

  if (focus)
    filterEdit_->setFocus();
}
