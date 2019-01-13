#include <CQChartsColumnEdit.h>
#include <CQChartsPlot.h>
#include <CQChartsModelUtil.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>

#include <QComboBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAbstractItemModel>
#include <QStylePainter>

CQChartsColumnEdit::
CQChartsColumnEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("columnEdit");

  setFrameShape(QFrame::StyledPanel);
  setFrameShadow(QFrame::Sunken);

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  //---

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  edit_ = new QLineEdit;
  edit_->setObjectName("edit");

  edit_->setFrame(false);

  connect(edit_, SIGNAL(textChanged(const QString &)),
          this, SLOT(textChanged(const QString &)));

  layout->addWidget(edit_);

  //---

  button_ = new CQChartsColumnEditMenuButton;
  button_->setObjectName("button");

  QStyleOptionComboBox opt;

  initStyle(opt);

  QRect r = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, this);

  button_->setFixedWidth(r.size().width());

  button_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  button_->setFocusPolicy(Qt::NoFocus);

  connect(button_, SIGNAL(clicked()), this, SLOT(showMenu()));

  layout->addWidget(button_);

  //---

  menu_ = new CQWidgetMenu(this);

  connect(menu_, SIGNAL(menuShown()), this, SLOT(updateMenu()));

  //---

  QFrame *menuFrame = new QFrame;

  QVBoxLayout *menuFrameLayout = new QVBoxLayout(menuFrame);
  menuFrameLayout->setMargin(2); menuFrameLayout->setSpacing(2);

  menu_->setWidget(menuFrame);

  //---

  menuColumnGroup_ = new QGroupBox("Column");

  menuColumnGroup_->setCheckable(true);
  menuColumnGroup_->setChecked(true);

  connect(menuColumnGroup_, SIGNAL(clicked(bool)), this, SLOT(menuColumnGroupClicked(bool)));

  menuFrameLayout->addWidget(menuColumnGroup_);

  //--

  QVBoxLayout *menuColumnGroupLayout = new QVBoxLayout(menuColumnGroup_);
  menuColumnGroupLayout->setMargin(2); menuColumnGroupLayout->setSpacing(2);

  columnCombo_ = new QComboBox;

  connect(columnCombo_, SIGNAL(currentIndexChanged(int)),
          this, SLOT(menuColumnChanged(int)));

  menuColumnGroupLayout->addWidget(columnCombo_);

  QFrame *roleFrame = new QFrame;

  QHBoxLayout *roleLayout = new QHBoxLayout(roleFrame);

  roleLayout->addWidget(new QLabel("Role"));

  roleEdit_ = new QLineEdit;

  connect(roleEdit_, SIGNAL(textChanged(const QString &)),
          this, SLOT(roleTextChanged(const QString &)));

  roleLayout->addWidget(roleEdit_);

  menuColumnGroupLayout->addWidget(roleFrame);

  //---

  menuExprGroup_ = new QGroupBox("Expression");

  menuExprGroup_->setCheckable(true);
  menuExprGroup_->setChecked(false);

  connect(menuExprGroup_, SIGNAL(clicked(bool)), this, SLOT(menuExprGroupClicked(bool)));

  menuFrameLayout->addWidget(menuExprGroup_);

  //--

  QVBoxLayout *menuExprGroupLayout = new QVBoxLayout(menuExprGroup_);
  menuExprGroupLayout->setMargin(2); menuExprGroupLayout->setSpacing(2);

  expressionEdit_ = new QLineEdit;

  connect(expressionEdit_, SIGNAL(textChanged(const QString &)),
          this, SLOT(expressionTextChanged(const QString &)));

  menuExprGroupLayout->addWidget(expressionEdit_);

  //---

  vheaderCheck_ = new QCheckBox("Vertical Header");

  connect(vheaderCheck_, SIGNAL(clicked(bool)), this, SLOT(vheaderCheckClicked(bool)));

  menuFrameLayout->addWidget(vheaderCheck_);

  //---

  updateState();
}

void
CQChartsColumnEdit::
setModel(QAbstractItemModel *model)
{
  model_ = model;
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
}

void
CQChartsColumnEdit::
connectSlots(bool b)
{
  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    if (b)
      connect(w, from, this, to);
    else
      disconnect(w, from, this, to);
  };

  connectDisconnect(b, edit_, SIGNAL(textChanged(const QString &)),
                    SLOT(textChanged(const QString &)));
  connectDisconnect(b, menuColumnGroup_, SIGNAL(clicked(bool)),
                    SLOT(menuColumnGroupClicked(bool)));
  connectDisconnect(b, columnCombo_, SIGNAL(currentIndexChanged(int)),
                    SLOT(menuColumnChanged(int)));
  connectDisconnect(b, roleEdit_, SIGNAL(textChanged(const QString &)),
                    SLOT(roleTextChanged(const QString &)));
  connectDisconnect(b, menuExprGroup_, SIGNAL(clicked(bool)),
                    SLOT(menuExprGroupClicked(bool)));
  connectDisconnect(b, expressionEdit_, SIGNAL(textChanged(const QString &)),
                    SLOT(expressionTextChanged(const QString &)));
  connectDisconnect(b, vheaderCheck_, SIGNAL(clicked(bool)),
                    SLOT(vheaderCheckClicked(bool)));
}

QString
CQChartsColumnEdit::
text() const
{
  return edit_->text();
}

void
CQChartsColumnEdit::
setText(const QString &s)
{
  edit_->setText(s);
}

QString
CQChartsColumnEdit::
placeholderText() const
{
  return edit_->placeholderText();
}

void
CQChartsColumnEdit::
setPlaceholderText(const QString &s)
{
  edit_->setPlaceholderText(s);
}

void
CQChartsColumnEdit::
showMenu()
{
  // popup menu below or above the widget bounding box
  QPoint tl = edit_->mapToGlobal(edit_->rect().topLeft());

  QRect rect(tl.x(), tl.y(), edit_->parentWidget()->rect().width(), edit_->rect().height());

  menu_->popup(rect.bottomLeft());
}

void
CQChartsColumnEdit::
updateMenu()
{
  connectSlots(false);

  //---

  int w = std::max(menu_->sizeHint().width(), this->width());

  menu_->setFixedWidth(w);

  menu_->updateAreaSize();

  //---

  columnCombo_->clear();

  columnCombo_->addItem("<none>", -1);

  if (model()) {
    int nc = model()->columnCount();

    for (int c = 0; c < nc; ++c) {
      QString name = model()->headerData(c, Qt::Horizontal).toString();

      QString label;

      if (! name.simplified().length())
        label = QString("%1 : <no name>").arg(c);
      else
        label = QString("%1 : %2").arg(c).arg(name);

      columnCombo_->addItem(label, c);
    }

    bool ok;

    int column = edit_->text().toInt(&ok);

    if (ok && column >= 0 && column < nc) {
      int ind = columnCombo_->findData(column);

      if (ind > 0)
        columnCombo_->setCurrentIndex(ind);
    }
  }

  //---

  connectSlots(true);
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
    menuColumnGroup_->setChecked(true);
    vheaderCheck_   ->setChecked(false);
  }
  else {
    menuColumnGroup_->setChecked(false);
    vheaderCheck_   ->setChecked(false);
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
    menuColumnGroup_->setChecked(false);
    menuExprGroup_  ->setChecked(true);
  }
  else {
    menuColumnGroup_->setChecked(false);
    menuExprGroup_  ->setChecked(false);
  }

  widgetsToColumn();

  updateState();

  //---

  connectSlots(true);

  emit columnChanged();
}

void
CQChartsColumnEdit::
menuColumnChanged(int)
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
textChanged(const QString &text)
{
  CQChartsColumn column;

  if (! CQChartsModelUtil::stringToColumn(model(), text, column))
    return;

  column_ = column;

  columnToWidgets();

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
columnToWidgets()
{
  connectSlots(false);

  edit_           ->setText("");
  menuColumnGroup_->setChecked(false);
  columnCombo_    ->setCurrentIndex(0);
  menuExprGroup_  ->setChecked(false);
  expressionEdit_ ->setText("");
  vheaderCheck_   ->setChecked(false);

  if (column_.isValid()) {
    edit_->setText(column_.toString());

    if      (column_.type() == CQChartsColumn::Type::DATA) {
      menuColumnGroup_->setChecked(true);

      int ind = columnCombo_->findData(column_.column());

      if (ind > 0)
        columnCombo_->setCurrentIndex(ind);
      else
        columnCombo_->setCurrentIndex(0);

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
      edit_        ->setText("");
      vheaderCheck_->setChecked(true);
    }
  }
  else {
    menuColumnGroup_->setChecked(true);
  }

  connectSlots(true);
}

void
CQChartsColumnEdit::
widgetsToColumn()
{
  CQChartsColumn column;

  if      (menuColumnGroup_->isChecked()) {
    bool ok;

    int icolumn = columnCombo_->itemData(columnCombo_->currentIndex()).toInt(&ok);

    if (icolumn < 0)
      icolumn = -1;

    int role = roleEdit_->text().toInt(&ok);

    if (! ok)
      role = -1;

    column = CQChartsColumn(icolumn, role);
  }
  else if (menuExprGroup_->isChecked()) {
    QString str;

    if (expressionEdit_->text().simplified().length())
      str = QString("(%1)").arg(expressionEdit_->text());
    else
      str = "";

    column = CQChartsColumn(CQChartsColumn::Type::EXPR, -1, str, -1);
  }
  else if (vheaderCheck_->isChecked()) {
    column = CQChartsColumn(CQChartsColumn::Type::VHEADER, -1, "", -1);
  }

  column_ = column;

  edit_->setText(column_.toString());
}

void
CQChartsColumnEdit::
updateState()
{
  columnCombo_   ->setEnabled(menuColumnGroup_->isChecked());
  expressionEdit_->setEnabled(menuExprGroup_  ->isChecked());
}

void
CQChartsColumnEdit::
paintEvent(QPaintEvent *)
{
  QStylePainter painter(this);

  painter.setPen(palette().color(QPalette::Text));

  // draw the combobox frame, focusrect and selected etc.
  QStyleOptionComboBox opt;

  initStyle(opt);

  painter.drawComplexControl(QStyle::CC_ComboBox, opt);

  // draw text
  painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

void
CQChartsColumnEdit::
resizeEvent(QResizeEvent *)
{
  button_->setFixedHeight(edit_->height() + 2);
}

void
CQChartsColumnEdit::
initStyle(QStyleOptionComboBox &opt)
{
  opt.initFrom(this);

  opt.editable = true;
  opt.frame    = true;

  if (hasFocus()) opt.state |= QStyle::State_Selected;

  opt.subControls = QStyle::SC_All;

  if      (button_ && button_->isDown()) {
    opt.activeSubControls = QStyle::SC_ComboBoxArrow;
    opt.state |= QStyle::State_Sunken;
  }
  else if (button_ && button_->underMouse()) {
    opt.activeSubControls = QStyle::SC_ComboBoxArrow;
  }
  else if (edit_ && edit_->underMouse()) {
    opt.activeSubControls = QStyle::SC_ComboBoxEditField;
  }
}

//------

CQChartsColumnEditMenuButton::
CQChartsColumnEditMenuButton(QWidget *parent) :
 QPushButton(parent)
{
}

void
CQChartsColumnEditMenuButton::
paintEvent(QPaintEvent*)
{
  // drawn by CQChartsColumnEdit
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
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsColumn column = value.value<CQChartsColumn>();

  QString str = column.toString();

  QFontMetricsF fm(option.font);

  double w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsColumnPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsColumn>().toString();

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

  CQChartsColumnEdit *edit = new CQChartsColumnEdit(parent);

  CQChartsPlot *plot = qobject_cast<CQChartsPlot *>(obj);

  if (plot)
    edit->setModel(plot->model().data());

  return edit;
}

void
CQChartsColumnPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsColumnEdit *edit = qobject_cast<CQChartsColumnEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(columnChanged()), obj, method);
}

QVariant
CQChartsColumnPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsColumnEdit *edit = qobject_cast<CQChartsColumnEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->column());
}

void
CQChartsColumnPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsColumnEdit *edit = qobject_cast<CQChartsColumnEdit *>(w);
  assert(edit);

  CQChartsColumn column = var.value<CQChartsColumn>();

  edit->setColumn(column);
}
