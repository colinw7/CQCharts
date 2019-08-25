#include <CQChartsModelControl.h>
#include <CQChartsModelData.h>
#include <CQChartsColumnType.h>
#include <CQChartsExprModel.h>
#include <CQChartsModelFilter.h>
#include <CQChartsModelUtil.h>
#include <CQChartsColorEdit.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewTree.h>
#include <CQPivotModel.h>
#include <CQCsvModel.h>
#include <CQTsvModel.h>
#include <CQGnuDataModel.h>
#include <CQJsonModel.h>
#include <CQDataModel.h>
#include <CQLineEdit.h>
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

  void setString(const QString &str="") {
    if (! edit_) {
      reset();

      edit_ = CQUtil::makeWidget<CQLineEdit>("edit");

      layout_->addWidget(edit_);
    }

    edit_->setText(str);
  }

  void setBool(bool b=false) {
    if (! check_) {
      reset();

      check_ = CQUtil::makeWidget<CQCheckBox>("edit");

      layout_->addWidget(check_);
    }

    check_->setChecked(b);
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
  }

  void setColor(const QString &str="") {
    if (! color_) {
      reset();

      color_ = CQUtil::makeWidget<CQChartsColorLineEdit>("edit");

      layout_->addWidget(color_);
    }

    CQChartsColor c(str);

    color_->setColor(c);
  }

  QString getString() const {
    assert(edit_);

    return edit_->text().simplified();
  }

  bool getBool() const {
    assert(check_);

    return check_->isChecked();
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
    delete check_;
    delete combo_;
    delete color_;

    edit_  = nullptr;
    check_ = nullptr;
    combo_ = nullptr;
    color_ = nullptr;
  }

 private:
  QHBoxLayout*           layout_ { nullptr };
  CQLineEdit*            edit_   { nullptr };
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

  QFrame *foldFrame = addFoldFrame();

  controlTab->addTab(foldFrame, "Fold");

  QFrame *columnDataFrame = addColumnDataFrame();

  controlTab->addTab(columnDataFrame, "Column Data");

  QFrame *propertiesFrame = addPropertiesFrame();

  controlTab->addTab(propertiesFrame, "Properties");

  //---

  setModelData(modelData);

  expressionModeSlot();
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

  QGridLayout *exprGridLayout = CQUtil::makeLayout<QGridLayout>(0, 2);

  exprFrameLayout->addLayout(exprGridLayout);

  int row = 0;

  //---

  exprValueLabel_ = CQUtil::makeLabelWidget<QLabel>("Expression", "exprValueLabel");

  exprValueEdit_ = CQUtil::makeWidget<CQLineEdit>("exprValueEdit");

  exprValueEdit_->setToolTip("+<expr> OR -<column> OR =<column>:<expr>\n"
                             "Use: @<number> as shorthand for column(<number>)\n"
                             "Functions: column, row, cell, setColumn, setRow, setCell\n"
                             " header, setHeader, type, setType, map, bucket, norm, key, rand");

  exprGridLayout->addWidget(exprValueLabel_, row, 0);
  exprGridLayout->addWidget(exprValueEdit_ , row, 1);

  ++row;

  //----

  exprColumnLabel_ = CQUtil::makeLabelWidget<QLabel>("Column", "exprEditLabel");

  exprColumnEdit_ = CQUtil::makeWidget<CQLineEdit>("exprColumnEdit");

  exprColumnEdit_->setToolTip("Column to Modify");

  exprGridLayout->addWidget(exprColumnLabel_, row, 0);
  exprGridLayout->addWidget(exprColumnEdit_ , row, 1);

  ++row;

  //----

  exprNameLabel_ = CQUtil::makeLabelWidget<QLabel>("Name", "exprNameLabel");

  exprNameEdit_ = CQUtil::makeWidget<CQLineEdit>("exprNameEdit");

  exprNameEdit_->setToolTip("Column Name");

  exprGridLayout->addWidget(exprNameLabel_, row, 0);
  exprGridLayout->addWidget(exprNameEdit_ , row, 1);

  ++row;

  //--

  exprTypeLabel_ = CQUtil::makeLabelWidget<QLabel>("Type", "exprTypeLabel");

  exprTypeEdit_ = CQUtil::makeWidget<CQLineEdit>("exprTypeEdit");

  exprTypeEdit_->setToolTip("Column Type");

  exprGridLayout->addWidget(exprTypeLabel_, row, 0);
  exprGridLayout->addWidget(exprTypeEdit_ , row, 1);

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

  foldColumnEdit_ = addLineEdit(foldWidgetsLayout, foldRow, "Column", "column");

  foldAutoCheck_ = CQUtil::makeWidget<CQCheckBox>("foldAuto");

  foldAutoCheck_->setText("Auto");
  foldAutoCheck_->setChecked(true);

  foldWidgetsLayout->addWidget(foldAutoCheck_, foldRow, 0, 1, 1); ++foldRow;

  foldDeltaEdit_ = addLineEdit(foldWidgetsLayout, foldRow, "Delta", "delta");

  foldDeltaEdit_->setText("1.0");

  foldCountEdit_ = addLineEdit(foldWidgetsLayout, foldRow, "Count", "count");

  foldCountEdit_->setText("20");

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

CQLineEdit *
CQChartsModelControl::
addLineEdit(QGridLayout *grid, int &row, const QString &name, const QString &objName) const
{
  QLabel     *label = CQUtil::makeLabelWidget<QLabel>("", objName + "Label");
  CQLineEdit *edit  = CQUtil::makeWidget<CQLineEdit>(objName + "Edit" );

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

    long icolumn = CQChartsUtil::toInt(columnStr, ok);

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

  bool ok;

  CQChartsModelData::FoldData foldData;

  foldData.columnsStr = foldColumnEdit_->text();
  foldData.isAuto     = foldAutoCheck_->isChecked();
  foldData.delta      = CQChartsUtil::toReal(foldDeltaEdit_->text(), ok);
  foldData.count      = CQChartsUtil::toInt (foldCountEdit_->text(), ok);

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
      propertyModel_->clear();

      ModelP model = modelData_->currentModel();

      QAbstractItemModel *absModel = CQChartsModelUtil::getBaseModel(model.data());

      CQBaseModel *baseModel = qobject_cast<CQBaseModel *>(absModel);
      CQDataModel *dataModel = CQChartsModelUtil::getDataModel(model.data());

      CQChartsExprModel *exprModel = CQChartsModelUtil::getExprModel(absModel);

      CQChartsModelFilter *modelFilter = qobject_cast<CQChartsModelFilter *>(absModel);

      CQPivotModel *pivotModel = qobject_cast<CQPivotModel *>(baseModel);

      CQCsvModel     *csvModel  = qobject_cast<CQCsvModel     *>(absModel);
      CQTsvModel     *tsvModel  = qobject_cast<CQTsvModel     *>(absModel);
      CQGnuDataModel *gnuModel  = qobject_cast<CQGnuDataModel *>(absModel);
      CQJsonModel    *jsonModel = qobject_cast<CQJsonModel    *>(absModel);

      if (baseModel) {
        propertyModel_->addProperty("", baseModel, "dataType"   , "");
        propertyModel_->addProperty("", baseModel, "title"      , "");
        propertyModel_->addProperty("", baseModel, "maxTypeRows", "");
      }

      if (dataModel) {
        propertyModel_->addProperty("", dataModel, "readOnly", "");
        propertyModel_->addProperty("", dataModel, "filter"  , "");
      }

      if (exprModel) {
        propertyModel_->addProperty("", exprModel, "debug", "");
      }

      if (modelFilter) {
        propertyModel_->addProperty("", modelFilter, "filter", "");
        propertyModel_->addProperty("", modelFilter, "type"  , "");
        propertyModel_->addProperty("", modelFilter, "invert", "");
      }

      if (pivotModel) {
        propertyModel_->addProperty("", pivotModel, "valueType"    , "");
        propertyModel_->addProperty("", pivotModel, "includeTotals", "");
      }

      if (csvModel) {
        propertyModel_->addProperty("", csvModel, "filename"         , "");
        propertyModel_->addProperty("", csvModel, "commentHeader"    , "");
        propertyModel_->addProperty("", csvModel, "firstLineHeader"  , "");
        propertyModel_->addProperty("", csvModel, "firstColumnHeader", "");
        propertyModel_->addProperty("", csvModel, "separator"        , "");
      }

      if (tsvModel) {
        propertyModel_->addProperty("", tsvModel, "filename"         , "");
        propertyModel_->addProperty("", tsvModel, "commentHeader"    , "");
        propertyModel_->addProperty("", tsvModel, "firstLineHeader"  , "");
        propertyModel_->addProperty("", tsvModel, "firstColumnHeader", "");
      }

      if (gnuModel) {
        propertyModel_->addProperty("", gnuModel, "filename"         , "");
        propertyModel_->addProperty("", gnuModel, "commentHeader"    , "");
        propertyModel_->addProperty("", gnuModel, "firstLineHeader"  , "");
        propertyModel_->addProperty("", gnuModel, "firstColumnHeader", "");
        propertyModel_->addProperty("", gnuModel, "commentChars"     , "");
        propertyModel_->addProperty("", gnuModel, "missingStr"       , "");
        propertyModel_->addProperty("", gnuModel, "separator"        , "");
        propertyModel_->addProperty("", gnuModel, "parseStrings"     , "");
        propertyModel_->addProperty("", gnuModel, "setBlankLines"    , "");
        propertyModel_->addProperty("", gnuModel, "subSetBlankLines" , "");
        propertyModel_->addProperty("", gnuModel, "keepQuotes"       , "");
      }

      if (jsonModel) {
        propertyModel_->addProperty("", jsonModel, "hierarchical", "");
        propertyModel_->addProperty("", jsonModel, "flat"        , "");
      }
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

      if      (param->type() == CQBaseModelType::BOOLEAN)
        value = (paramEdit.edit->getBool() ? "1" : "0");
      else if (param->type() == CQBaseModelType::ENUM)
        value = paramEdit.edit->getEnum();
      else if (param->type() == CQBaseModelType::COLOR)
        value = paramEdit.edit->getColor();
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
