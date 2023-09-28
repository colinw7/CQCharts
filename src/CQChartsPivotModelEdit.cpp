#include <CQChartsPivotModelEdit.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsColumnsEdit.h>
#include <CQCharts.h>

#include <CQPivotModel.h>
#include <CQIntegerSpin.h>
#include <CQUtil.h>

#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>

CQChartsPivotModelEdit::
CQChartsPivotModelEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("pivotModelEdit");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  enabledCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Enabled", "enabledCheck");

  enabledCheck_->setToolTip("Enable summary of model data");

  layout->addWidget(enabledCheck_);

  //---

  auto addColumnsEdit = [&](const QString &labelStr, const QString &name) {
    auto *hframe  = CQUtil::makeWidget<QFrame>(name + "_frame");
    auto *hlayout = CQUtil::makeLayout<QHBoxLayout>(hframe, 2, 2);

    auto *label = CQUtil::makeLabelWidget<QLabel>(labelStr, name + "_label");
    auto *edit  = CQUtil::makeWidget<CQChartsColumnsEdit>(name + "_edit");

    edit->setBasic(true);

    hlayout->addWidget(label);
    hlayout->addWidget(edit);

    layout->addWidget(hframe);

    return edit;
  };

  hColumnsEdit_ = addColumnsEdit("H Columns", "hcolumnsEdit");
  vColumnsEdit_ = addColumnsEdit("V Columns", "vcolumnsEdit");
  dColumnsEdit_ = addColumnsEdit("D Columns", "dcolumnsEdit");

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

  valueTypesEdit_ = CQUtil::makeWidget<QLineEdit>("valueTypesEdit");
  includeTotals_  = CQUtil::makeWidget<QCheckBox>("includeTotals");
  fillValue_      = CQUtil::makeWidget<QLineEdit>("fillValue");
  separatorEdit_  = CQUtil::makeWidget<QLineEdit>("separator");

  addEditWidget("Value Types"   , valueTypesEdit_, "valueTypes");
  addEditWidget("Include Totals", includeTotals_ , "includeTotals");
  addEditWidget("Fill Value"    , fillValue_     , "fillValue");
  addEditWidget("Separator"     , separatorEdit_ , "separator");

  //---

  layout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQChartsPivotModelEdit::
setModelData(ModelData *modelData)
{
  modelData_ = modelData;

  //--

  connectSlots(false);

  auto updateColumnsEdit = [&](CQChartsColumnsEdit *edit) {
    edit->setProxy    (false);
    edit->setModelData(modelData_);
  };

  updateColumnsEdit(hColumnsEdit_);
  updateColumnsEdit(vColumnsEdit_);
  updateColumnsEdit(dColumnsEdit_);

  connectSlots(true);

  //--

  updateWidgetsFromModel();
}

//---

void
CQChartsPivotModelEdit::
connectSlots(bool b)
{
  CQUtil::optConnectDisconnect(b,
    modelData_, SIGNAL(dataChanged()), this, SLOT(updateWidgetsFromModel()));

  CQUtil::connectDisconnect(b,
    enabledCheck_, SIGNAL(stateChanged(int)), this, SLOT(enabledSlot()));
  CQUtil::connectDisconnect(b,
    hColumnsEdit_, SIGNAL(columnsChanged()), this, SLOT(updateModelFromWidgets()));
  CQUtil::connectDisconnect(b,
    vColumnsEdit_, SIGNAL(columnsChanged()), this, SLOT(updateModelFromWidgets()));
  CQUtil::connectDisconnect(b,
    dColumnsEdit_, SIGNAL(columnsChanged()), this, SLOT(updateModelFromWidgets()));
  CQUtil::connectDisconnect(b,
    valueTypesEdit_, SIGNAL(editingFinished()), this, SLOT(updateModelFromWidgets()));
  CQUtil::connectDisconnect(b,
    includeTotals_, SIGNAL(stateChanged(int)), this, SLOT(updateModelFromWidgets()));
  CQUtil::connectDisconnect(b,
    fillValue_, SIGNAL(editingFinished()), this, SLOT(updateModelFromWidgets()));
  CQUtil::connectDisconnect(b,
    separatorEdit_, SIGNAL(editingFinished()), this, SLOT(updateModelFromWidgets()));
}

//---

void
CQChartsPivotModelEdit::
enabledSlot()
{
  connectSlots(false);

  //---

  if (modelData_) {
    if (enabledCheck_->isChecked())
      modelData_->addPivotModel();

    modelData_->setPivotEnabled(enabledCheck_->isChecked());
  }

  connectSlots(true);

  //---

  updateWidgetsFromModel();
}

//---

// update widgets from state
void
CQChartsPivotModelEdit::
updateWidgetsFromModel()
{
  connectSlots(false);

  auto *pivotModel = (modelData_ ? modelData_->pivotModel() : nullptr);

  //---

  if (modelData_) {
    auto model = modelData_->currentModel(/*proxy*/false);
    assert(model);

    //---

    enabledCheck_->setChecked(modelData_->isPivotEnabled());

    //--

    auto setEditColumns = [&](CQChartsColumnsEdit *edit, const CQPivotModel::Columns &columns) {
      CQChartsColumns ccolumns;

      for (const auto &column : columns)
        ccolumns.addColumn(CQChartsColumn(column));

      edit->setColumns(ccolumns);
    };

    if (pivotModel) {
      setEditColumns(hColumnsEdit_, pivotModel->hColumns());
      setEditColumns(vColumnsEdit_, pivotModel->vColumns());
      setEditColumns(dColumnsEdit_, pivotModel->valueColumns());

      //const auto &valueTypes = pivotModel->valueTypes();
      //valueTypesEdit_->setText(valueTypes);

      includeTotals_->setChecked(pivotModel->isIncludeTotals());

      fillValue_    ->setText(pivotModel->fillValue().toString());
      separatorEdit_->setText(QString(pivotModel->separator()));
    }
  }

  //--

  hColumnsEdit_->setEnabled(pivotModel);
  vColumnsEdit_->setEnabled(pivotModel);
  dColumnsEdit_->setEnabled(pivotModel);

  valueTypesEdit_->setEnabled(pivotModel);
  includeTotals_ ->setEnabled(pivotModel);
  fillValue_     ->setEnabled(pivotModel);
  separatorEdit_ ->setEnabled(pivotModel);

  //--

  connectSlots(true);
}

// update state from widgets
void
CQChartsPivotModelEdit::
updateModelFromWidgets()
{
  auto *pivotModel = (modelData_ ? modelData_->pivotModel() : nullptr);

  if (modelData_->isPivotEnabled() && pivotModel) {
    auto model = modelData_->currentModel(/*proxy*/false);
    assert(model);

    //--

    bool changed = false;

    //--

    auto getEditColumns = [&](CQChartsColumnsEdit *edit) {
      const auto &ccolumns = edit->columns();

      CQPivotModel::Columns columns;

      for (const auto &ccolumn : ccolumns)
        columns.push_back(ccolumn.column());

      return columns;
    };

    if (pivotModel) {
      pivotModel->setHColumns    (getEditColumns(hColumnsEdit_));
      pivotModel->setVColumns    (getEditColumns(vColumnsEdit_));
      pivotModel->setValueColumns(getEditColumns(dColumnsEdit_));

      changed = true;
    }

  //if (valueTypesEdit_->text() != pivotModel->valueTypes()) {
  //  pivotModel->setValueTypes(valueTypesEdit_->text()); changed = true;
  //}

    if (includeTotals_->isChecked() != pivotModel->isIncludeTotals()) {
      pivotModel->setIncludeTotals(includeTotals_->isChecked()); changed = true;
    }

    if (fillValue_->text() != pivotModel->fillValue().toString()) {
      pivotModel->setFillValue(fillValue_->text()); changed = true;
    }

    if (separatorEdit_->text().at(0) != pivotModel->separator()) {
      pivotModel->setSeparator(separatorEdit_->text().at(0)); changed = true;
    }

    //--

    if (changed)
      modelData_->emitModelChanged();
  }
}
