#include <CQChartsModelWidgets.h>
#include <CQChartsModelList.h>
#include <CQChartsModelControl.h>
#include <CQCharts.h>
#include <CQUtil.h>
#include <CQTabSplit.h>

#include <QVBoxLayout>

CQChartsModelWidgets::
CQChartsModelWidgets(CQCharts *charts, QWidget *parent) :
 QFrame(parent), charts_(charts)
{
  setObjectName("modelWidgets");

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //---

  CQTabSplit *split = CQUtil::makeWidget<CQTabSplit>("split");

  split->setOrientation(Qt::Vertical);

  layout->addWidget(split);

  //---

  // create models list
  modelList_ = new CQChartsModelList(charts_);

  split->addWidget(modelList_, "Model/Details");

  //---

  // create current model control
  modelControl_ = new CQChartsModelControl(charts_);

  split->addWidget(modelControl_, "Control");

  //---

  connect(charts_, SIGNAL(currentModelChanged(int)),
          modelControl_, SLOT(updateCurrentModel()));
}

void
CQChartsModelWidgets::
addModelData(CQChartsModelData *modelData)
{
  modelList_->addModelData(modelData);
}
