#include <CQChartsModelDlg.h>
#include <CQChartsModelList.h>
#include <CQChartsModelControl.h>
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

  QVBoxLayout *layout = new QVBoxLayout(this);

  //---

  // create models list
  modelList_ = new CQChartsModelList(charts_);

  layout->addWidget(modelList_);

  //---

  // create current model control
  modelControl_ = new CQChartsModelControl(charts_);

  layout->addWidget(modelControl_);

  //---

  modelList_   ->setModelControl(modelControl_);
  modelControl_->setModelList   (modelList_);

  //----

  // Bottom Buttons
  QHBoxLayout *buttonLayout = new QHBoxLayout;

  QPushButton *doneButton = new QPushButton("Done");
  doneButton->setObjectName("done");

  connect(doneButton, SIGNAL(clicked()), this, SLOT(cancelSlot()));

  buttonLayout->addStretch(1);

  buttonLayout->addWidget(doneButton);

  layout->addLayout(buttonLayout);

  //----

  CQCharts::ModelDatas modelDatas;

  charts_->getModelDatas(modelDatas);

  for (const auto &modelData : modelDatas)
    modelList_->addModelData(modelData);
}

CQChartsModelDlg::
~CQChartsModelDlg()
{
}

void
CQChartsModelDlg::
cancelSlot()
{
  hide();
}
