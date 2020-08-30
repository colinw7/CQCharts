#include <CQChartsModelFoldControl.h>

#ifdef CQCHARTS_FOLDED_MODEL

#include <CQChartsModelData.h>
#include <CQChartsLineEdit.h>
#include <CQChartsModelUtil.h>
#include <CQCharts.h>
#include <CQUtil.h>

#include <CQCheckBox.h>

#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

CQChartsModelFoldControl::
CQChartsModelFoldControl(QWidget *parent) :
 QFrame(parent)
{
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  auto *gridLayout = CQUtil::makeLayout<QGridLayout>(0, 2);

  layout->addLayout(gridLayout);

  int row = 0;

  //---

  auto addLineEdit = [&](const QString &name, const QString &objName) {
    auto *label = CQUtil::makeLabelWidget<QLabel>("", objName + "Label");
    auto *edit  = CQUtil::makeWidget<CQChartsLineEdit>(objName + "Edit" );

    label->setText(name);

    gridLayout->addWidget(label, row, 0);
    gridLayout->addWidget(edit , row, 1);

    ++row;

    return edit;
  };

  //---

  // fold column
  columnEdit_ = addLineEdit("Column", "column");

  columnEdit_->setToolTip("Fold Column");

  //---

  // fold type
  auto *foldTypeLabel = CQUtil::makeLabelWidget<QLabel>("Type", "type");

  typeCombo_ = CQUtil::makeWidget<QComboBox>("foldType");

  typeCombo_->addItems(QStringList() << "Bucketed" << "Hier Separator");

  gridLayout->addWidget(foldTypeLabel, row, 0);
  gridLayout->addWidget(typeCombo_   , row, 1); ++row;

  typeCombo_->setToolTip("Fold using bucketed values or hierarchical separator");

  //---

  // fold auto check
  auto *foldAutoLabel = CQUtil::makeLabelWidget<QLabel>("Auto", "auto");

  autoCheck_ = CQUtil::makeWidget<CQCheckBox>("foldAuto");

  autoCheck_->setText("Auto");
  autoCheck_->setChecked(true);

  gridLayout->addWidget(foldAutoLabel, row, 0);
  gridLayout->addWidget(autoCheck_   , row, 1); ++row;

  autoCheck_->setToolTip("Automatically determine fold buckets");

  //---

  // fold delta
  deltaEdit_ = addLineEdit("Delta", "delta");

  deltaEdit_->setText("1.0");
  deltaEdit_->setToolTip("Explicit fold delta");

  //---

  // fold count
  countEdit_ = addLineEdit("Count", "count");

  countEdit_->setText("20");
  countEdit_->setToolTip("Explicit fold count");

  //---

  // fold separator
  separatorEdit_ = addLineEdit("Separator", "separator");

  separatorEdit_->setText("/");
  separatorEdit_->setToolTip("Hierarchical separator");

  //---

  gridLayout->setRowStretch(row, 1); ++row;

  //--

  auto *foldButtonLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  layout->addLayout(foldButtonLayout);

  auto *foldApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "foldApply");

  connect(foldApplyButton, SIGNAL(clicked()), this, SLOT(applySlot()));

  auto *foldClearButton = CQUtil::makeLabelWidget<QPushButton>("Clear", "foldClear");

  connect(foldClearButton, SIGNAL(clicked()), this, SLOT(clearSlot()));

  foldButtonLayout->addStretch(1);
  foldButtonLayout->addWidget(foldApplyButton);
  foldButtonLayout->addWidget(foldClearButton);
}

void
CQChartsModelFoldControl::
setModelData(CQChartsModelData *modelData)
{
  modelData_ = modelData;
}

void
CQChartsModelFoldControl::
applySlot()
{
  if (! modelData_)
    return;

  bool ok;

  CQChartsModelData::FoldData foldData;

  foldData.columnsStr = columnEdit_->text();

  if      (typeCombo_->currentIndex() == 0) {
    foldData.foldType = CQChartsModelData::FoldData::FoldType::BUCKET;
    foldData.isAuto   = autoCheck_->isChecked();
    foldData.delta    = CQChartsUtil::toReal(deltaEdit_->text(), ok);
    foldData.count    = CQChartsUtil::toInt (countEdit_->text(), ok);
  }
  else if (typeCombo_->currentIndex() == 1) {
    foldData.foldType  = CQChartsModelData::FoldData::FoldType::SEPARATOR;
    foldData.separator = separatorEdit_->text();
  }

  modelData_->foldModel(foldData);
}

void
CQChartsModelFoldControl::
clearSlot()
{
  if (! modelData_)
    return;

  modelData_->foldClear();
}

#endif
