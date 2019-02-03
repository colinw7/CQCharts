#include <CQChartsModelControl.h>
#include <CQChartsModelData.h>
#include <CQChartsColumnType.h>
#include <CQChartsExprModel.h>
#include <CQChartsModelUtil.h>
#include <CQCharts.h>
#include <CQBaseModel.h>
#include <CQUtil.h>

#include <QVBoxLayout>
#include <QTabWidget>
#include <QStackedWidget>
#include <QRadioButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>

class CQChartsParamEdit : public QFrame {
 public:
  CQChartsParamEdit(QWidget *parent=nullptr) :
   QFrame(parent) {
    layout_ = new QHBoxLayout(this);
    layout_->setMargin(0); layout_->setSpacing(0);
  }

  void setString(const QString &str="") {
    reset();

    edit_ = new QLineEdit;

    edit_->setText(str);

    layout_->addWidget(edit_);
  }

  void setBool(bool b=false) {
    reset();

    check_ = new QCheckBox;

    check_->setChecked(b);

    layout_->addWidget(check_);
  }

  QString getString() const {
    assert(edit_);

    return edit_->text().simplified();
  }

  bool getBool() const {
    assert(check_);

    return check_->isChecked();
  }

  void reset() {
    delete edit_;
    delete check_;

    edit_  = nullptr;
    check_ = nullptr;
  }

 private:
  QHBoxLayout *layout_ { nullptr };
  QLineEdit   *edit_   { nullptr };
  QCheckBox   *check_  { nullptr };
};

//---

CQChartsModelControl::
CQChartsModelControl(CQCharts *charts) :
 charts_(charts)
{
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  //---

  QTabWidget *controlTab = CQUtil::makeWidget<QTabWidget>("controlTab");

  controlTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  layout->addWidget(controlTab);

  //------

  QFrame *exprFrame = CQUtil::makeWidget<QFrame>("exprFrame");

  controlTab->addTab(exprFrame, "Expression");

  QVBoxLayout *exprFrameLayout = new QVBoxLayout(exprFrame);
  exprFrameLayout->setMargin(0); exprFrameLayout->setSpacing(2);

  //--

  QFrame *exprModeFrame = CQUtil::makeWidget<QFrame>("exprMode");

  QHBoxLayout *exprModeLayout = new QHBoxLayout(exprModeFrame);
  exprModeLayout->setMargin(0); exprModeLayout->setSpacing(2);

  exprFrameLayout->addWidget(exprModeFrame);

  exprAddRadio_    = CQUtil::makeWidget<QRadioButton>("add"   );
  exprRemoveRadio_ = CQUtil::makeWidget<QRadioButton>("remove");
  exprModifyRadio_ = CQUtil::makeWidget<QRadioButton>("modify");

  exprAddRadio_   ->setText("Add");
  exprRemoveRadio_->setText("Remove");
  exprModifyRadio_->setText("Modify");

  exprAddRadio_->setChecked(true);

  exprModeLayout->addWidget(exprAddRadio_);
  exprModeLayout->addWidget(exprRemoveRadio_);
  exprModeLayout->addWidget(exprModifyRadio_);
  exprModeLayout->addStretch(1);

  connect(exprAddRadio_   , SIGNAL(toggled(bool)), this, SLOT(expressionModeSlot()));
  connect(exprRemoveRadio_, SIGNAL(toggled(bool)), this, SLOT(expressionModeSlot()));
  connect(exprModifyRadio_, SIGNAL(toggled(bool)), this, SLOT(expressionModeSlot()));

  //--

  QGridLayout *exprGridLayout = new QGridLayout;
  exprGridLayout->setMargin(0); exprGridLayout->setSpacing(2);

  exprFrameLayout->addLayout(exprGridLayout);

  int row = 0;

  //---

  exprValueLabel_ = new QLabel("Expression");
  exprValueLabel_->setObjectName("exprValueLabel");

  exprValueEdit_ = CQUtil::makeWidget<QLineEdit>("exprValueEdit");

  exprValueEdit_->setToolTip("+<expr> OR -<column> OR =<column>:<expr>\n"
                             "Use: @<number> as shorthand for column(<number>)\n"
                             "Functions: column, row, cell, setColumn, setRow, setCell\n"
                             " header, setHeader, type, setType, map, bucket, norm, key, rand");

  exprGridLayout->addWidget(exprValueLabel_, row, 0);
  exprGridLayout->addWidget(exprValueEdit_ , row, 1);

  ++row;

  //----

  exprColumnLabel_ = new QLabel("Column");
  exprColumnLabel_->setObjectName("exprEditLabel");

  exprColumnEdit_ = CQUtil::makeWidget<QLineEdit>("exprColumnEdit");

  exprColumnEdit_->setToolTip("Column to Modify");

  exprGridLayout->addWidget(exprColumnLabel_, row, 0);
  exprGridLayout->addWidget(exprColumnEdit_ , row, 1);

  ++row;

  //----

  exprNameLabel_ = new QLabel("Name");
  exprNameLabel_->setObjectName("exprNameLabel");

  exprNameEdit_ = CQUtil::makeWidget<QLineEdit>("exprNameEdit");

  exprNameEdit_->setToolTip("Column Name");

  exprGridLayout->addWidget(exprNameLabel_, row, 0);
  exprGridLayout->addWidget(exprNameEdit_ , row, 1);

  ++row;

  //--

  exprTypeLabel_ = new QLabel("Type");
  exprTypeLabel_->setObjectName("exprTypeLabel");

  exprTypeEdit_ = CQUtil::makeWidget<QLineEdit>("exprTypeEdit");

  exprTypeEdit_->setToolTip("Column Type");

  exprGridLayout->addWidget(exprTypeLabel_, row, 0);
  exprGridLayout->addWidget(exprTypeEdit_ , row, 1);

  ++row;

  //--

  exprFrameLayout->addStretch(1);

  //--

  QHBoxLayout *exprButtonLayout = new QHBoxLayout;
  exprButtonLayout->setMargin(0); exprButtonLayout->setSpacing(2);

  exprFrameLayout->addLayout(exprButtonLayout);

  QPushButton *exprApplyButton = CQUtil::makeWidget<QPushButton>("exprApply");

  exprApplyButton->setText("Apply");

  connect(exprApplyButton, SIGNAL(clicked()), this, SLOT(exprApplySlot()));

  exprButtonLayout->addStretch(1);
  exprButtonLayout->addWidget(exprApplyButton);

  //------

#ifdef CQCHARTS_FOLDED_MODEL
  QFrame *foldFrame = CQUtil::makeWidget<QFrame>("foldFrame");

  controlTab->addTab(foldFrame, "Fold");

  QVBoxLayout *foldFrameLayout = new QVBoxLayout(foldFrame);
  foldFrameLayout->setMargin(0); foldFrameLayout->setSpacing(2);

  //---

  QGridLayout *foldWidgetsLayout = new QGridLayout;
  foldWidgetsLayout->setMargin(0); foldWidgetsLayout->setSpacing(2);

  foldFrameLayout->addLayout(foldWidgetsLayout);

  int foldRow = 0;

  foldColumnEdit_ = addLineEdit(foldWidgetsLayout, foldRow, "Column", "column");

  foldAutoCheck_ = CQUtil::makeWidget<QCheckBox>("foldAuto");

  foldAutoCheck_->setText("Auto");
  foldAutoCheck_->setChecked(true);

  foldWidgetsLayout->addWidget(foldAutoCheck_, foldRow, 0, 1, 1); ++foldRow;

  foldDeltaEdit_ = addLineEdit(foldWidgetsLayout, foldRow, "Delta", "delta");

  foldDeltaEdit_->setText("1.0");

  foldCountEdit_ = addLineEdit(foldWidgetsLayout, foldRow, "Count", "count");

  foldCountEdit_->setText("20");

  foldWidgetsLayout->setRowStretch(foldRow, 1); ++foldRow;

  //--

  QHBoxLayout *foldButtonLayout = new QHBoxLayout;
  foldButtonLayout->setMargin(0); foldButtonLayout->setSpacing(2);

  foldFrameLayout->addLayout(foldButtonLayout);

  QPushButton *foldApplyButton = CQUtil::makeWidget<QPushButton>("foldApply");

  foldApplyButton->setText("Apply");

  connect(foldApplyButton, SIGNAL(clicked()), this, SLOT(foldApplySlot()));

  QPushButton *foldClearButton = CQUtil::makeWidget<QPushButton>("foldClear");

  foldClearButton->setText("Clear");

  connect(foldClearButton, SIGNAL(clicked()), this, SLOT(foldClearSlot()));

  foldButtonLayout->addStretch(1);
  foldButtonLayout->addWidget(foldApplyButton);
  foldButtonLayout->addWidget(foldClearButton);
#endif

  //------

  QFrame *columnDataFrame = CQUtil::makeWidget<QFrame>("columnDataFrame");

  controlTab->addTab(columnDataFrame, "Column Data");

  QVBoxLayout *columnDataLayout = new QVBoxLayout(columnDataFrame);
  columnDataLayout->setMargin(0); columnDataLayout->setSpacing(2);

  //---

  columnEditData_.editFrame = CQUtil::makeWidget<QFrame>("columnEditFrame");

  columnEditData_.editLayout = new QGridLayout(columnEditData_.editFrame);
  columnEditData_.editLayout->setMargin(0); columnEditData_.editLayout->setSpacing(2);

  columnDataLayout->addWidget(columnEditData_.editFrame);

  columnEditData_.row = 0;

  columnEditData_.numEdit  =
    addLineEdit(columnEditData_.editLayout, columnEditData_.row, "Number", "number");
  columnEditData_.nameEdit =
    addLineEdit(columnEditData_.editLayout, columnEditData_.row, "Name"  , "name"  );
  columnEditData_.typeCombo =
    addComboBox(columnEditData_.editLayout, columnEditData_.row, "Type"  , "type"  );

  QStringList typeNames;

  CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

  columnTypeMgr->typeNames(typeNames);

  columnEditData_.typeCombo->addItems(typeNames);

  columnEditData_.numEdit ->setToolTip("Column Number");
  columnEditData_.nameEdit->setToolTip("Column Name");
  columnEditData_.typeCombo->setToolTip("Column Type");

  columnEditData_.editLayout->setRowStretch(columnEditData_.row, 1);

  connect(columnEditData_.typeCombo, SIGNAL(currentIndexChanged(int)),
          this, SLOT(typeChangedSlot()));

  //---

  QHBoxLayout *columnButtonLayout = new QHBoxLayout;
  columnButtonLayout->setMargin(0); columnButtonLayout->setSpacing(2);

  columnDataLayout->addLayout(columnButtonLayout);

  QPushButton *typeApplyButton = CQUtil::makeWidget<QPushButton>("typeApply");

  typeApplyButton->setText("Apply");

  connect(typeApplyButton, SIGNAL(clicked()), this, SLOT(typeApplySlot()));

  columnButtonLayout->addStretch(1);
  columnButtonLayout->addWidget(typeApplyButton);

  //---

  expressionModeSlot();

  //---

  connect(charts, SIGNAL(currentModelChanged(int)), this, SLOT(updateCurrentModel()));
}

QLineEdit *
CQChartsModelControl::
addLineEdit(QGridLayout *grid, int &row, const QString &name, const QString &objName) const
{
  QLabel    *label = CQUtil::makeWidget<QLabel   >(objName + "Label");
  QLineEdit *edit  = CQUtil::makeWidget<QLineEdit>(objName + "Edit" );

  label->setText(name);

  grid->addWidget(label, row, 0);
  grid->addWidget(edit , row, 1);

  ++row;

  return edit;
}

QComboBox *
CQChartsModelControl::
addComboBox(QGridLayout *grid, int &row, const QString &name, const QString &objName) const
{
  QLabel    *label = CQUtil::makeWidget<QLabel   >(objName + "Label");
  QComboBox *combo  = CQUtil::makeWidget<QComboBox>(objName + "Combo");

  label->setText(name);

  grid->addWidget(label, row, 0);
  grid->addWidget(combo, row, 1);

  ++row;

  return combo;
}

void
CQChartsModelControl::
expressionModeSlot()
{
  exprMode_ = Mode::ADD;

  if      (exprAddRadio_   ->isChecked()) exprMode_ = Mode::ADD;
  else if (exprRemoveRadio_->isChecked()) exprMode_ = Mode::REMOVE;
  else if (exprModifyRadio_->isChecked()) exprMode_ = Mode::MODIFY;

  exprColumnLabel_->setEnabled(exprMode_ == Mode::MODIFY);
  exprColumnEdit_ ->setEnabled(exprMode_ == Mode::MODIFY);

  exprTypeLabel_->setEnabled(exprMode_ != Mode::REMOVE);
  exprNameEdit_ ->setEnabled(exprMode_ != Mode::REMOVE);
  exprTypeEdit_ ->setEnabled(exprMode_ != Mode::REMOVE);
}

void
CQChartsModelControl::
exprApplySlot()
{
  QString expr = exprValueEdit_->text().simplified();

  if (! expr.length())
    return;

  //---

  if (! modelData_) {
    charts_->errorMsg("No model data");
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

  QString columnStr = exprColumnEdit_->text();

  CQChartsColumn column;

  if (! CQChartsModelUtil::stringToColumn(model.data(), columnStr, column)) {
    bool ok;

    int icolumn = columnStr.toInt(&ok);

    if (ok)
      column = CQChartsColumn(icolumn);
  }

  int column1 = CQChartsModelUtil::processExpression(model.data(), function, column, expr);

  if (function == CQChartsExprModel::Function::ADD ||
      function == CQChartsExprModel::Function::ASSIGN) {
    QString nameStr = exprNameEdit_->text();
    QString typeStr = exprTypeEdit_->text();

    if (column1 < 0) {
      charts_->errorMsg("Invalid column");
      return;
    }

    if (nameStr.length())
      model->setHeaderData(column1, Qt::Horizontal, nameStr, Qt::DisplayRole);

    if (typeStr.length()) {
      if (! CQChartsModelUtil::setColumnTypeStr(charts_, model.data(), column1, typeStr)) {
        charts_->errorMsg("Invalid type '" + typeStr + "'");
        return;
      }
    }
  }
}

void
CQChartsModelControl::
foldApplySlot()
{
#ifdef CQCHARTS_FOLDED_MODEL
  if (! modelData_)
    return;

  CQChartsModelData::FoldData foldData;

  foldData.columnsStr = foldColumnEdit_->text();
  foldData.isAuto     = foldAutoCheck_->isChecked();
  foldData.delta      = foldDeltaEdit_->text().toDouble();
  foldData.count      = foldCountEdit_->text().toInt();

  modelData_->foldModel(foldData);

  updateModel();

  updateModelDetails();
#endif
}

void
CQChartsModelControl::
foldClearSlot()
{
#ifdef CQCHARTS_FOLDED_MODEL
  if (! modelData_)
    return;

  modelData_->foldClear();

  updateModel();

  updateModelDetails();
#endif
}

void
CQChartsModelControl::
updateCurrentModel()
{
  CQChartsModelData *modelData = charts_->currentModelData();

  if (modelData != modelData_) {
    if (modelData_)
      disconnect(modelData_, SIGNAL(currentColumnChanged(int)), this, SLOT(setColumnData(int)));

    modelData_ = modelData;

    if (modelData_)
      setColumnData(modelData_->currentColumn());

    if (modelData_)
      connect(modelData_, SIGNAL(currentColumnChanged(int)), this, SLOT(setColumnData(int)));
  }
}

void
CQChartsModelControl::
updateModel()
{
}

void
CQChartsModelControl::
updateModelDetails()
{
}

void
CQChartsModelControl::
typeChangedSlot()
{
}

void
CQChartsModelControl::
typeApplySlot()
{
  if (! modelData_)
    return;

  ModelP model = modelData_->currentModel();

  //---

  QString numStr = columnEditData_.numEdit->text();

  bool ok;

  int column = numStr.toInt(&ok);

  if (! ok) {
    charts_->errorMsg("Invalid column number '" + numStr + "'");
    return;
  }

  //--

  QString nameStr = columnEditData_.nameEdit->text();

  if (nameStr.length())
    model->setHeaderData(column, Qt::Horizontal, nameStr, Qt::DisplayRole);

  //---

  QString typeStr = columnEditData_.typeCombo->currentText();

#if 0
  if (! CQChartsUtil::setColumnTypeStr(charts_, model.data(), column, typeStr)) {
    charts_->errorMsg("Invalid type '" + typeStr + "'");
    return;
  }
#endif

  CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

  CQBaseModelType columnType = CQBaseModel::nameType(typeStr);

  const CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);

  if (typeData) {
    CQChartsNameValues nameValues;

    for (const auto &paramEdit : columnEditData_.paramEdits) {
      QString name = paramEdit.label->text();

      const CQChartsColumnTypeParam *param = typeData->getParam(name);
      if (! param) continue;

      QString value;

      if (param->type() == CQBaseModelType::BOOLEAN) {
        bool b = paramEdit.edit->getBool();

        value = (b ? "1" : "0");
      }
      else
        value = paramEdit.edit->getString();

      if (value != "")
        nameValues.setNameValue(name, value);
    }

    columnTypeMgr->setModelColumnType(model.data(), column, columnType, nameValues);
  }

  setColumnData(column);
}

void
CQChartsModelControl::
setColumnData(int column)
{
  columnEditData_.numEdit->setText(QString("%1").arg(column));

  //---

  if (! modelData_)
    return;

  ModelP model = modelData_->currentModel();

  //---

  QString headerStr = model->headerData(column, Qt::Horizontal).toString();

  columnEditData_.nameEdit->setText(headerStr);

  //---

  CQBaseModelType    columnType;
  CQBaseModelType    columnBaseType;
  CQChartsNameValues nameValues;

  if (CQChartsModelUtil::columnValueType(charts_, model.data(), column, columnType,
                                         columnBaseType, nameValues)) {
    CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

    const CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);

    //QString typeStr = columnTypeMgr->encodeTypeData(columnType, nameValues);

    int typeInd = columnEditData_.typeCombo->findText(typeData->name());

    if (typeInd >= 0)
      columnEditData_.typeCombo->setCurrentIndex(typeInd);

    int paramInd = 0;

    columnEditData_.editLayout->setRowStretch(columnEditData_.row, 0);

    for (const auto &param : typeData->params()) {
      while (paramInd >= int(columnEditData_.paramEdits.size())) {
        ParamEdit paramEdit;

        paramEdit.row   = columnEditData_.row + paramInd;
        paramEdit.label = new QLabel;
        paramEdit.edit  = new CQChartsParamEdit;

        columnEditData_.editLayout->addWidget(paramEdit.label, paramEdit.row, 0);
        columnEditData_.editLayout->addWidget(paramEdit.edit , paramEdit.row, 1);

        columnEditData_.paramEdits.push_back(paramEdit);
      }

      ParamEdit &paramEdit = columnEditData_.paramEdits[paramInd];

      paramEdit.label->setText(param.name());

      QVariant var;

      nameValues.nameValue(param.name(), var);

      if (param.type() == CQBaseModelType::BOOLEAN)
        paramEdit.edit->setBool(var.toBool());
      else
        paramEdit.edit->setString(var.toString());

      paramEdit.label->setObjectName(param.name() + "_label");
      paramEdit.edit ->setObjectName(param.name() + "_edit" );
      paramEdit.edit ->setToolTip(param.tip());

      ++paramInd;
    }

    while (paramInd < int(columnEditData_.paramEdits.size())) {
      ParamEdit &paramEdit1 = columnEditData_.paramEdits.back();

      CQUtil::removeGridRow(columnEditData_.editLayout, paramEdit1.row, /*delete*/false);

      delete paramEdit1.label;
      delete paramEdit1.edit;

      columnEditData_.paramEdits.pop_back();
    }

    columnEditData_.editLayout->setRowStretch(columnEditData_.row + paramInd, 1);

    columnEditData_.editLayout->invalidate();
  }
  else {
    columnEditData_.typeCombo->setCurrentIndex(-1);
  }
}
