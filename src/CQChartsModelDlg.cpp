#include <CQChartsModelDlg.h>
#include <CQChartsModelWidgets.h>
#include <CQChartsModelList.h>
#include <CQChartsModelData.h>
#include <CQChartsPlotDlg.h>
#include <CQCharts.h>
#include <CQChartsUtil.h>

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

CQChartsModelDlg::
CQChartsModelDlg(CQCharts *charts) :
 charts_(charts)
{
  setObjectName("modelDlg");

  setWindowTitle("Manage Models");
  //setWindowIcon(QIcon()); TODO

  QVBoxLayout *layout = new QVBoxLayout(this);

  //---

  // create model widgets
  modelWidgets_ = new CQChartsModelWidgets(charts_);

  layout->addWidget(modelWidgets_);

  //---

  // Bottom Buttons
  QHBoxLayout *buttonLayout = new QHBoxLayout;

  QPushButton *plotButton = new QPushButton("Plot");
  plotButton->setObjectName("plot");

  connect(plotButton, SIGNAL(clicked()), this, SLOT(plotSlot()));

  QPushButton *doneButton = new QPushButton("Done");
  doneButton->setObjectName("done");

  connect(doneButton, SIGNAL(clicked()), this, SLOT(cancelSlot()));

  buttonLayout->addWidget(plotButton);
  buttonLayout->addStretch(1);
  buttonLayout->addWidget(doneButton);

  layout->addLayout(buttonLayout);

  //----

  CQCharts::ModelDatas modelDatas;

  charts_->getModelDatas(modelDatas);

  for (const auto &modelData : modelDatas)
    modelWidgets_->addModelData(modelData);
}

CQChartsModelDlg::
~CQChartsModelDlg()
{
}

void
CQChartsModelDlg::
plotSlot()
{
  CQChartsModelData *modelData = modelWidgets_->modelList()->currentModelData();

  delete plotDlg_;

  plotDlg_ = new CQChartsPlotDlg(charts_, modelData);

  plotDlg_->show();
}

void
CQChartsModelDlg::
cancelSlot()
{
  hide();
}
