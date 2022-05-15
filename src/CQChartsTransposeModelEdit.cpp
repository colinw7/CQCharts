#include <CQChartsTransposeModelEdit.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsColumnsEdit.h>
#include <CQChartsWidgetUtil.h>
#include <CQCharts.h>

#include <CQTransposeModel.h>
#include <CQIntegerSpin.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QStackedWidget>
#include <QCheckBox>
#include <QLabel>

CQChartsTransposeModelEdit::
CQChartsTransposeModelEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("transposeModelEdit");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  enabledCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Enabled", "enabledCheck");

  enabledCheck_->setToolTip("Enable summary of model data");

  layout->addWidget(enabledCheck_);

  //---

  layout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQChartsTransposeModelEdit::
setModelData(ModelData *modelData)
{
  modelData_ = modelData;

  updateWidgetsFromModel();
}

//---

void
CQChartsTransposeModelEdit::
connectSlots(bool b)
{
  CQChartsWidgetUtil::connectDisconnect(b,
    enabledCheck_, SIGNAL(stateChanged(int)), this, SLOT(enabledSlot()));
}

//---

void
CQChartsTransposeModelEdit::
enabledSlot()
{
  if (modelData_) {
    if (enabledCheck_->isChecked())
      modelData_->addTransposeModel();

    modelData_->setTransposeEnabled(enabledCheck_->isChecked());
  }

  updateModelFromWidgets();
}

//---

// update widgets from state
void
CQChartsTransposeModelEdit::
updateWidgetsFromModel()
{
  connectSlots(false);

//auto *transposeModel = (modelData_ ? modelData_->transposeModel() : nullptr);

  if (modelData_) {
    auto model = modelData_->currentModel(/*proxy*/false);
    assert(model);

    //int nr = model.data()->rowCount();
    //int nc = model.data()->columnCount();

    //---

    enabledCheck_->setChecked(modelData_->isTransposeEnabled());
  }

  //--

  connectSlots(true);
}

// update state from widgets
void
CQChartsTransposeModelEdit::
updateModelFromWidgets()
{
  auto *transposeModel = (modelData_ ? modelData_->transposeModel() : nullptr);

  if (modelData_->isTransposeEnabled() && transposeModel) {
    auto model = modelData_->currentModel(/*proxy*/false);
    assert(model);

    //--

    bool changed = false;

    //--

    if (changed)
      modelData_->emitModelChanged();
  }
}
