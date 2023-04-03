#include <CQChartsHierSepModelEdit.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsColumnCombo.h>
#include <CQCharts.h>

#include <CQHierSepModel.h>
#include <CQUtil.h>

#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>

CQChartsHierSepModelEdit::
CQChartsHierSepModelEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("hierSepModelEdit");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  enabledCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Enabled", "enabledCheck");

  enabledCheck_->setToolTip("Enable summary of model data");

  layout->addWidget(enabledCheck_);

  //---

  auto *editFrame  = CQUtil::makeWidget<QFrame>("editFrame");
  auto *editLayout = CQUtil::makeLayout<QGridLayout>(editFrame, 0, 2);

  layout->addWidget(editFrame);

  //---

  int row = 0;

  auto addEditWidget = [&](const QString &label, QWidget *edit, const QString &name) {
    editLayout->addWidget(CQUtil::makeLabelWidget<QLabel>(label, name + "Label"), row, 0);
    editLayout->addWidget(edit                                                  , row, 1);

    ++row;
  };

  //---

  separatorEdit_ = CQUtil::makeWidget<QLineEdit>("separatorEdit");
  columnEdit_    = CQUtil::makeWidget<CQChartsColumnCombo>("columnEdit");

  addEditWidget("Separator", separatorEdit_, "separator");
  addEditWidget("Column"   , columnEdit_   , "column");

  //---

  layout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQChartsHierSepModelEdit::
setModelData(ModelData *modelData)
{
  modelData_ = modelData;

  updateWidgetsFromModel();
}

//---

void
CQChartsHierSepModelEdit::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b,
    enabledCheck_, SIGNAL(stateChanged(int)), this, SLOT(enabledSlot()));

  CQUtil::connectDisconnect(b,
    separatorEdit_, SIGNAL(editingFinished()), this, SLOT(updateModelFromWidgets()));
  CQUtil::connectDisconnect(b,
    columnEdit_, SIGNAL(columnChanged()), this, SLOT(updateWidgetsFromModel()));
}

//---

void
CQChartsHierSepModelEdit::
enabledSlot()
{
  connectSlots(false);

  //---

  if (modelData_) {
    bool enabled = enabledCheck_->isChecked();

    if (enabledCheck_->isChecked())
      modelData_->addHierSepModel();

    modelData_->setHierSepEnabled(enabled);
  }

  connectSlots(true);

  //---

  updateWidgetsFromModel();
}

//---

// update widgets from state
void
CQChartsHierSepModelEdit::
updateWidgetsFromModel()
{
  connectSlots(false);

  auto *hierSepModel = (modelData_ ? modelData_->hierSepModel() : nullptr);

  //---

  if (modelData_) {
    auto model = modelData_->currentModel(/*proxy*/false);
    assert(model);

    //---

    enabledCheck_->setChecked(modelData_->isHierSepEnabled());

    //---

    columnEdit_->setProxy(false);
    columnEdit_->setModelData(modelData_);

    if (hierSepModel) {
      separatorEdit_->setText(QString(hierSepModel->separator()));

      columnEdit_->setColumn(CQChartsColumn(hierSepModel->foldColumn()));
    }
  }

  //--

  separatorEdit_->setEnabled(hierSepModel);
  columnEdit_   ->setEnabled(hierSepModel);

  //--

  connectSlots(true);
}

// update state from widgets
void
CQChartsHierSepModelEdit::
updateModelFromWidgets()
{
  auto *hierSepModel = (modelData_ ? modelData_->hierSepModel() : nullptr);

  if (modelData_->isHierSepEnabled() && hierSepModel) {
    auto model = modelData_->currentModel(/*proxy*/false);
    assert(model);

    //--

    bool changed = false;

    //--

    auto separator = separatorEdit_->text().at(0);

    if (separator != hierSepModel->separator()) {
      hierSepModel->setSeparator(separator); changed = true;
    }

    int column = columnEdit_->getColumn().column();

    if (column != hierSepModel->foldColumn()) {
      hierSepModel->setFoldColumn(column); changed = true;
    }

    if (changed)
      modelData_->emitModelChanged();
  }
}
