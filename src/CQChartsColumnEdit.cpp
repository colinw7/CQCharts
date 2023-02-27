#include <CQChartsColumnEdit.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsLineEdit.h>
#include <CQChartsPlot.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsObjUtil.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>

#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QPainter>

CQChartsColumnLineEdit::
CQChartsColumnLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("columnLineEdit");

  setToolTip("Optional Column Number or Name (use empty string to unset)");

  //---

  menuEdit_ = dataEdit_ = new CQChartsColumnEdit(this);

  setNoFocus();

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);

  columnToWidgets();
}

void
CQChartsColumnLineEdit::
setPlot(CQChartsPlot *plot)
{
  CQChartsLineEditBase::setPlot(plot);

  if (plot)
    setModelData(plot->getModelData());
}

CQChartsModelData *
CQChartsColumnLineEdit::
modelData() const
{
  return dataEdit_->modelData();
}

void
CQChartsColumnLineEdit::
setModelData(CQChartsModelData *modelData)
{
  dataEdit_->setModelData(modelData);
}

const CQChartsColumn &
CQChartsColumnLineEdit::
column() const
{
  return dataEdit_->column();
}

void
CQChartsColumnLineEdit::
setColumn(const Column &column)
{
  updateColumn(column, /*updateText*/true);
}

void
CQChartsColumnLineEdit::
setNumericOnly(bool b)
{
  numericOnly_ = b;
}

void
CQChartsColumnLineEdit::
setProxy(bool b)
{
  proxy_ = b;
}

void
CQChartsColumnLineEdit::
setNoFocus()
{
  dataEdit_->setNoFocus();
}

void
CQChartsColumnLineEdit::
updateColumn(const Column &column, bool updateText)
{
  connectSlots(false);

  dataEdit_->setColumn(column);

  connectSlots(true);

  if (updateText)
    columnToWidgets();

  Q_EMIT columnChanged();
}

void
CQChartsColumnLineEdit::
textChanged()
{
  // update column from widget (text)
  Column column;

  auto text = edit_->text();

  if (text.trimmed() != "") {
    if (modelData()) {
      auto *model = modelData()->currentModel(isProxy()).data();

      if (! CQChartsModelUtil::stringToColumn(model, text, column))
        return;
    }
    else {
      column = Column(text);
    }
  }

  updateColumn(column, /*updateText*/false);
}

void
CQChartsColumnLineEdit::
columnToWidgets()
{
  // update widget from current symbol
  connectSlots(false);

  QString text;

  if (column().isValid()) {
    if (modelData()) {
      bool ok;
      text = CQChartsModelUtil::columnToString(modelData()->model().data(), column(), ok);
    }
    else
      text = column().toString();
  }

  edit_->setText(text);

  auto tip = QString("%1 (%2)").arg(toolTip()).arg(column().toString());

  edit_->setToolTip(tip);

  connectSlots(true);
}

void
CQChartsColumnLineEdit::
menuEditChanged()
{
  columnToWidgets();

  Q_EMIT columnChanged();
}

void
CQChartsColumnLineEdit::
connectSlots(bool b)
{
  connectBaseSlots(b);

  CQChartsWidgetUtil::connectDisconnect(b,
    dataEdit_, SIGNAL(columnChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsColumnLineEdit::
drawPreview(QPainter *painter, const QRect &)
{
  auto str = (column().isValid() ? column().toString() : "<none>");

  drawCenteredText(painter, str);
}

void
CQChartsColumnLineEdit::
updateMenu()
{
  // update menu width
  CQChartsLineEditBase::updateMenu();

  // update menu height
  dataEdit_->updateMenu();
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsColumnPropertyViewType::
CQChartsColumnPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsColumnPropertyViewType::
getEditor() const
{
  return new CQChartsColumnPropertyViewEditor;
}

bool
CQChartsColumnPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsColumnPropertyViewType::
draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  //---

  bool ok;

  auto str = valueString(item, value, ok);

  auto font = option.font;

  if (! ok)
    font.setItalic(true);

  //---

  QFontMetrics fm(font);

  int w = fm.horizontalAdvance(str);

  auto option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 2*margin());

  option1.font = font;

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsColumnPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;

  auto str = valueString(nullptr, value, ok);

  return str;
}

QString
CQChartsColumnPropertyViewType::
valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const
{
  auto column = Column::fromVariant(value);

  QString str;

  if (column.isValid()) {
    str = column.toString();

    auto *plot = (item ? qobject_cast<CQChartsPlot *>(item->object()) : nullptr);

    if (plot) {
      auto str1 = plot->columnHeaderName(column);

      if (str1.length())
        str += " (" + str1 + ")";
    }

    ok  = true;
  }
  else {
    str = "<none>";
    ok  = false;
  }

  return str;
}

//------

CQChartsColumnPropertyViewEditor::
CQChartsColumnPropertyViewEditor()
{
}

QWidget *
CQChartsColumnPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *item = CQPropertyViewMgrInst->editItem();
  auto *obj  = (item ? item->object() : nullptr);
  if (! obj) return nullptr;

  //---

  CQChartsObjUtil::ObjData objData;

  CQChartsObjUtil::getObjData(obj, objData);

  //---

  auto *edit = new CQChartsColumnLineEdit(parent);

  if (objData.plot)
    edit->setModelData(objData.plot->getModelData());

  return edit;
}

void
CQChartsColumnPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsColumnLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(columnChanged()), obj, method);
}

QVariant
CQChartsColumnPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsColumnLineEdit *>(w);
  assert(edit);

  return CQChartsColumn::toVariant(edit->column());
}

void
CQChartsColumnPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsColumnLineEdit *>(w);
  assert(edit);

  auto column = Column::fromVariant(var);

  edit->setColumn(column);
}

//------

CQChartsColumnEdit::
CQChartsColumnEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  setObjectName("columnEdit");

  lineEdit_ = qobject_cast<CQChartsColumnLineEdit *>(parent);

  //---

  auto *layout = CQUtil::makeLayout<QGridLayout>(this, 2, 2);

  //---

  int row = 0;

  auto addLabelWidget = [&](const QString &id, const QString &label, QWidget *edit) {
    auto *labelW = CQUtil::makeLabelWidget<QLabel>(label, id + "Label");

    layout->addWidget(labelW, row, 0);
    layout->addWidget(edit  , row, 1);

    widgetLabels_[edit] = labelW;

    ++row;
  };

  auto createLabelCombo = [&](const QString &id, const QString &label, const QString &tipStr) {
    auto *combo = CQUtil::makeWidget<QComboBox>(id + "Combo");

    combo->setToolTip(tipStr);

    addLabelWidget(id, label, combo);

    return combo;
  };

  auto createLabelEdit = [&](const QString &id, const QString &label, const QString &tipStr) {
    auto *edit = CQUtil::makeWidget<CQChartsLineEdit>(id + "Edit");

    edit->setToolTip(tipStr);

    addLabelWidget(id, label, edit);

    return edit;
  };

  auto createColumnEdit = [&](const QString &id, const QString &label, const QString &tipStr) {
    auto *combo = CQUtil::makeWidget<CQChartsColumnCombo>(id + "Combo");

    combo->setToolTip(tipStr);

    addLabelWidget(id, label, combo);

    return combo;
  };

  //---

  // column custom name
  nameEdit_ = createLabelEdit("name", "Name", "Column Custom Name");

  //---

  // type combo
  typeCombo_ = createLabelCombo("type", "Type", "Column Type");

  typeCombo_->addItems(QStringList() <<
    "Column"            << // DATA, DATA_INDEX
    "Column Ref"        << // COLUMN_REF
    "Expression"        << // EXPR
    "Tcl Data"          << // TCL_DATA
    "Row Number"        << // ROW
    "Vertical Header"   << // COLUMN
    "Cell Value"        << // CELL
    "Horizontal Header" << // HHEADER
    "Vertical Header"   << // VHEADER
    "Group"                // GROUP
  );

  //---

  // column name/number
  columnCombo_ = createColumnEdit("column", "Column", "Column Name or Number");

  // column role
  roleEdit_ = createLabelEdit("role", "Role", "Column Role");

  // column index
  indexEdit_ = createLabelEdit("index", "Index", "Column Index");

  //---

  // column expression
  expressionEdit_ = createLabelEdit("expr", "Expr", "Column Expression");

  //---

  layout->setRowStretch(row, 1);

  layout->setColumnStretch(1, 1);

  //---

  connectSlots(true);

  updateState();
}

void
CQChartsColumnEdit::
setModelData(CQChartsModelData *modelData)
{
  if (modelData_)
    disconnect(modelData_, SIGNAL(destroyed(QObject *)), this, SLOT(resetModelData()));

  modelData_ = modelData;

  if (modelData_)
    connect(modelData_, SIGNAL(destroyed(QObject *)), this, SLOT(resetModelData()));

  updateColumnsFromModel();

  columnToWidgets();

  updateState();
}

void
CQChartsColumnEdit::
resetModelData()
{
  modelData_ = nullptr;
}

void
CQChartsColumnEdit::
setColumn(const Column &column)
{
  column_ = column;

  columnToWidgets();

  updateState();

  Q_EMIT columnChanged();
}

void
CQChartsColumnEdit::
columnToWidgets()
{
  connectSlots(false);

  nameEdit_      ->setText("");
  columnCombo_   ->setCurrentIndex(0);
  roleEdit_      ->setText("");
  indexEdit_     ->setText("");
  expressionEdit_->setText("");

  if (column_.isValid()) {
    nameEdit_->setText(column_.name());

    if      (column_.type() == Column::Type::DATA ||
             column_.type() == Column::Type::DATA_INDEX) {
      typeCombo_->setCurrentIndex(0);

      columnCombo_->setColumn(column_);

      if (column_.hasRole())
        roleEdit_->setText(QString::number(column_.role()));

      if (column_.type() == Column::Type::DATA_INDEX)
        indexEdit_->setText(column_.index());
    }
    else if (column_.type() == Column::Type::EXPR) {
      typeCombo_->setCurrentIndex(1);

      expressionEdit_->setText(column_.expr());
    }
    else if (column_.type() == Column::Type::COLUMN_REF) {
      typeCombo_->setCurrentIndex(2);

      expressionEdit_->setText(column_.refName());
    }
    else if (column_.type() == Column::Type::TCL_DATA) {
      typeCombo_->setCurrentIndex(3);

      expressionEdit_->setText(column_.tclData());
    }
    else if (column_.type() == Column::Type::ROW) {
      typeCombo_->setCurrentIndex(4);
    }
    else if (column_.type() == Column::Type::COLUMN) {
      typeCombo_->setCurrentIndex(5);
    }
    else if (column_.type() == Column::Type::CELL) {
      typeCombo_->setCurrentIndex(6);
    }
    else if (column_.type() == Column::Type::HHEADER) {
      typeCombo_->setCurrentIndex(7);
    }
    else if (column_.type() == Column::Type::VHEADER) {
      typeCombo_->setCurrentIndex(8);
    }
    else if (column_.type() == Column::Type::GROUP) {
      typeCombo_->setCurrentIndex(9);
    }
  }
  else {
    typeCombo_->setCurrentIndex(0);

    if (modelData_)
      columnCombo_->setColumn(column_);

    expressionEdit_->setText("");
  }

  connectSlots(true);
}

void
CQChartsColumnEdit::
widgetsToColumn()
{
  Column column;

  auto getExprEditStr = [&]() {
    QString str;

    if (expressionEdit_->text().trimmed().length())
      str = expressionEdit_->text();

    return str;
  };

  // DATA, DATA_INDEX
  int typeInd = typeCombo_->currentIndex();

  if      (typeInd == 0) {
    auto icolumn = columnCombo_->getColumn();

    bool ok;

    long role = CQChartsUtil::toInt(roleEdit_->text(), ok);

    if (! ok)
      role = -1;

    if (indexEdit_->text().trimmed().length())
      column = Column::makeDataIndex(icolumn.column(), indexEdit_->text().trimmed(), int(role));
    else
      column = Column::makeData(icolumn.column(), int(role));
  }
  // EXPR
  else if (typeInd == 1) {
    auto str = getExprEditStr();

    column = Column::makeExpr(str);
  }
  // COLUMN_REF
  else if (typeInd == 2) {
    auto str = getExprEditStr();

    column = Column::makeColumnRef(str);
  }
  // TCL_DATA
  else if (typeInd == 3) {
    auto str = getExprEditStr();

    column = Column::makeTclData(str);
  }
  // ROW
  else if (typeInd == 4) {
    column = Column::makeRow();
  }
  // COLUMN
  else if (typeInd == 5) {
    column = Column::makeColumn();
  }
  // CELL
  else if (typeInd == 6) {
    column = Column::makeCell();
  }
  // HHEADER
  else if (typeInd == 7) {
    column = Column::makeHHeader(-1);
  }
  // VHEADER
  else if (typeInd == 8) {
    column = Column::makeVHeader();
  }
  // GROUP
  else if (typeInd == 9) {
    column = Column::makeGroup();
  }

  //---

  if (nameEdit_->text().trimmed().length())
    column.setName(nameEdit_->text().trimmed());

  //---

  column_ = column;

  //---

  updateState();

  Q_EMIT columnChanged();
}

void
CQChartsColumnEdit::
updateMenu()
{
  if (lineEdit_)
    lineEdit_->updateMenuEditHeight();
}

void
CQChartsColumnEdit::
setNoFocus()
{
//nameEdit_      ->setFocusPolicy(Qt::NoFocus);
//roleEdit_      ->setFocusPolicy(Qt::NoFocus);
//indexEdit_     ->setFocusPolicy(Qt::NoFocus);
//expressionEdit_->setFocusPolicy(Qt::NoFocus);
}

void
CQChartsColumnEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(b, w, from, this, to);
  };

  connectDisconnect(b, typeCombo_, SIGNAL(currentIndexChanged(int)),
                    SLOT(typeSlot()));

  connectDisconnect(b, nameEdit_, SIGNAL(textChanged(const QString &)),
                    SLOT(widgetsToColumnSlot()));

  connectDisconnect(b, columnCombo_, SIGNAL(columnChanged()),
                    SLOT(widgetsToColumnSlot()));
  connectDisconnect(b, roleEdit_, SIGNAL(textChanged(const QString &)),
                    SLOT(widgetsToColumnSlot()));
  connectDisconnect(b, indexEdit_, SIGNAL(textChanged(const QString &)),
                    SLOT(widgetsToColumnSlot()));

  connectDisconnect(b, expressionEdit_, SIGNAL(textChanged(const QString &)),
                    SLOT(widgetsToColumnSlot()));
}

void
CQChartsColumnEdit::
typeSlot()
{
  widgetsToColumnSlot();

  if (lineEdit_)
    lineEdit_->updateMenuEditHeight();
}

void
CQChartsColumnEdit::
widgetsToColumnSlot()
{
  connectSlots(false);

  widgetsToColumn();

  connectSlots(true);

  Q_EMIT columnChanged();
}

void
CQChartsColumnEdit::
updateColumnsFromModel()
{
  columnCombo_->setModelData(modelData());
}

void
CQChartsColumnEdit::
updateState()
{
  auto setEditVisible = [&](QWidget *w, bool visible) {
    w->setVisible(visible);

    widgetLabels_[w]->setVisible(visible);
  };

  auto setEditLabel = [&](QWidget *w,  const QString &label) {
    auto *labelW = qobject_cast<QLabel *>(widgetLabels_[w]);
    assert(labelW);

    labelW->setText(label);
  };

  int typeInd = typeCombo_->currentIndex();

  setEditVisible(columnCombo_   , typeInd == 0);
  setEditVisible(roleEdit_      , typeInd == 0);
  setEditVisible(indexEdit_     , typeInd == 0);

  setEditVisible(expressionEdit_, typeInd == 1 || // EXPR
                                  typeInd == 2 || // COLUMN_REF
                                  typeInd == 3);  // TCL DATA

  if      (typeInd == 1)
    setEditLabel(expressionEdit_, "Expression");
  else if (typeInd == 2)
    setEditLabel(expressionEdit_, "Ref Column");
  else if (typeInd == 3)
    setEditLabel(expressionEdit_, "Tcl Data");
}

QSize
CQChartsColumnEdit::
sizeHint() const
{
  int w = width();

  return QSize(w, widgetHeight_);
}

QSize
CQChartsColumnEdit::
minimumSizeHint() const
{
  QFontMetrics fm(font());

  int eh = fm.height() + 8;

  int typeInd = typeCombo_->currentIndex();

  int n = 1;

  if      (typeInd == 0) n = 5;
  else if (typeInd == 1) n = 3;
  else if (typeInd == 2) n = 3;
  else if (typeInd == 3) n = 3;
  else if (typeInd == 4) n = 2;
  else if (typeInd == 5) n = 2;
  else if (typeInd == 6) n = 2;
  else if (typeInd == 7) n = 2;
  else if (typeInd == 8) n = 2;
  else if (typeInd == 9) n = 2;

  return QSize(0, eh*n);
}
