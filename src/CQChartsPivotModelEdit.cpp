#include <CQChartsPivotModelEdit.h>
#include <CQChartsModelData.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsColumnsEdit.h>
#include <CQChartsWidgetUtil.h>
#include <CQCharts.h>

#include <CQPivotModel.h>
#include <CQIntegerSpin.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QStackedWidget>
#include <QCheckBox>
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

  layout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQChartsPivotModelEdit::
setModelData(ModelData *modelData)
{
  modelData_ = modelData;

  auto updateColumnsEdit = [&](CQChartsColumnsEdit *edit) {
    edit->setProxy    (false);
    edit->setModelData(modelData_);
  };

  updateColumnsEdit(hColumnsEdit_);
  updateColumnsEdit(vColumnsEdit_);
  updateColumnsEdit(dColumnsEdit_);

  updateWidgetsFromModel();
}

//---

void
CQChartsPivotModelEdit::
connectSlots(bool b)
{
  CQChartsWidgetUtil::connectDisconnect(b,
    enabledCheck_, SIGNAL(stateChanged(int)), this, SLOT(enabledSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    hColumnsEdit_, SIGNAL(columnsChanged()), this, SLOT(updateModelFromWidgets()));
  CQChartsWidgetUtil::connectDisconnect(b,
    vColumnsEdit_, SIGNAL(columnsChanged()), this, SLOT(updateModelFromWidgets()));
  CQChartsWidgetUtil::connectDisconnect(b,
    dColumnsEdit_, SIGNAL(columnsChanged()), this, SLOT(updateModelFromWidgets()));
}

//---

void
CQChartsPivotModelEdit::
enabledSlot()
{
  if (modelData_) {
    if (enabledCheck_->isChecked())
      modelData_->addPivotModel();

    modelData_->setPivotEnabled(enabledCheck_->isChecked());
  }

  updateModelFromWidgets();
}

//---

// update widgets from state
void
CQChartsPivotModelEdit::
updateWidgetsFromModel()
{
  connectSlots(false);

  auto *pivotModel = (modelData_ ? modelData_->pivotModel() : nullptr);

  if (modelData_) {
    auto model = modelData_->currentModel(/*proxy*/false);
    assert(model);

    //int nr = model.data()->rowCount();
    //int nc = model.data()->columnCount();

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
    }
  }

  //--

  hColumnsEdit_->setEnabled(pivotModel);
  vColumnsEdit_->setEnabled(pivotModel);
  dColumnsEdit_->setEnabled(pivotModel);

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
    }

    //--

    if (changed)
      modelData_->emitModelChanged();
  }
}
