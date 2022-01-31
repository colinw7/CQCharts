#include <CQChartsModelFlattenControl.h>

#include <CQChartsFilterModel.h>
#include <CQChartsModelData.h>
#include <CQChartsLineEdit.h>
#include <CQChartsModelUtil.h>
//#include <CQChartsColumnCombo.h>
#include <CQCharts.h>
#include <CQUtil.h>

#include <CQCheckBox.h>

#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

CQChartsModelFlattenControl::
CQChartsModelFlattenControl(QWidget *parent) :
 QFrame(parent)
{
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(0, 2);

  layout->addLayout(gridLayout);

  int row = 0;

  //---

  auto addWidgetEdit = [&](const QString &name, const QString &objName, QWidget *edit) {
    auto *label = CQUtil::makeLabelWidget<QLabel>("", objName + "Label");

    label->setText(name);

    gridLayout->addWidget(label, row, 0);
    gridLayout->addWidget(edit , row, 1);

    ++row;

    return edit;
  };

#if 0
  auto addLineEdit = [&](const QString &name, const QString &objName) {
    auto *edit = CQUtil::makeWidget<CQChartsLineEdit>(objName + "Edit" );

    (void) addWidgetEdit(name, objName, edit);

    return edit;
  };
#endif

  //---

  // group column
  //columnEdit_ = new CQChartsColumnCombo;
  columnEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("columnEdit");

  (void) addWidgetEdit("Column", "column", columnEdit_);

  columnEdit_->setToolTip("Group Column");

  //---

  opColumnsEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("sumEdit");

  (void) addWidgetEdit("Column Operations", "opColumns", opColumnsEdit_);

  opColumnsEdit_->setToolTip("List of operations on columns of the form <op>:<column>\n"
    "Operations: sum, mean, count, max, median, min, mode, range");

  //---

  gridLayout->setRowStretch(row, 1); ++row;

  //--

  auto *buttonLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  layout->addLayout(buttonLayout);

  auto *applyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "apply");

  connect(applyButton, SIGNAL(clicked()), this, SLOT(applySlot()));

  buttonLayout->addStretch(1);
  buttonLayout->addWidget(applyButton);
}

void
CQChartsModelFlattenControl::
setModelData(CQChartsModelData *modelData)
{
  modelData_ = modelData;

//columnEdit_->setModelData(modelData_);
}

void
CQChartsModelFlattenControl::
applySlot()
{
  if (! modelData_)
    return;

  auto *charts = modelData_->charts();
  auto *model  = modelData_->model().data();

  int nc = model->columnCount();

  //---

  CQChartsModelUtil::FlattenData flattenData;

  bool ok;
  long c = CQChartsUtil::toInt(columnEdit_->text(), ok);

  if (c >= 0 && c < nc)
    flattenData.groupColumn = CQChartsColumn(c);

  //---

  auto opStrs = opColumnsEdit_->text().split(" ", QString::SkipEmptyParts);

  for (const auto &s : opStrs) {
    auto parts = s.split(":", QString::SkipEmptyParts);

    if (parts.size() != 2)
      continue;

    bool ok1;

    auto opStr = parts[0].toLower();
    long c1    = CQChartsUtil::toInt(parts[1], ok1);

    auto flattenOp = CQChartsModelUtil::flattenStringToOp(opStr);

    if (flattenOp == CQChartsModelUtil::FlattenOp::NONE)
      continue;

    if (c1 >= 0 && c1 < nc)
      flattenData.columnOps.emplace_back(CQChartsColumn(c1), flattenOp);
  }

  //---

  auto *filterModel = CQChartsModelUtil::flattenModel(charts, model, flattenData);

  ModelP dataModelP(filterModel);

  (void) charts->initModelData(dataModelP);
}
