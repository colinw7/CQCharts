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

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //---

  auto *split = CQUtil::makeWidget<CQTabSplit>("split");

  split->setOrientation(Qt::Vertical);
  split->setGrouped(true);

  layout->addWidget(split);

  //---

  // create models list
  modelList_ = new CQChartsModelList(charts_);

  split->addWidget(modelList_, "Model/Details");

  //---

  // create current model control
  modelControl_ = new CQChartsModelControl(charts_);

  split->addWidget(modelControl_, "Control");

  connect(modelList_, SIGNAL(filterTextChanged(const QString &)),
          modelControl_, SLOT(filterTextSlot(const QString &)));

  //---

  connect(charts_, SIGNAL(currentModelChanged(int)),
          modelControl_, SLOT(updateCurrentModel()));

  //---

  int i1 = INT_MAX*0.6;
  int i2 = INT_MAX - i1;

  split->setSizes(QList<int>({i1, i2}));
}

void
CQChartsModelWidgets::
addModelData(CQChartsModelData *modelData)
{
  modelList_->addModelData(modelData);
}
