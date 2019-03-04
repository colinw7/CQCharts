#include <CQChartsKeyEdit.h>
#include <CQChartsKey.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsKeyLocationEdit.h>
#include <CQChartsKeyPressBehaviorEdit.h>
#include <CQChartsAlphaEdit.h>
#include <CQChartsTextDataEdit.h>
#include <CQChartsTextBoxDataEdit.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>

#include <CQGroupBox.h>
#include <CQCheckBox.h>
#include <CQIntegerSpin.h>
#include <CQPoint2DEdit.h>
#include <CQUtil.h>

#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>

CQChartsEditKeyDlg::
CQChartsEditKeyDlg(CQChartsKey *key) :
 QDialog(), key_(key)
{
  if (key_->plot())
    setWindowTitle(QString("Edit Plot Key (%1)").arg(key_->plot()->id()));
  else
    setWindowTitle("Edit View Key");

  //---

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  //---

  edit_ = new CQChartsKeyEdit(nullptr, key_);

  layout->addWidget(edit_);

  //---

  CQChartsDialogButtons *buttons = new CQChartsDialogButtons;

  buttons->connect(this, SLOT(okSlot()), SLOT(applySlot()), SLOT(cancelSlot()));

  layout->addWidget(buttons);
}

void
CQChartsEditKeyDlg::
okSlot()
{
  if (applySlot())
    cancelSlot();
}

bool
CQChartsEditKeyDlg::
applySlot()
{
  edit_->applyData();

  return true;
}

void
CQChartsEditKeyDlg::
cancelSlot()
{
  close();
}

//------

CQChartsKeyEdit::
CQChartsKeyEdit(QWidget *parent, CQChartsKey *key) :
 QFrame(parent), key_(key)
{
  setObjectName("keyEdit");

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  //---

  data_.visible        = key_->isVisible();
  data_.horizontal     = key_->isHorizontal();
  data_.autoHide       = key_->isAutoHide();
  data_.clipped        = key_->isClipped();
  data_.above          = key_->isAbove();
  data_.location       = key_->location();
  data_.hiddenAlpha    = key_->hiddenAlpha();
  data_.maxRows        = key_->maxRows();
  data_.interactive    = key_->isInteractive();
  data_.pressBehavior  = key_->pressBehavior();
  data_.header         = key_->headerStr();
  data_.headerTextData = key_->headerTextData();

  CQChartsPlotKey *plotKey = dynamic_cast<CQChartsPlotKey *>(key_);

  if (plotKey) {
    data_.flipped      = plotKey->isFlipped();
    data_.insideX      = plotKey->isInsideX();
    data_.insideY      = plotKey->isInsideY();
    data_.absPosition  = plotKey->absPosition();
    data_.spacing      = plotKey->spacing();
    data_.scrollWidth  = plotKey->scrollWidth();
    data_.scrollHeight = plotKey->scrollHeight();
  }

  data_.textBoxData.setText(key_->textData());
  data_.textBoxData.setBox (key_->boxData());

  //---

  // visible
  groupBox_ = CQUtil::makeWidget<CQGroupBox>("groupBox");

  groupBox_->setCheckable(true);
  groupBox_->setChecked(data_.visible);
  groupBox_->setTitle("Visible");

  connect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));

  layout->addWidget(groupBox_);

  //---

  QGridLayout *groupLayout = new QGridLayout(groupBox_);
  groupLayout->setMargin(2); groupLayout->setSpacing(2);

  int row = 0;

  //--

  // horizontal
  horizontalEdit_ = CQUtil::makeWidget<CQCheckBox>("horizontalEdit");

  horizontalEdit_->setChecked(data_.horizontal);

  connect(horizontalEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Horizontal", horizontalEdit_, row);

  //---

  if (plotKey) {
    // flipped
    flippedEdit_ = CQUtil::makeWidget<CQCheckBox>("flippedEdit");

    flippedEdit_->setChecked(data_.flipped);

    connect(flippedEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));

    CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Flipped", flippedEdit_, row);
  }

  //--

  // autoHide
  autoHideEdit_ = CQUtil::makeWidget<CQCheckBox>("autoHideEdit");

  autoHideEdit_->setChecked(data_.autoHide);

  connect(autoHideEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Auto Hide", autoHideEdit_, row);

  //--

  // clipped
  clippedEdit_ = CQUtil::makeWidget<CQCheckBox>("clippedEdit");

  clippedEdit_->setChecked(data_.clipped);

  connect(clippedEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Clipped", clippedEdit_, row);

  //----

  CQGroupBox *placementGroup = new CQGroupBox("Placement");
  placementGroup->setObjectName("placementGroup");

  QGridLayout *placementGroupLayout = new QGridLayout(placementGroup);
  placementGroupLayout->setMargin(0); placementGroupLayout->setSpacing(2);

  groupLayout->addWidget(placementGroup, row, 0, 1, 2); ++row;

  int placementRow = 0;

  //--

  // above
  aboveEdit_ = CQUtil::makeWidget<CQCheckBox>("aboveEdit");

  aboveEdit_->setChecked(data_.above);

  connect(aboveEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));

  CQChartsWidgetUtil::addGridLabelWidget(placementGroupLayout, "Above", aboveEdit_, placementRow);

  //--

  if (plotKey) {
    // insideX
    insideXEdit_ = CQUtil::makeWidget<CQCheckBox>("insideXEdit");

    insideXEdit_->setChecked(data_.insideX);

    connect(insideXEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));

    CQChartsWidgetUtil::addGridLabelWidget(placementGroupLayout, "Inside X",
                                           insideXEdit_, placementRow);

    //--

    // insideY
    insideYEdit_ = CQUtil::makeWidget<CQCheckBox>("insideYEdit");

    insideYEdit_->setChecked(data_.insideY);

    connect(insideYEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));

    CQChartsWidgetUtil::addGridLabelWidget(placementGroupLayout, "Inside Y",
                                           insideYEdit_, placementRow);
  }

  //--

  // location
  locationEdit_ = CQUtil::makeWidget<CQChartsKeyLocationEdit>("location");

  locationEdit_->setKeyLocation(data_.location);

  CQChartsWidgetUtil::addGridLabelWidget(placementGroupLayout, "Location",
                                         locationEdit_, placementRow);

  connect(locationEdit_, SIGNAL(keyLocationChanged()), this, SLOT(widgetsToData()));

  //--

  if (plotKey) {
    // absPosition
    absPositionEdit_ = CQUtil::makeWidget<CQPoint2DEdit>("absPositionEdit");

    absPositionEdit_->setValue(data_.absPosition);

    connect(absPositionEdit_, SIGNAL(valueChanged()), this, SLOT(widgetsToData()));

    CQChartsWidgetUtil::addGridLabelWidget(placementGroupLayout, "Abs Position",
                                           absPositionEdit_, placementRow);
  }

  //----

  // interactive
  interactiveEdit_ = CQUtil::makeWidget<CQCheckBox>("interactiveEdit");

  interactiveEdit_->setChecked(data_.interactive);

  connect(interactiveEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Interactive", interactiveEdit_, row);

  //--

  // pressBehavior
  pressBehaviorEdit_ = CQUtil::makeWidget<CQChartsKeyPressBehaviorEdit>("pressBehavior");

  pressBehaviorEdit_->setKeyPressBehavior(data_.pressBehavior);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Press Behavior", pressBehaviorEdit_, row);

  connect(pressBehaviorEdit_, SIGNAL(keyPressBehaviorChanged()), this, SLOT(widgetsToData()));

  //--

  // hiddenAlpha
  hiddenAlphaEdit_ = CQUtil::makeWidget<CQChartsAlphaEdit>("hiddenAlphaEdit");

  hiddenAlphaEdit_->setValue(data_.hiddenAlpha);

  connect(hiddenAlphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Hidden Alpha", hiddenAlphaEdit_, row);

  //--

  // maxRows
  maxRowsEdit_ = CQUtil::makeWidget<CQIntegerSpin>("maxRowsEdit");

  maxRowsEdit_->setValue(data_.maxRows);

  connect(maxRowsEdit_, SIGNAL(valueChanged(int)), this, SLOT(widgetsToData()));

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Max Rows", maxRowsEdit_, row);

  //--

  if (plotKey) {
    // spacing
    spacingEdit_ = CQUtil::makeWidget<CQIntegerSpin>("spacingEdit");

    spacingEdit_->setValue(data_.spacing);

    connect(spacingEdit_, SIGNAL(valueChanged(int)), this, SLOT(widgetsToData()));

    CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Spacing", spacingEdit_, row);

    //---

    // scrollWidth
    scrollWidthEdit_ = CQUtil::makeWidget<QLineEdit>("scrollWidthEdit");

    scrollWidthEdit_->setText(data_.scrollWidth.toString());

    connect(scrollWidthEdit_, SIGNAL(editingFinished()), this, SLOT(widgetsToData()));

    CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Scroll Width", scrollWidthEdit_, row);

    //---

    // scrollHeight
    scrollHeightEdit_ = CQUtil::makeWidget<QLineEdit>("scrollHeightEdit");

    scrollHeightEdit_->setText(data_.scrollHeight.toString());

    connect(scrollHeightEdit_, SIGNAL(editingFinished()), this, SLOT(widgetsToData()));

    CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Scroll Height", scrollHeightEdit_, row);
  }

  //----

  CQGroupBox *headerGroup = new CQGroupBox("Header");
  headerGroup->setObjectName("headerGroup");

  QGridLayout *headerGroupLayout = new QGridLayout(headerGroup);
  headerGroupLayout->setMargin(0); headerGroupLayout->setSpacing(2);

  groupLayout->addWidget(headerGroup, row, 0, 1, 2); ++row;

  int headerRow = 0;

  //--

  // header
  headerEdit_ = CQUtil::makeWidget<QLineEdit>("headerEdit");

  headerEdit_->setText(data_.header);

  connect(headerEdit_, SIGNAL(editingFinished()), this, SLOT(widgetsToData()));

  CQChartsWidgetUtil::addGridLabelWidget(headerGroupLayout, "Text", headerEdit_, headerRow);

  //--

  // header text data
  headerTextDataEdit_ = new CQChartsTextDataEdit(nullptr, /*optional*/false);

  headerTextDataEdit_->setPreview(false);
  headerTextDataEdit_->setPlot(key_->plot());
  headerTextDataEdit_->setView(key_->view());
  headerTextDataEdit_->setData(data_.headerTextData);

  connect(headerTextDataEdit_, SIGNAL(textDataChanged()), this, SLOT(widgetsToData()));

  headerGroupLayout->addWidget(headerTextDataEdit_, headerRow, 0, 1, 2); ++headerRow;

  //--

  // box (margin, passing, fill, border, text)
  textBoxEdit_ = new CQChartsTextBoxDataEdit(nullptr, /*tabbed*/true);

  textBoxEdit_->setPreview(false);
  textBoxEdit_->setPlot(key_->plot());
  textBoxEdit_->setView(key_->view());
  textBoxEdit_->setData(data_.textBoxData);

  connect(textBoxEdit_, SIGNAL(textBoxDataChanged()), this, SLOT(widgetsToData()));

  groupLayout->addWidget(textBoxEdit_, row, 0, 1, 2); ++row;

  //---

  groupLayout->setRowStretch(row, 1);

  //---

  widgetsToData();
}

void
CQChartsKeyEdit::
dataToWidgets()
{
  CQChartsPlotKey *plotKey = dynamic_cast<CQChartsPlotKey *>(key_);

  disconnect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));
  disconnect(horizontalEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  disconnect(autoHideEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  disconnect(clippedEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  disconnect(aboveEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  disconnect(locationEdit_, SIGNAL(keyLocationChanged()), this, SLOT(widgetsToData()));
  disconnect(hiddenAlphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  disconnect(maxRowsEdit_, SIGNAL(valueChanged(int)), this, SLOT(widgetsToData()));
  disconnect(interactiveEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  disconnect(pressBehaviorEdit_, SIGNAL(keyPressBehaviorChanged()), this, SLOT(widgetsToData()));
  disconnect(headerEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  disconnect(headerTextDataEdit_, SIGNAL(textDataChanged()), this, SLOT(widgetsToData()));

  if (plotKey) {
    disconnect(absPositionEdit_, SIGNAL(valueChanged()), this, SLOT(widgetsToData()));
    disconnect(insideXEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
    disconnect(insideYEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
    disconnect(spacingEdit_, SIGNAL(valueChanged(int)), this, SLOT(widgetsToData()));
    disconnect(flippedEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
    disconnect(scrollWidthEdit_, SIGNAL(textChanged(const QString &)),
               this, SLOT(widgetsToData()));
    disconnect(scrollHeightEdit_, SIGNAL(textChanged(const QString &)),
               this, SLOT(widgetsToData()));
    disconnect(textBoxEdit_, SIGNAL(textBoxDataChanged()), this, SLOT(widgetsToData()));
  }

  groupBox_          ->setChecked(data_.visible);
  horizontalEdit_    ->setChecked(data_.horizontal);
  autoHideEdit_      ->setChecked(data_.autoHide);
  clippedEdit_       ->setChecked(data_.clipped);
  aboveEdit_         ->setChecked(data_.above);
  locationEdit_      ->setKeyLocation(data_.location);
  hiddenAlphaEdit_   ->setValue(data_.hiddenAlpha);
  maxRowsEdit_       ->setValue(data_.maxRows);
  interactiveEdit_   ->setChecked(data_.interactive);
  pressBehaviorEdit_ ->setKeyPressBehavior(data_.pressBehavior);
  headerEdit_        ->setText(data_.header);
  headerTextDataEdit_->setData(data_.headerTextData);

  if (plotKey) {
    absPositionEdit_ ->setValue(data_.absPosition);
    insideXEdit_     ->setChecked(data_.insideX);
    insideYEdit_     ->setChecked(data_.insideY);
    spacingEdit_     ->setValue(data_.spacing);
    flippedEdit_     ->setChecked(data_.flipped);
    scrollWidthEdit_ ->setText(data_.scrollWidth.toString());
    scrollHeightEdit_->setText(data_.scrollHeight.toString());
  }

  textBoxEdit_->setData(data_.textBoxData);

  connect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));
  connect(horizontalEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  connect(autoHideEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  connect(clippedEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  connect(aboveEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  connect(locationEdit_, SIGNAL(keyLocationChanged()), this, SLOT(widgetsToData()));
  connect(hiddenAlphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  connect(maxRowsEdit_, SIGNAL(valueChanged(int)), this, SLOT(widgetsToData()));
  connect(interactiveEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  connect(pressBehaviorEdit_, SIGNAL(keyPressBehaviorChanged()), this, SLOT(widgetsToData()));
  connect(headerEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  connect(headerTextDataEdit_, SIGNAL(textDataChanged()), this, SLOT(widgetsToData()));

  if (plotKey) {
    connect(absPositionEdit_, SIGNAL(valueChanged()), this, SLOT(widgetsToData()));
    connect(insideXEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
    connect(insideYEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
    connect(spacingEdit_, SIGNAL(valueChanged(int)), this, SLOT(widgetsToData()));
    connect(flippedEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
    connect(scrollWidthEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(widgetsToData()));
    connect(scrollHeightEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(widgetsToData()));
    connect(textBoxEdit_, SIGNAL(textBoxDataChanged()), this, SLOT(widgetsToData()));
  }
}

void
CQChartsKeyEdit::
widgetsToData()
{
  CQChartsPlotKey *plotKey = dynamic_cast<CQChartsPlotKey *>(key_);

  data_.visible        = groupBox_->isChecked();
  data_.horizontal     = horizontalEdit_->isChecked();
  data_.autoHide       = autoHideEdit_->isChecked();
  data_.clipped        = clippedEdit_->isChecked();
  data_.above          = aboveEdit_->isChecked();
  data_.location       = locationEdit_->keyLocation();
  data_.hiddenAlpha    = hiddenAlphaEdit_->value();
  data_.maxRows        = maxRowsEdit_->value();
  data_.interactive    = interactiveEdit_->isChecked();
  data_.pressBehavior  = pressBehaviorEdit_->keyPressBehavior();
  data_.header         = headerEdit_->text();
  data_.headerTextData = headerTextDataEdit_->data();

  if (plotKey) {
    data_.absPosition  = absPositionEdit_->getQValue();
    data_.insideX      = insideXEdit_->isChecked();
    data_.insideY      = insideYEdit_->isChecked();
    data_.spacing      = spacingEdit_->value();
    data_.flipped      = flippedEdit_->isChecked();
    data_.scrollWidth  = CQChartsOptLength(scrollWidthEdit_->text());
    data_.scrollHeight = CQChartsOptLength(scrollHeightEdit_->text());
  }

  data_.textBoxData = textBoxEdit_->data();

  emit keyChanged();
}

void
CQChartsKeyEdit::
applyData()
{
  CQChartsPlotKey *plotKey = dynamic_cast<CQChartsPlotKey *>(key_);

  key_->setVisible      (data_.visible);
  key_->setHorizontal   (data_.horizontal);
  key_->setAutoHide     (data_.autoHide);
  key_->setClipped      (data_.clipped);
  key_->setAbove        (data_.above);
  key_->setLocation     (data_.location);
  key_->setHiddenAlpha  (data_.hiddenAlpha);
  key_->setMaxRows      (data_.maxRows);
  key_->setInteractive  (data_.interactive);
  key_->setPressBehavior(data_.pressBehavior);

  key_->setHeaderStr     (data_.header);
  key_->setHeaderTextData(data_.headerTextData);

  if (plotKey) {
    plotKey->setAbsPosition (data_.absPosition);
    plotKey->setInsideX     (data_.insideX);
    plotKey->setInsideY     (data_.insideY);
    plotKey->setSpacing     (data_.spacing);
    plotKey->setFlipped     (data_.flipped);
    plotKey->setScrollWidth (data_.scrollWidth);
    plotKey->setScrollHeight(data_.scrollHeight);
  }

  key_->setTextData(data_.textBoxData.text());
  key_->setBoxData (data_.textBoxData.box ());
}
