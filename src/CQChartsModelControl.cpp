#include <CQChartsModelControl.h>
#include <CQChartsModelData.h>
#include <CQChartsColumnType.h>
#include <CQChartsExprModel.h>
#include <CQChartsModelFilter.h>
#include <CQChartsModelUtil.h>
#include <CQChartsColorEdit.h>
#include <CQChartsLineEdit.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewTree.h>

#include <CQHierSepModel.h>
#include <CQCsvModel.h>
#include <CQTsvModel.h>
#include <CQGnuDataModel.h>
#include <CQJsonModel.h>
#include <CQDataModel.h>

#include <CQIntegerSpin.h>
#include <CQCheckBox.h>
#include <CQUtil.h>

#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QStackedWidget>
#include <QRadioButton>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>

class CQChartsParamEdit : public QFrame {
 public:
  CQChartsParamEdit(QWidget *parent=nullptr) :
   QFrame(parent) {
    layout_ = CQUtil::makeLayout<QHBoxLayout>(this, 0, 0);
  }

  CQBaseModelType type() const { return type_; }

  void setString(const QString &str="") {
    if (! edit_) {
      reset();

      edit_ = CQUtil::makeWidget<CQChartsLineEdit>("edit");

      layout_->addWidget(edit_);
    }

    edit_->setText(str);

    type_ = CQBaseModelType::STRING;
  }

  void setBool(bool b=false) {
    if (! check_) {
      reset();

      check_ = CQUtil::makeWidget<CQCheckBox>("edit");

      layout_->addWidget(check_);
    }

    check_->setChecked(b);

    type_ = CQBaseModelType::BOOLEAN;
  }

  void setInteger(int i=0) {
    if (! ispin_) {
      reset();

      ispin_ = CQUtil::makeWidget<CQIntegerSpin>("edit");

      layout_->addWidget(ispin_);
    }

    ispin_->setValue(i);

    type_ = CQBaseModelType::INTEGER;
  }

  void setEnum(const QString &str, const QStringList &values) {
    if (! combo_) {
      reset();

      combo_ = CQUtil::makeWidget<QComboBox>("edit");

      layout_->addWidget(combo_);
    }

    // make optional
    QStringList values1;
    values1 << "";
    values1 << values;

    combo_->clear();
    combo_->addItems(values1);

    int pos = combo_->findText(str);
    if (pos < 0) pos = 0;

    combo_->setCurrentIndex(pos);

    type_ = CQBaseModelType::ENUM;
  }

  void setColor(const QString &str="") {
    if (! color_) {
      reset();

      color_ = CQUtil::makeWidget<CQChartsColorLineEdit>("edit");

      layout_->addWidget(color_);
    }

    CQChartsColor c(str);

    color_->setColor(c);

    type_ = CQBaseModelType::COLOR;
  }

  QString getString() const {
    assert(edit_);

    return edit_->text().simplified();
  }

  bool getBool() const {
    assert(check_);

    return check_->isChecked();
  }

  int getInteger() const {
    assert(ispin_);

    return ispin_->value();
  }

  QString getEnum() const {
    assert(combo_);

    return combo_->currentText();
  }

  QString getColor() const {
    assert(color_);

    return color_->color().toString();
  }

  void reset() {
    delete edit_;
    delete ispin_;
    delete check_;
    delete combo_;
    delete color_;

    edit_  = nullptr;
    ispin_ = nullptr;
    check_ = nullptr;
    combo_ = nullptr;
    color_ = nullptr;
  }

 private:
  CQBaseModelType        type_   { CQBaseModelType::NONE };
  QHBoxLayout*           layout_ { nullptr };
  CQChartsLineEdit*      edit_   { nullptr };
  CQIntegerSpin*         ispin_  { nullptr };
  CQCheckBox*            check_  { nullptr };
  QComboBox*             combo_  { nullptr };
  CQChartsColorLineEdit* color_  { nullptr };
};

//---

CQChartsModelControl::
CQChartsModelControl(CQCharts *charts, CQChartsModelData *modelData) :
 charts_(charts)
{
  setObjectName("control");

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 2);

  //---

  QTabWidget *controlTab = CQUtil::makeWidget<QTabWidget>("tab");

  controlTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layout->addWidget(controlTab);

  //---

  QFrame *exprFrame = addExprFrame();

  controlTab->addTab(exprFrame, "Expression");

#ifdef CQCHARTS_FOLDED_MODEL
  QFrame *foldFrame = addFoldFrame();

  controlTab->addTab(foldFrame, "Fold");
#endif

  QFrame *columnDataFrame = addColumnDataFrame();

  controlTab->addTab(columnDataFrame, "Column Data");

  QFrame *propertiesFrame = addPropertiesFrame();

  controlTab->addTab(propertiesFrame, "Properties");

  //---

  setModelData(modelData);

  expressionModeSlot();
}

CQChartsModelControl::
~CQChartsModelControl()
{
  propertyTree_->setPropertyModel(propertyModel_);

  delete propertyModel_;
}

QFrame *
CQChartsModelControl::
addExprFrame()
{
  QFrame *exprFrame = CQUtil::makeWidget<QFrame>("exprFrame");

  QVBoxLayout *exprFrameLayout = CQUtil::makeLayout<QVBoxLayout>(exprFrame, 0, 2);

  //--

  QFrame *exprModeFrame = CQUtil::makeWidget<QFrame>("exprMode");

  QHBoxLayout *exprModeLayout = CQUtil::makeLayout<QHBoxLayout>(exprModeFrame, 0, 2);

  exprFrameLayout->addWidget(exprModeFrame);

  exprWidgets_.addRadio    = CQUtil::makeWidget<QRadioButton>("add"   );
  exprWidgets_.removeRadio = CQUtil::makeWidget<QRadioButton>("remove");
  exprWidgets_.modifyRadio = CQUtil::makeWidget<QRadioButton>("modify");

  exprWidgets_.addRadio   ->setText("Add");
  exprWidgets_.removeRadio->setText("Remove");
  exprWidgets_.modifyRadio->setText("Modify");

  exprWidgets_.addRadio   ->setToolTip("Add Column Expression");
  exprWidgets_.removeRadio->setToolTip("Remove Expression");
  exprWidgets_.modifyRadio->setToolTip("Modify Column Expression");

  exprWidgets_.addRadio->setChecked(true);

  exprModeLayout->addWidget(exprWidgets_.addRadio   );
  exprModeLayout->addWidget(exprWidgets_.removeRadio);
  exprModeLayout->addWidget(exprWidgets_.modifyRadio);
  exprModeLayout->addStretch(1);

  connect(exprWidgets_.addRadio   , SIGNAL(toggled(bool)), this, SLOT(expressionModeSlot()));
  connect(exprWidgets_.removeRadio, SIGNAL(toggled(bool)), this, SLOT(expressionModeSlot()));
  connect(exprWidgets_.modifyRadio, SIGNAL(toggled(bool)), this, SLOT(expressionModeSlot()));

  //--

  QGridLayout *exprGridLayout = CQUtil::makeLayout<QGridLayout>(0, 2);

  exprFrameLayout->addLayout(exprGridLayout);

  int row = 0;

  //---

  auto exprValueLabel = CQUtil::makeLabelWidget<QLabel>("Expression", "valueLabel");

  exprWidgets_.valueEdit = CQUtil::makeWidget<CQChartsLineEdit>("valueEdit");

  exprWidgets_.valueEdit->setToolTip("+<expr> OR -<column> OR =<column>:<expr>\n"
    "Use: @<number> as shorthand for column(<number>)\n"
    "Functions: column, row, cell, setColumn, setRow, setCell\n"
    " header, setHeader, type, setType, map, bucket, norm, key, rand");

  exprGridLayout->addWidget(exprValueLabel        , row, 0);
  exprGridLayout->addWidget(exprWidgets_.valueEdit, row, 1);

  ++row;

  //----

  exprWidgets_.columnLabel = CQUtil::makeLabelWidget<QLabel>("Column", "columnLabel");

  exprWidgets_.columnEdit = CQUtil::makeWidget<CQChartsLineEdit>("columnEdit");

  exprWidgets_.columnEdit->setToolTip("Column to Modify");

  exprGridLayout->addWidget(exprWidgets_.columnLabel, row, 0);
  exprGridLayout->addWidget(exprWidgets_.columnEdit , row, 1);

  ++row;

  //----

  auto exprNameLabel = CQUtil::makeLabelWidget<QLabel>("Name", "nameLabel");

  exprWidgets_.nameEdit = CQUtil::makeWidget<CQChartsLineEdit>("nameEdit");

  exprWidgets_.nameEdit->setToolTip("Column Name");

  exprGridLayout->addWidget(exprNameLabel        , row, 0);
  exprGridLayout->addWidget(exprWidgets_.nameEdit, row, 1);

  ++row;

  //--

  exprWidgets_.typeLabel = CQUtil::makeLabelWidget<QLabel>("Type", "typeLabel");

  exprWidgets_.typeEdit = CQUtil::makeWidget<CQChartsLineEdit>("typeEdit");

  exprWidgets_.typeEdit->setToolTip("Column Type");

  exprGridLayout->addWidget(exprWidgets_.typeLabel, row, 0);
  exprGridLayout->addWidget(exprWidgets_.typeEdit , row, 1);

  ++row;

  //--

  exprFrameLayout->addStretch(1);

  //--

  QHBoxLayout *exprButtonLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  exprFrameLayout->addLayout(exprButtonLayout);

  QPushButton *exprApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "exprApply");

  connect(exprApplyButton, SIGNAL(clicked()), this, SLOT(exprApplySlot()));

  exprButtonLayout->addStretch(1);
  exprButtonLayout->addWidget(exprApplyButton);

  //---

  return exprFrame;
}

#ifdef CQCHARTS_FOLDED_MODEL
QFrame *
CQChartsModelControl::
addFoldFrame()
{
  QFrame *foldFrame = CQUtil::makeWidget<QFrame>("foldFrame");

  QVBoxLayout *foldFrameLayout = CQUtil::makeLayout<QVBoxLayout>(foldFrame, 0, 2);

  //---

  QGridLayout *foldWidgetsLayout = CQUtil::makeLayout<QGridLayout>(0, 2);

  foldFrameLayout->addLayout(foldWidgetsLayout);

  int foldRow = 0;

  //---

  // fold column
  foldWidgets_.columnEdit = addLineEdit(foldWidgetsLayout, foldRow, "Column", "column");

  foldWidgets_.columnEdit->setToolTip("Fold Column");

  //---

  // fold type
  auto foldTypeLabel = CQUtil::makeLabelWidget<QLabel>("Type", "type");

  foldWidgets_.typeCombo = CQUtil::makeWidget<QComboBox>("foldType");

  foldWidgets_.typeCombo->addItems(QStringList() << "Bucketed" << "Hier Separator");

  foldWidgetsLayout->addWidget(foldTypeLabel         , foldRow, 0);
  foldWidgetsLayout->addWidget(foldWidgets_.typeCombo, foldRow, 1); ++foldRow;

  foldWidgets_.typeCombo->setToolTip("Fold using bucketed values or hierarchical separator");

  //---

  // fold auto check
  auto foldAutoLabel = CQUtil::makeLabelWidget<QLabel>("Auto", "auto");

  foldWidgets_.autoCheck = CQUtil::makeWidget<CQCheckBox>("foldAuto");

  foldWidgets_.autoCheck->setText("Auto");
  foldWidgets_.autoCheck->setChecked(true);

  foldWidgetsLayout->addWidget(foldAutoLabel         , foldRow, 0);
  foldWidgetsLayout->addWidget(foldWidgets_.autoCheck, foldRow, 1); ++foldRow;

  foldWidgets_.autoCheck->setToolTip("Automatically determine fold buckets");

  //---

  // fold delta
  foldWidgets_.deltaEdit = addLineEdit(foldWidgetsLayout, foldRow, "Delta", "delta");

  foldWidgets_.deltaEdit->setText("1.0");
  foldWidgets_.deltaEdit->setToolTip("Explicit fold delta");

  //---

  // fold count
  foldWidgets_.countEdit = addLineEdit(foldWidgetsLayout, foldRow, "Count", "count");

  foldWidgets_.countEdit->setText("20");
  foldWidgets_.countEdit->setToolTip("Explicit fold count");

  //---

  // fold separator
  foldWidgets_.separatorEdit = addLineEdit(foldWidgetsLayout, foldRow, "Separator", "separator");

  foldWidgets_.separatorEdit->setText("/");
  foldWidgets_.separatorEdit->setToolTip("Hierarchical separator");

  //---

  foldWidgetsLayout->setRowStretch(foldRow, 1); ++foldRow;

  //--

  QHBoxLayout *foldButtonLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  foldFrameLayout->addLayout(foldButtonLayout);

  QPushButton *foldApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "foldApply");

  connect(foldApplyButton, SIGNAL(clicked()), this, SLOT(foldApplySlot()));

  QPushButton *foldClearButton = CQUtil::makeLabelWidget<QPushButton>("Clear", "foldClear");

  connect(foldClearButton, SIGNAL(clicked()), this, SLOT(foldClearSlot()));

  foldButtonLayout->addStretch(1);
  foldButtonLayout->addWidget(foldApplyButton);
  foldButtonLayout->addWidget(foldClearButton);

  //--

  return foldFrame;
}
#endif

QFrame *
CQChartsModelControl::
addColumnDataFrame()
{
  QFrame *columnDataFrame = CQUtil::makeWidget<QFrame>("columnDataFrame");

  QVBoxLayout *columnDataLayout = CQUtil::makeLayout<QVBoxLayout>(columnDataFrame, 2, 2);

  //---

  columnEditData_.editFrame = CQUtil::makeWidget<QFrame>("columnEditFrame");

  columnEditData_.editLayout = CQUtil::makeLayout<QGridLayout>(columnEditData_.editFrame, 0, 2);

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

  columnEditData_.numEdit  ->setToolTip("Column Number");
  columnEditData_.nameEdit ->setToolTip("Column Name");
  columnEditData_.typeCombo->setToolTip("Column Type");

  columnEditData_.editLayout->setRowStretch(columnEditData_.row, 1);

  connect(columnEditData_.typeCombo, SIGNAL(currentIndexChanged(int)),
          this, SLOT(typeChangedSlot()));

  //---

  columnDataLayout->addStretch(1);

  //---

  QHBoxLayout *columnButtonLayout = CQUtil::makeLayout<QHBoxLayout>(0, 2);

  columnDataLayout->addLayout(columnButtonLayout);

  QPushButton *typeApplyButton = CQUtil::makeLabelWidget<QPushButton>("Apply", "typeApply");

  connect(typeApplyButton, SIGNAL(clicked()), this, SLOT(typeApplySlot()));

  columnButtonLayout->addStretch(1);
  columnButtonLayout->addWidget(typeApplyButton);

  //---

  return columnDataFrame;
}

QFrame *
CQChartsModelControl::
addPropertiesFrame()
{
  QFrame *propertiesFrame = CQUtil::makeWidget<QFrame>("propertiesFrame");

  QVBoxLayout *propertiesFrameLayout = CQUtil::makeLayout<QVBoxLayout>(propertiesFrame, 2, 2);

  //---

  propertyModel_ = new CQPropertyViewModel;

  propertyTree_ = new CQPropertyViewTree(this, propertyModel_);

  propertiesFrameLayout->addWidget(propertyTree_);

  //------

  return propertiesFrame;
}

CQChartsLineEdit *
CQChartsModelControl::
addLineEdit(QGridLayout *grid, int &row, const QString &name, const QString &objName) const
{
  auto label = CQUtil::makeLabelWidget<QLabel>("", objName + "Label");
  auto edit  = CQUtil::makeWidget<CQChartsLineEdit>(objName + "Edit" );

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
  QLabel    *label = CQUtil::makeLabelWidget<QLabel>("", objName + "Label");
  QComboBox *combo = CQUtil::makeWidget<QComboBox>(objName + "Combo");

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

  if      (exprWidgets_.addRadio   ->isChecked()) exprMode_ = Mode::ADD;
  else if (exprWidgets_.removeRadio->isChecked()) exprMode_ = Mode::REMOVE;
  else if (exprWidgets_.modifyRadio->isChecked()) exprMode_ = Mode::MODIFY;

  exprWidgets_.columnLabel->setEnabled(exprMode_ == Mode::MODIFY);
  exprWidgets_.columnEdit ->setEnabled(exprMode_ == Mode::MODIFY);

  exprWidgets_.typeLabel->setEnabled(exprMode_ != Mode::REMOVE);
  exprWidgets_.nameEdit ->setEnabled(exprMode_ != Mode::REMOVE);
  exprWidgets_.typeEdit ->setEnabled(exprMode_ != Mode::REMOVE);
}

void
CQChartsModelControl::
exprApplySlot()
{
  QString expr = exprWidgets_.valueEdit->text().simplified();

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

  QString columnStr = exprWidgets_.columnEdit->text();

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
    QString nameStr = exprWidgets_.nameEdit->text();
    QString typeStr = exprWidgets_.typeEdit->text();

    if (column1 < 0) {
      charts_->errorMsg("Invalid column");
      return;
    }

    if (nameStr.length())
      model->setHeaderData(column1, Qt::Horizontal, nameStr, Qt::DisplayRole);

    if (typeStr.length()) {
      if (! CQChartsModelUtil::setColumnTypeStr(charts_, model.data(),
                                                CQChartsColumn(column1), typeStr)) {
        charts_->errorMsg("Invalid type '" + typeStr + "'");
        return;
      }
    }
  }
}

#ifdef CQCHARTS_FOLDED_MODEL
void
CQChartsModelControl::
foldApplySlot()
{
  if (! modelData_)
    return;

  bool ok;

  CQChartsModelData::FoldData foldData;

  foldData.columnsStr = foldWidgets_.columnEdit->text();

  if      (foldWidgets_.typeCombo->currentIndex() == 0) {
    foldData.foldType = CQChartsModelData::FoldData::FoldType::BUCKET;
    foldData.isAuto   = foldWidgets_.autoCheck->isChecked();
    foldData.delta    = CQChartsUtil::toReal(foldWidgets_.deltaEdit->text(), ok);
    foldData.count    = CQChartsUtil::toInt (foldWidgets_.countEdit->text(), ok);
  }
  else if (foldWidgets_.typeCombo->currentIndex() == 1) {
    foldData.foldType  = CQChartsModelData::FoldData::FoldType::SEPARATOR;
    foldData.separator = foldWidgets_.separatorEdit->text();
  }

  modelData_->foldModel(foldData);

  updateModel();

  updateModelDetails();
}
#endif

#ifdef CQCHARTS_FOLDED_MODEL
void
CQChartsModelControl::
foldClearSlot()
{
  if (! modelData_)
    return;

  modelData_->foldClear();

  updateModel();

  updateModelDetails();
}
#endif

void
CQChartsModelControl::
updateCurrentModel()
{
  CQChartsModelData *modelData = charts_->currentModelData();

  setModelData(modelData);
}

void
CQChartsModelControl::
setModelData(CQChartsModelData *modelData)
{
  if (modelData != modelData_) {
    if (modelData_)
      disconnect(modelData_, SIGNAL(currentColumnChanged(int)), this, SLOT(setColumnData(int)));

    modelData_ = modelData;

    if (modelData_)
      setColumnData(modelData_->currentColumn());

    if (modelData_)
      connect(modelData_, SIGNAL(currentColumnChanged(int)), this, SLOT(setColumnData(int)));

    //---

    if (modelData_) {
      CQPropertyViewModel *propertyModel = modelData_->propertyViewModel();

      propertyTree_->setPropertyModel(propertyModel);
    }
    else {
      propertyTree_->setPropertyModel(propertyModel_);
    }
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

  long column = CQChartsUtil::toInt(numStr, ok);

  if (! ok) {
    charts_->errorMsg("Invalid column number '" + numStr + "'");
    return;
  }

  //--

  QString nameStr = columnEditData_.nameEdit->text();

  if (nameStr.length())
    model->setHeaderData(int(column), Qt::Horizontal, nameStr, Qt::DisplayRole);

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

      if      (param->type() == CQBaseModelType::BOOLEAN) {
        if (paramEdit.edit->type() == CQBaseModelType::BOOLEAN)
          value = (paramEdit.edit->getBool() ? "1" : "0");
      }
      else if (param->type() == CQBaseModelType::INTEGER) {
        if (paramEdit.edit->type() == CQBaseModelType::INTEGER)
          value = QString("%1").arg(paramEdit.edit->getInteger());
      }
      else if (param->type() == CQBaseModelType::ENUM) {
        if (paramEdit.edit->type() == CQBaseModelType::ENUM)
          value = paramEdit.edit->getEnum();
      }
      else if (param->type() == CQBaseModelType::COLOR) {
        if (paramEdit.edit->type() == CQBaseModelType::COLOR)
          value = paramEdit.edit->getColor();
       }
      else {
        if (paramEdit.edit->type() == CQBaseModelType::STRING)
          value = paramEdit.edit->getString();
      }

      if (value != "")
        nameValues.setNameValue(name, value);
    }

    columnTypeMgr->setModelColumnType(model.data(), CQChartsColumn(int(column)),
                                      columnType, nameValues);
  }

  setColumnData(int(column));
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

  if (CQChartsModelUtil::columnValueType(charts_, model.data(), CQChartsColumn(column),
                                         columnType, columnBaseType, nameValues)) {
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
        paramEdit.label = CQUtil::makeLabelWidget<QLabel>("", "label");
        paramEdit.edit  = new CQChartsParamEdit;

        columnEditData_.editLayout->addWidget(paramEdit.label, paramEdit.row, 0);
        columnEditData_.editLayout->addWidget(paramEdit.edit , paramEdit.row, 1);

        columnEditData_.paramEdits.push_back(paramEdit);
      }

      ParamEdit &paramEdit = columnEditData_.paramEdits[paramInd];

      paramEdit.label->setText(param->name());

      QVariant var;

      nameValues.nameValue(param->name(), var);

      if      (param->type() == CQBaseModelType::BOOLEAN)
        paramEdit.edit->setBool(var.toBool());
      else if (param->type() == CQBaseModelType::INTEGER)
        paramEdit.edit->setInteger(var.toInt());
      else if (param->type() == CQBaseModelType::ENUM)
        paramEdit.edit->setEnum(var.toString(), param->values());
      else if (param->type() == CQBaseModelType::COLOR)
        paramEdit.edit->setColor(var.toString());
      else
        paramEdit.edit->setString(var.toString());

      paramEdit.label->setObjectName(param->name() + "_label");
      paramEdit.edit ->setObjectName(param->name() + "_edit" );
      paramEdit.edit ->setToolTip(param->tip());

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
