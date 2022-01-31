#include <CQChartsModelFilterControl.h>

#include <CQChartsFilterModel.h>
#include <CQChartsModelData.h>
#include <CQChartsLineEdit.h>
//#include <CQChartsColumnCombo.h>
#include <CQCharts.h>
#include <CQUtil.h>

#include <CQCheckBox.h>

#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

CQChartsModelFilterControl::
CQChartsModelFilterControl(QWidget *parent) :
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
  filterEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("filterEdit");

  (void) addWidgetEdit("Filter", "filter", filterEdit_);

  filterEdit_->setToolTip("Filter Expression");

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
CQChartsModelFilterControl::
setModelData(CQChartsModelData *modelData)
{
  modelData_ = modelData;
}

void
CQChartsModelFilterControl::
setFilterText(const QString &text)
{
  filterEdit_->setText(text);
}

void
CQChartsModelFilterControl::
applySlot()
{
  if (! modelData_)
    return;

  auto *charts = modelData_->charts();

  //---

  CQChartsModelData::CopyData copyData;

  copyData.filter = filterEdit_->text();

  auto *filterModel = modelData_->copy(copyData);

  //---

  ModelP dataModelP(filterModel);

  (void) charts->initModelData(dataModelP);
}
