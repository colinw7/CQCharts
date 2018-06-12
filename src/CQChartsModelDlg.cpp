#include <CQChartsModelDlg.h>
#include <CQChartsModelWidgets.h>
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

  // create model widgets
  modelWidgets_ = new CQChartsModelWidgets(charts_);

  layout->addWidget(modelWidgets_);

  //---

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
    modelWidgets_->addModelData(modelData);
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
