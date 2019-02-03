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

#include <svg/add_svg.h>
#include <svg/remove_svg.h>

CQChartsColumnsLineEdit::
CQChartsColumnsLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("columnsLineEdit");

  //---

  menuEdit_ = new CQChartsColumnsEdit;

  menu_->setWidget(menuEdit_);

  connect(menuEdit_, SIGNAL(columnsChanged()), this, SLOT(menuEditChanged()));
}

QAbstractItemModel *
CQChartsColumnsLineEdit::
model() const
{
  return menuEdit_->model();
}

void
CQChartsColumnsLineEdit::
setModel(QAbstractItemModel *model)
{
  menuEdit_->setModel(model);
}

const CQChartsColumns &
CQChartsColumnsLineEdit::
columns() const
{
  return menuEdit_->columns();
}

void
CQChartsColumnsLineEdit::
setColumns(const CQChartsColumns &columns)
{
  updateColumns(columns, /*updateText*/true);
}

void
CQChartsColumnsLineEdit::
updateColumns(const CQChartsColumns &columns, bool updateText)
{
  connectSlots(false);

  menuEdit_->setColumns(columns);

  if (updateText)
    columnsToWidgets();

  connectSlots(true);

  emit columnsChanged();
}

void
CQChartsColumnsLineEdit::
textChanged()
{
  CQChartsColumns columns;

  if (! textToColumns(edit_->text(), columns))
    return;

  updateColumns(columns, /*updateText*/ false);
}

void
CQChartsColumnsLineEdit::
columnsToWidgets()
{
  connectSlots(false);

  if (columns().isValid())
    edit_->setText(columns().toString());
  else
    edit_->setText("");

  connectSlots(true);
}

void
CQChartsColumnsLineEdit::
menuEditChanged()
{
  updateMenu();

  columnsToWidgets();

  emit columnsChanged();
}

void
CQChartsColumnsLineEdit::
connectSlots(bool b)
{
  CQChartsLineEditBase::connectSlots(b);

  if (b)
    connect(menuEdit_, SIGNAL(columnsChanged()), this, SLOT(menuEditChanged()));
  else
    disconnect(menuEdit_, SIGNAL(columnsChanged()), this, SLOT(menuEditChanged()));
}

bool
CQChartsColumnsLineEdit::
textToColumns(const QString &str, CQChartsColumns &columns) const
{
  bool ok = true;

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

        if (model()) {
          if (! CQChartsModelUtil::stringToColumn(model(), str, col))
            col = CQChartsColumn();
        }
        else
          col = CQChartsColumn(str);

        if (col.isValid())
          columns.addColumn(col);
        else
          ok = false;
      }
    }
    else {
      CQChartsColumn col;

      if (model()) {
        if (! CQChartsModelUtil::stringToColumn(model(), str, col))
          col = CQChartsColumn();
      }
      else
        col = CQChartsColumn(str);

      if (col.isValid())
        columns.addColumn(col);
      else
        ok = false;
    }
  }

  return ok;
}

void
CQChartsColumnsLineEdit::
updateMenu()
{
  CQChartsLineEditBase::updateMenu();

  //---

  QSize s = menuEdit_->sizeHint();

  int w = std::max(this->width(), s.width());
  int h = s.height();

  //---

  menuEdit_->setFixedSize(w, h);

  int bw = 2;

  menu_->setFixedSize(QSize(w + 2*bw, h + 2*bw));

  menu_->updateAreaSize();
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

  CQChartsColumnsLineEdit *edit = new CQChartsColumnsLineEdit(parent);

  //---

  CQChartsPlot *plot = qobject_cast<CQChartsPlot *>(obj);

  if (plot)
    edit->setModel(plot->model().data());

  return edit;
}

void
CQChartsColumnsPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsColumnsLineEdit *edit = qobject_cast<CQChartsColumnsLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(columnsChanged()), obj, method);
}

QVariant
CQChartsColumnsPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsColumnsLineEdit *edit = qobject_cast<CQChartsColumnsLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->columns());
}

void
CQChartsColumnsPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsColumnsLineEdit *edit = qobject_cast<CQChartsColumnsLineEdit *>(w);
  assert(edit);

  CQChartsColumns columns = var.value<CQChartsColumns>();

  edit->setColumns(columns);
}

//------

CQChartsColumnsEdit::
CQChartsColumnsEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("columnsEdit");

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  controlFrame_ = new QFrame;

  QHBoxLayout *controlFrameLayout = new QHBoxLayout(controlFrame_);
  controlFrameLayout->setMargin(2); controlFrameLayout->setSpacing(2);

  QToolButton *addButton    = new QToolButton;
  QToolButton *removeButton = new QToolButton;

  addButton   ->setIcon(CQPixmapCacheInst->getIcon("ADD"));
  removeButton->setIcon(CQPixmapCacheInst->getIcon("REMOVE"));

  connect(addButton, SIGNAL(clicked()), this, SLOT(addSlot()));
  connect(removeButton, SIGNAL(clicked()), this, SLOT(removeSlot()));

  controlFrameLayout->addStretch(1);
  controlFrameLayout->addWidget(addButton);
  controlFrameLayout->addWidget(removeButton);

  layout->addWidget(controlFrame_);

  columnsFrame_ = new QFrame;

  QVBoxLayout *columnsFrameLayout = new QVBoxLayout(columnsFrame_);
  columnsFrameLayout->setMargin(2); columnsFrameLayout->setSpacing(2);

  layout->addWidget(columnsFrame_);

  layout->addStretch(1);

  //---

  updateEdits();
}

void
CQChartsColumnsEdit::
setModel(QAbstractItemModel *model)
{
  connectSlots(false);

  model_ = model;

  int ne = columnEdits_.size();

  for (int i = 0; i < ne; ++i)
    columnEdits_[i]->setModel(model_);

  connectSlots(true);
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

  columnsToWidgets();

  emit columnsChanged();
}

void
CQChartsColumnsEdit::
columnsToWidgets()
{
  updateEdits();

  //---

  connectSlots(false);

  int n  = columns_.count();
  int ne = columnEdits_.size();
  assert(n == ne);

  for (int i = 0; i < n; ++i)
    columnEdits_[i]->setColumn(columns_.getColumn(i));

  connectSlots(true);
}

void
CQChartsColumnsEdit::
widgetsToColumn()
{
  int n  = columns_.count();
  int ne = columnEdits_.size();
  assert(n == ne);

  for (int i = 0; i < n; ++i)
    columns_.setColumn(i, columnEdits_[i]->column());

  emit columnsChanged();
}

void
CQChartsColumnsEdit::
addSlot()
{
  columns_.addColumn(CQChartsColumn());

  columnsToWidgets();

  emit columnsChanged();
}

void
CQChartsColumnsEdit::
removeSlot()
{
  columns_.removeColumn();

  columnsToWidgets();

  emit columnsChanged();
}

void
CQChartsColumnsEdit::
updateEdits()
{
  connectSlots(false);

  int n  = columns_.count();
  int ne = columnEdits_.size();

  while (ne < n) {
    CQChartsColumnLineEdit *edit = new CQChartsColumnLineEdit;

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

  connectSlots(true);
}

void
CQChartsColumnsEdit::
connectSlots(bool b)
{
  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    if (b)
      connect(w, from, this, to);
    else
      disconnect(w, from, this, to);
  };

  for (auto &edit : columnEdits_)
    connectDisconnect(b, edit, SIGNAL(columnChanged()), SLOT(widgetsToColumn()));
}

QSize
CQChartsColumnsEdit::
sizeHint() const
{
  QSize s1 = controlFrame_->sizeHint();

  int ne = columnEdits_.size();

  int w1 = s1.width();
  int h1 = s1.height() + 2;

  for (int i = 0; i < ne; ++i) {
    QSize s2 = columnEdits_[i]->sizeHint();

    w1  = std::max(w1, s2.width());
    h1 += s2.height() + 2;
  }

  return QSize(w1 + 4, h1 + 2);
}
