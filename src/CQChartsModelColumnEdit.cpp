#include <CQChartsModelColumnEdit.h>
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

CQChartsModelColumnLineEdit::
CQChartsModelColumnLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("modelColumnLineEdit");

  setToolTip("Model with Column Number or Name (use empty string to unset)");

  //---

  menuEdit_ = dataEdit_ = new CQChartsModelColumnEdit(this);

  setNoFocus();

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);

  modelColumnToWidgets();
}

void
CQChartsModelColumnLineEdit::
setView(CQChartsView *view)
{
  dataEdit_->setCharts(view->charts());

  CQChartsLineEditBase::setView(view);
}

void
CQChartsModelColumnLineEdit::
setPlot(CQChartsPlot *plot)
{
  dataEdit_->setCharts(plot->charts());

  CQChartsLineEditBase::setPlot(plot);
}

const CQChartsModelColumn &
CQChartsModelColumnLineEdit::
modelColumn() const
{
  return dataEdit_->modelColumn();
}

void
CQChartsModelColumnLineEdit::
setModelColumn(const ModelColumn &modelColumn)
{
  updateModelColumn(modelColumn, /*updateText*/true);
}

void
CQChartsModelColumnLineEdit::
setNoFocus()
{
  dataEdit_->setNoFocus();
}

void
CQChartsModelColumnLineEdit::
updateModelColumn(const ModelColumn &column, bool updateText)
{
  connectSlots(false);

  dataEdit_->setModelColumn(column);

  connectSlots(true);

  if (updateText)
    modelColumnToWidgets();

  Q_EMIT modelColumnChanged();
}

void
CQChartsModelColumnLineEdit::
textChanged()
{
  // update column from widget (text)
  ModelColumn modelColumn;

  auto text = edit_->text();

  if (text.trimmed() != "") {
    modelColumn = ModelColumn(text);

    if (dataEdit_->charts())
      modelColumn.setCharts(dataEdit_->charts());
  }

  updateModelColumn(modelColumn, /*updateText*/false);
}

void
CQChartsModelColumnLineEdit::
modelColumnToWidgets()
{
  // update widget from current symbol
  connectSlots(false);

  QString text;
  QString columnText;

  if (modelColumn().isValid()) {
    text = modelColumn().toString();

    auto *modelData = modelColumn().modelData();

    if (modelData) {
      const auto &column = modelColumn().column();

      bool ok;
      columnText = CQChartsModelUtil::modelHHeaderString(modelData->model().data(), column, ok);
    }
  }

  edit_->setText(text);

  auto tip = QString("%1 (%2)").arg(toolTip()).arg(modelColumn().toString());

  edit_->setToolTip(tip);

  connectSlots(true);
}

void
CQChartsModelColumnLineEdit::
menuEditChanged()
{
  modelColumnToWidgets();

  Q_EMIT modelColumnChanged();
}

void
CQChartsModelColumnLineEdit::
connectSlots(bool b)
{
  connectBaseSlots(b);

  CQChartsWidgetUtil::connectDisconnect(b,
    dataEdit_, SIGNAL(modelColumnChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsModelColumnLineEdit::
drawPreview(QPainter *painter, const QRect &)
{
  auto str = (modelColumn().isValid() ? modelColumn().toString() : "<none>");

  drawCenteredText(painter, str);
}

void
CQChartsModelColumnLineEdit::
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

CQChartsModelColumnPropertyViewType::
CQChartsModelColumnPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsModelColumnPropertyViewType::
getEditor() const
{
  return new CQChartsModelColumnPropertyViewEditor;
}

bool
CQChartsModelColumnPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsModelColumnPropertyViewType::
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
CQChartsModelColumnPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;

  auto str = valueString(nullptr, value, ok);

  return str;
}

QString
CQChartsModelColumnPropertyViewType::
valueString(CQPropertyViewItem *, const QVariant &value, bool &ok) const
{
  auto modelColumn = ModelColumn::fromVariant(value);

  QString str;

  if (modelColumn.isValid()) {
    str = modelColumn.toString();

    auto *modelData = modelColumn.modelData();

    if (modelData) {
      const auto &column = modelColumn.column();

      auto str1 = CQChartsModelUtil::modelHHeaderString(modelData->model().data(), column, ok);

      if (str1.length())
        str += " (" + str1 + ")";

      ok = true;
    }
  }
  else {
    str = "<none>";
    ok  = false;
  }

  return str;
}

//------

CQChartsModelColumnPropertyViewEditor::
CQChartsModelColumnPropertyViewEditor()
{
}

QWidget *
CQChartsModelColumnPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *item = CQPropertyViewMgrInst->editItem();
  auto *obj  = (item ? item->object() : nullptr);
  if (! obj) return nullptr;

  //---

  CQChartsObjUtil::ObjData objData;

  CQChartsObjUtil::getObjData(obj, objData);

  //---

  auto *edit = new CQChartsModelColumnLineEdit(parent);

  if      (objData.plot)
    edit->setPlot(objData.plot);
  else if (objData.view)
    edit->setView(objData.view);

  return edit;
}

void
CQChartsModelColumnPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsModelColumnLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(modelColumnChanged()), obj, method);
}

QVariant
CQChartsModelColumnPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsModelColumnLineEdit *>(w);
  assert(edit);

  return CQChartsModelColumn::toVariant(edit->modelColumn());
}

void
CQChartsModelColumnPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsModelColumnLineEdit *>(w);
  assert(edit);

  auto modelColumn = ModelColumn::fromVariant(var);

  edit->setModelColumn(modelColumn);
}

//------

CQChartsModelColumnEdit::
CQChartsModelColumnEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  setObjectName("modelColumnEdit");

  lineEdit_ = qobject_cast<CQChartsModelColumnLineEdit *>(parent);

  //---

  auto *layout = CQUtil::makeLayout<QGridLayout>(this, 2, 2);

  //---

  int row = 0;

  layout->setRowStretch(row, 1);

  layout->setColumnStretch(1, 1);

  //---

  connectSlots(true);

  updateState();
}

void
CQChartsModelColumnEdit::
setCharts(CQCharts *charts)
{
  charts_ = charts;

  updateColumnsFromModel();

  modelColumnToWidgets();

  updateState();
}

void
CQChartsModelColumnEdit::
setModelColumn(const ModelColumn &modelColumn)
{
  modelColumn_ = modelColumn;

  modelColumnToWidgets();

  updateState();

  Q_EMIT modelColumnChanged();
}

void
CQChartsModelColumnEdit::
modelColumnToWidgets()
{
  connectSlots(false);

  connectSlots(true);
}

void
CQChartsModelColumnEdit::
widgetsToModelColumn()
{
  ModelColumn modelColumn;

  //---

  modelColumn_ = modelColumn;

  //---

  updateState();

  Q_EMIT modelColumnChanged();
}

void
CQChartsModelColumnEdit::
updateMenu()
{
  if (lineEdit_)
    lineEdit_->updateMenuEditHeight();
}

void
CQChartsModelColumnEdit::
setNoFocus()
{
}

void
CQChartsModelColumnEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;
}

void
CQChartsModelColumnEdit::
typeSlot()
{
  widgetsToModelColumnSlot();

  if (lineEdit_)
    lineEdit_->updateMenuEditHeight();
}

void
CQChartsModelColumnEdit::
widgetsToModelColumnSlot()
{
  connectSlots(false);

  widgetsToModelColumn();

  connectSlots(true);

  Q_EMIT modelColumnChanged();
}

void
CQChartsModelColumnEdit::
updateColumnsFromModel()
{
}

void
CQChartsModelColumnEdit::
updateState()
{
}

QSize
CQChartsModelColumnEdit::
sizeHint() const
{
  int w = width();

  return QSize(w, widgetHeight_);
}

QSize
CQChartsModelColumnEdit::
minimumSizeHint() const
{
  QFontMetrics fm(font());

  int eh = fm.height() + 8;

  int n = 1;

  return QSize(0, eh*n);
}
