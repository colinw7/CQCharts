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
CQChartsEditKeyDlg(QWidget *parent, CQChartsKey *key) :
 QDialog(parent), key_(key)
{
  if (key_->plot())
    setWindowTitle(QString("Edit Plot Key (%1)").arg(key_->plot()->id()));
  else
    setWindowTitle("Edit View Key");

  //---

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

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

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

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
    data_.flipped          = plotKey->isFlipped();
    data_.insideX          = plotKey->isInsideX();
    data_.insideY          = plotKey->isInsideY();
    data_.absolutePosition = plotKey->absolutePosition();
    data_.spacing          = plotKey->spacing();
    data_.scrollWidth      = plotKey->scrollWidth();
    data_.scrollHeight     = plotKey->scrollHeight();
  }

  data_.textBoxData.setText(key_->textData());
  data_.textBoxData.setBox (key_->boxData());

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

  // horizontal
  horizontalEdit_ = CQUtil::makeWidget<CQCheckBox>("horizontalEdit");

  horizontalEdit_->setChecked(data_.horizontal);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Horizontal", horizontalEdit_, row);

  //---

  if (plotKey) {
    // flipped
    flippedEdit_ = CQUtil::makeWidget<CQCheckBox>("flippedEdit");

    flippedEdit_->setChecked(data_.flipped);

    CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Flipped", flippedEdit_, row);
  }

  //--

  // autoHide
  autoHideEdit_ = CQUtil::makeWidget<CQCheckBox>("autoHideEdit");

  autoHideEdit_->setChecked(data_.autoHide);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Auto Hide", autoHideEdit_, row);

  //--

  // clipped
  clippedEdit_ = CQUtil::makeWidget<CQCheckBox>("clippedEdit");

  clippedEdit_->setChecked(data_.clipped);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Clipped", clippedEdit_, row);

  //----

  CQGroupBox *placementGroup = CQUtil::makeLabelWidget<CQGroupBox>("Placement", "placementGroup");

  QGridLayout *placementGroupLayout = CQUtil::makeLayout<QGridLayout>(placementGroup, 0, 2);

  groupLayout->addWidget(placementGroup, row, 0, 1, 2); ++row;

  int placementRow = 0;

  //--

  // above
  aboveEdit_ = CQUtil::makeWidget<CQCheckBox>("aboveEdit");

  aboveEdit_->setChecked(data_.above);

  CQChartsWidgetUtil::addGridLabelWidget(placementGroupLayout, "Above", aboveEdit_, placementRow);

  //--

  if (plotKey) {
    // insideX
    insideXEdit_ = CQUtil::makeWidget<CQCheckBox>("insideXEdit");

    insideXEdit_->setChecked(data_.insideX);

    CQChartsWidgetUtil::addGridLabelWidget(placementGroupLayout, "Inside X",
                                           insideXEdit_, placementRow);

    //--

    // insideY
    insideYEdit_ = CQUtil::makeWidget<CQCheckBox>("insideYEdit");

    insideYEdit_->setChecked(data_.insideY);

    CQChartsWidgetUtil::addGridLabelWidget(placementGroupLayout, "Inside Y",
                                           insideYEdit_, placementRow);
  }

  //--

  // location
  locationEdit_ = CQUtil::makeWidget<CQChartsKeyLocationEdit>("location");

  locationEdit_->setKeyLocation(data_.location);

  CQChartsWidgetUtil::addGridLabelWidget(placementGroupLayout, "Location",
                                         locationEdit_, placementRow);

  //--

  if (plotKey) {
    // absolutePosition
    absolutePositionEdit_ = CQUtil::makeWidget<CQPoint2DEdit>("absolutePositionEdit");

    absolutePositionEdit_->setValue(data_.absolutePosition);

    CQChartsWidgetUtil::addGridLabelWidget(placementGroupLayout, "Abs Position",
                                           absolutePositionEdit_, placementRow);
  }

  //----

  // interactive
  interactiveEdit_ = CQUtil::makeWidget<CQCheckBox>("interactiveEdit");

  interactiveEdit_->setChecked(data_.interactive);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Interactive", interactiveEdit_, row);

  //--

  // pressBehavior
  pressBehaviorEdit_ = CQUtil::makeWidget<CQChartsKeyPressBehaviorEdit>("pressBehavior");

  pressBehaviorEdit_->setKeyPressBehavior(data_.pressBehavior);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Press Behavior", pressBehaviorEdit_, row);

  //--

  // hiddenAlpha
  hiddenAlphaEdit_ = CQUtil::makeWidget<CQChartsAlphaEdit>("hiddenAlphaEdit");

  hiddenAlphaEdit_->setValue(data_.hiddenAlpha);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Hidden Alpha", hiddenAlphaEdit_, row);

  //--

  // maxRows
  maxRowsEdit_ = CQUtil::makeWidget<CQIntegerSpin>("maxRowsEdit");

  maxRowsEdit_->setValue(data_.maxRows);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Max Rows", maxRowsEdit_, row);

  //--

  if (plotKey) {
    // spacing
    spacingEdit_ = CQUtil::makeWidget<CQIntegerSpin>("spacingEdit");

    spacingEdit_->setValue(data_.spacing);

    CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Spacing", spacingEdit_, row);

    //---

    // scrollWidth
    scrollWidthEdit_ = CQUtil::makeWidget<CQLineEdit>("scrollWidthEdit");

    scrollWidthEdit_->setText(data_.scrollWidth.toString());

    CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Scroll Width", scrollWidthEdit_, row);

    //---

    // scrollHeight
    scrollHeightEdit_ = CQUtil::makeWidget<CQLineEdit>("scrollHeightEdit");

    scrollHeightEdit_->setText(data_.scrollHeight.toString());

    CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Scroll Height", scrollHeightEdit_, row);
  }

  //----

  CQGroupBox *headerGroup = CQUtil::makeLabelWidget<CQGroupBox>("Header", "headerGroup");

  QGridLayout *headerGroupLayout = CQUtil::makeLayout<QGridLayout>(headerGroup, 0, 2);

  groupLayout->addWidget(headerGroup, row, 0, 1, 2); ++row;

  int headerRow = 0;

  //--

  // header
  headerEdit_ = CQUtil::makeWidget<CQLineEdit>("headerEdit");

  headerEdit_->setText(data_.header);

  CQChartsWidgetUtil::addGridLabelWidget(headerGroupLayout, "Text", headerEdit_, headerRow);

  //--

  // header text data
  headerTextDataEdit_ = new CQChartsTextDataEdit(nullptr, /*optional*/false);

  headerTextDataEdit_->setPreview(false);
  headerTextDataEdit_->setPlot(key_->plot());
  headerTextDataEdit_->setView(key_->view());
  headerTextDataEdit_->setData(data_.headerTextData);

  headerGroupLayout->addWidget(headerTextDataEdit_, headerRow, 0, 1, 2); ++headerRow;

  //--

  // box (margin, passing, fill, stroke, text)
  textBoxEdit_ = new CQChartsTextBoxDataEdit(nullptr, /*tabbed*/true);

  textBoxEdit_->setPreview(false);
  textBoxEdit_->setPlot(key_->plot());
  textBoxEdit_->setView(key_->view());
  textBoxEdit_->setData(data_.textBoxData);

  groupLayout->addWidget(textBoxEdit_, row, 0, 1, 2); ++row;

  //---

  groupLayout->setRowStretch(row, 1);

  //---

  connectSlots(true);

  widgetsToData();
}

void
CQChartsKeyEdit::
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

  CQChartsPlotKey *plotKey = dynamic_cast<CQChartsPlotKey *>(key_);

  connectDisconnect(b, groupBox_, SIGNAL(clicked(bool)), SLOT(widgetsToData()));
  connectDisconnect(b, horizontalEdit_, SIGNAL(toggled(bool)), SLOT(widgetsToData()));
  connectDisconnect(b, autoHideEdit_, SIGNAL(toggled(bool)), SLOT(widgetsToData()));
  connectDisconnect(b, clippedEdit_, SIGNAL(toggled(bool)), SLOT(widgetsToData()));
  connectDisconnect(b, aboveEdit_, SIGNAL(toggled(bool)), SLOT(widgetsToData()));
  connectDisconnect(b, locationEdit_, SIGNAL(keyLocationChanged()), SLOT(widgetsToData()));
  connectDisconnect(b, hiddenAlphaEdit_, SIGNAL(valueChanged(double)), SLOT(widgetsToData()));
  connectDisconnect(b, maxRowsEdit_, SIGNAL(valueChanged(int)), SLOT(widgetsToData()));
  connectDisconnect(b, interactiveEdit_, SIGNAL(toggled(bool)), SLOT(widgetsToData()));
  connectDisconnect(b, pressBehaviorEdit_, SIGNAL(keyPressBehaviorChanged()),
                    SLOT(widgetsToData()));
  connectDisconnect(b, headerEdit_, SIGNAL(textChanged(const QString &)), SLOT(widgetsToData()));
  connectDisconnect(b, headerTextDataEdit_, SIGNAL(textDataChanged()), SLOT(widgetsToData()));

  if (plotKey) {
    connectDisconnect(b, absolutePositionEdit_, SIGNAL(valueChanged()), SLOT(widgetsToData()));
    connectDisconnect(b, insideXEdit_, SIGNAL(toggled(bool)), SLOT(widgetsToData()));
    connectDisconnect(b, insideYEdit_, SIGNAL(toggled(bool)), SLOT(widgetsToData()));
    connectDisconnect(b, spacingEdit_, SIGNAL(valueChanged(int)), SLOT(widgetsToData()));
    connectDisconnect(b, flippedEdit_, SIGNAL(toggled(bool)), SLOT(widgetsToData()));
    connectDisconnect(b, scrollWidthEdit_, SIGNAL(textChanged(const QString &)),
                      SLOT(widgetsToData()));
    connectDisconnect(b, scrollHeightEdit_, SIGNAL(textChanged(const QString &)),
                      SLOT(widgetsToData()));
    connectDisconnect(b, textBoxEdit_, SIGNAL(textBoxDataChanged()), SLOT(widgetsToData()));
  }
}

void
CQChartsKeyEdit::
dataToWidgets()
{
  connectSlots(false);

  CQChartsPlotKey *plotKey = dynamic_cast<CQChartsPlotKey *>(key_);

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
    absolutePositionEdit_->setValue(data_.absolutePosition);
    insideXEdit_         ->setChecked(data_.insideX);
    insideYEdit_         ->setChecked(data_.insideY);
    spacingEdit_         ->setValue(data_.spacing);
    flippedEdit_         ->setChecked(data_.flipped);
    scrollWidthEdit_     ->setText(data_.scrollWidth.toString());
    scrollHeightEdit_    ->setText(data_.scrollHeight.toString());
  }

  textBoxEdit_->setData(data_.textBoxData);

  connectSlots(true);
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
    data_.absolutePosition  = absolutePositionEdit_->getQValue();
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
    plotKey->setAbsolutePosition(data_.absolutePosition);
    plotKey->setInsideX         (data_.insideX);
    plotKey->setInsideY         (data_.insideY);
    plotKey->setSpacing         (data_.spacing);
    plotKey->setFlipped         (data_.flipped);
    plotKey->setScrollWidth     (data_.scrollWidth);
    plotKey->setScrollHeight    (data_.scrollHeight);
  }

  key_->setTextData(data_.textBoxData.text());
  key_->setBoxData (data_.textBoxData.box ());
}
