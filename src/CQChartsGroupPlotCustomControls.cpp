#include <CQChartsGroupPlotCustomControls.h>
#include <CQChartsGroupPlot.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsModelDetails.h>

#include <CQUtil.h>
#include <CQIconRadio.h>
#include <CQDoubleRangeSlider.h>
#include <CQRealSpin.h>
#include <CQIntegerSpin.h>
#include <CQGroupBox.h>
#include <CQLabel.h>

#include <QLineEdit>
#include <QHBoxLayout>

CQChartsGroupPlotCustomControls::
CQChartsGroupPlotCustomControls(CQCharts *charts, const QString &plotType) :
 CQChartsPlotCustomControls(charts, plotType)
{
}

void
CQChartsGroupPlotCustomControls::
addGroupColumnWidgets()
{
  // group group
  groupFrame_ = createGroupFrame("Group", "groupFrame");

  auto *bucketCornerFrame  = CQUtil::makeWidget<QFrame>("bucketCornerFrame");
  auto *bucketCornerLayout = CQUtil::makeLayout<QHBoxLayout>(bucketCornerFrame, 0, 2);

  bucketRadioGroup_ = new QButtonGroup(this);

  auto createBucketRadio = [&](const QString &name, const QString &icon, const QString &tip) {
    auto *radio = CQUtil::makeWidget<CQIconRadio>(name);

    radio->setIcon(icon);

    radio->setToolTip(tip);

    bucketRadioGroup_->addButton(radio);

    bucketCornerLayout->addWidget(radio);

    return radio;
  };

  uniqueBucketRadio_ =
    createBucketRadio("uniqueBucketRadio", "BUCKET_UNIQUE", "Bucket per Unique Value");
  fixedBucketRadio_  =
    createBucketRadio("fixedBucketRadio", "BUCKET_FIXED", "Bucket from Start and Delta");
  rangeBucketRadio_  =
    createBucketRadio("rangeBucketRadio", "BUCKET_RANGE", "Bucket from Range and Number");
  stopsBucketRadio_  =
    createBucketRadio("stopsBucketRadio", "BUCKET_STOPS", "Buckets for specified Stops");

  groupFrame_.groupBox->setCornerWidget(bucketCornerFrame);

  //---

  groupColumnCombo_ = CQUtil::makeWidget<CQChartsColumnCombo>("groupColumnCombo");

  addFrameWidget(groupFrame_, "Column", groupColumnCombo_);

  //---

  bucketRange_     = CQUtil::makeWidget<CQDoubleRangeSlider>("bucketRange");
  startBucketEdit_ = CQUtil::makeWidget<CQRealSpin>("startBucketEdit");
  deltaBucketEdit_ = CQUtil::makeWidget<CQRealSpin>("deltaBucketEdit");
  numBucketsEdit_  = CQUtil::makeWidget<CQIntegerSpin>("numBucketsEdit");
  bucketStopsEdit_ = CQUtil::makeWidget<QLineEdit>("bucketStopsEdit");
  uniqueCount_     = CQUtil::makeWidget<CQLabel>("uniqueCount");
  rangeLabel_      = CQUtil::makeWidget<QLabel>("rangeLabel");

  bucketRange_    ->setToolTip("Bucket Range");
  startBucketEdit_->setToolTip("Bucket Start");
  deltaBucketEdit_->setToolTip("Bucket Delta");
  numBucketsEdit_ ->setToolTip("Number of Buckets");
  bucketStopsEdit_->setToolTip("Bucket Stops");
  uniqueCount_    ->setToolTip("Number of Unique Values");
  rangeLabel_     ->setToolTip("Value Range");

  addFrameWidget(groupFrame_, "Range"      , bucketRange_);
  addFrameWidget(groupFrame_, "Start"      , startBucketEdit_);
  addFrameWidget(groupFrame_, "Delta"      , deltaBucketEdit_);
  addFrameWidget(groupFrame_, "Num Buckets", numBucketsEdit_);
  addFrameWidget(groupFrame_, "Stops"      , bucketStopsEdit_);
  addFrameWidget(groupFrame_, "Num Unique" , uniqueCount_);
  addFrameWidget(groupFrame_, "Value Range", rangeLabel_);

  //---

  //addFrameRowStretch(groupFrame_);
}

void
CQChartsGroupPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_)
    disconnect(plot_, SIGNAL(groupCustomDataChanged()), this, SLOT(updateWidgets()));

  plot_ = dynamic_cast<CQChartsGroupPlot *>(plot);

  CQChartsPlotCustomControls::setPlot(plot);

  if (plot_)
    connect(plot_, SIGNAL(groupCustomDataChanged()), this, SLOT(updateWidgets()));
}

void
CQChartsGroupPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  if (groupColumnCombo_) {
    groupColumnCombo_->setModelColumn(plot_->getModelData(), plot_->groupColumn());

    //---

    auto *details = plot_->columnDetails(plot_->groupColumn());

    bool isUnique   = plot_->isExactBucketValue();
    bool isString   = (details && details->type() == CQBaseModelType::STRING);
    bool isNumeric  = (details && details->isNumeric());
    bool isIntegral = (details && details->type() == CQBaseModelType::INTEGER);
    bool isFixed    = (plot_->bucketType() == CQBucketer::Type::REAL_RANGE);
    bool isAuto     = (plot_->bucketType() == CQBucketer::Type::REAL_AUTO);
    bool isStops    = (plot_->bucketType() == CQBucketer::Type::FIXED_STOPS);

    if      (! isUnique && isFixed) fixedBucketRadio_ ->setChecked(true);
    else if (! isUnique && isAuto ) rangeBucketRadio_ ->setChecked(true);
    else if (! isUnique && isStops) stopsBucketRadio_ ->setChecked(true);
    else if (isUnique             ) uniqueBucketRadio_->setChecked(true);

    uniqueBucketRadio_->setEnabled(details);
    fixedBucketRadio_ ->setEnabled(details && ! isString);
    rangeBucketRadio_ ->setEnabled(details && ! isString);
    stopsBucketRadio_ ->setEnabled(details && ! isString);

    if (isNumeric) {
      auto minValue = (details ? details->minValue() : 0.0);
      auto maxValue = (details ? details->maxValue() : 1.0);

      bucketRange_->setSliderMinMax(plot_->minBucketValue(), plot_->maxBucketValue());
      bucketRange_->setDecimalPlaces(isIntegral ? 0 : 3);
    }

    startBucketEdit_->setValue(plot_->startBucketValue());
    deltaBucketEdit_->setValue(plot_->deltaBucketValue());
    numBucketsEdit_ ->setValue(plot_->numAutoBuckets());
    bucketStopsEdit_->setText(plot_->bucketStops().toString());
    uniqueCount_    ->setValue(details ? details->numUnique() : 0);
    rangeLabel_     ->setText(QString("%1-%2").arg(plot_->minBucketValue()).
                                               arg(plot_->maxBucketValue()));

    setFrameWidgetVisible(bucketRange_    , details && ! isUnique && isAuto);
    setFrameWidgetVisible(startBucketEdit_, details && ! isUnique && isFixed);
    setFrameWidgetVisible(deltaBucketEdit_, details && ! isUnique && isFixed);
    setFrameWidgetVisible(numBucketsEdit_ , details && ! isUnique && isAuto);
    setFrameWidgetVisible(bucketStopsEdit_, details && ! isUnique && isStops);
    setFrameWidgetVisible(uniqueCount_    , details && (isUnique || isString));
    setFrameWidgetVisible(rangeLabel_     , details && (isUnique || isFixed || isStops));
  }

  //---

  CQChartsPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

void
CQChartsGroupPlotCustomControls::
connectSlots(bool b)
{
  if (groupColumnCombo_) {
    CQChartsWidgetUtil::connectDisconnect(b,
      groupColumnCombo_, SIGNAL(columnChanged()), this, SLOT(groupColumnSlot()));

    CQChartsWidgetUtil::connectDisconnect(b,
      bucketRadioGroup_, SIGNAL(buttonClicked(QAbstractButton *)),
      this, SLOT(bucketRadioGroupSlot(QAbstractButton *)));
    CQChartsWidgetUtil::connectDisconnect(b,
      bucketRange_, SIGNAL(sliderRangeChanged(double, double)), this, SLOT(bucketRangeSlot()));
    CQChartsWidgetUtil::connectDisconnect(b,
      startBucketEdit_, SIGNAL(valueChanged(double)), this, SLOT(startBucketSlot()));
    CQChartsWidgetUtil::connectDisconnect(b,
      deltaBucketEdit_, SIGNAL(valueChanged(double)), this, SLOT(deltaBucketSlot()));
    CQChartsWidgetUtil::connectDisconnect(b,
      numBucketsEdit_, SIGNAL(valueChanged(int)), this, SLOT(numBucketsSlot()));
    CQChartsWidgetUtil::connectDisconnect(b,
      bucketStopsEdit_, SIGNAL(editingFinished()), this, SLOT(bucketStopsSlot()));
  }

  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsGroupPlotCustomControls::
groupColumnSlot()
{
  connectSlots(false);

  plot_->setGroupColumn(groupColumnCombo_->getColumn());

  plot_->initGroupBucketer();

  bucketRange_->setRangeMinMax(plot_->minBucketValue(), plot_->maxBucketValue());

  updateWidgets();

  connectSlots(true);
}

void
CQChartsGroupPlotCustomControls::
bucketRadioGroupSlot(QAbstractButton *button)
{
  connectSlots(false);

  plot_->setUpdatesEnabled(false);

  if      (button == fixedBucketRadio_) {
    plot_->setExactBucketValue(false);
    plot_->setBucketType(CQBucketer::Type::REAL_RANGE);
  }
  else if (button == rangeBucketRadio_) {
    plot_->setExactBucketValue(false);
    plot_->setBucketType(CQBucketer::Type::REAL_AUTO);
  }
  else if (button == stopsBucketRadio_) {
    plot_->setExactBucketValue(false);
    plot_->setBucketType(CQBucketer::Type::FIXED_STOPS);
  }
  else if (button == uniqueBucketRadio_) {
    plot_->setExactBucketValue(true);
  }

  plot_->setUpdatesEnabled(true);

  connectSlots(true);
}

void
CQChartsGroupPlotCustomControls::
bucketRangeSlot()
{
  connectSlots(false);

  plot_->setUpdatesEnabled(false);

  plot_->setMinBucketValue(bucketRange_->sliderMin());
  plot_->setMaxBucketValue(bucketRange_->sliderMax());

  plot_->setUpdatesEnabled(true);

  connectSlots(true);
}

void
CQChartsGroupPlotCustomControls::
startBucketSlot()
{
  plot_->setStartBucketValue(startBucketEdit_->value());
}

void
CQChartsGroupPlotCustomControls::
deltaBucketSlot()
{
  plot_->setDeltaBucketValue(deltaBucketEdit_->value());
}

void
CQChartsGroupPlotCustomControls::
numBucketsSlot()
{
  plot_->setNumAutoBuckets(numBucketsEdit_->value());
}

void
CQChartsGroupPlotCustomControls::
bucketStopsSlot()
{
  plot_->setBucketStops(CQChartsReals(bucketStopsEdit_->text()));
}
