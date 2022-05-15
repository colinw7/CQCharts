#include <CQChartsColumnNumEdit.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsPlot.h>
#include <CQChartsModelData.h>
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
  if (plot)
    setModelData(plot->getModelData());
  else
    setModelData(nullptr);
}

void
CQChartsColumnNumLineEdit::
setModelData(CQChartsModelData *modelData)
{
  if (modelData != modelData_) {
    if (modelData_)
      disconnect(modelData_, SIGNAL(destroyed(QObject *)), this, SLOT(resetModelData()));

    modelData_ = modelData;

    if (modelData_)
      connect(modelData_, SIGNAL(destroyed(QObject *)), this, SLOT(resetModelData()));

    combo_->setProxy(isProxy());

    if (modelData_)
      combo_->setModel(modelData->currentModel(isProxy()).data());
  }
}

void
CQChartsColumnNumLineEdit::
resetModelData()
{
  modelData_ = nullptr;
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
setProxy(bool b)
{
  proxy_ = b;

  columnToWidgets();
}

void
CQChartsColumnNumLineEdit::
columnComboChanged()
{
  auto column = combo_->getColumn();

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
  auto column = combo_->getColumn();

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
CQChartsColumnNumPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;

  auto str = valueString(nullptr, value, ok);

  return str;
}

QString
CQChartsColumnNumPropertyViewType::
valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const
{
  auto columnNum = CQChartsColumnNum::fromVariant(value);

  QString str;

  if (columnNum.isValid()) {
    str = columnNum.toString();

    auto *plot = (item ? qobject_cast<CQChartsPlot *>(item->object()) : nullptr);

    if (plot) {
      auto str1 = plot->columnHeaderName(CQChartsColumn(columnNum.column()));

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
  auto *item = CQPropertyViewMgrInst->editItem();

  auto *obj = (item ? item->object() : nullptr);

  auto *plot = qobject_cast<CQChartsPlot *>(obj);

  //---

  auto *edit = new CQChartsColumnNumLineEdit(parent);

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

  return CQChartsColumnNum::toVariant(edit->columnNum());
}

void
CQChartsColumnNumPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsColumnNumLineEdit *>(w);
  assert(edit);

  auto columnNum = CQChartsColumnNum::fromVariant(var);

  edit->setColumnNum(columnNum);
}
