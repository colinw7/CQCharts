#include <CQChartsColumnEdit.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsLineEdit.h>
#include <CQChartsPlot.h>
#include <CQChartsModelUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQGroupBox.h>

#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAbstractItemModel>
#include <QPainter>

CQChartsColumnLineEdit::
CQChartsColumnLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("columnLineEdit");

  setToolTip("Optional Column Number or Name (use empty string to unset)");

  //---

  menuEdit_ = dataEdit_ = new CQChartsColumnEdit;

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);
}

void
CQChartsColumnLineEdit::
setPlot(CQChartsPlot *plot)
{
  CQChartsLineEditBase::setPlot(plot);

  if (plot)
    setModel(plot->model().data());
}

QAbstractItemModel *
CQChartsColumnLineEdit::
model() const
{
  return dataEdit_->model();
}

void
CQChartsColumnLineEdit::
setModel(QAbstractItemModel *model)
{
  dataEdit_->setModel(model);
}

const CQChartsColumn &
CQChartsColumnLineEdit::
column() const
{
  return dataEdit_->column();
}

void
CQChartsColumnLineEdit::
setColumn(const CQChartsColumn &column)
{
  updateColumn(column, /*updateText*/true);
}

void
CQChartsColumnLineEdit::
updateColumn(const CQChartsColumn &column, bool updateText)
{
  connectSlots(false);

  dataEdit_->setColumn(column);

  connectSlots(true);

  if (updateText)
    columnToWidgets();

  emit columnChanged();
}

void
CQChartsColumnLineEdit::
textChanged()
{
  CQChartsColumn column;

  QString text = edit_->text();

  if (text.simplified() == "") {
    column = CQChartsColumn();
  }
  else {
    if (model()) {
      if (! CQChartsModelUtil::stringToColumn(model(), text, column))
        return;
    }
    else {
      column = CQChartsColumn(text);
    }
  }

  updateColumn(column, /*updateText*/ false);
}

void
CQChartsColumnLineEdit::
columnToWidgets()
{
  connectSlots(false);

  if (column().isValid())
    edit_->setText(column().toString());
  else
    edit_->setText("");

  setToolTip(column().toString());

  connectSlots(true);
}

void
CQChartsColumnLineEdit::
menuEditChanged()
{
  columnToWidgets();

  emit columnChanged();
}

void
CQChartsColumnLineEdit::
connectSlots(bool b)
{
  connectBaseSlots(b);

  if (b)
    connect(dataEdit_, SIGNAL(columnChanged()), this, SLOT(menuEditChanged()));
  else
    disconnect(dataEdit_, SIGNAL(columnChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsColumnLineEdit::
drawPreview(QPainter *painter, const QRect &)
{
  QString str = (column().isValid() ? column().toString() : "<none>");

  drawCenteredText(painter, str);
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
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  //---

  bool ok;

  QString str = valueString(item, value, ok);

  QFont font = option.font;

  if (! ok)
    font.setItalic(true);

  //---

  QFontMetrics fm(font);

  int w = fm.width(str);

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  option1.font = font;

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsColumnPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;

  QString str = valueString(nullptr, value, ok);

  return str;
}

QString
CQChartsColumnPropertyViewType::
valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const
{
  CQChartsColumn column = value.value<CQChartsColumn>();

  QString str;

  if (column.isValid()) {
    str = column.toString();

    auto plot = (item ? qobject_cast<CQChartsPlot *>(item->object()) : nullptr);

    if (plot) {
      QString str1 = plot->columnHeaderName(column);

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
  CQPropertyViewItem *item = CQPropertyViewMgrInst->editItem();

  QObject *obj = (item ? item->object() : nullptr);

  auto plot = qobject_cast<CQChartsPlot *>(obj);

  //---

  CQChartsColumnLineEdit *edit = new CQChartsColumnLineEdit(parent);

  if (plot)
    edit->setModel(plot->model().data());

  return edit;
}

void
CQChartsColumnPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto edit = qobject_cast<CQChartsColumnLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(columnChanged()), obj, method);
}

QVariant
CQChartsColumnPropertyViewEditor::
getValue(QWidget *w)
{
  auto edit = qobject_cast<CQChartsColumnLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->column());
}

void
CQChartsColumnPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto edit = qobject_cast<CQChartsColumnLineEdit *>(w);
  assert(edit);

  CQChartsColumn column = var.value<CQChartsColumn>();

  edit->setColumn(column);
}

//------

CQChartsColumnEdit::
CQChartsColumnEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  setObjectName("columnEdit");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  columnGroup_ = CQUtil::makeLabelWidget<CQGroupBox>("Column", "columnGroup");

  columnGroup_->setCheckable(true);
  columnGroup_->setChecked(true);

  connect(columnGroup_, SIGNAL(clicked(bool)), this, SLOT(menuColumnGroupClicked(bool)));

  layout->addWidget(columnGroup_);

  //--

  // column name/number and role
  auto *menuColumnGroupLayout = CQUtil::makeLayout<QVBoxLayout>(columnGroup_, 2, 2);

  columnCombo_ = CQUtil::makeWidget<CQChartsColumnCombo>();

  connect(columnCombo_, SIGNAL(columnChanged()), this, SLOT(menuColumnChanged()));

  menuColumnGroupLayout->addWidget(columnCombo_);

  auto *roleFrame  = CQUtil::makeWidget<QFrame>("roleFrame");
  auto *roleLayout = CQUtil::makeLayout<QHBoxLayout>(roleFrame, 2, 2);

  roleLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("Role", "roleLabel"));

  roleEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("edit");

  roleEdit_->setToolTip("Column Role");

  connect(roleEdit_, SIGNAL(textChanged(const QString &)),
          this, SLOT(roleTextChanged(const QString &)));

  roleLayout->addWidget(roleEdit_);

  menuColumnGroupLayout->addWidget(roleFrame);

  //---

  // column expression
  menuExprGroup_ = CQUtil::makeLabelWidget<CQGroupBox>("Expression", "menuExprGroup");

  menuExprGroup_->setCheckable(true);
  menuExprGroup_->setChecked(false);

  connect(menuExprGroup_, SIGNAL(clicked(bool)), this, SLOT(menuExprGroupClicked(bool)));

  layout->addWidget(menuExprGroup_);

  //--

  auto *menuExprGroupLayout = CQUtil::makeLayout<QVBoxLayout>(menuExprGroup_, 2, 2);

  expressionEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("edit");

  expressionEdit_->setToolTip("Column Expression");

  connect(expressionEdit_, SIGNAL(textChanged(const QString &)),
          this, SLOT(expressionTextChanged(const QString &)));

  menuExprGroupLayout->addWidget(expressionEdit_);

  //---

  // vertical header
  vheaderCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Vertical Header", "vheaderCheck");

  vheaderCheck_->setToolTip("Get value from vertical header");

  connect(vheaderCheck_, SIGNAL(clicked(bool)), this, SLOT(vheaderCheckClicked(bool)));

  layout->addWidget(vheaderCheck_);

  //---

  updateState();
}

void
CQChartsColumnEdit::
setModel(QAbstractItemModel *model)
{
  model_ = model;

  updateColumnsFromModel();

  columnToWidgets();

  updateState();
}

const CQChartsColumn &
CQChartsColumnEdit::
column() const
{
  return column_;
}

void
CQChartsColumnEdit::
setColumn(const CQChartsColumn &column)
{
  column_ = column;

  columnToWidgets();

  updateState();

  emit columnChanged();
}

void
CQChartsColumnEdit::
columnToWidgets()
{
  connectSlots(false);

  columnGroup_   ->setChecked(false);
  columnCombo_   ->setCurrentIndex(0);
  menuExprGroup_ ->setChecked(false);
  expressionEdit_->setText("");
  vheaderCheck_  ->setChecked(false);

  if (column_.isValid()) {
    if      (column_.type() == CQChartsColumn::Type::DATA) {
      columnGroup_->setChecked(true);

      columnCombo_->setColumn(column_);

      if (column_.role() >= 0)
        roleEdit_->setText(QString("%1").arg(column_.role()));
      else
        roleEdit_->setText("");
    }
    else if (column_.type() == CQChartsColumn::Type::EXPR) {
      menuExprGroup_ ->setChecked(true);
      expressionEdit_->setText(column_.expr());
    }
    else if (column_.type() == CQChartsColumn::Type::VHEADER) {
      vheaderCheck_->setChecked(true);
    }
  }
  else {
    columnGroup_->setChecked(true);
  }

  connectSlots(true);
}

void
CQChartsColumnEdit::
widgetsToColumn()
{
  CQChartsColumn column;

  if      (columnGroup_->isChecked()) {
    CQChartsColumn icolumn = columnCombo_->getColumn();

    bool ok;

    long role = CQChartsUtil::toInt(roleEdit_->text(), ok);

    if (! ok)
      role = -1;

    column = CQChartsColumn(icolumn.column(), int(role));
  }
  else if (menuExprGroup_->isChecked()) {
    QString str;

    if (expressionEdit_->text().simplified().length())
      str = QString("%1").arg(expressionEdit_->text());
    else
      str = "";

    column = CQChartsColumn(CQChartsColumn::Type::EXPR, -1, str, -1);
  }
  else if (vheaderCheck_->isChecked()) {
    column = CQChartsColumn(CQChartsColumn::Type::VHEADER, -1, "", -1);
  }

  column_ = column;

  //---

  updateState();

  emit columnChanged();
}

void
CQChartsColumnEdit::
connectSlots(bool b)
{
  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(b, w, from, this, to);
  };

  connectDisconnect(b, columnGroup_, SIGNAL(clicked(bool)),
                    SLOT(menuColumnGroupClicked(bool)));
  connectDisconnect(b, columnCombo_, SIGNAL(columnChanged()),
                    SLOT(menuColumnChanged()));
  connectDisconnect(b, roleEdit_, SIGNAL(textChanged(const QString &)),
                    SLOT(roleTextChanged(const QString &)));
  connectDisconnect(b, menuExprGroup_, SIGNAL(clicked(bool)),
                    SLOT(menuExprGroupClicked(bool)));
  connectDisconnect(b, expressionEdit_, SIGNAL(textChanged(const QString &)),
                    SLOT(expressionTextChanged(const QString &)));
  connectDisconnect(b, vheaderCheck_, SIGNAL(clicked(bool)),
                    SLOT(vheaderCheckClicked(bool)));
}

void
CQChartsColumnEdit::
updateColumnsFromModel()
{
  columnCombo_->setModel(model());
}

void
CQChartsColumnEdit::
menuColumnGroupClicked(bool b)
{
  connectSlots(false);

  //---

  if (! b) {
    menuExprGroup_->setChecked(true);
    vheaderCheck_ ->setChecked(false);
  }
  else {
    menuExprGroup_->setChecked(false);
    vheaderCheck_ ->setChecked(false);
  }

  widgetsToColumn();

  updateState();

  //---

  connectSlots(true);

  emit columnChanged();
}

void
CQChartsColumnEdit::
menuExprGroupClicked(bool b)
{
  connectSlots(false);

  //---

  if (! b) {
    columnGroup_ ->setChecked(true);
    vheaderCheck_->setChecked(false);
  }
  else {
    columnGroup_ ->setChecked(false);
    vheaderCheck_->setChecked(false);
  }

  widgetsToColumn();

  updateState();

  //---

  connectSlots(true);

  emit columnChanged();
}

void
CQChartsColumnEdit::
vheaderCheckClicked(bool b)
{
  connectSlots(false);

  //---

  if (! b) {
    columnGroup_  ->setChecked(false);
    menuExprGroup_->setChecked(true);
  }
  else {
    columnGroup_  ->setChecked(false);
    menuExprGroup_->setChecked(false);
  }

  widgetsToColumn();

  updateState();

  //---

  connectSlots(true);

  emit columnChanged();
}

void
CQChartsColumnEdit::
menuColumnChanged()
{
  connectSlots(false);

  //---

  widgetsToColumn();

  //---

  connectSlots(true);

  emit columnChanged();
}

void
CQChartsColumnEdit::
roleTextChanged(const QString &)
{
  connectSlots(false);

  //---

  widgetsToColumn();

  //---

  connectSlots(true);

  emit columnChanged();
}

void
CQChartsColumnEdit::
expressionTextChanged(const QString &)
{
  connectSlots(false);

  //---

  widgetsToColumn();

  //---

  connectSlots(true);

  emit columnChanged();
}

void
CQChartsColumnEdit::
updateState()
{
  columnCombo_   ->setEnabled(columnGroup_  ->isChecked());
  expressionEdit_->setEnabled(menuExprGroup_->isChecked());
}
