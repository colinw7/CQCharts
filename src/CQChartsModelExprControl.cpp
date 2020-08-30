#include <CQChartsModelExprControl.h>

#include <CQChartsExprModel.h>
#include <CQChartsModelData.h>
#include <CQChartsLineEdit.h>
#include <CQChartsModelUtil.h>
#include <CQCharts.h>
#include <CQUtil.h>

#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

CQChartsModelExprControl::
CQChartsModelExprControl(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("exprControl");

  init();
}

void
CQChartsModelExprControl::
setModelData(CQChartsModelData *modelData)
{
  modelData_ = modelData;
}

void
CQChartsModelExprControl::
init()
{
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //--

  auto *exprModeFrame  = CQUtil::makeWidget<QFrame>("exprMode");
  auto *exprModeLayout = CQUtil::makeLayout<QHBoxLayout>(exprModeFrame, 0, 2);

  layout->addWidget(exprModeFrame);

  auto createRadio = [&](const QString &name, const QString &text,
                         const QString &tipText, bool checked=false) {
    auto *radio = CQUtil::makeWidget<QRadioButton>(name);

    radio->setText(text);
    radio->setToolTip(tipText);

    if (checked)
      radio->setChecked(true);

    connect(radio, SIGNAL(toggled(bool)), this, SLOT(modeSlot()));

    return radio;
  };

  addRadio_    = createRadio("add"   , "Add"   , "Add Column Expression"   , true);
  removeRadio_ = createRadio("remove", "Remove", "Remove Expression"       );
  modifyRadio_ = createRadio("modify", "Modify", "Modify Column Expression");

  exprModeLayout->addWidget(addRadio_   );
  exprModeLayout->addWidget(removeRadio_);
  exprModeLayout->addWidget(modifyRadio_);
  exprModeLayout->addStretch(1);

  //--

  auto *exprGridLayout = CQUtil::makeLayout<QGridLayout>(0, 2);

  layout->addLayout(exprGridLayout);

  int row = 0;

  //---

  auto *exprValueLabel = CQUtil::makeLabelWidget<QLabel>("Expression", "valueLabel");

  valueEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("valueEdit");

  valueEdit_->setToolTip("+<expr> OR -<column> OR =<column>:<expr>\n"
    "Use: @<number> as shorthand for column(<number>)\n"
    "Functions: column, row, cell, setColumn, setRow, setCell\n"
    " header, setHeader, type, setType, map, bucket, norm, key, rand");

  exprGridLayout->addWidget(exprValueLabel, row, 0);
  exprGridLayout->addWidget(valueEdit_    , row, 1);

  ++row;

  //----

  columnLabel_ = CQUtil::makeLabelWidget<QLabel>("Column", "columnLabel");
  columnEdit_  = CQUtil::makeWidget<CQChartsLineEdit>("columnEdit");

  columnEdit_->setToolTip("Column to Modify");

  exprGridLayout->addWidget(columnLabel_, row, 0);
  exprGridLayout->addWidget(columnEdit_ , row, 1);

  ++row;

  //----

  auto *exprNameLabel = CQUtil::makeLabelWidget<QLabel>("Name", "nameLabel");

  nameEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("nameEdit");

  nameEdit_->setToolTip("Column Name");

  exprGridLayout->addWidget(exprNameLabel, row, 0);
  exprGridLayout->addWidget(nameEdit_    , row, 1);

  ++row;

  //--

  typeLabel_ = CQUtil::makeLabelWidget<QLabel>("Type", "typeLabel");
  typeEdit_  = CQUtil::makeWidget<CQChartsLineEdit>("typeEdit");

  typeEdit_->setToolTip("Column Type");

  exprGridLayout->addWidget(typeLabel_, row, 0);
  exprGridLayout->addWidget(typeEdit_ , row, 1);

  ++row;

  //--

  layout->addStretch(1);

  //--

  auto *exprButtonLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  layout->addLayout(exprButtonLayout);

  auto *exprApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "exprApply");

  connect(exprApplyButton, SIGNAL(clicked()), this, SLOT(applySlot()));

  exprButtonLayout->addStretch(1);
  exprButtonLayout->addWidget(exprApplyButton);

  //---

  modeSlot();
}

void
CQChartsModelExprControl::
modeSlot()
{
  exprMode_ = Mode::ADD;

  if      (addRadio_   ->isChecked()) exprMode_ = Mode::ADD;
  else if (removeRadio_->isChecked()) exprMode_ = Mode::REMOVE;
  else if (modifyRadio_->isChecked()) exprMode_ = Mode::MODIFY;

  columnLabel_->setEnabled(exprMode_ == Mode::MODIFY);
  columnEdit_ ->setEnabled(exprMode_ == Mode::MODIFY);

  typeLabel_->setEnabled(exprMode_ != Mode::REMOVE);
  nameEdit_ ->setEnabled(exprMode_ != Mode::REMOVE);
  typeEdit_ ->setEnabled(exprMode_ != Mode::REMOVE);
}

void
CQChartsModelExprControl::
applySlot()
{
  QString expr = valueEdit_->text().simplified();

  if (! expr.length())
    return;

  //---

  auto *charts = modelData_->charts();

  if (! modelData_) {
    charts->errorMsg("No model data");
    return;
  }

  CQChartsExprModel::Function function { CQChartsExprModel::Function::EVAL };

  switch (exprMode_) {
    case Mode::ADD   : function = CQChartsExprModel::Function::ADD   ; break;
    case Mode::REMOVE: function = CQChartsExprModel::Function::DELETE; break;
    case Mode::MODIFY: function = CQChartsExprModel::Function::ASSIGN; break;
    default:                                                           break;
  }

  ModelP model = modelData_->currentModel();

  QString columnStr = columnEdit_->text();

  CQChartsColumn column;

  if (! CQChartsModelUtil::stringToColumn(model.data(), columnStr, column)) {
    bool ok;

    long icolumn = CQChartsUtil::toInt(columnStr, ok);

    if (ok)
      column = CQChartsColumn(int(icolumn));
  }

  int column1 = CQChartsModelUtil::processExpression(model.data(), function, column, expr);

  if (function == CQChartsExprModel::Function::ADD ||
      function == CQChartsExprModel::Function::ASSIGN) {
    QString nameStr = nameEdit_->text();
    QString typeStr = typeEdit_->text();

    if (column1 < 0) {
      charts->errorMsg("Invalid column");
      return;
    }

    if (nameStr.length())
      model->setHeaderData(column1, Qt::Horizontal, nameStr, Qt::DisplayRole);

    if (typeStr.length()) {
      if (! CQChartsModelUtil::setColumnTypeStr(charts, model.data(),
                                                CQChartsColumn(column1), typeStr)) {
        charts->errorMsg("Invalid type '" + typeStr + "'");
        return;
      }
    }
  }
}
