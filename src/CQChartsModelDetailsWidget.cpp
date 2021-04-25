#include <CQChartsModelDetailsWidget.h>
#include <CQChartsModelDetailsTable.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQCharts.h>

#include <CQUtil.h>
#include <CQLabel.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>

CQChartsModelDetailsWidget::
CQChartsModelDetailsWidget(CQCharts *charts) :
 charts_(charts)
{
  setObjectName("modelDetailsWidget");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //---

  auto *controlLayout = CQUtil::makeLayout<QHBoxLayout>(2, 2);

  //--

  // model label
  modelLabel_ = CQUtil::makeLabelWidget<QLabel>("");

  // update button
  updateButton_ = CQUtil::makeLabelWidget<QPushButton>("Update", "update");

  updateButton_->setToolTip("Update details from model data");

  connect(updateButton_, SIGNAL(clicked()), this, SLOT(updateSlot()));

  //--

  controlLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("<b>Model:</b> "));
  controlLayout->addWidget(modelLabel_);
  controlLayout->addStretch(1);
  controlLayout->addWidget(updateButton_);

  layout->addLayout(controlLayout);

  //---

  auto *summaryLayout = CQUtil::makeLayout<QHBoxLayout>(2, 2);

  // summary labels
  numColumnsLabel_ = CQUtil::makeLabelWidget<CQLabel>("0");
  numRowsLabel_    = CQUtil::makeLabelWidget<CQLabel>("0");
  hierLabel_       = CQUtil::makeLabelWidget<QLabel>("No");

  //--

  // flip check
  flipCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Flip", "flip");

  flipCheck_->setToolTip("Flip orientation of details table");

  connect(flipCheck_, SIGNAL(stateChanged(int)), this, SLOT(flipSlot(int)));

  //--

  summaryLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("<b>Columns:</b> "));
  summaryLayout->addWidget(numColumnsLabel_);
  summaryLayout->addWidget(CQUtil::makeLabelWidget<QLabel>(" "));
  summaryLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("<b>Rows:</b> "));
  summaryLayout->addWidget(numRowsLabel_);
  summaryLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("<b>Hier:</b> "));
  summaryLayout->addWidget(hierLabel_);
  summaryLayout->addStretch(1);
  summaryLayout->addWidget(flipCheck_);

  layout->addLayout(summaryLayout);

  //--

  detailsTable_ = new CQChartsModelDetailsTable();

  detailsTable_->setAutoUpdate(false);

  layout->addWidget(detailsTable_);
}

bool
CQChartsModelDetailsWidget::
isFlip() const
{
  return detailsTable_->isFlip();
}

void
CQChartsModelDetailsWidget::
setFlip(bool b)
{
  detailsTable_->setFlip(b);
}

void
CQChartsModelDetailsWidget::
setModelData(CQChartsModelData *modelData, bool invalidate)
{
  if (modelData != modelData_) {
    if (modelData_)
      disconnect(modelData_, SIGNAL(modelChanged()), this, SLOT(invalidateSlot()));

    modelData_ = modelData;

    detailsTable_->setModelData(modelData_);

    if (modelData_)
      connect(modelData_, SIGNAL(modelChanged()), this, SLOT(invalidateSlot()));
  }

  if (invalidate)
    this->invalidate();
}

void
CQChartsModelDetailsWidget::
invalidateSlot()
{
  invalidate();
}

void
CQChartsModelDetailsWidget::
invalidate()
{
  if (! modelData_)
    return;

  auto *details = modelData_->details();

  updateButton_->setEnabled(details);
}

void
CQChartsModelDetailsWidget::
flipSlot(int state)
{
  setFlip(state);
}

void
CQChartsModelDetailsWidget::
updateSlot()
{
  if (! modelData_)
    return;

  auto *details = modelData_->details();

  //---

  modelLabel_->setText(QString("%1").arg(details->data()->ind()));

  updateButton_->setEnabled(false);

  //---

  int  nc     = details->numColumns    ();
  int  nr     = details->numRows       ();
  bool isHier = details->isHierarchical();

  numColumnsLabel_->setValue(nc);
  numRowsLabel_   ->setValue(nr);

  hierLabel_->setText(QString("%1").arg(isHier ? "Yes" : "No"));

  //---

  detailsTable_->checkedUpdate(/*force*/true);
}
