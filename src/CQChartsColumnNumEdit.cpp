#include <CQChartsColumnNumEdit.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsPlot.h>
#include <CQChartsVariant.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>

#include <QHBoxLayout>
#include <QPainter>

CQChartsColumnNumLineEdit::
CQChartsColumnNumLineEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("columnNumLineEdit");

  setToolTip("Optional Column Number or Name (use empty string to unset)");

  //---

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 0);

  combo_ = CQUtil::makeWidget<CQChartsColumnCombo>("combo");

  layout->addWidget(combo_);

  connectSlots(true);
}

void
CQChartsColumnNumLineEdit::
setPlot(CQChartsPlot *plot)
{
  plot_ = plot;

  combo_->setModel(plot->model().data());
}

const CQChartsColumnNum &
CQChartsColumnNumLineEdit::
columnNum() const
{
  return columnNum_;
}

void
CQChartsColumnNumLineEdit::
setColumnNum(const CQChartsColumnNum &c)
{
  columnNum_ = c;

  columnToWidgets();
}

void
CQChartsColumnNumLineEdit::
columnComboChanged()
{
  CQChartsColumn column = combo_->getColumn();

  CQChartsColumnNum columnNum;

  if (column.isValid())
    columnNum = CQChartsColumnNum(column.column());
  else
    columnNum = CQChartsColumnNum();

  if (columnNum != columnNum_) {
    columnNum_ = columnNum;

    emit columnChanged();
  }
}

void
CQChartsColumnNumLineEdit::
columnToWidgets()
{
  connectSlots(false);

  combo_->setColumn(CQChartsColumn(columnNum_.column()));

  connectSlots(true);
}

void
CQChartsColumnNumLineEdit::
widgetsToColumn()
{
  CQChartsColumn column = combo_->getColumn();

  if (column.isValid())
    columnNum_ = CQChartsColumnNum(column.column());
}

void
CQChartsColumnNumLineEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  CQChartsWidgetUtil::connectDisconnect(connected_,
    combo_, SIGNAL(columnChanged()), this, SLOT(columnComboChanged()));
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsColumnNumPropertyViewType::
CQChartsColumnNumPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsColumnNumPropertyViewType::
getEditor() const
{
  return new CQChartsColumnNumPropertyViewEditor;
}

bool
CQChartsColumnNumPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsColumnNumPropertyViewType::
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
CQChartsColumnNumPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;

  QString str = valueString(nullptr, value, ok);

  return str;
}

QString
CQChartsColumnNumPropertyViewType::
valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const
{
  CQChartsColumnNum columnNum = value.value<CQChartsColumnNum>();

  QString str;

  if (columnNum.isValid()) {
    str = columnNum.toString();

    auto *plot = (item ? qobject_cast<CQChartsPlot *>(item->object()) : nullptr);

    if (plot) {
      QString str1 = plot->columnHeaderName(CQChartsColumn(columnNum.column()));

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

CQChartsColumnNumPropertyViewEditor::
CQChartsColumnNumPropertyViewEditor()
{
}

QWidget *
CQChartsColumnNumPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQPropertyViewItem *item = CQPropertyViewMgrInst->editItem();

  QObject *obj = (item ? item->object() : nullptr);

  auto *plot = qobject_cast<CQChartsPlot *>(obj);

  //---

  CQChartsColumnNumLineEdit *edit = new CQChartsColumnNumLineEdit(parent);

  if (plot)
    edit->setPlot(plot);

  return edit;
}

void
CQChartsColumnNumPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsColumnNumLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(columnChanged()), obj, method);
}

QVariant
CQChartsColumnNumPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsColumnNumLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->columnNum());
}

void
CQChartsColumnNumPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsColumnNumLineEdit *>(w);
  assert(edit);

  CQChartsColumnNum columnNum = var.value<CQChartsColumnNum>();

  edit->setColumnNum(columnNum);
}
