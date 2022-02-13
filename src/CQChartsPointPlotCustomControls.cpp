#include <CQChartsPointPlotCustomControls.h>
#include <CQChartsPointPlot.h>
#include <CQChartsColumnControlGroup.h>
#include <CQChartsSymbolSizeRangeSlider.h>
#include <CQChartsSymbolTypeRangeSlider.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsSymbolSetEdit.h>
#include <CQChartsSymbolEdit.h>
#include <CQChartsLengthEdit.h>
#include <CQChartsMapKey.h>
#include <CQChartsWidgetUtil.h>
#include <CQCharts.h>
#include <CQUtil.h>

#include <QLayout>

CQChartsPointPlotCustomControls::
CQChartsPointPlotCustomControls(CQCharts *charts, const QString &plotType) :
 CQChartsGroupPlotCustomControls(charts, plotType)
{
}

void
CQChartsPointPlotCustomControls::
addSymbolSizeWidgets()
{
  // symbol size group
  auto symbolSizeControlGroupData =
    createColumnControlGroup("symbolSizeControlGroup", "Point Size");

  symbolSizeControlGroup_ = symbolSizeControlGroupData.group;

  //---

  // symbol size widgets
  symbolSizeLengthEdit_  = CQUtil::makeWidget<CQChartsLengthEdit>("symbolSizeLengthEdit");
  symbolSizeColumnCombo_ = CQUtil::makeWidget<CQChartsColumnCombo>("symbolSizeColumnCombo");
  symbolSizeRange_       = CQUtil::makeWidget<CQChartsSymbolSizeRangeSlider>("symbolSizeRange");
  symbolSizeMappingEdit_ = CQUtil::makeWidget<QLineEdit>("symbolSizeMappingEdit");

  addFrameWidget(symbolSizeControlGroupData.fixedFrame , "Size"   , symbolSizeLengthEdit_);
  addFrameWidget(symbolSizeControlGroupData.columnFrame, "Column" , symbolSizeColumnCombo_);
  addFrameWidget(symbolSizeControlGroupData.columnFrame, "Range"  , symbolSizeRange_);
  addFrameWidget(symbolSizeControlGroupData.columnFrame, "Mapping", symbolSizeMappingEdit_);

  addFrameRowStretch(symbolSizeControlGroupData.fixedFrame );
  addFrameRowStretch(symbolSizeControlGroupData.columnFrame);

  //---

  symbolSizeMapKey_ = new CQChartsMapKeyWidget;

  symbolSizeMapKey_->setFixedSize(16, 16);

  symbolSizeControlGroupData.columnControls->layout()->addWidget(symbolSizeMapKey_);

  connect(symbolSizeControlGroupData.group, SIGNAL(showKey(bool)),
          this, SLOT(showSymbolSizeMapKeySlot(bool)));

  //---

  // symbol type group
  auto symbolTypeControlGroupData =
    createColumnControlGroup("symbolTypeControlGroup", "Point Symbol");

  symbolTypeControlGroup_ = symbolTypeControlGroupData.group;

  //---

  // symbol type widgets
  symbolTypeEdit_        = CQUtil::makeWidget<CQChartsSymbolLineEdit>("symbolTypeEdit");
  symbolTypeColumnCombo_ = CQUtil::makeWidget<CQChartsColumnCombo>("symbolTypeColumnCombo");
  symbolTypeRange_       = CQUtil::makeWidget<CQChartsSymbolTypeRangeSlider>("symbolTypeRange");
  symbolTypeSetEdit_     = CQUtil::makeWidget<CQChartsSymbolSetEdit>("symbolSetEdit");
  symbolTypeMappingEdit_ = CQUtil::makeWidget<QLineEdit>("symbolTypeMappingEdit");

  symbolTypeSetEdit_->setCharts(charts_);

  addFrameWidget(symbolTypeControlGroupData.fixedFrame , "Symbol" , symbolTypeEdit_);
  addFrameWidget(symbolTypeControlGroupData.columnFrame, "Column" , symbolTypeColumnCombo_);
  addFrameWidget(symbolTypeControlGroupData.columnFrame, "Range"  , symbolTypeRange_);
  addFrameWidget(symbolTypeControlGroupData.columnFrame, "Set"    , symbolTypeSetEdit_);
  addFrameWidget(symbolTypeControlGroupData.columnFrame, "Mapping", symbolTypeMappingEdit_);

  addFrameRowStretch(symbolTypeControlGroupData.fixedFrame );
  addFrameRowStretch(symbolTypeControlGroupData.columnFrame);

  //---

  symbolTypeMapKey_ = new CQChartsMapKeyWidget;

  symbolTypeMapKey_->setFixedSize(16, 16);

  symbolTypeControlGroupData.columnControls->layout()->addWidget(symbolTypeMapKey_);

  connect(symbolTypeControlGroupData.group, SIGNAL(showKey(bool)),
          this, SLOT(showSymbolTypeMapKeySlot(bool)));
}

void
CQChartsPointPlotCustomControls::
connectSlots(bool b)
{
  if (plot_) {
    CQChartsWidgetUtil::connectDisconnect(b,
      plot_, SIGNAL(symbolSizeDetailsChanged()), this, SLOT(symbolSizeDetailsSlot()));
    CQChartsWidgetUtil::connectDisconnect(b,
      plot_, SIGNAL(symbolTypeDetailsChanged()), this, SLOT(symbolTypeDetailsSlot()));
  }

  if (symbolSizeLengthEdit_) {
    CQChartsWidgetUtil::connectDisconnect(b,
      symbolSizeControlGroup_, SIGNAL(groupChanged()), this, SLOT(symbolSizeGroupChanged()));
    CQChartsWidgetUtil::connectDisconnect(b,
      symbolSizeLengthEdit_, SIGNAL(lengthChanged()), this, SLOT(symbolSizeLengthSlot()));
    CQChartsWidgetUtil::connectDisconnect(b,
      symbolSizeColumnCombo_, SIGNAL(columnChanged()), this, SLOT(symbolSizeColumnSlot()));
    CQChartsWidgetUtil::connectDisconnect(b,
      symbolSizeRange_, SIGNAL(sliderRangeChanged(double, double)),
      this, SLOT(symbolSizeRangeSlot(double, double)));
    CQChartsWidgetUtil::connectDisconnect(b,
      symbolSizeMappingEdit_, SIGNAL(editingFinished()),
      this, SLOT(symbolSizeMappingSlot()));
  }

  if (symbolTypeEdit_) {
    CQChartsWidgetUtil::connectDisconnect(b,
      symbolTypeControlGroup_, SIGNAL(groupChanged()), this, SLOT(symbolTypeGroupChanged()));
    CQChartsWidgetUtil::connectDisconnect(b,
      symbolTypeEdit_, SIGNAL(symbolChanged()), this, SLOT(symbolTypeSlot()));
    CQChartsWidgetUtil::connectDisconnect(b,
      symbolTypeColumnCombo_, SIGNAL(columnChanged()), this, SLOT(symbolTypeColumnSlot()));
    CQChartsWidgetUtil::connectDisconnect(b,
      symbolTypeRange_, SIGNAL(sliderRangeChanged(int, int)),
      this, SLOT(symbolTypeRangeSlot(int, int)));
    CQChartsWidgetUtil::connectDisconnect(b,
      symbolTypeSetEdit_, SIGNAL(setChanged(const QString &)),
      this, SLOT(symbolTypeSetSlot(const QString &)));
    CQChartsWidgetUtil::connectDisconnect(b,
      symbolTypeMappingEdit_, SIGNAL(editingFinished()),
      this, SLOT(symbolTypeMappingSlot()));
  }

  CQChartsGroupPlotCustomControls::connectSlots(b);
}

void
CQChartsPointPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_) {
    disconnect(plot_, SIGNAL(plotDrawn()), this, SLOT(plotDrawnSlot()));
    disconnect(plot_, SIGNAL(symbolSizeDetailsChanged()), this, SLOT(symbolSizeDetailsSlot()));
    disconnect(plot_, SIGNAL(symbolTypeDetailsChanged()), this, SLOT(symbolTypeDetailsSlot()));
  }

  plot_ = dynamic_cast<CQChartsPointPlot *>(plot);

  if (plot_) {
    connect(plot_, SIGNAL(plotDrawn()), this, SLOT(plotDrawnSlot()));
    connect(plot_, SIGNAL(symbolSizeDetailsChanged()), this, SLOT(symbolSizeDetailsSlot()));
    connect(plot_, SIGNAL(symbolTypeDetailsChanged()), this, SLOT(symbolTypeDetailsSlot()));
  }

  CQChartsGroupPlotCustomControls::setPlot(plot);
}

void
CQChartsPointPlotCustomControls::
showSymbolSizeMapKeySlot(bool)
{
  updateSymbolSizeMapKeyVisible();
}

void
CQChartsPointPlotCustomControls::
showSymbolTypeMapKeySlot(bool)
{
  updateSymbolTypeMapKeyVisible();
}

void
CQChartsPointPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  if (symbolSizeLengthEdit_) {
    auto hasSymbolSizeColumn = plot_->symbolSizeColumn().isValid();

    symbolSizeLengthEdit_ ->setEnabled(! hasSymbolSizeColumn);
    symbolSizeRange_      ->setEnabled(hasSymbolSizeColumn);
    symbolSizeMappingEdit_->setEnabled(hasSymbolSizeColumn);

    symbolSizeLengthEdit_ ->setLength(plot_->fixedSymbolSize());
    symbolSizeColumnCombo_->setModelColumn(plot_->getModelData(), plot_->symbolSizeColumn());
    symbolSizeRange_      ->setPlot(plot_);
    symbolSizeMappingEdit_->setText(plot_->symbolSizeMap().toString());

    if (hasSymbolSizeColumn)
      symbolSizeControlGroup_->setColumn();

    bool hasSizeMap = plot_->symbolSizeMap().isValid();

    setFrameWidgetVisible(symbolSizeRange_      , isShowSizeRange  ().boolOr(! hasSizeMap));
    setFrameWidgetVisible(symbolSizeMappingEdit_, isShowSizeMapping().boolOr(  hasSizeMap));
  }

  if (symbolTypeEdit_) {
    auto hasSymbolTypeColumn = plot_->symbolTypeColumn().isValid();

    symbolTypeEdit_       ->setEnabled(! hasSymbolTypeColumn);
    symbolTypeRange_      ->setEnabled(hasSymbolTypeColumn);
    symbolTypeSetEdit_    ->setEnabled(hasSymbolTypeColumn);
    symbolTypeMappingEdit_->setEnabled(hasSymbolTypeColumn);

    auto *symbolSetMgr = charts_->symbolSetMgr();
    auto *symbolSet    = symbolSetMgr->symbolSet(plot_->symbolTypeSetName());

    symbolTypeEdit_       ->setSymbol(plot_->fixedSymbol());
    symbolTypeColumnCombo_->setModelColumn(plot_->getModelData(), plot_->symbolTypeColumn());
    symbolTypeSetEdit_    ->setSymbolSetName(symbolSet ? symbolSet->name() : "");
    symbolTypeRange_      ->setPlot(plot_);
    symbolTypeRange_      ->setSymbolSetName(symbolSet ? symbolSet->name() : "");
    symbolTypeMappingEdit_->setText(plot_->symbolTypeMap().toString());

    if (hasSymbolTypeColumn)
      symbolTypeControlGroup_->setColumn();

    bool hasTypeMap = plot_->symbolTypeMap().isValid();

    setFrameWidgetVisible(symbolTypeRange_      , isShowSymbolRange  ().boolOr(! hasTypeMap));
    setFrameWidgetVisible(symbolTypeSetEdit_    , isShowSymbolSet    ().boolOr(! hasTypeMap));
    setFrameWidgetVisible(symbolTypeMappingEdit_, isShowSymbolMapping().boolOr(  hasTypeMap));
  }

  //---

  CQChartsGroupPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

void
CQChartsPointPlotCustomControls::
plotDrawnSlot()
{
  handlePlotDrawn();
}

void
CQChartsPointPlotCustomControls::
handlePlotDrawn()
{
  updateSymbolSizeMapKeyVisible();
  updateSymbolTypeMapKeyVisible();
}

void
CQChartsPointPlotCustomControls::
updateSymbolSizeMapKeyVisible()
{
  if (symbolSizeMapKey_) {
    auto hasSymbolSizeColumn = plot_->symbolSizeColumn().isValid();

    bool hasSymbolSizeMapKey = (hasSymbolSizeColumn && ! plot_->symbolSizeMapKey()->isNative());

    symbolSizeMapKey_->setVisible(hasSymbolSizeMapKey && symbolSizeControlGroup_->isKeyVisible());

    symbolSizeMapKey_->setKey(plot_->symbolSizeMapKey());
  }
}

void
CQChartsPointPlotCustomControls::
updateSymbolTypeMapKeyVisible()
{
  if (symbolTypeMapKey_) {
    auto hasSymbolTypeColumn = plot_->symbolTypeColumn().isValid();

    bool hasSymbolTypeMapKey = (hasSymbolTypeColumn && ! plot_->symbolTypeMapKey()->isNative());

    symbolTypeMapKey_->setVisible(hasSymbolTypeMapKey && symbolTypeControlGroup_->isKeyVisible());

    symbolTypeMapKey_->setKey(plot_->symbolTypeMapKey());
  }

  CQChartsGroupPlotCustomControls::handlePlotDrawn();
}

void
CQChartsPointPlotCustomControls::
symbolSizeGroupChanged()
{
  if (symbolSizeControlGroup_->isFixed())
    plot_->setSymbolSizeColumn(CQChartsColumn());
}

void
CQChartsPointPlotCustomControls::
symbolSizeDetailsSlot()
{
  // plot symbol size details changed
  updateWidgets();
}

void
CQChartsPointPlotCustomControls::
symbolSizeLengthSlot()
{
  plot_->setFixedSymbolSize(symbolSizeLengthEdit_->length());

  // TODO: need plot signal
  updateWidgets();
}

void
CQChartsPointPlotCustomControls::
symbolSizeColumnSlot()
{
  plot_->setSymbolSizeColumn(symbolSizeColumnCombo_->getColumn());
}

void
CQChartsPointPlotCustomControls::
symbolSizeRangeSlot(double min, double max)
{
  connectSlots(false);

  plot_->setSymbolSizeUserMapMin(min);
  plot_->setSymbolSizeUserMapMax(max);

  connectSlots(true);

  updateWidgets();
}

void
CQChartsPointPlotCustomControls::
symbolSizeMappingSlot()
{
  connectSlots(false);

  plot_->setSymbolSizeMap(CQChartsSymbolSizeMap(symbolSizeMappingEdit_->text()));

  connectSlots(true);

  updateWidgets();
}

void
CQChartsPointPlotCustomControls::
symbolTypeGroupChanged()
{
  if (symbolTypeControlGroup_->isFixed())
    plot_->setSymbolTypeColumn(CQChartsColumn());
}

void
CQChartsPointPlotCustomControls::
symbolTypeDetailsSlot()
{
  // plot symbol type details changed
  updateWidgets();
}

void
CQChartsPointPlotCustomControls::
symbolTypeSlot()
{
  plot_->setFixedSymbol(CQChartsSymbol(symbolTypeEdit_->symbol()));
}

void
CQChartsPointPlotCustomControls::
symbolTypeColumnSlot()
{
  plot_->setSymbolTypeColumn(symbolTypeColumnCombo_->getColumn());
}

void
CQChartsPointPlotCustomControls::
symbolTypeRangeSlot(int min, int max)
{
  connectSlots(false);

  plot_->setSymbolTypeMapMin(min);
  plot_->setSymbolTypeMapMax(max);

  connectSlots(true);

  updateWidgets();
}

void
CQChartsPointPlotCustomControls::
symbolTypeSetSlot(const QString &name)
{
  plot_->setSymbolTypeSetName(name);
}

void
CQChartsPointPlotCustomControls::
symbolTypeMappingSlot()
{
  connectSlots(false);

  plot_->setSymbolTypeMap(CQChartsSymbolTypeMap(symbolTypeMappingEdit_->text()));

  connectSlots(true);

  updateWidgets();
}
