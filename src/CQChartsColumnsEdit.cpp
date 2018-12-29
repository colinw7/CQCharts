#include <CQChartsColumnsEdit.h>
#include <CQChartsColumnEdit.h>
#include <CQChartsPlot.h>
#include <CQChartsModelUtil.h>

#include <CQPropertyView.h>
#include <CQWidgetMenu.h>
#include <CQPixmapCache.h>

#include <QLineEdit>
#include <QToolButton>
#include <QHBoxLayout>
#include <QAbstractItemModel>
#include <QStylePainter>

#include <svg/add_svg.h>
#include <svg/remove_svg.h>

CQChartsColumnsEdit::
CQChartsColumnsEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("columnsEdit");

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

  button_ = new CQChartsColumnsEditMenuButton;
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

  menuFrame_ = new QFrame;

  QVBoxLayout *menuFrameLayout = new QVBoxLayout(menuFrame_);
  menuFrameLayout->setMargin(2); menuFrameLayout->setSpacing(2);

  menu_->setWidget(menuFrame_);

  controlFrame_ = new QFrame;

  QHBoxLayout *controlFrameLayout = new QHBoxLayout(controlFrame_);
  controlFrameLayout->setMargin(2); controlFrameLayout->setSpacing(2);

  QToolButton *addButton    = new QToolButton;
  QToolButton *removeButton = new QToolButton;

  addButton   ->setIcon(CQPixmapCacheInst->getIcon("ADD"));
  removeButton->setIcon(CQPixmapCacheInst->getIcon("REMOVE"));

  controlFrameLayout->addStretch(1);
  controlFrameLayout->addWidget(addButton);
  controlFrameLayout->addWidget(removeButton);

  connect(addButton, SIGNAL(clicked()), this, SLOT(addSlot()));
  connect(removeButton, SIGNAL(clicked()), this, SLOT(removeSlot()));

  menuFrameLayout->addWidget(controlFrame_);

  columnsFrame_ = new QFrame;

  QVBoxLayout *columnsFrameLayout = new QVBoxLayout(columnsFrame_);
  columnsFrameLayout->setMargin(2); columnsFrameLayout->setSpacing(2);

  menuFrameLayout->addWidget(columnsFrame_);

  //---

  updateState();
}

void
CQChartsColumnsEdit::
setModel(QAbstractItemModel *model)
{
  model_ = model;

  int n = columnEdits_.size();

  for (int i = 0; i < n; ++i)
    columnEdits_[i]->setModel(model_);
}

const CQChartsColumns &
CQChartsColumnsEdit::
columns() const
{
  return columns_;
}

void
CQChartsColumnsEdit::
setColumns(const CQChartsColumns &columns)
{
  columns_ = columns;

  columnsToText();

  updateState();

  emit columnsChanged();
}

void
CQChartsColumnsEdit::
columnsToText()
{
  connectSlots(false);

  edit_->setText(columns_.toString());

  connectSlots(true);
}

void
CQChartsColumnsEdit::
textToColumns()
{
  bool ok = true;

  CQChartsColumns columns;

  QString str = edit_->text();

  // TODO: better split to handle spaces in column names/expressions
  QStringList strs = str.split(" ", QString::SkipEmptyParts);

  for (int i = 0; i < strs.size(); ++i) {
    const QString &str = strs[i];

    if (str.left(1) != "(") {
      // support column numeric range <n>-<m>
      QStringList strs1 = str.split("-", QString::SkipEmptyParts);

      if (strs1.size() == 2) {
        bool ok1, ok2;

        int startCol = strs1[0].toInt(&ok1);
        int endCol   = strs1[1].toInt(&ok2);

        if (ok1 && ok2) {
          for (int col = startCol; col <= endCol; ++col)
            columns.addColumn(col);
        }
        else
          ok = false;
      }
      else {
        CQChartsColumn col;

        if (CQChartsModelUtil::stringToColumn(model(), str, col))
          columns.addColumn(col);
        else
          ok = false;
      }
    }
    else {
      CQChartsColumn col;

      if (CQChartsModelUtil::stringToColumn(model(), str, col))
        columns.addColumn(col);
      else
        ok = false;
    }
  }

  if (! ok)
    return;

  columns_ = columns;
}

void
CQChartsColumnsEdit::
connectSlots(bool b)
{
  if (b) {
    connect(edit_, SIGNAL(textChanged(const QString &)),
            this, SLOT(textChanged(const QString &)));
  }
  else {
    disconnect(edit_, SIGNAL(textChanged(const QString &)),
               this, SLOT(textChanged(const QString &)));
  }
}

QString
CQChartsColumnsEdit::
placeholderText() const
{
  return edit_->placeholderText();
}

void
CQChartsColumnsEdit::
setPlaceholderText(const QString &s)
{
  edit_->setPlaceholderText(s);
}

void
CQChartsColumnsEdit::
showMenu()
{
  // popup menu below or above the widget bounding box
  QPoint tl = edit_->mapToGlobal(edit_->rect().topLeft());

  QRect rect(tl.x(), tl.y(), edit_->parentWidget()->rect().width(), edit_->rect().height());

  menu_->popup(rect.bottomLeft());
}

void
CQChartsColumnsEdit::
updateMenu()
{
  connectSlots(false);

  //---

  int n  = columns_.count();
  int ne = columnEdits_.size();
  assert(n == ne);

  for (int i = 0; i < n; ++i)
    columnEdits_[i]->setColumn(columns_.getColumn(i));

  //---

  menu_->updateAreaSize();

  //---

  connectSlots(true);
}

void
CQChartsColumnsEdit::
textChanged(const QString &)
{
  textToColumns();

  updateState();

  emit columnsChanged();
}

void
CQChartsColumnsEdit::
addSlot()
{
  columns_.addColumn(CQChartsColumn());

  columnsToText();

  updateState();

  emit columnsChanged();
}

void
CQChartsColumnsEdit::
removeSlot()
{
  columns_.removeColumn();

  columnsToText();

  updateState();

  emit columnsChanged();
}

void
CQChartsColumnsEdit::
updateState()
{
  int ew = this->width();
  int eh = this->height();

  int w = ew, h = controlFrame_->sizeHint().height();

  int n = columns_.count();

  int ch = n*eh;

  h += ch;

  //---

  int ne = columnEdits_.size();

  while (ne < n) {
    CQChartsColumnEdit *edit = new CQChartsColumnEdit;

    connect(edit, SIGNAL(columnChanged()), this, SLOT(columnSlot()));

    edit->setModel(model());

    qobject_cast<QVBoxLayout *>(columnsFrame_->layout())->addWidget(edit);

    columnEdits_.push_back(edit);

    ++ne;
  }

  while (ne > n) {
    delete columnEdits_.back();

    columnEdits_.pop_back();

    --ne;
  }

  columnsFrame_->setFixedSize(QSize(w, ch));

  menuFrame_->setFixedSize(QSize(w, h));

  int bw = 2;

  menu_->setFixedSize(QSize(w + 2*bw, h + 2*bw));
}

void
CQChartsColumnsEdit::
columnSlot()
{
  int n  = columns_.count();
  int ne = columnEdits_.size();
  assert(n == ne);

  for (int i = 0; i < n; ++i)
    columns_.setColumn(i, columnEdits_[i]->column());

  columnsToText();

  updateState();

  emit columnsChanged();
}

void
CQChartsColumnsEdit::
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
CQChartsColumnsEdit::
resizeEvent(QResizeEvent *)
{
  button_->setFixedHeight(edit_->height() + 2);
}

void
CQChartsColumnsEdit::
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

CQChartsColumnsEditMenuButton::
CQChartsColumnsEditMenuButton(QWidget *parent) :
 QPushButton(parent)
{
}

void
CQChartsColumnsEditMenuButton::
paintEvent(QPaintEvent*)
{
  // drawn by CQChartsColumnsEdit
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsColumnsPropertyViewType::
CQChartsColumnsPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsColumnsPropertyViewType::
getEditor() const
{
  return new CQChartsColumnsPropertyViewEditor;
}

bool
CQChartsColumnsPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsColumnsPropertyViewType::
draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsColumns columns = value.value<CQChartsColumns>();

  QString str = columns.toString();

  QFontMetricsF fm(option.font);

  double w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsColumnsPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsColumns>().toString();

  return str;
}

//------

CQChartsColumnsPropertyViewEditor::
CQChartsColumnsPropertyViewEditor()
{
}

QWidget *
CQChartsColumnsPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQPropertyViewItem *item = CQPropertyViewMgrInst->editItem();

  QObject *obj = (item ? item->object() : nullptr);

  CQChartsColumnsEdit *edit = new CQChartsColumnsEdit(parent);

  CQChartsPlot *plot = qobject_cast<CQChartsPlot *>(obj);

  if (plot)
    edit->setModel(plot->model().data());

  return edit;
}

void
CQChartsColumnsPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsColumnsEdit *edit = qobject_cast<CQChartsColumnsEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(columnsChanged()), obj, method);
}

QVariant
CQChartsColumnsPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsColumnsEdit *edit = qobject_cast<CQChartsColumnsEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->columns());
}

void
CQChartsColumnsPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsColumnsEdit *edit = qobject_cast<CQChartsColumnsEdit *>(w);
  assert(edit);

  CQChartsColumns columns = var.value<CQChartsColumns>();

  edit->setColumns(columns);
}
