#include <CQChartsAxisEdit.h>
#include <CQChartsAxis.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsAxisSideEdit.h>
#include <CQChartsLineDataEdit.h>
#include <CQChartsFillDataEdit.h>
#include <CQChartsTextDataEdit.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>

#include <CQGroupBox.h>
#include <CQCheckBox.h>
#include <CQIntegerSpin.h>
#include <CQRealSpin.h>
#include <CQRadioButtons.h>
#include <CQUtil.h>

#include <QLabel>
#include <QComboBox>
#include <QTabWidget>
#include <QVBoxLayout>

namespace {

int valueTypeToInd(const CQChartsAxisValueType &valueType) {
  if (valueType.type() != CQChartsAxisValueType::Type::NONE)
    return ((int) valueType.type()) - 1;
  else
    return -1;
}

CQChartsAxisValueType indToValueType(int ind) {
  if (ind <= 0)
    return CQChartsAxisValueType::Type::NONE;
  else
    return CQChartsAxisValueType((CQChartsAxisValueType::Type) ind);
}

}

CQChartsEditAxisDlg::
CQChartsEditAxisDlg(QWidget *parent, CQChartsAxis *axis) :
 QDialog(parent), axis_(axis)
{
  setWindowTitle(QString("Edit Plot Axis (%1)").arg(axis->plot()->id()));

  //---

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  edit_ = new CQChartsAxisEdit(nullptr, axis_);

  layout->addWidget(edit_);

  //---

  CQChartsDialogButtons *buttons = new CQChartsDialogButtons;

  buttons->connect(this, SLOT(okSlot()), SLOT(applySlot()), SLOT(cancelSlot()));

  layout->addWidget(buttons);
}

void
CQChartsEditAxisDlg::
okSlot()
{
  if (applySlot())
    cancelSlot();
}

bool
CQChartsEditAxisDlg::
applySlot()
{
  edit_->applyData();

  return true;
}

void
CQChartsEditAxisDlg::
cancelSlot()
{
  close();
}

QSize
CQChartsEditAxisDlg::
sizeHint() const
{
  QSize s = QDialog::sizeHint();

  QFontMetrics fm(font());

  int w = fm.width("Major Grid Line") + fm.width("Minor Grid Line") + fm.width("Grid Fill") + 100;

  s.setWidth(w);

  return s;
}

//------

CQChartsAxisEdit::
CQChartsAxisEdit(QWidget *parent, CQChartsAxis *axis) :
 QFrame(parent), axis_(axis)
{
  setObjectName("axisEdit");

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  data_.visible        = axis->isVisible();
  data_.direction      = axis->direction();
  data_.side           = axis->side();
  data_.valueType      = axis->valueType();
  data_.format         = axis->format();
  data_.tickIncrement  = axis->tickIncrement ().integerOr(0);
  data_.majorIncrement = axis->majorIncrement().integerOr(0);
  data_.start          = axis->start();
  data_.end            = axis->end();
  data_.includeZero    = axis->isIncludeZero();
  data_.position       = axis->position();

  data_.lineData          = axis->axesLineData();
  data_.tickLabelTextData = axis->axesTickLabelTextData();
  data_.labelTextData     = axis->axesLabelTextData();
  data_.majorGridLineData = axis->axesMajorGridLineData();
  data_.minorGridLineData = axis->axesMinorGridLineData();
  data_.gridFillData      = axis->axesGridFillData();

  //---

  // visible
  groupBox_ = CQUtil::makeWidget<CQGroupBox>("groupBox");

  groupBox_->setCheckable(true);
  groupBox_->setChecked(data_.visible);
  groupBox_->setTitle("Visible");

  layout->addWidget(groupBox_);

  //---

  QGridLayout *groupLayout = CQUtil::makeLayout<QGridLayout>(groupBox_, 2, 2);

  int row = 0;

  //--

  // direction
  directionEdit_ = new CQHRadioButtons(nullptr, "Horizontal", "Vertical", nullptr);

  directionEdit_->setCurrentName(data_.direction == Qt::Horizontal ? "Horizontal" : "Vertical");

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Direction", directionEdit_, row);

  //--

  // side
  sideEdit_ = CQUtil::makeWidget<CQChartsAxisSideEdit>("sideEdit");

  sideEdit_->setAxisSide(data_.side);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Side", sideEdit_, row);

  //--

  // valueType
  valueTypeCombo_ = CQUtil::makeWidget<QComboBox>("valueTypeCombo");

  valueTypeCombo_->addItems(data_.valueType.enumNames());

  valueTypeCombo_->setCurrentIndex(valueTypeToInd(data_.valueType));

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Integral", valueTypeCombo_, row);

  //--

  // format
  formatEdit_ = CQUtil::makeWidget<CQLineEdit>("formatEdit");

  formatEdit_->setText(data_.format);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Format", formatEdit_, row);

  //--

  // tickIncrement
  tickIncrementEdit_ = CQUtil::makeWidget<CQIntegerSpin>("tickIncrementEdit");

  tickIncrementEdit_->setValue(data_.tickIncrement);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Tick Increment", tickIncrementEdit_, row);

  //--

  // majorIncrement
  majorIncrementEdit_ = CQUtil::makeWidget<CQIntegerSpin>("majorIncrementEdit");

  majorIncrementEdit_->setValue(data_.majorIncrement);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Major Increment", majorIncrementEdit_, row);

  //--

  // start
  startEdit_ = CQUtil::makeWidget<CQRealSpin>("startEdit");

  startEdit_->setValue(data_.start);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Start", startEdit_, row);

  //--

  // end
  endEdit_ = CQUtil::makeWidget<CQRealSpin>("endEdit");

  endEdit_->setValue(data_.end);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "End", endEdit_, row);

  //--

  // includeZero
  includeZeroEdit_ = CQUtil::makeWidget<CQCheckBox>("includeZeroEdit");

  includeZeroEdit_->setChecked(data_.includeZero);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Include Zero", includeZeroEdit_, row);

  //--

  // position
  positionEdit_ = CQUtil::makeWidget<CQLineEdit>("positionEdit");

  positionEdit_->setText(data_.position.toString());

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Position", positionEdit_, row);

  //--

  // line
  lineDataEdit_ = CQUtil::makeWidget<CQChartsLineDataEdit>("lineDataEdit");

  lineDataEdit_->setTitle("Line");
  lineDataEdit_->setPreview(false);
  lineDataEdit_->setPlot(axis_->plot());
  lineDataEdit_->setView(axis_->view());
  lineDataEdit_->setData(data_.lineData);

  groupLayout->addWidget(lineDataEdit_, row, 0, 1, 2); ++row;

  //------

  QTabWidget *labelTab = CQUtil::makeWidget<QTabWidget>("labelTab");

  layout->addWidget(labelTab);

  //--

  // label
  QFrame *labelFrame = CQUtil::makeWidget<QFrame>("labelFrame");

  QVBoxLayout *labelLayout = CQUtil::makeLayout<QVBoxLayout>(labelFrame, 0, 2);

  labelTab->addTab(labelFrame, "Axis Label");

  labelTextDataEdit_ = CQUtil::makeWidget<CQChartsTextDataEdit>("labelTextDataEdit");

//labelTextDataEdit_->setTitle("Label");
  labelTextDataEdit_->setPreview(false);
  labelTextDataEdit_->setPlot(axis_->plot());
  labelTextDataEdit_->setView(axis_->view());
  labelTextDataEdit_->setData(data_.labelTextData);

  labelLayout->addWidget(labelTextDataEdit_);

  //--

  // tick label
  QFrame *tickLabelFrame = CQUtil::makeWidget<QFrame>("tickLabelFrame");

  QVBoxLayout *tickLabelLayout = CQUtil::makeLayout<QVBoxLayout>(tickLabelFrame, 0, 2);

  labelTab->addTab(tickLabelFrame, "Tick Label");

  tickLabelTextDataEdit_ = CQUtil::makeWidget<CQChartsTextDataEdit>("tickLabelTextDataEdit");

//tickLabelTextDataEdit_->setTitle("Tick Label");
  tickLabelTextDataEdit_->setPreview(false);
  tickLabelTextDataEdit_->setPlot(axis_->plot());
  tickLabelTextDataEdit_->setView(axis_->view());
  tickLabelTextDataEdit_->setData(data_.tickLabelTextData);

  tickLabelLayout->addWidget(tickLabelTextDataEdit_);

  //---

  QTabWidget *gridTab = CQUtil::makeWidget<QTabWidget>("gridTab");

  layout->addWidget(gridTab);

  //--

  // major grid line
  QFrame *majorGridLineFrame = CQUtil::makeWidget<QFrame>("majorGridLineFrame");

  QVBoxLayout *majorGridLineLayout = CQUtil::makeLayout<QVBoxLayout>(majorGridLineFrame, 0, 2);

  gridTab->addTab(majorGridLineFrame, "Major Grid Line");

  majorGridLineDataEdit_ = CQUtil::makeWidget<CQChartsLineDataEdit>("majorGridLineDataEdit");

//majorGridLineDataEdit_->setTitle("Major Grid Line");
  majorGridLineDataEdit_->setPreview(false);
  majorGridLineDataEdit_->setPlot(axis_->plot());
  majorGridLineDataEdit_->setView(axis_->view());
  majorGridLineDataEdit_->setData(data_.majorGridLineData);

  majorGridLineLayout->addWidget(majorGridLineDataEdit_);

  //---

  // minor grid line
  QFrame *minorGridLineFrame = CQUtil::makeWidget<QFrame>("minorGridLineFrame");

  QVBoxLayout *minorGridLineLayout = CQUtil::makeLayout<QVBoxLayout>(minorGridLineFrame, 0, 2);

  gridTab->addTab(minorGridLineFrame, "Minor Grid Line");

  minorGridLineDataEdit_ = CQUtil::makeWidget<CQChartsLineDataEdit>("minorGridLineDataEdit");

//minorGridLineDataEdit_->setTitle("Minor Grid Line");
  minorGridLineDataEdit_->setPreview(false);
  minorGridLineDataEdit_->setPlot(axis_->plot());
  minorGridLineDataEdit_->setView(axis_->view());
  minorGridLineDataEdit_->setData(data_.minorGridLineData);

  minorGridLineLayout->addWidget(minorGridLineDataEdit_);

  //---

  // grid fill
  QFrame *gridFillFrame = CQUtil::makeWidget<QFrame>("gridFillFrame");

  QVBoxLayout *gridFillLayout = CQUtil::makeLayout<QVBoxLayout>(gridFillFrame, 0, 2);

  gridTab->addTab(gridFillFrame, "Grid Fill");

  gridFillDataEdit_ = CQUtil::makeWidget<CQChartsFillDataEdit>("gridFillDataEdit");

//gridFillDataEdit_->setTitle("Grid Fill");
  gridFillDataEdit_->setPreview(false);
  gridFillDataEdit_->setPlot(axis_->plot());
  gridFillDataEdit_->setView(axis_->view());
  gridFillDataEdit_->setData(data_.gridFillData);

  gridFillLayout->addWidget(gridFillDataEdit_);

  //---

  layout->addStretch(1);

  //---

  connectSlots(true);

  widgetsToData();
}

void
CQChartsAxisEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    if (b)
      connect(w, from, this, to);
    else
      disconnect(w, from, this, to);
  };

  connectDisconnect(b, groupBox_, SIGNAL(clicked(bool)), SLOT(widgetsToData()));
  connectDisconnect(b, valueTypeCombo_, SIGNAL(currentIndexChanged(int)), SLOT(widgetsToData()));
  connectDisconnect(b, formatEdit_, SIGNAL(editingFinished()), SLOT(widgetsToData()));
  connectDisconnect(b, tickIncrementEdit_, SIGNAL(valueChanged(int)), SLOT(widgetsToData()));
  connectDisconnect(b, majorIncrementEdit_, SIGNAL(valueChanged(int)), SLOT(widgetsToData()));
  connectDisconnect(b, startEdit_, SIGNAL(valueChanged(double)), SLOT(widgetsToData()));
  connectDisconnect(b, endEdit_, SIGNAL(valueChanged(double)), SLOT(widgetsToData()));
  connectDisconnect(b, includeZeroEdit_, SIGNAL(toggled(bool)), SLOT(widgetsToData()));
  connectDisconnect(b, positionEdit_, SIGNAL(editingFinished()), SLOT(widgetsToData()));
  connectDisconnect(b, lineDataEdit_, SIGNAL(lineDataChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, tickLabelTextDataEdit_, SIGNAL(textDataChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, labelTextDataEdit_, SIGNAL(textDataChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, majorGridLineDataEdit_, SIGNAL(lineDataChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, minorGridLineDataEdit_, SIGNAL(lineDataChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, gridFillDataEdit_, SIGNAL(fillDataChanged()), SLOT(widgetsToData()));
}

void
CQChartsAxisEdit::
dataToWidgets()
{
  connectSlots(false);

  groupBox_             ->setChecked(data_.visible);
//directionEdit_        ->setDirection(data_.direction);
  sideEdit_             ->setAxisSide(data_.side);
  valueTypeCombo_       ->setCurrentIndex(valueTypeToInd(data_.valueType));
  formatEdit_           ->setText(data_.format);
  tickIncrementEdit_    ->setValue(data_.tickIncrement);
  majorIncrementEdit_   ->setValue(data_.majorIncrement);
  startEdit_            ->setValue(data_.start);
  endEdit_              ->setValue(data_.end);
  includeZeroEdit_      ->setChecked(data_.includeZero);
  positionEdit_         ->setText(data_.position.toString());
  lineDataEdit_         ->setData(data_.lineData);
  tickLabelTextDataEdit_->setData(data_.tickLabelTextData);
  labelTextDataEdit_    ->setData(data_.labelTextData);
  majorGridLineDataEdit_->setData(data_.majorGridLineData);
  minorGridLineDataEdit_->setData(data_.minorGridLineData);
  gridFillDataEdit_     ->setData(data_.gridFillData);

  connectSlots(true);
}

void
CQChartsAxisEdit::
widgetsToData()
{
  data_.visible           = groupBox_->isChecked();
//data_.direction         = directionEdit_->direction();
  data_.side              = sideEdit_->axisSide();
  data_.valueType         = indToValueType(valueTypeCombo_->currentIndex());
  data_.format            = formatEdit_->text();
  data_.tickIncrement     = tickIncrementEdit_ ->value();
  data_.majorIncrement    = majorIncrementEdit_->value();
  data_.start             = startEdit_->value();
  data_.end               = endEdit_->value();
  data_.includeZero       = includeZeroEdit_->isChecked();
  data_.position          = CQChartsOptReal(positionEdit_->text());
  data_.lineData          = lineDataEdit_->data();
  data_.tickLabelTextData = tickLabelTextDataEdit_->data();
  data_.labelTextData     = labelTextDataEdit_->data();
  data_.majorGridLineData = majorGridLineDataEdit_->data();
  data_.minorGridLineData = minorGridLineDataEdit_->data();
  data_.gridFillData      = gridFillDataEdit_->data();

  emit axisChanged();
}

void
CQChartsAxisEdit::
applyData()
{
  axis_->setVisible       (data_.visible);
//axis_->setDirection     (data_.direction);
  axis_->setSide          (data_.side);
  axis_->setValueType     (data_.valueType);
  axis_->setFormat        (data_.format);
  axis_->setTickIncrement (data_.tickIncrement);
  axis_->setMajorIncrement(data_.majorIncrement);
  axis_->setStart         (data_.start);
  axis_->setEnd           (data_.end);
  axis_->setIncludeZero   (data_.includeZero);
  axis_->setPosition      (data_.position);

  axis_->setAxesLineData         (data_.lineData);
  axis_->setAxesTickLabelTextData(data_.tickLabelTextData);
  axis_->setAxesLabelTextData    (data_.labelTextData);
  axis_->setAxesMajorGridLineData(data_.majorGridLineData);
  axis_->setAxesMinorGridLineData(data_.minorGridLineData);
  axis_->setAxesGridFillData     (data_.gridFillData);
}
