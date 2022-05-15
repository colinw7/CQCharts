#include <CQChartsModelProxyControl.h>

#include <CQChartsBucketModelEdit.h>
#include <CQChartsHierSepModelEdit.h>
#include <CQChartsPivotModelEdit.h>
#include <CQChartsSummaryModelEdit.h>
#include <CQChartsTransposeModelEdit.h>

#include <CQChartsModelData.h>
#include <CQCharts.h>

#include <CQTabSplit.h>

CQChartsModelProxyControl::
CQChartsModelProxyControl(QWidget *parent) :
 QFrame(parent)
{
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  auto *split = CQUtil::makeWidget<CQTabSplit>("split");

  split->setOrientation(Qt::Vertical);
  split->setGrouped(true);
  split->setState(CQTabSplit::State::TAB);

  layout->addWidget(split);

  //---

  auto addFrame = [&](const QString &label, const QString &name) {
    auto *frame  =    CQUtil::makeWidget<QFrame>(name);
  /*auto *layout = */ CQUtil::makeLayout<QVBoxLayout>(frame, 2, 2);

    split->addWidget(frame, label);

    return frame;
  };

  //---

  // Create Bucket Model Proxy Frame
  auto bucketFrame = addFrame("Bucket", "bucketFrame");

  bucketEdit_ = new CQChartsBucketModelEdit(this);

  bucketFrame->layout()->addWidget(bucketEdit_);

  //---

  // Create HierSep Model Proxy Frame
  auto hierSepFrame = addFrame("Hier Sep", "hierSepFrame");

  hierSepEdit_ = new CQChartsHierSepModelEdit(this);

  hierSepFrame->layout()->addWidget(hierSepEdit_);

  //---

  // Create Pivot Model Proxy Frame
  auto pivotFrame = addFrame("Pivot", "pivotFrame");

  pivotEdit_ = new CQChartsPivotModelEdit(this);

  pivotFrame->layout()->addWidget(pivotEdit_);

  //---

  // Create Summary Model Proxy Frame
  auto summaryFrame = addFrame("Summary", "summaryFrame");

  summaryEdit_ = new CQChartsSummaryModelEdit(this);

  summaryFrame->layout()->addWidget(summaryEdit_);

  //---

  // Create Transpose Model Proxy Frame
  auto *transposeFrame = addFrame("Transpose", "transposeFrame");

  transposeEdit_ = new CQChartsTransposeModelEdit(this);

  transposeFrame->layout()->addWidget(transposeEdit_);

  //---

  split->setSizes(QList<int>({int(INT_MAX*0.2), int(INT_MAX*0.2),
                              int(INT_MAX*0.2), int(INT_MAX*0.2),
                              int(INT_MAX*0.2)}));
}

void
CQChartsModelProxyControl::
setModelData(CQChartsModelData *modelData)
{
  modelData_ = modelData;

  bucketEdit_   ->setModelData(modelData);
  hierSepEdit_  ->setModelData(modelData);
  pivotEdit_    ->setModelData(modelData);
  summaryEdit_  ->setModelData(modelData);
  transposeEdit_->setModelData(modelData);
}
