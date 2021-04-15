#include <CQChartsPlotCustomControls.h>
#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotParameterEdit.h>
#include <CQChartsColumnControlGroup.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsColorEdit.h>
#include <CQChartsColorRangeSlider.h>
#include <CQChartsPaletteNameEdit.h>
#include <CQChartsMapKey.h>
#include <CQChartsWidgetUtil.h>
#include <CQCharts.h>

//#include <CQTabSplit.h>
#include <CQGroupBox.h>
#include <CQUtil.h>

#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>

CQChartsPlotCustomControls::
CQChartsPlotCustomControls(CQCharts *charts, const QString &plotType) :
 QFrame(nullptr), charts_(charts), plotType_(plotType)
{
  setObjectName("customControls");

  layout_ = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  titleWidget_ = CQUtil::makeLabelWidget<QLabel>("", "title");

  layout_->addWidget(titleWidget_);

  //---

#if 0
  split_ = CQUtil::makeWidget<CQTabSplit>("split");

  split_->setOrientation(Qt::Vertical);
  split_->setGrouped(true);
  split_->setAutoFit(true);

  layout_->addWidget(split_);
#endif
}

void
CQChartsPlotCustomControls::
addLayoutStretch()
{
  layout_->addStretch(1);
}

void
CQChartsPlotCustomControls::
addColumnWidgets(const QStringList &columnNames, FrameData &frameData)
{
  auto *plotType = this->plotType();
  assert(plotType);

  bool isNumeric = false;

  for (const auto &name : columnNames) {
    const auto *parameter = plotType->getParameter(name);
    assert(parameter);

    if (parameter->isNumeric())
      isNumeric = true;

    if      (parameter->type() == CQChartsPlotParameter::Type::COLUMN) {
      auto *columnEdit = new CQChartsColumnParameterEdit(parameter, /*isBasic*/true);

      addFrameWidget(frameData, parameter->desc(), columnEdit);

      columnEdits_.push_back(columnEdit);
    }
    else if (parameter->type() == CQChartsPlotParameter::Type::COLUMN_LIST) {
      auto *columnsEdit = new CQChartsColumnsParameterEdit(parameter, /*isBasic*/true);

      addFrameWidget(frameData, parameter->desc(), columnsEdit);

      columnsEdits_.push_back(columnsEdit);
    }
    else
      assert(false);
  }

  if (isNumeric) {
    numericCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Numeric Only", "numericCheck");

    numericCheck_->setToolTip("Restrict to columns with numeric values");

    frameData.layout->addWidget(numericCheck_, frameData.row, 0, 1, 2); ++frameData.row;
  }

  //addFrameRowStretch(frameData);
}

void
CQChartsPlotCustomControls::
showColumnWidgets(const QStringList &columnNames)
{
  QWidget *w = nullptr;

  if      (! columnEdits_.empty())
    w = columnEdits_[0];
  else if (! columnsEdits_.empty())
    w = columnsEdits_[0];

  assert(w);

  auto *parent = w->parentWidget();

  auto *layout = qobject_cast<QGridLayout *>(parent->layout());
  assert(layout);

  for (int r = 0; r < layout->rowCount(); ++r) {
    auto *item1 = layout->itemAtPosition(r, 1);

    auto *edit = (item1 ? item1->widget() : nullptr);
    if (! edit) continue;

    auto *ce1 = qobject_cast<CQChartsColumnParameterEdit  *>(edit);
    auto *ce2 = qobject_cast<CQChartsColumnsParameterEdit *>(edit);
    if (! ce1 && ! ce2) continue;

    auto *item0 = layout->itemAtPosition(r, 0);
    auto *label = (item0 ? item0->widget() : nullptr);

    auto *parameter = (ce1 ? ce1->parameter() : ce2->parameter());

    bool visible = (columnNames.indexOf(parameter->name()) >= 0);

    if (label)
      label->setVisible(visible);

    edit ->setVisible(visible);
  }
}

void
CQChartsPlotCustomControls::
addColorColumnWidgets(const QString &title)
{
  // color group
  auto colorControlGroupData = createColumnControlGroup("colorControlGroup", title);

  colorControlGroup_ = colorControlGroupData.group;

  //---

  // color widgets
  colorEdit_        = CQUtil::makeWidget<CQChartsColorLineEdit>("colorEdit");
  colorColumnCombo_ = CQUtil::makeWidget<CQChartsColumnCombo>("colorColumnCombo");
  colorRange_       = CQUtil::makeWidget<CQChartsColorRangeSlider>("colorRange");
  colorPaletteEdit_ = CQUtil::makeWidget<CQChartsPaletteNameEdit>("colorPaletteEdit");

  addFrameWidget(colorControlGroupData.fixedFrame , "Color"  , colorEdit_);
  addFrameWidget(colorControlGroupData.columnFrame, "Column" , colorColumnCombo_);
  addFrameWidget(colorControlGroupData.columnFrame, "Range"  , colorRange_);
  addFrameWidget(colorControlGroupData.columnFrame, "Palette", colorPaletteEdit_);

  addFrameRowStretch(colorControlGroupData.fixedFrame );
  addFrameRowStretch(colorControlGroupData.columnFrame);

  //---

  colorMapKey_ = new CQChartsMapKeyWidget;

  colorMapKey_->setFixedSize(16, 16);

  colorControlGroupData.columnControls->layout()->addWidget(colorMapKey_);

  connect(colorControlGroupData.group, SIGNAL(showKey(bool)),
          this, SLOT(showColorKeySlot(bool)));

  //---

  connectSlots(true);
}

void
CQChartsPlotCustomControls::
connectSlots(bool b)
{
  if (plot_)
    CQChartsWidgetUtil::connectDisconnect(b,
      plot_, SIGNAL(colorDetailsChanged()), this, SLOT(colorDetailsSlot()));

  if (colorEdit_) {
    CQChartsWidgetUtil::connectDisconnect(b,
      colorControlGroup_, SIGNAL(groupChanged()), this, SLOT(colorGroupChanged()));
    CQChartsWidgetUtil::connectDisconnect(b,
      colorEdit_, SIGNAL(colorChanged()), this, SLOT(colorSlot()));
    CQChartsWidgetUtil::connectDisconnect(b,
      colorColumnCombo_, SIGNAL(columnChanged()), this, SLOT(colorColumnSlot()));
    CQChartsWidgetUtil::connectDisconnect(b,
      colorRange_, SIGNAL(sliderRangeChanged(double, double)), this, SLOT(colorRangeSlot()));
    CQChartsWidgetUtil::connectDisconnect(b,
      colorPaletteEdit_, SIGNAL(nameChanged()), this, SLOT(colorPaletteSlot()));
  }

  for (auto *columnEdit : columnEdits_)
    CQChartsWidgetUtil::connectDisconnect(b,
      columnEdit, SIGNAL(columnChanged()), this, SLOT(columnSlot()));

  for (auto *columnsEdit : columnsEdits_)
    CQChartsWidgetUtil::connectDisconnect(b,
      columnsEdit, SIGNAL(columnsChanged()), this, SLOT(columnsSlot()));

  if (numericCheck_)
    CQChartsWidgetUtil::connectDisconnect(b,
      numericCheck_, SIGNAL(stateChanged(int)), this, SLOT(numericOnlySlot(int)));
}

void
CQChartsPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_) {
    disconnect(plot_, SIGNAL(plotDrawn()), this, SLOT(plotDrawnSlot()));
    disconnect(plot_, SIGNAL(colorDetailsChanged()), this, SLOT(colorDetailsSlot()));
  }

  plot_ = plot;

  if (plot_) {
    connect(plot_, SIGNAL(plotDrawn()), this, SLOT(plotDrawnSlot()));
    connect(plot_, SIGNAL(colorDetailsChanged()), this, SLOT(colorDetailsSlot()));
  }
}

void
CQChartsPlotCustomControls::
colorGroupChanged()
{
  if (colorControlGroup_->isFixed())
    plot()->setColorColumn(CQChartsColumn());
}

void
CQChartsPlotCustomControls::
plotDrawnSlot()
{
  handlePlotDrawn();
}

void
CQChartsPlotCustomControls::
handlePlotDrawn()
{
  updateColorKeyVisible();
}

void
CQChartsPlotCustomControls::
updateColorKeyVisible()
{
  if (colorMapKey_) {
    auto hasColorColumn = plot()->colorColumn().isValid();

    bool hasColorMapKey = (hasColorColumn && ! plot()->colorMapKey()->isNative());

    colorMapKey_->setVisible(hasColorMapKey && colorControlGroup_->isKeyVisible());

    colorMapKey_->setKey(plot()->colorMapKey());
  }
}

void
CQChartsPlotCustomControls::
colorDetailsSlot()
{
  // plot color details changed
  updateWidgets();
}

void
CQChartsPlotCustomControls::
columnSlot()
{
  auto *columnEdit = qobject_cast<CQChartsColumnParameterEdit *>(sender());
  auto *parameter  = columnEdit->parameter();

  plot()->setNamedColumn(parameter->name(), columnEdit->getColumn());
}

void
CQChartsPlotCustomControls::
columnsSlot()
{
  auto *columnsEdit = qobject_cast<CQChartsColumnsParameterEdit *>(sender());
  auto *parameter   = columnsEdit->parameter();

  plot()->setNamedColumns(parameter->name(), columnsEdit->columns());
}

void
CQChartsPlotCustomControls::
numericOnlySlot(int state)
{
  for (auto *columnEdit : columnEdits_) {
    auto *parameter = columnEdit->parameter();

    if (parameter->isNumeric())
      columnEdit->setNumericOnly(state);
  }

  for (auto *columnsEdit : columnsEdits_) {
    auto *parameter = columnsEdit->parameter();

    if (parameter->isNumeric())
      columnsEdit->setNumericOnly(state);
  }
}

CQChartsPlotType *
CQChartsPlotCustomControls::
plotType() const
{
  return charts_->plotType(plotType_);
}

CQChartsPlotCustomControls::ColumnControlGroupData
CQChartsPlotCustomControls::
createColumnControlGroup(const QString &name, const QString &title)
{
  ColumnControlGroupData columnControlGroupData;

  // control group
  columnControlGroupData.group = CQUtil::makeWidget<CQChartsColumnControlGroup>(name);

  columnControlGroupData.group->setTitle(title);

  layout_->addWidget(columnControlGroupData.group);

  columnControlGroupData.fixedFrame     = createFrame("fixedFrame");
  columnControlGroupData.columnFrame    = createFrame("columnFrame");
  columnControlGroupData.columnControls = CQUtil::makeWidget<QFrame>("columnControls");

  auto *columnControlsLayout =
    CQUtil::makeLayout<QHBoxLayout>(columnControlGroupData.columnControls, 0, 0);

  columnControlsLayout->addWidget(columnControlGroupData.columnFrame.frame);

  columnControlGroupData.group->addFixedWidget (columnControlGroupData.fixedFrame.frame);
  columnControlGroupData.group->addColumnWidget(columnControlGroupData.columnControls  );

  return columnControlGroupData;
}

CQChartsPlotCustomControls::FrameData
CQChartsPlotCustomControls::
createGroupFrame(const QString &name, const QString &objName)
{
  auto frameData = createFrame(objName);

#if 0
  split_->addWidget(frameData.frame, name);
#else
  auto *groupBox    = CQUtil::makeLabelWidget<CQGroupBox>(name, "groupBox");
  auto *groupLayout = CQUtil::makeLayout<QVBoxLayout>(groupBox, 0, 0);

  groupBox->setTitleScale(0.85);
  groupBox->setTitleColored(true);

  groupLayout->addWidget(frameData.frame);

  layout_->addWidget(groupBox);
#endif

  return frameData;
}

CQChartsPlotCustomControls::FrameData
CQChartsPlotCustomControls::
createFrame(const QString &objName)
{
  FrameData frameData;

  frameData.frame  = CQUtil::makeWidget<QFrame>(objName);
  frameData.layout = CQUtil::makeLayout<QGridLayout>(frameData.frame, 2, 2);

  frameData.layout->setColumnStretch(1, 1);

  return frameData;
}

void
CQChartsPlotCustomControls::
addFrameWidget(FrameData &frameData, const QString &label, QWidget *w)
{
  frameData.layout->addWidget(new QLabel(label), frameData.row, 0);
  frameData.layout->addWidget(w                , frameData.row, 1); ++frameData.row;
}

void
CQChartsPlotCustomControls::
addFrameColWidget(FrameData &frameData, QWidget *w)
{
  frameData.layout->addWidget(w, frameData.row, frameData.col); ++frameData.col;
}

void
CQChartsPlotCustomControls::
addFrameRowStretch(FrameData &frameData)
{
  frameData.layout->setRowStretch(frameData.row, 1);
}

void
CQChartsPlotCustomControls::
showColorKeySlot(bool)
{
  updateColorKeyVisible();
}

CQChartsBoolParameterEdit *
CQChartsPlotCustomControls::
createBoolEdit(const QString &name)
{
  auto *plotType = this->plotType();
  assert(plotType);

  const auto *parameter = plotType->getParameter(name);
  assert(parameter && parameter->type() == CQChartsPlotParameter::Type::BOOLEAN);

  const auto *bparameter = dynamic_cast<const CQChartsBoolParameter *>(parameter);
  assert(bparameter);

  return new CQChartsBoolParameterEdit(bparameter, /*choice*/true);
}

CQChartsEnumParameterEdit *
CQChartsPlotCustomControls::
createEnumEdit(const QString &name)
{
  auto *plotType = this->plotType();
  assert(plotType);

  const auto *parameter = plotType->getParameter(name);
  assert(parameter && parameter->type() == CQChartsPlotParameter::Type::ENUM);

  const auto *eparameter = dynamic_cast<const CQChartsEnumParameter *>(parameter);
  assert(eparameter);

  return new CQChartsEnumParameterEdit(eparameter);
}

void
CQChartsPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  auto titleStr = plot()->titleStr();

  if (titleStr == "")
    titleStr = plot()->calcName();

  titleWidget_->setText(QString("<b>%1</b>").arg(titleStr));

  //----

  if (colorEdit_) {
    auto hasColorColumn = plot()->colorColumn().isValid();
    auto isNative       = (plot()->colorMapType() == CQChartsPlot::ColumnType::COLOR);

    colorEdit_       ->setEnabled(! hasColorColumn);
    colorRange_      ->setEnabled(hasColorColumn && ! isNative);
    colorPaletteEdit_->setEnabled(hasColorColumn && ! isNative);

    colorEdit_->setColor(getColorValue());

    colorColumnCombo_->setModelColumn(plot()->getModelData(), plot()->colorColumn());

    auto paletteName = plot()->colorMapPalette();

    colorRange_->setPlot(plot());
    colorRange_->setPaletteName(paletteName);

    colorPaletteEdit_->setChartsPaletteName(plot()->charts(), paletteName);

    if (hasColorColumn)
      colorControlGroup_->setColumn();
  }

  //---

  for (auto *columnEdit : columnEdits_) {
    auto *parameter = columnEdit->parameter();

    columnEdit->setModelData(plot()->getModelData());
    columnEdit->setColumn   (plot()->getNamedColumn(parameter->name()));
  }

  for (auto *columnsEdit : columnsEdits_) {
    auto *parameter = columnsEdit->parameter();

    columnsEdit->setModelData(plot()->getModelData());
    columnsEdit->setColumns  (plot()->getNamedColumns(parameter->name()));
  }

  //---

  connectSlots(true);
}

void
CQChartsPlotCustomControls::
colorSlot()
{
  setColorValue(colorEdit_->color());
}

void
CQChartsPlotCustomControls::
colorColumnSlot()
{
  plot()->setColorColumn(colorColumnCombo_->getColumn());
}

void
CQChartsPlotCustomControls::
colorRangeSlot()
{
  connectSlots(false);

  plot()->setColorMapMin(colorRange_->sliderMin());
  plot()->setColorMapMax(colorRange_->sliderMax());

  updateWidgets();

  connectSlots(true);
}

void
CQChartsPlotCustomControls::
colorPaletteSlot()
{
  plot()->setColorMapPalette(colorPaletteEdit_->paletteName());
}
