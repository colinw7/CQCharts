#include <CQChartsKeyEdit.h>
#include <CQChartsKey.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsKeyLocationEdit.h>
#include <CQChartsAlphaEdit.h>
#include <CQChartsTextBoxDataEdit.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>

#include <CQGroupBox.h>
#include <CQCheckBox.h>
#include <CQIntegerSpin.h>
#include <CQUtil.h>

#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>

CQChartsEditKeyDlg::
CQChartsEditKeyDlg(CQChartsKey *key) :
 QDialog(), key_(key)
{
  setWindowTitle("Edit Key");

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

  data_.visible       = key->isVisible();
  data_.horizontal    = key->isHorizontal();
  data_.autoHide      = key->isAutoHide();
  data_.clipped       = key->isClipped();
  data_.above         = key->isAbove();
  data_.interactive   = key->isInteractive();
  data_.location      = key->location();
  data_.header        = key->headerStr();
  data_.hiddenAlpha   = key->hiddenAlpha();
  data_.maxRows       = key->maxRows();
//data_.pressBehavior = key->pressBehavior();

  data_.textBoxData.setText(key->textData());
  data_.textBoxData.setBox (key->boxData());

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

  //--

  // above
  aboveEdit_ = CQUtil::makeWidget<CQCheckBox>("aboveEdit");

  aboveEdit_->setChecked(data_.above);

  connect(aboveEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Above", aboveEdit_, row);

  //--

  // interactive
  interactiveEdit_ = CQUtil::makeWidget<CQCheckBox>("interactiveEdit");

  interactiveEdit_->setChecked(data_.interactive);

  connect(interactiveEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Interactive", interactiveEdit_, row);

  //--

  // location
  locationEdit_ = CQUtil::makeWidget<CQChartsKeyLocationEdit>("location");

  locationEdit_->setKeyLocation(data_.location);

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Location", locationEdit_, row);

  connect(locationEdit_, SIGNAL(keyLocationChanged()), this, SLOT(widgetsToData()));

  //--

  // header
  headerEdit_ = CQUtil::makeWidget<QLineEdit>("headerEdit");

  headerEdit_->setText(data_.header);

  connect(headerEdit_, SIGNAL(editingFinished()), this, SLOT(widgetsToData()));

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Header", headerEdit_, row);

  //--

  // hiddenAlpha
  hiddenAlphaEdit_ = CQUtil::makeWidget<CQChartsAlphaEdit>("hiddenAlphaEdit");

  hiddenAlphaEdit_->setValue(data_.hiddenAlpha);

  connect(hiddenAlphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Header Alpha", hiddenAlphaEdit_, row);

  //--

  // maxRows
  maxRowsEdit_ = CQUtil::makeWidget<CQIntegerSpin>("maxRowsEdit");

  maxRowsEdit_->setValue(data_.maxRows);

  connect(maxRowsEdit_, SIGNAL(valueChanged(int)), this, SLOT(widgetsToData()));

  CQChartsWidgetUtil::addGridLabelWidget(groupLayout, "Max Rows", maxRowsEdit_, row);

  //--

  // box (margin, passing, fill, border, text)
  textBoxEdit_ = CQUtil::makeWidget<CQChartsTextBoxDataEdit>("textBoxEdit");

  textBoxEdit_->setTitle("Box");
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
  disconnect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));
  disconnect(horizontalEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  disconnect(autoHideEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  disconnect(clippedEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  disconnect(aboveEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  disconnect(interactiveEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  disconnect(locationEdit_, SIGNAL(keyLocationChanged()), this, SLOT(widgetsToData()));
  disconnect(headerEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  disconnect(hiddenAlphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  disconnect(maxRowsEdit_, SIGNAL(valueChanged(int)), this, SLOT(widgetsToData()));
  disconnect(textBoxEdit_, SIGNAL(textBoxDataChanged()), this, SLOT(widgetsToData()));

  groupBox_       ->setChecked(data_.visible);
  horizontalEdit_ ->setChecked(data_.horizontal);
  autoHideEdit_   ->setChecked(data_.autoHide);
  clippedEdit_    ->setChecked(data_.clipped);
  aboveEdit_      ->setChecked(data_.above);
  interactiveEdit_->setChecked(data_.interactive);
  locationEdit_   ->setKeyLocation(data_.location);
  headerEdit_     ->setText(data_.header);
  hiddenAlphaEdit_->setValue(data_.hiddenAlpha);
  maxRowsEdit_    ->setValue(data_.maxRows);
  textBoxEdit_    ->setData(data_.textBoxData);

  connect(groupBox_, SIGNAL(clicked(bool)), this, SLOT(widgetsToData()));
  connect(horizontalEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  connect(autoHideEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  connect(clippedEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  connect(aboveEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  connect(interactiveEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  connect(locationEdit_, SIGNAL(keyLocationChanged()), this, SLOT(widgetsToData()));
  connect(headerEdit_, SIGNAL(toggled(bool)), this, SLOT(widgetsToData()));
  connect(hiddenAlphaEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToData()));
  connect(maxRowsEdit_, SIGNAL(valueChanged(int)), this, SLOT(widgetsToData()));
  connect(textBoxEdit_, SIGNAL(textBoxDataChanged()), this, SLOT(widgetsToData()));
}

void
CQChartsKeyEdit::
widgetsToData()
{
  data_.visible       = groupBox_->isChecked();
  data_.horizontal    = horizontalEdit_->isChecked();
  data_.autoHide      = autoHideEdit_->isChecked();
  data_.clipped       = clippedEdit_->isChecked();
  data_.above         = aboveEdit_->isChecked();
  data_.interactive   = interactiveEdit_->isChecked();
  data_.location      = locationEdit_->keyLocation();
  data_.header        = headerEdit_->text();
  data_.hiddenAlpha   = hiddenAlphaEdit_->value();
  data_.maxRows       = maxRowsEdit_->value();
//data_.pressBehavior = pressBehaviorEdit_->//pressBehavior();
  data_.textBoxData   = textBoxEdit_->data();

  emit keyChanged();
}

void
CQChartsKeyEdit::
applyData()
{
  key_->setVisible      (data_.visible);
  key_->setHorizontal   (data_.horizontal);
  key_->setAutoHide     (data_.autoHide);
  key_->setClipped      (data_.clipped);
  key_->setAbove        (data_.above);
  key_->setInteractive  (data_.interactive);
  key_->setLocation     (data_.location);
  key_->setHeaderStr    (data_.header);
  key_->setHiddenAlpha  (data_.hiddenAlpha);
  key_->setMaxRows      (data_.maxRows);
//key_->setPressBehavior(data_.pressBehavior);

  key_->setTextData(data_.textBoxData.text());
  key_->setBoxData (data_.textBoxData.box ());
}
